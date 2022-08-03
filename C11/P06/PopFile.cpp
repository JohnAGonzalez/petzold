#include <Windows.h>
#include <commdlg.h>

static OPENFILENAME	ofn;

void PopFileInitialize(HWND hWnd)
{
	static TCHAR	szFilter[] =
		TEXT("Text Files (*.TXT)\0*.txt\0")		\
		TEXT("ASCII Files (*.ASC)\0*.asc\0")	\
		TEXT("All Files (*.*)\0*.*\0\0");

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = NULL;		// Set in Open and Close functions
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrFile = NULL;
	ofn.Flags = 0;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = TEXT("txt");
	ofn.lCustData = 0L;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;
}

BOOL PopFileOpenDlg(HWND hWnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;

	return GetOpenFileName(&ofn);
}

BOOL PopFileSaveDlg(HWND hWnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	return GetSaveFileName(&ofn);
}

BOOL PopFileRead(HWND hWnd, PTSTR pstrFileName)
{
	BYTE	bySwap;
	DWORD	dwBytesRead;
	HANDLE	hFile;
	int		i, iFileLength, iUniTest;
	PBYTE	pBuffer, pText, pConv;

	// Open the file
	if (INVALID_HANDLE_VALUE == (hFile = CreateFile(pstrFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)))
		return FALSE;

	// Get file size in bytes and allowcate memory for read.
	// Add an extra two bytes for zero termination.
	iFileLength = GetFileSize(hFile, NULL);
	pBuffer = (PBYTE)malloc(iFileLength + 2);

	// Read file and put terminating zeros at end.
	ReadFile(hFile, pBuffer, iFileLength, &dwBytesRead, NULL);
	CloseHandle(hFile);
	pBuffer[iFileLength] = '\0';
	pBuffer[iFileLength + 1] = '\0';

	// Test to see if the text is Unicode
	iUniTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE;
	if (IsTextUnicode(pBuffer, iFileLength, &iUniTest))
	{
		pText = pBuffer + 2;
		iFileLength -= 2;

		if (iUniTest & IS_TEXT_UNICODE_REVERSE_SIGNATURE)
		{
			for (i = 0; i < iFileLength / 2; ++i)
			{
				bySwap = ((BYTE *)pText)[2 * i];
				((BYTE *)pText)[2 * i] = ((BYTE *)pText)[2 * i + 1];
				((BYTE *)pText)[2 * i + 1] = bySwap;
			}
		}

		// Allocate memory for possibly converted string
		pConv = (PBYTE)malloc(iFileLength + 2);

#ifdef UNICODE
		// If the edit control is ni Unicode, convert Unicode text to
		// non-Unicode (i.e. in general, wide character).
		WideCharToMultiByte(CP_ACP, 0, (PWSTR)pText, -1, pConv, iFileLength + 2, NULL, NULL);
#else
		// If the edit control is Unicode, just copy the string
		lstrcpy((PTSTR)pConv, (PTSTR)pText);
#endif
	}
	else
	{
		// the file is not Unicode
		pText = pBuffer;

		// Allocate memory for possibly converted string.
		pConv = (PBYTE)malloc(2 * iFileLength + 2);

#ifdef UNICODE
		// If the edit control is Unicode, convert ASCII text.
		MultiByteToWideChar(CP_ACP, 0, pText, -1, (PTSTR)pConv, iFileLength + 1);
#else
		// If not, just copy the buffer
		lstrcpy((PTSTR)pConv, (PTSTR)pText);
#endif
	}

	SetWindowText(hWnd, (PTSTR)pConv);
	free(pBuffer);
	free(pConv);
	return TRUE;
}

BOOL PopFileWrite(HWND hWnd, PTSTR pstrFileName)
{
	DWORD	dwBytesWritten;
	HANDLE	hFile;
	int		iLength;
	PTSTR	pstrBuffer;
	WORD	wByteOrderMark = 0XFEFF;

	// Open the file, creating it if necessary
	if (INVALID_HANDLE_VALUE == (hFile = CreateFile(pstrFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL)))
		return FALSE;

	// Get the number of characters in th eedit control an dallocate
	// memory for them.
	iLength = GetWindowTextLength(hWnd);
	pstrBuffer = (PTSTR)malloc((iLength + 1) * sizeof(TCHAR));

	if (!pstrBuffer)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	// If the edit contorl will return Unicode text, write the
	// byte order mark to the file.
#ifdef UNICODE
	WriteFile(hFile, &wByteOrderMark, 2, &dwBytesWritten, NULL);
#endif

	// Get the edit buffer and write that out to the file.
	GetWindowText(hWnd, pstrBuffer, iLength + 1);
	WriteFile(hFile, pstrBuffer, iLength * sizeof(TCHAR), &dwBytesWritten, NULL);

	if ((iLength * sizeof(TCHAR)) != (int)dwBytesWritten)
	{
		CloseHandle(hFile);
		free(pstrBuffer);
		return FALSE;
	}

	CloseHandle(hFile);
	free(pstrBuffer);
	return TRUE;
}