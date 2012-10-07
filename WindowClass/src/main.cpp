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
    if (GetAsyncKeyState(VK_MENU) & 0x8000)
    {
        *(((StopData*)pt)->runningPt) = false;
        return;
    }
    if (!((StopData*)pt)->pagePt->CheckForCompletion ())
    {
        MessageBoxW (((StopData*)pt)->wnd,
                     ((StopData*)pt)->message,
                     ((StopData*)pt)->caption,
                     MB_OK | MB_ICONEXCLAMATION);
    }
    else
    {
        *(((StopData*)pt)->runningPt) = false;
    }
}

void ReadSettingsAndRun ()
{
    AdjustLocale ();

    Settings st;

    if (!ReadSettingsFile (st))
    {
        printf ("Press any key to exit\n");
        getch();
        return;
    }

    SIZE scr = { GetSystemMetrics (SM_CXSCREEN), GetSystemMetrics (SM_CYSCREEN) };
    ApplicationWindow app ((scr.cx - WINDOW_SIZE.x) / 2, (scr.cy - WINDOW_SIZE.y) / 2,
                           WINDOW_SIZE.x, WINDOW_SIZE.y,
                           STYLE_CONST | WS_CLIPCHILDREN,
                           st.caption.data, true, false);
    if (st.grayFill)
    {
        DeleteObject (SelectObject (HDC (app), (HGDIOBJ)(CreateSolidBrush (WINDOW_COLOR))));
        PatBlt (HDC (app), 0, 0, ScreenSize.x, ScreenSize.y, PATCOPY);
    }

    volatile bool running = true;

    for (int currentPage = 1; currentPage <= st.pages; currentPage++)
    {
        running = true;
        MemContainer<wchar_t> filename (new wchar_t [MAX_PATH], MEMORY_ARRAY);
        wsprintf (filename.data, L"%s%d%s",
                  st.filenamePart1.data,
                  currentPage,
                  st.filenamePart2.data);
        Page_Management p (app);

        if (!p.ParseFile(filename.data))
        {
            printf ("Press any key to exit\n");
            getch();
            break;
        }

        filename.Delete();

        StopData data = {(bool*)&running,
                         &p,
                         HWND (app),
                         st.caption.data,
                         st.notCompletedMessage.data};

        Button exit (st.buttonFont.data, st.buttonFontSize,
                     WINDOW_SIZE.x/2 - 75, WINDOW_SIZE.y - 50,
                     150, 50,
                     (currentPage != st.pages) ? st.nextPageButton.data : st.doneButton.data,
                     WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | BS_MULTILINE,
                     0,
                     &app,
                     function_,
                     (LPVOID)&data);
        while (running)
        {
            if (ExitProgram) break;
            Sleep (10);
        }
        if (ExitProgram) break;
    }

}

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR CmdLine,int CmdShow)
{
    ReadSettingsAndRun ();
    return 0;
}
