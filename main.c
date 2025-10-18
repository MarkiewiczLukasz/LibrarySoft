#include "headers/formFunctions.h"
#include "headers/lists.h"
#include "headers/resource.h"
#include <stdio.h>   // snprintf, printf
#include <stdlib.h>  // atoi
#include <time.h>    // time, srand
#include <windows.h> // WinAPI types & functions

// Global window handles for dialogs
HWND hwndNewBook = NULL;   // Dialog: Dodawanie/edycja książki
HWND hwndNewUser = NULL;   // Dialog: Dodawanie/edycja użytkownika
HWND hwndBorrowBook = NULL; // Dialog: Wypożyczanie książki
HWND mainHWND;             // Główny dialog aplikacji

// Deklaracje procedur okien dialogowych
BOOL CALLBACK addStoredBookWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK mainWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK addLibraryUserWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK BorrowBookWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

// Zmienne stanu aplikacji
int editedStoredBookId = -1; // Indeks edytowanej książki magazynowej (StoredBook) lub -1 dla nowej
int editedUserId = -1;       // Indeks edytowanego użytkownika lub -1 dla nowego
int sourceSelected = -1;     // Aktualnie wybrane źródło (lista)
int borrowingBookId = -1;    // Indeks książki wybieranej do wypożyczenia

// === Punkt wejścia aplikacji (dialog-based) ===
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Uruchamiamy główny dialog modalny i przekazujemy procedurę obsługi zdarzeń
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDLG), NULL, (DLGPROC)mainWindow);
}

// === Dialog: Wypożycz książkę ===
BOOL CALLBACK BorrowBookWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
    case WM_INITDIALOG:
        // Inicjalizacja kontrolek dialogu wypożyczania (lista użytkowników ustawiana w mainWindow)
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            // Zatwierdzenie wypożyczenia książki wskazanemu użytkownikowi
            HWND list = GetDlgItem(mainHWND, IDC_MainList1);
            HWND comboBox = GetDlgItem(hwndBorrowBook, IDC_CB);
            int indexSelected = (int)SendMessage(comboBox, CB_GETCURSEL, 0, 0);
            if (indexSelected == LB_ERR)
            {
                borrowingBookId = -1;
                return MessageBox(hwnd, "Wybierz użytkownika", "Błąd", MB_OK);
            }
            int idOfUser = (int)SendMessage(comboBox, CB_GETITEMDATA, indexSelected, 0); // Pobierz ID użytkownika

            BorrowedBook borrowed;
            borrowedBook_Init(&borrowed,
                              ListBorrowedBooks[ListBorrowedBooksLastIndex].id + 1,
                              ListStoredBooks[borrowingBookId].name,
                              ListStoredBooks[borrowingBookId].author,
                              ListStoredBooks[borrowingBookId].type,
                              ListLibraryUsers[idOfUser].id);
            ListBorrowedBooks_Add(&borrowed);

            ListBorrowedBooks_Save();
            ListStoredBooks_Delete(borrowingBookId);
            ListStoredBooks_Save();
            FillList(mainHWND, sourceSelected);
            EndDialog(hwndBorrowBook, 0);
            hwndBorrowBook = NULL;
            borrowingBookId = -1;
        }
        break;

        case IDCANCEL:
            // Rezygnacja z wypożyczenia
            borrowingBookId = -1;
            EndDialog(hwndBorrowBook, 0);
            hwndBorrowBook = NULL;
            break;
        }
        break;

    case WM_CLOSE:
        // Zamknięcie okna dialogu wypożyczania
        borrowingBookId = -1;
        EndDialog(hwndBorrowBook, 0);
        hwndBorrowBook = NULL;
        break;

    default:
        return FALSE;
    }
    return TRUE;
}

