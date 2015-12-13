#include "chowstring.h"

template <bool ins>
void replace_chowstring(chowstring & s, const chowstring & from, const chowstring & to)
{
    s.make_unique();
    if (to_size == from_size) {
        char * pos = ins ? s.findi_c(from) : s.find_c(from);
        while (pos != NULL) {
            for (unsigned int i = 0; i < to_size; i++) {
                pos[i] = ReplacementText[i];
            }

            if (pos + from_size - **this < Len()) {
                pos = ins ? findi_c(pos + from_size, from) : find_c(pos + from_size, from);
            } else {
                break;
            }
        }
    } else {
        FString Copy(*this);
        Empty(Len());

        // get a pointer into the character data
        char* WritePosition = (char*)Copy.Data.GetData();
        // look for From in the remaining string
        char* SearchPosition = SearchCase == ESearchCase::IgnoreCase ?
                                              FCString::Stristr(WritePosition, from) :
                                              FCString::Strstr(WritePosition, from);
        while (SearchPosition != NULL) {
            // replace the first letter of the From with 0 so we can do a strcpy (FString +=)
            *SearchPosition = 0;

            // copy everything up to the SearchPosition
            (*this) += WritePosition;

            // copy over the ReplacementText
            (*this) += ReplacementText;

            // restore the letter, just so we don't have 0's in the string
            *SearchPosition = *from;

            WritePosition = SearchPosition + from_size;
            SearchPosition = SearchCase == ESearchCase::IgnoreCase ? FCString::Stristr(WritePosition, from) : FCString::Strstr(WritePosition, from);
        }

        // copy anything left over
        (*this) += WritePosition;
    }
}

template <typename F>
char * find_chowstring(F f, const chowstring & src, unsigned int start,
                       const chowstring & str)
{
    if (str.empty() || str.data_size > src.data_size || start >= src.data_size)
        return NULL;
    char * str_c = str.buffer;
    unsigned int len = str.data_size - 1;
    char c = f(*str_c);
    str_c++;

    char * pos = src.buffer + start;
    char * end = pos + src.data_size - len;
    while (true) {
        char cc = *pos;
        pos++;
        if (cc == c) {
            for (unsigned int i = 0; i < len; ++i) {
                if (f(pos[i]) != f(str_c[i]))
                    goto end;
            }
            return src.buffer;
        }
    end:
        if (pos >= end)
            break;
    }
    return NULL;
}

template <typename F>
char * rfind_chowstring(F f, const chowstring & src, unsigned int start,
                        const chowstring & str)
{
    if (str.empty() || str.data_size > src.data_size || start >= src.data_size)
        return NULL;
    char * str_c = str.buffer;
    unsigned int len = str.data_size - 1;
    char c = f(*str_c);
    str_c++;

    char * pos = src.buffer + src.data_size - str.data_size;
    char * end = src.buffer + start;
    while (true) {
        char cc = *pos;
        pos++;
        if (cc == c) {
            for (unsigned int i = 0; i < len; ++i) {
                if (f(pos[i]) != f(str_c[i]))
                    goto end;
            }
            return src.buffer;
        }
    end:
        if (pos <= end)
            break;
    }
    return NULL;
}

inline char get_dummy(char c)
{
    return c;
}

char * chowstring::findi_c(const chowstring & str, unsigned int start) const
{
    return find_chowstring(toupper, *this, start, str);
}

char * chowstring::find_c(const chowstring & str, unsigned int start) const
{
    return find_chowstring(get_dummy, *this, start, str);
}

unsigned int chowstring::rfind(const chowstring & str, unsigned int start) const
{
    char * res = rfind_chowstring(get_dummy, *this, start, str);
    if (res == NULL)
        return npos;
    return res - buffer;
}

unsigned int chowstring::find(const chowstring & str, unsigned int start) const
{
    char * res = find_chowstring(get_dummy, *this, start, str);
    if (res == NULL)
        return npos;
    return res - buffer;
}

std::ostream &operator<<(std::ostream &os, chowstring const &m)
{
    return os.write(m.buffer, m.data_size);
}