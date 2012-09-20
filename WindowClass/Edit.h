#ifndef EDIT_H_INCLUDED
#define EDIT_H_INCLUDED

LPCWSTR EditBoxClassName = L"EDIT";

class EditBox : public WindowObject
{
    public:
    INT x_, y_;
    INT width_, height_;
    LPCWSTR name_;
    ApplicationWindow* awpt_;
    HFONT font_;

    EditBox (INT x,
             INT y,
             INT width,
             INT height,
             LPCWSTR name,
             DWORD style,
             DWORD exStyle,
             ApplicationWindow* awpt,
             OBJECTFUNC f,
             LPVOID pt) :
        WindowObject (NULL, f, pt, EditBoxClassName),
        x_ (x),
        y_ (y),
        width_ (width),
        height_ (height),
        name_   (name),
        awpt_   (awpt),
        font_   (CreateFont (0,
                             ((double)width_ / double(wcslen(name))),
                              0, 0, 0, 0, 0, 0,
                              RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              DEFAULT_QUALITY, FIXED_PITCH,
                              _TEXT_("Lucida Console")))
    {
        CreateRequest cr (exStyle,
                          name,
                          style,
                          (POINT){x_, y_},
                          (SIZE){width_, height_},
                          (WindowObject*)this);
        awpt_->__SetRequest(cr);
        SendMessage (HWND(*awpt), WM_NULL, 0, 0);
        SendMessage(WindowObject::handle_,WM_SETFONT,(WPARAM)font_,0);
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
    }
};

#endif // EDIT_H_INCLUDED
