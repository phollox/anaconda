# Copyright (c) Mathias Kaerlev 2012-2015.
#
# This file is part of Anaconda.
#
# Anaconda is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Anaconda is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Anaconda.  If not, see <http://www.gnu.org/licenses/>.

"""
Build script for Linux and Mac
"""
import shutil
import tempfile
import os
import urllib2
import tarfile
import stat
import argparse
import subprocess

chroot_prefix = 'steamrt_scout_'
chroots = '/var/chroots'

steamrt_archive = ('https://codeload.github.com/ValveSoftware/steam-runtime/'
                   'tar.gz/master')

base_dir = os.path.abspath(os.path.dirname(__file__))

def makedirs(path):
    try:
        os.makedirs(path)
    except OSError:
        return

class Builder(object):
    def __init__(self):
        self.root_dir = os.getcwd()

    def setup_parser(self, parser):
        pass

    def setup(self, args):
        self.args = args

    def create_project(self):
        cwd = os.getcwd()
        makedirs(self.build_dir)

        os.chdir(self.build_dir)

        build_type = 'Release'
        if self.args.build_type is not None:
            build_type = self.args.build_type

        defs = ['-DCMAKE_BUILD_TYPE=%s' % build_type]
        if self.args.steam:
            defs += ['-DUSE_STEAM=ON']
        cmd = [self.get_cmake_path(),  '..'] + defs + self.get_cmake_args()

        self.call(cmd)
        os.chdir(cwd)

    def get_cmake_args(self):
        return []

    def get_cmake_path(self):
        return 'cmake'

    def call(self, args):
        print ' '.join(args)
        return subprocess.check_call(args)

    def finish(self):
        pass

class LinuxBuilder(Builder):
    temp = None
    chroot = None

    def install_chroot(self, arch):
        chroot = chroot_prefix + arch
        if os.path.isdir(os.path.join(chroots, chroot)):
            return chroot
        cwd = os.getcwd()
        if self.temp is None:
            self.temp = tempfile.mkdtemp()
            os.chdir(self.temp)
            data = urllib2.urlopen(steamrt_archive).read()
            with open('steamrt.tar.gz', 'wb') as fp:
                fp.write(data)
            archive = tarfile.open('steamrt.tar.gz')
            archive.extractall()

        os.chdir(os.path.join(self.temp, 'steam-runtime-master'))
        self.call(['./setup_chroot.sh',  '--%s' % arch])
        os.chdir(cwd)
        return chroot

    def call(self, args):
        if self.chroot:
            args = ['schroot', '--chroot', self.chroot, '--'] + args
        return Builder.call(self, args)

    def build_arch(self, arch):
        chroot = self.install_chroot(arch)
        self.build_dir = os.path.join(self.root_dir, 'build_%s' % arch)
        self.dist_dir = os.path.join(self.root_dir, 'dist')
        if self.args.steam:
            self.build_dir += '_steamworks'
            self.dist_dir += '_steamworks'

        self.install_dir = os.path.join(self.build_dir, 'install')

        if arch == 'amd64':
            bin_dir = 'bin64'
        else:
            bin_dir = 'bin32'

        self.src_bin_dir = os.path.join(self.install_dir, bin_dir)
        self.dst_bin_dir = os.path.join(self.dist_dir, bin_dir)

        self.chroot = chroot
        self.create_project()
        self.copy_dependencies(arch)
        self.build_project()
        self.create_dist(arch)
        self.chroot = None

    def copy_dependencies(self, arch):
        makedirs(self.src_bin_dir)

        if not self.args.steam:
            return

        arch_dirs = {
            'amd64': 'linux64',
            'i386': 'linux32'
        }
        arch_dir = arch_dirs[arch]
        steam_bin = os.path.join(base_dir, 'steam', 'sdk',
                                 'redistributable_bin', arch_dir,
                                 'libsteam_api.so')
        shutil.copy(steam_bin,
                    os.path.join(self.src_bin_dir, 'libsteam_api.so'))

    def create_dist(self, arch):
        makedirs(self.dist_dir)

        src_bin_dir = self.src_bin_dir
        dst_bin_dir = self.dst_bin_dir

        shutil.rmtree(dst_bin_dir, ignore_errors=True)
        shutil.copytree(src_bin_dir, dst_bin_dir)

        with open(os.path.join(self.install_dir, 'run.sh'), 'rb') as fp:
            run_data = '\n'.join(fp.read().splitlines())

        with open(os.path.join(self.dist_dir, 'run.sh'), 'wb') as fp:
            fp.write(run_data)

        arch_deps = {
            'amd64': ('/usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0',
                      '/usr/lib/x86_64-linux-gnu/libopenal.so.1'),
            'i386': ('/usr/lib/i386-linux-gnu/libSDL2-2.0.so.0',
                     '/usr/lib/i386-linux-gnu/libopenal.so.1')
        }

        for dep in arch_deps[arch]:
            dep = os.path.join(chroots, self.chroot, dep[1:])
            shutil.copy(dep, os.path.join(dst_bin_dir, os.path.basename(dep)))

    def make_dist(self):
        shutil.copy(os.path.join(self.root_dir, 'Assets.dat'),
                    os.path.join(self.dist_dir, 'Assets.dat'))
        run_sh = os.path.join(self.dist_dir, 'run.sh')
        st = os.stat(run_sh)
        os.chmod(run_sh, st.st_mode | stat.S_IEXEC)

    def build_project(self):
        cwd = os.getcwd()
        os.chdir(self.build_dir)
        self.call(['make',  '-j4'])
        self.call(['make', 'install'])
        os.chdir(cwd)

    def build(self):
        self.build_arch('amd64')
        self.build_arch('i386')
        self.make_dist()

    def finish(self):
        if self.temp is None:
            return
        shutil.rmtree(self.temp)

