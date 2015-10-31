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

#include "objects/blowfishext.h"
#include "objects/blowfish.h"
#include "fileio.h"
#include "stringcommon.h"
#include "image.h"
#include <iostream>
#include "types.h"
#include "platform.h"

static Blowfish cipher;
static std::string last_cipher;
static std::string gif_ext(".gif");
static hash_map<std::string, std::string> cipher_store;

void BlowfishObject::encrypt_file(const std::string & key,
                                  const std::string & in_file)
{
    std::string filename = convert_path(in_file);
    hash_map<std::string, std::string>::iterator it;
    it = cipher_store.find(filename);
    if (it != cipher_store.end())
        return;

    std::string data;
    if (!read_file(filename.c_str(), data)) {
        cipher_store[filename] = empty_string;
        std::cout << "Could not read file: " << filename << std::endl;
        return;
    }

    cipher_store[filename] = data;

    if (last_cipher != key) {
        last_cipher = key;
        cipher.set_key(key);
    }

    std::string out;
    cipher.encrypt(&out, data);

    FSFile fp(filename.c_str(), "w");
    if (!fp.is_open())
        return;
    fp.write(&out[0], out.size());
    fp.close();
}

void BlowfishObject::decrypt_file(const std::string & key,
                                  const std::string & in_file)
{
    std::string filename = convert_path(in_file);

    std::cout << "Decrypt file: " << key << " " << filename << std::endl;

    std::string & cache = cipher_store[filename];
    if (!cache.empty()) {
        std::cout << "Already decrypted" << std::endl;
        return;
    }

    std::string data;
    if (!read_file(filename.c_str(), data)) {
        std::cout << "Could not read file: " << filename << std::endl;
        cipher_store[filename] = empty_string;
        return;
    }

    if (last_cipher != key) {
        last_cipher = key;
        cipher.set_key(key);
    }

    std::string out;
    cipher.decrypt(&out, data);

    if (ends_with(filename, gif_ext) && !has_image_cache(filename)) {
        FileImage * image = new FileImage(filename, 0, 0, 0, 0,
                                          TransparentColor(255, 255, 255));
        image->load_data((unsigned char*)&out[0], out.size());
        set_image_cache(filename, image);
#ifndef NDEBUG
        std::string save_filename = filename + ".decrypted.gif";
        FSFile fp(save_filename.c_str(), "w");
        fp.write(&out[0], out.size());
        fp.close();
#endif
    } else {
        cache = out;
    }
}

const std::string & BlowfishObject::get_cache(const std::string & filename)
{
    if (!platform_is_file(filename))
        return empty_string;
    hash_map<std::string, std::string>::iterator it;
    it = cipher_store.find(filename);
    if (it == cipher_store.end())
        return empty_string;
    return it->second;
}

bool BlowfishObject::set_cache(const std::string & filename,
                               const std::string & data)
{
    hash_map<std::string, std::string>::iterator it;
    it = cipher_store.find(filename);
    if (it == cipher_store.end())
        return false;
    it->second = data;

    std::string out;
    cipher.encrypt(&out, data);

    FSFile fp(filename.c_str(), "w");
    if (!fp.is_open()) {
        std::cout << "Could not save Blowfish file: " << filename << std::endl;
        return true;
    }
    fp.write(&out[0], out.size());
    fp.close();
    return true;
}