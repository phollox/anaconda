#ifndef CHOWDREN_DEBUGDRAW_H
#define CHOWDREN_DEBUGDRAW_H

#include "chowstring.h"

// #define USE_DEBUGDRAW

#ifdef USE_DEBUGDRAW

namespace Debug
{
    void print(const chowstring & s);
    void draw();
};

#endif

#endif // CHOWDREN_DEBUGDRAW_H
