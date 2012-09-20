#define   DEBUG_1
#include "Make.h"


void function_ (void* pt, WPARAM, LPARAM)
{
    const int MaxStringLen = 100;
    wchar_t str[MaxStringLen] = L"";
    reinterpret_cast<EditBox*> (pt) -> GetText (str, MaxStringLen);
    wprintf (L"BUTTON PRESSED\n{\n  EditBox contains:\n  \"%s\"\n}\n", str);
}

void function1_ (void* pt, WPARAM, LPARAM)
{
    *((bool*)pt) = false;
}

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR CmdLine,int CmdShow)
{
    srand (time (NULL));
    ApplicationWindow app (100, 100,
                           300, 150,
                           STYLE_RESIZABLE | WS_CLIPCHILDREN,
                           L"Тест русских букв через CreateWindowExW", true, false);
    DeleteObject (SelectObject (HDC (app), (HGDIOBJ)(CreateSolidBrush (RGB(rand () % 256, rand () % 256, rand () % 256)))));
    PatBlt (HDC (app), 0, 0, ScreenSize.x, ScreenSize.y, PATCOPY);
    {
        EditBox e (10, 10,
                   280, 50,
                   L"Привет бла бла бла",//", меня зовут EditBox",
                   WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_WANTRETURN | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                   WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE,
                   &app,
                   NULL,
                   (LPVOID)100500);
        Button b (10, 70,
                  135, 50,
                  L"А меня зовут Button",
                  WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | BS_MULTILINE,
                  0,
                  &app,
                  function_,
                  (LPVOID)&e);
        getch();
    }
    volatile bool running = true;

    Button exit (155, 70,
                 135, 50,
                 L"Press to Exit",
                 WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | BS_MULTILINE,
                 WS_EX_STATICEDGE,
                 &app,
                 function1_,
                 (LPVOID)&running);

    wprintf (L"Тест русских букв через wprintf\n");

    while (running);

    return 0;
}
