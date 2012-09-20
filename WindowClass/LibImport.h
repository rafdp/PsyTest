
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
