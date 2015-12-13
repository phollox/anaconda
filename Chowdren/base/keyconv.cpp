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

#include "chowstring.h"
#include "keydef.h"
#include "stringcommon.h"

int translate_vk_to_key(int vk)
{
    switch (vk) {
        case 1: return SDL_BUTTON_LEFT;
        case 2: return SDL_BUTTON_RIGHT;
        case 4: return SDL_BUTTON_MIDDLE;
        case 5: return SDL_BUTTON_X1;
        case 6: return SDL_BUTTON_X2;
        case 8: return SDLK_BACKSPACE;
        case 9: return SDLK_TAB;
        case 12: return SDLK_CLEAR;
        case 13: return SDLK_RETURN;
        case 16: return SDLK_LSHIFT;
        case 17: return SDLK_LCTRL;
        case 18: return SDLK_RALT;
        case 19: return SDLK_PAUSE;
        case 20: return SDLK_CAPSLOCK;
        case 27: return SDLK_ESCAPE;
        case 32: return SDLK_SPACE;
        case 33: return SDLK_PAGEUP;
        case 34: return SDLK_PAGEDOWN;
        case 35: return SDLK_END;
        case 36: return SDLK_HOME;
        case 37: return SDLK_LEFT;
        case 38: return SDLK_UP;
        case 39: return SDLK_RIGHT;
        case 40: return SDLK_DOWN;
        case 45: return SDLK_INSERT;
        case 46: return SDLK_DELETE;
        case 48: return SDLK_0;
        case 49: return SDLK_1;
        case 50: return SDLK_2;
        case 51: return SDLK_3;
        case 52: return SDLK_4;
        case 53: return SDLK_5;
        case 54: return SDLK_6;
        case 55: return SDLK_7;
        case 56: return SDLK_8;
        case 57: return SDLK_9;
        case 65: return SDLK_a;
        case 66: return SDLK_b;
        case 67: return SDLK_c;
        case 68: return SDLK_d;
        case 69: return SDLK_e;
        case 70: return SDLK_f;
        case 71: return SDLK_g;
        case 72: return SDLK_h;
        case 73: return SDLK_i;
        case 74: return SDLK_j;
        case 75: return SDLK_k;
        case 76: return SDLK_l;
        case 77: return SDLK_m;
        case 78: return SDLK_n;
        case 79: return SDLK_o;
        case 80: return SDLK_p;
        case 81: return SDLK_q;
        case 82: return SDLK_r;
        case 83: return SDLK_s;
        case 84: return SDLK_t;
        case 85: return SDLK_u;
        case 86: return SDLK_v;
        case 87: return SDLK_w;
        case 88: return SDLK_x;
        case 89: return SDLK_y;
        case 90: return SDLK_z;
        case 91: return SDLK_LGUI;
        case 92: return SDLK_RGUI;
        case 93: return SDLK_MENU;
        case 96: return SDLK_KP_0;
        case 97: return SDLK_KP_1;
        case 98: return SDLK_KP_2;
        case 99: return SDLK_KP_3;
        case 100: return SDLK_KP_4;
        case 101: return SDLK_KP_5;
        case 102: return SDLK_KP_6;
        case 103: return SDLK_KP_7;
        case 104: return SDLK_KP_8;
        case 105: return SDLK_KP_9;
        case 106: return SDLK_KP_MULTIPLY;
        case 107: return SDLK_KP_PLUS;
        case 109: return SDLK_KP_MINUS;
        case 110: return SDLK_KP_DECIMAL;
        case 111: return SDLK_KP_DIVIDE;
        case 112: return SDLK_F1;
        case 113: return SDLK_F2;
        case 114: return SDLK_F3;
        case 115: return SDLK_F4;
        case 116: return SDLK_F5;
        case 117: return SDLK_F6;
        case 118: return SDLK_F7;
        case 119: return SDLK_F8;
        case 120: return SDLK_F9;
        case 121: return SDLK_F10;
        case 122: return SDLK_F11;
        case 123: return SDLK_F12;
        case 124: return SDLK_F13;
        case 125: return SDLK_F14;
        case 126: return SDLK_F15;
        case 127: return SDLK_F16;
        case 128: return SDLK_F17;
        case 129: return SDLK_F18;
        case 130: return SDLK_F19;
        case 131: return SDLK_F20;
        case 132: return SDLK_F21;
        case 133: return SDLK_F22;
        case 134: return SDLK_F23;
        case 135: return SDLK_F24;
        case 144: return SDLK_NUMLOCKCLEAR;
        case 145: return SDLK_SCROLLLOCK;
        case 160: return SDLK_LSHIFT;
        case 161: return SDLK_RSHIFT;
        case 162: return SDLK_LCTRL;
        case 163: return SDLK_RCTRL;
        case 164: return SDLK_LALT;
        case 165: return SDLK_RALT;
        case 186: return SDLK_SEMICOLON;
        case 187: return SDLK_EQUALS;
        case 188: return SDLK_COMMA;
        case 189: return SDLK_MINUS;
        case 190: return SDLK_PERIOD;
        case 191: return SDLK_SLASH;
        case 192: return SDLK_BACKQUOTE;
        case 219: return SDLK_LEFTBRACKET;
        case 220: return SDLK_BACKSLASH;
        case 221: return SDLK_RIGHTBRACKET;
        case 222: return SDLK_QUOTE;
        case 226: return SDLK_BACKSLASH;
    }
    return -1;
}

