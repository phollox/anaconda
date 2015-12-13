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

#ifndef CHOWDREN_INI_H
#define CHOWDREN_INI_H

#include "frameobject.h"
#include "types.h"
#include "assetfile.h"

#ifdef CHOWDREN_INI_KEEP_ORDER
#include <boost/container/flat_map.hpp>
#define ini_map boost::container::flat_map
#else
#define ini_map hash_map
#endif

typedef ini_map<chowstring, chowstring> OptionMap;
typedef ini_map<chowstring, OptionMap> SectionMap;

class INI : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(INI)

    static hash_map<chowstring, SectionMap> global_data;
    chowstring current_group;
    chowstring current_item;
    bool is_global;
    SectionMap * data;
    vector<std::pair<chowstring, chowstring> > search_results;
    bool overwrite;
    bool read_only;
    bool auto_save;
    bool use_compression;
    chowstring filename;
    chowstring encrypt_key;
    unsigned int search_time;
    bool changed;

    INI(int x, int y, int type_id);
    static void reset_global_data();
#ifdef CHOWDREN_CACHE_INI
    static void reset_cache();
#endif
    static int _parse_handler(void* user, const char* section,
                              const char* name, const char* value);
    void parse_handler(const chowstring & section, const chowstring & name,
                       const chowstring & value);
    void set_group(const chowstring & name);
    void set_group(const chowstring & name, bool new_group);
    void set_item(const chowstring & name);
    const chowstring & get_string(const chowstring & group,
                                   const chowstring & item);
    const chowstring & get_string(const chowstring & item);
    const chowstring & get_string_default(const chowstring & group,
                                           const chowstring & item,
                                           const chowstring & def);
    const chowstring & get_string_default(const chowstring & item,
                                           const chowstring & def);
    const chowstring & get_string_index(const chowstring & group,
                                         unsigned int index);
    const chowstring & get_string_index(unsigned int index);
    const chowstring & get_item_name(const chowstring & group,
                                      unsigned int index);
    const chowstring & get_item_name(unsigned int index);
    const chowstring & get_group_name(unsigned int index);

    // value double
    double get_value(const chowstring & group, const chowstring & item,
                     double def=0.0);
    double get_value(const chowstring & item, double def=0.0);
    double get_value_index(const chowstring & group, unsigned int index);
    double get_value_index(unsigned int index);
    void set_value(const chowstring & group, const chowstring & item,
                   double value);
    void set_value(const chowstring & item, double value);
    int get_value_int(const chowstring & group, const chowstring & item,
                      int def=0);
    int get_value_int(const chowstring & item, int def=0);
    void set_value_int(const chowstring & group, const chowstring & item,
                       int value);
    void set_value_int(const chowstring & item, int value);
    void set_string(const chowstring & group, const chowstring & item,
                    const chowstring & value);
    void set_string(const chowstring & item, const chowstring & value);
    void load_file(const chowstring & fn, bool read_only = false,
                   bool merge = false, bool overwrite = false);
    void load_string(const chowstring & data, bool merge);
    void load_file(TempPath path);
    void merge_file(const chowstring & fn, bool overwrite);
    void get_data(std::stringstream & out);
    void save_file(const chowstring & fn, bool force = true);
    void set_encryption_key(const chowstring & key);
    void set_compression(bool value);
    chowstring as_string();
    void save_file(bool force = true);
    void save_auto();
    void close();
    int get_item_count(const chowstring & section);
    int get_item_count();
    int get_group_count();
    bool has_group(const chowstring & group);
    bool has_item(const chowstring & group, const chowstring & option);
    bool has_item(const chowstring & option);
    void search(const chowstring & group, const chowstring & item,
                const chowstring & value);
    void delete_pattern(const chowstring & group, const chowstring & item,
                        const chowstring & value);
    void sort_group_by_name(const chowstring & group);
    void sort_group_by_value(const chowstring & group);
    void reset(bool save = true);
    void delete_group(const chowstring & group);
    void delete_group();
    void delete_item(const chowstring & group, const chowstring & item);
    void delete_item(const chowstring & item);
    void set_global_data(const chowstring & key);
    void merge_object(INI * other, bool overwrite);
    void merge_group(INI * other, const chowstring & src_group,
                     const chowstring & dst_group, bool overwrite);
    void merge_map(const SectionMap & data2, bool overwrite);
    void merge_map(SectionMap & data2, const chowstring & src_group,
                   const chowstring & dst_group, bool overwrite);
    bool has_search_results();
    size_t get_search_count();
    const chowstring & get_search_result_group(int index);
    const chowstring & get_search_result_item(int index);
    chowstring get_item_part(const chowstring & group,
                              const chowstring & item, int index,
                              const chowstring & def);
    void set_auto(bool save, bool load);
    ~INI();
};

#endif // CHOWDREN_INI_H
