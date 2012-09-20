
///-----------------------------------------------------------------------
/// Constructor
//{
ApplicationWindow::ApplicationWindow (INT x,
                                      INT y,
                                      INT width,
                                      INT height,
                                      DWORD style,
                                      LPCWSTR caption,
                                      boolean inputToConsole,
                                      boolean hideConsole) :
    handle_         (),
    dc_             (),
    paint_          (),
    buffer_         (),
    size_           (),
    running_        (),
    inputToConsole_ (inputToConsole),
    req_            (),
    addObjectCode_  (StartObjectCode),
    addedObjects_   ()
{
    BEGIN_DEBUG
    RECT r   = { 0, 0, width, height };
    AdjustWindowRect (&r, style, FALSE);
    SIZE sz  = { r.right - r.left, r.bottom - r.top };

    r = {x, y, x + sz.cx, y + sz.cy};
    PassToThread ptt = {style, r, caption, this};
    verify_do (_beginthread  (WindowThread, 0, &ptt), return);
    while (!running_);
    if (hideConsole) ShowWindow (GetConsoleWindow (), SW_HIDE);
    __Paint ();
    END_DEBUG
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// Destructor
//{
ApplicationWindow::~ApplicationWindow ()
{
    BEGIN_DEBUG
    verify_do(running_, return);
    EndPaint(handle_, &paint_);
    if (handle_) SendNotifyMessage (handle_, WM_DESTROY, 0, 100500);
    handle_ = NULL;
    _BufferAutoLock lock;
    if (buffer_) _DeleteDC (buffer_);
    dc_ = NULL;
    buffer_ = NULL;
    size_ = (SIZE){0};
    running_ = FALSE;
    END_DEBUG
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// Initialize
//{
boolean ApplicationWindow::__Initialize ()
{
    boolean ok = TRUE;
    BEGIN_DEBUG
    AdjustLocale();
    _BufferAutoLock lock;
    dc_     = BeginPaint (handle_, &paint_);
    buffer_ = _CreateVirtualDC (ScreenSize.x, ScreenSize.y);
    verify_do (buffer_, ok = FALSE);
    if (ok) verify_do (dc_, ok = FALSE);
    if (ok) verify_do (PatBlt (buffer_, 0, 0, ScreenSize.x, ScreenSize.y, WHITENESS), ok = false);
    END_DEBUG
    return ok;
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// Uninitialize
//{
VOID ApplicationWindow::__Uninitialize ()
{
    BEGIN_DEBUG
    verify_do(running_, return);
    EndPaint(handle_, &paint_);
    handle_ = NULL;
    _BufferAutoLock lock;
    if (buffer_) _DeleteDC (buffer_);
    dc_ = NULL;
    buffer_ = NULL;
    size_ = {};
    running_ = FALSE;
    END_DEBUG
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// Show
//{
boolean ApplicationWindow::__Show ()
{
    boolean ok = TRUE;
    BEGIN_DEBUG
    verify_do (handle_, ok = FALSE);
    if (ok) ShowWindow (handle_, SW_SHOWNORMAL);
    END_DEBUG
    return ok;
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// Activate
//{
boolean ApplicationWindow::__Activate ()
{
    boolean ok = TRUE;
    BEGIN_DEBUG
    verify_do (handle_, ok = FALSE);
    if (ok) verify_do (SetForegroundWindow (handle_), ok = FALSE);
    END_DEBUG
    return ok;
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// Update
//{
boolean ApplicationWindow::__Update ()
{
    boolean ok = TRUE;
    BEGIN_DEBUG
    verify_do (handle_, ok = FALSE);
    if (ok) verify_do (UpdateWindow (handle_), ok = FALSE);
    END_DEBUG
    return ok;
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// SetActive
//{
VOID ApplicationWindow::__SetActive (boolean mode)
{
    BEGIN_DEBUG
    running_ = mode;
    END_DEBUG
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// SetWindowHandle
//{
VOID ApplicationWindow::__SetWindowHandle (HWND handle)
{
    BEGIN_DEBUG
    handle_ = handle;
    END_DEBUG
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// Paint
//{
boolean ApplicationWindow::__Paint ()
{
    boolean ok = TRUE;
    BEGIN_DEBUG
    _BufferAutoLock lock;
    verify_do (handle_, ok = FALSE);
    if (ok) verify_do (buffer_, ok = FALSE);
    if (ok) verify_do (dc_, ok = FALSE);

    if (ok)
    {
        BitBlt (dc_,
                0,
                0,
                size_.cx,
                size_.cy,
                buffer_,
                0,
                0,
                SRCCOPY);
    }
    if (ok)
    {
        for (auto iter = addedObjects_.begin(); iter != addedObjects_.end(); iter++)
        {
            //if (iter->second.handle) SendNotifyMessage (iter->second.handle, WM_DRAWITEM, 0, 0);
        }
    }
    END_DEBUG
    return ok;
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// UpdateSize
//{
boolean ApplicationWindow::__UpdateSize ()
{
    boolean ok = TRUE;
    BEGIN_DEBUG
    verify_do (handle_, (ok = FALSE, size_ = {}));
    RECT r = {};
    if (ok) verify_do (GetClientRect (handle_, &r), (ok = FALSE, size_ = {}));
    if (ok) size_ = (SIZE) {r.right - r.left, r.bottom - r.top};
    END_DEBUG
    return ok;
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// OnCHAR
//{
boolean ApplicationWindow::__OnCHAR (WPARAM ch, LPARAM key)
{
    boolean ok = TRUE;
    BEGIN_DEBUG
    verify_do (handle_, ok = FALSE);
    if (ok && inputToConsole_)
    {
        INPUT_RECORD evt = {0};

        evt.EventType                        = KEY_EVENT;
        evt.Event.KeyEvent.bKeyDown          = TRUE;
        evt.Event.KeyEvent.wRepeatCount      = 1;
        evt.Event.KeyEvent.uChar.AsciiChar   = (BYTE) (ch);
        evt.Event.KeyEvent.wVirtualScanCode  = (BYTE) (key >> 16);
        evt.Event.KeyEvent.wVirtualKeyCode   = (SHORT) MapVirtualKey (evt.Event.KeyEvent.wVirtualKeyCode, 3);
        evt.Event.KeyEvent.dwControlKeyState = 0;

        DWORD written = 0;
        WriteConsoleInput (GetStdHandle (STD_INPUT_HANDLE), &evt, 1, &written);
    }
    END_DEBUG
    return ok;
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// operator HDC
//{
ApplicationWindow::operator HDC ()
{
    BEGIN_DEBUG
    END_DEBUG
    return buffer_;
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// operator HDC
//{
ApplicationWindow::operator HWND ()
{
    BEGIN_DEBUG
    END_DEBUG
    return handle_;
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// Update
//{
bool ApplicationWindow::Update ()
{
    BEGIN_DEBUG
    END_DEBUG
    return __Paint();
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// SetRequest
//{
VOID ApplicationWindow::__SetRequest (const CreateRequest& r)
{
    BEGIN_DEBUG
    req_ = r;
    END_DEBUG
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// RemoveRequest
//{
VOID ApplicationWindow::__RemoveRequest ()
{
    BEGIN_DEBUG
    req_.Flush();
    END_DEBUG
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// ActivateRequest
//{
VOID ApplicationWindow::__ActivateRequest ()
{
    BEGIN_DEBUG
    req_.wopt_->handle_ = CreateWindowExW (req_.exStyle_,
                                           req_.wopt_->className_,
                                           req_.menuName_,
                                           req_.style_,
                                           req_.pos_.x,
                                           req_.pos_.y,
                                           req_.size_.cx,
                                           req_.size_.cy,
                                           handle_,
                                           (HMENU)addObjectCode_,
                                           NULL,
                                           NULL);
    req_.wopt_->code_ = addObjectCode_;
    ObjectFunctionCall fc = {req_.wopt_->func_, req_.wopt_->pt_, req_.wopt_->handle_ };
    addedObjects_[addObjectCode_] = fc;
    addObjectCode_++;

    END_DEBUG
}
//}
///-----------------------------------------------------------------------

///-----------------------------------------------------------------------
/// RemoveObject
//{
VOID ApplicationWindow::__RemoveObject (INT code)
{
    BEGIN_DEBUG
    ObjectFunctionCall fc = {NULL, NULL, NULL};
    addedObjects_[code] = fc;
    END_DEBUG
}
//}
///-----------------------------------------------------------------------
