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

#ifndef CROSSRAND_H
#define CROSSRAND_H

#define CROSS_RAND_MAX 0x7FFF

// portable rand functions
extern unsigned int cross_seed;

inline void cross_srand(unsigned int value)
{
    cross_seed = value;
}

inline unsigned int cross_rand()
{
    cross_seed = cross_seed * 214013 + 2531011;
    return (cross_seed >> 16) & CROSS_RAND_MAX;
}

#endif // CROSSRAND_H
