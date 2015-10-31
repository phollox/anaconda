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

#ifndef CHOWDREN_FILEOP_H
#define CHOWDREN_FILEOP_H

class File
{
public:
    static const std::string & get_appdata_directory();
    static void change_directory(const std::string & path);
    static void create_directory(const std::string & path);
    static bool file_exists(const std::string & path);
    static bool name_exists(const std::string & path);
    static bool directory_exists(const std::string & path);
    static void delete_file(const std::string & path);
    static void delete_folder(const std::string & path);
    static bool file_readable(const std::string & path);
    static int get_size(const std::string & path);
    static bool copy_file(const std::string & src, const std::string & dst);
    static void rename_file(const std::string & src, const std::string & dst);
    static void append_text(const std::string & text,
                            const std::string & path);
    static std::string get_ext(const std::string & path);
    static std::string get_title(const std::string & path);
};

#endif // CHOWDREN_FILEOP_H
