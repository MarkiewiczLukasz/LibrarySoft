#include "FormFunctions.h"
#include "Lists.h"
#include "resource.h" 
#include <stdio.h>

HWND hwndNewBook = NULL;  // Window handle of dialog box 
HWND hwndNewUser = NULL;  // Window handle of dialog box 
HWND hwndBorrowBook = NULL;
HWND Mainhwnd; //MAIN handler
// WINDOW CALLBACKS DECLARATION
BOOL CALLBACK addStoredBookWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK mainWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK addLibraryUserWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK BorrowBookWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
int editedStoredBookId = -1;
int editedUserId = -1;
int sourceSelected = -1;
int borrowingBookId = -1;



// MAIN RUN
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDLG), NULL, (DLGPROC)mainWindow);
}

BorrowBookWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
		case WM_INITDIALOG:
		{
			
			break;
		}
				
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDOK:
				{
					HWND list = GetDlgItem(Mainhwnd, IDC_MainList1); 
					HWND comboBox = GetDlgItem(hwndBorrowBook, IDC_CB);
					int indexSelected = SendMessage(comboBox, CB_GETCURSEL, 0, 0); //check selection of changed list
					if (indexSelected == LB_ERR){ 
						borrowingBookId = -1;
						return MessageBox(hwnd, "Wybierz u¿ytkownika", "B³¹d", MB_OK); 
                	}
                	int idOfUser = SendMessage(comboBox, CB_GETITEMDATA, indexSelected, 0); //Get selected ID
                	BorrowedBook borrowed;
					borrowedBook_Init(&borrowed, ListBorrowedBooks[ListBorrowedBooksLastIndex].id + 1, ListStoredBooks[borrowingBookId].name, ListStoredBooks[borrowingBookId].author, ListStoredBooks[borrowingBookId].type, ListLibraryUsers[idOfUser].id);
					ListBorrowedBooks_Add(&borrowed);
					
					ListBorrowedBooks_Save();
					ListStoredBooks_Delete(borrowingBookId);
					ListStoredBooks_Save();
					FillList(Mainhwnd, sourceSelected);
					EndDialog(hwndBorrowBook, 0);
					hwndBorrowBook = NULL;
					borrowingBookId = -1;
				}
				break;
				case IDCANCEL:
				{
					borrowingBookId = -1;
					EndDialog(hwndBorrowBook, 0);
					hwndBorrowBook = NULL;
				}
				break;
			}
		break;
		case WM_CLOSE:
			borrowingBookId = -1;
			EndDialog(hwndBorrowBook, 0);
			hwndBorrowBook = NULL;
		break;
		default:
			return FALSE;
	}
	return TRUE;
}
BOOL CALLBACK addStoredBookWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
		case WM_INITDIALOG:
		break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDOK:
				{
					HWND nameBox = GetDlgItem(hwndNewBook, IDC_NAME); 
                	HWND authorBox = GetDlgItem(hwndNewBook, IDC_AUTHOR); 
                	HWND typeBox = GetDlgItem(hwndNewBook, IDC_TYPE); 
                	HWND countBox = GetDlgItem(hwndNewBook, IDC_COUNT); 
                	HWND placeBox = GetDlgItem(hwndNewBook, IDC_PLACE); 
                	char name[100] = "";
					GetWindowTextA(nameBox, name, 100);
                	char type[100] = "";
					GetWindowTextA(typeBox, type, 100);
                	char author[100] = "";
					GetWindowTextA(authorBox, author, 100);
                	char place[100] = "";
					GetWindowTextA(placeBox, place, 100);
                	char quantity[100] = "";
					GetWindowTextA(countBox, quantity, 100); 
					int quantityINT = atoi(quantity);
					if (editedStoredBookId != -1){ // THIS IS EDITING OF STOREDBOOK
						int id = editedStoredBookId;
	                	ListStoredBooks[id].name = name;
	                	ListStoredBooks[id].author = author;
	                	ListStoredBooks[id].type = type;
	                	ListStoredBooks[id].quantity = quantityINT;
	                	ListStoredBooks[id].placeInLibrary = place;
					}else{ // NEW STOREDBOOK
						StoredBook newBook;
						storedBook_Init(&newBook, ListStoredBooks[ListStoredBooksLastIndex].id + 1, name, author, type, quantityINT, place);
						ListStoredBooks_Add(&newBook);
					}
					ListStoredBooks_Save();
					FillList(Mainhwnd, sourceSelected);
					EndDialog(hwndNewBook, 0);
					hwndNewBook = NULL;
				}
				break;
				case IDCANCEL:
				{
					editedStoredBookId = -1;
					EndDialog(hwndNewBook, 0);
					hwndNewBook = NULL;
				}
				break;
			}
		break;
		case WM_CLOSE:
			EndDialog(hwndNewBook, 0);
			hwndNewBook = NULL;
		break;
		default:
			return FALSE;
	}
	return TRUE;
}

