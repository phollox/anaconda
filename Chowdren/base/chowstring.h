#ifndef CHOWDREN_CHOWSTRING_H
#define CHOWDREN_CHOWSTRING_H

#include <string>
#include <string.h>

// #define USE_CHOWSTRING

#ifdef USE_CHOWSTRING

class chowstring
{
public:
    enum Flags
    {
        INTERN = 1 << 0,
        SUBSTR = 1 << 1
    };

    enum {
        npos = (unsigned int)(-1)
    };

    char * buffer;
    unsigned int data_size;
    unsigned int data_capacity;
    unsigned int flags;

    typedef unsigned int size_type;
    typedef char * iterator;
    typedef char * const_iterator;

    chowstring(const char * data, unsigned int size)
    : buffer((char*)data), data_size(size), data_capacity(size), flags(INTERN)
    {
    }

    template <unsigned int N>
    chowstring(const char (&s)[N])
    : data_size(N), data_capacity(N), flags(0)
    {
        buffer = (char*)malloc(N+1);
        memcpy(buffer, s, N+1);
    }

    chowstring(const char * s)
    : flags(0)
    {
        data_size = data_capacity = strlen(s);
        buffer = (char*)malloc(data_size+1);
        memcpy(buffer, s, data_size+1);
    }

    chowstring(const chowstring & s)
    {
        data_size = data_capacity = s.data_size;
        if (s.flags & INTERN) {
            buffer = s.buffer;
            flags = s.flags;
            return;
        }

        flags = 0;
        buffer = (char*)malloc(data_size+1);
        memcpy(buffer, s.buffer, data_size+1);
    }

    chowstring(const chowstring & s, unsigned int pos)
    {
        data_size = data_capacity = s.data_size - pos;

        if (s.flags & INTERN) {
            buffer = s.buffer + pos;
            flags = s.flags;
            return;
        }

        flags = 0;
        buffer = (char*)malloc(data_size+1);
        memcpy(buffer, s.buffer + pos, data_size+1);
    }

    chowstring(const chowstring & s, unsigned int pos, unsigned int len)
    {
        data_size = data_capacity = len;

        if (s.flags & INTERN) {
            buffer = s.buffer + pos;
            flags = s.flags | SUBSTR;
            return;
        }

        flags = 0;
        buffer = (char*)malloc(data_size+1);
        memcpy(buffer, s.buffer + pos, data_size+1);
    }

    chowstring()
    : buffer("\0"), data_size(0), data_capacity(0), flags(INTERN)
    {
    }

    chowstring(const std::string & in)
    : flags(0)
    {
        data_size = in.size();
        buffer = (char*)malloc(data_size+1);
        memcpy(buffer, in.data(), data_size);
        buffer[data_size] = '\0';
    }

    operator std::string() const
    {
        return std::string(buffer, data_size);
    }

    char & operator[](unsigned int index) const
    {
        return buffer[index];
    }

    void operator +=(char c)
    {
        resize(data_size + 1);
        buffer[data_size - 1] = c;
    }

    template <unsigned int N>
    void operator +=(const char (&s)[N])
    {
        unsigned int old_size = data_size;
        resize(data_size + N);
        memcpy(buffer + old_size, s, N);
    }

    void operator +=(const char * s)
    {
        unsigned int N = strlen(s);
        unsigned int old_size = data_size;
        resize(data_size + N);
        memcpy(buffer + old_size, s, N);
    }

    void operator +=(const chowstring & s)
    {
        unsigned int old_size = data_size;
        resize(data_size + s.data_size);
        memcpy(buffer + old_size, s.buffer, s.data_size);
    }

    void operator =(const char * s)
    {
        unsigned int N = strlen(s);
        resize(N);
        memcpy(buffer, s, N);
    }

    void operator =(const std::string & s)
    {
        resize(s.size());
        memcpy(buffer, s.data(), s.size());
    }

    chowstring operator +(const chowstring & s) const
    {
        chowstring new_s;
        new_s.data_size = new_s.data_capacity = data_size + s.data_size;
        new_s.buffer = (char*)malloc(new_s.data_size+1);
        memcpy(new_s.buffer, buffer, data_size);
        memcpy(new_s.buffer + data_size, s.buffer, s.data_size + 1);
        return new_s;
    }

    template <unsigned int N>
    chowstring operator +(const char (&s)[N]) const
    {
        chowstring new_s;
        new_s.data_size = new_s.data_capacity = data_size + N;
        new_s.buffer = (char*)malloc(new_s.data_size+1);
        memcpy(new_s.buffer, buffer, data_size);
        memcpy(new_s.buffer + data_size, s, N + 1);
        return new_s;
    }

    bool operator ==(const chowstring & s) const
    {
        if (data_size != s.data_size)
            return false;
        if ((flags & s.flags) & INTERN)
            return buffer == s.buffer;
        return memcmp(buffer, s.buffer, data_size) == 0;
    }

    template <unsigned int N>
    bool operator ==(const char (&s)[N]) const
    {
        if (data_size != N)
            return false;
        return memcmp(buffer, s, N) == 0;
    }

