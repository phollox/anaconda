#ifndef ALTERABLES_H
#define ALTERABLES_H

// for size_t
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include "dynnum.h"
#include "pool.h"
#include "stringcommon.h"

#define ALT_VALUES 26
#define ALT_STRINGS 10

#ifdef CHOWDREN_USE_DYNAMIC_NUMBER
void save_alterable_debug();
#endif

class AlterableValues
{
public:
#if defined(CHOWDREN_FORCE_ALT_DOUBLE)
    double values[ALT_VALUES];

    AlterableValues()
    {
        for (int i = 0; i < ALT_VALUES; i++) {
            values[i] = 0;
        }
    }

    double get(size_t index)
    {
        if (index >= ALT_VALUES)
            return 0;
        return values[index];
    }

    int get_int(size_t index)
    {
        return int(get(index));
    }

    void set(size_t index, double value)
    {
        if (index >= ALT_VALUES)
            return;
        values[index] = value;
    }

    void add(size_t index, double value)
    {
        set(index, get(index) + value);
    }

    void sub(size_t index, double value)
    {
        set(index, get(index) - value);
    }

    void set_int(size_t index, double value)
    {
        set(index, value);
    }

    void add_int(size_t index, double value)
    {
        add(index, value);
    }

    void sub_int(size_t index, double value)
    {
        sub(index, value);
    }

    void set_fp(size_t index)
    {
    }

    double get_dynamic(size_t index)
    {
        return get(index);
    }

    void set(const AlterableValues & v)
    {
        memcpy(values, v.values, ALT_VALUES*sizeof(double));
    }

#elif defined(CHOWDREN_USE_DYNAMIC_NUMBER)
    struct AlterableDebug
    {
        unsigned char is_fp[ALT_VALUES];

		AlterableDebug()
        {
            for (int i = 0; i < ALT_VALUES; ++i)
                is_fp[i] = 0;
        }
    };

    AlterableDebug * debug;
    DynamicNumber values[ALT_VALUES];

    AlterableValues()
    {
        for (int i = 0; i < ALT_VALUES; i++) {
            values[i] = 0;
        }
    }

    DynamicNumber get(size_t index)
    {
        if (index >= ALT_VALUES)
            return 0;
        return values[index];
    }

    DynamicNumber get_int(size_t index)
    {
        return get(index);
    }

    void set(size_t index, DynamicNumber value)
    {
        if (index >= ALT_VALUES)
            return;
        values[index] = value;
#ifndef CHOWDREN_USE_DYNAMIC_NUMBER
        unsigned char * v = &debug->is_fp[index];
        if (value.is_fp)
            *v = 2;
        else if (*v == 0)
            *v = 1;
#endif
    }

    void add(size_t index, DynamicNumber value)
    {
        set(index, get(index) + value);
    }

    void sub(size_t index, DynamicNumber value)
    {
        set(index, get(index) - value);
    }

    void set_int(size_t index, DynamicNumber value)
    {
        set(index, value);
    }

    void add_int(size_t index, DynamicNumber value)
    {
        add(index, value);
    }

    void sub_int(size_t index, DynamicNumber value)
    {
        sub(index, value);
    }

    double get_dynamic(size_t index)
    {
        return get(index);
    }

    void set_fp(size_t index)
    {
    }

    void set(const AlterableValues & v)
    {
        memcpy(values, v.values, ALT_VALUES*sizeof(DynamicNumber));
    }
#else
    union
    {
        double f64[ALT_VALUES];
        int i32[ALT_VALUES][2];
    };

    AlterableValues()
    {
        for (int i = 0; i < ALT_VALUES; i++) {
            i32[i][0] = 0;
            i32[i][1] = -1;
        }
    }

    double get(size_t index)
    {
        if (index >= ALT_VALUES)
            return 0;
        return f64[index];
    }

    void set(size_t index, double value)
    {
        if (index >= ALT_VALUES)
            return;
        values[index] = value;
    }

    void add(size_t index, double value)
    {
        if (index >= ALT_VALUES)
            return;
        f64[index] += value;
    }

    void sub(size_t index, double value)
    {
        if (index >= ALT_VALUES)
            return;
        f64[index] -= value;
    }

    int get_int(size_t index)
    {
        if (index >= ALT_VALUES)
            return 0;
        return i32[index][0];
    }

    void set_int(size_t index, int value)
    {
        if (index >= ALT_VALUES)
            return;
        i32[index][0] = value;
    }

    void add_int(size_t index, int value)
    {
        if (index >= ALT_VALUES)
            return;
        i32[index][0] += value;
    }

    void sub_int(size_t index, int value)
    {
        if (index >= ALT_VALUES)
            return;
        i32[index][0] -= value;
    }

    void set_fp(size_t index)
    {
        i32[index][1] = 0;
    }

    double get_dynamic(size_t index)
    {
        if (i32[index][1] == -1)
            return f64[index];
        return i32[index][0];
    }

    void set(const AlterableValues & v)
    {
        memcpy(f64, v.f64, ALT_VALUES*sizeof(double));
    }
#endif

};

class AlterableStrings
{
public:
    std::string values[ALT_STRINGS];

    AlterableStrings()
    {
    }

    const std::string & get(size_t index)
    {
        if (index >= ALT_STRINGS)
            return empty_string;
        return values[index];
    }

    void set(size_t index, const std::string & value)
    {
        if (index >= ALT_STRINGS)
            return;
        values[index] = value;
    }

    void set(const AlterableStrings & v)
    {
        for (int i = 0; i < ALT_STRINGS; i++)
            values[i] = v.values[i];
    }
};

class AlterableFlags
{
public:
    unsigned int flags;

    AlterableFlags()
    : flags(0)
    {
    }

    void enable(int index)
    {
        index &= 31;
        flags |= 1 << index;
    }

    void disable(int index)
    {
        index &= 31;
        flags &= ~(1 << index);
    }

    void toggle(int index)
    {
        index &= 31;
        flags ^= 1 << index;
    }

    bool is_on(int index)
    {
        index &= 31;
        return (flags & (1 << index)) != 0;
    }

    bool is_off(int index)
    {
        index &= 31;
        return (flags & (1 << index)) == 0;
    }

    int get(int index)
    {
        index &= 31;
        return int(is_on(index));
    }

    void set(const AlterableFlags & other)
    {
        flags = other.flags;
    }
};

class Alterables
{
public:
    AlterableStrings strings;
    AlterableValues values;
    AlterableFlags flags;

    void set(const Alterables & other)
    {
        strings.set(other.strings);
        values.set(other.values);
        flags.set(other.flags);
    }

    static Alterables * create();
    static void destroy(Alterables * ptr);
};

struct SavedAlterables
{
    bool init;
    Alterables value;

    SavedAlterables()
    : init(false)
    {
    }
};

extern ObjectPool<Alterables> alterable_pool;

inline Alterables * Alterables::create()
{
    return new (alterable_pool.create()) Alterables();
}

inline void Alterables::destroy(Alterables * ptr)
{
    alterable_pool.destroy(ptr);
}

#endif // ALTERABLES_H
