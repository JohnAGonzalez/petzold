#include <Windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szCmdLine,
	int iCmdShow)
{
	static TCHAR	szAppName[] = TEXT("WhatSize");
	HWND			hWnd;
	MSG				msg;
	WNDCLASS		wndClass;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName, TEXT("What Size is the Window"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.lParam;
}

void Show(
	HWND hWnd,
	HDC hDC,
	int xText,
	int yText,
	int iMapMode,
	const char * szMapMode)
{
	TCHAR	szBuffer[60];
	RECT	rect;

	SaveDC(hDC);

	SetMapMode(hDC, iMapMode);
	GetClientRect(hWnd, &rect);
	DPtoLP(hDC, (PPOINT)&rect, 2);

	RestoreDC(hDC, -1);

	TextOut(hDC, xText, yText, szBuffer,
		wsprintf(
			szBuffer,
			TEXT("%-20s %7d %7d %7d %7d"),
			szMapMode, rect.left, rect.right, rect.top, rect.bottom));
}

LRESULT CALLBACK WndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	static TCHAR	szHeading[] = TEXT("Mapping Mode         Left    Right   Top     Bottom");
	static TCHAR	szUndLine[] = TEXT("------------         ------- ------- ------- -------");
	static int		cxChar, cyChar;
	HDC				hDC;
	PAINTSTRUCT		ps;
	TEXTMETRIC		tm;

	switch (message)
	{
	case WM_CREATE:
		hDC = GetDC(hWnd);
		SelectObject(hDC, GetStockObject(SYSTEM_FIXED_FONT));

		GetTextMetrics(hDC, &tm);
		cxChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight + tm.tmExternalLeading;

		ReleaseDC(hWnd, hDC);
		return 0;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		SelectObject(hDC, GetStockObject(SYSTEM_FIXED_FONT));

		SetMapMode(hDC, MM_ANISOTROPIC);
		SetWindowExtEx(hDC, 1, 1, NULL);
		SetViewportExtEx(hDC, cxChar, cyChar, NULL);

		TextOut(hDC, 1, 1, szHeading, lstrlen(szHeading));
		TextOut(hDC, 1, 2, szUndLine, lstrlen(szUndLine));

		Show(hWnd, hDC, 1, 3, MM_TEXT,		TEXT("TEXT (pixels)"));
		Show(hWnd, hDC, 1, 4, MM_LOMETRIC,	TEXT("LOMETRIC (0.1mm)"));
		Show(hWnd, hDC, 1, 5, MM_HIMETRIC,	TEXT("HIMETRIC (0.01mm)"));
		Show(hWnd, hDC, 1, 6, MM_LOENGLISH,	TEXT("LOENGLISH (0.01 in)"));
		Show(hWnd, hDC, 1, 7, MM_HIENGLISH,	TEXT("HIENGLISH (0.001 in"));
		Show(hWnd, hDC, 1, 8, MM_TWIPS,		TEXT("TWIPS (1/1440 in)"));

		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}