inline unsigned int
get_key_string_hash (register const char *str, register unsigned int len)
{
  static unsigned char asso_values[] =
    {
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255,   0, 255, 255, 255,
      255, 255, 112, 107, 255, 102, 255,  97,  27,  25,
       15,  75,  50,   5,   0,   7, 125, 120,   0,   0,
        5,   0, 255, 255, 255, 100,  17,  40,  70,  60,
       10,  37,  42,  80,  92,  32,  45,  30,  95,  87,
       90,  82,  65,  20,  35, 115,  62,  85,  77,   2,
       72, 255, 255, 255, 255, 255, 255, 100,  17,  40,
       70,  60,  10,  37,  42,  80,  92,  32,  45,  30,
       95,  87,  90,  82,  65,  20,  35, 115,  62,  85,
       77,   2,  72, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[1]+4];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}


int translate_string_to_key(const chowstring & in)
{
    if (in.empty())
        return -1;
    unsigned int hash = get_key_string_hash(&in[0], in.size());
    switch (hash) {
        case 1: return SDLK_6;
        case 5: return SDLK_y;
        case 11: return SDLK_5;
        case 12: return SDLK_F6;
        case 15: return SDLK_7;
        case 18: return SDLK_F16;
        case 19: return SDLK_F7;
        case 21: return SDLK_f;
        case 23: return SDLK_F15;
        case 25: return SDLK_F17;
        case 27: return SDLK_F2;
        case 28: return SDLK_F22;
        case 31: return SDLK_2;
        case 33: return SDLK_F12;
        case 35: return SDLK_b;
        case 38: return SDLK_F21;
        case 40: return SDLK_F20;
        case 41: return SDLK_s;
        case 42: return SDLK_F1;
        case 43: return SDLK_F11;
        case 45: return SDLK_F10;
        case 49: return SDLK_KP_6;
        case 51: return SDLK_1;
        case 54: return SDLK_KP_5;
        case 55: return SDLK_0;
        case 56: return SDLK_KP_7;
        case 61: return SDLK_m;
        case 63: return SDLK_F24;
        case 64: return SDLK_KP_2;
        case 65: return SDLK_k;
        case 68: return SDLK_F14;
        case 71: return SDLK_t;
        case 73: return SDL_BUTTON_X2;
        case 74: return SDLK_KP_1;
        case 75: return SDLK_g;
        case 76: return SDLK_KP_0;
        case 81: return SDLK_c;
        case 83: return SDL_BUTTON_X1;
        case 85: return SDLK_h;
        case 88: return SDLK_F23;
        case 91: return SDLK_l;
        case 93: return SDLK_F13;
        case 94: return SDLK_F3;
        case 95: return SDL_BUTTON_LEFT;
        case 96: return SDL_BUTTON_RIGHT;
        case 98: return SDLK_SCROLLLOCK;
        case 99: return SDLK_KP_4;
        case 101: return SDLK_4;
        case 105: return SDLK_LSHIFT;
        case 107: return SDLK_LCTRL;
        case 108: return SDLK_RCTRL;
        case 110: return SDLK_LSHIFT;
        case 111: return SDLK_RSHIFT;
        case 112: return SDLK_LCTRL;
        case 115: return SDLK_TAB;
        case 116: return SDLK_PAUSE;
        case 120: return SDLK_SPACE;
        case 121: return SDLK_e;
        case 122: return SDL_BUTTON_MIDDLE;
        case 124: return SDLK_KP_3;
        case 125: return SDLK_v;
        case 126: return SDLK_HOME;
        case 131: return SDLK_r;
        case 132: return SDLK_F9;
        case 135: return SDLK_RIGHT;
        case 136: return SDLK_NUMLOCKCLEAR;
        case 137: return SDLK_F5;
        case 138: return SDLK_F19;
        case 140: return SDLK_CAPSLOCK;
        case 141: return SDLK_d;
        case 142: return SDLK_F8;
        case 143: return SDLK_F18;
        case 145: return SDLK_z;
        case 146: return SDLK_BACKSPACE;
        case 151: return SDLK_3;
        case 155: return SDLK_x;
        case 161: return SDLK_i;
        case 164: return SDLK_LEFT;
        case 165: return SDLK_q;
        case 169: return SDLK_KP_9;
        case 171: return SDLK_w;
        case 174: return SDLK_KP_8;
        case 175: return SDLK_o;
        case 181: return SDLK_p;
        case 185: return SDLK_j;
        case 186: return SDLK_INSERT;
        case 187: return SDLK_F4;
        case 189: return SDLK_DOWN;
        case 191: return SDLK_n;
        case 192: return SDLK_RGUI;
        case 195: return SDLK_KP_DIVIDE;
        case 198: return SDLK_END;
        case 200: return SDLK_CLEAR;
        case 201: return SDLK_a;
        case 202: return SDLK_KP_DECIMAL;
        case 205: return SDLK_KP_MINUS;
        case 211: return SDLK_ESCAPE;
        case 215: return SDLK_KP_PLUS;
        case 216: return SDLK_DELETE;
        case 221: return SDLK_LGUI;
        case 225: return SDLK_KP_MULTIPLY;
        case 228: return SDLK_RALT;
        case 229: return SDLK_MENU;
        case 231: return SDLK_u;
        case 233: return SDLK_LALT;
        case 234: return SDLK_RALT;
        case 241: return SDLK_9;
        case 242: return SDLK_UP;
        case 246: return SDLK_RETURN;
        case 247: return SDLK_PAGEUP;
        case 251: return SDLK_8;
        case 254: return SDLK_PAGEDOWN;
    }
    return -1;
}

