
HDC _CreateVirtualDC (int width, int height)
{
    HDC dc = NULL;
    HBITMAP bmap = {};
    BEGIN_DEBUG
    HDC dc_ = GetDC (NULL);
    dc = CreateCompatibleDC (dc_);
    if (dc) bmap = CreateCompatibleBitmap (dc_, width, height);
    if (dc && bmap) SelectObject (dc, bmap);
    END_DEBUG
    return dc;
}

VOID _DeleteDC (HDC dc)
{
    BEGIN_DEBUG
    DeleteObject (GetCurrentObject (dc, OBJ_BITMAP));
    DeleteDC (dc);
    END_DEBUG
}

struct PassToThread
{
    UINT style;
    RECT pos;
    LPCWSTR caption;
    ApplicationWindow* pointer;
};

VOID RegisterApplication ()
{
    BEGIN_DEBUG
    static boolean done = FALSE;
    if (!done)
    {
        WNDCLASSEXW wnd = {};

        wnd.cbSize           = sizeof (wnd);
        wnd.style            = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wnd.lpfnWndProc      = (WNDPROC)WindowProcedure;
        wnd.hIcon            = LoadIcon(NULL, IDI_APPLICATION);
        wnd.hCursor          = LoadCursor(NULL, IDC_ARROW);
        wnd.hbrBackground    = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
        wnd.lpszClassName    = APP_NAME_W;

        Verify (RegisterClassExW (&wnd));
        done = TRUE;
    }

    END_DEBUG
}

VOID WindowThread (LPVOID pt)
{
    BEGIN_DEBUG
    RegisterApplication ();

    PassToThread* data = (PassToThread*) pt;

    HWND handle = CreateWindowExW ( 0, APP_NAME_W, data->caption, data->style,
                                    data->pos.left,
                                    data->pos.top,
                                    data->pos.right - data->pos.left,
                                    data->pos.bottom - data->pos.top,
                                    NULL, NULL, NULL, data->pointer);
    verify_do (handle, return);
    Verify (data->pointer->__Show ());
    Verify (data->pointer->__Activate ());
    Verify (data->pointer->__Update ());

    data->pointer->__SetActive (TRUE);

    MSG msg = {0};
    while (GetMessage (&msg, NULL, 0, 0))
    {
        TranslateMessage (&msg);
        DispatchMessage  (&msg);
    }
    END_DEBUG
}

namespace on
{
    VOID Create(HWND wnd, LPARAM& createStr)
    {
        BEGIN_DEBUG
        ApplicationWindow * pt = (ApplicationWindow *) ((CREATESTRUCT*)createStr)->lpCreateParams;
        SetWindowLong (wnd, GWL_USERDATA, (LONG)pt);
        pt->__SetWindowHandle (wnd);

        verify_do (pt->__Initialize(), return);
        END_DEBUG
    }

    boolean Close(HWND wnd)
    {
        BEGIN_DEBUG
        ((ApplicationWindow*)GetWindowLong(wnd, GWL_USERDATA))->__Uninitialize();
        SendNotifyMessage (wnd, WM_DESTROY, 0, 100500);
        END_DEBUG
        return FALSE;
    }

    VOID Destroy(HWND wnd, LPARAM lparam)
    {
        BEGIN_DEBUG
        ApplicationWindow* pt = (ApplicationWindow*)GetWindowLong(wnd, GWL_USERDATA);
        if (lparam != 100500)
        {
            pt->__Uninitialize();
        }
        PostQuitMessage (WM_DESTROY);
        ExitProgram = true;
        END_DEBUG
    }

    VOID Paint(HWND wnd)
    {
        BEGIN_DEBUG
        ApplicationWindow* pt = (ApplicationWindow*)GetWindowLong(wnd, GWL_USERDATA);
        Verify (pt->__Paint());
        END_DEBUG
    }

    VOID Char (HWND wnd, WPARAM ch, LPARAM key)
    {
        BEGIN_DEBUG
        ApplicationWindow* pt = (ApplicationWindow*)GetWindowLong(wnd, GWL_USERDATA);
        Verify (pt->__OnCHAR(ch, key));
        END_DEBUG
    }

