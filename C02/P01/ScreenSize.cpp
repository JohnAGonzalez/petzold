#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

int CDECL MessageBoxPrintf(
	TCHAR * szCaption,
	TCHAR * szFormat,
	...)
{
	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szFormat);
	_vsntprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
	va_end(pArgList);

	return MessageBox(NULL, szBuffer, szCaption, 0);
}

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szCmdLine,
	int iCmdShow)
{
	int cxScreen = GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);

	TCHAR szCaption[50];
	TCHAR szFormat[50];
	sprintf_s(szCaption, TEXT("%hs"), TEXT("ScreenSize"));
	sprintf_s(szFormat, TEXT("%hs"), TEXT("The screen is %i pixels wide by %i pixels high."));

	MessageBoxPrintf(szCaption, szFormat, cxScreen, cyScreen);

	return 0;
}