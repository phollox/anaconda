#ifndef CHOWDREN_BLOWFISHEXT_H
#define CHOWDREN_BLOWFISHEXT_H

#include <string>

class BlowfishObject
{
public:
    static void encrypt_file(const std::string & key,
                             const std::string & filename);
    static void decrypt_file(const std::string & key,
                             const std::string & filename);
    static const std::string & get_cache(const std::string & filename);
};

#endif // CHOWDREN_BLOWFISHEXT_H
