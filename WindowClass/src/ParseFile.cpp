
const int MAX_STRING_LEN = 250;
const int HEADER_Y = 40;

const int START_X = 10;

const int EDIT_X = 780;
const int EDIT_Y = 25;
const int JUMP_Y = 10;
const int BUTTON_Y = 50;

const int MAX_Y = WINDOW_SIZE.x - 2*BUTTON_Y;

#define FIND_NEXT_CHAR(pointer, what) while (*pointer != what && *pointer != L'\n' && *pointer != EOF) pointer++
#define CHECK_POINTER(pointer) \
if (*pointer == L'\n' || *pointer == EOF) \
{ \
    wprintf (L"Bad argument on line %d (\"%s\")\n", line, filename); \
    window_.RegisterObjects(true); \
    return; \
}

#define SEPARATE_STRING \
        FIND_NEXT_CHAR (startPt, L'"'); \
        startPt ++; \
        CHECK_POINTER (startPt); \
        endPt = startPt; \
        FIND_NEXT_CHAR (endPt, L'"'); \
        CHECK_POINTER (endPt); \
        *endPt = 0

#define ARRANGE_START \
while (true) \
{ \
fgetws (str, MAX_STRING_LEN, f); \
line++; \
startPt = str; \
while (!iswalpha (*startPt) && \
       *startPt != EOF && \
       *startPt != L'\n') startPt++; \
if (*startPt != L'\n') break; \
CHECK_POINTER (startPt); \
}

#define ARRANGE_END endPt = startPt + 1;\
                    while (iswalpha(*endPt)) endPt++;\
                    *endPt = 0

#define ARRANGE_POINTERS ARRANGE_START \
                         ARRANGE_END

#define Y_OK currentY_ < MAX_Y &&

const DWORD CHECKBOX_STYLE = WS_TABSTOP |
                             WS_VISIBLE |
                             WS_CHILD |
                             BS_AUTOCHECKBOX |
                             BS_MULTILINE;

const DWORD EDITBOX_STYLE  = WS_VISIBLE |
                             WS_CHILD |
                             ES_WANTRETURN;

