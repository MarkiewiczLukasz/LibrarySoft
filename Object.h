#ifndef OBJECT_H
#define OBJECT_H

// BOOK  - Base for storedBook and borrowedBook
typedef struct Book{
	int id;
    char* name;
    char* author;
    char* type;
    int initialized;
} Book;


// StoredBook 
typedef struct StoredBook{
	struct Book; //this extenstion requires -fms-extensions in C compiler flags 
	int quantity;
	char* placeInLibrary;
} StoredBook;

void storedBook_Init(StoredBook *self, int id, char* name, char* author, char* type, int quantity, char* placeInLibrary);
char *storedBook_ToString(StoredBook *self);

// BorrowedBook 
typedef struct BorrowedBook{
	struct Book; //this extenstion requires -fms-extensions in C compiler flags
	int user_id;
} BorrowedBook;

void borrowedBook_Init(BorrowedBook *self, int id, char* name, char* author, char* type, int user_id);
char *borrowedBook_ToString(BorrowedBook *self);

//Client of the library
typedef struct LibraryUser{
	int id;
	int number;
	char* name;
	char* surname;
}LibraryUser;

void libraryUser_Init(LibraryUser *self, int id, int number, char* name, char* surname);
char *libraryUser_ToString(LibraryUser *self);




#endif




