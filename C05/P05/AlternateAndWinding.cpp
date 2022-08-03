#include <Windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szCmdLine,
	int iCmdShow)
{
	static TCHAR	szAppName[] = TEXT("AltWind");
	HWND			hWnd;
	MSG				msg;
	WNDCLASS		wndClass;

	wndClass.style = CS_VREDRAW | CS_HREDRAW;
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
		MessageBox(NULL, TEXT("This application requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName, TEXT("Alternate and Winding Fill Modes"), WS_OVERLAPPEDWINDOW,
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static POINT	aptFigure[] = {
		10, 70,
		50, 70,
		50, 10,
		90, 10,
		90, 50,
		30, 50,
		30, 90,
		70, 90,
		70, 30,
		10, 30 };

	static int	cxClient, cyClient;
	HDC			hDC;
	int			i;
	PAINTSTRUCT	ps;
	POINT		apt[sizeof(aptFigure) / sizeof(aptFigure[0])];

	switch (msg)
	{
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		SelectObject(hDC, GetStockObject(GRAY_BRUSH));

		for (i = 0; i < (sizeof(aptFigure) / sizeof(aptFigure[0])); ++i)
		{
			apt[i].x = cxClient * aptFigure[i].x / 200;
			apt[i].y = cyClient * aptFigure[i].y / 100;
		}

		SetPolyFillMode(hDC, ALTERNATE);
		Polygon(hDC, apt, sizeof(apt) / sizeof(apt[0]));

		for (i = 0; i < (sizeof(aptFigure) / sizeof(aptFigure[0])); ++i)
		{
			apt[i].x += cxClient / 2;
		}

		SetPolyFillMode(hDC, WINDING);
		Polygon(hDC, apt, sizeof(apt) / sizeof(apt[0]));

		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}