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
                strncpy(ListStoredBooks[id].name, name, sizeof(ListStoredBooks[id].name) - 1);
                strncpy(ListStoredBooks[id].author, author, sizeof(ListStoredBooks[id].author) - 1);
                strncpy(ListStoredBooks[id].type, type, sizeof(ListStoredBooks[id].type) - 1);
                ListStoredBooks[id].quantity = quantityINT;
                strncpy(ListStoredBooks[id].placeInLibrary, place, sizeof(ListStoredBooks[id].placeInLibrary) - 1);
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
                strncpy(ListLibraryUsers[id].name, name, sizeof(ListLibraryUsers[id].name) - 1);
                strncpy(ListLibraryUsers[id].surname, surname, sizeof(ListLibraryUsers[id].surname) - 1);
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
        }

        case IDC_ACTION_ADD:
            if (HIWORD(wParam) == BN_CLICKED)
                HandleActionAdd(hwnd, sourceSelected);
            break;

        case IDC_ACTION_EDIT:
            if (HIWORD(wParam) == BN_CLICKED)
                HandleActionEdit(hwnd, sourceSelected);
            break;

        case IDC_ACTION_RETURN:
            if (HIWORD(wParam) == BN_CLICKED)
                HandleActionReturn(hwnd, sourceSelected);
            break;

        case IDC_ACTION_BORROW:
            if (HIWORD(wParam) == BN_CLICKED)
                HandleActionBorrow(hwnd, sourceSelected);
            break;


        case IDC_ACTION_DELETE:
            if (HIWORD(wParam) == BN_CLICKED)
                HandleActionDelete(hwnd, sourceSelected);
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
