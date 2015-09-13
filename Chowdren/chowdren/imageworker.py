def worker(in_queue, use_zopfli=True):
    if use_zopfli:
        from mmfparser import zopfli as complib
    else:
        import zlib as complib
    while True:
        obj = in_queue.get()
        if obj is None:
            return
        data, index, p, cache_path = obj
        print 'Compressing %s (%s)' % (index, p)
        data = complib.compress(data)
        open(cache_path, 'wb').write(data)
