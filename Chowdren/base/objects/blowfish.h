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

#ifndef CHOWDREN_BLOWFISH_H
#define CHOWDREN_BLOWFISH_H

#include <stdint.h>
#include <stddef.h>
#include <string>

class Blowfish {
public:
    void set_key(const std::string& key);
    void set_key(const char* key, size_t byte_length);

    // Buffer will be padded with PKCS #5 automatically
    // "dst" and "src" must be different instance
    void encrypt(std::string* dst, const std::string& src) const;
    void decrypt(std::string* dst, const std::string& src) const;

    // Buffer length must be a multiple of the block length (64bit)
    void encrypt(char* dst, const char* src, size_t byte_length) const;
    void decrypt(char* dst, const char* src, size_t byte_length) const;

private:
    void encrypt_block(uint32_t *left, uint32_t *right) const;
    void decrypt_block(uint32_t *left, uint32_t *right) const;
    uint32_t feistel(uint32_t value) const;

private:
    uint32_t pary_[18];
    uint32_t sbox_[4][256];
};

#endif // CHOWDREN_BLOWFISH_H
