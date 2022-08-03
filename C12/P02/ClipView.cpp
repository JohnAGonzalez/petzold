#include <Windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = TEXT("ClipView");

int WINAPI WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	PSTR		szCmdLine,
	int			iCmdShow)
{
	HWND		hWnd;
	MSG			msg;
	WNDCLASS	wndClass;

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
		szAppName, TEXT("Simple Clipboard Viewer (Text Only)"), WS_OVERLAPPEDWINDOW,
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
	static HWND	hWndNextViewer;
	HGLOBAL		hGlobal;
	HDC			hDC;
	PTSTR		pGlobal;
	PAINTSTRUCT	ps;
	RECT		rect;

	switch (msg)
	{
	case WM_CREATE:
		hWndNextViewer = SetClipboardViewer(hWnd);
		return 0;

	case WM_CHANGECBCHAIN:
		if ((HWND)wParam == hWndNextViewer)
			hWndNextViewer = (HWND)lParam;

		else if (hWndNextViewer)
			SendMessage(hWndNextViewer, msg, wParam, lParam);

		return 0;

	case WM_DRAWCLIPBOARD:
		if (hWndNextViewer)
			SendMessage(hWndNextViewer, msg, wParam, lParam);

		InvalidateRect(hWnd, NULL, TRUE);
		return 0;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);
		OpenClipboard(hWnd);
#ifdef UNICODE
		hGlobal = GetClipboardData(CF_UNICODETEXT);
#else
		hGlobal = GetClipboardData(CF_TEXT);
#endif

		if (hGlobal != NULL)
		{
			pGlobal = (PTSTR)GlobalLock(hGlobal);
			DrawText(hDC, pGlobal, -1, &rect, DT_EXPANDTABS);
			GlobalUnlock(hGlobal);
		}
		
		CloseClipboard();
		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		ChangeClipboardChain(hWnd, hWndNextViewer);
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}