# Copyright (c) Mathias Kaerlev 2012-2015.
#
# This file is part of Anaconda.
#
# Anaconda is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Anaconda is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Anaconda.  If not, see <http://www.gnu.org/licenses/>.

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
