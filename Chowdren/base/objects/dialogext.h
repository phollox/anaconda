#ifndef CHOWDREN_DIALOGEXT_H
#define CHOWDREN_DIALOGEXT_H

#include <string>

class DialogObject
{
public:
    static void set_title(const std::string & title);
    static void set_text(const std::string & text);
    static void create(unsigned int id);
    static void set_ok();
    static void set_yes_no();
    static void set_modal();
    static void set_topmost();
    static void set_filter(const std::string & filter, int count);
    static void set_default_filename(const std::string & filename);
    static void set_default_extension(const std::string & filename);
    static void set_default_directory(const std::string & filename);
    static void open_load_selector(const std::string & start);
    static void open_save_selector(const std::string & start);
};

#endif // CHOWDREN_DIALOGEXT_H