chowstring translate_vk_to_string(int vk)
{
    switch (vk) {
        case 1: return chowstring("Mouse Left", 10);
        case 2: return chowstring("Mouse Right", 11);
        case 4: return chowstring("Mouse Middle", 12);
        case 5: return chowstring("Mouse X1", 8);
        case 6: return chowstring("Mouse X2", 8);
        case 8: return chowstring("Backspace", 9);
        case 9: return chowstring("Tab", 3);
        case 12: return chowstring("Clear", 5);
        case 13: return chowstring("Return", 6);
        case 16: return chowstring("Shift", 5);
        case 17: return chowstring("Control", 7);
        case 18: return chowstring("Alt", 3);
        case 19: return chowstring("Break", 5);
        case 20: return chowstring("Capslock", 8);
        case 27: return chowstring("Escape", 6);
        case 32: return chowstring("Space", 5);
        case 33: return chowstring("Page up", 7);
        case 34: return chowstring("Page down", 9);
        case 35: return chowstring("End", 3);
        case 36: return chowstring("Home", 4);
        case 37: return chowstring("Left", 4);
        case 38: return chowstring("Up", 2);
        case 39: return chowstring("Right", 5);
        case 40: return chowstring("Down", 4);
        case 45: return chowstring("Insert", 6);
        case 46: return chowstring("Delete", 6);
        case 48: return chowstring("0", 1);
        case 49: return chowstring("1", 1);
        case 50: return chowstring("2", 1);
        case 51: return chowstring("3", 1);
        case 52: return chowstring("4", 1);
        case 53: return chowstring("5", 1);
        case 54: return chowstring("6", 1);
        case 55: return chowstring("7", 1);
        case 56: return chowstring("8", 1);
        case 57: return chowstring("9", 1);
        case 65: return chowstring("A", 1);
        case 66: return chowstring("B", 1);
        case 67: return chowstring("C", 1);
        case 68: return chowstring("D", 1);
        case 69: return chowstring("E", 1);
        case 70: return chowstring("F", 1);
        case 71: return chowstring("G", 1);
        case 72: return chowstring("H", 1);
        case 73: return chowstring("I", 1);
        case 74: return chowstring("J", 1);
        case 75: return chowstring("K", 1);
        case 76: return chowstring("L", 1);
        case 77: return chowstring("M", 1);
        case 78: return chowstring("N", 1);
        case 79: return chowstring("O", 1);
        case 80: return chowstring("P", 1);
        case 81: return chowstring("Q", 1);
        case 82: return chowstring("R", 1);
        case 83: return chowstring("S", 1);
        case 84: return chowstring("T", 1);
        case 85: return chowstring("U", 1);
        case 86: return chowstring("V", 1);
        case 87: return chowstring("W", 1);
        case 88: return chowstring("X", 1);
        case 89: return chowstring("Y", 1);
        case 90: return chowstring("Z", 1);
        case 91: return chowstring("Left Window", 11);
        case 92: return chowstring("Right Window", 12);
        case 93: return chowstring("Menu", 4);
        case 96: return chowstring("0 numeric", 9);
        case 97: return chowstring("1 numeric", 9);
        case 98: return chowstring("2 numeric", 9);
        case 99: return chowstring("3 numeric", 9);
        case 100: return chowstring("4 numeric", 9);
        case 101: return chowstring("5 numeric", 9);
        case 102: return chowstring("6 numeric", 9);
        case 103: return chowstring("7 numeric", 9);
        case 104: return chowstring("8 numeric", 9);
        case 105: return chowstring("9 numeric", 9);
        case 106: return chowstring("*", 1);
        case 107: return chowstring("+", 1);
        case 109: return chowstring("-", 1);
        case 110: return chowstring("Decimal", 7);
        case 111: return chowstring("/", 1);
        case 112: return chowstring("F1", 2);
        case 113: return chowstring("F2", 2);
        case 114: return chowstring("F3", 2);
        case 115: return chowstring("F4", 2);
        case 116: return chowstring("F5", 2);
        case 117: return chowstring("F6", 2);
        case 118: return chowstring("F7", 2);
        case 119: return chowstring("F8", 2);
        case 120: return chowstring("F9", 2);
        case 121: return chowstring("F10", 3);
        case 122: return chowstring("F11", 3);
        case 123: return chowstring("F12", 3);
        case 124: return chowstring("F13", 3);
        case 125: return chowstring("F14", 3);
        case 126: return chowstring("F15", 3);
        case 127: return chowstring("F16", 3);
        case 128: return chowstring("F17", 3);
        case 129: return chowstring("F18", 3);
        case 130: return chowstring("F19", 3);
        case 131: return chowstring("F20", 3);
        case 132: return chowstring("F21", 3);
        case 133: return chowstring("F22", 3);
        case 134: return chowstring("F23", 3);
        case 135: return chowstring("F24", 3);
        case 144: return chowstring("Numlock", 7);
        case 145: return chowstring("Scrollock", 9);
        case 160: return chowstring("Shift Left", 10);
        case 161: return chowstring("Shift Right", 11);
        case 162: return chowstring("Control Left", 12);
        case 163: return chowstring("Control Right", 13);
        case 164: return chowstring("Alt Left", 8);
        case 165: return chowstring("Alt Right", 9);
    }
    return "";
}

