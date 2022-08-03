#include <Windows.h>
#include "resource.h"

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK EllipPushWndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = TEXT("About3");

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
	wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ABOUT3));
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hInstance = hInstance;
	wndClass.lpszMenuName = MAKEINTRESOURCE(ABOUT3);
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = EllipPushWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hIcon = NULL;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("EllipPush");

	RegisterClass(&wndClass);

	hWnd = CreateWindow(
		szAppName, TEXT("About Box Demo Program"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static HINSTANCE hInstance;

	switch (msg)
	{
	case WM_CREATE:
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_APP_ABOUT:
			DialogBox(hInstance, MAKEINTRESOURCE(ABOUTBOX), hWnd, AboutDlgProc);
			return 0;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

BOOL CALLBACK AboutDlgProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK EllipPushWndProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	TCHAR		szText[40];
	HBRUSH		hBrush;
	HDC			hDC;
	PAINTSTRUCT	ps;
	RECT		rect;

	switch (msg)
	{
	case WM_PAINT:
		GetClientRect(hWnd, &rect);
		GetWindowText(hWnd, szText, sizeof(szText));

		hDC = BeginPaint(hWnd, &ps);

		hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		hBrush = (HBRUSH)SelectObject(hDC, hBrush);
		SetBkColor(hDC, COLOR_WINDOW);
		SetTextColor(hDC, COLOR_WINDOWTEXT);

		Ellipse(hDC, rect.left, rect.top, rect.right, rect.bottom);
		DrawText(hDC, szText, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		DeleteObject(SelectObject(hDC, hBrush));

		EndPaint(hWnd, &ps);
		return 0;

	case WM_KEYUP:
		if (wParam != VK_SPACE)
			break;	// fall through

	case WM_LBUTTONUP:
		SendMessage(GetParent(hWnd), WM_COMMAND, GetWindowLong(hWnd, GWL_ID), (LPARAM)hWnd);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}