void Page_Management::ParseFile (LPCWSTR filename)
{
    FILE* f = _wfopen (filename, L"r");
    if (!f)
    {
        wprintf(L"Cannot open file:%s\n", filename);
        return;
    }
    window_.RegisterObjects(false);
    int line = 1;

    while (Y_OK !feof (f))
    {
        wchar_t str[MAX_STRING_LEN] = L"";
        fgetws (str, MAX_STRING_LEN, f);
        int first_arg_end = 0;
        for (int i = 0; str[i]; i++)
        {
            if (str[i] == L' ')
            {
                str[i] = 0;
                first_arg_end = i;
                break;
            }
        }
        if (Y_OK !wcscmp(str, L"Header"))
        {
            LPWSTR startPt = str + first_arg_end;
            LPWSTR endPt = NULL;
            SEPARATE_STRING;
            LPWSTR name = new wchar_t [endPt - startPt + 1];
            wcscpy (name, startPt);
            startPt = endPt + 1;
            SEPARATE_STRING;
            LPWSTR font = new wchar_t [endPt - startPt + 1];
            wcscpy (font, startPt);
            startPt = endPt + 1;
            while (!iswdigit (*startPt) &&
                    *startPt != EOF &&
                    *startPt != L'\n') startPt++;
            CHECK_POINTER (startPt);
            int size = _wtoi (startPt);
            header_ = new EditBox (font,
                                   size,
                                   START_X,
                                   currentY_,
                                   EDIT_X,
                                   HEADER_Y,
                                   name,
                                   WS_VISIBLE |
                                   WS_CHILD |
                                   ES_READONLY |
                                   ES_CENTER,
                                   0,
                                   &window_,
                                   NULL, NULL);
            SecureArrayDelete (name);
            SecureArrayDelete (font);
            currentY_ += HEADER_Y;
        }
        if (Y_OK !wcscmp(str, L"Load"))
        {
            LPWSTR startPt = str + first_arg_end;
            LPWSTR endPt = NULL;
            SEPARATE_STRING;
            ParseFile(startPt);
        }
        if (Y_OK !wcscmp(str, L"Question"))
        {
            int question_y = currentY_;
            currentY_ += EDIT_Y;
            questions_.push_back (new Question);
            Question& q = *questions_.back();
            LPWSTR startPt = str + first_arg_end;
            LPWSTR endPt = NULL;

            SEPARATE_STRING;
            q.text (startPt, endPt);

            LPWSTR currentFont = NULL;
            int currentFontSize = 0;

            memset (str, 0, MAX_STRING_LEN * sizeof (wchar_t));

            ARRANGE_POINTERS;

            if (wcscmp(startPt, L"START"))
            {
                printf ("Expected START after Question creation\n");
                window_.RegisterObjects(true);
                return;
            }
            while (!feof (f))
            {
                memset (str, 0, MAX_STRING_LEN * sizeof (wchar_t));

                ARRANGE_POINTERS;

                if (Y_OK !wcscmp (startPt, L"Lines"))
                {
                    int lines = q.nLines_;
                    q.nLines (endPt + 1);
                    currentY_ += EDIT_Y * (q.nLines_ - lines);
                }

                if (Y_OK !wcscmp (startPt, L"QuestionFont"))
                {
                    startPt = endPt + 1;
                    SEPARATE_STRING;
                    q.font (startPt, endPt);
                    startPt = endPt + 1;
                    while (!iswdigit (*startPt) &&
                            *startPt != EOF &&
                            *startPt != L'\n') startPt++;
                    CHECK_POINTER (startPt);
                    q.fontsize (startPt);
                }

                if (Y_OK !wcscmp (startPt, L"SelectFont"))
                {
                    startPt = endPt + 1;
                    SEPARATE_STRING;
                    currentFont = new wchar_t [endPt - startPt + 1];
                    wcscpy (currentFont, startPt);
                    startPt = endPt + 1;
                    while (!iswdigit (*startPt) &&
                            *startPt != EOF &&
                            *startPt != L'\n') startPt++;
                    CHECK_POINTER (startPt);
                    currentFontSize = _wtoi (startPt);
                }

                if (Y_OK !wcscmp (startPt, L"Buttons"))
                {
                    startPt = endPt + 1;
                    while (!iswalpha (*startPt) &&
                            *startPt != EOF &&
                            *startPt != L'\n') startPt++;
                    CHECK_POINTER (startPt);
                    ARRANGE_END;
                    CHECK_POINTER (endPt);
                    DWORD style = 0;
                    if (!wcscmp (startPt, L"Radio")) style = BS_RADIOBUTTON;
                    else
                    if (!wcscmp (startPt, L"Checkbox")) style = BS_CHECKBOX;
                    else
                    {
                        printf ("Invalid button style on line %d\n", line);
                        window_.RegisterObjects(true);
                        return;
                    }
                    RadioButtonSystem* rbs = NULL;
                    if (style == BS_RADIOBUTTON)
                    {
                        rbs = new RadioButtonSystem (&window_);
                        q.StoreElement (rbs, DeleteRBS);
                    }

                    startPt = endPt + 1;
                    while (!iswdigit (*startPt) &&
                            *startPt != EOF &&
                            *startPt != L'\n') startPt++;
                    CHECK_POINTER (startPt);
                    int n = _wtoi (startPt);
                    int part = EDIT_X / (3*n + 1);
                    for (int i = 0; i < n; i++)
                    {
                        SEPARATE_STRING;
                        if (rbs) rbs->AddButton (currentFont, currentFontSize,
                                                 START_X + part*(3*i + 1),
                                                 currentY_,
                                                 2*part,
                                                 BUTTON_Y,
                                                 startPt);

                        else q.StoreElement (new Button (currentFont, currentFontSize,
                                                         START_X + part*(3*i + 1),
                                                         currentY_,
                                                         2*part,
                                                         BUTTON_Y,
                                                         startPt,
                                                         CHECKBOX_STYLE,
                                                         0,
                                                         &window_,
                                                         NULL, NULL), DeleteButton);
                        startPt = endPt + 1;
                    }
                    currentY_ += BUTTON_Y;
                }

                if (Y_OK !wcscmp (startPt, L"StaticText"))
                {
                    startPt = endPt + 1;
                    SEPARATE_STRING;
                    LPCWSTR startBak = startPt;
                    startPt = endPt + 1;
                    while (!iswdigit (*startPt) &&
                            *startPt != EOF &&
                            *startPt != L'\n') startPt++;
                    CHECK_POINTER (startPt);
                    int n = _wtoi (startPt);
                    EditBox* pt = new EditBox (currentFont, currentFontSize,
                                               START_X,
                                               currentY_,
                                               EDIT_X,
                                               EDIT_Y * n,
                                               startBak,
                                               EDITBOX_STYLE |
                                               (n > 1 ? ES_MULTILINE : 0) |
                                               ES_READONLY,
                                               0,
                                               &window_,
                                               NULL, NULL);
                    q.StoreElement (pt, DeleteEditBox);
                    currentY_ += EDIT_Y * n;
                }

                if (Y_OK !wcscmp (startPt, L"InputBox"))
                {
                    startPt = endPt + 1;
                    while (!iswdigit (*startPt) &&
                            *startPt != EOF &&
                            *startPt != L'\n') startPt++;
                    CHECK_POINTER (startPt);
                    int n = _wtoi (startPt);
                    EditBox* pt = new EditBox (currentFont, currentFontSize,
                                               START_X,
                                               currentY_,
                                               EDIT_X,
                                               EDIT_Y * n,
                                               NULL,
                                               EDITBOX_STYLE |
                                               (n > 1 ? ES_MULTILINE : 0) |
                                               WS_BORDER,
                                               0,
                                               &window_,
                                               NULL, NULL);
                    q.StoreElement (pt, DeleteEditBox);
                    currentY_ += EDIT_Y * n;
                }

                if (!wcscmp (startPt, L"END"))
                {
                    EditBox* pt = new EditBox (q.font_,
                                               q.fontsize_,
                                               START_X,
                                               question_y,
                                               EDIT_X,
                                               q.nLines_ * EDIT_Y,
                                               q.text_,
                                               EDITBOX_STYLE |
                                               (q.nLines_ > 1 ? ES_MULTILINE : 0) |
                                               ES_READONLY,
                                               0,
                                               &window_,
                                               NULL,
                                               NULL);
                    q.StoreElement (pt, DeleteEditBox);
                    SecureArrayDelete (currentFont);
                    break;
                }
            }
        }
        line++;
    }

    window_.RegisterObjects(true);

    fclose (f);
}

