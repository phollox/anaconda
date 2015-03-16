"""
Build script for Linux and Mac
"""
import shutil
import tempfile
import os

chroot_prefix = 'steamrt_scout_'
chroots = '/var/chroots'

steamrt_archive = 'https://github.com/ValveSoftware/archive/master.tar.gz'

class Builder(object):
    def __init__(self):
        self.root_dir = os.getcwd()

    def create_project(self):
        cwd = os.getcwd()
        try:
            os.makedirs(self.build_dir)
        except OSError:
            pass

        os.chdir(self.build_dir)
        self.system('cmake .. -DCMAKE_BUILD_TYPE=Release')
        os.chdir(cwd)

    def system(self, command):
        return os.system(command)

class LinuxBuilder(Builder):
    temp = None

    def install_chroot(arch):
        chroot = chroot_prefix + arch
        if os.path.isdir(os.path.join(chroots, chroot)):
            return chroot
        cwd = os.getcwd()
        if self.temp is None:
            self.temp = tempfile.mkdtemp()
            os.chdir(self.temp)
            self.system('curl -L %s | tar xz' % steamrt_archive)

        os.chdir(os.path.join(self.temp, 'steam-runtime'))
        self.system('./setup_chroot --%s' % arch)
        os.chdir(cwd)
        return chroot

    def system(self, command):
        if self.chroot:
            command = 'schroot --chroot %s -- %s' % (self.chroot, command)
        return os.system(command)

    def build_arch(arch):
        chroot = self.install_chroot(arch)
        self.build_dir = os.path.join(self.root_dir, 'build_%s' % arch)
        self.install_dir = os.path.join(self.build_dir, 'install')
        self.dist_dir = os.path.join(self.root_dir, 'dist')
        self.chroot = chroot
        self.create_project()
        self.build_project()
        self.copy_dependencies(arch)

    def copy_dependencies(self, arch):
        try:
            os.makedirs(self.dist_dir)
        except OSError:
            pass

        if arch == 'amd64':
            bin_dir = 'bin32'
        else:
            bin_dir = 'bin64'

        src_bin_dir = os.path.join(self.install_dir, bin_dir)
        dst_bin_dir = os.path.join(self.dist_dir, bin_dir)

        shutil.copytree(src_bin_dir, dst_bin_dir)

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

    def build_project(self):
        cwd = os.getcwd()
        os.chdir(self.build_dir)
        self.system('make -j4')
        self.system('make install')
        os.chdir(cwd)

    def build():
        self.build_arch('amd64')
        self.build_arch('i386')
        self.make_dist()

    def finish(self):
        if self.temp is None:
            return
        shutil.rmtree(self.temp)

class MacBuilder(Builder):
    pass

def main():
    import platform
    if platform.system() == 'Linux':
        builder = LinuxBuilder()
        builder.build()
    else:
        builder = MacBuilder()
        builder.build()

    builder.finish()

if __name__ == '__main__':
    main()
