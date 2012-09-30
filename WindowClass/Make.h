#ifdef __TXLIB_H
#error TXLib will cause errors with Unicode
#endif

#ifndef MAKE_H_INCLUDED
#define MAKE_H_INCLUDED

//!/////////////////////////////////////////
//! Defines
//{
#ifndef _WINDOWS_H
#define UNICODE
#endif

#undef __STRICT_ANSI__

#if   defined (DEBUG_3)
//{
#define BEGIN_DEBUG \
        printf ("%s started. Thread %u\n", \
                __PRETTY_FUNCTION__, \
                (unsigned int)GetCurrentThreadId ()); \
                getch(); \
        try {

#define END_DEBUG \
        printf ("%s stopped. Thread %u\n", \
                __PRETTY_FUNCTION__, \
                (unsigned int)GetCurrentThreadId ()); \
                getch(); \
        } \
        catch (...) \
        { \
            printf ("Exception occured:\n function \"%s\"\n file \"%s\"\n line %d\n", __PRETTY_FUNCTION__, __FILE__, __LINE__);\
            abort ();\
        }
#ifdef NDEBUG
#define verify_do(expr, do) \
{ \
    if (!(expr)) \
    { \
        TCHAR message[MAX_MESSAGE] = ""; \
        snprintf (message, MAX_MESSAGE, "%s:: \"%s\" returned bad result", __PRETTY_FUNCTION__, #expr); \
        MessageBoxA (NULL, \
                    message, \
                    APP_NAME_A, \
                    MB_OK | MB_ICONSTOP); \
        abort(); \
    } \
}
#else
#define verify_do(expr, do) assert(expr)
#endif

//}
#elif defined (DEBUG_2)
//{
#define BEGIN_DEBUG \
        printf ("%s started. Thread %u\n", \
                __PRETTY_FUNCTION__, \
                (unsigned int)GetCurrentThreadId ()); \
        try {

#define END_DEBUG \
        printf ("%s stopped. Thread %u\n", \
                __PRETTY_FUNCTION__, \
                (unsigned int)GetCurrentThreadId ()); \
        } \
        catch (...) \
        {\
            printf ("Exception occured:\n function \"%s\"\n file \"%s\"\n line %d\n", __PRETTY_FUNCTION__, __FILE__, __LINE__);\
            abort ();\
        }

#define verify_do(expr, do) \
{ \
    if (!(expr)) \
    { \
        CHAR message[MAX_MESSAGE] = ""; \
        snprintf (message, MAX_MESSAGE, "%s:: \"%s\" returned bad result", __PRETTY_FUNCTION__, #expr); \
        MessageBoxA (NULL, \
                     message, \
                     APP_NAME_A, \
                     MB_OK | MB_ICONSTOP); \
        do; \
    } \
}
//}
#elif defined (DEBUG_1)
//{
#define BEGIN_DEBUG \
        try {

#define END_DEBUG \
        } \
        catch (...) \
        { \
            printf ("Exception occured:\n function \"%s\"\n file \"%s\"\n line %d\n", __FUNCTION__, __FILE__, __LINE__);\
            abort ();\
        }

#define verify_do(expr, do) \
{ \
    if (!(expr)) \
    { \
        CHAR message[MAX_MESSAGE] = ""; \
        snprintf (message, MAX_MESSAGE, "%s:: \"%s\" returned bad result", __PRETTY_FUNCTION__, #expr); \
        MessageBoxA (NULL, \
                     message, \
                     APP_NAME_A, \
                     MB_OK | MB_ICONSTOP); \
        do; \
    } \
}

//}
#else
//{
#define BEGIN_DEBUG ;
#define END_DEBUG ;
#define verify_do(expr, do) if (!(expr)) do
//}
#endif

#define Verify(expr) \
{ \
    if (!(expr)) \
    { \
        CHAR message[MAX_MESSAGE] = ""; \
        snprintf (message, MAX_MESSAGE, "%s:: \"%s\" returned bad result", __PRETTY_FUNCTION__, #expr); \
        MessageBoxA (NULL, \
                     message, \
                     APP_NAME_A, \
                     MB_OK | MB_ICONSTOP); \
        abort(); \
    } \
}

#ifdef UNICODE
#define _TEXT_(q) L##q
#define _printf_ wprintf
#else
#define _TEXT_(q) q
#define _printf_ printf
#endif

//}
//!/////////////////////////////////////////

//!/////////////////////////////////////////
//! Includes
//{
#ifdef WINVER
#undef WINVER
#endif

#define   WINVER            0x0600

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <process.h>  //! thread stuff
#include <conio.h>
#include <string.h>
#include <assert.h>
#include <direct.h>   //! mkdir
#include <stdint.h>   //! int types
#include <map>
#include <vector>
#include <locale.h>
#include <string.h>
#include <time.h>
#include <queue>
#include <wctype.h>
#include "LibImport.h"
//}
//!/////////////////////////////////////////


//!/////////////////////////////////////////
//! Constants, typedefs
//{
typedef const int CINT;
typedef void (*OBJECTFUNC) (void*, void*, WPARAM, LPARAM);
const DWORD STYLE_RESIZABLE = WS_OVERLAPPEDWINDOW;
const DWORD STYLE_CONST     = WS_MINIMIZEBOX | WS_BORDER | WS_CAPTION | WS_SYSMENU;
LPCSTR APP_NAME_A = "__window__";
LPCWSTR APP_NAME_W = L"__window__";
CONST POINT ScreenSize = {GetSystemMetrics (SM_CXSCREEN), GetSystemMetrics (SM_CYSCREEN)};
CINT MAX_MESSAGE = 200;

const int  BUTTON_HEADER = 0x123456;
const int EDITBOX_HEADER = 0x234561;
const int     RBS_HEADER = 0x345612;
volatile bool ExitProgram = false;
//}
//!/////////////////////////////////////////

//!/////////////////////////////////////////
//! Prototypes
//{
class ApplicationWindow;
struct PassToThread;
struct CreateRequest;
VOID RegisterApplication();
VOID WindowThread (LPVOID pt);
LRESULT CALLBACK WindowProcedure (HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
HDC CreateVirtualDC (int width, int height);
VOID _DeleteDC (HDC dc);
VOID AdjustLocale();
namespace on
{
    VOID Create(HWND wnd, LPARAM& createStr);
    boolean Close(HWND wnd);
    VOID Destroy(HWND wnd, LPARAM lparam);
    VOID Paint(HWND wnd);
    VOID Char (HWND wnd, WPARAM ch, LPARAM key);
    VOID Size (HWND wnd, WPARAM param);
}
//}
//!/////////////////////////////////////////

//!/////////////////////////////////////////
//! Local includes
//{
#include "WindowObject.h"
#include "ApplicationWindow.h"
#include "Win32.cpp"
#include "ApplicationWindow.cpp"
#include "Button.h"
#include "RadioButtonSystem.cpp"
#include "Edit.h"
#include "InterfaceBuilder.h"
//}
//!/////////////////////////////////////////
#endif // MAKE_H_INCLUDED
