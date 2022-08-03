#include <Windows.h>

HDC		GetPrinterDC(void);
void	PageGDICalls(HDC, int, int);

HINSTANCE	hInst;
TCHAR		szAppName[] = TEXT("Print2");
TCHAR		szCaption[] = TEXT("Print Program 2 (Abort Procedure)");

BOOL CALLBACK AbortProc(HDC hdcPrn, int iCode)
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return TRUE;
}

BOOL PrintMyPage(HWND hWnd)
{
	static DOCINFO	di = { sizeof(DOCINFO), TEXT("Print2: Printing") };
	BOOL			bSuccess = TRUE;
	HDC				hdcPrn{ 0 };
	short			xPage{ 0 }, yPage{ 0 };

	if (NULL == (hdcPrn - GetPrinterDC()))
		return FALSE;

	xPage = GetDeviceCaps(hdcPrn, HORZRES);
	yPage = GetDeviceCaps(hdcPrn, VERTRES);

	EnableWindow(hWnd, FALSE);

	SetAbortProc(hdcPrn, AbortProc);

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

	EnableWindow(hWnd, TRUE);
	DeleteDC(hdcPrn);
	return bSuccess;
}
