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

int remap_button(int n)
{
#ifdef CHOWDREN_SNES_CONTROLLER
    switch (n) {
        case CHOWDREN_BUTTON_X: return CHOWDREN_BUTTON_A;
        case CHOWDREN_BUTTON_Y: return CHOWDREN_BUTTON_X;
        case CHOWDREN_BUTTON_GUIDE: return CHOWDREN_BUTTON_RIGHTSHOULDER;
        case CHOWDREN_BUTTON_BACK: return CHOWDREN_BUTTON_LEFTSHOULDER;
        case CHOWDREN_BUTTON_LEFTSHOULDER: return CHOWDREN_BUTTON_START;
        case CHOWDREN_BUTTON_B: return CHOWDREN_BUTTON_B;
    }
#endif
#ifdef CHOWDREN_JOYSTICK2_CONTROLLER
    switch (n) {
        case 5: return CHOWDREN_BUTTON_LEFTSHOULDER;
        case 6: return CHOWDREN_BUTTON_RIGHTSHOULDER;
        case 7: return CHOWDREN_BUTTON_BACK;
        case 8: return CHOWDREN_BUTTON_START;
        case 9: return CHOWDREN_BUTTON_LEFTSTICK;
        case 10: return CHOWDREN_BUTTON_RIGHTSTICK;
        case 11: return CHOWDREN_BUTTON_GUIDE;
    }
#endif
    return n;
}
int unremap_button(int n)
{
#ifdef CHOWDREN_SNES_CONTROLLER
    switch (n) {
        case CHOWDREN_BUTTON_A: return CHOWDREN_BUTTON_X;
        case CHOWDREN_BUTTON_X: return CHOWDREN_BUTTON_Y;
        case CHOWDREN_BUTTON_RIGHTSHOULDER: return CHOWDREN_BUTTON_GUIDE;
        case CHOWDREN_BUTTON_LEFTSHOULDER: return CHOWDREN_BUTTON_BACK;
        case CHOWDREN_BUTTON_START: return CHOWDREN_BUTTON_LEFTSHOULDER;
        case CHOWDREN_BUTTON_B: return CHOWDREN_BUTTON_B;
    }
#endif
#ifdef CHOWDREN_JOYSTICK2_CONTROLLER
    switch (n) {
        case CHOWDREN_BUTTON_LEFTSHOULDER: return 5;
        case CHOWDREN_BUTTON_RIGHTSHOULDER: return 6;
        case CHOWDREN_BUTTON_BACK: return 7;
        case CHOWDREN_BUTTON_START: return 8;
        case CHOWDREN_BUTTON_LEFTSTICK: return 9;
        case CHOWDREN_BUTTON_RIGHTSTICK: return 10;
        case CHOWDREN_BUTTON_GUIDE: return 11;
    }
#endif
    return n;
}
