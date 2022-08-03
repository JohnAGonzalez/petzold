#include <Windows.h>
#include "resource.h"

#define ID_EDIT		1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = TEXT("PopPad2");

int WINAPI WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	PSTR		szCmdLine,
	int			iCmdShow)
{
	HACCEL		hAccel;
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
	wndClass.lpszMenuName = MAKEINTRESOURCE(POPPAD2);
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName, szAppName, WS_OVERLAPPEDWINDOW,
		GetSystemMetrics(SM_CXSCREEN) / 4, GetSystemMetrics(SM_CYSCREEN) / 4, GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	hAccel = LoadAccelerators(hInstance, szAppName);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hWnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

int AskConfirmation(HWND hWnd)
{
	return MessageBox(hWnd, TEXT("Really want to close PopPad2?"), szAppName, MB_ICONQUESTION | MB_YESNO);
}

LRESULT CALLBACK WndProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static HWND	hWndEdit;
	int			iSelect, iEnable;

	switch (msg)
	{
	case WM_CREATE:
		hWndEdit = CreateWindow(
			TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
			0, 0, 0, 0,
			hWnd, (HMENU)ID_EDIT, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

		return 0;

	case WM_SETFOCUS:
		SetFocus(hWndEdit);
		return 0;

	case WM_SIZE:
		MoveWindow(hWndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;

	case WM_INITMENUPOPUP:
		if (1 == lParam)
		{
			EnableMenuItem((HMENU)wParam, IDM_EDIT_UNDO, SendMessage(hWndEdit, EM_CANUNDO, 0, 0) ? MF_ENABLED : MF_GRAYED);
			EnableMenuItem((HMENU)wParam, IDM_EDIT_PASTE, IsClipboardFormatAvailable(CF_TEXT) ? MF_ENABLED : MF_GRAYED);
			iSelect = SendMessage(hWndEdit, EM_GETSEL, 0, 0);

			if (HIWORD(iSelect) == LOWORD(iSelect))
				iEnable = MF_GRAYED;
			else
				iEnable = MF_ENABLED;

			EnableMenuItem((HMENU)wParam, IDM_EDIT_CUT, iEnable);
			EnableMenuItem((HMENU)wParam, IDM_EDIT_COPY, iEnable);
			EnableMenuItem((HMENU)wParam, IDM_EDIT_DELETE, iEnable);
			return 0;
		}
		break;

	case WM_COMMAND:
		if (lParam)
		{
			if (ID_EDIT == LOWORD(lParam) && (EN_ERRSPACE == HIWORD(wParam) || EN_MAXTEXT == HIWORD(wParam)))
				MessageBox(hWnd, TEXT("Edit control out of space."), szAppName, MB_ICONSTOP | MB_OK);

			return 0;
		}
		else
		{
			switch (LOWORD(wParam))
			{
			case IDM_FILE_NEW:
			case IDM_FILE_OPEN:
			case IDM_FILE_SAVE:
			case IDM_FILE_SAVEAS:
			case IDM_FILE_PRINT:		MessageBeep(0);							return 0;
			case IDM_FILE_EXIT:			SendMessage(hWnd, WM_CLOSE, 0, 0);		return 0;
			case IDM_EDIT_UNDO:			SendMessage(hWnd, WM_UNDO, 0, 0);		return 0;
			case IDM_EDIT_CUT:			SendMessage(hWnd, WM_CUT, 0, 0);		return 0;
			case IDM_EDIT_COPY:			SendMessage(hWnd, WM_COPY, 0, 0);		return 0;
			case IDM_EDIT_PASTE:		SendMessage(hWnd, WM_PASTE, 0, 0);		return 0;
			case IDM_EDIT_DELETE:		SendMessage(hWnd, WM_CLEAR, 0, 0);		return 0;
			case IDM_EDIT_SELECTALL:	SendMessage(hWnd, EM_SETSEL, 0, -1);	return 0;
			case IDM_HELP_HELP:
				MessageBox(hWnd, TEXT("Help not yet implemented!"), szAppName, MB_ICONEXCLAMATION | MB_OK);
				return 0;

			case IDM_HELP_ABOUT:
				MessageBox(hWnd, TEXT("POPPAD2 (c) Charles Petzold, 1998"), szAppName, MB_ICONINFORMATION | MB_OK);
				return 0;
			}

		}
		break;

	case WM_CLOSE:
		if (IDYES == AskConfirmation(hWnd))
			DestroyWindow(hWnd);
		return 0;

	case WM_QUERYENDSESSION:
		if (IDYES == AskConfirmation(hWnd))
			return 1;
		else
			return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}