    VOID Size (HWND wnd, WPARAM param)
    {
        BEGIN_DEBUG
        ApplicationWindow* pt = (ApplicationWindow*)GetWindowLong(wnd, GWL_USERDATA);
        if (param == SIZE_MINIMIZED) return;
        Verify (pt->__UpdateSize ());
        Verify (pt->__Update ());
        END_DEBUG
    }
}

LRESULT CALLBACK WindowProcedure (HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    BEGIN_DEBUG
    if (msg == WM_NULL)
    {
        ApplicationWindow* pt = (ApplicationWindow*) GetWindowLong (wnd, GWL_USERDATA);
        if (pt && pt->req_.front().active_)
        {
            pt->__ActivateRequest();
        }
        return 0;
    }
    switch(msg)
    {
        case WM_CREATE:
            on::Create(wnd, lparam);
            break;

        case WM_CLOSE:
            return on::Close(wnd);
            break;

        case WM_DESTROY:
            on::Destroy(wnd, lparam);
            break;

        case WM_PAINT:
            on::Paint(wnd);
            break;

        case WM_CHAR:
            on::Char(wnd, wparam, lparam);
            break;
        case WM_COMMAND:
        {
            ApplicationWindow* pt = (ApplicationWindow*) GetWindowLong (wnd, GWL_USERDATA);
            ObjectFunctionCall fc = pt->addedObjects_[LOWORD(wparam)];
            if (fc.function) fc.function(fc.objectPt, fc.pt, wparam, lparam);
            break;
        }

        case WM_SIZE:
            on::Size(wnd, wparam);
            break;
        case WM_LBUTTONUP:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDOWN:
            SetFocus (wnd);
            break;
        default:
            break;

    }
    END_DEBUG
    return DefWindowProc(wnd, msg, wparam, lparam);
}

VOID AdjustLocale ()
{
    BEGIN_DEBUG
    SetConsoleCP       (1251);
    SetConsoleOutputCP (1251);
    setlocale (LC_CTYPE, "Russian");
    _wsetlocale (LC_CTYPE, L"Russian");

#if (WINVER > 0x0501)
    HANDLE out = GetStdHandle (STD_OUTPUT_HANDLE);

    CONSOLE_FONT_INFOEX info = { sizeof (info) };
    if (!GetCurrentConsoleFontEx (out, false, &info)) return;

    info.FontFamily = 0x36;                       // Unicode fixed-pitch
    if (!*info.FaceName) info.dwFontSize.Y += 2;  // Terminal font is too small
    wcsncpy (info.FaceName, L"Lucida Console", sizeof (info.FaceName) / sizeof (info.FaceName[0]));
    SetCurrentConsoleFontEx (out, false, &info);
#endif

    END_DEBUG
}

bool ShowConsole (bool mode)
{
    boolean ok = TRUE;
    BEGIN_DEBUG
    HWND console = GetConsoleWindow ();
    if (!console) ok = FALSE;
    if (ok) ShowWindow (console, mode ? SW_SHOW : SW_HIDE);
    END_DEBUG
    return ok;
}

template <class T>
bool SecureArrayDelete (T*& data)
{
    if (!data) return false;

    delete [] data;
    data = NULL;

    return true;
}

template <class T>
bool SecureElementDelete (T*& data)
{
    if (!data) return false;

    delete data;
    data = NULL;

    return true;
}

int OutputDebugPrintfA (LPCSTR format, ...)
{
    if (!format) return 0;

    char str[2048] = "";

    va_list arg; va_start (arg, format);
    int n = _vsnprintf (str, sizeof (str) - 1, format, arg);
    va_end (arg);

    OutputDebugStringA (str);
    return n;
}

int OutputDebugPrintfW (LPCWSTR format, ...)
{
    if (!format) return 0;

    wchar_t str[2048] = L"";

    va_list arg; va_start (arg, format);
    int n = _vsnwprintf (str, sizeof (str) - 1, format, arg);
    va_end (arg);

    OutputDebugStringW (str);
    return n;
}
