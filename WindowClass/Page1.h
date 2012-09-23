#ifndef PAGE1_H_INCLUDED
#define PAGE1_H_INCLUDED

class Question1
{

};

class Page1_Management
{
    public:
    ApplicationWindow& window_;
    Question* questions_;
    size_t questionsN_;

    Page1_Management (ApplicationWindow& window);
    ~Page1_Management ();
};


#endif // PAGE1_H_INCLUDED
