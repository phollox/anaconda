from mmfparser._zopfli import lib, ffi

lib.init_zopfli_internal()

def compress(data, level=None):
    out_size = ffi.new('size_t*')
    out_data = ffi.new('char**')
    lib.compress_zopfli(data, len(data), out_data, out_size)
    out_size = out_size[0]
    out_data = out_data[0]
    out_data = ffi.gc(out_data, lib.free)
    return ffi.buffer(out_data, out_size)
