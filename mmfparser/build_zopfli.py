import glob
import os
from cffi import FFI
ffi = FFI()


zopfli_dir = os.path.join(os.path.dirname(__file__), 'zopfli')
zopfli_srcs = glob.glob(os.path.join(zopfli_dir, 'zopfli', '*.c'))

ffi.set_source("mmfparser._zopfli",
    '''
    #include "frontend.cpp"
    #include <stdlib.h>
    ''',
    sources=zopfli_srcs, include_dirs=[zopfli_dir]
)

ffi.cdef('''
    void init_zopfli_internal();
    void compress_zopfli(char * data, size_t size,
                         char ** out, size_t * out_size);
    void free(void * data);
    ''')

if __name__ == "__main__":
    ffi.compile()