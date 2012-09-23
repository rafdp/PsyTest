#ifndef RADIOBUTTONSYSTEM_H_INCLUDED
#define RADIOBUTTONSYSTEM_H_INCLUDED

void RBSManagementCallback (void*, void*, WPARAM, LPARAM);

class RadioButtonSystem
{
    private:
    Button** buttons_;
    size_t buttonsN_;
    int current_;

    public:
    RadioButtonSystem ();

    RadioButtonSystem (Button** array, size_t num);

    void Set (Button** array, size_t num);

    void SetCurrent (Button* pt);

    ~RadioButtonSystem();

    friend void RBSManagementCallback (void*, void*, WPARAM, LPARAM);
};

typedef RadioButtonSystem* LPRadioButtonSystem;

#endif // RADIOBUTTONSYSTEM_H_INCLUDED
