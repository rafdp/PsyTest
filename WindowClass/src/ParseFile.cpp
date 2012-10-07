
const int MAX_STRING_LEN = 250;
const int HEADER_Y = 40;

const int START_X = 10;

const int EDIT_X = 780;
const int EDIT_Y = 30;
const int JUMP_Y = 10;
const int BUTTON_Y = 50;

const int MAX_Y = WINDOW_SIZE.x - 2*BUTTON_Y;

#define FIND_NEXT_CHAR(pointer, what) \
    while (*pointer != what && \
           *pointer != L'\n' && \
           *pointer != EOF) pointer++

#define CHECK_POINTER(pointer) \
    if (*pointer == L'\n' || *pointer == EOF) \
    { \
        ErrorPrintfBoxW (HWND(window_), \
                         MB_OK | MB_ICONSTOP, \
                         L"Неправильные аргументы на строке %d (\"%s\")\n", \
                         line, \
                         filename); \
        return false; \
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

#define ARRANGE_END \
    endPt = startPt + 1;\
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

bool Page_Management::ParseFile (LPCWSTR filename)
{
    FILE* f = _wfopen (filename, L"r");
    if (!f)
    {
        ErrorPrintfBoxW (HWND(window_),
                         MB_OK | MB_ICONSTOP,
                         L"Возникла ошибка при открытии \"%s\"", filename);
        return false;
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
            MemContainer<wchar_t> name (new wchar_t [endPt - startPt + 1],
                                        MEMORY_ARRAY);
            wcscpy (name.data, startPt);
            startPt = endPt + 1;
            SEPARATE_STRING;
            MemContainer<wchar_t> font (new wchar_t [endPt - startPt + 1],
                                        MEMORY_ARRAY);

            wcscpy (font.data, startPt);
            startPt = endPt + 1;
            while (!iswdigit (*startPt) &&
                    *startPt != EOF &&
                    *startPt != L'\n') startPt++;
            CHECK_POINTER (startPt);
            int size = _wtoi (startPt);
            header_.Set(new EditBox (font.data,
                                     size,
                                     START_X,
                                     currentY_,
                                     EDIT_X,
                                     HEADER_Y,
                                     name.data,
                                     WS_VISIBLE |
                                     WS_CHILD |
                                     ES_READONLY |
                                     ES_CENTER,
                                     0,
                                     &window_,
                                     NULL, NULL),
                        MEMORY_OBJECT);
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

            MemContainer <wchar_t> currentFont;
            int currentFontSize = 0;

            memset (str, 0, MAX_STRING_LEN * sizeof (wchar_t));

            ARRANGE_POINTERS;

            if (wcscmp(startPt, L"START"))
            {
                ErrorPrintfBoxW (HWND(window_),
                                 MB_OK | MB_ICONSTOP,
                                 L"Ожидается START после создания Question");
                return false;
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
                    currentFont.Set (new wchar_t [endPt - startPt + 1],
                                     MEMORY_ARRAY);
                    wcscpy (currentFont.data, startPt);
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
                    bool check = true;
                    while (!iswalpha (*startPt) &&
                            *startPt != EOF &&
                            *startPt != L'\n') startPt++;
                    CHECK_POINTER (startPt);
                    ARRANGE_END;
                    CHECK_POINTER (endPt);
                    DWORD style = 0;
                    if (!wcscmp (startPt, L"Radio"))
                    {
                        style = BS_RADIOBUTTON;
                        startPt = endPt + 1;
                        SEPARATE_STRING;
                        if (!wcscmp (startPt, L"Check"))
                            check = true;
                        else
                        if (!wcscmp (startPt, L"!Check"))
                            check = false;
                        else
                        {
                            ErrorPrintfBoxW (HWND(window_),
                                             MB_OK | MB_ICONSTOP,
                                             L"Неправильные аргументы на строке %d (\"%s\")\n",
                                             line,
                                             filename);
                            return false;
                        }
                    }
                    else
                    if (!wcscmp (startPt, L"Checkbox")) style = BS_CHECKBOX;
                    else
                    {
                        ErrorPrintfBoxW (HWND(window_),
                                         MB_OK | MB_ICONSTOP,
                                         L"Стиль кнопки, указанный в файле \"%s\" на строке %d,не соответствует ни одному из заданных",
                                         filename,
                                         line);
                        return false;
                    }
                    RadioButtonSystem* rbs = NULL;
                    if (style == BS_RADIOBUTTON)
                    {
                        rbs = new RadioButtonSystem (&window_, check);
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
                        if (rbs) rbs->AddButton (currentFont.data, currentFontSize,
                                                 START_X + part*(3*i + 1),
                                                 currentY_,
                                                 2*part,
                                                 BUTTON_Y,
                                                 startPt);

                        else q.StoreElement (new Button (currentFont.data, currentFontSize,
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
                    EditBox* pt = new EditBox (currentFont.data, currentFontSize,
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
                    EditBox* pt = new EditBox (currentFont.data, currentFontSize,
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
                    EditBox* pt = new EditBox (q.font_.data,
                                               q.fontsize_,
                                               START_X,
                                               question_y,
                                               EDIT_X,
                                               q.nLines_ * EDIT_Y,
                                               q.text_.data,
                                               EDITBOX_STYLE |
                                               ((q.nLines_ > 1) ? ES_MULTILINE : 0) |
                                               ES_READONLY,
                                               0,
                                               &window_,
                                               NULL,
                                               NULL);
                    q.StoreElement (pt, DeleteEditBox);
                    break;
                }
            }
        }
        line++;
    }

    window_.RegisterObjects(true);

    fclose (f);
    return true;
}

#undef Y_OK
#undef CHECK_POINTER
#define CHECK_POINTER(pointer) \
if (*pointer == L'\n' || *pointer == EOF) \
{ \
    ErrorPrintfBoxW (NULL, \
                     MB_OK | MB_ICONSTOP, \
                     L"Неправильные аргументы на строке %d (\"settings.txt\")\n", \
                     line); \
    return false; \
}

#define CHECK_FILE(file) if (feof (file)) { MessageBoxW (NULL, L"Неправильный формат файла настроек\n", APP_NAME_W, MB_OK | MB_ICONEXCLAMATION); return false;}

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
    if (endPt - startPt) save.Set ( new wchar_t [endPt - startPt + 1], MEMORY_ARRAY); \
    wcscpy (save.data, startPt); \
    line++; \
} \

struct Settings
{
    MemContainer<wchar_t> caption;
    bool   grayFill;
    int    pages;
    MemContainer<wchar_t> filenamePart1;
    MemContainer<wchar_t> filenamePart2;
    MemContainer<wchar_t> notCompletedMessage;
    MemContainer<wchar_t> nextPageButton;
    MemContainer<wchar_t> doneButton;
    MemContainer<wchar_t> buttonFont;
    size_t buttonFontSize;

    Settings () :
        caption        (),
        grayFill       (false),
        pages          (0),
        filenamePart1  (),
        filenamePart2  (),
        nextPageButton (),
        doneButton     (),
        buttonFont     (),
        buttonFontSize (0)
    {}

    ~Settings ()
    {
        FreeStuff();
    }

    void FreeStuff ()
    {
        pages = 0;
        buttonFontSize = 0;
        grayFill = false;
    }
};

bool ReadSettingsFile (Settings& st)
{
    FILE* settings = _wfopen (L"settings.txt", L"r");
    if (!settings)
    {
        ErrorPrintfBoxW (NULL,
                         MB_OK | MB_ICONSTOP,
                         L"Не найден файл настроек (\"settings.txt\")");
        return false;
    }
    wchar_t str[MAX_STRING_LEN] = L"";

    int line = 1;

    LPWSTR startPt = str;
    LPWSTR endPt = startPt;

    READ_WORD (st.caption);

    CHECK_FILE (settings);

    {
        fgetws (str, MAX_STRING_LEN, settings);
        startPt = str;
        endPt = startPt;
        FIND_NEXT_CHAR (startPt, L'"');
        startPt++;
        CHECK_POINTER (startPt);
        endPt = startPt;
        FIND_NEXT_CHAR (endPt, L'"');
        CHECK_POINTER (endPt);
        *endPt = 0;
        if (!wcscmp (startPt, L"White")) st.grayFill = false;
        else
        if (!wcscmp (startPt, L"Gray")) st.grayFill = true;
        else
        {
            ErrorPrintfBoxW (NULL,
                             MB_OK | MB_ICONSTOP,
                             L"Цвет заливки, указанный в файле \"settings.txt\" на строке %d, не соответствует ни одному из заданных",
                             line);
            return false;
        }
        line++;
    }

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
        if (endPt - startPt) st.filenamePart1.Set (new wchar_t [endPt - startPt + 1],
                                                   MEMORY_ARRAY);
        wcscpy (st.filenamePart1.data, startPt);
        endPt++;
        startPt = endPt;
        while (*endPt != L' ' &&
               *endPt != L'\n' &&
               *endPt != EOF &&
               *endPt) endPt++;
        *endPt = 0;
        if (endPt - startPt) st.filenamePart2.Set (new wchar_t [endPt - startPt + 1],
                                                   MEMORY_ARRAY);
        wcscpy (st.filenamePart2.data, startPt);
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
