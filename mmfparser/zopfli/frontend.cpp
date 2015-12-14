#include "zopfli/zopfli.h"

static ZopfliOptions options;

void init_zopfli_internal()
{
    ZopfliInitOptions(&options);
}

void compress_zopfli(char * data, size_t size, char ** out, size_t * out_size)
{
    ZopfliCompress(&options, ZOPFLI_FORMAT_ZLIB,
                   (const unsigned char*)data, size,
                   (unsigned char**)out, out_size);
}
