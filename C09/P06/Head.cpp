#include <Windows.h>

#define ID_LIST		1
#define ID_TEXT		2

#define MAXREAD		8192
#define DIRATTR		(DDL_READWRITE|DDL_READONLY|DDL_HIDDEN|DDL_SYSTEM|DDL_DIRECTORY|DDL_ARCHIVE|DDL_DRIVES)
#define DTFLAGS		(DT_WORDBREAK|DT_EXPANDTABS|DT_NOCLIP|DT_NOPREFIX)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ListProc(HWND, UINT, WPARAM, LPARAM);

WNDPROC oldList;

TCHAR	szAppName[] = TEXT("head");

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
	wndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndClass.hInstance = hInstance;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName, TEXT(""), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
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
	static BOOL		bValidFile;
	static BYTE		buffer[MAXREAD];
	static HWND		hWndList, hWndText;
	static RECT		rect;
	static TCHAR	szFile[MAX_PATH + 1];
	HANDLE			hFile;
	HDC				hDC;
	DWORD			i;
	int				cxChar, cyChar;
	PAINTSTRUCT		ps;
	TCHAR			szBuffer[MAX_PATH + 1];

	switch (msg)
	{
	case WM_CREATE:
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());

		rect.left = 20 * cyChar;
		rect.top = 3 * cyChar;

		hWndList = CreateWindow(
			TEXT("listbox"), NULL, WS_CHILDWINDOW | WS_VISIBLE | LBS_STANDARD,
			cxChar, cyChar * 3, cxChar * 13 + GetSystemMetrics(SM_CXVSCROLL), cyChar * 10,
			hWnd, (HMENU)ID_LIST, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

		GetCurrentDirectory(MAX_PATH + 1, szBuffer);

		hWndText = CreateWindow(
			TEXT("static"), szBuffer, WS_CHILDWINDOW | WS_VISIBLE | SS_LEFT,
			cxChar, cyChar, cxChar * MAX_PATH, cyChar,
			hWnd, (HMENU)ID_TEXT, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

		oldList = (WNDPROC)SetWindowLong(hWndList, GWL_WNDPROC, (LPARAM)ListProc);

		SendMessage(hWndList, LB_DIR, DIRATTR, (LPARAM)TEXT("*.*"));
		return 0;

	case WM_SIZE:
		rect.right = LOWORD(lParam);
		rect.bottom = HIWORD(lParam);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hWndList);
		return 0;

	case WM_COMMAND:
		if (ID_LIST == LOWORD(wParam) && LBN_DBLCLK == HIWORD(wParam))
		{
			if (LB_ERR == (i = SendMessage(hWndList, LB_GETCURSEL, 0, 0)))
				break;

			SendMessage(hWndList, LB_GETTEXT, i, (LPARAM)szBuffer);

			if (INVALID_HANDLE_VALUE != (hFile = CreateFile(szBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)))
			{
				CloseHandle(hFile);
				bValidFile = TRUE;
				lstrcpy(szFile, szBuffer);
				GetCurrentDirectory(MAX_PATH + 1, szBuffer);

				if (szBuffer[lstrlen(szBuffer) - 1] != '\\')
					lstrcat(szBuffer, TEXT("\\"));

				SetWindowText(hWndText, lstrcat(szBuffer, szFile));
			}
			else
			{
				bValidFile = FALSE;
				szBuffer[lstrlen(szBuffer) - 1] = '\0';

				// If setting the cirectory doesn't work, maybe it's
				// a drive change, so try that.
				if (!SetCurrentDirectory(szBuffer + 1))
				{
					szBuffer[3] = ':';
					szBuffer[4] = '\0';
					SetCurrentDirectory(szBuffer + 2);
				}

				// Get the new directory name and fill the list box
				GetCurrentDirectory(MAX_PATH + 1, szBuffer);
				SetWindowText(hWndText, szBuffer);
				SendMessage(hWndList, LB_RESETCONTENT, 0, 0);
				SendMessage(hWndList, LB_DIR, DIRATTR, (LPARAM)TEXT("*.*"));
			}

			InvalidateRect(hWnd, NULL, TRUE);
		}
		return 0;

	case WM_PAINT:
		if (!bValidFile)
			break;

		if (INVALID_HANDLE_VALUE == (hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)))
		{
			bValidFile = FALSE;
			break;
		}

		ReadFile(hFile, buffer, MAXREAD, &i, NULL);
		CloseHandle(hFile);

		// i now equals the number of bytes in buffer.
		// Commence getting a device context for displaying text.

		hDC = BeginPaint(hWnd, &ps);
		SelectObject(hDC, GetStockObject(SYSTEM_FIXED_FONT));
		SetTextColor(hDC, GetSysColor(COLOR_BTNTEXT));
		SetBkColor(hDC, GetSysColor(COLOR_BTNFACE));

		// Assume the file is ASCII
		DrawTextA(hDC, (LPCSTR)buffer, i, &rect, DTFLAGS);

		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK ListProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	if (WM_KEYDOWN == msg && VK_RETURN == wParam)
		SendMessage(GetParent(hWnd), WM_COMMAND, MAKELONG(1, LBN_DBLCLK), (LPARAM)hWnd);

	return CallWindowProc(oldList, hWnd, msg, wParam, lParam);
}