#undef CHECK_POINTER
#define CHECK_POINTER(pointer) \
if (*pointer == L'\n' || *pointer == EOF) \
{ \
    printf ("Bad argument on line %d (\"settings.txt\")\n", line); \
    return false; \
}

#define CHECK_FILE(file) if (feof (file)) { MessageBoxW (NULL, L"Invalid settings file format\n", APP_NAME_W, MB_OK | MB_ICONEXCLAMATION); return false;}

#define READ_WORD(save) \
{ \
    fgetws (str, MAX_STRING_LEN, settings); \
    startPt = str; \
    endPt = startPt; \
    FIND_NEXT_CHAR (startPt, L'"'); \
    startPt++; \
    CHECK_POINTER (startPt); \
    endPt = startPt; \
    FIND_NEXT_CHAR (endPt, L'"'); \
    CHECK_POINTER (endPt); \
    *endPt = 0; \
    if (endPt - startPt) save = new wchar_t [endPt - startPt + 1]; \
    wcscpy (save, startPt); \
    line++; \
} \

struct Settings
{
    LPWSTR caption;
    int    pages;
    LPWSTR filenamePart1;
    LPWSTR filenamePart2;
    LPWSTR notCompletedMessage;
    LPWSTR nextPageButton;
    LPWSTR doneButton;
    LPWSTR buttonFont;
    size_t buttonFontSize;