// === Dialog: Dodaj/edytuj książkę magazynową ===
BOOL CALLBACK addStoredBookWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
    case WM_INITDIALOG:
        // Brak inicjalizacji szczególnej — pola ustawiane przy edycji w mainWindow
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            // Zatwierdzenie dodania/edycji książki
            HWND nameBox = GetDlgItem(hwndNewBook, IDC_NAME);
            HWND authorBox = GetDlgItem(hwndNewBook, IDC_AUTHOR);
            HWND typeBox = GetDlgItem(hwndNewBook, IDC_TYPE);
            HWND countBox = GetDlgItem(hwndNewBook, IDC_COUNT);
            HWND placeBox = GetDlgItem(hwndNewBook, IDC_PLACE);

            char name[100] = "";    GetWindowTextA(nameBox, name, 100);
            char type[100] = "";    GetWindowTextA(typeBox, type, 100);
            char author[100] = "";  GetWindowTextA(authorBox, author, 100);
            char place[100] = "";   GetWindowTextA(placeBox, place, 100);
            char quantity[100] = "";GetWindowTextA(countBox, quantity, 100);
            int quantityINT = atoi(quantity);

            if (editedStoredBookId != -1)
            {
                // Tryb edycji istniejącego rekordu
                int id = editedStoredBookId;
                ListStoredBooks[id].name = name;
                ListStoredBooks[id].author = author;
                ListStoredBooks[id].type = type;
                ListStoredBooks[id].quantity = quantityINT;
                ListStoredBooks[id].placeInLibrary = place;
            }
            else
            {
                // Tryb dodawania nowego rekordu
                StoredBook newBook;
                storedBook_Init(&newBook,
                                ListStoredBooks[ListStoredBooksLastIndex].id + 1,
                                name, author, type, quantityINT, place);
                ListStoredBooks_Add(&newBook);
            }

            ListStoredBooks_Save();
            FillList(mainHWND, sourceSelected);
            EndDialog(hwndNewBook, 0);
            hwndNewBook = NULL;
        }
        break;

        case IDCANCEL:
            // Anulowanie dodawania/edycji książki
            editedStoredBookId = -1;
            EndDialog(hwndNewBook, 0);
            hwndNewBook = NULL;
            break;
        }
        break;

    case WM_CLOSE:
        // Zamknięcie okna dialogu książki
        EndDialog(hwndNewBook, 0);
        hwndNewBook = NULL;
        break;

    default:
        return FALSE;
    }
    return TRUE;
}

// === Dialog: Dodaj/edytuj użytkownika ===
BOOL CALLBACK addLibraryUserWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
    case WM_INITDIALOG:
        // Brak inicjalizacji szczególnej — pola ustawiane przy edycji w mainWindow
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            // Zatwierdzenie dodania/edycji użytkownika
            HWND nameBox = GetDlgItem(hwndNewUser, IDC_NAME);
            HWND surnameBox = GetDlgItem(hwndNewUser, IDC_SURNAME);
            HWND numberBox = GetDlgItem(hwndNewUser, IDC_NUMBER);

            char name[100] = "";     GetWindowTextA(nameBox, name, 100);
            char surname[100] = "";  GetWindowTextA(surnameBox, surname, 100);
            char number[100] = "";   GetWindowTextA(numberBox, number, 100);

            if (editedUserId != -1)
            {
                // Tryb edycji użytkownika
                int id = editedUserId;
                ListLibraryUsers[id].name = name;
                ListLibraryUsers[id].surname = surname;
            }
            else
            {
                // Dodawanie nowego użytkownika
                LibraryUser newUser;
                libraryUser_Init(&newUser,
                                  ListLibraryUsers[ListLibraryUsersLastIndex].id + 1,
                                  atoi(number), name, surname);
                ListLibraryUsers_Add(&newUser);
            }

            ListLibraryUsers_Save();
            FillList(mainHWND, sourceSelected);
            EndDialog(hwndNewUser, 0);
            hwndNewUser = NULL;
        }
        break;

        case IDCANCEL:
            // Anulowanie dodawania/edycji użytkownika
            editedUserId = -1;
            EndDialog(hwndNewUser, 0);
            hwndNewUser = NULL;
            break;
        }
        break;

    case WM_CLOSE:
        // Zamknięcie okna dialogu użytkownika
        EndDialog(hwndNewUser, 0);
        hwndNewUser = NULL;
        break;

    default:
        return FALSE;
    }
    return TRUE;
}

