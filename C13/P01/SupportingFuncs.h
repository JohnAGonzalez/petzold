#pragma once
#include <Windows.h>
#include <VersionHelpers.h>

#define IDM_DEVMODE		1000

typedef struct
{
	int		iMask;
	PCSTR	szDesc;
} BITS;

HDC GetPrinterDC(void);
void DoBasicInfo(HDC, HDC, int, int);
void DoOtherInfo(HDC, HDC, int, int);
void DoBitCodedCaps(HDC, HDC, int, int, int);