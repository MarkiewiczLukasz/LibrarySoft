#include "headers/objects.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//STORED BOOK
void storedBook_Init(StoredBook *self, int id, const char* name, const char* author, const char* type, int quantity, const char* placeInLibrary){
	self->id = id;
	strncpy(self->name, name ? name : "", sizeof(self->name) - 1);
	strncpy(self->author, author ? author : "", sizeof(self->author) - 1);
	strncpy(self->type, type ? type : "", sizeof(self->type) - 1);
	self->quantity = quantity;
	strncpy(self->placeInLibrary, placeInLibrary ? placeInLibrary : "", sizeof(self->placeInLibrary) - 1);
	self->initialized = 1;
}

char *storedBook_ToString(StoredBook *self){
	if(!self) return NULL;
	int len = snprintf(NULL, 0, "%d;%s;%s;%s;%d;%s\n",
					   self->id, self->name, self->author, self->type, 
					   self->quantity, self->placeInLibrary);
	if(len < 0) return NULL;
	char *stringRep = (char*)malloc((size_t)len + 1);
	if(!stringRep) return NULL;
	snprintf(stringRep, (size_t)len + 1, "%d;%s;%s;%s;%d;%s\n",
			 self->id, self->name, self->author, self->type,
			 self->quantity, self->placeInLibrary);
	return stringRep;
}

// STORED BOOK __ END
//BORROWED BOOK
void borrowedBook_Init(BorrowedBook *self, int id, const char* name, const char* author, const char* type, int user_id){
	self->id = id;
	strncpy(self->name, name ? name : "", sizeof(self->name) - 1);
	strncpy(self->author, author ? author : "", sizeof(self->author) - 1);
	strncpy(self->type, type ? type : "", sizeof(self->type) - 1);
	self->user_id = user_id;
	self->initialized = 1;
}

char *borrowedBook_ToString(BorrowedBook *self){
	if(!self) return NULL;
	int len = snprintf(NULL, 0, "%d;%s;%s;%s;%d\n",
					   self->id, self->name, self->author, self->type,
					   self->user_id);
	if(len < 0) return NULL;
	char *stringRep = (char*)malloc((size_t)len + 1);
	if(!stringRep) return NULL;
	snprintf(stringRep, (size_t)len + 1, "%d;%s;%s;%s;%d\n",
			 self->id, self->name, self->author, self->type,
			 self->user_id);
	return stringRep;
}

// BORROWED BOOK __ END
// LIBRARY USER
void libraryUser_Init(LibraryUser *self, int id, int number, const char* name, const char* surname){
	self->id = id;
	self->number = number;
	strncpy(self->name, name ? name : "", sizeof(self->name) - 1);
	strncpy(self->surname, surname ? surname : "", sizeof(self->surname) - 1);
}

char *libraryUser_ToString(LibraryUser *self){
	if(!self) return NULL;
	int len = snprintf(NULL, 0, "%d;%d;%s;%s\n",
					   self->id, self->number, self->name, self->surname);
	if(len < 0) return NULL;
	char *stringRep = (char*)malloc((size_t)len + 1);
	if(!stringRep) return NULL;
	snprintf(stringRep, (size_t)len + 1, "%d;%d;%s;%s\n",
			 self->id, self->number, self->name, self->surname);
	return stringRep;
}


