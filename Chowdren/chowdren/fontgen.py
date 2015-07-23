import sys
sys.path.append('..')
import freetype
from ctypes import byref
from mmfparser.bytereader import ByteReader
from PIL import Image

"""
XXX use kerning information?

Format:

struct Glyph
{
    uint32 charcode
    float x1, y1, x2, y2
    float advance_x, advance_y
    float corner_x, corner_y
    uint32 width, height
    uint8 data[width * height];
};

struct Font
{
    uint32 size;
    float width;
    float height;
    float ascender;
    float descender;
    uint32 num_glyphs;
    Glyph glyphs[num_glyphs];
};

struct FontBank
{
    uint32 font_count;
    Font fonts[font_count];
};
"""

class Glyph(object):
    def __init__(self, generator, char):
        self.char = char
        self.font = generator.font
        self.glyph = glyph = generator.font.glyph
        self.advance = (glyph.advance.x / 64.0, glyph.advance.y / 64.0)
        box = self.get_cbox()
        self.x1 = box.xMin / 64.0
        self.y1 = box.yMin / 64.0
        self.x2 = box.xMax / 64.0
        self.y2 = box.yMax / 64.0
        real_glyph = glyph.get_glyph()
        bitmap_glyph = real_glyph.to_bitmap(freetype.FT_RENDER_MODE_NORMAL, 0,
                                            True)
        self.corner = (bitmap_glyph.left, bitmap_glyph.top)
        bitmap = bitmap_glyph.bitmap
        self.width = bitmap.width
        self.height = bitmap.rows
        self.buf = bitmap.buffer
        self.horizontal_advance = glyph.metrics.horiAdvance

    def get_cbox(self):
        outline = self.glyph.outline
        bbox = freetype.FT_BBox()
        freetype.FT_Outline_Get_CBox(byref(outline._FT_Outline), byref(bbox))
        return freetype.BBox(bbox)

    def get_data(self):
        buf = ''
        for c in self.buf:
            buf += chr(c)
        return buf

    def get_image(self):
        data = self.get_data()
        image = Image.frombytes('L', (self.width, self.height), data)
        return image

    def write(self, writer):
        writer.writeInt(ord(self.char), True)
        writer.writeFloat(self.x1)
        writer.writeFloat(self.y1)
        writer.writeFloat(self.x2)
        writer.writeFloat(self.y2)
        writer.writeFloat(self.advance[0])
        writer.writeFloat(self.advance[1])
        writer.writeFloat(self.corner[0])
        writer.writeFloat(self.corner[1])
        writer.writeInt(self.width)
        writer.writeInt(self.height)
        writer.write(self.get_data())


RESOLUTION = 96


class Font(object):
    def __init__(self, name, charset, size=None, pixel_size=None):
        self.size = size
        font = self.font = freetype.Face(name)

        if pixel_size is not None:
            font.set_pixel_sizes(pixel_size, pixel_size)
        else:
            size = int(size * 64)
            font.set_char_size(size, size, RESOLUTION, RESOLUTION)

        self.width = self.get_width()
        self.height = self.get_height()
        metrics = self.font.size
        self.ascender = metrics.ascender / 64.0
        self.descender = metrics.descender / 64.0
        self.glyph_dict = {}
        self.glyphs = []
        self.charmap = {}
        charcode, agindex = font.get_first_char()
        while agindex != 0:
            self.charmap[agindex] = charcode
            charcode, agindex = font.get_next_char(charcode, agindex)
        for c in charset:
            self.load_char(c)
        space_glyph = self.load_char(' ', force=True)
        print space_glyph.advance, space_glyph.horizontal_advance

    def get_width(self):
        size = self.font.size
        if self.font.is_scalable:
            bbox = self.font.bbox
            return (bbox.xMax - bbox.xMin) * (float(size.x_ppem) /
                                              self.font.units_per_EM)
        else:
            return size.max_advance / 64.0

    def get_height(self):
        size = self.font.size
        if self.font.is_scalable:
            bbox = self.font.bbox
            return (bbox.yMax - bbox.yMin) * (float(size.y_ppem) /
                                              self.font.units_per_EM)
        else:
            return size.height / 64.0

    def load_char(self, c, force=False):
        self.font.load_char(c, freetype.FT_LOAD_FORCE_AUTOHINT)
        glyph = Glyph(self, c)
        if (glyph.x1 == 0.0 and glyph.y1 == 0.0 and
            glyph.x2 == 0.0 and glyph.y2 == 0.0 and not force):
            return
        self.glyphs.append(glyph)
        self.glyph_dict[c] = glyph
        return glyph

    def get_sheet(self):
        width = sum(glyph.width for glyph in self.glyphs)
        height = max(glyph.height for glyph in self.glyphs)
        image = Image.new('L', (width, height), 1)
        x = 0
        for glyph in self.glyphs:
            if glyph.width == 0 or glyph.height == 0:
                continue
            image.paste(glyph.get_image(), (x, 0))
            x += glyph.width
        return image

    def get_glyph(self, c):
        return self.glyph_dict[c]

    def write(self, writer):
        writer.writeInt(self.size)
        writer.writeFloat(self.width)
        writer.writeFloat(self.height)
        writer.writeFloat(self.ascender)
        writer.writeFloat(self.descender)
        writer.writeInt(len(self.glyphs))
        for glyph in self.glyphs:
            glyph.write(writer)

def generate_font(filename, charset, out, sizes):
    fonts = []
    for size in sizes:
        fonts.append(Font(filename, size, charset))
    write_fonts(out, fonts)

def write_fonts(out, fonts):
    fp = open(out, 'wb')
    writer = ByteReader(fp)
    writer.writeInt(len(fonts))
    for font in fonts:
        font.write(writer)
    fp.close()