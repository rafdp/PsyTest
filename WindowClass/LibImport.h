
#ifdef __cplusplus
extern "C"
{
#endif

//HWND WINAPI GetConsoleWindow (VOID);
//BOOL WINAPI AllocConsole (VOID);
//BOOL WINAPI FreeConsole (VOID);

struct CONSOLE_FONT_INFOEX
    {
    ULONG cbSize;
    DWORD nFont;
    COORD dwFontSize;
    UINT  FontFamily;
    UINT  FontWeight;
    WCHAR FaceName[LF_FACESIZE];
    };

BOOL WINAPI GetCurrentConsoleFontEx (HANDLE con, BOOL maxWnd, CONSOLE_FONT_INFOEX* curFont);
BOOL WINAPI SetCurrentConsoleFontEx (HANDLE con, BOOL maxWnd, CONSOLE_FONT_INFOEX* curFont);

#ifdef __cplusplus
}
#endif

/*
#define   DEBUG_1
#include "Make.h"

struct StopData
{
    bool* runningPt;
    Page_Management* pagePt;
    HWND wnd;
};

void function_ (void*, void* pt, WPARAM, LPARAM)
{
    if (!((StopData*)pt)->pagePt->CheckForCompletion ())
    {
        MessageBoxW (((StopData*)pt)->wnd,
                     L"Заполните все ответы",
                     L"Тест на нагрузку",
                     MB_OK | MB_ICONEXCLAMATION);
    }
    else *(((StopData*)pt)->runningPt) = false;
}

void ReadSettingsAndRun ()
{
    Settings st;

    if (!ReadSettingsFile (st)) exit (0);

    st.Print();

    SIZE scr = { GetSystemMetrics (SM_CXSCREEN), GetSystemMetrics (SM_CYSCREEN) };
    ApplicationWindow app ((scr.cx - WINDOW_SIZE.x) / 2, (scr.cy - WINDOW_SIZE.y) / 2,
                           WINDOW_SIZE.x, WINDOW_SIZE.y,
                           STYLE_CONST | WS_CLIPCHILDREN,
                           st.caption, true, false);
    DeleteObject (SelectObject (HDC (app), (HGDIOBJ)(CreateSolidBrush (WINDOW_COLOR))));
    PatBlt (HDC (app), 0, 0, ScreenSize.x, ScreenSize.y, PATCOPY);


    volatile bool running = true;

    for (int currentPage = 1; currentPage <= st.pages; currentPage++)
    {
        LPWSTR filename = new wchar_t [MAX_PATH];
        wsprintf (filename, L"%s%d%s",
                  st.filenamePart1,
                  currentPage,
                  st.filenamePart2);
        Page_Management p (app, filename);
        SecureArrayDelete (filename);

        StopData data = {(bool*)&running, &p, HWND (app)};

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
*/
