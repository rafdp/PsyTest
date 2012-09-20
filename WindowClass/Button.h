#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

LPCWSTR ButtonClassName = L"BUTTON";

class Button : public WindowObject
{
    public:
    INT x_, y_;
    INT width_, height_;
    LPCWSTR name_;
    ApplicationWindow* awpt_;

    Button (INT x,
            INT y,
            INT width,
            INT height,
            LPCWSTR name,
            DWORD style,
            DWORD exStyle,
            ApplicationWindow* awpt,
            OBJECTFUNC f,
            LPVOID pt) :
        WindowObject (NULL, f, pt, ButtonClassName),
        x_ (x),
        y_ (y),
        width_ (width),
        height_ (height),
        name_   (name),
        awpt_   (awpt)
    {
        CreateRequest cr (exStyle,
                          name,
                          style,
                          (POINT){x_, y_},
                          (SIZE){width_, height_},
                          (WindowObject*)this);
        awpt_->__SetRequest(cr);
        SendMessage (HWND(*awpt), WM_NULL, 0, 0);
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
    }
};

#endif // BUTTON_H_INCLUDED
