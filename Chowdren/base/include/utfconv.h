#ifndef UTFCONV_H
#define UTFCONV_H

#include "chowstring.h"

void convert_utf8_to_utf16(const chowstring & value, chowstring & out);
void convert_utf16_to_utf8(const chowstring & value, chowstring & out);
void convert_windows1252_to_utf8(const chowstring & value, chowstring & out);

#endif // UTFCONV_H
