#include <Windows.h>

HDC GetPrinterDC(void);				// in GetPrinterDC.cpp
void PageGDICalls(HDC, int, int);	// in Print.cpp

HINSTANCE	hInst;
TCHAR		szAppName[] = TEXT("Print1");
TCHAR		szCaption[] = TEXT("Print Program 1");

BOOL PrintMyPage(HWND hWnd)
{
	static DOCINFO	di = { sizeof(DOCINFO), TEXT("Print1: Printing") };
	BOOL			bSuccess = TRUE;
	HDC				hdcPrn{ 0 };
	int				xPage{ 0 }, yPage{ 0 };

	if (NULL == (hdcPrn = GetPrinterDC()))
		return FALSE;

	xPage = GetDeviceCaps(hdcPrn, HORZRES);
	yPage = GetDeviceCaps(hdcPrn, VERTRES);

	if (StartDoc(hdcPrn, &di) > 0)
	{
		if (StartPage(hdcPrn) > 0)
		{
			PageGDICalls(hdcPrn, xPage, yPage);

			if (EndPage(hdcPrn) > 0)
				EndDoc(hdcPrn);
			else
				bSuccess = FALSE;
		}
	}
	else
		bSuccess = FALSE;

	DeleteDC(hdcPrn);
	return bSuccess;
}