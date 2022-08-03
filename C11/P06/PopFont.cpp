#include <Windows.h>
#include <commdlg.h>

static LOGFONT	logFont;
static HFONT	hFont;

void PopFontInitialize(HWND hWnd)
{
	GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), (PTSTR)&logFont);
	hFont = CreateFontIndirect(&logFont);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, 0);
}

BOOL PopFontChooseFont(HWND hWnd)
{
	CHOOSEFONT cf;

	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = hWnd;
	cf.hDC = NULL;
	cf.lpLogFont = &logFont;
	cf.iPointSize = 0;
	cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS;
	cf.rgbColors = 0;
	cf.lCustData = 0;
	cf.lpfnHook = NULL;
	cf.lpTemplateName = NULL;
	cf.hInstance = NULL;
	cf.lpszStyle = NULL;
	cf.nFontType = 0;			// Returned from ChooseFont
	cf.nSizeMin = 0;
	cf.nSizeMax = 0;

	return ChooseFont(&cf);
}

void PopFontSetFont(HWND hWnd)
{
	HFONT	hFontNew;
	RECT	rect;

	hFontNew = CreateFontIndirect(&logFont);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)hFontNew, 0);
	DeleteObject(hFont);
	hFont = hFontNew;
	GetClientRect(hWnd, &rect);
	InvalidateRect(hWnd, &rect, TRUE);
}

void PopFontDeinitialize(void)
{
	DeleteObject(hFont);
}
