#ifndef PAGE1_H_INCLUDED
#define PAGE1_H_INCLUDED

const int MAX_QUESTIONS = 10;

struct WindowElement
{
    void* pt;
    void (*deleteFunc) (void*);

    template <class T>
    void Set (T* pointer, void (*func) (void*))
    {
        pt = (void*)pointer;
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
    LPWSTR         text_;
    LPWSTR         font_;
    size_t         fontsize_;
    char           nLines_;
    std::vector<WindowElement*> allocatedElements_;

    Question ();

    Question& text (LPWSTR start, LPWSTR end);

    Question& font (LPWSTR start, LPWSTR end);

    Question& fontsize (LPWSTR start);

    Question& nLines (LPWSTR start);

    Question& currentlyAllocated ();

    template <class T>
    void StoreElement (T* element, void (*func) (void*));

    void FreeStuff ();

    ~Question ();
};

class Page_Management
{
    ApplicationWindow& window_;
    EditBox* header_;
    std::vector<Question*> questions_;
    int currentY_;

    std::vector<RadioButtonSystem*> not_completed_;
    bool check_run_;

    public:
    Page_Management (ApplicationWindow& window, LPCWSTR filename);
    ~Page_Management ();

    void ParseFile (LPCWSTR filename);

    bool CheckForCompletion ();
};


#endif // PAGE1_H_INCLUDED
