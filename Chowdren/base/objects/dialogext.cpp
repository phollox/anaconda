#include "dialogext.h"
#include "stringcommon.h"
#include <iostream>

static std::string title;
static std::string text;

static std::string path;

void DialogObject::set_title(const std::string & value)
{
    title = value;
}

void DialogObject::set_text(const std::string & value)
{
    text = value;
}

void DialogObject::create(unsigned int id)
{
    std::cout << "DialogObject::create not implemented" << std::endl;
}

void DialogObject::set_ok()
{
    std::cout << "DialogObject::set_ok not implemented" << std::endl;
}

void DialogObject::set_yes_no()
{
    std::cout << "DialogObject::set_yes_no not implemented" << std::endl;
}

void DialogObject::set_modal()
{
    std::cout << "DialogObject::set_modal not implemented" << std::endl;
}

void DialogObject::set_topmost()
{
    std::cout << "DialogObject::set_topmost not implemented" << std::endl;
}

void DialogObject::set_filter(const std::string & filter, int count)
{
    std::cout << "DialogObject::set_filter not implemented: "
        << filter << " " << count << std::endl;
}

void DialogObject::set_default_filename(const std::string & filename)
{
    std::cout << "DialogObject::set_default_filename not implemented"
        << filename << std::endl;
    path = filename;
}

void DialogObject::set_default_extension(const std::string & ext)
{
    std::cout << "DialogObject::set_default_extension not implemented"
        << ext << std::endl;
}

void DialogObject::set_default_directory(const std::string & dir)
{
    std::cout << "DialogObject::set_default_directory not implemented"
        << dir << std::endl;
}

void DialogObject::open_load_selector(const std::string & dir)
{
    std::cout << "DialogObject::open_load_selector not implemented"
        << dir << std::endl;
}

void DialogObject::open_save_selector(const std::string & dir)
{
    std::cout << "DialogObject::open_save_selector not implemented"
        << dir << std::endl;
}

const std::string & DialogObject::get_path()
{
    return path;
}
