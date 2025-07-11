#include "antidebug.h"

//
// [SECTION] Types
//

using TNtQueryInformationProcess = NTSTATUS(WINAPI*)(HANDLE ProcessHandle, DWORD ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);

//
// [SECTION] Functions
//

TNtQueryInformationProcess getNtQueryInformationProcess() 
{
	static TNtQueryInformationProcess nt_query{};

	if (!nt_query) 
	{
		HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
		if (hNtdll) 
			nt_query = reinterpret_cast<TNtQueryInformationProcess>(GetProcAddress(hNtdll, "NtQueryInformationProcess"));
	}

	return nt_query;
}

void AntiDebug::callbackIsDebuggerPresent(AntiDebugOption& option)
{
	option.detected = IsDebuggerPresent();
}

void AntiDebug::callbackBeingDebugged(AntiDebugOption& option)
{
	_PEB* p_peb{ NtCurrentTeb()->ProcessEnvironmentBlock };

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