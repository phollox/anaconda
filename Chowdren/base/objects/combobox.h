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

    Gwen::Controls::ComboBox combo_box;

    void update();
    void draw();
};

#endif // CHOWDREN_COMBOBOX_H
