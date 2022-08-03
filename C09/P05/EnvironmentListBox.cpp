#include <Windows.h>

#define ID_LIST		1
#define ID_TEXT		2

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = TEXT("EnvironListBox");

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
	wndClass.lpszMenuName = 0;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName, TEXT("Environment List Box"), WS_OVERLAPPEDWINDOW,
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

void FillListBox(HWND hWndList)
{
	int		iLength;
	LPCH	pVarSource, pVarBlock, pVarBeg, pVarEnd;
	TCHAR *	pVarName;

	// get pointer to the environment block
	pVarBlock = pVarSource = GetEnvironmentStrings();

	while (*pVarBlock)
	{
		// skip variable names beginning with '='
		if (*pVarBlock != '=')
		{
			pVarBeg = pVarBlock;			// beginning of variable name
			while (*pVarBlock++ != '=');	// scan until '='
			pVarEnd = pVarBlock - 1;		// points to the '=' sign
			iLength = pVarEnd - pVarBeg;	// length of variable name

			// Allocate memory for the variable name and terminating
			// zero.  Copy the variable name and append a zero.
			pVarName = (TCHAR *)calloc(iLength + 1, sizeof(TCHAR));
			CopyMemory(pVarName, pVarBeg, iLength * sizeof(TCHAR));
			pVarName[iLength] = '\0';

			// Put the variable name in the list box and free memory.
			SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)pVarName);
			free(pVarName);
		}

		// Scan until terminating zero
		while (*pVarBlock++ != '\0');
	}

	FreeEnvironmentStrings(pVarSource);
}

LRESULT CALLBACK WndProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static HWND		hWndList, hWndText;
	int				iIndex, iLength, cxChar, cyChar;
	LPCH			pVarName, pVarValue;

	switch (msg)
	{
	case WM_CREATE:
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());

		// Create listbox and static text windows
		hWndList = CreateWindow(
			TEXT("listbox"), NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
			cxChar, cyChar * 3, cxChar * 64 + GetSystemMetrics(SM_CXVSCROLL), cyChar * 15,
			hWnd, (HMENU)ID_LIST, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

		hWndText = CreateWindow(
			TEXT("static"), NULL, WS_CHILD | WS_VISIBLE | SS_LEFT,
			cxChar, cyChar, GetSystemMetrics(SM_CXSCREEN), cyChar,
			hWnd, (HMENU)ID_TEXT, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

		FillListBox(hWndList);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hWndList);
		return 0;

	case WM_COMMAND:
		if (ID_LIST == LOWORD(wParam) && LBN_SELCHANGE == HIWORD(wParam))
		{
			// Get current selection
			iIndex = SendMessage(hWndList, LB_GETCURSEL, 0, 0);
			iLength = SendMessage(hWndList, LB_GETTEXTLEN, iIndex, 0) + 1;
			pVarName = (TCHAR *)calloc(iLength, sizeof(TCHAR));
			SendMessage(hWndList, LB_GETTEXT, iIndex, (LPARAM)pVarName);

			// Get environment string
			iLength = GetEnvironmentVariable(pVarName, NULL, 0);
			pVarValue = (TCHAR *)calloc(iLength, sizeof(TCHAR));
			GetEnvironmentVariable(pVarName, pVarValue, iLength);

			// Show it in window
			SetWindowText(hWndText, pVarValue);
			free(pVarName);
			free(pVarValue);
		}

		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}