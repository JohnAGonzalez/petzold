#include <Windows.h>

#define MAXPOINTS 1000

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szCmdLine,
	int iCmdShow)
{
	static TCHAR	szAppName[] = TEXT("Connect");
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

	hWnd = CreateWindow(szAppName, TEXT("Connect-the-Points Mouse Demo"), WS_OVERLAPPEDWINDOW,
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
	static POINT	pt[MAXPOINTS];
	static int		iCount;
	HDC				hDC;
	int				i, j;
	PAINTSTRUCT		ps;

	switch (msg)
	{
	case WM_LBUTTONDOWN:
		iCount = 0;

	case WM_LBUTTONUP:
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;

	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON && iCount < MAXPOINTS)
		{
			pt[iCount].x = LOWORD(lParam);
			pt[iCount].y = HIWORD(lParam);
			++iCount;

			hDC = GetDC(hWnd);
			SetPixel(hDC, LOWORD(lParam), HIWORD(lParam), 0);
			ReleaseDC(hWnd, hDC);
		}
		return 0;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		SetCursor(LoadCursor(NULL, IDC_WAIT));
		ShowCursor(TRUE);

		for (i = 0; i < iCount - 1; ++i)
			for (j = i + 1; j < iCount; ++j)
			{
				MoveToEx(hDC, pt[i].x, pt[i].y, NULL);
				LineTo(hDC, pt[j].x, pt[j].y);
			}

		ShowCursor(FALSE);
		SetCursor(LoadCursor(NULL, IDC_ARROW));

		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}