    Settings () :
        caption        (NULL),
        pages          (0),
        filenamePart1  (NULL),
        filenamePart2  (NULL),
        nextPageButton (NULL),
        doneButton     (NULL),
        buttonFont     (NULL),
        buttonFontSize (0)
    {}

    ~Settings ()
    {
        FreeStuff();
    }

    void FreeStuff ()
    {
        SecureArrayDelete (caption);
        SecureArrayDelete (filenamePart1);
        SecureArrayDelete (filenamePart2);
        SecureArrayDelete (nextPageButton);
        SecureArrayDelete (doneButton);
        SecureArrayDelete (buttonFont);
        pages = 0;
        buttonFontSize = 0;
    }

    void Print ()
    {
        wprintf (L"%s\n%d\n%s\n%s%s\n%s\n%s\n%s\n%d\n",
                 caption,
                 pages,
                 filenamePart1,
                 filenamePart2,
                 notCompletedMessage,
                 nextPageButton,
                 doneButton,
                 buttonFont,
                 buttonFontSize);
    }
};

bool ReadSettingsFile (Settings& st)
{
    FILE* settings = _wfopen (L"settings.txt", L"r");
    if (!settings)
    {
        MessageBoxW (NULL, L"Settings file not found", APP_NAME_W, MB_OK | MB_ICONEXCLAMATION);
        return false;
    }
    wchar_t str[MAX_STRING_LEN] = L"";

    int line = 1;

    LPWSTR startPt = str;
    LPWSTR endPt = startPt;

    READ_WORD (st.caption);

    CHECK_FILE (settings);

    {//! Pages
        fgetws (str, MAX_STRING_LEN, settings);
        startPt = str;
        while (!iswdigit (*startPt) &&
                *startPt != EOF &&
                *startPt != L'\n') startPt++;
        CHECK_POINTER (startPt);
        st.pages = _wtoi (str);
        line++;
    }

    CHECK_FILE (settings);

    {//! filenamePart1 & filenamePart2
        fgetws (str, MAX_STRING_LEN, settings);
        startPt = str;
        endPt = startPt;
        while (*startPt == L' ') startPt++;
        CHECK_POINTER (startPt);
        LPWSTR endPt = startPt;
        FIND_NEXT_CHAR (endPt, L'$');
        CHECK_POINTER (endPt);
        *endPt = 0;
        if (endPt - startPt) st.filenamePart1 = new wchar_t [endPt - startPt + 1];
        wcscpy (st.filenamePart1, startPt);
        endPt++;
        startPt = endPt;
        while (*endPt != L' ' &&
               *endPt != L'\n' &&
               *endPt != EOF &&
               *endPt) endPt++;
        *endPt = 0;
        if (endPt - startPt) st.filenamePart2 = new wchar_t [endPt - startPt + 1];
        wcscpy (st.filenamePart2, startPt);
        line++;
    }

    CHECK_FILE (settings);

    READ_WORD (st.notCompletedMessage);

    CHECK_FILE (settings);

    READ_WORD (st.nextPageButton);

    CHECK_FILE (settings);

    READ_WORD (st.doneButton);

    CHECK_FILE (settings);

    READ_WORD (st.buttonFont);

    CHECK_FILE (settings);

    {//! buttonFontSize
        fgetws (str, MAX_STRING_LEN, settings);
        startPt = str;
        while (!iswdigit (*startPt) &&
                *startPt != EOF &&
                *startPt != L'\n') startPt++;
        CHECK_POINTER (startPt);
        st.buttonFontSize = _wtoi (str);
        line++;
    }

    fclose (settings);

    return true;
}

#undef FIND_NEXT_CHAR
#undef CHECK_POINTER
#undef SEPARATE_STRING
#undef ARRANGE_START
#undef ARRANGE_END
#undef ARRANGE_POINTERS
