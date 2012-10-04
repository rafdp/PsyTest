#ifndef EDIT_H_INCLUDED
#define EDIT_H_INCLUDED

LPCWSTR EditBoxClassName = L"EDIT";

void InitEditBox (void* pt);

class EditBox : public WindowObject
{
    public:
    INT x_, y_;
    INT width_, height_;
    LPWSTR name_;
    ApplicationWindow* awpt_;
    HFONT font_;

    EditBox (LPCWSTR font,
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
        WindowObject (NULL, f, pt, EditBoxClassName, InitEditBox, EDITBOX_HEADER),
        x_ (x),
        y_ (y),
        width_ (width),
        height_ (height),
        name_   (name ? new wchar_t [wcslen(name) + 1] : NULL),
        awpt_   (awpt),
        font_   (CreateFontW (2*size, 0,
                              0, 0, 0, 0, 0, 0,
                              RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              ANTIALIASED_QUALITY, 0,
                              font))
    {
        if (name_) wcscpy (name_, name);
        CreateRequest cr (exStyle,
                          name_,
                          style,
                          (POINT){x_, y_},
                          (SIZE){width_, height_},
                          (WindowObject*)this, (void*)this);
        awpt_->__SetRequest(cr);
        SendMessage (HWND(*awpt), WM_NULL, 0, 0);
    }

    VOID GetText (LPWSTR str, SIZE_T s)
    {
        SendMessage (WindowObject::handle_, WM_GETTEXT, (WPARAM)(s), (LPARAM)(str));
    }

    ~EditBox ()
    {
        SendMessage (WindowObject::handle_, WM_CLOSE, 0, 0);
        DeleteObject (font_);
        awpt_->__RemoveObject (WindowObject::code_);
        x_ = 0;
        y_ = 0;
        width_ = 0;
        height_ = 0;
        name_ = NULL;
        SecureArrayDelete (name_);
    }
};

void DeleteEditBox (void* pt)
{
    ((EditBox*)pt)->~EditBox();
}

void InitEditBox (void* pt)
{
    SendMessage(((EditBox*)pt)->WindowObject::handle_, WM_SETFONT, (WPARAM)((EditBox*)pt)->font_, TRUE);
}

#endif // EDIT_H_INCLUDED