    void reserve(unsigned int size)
    {
        if (data_capacity >= size)
            return;
        data_capacity = size;
        if (flags & INTERN) {
            char * new_data = (char*)malloc(size+1);
            memcpy(new_data, buffer, data_size);
            buffer = new_data;
            flags = 0;
        } else {
            buffer = (char*)realloc(buffer, size+1);
        }
        buffer[size] = '\0';
    }

    void resize(unsigned int size)
    {
        if (data_capacity >= size) {
            if (data_size != size) {
                data_size = size;
                flags |= SUBSTR;
            }
            return;
        }
        unsigned int old_size = data_size;
        data_size = data_capacity = size;
        if (flags & INTERN) {
            char * new_data = (char*)malloc(size+1);
            memcpy(new_data, buffer, old_size);
            buffer = new_data;
            flags = 0;
        } else {
            buffer = (char*)realloc(buffer, size+1);
        }
        buffer[size] = '\0';
    }

    void clear()
    {
        data_size = 0;
    }

    void push_back(char c)
    {
        (*this) += c;
    }

    void append(const char * s, unsigned int size)
    {
        // XXX slow
        resize(data_size + size);
        memcpy(buffer + data_size - size, s, size);
    }

    const char * data() const
    {
        return buffer;
    }

    char & get(unsigned int index) const
    {
        return buffer[index];
    }

    void _make_unique()
    {
        char * new_data = (char*)malloc(data_size+1);
        memcpy(new_data, buffer, data_size);
        new_data[data_size] = '\0';
        flags = 0;
    }

    void make_unique()
    {
        if (flags & INTERN)
            _make_unique();
    }

    char * c_str() const
    {
        if (flags & SUBSTR)
            ((chowstring*)this)->_make_unique();
        return buffer;
    }

    char * begin() const
    {
        return buffer;
    }

    char * end() const
    {
        return buffer + data_size;
    }

    unsigned int size() const
    {
        return data_size;
    }

    bool empty() const
    {
        return data_size == 0;
    }

    ~chowstring()
    {
        if (flags & INTERN)
            return;
        free(buffer);
    }

    chowstring substr(unsigned int pos) const
    {
        return chowstring(*this, pos);
    }

    chowstring substr(unsigned int pos, unsigned int len) const
    {
        return chowstring(*this, pos, len);
    }

    unsigned int find(char c) const
    {
        for (unsigned int i = 0; i < data_size; ++i) {
            if (buffer[i] == c)
                return i;
        }
        return npos;
    }

    unsigned int find_last_of(char c) const
    {
        for (int i = (int)(data_size - 1); i >= 0; --i) {
            if (buffer[i] == c)
                return i;
        }
        return npos;
    }

    unsigned int find_last_of(const char * c) const
    {
        for (int i = (int)(data_size - 1); i >= 0; --i) {
            const char * cc = c;
            while (*cc != '\0') {
                if (buffer[i] == *cc)
                    return i;
                cc++;
            }
        }
        return npos;
    }

    int compare(unsigned int pos, unsigned int len, const chowstring & s) const
    {
        if (len > data_size - pos)
            len = data_size - pos;
        if (len < s.size())
            return -1;
        char * data = buffer + pos;
        for (unsigned int i = 0; i < len; ++i) {
            if (buffer[i] != s.buffer[i])
                return -1;
        }
        return 0;
    }

    template <unsigned int N>
    int compare(unsigned int pos, unsigned int len, const char (&s)[N]) const
    {
        if (len > data_size - pos)
            len = data_size - pos;
        if (len < N)
            return -1;
        char * data = buffer + pos;
        for (unsigned int i = 0; i < len; ++i) {
            if (buffer[i] != s[i])
                return -1;
        }
        return 0;
    }

    template <unsigned int N>
    int compare(unsigned int pos, unsigned int len,
                const char (&s)[N], unsigned int s_len) const
    {
        // XXX does not compare s.size() and s_len. needed?
        if (len > data_size - pos)
            len = data_size - pos;
        if (len < N)
            return -1;
        if (len > s_len)
            len = s_len;
        char * data = buffer + pos;
        for (unsigned int i = 0; i < len; ++i) {
            if (buffer[i] != s[i])
                return -1;
        }
        return 0;
    }

    unsigned int find(const chowstring & str, unsigned int start=0) const;
    unsigned int rfind(const chowstring & str, unsigned int start=0) const;
    char * find_c(const chowstring & str, unsigned int start=0) const;
    char * findi_c(const chowstring & str, unsigned int start=0) const;
    void replace_i(const chowstring & from, const chowstring & to);
    void replace(const chowstring & from, const chowstring & to);
};

#include <boost/functional/hash.hpp>

inline std::size_t hash_value(chowstring const& b)
{
    return boost::hash_range(b.begin(), b.end());
}

#include <ostream>

std::ostream &operator<<(std::ostream &os, chowstring const &m);

template <unsigned int N>
chowstring operator+(const char (&s1)[N], const chowstring & s2)
{
    chowstring s;
    s.resize(N + s2.data_size);
    memcpy(s.buffer, s1, N);
    memcpy(s.buffer + N, s2.buffer, s2.data_size);
    return s;
}

#else

#define chowstring std::string

#endif

#endif // CHOWDREN_CHOWSTRING_H
