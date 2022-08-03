#include <Windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szCmdLine,
	int iCmdShow)
{
	static TCHAR	szAppName[] = TEXT("KeyView2");
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

	hWnd = CreateWindow(szAppName, TEXT("Keyboard Message Viewer #2"), WS_OVERLAPPEDWINDOW,
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
	static DWORD	dwCharSet = DEFAULT_CHARSET;
	static int		cxClientMax, cyClientMax, cxClient, cyClient, cxChar, cyChar;
	static int		cLinesMax, cLines;
	static PMSG		pMsg;
	static RECT		rectScroll;
	static TCHAR	szTop[] =
		TEXT("Message        Key       Char     ")
		TEXT("Repeat Scan Ext ALT Prev TRAN");
	static TCHAR	szUnd[] =
		TEXT("_______        ___       ____     ")
		TEXT("______ ____ ___ ___ ____ ____");
	static PCNZCH	szFormat[2] =
	{
		TEXT("%-13s %3d %-15s%c%6u %4d %3s %3s %4s %4s"),
		TEXT("%-13s            0x%04X%1s%c %6u %4d %3s %3s %4s %4s")
	};

	static TCHAR	szYes[] = TEXT("Yes");
	static TCHAR	szNo[] = TEXT("No");
	static TCHAR	szDown[] = TEXT("Down");
	static TCHAR	szUp[] = TEXT("Up");

	static PCNZCH	szMessage[] =
	{
		TEXT("WM_KEYDOWN"),			TEXT("WM_KEYUP"),
		TEXT("WM_CHAR"),			TEXT("WM_DEADCHAR"),
		TEXT("WM_SYSKEYDOWN"),		TEXT("WM_SYSKEYUP"),
		TEXT("WM_SYSCHAR"),			TEXT("WM_SYSDEADCHAR")
	};

	HDC			hDC;
	int			i, iType;
	PAINTSTRUCT	ps;
	TCHAR		szBuffer[128], szKeyName[32];
	TEXTMETRIC	tm;

	switch (msg)
	{
	case WM_INPUTLANGCHANGE:
		dwCharSet = wParam;

	case WM_CREATE:
	case WM_DISPLAYCHANGE:
		// Get maximum size of client area
		cxClientMax = GetSystemMetrics(SM_CXMAXIMIZED);
		cyClientMax = GetSystemMetrics(SM_CYMAXIMIZED);

		// Get character size for fixed-pitch font
		hDC = GetDC(hWnd);

		SelectObject(hDC, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, dwCharSet, 0, 0, 0, FIXED_PITCH, NULL));

		GetTextMetrics(hDC, &tm);
		cxChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight;

		DeleteObject(SelectObject(hDC, GetStockObject(SYSTEM_FONT)));
		ReleaseDC(hWnd, hDC);

		// Allocate memory for display lines
		if (pMsg)
			free(pMsg);

		cLinesMax = cyClientMax / cyChar;
		pMsg = (PMSG)malloc(cLinesMax * sizeof(MSG));
		cLines = 0;

	case WM_SIZE:
		if (msg == WM_SIZE)
		{
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
		}

		// Calculate scrolling rectangle
		rectScroll.left = 0;
		rectScroll.right = cxClient;
		rectScroll.top = cyChar;
		rectScroll.bottom = cyChar * (cyClient / cyChar);

		InvalidateRect(hWnd, NULL, TRUE);

		if (msg == WM_INPUTLANGCHANGE)
			return TRUE;

		return 0;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_DEADCHAR:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSCHAR:
	case WM_SYSDEADCHAR:
		// Rearrange storage array
		for (i = cLinesMax - 1; i > 0; --i)
		{
			pMsg[i] = pMsg[i - 1];
		}

		// Store new message
		pMsg[0].hwnd = hWnd;
		pMsg[0].message = msg;
		pMsg[0].wParam = wParam;
		pMsg[0].lParam = lParam;

		cLines = min(cLines + 1, cLinesMax);

		// Scroll up the display
		ScrollWindow(hWnd, 0, -cyChar, &rectScroll, &rectScroll);
		break;	// to allow the default processing of system messages

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		SelectObject(hDC, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, dwCharSet, 0, 0, 0, FIXED_PITCH, NULL));

		SetBkMode(hDC, TRANSPARENT);
		TextOut(hDC, 0, 0, szTop, lstrlen(szTop));
		TextOut(hDC, 0, 0, szUnd, lstrlen(szUnd));

		for (i = 0; i < min(cLines, cyClient / cyChar - 1); ++i)
		{
			iType =
				pMsg[i].message == WM_CHAR ||
				pMsg[i].message == WM_SYSCHAR ||
				pMsg[i].message == WM_DEADCHAR ||
				pMsg[i].message == WM_SYSDEADCHAR;

			GetKeyNameText(pMsg[i].lParam, szKeyName, sizeof(szKeyName) / sizeof(TCHAR));
			TextOut(hDC,
				0,
				(cyClient / cyChar - 1 - i) * cyChar,
				szBuffer,
				wsprintf(szBuffer,
					szFormat[iType],
					szMessage[pMsg[i].message - WM_KEYFIRST],
					pMsg[i].wParam,
					(PTSTR)(iType ? TEXT(" ") : szKeyName),
					(TCHAR)(iType ? pMsg[i].wParam : ' '),
					LOWORD(pMsg[i].lParam),
					HIWORD(pMsg[i].lParam) & 0xFF,
					0x01000000 & pMsg[i].lParam ? szYes : szNo,
					0x20000000 & pMsg[i].lParam ? szYes : szNo,
					0x40000000 & pMsg[i].lParam ? szDown : szUp,
					0x80000000 & pMsg[i].lParam ? szUp : szDown));
		}

		DeleteObject(SelectObject(hDC, GetStockObject(SYSTEM_FONT)));
		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}