#include <Windows.h>
#include "resource.h"

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

typedef struct
{
	int iColor, iFigure;
} ABOUTBOX_DATA;

TCHAR szAppName[] = TEXT("About2");

int WINAPI WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	PSTR		szCmdLine,
	int			iCmdShow)
{
	HWND		hWnd;
	MSG			msg;
	WNDCLASS	wndClass;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ABOUT2));
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hInstance = hInstance;
	wndClass.lpszMenuName = MAKEINTRESOURCE(ABOUT2);
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName, TEXT("About Box Demo Program"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void PaintWindow(
	HWND	hWnd,
	int		iColor,
	int		iFigure)
{
	static COLORREF	crColor[8] = {
		RGB(  0,   0,   0),RGB(  0,   0, 255),RGB(  0, 255,   0),RGB(  0, 255, 255),
		RGB(255,   0,   0),RGB(255,   0, 255),RGB(255, 255,   0),RGB(255, 255, 255) };

	HBRUSH			hBrush;
	HDC				hDC;
	RECT			rect;

	hDC = GetDC(hWnd);
	GetClientRect(hWnd, &rect);
	hBrush = CreateSolidBrush(crColor[iColor - IDC_BLACK]);
	hBrush = (HBRUSH)SelectObject(hDC, hBrush);

	if (IDC_RECT == iFigure)
		Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
	else
		Ellipse(hDC, rect.left, rect.top, rect.right, rect.bottom);

	DeleteObject(hBrush);
	ReleaseDC(hWnd, hDC);
}

void PaintTheBlock(
	HWND	hCtrl,
	int		iColor,
	int		iFigure)
{
	InvalidateRect(hCtrl, NULL, TRUE);
	UpdateWindow(hCtrl);
	PaintWindow(hCtrl, iColor, iFigure);
}

LRESULT CALLBACK WndProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static HINSTANCE		hInstance;
	static ABOUTBOX_DATA	ad = { IDC_BLACK, IDC_RECT };
	PAINTSTRUCT				ps;

	switch (msg)
	{
	case WM_CREATE:
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_APP_ABOUT:
			if (DialogBoxParam(hInstance, MAKEINTRESOURCE(ABOUTBOX), hWnd, AboutDlgProc, (LPARAM)&ad))
				InvalidateRect(hWnd, NULL, TRUE);
			return 0;
		}
		break;

	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);

		PaintWindow(hWnd, ad.iColor, ad.iFigure);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

BOOL CALLBACK AboutDlgProc(
	HWND	hDlg,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static HWND				hCtrlBlock;
	static ABOUTBOX_DATA	ad, *pAD;

	switch (msg)
	{
	case WM_INITDIALOG:
		pAD = (ABOUTBOX_DATA *)lParam;
		ad = *pAD;

		CheckRadioButton(hDlg, IDC_BLACK, IDC_WHITE, ad.iColor);
		CheckRadioButton(hDlg, IDC_RECT, IDC_ELLIPSE, ad.iFigure);

		hCtrlBlock = GetDlgItem(hDlg, IDC_PAINT);

		SetFocus(GetDlgItem(hDlg, ad.iColor));
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			*pAD = ad;
			EndDialog(hDlg, TRUE);
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return TRUE;

		case IDC_BLACK:
		case IDC_RED:
		case IDC_GREEN:
		case IDC_YELLOW:
		case IDC_BLUE:
		case IDC_MAGENTA:
		case IDC_CYAN:
		case IDC_WHITE:
			ad.iColor = LOWORD(wParam);
			CheckRadioButton(hDlg, IDC_BLACK, IDC_WHITE, LOWORD(wParam));
			PaintTheBlock(hCtrlBlock, ad.iColor, ad.iFigure);
			return TRUE;

		case IDC_RECT:
		case IDC_ELLIPSE:
			ad.iFigure = LOWORD(wParam);
			CheckRadioButton(hDlg, IDC_RECT, IDC_ELLIPSE, LOWORD(wParam));
			PaintTheBlock(hCtrlBlock, ad.iColor, ad.iFigure);
			return TRUE;

		}
		break;

	case WM_PAINT:
		PaintTheBlock(hCtrlBlock, ad.iColor, ad.iFigure);
		break;

	}

	return FALSE;
}