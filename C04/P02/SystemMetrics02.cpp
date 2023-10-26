#include <Windows.h>
#include "..\\..\\include\\SystemMetrics.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szCmdLine,
	int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("SystemMetrics02");
	HWND		hWnd;
	MSG			msg;
	WNDCLASS	wndClass;

	wndClass.style			= CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc	= WndProc;
	wndClass.cbClsExtra		= 0;
	wndClass.cbWndExtra		= 0;
	wndClass.hInstance		= hInstance;
	wndClass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName	= NULL;
	wndClass.lpszClassName	= szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("This program requires a minimum of Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName,
		TEXT("Get System Metrics No. 2"),
		WS_OVERLAPPEDWINDOW | WS_VSCROLL,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int	cxChar = 0, cxCaps = 0, cyChar = 0, cyClient = 0, iVScrollPos = 0;
	HDC			hDC = (HDC)0;
	int			i = 0, y = 0;
	PAINTSTRUCT	ps = { 0 };
	TCHAR		szBuffer[10] = { 0 };
	TEXTMETRIC	tm = { 0 };

	switch (message)
	{
	case WM_CREATE:
		hDC = GetDC(hWnd);
		GetTextMetrics(hDC, &tm);

		cxChar = tm.tmAveCharWidth;
		cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
		cyChar = tm.tmHeight + tm.tmExternalLeading;

		ReleaseDC(hWnd, hDC);

		SetScrollRange(hWnd, SB_VERT, 0, NUMLINES - 1, FALSE);
		SetScrollPos(hWnd, SB_VERT, iVScrollPos, TRUE);
		return 0;

	case WM_SIZE:
		cyClient = HIWORD(lParam);
		return 0;

	case WM_VSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			iVScrollPos -= 1;
			break;

		case SB_LINEDOWN:
			iVScrollPos += 1;
			break;

		case SB_PAGEUP:
			iVScrollPos -= cyClient / cyChar;
			break;

		case SB_PAGEDOWN:
			iVScrollPos += cyClient / cyChar;
			break;

		case SB_THUMBPOSITION:
			iVScrollPos = HIWORD(wParam);
			break;

		default:
			break;
		}

		iVScrollPos = max(0, min(iVScrollPos, NUMLINES - 1));
		
		if (iVScrollPos != GetScrollPos(hWnd, SB_VERT))
		{
			SetScrollPos(hWnd, SB_VERT, iVScrollPos, TRUE);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		return 0;

	case WM_MOUSEWHEEL:
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			iVScrollPos -= 1;
		else
			iVScrollPos += 1;

				iVScrollPos = max(0, min(iVScrollPos, NUMLINES - 1));
		
		if (iVScrollPos != GetScrollPos(hWnd, SB_VERT))
		{
			SetScrollPos(hWnd, SB_VERT, iVScrollPos, TRUE);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		return 0;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		for (i = 0; i < NUMLINES; ++i)
		{
			y = cyChar * (i - iVScrollPos);

			TextOut(
				hDC,
				0,
				y,
				sysmetrics[i].szLabel,
				lstrlen(sysmetrics[i].szLabel));

			TextOut(
				hDC,
				22 * cxCaps,
				y,
				sysmetrics[i].szDesc,
				lstrlen(sysmetrics[i].szDesc));

			SetTextAlign(hDC, TA_RIGHT | TA_TOP);

			TextOut(
				hDC,
				22 * cxCaps + 40 * cxChar,
				y,
				szBuffer,
				wsprintf(
					szBuffer,
					TEXT("%5d"),
					GetSystemMetrics(sysmetrics[i].iIndex)));

			SetTextAlign(hDC, TA_LEFT | TA_TOP);
		}

		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}