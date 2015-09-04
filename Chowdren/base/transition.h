#ifndef CHOWDREN_TRANSITION_H
#define CHOWDREN_TRANSITION_H

class Transition
{
public:
    enum Type
    {
        FADE = 0,
        DOOR = 1
    };

    // Type type;
    // Color color;

    static void draw(int fade_type, float fade_value, Color color);

    // Transition(Type type, const Color & color, float dir);
};

#endif // CHOWDREN_TRANSITION_H
