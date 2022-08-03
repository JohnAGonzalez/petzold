#include <Windows.h>

#define DIVISIONS	5

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);

int		idFocus = 0;
TCHAR	szChildClass[] = TEXT("Checker4_Child");

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szCmdLine,
	int iCmdShow)
{
	static TCHAR	szAppName[] = TEXT("Checker4");
	HWND			hWnd;
	MSG				msg;
	WNDCLASS		wndClass;

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
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	wndClass.lpfnWndProc = ChildWndProc;
	wndClass.cbWndExtra = sizeof(long);
	wndClass.hIcon = NULL;
	wndClass.lpszClassName = szChildClass;

	RegisterClass(&wndClass);

	hWnd = CreateWindow(szAppName, TEXT("Checker4 Mouse Hit-Test Demo"), WS_OVERLAPPEDWINDOW,
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
	static HWND	hWndChild[DIVISIONS][DIVISIONS];
	int			cxBlock, cyBlock, x, y;

	switch (msg)
	{
	case WM_CREATE:
		for (x = 0; x < DIVISIONS; ++x)
			for (y = 0; y < DIVISIONS; ++y)
				hWndChild[x][y] = CreateWindow(
					szChildClass, NULL, WS_CHILDWINDOW | WS_VISIBLE,
					0, 0, 0, 0,
					hWnd, (HMENU)(y << 8 | x),
					(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
					NULL);

		return 0;

	case WM_SIZE:
		cxBlock = LOWORD(lParam) / DIVISIONS;
		cyBlock = HIWORD(lParam) / DIVISIONS;

		for (x = 0; x < DIVISIONS; ++x)
			for (y = 0; y < DIVISIONS; ++y)
				MoveWindow(
					hWndChild[x][y],
					x * cxBlock, y * cyBlock,
					cxBlock, cyBlock, TRUE);

		return 0;

	case WM_LBUTTONDOWN:
		MessageBeep(0);
		return 0;

	case WM_SETFOCUS:
		SetFocus(GetDlgItem(hWnd, idFocus));
		return 0;

	case WM_KEYDOWN:
		x = idFocus & 0xFF;
		y = idFocus >> 8;

		switch (wParam)
		{
		case VK_UP:		--y;					break;
		case VK_DOWN:	++y;					break;
		case VK_LEFT:	--x;					break;
		case VK_RIGHT:	++x;					break;
		case VK_HOME:	x = y = 0;				break;
		case VK_END:	x = y = DIVISIONS - 1;	break;
		default:								return 0;
		}

		x = (x + DIVISIONS) % DIVISIONS;
		y = (y + DIVISIONS) % DIVISIONS;

		idFocus = y << 8 | x;

		SetFocus(GetDlgItem(hWnd, idFocus));
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK ChildWndProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	HDC			hDC;
	PAINTSTRUCT	ps;
	RECT		rect;

	switch (msg)
	{
	case WM_CREATE:			// set the on/off flag
		SetWindowLong(hWnd, 0, 0);
		return 0;

	case WM_KEYDOWN:		// send most key presses to the parent window
		if (wParam != VK_RETURN && wParam != VK_SPACE)
		{
			SendMessage(GetParent(hWnd), msg, wParam, lParam);
			return 0;
		}

	case WM_LBUTTONDOWN:	// for return and space, fall through to toggle the square
		SetWindowLong(hWnd, 0, 1 ^ GetWindowLong(hWnd, 0));
		SetFocus(hWnd);
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;

	case WM_SETFOCUS:		// for focus messages, invalidate the window and fall through
		idFocus = GetWindowLong(hWnd, GWL_ID);

	case WM_KILLFOCUS:		// repaint
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;

	case WM_PAINT:			// draw the "X" mark
		hDC = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &rect);
		Rectangle(hDC, 0, 0, rect.right, rect.bottom);

		if (GetWindowLong(hWnd, 0))
		{
			MoveToEx(hDC, 0, 0, NULL);
			LineTo(hDC, rect.right, rect.bottom);
			MoveToEx(hDC, 0, rect.bottom, NULL);
			LineTo(hDC, rect.right, 0);
		}

		// draw the "focus" rectangle
		if (hWnd == GetFocus())
		{
			rect.left	+= rect.right / 10;
			rect.right	-= rect.left;
			rect.top	+= rect.bottom / 10;
			rect.bottom	-= rect.top;

			SelectObject(hDC, GetStockObject(NULL_BRUSH));
			SelectObject(hDC, CreatePen(PS_DASH, 0, 0));
			Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
			DeleteObject(SelectObject(hDC, GetStockObject(BLACK_PEN)));
		}

		EndPaint(hWnd, &ps);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}