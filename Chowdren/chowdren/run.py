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

import sys
sys.path.append('..')

from chowdren.converter import Converter
import argparse

def main():
    parser = argparse.ArgumentParser(
        description='Chowdren - MMF to C++ converter')
    parser.add_argument('filenames', type=str,
        help='input files to convert (should be an EXE or CCN file)',
        nargs='+')
    parser.add_argument('outdir', type=str, help='destination directory')
    parser.add_argument('--assets', type=str, action='store',
        default='Assets.dat', help='destination file for assets')
    parser.add_argument('--skipassets', action='store_true',
        help='do not generate an assets file')
    parser.add_argument('--ico', type=str, action='store', default=None,
        help='icon to use for Windows')
    parser.add_argument('--icns', type=str, action='store', default=None,
        help='icon to use for OS X')
    parser.add_argument('--version', type=str, action='store',
        default=None, help='version to set in executable')
    parser.add_argument('--company', type=str, action='store',
        default=None, help='company to set in executable')
    parser.add_argument('--copyright', type=str, action='store',
        default=None, help='copyright to set in executable')
    parser.add_argument('--dlls', action='store_true',
                        help='use DLL architecture')
    parser.add_argument('--platform', type=str, action='store',
        default=None, help='platform for which to generate')
    parser.add_argument('--config', type=str, action='store', default=None,
                        help='game-specific configuration file')
    parser.add_argument('--copy_base', action='store_true',
                        help='copy base runtime')
    parser.add_argument('--zlib', action='store_true',
                        help='use zlib instead of zopfli')
    args = parser.parse_args()
    Converter(args)

if __name__ == '__main__':
    main()