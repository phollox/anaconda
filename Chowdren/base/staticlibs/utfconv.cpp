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

#include "utfconv.h"
#include "utf8/unchecked.h"

void convert_utf8_to_utf16(const std::string & value, std::string & out)
{
    if (value.empty()) {
        out.resize(0);
        return;
    }
    unsigned char * source = (unsigned char*)&value[0];
    unsigned char * end = source + value.size();
    out.resize(value.size() * 2);

    union {
        unsigned short * target;
        unsigned char * target_c;
    };
    union {
        unsigned short * res;
        unsigned char * res_c;
    };
    target_c = (unsigned char*)&out[0];
    res = utf8::unchecked::utf8to16(source, end, target);
    out.resize((unsigned long)(res_c - target_c));
}

void convert_utf16_to_utf8(const std::string & value, std::string & out)
{
    if (value.empty()) {
        out.resize(0);
        return;
    }
    union {
        unsigned short * source;
        unsigned char * source_c;
    };
    union {
        unsigned short * end;
        unsigned char * end_c;
    };
    source_c = (unsigned char*)&value[0];
    end_c = (unsigned char*)(&value[0] + value.size());
    if (value.size() >= 2 && (unsigned char)value[0] == 0xFF
        && (unsigned char)value[1] == 0xFE)
    {
        source++;
    }
    out.resize((value.size() / 2) * 4);
    unsigned char * target = (unsigned char*)&out[0];
    unsigned char * res = utf8::unchecked::utf16to8(source, end, target);
    out.resize((unsigned long)(res - target));
}

