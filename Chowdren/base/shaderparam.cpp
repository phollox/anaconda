// Copyright (c) Mathias Kaerlev 2012-2015.
//
// This file is part of Anaconda.
//
// Anaconda is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Anaconda is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Anaconda.  If not, see <http://www.gnu.org/licenses/>.

unsigned int
hash_shader_parameter (register const char *str, register unsigned int len)
{
  static unsigned char asso_values[] =
    {
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183,  30,   5,  50,   5,  10,
       10,  20,  75,  55, 183,   0,   0,   0,  10,  80,
        5, 183,   0,  50,  15, 183,   0,   5,  40,  35,
        5, 183, 183, 183, 183,  90, 183,  30,   5,  50,
        5,  10,  10,  20,  75,  55, 183,   0,   0,   0,
       10,  80,   5, 183,   0,  50,  15, 183,   0,   5,
       40,  35,   5, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
      183, 183, 183, 183, 183, 183
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
      case 4:
      case 3:
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}


