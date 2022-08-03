#include <Windows.h>
#include "..\\..\\include\\EDRLib.h"

int WINAPI DllMain(
	HINSTANCE	hInstance,
	DWORD		fdwReason,
	PVOID		pvReserved)
{
	return TRUE;
}

EXPORT BOOL CALLBACK EdrCenterTextA(
	HDC		hDC,
	PRECT	pRC,
	PCSTR	pString)
{
	int		iLength;
	SIZE	size;

	iLength = lstrlenA(pString);
	GetTextExtentPoint32A(hDC, pString, iLength, &size);

	return TextOutA(
		hDC,
		(pRC->right - pRC->left - size.cx) / 2,
		(pRC->bottom - pRC->top - size.cy) / 2,
		pString,
		iLength);
}

EXPORT BOOL CALLBACK EdrCenterTextW(
	HDC		hDC,
	PRECT	pRC,
	PCWSTR	pString)
{
	int		iLength;
	SIZE	size;

	iLength = lstrlenW(pString);
	GetTextExtentPoint32W(hDC, pString, iLength, &size);

	return TextOutW(
		hDC,
		(pRC->right - pRC->left - size.cx) / 2,
		(pRC->bottom - pRC->top - size.cy) / 2,
		pString,
		iLength);
}
