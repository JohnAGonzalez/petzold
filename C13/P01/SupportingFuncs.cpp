#include <Windows.h>
#include "SupportingFuncs.h"

HDC GetPrinterDC(void)
{
	DWORD				dwNeeded, dwReturned;
	HDC					hDC = NULL;
	PRINTER_INFO_4 *	pInfo4;
	PRINTER_INFO_5 *	pInfo5;

	if (!IsWindowsXPOrGreater())			// Windows 98
	{
		EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 5, NULL, 0, &dwNeeded, &dwReturned);
		pInfo5 = (PRINTER_INFO_5 *)malloc(dwNeeded);
		EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 5, (PBYTE)pInfo5, dwNeeded, &dwNeeded, &dwReturned);
		hDC = CreateDC(NULL, pInfo5->pPrinterName, NULL, NULL);
		free(pInfo5);
		pInfo5 = NULL;
	}
	else									// Windows NT
	{
		EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 4, NULL, 0, &dwNeeded, &dwReturned);
		pInfo4 = (PRINTER_INFO_4 *)malloc(dwNeeded);
		EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 4, (PBYTE)pInfo4, dwNeeded, &dwNeeded, &dwReturned);
		hDC = CreateDC(NULL, pInfo4->pPrinterName, NULL, NULL);
		free(pInfo4);
		pInfo4 = NULL;
	}

	return hDC;
}

void DoBasicInfo(HDC hDC, HDC hdcInfo, int cxChar, int cyChar)
{
	static struct
	{
		int		nIndex;
		PCSTR	szDesc;
	}
	info[]
	{
		HORZSIZE,			TEXT("HORZSIZE         Width in millimeters                   :"),
		VERTSIZE,			TEXT("VERTSIZE         Height in millimeters                  :"),
		HORZRES,			TEXT("HORZRES          Width in pixels                        :"),
		VERTRES,			TEXT("VERTRES          Height in pixels                       :"),
		BITSPIXEL,			TEXT("BITSPIXEL        Color bits per pixel                   :"),
		PLANES,				TEXT("PLANES           Number of color planes                 :"),
		NUMBRUSHES,			TEXT("NUMBRUSHES       Number of device brushes               :"),
		NUMPENS,			TEXT("NUMPENS          Number of device pens                  :"),
		NUMMARKERS,			TEXT("NUMMARKERS       Number of device markers               :"),
		NUMFONTS,			TEXT("NUMFONTS         Number of device fonts                 :"),
		NUMCOLORS,			TEXT("NUMCOLORS        Number of device colors                :"),
		PDEVICESIZE,		TEXT("PDEVICESIZE      Size of device structure               :"),
		ASPECTX,			TEXT("ASPECTX          Relative width of pixel                :"),
		ASPECTY,			TEXT("ASPECTY          Relative height of pixel               :"),
		ASPECTXY,			TEXT("ASPECTXY         Relative diagonal of pixel             :"),
		LOGPIXELSX,			TEXT("LOGPIXELSX       Horizontal dots per inch               :"),
		LOGPIXELSY,			TEXT("LOGPIXELSY       Vertical dots per inch                 :"),
		SIZEPALETTE,		TEXT("SIZEPALETTE      Number of palette entries              :"),
		NUMRESERVED,		TEXT("NUMRESERVED      Reserved palette entries               :"),
		COLORRES,			TEXT("COLORRES         Actual color resolution                :"),
		PHYSICALWIDTH,		TEXT("PHYSICALWIDTH    Printer page pixel width               :"),
		PHYSICALHEIGHT,		TEXT("PHYSICALHEIGHT   Printer page pixel height              :"),
		PHYSICALOFFSETX,	TEXT("PHYSICALOFFSETX  Printer page x offset                  :"),
		PHYSICALOFFSETY,	TEXT("PHYSICALOFFSETY  Printer page y offset                  :")
	};

	TCHAR szBuffer[80] = { 0 };
	for (int i = 0; i < sizeof(info) / sizeof(info[0]); ++i)
		TextOut(hDC, cxChar, (i + 1) * cyChar, szBuffer,
			wsprintf(szBuffer, TEXT("%-45s %8d"), info[i].szDesc,
				GetDeviceCaps(hdcInfo, info[i].nIndex)));
}

