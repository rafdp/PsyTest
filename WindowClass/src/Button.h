#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

LPCWSTR ButtonClassName = L"BUTTON";

void InitButton (void* pt);

class Button : public WindowObject, public ControlInformation
{
    public:
    INT x_, y_;
    INT width_, height_;
    MemContainer<wchar_t> name_
    ApplicationWindow* awpt_;
    CreateRequest req_;
    HFONT font_;

    Button (LPCWSTR font,
            size_t size,
            INT x,
            INT y,
            INT width,
            INT height,
            LPCWSTR name,
            DWORD style,
            DWORD exStyle,
            ApplicationWindow* awpt,
            OBJECTFUNC f,
            LPVOID pt) :
        WindowObject (NULL, f, pt, ButtonClassName, InitButton, BUTTON_HEADER),
        x_      (x),
        y_      (y),
        width_  (width),
        height_ (height),
        name_   (new wchar_t [wcslen(name) + 1],
                 MEMORY_ARRAY),
        awpt_   (awpt),
        req_    (exStyle,
                 name_,
                 style,
                 (POINT){x_, y_},
                 (SIZE){width_, height_},
                 (WindowObject*)this, (void*)this),
        font_   (CreateFontW (2*size, 0,
                              0, 0, 0, 0, 0, 0,
                              RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              ANTIALIASED_QUALITY, 0,
                              font))
    {
        wcscpy (name_, name);
        awpt_->__SetRequest(req_);
        SendMessage (HWND(*awpt), WM_NULL, 0, 0);
    }

    operator HWND ()
    {
        return WindowObject::handle_;
    }

    ~Button ()
    {
        SendMessage (WindowObject::handle_, WM_CLOSE, 0, 0);
        awpt_->__RemoveObject (WindowObject::code_);
        x_ = 0;
        y_ = 0;
        width_ = 0;
        height_ = 0;
        name_ = NULL;
        SafeArrayDelete (name_);
    }

    virtual bool Activated ()
    {
        return true;
    }
};

void InitButton (void* pt)
{
    SendMessage(((Button*)pt)->WindowObject::handle_, WM_SETFONT, (WPARAM)((Button*)pt)->font_, TRUE);
}

void DeleteButton (ControlInformation* pt)
{
    ((Button*)pt)->~Button();
}

#endif // BUTTON_H_INCLUDED
