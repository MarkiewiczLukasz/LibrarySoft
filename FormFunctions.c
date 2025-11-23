#include "headers/formFunctions.h"
#include "headers/resource.h"
#include "headers/lists.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ROW_WIDTH 400
#define COLUMN_MAX_LEN 42
#define MAX_COLUMNS 5

// Bufory globalne (nagłówki kolumn)
static char headersString[MAX_COLUMNS * COLUMN_MAX_LEN + 1];

// Nazwy źródeł (UTF-8)
static const char sources[3][20] = {"Zasoby", "Wypożyczone", "Czytelnicy"};

// Bieżący wybór zakładki/źródła
extern int sourceSelected;

// Nagłówki kolumn dla każdego źródła (UTF-8)
static const char sourcesHeaders[3][MAX_COLUMNS][COLUMN_MAX_LEN] = {
    {"Tytuł", "Autor", "Gatunek", "Ilość", "Regał + Półka"},
    {"Tytuł", "Autor", "Gatunek", "Wypożyczona", ""},
    {"Karta biblioteczna", "Imię", "Nazwisko", "", ""}
};

// -------------------- API --------------------
int InitMainDialog(HWND hwnd) {
    // Wypełnij combobox wyboru źródła
    HWND sourceListCB = GetDlgItem(hwnd, IDC_SourceList);
    SendMessage(sourceListCB, CB_ADDSTRING, 0, (LPARAM)sources[0]);
    SendMessage(sourceListCB, CB_ADDSTRING, 0, (LPARAM)sources[1]);
    SendMessage(sourceListCB, CB_ADDSTRING, 0, (LPARAM)sources[2]);

    // Domyślnie wybierz pierwsze źródło
    SendMessage(sourceListCB, CB_SETCURSEL, (WPARAM)0, 0);

    // Ręcznie wywołaj obsługę zmiany, bo podczas inicjalizacji komunikaty mogą nie przyjść
    HandleSourceListChange(hwnd);
    return 1;
}

int HandleSourceListChange(HWND hwnd) {
    HWND sourceListCB = GetDlgItem(hwnd, IDC_SourceList);
    int cursor = (int)SendMessage(sourceListCB, CB_GETCURSEL, 0, 0);
    if (cursor < 0 || cursor > 2) cursor = 0;

    sourceSelected = cursor;

    // Odśwież listy i nagłówki
    FillList(hwnd, cursor);
    refreshColumnHeaders(cursor);
    SetDlgItemTextA(hwnd, IDC_LOADEDSOURCE, headersString); // nagłówek tabeli

    // Dostosuj widoczność i podpisy przycisków akcji
    PrepareActionButtonsForActiveSource(hwnd, cursor);
    return 1;
}

int PrepareActionButtonsForActiveSource(HWND hwnd, int selectedSource) {
    SetAllActionButtonsNotVisible(hwnd);

    switch (selectedSource) {
    case 0: // Zasoby
        ButtonShowAndSetText(hwnd, IDC_ACTION_ADD, "Dodaj książkę");
        ButtonShowAndSetText(hwnd, IDC_ACTION_EDIT, "Edytuj książkę");
        ButtonShowAndSetText(hwnd, IDC_ACTION_BORROW, "Wypożycz książkę");
        ButtonShowAndSetText(hwnd, IDC_ACTION_DELETE, "Usuń książkę");
        break;
    case 1: // Wypożyczone
        ButtonShowAndSetText(hwnd, IDC_ACTION_RETURN, "Zwróć książkę");
        break;
    case 2: // Czytelnicy
        ButtonShowAndSetText(hwnd, IDC_ACTION_ADD, "Dodaj czytelnika");
        ButtonShowAndSetText(hwnd, IDC_ACTION_EDIT, "Edytuj czytelnika");
        ButtonShowAndSetText(hwnd, IDC_ACTION_DELETE, "Usuń czytelnika");
        break;
    default:
        return 0;
    }
    return 1;
}

