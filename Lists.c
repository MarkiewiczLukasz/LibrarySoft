#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> // dla GetModuleFileNameA
#include "headers/lists.h"

#define MAX_BOOKS 500
#define MAX_LINE_LENGTH 256

// Ostatnie indeksy list
int ListStoredBooksLastIndex = -1;
int ListBorrowedBooksLastIndex = -1;
int ListLibraryUsersLastIndex = -1;

StoredBook   ListStoredBooks[MAX_BOOKS];
BorrowedBook ListBorrowedBooks[MAX_BOOKS];
LibraryUser  ListLibraryUsers[MAX_BOOKS];

// Pełne ścieżki do plików (ustawiane dynamicznie na podstawie folderu EXE/Data)
static char ListStoredBooksFilePath[MAX_PATH];
static char ListBorrowedBooksFilePath[MAX_PATH];
static char ListLibraryUsersFilePath[MAX_PATH];

// Pomocnicze: zwróć katalog, w którym znajduje się EXE
static void get_exe_dir(char *out, size_t out_sz) {
    char buf[MAX_PATH];
    DWORD n = GetModuleFileNameA(NULL, buf, (DWORD)sizeof(buf));
    if (n == 0 || n >= sizeof(buf)) { out[0] = '\0'; return; }
    char *last = strrchr(buf, '\\');
    if (!last) { out[0] = '\0'; return; }
    size_t len = (size_t)(last - buf + 1);
    if (len >= out_sz) len = out_sz - 1;
    memcpy(out, buf, len);
    out[len] = '\0';
}

// Ustaw ścieżki do plików w podkatalogu "Data" obok EXE
static void Lists_InitPaths(void) {
    char exe_dir[MAX_PATH];
    get_exe_dir(exe_dir, sizeof(exe_dir));
    snprintf(ListStoredBooksFilePath, sizeof(ListStoredBooksFilePath), "%sData\\ListStoredBooks.DAT", exe_dir);
    snprintf(ListBorrowedBooksFilePath, sizeof(ListBorrowedBooksFilePath), "%sData\\ListBorrowedBooks.DAT", exe_dir);
    snprintf(ListLibraryUsersFilePath, sizeof(ListLibraryUsersFilePath), "%sData\\ListLibraryUsers.DAT", exe_dir);
}

// === INIT ===
int ListStoredBooks_Init() {
    Lists_InitPaths();
    ListStoredBooksLastIndex = -1;

    FILE *fptr = fopen(ListStoredBooksFilePath, "r");
    if (!fptr) return 0;

    char line[MAX_LINE_LENGTH + 1];
    while (fgets(line, sizeof(line), fptr)) {
        StoredBook book;
        memset(&book, 0, sizeof(book));
        
        if (sscanf(line, "%d;%99[^;];%99[^;];%49[^;];%d;%49[^\n]",
                   &book.id, book.name, book.author, book.type,
                   &book.quantity, book.placeInLibrary) >= 5) {
            book.initialized = 1;
            ListStoredBooks_Add(&book);
        }
    }
    fclose(fptr);
    return 1;
}

int ListBorrowedBooks_Init() {
    Lists_InitPaths();
    ListBorrowedBooksLastIndex = -1;

    FILE *fptr = fopen(ListBorrowedBooksFilePath, "r");
    if (!fptr) return 0;

    char line[MAX_LINE_LENGTH + 1];
    while (fgets(line, sizeof(line), fptr)) {
        BorrowedBook book;
        memset(&book, 0, sizeof(book));
        
        if (sscanf(line, "%d;%99[^;];%99[^;];%49[^;];%d",
                   &book.id, book.name, book.author, book.type,
                   &book.user_id) == 5) {
            book.initialized = 1;
            ListBorrowedBooks_Add(&book);
        }
    }
    fclose(fptr);
    return 1;
}

int ListLibraryUsers_Init() {
    Lists_InitPaths();
    ListLibraryUsersLastIndex = -1;

    FILE *fptr = fopen(ListLibraryUsersFilePath, "r");
    if (!fptr) return 0;

    char line[MAX_LINE_LENGTH + 1];
    while (fgets(line, sizeof(line), fptr)) {
        LibraryUser user;
        memset(&user, 0, sizeof(user));
        
        if (sscanf(line, "%d;%d;%99[^;];%99[^\n]",
                   &user.id, &user.number, user.name, user.surname) >= 3) {
            ListLibraryUsers_Add(&user);
        }
    }
    fclose(fptr);
    return 1;
}

