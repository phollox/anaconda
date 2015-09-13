// Copyright (c) MP2 Games ApS 2015.
//
// This file is part of fuse.
//
// fuse is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// fuse is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with fuse.  If not, see <http://www.gnu.org/licenses/>.

#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#define MMF2_CLIPBOARD 50194
#define MMF2_CLIPBOARD_NAME "CF_CNCV2_EVENTS"
#define F25_CLIPBOARD 50210
#define F25_CLIPBOARD_NAME "CF_CNCV25_EVENTS"

#define WINDOW_WIDTH 500
#define WINDOW_HEIGTH 50

#define WM_OPEN_MENU (WM_USER + 0x0001)
#define WM_CLOSE_MENU (WM_USER + 0x0002)

#define EDIT_HANDLE 1

struct Shortcut
{
    unsigned int format;
    std::string name;
    std::string data;
};

static std::vector<Shortcut> shortcuts;
static HMENU menu;
static bool menu_init;
static HWND window_handle;
static HHOOK hook;
static bool menu_open;
static HINSTANCE global_instance;
static HWND edit_desc_handle;
static HWND edit_handle;
static bool destroy_item;
static Shortcut current_shortcut;
static bool edit_open;
static HWND old_window;

static void read_config()
{
    FILE * fp = fopen("shortcuts.txt", "rb");
    if (fp == NULL)
        return;
    char line[4096];
    std::string name;
    std::string path;
    while (true) {
        fgets(line, sizeof(line)-1, fp);
        if (feof(fp))
            break;
        name = line;
        name.resize(name.size()-1);
        path = name + ".bin";
        fgets(line, sizeof(line)-1, fp);
        if (feof(fp))
            break;
        unsigned int format = atoi(line);
        FILE * bin = fopen(path.c_str(), "rb");
        if (bin == NULL)
            continue;
        fseek(bin, 0, SEEK_END);
        size_t size = ftell(bin);
        fseek(bin, 0, SEEK_SET);
        current_shortcut.data.resize(size);
        fread(&current_shortcut.data[0], size, 1, bin);
        fclose(bin);
        printf("Read shortcut: %s\n", name.c_str(), format);
        current_shortcut.format = format;
        current_shortcut.name = name;
        shortcuts.push_back(current_shortcut);
    }
    fclose(fp);
}

static void save_config(Shortcut * shortcut)
{
    if (shortcut != NULL) {
        std::string path = shortcut->name + ".bin";
        FILE * bin = fopen(path.c_str(), "wb");
        fwrite(&shortcut->data[0], shortcut->data.size(), 1, bin);
        fclose(bin);
    }

    std::stringstream data;
    std::vector<Shortcut>::iterator it;
    for (it = shortcuts.begin(); it != shortcuts.end(); ++it) {
        data << it->name << "\n" << it->format << "\n";
    }
    std::string s = data.str();
    FILE * fp = fopen("shortcuts.txt", "wb");
    fwrite(&s[0], s.size(), 1, fp);
    fclose(fp);
}

static HMENU get_menu()
{
    if (menu_init)
        DestroyMenu(menu);
    menu_init = true;
    menu = CreatePopupMenu();
    std::vector<Shortcut>::iterator it;
    int i = 1;
    for (it = shortcuts.begin(); it != shortcuts.end(); ++it) {
        AppendMenuA(menu, MF_STRING, i, it->name.c_str());
        i++;
    }
    if (destroy_item)
        return menu;
    AppendMenuA(menu, MF_STRING, i++, "Add shortcut from current clipboard");
    AppendMenuA(menu, MF_STRING, i++, "Remove shortcut");
    AppendMenuA(menu, MF_STRING, i++, "Quit");
    return menu;
}

static void add_shortcut()
{
    OpenClipboard(NULL);
    unsigned int format = MMF2_CLIPBOARD;
    HANDLE data = GetClipboardData(format);
    if (data == NULL) {
        format = F25_CLIPBOARD;
        data = GetClipboardData(format);
        if (data == NULL) {
            CloseClipboard();
            return;
        }
    }
    const char * data_c = (char*)GlobalLock(data);
    if (data == NULL) {
        CloseClipboard();
        GlobalUnlock(data);
        return;
    }
    int size = GlobalSize(data);
    std::string shortcut_data(data_c, size);
    GlobalUnlock(data);
    CloseClipboard();

    edit_open = true;

    current_shortcut.format = format;
    current_shortcut.data = shortcut_data;
    ShowWindow(window_handle, SW_SHOW);
    SetForegroundWindow(window_handle);
}

static void close_shortcut()
{
    edit_open = false;
    char line[512];
    unsigned int ret = GetDlgItemTextA(window_handle, EDIT_HANDLE,
                                       &line[0], sizeof(line)-1);
    current_shortcut.name = line;
    shortcuts.push_back(current_shortcut);
    save_config(&current_shortcut);
    ShowWindow(window_handle, SW_HIDE);
    SendMessage(edit_handle, WM_SETTEXT, NULL, (LPARAM)"");
}

