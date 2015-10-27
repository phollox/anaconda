from mmfparser.bytereader import ByteReader
import sys
from PIL import Image

def to_png(data):
    reader = ByteReader(data)
    width = reader.readInt(True)
    height = reader.readInt(True)
    data = reader.read(width*height)
    return Image.frombytes('L', (width, height), data)

def main():
    with open(sys.argv[1], 'rb') as fp:
        data = fp.read()
    image = to_png(data)
    image.save(sys.argv[2])

if __name__ == '__main__':
    main()