#include <Windows.h>

#define ID_SMALLER	1
#define ID_LARGER	2
#define BTN_WIDTH	(8 * cxChar)
#define BTN_HEIGHT	(4 * cyChar)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInst;

int WINAPI WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	PSTR		szCmdLine,
	int			iCmdShow)
{
	static TCHAR	szAppName[] = TEXT("OwnerDraw");
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
		szAppName, TEXT("Owner-Draw Button Demo"), WS_OVERLAPPEDWINDOW,
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

void Triangle(HDC hDC, POINT pt[])
{
	SelectObject(hDC, GetStockObject(BLACK_BRUSH));
	Polygon(hDC, pt, 3);
	SelectObject(hDC, GetStockObject(WHITE_BRUSH));
}

LRESULT CALLBACK WndProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static HWND			hWndSmaller, hWndLarger;
	static int			cxClient, cyClient, cxChar, cyChar;
	int					cx, cy;
	LPDRAWITEMSTRUCT	pDis;
	POINT				pt[3];
	RECT				rc;

	switch (msg)
	{
	case WM_CREATE:
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());

		// create the owner-draw pushbuttons
		hWndSmaller = CreateWindow(
			TEXT("button"), TEXT(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			0, 0, BTN_WIDTH, BTN_HEIGHT,
			hWnd, (HMENU)ID_SMALLER, hInst, NULL);

		hWndLarger = CreateWindow(
			TEXT("button"), TEXT(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			0, 0, BTN_WIDTH, BTN_HEIGHT,
			hWnd, (HMENU)ID_LARGER, hInst, NULL);

		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		// move the buttons to the new center
		MoveWindow(hWndSmaller,
			cxClient / 2 - 3 * BTN_WIDTH / 2,
			cyClient / 2 - BTN_HEIGHT / 2,
			BTN_WIDTH, BTN_HEIGHT, TRUE);

		MoveWindow(hWndLarger,
			cxClient / 2 + BTN_WIDTH / 2,
			cyClient / 2 - BTN_HEIGHT / 2,
			BTN_WIDTH, BTN_HEIGHT, TRUE);

		return 0;

	case WM_COMMAND:
		GetWindowRect(hWnd, &rc);

		switch (wParam)
		{
		case ID_SMALLER:
			rc.left += cxClient / 20;
			rc.right -= cxClient / 20;
			rc.top += cyClient / 20;
			rc.bottom -= cyClient / 20;
			break;

		case ID_LARGER:
			rc.left -= cxClient / 20;
			rc.right += cxClient / 20;
			rc.top -= cyClient / 20;
			rc.bottom += cyClient / 20;
			break;
		}

		MoveWindow(hWnd,
			rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			TRUE);

		return 0;

	case WM_DRAWITEM:
		pDis = (LPDRAWITEMSTRUCT)lParam;

		// fill area with white and frame it black
		FillRect(pDis->hDC, &pDis->rcItem, (HBRUSH)GetStockObject(WHITE_BRUSH));
		FrameRect(pDis->hDC, &pDis->rcItem, (HBRUSH)GetStockObject(BLACK_BRUSH));

		// draw inward and outward black triangles
		cx = pDis->rcItem.right - pDis->rcItem.left;
		cy = pDis->rcItem.bottom - pDis->rcItem.top;

		switch (pDis->CtlID)
		{
		case ID_SMALLER:
			pt[0].x = 3 * cx / 8;	pt[0].y = 1 * cy / 8;
			pt[1].x = 5 * cx / 8;	pt[1].y = 1 * cy / 8;
			pt[2].x = 4 * cx / 8;	pt[2].y = 3 * cy / 8;
			Triangle(pDis->hDC, pt);

			pt[0].x = 7 * cx / 8;	pt[0].y = 3 * cy / 8;
			pt[1].x = 7 * cx / 8;	pt[1].y = 5 * cy / 8;
			pt[2].x = 5 * cx / 8;	pt[2].y = 4 * cy / 8;
			Triangle(pDis->hDC, pt);

			pt[0].x = 5 * cx / 8;	pt[0].y = 7 * cy / 8;
			pt[1].x = 3 * cx / 8;	pt[1].y = 7 * cy / 8;
			pt[2].x = 4 * cx / 8;	pt[2].y = 5 * cy / 8;
			Triangle(pDis->hDC, pt);

			pt[0].x = 1 * cx / 8;	pt[0].y = 5 * cy / 8;
			pt[1].x = 1 * cx / 8;	pt[1].y = 3 * cy / 8;
			pt[2].x = 3 * cx / 8;	pt[2].y = 4 * cy / 8;
			Triangle(pDis->hDC, pt);
			break;

		case ID_LARGER:
			pt[0].x = 5 * cx / 8;	pt[0].y = 3 * cy / 8;
			pt[1].x = 3 * cx / 8;	pt[1].y = 3 * cy / 8;
			pt[2].x = 4 * cx / 8;	pt[2].y = 1 * cy / 8;
			Triangle(pDis->hDC, pt);

			pt[0].x = 5 * cx / 8;	pt[0].y = 5 * cy / 8;
			pt[1].x = 5 * cx / 8;	pt[1].y = 3 * cy / 8;
			pt[2].x = 7 * cx / 8;	pt[2].y = 4 * cy / 8;
			Triangle(pDis->hDC, pt);

			pt[0].x = 3 * cx / 8;	pt[0].y = 5 * cy / 8;
			pt[1].x = 5 * cx / 8;	pt[1].y = 5 * cy / 8;
			pt[2].x = 4 * cx / 8;	pt[2].y = 7 * cy / 8;
			Triangle(pDis->hDC, pt);

			pt[0].x = 3 * cx / 8;	pt[0].y = 3 * cy / 8;
			pt[1].x = 3 * cx / 8;	pt[1].y = 5 * cy / 8;
			pt[2].x = 1 * cx / 8;	pt[2].y = 4 * cy / 8;
			Triangle(pDis->hDC, pt);
			break;

		}

		// invert the rectangle if the button is selected
		if (pDis->itemState & ODS_SELECTED)
			InvertRect(pDis->hDC, &pDis->rcItem);

		// draw a focus rectangle if the button has the focus
		if (pDis->itemState & ODS_FOCUS)
		{
			pDis->rcItem.left += cx / 16;
			pDis->rcItem.top += cy / 16;
			pDis->rcItem.bottom -= cx / 16;
			pDis->rcItem.right -= cy / 16;
			DrawFocusRect(pDis->hDC, &pDis->rcItem);
		}

		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}