chowstring translate_key_to_string(int key)
{
    switch (key) {
        case SDLK_KP_MULTIPLY: return chowstring("*", 1);
        case SDLK_CAPSLOCK: return chowstring("Capslock", 8);
        case SDLK_DELETE: return chowstring("Delete", 6);
        case SDLK_INSERT: return chowstring("Insert", 6);
        case SDLK_UP: return chowstring("Up", 2);
        case SDLK_b: return chowstring("B", 1);
        case SDLK_RCTRL: return chowstring("Control Right", 13);
        case SDL_BUTTON_MIDDLE: return chowstring("Mouse Middle", 12);
        case SDLK_NUMLOCKCLEAR: return chowstring("Numlock", 7);
        case SDL_BUTTON_X1: return chowstring("Mouse X1", 8);
        case SDL_BUTTON_X2: return chowstring("Mouse X2", 8);
        case SDLK_k: return chowstring("K", 1);
        case SDLK_RGUI: return chowstring("Right Window", 12);
        case SDLK_LGUI: return chowstring("Left Window", 11);
        case SDLK_F24: return chowstring("F24", 3);
        case SDLK_F23: return chowstring("F23", 3);
        case SDLK_F22: return chowstring("F22", 3);
        case SDLK_F21: return chowstring("F21", 3);
        case SDLK_F20: return chowstring("F20", 3);
        case SDLK_KP_8: return chowstring("8 numeric", 9);
        case SDLK_KP_9: return chowstring("9 numeric", 9);
        case SDLK_KP_4: return chowstring("4 numeric", 9);
        case SDLK_KP_5: return chowstring("5 numeric", 9);
        case SDLK_KP_6: return chowstring("6 numeric", 9);
        case SDLK_KP_7: return chowstring("7 numeric", 9);
        case SDLK_KP_0: return chowstring("0 numeric", 9);
        case SDLK_KP_1: return chowstring("1 numeric", 9);
        case SDLK_KP_2: return chowstring("2 numeric", 9);
        case SDLK_KP_3: return chowstring("3 numeric", 9);
        case SDLK_F12: return chowstring("F12", 3);
        case SDLK_F13: return chowstring("F13", 3);
        case SDLK_F10: return chowstring("F10", 3);
        case SDLK_F11: return chowstring("F11", 3);
        case SDLK_F16: return chowstring("F16", 3);
        case SDLK_F17: return chowstring("F17", 3);
        case SDLK_F14: return chowstring("F14", 3);
        case SDLK_F15: return chowstring("F15", 3);
        case SDLK_F18: return chowstring("F18", 3);
        case SDLK_F19: return chowstring("F19", 3);
        case SDLK_LCTRL: return chowstring("Control Left", 12);
        case SDL_BUTTON_RIGHT: return chowstring("Mouse Right", 11);
        case SDLK_RALT: return chowstring("Alt Right", 9);
        case SDLK_RETURN: return chowstring("Return", 6);
        case SDLK_PAUSE: return chowstring("Break", 5);
        case SDLK_SCROLLLOCK: return chowstring("Scrollock", 9);
        case SDLK_KP_ENTER: return chowstring("Return", 6);
        case SDLK_ESCAPE: return chowstring("Escape", 6);
        case SDLK_RSHIFT: return chowstring("Shift Right", 11);
        case SDLK_KP_MINUS: return chowstring("-", 1);
        case SDLK_KP_DECIMAL: return chowstring("Decimal", 7);
        case SDLK_PAGEDOWN: return chowstring("Page down", 9);
        case SDLK_PAGEUP: return chowstring("Page up", 7);
        case SDL_BUTTON_LEFT: return chowstring("Mouse Left", 10);
        case SDLK_0: return chowstring("0", 1);
        case SDLK_KP_PLUS: return chowstring("+", 1);
        case SDLK_END: return chowstring("End", 3);
        case SDLK_TAB: return chowstring("Tab", 3);
        case SDLK_HOME: return chowstring("Home", 4);
        case SDLK_KP_DIVIDE: return chowstring("/", 1);
        case SDLK_RIGHT: return chowstring("Right", 5);
        case SDLK_DOWN: return chowstring("Down", 4);
        case SDLK_u: return chowstring("U", 1);
        case SDLK_t: return chowstring("T", 1);
        case SDLK_w: return chowstring("W", 1);
        case SDLK_v: return chowstring("V", 1);
        case SDLK_q: return chowstring("Q", 1);
        case SDLK_p: return chowstring("P", 1);
        case SDLK_s: return chowstring("S", 1);
        case SDLK_r: return chowstring("R", 1);
        case SDLK_BACKSPACE: return chowstring("Backspace", 9);
        case SDLK_y: return chowstring("Y", 1);
        case SDLK_x: return chowstring("X", 1);
        case SDLK_z: return chowstring("Z", 1);
        case SDLK_e: return chowstring("E", 1);
        case SDLK_d: return chowstring("D", 1);
        case SDLK_g: return chowstring("G", 1);
        case SDLK_f: return chowstring("F", 1);
        case SDLK_a: return chowstring("A", 1);
        case SDLK_c: return chowstring("C", 1);
        case SDLK_LEFT: return chowstring("Left", 4);
        case SDLK_m: return chowstring("M", 1);
        case SDLK_l: return chowstring("L", 1);
        case SDLK_o: return chowstring("O", 1);
        case SDLK_n: return chowstring("N", 1);
        case SDLK_i: return chowstring("I", 1);
        case SDLK_h: return chowstring("H", 1);
        case SDLK_LSHIFT: return chowstring("Shift Left", 10);
        case SDLK_j: return chowstring("J", 1);
        case SDLK_F8: return chowstring("F8", 2);
        case SDLK_F9: return chowstring("F9", 2);
        case SDLK_F1: return chowstring("F1", 2);
        case SDLK_F2: return chowstring("F2", 2);
        case SDLK_F3: return chowstring("F3", 2);
        case SDLK_F4: return chowstring("F4", 2);
        case SDLK_F5: return chowstring("F5", 2);
        case SDLK_F6: return chowstring("F6", 2);
        case SDLK_F7: return chowstring("F7", 2);
        case SDLK_LALT: return chowstring("Alt Left", 8);
        case SDLK_5: return chowstring("5", 1);
        case SDLK_4: return chowstring("4", 1);
        case SDLK_7: return chowstring("7", 1);
        case SDLK_6: return chowstring("6", 1);
        case SDLK_1: return chowstring("1", 1);
        case SDLK_MENU: return chowstring("Menu", 4);
        case SDLK_3: return chowstring("3", 1);
        case SDLK_2: return chowstring("2", 1);
        case SDLK_9: return chowstring("9", 1);
        case SDLK_8: return chowstring("8", 1);
        case SDLK_CLEAR: return chowstring("Clear", 5);
        case SDLK_SPACE: return chowstring("Space", 5);
    }
    return "";
}
