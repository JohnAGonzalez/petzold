#include <Windows.h>
#include "resource.h"

#define ID_TIMER	1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = TEXT("MenuDemo");

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
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hInstance = hInstance;
	wndClass.lpszMenuName = szAppName;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName, TEXT("Menu Demonstration"), WS_OVERLAPPEDWINDOW,
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
	static int	idColor[5] = { WHITE_BRUSH, LTGRAY_BRUSH,GRAY_BRUSH,DKGRAY_BRUSH,BLACK_BRUSH };
	static int	iSelection = IDM_BKGND_WHITE;
	HMENU		hMenu;

	switch (msg)
	{
	case WM_COMMAND:
		hMenu = GetMenu(hWnd);

		switch (LOWORD(wParam))
		{
		case IDM_FILE_NEW:
		case IDM_FILE_OPEN:
		case IDM_FILE_SAVE:
		case IDM_FILE_SAVE_AS:
		case IDM_EDIT_UNDO:
		case IDM_EDIT_CUT:
		case IDM_EDIT_COPY:
		case IDM_EDIT_PASTE:
		case IDM_EDIT_CLEAR:
			MessageBeep(0);
			return 0;

		case IDM_APP_EXIT:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			return 0;

		case IDM_BKGND_WHITE:		// Note: Logic below
		case IDM_BKGND_LTGRAY:		//   assumes that IDM_WHITE
		case IDM_BKGND_GRAY:		//   through IDM_BLACK are
		case IDM_BKGND_DKGRAY:		//   consecutive in
		case IDM_BKGND_BLACK:		//   the order shown here
			CheckMenuItem(hMenu, iSelection, MF_UNCHECKED);
			iSelection = LOWORD(wParam);
			CheckMenuItem(hMenu, iSelection, MF_CHECKED);

			SetClassLong(hWnd, GCL_HBRBACKGROUND, (LONG)GetStockObject(idColor[LOWORD(wParam) - IDM_BKGND_WHITE]));

			InvalidateRect(hWnd, NULL, TRUE);
			return 0;

		case IDM_TIMER_START:
			if (SetTimer(hWnd, ID_TIMER, 1000, NULL))
			{
				EnableMenuItem(hMenu, IDM_TIMER_START, MF_GRAYED);
				EnableMenuItem(hMenu, IDM_TIMER_STOP, MF_ENABLED);
			}
			return 0;

		case IDM_TIMER_STOP:
			KillTimer(hWnd, ID_TIMER);
			EnableMenuItem(hMenu, IDM_TIMER_START, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_TIMER_STOP, MF_GRAYED);
			return 0;

		case IDM_APP_HELP:
			MessageBox(hWnd, TEXT("Help not yet implemented!"), szAppName, MB_ICONEXCLAMATION | MB_OK);
			return 0;

		case IDM_APP_ABOUT:
			MessageBox(hWnd, TEXT("Menu Demonstration Program\n(c) Charles Petzold, 1998"), szAppName, MB_ICONINFORMATION | MB_OK);
			return 0;

		}
		break;

	case WM_TIMER:
		MessageBeep(0);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void DefineMenu()
{
	HMENU hMenu = CreateMenu();

	HMENU hMenuPopup = CreateMenu();
	AppendMenu(hMenuPopup, MF_STRING, IDM_FILE_NEW, "&New");
	AppendMenu(hMenuPopup, MF_STRING, IDM_FILE_OPEN, "&Open...");
	AppendMenu(hMenuPopup, MF_STRING, IDM_FILE_SAVE, "&Save");
	AppendMenu(hMenuPopup, MF_STRING, IDM_FILE_SAVE_AS, "Save &As...");
	AppendMenu(hMenuPopup, MF_SEPARATOR, 0, NULL);
	AppendMenu(hMenuPopup, MF_STRING, IDM_APP_EXIT, "E&xit");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, "&File");

	hMenuPopup = CreateMenu();
	AppendMenu(hMenuPopup, MF_STRING, IDM_EDIT_UNDO, "&Undo");
	AppendMenu(hMenuPopup, MF_SEPARATOR, 0, NULL);
	AppendMenu(hMenuPopup, MF_STRING, IDM_EDIT_CUT, "&Cu&t");
	AppendMenu(hMenuPopup, MF_STRING, IDM_EDIT_COPY, "&Copy");
	AppendMenu(hMenuPopup, MF_STRING, IDM_EDIT_PASTE, "&Paste");
	AppendMenu(hMenuPopup, MF_STRING, IDM_EDIT_CLEAR, "De&lete");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, "&Edit");

	hMenuPopup = CreateMenu();
	AppendMenu(hMenuPopup, MF_STRING | MF_CHECKED, IDM_BKGND_WHITE, "&White");
	AppendMenu(hMenuPopup, MF_STRING, IDM_BKGND_LTGRAY, "&Light Gray");
	AppendMenu(hMenuPopup, MF_STRING, IDM_BKGND_GRAY, "&Gray");
	AppendMenu(hMenuPopup, MF_STRING, IDM_BKGND_DKGRAY, "&Dark Gray");
	AppendMenu(hMenuPopup, MF_STRING, IDM_BKGND_BLACK, "&Black");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, "&Background");

	hMenuPopup = CreateMenu();
	AppendMenu(hMenuPopup, MF_STRING, IDM_TIMER_START, "&Start");
	AppendMenu(hMenuPopup, MF_STRING | MF_GRAYED, IDM_TIMER_STOP, "S&top");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, "&Timer");

	hMenuPopup = CreateMenu();
	AppendMenu(hMenuPopup, MF_STRING, IDM_APP_HELP, "&Help");
	AppendMenu(hMenuPopup, MF_STRING, IDM_APP_ABOUT, "&About MenuDemo...");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, "&Help");
}