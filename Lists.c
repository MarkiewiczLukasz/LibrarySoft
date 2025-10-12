#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Lists.h"
#define MAX_BOOKS 500
#define MAX_LINE_LENGTH 100
//Last indexes of Lists
int ListStoredBooksLastIndex = -1;
int ListBorrowedBooksLastIndex = -1;
int ListLibraryUsersLastIndex = -1;
//Lists' file names
char ListStoredBooksFileName[] = "ListStoredBooks.DAT";
char ListBorrowedBooksFileName[] = "ListBorrowedBooks.DAT";
char ListLibraryUsersFileName[] = "ListLibraryUsers.DAT";

//INIT ***
int ListStoredBooks_Init(){
	
	ListStoredBooksLastIndex = -1;
	char line[MAX_LINE_LENGTH + 1];
	FILE * fptr;
	char * item;
	int index = 0;
	char delim[] = ";";
	fptr = fopen(ListStoredBooksFileName,"r");
	while (fgets(line,MAX_LINE_LENGTH,fptr)) {
		StoredBook storedBook;
        item = strtok(line,delim);
        sscanf(item, "%d", &storedBook.id);
		
        item = strtok(NULL,delim);
        storedBook.name = malloc(sizeof(item) +1);
		strcpy(storedBook.name,item);
		
		item = strtok(NULL,delim);
        storedBook.author = malloc(sizeof(item) +1);
		strcpy(storedBook.author,item);

        item = strtok(NULL,delim);
        storedBook.type = malloc(sizeof(item) +1);
        strcpy(storedBook.type,item);
		
		item = strtok(NULL,delim);
		sscanf(item, "%d", &storedBook.quantity);
        
        item = strtok(NULL,"\n");
        storedBook.placeInLibrary = malloc(sizeof(item) +1);
        strcpy(storedBook.placeInLibrary,item);
        storedBook.initialized = 1;
        
        index++;
        ListStoredBooks_Add(&storedBook);
    }
    fclose(fptr);
	return 1;
}
int ListBorrowedBooks_Init(){
	ListBorrowedBooksLastIndex = -1;
	char line[MAX_LINE_LENGTH + 1];
	FILE * fptr;
	char * item;
	int index = 0;
	char delim[] = ";";
	fptr = fopen(ListBorrowedBooksFileName,"r");
	while (fgets(line,MAX_LINE_LENGTH,fptr)) {
		BorrowedBook borrowedBook;
        item = strtok(line,delim);
        sscanf(item, "%d", &borrowedBook.id);
		
        item = strtok(NULL,delim);
        borrowedBook.name = malloc(sizeof(item) +1);
		strcpy(borrowedBook.name, item);
		
		item = strtok(NULL,delim);
        borrowedBook.author = malloc(sizeof(item) +1);
		strcpy(borrowedBook.author, item);

        item = strtok(NULL,delim);
        borrowedBook.type = malloc(sizeof(item) +1);
        strcpy(borrowedBook.type, item);
		
		item = strtok(NULL,"\n");
		sscanf(item, "%d", &borrowedBook.user_id);
        
        borrowedBook.initialized = 1;
        
        index++;
        ListBorrowedBooks_Add(&borrowedBook);
    }
    fclose(fptr);
	return 1;
}
int ListLibraryUsers_Init(){
	ListLibraryUsersLastIndex = -1;
	char line[MAX_LINE_LENGTH + 1];
	FILE * fptr;
	char * item;
	int index = 0;
	char delim[] = ";";
	fptr = fopen(ListLibraryUsersFileName,"r");
	while (fgets(line,MAX_LINE_LENGTH,fptr)) {
		if (line != " "){
			LibraryUser libraryUser;
	        item = strtok(line,delim);
	        sscanf(item, "%d", &libraryUser.id);
			
			item = strtok(NULL,delim);
	        sscanf(item, "%d", &libraryUser.number);
	        
	        item = strtok(NULL,delim);
	        libraryUser.name = malloc(sizeof(item) +1);
			strcpy(libraryUser.name,item);
			
			item = strtok(NULL,"\n");
	        libraryUser.surname = malloc(sizeof(item) +1);
			strcpy(libraryUser.surname, item);
	        
	        index++;
	        ListLibraryUsers_Add(&libraryUser);
		}
		
    }
    fclose(fptr);
	return 1;
}

//ADD
int ListStoredBooks_Add(StoredBook *storedBook){
	if (storedBook){
		int index = ListStoredBooks_FindName(storedBook->name);
		if (index != -1 && strcmp (ListStoredBooks[index].author, storedBook->author) == 0){
			ListStoredBooks[index].quantity += storedBook->quantity;
		}else{
			ListStoredBooksLastIndex++;
			ListStoredBooks[ListStoredBooksLastIndex] = *storedBook;	
		}
		return 1;
	}else return 0;
}
int ListLibraryUsers_Add(LibraryUser *libraryUser){
	if (libraryUser){
			ListLibraryUsersLastIndex++;
			ListLibraryUsers[ListLibraryUsersLastIndex] = *libraryUser;	
	}else return 0;
}

