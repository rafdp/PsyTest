
#include "RadioButtonSystem.h"

RadioButtonSystem::RadioButtonSystem () :
    buttons_  (NULL),
    buttonsN_ (0),
    current_ (-1)
{}

RadioButtonSystem::RadioButtonSystem (Button** array, size_t num) :
    buttons_  (array),
    buttonsN_ (num),
    current_ (-1)
{}

void RadioButtonSystem::Set (Button** array, size_t num)
{
    if (buttons_ == NULL) buttons_ = array;
    if (buttonsN_ == 0) buttonsN_ = num;
    current_ = -1;
}

void RadioButtonSystem::SetCurrent (Button* pt)
{
    for (int i = 0; i < buttonsN_; i ++)
    {
        if (pt == buttons_[i])
        {
            if (current_ != -1 && current_ < buttonsN_)
                SendMessage (HWND (*buttons_[current_]),
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
    for (int i = 0; i < buttonsN_; i ++)
        delete buttons_[i];
    buttons_ = NULL;
    buttonsN_ = 0;
    current_ = -1;
}

void RBSManagementCallback (void* obj, void* pt, WPARAM, LPARAM)
{
    RadioButtonSystem* rbs = (LPRadioButtonSystem) pt;
    rbs->SetCurrent ((Button*)obj);
}