ARCHS = ('x86', 'armeabi')
BUILD_TOOLS_VERSION = '23.0.1'

ANDROID_SDK = 'C:/Program Files (x86)/Android/android-sdk'
PLATFORM_TOOLS = os.path.join(ANDROID_SDK, 'platform-tools')
BUILD_TOOLS = os.path.join(ANDROID_SDK, 'build-tools', BUILD_TOOLS_VERSION)
ADB = os.path.join(PLATFORM_TOOLS, 'adb.exe')
AAPT = os.path.join(BUILD_TOOLS, 'aapt.exe')
DX = os.path.join(BUILD_TOOLS, 'dx.bat')
ZIPALIGN = os.path.join(BUILD_TOOLS, 'zipalign.exe')
ANDROID_PROJECT_SRC = os.path.join(base_dir, 'android', 'project')
ANDROID_SRC = os.path.join(ANDROID_PROJECT_SRC, 'src')
RES_DIR = os.path.join(ANDROID_PROJECT_SRC, 'res')
ANDROID_MANIFEST = os.path.join(ANDROID_PROJECT_SRC, 'AndroidManifest.xml')

R_JAVA = os.path.join('gen', 'org', 'libsdl', 'app', 'R.java')
SDL_JAVA = os.path.join(ANDROID_SRC, 'org', 'libsdl', 'app',
                        'SDLActivity.java')
JAVA_SRCS = [R_JAVA, SDL_JAVA]
DEBUG_KEYSTORE = os.path.join(os.path.expanduser('~'), '.android',
                              'debug.keystore')

