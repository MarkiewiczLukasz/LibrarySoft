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
//END MAIN FORM

#endif
