#include "chowstring.h"

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