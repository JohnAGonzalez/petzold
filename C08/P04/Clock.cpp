#include <Windows.h>
#include <math.h>

#define ID_TIMER	1
#define TWO_PI		(2 * 3.14159265)


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	PSTR		szCmdLine,
	int			iCmdShow)
{
	static TCHAR	szAppName[] = TEXT("Clock");
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
		szAppName, TEXT("Analog Clock"), WS_OVERLAPPEDWINDOW,
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

void SetIsotropic(
	HDC	hDC,
	int	cxClient,
	int	cyClient)
{
	SetMapMode(hDC, MM_ISOTROPIC);
	SetWindowExtEx(hDC, 1000, 1000, NULL);
	SetViewportExtEx(hDC, cxClient / 2, -cyClient / 2, NULL);
	SetViewportOrgEx(hDC, cxClient / 2, cyClient / 2, NULL);
}

void RotatePoint(
	POINT	pt[],
	int		number,
	int		angle)
{
	POINT	ptTemp;

	for (int i = 0; i < number; ++i)
	{
		ptTemp.x = (int)(pt[i].x * cos(TWO_PI * angle / 360) + pt[i].y * sin(TWO_PI * angle / 360));
		ptTemp.y = (int)(pt[i].y * cos(TWO_PI * angle / 360) - pt[i].x * sin(TWO_PI * angle / 360));
		pt[i] = ptTemp;
	}
}

void DrawClock(HDC hDC)
{
	POINT	pt[3];

	for (int angle = 0; angle < 360; angle += 6)
	{
		pt[0].x = 0;
		pt[0].y = 900;

		RotatePoint(pt, 1, angle);

		pt[2].x = pt[2].y = angle % 5 ? 33 : 100;

		pt[0].x -= pt[2].x / 2;
		pt[0].y -= pt[2].y / 2;

		pt[1].x = pt[0].x + pt[2].x;
		pt[1].y = pt[0].y + pt[2].y;

		SelectObject(hDC, GetStockObject(BLACK_BRUSH));

		Ellipse(hDC, pt[0].x, pt[0].y, pt[1].x, pt[1].y);
	}
}

void DrawHands(
	HDC				hDC,
	SYSTEMTIME *	pST,
	BOOL			fChange)
{
	static POINT pt[3][5] = {
		0, -150, 100, 0, 0, 600, -100, 0, 0, -150,
		0, -200,  50, 0, 0, 800,  -50, 0, 0, -200,
		0,    0,   0, 0, 0,   0,    0, 0, 0,  800 };
	int			angle[3];
	POINT		ptTemp[3][5];

	angle[0] = (pST->wHour * 30) % 360 + pST->wMinute / 2;
	angle[1] = pST->wMinute * 6;
	angle[2] = pST->wSecond * 6;

	memcpy(ptTemp, pt, sizeof(pt));

	for (int i = fChange ? 0 : 2; i < 3; ++i)
	{
		RotatePoint(ptTemp[i], 5, angle[i]);
		Polyline(hDC, ptTemp[i], 5);
	}
}

LRESULT CALLBACK WndProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static int			cxClient, cyClient;
	static SYSTEMTIME	stPrevious;
	BOOL				fChange;
	HDC					hDC;
	PAINTSTRUCT			ps;
	SYSTEMTIME			st;

	switch (msg)
	{
	case WM_CREATE:
		SetTimer(hWnd, ID_TIMER, 1000, NULL);
		GetLocalTime(&st);
		stPrevious = st;
		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;

	case WM_TIMER:
		GetLocalTime(&st);

		fChange =
			st.wHour != stPrevious.wHour ||
			st.wMinute != stPrevious.wMinute;

		hDC = GetDC(hWnd);

		SetIsotropic(hDC, cxClient, cyClient);

		SelectObject(hDC, GetStockObject(WHITE_PEN));
		DrawHands(hDC, &stPrevious, fChange);

		SelectObject(hDC, GetStockObject(BLACK_PEN));
		DrawHands(hDC, &st, TRUE);

		ReleaseDC(hWnd, hDC);

		stPrevious = st;
		return 0;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		SetIsotropic(hDC, cxClient, cyClient);

		DrawClock(hDC);
		DrawHands(hDC, &stPrevious, TRUE);

		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		KillTimer(hWnd, ID_TIMER);
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
