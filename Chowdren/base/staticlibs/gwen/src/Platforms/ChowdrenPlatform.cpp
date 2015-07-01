/*
	GWEN
	Copyright (c) 2012 Facepunch Studios
	See license in Gwen.h
*/

#include "Gwen/Macros.h"
#include "Gwen/Platform.h"
#include "platform.h"
#include "manager.h"

static Gwen::UnicodeString gs_ClipboardEmulator;

void Gwen::Platform::Sleep( unsigned int iMS )
{
    platform_sleep((double)iMS / 1000.0);
}

void Gwen::Platform::SetCursor( unsigned char iCursor )
{
}

Gwen::UnicodeString Gwen::Platform::GetClipboardText()
{
	return gs_ClipboardEmulator;
}

bool Gwen::Platform::SetClipboardText( const Gwen::UnicodeString & str )
{
	gs_ClipboardEmulator = str;
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
    // Only needed by WindowCanvas, can probably be ignored
	w = 1024;
	h = 768;
}

void Gwen::Platform::GetCursorPos( Gwen::Point & po )
{
    manager.frame->get_mouse_pos(&po.x, &po.y);
}
