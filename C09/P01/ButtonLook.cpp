#include <Windows.h>

struct
{
	int		iStyle;
	PCNZCH	szText;
}
button[] =
{
	BS_PUSHBUTTON,			TEXT("PUSHBUTTON"),
	BS_DEFPUSHBUTTON,		TEXT("DEFPUSHBUTTON"),
	BS_CHECKBOX,			TEXT("CHECKBOX"),
	BS_AUTOCHECKBOX,		TEXT("AUTOCHECKBOX"),
	BS_RADIOBUTTON,			TEXT("RADIOBUTTON"),
	BS_3STATE,				TEXT("3STATE"),
	BS_AUTO3STATE,			TEXT("AUTO3STATE"),
	BS_GROUPBOX,			TEXT("GROUPBOX"),
	BS_AUTORADIOBUTTON,		TEXT("AUTORADIOBUTTON"),
	BS_OWNERDRAW,			TEXT("OWNERDRAW")
};

#define NUM	(sizeof(button) / sizeof(button[0]))

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	PSTR		szCmdLine,
	int			iCmdShow)
{
	static TCHAR	szAppName[] = TEXT("ButtonLook");
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

	hWnd = CreateWindow(
		szAppName, TEXT("Button Look"), WS_OVERLAPPEDWINDOW,
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
	static HWND		hWndButton[NUM];
	static RECT		rect;
	static TCHAR	szTop[] = TEXT("message            wParam       lParam"),
					szUnd[] = TEXT("_______            ______       ______"),
					szFmt[] = TEXT("%-16s%04X-%04x    %04x-%04x"),
					szBuf[50];
	static int		cxChar, cyChar;
	HDC				hDC;
	PAINTSTRUCT		ps;

	switch (msg)
	{
	case WM_CREATE:
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());

		for (int i = 0; i < NUM; ++i)
			hWndButton[i] = CreateWindow(
				TEXT("button"), button[i].szText, WS_CHILD | WS_VISIBLE | button[i].iStyle,
				cxChar, cyChar * (1 + 2 * i), 20 * cxChar, 7 * cyChar / 4,
				hWnd, (HMENU)i, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

		return 0;

	case WM_SIZE:
		rect.left = 24 * cxChar;
		rect.top = 2 * cyChar;
		rect.right = LOWORD(lParam);
		rect.bottom = HIWORD(lParam);
		return 0;

	case WM_PAINT:
		InvalidateRect(hWnd, &rect, TRUE);

		hDC = BeginPaint(hWnd, &ps);
		SelectObject(hDC, GetStockObject(SYSTEM_FIXED_FONT));
		SetBkMode(hDC, TRANSPARENT);

		TextOut(hDC, 24 * cxChar, cyChar, szTop, lstrlen(szTop));
		TextOut(hDC, 24 * cxChar, cyChar, szUnd, lstrlen(szUnd));

		EndPaint(hWnd, &ps);
		return 0;

	case WM_DRAWITEM:
	case WM_COMMAND:
		ScrollWindow(hWnd, 0, -cyChar, &rect, &rect);

		hDC = GetDC(hWnd);
		SelectObject(hDC, GetStockObject(SYSTEM_FIXED_FONT));

		TextOut(hDC, 24 * cxChar, cyChar * (rect.bottom / cyChar - 1), szBuf,
			wsprintf(szBuf, szFmt,
				msg == WM_DRAWITEM ? TEXT("WM_DRAWITEM") : TEXT("WM_COMMAND"),
				HIWORD(wParam), LOWORD(wParam),
				HIWORD(lParam), LOWORD(lParam)));

		ReleaseDC(hWnd, hDC);
		ValidateRect(hWnd, &rect);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}