int ListBorrowedBooks_Add(BorrowedBook *borrowedBook){
	if (borrowedBook){
			ListBorrowedBooksLastIndex++;
			ListBorrowedBooks[ListBorrowedBooksLastIndex] = *borrowedBook;	
	}else return 0;
}

//DELETE
int ListStoredBooks_Delete(int index){
	if (index <= ListStoredBooksLastIndex && index >= 0){
		if(ListStoredBooks[index].quantity > 1){
			ListStoredBooks[index].quantity--;
		}else{
			int x = index;
			for (x; x<ListStoredBooksLastIndex; x++){
				ListStoredBooks[x] = ListStoredBooks[x+1];
			}
			ListStoredBooksLastIndex--;
		}
		return 1;
	}else return 0;
}
int ListLibraryUsers_Delete(int index){
	// DELETE BORROWED BOOKS BY THIS USER
	int y = 0;
	for (y; y<=ListBorrowedBooksLastIndex; y++){
		if (ListBorrowedBooks[y].user_id == ListLibraryUsers[index].id)
			ListBorrowedBooks_Delete(y);
	}
	ListBorrowedBooks_Save();
	
	if (index <= ListLibraryUsersLastIndex && index >= 0){
		int x = index;
		for (x; x<ListLibraryUsersLastIndex; x++){
			ListLibraryUsers[x] = ListLibraryUsers[x+1];
		}
		ListLibraryUsersLastIndex--;
	}else if (index == ListLibraryUsersLastIndex) 
		ListLibraryUsersLastIndex--;
		
	
	return 1;
}
int ListBorrowedBooks_Delete(int index){
	if (index <= ListBorrowedBooksLastIndex && index >= 0){
		int x = index;
		for (x; x<ListBorrowedBooksLastIndex; x++){
			ListBorrowedBooks[x] = ListBorrowedBooks[x+1];
		}
		ListBorrowedBooksLastIndex--;
	}else if (index == ListBorrowedBooksLastIndex) 
		ListBorrowedBooksLastIndex--;
	return 1;
}
// FIND
int ListStoredBooks_FindName(char *name){
	if (name != "")	{
		int x = 0;

		for(x;x<=ListStoredBooksLastIndex;x++){
			if (strcmp (ListStoredBooks[x].name, name) == 0) return x;
		}
		return -1;
	}	
	else return -1;
}

//SAVE
int ListStoredBooks_Save(){
	fclose(fopen(ListStoredBooksFileName, "w")); //CLEAN FILE
	int i = 0;
	FILE *fptr;
	fptr = fopen(ListStoredBooksFileName, "a");
	for (i;i<=ListStoredBooksLastIndex; i++){
		StoredBook s = ListStoredBooks[i];
		char *string = storedBook_ToString(&s);
		fwrite(string, strlen(string),1, fptr);
		free(string);
	}
	fclose(fptr);
	return 1;
}
int ListLibraryUsers_Save(){
	fclose(fopen(ListLibraryUsersFileName, "w")); //CLEAN FILE
	int i = 0;
	FILE *fptr;
	fptr = fopen(ListLibraryUsersFileName, "a");
	for (i;i<=ListLibraryUsersLastIndex; i++){
		LibraryUser s = ListLibraryUsers[i];
		char *string = libraryUser_ToString(&s);
		fwrite(string, strlen(string), 1, fptr);
		free(string);
	}
	fclose(fptr);
	return 1;
}
int ListBorrowedBooks_Save(){
	fclose(fopen(ListBorrowedBooksFileName, "w")); //CLEAN FILE
	int i = 0;
	FILE *fptr;
	fptr = fopen(ListBorrowedBooksFileName, "a");
	for (i;i<=ListBorrowedBooksLastIndex; i++){
		BorrowedBook s = ListBorrowedBooks[i];
		char *string = borrowedBook_ToString(&s);
		fwrite(string, strlen(string),1, fptr);
		free(string);
	}
	fclose(fptr);
	return 1;
}

// FREE
int ListStoredBooks_Free(){
	int i = 0;
	for (i;i<=ListStoredBooksLastIndex; i++){
		StoredBook s = ListStoredBooks[i];
		s.id = 0;
		free(s.name);
		free(s.author);
		free(s.type);
		s.quantity = 0;
		free(s.placeInLibrary);
	}
	ListStoredBooksLastIndex = -1;
}

//FIND
int ListLibraryUsers_FindId(int id){
	int x = 0;
	for (x; x<= ListLibraryUsersLastIndex; x++){
		if (ListLibraryUsers[x].id == id) return x;
	}
}



