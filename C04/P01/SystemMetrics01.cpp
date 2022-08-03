#include <Windows.h>
#include "..\\..\\include\\SystemMetrics.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szCmdLine,
	int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("SystemMetrics01");
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
		TEXT("Get System Metrics No. 1"),
		WS_OVERLAPPEDWINDOW,
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
	static int	cxChar = 0, cxCaps= 0, cyChar = 0;
	HDC			hDC = (HDC)0;
	int			i = 0;
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
		return 0;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		for (i = 0; i < NUMLINES; ++i)
		{
			TextOut(
				hDC,
				0,
				cyChar * i,
				sysmetrics[i].szLabel,
				lstrlen(sysmetrics[i].szLabel));

			TextOut(
				hDC,
				22 * cxCaps,
				cyChar * i,
				sysmetrics[i].szDesc,
				lstrlen(sysmetrics[i].szDesc));

			SetTextAlign(hDC, TA_RIGHT | TA_TOP);

			TextOut(
				hDC,
				22 * cxCaps + 40 * cxChar,
				cyChar * i,
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