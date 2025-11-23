#ifndef LISTS_H
#define LISTS_H
#include "objects.h"
#include <stdbool.h>
#define MAX_BOOKS 500

extern int ListStoredBooksLastIndex;
extern int ListBorrowedBooksLastIndex;
extern int ListLibraryUsersLastIndex;

extern StoredBook   ListStoredBooks[MAX_BOOKS];
extern BorrowedBook ListBorrowedBooks[MAX_BOOKS];
extern LibraryUser  ListLibraryUsers[MAX_BOOKS];

//Init lists - read data from files and create structs
int ListStoredBooks_Init();
int ListBorrowedBooks_Init();
int ListLibraryUsers_Init();

// Add item to the list
int ListStoredBooks_Add(StoredBook *storedBook);
int ListBorrowedBooks_Add(BorrowedBook *borrowedBook);
int ListLibraryUsers_Add(LibraryUser *user);

// Delete item from the list
int ListStoredBooks_Delete(int id, bool deleteAll);
int ListBorrowedBooks_Delete(int id);
int ListLibraryUsers_Delete(int id);

//Return id
int ListStoredBooks_FindName(char *name);
int ListBorrowedBooks_FindName(char *name);
int ListLibraryUsers_FindName(char *name);

//Save LIST to file
int ListStoredBooks_Save();
int ListBorrowedBooks_Save();
int ListLibraryUsers_Save();

// Lists free
int ListStoredBooks_Free();
int ListBorrowedBooks_Free();
int ListLibraryUsers_Free();

//find
int ListLibraryUsers_FindId(int id);

#endif