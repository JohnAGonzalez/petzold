#include <Windows.h>
#include <commdlg.h>
#include "resource.h"

#define EDITID		1
#define UNTITLED	TEXT("(untitled)")

LRESULT CALLBACK WndProc		(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK AboutDlgProc	(HWND, UINT, WPARAM, LPARAM);

// Functions in PopFile.cpp
void PopFileInitialize	(HWND);
BOOL PopFileOpenDlg		(HWND, PTSTR, PTSTR);
BOOL PopFileSaveDlg		(HWND, PTSTR, PTSTR);
BOOL PopFileRead		(HWND, PTSTR);
BOOL PopFileWrite		(HWND, PTSTR);

// Functions in PopFind.cpp
HWND PopFindFindDlg		(HWND);
HWND PopFindReplaceDlg	(HWND);
BOOL PopFindFindText	(HWND, int *, LPFINDREPLACE);
BOOL PopFindReplaceText	(HWND, int *, LPFINDREPLACE);
BOOL PopFindNextText	(HWND, int *);
BOOL PopFindValidFind	(void);

// Functions in PopFont.cpp
void PopFontInitialize		(HWND);
BOOL PopFontChooseFont		(HWND);
void PopFontSetFont			(HWND);
void PopFontDeinitialize	(void);

// Functions in PopPrint0.cpp
BOOL PopPrntPrintFile	(HINSTANCE, HWND, HWND, PTSTR);

// Global variables
static HWND		hWndModeless;
static TCHAR	szAppName[] = TEXT("PopPad");

int WINAPI WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	PSTR		szCmdLine,
	int			iCmdShow)
{
	HWND		hWnd;
	MSG			msg;
	HACCEL		hAccel;
	WNDCLASS	wndClass;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(POPPAD));
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hInstance = hInstance;
	wndClass.lpszMenuName = MAKEINTRESOURCE(POPPAD);
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(
		szAppName, NULL, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, szCmdLine);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(POPPAD));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (hWndModeless == NULL || !IsDialogMessage(hWndModeless, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

void DoCaption(
	HWND	hWnd,
	TCHAR * szTitleName)
{
	TCHAR szCaption[64 + MAX_PATH] = { 0 };
	wsprintf(szCaption, TEXT("%s - %s"), szAppName, szTitleName[0] ? szTitleName : UNTITLED);

	SetWindowText(hWnd, szCaption);
}

void OkMessage(
	HWND	hWnd,
	PCSTR	szMessage,
	PCSTR	szTitleName)
{
	TCHAR szBuffer[64 + MAX_PATH] = { 0 };

	wsprintf(szBuffer, szMessage, szTitleName[0] ? szTitleName : UNTITLED);

	MessageBox(hWnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION);
}

short AskAboutSave(
	HWND	hWnd,
	TCHAR * szTitleName)
{
	TCHAR	szBuffer[64 + MAX_PATH] = { 0 };
	int		iReturn;

	wsprintf(szBuffer, TEXT("Save current changes in %s?"), szTitleName[0] ? szTitleName : UNTITLED);

	iReturn = MessageBox(hWnd, szBuffer, szAppName, MB_YESNOCANCEL | MB_ICONQUESTION);

	if (IDYES == iReturn)
		if (!SendMessage(hWnd, WM_COMMAND, IDM_FILE_SAVE, 0))
			iReturn = IDCANCEL;
	
	return iReturn;
}

LRESULT CALLBACK WndProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static BOOL			bNeedSave = FALSE;
	static HINSTANCE	hInst;
	static HWND			hWndEdit;
	static int			iOffset;
	static TCHAR		szFileName[MAX_PATH], szTitleName[MAX_PATH];
	static UINT			messageFindReplace;
	int					iSelBeg, iSelEnd, iEnable;
	LPFINDREPLACE		pFR;

	switch (msg)
	{
	case WM_CREATE:
		hInst = ((LPCREATESTRUCT)lParam)->hInstance;

		// Create the edit control child window
		hWndEdit = CreateWindow(
			TEXT("edit"), NULL,
			WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
			0, 0, 0, 0,
			hWnd, (HMENU)EDITID, hInst, NULL);

		SendMessage(hWndEdit, EM_LIMITTEXT, 32000, 0L);

		// Initialize common dialog box stuff
		PopFileInitialize(hWnd);
		PopFontInitialize(hWndEdit);

		messageFindReplace = RegisterWindowMessage(FINDMSGSTRING);

		DoCaption(hWnd, szTitleName);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hWndEdit);
		return 0;

	case WM_SIZE:
		MoveWindow(hWndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;

	case WM_INITMENUPOPUP:
		switch (lParam)
		{
		case 1:		// Edit Menu
			// Enable Undo if edit control can do it
			EnableMenuItem((HMENU)wParam, IDM_EDIT_UNDO, SendMessage(hWndEdit, EM_CANUNDO, 0, 0L) ? MF_ENABLED : MF_GRAYED);

			// Enable Paste if text is in the clipboard
			EnableMenuItem((HMENU)wParam, IDM_EDIT_PASTE, IsClipboardFormatAvailable(CF_TEXT) ? MF_ENABLED : MF_GRAYED);

			// Enable Cut, Copy, and Del if text is selected
			SendMessage(hWndEdit, EM_GETSEL, (WPARAM)&iSelBeg, (LPARAM)&iSelEnd);

			iEnable = iSelBeg != iSelEnd ? MF_ENABLED : MF_GRAYED;
			EnableMenuItem((HMENU)wParam, IDM_EDIT_CUT, iEnable);
			EnableMenuItem((HMENU)wParam, IDM_EDIT_COPY, iEnable);
			EnableMenuItem((HMENU)wParam, IDM_EDIT_CLEAR, iEnable);
			break;

		case 2:		// Search menu
			// Enable Find, next, and Replace if modeless dialogs are not already active
			iEnable = hWndModeless == NULL ? MF_ENABLED : MF_GRAYED;
			EnableMenuItem((HMENU)wParam, IDM_SEARCH_FIND, iEnable);
			EnableMenuItem((HMENU)wParam, IDM_SEARCH_NEXT, iEnable);
			EnableMenuItem((HMENU)wParam, IDM_SEARCH_REPLACE, iEnable);
			break;

		}
		return 0;

	case WM_COMMAND:
		// Message from edit control
		if (lParam && EDITID == LOWORD(wParam))
		{
			switch (HIWORD(wParam))
			{
			case EN_UPDATE:
				bNeedSave = TRUE;
				return 0;

			case EN_ERRSPACE:
			case EN_MAXTEXT:
				MessageBox(hWnd, TEXT("Edit Control out of space."), szAppName, MB_OK | MB_ICONSTOP);
				return 0;

			}
			break;

		}

		switch (LOWORD(wParam))
		{
			// messages from File menu
		case IDM_FILE_NEW:
			SetWindowText(hWnd, TEXT("\0"));
			szFileName[0] = '\0';
			szTitleName[0] = '\0';
			DoCaption(hWnd, szTitleName);
			bNeedSave = FALSE;
			return 0;

		case IDM_FILE_OPEN:
			if (bNeedSave && IDCANCEL == AskAboutSave(hWnd, szTitleName))
				return 0;

			if (PopFileOpenDlg(hWnd, szFileName, szTitleName))
			{
				if (!PopFileRead(hWndEdit, szFileName))
				{
					OkMessage(hWnd, TEXT("Could not read file %s!"), szTitleName);
					szFileName[0] = '\0';
					szTitleName[0] = '\0';
				}
			}

			DoCaption(hWnd, szTitleName);
			bNeedSave = FALSE;
			return 0;

		case IDM_FILE_SAVE:
			if (szFileName[0])
			{
				if (PopFileWrite(hWndEdit, szFileName))
				{
					bNeedSave = FALSE;
					return 1;
				}
				else
				{
					OkMessage(hWnd, TEXT("Could not write file %s"), szTitleName);
					return 0;
				}
			}

			// fall through

		case IDM_FILE_SAVE_AS:
			if (PopFileSaveDlg(hWnd, szFileName, szTitleName))
			{
				DoCaption(hWnd, szFileName);

				if (PopFileWrite(hWndEdit, szFileName))
				{
					bNeedSave = FALSE;
					return 1;
				}
				else
				{
					OkMessage(hWnd, TEXT("Could not write file %s"), szTitleName);
					return 0;
				}
			}
			return 0;

		case IDM_FILE_PRINT:
			if (!PopPrntPrintFile(hInst, hWnd, hWndEdit, szTitleName))
				OkMessage(hWnd, TEXT("Could not print file %s"), szTitleName);

			return 0;

		case IDM_APP_EXIT:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			return 0;

			// Messages from Edit menu

		case IDM_EDIT_UNDO:			SendMessage(hWndEdit, WM_UNDO, 0, 0);		return 0;
		case IDM_EDIT_CUT:			SendMessage(hWndEdit, WM_CUT, 0, 0);		return 0;
		case IDM_EDIT_COPY:			SendMessage(hWndEdit, WM_COPY, 0, 0);		return 0;
		case IDM_EDIT_PASTE:		SendMessage(hWndEdit, WM_PASTE, 0, 0);		return 0;
		case IDM_EDIT_CLEAR:		SendMessage(hWndEdit, WM_CLEAR, 0, 0);		return 0;
		case IDM_EDIT_SELECT_ALL:	SendMessage(hWndEdit, EM_SETSEL, 0, -1);	return 0;

			// Messages from Search Menu

		case IDM_SEARCH_FIND:
			SendMessage(hWndEdit, EM_GETSEL, 0, (LPARAM)&iOffset);
			hWndModeless = PopFindFindDlg(hWnd);
			return 0;

		case IDM_SEARCH_NEXT:
			SendMessage(hWndEdit, EM_GETSEL, 0, (LPARAM)&iOffset);

			if (PopFindValidFind())
				PopFindNextText(hWndEdit, &iOffset);
			else
				hWndModeless = PopFindFindDlg(hWnd);

			return 0;

		case IDM_SEARCH_REPLACE:
			SendMessage(hWndEdit, EM_GETSEL, 0, (LPARAM)&iOffset);
			hWndModeless = PopFindReplaceDlg(hWnd);
			return 0;

		case IDM_FORMAT_FONT:
			if (PopFontChooseFont(hWnd))
				PopFontSetFont(hWndEdit);

			return 0;

			// Messages from Help menu

		case IDM_HELP:
			OkMessage(hWnd, TEXT("Help not yet implemented"), TEXT("\0"));
			return 0;

		case IDM_APP_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(ABOUTBOX), hWnd, AboutDlgProc);
			return 0;

		}
		break;

	case WM_CLOSE:
		if (!bNeedSave || IDCANCEL != AskAboutSave(hWnd, szTitleName))
			DestroyWindow(hWnd);

		return 0;

	case WM_QUERYENDSESSION:
		if (!bNeedSave || IDCANCEL != AskAboutSave(hWnd, szTitleName))
			return 1;

		return 0;

	case WM_DESTROY:
		PopFontDeinitialize();
		PostQuitMessage(0);
		return 0;

	default:
		// process Find-Replace messages
		if (msg == messageFindReplace)
		{
			pFR = (LPFINDREPLACE)lParam;

			if (pFR->Flags & FR_DIALOGTERM)
				hWndModeless = NULL;

			if (pFR->Flags & FR_FINDNEXT)
				if (!PopFindFindText(hWndEdit, &iOffset, pFR))
					OkMessage(hWnd, TEXT("Text not found!"), TEXT("\0"));

			if (pFR->Flags & FR_REPLACE || pFR->Flags & FR_REPLACEALL)
				if (!PopFindReplaceText(hWndEdit, &iOffset, pFR))
					OkMessage(hWnd, TEXT("Text not found!"), TEXT("\0"));

			if (pFR->Flags & FR_REPLACEALL)
				while (PopFindReplaceText(hWndEdit, &iOffset, pFR));

			return 0;
		}
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

BOOL CALLBACK AboutDlgProc(
	HWND	hWnd,
	UINT	msg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hWnd, 0);
			return TRUE;
		}
		
		break;
	}

	return FALSE;
}