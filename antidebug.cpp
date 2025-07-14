#include "antidebug.h"

//
// [SECTION] Types
//

using TNtQueryInformationProcess = NTSTATUS(WINAPI*)(HANDLE ProcessHandle, DWORD ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);

//
// [SECTION] Functions (Utils)
//

TNtQueryInformationProcess getNtQueryInformationProcess()
{
	static TNtQueryInformationProcess nt_query{};

	if (!nt_query)
	{
		HMODULE h_ntdll{ GetModuleHandleA("ntdll.dll") };
		if (h_ntdll)
			nt_query = reinterpret_cast<TNtQueryInformationProcess>(GetProcAddress(h_ntdll, "NtQueryInformationProcess"));
	}

	return nt_query;
}

//
// [SECTION] Functions (Callbacks)
//

void AntiDebug::callbackIsDebuggerPresent(AntiDebugOption& option)
{
	option.detected = IsDebuggerPresent();
}

void AntiDebug::callbackBeingDebugged(AntiDebugOption& option)
{
	PEB* p_peb{ NtCurrentTeb()->ProcessEnvironmentBlock }; // You can access the TEB from the file segment, and then the offset to the PEB pointer is 0x30

	option.detected = p_peb->BeingDebugged;
}

void AntiDebug::callbackCheckRemoteDebuggerPresent(AntiDebugOption& option)
{
	BOOL is_debugged{};
	CheckRemoteDebuggerPresent(GetCurrentProcess(), &is_debugged);

	option.detected = is_debugged;
}

void AntiDebug::callbackNtQueryInformationProcessProcessDebugPort(AntiDebugOption& option)
{
	DWORD_PTR debug_port{};
	if (NT_SUCCESS(getNtQueryInformationProcess()(GetCurrentProcess(), ProcessDebugPort, &debug_port, sizeof(debug_port), nullptr)) && debug_port != 0)
		option.detected = true;
	else
		option.detected = false;
}

void AntiDebug::callbackNtQueryInformationProcessProcessDebugFlags(AntiDebugOption& option)
{
	DWORD debug_flags{};
	if (NT_SUCCESS(getNtQueryInformationProcess()(GetCurrentProcess(), 31, &debug_flags, sizeof(debug_flags), nullptr)) && debug_flags == 0)
		option.detected = true;
	else
		option.detected = false;
}

void AntiDebug::callbackNtQueryInformationProcessProcessDebugHandle(AntiDebugOption& option)
{
	HANDLE debug_object{};
	if (NT_SUCCESS(getNtQueryInformationProcess()(GetCurrentProcess(), 30, &debug_object, sizeof(debug_object), nullptr)) && debug_object != 0)
		option.detected = true;
	else
		option.detected = false;
}

void AntiDebug::callbackFindWindowByTitleAndClass(AntiDebugOption& option)
{
	static const char* titles[] = { "Cheat Engine", "Process Hacker", "x64dbg", "x32dbg", "IDA Pro", "Ghidra", "Binary Ninja", nullptr };
	static const char* classes[] = { "OLLYDBG", "WinDbgFrameClass", "ProcessHacker", "PROCMON_WINDOW_CLASS", nullptr };

	bool (*findWindowByTitle)(const char*) = [](const char* substring) -> bool {
		HWND hwnd = nullptr;
		char windowText[256];
		while ((hwnd = FindWindowExA(nullptr, hwnd, nullptr, nullptr)) != nullptr)
			if (GetWindowTextA(hwnd, windowText, sizeof(windowText)) > 0 && strstr(windowText, substring) != nullptr)
				return true;
		return false;
	};

	for (int i = 0; titles[i]; i++)
		if (findWindowByTitle(titles[i])) { option.detected = true; return; }

	for (int i = 0; classes[i]; i++)
		if (FindWindowA(classes[i], nullptr)) { option.detected = true; return; }

	option.detected = false;
}

/* checks for hardware breakpoints */
void AntiDebug::callbackGetThreadContext(AntiDebugOption& option)
{
	CONTEXT ctx{};
	ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	option.detected = GetThreadContext(GetCurrentThread(), &ctx) && 
	                  (ctx.Dr0 || ctx.Dr1 || ctx.Dr2 || ctx.Dr3);
}