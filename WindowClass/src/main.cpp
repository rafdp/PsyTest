#define   DEBUG_1
#include "Make.h"

struct StopData
{
    bool* runningPt;
    Page_Management* pagePt;
    HWND wnd;
    LPCWSTR caption;
    LPCWSTR message;
};

void function_ (void*, void* pt, WPARAM, LPARAM)
{
    if (!((StopData*)pt)->pagePt->CheckForCompletion () &&
        !GetAsyncKeyState(VK_MENU))
    {
        MessageBoxW (((StopData*)pt)->wnd,
                     ((StopData*)pt)->message,
                     ((StopData*)pt)->caption,
                     MB_OK | MB_ICONEXCLAMATION);
    }
    else *(((StopData*)pt)->runningPt) = false;
}

void ReadSettingsAndRun ()
{
    AdjustLocale ();

    Settings st;

    if (!ReadSettingsFile (st)) exit (0);

    SIZE scr = { GetSystemMetrics (SM_CXSCREEN), GetSystemMetrics (SM_CYSCREEN) };
    ApplicationWindow app ((scr.cx - WINDOW_SIZE.x) / 2, (scr.cy - WINDOW_SIZE.y) / 2,
                           WINDOW_SIZE.x, WINDOW_SIZE.y,
                           STYLE_CONST | WS_CLIPCHILDREN,
                           st.caption, true, false);
    //DeleteObject (SelectObject (HDC (app), (HGDIOBJ)(CreateSolidBrush (WINDOW_COLOR))));
    //PatBlt (HDC (app), 0, 0, ScreenSize.x, ScreenSize.y, PATCOPY);

    volatile bool running = true;

    for (int currentPage = 1; currentPage <= st.pages; currentPage++)
    {
        running = true;
        LPWSTR filename = new wchar_t [MAX_PATH];
        wsprintf (filename, L"%s%d%s",
                  st.filenamePart1,
                  currentPage,
                  st.filenamePart2);
        Page_Management p (app, filename);
        SecureArrayDelete (filename);

        StopData data = {(bool*)&running,
                         &p,
                         HWND (app),
                         st.caption,
                         st.notCompletedMessage};

        Button exit (st.buttonFont, st.buttonFontSize,
                     WINDOW_SIZE.x/2 - 75, WINDOW_SIZE.y - 50,
                     150, 50,
                     (currentPage != st.pages) ? st.nextPageButton : st.doneButton,
                     WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | BS_MULTILINE,
                     0,
                     &app,
                     function_,
                     (LPVOID)&data);
        while (running)
        { if (ExitProgram) break; }
          if (ExitProgram) break;
    }

}

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR CmdLine,int CmdShow)
{
    ReadSettingsAndRun ();
    return 0;
}
