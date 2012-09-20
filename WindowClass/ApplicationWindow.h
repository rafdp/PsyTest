#ifndef CLASS_H_INCLUDED
#define CLASS_H_INCLUDED

const int StartObjectCode = 101;

struct ObjectFunctionCall
{
    OBJECTFUNC function;
    void* pt;
    HWND handle;
};

class ApplicationWindow
{
    private:
    HWND             handle_;
    HDC              dc_;
    PAINTSTRUCT      paint_;
    HDC              buffer_;
    SIZE             size_;
    volatile boolean running_;
    boolean          inputToConsole_;
    CreateRequest    req_;
    INT              addObjectCode_;

    std::map<INT, ObjectFunctionCall> addedObjects_;

    boolean __Initialize ();
    VOID    __Uninitialize ();
    boolean __Show ();
    boolean __Activate ();
    boolean __Update ();
    VOID    __SetActive (boolean mode);
    VOID    __SetWindowHandle (HWND handle);
    boolean __Paint ();
    boolean __UpdateSize ();
    boolean __OnCHAR (WPARAM ch, LPARAM key);
    VOID    __SetRequest (CONST CreateRequest& r);
    VOID    __RemoveRequest ();
    VOID    __ActivateRequest ();
    VOID    __RemoveObject (INT code);

    friend VOID WindowThread (LPVOID);
    friend LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
    friend VOID on::Create(HWND wnd, LPARAM& createStr);
    friend boolean on::Close(HWND wnd);
    friend VOID on::Destroy(HWND wnd, LPARAM lparam);
    friend VOID on::Paint(HWND wnd);
    friend VOID on::Char (HWND wnd, WPARAM ch, LPARAM key);
    friend VOID on::Size (HWND wnd, WPARAM param);

    friend class Button;
    friend class EditBox;

    public:
    ApplicationWindow (INT     x,
                       INT     y,
                       INT     width,
                       INT     height,
                       DWORD   style,
                       LPCWSTR caption,
                       boolean inputToConsole,
                       boolean hideConsole);
    ~ApplicationWindow ();

    operator HDC ();
    operator HWND ();
    bool Update ();
};

class _BufferAutoLock
{
public:
    static boolean locked_;
    static UINT blocking_thread_;

    _BufferAutoLock ()
    {
        UINT thread = GetCurrentThreadId ();
        if (locked_ == FALSE || blocking_thread_ == thread)
        {
            locked_ = TRUE;
            blocking_thread_ = thread;
        }
        else
        {
            while (TRUE) if (!locked_) break;
            locked_ = TRUE;
            blocking_thread_ = thread;
        }
    }

    ~_BufferAutoLock ()
    {
        if (blocking_thread_ == GetCurrentThreadId ())
        {
            locked_ = FALSE;
            blocking_thread_ = 0;
        }
    }
};

boolean _BufferAutoLock::locked_ = FALSE;
UINT _BufferAutoLock::blocking_thread_ = 0;


#endif // CLASS_H_INCLUDED
