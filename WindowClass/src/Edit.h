#ifndef EDIT_H_INCLUDED
#define EDIT_H_INCLUDED

LPCWSTR EditBoxClassName = L"EDIT";

void InitEditBox (void* pt);

class EditBox : public WindowObject, public ControlInformation
{
    public:
    INT x_, y_;
    INT width_, height_;
    MemContainer<wchar_t> name_;
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
        name_   (name ? new wchar_t [wcslen(name) + 1] : NULL, MEMORY_ARRAY),
        awpt_   (awpt),
        font_   (CreateFontW (2*size, 0,
                              0, 0, 0, 0, 0, 0,
                              RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              ANTIALIASED_QUALITY, 0,
                              font))
    {
        if (name_.data) wcscpy (name_.data, name);
        CreateRequest cr (exStyle,
                          name_.data,
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
    }

    virtual bool Activated ()
    {
        return true;
    }
};

void InitEditBox (void* pt)
{
    SendMessage(((EditBox*)pt)->WindowObject::handle_, WM_SETFONT, (WPARAM)((EditBox*)pt)->font_, TRUE);
}

void DeleteEditBox (ControlInformation* pt)
{
    ((EditBox*)pt)->~EditBox();
}

#endif // EDIT_H_INCLUDED
