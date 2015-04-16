from cStringIO import StringIO
from mmfparser.bytereader import ByteReader
from mmfparser.webp import encode

class Platform(object):
    save_dir = '.'

    def __init__(self, converter):
        self.converter = converter
        self.initialize()
        self.problem_images = 0

    def get_image(self, image):
        if self.converter.config.use_webp():
            webp = encode(image.tobytes('raw', 'RGBA'),
                          image.size[0], image.size[1])
            return webp
        else:
            temp = StringIO()
            image.save(temp, 'PNG')
            return temp.getvalue()

    def get_shader(self, vertex, fragment):
        raise NotImplementedError()

    def initialize(self):
        pass

    def install(self):
        pass

    def get_sound(self, name, data):
        return name, data
