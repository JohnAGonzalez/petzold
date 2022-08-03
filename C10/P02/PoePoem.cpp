#include <Windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE	hInst;

int WINAPI WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	PSTR		szCmdLine,
	int			iCmdShow)
{
	TCHAR		szAppName[16] = { 0 }, szCaption[64] = { 0 }, szErrMsg[64] = { 0 };
	HWND		hWnd;
	MSG			msg;
	WNDCLASS	wndClass;

	LoadString(hInstance, IDS_APPNAME, szAppName, sizeof(szAppName) / sizeof(TCHAR));
	LoadString(hInstance, IDS_CAPTION, szCaption, sizeof(szCaption) / sizeof(TCHAR));

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(hInstance, szAppName);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		LoadStringA(hInstance, IDS_APPNAME, (char *)szAppName, sizeof(szAppName));
		LoadStringA(hInstance, IDS_ERRMSG, (char *)szErrMsg, sizeof(szErrMsg));
		MessageBoxA(NULL, (char *)szErrMsg, (char *)szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName, szCaption, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
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

LRESULT CALLBACK WndProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static char *	pText;
	static HGLOBAL	hResource;
	static HWND		hScroll;
	static int		iPosition, cxChar, cyChar, cyClient, iNumLines, xScroll;
	HDC				hDC;
	PAINTSTRUCT		ps;
	RECT			rect;
	TEXTMETRIC		tm;

	switch (msg)
	{
	case WM_CREATE:
		hDC = GetDC(hWnd);
		GetTextMetrics(hDC, &tm);
		cxChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight + tm.tmExternalLeading;
		ReleaseDC(hWnd, hDC);

		xScroll = GetSystemMetrics(SM_CXVSCROLL);
		hScroll = CreateWindow(
			TEXT("scrollbar"), NULL, WS_CHILD | WS_VISIBLE | SBS_VERT,
			0, 0, 0, 0,
			hWnd, (HMENU)1, hInst, NULL);

		hResource = LoadResource(hInst, FindResource(hInst, TEXT("AnnabelLee"), TEXT("TEXT")));
		pText = (char *)LockResource(hResource);
		iNumLines = 0;

		while (*pText != '\\' && *pText != '\0')
		{
			if (*pText == '\n')	++iNumLines;
			pText = AnsiNext(pText);
		}
		//*pText = '\0';

		SetScrollRange(hScroll, SB_CTL, 0, iNumLines, FALSE);
		SetScrollPos(hScroll, SB_CTL, 0, FALSE);
		return 0;

	case WM_SIZE:
		MoveWindow(hScroll, LOWORD(lParam) - xScroll, 0, xScroll, cyClient = HIWORD(lParam), TRUE);
		SetFocus(hWnd);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hScroll);
		return 0;

	case WM_VSCROLL:
		switch (wParam)
		{
		case SB_TOP:			iPosition = 0;						break;
		case SB_BOTTOM:			iPosition = iNumLines;				break;
		case SB_LINEUP:			--iPosition;						break;
		case SB_LINEDOWN:		++iPosition;						break;
		case SB_PAGEUP:			iPosition -= cyClient / cyChar;		break;
		case SB_PAGEDOWN:		iPosition += cyClient / cyChar;		break;
		case SB_THUMBPOSITION:	iPosition = LOWORD(lParam);			break;
		}

		iPosition = max(0, min(iPosition, iNumLines));

		if (iPosition != GetScrollPos(hScroll, SB_CTL))
		{
			SetScrollPos(hScroll, SB_CTL, iPosition, TRUE);
			InvalidateRect(hScroll, NULL, TRUE);
		}
		return 0;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		pText = (char *)LockResource(hResource);

		GetClientRect(hWnd, &rect);
		rect.left += cxChar;
		rect.top += cyChar * (1 - iPosition);
		DrawTextA(hDC, pText, -1, &rect, DT_EXTERNALLEADING);

		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		FreeResource(hResource);
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}