int SetAllActionButtonsNotVisible(HWND hwnd) {
    // Zakładamy, że ID akcji to 5000..5006 (IDC_ACTION_ADD..IDC_ACTION_DELETE)
    for (int index = 5000; index <= 5006; index++) {
        ShowWindow(GetDlgItem(hwnd, index), SW_HIDE);
    }
    return 1;
}

int ButtonShowAndSetText(HWND hwnd, int IDdglItem, const char *text) {
    HWND hBtn = GetDlgItem(hwnd, IDdglItem);
    if (!hBtn) return 0;
    ShowWindow(hBtn, SW_SHOW);
    SetWindowTextA(hBtn, text ? text : "");
    return 1;
}

int refreshColumnHeaders(int source) {
    memset(headersString, 0, sizeof(headersString));

    size_t used = 0;
    for (int column = 0; column < MAX_COLUMNS; column++) {
        char columnString[COLUMN_MAX_LEN + 1];
        // Wyrównanie do lewej na szerokość COLUMN_MAX_LEN
        // (jeśli nagłówek krótszy – dopełniamy spacją)
        snprintf(columnString, sizeof(columnString), "%-*s", COLUMN_MAX_LEN, sourcesHeaders[source][column]);

        size_t left = sizeof(headersString) - used - 1;
        if (left == 0) break;
        strncat(headersString, columnString, left);
        used = strlen(headersString);
    }
    return 1;
}

static void ResetAllLists(HWND hwnd) {
    HWND mainList1 = GetDlgItem(hwnd, IDC_MainList1);
    HWND mainList2 = GetDlgItem(hwnd, IDC_MainList2);
    HWND mainList3 = GetDlgItem(hwnd, IDC_MainList3);
    HWND mainList4 = GetDlgItem(hwnd, IDC_MainList4);
    HWND mainList5 = GetDlgItem(hwnd, IDC_MainList5);

    SendMessage(mainList1, LB_RESETCONTENT, 0, 0);
    SendMessage(mainList2, LB_RESETCONTENT, 0, 0);
    SendMessage(mainList3, LB_RESETCONTENT, 0, 0);
    SendMessage(mainList4, LB_RESETCONTENT, 0, 0);
    SendMessage(mainList5, LB_RESETCONTENT, 0, 0);
}

int FillList(HWND hwnd, int selectedSource) {
    ResetAllLists(hwnd);

    HWND mainList1 = GetDlgItem(hwnd, IDC_MainList1);
    HWND mainList2 = GetDlgItem(hwnd, IDC_MainList2);
    HWND mainList3 = GetDlgItem(hwnd, IDC_MainList3);
    HWND mainList4 = GetDlgItem(hwnd, IDC_MainList4);
    HWND mainList5 = GetDlgItem(hwnd, IDC_MainList5);

    switch (selectedSource) {
    case 0: {
        // Zasoby
        ListStoredBooks_Init();
        for (int x = 0; x <= ListStoredBooksLastIndex; x++) {
            StoredBook s = ListStoredBooks[x];
            InsertToTable(mainList1, x, s.name);
            InsertToTable(mainList2, x, s.author);
            InsertToTable(mainList3, x, s.type);

            char intAsStr[32];
            snprintf(intAsStr, sizeof(intAsStr), "%i", s.quantity);
            InsertToTable(mainList4, x, intAsStr);

            InsertToTable(mainList5, x, s.placeInLibrary);
        }
        break;
    }
    case 1: {
        // Wypożyczone
        ListBorrowedBooks_Init();
        for (int x = 0; x <= ListBorrowedBooksLastIndex; x++) {
            BorrowedBook s = ListBorrowedBooks[x];
            InsertToTable(mainList1, x, s.name);
            InsertToTable(mainList2, x, s.author);
            InsertToTable(mainList3, x, s.type);

            int userIndexWithID = ListLibraryUsers_FindId(s.user_id);
            char userInfo[256];
            if (userIndexWithID >= 0) {
                snprintf(userInfo, sizeof(userInfo), "(%d)%s %s",
                         ListLibraryUsers[userIndexWithID].number,
                         ListLibraryUsers[userIndexWithID].name,
                         ListLibraryUsers[userIndexWithID].surname);
            } else {
                snprintf(userInfo, sizeof(userInfo), "(?) Nieznany użytkownik");
            }
            InsertToTable(mainList4, x, userInfo);
        }
        break;
    }
    case 2: {
        // Użytkownicy
        ListLibraryUsers_Init();
        for (int x = 0; x <= ListLibraryUsersLastIndex; x++) {
            LibraryUser s = ListLibraryUsers[x];

            char numberStr[32];
            snprintf(numberStr, sizeof(numberStr), "%i", s.number);
            InsertToTable(mainList1, x, numberStr);
            InsertToTable(mainList2, x, s.name);
            InsertToTable(mainList3, x, s.surname);
        }
        break;
    }
    default:
        return 0;
    }
    return 1;
}

