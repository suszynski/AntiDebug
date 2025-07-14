#include "antidebug.h"

//
// [SECTION] Types and defines
//

#define ADD_ANTI_DEBUG_OPTION(name, default_enabled, callback) AntiDebug::AntiDebugOption(name, default_enabled, AntiDebug::callback)

using TNtQueryInformationProcess = NTSTATUS(WINAPI*)(HANDLE ProcessHandle, DWORD ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);
using TNtQuerySystemInformation = NTSTATUS(WINAPI*)(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX 
{
	BOOLEAN DebuggerAllowed;
	BOOLEAN DebuggerEnabled;
	BOOLEAN DebuggerPresent;
} SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX, * PSYSTEM_KERNEL_DEBUGGER_INFORMATION_EX;

//
// [SECTION] Variables
//

AntiDebug::AntiDebugOptions anti_debug_options
{
	ADD_ANTI_DEBUG_OPTION("IsDebuggerPresent", true, callbackIsDebuggerPresent),
	ADD_ANTI_DEBUG_OPTION("BeingDebugged", true, callbackBeingDebugged),
	ADD_ANTI_DEBUG_OPTION("CheckRemoteDebuggerPresent", true, callbackCheckRemoteDebuggerPresent),
	ADD_ANTI_DEBUG_OPTION("NtQueryInformationProcess_ProcessDebugPort", true, callbackNtQueryInformationProcessProcessDebugPort),
	ADD_ANTI_DEBUG_OPTION("NtQueryInformationProcess_ProcessDebugFlags", true, callbackNtQueryInformationProcessProcessDebugFlags),
	ADD_ANTI_DEBUG_OPTION("NtQueryInformationProcess_ProcessDebugHandle", true, callbackNtQueryInformationProcessProcessDebugHandle),
	ADD_ANTI_DEBUG_OPTION("NtQuerySystemInformation", true, callbackNtQuerySystemInformation_DebuggerInformation)
};

//
// [SECTION] Functions (Utils)
//

AntiDebug::AntiDebugOptions& AntiDebug::getOptions()
{
	return anti_debug_options;
}

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

TNtQuerySystemInformation getNtQuerySystemInformation()
{
	static TNtQuerySystemInformation nt_query{};

	if (!nt_query)
	{
		HMODULE h_ntdll{ GetModuleHandleA("ntdll.dll") };
		if (h_ntdll)
			nt_query = reinterpret_cast<TNtQuerySystemInformation>(GetProcAddress(h_ntdll, "NtQuerySystemInformation"));
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

// checks for hardware breakpoints
void AntiDebug::callbackGetThreadContext(AntiDebugOption& option)
{
	CONTEXT ctx{};
	ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	option.detected = GetThreadContext(GetCurrentThread(), &ctx) && (ctx.Dr0 || ctx.Dr1 || ctx.Dr2 || ctx.Dr3);
}

// By kenanwastaken, some turkish kid (unable to make PRs)
void AntiDebug::callbackNtQuerySystemInformation_DebuggerInformation(AntiDebugOption& option)
{
	SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX debugger_info{};

	ULONG len;
	getNtQuerySystemInformation()((SYSTEM_INFORMATION_CLASS)0x95, &debugger_info, sizeof(debugger_info), &len);

	// This is when detection descriptions would be useful: https://github.com/haxo-games/AntiDebug/issues/7
	option.detected = debugger_info.DebuggerEnabled || debugger_info.DebuggerPresent;
}