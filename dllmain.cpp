#include <Windows.h>
#include "dllmain.h"

extern "C" {
	HWND hwndApp = nullptr;
	HINSTANCE hInst = nullptr;
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	//ƒNƒ‰ƒX–¼Žæ“¾ 
	DWORD pid = 0;
	GetWindowThreadProcessId(hWnd, &pid);
	if (pid == 0)
	{
		return TRUE;
	}
	if (pid == GetCurrentProcessId()) {
		hwndApp = hWnd;
		hInst = (HINSTANCE)GetModuleHandle(0);
		return TRUE;
	}
	return FALSE;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		EnumWindows(EnumWindowsProc, NULL);
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
