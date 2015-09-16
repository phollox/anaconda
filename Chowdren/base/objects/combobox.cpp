#include "objects/combobox.h"
#include "chowconfig.h"

// ComboBox

ComboBox::ComboBox(int x, int y, int type_id)
: FrameObject(x, y, type_id), combo_box(manager.frame->gwen.canvas)
{
    collision = new InstanceBox(this);
}

ComboBox::~ComboBox()
{
    delete collision;
}

void ComboBox::update()
{
    combo_box.SetPos(x, y);
    combo_box.SetWidth(width);
}

void ComboBox::draw()
{
    frame->gwen.render(&combo_box);
    if (combo_box.m_Menu != NULL && combo_box.IsMenuOpen())
        frame->gwen.render(combo_box.m_Menu);
}

const std::string & ComboBox::get_line(int index)
{
    std::cout << "get_line not implemented" << std::endl;
    return empty_string;
}

void ComboBox::set_current_line(int index)
{
    std::cout << "set_current_line not implemented" << std::endl;
}

void ComboBox::add_line(const std::string value)
{
    std::cout << "add_line not implemented" << std::endl;
}

int ComboBox::get_current_line_number()
{
    std::cout << "get_current_line_number not implemented" << std::endl;
    return 0;
}

const std::string & ComboBox::get_current_line()
{
    std::cout << "get_current_line not implemented" << std::endl;
    return empty_string;
}

void ComboBox::highlight()
{
    std::cout << "ComboBox::highlight not implemented" << std::endl;
}

void ComboBox::dehighlight()
{
    std::cout << "ComboBox::dehighlight not implemented" << std::endl;
}

void ComboBox::lose_focus()
{
    std::cout << "ComboBox::lose_focus not implemented" << std::endl;
}

void ComboBox::reset()
{
    std::cout << "ComboBox::reset not implemented" << std::endl;
}

bool ComboBox::is_list_dropped()
{
    std::cout << "ComboBox::is_list_dropped not implemented" << std::endl;
    return false;
}

int ComboBox::find_string_exact(const std::string & text, int flag)
{
    std::cout << "find_string_exact not implemented" << text << " " << flag
        << std::endl;
    return 0;
}