class AndroidBuilder(Builder):
    def setup_parser(self, parser):
        parser.add_argument('--install', action='store_true',
                            help='Install on Android device')
        parser.add_argument('--tv', action='store_true',
                            help='Make APK compatible with Android TV')

    def build(self):
        if self.args.tv:
            self.target_version = 21
        else:
            self.target_version = 12
        self.android_jar = os.path.join(ANDROID_SDK, 'platforms',
                                        'android-%s' % self.target_version,
                                        'android.jar')

        with open(ANDROID_MANIFEST, 'rb') as fp:
            data = fp.read()
            if self.args.tv:
                data = data.replace('android:minSdkVersion="10"',
                                    ('android:minSdkVersion="%s"'
                                     % self.target_version))
                data = data.replace('android:targetSdkVersion="12"',
                                    ('android:targetSdkVersion="%s"'
                                     % self.target_version))

        with open('AndroidManifest.xml', 'wb') as fp:
            fp.write(data)

        makedirs('bin/lib')
        archs = ('x86', 'armeabi')
        for arch in archs:
            self.build_arch(arch)
        makedirs('gen')
        makedirs('bin')
        makedirs('obj')
        makedirs('assets')
        makedirs('apk')

        shutil.copy('Assets.dat', os.path.join('assets', 'Assets.dat'))
        self.call([AAPT, 'package', '-m', '-J', 'gen', '-A', 'assets', '-M',
                   './AndroidManifest.xml', '-S', RES_DIR, '-I',
                   self.android_jar])
        self.call(['javac', '-source', '1.6', '-target', '1.6', '-classpath',
                   self.android_jar, '-d', 'obj',
                   '-sourcepath', '%s;gen' % ANDROID_SRC] + JAVA_SRCS)
        self.call([DX, '--dex', '--verbose', '--output=bin/classes.dex',
                   'obj'])
        self.call([DX, '--dex', '--verbose', '--output=bin/classes.dex',
                   'obj'])
        self.call([AAPT, 'package', '-f', '-A', 'assets', '-M',
                   './AndroidManifest.xml', '-S', RES_DIR, '-I',
                   self.android_jar, '-F', 'apk/unsigned.apk', 'bin'])
        # if not os.path.isfile('debug.keystore'):
        #     self.create_keystore()

        self.call(['jarsigner', '-verbose', '-sigalg', 'SHA1withRSA',
                   '-digestalg', 'SHA1', '-keystore', DEBUG_KEYSTORE,
                   '-storepass', 'android', '-keypass', 'android',
                   '-signedjar', 'apk/signed.apk', 'apk/unsigned.apk',
                   'androiddebugkey'])
        self.call([ZIPALIGN, '-f', '4', 'apk/signed.apk', 'apk/release.apk'])

        if self.args.install:
            self.install()

    def install(self):
        self.call([ADB, 'install', '-r', 'apk/release.apk'])

    def create_keystore(self):
        self.call(['keytool', '-genkey', '-v', '-keystore', 'debug.keystore',
                   '-storepass', 'android', '-alias', 'androiddebugkey',
                   '-keypass', 'android', '-keyalg', 'RSA', '-validity', 
                   '14000', '-dname', 'CN=Android Debug,O=Android,C=US'])

    def build_arch(self, arch):
        self.arch = arch
        self.build_dir = 'build_%s' % arch
        self.create_project()
        self.build_project()

        makedirs(os.path.join('bin', 'lib', arch))
        shutil.copy(os.path.join('libs', self.arch, 'libChowdren.so'),
                    os.path.join('bin', 'lib', arch, 'libChowdren.so'))

    def build_project(self):
        cwd = os.getcwd()
        os.chdir(self.build_dir)
        self.call([self.get_cmake_path(), '--build', '.', '--', '-j6'])
        os.chdir(cwd)

    def get_cmake_args(self):
        toolchain = os.path.join(base_dir, self.get_cmake_path(),
                                 'android.toolchain.cmake')
        return ['-GMinGW Makefiles',
                '-DCMAKE_TOOLCHAIN_FILE=%s' % toolchain,
                '-DANDROID_ABI=%s' % self.arch,
                '-DANDROID_NATIVE_API_LEVEL=%s' % self.target_version]

CMAKE_URL = 'https://cmake.org/files/v3.4/cmake-3.4.0-rc3-Darwin-x86_64.tar.gz'

SET_PATHS = (
    ('@rpath/SDL2.framework/Versions/A/SDL2',
     '@executable_path/../MacOS/libSDL2.dylib'),
    ('@loader_path/libsteam_api.dylib',
     '@executable_path/../MacOS/libsteam_api.dylib'),
    ('/Users/travis/build/LWJGL-CI/openal-soft/OSX/libopenal.1.dylib',
     '@executable_path/../MacOS/libopenal.1.dylib')
)

CODE_FILES = (
    'MacOS/Chowdren',
    'MacOS/libopenal.1.dylib',
    'MacOS/libsteam_api.dylib',
    'MacOS/libSDL2.dylib'
)

