#include "headers/objects.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//STORED BOOK
void storedBook_Init(StoredBook *self, int id, char* name, char* author, char* type, int quantity, char* placeInLibrary){
	self->id = id;
	self->name = name;
	self->author = author;
	self->type = type;
	self->quantity = quantity;
	self->placeInLibrary = placeInLibrary;
	self->initialized = 1;
}

char *storedBook_ToString(StoredBook *self){
	size_t len = 200;
	sizeof self;
	snprintf(NULL, len, "%d;%s;%s;%s;%d;%s\n", self->id, self->name, self->author, self->type, self->quantity, self->placeInLibrary);
	char * stringRep = malloc(len+1);
	snprintf(stringRep, len+1, "%d;%s;%s;%s;%d;%s\n", self->id, self->name, self->author, self->type, self->quantity, self->placeInLibrary);
	printf("%s", stringRep);
	return stringRep;
}

// STORED BOOK __ END
//BORROWED BOOK
void borrowedBook_Init(BorrowedBook *self, int id, char* name, char* author, char* type, int user_id){
	self->id = id;
	self->name = name;
	self->author = author;
	self->type = type;
	self->user_id = user_id;
	self->initialized = 1;
}
char *borrowedBook_ToString(BorrowedBook *self){
	size_t len = 200;
	sizeof self;
	snprintf(NULL, len, "%d;%s;%s;%s;%d\n", self->id, self->name, self->author, self->type, self->user_id);
	char * stringRep = malloc(len+1);
	snprintf(stringRep, len+1, "%d;%s;%s;%s;%d\n", self->id, self->name, self->author, self->type, self->user_id);
	printf("%s", stringRep);
	return stringRep;
}

// BORROWED BOOK __ END
// LIBRARY USER
void libraryUser_Init(LibraryUser *self, int id, int number, char* name, char* surname){
	self->id = id;
	self->number = number;
	self->name = name;
	self->surname = surname;
}
char *libraryUser_ToString(LibraryUser *self){
	size_t len = 200;
	sizeof self;
	snprintf(NULL, len, "%d;%d;%s;%s\n", self->id, self->number, self->name, self->surname);
	char * stringRep = malloc(len+1);
	snprintf(stringRep, len, "%d;%d;%s;%s\n", self->id, self->number, self->name, self->surname);
	printf("%s", stringRep);
	return stringRep;
}

