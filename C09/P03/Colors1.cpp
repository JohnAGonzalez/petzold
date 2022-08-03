#include <Windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ScrollProc(HWND, UINT, WPARAM, LPARAM);

int		idFocus;
WNDPROC	oldScroll[3];

int WINAPI WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	PSTR		szCmdLine,
	int			iCmdShow)
{
	static TCHAR	szAppName[] = TEXT("Colors1");
	HWND			hWnd;
	MSG				msg;
	WNDCLASS		wndClass;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hInstance = hInstance;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName, TEXT("Color Scroll"), WS_OVERLAPPEDWINDOW,
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
	static COLORREF	crPrim[3] = { RGB(255,0,0), RGB(0,255,0), RGB(0,0,255) };
	static HBRUSH	hBrush[3], hBrushStatic;
	static HWND		hWndScroll[3], hWndLabel[3], hWndValue[3], hWndRect;
	static int		i, color[3], cyChar;
	static RECT		rcColor;
	static PCNZCH	szColorLabel[] = { TEXT("Red"), TEXT("Green"), TEXT("Blue") };

	HINSTANCE		hInstance;
	int				cxClient, cyClient;
	TCHAR			szBuffer[10];

	switch (msg)
	{
	case WM_CREATE:
		hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

		// Create the white-rectanble window against which the
		// scroll bars will be positioned.  The child window ID is 9

		hWndRect = CreateWindow(
			TEXT("static"), NULL, WS_CHILD | WS_VISIBLE | SS_WHITERECT,
			0, 0, 0, 0,
			hWnd, (HMENU)9, hInstance, NULL);

		for (i = 0; i < 3; ++i)
		{
			// The three scroll bars have IDs 0, 1, and 2, with
			// scroll bar ranges from 0 through 255.
			hWndScroll[i] = CreateWindow(
				TEXT("scrollbar"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | SBS_VERT,
				0, 0, 0, 0,
				hWnd, (HMENU)i, hInstance, NULL);

			SetScrollRange(hWndScroll[i], SB_CTL, 0, 255, FALSE);
			SetScrollPos(hWndScroll[i], SB_CTL, 0, FALSE);

			// The three color-name labels have IDs 3, 4, and 5,
			// and text strings "Red", "Green", and "Blue".
			hWndLabel[i] = CreateWindow(
				TEXT("static"), NULL, WS_CHILD | WS_VISIBLE | SS_CENTER,
				0, 0, 0, 0,
				hWnd, (HMENU)i + 3, hInstance, NULL);

			// The three color-value text fields have IDs 6, 7,
			// and 8, and initial text strings of "0".
			hWndValue[i] = CreateWindow(
				TEXT("static"), TEXT("0"), WS_CHILD | WS_VISIBLE | SS_CENTER,
				0, 0, 0, 0,
				hWnd, (HMENU)i + 6, hInstance, NULL);

			oldScroll[i] = (WNDPROC)SetWindowLong(hWndScroll[i], GWL_WNDPROC, (LONG)ScrollProc);
			hBrush[i] = CreateSolidBrush(crPrim[i]);
		}

		cyChar = HIWORD(GetDialogBaseUnits());
		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		SetRect(&rcColor, cxClient / 2, 0, cxClient, cyClient);
		MoveWindow(hWndRect, 0, 0, cxClient / 2, cyClient, TRUE);

		for (i = 0; i < 3; ++i)
		{
			MoveWindow(
				hWndScroll[i],
				(2 * i + 1) * cxClient / 14, 2 * cyChar,
				cxClient / 14, cyClient - 4 * cyChar, TRUE);

			MoveWindow(
				hWndLabel[i],
				(4 * i + 1) * cxClient / 28, cyChar / 2,
				cxClient / 7, cyChar, TRUE);

			MoveWindow(
				hWndValue[i],
				(4 * i + 1) * cxClient / 28,
				cyClient - 3 * cyChar / 2,
				cxClient / 7, cyChar, TRUE);
		}

		SetFocus(hWnd);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hWndScroll[idFocus]);
		return 0;

	case WM_VSCROLL:
		i = GetWindowLong((HWND)lParam, GWL_ID);

		switch (LOWORD(wParam))
		{
		case SB_PAGEDOWN:
			color[i] += 15;

			// fall through
		case SB_LINEDOWN:
			color[i] = min(255, color[i] + 1);
			break;

		case SB_PAGEUP:
			color[i] -= 15;

			// fall through
		case SB_LINEUP:
			color[i] = max(0, color[i] - 1);
			break;

		case SB_TOP:
			color[i] = 0;
			break;

		case SB_BOTTOM:
			color[i] = 255;
			break;

		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			color[i] = HIWORD(wParam);
			break;

		default:
			break;

		}

		SetScrollPos(hWndScroll[i], SB_CTL, color[i], TRUE);
		wsprintf(szBuffer, TEXT("%i"), color[i]);
		SetWindowText(hWndValue[i], szBuffer);

		DeleteObject(
			(HBRUSH)SetClassLong(
				hWnd,
				GCL_HBRBACKGROUND,
				(LONG)CreateSolidBrush(
					RGB(color[0], color[1], color[2]))));

		InvalidateRect(hWnd, &rcColor, TRUE);
		return 0;

	case WM_CTLCOLORSCROLLBAR:
		i = GetWindowLong((HWND)lParam, GWL_ID);
		return (LRESULT)hBrush[i];

	case WM_CTLCOLORSTATIC:
		i = GetWindowLong((HWND)lParam, GWL_ID);

		if (i >= 3 && i <= 8)
		{
			SetTextColor((HDC)wParam, crPrim[i % 3]);
			SetBkColor((HDC)wParam, GetSysColor(COLOR_BTNHIGHLIGHT));
			return (LRESULT)hBrushStatic;
		}

		break;

	case WM_SYSCOLORCHANGE:
		DeleteObject(hBrushStatic);
		hBrushStatic = CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));
		return 0;

	case WM_DESTROY:
		DeleteObject(
			(HBRUSH)SetClassLong(
				hWnd,
				GCL_HBRBACKGROUND,
				(LONG)GetStockObject(WHITE_BRUSH)));

		for (i = 0; i < 3; ++i)
			DeleteObject(hBrush[i]);

		DeleteObject(hBrushStatic);
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK ScrollProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	int id = GetWindowLong(hWnd, GWL_ID);

	switch (msg)
	{
	case WM_KEYDOWN:
		if (VK_TAB == wParam)
			SetFocus(
				GetDlgItem(
					GetParent(hWnd),
					(id + (GetKeyState(VK_SHIFT) < 0 ? 2 : 1)) % 3));

		break;

	case WM_SETFOCUS:
		idFocus = id;
		break;
	
	}

	return CallWindowProc(oldScroll[id], hWnd, msg, wParam, lParam);
}