void DoOtherInfo(HDC hDC, HDC hdcInfo, int cxChar, int cyChar)
{
	static BITS clip[] =
	{
		CP_RECTANGLE,		TEXT("CP_RECTANGLE     Can Clip To Rectangle     :")
	};

	static BITS raster[] =
	{
		RC_BITBLT,			TEXT("RC_BITBLT        Capable of simple BitBlt               :"),
		RC_BANDING,			TEXT("RC_BANDING       Requires banding support               :"),
		RC_SCALING,			TEXT("RC_SCALING       Requires scaling support               :"),
		RC_BITMAP64,		TEXT("RC_BITMAP64      Supports bitmaps > 64K                 :"),
		RC_GDI20_OUTPUT,	TEXT("RC_GDI20_OUTPUT  Has GDI 2.0 output calls               :"),
		RC_DI_BITMAP,		TEXT("RC_DI_BITMAP     Supports DIB to memory                 :"),
		RC_PALETTE,			TEXT("RC_PALETTE       Supports a palette                     :"),
		RC_DIBTODEV,		TEXT("RC_DIBTODEV      Supports bitmap conversion             :"),
		RC_BIGFONT,			TEXT("RC_BIGFONT       Supports fonts > 64K                   :"),
		RC_STRETCHBLT,		TEXT("RC_STRETCHBLT    Supports StretchBlt                    :"),
		RC_FLOODFILL,		TEXT("RC_FLOODFILL     Supports FloodFill                     :"),
		RC_STRETCHDIB,		TEXT("RC_STRETCHDIB    Supports StretchDIBits                 :")
	};

	static PCSTR szTech[] =
	{
		TEXT("DT_PLOTTER (Vector plotter)"),
		TEXT("DT_RASDISPLAY (Raster display)"),
		TEXT("DT_RASPRINTER (Raster printer)"),
		TEXT("DT_RASCAMERA (Raster camera)"),
		TEXT("DT_CHARSTREAM (Character stream)"),
		TEXT("DT_METAFILE (Metafile)"),
		TEXT("DT_DISPFILE (Display file)")
	};

	int		i = 0;
	TCHAR	szBuffer[80] = { 0 };
	TextOut(hDC, cxChar, cyChar, szBuffer,
		wsprintf(szBuffer, TEXT("%-24s%04XH"), TEXT("DRIVERVERSION: "),
			GetDeviceCaps(hdcInfo, DRIVERVERSION)));

	TextOut(hDC, cxChar, 2 * cyChar, szBuffer,
		wsprintf(szBuffer, TEXT("%-24s%-40s"), TEXT("TECHNOLOGY: "),
			szTech[GetDeviceCaps(hdcInfo, TECHNOLOGY)]));

	TextOut(hDC, cxChar, 4 * cyChar, szBuffer,
		wsprintf(szBuffer, TEXT("CLIPCAPS (Clipping Capabilities)")));

	for (i = 0; i < sizeof(clip) / sizeof(clip[0]); ++i)
		TextOut(hDC, 9 * cxChar, (i + 6) * cyChar, szBuffer,
			wsprintf(szBuffer, TEXT("%-45 s%3s"), clip[i].szDesc,
				GetDeviceCaps(hdcInfo, CLIPCAPS) & clip[i].iMask ?
					TEXT("Yes") : TEXT("No")));

	TextOut(hDC, cxChar, 8 * cyChar, szBuffer,
		wsprintf(szBuffer, TEXT("RASTERCAPS (Raster Capabilities)")));

	for (i = 0; i < sizeof(raster) / sizeof(raster[0]); ++i)
		TextOut(hDC, 9 * cxChar, (i + 10) * cyChar, szBuffer,
			wsprintf(szBuffer, TEXT("%-45 s%3s"), raster[i].szDesc,
				GetDeviceCaps(hdcInfo, RASTERCAPS) & raster[i].iMask ?
					TEXT("Yes") : TEXT("NO")));
}

