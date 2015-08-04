import sys
sys.path.append('../../')

from mmfparser.data.mfa import (MFA, Backdrop, FrameItem, ChunkList,
                                ItemFolder, FrameInstance, QuickBackdrop)
from mmfparser.bytereader import ByteReader
from mmfparser.data.chunkloaders.imagebank import ImageItem
from mmfparser.data.chunkloaders.objectinfo import BACKDROP, QUICKBACKDROP
from mmfparser.data.chunkloaders.frame import NONE_PARENT
from mmfparser.player.dialog import open_file_selector
from mmfparser.data.chunkloaders import objects
from PIL import Image

import sys
import os

BIN_DIR = os.path.dirname(os.path.realpath(sys.argv[0]))

if hasattr(sys, 'frozen'):
    DATA_DIR = os.path.realpath(os.path.join(BIN_DIR, '..', '..'))
else:
    DATA_DIR = BIN_DIR

SETS_PATH = os.path.join(DATA_DIR, u'sets')

class Tiler(object):
    def __init__(self, filename):
        filename = os.path.join(DATA_DIR, filename)
        self.filename = filename

        self.tiles = os.stat(filename).st_size != 0

        name = os.path.basename(filename).encode('utf-8')
        self.name = os.path.splitext(name)[0]

        self.outdir = os.path.join(DATA_DIR, 'mfas')# + self.name
        try:
            os.makedirs(self.outdir)
        except OSError:
            pass

        self.objects = {}

        self.x_size = 16
        self.y_size = 16

        template_path = os.path.join(DATA_DIR, 'template.mfa')
        self.mfa = MFA(ByteReader(open(template_path, 'rb')))

        self.object_id = 32
        self.image_id = 0
        self.icon_id = max(self.mfa.icons.itemDict) + 1

    def run(self, cmd):
        self.room = ByteReader(open(self.filename, 'rb'))
        self.file_ver = self.room.readByte(True)
        self.x_size = self.room.readByte(True)
        self.y_size = self.room.readByte(True)
        tileset_len = self.room.readByte(True)
        self.tileset = self.room.readString(tileset_len)
        self.tile_count = self.room.readInt(True)

        image_path = os.path.join(SETS_PATH, '%s.png' % self.tileset)
        image = Image.open(image_path).convert('RGBA')

        col = image.getpixel((0, 0))
        self.tileset_transparent = col[:3]
        if col[3] == 255:
            print 'Filtering image with transparent color', col
            buf = image.load()
            for y in xrange(image.size[1]):
                for x in xrange(image.size[0]):
                    test_col = buf[(x, y)]
                    if test_col != col:
                        continue
                    buf[(x, y)] = (0, 0, 0, 0)

        frame = self.mfa.frames[0]

        if cmd == 'extract_tiles':
            self.run_extract_tiles(image)
            return
        elif cmd == 'build':
            self.run_build(image)

        frame.folders = []
        for item in frame.items:
            folder = frame.new(ItemFolder)
            folder.items = [item.handle]
            frame.folders.append(folder)

        basename = os.path.basename(self.filename)
        out_name = os.path.splitext(basename)[0] + '.mfa'
        out_path = os.path.join(self.outdir, out_name)

        if os.path.isfile(out_path):
            os.remove(out_path)
        self.mfa.write(ByteReader(open(out_path, 'wb')))
        print 'Created %s' % out_name

    def run_build(self, image):
        mask_path = os.path.join(SETS_PATH, '%s_mask.png' % self.tileset)
        mask = Image.open(mask_path)

        if mask.mode != 'P':
            raise ValueError('The mask has to be palette image')

        #mask = mask.convert('P')
        colors = mask.getcolors()
        transparent = mask.getpixel((0, 0))

        if transparent != 0:
            raise ValueError('The transparent color should be the first color '
                             'in the palette')

        buf = image.load()

        for (_, color) in colors:
            if color == transparent or color == 255:
                continue
            color_image = mask.point(lambda x: 1 if x == color else 0)
            box = color_image.getbbox()
            tile_x = box[0] / self.x_size
            tile_y = box[1] / self.y_size
            height = box[3] - box[1]
            width = box[2] - box[0]
            new_image = Image.new('RGBA', (width, height),
                                  self.tileset_transparent)

            buf2 = color_image.load()
            buf3 = new_image.load()


            for y in xrange(height):
                for x in xrange(width):
                    xx = x + box[0]
                    yy = y + box[1]
                    if buf2[(xx, yy)] == 0 or buf[(xx, yy)][3] == 0:
                        continue
                    buf3[(x, y)] = buf[(xx, yy)]

            obstacle = mask.getpixel((tile_x, tile_y)) == 255
            self.create_object("%s%s" % (self.tileset, color), color,
                               new_image, obstacle)

        room_width = 0
        room_height = 0
        instances = []
        map = dict()
        for _ in xrange(self.tile_count):
            try:
                x = self.room.readShort() * self.x_size
                y = self.room.readShort() * self.y_size
                room_width = max(room_width, x + 200)
                room_height = max(room_height, y + 200)
                key = self.room.readByte(True)
                frameitem = self.objects[key]
                instances.append((frameitem, x, y))
                map[(x,y)] = 1
            except:
                break

        for i in instances:
            self.create_instance(*i)
            #statt self.create_instance(i[0], i[1], i[2])

        frame = self.mfa.frames[0]
        frame.size = room_width, room_height

    def run_extract_tiles(self, image):
        tile_folder = sys.argv[3]

        image_path = os.path.join(SETS_PATH, '%s.png' % self.tileset)
        image = Image.open(image_path).convert('RGBA')
        mask_path = os.path.join(SETS_PATH, '%s_mask.png' % self.tileset)
        mask = Image.open(mask_path)
        transparent_color = image.getpixel((0, 0))

        if mask.mode != 'P':
            raise ValueError('The mask has to be palette image')

        #mask = mask.convert('P')
        colors = mask.getcolors()
        transparent = mask.getpixel((0, 0))

        if transparent != 0:
            raise ValueError('The transparent color should be the first color '
                             'in the palette')

        ini = open(os.path.join(DATA_DIR, tile_folder, 'tiles.ini'), 'w')
        for (_, color) in colors:
            if color == transparent:
                continue
            # Extract the current tile's shape from the mask image
            tile_mask = mask.point(lambda x: 1 if x == color else 0, '1')
            box = tile_mask.getbbox()
            tile_mask = tile_mask.crop(box)
            # Create an image and render the tile to it
            tile = Image.new('RGBA', tile_mask.size)
            tile.paste(image.crop(box), tile_mask)
            col = image.getpixel((0, 0))
            buf = tile.load()
            # Clear transparent pixels
            for y in xrange(tile.size[1]):
                for x in xrange(tile.size[0]):
                    if buf[(x, y)] == transparent_color:
                        buf[(x, y)] = (0, 0, 0, 0)
            tile.save(os.path.join(DATA_DIR, tile_folder, "%s.png" % color),
                      "PNG")

            x = box[0]
            y = box[1]
            height = box[3] - y
            width = box[2] - x
            tile_x = x / self.x_size
            tile_y = y / self.y_size

            if color == 25:
                print tile_x * self.x_size, x

            ini.write("[t%s]\n" % color)
            ini.write("x=%s\n" % x)
            ini.write("y=%s\n" % y)
            ini.write("x_off=%s\n" % (x - tile_x * self.x_size))
            ini.write("y_off=%s\n" % (y - tile_y * self.y_size))
            ini.write("width=%s\n" % width)
            ini.write("height=%s\n" % height)

        ini.write("[image]\n")
        ini.write("width=%s\n" % image.size[0])
        ini.write("height=%s\n" % image.size[1])
        ini.close()

    def create_instance(self, frameitem, x, y):
        frame = self.mfa.frames[0]
        instance = frame.new(FrameInstance)
        instance.x = x
        instance.y = y
        instance.handle = len(frame.instances) + 1
        instance.flags = 0
        instance.parentType = NONE_PARENT
        instance.itemHandle = frameitem.handle
        instance.parentHandle = 0
        instance.layer = 0
        frame.instances.append(instance)

    def create_image(self, image, icon):
        if icon:
            bank = self.mfa.icons
            handle = self.icon_id
            self.icon_id += 1
            image = image.resize((32, 32), Image.LANCZOS)
        else:
            bank = self.mfa.images
            handle = self.image_id
            self.image_id += 1

        item = bank.new(ImageItem, debug=True)
        item.handle = handle

        item.checksum = 123
        item.references = 0
        item.width = image.size[0]
        item.height = image.size[1]
        item.xHotspot = item.yHotspot = item.actionX = item.actionY = 0
        item.flags['Alpha'] = False#True
        item.transparent = self.tileset_transparent
        item.graphicMode = 4

        item.image = image.tobytes('raw', 'RGBA')
        item.alpha = image.tobytes('raw', 'A')

        bank.items.append(item)
        bank.itemDict[item.handle] = item
        return item

    def create_object(self, name, key, image, obstacle=False, size=None):
        item = self.create_image(image, False)
        icon = self.create_image(image, True)

        frame = self.mfa.frames[0]
        frameitem = frame.new(FrameItem)
        frame.items.append(frameitem)

        frameitem.name = name
        if size is not None:
            frameitem.objectType = QUICKBACKDROP
        else:
            frameitem.objectType = BACKDROP
        frameitem.handle = self.object_id
        self.object_id += 1
        frameitem.transparent = True
        frameitem.inkEffect = frameitem.inkEffectParameter = 0
        frameitem.antiAliasing = False
        frameitem.iconHandle = icon.handle
        frameitem.chunks = frameitem.new(ChunkList)

        if size is not None:
            obj = frameitem.new(QuickBackdrop)
            obj.width = size[0]
            obj.height = size[1]
            obj.shape = objects.RECTANGLE_SHAPE
            obj.borderSize = 0
            obj.borderColor = (0, 0, 0)
            obj.fillType = objects.MOTIF_FILL
            obj.color1 = (0, 0, 0)
            obj.color2 = (0, 0, 0)
            obj.image = item.handle
        else:
            obj = frameitem.new(Backdrop)
            obj.handle = item.handle
        obj.obstacleType = 1 if obstacle else 0
        obj.collisionType = 0

        frameitem.loader = obj

        if key is not None:
            self.objects[key] = frameitem

        return frameitem

def main():

    if len(sys.argv) < 3:
        sys.exit(1)

    cmd = sys.argv[1]
    room = sys.argv[2]

    tiler = Tiler(room)
    tiler.run(cmd)
    return

if __name__ == '__main__':
    main()
