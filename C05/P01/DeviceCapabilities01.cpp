#include <Windows.h>

#define NUMLINES ((int)(sizeof devcaps / sizeof devcaps[0]))

struct
{
	int		iIndex;
	PCNZCH	szLabel;
	PCNZCH	szDesc;
}
devcaps[] =
{
	HORZSIZE,		TEXT("HORZSIZE"),		TEXT("Width in millimeters:"),
	VERTSIZE,		TEXT("VERTSIZE"),		TEXT("Height in millimeters:"),
	HORZRES,		TEXT("HORZRES"),		TEXT("Width in pixels:"),
	VERTRES,		TEXT("VERTRES"),		TEXT("Height in pixels:"),
	BITSPIXEL,		TEXT("BITSPIXEL"),		TEXT("Color bits per pixel:"),
	PLANES,			TEXT("PLANES"),			TEXT("Number of color planes"),
	NUMBRUSHES,		TEXT("NUMBRUSHES"),		TEXT("Number of device brushes"),
	NUMPENS,		TEXT("NUMPENS"),		TEXT("Number of device pens:"),
	NUMMARKERS,		TEXT("NUMMARKERS"),		TEXT("Number of device markers:"),
	NUMFONTS,		TEXT("NUMFONTS"),		TEXT("Number of device fonts:"),
	NUMCOLORS,		TEXT("NUMCOLORS"),		TEXT("Number of device colors:"),
	PDEVICESIZE,	TEXT("PDEVICESIZE"),	TEXT("Size of device structure:"),
	ASPECTX,		TEXT("ASPECTX"),		TEXT("Relative width of pixel:"),
	ASPECTY,		TEXT("ASPECTY"),		TEXT("Relative height of pixel:"),
	ASPECTXY,		TEXT("ASPECTXY"),		TEXT("Relative diagonal of pixel:"),
	LOGPIXELSX,		TEXT("LOGPIXELSX"),		TEXT("Horizontal dots per inch:"),
	LOGPIXELSY,		TEXT("LOGPIXELSY"),		TEXT("Vertical dots per inch:"),
	SIZEPALETTE,	TEXT("SIZEPALETTE"),	TEXT("Number of palette entries:"),
	NUMRESERVED,	TEXT("NUMRESERVED"),	TEXT("Reserved palette entries:"),
	COLORRES,		TEXT("COLORRES"),		TEXT("Actual color resolution:")
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szCmdLine,
	int iCmdShow)
{
	static TCHAR	szAppName[] = TEXT("DeviceCapabilities1");
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
		MessageBox(NULL, TEXT("This program requires WindowsNT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName, TEXT("Device Capabilities"), WS_OVERLAPPEDWINDOW,
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int	cxChar = 0, cxCaps = 0, cyChar = 0;
	TCHAR		szBuffer[10] = { 0 };
	HDC			hDC = 0;
	int			i = 0;
	PAINTSTRUCT	ps = { 0 };
	TEXTMETRIC	tm = { 0 };

	switch (message)
	{
	case WM_CREATE:
		hDC = GetDC(hWnd);

		GetTextMetrics(hDC, &tm);
		cxChar = tm.tmAveCharWidth;
		cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2)*cxChar / 2;
		cyChar = tm.tmHeight + tm.tmExternalLeading;

		ReleaseDC(hWnd, hDC);
		return 0;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		for (i = 0; i < NUMLINES; ++i)
		{
			TextOut(hDC, 0, cyChar * i, devcaps[i].szLabel, lstrlen(devcaps[i].szLabel));
			TextOut(hDC, 14 * cxCaps, cyChar * i, devcaps[i].szDesc, lstrlen(devcaps[i].szDesc));
			SetTextAlign(hDC, TA_RIGHT | TA_TOP);
			TextOut(hDC, 14 * cxCaps + 35 * cxChar, cyChar * i, szBuffer,
				wsprintf(szBuffer, TEXT("%5d"), GetDeviceCaps(hDC, devcaps[i].iIndex)));
			SetTextAlign(hDC, TA_LEFT | TA_TOP);
		}

		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
