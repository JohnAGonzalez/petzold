#include <Windows.h>
#include <VersionHelpers.h>

HDC GetPrinterDC(void)
{
	DWORD				dwNeeded, dwReturned;
	HDC					hDC = NULL;
	PRINTER_INFO_4 *	pInfo4;
	PRINTER_INFO_5 *	pInfo5;

	if (!IsWindowsXPOrGreater())			// Windows 98
	{
		EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_NETWORK, NULL, 5, NULL, 0, &dwNeeded, &dwReturned);
		if (!dwNeeded)
			return NULL;
		pInfo5 = (PRINTER_INFO_5 *)malloc(dwNeeded);
		EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_NETWORK, NULL, 5, (PBYTE)pInfo5, dwNeeded, &dwNeeded, &dwReturned);
		hDC = CreateDC(NULL, pInfo5->pPrinterName, NULL, NULL);
		free(pInfo5);
		pInfo5 = NULL;
	}
	else									// Windows NT
	{
		EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_NETWORK, NULL, 4, NULL, 0, &dwNeeded, &dwReturned);
		if (!dwNeeded)
			return NULL;
		pInfo4 = (PRINTER_INFO_4 *)malloc(dwNeeded);
		EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_NETWORK, NULL, 4, (PBYTE)pInfo4, dwNeeded, &dwNeeded, &dwReturned);
		hDC = CreateDC(NULL, pInfo4->pPrinterName, NULL, NULL);
		free(pInfo4);
		pInfo4 = NULL;
	}

	return hDC;
}
