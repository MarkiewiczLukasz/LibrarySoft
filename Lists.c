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
    // znajdź ostatni separator katalogów
    char *last = strrchr(buf, '\\');
    if (!last) { out[0] = '\0'; return; }
    size_t len = (size_t)(last - buf + 1); // z końcowym '\\'
    if (len >= out_sz) len = out_sz - 1;
    memcpy(out, buf, len);
    out[len] = '\0';
}

// Ustaw ścieżki do plików w podkatalogu "Data" obok EXE
static void Lists_InitPaths(void) {
    char exe_dir[MAX_PATH];
    get_exe_dir(exe_dir, sizeof(exe_dir));

    // Zbuduj ścieżki Data\\*.DAT
    snprintf(ListStoredBooksFilePath, sizeof(ListStoredBooksFilePath), "%sData\\ListStoredBooks.DAT", exe_dir);
    snprintf(ListBorrowedBooksFilePath, sizeof(ListBorrowedBooksFilePath), "%sData\\ListBorrowedBooks.DAT", exe_dir);
    snprintf(ListLibraryUsersFilePath, sizeof(ListLibraryUsersFilePath), "%sData\\ListLibraryUsers.DAT", exe_dir);
}

// bezpieczne duplikowanie łańcucha (zabezpieczenie przed NULL)
static char *dup_str(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = (char*)malloc(len + 1);
    if (!p) return NULL;
    memcpy(p, s, len + 1);
    return p;
}

