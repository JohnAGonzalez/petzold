#include <Windows.h>

#define ID_TIMER	1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
VOID	CALLBACK TimerProc(HWND, UINT, UINT, DWORD);

int WINAPI WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	PSTR		szCmdLine,
	int			iCmdShow)
{
	static TCHAR	szAppName[] = TEXT("Beeper2");
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
		szAppName, TEXT("Beeper2 Timer Demo"), WS_OVERLAPPEDWINDOW,
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
	switch (msg)
	{
	case WM_CREATE:
		SetTimer(hWnd, ID_TIMER, 1000, TimerProc);
		return 0;

	case WM_DESTROY:
		KillTimer(hWnd, ID_TIMER);
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

VOID CALLBACK TimerProc(
	HWND	hWnd,
	UINT	msg,
	UINT	iTimerID,
	DWORD	dwTime)
{
	static BOOL fFlipFlop = false;
	HBRUSH		hBrush;
	HDC			hDC;
	RECT		rect;

	MessageBeep(-1);
	fFlipFlop = !fFlipFlop;

	GetClientRect(hWnd, &rect);

	hDC = GetDC(hWnd);
	hBrush = CreateSolidBrush(fFlipFlop ? RGB(255, 0, 0) : RGB(0, 0, 255));

	FillRect(hDC, &rect, hBrush);
	ReleaseDC(hWnd, hDC);
	DeleteObject(hBrush);
}