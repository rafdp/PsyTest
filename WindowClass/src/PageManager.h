#ifndef PAGE1_H_INCLUDED
#define PAGE1_H_INCLUDED

struct UnreferencedControlHolder
{
    ControlInformation* pt;
    void (*deleteFunc) (ControlInformation*);

    void Set (ControlInformation* pointer, void (*func) (ControlInformation*))
    {
        pt = pointer;
        deleteFunc = func;
    }

    void Delete ()
    {
        if (deleteFunc && pt) deleteFunc (pt);
    }
};

class Question
{
    public:
    MemContainer<wchar_t> text_;
    MemContainer<wchar_t> font_;
    size_t                fontsize_;
    char                  nLines_;
    std::vector<UnreferencedControlHolder*> allocatedElements_;

    Question ();

    Question& text (LPWSTR start, LPWSTR end);

    Question& font (LPWSTR start, LPWSTR end);

    Question& fontsize (LPWSTR start);

    Question& nLines (LPWSTR start);

    Question& currentlyAllocated ();

    void StoreElement (ControlInformation* element,
                       void (*func) (ControlInformation*));

    void FreeStuff ();

    ~Question ();
};

class Page_Management
{
    ApplicationWindow& window_;
    MemContainer<EditBox> header_;
    std::vector<Question*> questions_;
    int currentY_;

    std::vector<RadioButtonSystem*> not_completed_;
    bool check_run_;

    public:
    Page_Management (ApplicationWindow& window);
    ~Page_Management ();

    bool ParseFile (LPCWSTR filename);

    bool CheckForCompletion ();
};


#endif // PAGE1_H_INCLUDED
