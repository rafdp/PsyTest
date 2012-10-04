
#include "RadioButtonSystem.h"

RadioButtonSystem::RadioButtonSystem (ApplicationWindow* awpt) :
    HEADER (RBS_HEADER),
    buttons_  (),
    current_ (-1),
    awpt_    (awpt)
{}

void RadioButtonSystem::AddButton (LPCWSTR font,
                                   size_t size,
                                   INT x,
                                   INT y,
                                   INT width,
                                   INT height,
                                   LPCWSTR name)
{
    Button* pointer = new Button (font,
                                  size,
                                  x, y,
                                  width, height,
                                  name,
                                  WS_TABSTOP |
                                  WS_VISIBLE |
                                  WS_CHILD |
                                  BS_RADIOBUTTON |
                                  BS_MULTILINE,
                                  0,
                                  awpt_,
                                  RBSManagementCallback, this);
    buttons_.push_back (pointer);
}

void RadioButtonSystem::SetCurrent (Button* pt)
{
    for (size_t i = 0; i < buttons_.size(); i ++)
    {
        if (pt == buttons_.at(i))
        {
            if (current_ != -1 && size_t (current_) < buttons_.size())
                SendMessage (HWND (*buttons_.at(current_)),
                             BM_SETCHECK,
                             BST_UNCHECKED,
                             0);

            SendMessage (HWND (*pt),
                         BM_SETCHECK,
                         BST_CHECKED,
                         0);
            current_ = i;
            break;
        }
    }
}

RadioButtonSystem::~RadioButtonSystem()
{
    for (size_t i = 0; i < buttons_.size(); i ++)
        SecureElementDelete(buttons_[i]);
    buttons_.clear();
    current_ = -1;
}

int RadioButtonSystem::GetCurrent ()
{
    return current_;
}

void RBSManagementCallback (void* obj, void* pt, WPARAM, LPARAM)
{
    RadioButtonSystem* rbs = (LPRadioButtonSystem) pt;
    rbs->SetCurrent ((Button*)obj);
}
