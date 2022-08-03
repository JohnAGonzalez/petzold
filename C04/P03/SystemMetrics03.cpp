#include <Windows.h>
#include "..\\..\\include\\SystemMetrics.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szCmdLine,
	int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("SystemMetrics03");
	HWND		hWnd;
	MSG			msg;
	WNDCLASS	wndClass;

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
		MessageBox(NULL, TEXT("This program requires a minimum of Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName,
		TEXT("Get System Metrics No. 3"),
		WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
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
	static int	cxChar = 0,
				cxCaps = 0,
				cyChar = 0,
				cxClient = 0,
				cyClient = 0,
				iMaxWidth = 0;
	HDC			hDC = (HDC)0;
	int			i = 0,
				x = 0,
				y = 0,
				iVertPos = 0,
				iHorzPos = 0,
				iPaintBeg = 0,
				iPaintEnd = 0;
	PAINTSTRUCT	ps = { 0 };
	SCROLLINFO	si = { 0 };
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

		iMaxWidth = 40 * cxChar + 22 * cyChar;
		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		// set vertical scroll bar range and page size
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = NUMLINES - 1;
		si.nPage = cyClient / cyChar;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

		// set the horizontal bar range and page size
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = 2 + iMaxWidth / cxChar;
		SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
		return 0;

	case WM_VSCROLL:
		// get all the vertical scroll bar information
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_VERT, &si);

		// save the position for comparison later on
		iVertPos = si.nPos;

		switch (LOWORD(wParam))
		{
		case SB_TOP:			si.nPos = si.nMin;		break;			
		case SB_BOTTOM:			si.nPos = si.nMax;		break;
		case SB_LINEUP:			si.nPos -= 1;			break;
		case SB_LINEDOWN:		si.nPos += 1;			break;
		case SB_PAGEUP:			si.nPos -= si.nPage;	break;
		case SB_PAGEDOWN:		si.nPos += si.nPage;	break;
		case SB_THUMBPOSITION:	si.nPos = si.nTrackPos;	break;
		default:										break;
		}

		// Set the position and then retrieve it.  Due to adjustments
		// by Windows it may not be the same as the value set.
		si.fMask = SIF_POS;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		GetScrollInfo(hWnd, SB_VERT, &si);

		// If the position has changed, scroll the window and update it
		if (si.nPos != iVertPos)
		{
			ScrollWindow(hWnd, 0, cyChar * (iVertPos - si.nPos), NULL, NULL);
			UpdateWindow(hWnd);
		}
		return 0;

	case WM_HSCROLL:
		// Get all the vertical scroll bar information
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;

		// Save the position for comparison later on
		GetScrollInfo(hWnd, SB_HORZ, &si);
		iHorzPos = si.nPos;

		switch (LOWORD(wParam))
		{
		case SB_LINELEFT:		si.nPos -= 1;			break;
		case SB_LINERIGHT:		si.nPos += 1;			break;
		case SB_PAGELEFT:		si.nPos -= si.nPage;	break;
		case SB_PAGERIGHT:		si.nPos += si.nPage;	break;
		case SB_THUMBPOSITION:	si.nPos = si.nTrackPos;	break;
		default:										break;
		}

		// Set the position and then retrieve it.  Due to adjustments
		// by Windows it may not be the same as the value set.
		si.fMask = SIF_POS;
		SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
		GetScrollInfo(hWnd, SB_HORZ, &si);

		// If the position has changed, scroll the window and update it
		if (si.nPos != iHorzPos)
		{
			ScrollWindow(hWnd, cxChar*(iHorzPos - si.nPos), 0, NULL, NULL);
		}
		return 0;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		// Get vertical scroll bar position
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		GetScrollInfo(hWnd, SB_VERT, &si);
		iVertPos = si.nPos;

		// Get horizontal scroll bar position
		GetScrollInfo(hWnd, SB_HORZ, &si);
		iHorzPos = si.nPos;

		// Find painting limits
		iPaintBeg = max(0, iVertPos + ps.rcPaint.top / cyChar);
		iPaintEnd = min(NUMLINES - 1, iVertPos + ps.rcPaint.bottom / cyChar);

		for (i = iPaintBeg; i <= iPaintEnd; ++i)
		{
			x = cxChar * (1 - iHorzPos);
			y = cyChar * (i - iVertPos);

			TextOut(
				hDC,
				x,
				y,
				sysmetrics[i].szLabel,
				lstrlen(sysmetrics[i].szLabel));

			TextOut(
				hDC,
				x + 22 * cxCaps,
				y,
				sysmetrics[i].szDesc,
				lstrlen(sysmetrics[i].szDesc));

			SetTextAlign(hDC, TA_RIGHT | TA_TOP);

			TextOut(
				hDC,
				x + 22 * cxCaps + 40 * cxChar,
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