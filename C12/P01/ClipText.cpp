#include <Windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#ifdef UNICODE
#define CF_TCHAR CF_UNICODETEXT
TCHAR szDefaultText[] = TEXT("Default Text - Unicode Version");
TCHAR szCaption[] = TEXT("Clipboard Text Transfers - Unicode Version");
#else
#define CF_TCHAR CF_TEXT
TCHAR szDefaultText[] = TEXT("Default Text - ANSI Version");
TCHAR szCaption[] = TEXT("Clipboard Text Transfers - ANSI Version");
#endif

TCHAR szAppName[] = TEXT("ClipText");

int WINAPI WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	PSTR		szCmdLine,
	int			iCmdShow)
{
	HWND		hWnd;
	MSG			msg;
	WNDCLASS	wndClass;
	HACCEL		hAccel;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hInstance = hInstance;
	wndClass.lpszMenuName = MAKEINTRESOURCE(CLIPTEXT);
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName, szCaption, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(CLIPTEXT));

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

LPARAM CALLBACK WndProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static PTSTR	pText;
	BOOL			bEnable;
	HGLOBAL			hGlobal;
	HDC				hDC;
	PTSTR			pGlobal;
	PAINTSTRUCT		ps;
	RECT			rect;

	switch (msg)
	{
	case WM_CREATE:
		SendMessage(hWnd, WM_COMMAND, IDM_EDIT_RESET, 0);
		return 0;

	case WM_INITMENUPOPUP:
		EnableMenuItem((HMENU)wParam, IDM_EDIT_PASTE, IsClipboardFormatAvailable(CF_TCHAR) ? MF_ENABLED : MF_GRAYED);

		bEnable = pText ? MF_ENABLED : MF_GRAYED;

		EnableMenuItem((HMENU)wParam, IDM_EDIT_CUT, bEnable);
		EnableMenuItem((HMENU)wParam, IDM_EDIT_COPY, bEnable);
		EnableMenuItem((HMENU)wParam, IDM_EDIT_CLEAR, bEnable);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_EDIT_PASTE:
			OpenClipboard(hWnd);

			if (hGlobal = GetClipboardData(CF_TCHAR))
			{
				pGlobal = (PTSTR)GlobalLock(hGlobal);

				if (pText)
				{
					free(pText);
					pText = NULL;
				}

				pText = (PTSTR)malloc(GlobalSize(hGlobal));
				lstrcpy(pText, pGlobal);
				InvalidateRect(hWnd, &rect, TRUE);
			}

			CloseClipboard();
			return 0;

		case IDM_EDIT_CUT:
		case IDM_EDIT_COPY:
			if (!pText)
				return 0;

			hGlobal = GlobalAlloc(GHND | GMEM_SHARE, (lstrlen(pText) + 1) * sizeof(TCHAR));
			pGlobal = (PTSTR)GlobalLock(hGlobal);
			lstrcpy(pGlobal, pText);
			GlobalUnlock(hGlobal);

			OpenClipboard(hWnd);
			EmptyClipboard();
			SetClipboardData(CF_TCHAR, hGlobal);
			CloseClipboard();

			if (IDM_EDIT_COPY == LOWORD(wParam))
				return 0;	// fall through for IDM_EDIT_CUT

		case IDM_EDIT_CLEAR:
			if (pText)
			{
				free(pText);
				pText = NULL;
			}
			InvalidateRect(hWnd, &rect, TRUE);
			return 0;

		case IDM_EDIT_RESET:
			if (pText)
			{
				free(pText);
				pText = NULL;
			}

			pText = (PTSTR)malloc((lstrlen(szDefaultText) + 1) * sizeof(TCHAR));
			lstrcpy(pText, szDefaultText);
			InvalidateRect(hWnd, NULL, TRUE);
			return 0;
		
		}
		break;
		
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &rect);

		if (pText != NULL)
			DrawText(hDC, pText, -1, &rect, DT_EXPANDTABS | DT_WORDBREAK);

		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		if (pText)
			free(pText);

		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
