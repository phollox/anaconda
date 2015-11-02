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

from mmfparser.gperf import get_hash_function
from chowdren.codewriter import CodeWriter

def get_string_int_map(map_func, hash_func, string_map, case_sensitive=True):
    strings = list(string_map.iterkeys())
    writer = CodeWriter()
    if strings:
        hash_data = get_hash_function(hash_func, strings, case_sensitive)
        writer.putlnc(hash_data.code)
        hashes = dict((v, k) for (k, v) in hash_data.strings.iteritems())

    writer.putmeth('int %s' % map_func, 'const std::string & in')
    writer.putln('if (in.empty())')
    writer.indent()
    writer.putln('return -1;')
    writer.dedent()

    if strings:
        writer.putlnc('unsigned int hash = %s(&in[0], in.size());',
                      hash_func)
        writer.putlnc('switch (hash) {')
        writer.indent()
        for i in xrange(hash_data.max_hash_value + 1):
            value = hashes.get(i, None)
            if value is None:
                continue
            else:
                value = string_map[value]
            writer.putlnc('case %s: return %s;', i, value)
        writer.end_brace()
    writer.putlnc('return -1;')
    writer.end_brace()
    return writer.get_data()