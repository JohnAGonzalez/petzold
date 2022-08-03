#include <Windows.h>

#define ID_TIMER	1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	PSTR		szCmdLine,
	int			iCmdShow)
{
	static TCHAR	szAppName[] = TEXT("DigitalClock");
	HWND			hWnd;
	MSG				msg;
	WNDCLASS		wndClass;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hInstance = hInstance;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName, TEXT("Digital Clock"), WS_OVERLAPPEDWINDOW,
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

void DisplayDigit(
	HDC hDC,
	int	iNumber)
{
	static BOOL	fSevenSegment[10][7] = {
		1, 1, 1, 0, 1, 1, 1,	// 0
		0, 0, 1, 0, 0, 1, 0,	// 1
		1, 0, 1, 1, 1, 0, 1,	// 2
		1, 0, 1, 1, 0, 1, 1,	// 3
		0, 1, 1, 1, 0, 1, 0,	// 4
		1, 1, 0, 1, 0, 1, 1,	// 5
		1, 1, 0, 1, 1, 1, 1,	// 6
		1, 0, 1, 0, 0, 1, 0,	// 7
		1, 1, 1, 1, 1, 1, 1,	// 8
		1, 1, 1, 1, 0, 1, 1 };	// 9

	static POINT ptSegment[7][6] = {
		 7,  6, 11,  2, 31,  2, 35,  6, 31, 10, 11, 10,
		 6,  7, 10, 11, 10, 31,  6, 35,  2, 31,  2, 11,
		36,  7, 40, 11, 40, 31, 36, 35, 32, 31, 32, 11,
		 7, 36, 11, 32, 31, 32, 35, 36, 31, 40, 11, 40,
		 6, 37, 10, 41, 10, 61,  6, 65,  2, 61,  2, 41,
		36, 37, 40, 41, 40, 61, 36, 65, 32, 61, 32, 41,
		 7, 66, 11, 62, 31, 62, 35, 66, 31, 70, 11, 70 };
	
	for (int iSeg = 0; iSeg < 7; ++iSeg)
		if (fSevenSegment[iNumber][iSeg])
			Polygon(hDC, ptSegment[iSeg], 6);
}

void DisplayTwoDigits(
	HDC		hDC,
	int		iNumber,
	BOOL	fSuppress)
{
	if (!fSuppress || (iNumber / 10 != 0))
		DisplayDigit(hDC, iNumber / 10);

	OffsetWindowOrgEx(hDC, -42, 0, NULL);
	DisplayDigit(hDC, iNumber % 10);
	OffsetWindowOrgEx(hDC, -42, 0, NULL);
}

void DisplayColon(HDC hDC, WORD milliseconds)
{
	if (milliseconds > 500)
	{
		POINT ptColon[2][4] = {
			2, 21, 6, 17, 10, 21, 6, 25,
			2, 51, 6, 47, 10, 51, 6, 55 };

		Polygon(hDC, ptColon[0], 4);
		Polygon(hDC, ptColon[1], 4);
	}

	OffsetWindowOrgEx(hDC, -12, 0, NULL);
}

void DisplayTime(
	HDC		hDC,
	BOOL	f24Hour,
	BOOL	fSuppress)
{
	SYSTEMTIME st;

	GetLocalTime(&st);

	if (f24Hour)
		DisplayTwoDigits(hDC, st.wHour, fSuppress);
	else
		DisplayTwoDigits(hDC, (st.wHour %= 12) ? st.wHour : 12, fSuppress);

	WORD milliseconds = st.wMilliseconds;

	DisplayColon(hDC, milliseconds);
	DisplayTwoDigits(hDC, st.wMinute, FALSE);
	DisplayColon(hDC, st.wMilliseconds);
	DisplayTwoDigits(hDC, st.wSecond, FALSE);
}

LRESULT CALLBACK WndProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static BOOL		f24Hour, fSuppress;
	static HBRUSH	hBrushRed;
	static int		cxClient, cyClient;
	HDC				hDC;
	PAINTSTRUCT		ps;
	TCHAR			szBuffer[2];

	switch (msg)
	{
	case WM_CREATE:
		hBrushRed = CreateSolidBrush(RGB(255, 95, 95));
		SetTimer(hWnd, ID_TIMER, 100, NULL);

	case WM_SETTINGCHANGE:
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITIME, szBuffer, 2);
		f24Hour = (szBuffer[0] == '1');

		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITLZERO, szBuffer, 2);
		fSuppress = (szBuffer[0] == '0');

	case WM_TIMER:
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		SetMapMode(hDC, MM_ISOTROPIC);

		SetWindowExtEx(hDC, 276, 72, NULL);
		SetViewportExtEx(hDC, cxClient, cyClient, NULL);

		SetWindowOrgEx(hDC, 138, 36, NULL);
		SetViewportOrgEx(hDC, cxClient / 2, cyClient / 2, NULL);

		SelectObject(hDC, GetStockObject(NULL_PEN));
		SelectObject(hDC, hBrushRed);

		DisplayTime(hDC, f24Hour, fSuppress);

		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		KillTimer(hWnd, ID_TIMER);
		DeleteObject(hBrushRed);
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}