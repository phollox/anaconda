#include "objects/combobox.h"
#include "chowconfig.h"

// ComboBox

ComboBox::ComboBox(int x, int y, int type_id)
: FrameObject(x, y, type_id), combo_box(gwen.canvas)
{
    collision = new InstanceBox(this);
    combo_box.AddItem(L"Hello");
    combo_box.AddItem(L"World");
    combo_box.AddItem(L"Test");
}

ComboBox::~ComboBox()
{
    delete collision;
}

void ComboBox::update()
{
    combo_box.SetWidth(width);
}

void ComboBox::draw()
{
    gwen.render(&combo_box);
    if (combo_box.m_Menu != NULL && combo_box.IsMenuOpen())
        gwen.render(combo_box.m_Menu);
}