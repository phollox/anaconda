#include "objects/combobox.h"
#include "chowconfig.h"

class GwenCombo : public Gwen::Controls::ComboBox
{
public:
    GWEN_CONTROL_INLINE(GwenCombo, Gwen::Controls::ComboBox)
    {
    }
};

// ComboBox

ComboBox::ComboBox(int x, int y, int type_id)
: FrameObject(x, y, type_id), old_index(-5)
{
    collision = new InstanceBox(this);
}

void ComboBox::init_control()
{
    combo_box = new GwenCombo(manager.frame->gwen.frame_base);
}

ComboBox::~ComboBox()
{
    delete collision;
    combo_box->DelayedDelete();
}

void ComboBox::update()
{
    combo_box->SetHidden(!get_visible());
    combo_box->SetPos(x, y);
    combo_box->SetWidth(width);
    int new_index = get_current_line_number();
    if (old_index != new_index) {
        selection_changed = 2;
    }
    old_index = new_index;
    selection_changed = std::max(0, selection_changed - 1);
}

bool ComboBox::is_selection_changed()
{
    return selection_changed > 0;
}

void ComboBox::draw()
{
    combo_box->SetHidden(!get_visible());
    combo_box->SetPos(x, y);
    combo_box->SetWidth(width);
}

std::string ComboBox::get_line(int index)
{
    Gwen::Controls::MenuItem * item;
    item = (Gwen::Controls::MenuItem*)combo_box->GetItemByIndex(index +
                                                                index_offset);
    const Gwen::TextObject & text = item->GetText();
    return text.c_str();
}

void ComboBox::set_current_line(int index)
{
    combo_box->SelectItemByIndex(index + index_offset);
}

void ComboBox::add_line(const std::string value)
{
    combo_box->AddItem(Gwen::TextObject(value));
}

int ComboBox::get_current_line_number()
{
	Gwen::Controls::MenuItem * item;
    item = (Gwen::Controls::MenuItem*)combo_box->GetSelectedItem();
    Gwen::Controls::Menu * menu = combo_box->GetMenu();
    int index = menu->GetIndex(item) - index_offset;
	return index;
}

std::string ComboBox::get_current_line()
{
    Gwen::Controls::MenuItem * item;
    item = (Gwen::Controls::MenuItem*)combo_box->GetSelectedItem();
    const Gwen::TextObject & text = item->GetText();
    return text.c_str();
}

void ComboBox::highlight()
{
    combo_box->SetDisabled(false);
}

void ComboBox::dehighlight()
{
    combo_box->SetDisabled(true);
}

void ComboBox::lose_focus()
{
    std::cout << "ComboBox::lose_focus not implemented" << std::endl;
}

void ComboBox::reset()
{
    // this is stupid and slow
    // bool disabled = combo_box->IsDisabled();
    // combo_box->DelayedDelete();
    // init_control();
    // combo_box->SetDisabled(disabled);
    combo_box->ClearItems();
}

bool ComboBox::is_list_dropped()
{
    return combo_box->IsMenuOpen();
}

int ComboBox::find_string_exact(const std::string & text, int flag)
{
#ifndef NDEBUG
    if (flag != -1)
        std::cout << "Unsupported find_string_exact: " << flag << std::endl;
#endif
    return combo_box->GetItemIndex(text.c_str()) - index_offset;
}
