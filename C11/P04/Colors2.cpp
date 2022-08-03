#include <Windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc		(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK ColorScrDlg	(HWND, UINT, WPARAM, LPARAM);

HWND	hDlgModeless;

TCHAR szAppName[] = TEXT("Colors2");

int WINAPI WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	PSTR		szCmdLine,
	int			iCmdShow)
{
	HWND		hWnd;
	MSG			msg;
	WNDCLASS	wndClass;

	wndClass.style = CS_VREDRAW | CS_HREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = CreateSolidBrush(0L);
	wndClass.hInstance = hInstance;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName, TEXT("Color Scroll"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	hDlgModeless = CreateDialog(hInstance, MAKEINTRESOURCE(COLORSCRCLG), hWnd, ColorScrDlg);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (0 == hDlgModeless || !IsDialogMessage(hDlgModeless, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		DeleteObject((HGDIOBJ)SetClassLong(hWnd, GCL_HBRBACKGROUND, (LONG)GetStockObject(WHITE_BRUSH)));
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

BOOL CALLBACK ColorScrDlg(
	HWND	hDlg,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static int	iColor[3];
	HWND		hWndParent, hCtrl;
	int			iCtrlID, iIndex;

	switch (msg)
	{
	case WM_INITDIALOG:
		for (iCtrlID = 10; iCtrlID < 13; ++iCtrlID)
		{
			hCtrl = GetDlgItem(hDlg, iCtrlID);
			SetScrollRange(hCtrl, SB_CTL, 0, 255, FALSE);
			SetScrollPos(hCtrl, SB_CTL, 0, FALSE);
		}

		return TRUE;

	case WM_VSCROLL:
		hCtrl = (HWND)lParam;
		iCtrlID = GetWindowLong(hCtrl, GWL_ID);
		iIndex = iCtrlID - 10;
		hWndParent = GetParent(hDlg);

		switch (LOWORD(wParam))
		{
		case SB_PAGEDOWN:		iColor[iIndex] += 15;								// fallthrough
		case SB_LINEDOWN:		iColor[iIndex] = min(255, iColor[iIndex] + 1);		break;
		case SB_PAGEUP:			iColor[iIndex] -= 15;								// fallthrough
		case SB_LINEUP:			iColor[iIndex] = max(0, iColor[iIndex] - 1);		break;
		case SB_TOP:			iColor[iIndex] = 0;									break;
		case SB_BOTTOM:			iColor[iIndex] = 255;								break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:		iColor[iIndex] = HIWORD(wParam);					break;
		default:
			return FALSE;
		}

		SetScrollPos(hCtrl, SB_CTL, iColor[iIndex], TRUE);
		SetDlgItemInt(hDlg, iCtrlID + 3, iColor[iIndex], FALSE);

		DeleteObject((HGDIOBJ)SetClassLong(hWndParent, GCL_HBRBACKGROUND, (LONG)CreateSolidBrush(RGB(iColor[0], iColor[1], iColor[2]))));

		InvalidateRect(hWndParent, NULL, TRUE);
		return TRUE;

	}

	return FALSE;
}