#ifndef FORM_F_H
#define FORM_F_H
#include <windows.h>
//MAIN FORM
extern int InitMainDialog(HWND hwnd);
extern int HandleSourceListChange(HWND hwnd);
extern int PrepareActionButtonsForActiveSource(HWND hwnd, int selectedSource);
extern int SetAllActionButtonsNotVisible(HWND hwnd);
extern int ButtonShowAndSetText(HWND hwnd, int IDdglItem, const char* text);
extern int refreshColumnHeaders(int source);
extern int FillList(HWND hwnd, int selectedSource);
extern int InsertToTable(HWND hwnd, int index, const char * text);

// Action handlers
void HandleAction1(HWND hwnd, int sourceSelected);
void HandleAction2(HWND hwnd, int sourceSelected);
void HandleAction3(HWND hwnd, int sourceSelected);
void HandleAction4(HWND hwnd, int sourceSelected);
void HandleAction6(HWND hwnd, int sourceSelected);

//END MAIN FORM

#endif