// === INIT ===
int ListStoredBooks_Init() {
    Lists_InitPaths();
    ListStoredBooksLastIndex = -1;

    FILE *fptr = fopen(ListStoredBooksFilePath, "r");
    if (!fptr) return 0; // brak pliku = brak danych

    char line[MAX_LINE_LENGTH + 1];
    const char *delim = ";";

    while (fgets(line, sizeof(line), fptr)) {
        // pomiń puste linie
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') continue;

        StoredBook storedBook; memset(&storedBook, 0, sizeof(storedBook));

        char *ctx = NULL;
        char *item = strtok_s(line, delim, &ctx);
        if (!item) continue; storedBook.id = atoi(item);

        item = strtok_s(NULL, delim, &ctx);
        storedBook.name = dup_str(item);

        item = strtok_s(NULL, delim, &ctx);
        storedBook.author = dup_str(item);

        item = strtok_s(NULL, delim, &ctx);
        storedBook.type = dup_str(item);

        item = strtok_s(NULL, delim, &ctx);
        storedBook.quantity = item ? atoi(item) : 0;

        item = strtok_s(NULL, "\n", &ctx);
        storedBook.placeInLibrary = dup_str(item ? item : "");

        storedBook.initialized = 1;
        ListStoredBooks_Add(&storedBook);
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
    const char *delim = ";";

    while (fgets(line, sizeof(line), fptr)) {
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') continue;

        BorrowedBook borrowedBook; memset(&borrowedBook, 0, sizeof(borrowedBook));

        char *ctx = NULL;
        char *item = strtok_s(line, delim, &ctx);
        if (!item) continue; borrowedBook.id = atoi(item);

        item = strtok_s(NULL, delim, &ctx);
        borrowedBook.name = dup_str(item);

        item = strtok_s(NULL, delim, &ctx);
        borrowedBook.author = dup_str(item);

        item = strtok_s(NULL, delim, &ctx);
        borrowedBook.type = dup_str(item);

        item = strtok_s(NULL, "\n", &ctx);
        borrowedBook.user_id = item ? atoi(item) : 0;

        borrowedBook.initialized = 1;
        ListBorrowedBooks_Add(&borrowedBook);
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
    const char *delim = ";";

    while (fgets(line, sizeof(line), fptr)) {
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') continue;

        LibraryUser libraryUser; memset(&libraryUser, 0, sizeof(libraryUser));

        char *ctx = NULL;
        char *item = strtok_s(line, delim, &ctx);
        if (!item) continue; libraryUser.id = atoi(item);

        item = strtok_s(NULL, delim, &ctx);
        libraryUser.number = item ? atoi(item) : 0;

        item = strtok_s(NULL, delim, &ctx);
        libraryUser.name = dup_str(item);

        item = strtok_s(NULL, "\n", &ctx);
        libraryUser.surname = dup_str(item ? item : "");

        ListLibraryUsers_Add(&libraryUser);
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
    if (ListStoredBooks[index].quantity > 1) {
        ListStoredBooks[index].quantity--;
    } else {
        for (int x = index; x < ListStoredBooksLastIndex; x++) {
            ListStoredBooks[x] = ListStoredBooks[x + 1];
        }
        ListStoredBooksLastIndex--;
    }
    return 1;
}

int ListLibraryUsers_Delete(int index) {
    if (index < 0 || index > ListLibraryUsersLastIndex) return 0;

    // Usuń wypożyczenia tego użytkownika
    for (int y = 0; y <= ListBorrowedBooksLastIndex; ) {
        if (ListBorrowedBooks[y].user_id == ListLibraryUsers[index].id) {
            ListBorrowedBooks_Delete(y); // nie inkrementujemy y — elementy się przesuwają
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
static int replace_file_atomic(const char *final_path, const char *tmp_path) {
    char bak_path[MAX_PATH];
    snprintf(bak_path, sizeof(bak_path), "%s.bak", final_path);
    remove(bak_path);
    rename(final_path, bak_path);
    remove(final_path);
    if (rename(tmp_path, final_path) != 0) {
        rename(bak_path, final_path);
        remove(tmp_path);
        return 0;
    }
    return 1;
}

int ListStoredBooks_Save(void) {
    Lists_InitPaths();
    char tmp[MAX_PATH];
    snprintf(tmp, sizeof(tmp), "%s.tmp", ListStoredBooksFilePath);
    FILE *f = fopen(tmp, "w"); if (!f) return 0;
    for (int i = 0; i <= ListStoredBooksLastIndex; i++) {
        char *line = storedBook_ToString(&ListStoredBooks[i]);
        if (!line) { fclose(f); remove(tmp); return 0; }
        fwrite(line, strlen(line), 1, f); free(line);
    }
    fclose(f);
    return replace_file_atomic(ListStoredBooksFilePath, tmp);
}

int ListBorrowedBooks_Save(void) {
    Lists_InitPaths();
    char tmp[MAX_PATH];
    snprintf(tmp, sizeof(tmp), "%s.tmp", ListBorrowedBooksFilePath);
    FILE *f = fopen(tmp, "w"); if (!f) return 0;
    for (int i = 0; i <= ListBorrowedBooksLastIndex; i++) {
        char *line = borrowedBook_ToString(&ListBorrowedBooks[i]);
        if (!line) { fclose(f); remove(tmp); return 0; }
        fwrite(line, strlen(line), 1, f); free(line);
    }
    fclose(f);
    return replace_file_atomic(ListBorrowedBooksFilePath, tmp);
}

int ListLibraryUsers_Save(void) {
    Lists_InitPaths();
    char tmp[MAX_PATH];
    snprintf(tmp, sizeof(tmp), "%s.tmp", ListLibraryUsersFilePath);
    FILE *f = fopen(tmp, "w"); if (!f) return 0;
    for (int i = 0; i <= ListLibraryUsersLastIndex; i++) {
        char *line = libraryUser_ToString(&ListLibraryUsers[i]);
        if (!line) { fclose(f); remove(tmp); return 0; }
        fwrite(line, strlen(line), 1, f); free(line);
    }
    fclose(f);
    return replace_file_atomic(ListLibraryUsersFilePath, tmp);
}

// === FREE ===
int ListStoredBooks_Free() {
    for (int i = 0; i <= ListStoredBooksLastIndex; i++) {
        StoredBook *s = &ListStoredBooks[i];
        s->id = 0;
        free(s->name); s->name = NULL;
        free(s->author); s->author = NULL;
        free(s->type); s->type = NULL;
        s->quantity = 0;
        free(s->placeInLibrary); s->placeInLibrary = NULL;
    }
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
