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

#ifndef CHOWDREN_TYPES_H
#define CHOWDREN_TYPES_H

#ifdef _MSC_VER
#include <stddef.h>
typedef signed __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

// because this isn't included automatically with ARMCC
#include <string.h>

#if defined(CHOWDREN_IS_PS4) || defined(CHOWDREN_IS_VITA)
#include <unordered_map>
#define hash_map std::unordered_map
#else
#include <boost/unordered_map.hpp>
#define hash_map boost::unordered_map
#endif

#include <boost/container/vector.hpp>
using boost::container::vector;

#endif // CHOWDREN_TYPES_H
