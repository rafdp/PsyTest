
#define VECTOR_ITERATOR(vec, name) \
for (auto name = vec.begin(); name < vec.end(); name++)

Question::Question () :
    text_               (),
    font_               (),
    fontsize_           (0),
    nLines_             (1),
    allocatedElements_  ()
{}

Question& Question::text (LPWSTR start, LPWSTR end)
{
    Verify (start);
    int len = 0;
    if (end == NULL)
         len = wcslen (start);
    else len = end - start;
    Verify (len > 0);

    text_.Set (new wchar_t [len + 1], MEMORY_ARRAY);
    wcsncpy (text_.data, start, len + 1);
    return *this;
}

Question& Question::font (LPWSTR start, LPWSTR end)
{
    Verify (start);
    int len = 0;
    if (end == NULL)
         len = wcslen (start);
    else len = end - start;
    Verify (len > 0);

    font_.Set(new wchar_t [len + 1], MEMORY_ARRAY);
    wcsncpy (font_.data, start, len + 1);
    return *this;
}

Question& Question::fontsize (LPWSTR start)
{
    Verify (start);
    fontsize_ = _wtoi (start);
    return *this;
}

Question& Question::nLines (LPWSTR start)
{
    Verify (start);
    nLines_ = _wtoi (start);
    return *this;
}

void Question::StoreElement (ControlInformation* element,
                             void (*func) (ControlInformation*))
{
    allocatedElements_.push_back (new UnreferencedControlHolder);
    allocatedElements_.back()->Set(element, func);
}

void Question::FreeStuff ()
{
    fontsize_ = 0;
    nLines_ = 0;
    VECTOR_ITERATOR (allocatedElements_, iter)
    {
        (*iter)->Delete();
        SafeElementDelete (*iter);
    }
    allocatedElements_.clear();
}

Question::~Question ()
{
    FreeStuff();
}

//!}------------------------------------------------------------

Page_Management::Page_Management (ApplicationWindow& window) :
    window_         (window),
    header_         (),
    questions_      (),
    currentY_       (0),
    not_completed_  (),
    check_run_      ()
{}

Page_Management::~Page_Management ()
{
    VECTOR_ITERATOR (questions_, iter)
        SafeElementDelete (*iter);
    questions_.clear();
}

bool Page_Management::CheckForCompletion ()
{
    bool ok = true;
    if (!check_run_)
    {
        VECTOR_ITERATOR (questions_, iter)
        {
            VECTOR_ITERATOR ((*iter)->allocatedElements_, iterButton)
            {
                if (!((*iterButton)->pt->Activated ()))
                {
                    not_completed_.push_back ((RadioButtonSystem*)((void*)(*iterButton)->pt));
                    ok = false;
                }
            }
        }
        check_run_ = true;
    }
    else
    {
        VECTOR_ITERATOR (not_completed_, iter)
        {
            if ((*iter)->GetCurrent () != -1) {not_completed_.erase (iter); iter --;}
            else ok = false;
        }
    }
    return ok;
}
