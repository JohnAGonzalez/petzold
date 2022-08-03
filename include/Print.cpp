#include <Windows.h>

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL				PrintMyPage(HWND);

extern HINSTANCE	hInst;
extern TCHAR		szAppName[];
extern TCHAR		szCaption[];

int WINAPI WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	PSTR		szCmdLine,
	int			iCmdShow)
{
	HWND		hWnd{ 0 };
	MSG			msg{ 0 };
	WNDCLASS	wndClass{ 0 };

	wndClass.style = CS_VREDRAW | CS_HREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hInstance = hInst = hInstance;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName, szCaption, WS_OVERLAPPEDWINDOW,
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

void PageGDICalls(
	HDC		hdcPrn,
	int		cxPage,
	int		cyPage)
{
	static TCHAR	szTextStr[] = TEXT("Hello, Printer!");

	Rectangle(hdcPrn, 0, 0, cxPage, cyPage);

	MoveToEx(hdcPrn, 0, 0, NULL);
	LineTo(hdcPrn, cxPage, cyPage);
	MoveToEx(hdcPrn, cxPage, 0, NULL);
	LineTo(hdcPrn, 0, cyPage);

	SaveDC(hdcPrn);

	SetMapMode(hdcPrn, MM_ISOTROPIC);
	SetWindowExtEx(hdcPrn, 1000, 1000, NULL);
	SetViewportExtEx(hdcPrn, cxPage / 2, -cyPage / 2, NULL);
	SetViewportOrgEx(hdcPrn, cxPage / 2, cyPage / 2, NULL);

	Ellipse(hdcPrn, -500, 500, 500, -500);

	SetTextAlign(hdcPrn, TA_BASELINE | TA_CENTER);
	TextOut(hdcPrn, 0, 0, szTextStr, lstrlen(szTextStr));
	RestoreDC(hdcPrn, -1);
}

LRESULT CALLBACK WndProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static int	cxClient{ 0 }, cyClient{ 0 };
	HDC			hDC{ 0 };
	HMENU		hMenu{ 0 };
	PAINTSTRUCT	ps{ 0 };

	switch (msg)
	{
	case WM_CREATE:
		hMenu = GetSystemMenu(hWnd, FALSE);
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
		AppendMenu(hMenu, 0, 1, TEXT("&Print"));
		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;

	case WM_SYSCOMMAND:
		if (wParam == 1)
		{
			if (!PrintMyPage(hWnd))
				MessageBox(hWnd, TEXT("Could not print page!"), szAppName, MB_OK | MB_ICONEXCLAMATION);

			return 0;
		}
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		PageGDICalls(hDC, cxClient, cyClient);

		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}