from chowdren.platforms.generic import GenericPlatform
from chowdren.shader import translate_shader_data
from mmfparser.bytereader import ByteReader

GRADLE_BUILD = '''\
apply plugin: 'com.android.application'

android {
    compileSdkVersion 12
    buildToolsVersion "12.0.0"

    buildTypes {
        release {
            minifyEnabled true
        }
    }
}

dependencies {
    compile project(":lib")
    compile 'com.android.support:appcompat-v7:19.0.1'
    compile fileTree(dir: 'libs', include: ['*.jar'])
}
'''

class AndroidPlatform(GenericPlatform):
    def get_shader(self, name, vert, frag):
        vert = translate_shader_data(vert, 'vertex', 'gles')
        frag = translate_shader_data(frag, 'fragment', 'gles')
        writer = ByteReader()
        writer.writeInt(len(vert))
        writer.write(vert)
        writer.writeInt(len(frag))
        writer.write(frag)
        return str(writer)

    def install(self):
        pass