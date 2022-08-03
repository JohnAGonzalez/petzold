#include <Windows.h>
#include <commdlg.h>
#include <tchar.h>		// for _tcsstr (strstr for Unicode & non-Unicode

#define MAX_STRING_LEN	 256

static TCHAR szFindText[MAX_STRING_LEN];
static TCHAR szReplText[MAX_STRING_LEN];

HWND PopFindFindDlg(HWND hWnd)
{
	static FINDREPLACE fr;	// must be static for modeless dialog!!!

	fr.lStructSize = sizeof(FINDREPLACE);
	fr.hwndOwner = hWnd;
	fr.hInstance = NULL;
	fr.Flags = FR_HIDEUPDOWN | FR_HIDEMATCHCASE | FR_HIDEWHOLEWORD;
	fr.lpstrFindWhat = szFindText;
	fr.lpstrReplaceWith = NULL;
	fr.wFindWhatLen = MAX_STRING_LEN;
	fr.wReplaceWithLen = 0;
	fr.lCustData = 0;
	fr.lpfnHook = NULL;
	fr.lpTemplateName = NULL;

	return FindText(&fr);
}

HWND PopFindReplaceDlg(HWND hWnd)
{
	static FINDREPLACE fr;	// must be static for modeless dialog!!!

	fr.lStructSize = sizeof(FINDREPLACE);
	fr.hwndOwner = hWnd;
	fr.hInstance = NULL;
	fr.Flags = FR_HIDEUPDOWN | FR_HIDEMATCHCASE | FR_HIDEWHOLEWORD;
	fr.lpstrFindWhat = szFindText;
	fr.lpstrReplaceWith = szReplText;
	fr.wFindWhatLen = MAX_STRING_LEN;
	fr.wReplaceWithLen = MAX_STRING_LEN;
	fr.lCustData = 0;
	fr.lpfnHook = NULL;
	fr.lpTemplateName = NULL;

	return ReplaceText(&fr);
}

BOOL PopFindFindText(HWND hWnd, int *iSearchOffset, LPFINDREPLACE pfr)
{
	int		iLength, iPos;
	PTSTR	pstrDoc, pstrPos;

	// Read in the edit document
	iLength = GetWindowTextLength(hWnd);

	if (NULL == (pstrDoc = (PTSTR)malloc((iLength + 1) * sizeof(TCHAR))))
		return FALSE;

	GetWindowText(hWnd, pstrDoc, iLength + 1);

	// Search the document for the find string
	pstrPos = _tcsstr(pstrDoc + *iSearchOffset, pfr->lpstrFindWhat);

	// Return an error code if the string cannot be found
	if (pstrPos == NULL)
		return FALSE;

	// Find the position in the document and the new start offset
	iPos = pstrPos - pstrDoc;
	*iSearchOffset = iPos + lstrlen(pfr->lpstrFindWhat);

	// Select the found text
	SendMessage(hWnd, EM_SETSEL, iPos, *iSearchOffset);
	SendMessage(hWnd, EM_SCROLLCARET, 0, 0);

	free(pstrDoc);

	return TRUE;
}

BOOL PopFindReplaceText(HWND hWnd, int *iSearchOffset, LPFINDREPLACE pfr)
{
	// Find the text
	if (!PopFindFindText(hWnd, iSearchOffset, pfr))
		return FALSE;

	// Replace it
	SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)pfr->lpstrReplaceWith);

	return TRUE;
}

BOOL PopFindNextText(HWND hWnd, int *iSearchOffset)
{
	FINDREPLACE fr;

	fr.lpstrFindWhat = szFindText;

	return PopFindFindText(hWnd, iSearchOffset, &fr);
}

BOOL PopFindValidFind(void)
{
	return *szFindText != '\0';
}
