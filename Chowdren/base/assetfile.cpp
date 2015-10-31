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

#include "assetfile.h"
#include "chowconfig.h"
#include "datastream.h"
#include "packfiles.cpp"
#include "path.h"
#include <iostream>

static bool assets_initialized = false;

static unsigned int image_offsets[IMAGE_ARRAY_SIZE];
static unsigned int sound_offsets[SOUND_ARRAY_SIZE];
static unsigned int font_offsets[FONT_ARRAY_SIZE];
static unsigned int shader_offsets[SHADER_ARRAY_SIZE];
static unsigned int file_offsets[FILE_ARRAY_SIZE];
static unsigned int type_sizes[AssetFile::ASSET_TYPE_MAX];

static unsigned int * asset_offsets[] = {
    image_offsets,
    sound_offsets,
    font_offsets,
    shader_offsets,
    file_offsets
};

void read_offsets(FileStream & stream, int count, unsigned int * array)
{
    for (int i = 0; i < count; i++) {
        array[i] = stream.read_uint32();
    }
}

void init_assets(FSFile & fp)
{
    assets_initialized = true;

    FileStream stream(fp);

    // skip image preload
    fp.seek(IMAGE_COUNT * 2, SEEK_CUR);

    read_offsets(stream, IMAGE_COUNT, image_offsets);
    read_offsets(stream, SOUND_COUNT, sound_offsets);
    read_offsets(stream, FONT_COUNT, font_offsets);
    read_offsets(stream, SHADER_COUNT, shader_offsets);
    read_offsets(stream, FILE_COUNT, file_offsets);

    // read sizes
    read_offsets(stream, AssetFile::ASSET_TYPE_MAX, type_sizes);
}

// AssetFile

AssetFile::AssetFile()
: FSFile()
{
}

void AssetFile::open()
{
    FSFile::open("./Assets.dat", "r");
}

void AssetFile::set_item(int index, AssetType type)
{
    if (!assets_initialized) {
        init_assets(*this);
    }

    seek(asset_offsets[type][index]);
}

unsigned int AssetFile::get_offset(int index, AssetType type)
{
    if (!assets_initialized) {
        AssetFile fp;
        fp.open();
        init_assets(fp);
    }

    return asset_offsets[type][index];
}

unsigned int AssetFile::get_size(AssetType type)
{
    if (!assets_initialized) {
        AssetFile fp;
        fp.open();
        init_assets(fp);
    }

    return type_sizes[type];
}

// temp files

TempPath create_temp_file(const std::string & path)
{
    return TempPath(get_file_id(get_path_filename(path)));
}

void TempPath::read(std::string & data)
{
    AssetFile fp;
    fp.open();
    fp.set_item(id, AssetFile::FILE_DATA);
    FileStream stream(fp);
    unsigned int size = stream.read_uint32();
    stream.read_string(data, size);
}
