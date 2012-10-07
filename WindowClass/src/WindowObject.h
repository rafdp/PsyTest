#ifndef WINDOWOBJECT_H_INCLUDED
#define WINDOWOBJECT_H_INCLUDED

class ControlInformation
{
public:
    virtual bool Activated () = 0;
};

class WindowObject
{
    public:
    const int HEADER;
    HWND handle_;
    INT code_;
    OBJECTFUNC func_;
    LPVOID pt_;
    LPCWSTR className_;
    void (*initFunc_) (void*);

    WindowObject (HWND handle,
                  OBJECTFUNC func,
                  void* pt,
                  LPCWSTR className,
                  void (*initFunc) (void*),
                  const int header) :
        HEADER     (header),
        handle_    (handle),
        code_      (0),
        func_      (func),
        pt_        (pt),
        className_ (className),
        initFunc_  (initFunc)
    {}

    virtual ~WindowObject ()
    {
        handle_ = NULL;
        func_ = NULL;
        pt_ = NULL;
        code_ = 0;
        initFunc_ = 0;
    }

};

struct CreateRequest
{
    boolean active_;
    DWORD exStyle_;
    LPCWSTR menuName_;
    DWORD style_;
    POINT pos_;
    SIZE size_;
    WindowObject* wopt_;
    void* objectPt_;

    CreateRequest () :
        active_    (FALSE),
        exStyle_   (0),
        menuName_  (NULL),
        style_     (0),
        pos_       (),
        size_      (),
        wopt_      (NULL)
    {

    }

    CreateRequest (DWORD exStyle,
                   LPCWSTR menuName,
                   DWORD style,
                   POINT pos,
                   SIZE size,
                   WindowObject* wopt,
                   void* objPt) :
        active_    (TRUE),
        exStyle_   (exStyle),
        menuName_  (menuName),
        style_     (style),
        pos_       (pos),
        size_      (size),
        wopt_      (wopt),
        objectPt_  (objPt)
    {}

    VOID Flush ()
    {
        active_    = FALSE;
        exStyle_   = 0;
        menuName_  = NULL;
        style_     = 0;
        pos_       = {};
        size_      = {};
        wopt_      = NULL;
    }
};


#endif // WINDOWOBJECT_H_INCLUDED