void convert_windows1252_to_utf8(const std::string & value, std::string & out)
{
    std::string::const_iterator it;
    out.clear();
    for (it = value.begin(); it != value.end(); ++it) {
        char c = *it;
        unsigned char cc = (unsigned char)c;
        if (cc < 128) {
            out.push_back(c);
            continue;
        }
        switch (cc) {
            case 0x80:
                out.append("\342\202\254", 3);
                break;
            case 0x81:
                break;
            case 0x82:
                out.append("\342\200\232", 3);
                break;
            case 0x83:
                out.append("\306\222", 2);
                break;
            case 0x84:
                out.append("\342\200\236", 3);
                break;
            case 0x85:
                out.append("\342\200\246", 3);
                break;
            case 0x86:
                out.append("\342\200\240", 3);
                break;
            case 0x87:
                out.append("\342\200\241", 3);
                break;
            case 0x88:
                out.append("\313\206", 2);
                break;
            case 0x89:
                out.append("\342\200\260", 3);
                break;
            case 0x8A:
                out.append("\305\240", 2);
                break;
            case 0x8B:
                out.append("\342\200\271", 3);
                break;
            case 0x8C:
                out.append("\305\222", 2);
                break;
            case 0x8D:
                break;
            case 0x8E:
                out.append("\305\275", 2);
                break;
            case 0x8F:
                break;
            case 0x90:
                break;
            case 0x91:
                out.append("\342\200\230", 3);
                break;
            case 0x92:
                out.append("\342\200\231", 3);
                break;
            case 0x93:
                out.append("\342\200\234", 3);
                break;
            case 0x94:
                out.append("\342\200\235", 3);
                break;
            case 0x95:
                out.append("\342\200\242", 3);
                break;
            case 0x96:
                out.append("\342\200\223", 3);
                break;
            case 0x97:
                out.append("\342\200\224", 3);
                break;
            case 0x98:
                out.append("\313\234", 2);
                break;
            case 0x99:
                out.append("\342\204\242", 3);
                break;
            case 0x9A:
                out.append("\305\241", 2);
                break;
            case 0x9B:
                out.append("\342\200\272", 3);
                break;
            case 0x9C:
                out.append("\305\223", 2);
                break;
            case 0x9D:
                break;
            case 0x9E:
                out.append("\305\276", 2);
                break;
            case 0x9F:
                out.append("\305\270", 2);
                break;
            case 0xA0:
                out.append("\302\240", 2);
                break;
            case 0xA1:
                out.append("\302\241", 2);
                break;
            case 0xA2:
                out.append("\302\242", 2);
                break;
            case 0xA3:
                out.append("\302\243", 2);
                break;
            case 0xA4:
                out.append("\302\244", 2);
                break;
            case 0xA5:
                out.append("\302\245", 2);
                break;
            case 0xA6:
                out.append("\302\246", 2);
                break;
            case 0xA7:
                out.append("\302\247", 2);
                break;
            case 0xA8:
                out.append("\302\250", 2);
                break;
            case 0xA9:
                out.append("\302\251", 2);
                break;
            case 0xAA:
                out.append("\302\252", 2);
                break;
            case 0xAB:
                out.append("\302\253", 2);
                break;
            case 0xAC:
                out.append("\302\254", 2);
                break;
            case 0xAD:
                out.append("\302\255", 2);
                break;
            case 0xAE:
                out.append("\302\256", 2);
                break;
            case 0xAF:
                out.append("\302\257", 2);
                break;
            case 0xB0:
                out.append("\302\260", 2);
                break;
            case 0xB1:
                out.append("\302\261", 2);
                break;
            case 0xB2:
                out.append("\302\262", 2);
                break;
            case 0xB3:
                out.append("\302\263", 2);
                break;
            case 0xB4:
                out.append("\302\264", 2);
                break;
            case 0xB5:
                out.append("\302\265", 2);
                break;
            case 0xB6:
                out.append("\302\266", 2);
                break;
            case 0xB7:
                out.append("\302\267", 2);
                break;
            case 0xB8:
                out.append("\302\270", 2);
                break;
            case 0xB9:
                out.append("\302\271", 2);
                break;
            case 0xBA:
                out.append("\302\272", 2);
                break;
            case 0xBB:
                out.append("\302\273", 2);
                break;
            case 0xBC:
                out.append("\302\274", 2);
                break;
            case 0xBD:
                out.append("\302\275", 2);
                break;
            case 0xBE:
                out.append("\302\276", 2);
                break;
            case 0xBF:
                out.append("\302\277", 2);
                break;
            case 0xC0:
                out.append("\303\200", 2);
                break;
            case 0xC1:
                out.append("\303\201", 2);
                break;
            case 0xC2:
                out.append("\303\202", 2);
                break;
            case 0xC3:
                out.append("\303\203", 2);
                break;
            case 0xC4:
                out.append("\303\204", 2);
                break;
            case 0xC5:
                out.append("\303\205", 2);
                break;
            case 0xC6:
                out.append("\303\206", 2);
                break;
            case 0xC7:
                out.append("\303\207", 2);
                break;
            case 0xC8:
                out.append("\303\210", 2);
                break;
            case 0xC9:
                out.append("\303\211", 2);
                break;
            case 0xCA:
                out.append("\303\212", 2);
                break;
            case 0xCB:
                out.append("\303\213", 2);
                break;
            case 0xCC:
                out.append("\303\214", 2);
                break;
            case 0xCD:
                out.append("\303\215", 2);
                break;
            case 0xCE:
                out.append("\303\216", 2);
                break;
            case 0xCF:
                out.append("\303\217", 2);
                break;
            case 0xD0:
                out.append("\303\220", 2);
                break;
            case 0xD1:
                out.append("\303\221", 2);
                break;
            case 0xD2:
                out.append("\303\222", 2);
                break;
            case 0xD3:
                out.append("\303\223", 2);
                break;
            case 0xD4:
                out.append("\303\224", 2);
                break;
            case 0xD5:
                out.append("\303\225", 2);
                break;
            case 0xD6:
                out.append("\303\226", 2);
                break;
            case 0xD7:
                out.append("\303\227", 2);
                break;
            case 0xD8:
                out.append("\303\230", 2);
                break;
            case 0xD9:
                out.append("\303\231", 2);
                break;
            case 0xDA:
                out.append("\303\232", 2);
                break;
            case 0xDB:
                out.append("\303\233", 2);
                break;
            case 0xDC:
                out.append("\303\234", 2);
                break;
            case 0xDD:
                out.append("\303\235", 2);
                break;
            case 0xDE:
                out.append("\303\236", 2);
                break;
            case 0xDF:
                out.append("\303\237", 2);
                break;
            case 0xE0:
                out.append("\303\240", 2);
                break;
            case 0xE1:
                out.append("\303\241", 2);
                break;
            case 0xE2:
                out.append("\303\242", 2);
                break;
            case 0xE3:
                out.append("\303\243", 2);
                break;
            case 0xE4:
                out.append("\303\244", 2);
                break;
            case 0xE5:
                out.append("\303\245", 2);
                break;
            case 0xE6:
                out.append("\303\246", 2);
                break;
            case 0xE7:
                out.append("\303\247", 2);
                break;
            case 0xE8:
                out.append("\303\250", 2);
                break;
            case 0xE9:
                out.append("\303\251", 2);
                break;
            case 0xEA:
                out.append("\303\252", 2);
                break;
            case 0xEB:
                out.append("\303\253", 2);
                break;
            case 0xEC:
                out.append("\303\254", 2);
                break;
            case 0xED:
                out.append("\303\255", 2);
                break;
            case 0xEE:
                out.append("\303\256", 2);
                break;
            case 0xEF:
                out.append("\303\257", 2);
                break;
            case 0xF0:
                out.append("\303\260", 2);
                break;
            case 0xF1:
                out.append("\303\261", 2);
                break;
            case 0xF2:
                out.append("\303\262", 2);
                break;
            case 0xF3:
                out.append("\303\263", 2);
                break;
            case 0xF4:
                out.append("\303\264", 2);
                break;
            case 0xF5:
                out.append("\303\265", 2);
                break;
            case 0xF6:
                out.append("\303\266", 2);
                break;
            case 0xF7:
                out.append("\303\267", 2);
                break;
            case 0xF8:
                out.append("\303\270", 2);
                break;
            case 0xF9:
                out.append("\303\271", 2);
                break;
            case 0xFA:
                out.append("\303\272", 2);
                break;
            case 0xFB:
                out.append("\303\273", 2);
                break;
            case 0xFC:
                out.append("\303\274", 2);
                break;
            case 0xFD:
                out.append("\303\275", 2);
                break;
            case 0xFE:
                out.append("\303\276", 2);
                break;
            case 0xFF:
                out.append("\303\277", 2);
                break;
        }
    }
}
