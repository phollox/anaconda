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

#ifndef CHOWDREN_ASSETFILE_H
#define CHOWDREN_ASSETFILE_H

#include "fileio.h"
#include "assets.h"
#include <string>

#define OFFSET_SIZE(x) ((x)>1?(x):1)
#define IMAGE_ARRAY_SIZE OFFSET_SIZE(IMAGE_COUNT)
#define FONT_ARRAY_SIZE OFFSET_SIZE(FONT_COUNT)
#define SOUND_ARRAY_SIZE OFFSET_SIZE(SOUND_COUNT)
#define SHADER_ARRAY_SIZE OFFSET_SIZE(SHADER_COUNT)
#define FILE_ARRAY_SIZE OFFSET_SIZE(FILE_COUNT)
#define INVALID_ASSET_ID ((unsigned int)(-1))

class AssetFile : public FSFile
{
public:
    enum AssetType
    {
        IMAGE_DATA = 0,
        SOUND_DATA,
        FONT_DATA,
        SHADER_DATA,
        FILE_DATA,
        ASSET_TYPE_MAX
    };

    AssetFile();
    void open();
    using FSFile::open;
    void set_item(int index, AssetType type);
    static unsigned int get_offset(int index, AssetType type);
    static unsigned int get_size(AssetType type);
};

// for temporary files
class TempPath
{
public:
    int id;

    TempPath(int id)
    : id(id)
    {
    }

    void read(std::string & data);
};

TempPath create_temp_file(const std::string & path);

#endif // CHOWDREN_ASSETFILE_H