ID_FILES = (
    ('MacOS/libopenal.1.dylib', '@executable_path/../MacOS/libopenal.1.dylib'),
    ('MacOS/libsteam_api.dylib',
     '@executable_path/../MacOS/libsteam_api.dylib'),
    ('MacOS/libSDL2.dylib', '@executable_path/../MacOS/libSDL2.dylib')
)

class MacBuilder(Builder):
    def get_cmake_args(self):
        return ['-DCMAKE_OSX_DEPLOYMENT_TARGET=10.6', '-GXcode']

    def install_cmake(self):
        print 'Installing CMake...'
        cwd = os.getcwd()
        temp = tempfile.mkdtemp()

        os.chdir(temp)
        data = urllib2.urlopen(CMAKE_URL).read()
        with open('cmake.tar.gz', 'wb') as fp:
            fp.write(data)
        archive = tarfile.open('cmake.tar.gz')
        archive.extractall()

        directory = CMAKE_URL.split('/')[-1].replace('.tar.gz', '')
        app_path = os.path.join(directory, 'CMake.app')
        shutil.copytree(app_path, '/Applications/CMake.app')
        os.chdir(cwd)

        print 'Installed CMake.'

    def build(self):
        if not os.path.isdir('/Applications/CMake.app'):
            self.install_cmake()

        if not os.path.isdir('/Applications/Xcode.app'):
            print ('Error: Please install Xcode from the App Store, then open '
                   'and accept its EULA.')
            return

        self.build_dir = os.path.join(self.root_dir, 'build2')
        self.create_project()
        self.build_project()

    def build_project(self):
        cwd = os.getcwd()
        os.chdir(self.build_dir)
        self.call([self.get_cmake_path(), '--build', '.',
                   '--config', 'Release'])
        app_path = os.path.join('Release', 'Chowdren.app', 'Contents')
        lib_dir = os.path.join(self.base_path, 'lib', 'osx')
        shutil.copy(os.path.join(lib_dir, 'libopenal.dylib'),
                    os.path.join(app_path, 'MacOS', 'libopenal.1.dylib'))
        shutil.copy(os.path.join(lib_dir, 'libSDL2.dylib'),
                    os.path.join(app_path, 'MacOS', 'libSDL2.dylib'))
        if self.args.steam:
            steam_path = os.path.join(self.base_path, 'steam', 'sdk',
                                      'redistributable_bin', 'osx32')
            shutil.copy(os.path.join(steam_path, 'libsteam_api.dylib'),
                os.path.join(app_path, 'MacOS', 'libsteam_api.dylib'))
        for (name, id_path) in ID_FILES:
            path = os.path.join(app_path, name)
            if not os.path.isfile(path):
                continue
            self.call(['install_name_tool', '-id', id_path, path])
        for name in CODE_FILES:
            path = os.path.join(app_path, name)
            if not os.path.isfile(path):
                continue
            for (src_path, to_path) in SET_PATHS:
                self.call(['install_name_tool', '-change', src_path, to_path,
                           path])
        shutil.copy(os.path.join(self.root_dir, 'Assets.dat'),
                    os.path.join(app_path, 'Resources', 'Assets.dat'))
        os.chdir(cwd)

    def get_cmake_path(self):
        return '/Applications/CMake.app/Contents/bin/cmake'

BUILDERS = {
    'android': AndroidBuilder
}

def main():
    with open('config.py', 'rb') as fp:
        config = eval(fp.read())
    if config['platform'] == 'generic':
        import platform
        if platform.system() == 'Linux':
            build_class = LinuxBuilder
        else:
            build_class = MacBuilder
    else:
        build_class = BUILDERS[config['platform']]

    builder = build_class()
    base_path = config['base_path'].replace('${CMAKE_CURRENT_SOURCE_DIR}',
                                            os.getcwd())
    builder.base_path = base_path

    parser = argparse.ArgumentParser(description='Chowdren builder')
    builder.setup_parser(parser)
    parser.add_argument('--steam', action='store_true',
                        help='Performs a build with Steamworks')
    parser.add_argument('--build_type')
    builder.setup(parser.parse_args())
    builder.build()
    builder.finish()

if __name__ == '__main__':
    main()
