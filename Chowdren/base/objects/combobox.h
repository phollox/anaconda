#ifndef CHOWDREN_COMBOBOX_H
#define CHOWDREN_COMBOBOX_H

#include "frameobject.h"
#include "common.h"

#include "Gwen/Gwen.h"
#include "Gwen/Controls/ComboBox.h"
#include "gui/gwen.h"


class ComboBox : public FrameObject
{
public:
    FRAMEOBJECT_HEAD(ComboBox)

    ComboBox(int x, int y, int type_id);
    ~ComboBox();

    Gwen::Controls::ComboBox * combo_box;
    int old_index;
    int selection_changed;
    int index_offset;

    void update();
    void draw();
    int get_current_line_number();
    std::string get_current_line();
    std::string get_line(int index);
    void set_current_line(int index);
    void add_line(const std::string line);
    void highlight();
    void dehighlight();
    void lose_focus();
    void reset();
    bool is_list_dropped();
    bool is_selection_changed();
    int find_string_exact(const std::string & text, int flag);
    void init_control();
};

#endif // CHOWDREN_COMBOBOX_H
