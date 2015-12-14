# Copyright (c) Mathias Kaerlev 2012.

# This file is part of Anaconda.

# Anaconda is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# Anaconda is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with Anaconda.  If not, see <http://www.gnu.org/licenses/>.

import glob
import sys
import os
import platform
import cffi
ffi = cffi.FFI()
from setuptools import setup

ext_modules = []
libraries = []
include_dirs = ['./mmfparser/player']

names = open('names.txt', 'rb').read().splitlines()

is_pypy = platform.python_implementation() == 'PyPy'

trans_start = os.environ.get('MMF_TRANS_START', None)
compile_env = {'IS_PYPY': True, 'USE_TRANS': trans_start is not None}
define_macros = []
if trans_start is not None:
    define_macros.append(('TRANS_START', trans_start))

kw = dict(language='c++')

# for name in names:
#     if name.startswith('#'):
#         continue
#     ext_modules.append(Extension(name,
#                                  ['./' + name.replace('.', '/') + '.pyx'],
#                                  define_macros=define_macros,
#                                  include_dirs=include_dirs, **kw))

# webp_srcs = glob.glob('./mmfparser/webp/*/*.c')
# ext_modules.append(Extension('mmfparser.webp',
#                              ['./mmfparser/webp.pyx'] + webp_srcs,
#                              include_dirs=include_dirs + ['./mmfparser/webp'],
#                              **kw))

# zopfli_srcs = glob.glob('./mmfparser/zopfli/zopfli/*.c')
# ext_modules.append(Extension('mmfparser.zopfli',
#                              ['./mmfparser/zopfli.pyx'] + zopfli_srcs,
#                              include_dirs=include_dirs + ['./mmfparser/zopfli'],
#                              **kw))

import _cffi_backend
requires_cffi = "cffi==" + _cffi_backend.__version__

setup(
    name = 'mmfparser extensions',
    install_requires=[requires_cffi],
    cffi_modules=["./mmfparser/build_zopfli.py:ffi"],
    setup_requires=[requires_cffi]
)