// === ADD ===
int ListStoredBooks_Add(StoredBook *storedBook) {
    if (!storedBook) return 0;
    int idx = ListStoredBooks_FindName(storedBook->name);
    if (idx != -1 && strcmp(ListStoredBooks[idx].author, storedBook->author) == 0) {
        ListStoredBooks[idx].quantity += storedBook->quantity;
    } else {
        ListStoredBooksLastIndex++;
        ListStoredBooks[ListStoredBooksLastIndex] = *storedBook;
    }
    return 1;
}

int ListLibraryUsers_Add(LibraryUser *libraryUser) {
    if (!libraryUser) return 0;
    ListLibraryUsersLastIndex++;
    ListLibraryUsers[ListLibraryUsersLastIndex] = *libraryUser;
    return 1;
}

int ListBorrowedBooks_Add(BorrowedBook *borrowedBook) {
    if (!borrowedBook) return 0;
    ListBorrowedBooksLastIndex++;
    ListBorrowedBooks[ListBorrowedBooksLastIndex] = *borrowedBook;
    return 1;
}

// === DELETE ===
int ListStoredBooks_Delete(int index) {
    if (index < 0 || index > ListStoredBooksLastIndex) return 0;

    for (int x = index; x < ListStoredBooksLastIndex; x++) {
        ListStoredBooks[x] = ListStoredBooks[x + 1];
    }
    ListStoredBooksLastIndex--;
    
    return 1;
}

int ListLibraryUsers_Delete(int index) {
    if (index < 0 || index > ListLibraryUsersLastIndex) return 0;

    // Usuń wypożyczenia tego użytkownika
    for (int y = 0; y <= ListBorrowedBooksLastIndex; ) {
        if (ListBorrowedBooks[y].user_id == ListLibraryUsers[index].id) {
            ListBorrowedBooks_Delete(y);
        } else {
            y++;
        }
    }
    ListBorrowedBooks_Save();

    for (int x = index; x < ListLibraryUsersLastIndex; x++) {
        ListLibraryUsers[x] = ListLibraryUsers[x + 1];
    }
    ListLibraryUsersLastIndex--;
    return 1;
}

int ListBorrowedBooks_Delete(int index) {
    if (index < 0 || index > ListBorrowedBooksLastIndex) return 0;
    for (int x = index; x < ListBorrowedBooksLastIndex; x++) {
        ListBorrowedBooks[x] = ListBorrowedBooks[x + 1];
    }
    ListBorrowedBooksLastIndex--;
    return 1;
}

// === FIND ===
int ListStoredBooks_FindName(char *name) {
    if (!name || !*name) return -1;
    for (int x = 0; x <= ListStoredBooksLastIndex; x++) {
        if (strcmp(ListStoredBooks[x].name, name) == 0) return x;
    }
    return -1;
}

// === SAVE ===
int ListStoredBooks_Save(void) {
    Lists_InitPaths();
    FILE *f = fopen(ListStoredBooksFilePath, "w");
    if (!f) return 0;
    
    for (int i = 0; i <= ListStoredBooksLastIndex; i++) {
        fprintf(f, "%d;%s;%s;%s;%d;%s\n",
                ListStoredBooks[i].id,
                ListStoredBooks[i].name,
                ListStoredBooks[i].author,
                ListStoredBooks[i].type,
                ListStoredBooks[i].quantity,
                ListStoredBooks[i].placeInLibrary);
    }
    fclose(f);
    return 1;
}

int ListBorrowedBooks_Save(void) {
    Lists_InitPaths();
    FILE *f = fopen(ListBorrowedBooksFilePath, "w");
    if (!f) return 0;
    
    for (int i = 0; i <= ListBorrowedBooksLastIndex; i++) {
        fprintf(f, "%d;%s;%s;%s;%d\n",
                ListBorrowedBooks[i].id,
                ListBorrowedBooks[i].name,
                ListBorrowedBooks[i].author,
                ListBorrowedBooks[i].type,
                ListBorrowedBooks[i].user_id);
    }
    fclose(f);
    return 1;
}

int ListLibraryUsers_Save(void) {
    Lists_InitPaths();
    FILE *f = fopen(ListLibraryUsersFilePath, "w");
    if (!f) return 0;
    
    for (int i = 0; i <= ListLibraryUsersLastIndex; i++) {
        fprintf(f, "%d;%d;%s;%s\n",
                ListLibraryUsers[i].id,
                ListLibraryUsers[i].number,
                ListLibraryUsers[i].name,
                ListLibraryUsers[i].surname);
    }
    fclose(f);
    return 1;
}

// === FREE ===
int ListStoredBooks_Free() {
    ListStoredBooksLastIndex = -1;
    return 1;
}

// === FIND by ID ===
int ListLibraryUsers_FindId(int id) {
    for (int x = 0; x <= ListLibraryUsersLastIndex; x++) {
        if (ListLibraryUsers[x].id == id) return x;
    }
    return -1;
}
