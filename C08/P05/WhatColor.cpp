#include <Windows.h>

#define ID_TIMER	1

void FindWindowSize(int *, int*);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	PSTR		szCmdLine,
	int			iCmdShow)
{
	static TCHAR	szAppName[] = TEXT("WhatColor");
	HWND			hWnd;
	MSG				msg;
	WNDCLASS		wndClass;
	int				cxWindow, cyWindow;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_CROSS);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hInstance = hInstance;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	FindWindowSize(&cxWindow, &cyWindow);

	hWnd = CreateWindow(
		szAppName, TEXT("What Color"), WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_BORDER,
		CW_USEDEFAULT, CW_USEDEFAULT, cxWindow, cyWindow,
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

void FindWindowSize(
	int * pcxWindow,
	int * pcyWindow)
{
	HDC			hdcScreen;
	TEXTMETRIC	tm;

	hdcScreen = CreateIC(TEXT("DISPLAY"), NULL, NULL, NULL);
	GetTextMetrics(hdcScreen, &tm);
	DeleteDC(hdcScreen);

	*pcxWindow = 2 * GetSystemMetrics(SM_CXBORDER) + 12 * tm.tmAveCharWidth;
	*pcyWindow = 2 * GetSystemMetrics(SM_CYBORDER) + GetSystemMetrics(SM_CYCAPTION) + 2 * tm.tmHeight;
}

LRESULT CALLBACK WndProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static COLORREF	cr, crLast;
	static HDC		hdcScreen;
	HDC				hDC;
	PAINTSTRUCT		ps;
	POINT			pt;
	RECT			rc;
	TCHAR			szBuffer[16];

	switch (msg)
	{
	case WM_CREATE:
		SetTimer(hWnd, ID_TIMER, 100, NULL);
		return 0;

	case WM_TIMER:
		GetCursorPos(&pt);
		cr = GetPixel(hdcScreen, pt.x, pt.y);

		SetPixel(hdcScreen, pt.x, pt.y, 0);

		if (cr != crLast)
		{
			crLast = cr;
			InvalidateRect(hWnd, NULL, FALSE);
		}

		return true;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &rc);
		wsprintf(szBuffer, TEXT("  %02X %02X %02X  "), GetRValue(cr), GetGValue(cr), GetBValue(cr));
		DrawText(hDC, szBuffer, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		DeleteDC(hdcScreen);
		KillTimer(hWnd, ID_TIMER);
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}