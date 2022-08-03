#include <Windows.h>
#include "resource.h"
#include "SupportingFuncs.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = TEXT("DevCaps2");

int WINAPI WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	PSTR		szCmdLine,
	int			iCmdShow)
{
	HWND		hWnd{ 0 };
	MSG			msg{ 0 };
	WNDCLASS	wndClass{ 0 };

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hInstance = hInstance;
	wndClass.lpszMenuName = MAKEINTRESOURCE(DEVCAPS2);
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName, NULL, WS_OVERLAPPEDWINDOW,
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
	static TCHAR			szDevice[32] = { 0 }, szWindowText[64] = { 0 };
	static int				cxChar = 0, cyChar = 0, nCurrentDevice = IDM_SCREEN, nCurrentInfo = IDM_BASIC;
	static DWORD			dwNeeded = 0, dwReturned = 0;
	static PRINTER_INFO_4 *	pInfo4{ 0 };
	static PRINTER_INFO_5 *	pInfo5{ 0 };
	DWORD					i = 0;
	HDC						hDC{ 0 }, hdcInfo{ 0 };
	HMENU					hMenu{ 0 };
	HANDLE					hPrint{ 0 };
	PAINTSTRUCT				ps{ 0 };
	TEXTMETRIC				tm{ 0 };
	OSVERSIONINFO			verInfo{ 0 };

	switch (msg)
	{
	case WM_CREATE:
		hDC = GetDC(hWnd);
		SelectObject(hDC, GetStockObject(SYSTEM_FIXED_FONT));
		GetTextMetrics(hDC, &tm);
		cxChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight + tm.tmExternalLeading;
		ReleaseDC(hWnd, hDC);
		// fall through

	case WM_SETTINGCHANGE:
		hMenu = GetSubMenu(GetMenu(hWnd), 0);

		while (GetMenuItemCount(hMenu) > 1)
			DeleteMenu(hMenu, 1, MF_BYPOSITION);

		// Get a list of all local and remote printers
		//
		// First, find out how large an array we need; this
		//   call will fail, leaving the required size in dwNeeded.
		//
		// Next, allocate space for the info array and fill it
		//
		// Put the printer names on the menu
		
		if (!IsWindowsXPOrGreater())		// Windows 98
		{
			EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 5, NULL, 0, &dwNeeded, &dwReturned);
			pInfo5 = (PRINTER_INFO_5 *)malloc(dwNeeded);
			EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 5, (PBYTE)pInfo5, dwNeeded, &dwNeeded, &dwReturned);

			for (i = 0; i < dwReturned; ++i)
				AppendMenu(hMenu, (i + 1) % 16 ? 0 : MF_MENUBARBREAK, i + 1, pInfo5[i].pPrinterName);

			free(pInfo5);
			pInfo5 = NULL;
		}
		else								// Windows NT
		{
			EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, NULL, 0, &dwNeeded, &dwReturned);
			pInfo4 = (PRINTER_INFO_4 *)malloc(dwNeeded);
			EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, (PBYTE)pInfo4, dwNeeded, &dwNeeded, &dwReturned);

			for (i = 0; i < dwReturned; ++i)
				AppendMenu(hMenu, (i + 1) % 16 ? 0 : MF_MENUBARBREAK, i + 1, pInfo4[i].pPrinterName);

			free(pInfo4);
			pInfo4 = NULL;
		}

		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
		AppendMenu(hMenu, 0, IDM_DEVMODE, TEXT("Properties"));

		wParam = IDM_SCREEN;
		// fall through

	case WM_COMMAND:
		hMenu = GetMenu(hWnd);

		if (LOWORD(wParam) == IDM_SCREEN || LOWORD(wParam) < IDM_DEVMODE)
		{
			// IDM_SCREEN & Printers
			CheckMenuItem(hMenu, nCurrentDevice, MF_UNCHECKED);
			nCurrentDevice = LOWORD(wParam);
			CheckMenuItem(hMenu, nCurrentDevice, MF_CHECKED);
		}
		else if (LOWORD(wParam) == IDM_DEVMODE)
		{
			// Properties selection
			GetMenuString(hMenu, nCurrentDevice, szDevice, sizeof(szDevice) / sizeof(TCHAR), MF_BYCOMMAND);
			if (OpenPrinter(szDevice, &hPrint, NULL))
			{
				PrinterProperties(hWnd, hPrint);
				ClosePrinter(hPrint);
			}
		}
		else
		{
			CheckMenuItem(hMenu, nCurrentDevice, MF_UNCHECKED);
			nCurrentDevice = LOWORD(wParam);
			CheckMenuItem(hMenu, nCurrentDevice, MF_CHECKED);
		}

		InvalidateRect(hWnd, NULL, TRUE);
		return 0;

	case WM_INITMENUPOPUP:
		if (lParam == 0)
			EnableMenuItem(GetMenu(hWnd), IDM_DEVMODE, nCurrentDevice == IDM_SCREEN ? MF_GRAYED : MF_ENABLED);
		
		return 0;

	case WM_PAINT:
		lstrcpy(szWindowText, TEXT("Device Capabilities: "));

		if (nCurrentDevice == IDM_SCREEN)
		{
			lstrcpy(szDevice, TEXT("DISPLAY"));
			hdcInfo = CreateIC(szDevice, NULL, NULL, NULL);
		}
		else
		{
			hMenu = GetMenu(hWnd);
			GetMenuString(hMenu, nCurrentDevice, szDevice, sizeof(szDevice), MF_BYCOMMAND);
			hdcInfo = CreateIC(NULL, szDevice, NULL, NULL);
		}

		lstrcat(szWindowText, szDevice);
		SetWindowText(hWnd, szWindowText);

		hDC = BeginPaint(hWnd, &ps);
		SelectObject(hDC, GetStockObject(SYSTEM_FIXED_FONT));

		if (hdcInfo)
		{
			switch (nCurrentInfo)
			{
			case IDM_BASIC:		DoBasicInfo(hDC, hdcInfo, cxChar, cyChar);		break;
			case IDM_OTHER:		DoOtherInfo(hDC, hdcInfo, cxChar, cyChar);		break;
			case IDM_CURVE:
			case IDM_LINE:
			case IDM_POLY:
			case IDM_TEXT:		DoBitCodedCaps(hDC, hdcInfo, cxChar, cyChar, nCurrentInfo - IDM_CURVE);		break;
			}
			DeleteDC(hdcInfo);
		}

		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}