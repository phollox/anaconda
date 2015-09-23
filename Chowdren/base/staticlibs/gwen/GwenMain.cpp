/*
    GWEN
    Copyright (c) 2010 Facepunch Studios
    See license in Gwen.h
*/


#include "Gwen/Gwen.h"
#include <stdio.h>
#include <stdarg.h>
#include "utf8/unchecked.h"
#include <iterator>

namespace Gwen
{
    // Globals
    GWEN_EXPORT Controls::Base* GlobalHoveredControl = NULL;
    GWEN_EXPORT Controls::Base* HoveredControl = NULL;
    GWEN_EXPORT Controls::Base* KeyboardFocus = NULL;
    GWEN_EXPORT Controls::Base* MouseFocus = NULL;

    namespace Debug
    {
        void Msg( const char* str, ... )
        {
            char strOut[1024];
            va_list s;
            va_start( s, str );
            vsnprintf( strOut, sizeof( strOut ), str, s );
            va_end( s );
            GwenUtil_OutputDebugCharString( strOut );
        }
#ifdef UNICODE
        void Msg( const wchar_t* str, ... )
        {
            wchar_t strOut[1024];
            va_list s;
            va_start( s, str );
            vswprintf( strOut, sizeof( strOut ), str, s );
            va_end( s );
            GwenUtil_OutputDebugWideString( strOut );
        }
#endif
        void AssertCheck( bool b, const char* strMsg )
        {
            if ( b ) { return; }

            Msg( "Assert: %s\n", strMsg );
#ifdef _WIN32
            MessageBoxA( NULL, strMsg, "Assert", MB_ICONEXCLAMATION | MB_OK );
            __debugbreak();
#endif
        }
    }

    namespace Utility
    {
        String UnicodeToString( const UnicodeString & strIn )
        {
            if ( !strIn.length() ) { return ""; }
            String target;
            if (sizeof(wchar_t) == 2) {
                utf8::unchecked::utf16to8(strIn.begin(), strIn.end(),
                                          std::back_inserter(target));
            } else {
                utf8::unchecked::utf32to8(strIn.begin(), strIn.end(),
                                          std::back_inserter(target));
            }
            return target;
        }

        UnicodeString StringToUnicode( const String & strIn )
        {
            if ( !strIn.length() ) { return L""; }
            UnicodeString target;
            if (sizeof(wchar_t) == 2) {
                utf8::unchecked::utf8to16(strIn.begin(), strIn.end(),
                                          std::back_inserter(target));
            } else {
                utf8::unchecked::utf8to32(strIn.begin(), strIn.end(),
                                          std::back_inserter(target));

            }
            return target;
        }
    }
}
