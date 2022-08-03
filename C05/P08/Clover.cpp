#include <Windows.h>
#include <math.h>

#define PI		3.14159265
#define TWOPI	(2 * PI)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szCmdLine,
	int iCmdShow)
{
	static TCHAR	szAppName[] = TEXT("Clover");
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
		szAppName, TEXT("Draw a Clover"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.lParam;
}

LRESULT CALLBACK WndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	static HRGN	hRgnClip;
	static int	cxClient, cyClient;
	double		fAngle, fRadius;
	HCURSOR		hCursor;
	HDC			hDC;
	HRGN		hRgnTemp[6];
	int			i;
	PAINTSTRUCT	ps;

	switch (message)
	{
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
		ShowCursor(TRUE);

		if (hRgnClip)
			DeleteObject(hRgnClip);

		hRgnTemp[0] = CreateEllipticRgn(0,				cyClient / 3,	cxClient / 2,		2 * cyClient / 3);
		hRgnTemp[1] = CreateEllipticRgn(cxClient / 2,	cyClient / 3,	cxClient,			2 * cyClient / 3);
		hRgnTemp[2] = CreateEllipticRgn(cxClient / 3,	0,				2 * cxClient / 3,	cyClient / 2);
		hRgnTemp[3] = CreateEllipticRgn(cxClient / 3,	cyClient / 2,	2 * cxClient / 3,	cyClient);

		hRgnTemp[4] = CreateRectRgn(0, 0, 1, 1);
		hRgnTemp[5] = CreateRectRgn(0, 0, 1, 1);
		hRgnClip	= CreateRectRgn(0, 0, 1, 1);

		CombineRgn(hRgnTemp[4], hRgnTemp[0], hRgnTemp[1], RGN_OR);
		CombineRgn(hRgnTemp[5], hRgnTemp[2], hRgnTemp[3], RGN_OR);
		CombineRgn(hRgnClip,	hRgnTemp[4], hRgnTemp[5], RGN_XOR);

		for (i = 0; i < (sizeof(hRgnTemp) / sizeof(hRgnTemp[0])); ++i)
			DeleteObject(hRgnTemp[i]);

		SetCursor(hCursor);
		ShowCursor(FALSE);
		return 0;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		SetViewportOrgEx(hDC, cxClient / 2, cyClient / 2, NULL);
		SelectClipRgn(hDC, hRgnClip);

		fRadius = _hypot(cxClient / 2.0, cyClient / 2.0);

		for (fAngle = 0.0; fAngle < TWOPI; fAngle += (TWOPI / 360))
		{
			MoveToEx(hDC, 0, 0, NULL);
			LineTo(hDC,
				(int)( fRadius * cos(fAngle) + 0.5),
				(int)(-fRadius * sin(fAngle) + 0.5));
		}

		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		DeleteObject(hRgnClip);
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}