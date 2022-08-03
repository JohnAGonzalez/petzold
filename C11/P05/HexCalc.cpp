#include <Windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = TEXT("HexCalc");

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
	wndClass.cbWndExtra = DLGWINDOWEXTRA;
	wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(HEXCALC));
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndClass.hInstance = hInstance;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(HEXCALC), 0, NULL);

	ShowWindow(hWnd, iCmdShow);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void ShowNumber(
	HWND	hWnd,
	UINT	iNumber)
{
	TCHAR	szBuffer[20];

	wsprintf(szBuffer, TEXT("%X"), iNumber);
	SetDlgItemText(hWnd, VK_ESCAPE, szBuffer);
}

DWORD CalcIt(
	UINT	iFirstNum,
	int		iOperation,
	UINT	iNum)
{
	switch (iOperation)
	{
	case '=':	return iNum;
	case '+':	return iFirstNum + iNum;
	case '-':	return iFirstNum - iNum;
	case '*':	return iFirstNum * iNum;
	case '&':	return iFirstNum & iNum;
	case '|':	return iFirstNum | iNum;
	case '^':	return iFirstNum ^ iNum;
	case '<':	return iFirstNum << iNum;
	case '>':	return iFirstNum >> iNum;
	case '/':	return iNum ? iFirstNum / iNum : MAXDWORD;
	case '%':	return iNum ? iFirstNum % iNum : MAXDWORD;
	default:	return 0;
	}
}

LRESULT CALLBACK WndProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static BOOL		bNewNumber = TRUE;
	static int		iOperation = '=';
	static UINT		iNumber, iFirstNumber;
	HWND			hButton;

	switch (msg)
	{
	case WM_KEYDOWN:			// left arrow --> backspace
		if (wParam != VK_LEFT)
			break;

		wParam = VK_BACK;		// fall through

	case WM_CHAR:
		if ((wParam = (WPARAM)CharUpper((TCHAR *)wParam)) == VK_RETURN)
			wParam = '=';

		if (hButton = GetDlgItem(hWnd, wParam))
		{
			SendMessage(hButton, BM_SETSTATE, 1, 0);
			Sleep(100);
			SendMessage(hButton, BM_SETSTATE, 0, 0);
		}
		else
		{
			MessageBeep(0);
			break;
		}

		// fall through

	case WM_COMMAND:
		SetFocus(hWnd);

		if (LOWORD(wParam) == VK_BACK)			// backspace
			ShowNumber(hWnd, iNumber /= 16);

		else if (LOWORD(wParam) == VK_ESCAPE)	// escape
			ShowNumber(hWnd, iNumber = 0);

		else if (isxdigit(LOWORD(wParam)))		// hex digit
		{
			if (bNewNumber)
			{
				iFirstNumber = iNumber;
				iNumber = 0;
			}
			bNewNumber = false;

			if (iNumber <= MAXDWORD >> 4)
				ShowNumber(hWnd, iNumber = 16 * iNumber + wParam - (isdigit(wParam) ? '0' : 'A' - 10));
			else
				MessageBeep(0);
		}
		else									// operation
		{
			if (!bNewNumber)
				ShowNumber(hWnd, iNumber = CalcIt(iFirstNumber, iOperation, iNumber));

			bNewNumber = TRUE;
			iOperation = LOWORD(wParam);
		}

		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}