void DoBitCodedCaps(HDC hDC, HDC hdcInfo, int cxChar, int cyChar, int iType)
{
	static BITS curves[] =
	{
		CC_CIRCLES,		TEXT("CC_CIRCLES           Can do circles                         :"),
		CC_PIE,			TEXT("CC_PIE               Can do pie wedges                      :"),
		CC_CHORD,		TEXT("CC_CHORD             Can do chord arcs                      :"),
		CC_ELLIPSES,	TEXT("CC_ELLIPSES          Can do ellipses                        :"),
		CC_WIDE,		TEXT("CC_WIDE              Can do wide borders                    :"),
		CC_STYLED,		TEXT("CC_STYLED            Can do styled borders                  :"),
		CC_WIDESTYLED,	TEXT("CC_WIDESTYLED        Can do wide and styled borders         :"),
		CC_INTERIORS,	TEXT("CC_INTERIORS         Can do interiors                       :")
	};

	static BITS lines[] =
	{
		LC_POLYLINE,	TEXT("LC_POLYLINE          Can do polyline                        :"),
		LC_MARKER,		TEXT("LC_MARKER            Can do markers                         :"),
		LC_POLYMARKER,	TEXT("LC_POLYMARKER        Can do polymarkers                     :"),
		LC_WIDE,		TEXT("LC_WIDE              Can do wide lines                      :"),
		LC_STYLED,		TEXT("LC_STYLED            Can do styled lines                    :"),
		LC_WIDESTYLED,	TEXT("LC_WIDESTYLED        Can do wide and styled lines           :"),
		LC_INTERIORS,	TEXT("LC_INTERIORS         Can do interiors                       :")
	};

	static BITS poly[] =
	{
		PC_POLYGON,		TEXT("PC_POLYGON           Can do alternate fill polygon          :"),
		PC_RECTANGLE,	TEXT("PC_RECTANGLE         Can do rectangle                       :"),
		PC_WINDPOLYGON,	TEXT("PC_WINDPOLYGON       Can do winding number fill polygon     :"),
		PC_SCANLINE,	TEXT("PC_SCANLINE          Can do scanlines                       :"),
		PC_WIDE,		TEXT("PC_WIDE              Can do wide borders                    :"),
		PC_STYLED,		TEXT("PC_STYLED            Can do styled borders                  :"),
		PC_WIDESTYLED,	TEXT("PC_WIDESTYLED        Can do wide and styled borders         :"),
		PC_INTERIORS,	TEXT("PC_INTERIORS         Can do interiors                       :")
	};

	static BITS text[] =
	{
		TC_OP_CHARACTER,	TEXT("TC_OP_CHARACTER      Can do character output precision      :"),
		TC_OP_STROKE,		TEXT("TC_OP_STROKE         Can do stroke output precision         :"),
		TC_CP_STROKE,		TEXT("TC_CP_STROKE         Can do stroke clip precision           :"),
		TC_CR_90,			TEXT("TC_CR_90             Can do 90 degree character rotation    :"),
		TC_CR_ANY,			TEXT("TC_CR_ANY            Can do any character rotation          :"),
		TC_SF_X_YINDEP,		TEXT("TC_SF_X_YINDEP       Can do scaling independent of X and Y  :"),
		TC_SA_DOUBLE,		TEXT("TC_SA_DOUBLE         Can do doubled character for scaling   :"),
		TC_SA_INTEGER,		TEXT("TC_SA_INTEGER        Can do integer multiples for scaling   :"),
		TC_SA_CONTIN,		TEXT("TC_SA_CONTIN         Can do any multiples for exact scaling :"),
		TC_IA_ABLE,			TEXT("TC_IA_ABLE           Can do italicizing                     :"),
		TC_UA_ABLE,			TEXT("TC_UA_ABLE           Can do underlining                     :"),
		TC_SO_ABLE,			TEXT("TC_SO_ABLE           Can do strikeouts                      :"),
		TC_RA_ABLE,			TEXT("TC_RA_ABLE           Can do raster fonts                    :"),
		TC_VA_ABLE,			TEXT("TC_VA_ABLE           Can do vector fonts                    :")
	};

	static struct
	{
		int		iIndex;
		PCSTR	szTitle;
		BITS	(*pBits)[];
		int		iSize;
	}
	bitInfo[] =
	{
		CURVECAPS,		TEXT("CURVECAPS (Curve Capabilities)"),			(BITS (*)[])curves, sizeof(curves) / sizeof(curves[0]),
		LINECAPS,		TEXT("LINECAPS (Line Capabilities)"),			(BITS (*)[])lines,	sizeof(lines) / sizeof(lines[0]),
		POLYGONALCAPS,	TEXT("POLYGONALCAPS (Polygonal Capabilities)"),	(BITS (*)[])poly,	sizeof(poly) / sizeof(poly[0]),
		TEXTCAPS,		TEXT("TEXTCAPS (Text Capabilities)"),			(BITS (*)[])text,	sizeof(text) / sizeof(text[0])
	};

	static TCHAR	szBuffer[80] = { 0 };
	BITS			(*pBits)[] = bitInfo[iType].pBits;
	int				i = 0, iDevCaps = GetDeviceCaps(hdcInfo, bitInfo[iType].iIndex);

	TextOut(hDC, cxChar, cyChar, bitInfo[iType].szTitle, lstrlen(bitInfo[iType].szTitle));

	for (i = 0; i < bitInfo[iType].iSize; ++i)
		TextOut(hDC, cxChar, (i + 3) * cyChar, szBuffer,
			wsprintf(szBuffer, TEXT("%-61s %3s"), (*pBits)[i].szDesc,
				iDevCaps & (*pBits)[i].iMask ? TEXT("Yes") : TEXT("No")));
}
