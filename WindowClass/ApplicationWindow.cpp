
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
                                      boolean registerChild) :
    handle_         (),
    dc_             (),
    paint_          (),
    buffer_         (),
    size_           (),
    running_        (),
    inputToConsole_ (inputToConsole),
    addObjectCode_  (StartObjectCode),
    registerChild_  (registerChild),
    req_            (),
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
    if (!running_) return;
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
    if (!running_) return;
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
/// operator HWND
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
    req_.push (r);
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
    while (!req_.empty() && registerChild_)
    {
        CreateRequest* reqPt = &(req_.front());
        reqPt->wopt_->handle_ = CreateWindowExW (reqPt->exStyle_,
                                                 reqPt->wopt_->className_,
                                                 reqPt->menuName_,
                                                 reqPt->style_,
                                                 reqPt->pos_.x,
                                                 reqPt->pos_.y,
                                                 reqPt->size_.cx,
                                                 reqPt->size_.cy,
                                                 handle_,
                                                 (HMENU)addObjectCode_,
                                                 NULL,
                                                 NULL);
        reqPt->wopt_->code_ = addObjectCode_;
        if (reqPt->wopt_->initFunc_) reqPt->wopt_->initFunc_ (reqPt->objectPt_);
        ObjectFunctionCall fc = {reqPt->wopt_->func_, reqPt->wopt_->pt_, reqPt->wopt_->handle_, reqPt->objectPt_};
        addedObjects_[addObjectCode_] = fc;
        addObjectCode_++;
        req_.pop();
    }
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

///-----------------------------------------------------------------------
/// RegisterObjects
//{
bool ApplicationWindow::RegisterObjects (bool reg)
{
    BEGIN_DEBUG
    registerChild_ = reg;
    if (registerChild_) SendMessage (handle_, WM_NULL, 0, 0);
    END_DEBUG
    return reg;
}
//}
///-----------------------------------------------------------------------