int InsertToTable(HWND hwnd, int index, const char *text) {
    if (!hwnd) return -1;
    const char *safe = text ? text : "";
    int pos = (int)SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)safe);
    if (pos >= 0)
        SendMessage(hwnd, LB_SETITEMDATA, pos, (LPARAM)index); // zapisz indeks jako item data
    return pos;
}

// === ACTION HANDLERS ===

// External globals from main.c
extern HWND hwndNewBook;
extern HWND hwndNewUser;
extern HWND hwndBorrowBook;
extern HWND mainHWND;
extern int editedStoredBookId;
extern int editedUserId;
extern int borrowingBookId;

// Forward declarations of dialog procedures from main.c
BOOL CALLBACK addStoredBookWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK addLibraryUserWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK BorrowBookWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

// === Akcja 1: Dodaj nowy rekord ===
void HandleActionAdd(HWND hwnd, int sourceSelected) {
    switch (sourceSelected) {
    case 0: // Zasoby: dodaj książkę magazynową
        if (hwndNewBook == NULL) {
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
        if (hwndNewUser == NULL) {
            editedUserId = -1;
            HINSTANCE instance = GetModuleHandle(NULL);
            hwndNewUser = CreateDialog(instance,
                                       MAKEINTRESOURCE(IDD_AddLibraryUser),
                                       hwnd,
                                       (DLGPROC)addLibraryUserWindow);
            ShowWindow(hwndNewUser, SW_SHOW);

            // Wygeneruj unikalny numer karty bibliotecznej
            HWND numberBox = GetDlgItem(hwndNewUser, IDC_NUMBER);
            int randNumber;
            int isUnique;
            do {
                randNumber = rand() % 5000;
                isUnique = 1;
                for (int i = 0; i <= ListLibraryUsersLastIndex; i++) {
                    if (ListLibraryUsers[i].number == randNumber) {
                        isUnique = 0;
                        break;
                    }
                }
            } while (!isUnique);
            char buff[32];
            snprintf(buff, sizeof(buff), "%i", randNumber);
            SetWindowTextA(numberBox, buff);
        }
        break;
    }
}

// === Akcja 2: Edytuj rekord ===
void HandleActionEdit(HWND hwnd, int sourceSelected) {
    switch (sourceSelected) {
    case 0: // Zasoby: edytuj książkę magazynową
        if (hwndNewBook == NULL) {
            HWND list = GetDlgItem(hwnd, IDC_MainList1);
            int indexSelected = (int)SendMessage(list, LB_GETCURSEL, 0, 0);
            if (indexSelected == LB_ERR) {
                MessageBox(hwnd, "Wybierz książkę", "Błąd", MB_OK);
                return;
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
        if (hwndNewUser == NULL) {
            HWND list = GetDlgItem(hwnd, IDC_MainList1);
            int indexSelected = (int)SendMessage(list, LB_GETCURSEL, 0, 0);
            if (indexSelected == LB_ERR) {
                MessageBox(hwnd, "Wybierz czytelnika", "Błąd", MB_OK);
                return;
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

// === Akcja 3: Zwróć książkę ===
void HandleActionReturn(HWND hwnd, int sourceSelected) {
    if (sourceSelected == 1) { // Wypożyczone: zwróć książkę do zasobów
        HWND list = GetDlgItem(hwnd, IDC_MainList1);
        int indexSelected = (int)SendMessage(list, LB_GETCURSEL, 0, 0);
        int idOfBook = (int)SendMessage(list, LB_GETITEMDATA, indexSelected, 0);
        if (idOfBook == -1) {
            MessageBox(hwnd, "Wybierz książkę", "Błąd", MB_OK);
            return;
        }

        int msgResult = MessageBox(hwnd,
                                   "Czy jesteś pewny, że chcesz zwrócić wybraną książkę?",
                                   "Zwróć książkę",
                                   MB_ICONWARNING | MB_YESNO);
        if (msgResult == IDYES) {
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
}

// === Akcja 4: Wypożycz książkę ===
void HandleActionBorrow(HWND hwnd, int sourceSelected) {
    if (sourceSelected == 0 && hwndBorrowBook == NULL) { // Zasoby: otwórz dialog wypożyczenia
        HWND list = GetDlgItem(hwnd, IDC_MainList1);
        int indexSelected = (int)SendMessage(list, LB_GETCURSEL, 0, 0);
        int idOfBook = (int)SendMessage(list, LB_GETITEMDATA, indexSelected, 0);
        if (idOfBook == -1) {
            MessageBox(hwnd, "Wybierz książkę", "Błąd", MB_OK);
            return;
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
        SetWindowTextA(description, "Wybierz czytelnika,\\nktóremu wypożyczana jest książka.");

        for (int x = 0; x <= ListLibraryUsersLastIndex; x++) {
            char tempStr[128];
            snprintf(tempStr, sizeof(tempStr), "(%d)%s %s", ListLibraryUsers[x].number, ListLibraryUsers[x].name, ListLibraryUsers[x].surname);
            int pos = (int)SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)tempStr);
            SendMessage(comboBox, CB_SETITEMDATA, pos, (LPARAM)x);
        }
        SendMessage(comboBox, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);
    }
}

// === Akcja 6: Usuń rekord ===
void HandleActionDelete(HWND hwnd, int sourceSelected) {
    switch (sourceSelected) {
    case 0: // Zasoby: usuń książkę
    {
        HWND list = GetDlgItem(hwnd, IDC_MainList1);
        int indexSelected = (int)SendMessage(list, LB_GETCURSEL, 0, 0);
        int idOfBook = (int)SendMessage(list, LB_GETITEMDATA, indexSelected, 0);
        if (idOfBook == -1) {
            MessageBox(hwnd, "Wybierz książkę", "Błąd", MB_OK);
            return;
        }

        int msgResult = MessageBox(hwnd,
                                   "Czy jesteś pewny, że chcesz usunąć wybraną pozycję i jej wszystkie sztuki?",
                                   "Usuń pozycję",
                                   MB_ICONWARNING | MB_YESNO);
        if (msgResult == IDYES) {
            ListStoredBooks_Delete(idOfBook);
            ListStoredBooks_Save();
            FillList(mainHWND, sourceSelected);
        }
    }
    break;

    case 2: // Użytkownicy: usuń użytkownika
    {
        HWND list = GetDlgItem(hwnd, IDC_MainList1);
        int indexSelected = (int)SendMessage(list, LB_GETCURSEL, 0, 0);
        int idOfuser = (int)SendMessage(list, LB_GETITEMDATA, indexSelected, 0);
        if (idOfuser == -1) {
            MessageBox(hwnd, "Wybierz czytelnika", "Błąd", MB_OK);
            return;
        }

        int msgResult = MessageBox(hwnd,
                                   "Czy jesteś pewny, że chcesz usunąć wybranego czytelnika?\\nWszystkie wypożyczone przez niego książki przepadną!",
                                   "Usuń czytelnika",
                                   MB_ICONWARNING | MB_YESNO);
        if (msgResult == IDYES) {
            ListLibraryUsers_Delete(idOfuser);
            ListLibraryUsers_Save();
            FillList(mainHWND, sourceSelected);
        }
    }
    break;
    }
}
