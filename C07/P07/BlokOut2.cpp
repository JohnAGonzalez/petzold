#include <Windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szCmdLine,
	int iCmdShow)
{
	static TCHAR	szAppName[] = TEXT("BlokOut2");
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

	hWnd = CreateWindow(szAppName, TEXT("Mouse Button & Capture Demo"), WS_OVERLAPPEDWINDOW,
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

void DrawBoxOutline(HWND hWnd, POINT ptBeg, POINT ptEnd)
{
	HDC	hDC = GetDC(hWnd);

	SetROP2(hDC, R2_NOT);
	SelectObject(hDC, GetStockObject(NULL_BRUSH));
	Rectangle(hDC, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);

	ReleaseDC(hWnd, hDC);
}

LRESULT CALLBACK WndProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static BOOL		fBlocking, fValidBox;
	static POINT	ptBeg, ptEnd, ptBoxBeg, ptBoxEnd;
	HDC				hDC;
	PAINTSTRUCT		ps;

	switch (msg)
	{
	case WM_LBUTTONDOWN:
		ptBeg.x = ptEnd.x = LOWORD(lParam);
		ptBeg.y = ptEnd.y = HIWORD(lParam);

		DrawBoxOutline(hWnd, ptBeg, ptEnd);

		SetCapture(hWnd);
		SetCursor(LoadCursor(NULL, IDC_CROSS));

		fBlocking = TRUE;
		return 0;

	case WM_MOUSEMOVE:
		if (fBlocking)
		{
			SetCursor(LoadCursor(NULL, IDC_CROSS));

			DrawBoxOutline(hWnd, ptBeg, ptEnd);

			ptEnd.x = LOWORD(lParam);
			ptEnd.y = HIWORD(lParam);

			DrawBoxOutline(hWnd, ptBeg, ptEnd);
		}
		return 0;

	case WM_LBUTTONUP:
		if (fBlocking)
		{
			DrawBoxOutline(hWnd, ptBeg, ptEnd);

			ptBoxBeg = ptBeg;
			ptBoxEnd.x = LOWORD(lParam);
			ptBoxEnd.y = HIWORD(lParam);

			ReleaseCapture();
			SetCursor(LoadCursor(NULL, IDC_ARROW));

			fBlocking = FALSE;
			fValidBox = TRUE;

			InvalidateRect(hWnd, NULL, TRUE);
		}
		return 0;

	case WM_CHAR:
		if (fBlocking & wParam == '\x1B')	// ESC character
		{
			DrawBoxOutline(hWnd, ptBeg, ptEnd);

			ReleaseCapture();
			SetCursor(LoadCursor(NULL, IDC_ARROW));

			fBlocking = FALSE;
		}

		return 0;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		if (fValidBox)
		{
			SelectObject(hDC, GetStockObject(BLACK_BRUSH));
			Rectangle(hDC, ptBoxBeg.x, ptBoxBeg.y, ptBoxEnd.x, ptBoxEnd.y);
		}

		if (fBlocking)
		{
			SetROP2(hDC, R2_NOT);
			SelectObject(hDC, GetStockObject(NULL_BRUSH));
			Rectangle(hDC, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
		}

		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}