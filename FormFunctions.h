#ifndef FORMFUNCTIONS_H
#define FORMFUNCTIONS_H
#include <windows.h>
//MAIN FORM
char sources[3][20];

int InitMainDialog(HWND hwnd);
int HandleSourceListChange(HWND hwnd);
int PrepareActionButtonsForActiveSource(HWND hwnd, int selectedSource);
int SetAllActionButtonsNotVisible(HWND hwnd);
int ButtonShowAndSetText(HWND hwnd, int IDdglItem, char *text);
int refreshColumnHeaders(int source);
int FillList(HWND hwnd, int selectedSource);
int InsertToTable(HWND hwnd, int index, char * text);
//END MAIN FORM



#endif