BOOL CALLBACK addLibraryUserWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
		case WM_INITDIALOG:
		break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDOK:
				{
					HWND nameBox = GetDlgItem(hwndNewUser, IDC_NAME); 
                	HWND surnameBox = GetDlgItem(hwndNewUser, IDC_SURNAME); 
                	HWND numberBox = GetDlgItem(hwndNewUser, IDC_NUMBER); 
                	char name[100] = "";
					GetWindowTextA(nameBox, name, 100);
                	char surname[100] = "";
					GetWindowTextA(surnameBox, surname, 100);
					char number[100] = "";
					GetWindowTextA(numberBox, number, 100);
					if (editedUserId != -1){ // THIS IS EDITING OF user
						int id = editedUserId;
	                	ListLibraryUsers[editedUserId].name = name;
	                	ListLibraryUsers[editedUserId].surname = surname;
					}else{ // NEW user
						LibraryUser newUser;
						libraryUser_Init(&newUser, ListLibraryUsers[ListLibraryUsersLastIndex].id + 1, atoi(number), name, surname);
						ListLibraryUsers_Add(&newUser);
					}
					ListLibraryUsers_Save();
					FillList(Mainhwnd, sourceSelected);
					EndDialog(hwndNewUser, 0);
					hwndNewUser = NULL;
				}
				break;
				case IDCANCEL:
				{
					editedUserId = -1;
					EndDialog(hwndNewUser, 0);
					hwndNewUser = NULL;
				}
				break;
			}
		break;
		case WM_CLOSE:
			EndDialog(hwndNewUser, 0);
			hwndNewUser = NULL;
		break;
		default:
			return FALSE;
	}
	return TRUE;
}