// === Dialog główny aplikacji ===
BOOL CALLBACK mainWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
    case WM_INITDIALOG:
        mainHWND = hwnd;
        HINSTANCE hInst = GetModuleHandle(NULL);
        HICON hBig   = LoadIcon(hInst, MAKEINTRESOURCE(IDI_APPICON));
        HICON hSmall = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON,
                                        GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
        SendMessage(hwnd, WM_SETICON, ICON_BIG,   (LPARAM)hBig);
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hSmall);
        InitMainDialog(hwnd);
        ListStoredBooks_Init();
        ListBorrowedBooks_Init();
        ListLibraryUsers_Init();
        srand((unsigned)time(NULL));
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_SourceList:
            // Zmiana źródła danych (zakładki/listy) w comboboxie
            if (HIWORD(wParam) == CBN_SELCHANGE) HandleSourceListChange(hwnd);
            break;

        case IDC_MainList1:
        case IDC_MainList2:
        case IDC_MainList3:
        case IDC_MainList4:
        case IDC_MainList5:
        {
            // Synchronizacja zaznaczenia między wszystkimi listboxami
            if (HIWORD(wParam) == LBN_SELCHANGE)
            {
                HWND changedList = GetDlgItem(hwnd, LOWORD(wParam));
                int indexToSelectOnAll = (int)SendMessage(changedList, LB_GETCURSEL, 0, 0);
                if (indexToSelectOnAll != LB_ERR)
                {
                    for (int i = 1027; i <= 1031; ++i)
                    {
                        HWND currentlist = GetDlgItem(hwnd, i);
                        int currSelected = (int)SendMessage(currentlist, LB_GETCURSEL, 0, 0);
                        if (currSelected != indexToSelectOnAll)
                        {
                            SendMessage(currentlist, LB_SETCURSEL, indexToSelectOnAll, 0);
                        }
                    }
                }
            }
            // brak "break" – dalej obsługa przycisków akcji (fallthrough kontrolowany)
        }
        // === Akcja 1 ===
        case IDC_ACTION1:
        {
            // Dodanie nowego rekordu zależnie od aktywnego źródła
            switch (sourceSelected)
            {
            case 0: // Zasoby: dodaj książkę magazynową
                if (HIWORD(wParam) == BN_CLICKED && hwndNewBook == NULL)
                {
                    editedStoredBookId = -1;
                    HINSTANCE instance = GetModuleHandle(NULL);
                    hwndNewBook = CreateDialog(instance,
                                               MAKEINTRESOURCE(IDD_AddStoredBook),
                                               hwnd,
                                               (DLGPROC)addStoredBookWindow);
                    ShowWindow(hwndNewBook, SW_SHOW);
                }
                break;

            case 2: // Użytkownicy: dodaj użytkownika
                if (HIWORD(wParam) == BN_CLICKED && hwndNewUser == NULL)
                {
                    editedUserId = -1;
                    HINSTANCE instance = GetModuleHandle(NULL);
                    hwndNewUser = CreateDialog(instance,
                                               MAKEINTRESOURCE(IDD_AddLibraryUser),
                                               hwnd,
                                               (DLGPROC)addLibraryUserWindow);
                    ShowWindow(hwndNewUser, SW_SHOW);

                    // Wypełnij pole numeru losową wartością
                    HWND numberBox = GetDlgItem(hwndNewUser, IDC_NUMBER);
                    int randNumber = rand() % 5000;
                    char buff[32];
                    snprintf(buff, sizeof(buff), "%i", randNumber);
                    SetWindowTextA(numberBox, buff);
                }
                break;
            }
        }
        break;

        // === Akcja 2 ===
        case IDC_ACTION2:
        {
            // Edycja istniejącego rekordu zależnie od źródła
            switch (sourceSelected)
            {
            case 0: // Zasoby: edytuj książkę magazynową
                if (HIWORD(wParam) == BN_CLICKED && hwndNewBook == NULL)
                {
                    HWND list = GetDlgItem(hwnd, IDC_MainList1);
                    int indexSelected = (int)SendMessage(list, LB_GETCURSEL, 0, 0);
                    if (indexSelected == LB_ERR)
                    {
                        return MessageBox(hwnd, "Wybierz książkę", "Błąd", MB_OK);
                    }

                    HINSTANCE instance = GetModuleHandle(NULL);
                    hwndNewBook = CreateDialog(instance,
                                               MAKEINTRESOURCE(IDD_AddStoredBook),
                                               hwnd,
                                               (DLGPROC)addStoredBookWindow);
                    ShowWindow(hwndNewBook, SW_SHOW);

                    int idOfBook = (int)SendMessage(list, LB_GETITEMDATA, indexSelected, 0);
                    StoredBook s = ListStoredBooks[idOfBook];

                    HWND description = GetDlgItem(hwndNewBook, IDC_STATIC);
                    HWND nameBox = GetDlgItem(hwndNewBook, IDC_NAME);
                    HWND authorBox = GetDlgItem(hwndNewBook, IDC_AUTHOR);
                    HWND typeBox = GetDlgItem(hwndNewBook, IDC_TYPE);
                    HWND countBox = GetDlgItem(hwndNewBook, IDC_COUNT);
                    HWND placeBox = GetDlgItem(hwndNewBook, IDC_PLACE);

                    SetWindowTextA(description, "Edytuj wybraną książkę");
                    SetWindowTextA(nameBox, s.name);
                    SetWindowTextA(authorBox, s.author);
                    SetWindowTextA(typeBox, s.type);

                    char intAsStr[32];
                    snprintf(intAsStr, sizeof(intAsStr), "%i", s.quantity);
                    SetWindowTextA(countBox, intAsStr);
                    SetWindowTextA(placeBox, s.placeInLibrary);
                    editedStoredBookId = idOfBook;
                }
                break;

            case 2: // Użytkownicy: edytuj użytkownika
                if (HIWORD(wParam) == BN_CLICKED && hwndNewUser == NULL)
                {
                    HWND list = GetDlgItem(hwnd, IDC_MainList1);
                    int indexSelected = (int)SendMessage(list, LB_GETCURSEL, 0, 0);
                    if (indexSelected == LB_ERR)
                    {
                        return MessageBox(hwnd, "Wybierz czytelnika", "Błąd", MB_OK);
                    }

                    HINSTANCE instance = GetModuleHandle(NULL);
                    hwndNewUser = CreateDialog(instance,
                                               MAKEINTRESOURCE(IDD_AddLibraryUser),
                                               hwnd,
                                               (DLGPROC)addLibraryUserWindow);
                    ShowWindow(hwndNewUser, SW_SHOW);

                    int idOfUser = (int)SendMessage(list, LB_GETITEMDATA, indexSelected, 0);
                    LibraryUser s = ListLibraryUsers[idOfUser];

                    HWND description = GetDlgItem(hwndNewUser, IDC_STATIC);
                    HWND nameBox = GetDlgItem(hwndNewUser, IDC_NAME);
                    HWND surnameBox = GetDlgItem(hwndNewUser, IDC_SURNAME);
                    HWND numberBox = GetDlgItem(hwndNewUser, IDC_NUMBER);

                    SetWindowTextA(description, "Edytuj wybranego czytelnika");
                    SetWindowTextA(nameBox, s.name);
                    SetWindowTextA(surnameBox, s.surname);

                    char intAsStr[32];
                    snprintf(intAsStr, sizeof(intAsStr), "%i", s.number);
                    SetWindowTextA(numberBox, intAsStr);

                    editedUserId = idOfUser;
                }
                break;
            }
        }
        break;

        // === Akcja 3 ===
        case IDC_ACTION3:
        {
            // Operacje na wypożyczonych książkach (zwrot)
            switch (sourceSelected)
            {
            case 1: // Wypożyczone: zwróć książkę do zasobów
                if (HIWORD(wParam) == BN_CLICKED)
                {
                    HWND list = GetDlgItem(hwnd, IDC_MainList1);
                    int indexSelected = (int)SendMessage(list, LB_GETCURSEL, 0, 0);
                    int idOfBook = (int)SendMessage(list, LB_GETITEMDATA, indexSelected, 0);
                    if (idOfBook == -1)
                    {
                        return MessageBox(hwnd, "Wybierz książkę", "Błąd", MB_OK);
                    }

                    int msgResult = MessageBox(hwnd,
                                               "Czy jesteś pewny, że chcesz zwrócić wybraną książkę?",
                                               "Zwróć książkę",
                                               MB_ICONWARNING | MB_YESNO);
                    if (msgResult == IDYES)
                    {
                        StoredBook s;
                        storedBook_Init(&s,
                                        ListStoredBooks[ListBorrowedBooksLastIndex].id + 1,
                                        ListBorrowedBooks[idOfBook].name,
                                        ListBorrowedBooks[idOfBook].author,
                                        ListBorrowedBooks[idOfBook].type,
                                        1,
                                        "ZWROTY");
                        ListStoredBooks_Add(&s);
                        ListStoredBooks_Save();
                        ListBorrowedBooks_Delete(idOfBook);
                        ListBorrowedBooks_Save();
                        FillList(mainHWND, sourceSelected);
                    }
                }
                break;
            }
        }
        break;

        // === Akcja 4 ===
        case IDC_ACTION4:
        {
            // Wypożycz książkę wybranemu użytkownikowi
            switch (sourceSelected)
            {
            case 0: // Zasoby: otwórz dialog wypożyczenia
                if (HIWORD(wParam) == BN_CLICKED)
                {
                    if (HIWORD(wParam) == BN_CLICKED && hwndBorrowBook == NULL)
                    {
                        HWND list = GetDlgItem(hwnd, IDC_MainList1);
                        int indexSelected = (int)SendMessage(list, LB_GETCURSEL, 0, 0);
                        int idOfBook = (int)SendMessage(list, LB_GETITEMDATA, indexSelected, 0);
                        if (idOfBook == -1)
                        {
                            return MessageBox(hwnd, "Wybierz książkę", "Błąd", MB_OK);
                        }

                        HINSTANCE instance = GetModuleHandle(NULL);
                        hwndBorrowBook = CreateDialog(instance,
                                                      MAKEINTRESOURCE(IDD_BorrowBook),
                                                      hwnd,
                                                      (DLGPROC)BorrowBookWindow);
                        ShowWindow(hwndBorrowBook, SW_SHOW);
                        borrowingBookId = idOfBook;

                        HWND description = GetDlgItem(hwndBorrowBook, IDC_TEXT);
                        HWND comboBox = GetDlgItem(hwndBorrowBook, IDC_CB);
                        SetWindowTextA(description, "Wybierz czytelnika,\nktóremu wypożyczana jest książka.");

                        for (int x = 0; x <= ListLibraryUsersLastIndex; x++)
                        {
                            char tempStr[128];
                            snprintf(tempStr, sizeof(tempStr), "(%d)%s %s", ListLibraryUsers[x].number, ListLibraryUsers[x].name, ListLibraryUsers[x].surname);
                            int pos = (int)SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)tempStr);
                            SendMessage(comboBox, CB_SETITEMDATA, pos, (LPARAM)x); // zapisz indeks jako item data
                        }
                        SendMessage(comboBox, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);
                    }
                }
                break;
            }
        }
        break;

        // === Akcja 5 ===
        case IDC_ACTION5:
            // Aktualnie puste miejsce na przyszłą funkcję (np. eksport, druk, itp.)
            break;

        // === Akcja 6 ===
        case IDC_ACTION6:
        {
            // Usuwanie rekordów (zasoby / użytkownicy)
            switch (sourceSelected)
            {
            case 0: // Zasoby: usuń książkę
                if (HIWORD(wParam) == BN_CLICKED)
                {
                    HWND list = GetDlgItem(hwnd, IDC_MainList1);
                    int indexSelected = (int)SendMessage(list, LB_GETCURSEL, 0, 0);
                    int idOfBook = (int)SendMessage(list, LB_GETITEMDATA, indexSelected, 0);
                    if (idOfBook == -1)
                    {
                        return MessageBox(hwnd, "Wybierz książkę", "Błąd", MB_OK);
                    }

                    int msgResult = MessageBox(hwnd,
                                               "Czy jesteś pewny, że chcesz usunąć wybraną książkę?",
                                               "Usuń książkę",
                                               MB_ICONWARNING | MB_YESNO);
                    if (msgResult == IDYES)
                    {
                        ListStoredBooks_Delete(idOfBook);
                        ListStoredBooks_Save();
                        FillList(mainHWND, sourceSelected);
                    }
                }
                break;

            case 2: // Użytkownicy: usuń użytkownika (z ostrzeżeniem)
                if (HIWORD(wParam) == BN_CLICKED)
                {
                    HWND list = GetDlgItem(hwnd, IDC_MainList1);
                    int indexSelected = (int)SendMessage(list, LB_GETCURSEL, 0, 0);
                    int idOfuser = (int)SendMessage(list, LB_GETITEMDATA, indexSelected, 0);
                    if (idOfuser == -1)
                    {
                        return MessageBox(hwnd, "Wybierz czytelnika", "Błąd", MB_OK);
                    }

                    int msgResult = MessageBox(hwnd,
                                               "Czy jesteś pewny, że chcesz usunąć wybranego czytelnika?\nWszystkie wypożyczone przez niego książki przepadną!",
                                               "Usuń czytelnika",
                                               MB_ICONWARNING | MB_YESNO);
                    if (msgResult == IDYES)
                    {
                        ListLibraryUsers_Delete(idOfuser);
                        // TODO: usuwanie wypożyczonych książek powiązanych z użytkownikiem
                        ListLibraryUsers_Save();
                        FillList(mainHWND, sourceSelected);
                    }
                }
                break;
            }
        }
        break;
        }
        break; // WM_COMMAND

    case WM_CLOSE:
        // Zamknięcie aplikacji
        EndDialog(hwnd, 0);
        break;

    default:
        return FALSE;
    }
    return TRUE;
}
