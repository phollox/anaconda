#ifndef UTFCONV_H
#define UTFCONV_H

#include <string>

void convert_utf8_to_utf16(const std::string & value, std::string & out);
void convert_utf16_to_utf8(const std::string & value, std::string & out);
void convert_windows1252_to_utf8(const std::string & value, std::string & out);

#endif // UTFCONV_H
