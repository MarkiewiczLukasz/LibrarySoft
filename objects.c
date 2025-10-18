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
	if(!self) return NULL;
	int len = snprintf(NULL, 0, "%d;%s;%s;%s;%d;%s\n",
					   self->id,
					   self->name ? self->name : "",
					   self->author ? self->author : "",
					   self->type ? self->type : "",
					   self->quantity,
					   self->placeInLibrary ? self->placeInLibrary : "");
	if(len < 0) return NULL;
	char *stringRep = (char*)malloc((size_t)len + 1);
	if(!stringRep) return NULL;
	snprintf(stringRep, (size_t)len + 1, "%d;%s;%s;%s;%d;%s\n",
			 self->id,
			 self->name ? self->name : "",
			 self->author ? self->author : "",
			 self->type ? self->type : "",
			 self->quantity,
			 self->placeInLibrary ? self->placeInLibrary : "");
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
	if(!self) return NULL;
	int len = snprintf(NULL, 0, "%d;%s;%s;%s;%d\n",
					   self->id,
					   self->name ? self->name : "",
					   self->author ? self->author : "",
					   self->type ? self->type : "",
					   self->user_id);
	if(len < 0) return NULL;
	char *stringRep = (char*)malloc((size_t)len + 1);
	if(!stringRep) return NULL;
	snprintf(stringRep, (size_t)len + 1, "%d;%s;%s;%s;%d\n",
			 self->id,
			 self->name ? self->name : "",
			 self->author ? self->author : "",
			 self->type ? self->type : "",
			 self->user_id);
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
	if(!self) return NULL;
	int len = snprintf(NULL, 0, "%d;%d;%s;%s\n",
					   self->id,
					   self->number,
					   self->name ? self->name : "",
					   self->surname ? self->surname : "");
	if(len < 0) return NULL;
	char *stringRep = (char*)malloc((size_t)len + 1);
	if(!stringRep) return NULL;
	snprintf(stringRep, (size_t)len + 1, "%d;%d;%s;%s\n",
			 self->id,
			 self->number,
			 self->name ? self->name : "",
			 self->surname ? self->surname : "");
	return stringRep;
}

