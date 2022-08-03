#include <Windows.h>

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szCommandLine,
	int iCmdShow)
{
	MessageBox(NULL, TEXT("Hello Windows 10!"), TEXT("HelloMsg"), 0);
	return 0;
}