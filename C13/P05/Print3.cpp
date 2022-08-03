#include <Windows.h>

HDC		GetPrinterDC(void);
void	PageGDICalls(HDC, int, int);

HINSTANCE	hInst;
TCHAR		szAppName[] = TEXT("Print3");
TCHAR		szCaption[] = TEXT("Print Program 3 (Dialog Box)");

BOOL	bUserAbort;
HWND	hDlgPrint;

BOOL CALLBACK PrintDlgProc(
	HWND	hDlg,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		SetWindowText(hDlg, szAppName);
		EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
		return TRUE;

	case WM_COMMAND:
		bUserAbort = TRUE;
		EnableWindow(GetParent(hDlg), TRUE);
		DestroyWindow(hDlg);
		hDlgPrint = NULL;
		return TRUE;
	}

	return FALSE;
}

BOOL CALLBACK AbortProc(HDC hdcPrn, int iCode)
{
	MSG msg;

	while (!bUserAbort && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (!hDlgPrint || !IsDialogMessage(hDlgPrint, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return !bUserAbort;
}

BOOL PrintMyPage(HWND hWnd)
{
	static DOCINFO	di = { sizeof(DOCINFO), TEXT("Print3: Printing") };
	BOOL			bSuccess = TRUE;
	HDC				hdcPrn{ 0 };
	int				xPage{ 0 }, yPage{ 0 };

	if (NULL == (hdcPrn = GetPrinterDC()))
		return FALSE;

	xPage = GetDeviceCaps(hdcPrn, HORZRES);
	yPage = GetDeviceCaps(hdcPrn, VERTRES);

	EnableWindow(hWnd, FALSE);

	bUserAbort = FALSE;
	hDlgPrint = CreateDialog(hInst, TEXT("PrintDlgBox"), hWnd, PrintDlgProc);

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

	if (!bUserAbort)
	{
		EnableWindow(hWnd, TRUE);
		DestroyWindow(hDlgPrint);
	}

	DeleteDC(hdcPrn);

	return bSuccess && !bUserAbort;
}