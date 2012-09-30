
#define VECTOR_ITERATOR(vec, name) \
for (auto name = vec.begin(); name < vec.end(); name++)

Question::Question () :
    text_               (NULL),
    font_               (NULL),
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

    text_ = new wchar_t [len + 1];
    wcsncpy (text_, start, len + 1);
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

    font_ = new wchar_t [len + 1];
    wcsncpy (font_, start, len + 1);
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

template <class T>
void Question::StoreElement (T* element, void (*func) (void*))
{
    allocatedElements_.push_back (new WindowElement);
    allocatedElements_.back()->Set(element, func);
}

void Question::FreeStuff ()
{
    SecureArrayDelete (text_);
    SecureArrayDelete (font_);
    fontsize_ = 0;
    nLines_ = 0;
    VECTOR_ITERATOR (allocatedElements_, iter)
    {
        (*iter)->Delete();
        SecureElementDelete (*iter);
    }
    allocatedElements_.clear();
}

Question::~Question ()
{
    FreeStuff();
}


Page_Management::Page_Management (ApplicationWindow& window, LPCWSTR filename) :
    window_         (window),
    header_         (NULL),
    questions_      (),
    currentY_       (0)
{
    ParseFile (filename);
}

Page_Management::~Page_Management ()
{
    SecureElementDelete (header_);
    VECTOR_ITERATOR (questions_, iter)
        SecureElementDelete (*iter);
    questions_.clear();
}

bool Page_Management::CheckForCompletion ()
{
    static std::vector<RadioButtonSystem*> not_completed;
    static bool run = false;
    bool ok = true;
    if (!run)
    {
        VECTOR_ITERATOR (questions_, iter)
        {
            VECTOR_ITERATOR ((*iter)->allocatedElements_, iterButton)
            {
                //!if (*(int*)((WindowObject*)(*iterButton)->pt) != RBS_HEADER) continue;
                if (**(int**)(*iterButton) != RBS_HEADER) continue;
                else
                //!if (((RadioButtonSystem*)((WindowObject*)(*iterButton)->pt))->GetCurrent () == -1)
                if (((RadioButtonSystem*)(*(int*)(*iterButton)))->GetCurrent () == -1)
                {
                    not_completed.push_back ((RadioButtonSystem*)((WindowObject*)(*iterButton)->pt));
                    ok = false;
                }
            }
        }
    }
    else
    {
        VECTOR_ITERATOR (not_completed, iter)
        {
            if ((*iter)->GetCurrent () != -1) {not_completed.erase (iter); iter --;}
            else ok = false;
        }
    }
    run = true;
    return ok;
}
