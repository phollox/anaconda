#include "objects/calcrect.h"
#include <iostream>

void CalcRect::set_text(const std::string & text)
{
    std::cout << "CalcRect: Set text: " << text << std::endl;
}

void CalcRect::set_font(const std::string & font, int size, int style)
{
    std::cout << "CalcRect: Set font: " << font << " " << size << " " << style
        << std::endl;
}

void CalcRect::set_max_width(int width)
{
    std::cout << "CalcRect: Set max width: " << width << std::endl;
}

int CalcRect::get_width()
{
    return 0;
}

int CalcRect::get_height()
{
    return 0;
}
