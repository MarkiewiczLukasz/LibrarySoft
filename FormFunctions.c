#include "FormFunctions.h"
#include "resource.h"
#include <stdio.h>
#include "Lists.h"
#define ROW_WIDTH 400
#define COLUMN_MAX_LEN 42
#define MAX_COLUMNS 5
char tempStr[100];
char headersString[MAX_COLUMNS*COLUMN_MAX_LEN +1];
char sources[3][20] = {"Zasoby", "Wypo¿yczone", "U¿ytkownicy"};
int sourceSelected;
char sourcesHeaders[3][MAX_COLUMNS][COLUMN_MAX_LEN] ={
									{"Tytu³", "Autor", "Gatunek", "Iloœæ", "Rega³ + Pó³ka"},
									{"Tytu³", "Autor", "Gatunek", "Wypo¿yczona", " "},	
									{"Karta biblioteczna","Imiê", "Nazwisko", " ", " "}	
							   	};
//*****************
//		MAIN FORM
//*****************
int InitMainDialog(HWND hwnd){
	//Fill source list combobox
	HWND sourceListCB = GetDlgItem(hwnd, IDC_SourceList); 
	SendMessage(sourceListCB,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) sources[0]);
	SendMessage(sourceListCB,(UINT) CB_ADDSTRING,(WPARAM) 1,(LPARAM) sources[1]);
	SendMessage(sourceListCB,(UINT) CB_ADDSTRING,(WPARAM) 2,(LPARAM) sources[2]);
	//Select First 
	SendMessage(sourceListCB, CB_SETCURSEL, (WPARAM)0, (LPARAM)0); //should be sent msg CB_SETCURSEL but it is an INIT 
	HandleSourceListChange(hwnd);//WHEN FORM IS INITIALIZED MSGS are not sent so manualy Call HandleSourceListChange
}
int HandleSourceListChange(HWND hwnd){
	HWND sourceListCB = GetDlgItem(hwnd, IDC_SourceList); 
	int cursor = (int)SendMessage((HWND) sourceListCB, (UINT) CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0);
	FillList(hwnd, cursor);
	refreshColumnHeaders(cursor);
	sourceSelected = cursor;
	SetDlgItemText(hwnd, IDC_LOADEDSOURCE, headersString); //LIST HEADER
	PrepareActionButtonsForActiveSource(hwnd, cursor);
}
int PrepareActionButtonsForActiveSource(HWND hwnd, int selectedSource){
	SetAllActionButtonsNotVisible(hwnd);
	switch(selectedSource){
		case 0:{
			ButtonShowAndSetText(hwnd, IDC_ACTION1, "Dodaj ksi¹¿kê");
			ButtonShowAndSetText(hwnd, IDC_ACTION2, "Edytuj ksi¹¿kê");
			ButtonShowAndSetText(hwnd, IDC_ACTION4, "Wypo¿ycz ksi¹¿kê");
			ButtonShowAndSetText(hwnd, IDC_ACTION6, "Usuñ ksi¹¿kê");
		}break;
		case 1:{
			ButtonShowAndSetText(hwnd, IDC_ACTION3, "Zwróæ ksi¹¿kê");
		}break;
		case 2:{
			ButtonShowAndSetText(hwnd, IDC_ACTION1, "Dodaj u¿ytkownika");
			ButtonShowAndSetText(hwnd, IDC_ACTION2, "Edytuj u¿ytkownika");
			//ButtonShowAndSetText(hwnd, IDC_ACTION4, "Zwróæ ksi¹¿kê");
			ButtonShowAndSetText(hwnd, IDC_ACTION6, "Usuñ u¿ytkownika");
		}break;
		default:
			return 0;
	break;
	}
}
int SetAllActionButtonsNotVisible(HWND hwnd){
	int index= 5000;
	int lastActionButton = 5006;
	for (index; index<=lastActionButton; index++){
		ShowWindow(GetDlgItem(hwnd, index), SW_HIDE);
	}
}
int ButtonShowAndSetText(HWND hwnd, int IDdglItem, char *text){
	ShowWindow(GetDlgItem(hwnd, IDdglItem), SW_SHOW);
	SetWindowTextA(GetDlgItem(hwnd, IDdglItem), text);
}
int refreshColumnHeaders(int source){
	memset(headersString, 0, MAX_COLUMNS*COLUMN_MAX_LEN +1);
	int column = 0;
	for (column; column<MAX_COLUMNS; column++){
		char columnString[COLUMN_MAX_LEN];
		sprintf(columnString, "%*s",-COLUMN_MAX_LEN, sourcesHeaders[source][column]);
		strcat(headersString, columnString);
	}
	return 1;
}
int FillList(HWND hwnd, int selectedSource){
	
	
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
	int x = 0;
	switch(selectedSource){
		case 0:
		{
			ListStoredBooks_Init();
			for (x; x<=ListStoredBooksLastIndex;x++){
				StoredBook s = ListStoredBooks[x];
				//Insert to tables
				InsertToTable(mainList1, x, s.name);
				InsertToTable(mainList2, x, s.author);
				InsertToTable(mainList3, x, s.type);
				char* intAsStr;// INTEGER TO STRING
				asprintf (&intAsStr, "%i", s.quantity);// INTEGER TO STRING
				InsertToTable(mainList4, x, intAsStr);
				free(intAsStr);// FREE STRING
				InsertToTable(mainList5, x, s.placeInLibrary);
			}
		}
		break;
		case 1:
		{
			ListBorrowedBooks_Init();
			for (x; x<=ListBorrowedBooksLastIndex;x++){
				BorrowedBook s = ListBorrowedBooks[x];
				//Insert to tables
				InsertToTable(mainList1, x, s.name);
				InsertToTable(mainList2, x, s.author);
				InsertToTable(mainList3, x, s.type);
				char* intAsStr;// GET USER BY ID
				int userIndexWithID = ListLibraryUsers_FindId(s.user_id);
				asprintf (&intAsStr, "(%d)%s %s", ListLibraryUsers[userIndexWithID].number, ListLibraryUsers[userIndexWithID].name, ListLibraryUsers[userIndexWithID].surname);
				InsertToTable(mainList4, x, intAsStr);
				free(intAsStr);// FREE STRING
			}
			
		}break;
		case 2:
		{
			ListLibraryUsers_Init();
			for (x; x<=ListLibraryUsersLastIndex;x++){
				LibraryUser s = ListLibraryUsers[x];
				//Insert to tables
				InsertToTable(mainList2, x, s.name);
				InsertToTable(mainList3, x, s.surname);
				char* intAsStr;// INTEGER TO STRING
				asprintf (&intAsStr, "%i", s.number);// INTEGER TO STRING
				InsertToTable(mainList1, x, intAsStr);
				free(intAsStr);// FREE STRING
			}
			
		}break;
		default:
			return 0;
		break;
	}
}
int InsertToTable(HWND hwnd, int index, char * text){
	int pos = (int)SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM) text);
	SendMessage(hwnd, LB_SETITEMDATA, pos, (LPARAM) index); //SENDING INDEX to "ITEMDATA"
}
//*****************
//		END  MAIN FORM
//*****************

