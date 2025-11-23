#ifndef OBJECTS_H
#define OBJECTS_H

// BOOK  - Base for storedBook and borrowedBook
typedef struct Book{
    int id;
    char name[100];
    char author[100];
    char type[50];
    int initialized;
} Book;

// StoredBook
typedef struct StoredBook{
    struct Book; //this extension requires -fms-extensions in C compiler flags
    int quantity;
    char placeInLibrary[50];
} StoredBook;

void storedBook_Init(StoredBook *self, int id, const char* name, const char* author, const char* type, int quantity, const char* placeInLibrary);
char *storedBook_ToString(StoredBook *self);

// BorrowedBook
typedef struct BorrowedBook{
    struct Book; //this extension requires -fms-extensions in C compiler flags
    int user_id;
} BorrowedBook;

void borrowedBook_Init(BorrowedBook *self, int id, const char* name, const char* author, const char* type, int user_id);
char *borrowedBook_ToString(BorrowedBook *self);

//Client of the library
typedef struct LibraryUser{
    int id;
    int number;
    char name[100];
    char surname[100];
}LibraryUser;

void libraryUser_Init(LibraryUser *self, int id, int number, const char* name, const char* surname);
char *libraryUser_ToString(LibraryUser *self);

#endif