BOOL CALLBACK mainWindow(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
		case WM_INITDIALOG:
			Mainhwnd = hwnd;
			InitMainDialog(hwnd);
			ListStoredBooks_Init();
			ListBorrowedBooks_Init();
			ListLibraryUsers_Init();
			srand(time(NULL));
		break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_SourceList:
				{
					if(HIWORD(wParam) == CBN_SELCHANGE) HandleSourceListChange(hwnd); 
				}
				break;
				case IDC_MainList1:
				case IDC_MainList2:
				case IDC_MainList3:
				case IDC_MainList4:
				case IDC_MainList5:
				{
					if(HIWORD(wParam) == LBN_SELCHANGE  ){ //CHANGE SELLECTION ON OTHERS
						HWND changedList = GetDlgItem(hwnd, LOWORD(wParam)); 
						int indexToSelectOnAll = SendMessage(changedList, LB_GETCURSEL, 0, 0); //check selection of changed list
						if (indexToSelectOnAll != LB_ERR){ //if something selected change on the others
							//Lists numbers 1027-1031
							int i = 1027;
							for(i;i<=1031; i++){
								HWND currentlist = GetDlgItem(hwnd, i); 
								int currSelected = SendMessage(currentlist, LB_GETCURSEL, 0, 0);
								if (currSelected != indexToSelectOnAll){ 
									SendMessage(currentlist, LB_SETCURSEL, indexToSelectOnAll, 0);
								}
							}
						}
					}
				}
				case IDC_ACTION1:
				{
					switch(sourceSelected){ 
						case 0: //ZASOBY
						{
							if(HIWORD(wParam) == BN_CLICKED && hwndNewBook == NULL){
								editedStoredBookId = -1;
								HINSTANCE instance = GetModuleHandle(NULL);
								hwndNewBook = CreateDialog(instance, 
			                                        MAKEINTRESOURCE(IDD_AddStoredBook), 
			                                        hwnd, 
			                                        (DLGPROC)addStoredBookWindow); 
			                	ShowWindow(hwndNewBook, SW_SHOW); 
	                		}
						}
						break;
						case 2: //Uzytkownicy
						{
							if(HIWORD(wParam) == BN_CLICKED && hwndNewUser == NULL){
								editedUserId = -1;
								HINSTANCE instance = GetModuleHandle(NULL);
								hwndNewUser = CreateDialog(instance, 
			                                        MAKEINTRESOURCE(IDD_AddLibraryUser), 
			                                        hwnd, 
			                                        (DLGPROC)addLibraryUserWindow); 
			                	ShowWindow(hwndNewUser, SW_SHOW); 
			                	HWND numberBox = GetDlgItem(hwndNewUser, IDC_NUMBER);
			                	int randNumber = rand() % 5000;
			                	char * buff;
			                	asprintf (&buff, "%i", randNumber);
			                	SetWindowTextA(numberBox, buff);
			                	
	                		}
						}
						break;
					}
				
				}
				break;
				case IDC_ACTION2: 
				{
					switch(sourceSelected){ 
						case 0: //ZASOBY EDIT
						{
							if(HIWORD(wParam) == BN_CLICKED && hwndNewBook == NULL){
								HWND list = GetDlgItem(hwnd, IDC_MainList1); 
								int indexSelected = SendMessage(list, LB_GETCURSEL, 0, 0); //check selection of changed list
								if (indexSelected == LB_ERR){ 
									return MessageBox(hwnd, "Wybierz ksi¹¿kê", "B³¹d", MB_OK); 
			                	}
								HINSTANCE instance = GetModuleHandle(NULL);
								hwndNewBook = CreateDialog(instance, 
			                                        MAKEINTRESOURCE(IDD_AddStoredBook), 
			                                        hwnd, 
			                                        (DLGPROC)addStoredBookWindow); 
			                	ShowWindow(hwndNewBook, SW_SHOW); 
			                	int idOfBook = SendMessage(list, LB_GETITEMDATA, indexSelected, 0); //Get selected ID from ListBooks
			                	StoredBook s = ListStoredBooks[idOfBook];
			                	HWND description = GetDlgItem(hwndNewBook, IDC_STATIC); 
			                	HWND nameBox = GetDlgItem(hwndNewBook, IDC_NAME); 
			                	HWND authorBox = GetDlgItem(hwndNewBook, IDC_AUTHOR); 
			                	HWND typeBox = GetDlgItem(hwndNewBook, IDC_TYPE); 
			                	HWND countBox = GetDlgItem(hwndNewBook, IDC_COUNT); 
			                	HWND placeBox = GetDlgItem(hwndNewBook, IDC_PLACE); 
			                	SetWindowTextA(description, "Edytuj wybran¹ ksi¹¿kê");
			                	SetWindowTextA(nameBox, s.name);
			                	SetWindowTextA(authorBox, s.author);
			                	SetWindowTextA(typeBox, s.type);
			                	char* intAsStr;// INTEGER TO STRING
								asprintf (&intAsStr, "%i", s.quantity);// INTEGER TO STRING
			                	SetWindowTextA(countBox, intAsStr);
			                	free(intAsStr);
			                	SetWindowTextA(placeBox, s.placeInLibrary);
			                	editedStoredBookId = idOfBook;
			            	}
						}
						break;
						case 2: //UZYTKOWNICY EDIT
						{
							if(HIWORD(wParam) == BN_CLICKED && hwndNewUser == NULL){
								HWND list = GetDlgItem(hwnd, IDC_MainList1); 
								int indexSelected = SendMessage(list, LB_GETCURSEL, 0, 0); //check selection of changed list
								if (indexSelected == LB_ERR){ 
									return MessageBox(hwnd, "Wybierz u¿ytkownika", "B³¹d", MB_OK); 
			                	}
								HINSTANCE instance = GetModuleHandle(NULL);
								hwndNewUser = CreateDialog(instance, 
			                                        MAKEINTRESOURCE(IDD_AddLibraryUser), 
			                                        hwnd, 
			                                        (DLGPROC)addLibraryUserWindow); 
			                	ShowWindow(hwndNewUser, SW_SHOW); 
			                	int idOfUser = SendMessage(list, LB_GETITEMDATA, indexSelected, 0); //Get selected ID from ListBooks
			                	LibraryUser s = ListLibraryUsers[idOfUser];
			                	HWND description = GetDlgItem(hwndNewUser, IDC_STATIC); 
			                	HWND nameBox = GetDlgItem(hwndNewUser, IDC_NAME); 
			                	HWND surnameBox = GetDlgItem(hwndNewUser, IDC_SURNAME); 
			                	HWND numberBox = GetDlgItem(hwndNewUser, IDC_NUMBER); 
			                	SetWindowTextA(description, "Edytuj wybranego u¿ytkownika");
			                	SetWindowTextA(nameBox, s.name);
			                	SetWindowTextA(surnameBox, s.surname);
			                	char* intAsStr;// INTEGER TO STRING
								asprintf (&intAsStr, "%i", s.number);// INTEGER TO STRING
			                	SetWindowTextA(numberBox, intAsStr);
			                	free(intAsStr);
			                	editedUserId = idOfUser;
			            	}
						}
						break;
					}
				}
				break;
				case IDC_ACTION3:
				{
					switch(sourceSelected){ 
						case 1: //Wypo¿yczone
						{
							if(HIWORD(wParam) == BN_CLICKED){
									HWND list = GetDlgItem(hwnd, IDC_MainList1); 
									int indexSelected = SendMessage(list, LB_GETCURSEL, 0, 0); //check selection of changed list
									int idOfBook = SendMessage(list, LB_GETITEMDATA, indexSelected, 0); //Get selected ID from ListBooks
									if (idOfBook == -1){
										return MessageBox(hwnd, "Wybierz ksi¹¿kê", "B³¹d", MB_OK); 
									} 	
									int msgResult = MessageBox(hwnd, "Czy jesteœ pewny, ¿e chcesz zwróciæ wybran¹ ksi¹¿kê?", "Zwróæ ksi¹¿kê", MB_ICONWARNING | MB_YESNO);
									if (msgResult == IDYES) {
										StoredBook s;
										storedBook_Init(&s, ListStoredBooks[ListBorrowedBooksLastIndex].id + 1, ListBorrowedBooks[idOfBook].name, ListBorrowedBooks[idOfBook].author, ListBorrowedBooks[idOfBook].type, 1, "ZWROTY");
										ListStoredBooks_Add(&s);
										ListStoredBooks_Save();
										ListBorrowedBooks_Delete(idOfBook);
										ListBorrowedBooks_Save();
										FillList(Mainhwnd, sourceSelected);
									}
							}
						}break;
					}
					
				}
				break;
				case IDC_ACTION4:
				{
					switch(sourceSelected){ 
						case 0: //ZASOBY
						{
							if(HIWORD(wParam) == BN_CLICKED){
								if(HIWORD(wParam) == BN_CLICKED && hwndBorrowBook == NULL){
									HWND list = GetDlgItem(hwnd, IDC_MainList1); 
									int indexSelected = SendMessage(list, LB_GETCURSEL, 0, 0); //check selection of changed list
									int idOfBook = SendMessage(list, LB_GETITEMDATA, indexSelected, 0); //Get selected ID from ListBooks
									if (idOfBook == -1){
										return MessageBox(hwnd, "Wybierz ksi¹¿kê", "B³¹d", MB_OK); 
									} 
										
									HINSTANCE instance = GetModuleHandle(NULL);
									hwndBorrowBook = CreateDialog(instance, 
				                                        MAKEINTRESOURCE(IDD_BorrowBook), 
				                                        hwnd, 
				                                        (DLGPROC)BorrowBookWindow); 
				                	ShowWindow(hwndBorrowBook, SW_SHOW); 
				                	borrowingBookId = idOfBook;
									HWND description = GetDlgItem(hwndBorrowBook, IDC_TEXT);
									HWND comboBox = GetDlgItem(hwndBorrowBook, IDC_CB);
									SetWindowTextA(description, "Wybierz u¿ytkownika,\nktóremu wypo¿yczana jest ksi¹¿ka.");
									int x=0;
									for (x;x<=ListLibraryUsersLastIndex; x++){
										char tempStr[30];
										sprintf(tempStr, "(%d)%s %s", ListLibraryUsers[x].number, ListLibraryUsers[x].name, ListLibraryUsers[x].surname);
										int pos = (int)SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM) tempStr);
										SendMessage(comboBox, CB_SETITEMDATA, pos, (LPARAM) x); //SENDING INDEX to "ITEMDATA"
									}
									SendMessage(comboBox, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);
									//SendMessage(comboBox, CB_SETITEMHEIGHT, (WPARAM)150, (LPARAM)150);
									 
								}
	                		}
						}
						break;
					}
				}
				break;
				case IDC_ACTION5:
				{
					
				}
				break;
				case IDC_ACTION6:
				{
					switch(sourceSelected){ 
						case 0: //ZASOBY
						{
							if(HIWORD(wParam) == BN_CLICKED){
								HWND list = GetDlgItem(hwnd, IDC_MainList1); 
								int indexSelected = SendMessage(list, LB_GETCURSEL, 0, 0); //check selection of changed list
								int idOfBook = SendMessage(list, LB_GETITEMDATA, indexSelected, 0); //Get selected ID from ListBooks
								if (idOfBook == -1){
									return MessageBox(hwnd, "Wybierz ksi¹¿kê", "B³¹d", MB_OK); 
								} 
									
								int msgResult = MessageBox(hwnd, "Czy jesteœ pewny, ¿e chcesz usun¹æ wybran¹ ksi¹¿kê?", "Usuñ ksi¹¿kê", MB_ICONWARNING | MB_YESNO);
								if (msgResult == IDYES) {
									ListStoredBooks_Delete(idOfBook);
									ListStoredBooks_Save();
									FillList(Mainhwnd, sourceSelected);
								}
														
	                		}
						}
						break;
						case 2: //U¯ytkownicy
						{
							if(HIWORD(wParam) == BN_CLICKED){
								HWND list = GetDlgItem(hwnd, IDC_MainList1); 
								int indexSelected = SendMessage(list, LB_GETCURSEL, 0, 0); //check selection of changed list
								int idOfuser = SendMessage(list, LB_GETITEMDATA, indexSelected, 0); //Get selected ID from List
								if (idOfuser == -1){
									return MessageBox(hwnd, "Wybierz u¿ytkownika", "B³¹d", MB_OK); 
								} 
									
								int msgResult = MessageBox(hwnd, "Czy jesteœ pewny, ¿e chcesz usun¹æ wybranego u¿ytkownika?\nWszystkie wypo¿yczone przez niego ksi¹¿ki przepadn¹!", "Usuñ u¿ytkownika", MB_ICONWARNING | MB_YESNO);
								if (msgResult == IDYES) {
									ListLibraryUsers_Delete(idOfuser);
									//DODAC USUWANIE BORROWED BOOKS
									ListLibraryUsers_Save();
									FillList(Mainhwnd, sourceSelected);
								}
														
	                		}
						}
						break;
					}
				}
				break;
			}
		break;
		case WM_CLOSE:
			//ListStoredBooks_Free();
			EndDialog(hwnd, 0);
		break;
		default:
			return FALSE;
	}
	return TRUE;
}





