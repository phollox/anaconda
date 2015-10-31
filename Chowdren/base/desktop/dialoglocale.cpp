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

#include "platform.h"

static const char * dialog_locale_ok;
static const char * dialog_locale_cancel;
static const char * dialog_locale_yes;
static const char * dialog_locale_no;
static bool has_dialog_locale = false;

void init_dialog_locale()
{
    if (has_dialog_locale) return;
    has_dialog_locale = true;
    const std::string & lang = platform_get_language();
    if (lang == "Swedish") {
        dialog_locale_ok = "OK";
        dialog_locale_yes = "Ja";
        dialog_locale_no = "Nej";
        dialog_locale_cancel = "Avbryt";
        return;
    }
    if (lang == "Portuguese (Brazil)") {
        dialog_locale_ok = "Ok";
        dialog_locale_yes = "Sim";
        dialog_locale_no = "N\303\243o";
        dialog_locale_cancel = "Cancelar";
        return;
    }
    if (lang == "Turkish") {
        dialog_locale_ok = "TAMAM";
        dialog_locale_yes = "Evet";
        dialog_locale_no = "Hay\304\261r";
        dialog_locale_cancel = "\304\260ptal";
        return;
    }
    if (lang == "Romanian") {
        dialog_locale_ok = "OK";
        dialog_locale_yes = "Da";
        dialog_locale_no = "Nu";
        dialog_locale_cancel = "Anulare";
        return;
    }
    if (lang == "Dutch") {
        dialog_locale_ok = "OK";
        dialog_locale_yes = "Ja";
        dialog_locale_no = "Nee";
        dialog_locale_cancel = "Annuleren";
        return;
    }
    if (lang == "Korean") {
        dialog_locale_ok = "\355\231\225\354\235\270";
        dialog_locale_yes = "\354\230\210";
        dialog_locale_no = "\354\225\204\353\213\210\354\232\224";
        dialog_locale_cancel = "\354\267\250\354\206\214";
        return;
    }
    if (lang == "Italian ") {
        dialog_locale_ok = "Ok";
        dialog_locale_yes = "S\303\254";
        dialog_locale_no = "No";
        dialog_locale_cancel = "Cancella";
        return;
    }
    if (lang == "Danish") {
        dialog_locale_ok = "OK";
        dialog_locale_yes = "Ja";
        dialog_locale_no = "Nej";
        dialog_locale_cancel = "Annuler";
        return;
    }
    if (lang == "Bulgarian") {
        dialog_locale_ok = "\320\237\321\200\320\270\320\265\320\274\320\270";
        dialog_locale_yes = "\320\224\320\260";
        dialog_locale_no = "\320\235\320\265";
        dialog_locale_cancel = "\320\236\321\202\320\274\320\265\320\275\320\270";
        return;
    }
    if (lang == "Hungarian") {
        dialog_locale_ok = "OK";
        dialog_locale_yes = "Igen";
        dialog_locale_no = "Nem";
        dialog_locale_cancel = "M\303\251gse";
        return;
    }
    if (lang == "Traditional Chinese") {
        dialog_locale_ok = "OK";
        dialog_locale_yes = "\346\230\257";
        dialog_locale_no = "\345\220\246";
        dialog_locale_cancel = "\345\217\226\346\266\210";
        return;
    }
    if (lang == "French") {
        dialog_locale_ok = "OK";
        dialog_locale_yes = "Oui";
        dialog_locale_no = "Non";
        dialog_locale_cancel = "Annuler";
        return;
    }
    if (lang == "Norwegian") {
        dialog_locale_ok = "OK";
        dialog_locale_yes = "Ja";
        dialog_locale_no = "Nei";
        dialog_locale_cancel = "Avbryte";
        return;
    }
    if (lang == "Russian") {
        dialog_locale_ok = "\320\236\320\232";
        dialog_locale_yes = "\320\224\320\260";
        dialog_locale_no = "\320\235\320\265\321\202";
        dialog_locale_cancel = "\320\236\321\202\320\274\320\265\320\275\320\260";
        return;
    }
    if (lang == "Thai") {
        dialog_locale_ok = "\340\270\225\340\270\201\340\270\245\340\270\207";
        dialog_locale_yes = "\340\271\203\340\270\212\340\271\210";
        dialog_locale_no = "\340\271\204\340\270\241\340\271\210\340\271\203\340\270\212\340\271\210\340\271\210";
        dialog_locale_cancel = "\340\270\242\340\270\201\340\271\200\340\270\245\340\270\264\340\270\201";
        return;
    }
    if (lang == "Finnish") {
        dialog_locale_ok = "OK";
        dialog_locale_yes = "Kyll\303\244";
        dialog_locale_no = "Ei";
        dialog_locale_cancel = "Peruuta";
        return;
    }
    if (lang == "Hebrew") {
        dialog_locale_ok = "\327\230\327\225\327\221";
        dialog_locale_yes = "\327\233\327\237";
        dialog_locale_no = "\327\234\327\220";
        dialog_locale_cancel = "\327\221\327\231\327\230\327\225\327\234";
        return;
    }
    if (lang == "Greek") {
        dialog_locale_ok = "\316\237\316\232";
        dialog_locale_yes = "\316\235\316\261\316\271";
        dialog_locale_no = "\316\214\317\207\316\271";
        dialog_locale_cancel = "\316\221\316\272\317\215\317\201\317\211\317\203\316\267";
        return;
    }
    if (lang == "English") {
        dialog_locale_ok = "OK";
        dialog_locale_yes = "Yes";
        dialog_locale_no = "No";
        dialog_locale_cancel = "Cancel";
        return;
    }
    if (lang == "Simplified Chinese") {
        dialog_locale_ok = "OK";
        dialog_locale_yes = "\346\230\257";
        dialog_locale_no = "\345\220\246";
        dialog_locale_cancel = "\345\217\226\346\266\210";
        return;
    }
    if (lang == "Portuguese") {
        dialog_locale_ok = "Ok";
        dialog_locale_yes = "Sim";
        dialog_locale_no = "N\303\243o";
        dialog_locale_cancel = "Cancelar";
        return;
    }
    if (lang == "German") {
        dialog_locale_ok = "OK";
        dialog_locale_yes = "Ja";
        dialog_locale_no = "Nein";
        dialog_locale_cancel = "Abbrechen";
        return;
    }
    if (lang == "Japanese") {
        dialog_locale_ok = "OK";
        dialog_locale_yes = "\343\201\257\343\201\204";
        dialog_locale_no = "\343\201\204\343\201\204\343\201\210";
        dialog_locale_cancel = "\343\202\255\343\203\243\343\203\263\343\202\273\343\203\253";
        return;
    }
    if (lang == "Czech") {
        dialog_locale_ok = "OK";
        dialog_locale_yes = "Ano";
        dialog_locale_no = "Ne";
        dialog_locale_cancel = "Storno";
        return;
    }
    if (lang == "Spanish") {
        dialog_locale_ok = "Ok";
        dialog_locale_yes = "S\303\255";
        dialog_locale_no = "No";
        dialog_locale_cancel = "Cancelar";
        return;
    }
    if (lang == "Polish") {
        dialog_locale_ok = "OK";
        dialog_locale_yes = "Tak";
        dialog_locale_no = "Nie";
        dialog_locale_cancel = "Anuluj";
        return;
    }
}
const char * get_locale_ok()
{
    if (!has_dialog_locale) init_dialog_locale();
    return dialog_locale_ok;
}
const char * get_locale_cancel()
{
    if (!has_dialog_locale) init_dialog_locale();
    return dialog_locale_cancel;
}
const char * get_locale_yes()
{
    if (!has_dialog_locale) init_dialog_locale();
    return dialog_locale_yes;
}
const char * get_locale_no()
{
    if (!has_dialog_locale) init_dialog_locale();
    return dialog_locale_no;
}