static void show_menu();

static void on_edit(int index)
{
    switch (index) {
        case 0: // add shortcut
            add_shortcut();
            break;
        case 1:
            if (shortcuts.size() <= 0)
                break;
            destroy_item = true;
            show_menu();
            destroy_item = false;
            break;
        case 2:
            exit(0);
            break;
    }
}

static void on_menu(int index)
{
    if (destroy_item) {
        shortcuts.erase(shortcuts.begin() + index);
        destroy_item = false;
        save_config(NULL);
        return;
    }
    if (index >= int(shortcuts.size())) {
        on_edit(index - int(shortcuts.size()));
        return;
    }

    Shortcut & shortcut = shortcuts[index];
    HGLOBAL mem =  GlobalAlloc(GMEM_MOVEABLE, shortcut.data.size());
    memcpy(GlobalLock(mem), &shortcut.data[0], shortcut.data.size());
    GlobalUnlock(mem);
    OpenClipboard(window_handle);
    EmptyClipboard();
    SetClipboardData(shortcut.format, mem);
    CloseClipboard();

    SetForegroundWindow(old_window);

    INPUT ip;
    Sleep(50);
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    ip.ki.dwFlags = 0;
    
    ip.ki.wVk = VK_CONTROL;
    SendInput(1, &ip, sizeof(INPUT));

    ip.ki.wVk = 'V';
    SendInput(1, &ip, sizeof(INPUT));

    ip.ki.dwFlags = KEYEVENTF_KEYUP;

    ip.ki.wVk = 'V';
    SendInput(1, &ip, sizeof(INPUT));

    ip.ki.wVk = VK_CONTROL;
    SendInput(1, &ip, sizeof(INPUT));
}

static void show_menu()
{
    if (menu_open)
        return;
    menu_open = true;
    int index;
    old_window = GetForegroundWindow();
    POINT point = {200, 200};
    GetCursorPos(&point);
    index = TrackPopupMenu(get_menu(),
                           TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON |
                           TPM_RETURNCMD | TPM_NONOTIFY,
                           point.x, point.y, 0, window_handle, NULL) - 1;
    menu_open = false;
    if (index != -1)
        on_menu(index);
}

static LRESULT CALLBACK hook_proc(int code, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT * data = (KBDLLHOOKSTRUCT*)lParam;
    if (data->vkCode == VK_F5 && wParam == WM_KEYDOWN && !menu_open) {
        PostMessage(window_handle, WM_OPEN_MENU, 0, 0);
    }
    if (data->vkCode == VK_RETURN && wParam == WM_KEYDOWN && edit_open) {
        PostMessage(window_handle, WM_CLOSE_MENU, 0, 0);
    }
    return CallNextHookEx(hook, code, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR lpCmdLine, int nCmdShow)
{
    printf("fuse - Fusion Shortcut Editor\n"
           "(c) MP2 Games ApS 2015\n\n"
           "Licensed under the GPL.\n\n"
           "Press F5 to open the shortcut menu!\n\n");

    read_config();

    global_instance = hInstance;
    MSG msg;    
    WNDCLASSW wc = {0};
    wc.lpszClassName = L"Menu";
    wc.hInstance = hInstance;
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(0, IDC_ARROW);

    hook = SetWindowsHookEx(WH_KEYBOARD_LL, hook_proc, NULL, 0);

    RegisterClassW(&wc);
    window_handle = CreateWindowW(wc.lpszClassName, L"Menu",
                                  0, 200, 200,
                                  WINDOW_WIDTH+6, WINDOW_HEIGTH+25, 0, 0,
                                  hInstance, 0);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hook);

    return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, 
                         WPARAM wParam, LPARAM lParam)
{
    switch (msg)  
    {
        case WM_CREATE:
            edit_handle = CreateWindow("edit", 0,
                                       WS_CHILD | WS_VISIBLE | WS_BORDER,
                                       0, 25, WINDOW_WIDTH, WINDOW_HEIGTH - 25,
                                       hwnd, (HMENU)EDIT_HANDLE,
                                       global_instance, NULL);
            edit_desc_handle = CreateWindow("static", 0,
                                            WS_CHILD | WS_VISIBLE,
                                            0, 5, WINDOW_WIDTH, 20,
                                            hwnd, (HMENU)0, global_instance,
                                            NULL);
            SendMessage(edit_desc_handle, WM_SETTEXT, NULL,
                        (LPARAM)"Pick the name for your shortcut and press "
                                "'Enter'");
            break;

        case WM_OPEN_MENU:
            // printf("Open menu\n");
            show_menu();
            break;

        case WM_CLOSE_MENU:
            close_shortcut();
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
