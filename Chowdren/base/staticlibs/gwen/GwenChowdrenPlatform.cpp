/*
	GWEN
	Copyright (c) 2012 Facepunch Studios
	See license in Gwen.h
*/

#include "SDL_clipboard.h"
#include "SDL_mouse.h"
#include "Gwen/Macros.h"
#include "Gwen/Platform.h"
#include "Gwen/Utility.h"
#include "platform.h"
#include "manager.h"

#ifndef CHOWDREN_IS_DESKTOP
static Gwen::UnicodeString gs_ClipboardEmulator;
#endif

void Gwen::Platform::Sleep(unsigned int iMS)
{
    platform_sleep((double)iMS / 1000.0);
}

void Gwen::Platform::SetCursor(unsigned char cursor)
{
#ifdef CHOWDREN_IS_DESKTOP
    static SDL_Cursor * system_cursor = NULL;
    static int current_cursor = -1;
    if (cursor == current_cursor)
        return;
    current_cursor = cursor;
    if (system_cursor != NULL)
		SDL_FreeCursor(system_cursor);
    SDL_SystemCursor id = SDL_SYSTEM_CURSOR_ARROW;
    switch (cursor) {
        case Gwen::CursorType::Normal:
            id = SDL_SYSTEM_CURSOR_ARROW;
            break;
        case Gwen::CursorType::Beam:
            id = SDL_SYSTEM_CURSOR_IBEAM;
            break;
        case Gwen::CursorType::SizeNS:
            id = SDL_SYSTEM_CURSOR_SIZENS;
            break;
        case Gwen::CursorType::SizeWE:
            id = SDL_SYSTEM_CURSOR_SIZEWE;
            break;
        case Gwen::CursorType::SizeNWSE:
            id = SDL_SYSTEM_CURSOR_SIZENWSE;
            break;
        case Gwen::CursorType::SizeNESW:
            id = SDL_SYSTEM_CURSOR_SIZENESW;
            break;
        case Gwen::CursorType::SizeAll:
            id = SDL_SYSTEM_CURSOR_SIZEALL;
            break;
        case Gwen::CursorType::No:
            id = SDL_SYSTEM_CURSOR_NO;
            break;
        case Gwen::CursorType::Wait:
            id = SDL_SYSTEM_CURSOR_WAIT;
            break;
        case Gwen::CursorType::Finger:
            id = SDL_SYSTEM_CURSOR_HAND;
            break;
    }
	system_cursor = SDL_CreateSystemCursor(id);
	SDL_SetCursor(system_cursor);
#endif
}

Gwen::UnicodeString Gwen::Platform::GetClipboardText()
{
#ifdef CHOWDREN_IS_DESKTOP
    return Gwen::Utility::StringToUnicode(SDL_GetClipboardText());
#else
	return gs_ClipboardEmulator;
#endif
}

bool Gwen::Platform::SetClipboardText( const Gwen::UnicodeString & str )
{
#ifdef CHOWDREN_IS_DESKTOP
    SDL_SetClipboardText(Gwen::Utility::UnicodeToString(str).c_str());
#else
	gs_ClipboardEmulator = str;
#endif
	return true;
}

float Gwen::Platform::GetTimeInSeconds()
{
	return platform_get_time();
}

bool Gwen::Platform::FileOpen( const String & Name, const String & StartPath,
    const String & Extension, Gwen::Event::Handler* pHandler,
    Event::Handler::FunctionWithInformation fnCallback )
{
	// No platform independent way to do this.
	// Ideally you would open a system dialog here
	return false;
}

bool Gwen::Platform::FileSave( const String & Name, const String & StartPath,
    const String & Extension, Gwen::Event::Handler* pHandler,
    Gwen::Event::Handler::FunctionWithInformation fnCallback )
{
	// No platform independent way to do this.
	// Ideally you would open a system dialog here
	return false;
}

bool Gwen::Platform::FolderOpen( const String & Name, const String & StartPath,
    Gwen::Event::Handler* pHandler,
    Event::Handler::FunctionWithInformation fnCallback )
{
	return false;
}

void* Gwen::Platform::CreatePlatformWindow( int x, int y, int w, int h,
    const Gwen::String & strWindowTitle )
{
    // Only needed by WindowCanvas, can probably be ignored
	return NULL;
}

void Gwen::Platform::DestroyPlatformWindow( void* pPtr )
{
    // Only needed by WindowCanvas, can probably be ignored
}

void Gwen::Platform::MessagePump( void* pWindow,
    Gwen::Controls::Canvas* ptarget )
{
}

void Gwen::Platform::SetBoundsPlatformWindow( void* pPtr, int x, int y,
    int w, int h )
{
}

void Gwen::Platform::SetWindowMaximized( void* pPtr, bool bMax,
    Gwen::Point & pNewPos, Gwen::Point & pNewSize )
{
}

void Gwen::Platform::SetWindowMinimized( void* pPtr, bool bMinimized )
{
}

bool Gwen::Platform::HasFocusPlatformWindow( void* pPtr )
{
	return false;
}

void Gwen::Platform::GetDesktopSize( int & w, int & h )
{
}

void Gwen::Platform::GetCursorPos( Gwen::Point & po )
{
    manager.frame->get_mouse_pos(&po.x, &po.y);
}
