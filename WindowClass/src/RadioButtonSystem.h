#ifndef RADIOBUTTONSYSTEM_H_INCLUDED
#define RADIOBUTTONSYSTEM_H_INCLUDED

void RBSManagementCallback (void*, void*, WPARAM, LPARAM);

class RadioButtonSystem : public ControlInformation
{
    private:
    const int HEADER;
    std::vector<Button*> buttons_;
    int current_;
    ApplicationWindow* awpt_;
    bool toCheck_;

    public:

    virtual bool Activated ();

    RadioButtonSystem (ApplicationWindow* awpt, bool toCheck = true);

    void AddButton (LPCWSTR font,
                    size_t size,
                    INT x,
                    INT y,
                    INT width,
                    INT height,
                    LPCWSTR name);

    void SetCurrent (Button* pt);

    ~RadioButtonSystem();

    int GetCurrent ();

    friend void RBSManagementCallback (void*, void*, WPARAM, LPARAM);
};

void DeleteRBS (ControlInformation* pt)
{
    ((RadioButtonSystem*)pt)->~RadioButtonSystem ();
}

typedef RadioButtonSystem* LPRadioButtonSystem;

#endif // RADIOBUTTONSYSTEM_H_INCLUDED
