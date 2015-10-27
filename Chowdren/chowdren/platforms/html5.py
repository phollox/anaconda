from chowdren.platforms.generic import GenericPlatform
from chowdren.shader import translate_shader_data
from mmfparser.bytereader import ByteReader

class HTML5Platform(GenericPlatform):
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
