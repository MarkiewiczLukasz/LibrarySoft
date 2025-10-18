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
        ButtonShowAndSetText(hwnd, IDC_ACTION1, "Dodaj książkę");
        ButtonShowAndSetText(hwnd, IDC_ACTION2, "Edytuj książkę");
        ButtonShowAndSetText(hwnd, IDC_ACTION4, "Wypożycz książkę");
        ButtonShowAndSetText(hwnd, IDC_ACTION6, "Usuń książkę");
        break;
    case 1: // Wypożyczone
        ButtonShowAndSetText(hwnd, IDC_ACTION3, "Zwróć książkę");
        break;
    case 2: // Czytelnicy
        ButtonShowAndSetText(hwnd, IDC_ACTION1, "Dodaj czytelnika");
        ButtonShowAndSetText(hwnd, IDC_ACTION2, "Edytuj czytelnika");
        ButtonShowAndSetText(hwnd, IDC_ACTION6, "Usuń czytelnika");
        break;
    default:
        return 0;
    }
    return 1;
}

int SetAllActionButtonsNotVisible(HWND hwnd) {
    // Zakładamy, że ID akcji to 5000..5006 (IDC_ACTION1..IDC_ACTION6)
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
