#include "antidebug.h"
#include "winstructs.h"

//
// [SECTION] Types and defines
//

#define ADD_ANTI_DEBUG_OPTION(name, default_enabled, callback, delay) AntiDebug::AntiDebugOption(name, default_enabled, AntiDebug::callback, std::chrono::milliseconds(delay))

//
// [SECTION] Variables
//

AntiDebug::AntiDebugOptions anti_debug_options
{
	ADD_ANTI_DEBUG_OPTION("IsDebuggerPresent", true, callbackIsDebuggerPresent),
	ADD_ANTI_DEBUG_OPTION("BeingDebugged", true, callbackBeingDebugged),
	ADD_ANTI_DEBUG_OPTION("NtGlobalFlag", true, callbackNtGlobalFlag),
	ADD_ANTI_DEBUG_OPTION("CheckRemoteDebuggerPresent", true, callbackCheckRemoteDebuggerPresent),
	ADD_ANTI_DEBUG_OPTION("NtQueryInformationProcess_ProcessDebugPort", true, callbackNtQueryInformationProcessProcessDebugPort),
	ADD_ANTI_DEBUG_OPTION("NtQueryInformationProcess_ProcessDebugFlags", true, callbackNtQueryInformationProcessProcessDebugFlags),
	ADD_ANTI_DEBUG_OPTION("NtQueryInformationProcess_ProcessDebugHandle", true, callbackNtQueryInformationProcessProcessDebugHandle),
	ADD_ANTI_DEBUG_OPTION("FindWindowByTitle", true, callbackFindWindowByTitle, 250),
	ADD_ANTI_DEBUG_OPTION("FindWindowByClass", true, callbackFindWindowByClass, 250),
	ADD_ANTI_DEBUG_OPTION("GetThreadContext", true, callbackGetThreadContext),
	ADD_ANTI_DEBUG_OPTION("NtQuerySystemInformation_DebuggerInformation", true, callbackNtQuerySystemInformation_DebuggerInformation),
	ADD_ANTI_DEBUG_OPTION("CloseHandle", false, callbackCloseHandle),
	ADD_ANTI_DEBUG_OPTION("DbgPrint", true, callbackDbgPrint),
	ADD_ANTI_DEBUG_OPTION("EnumDeviceDrivers", false, callbackEnumDeviceDrivers, 250)
};

//
// [SECTION] Functions (Utils)
//

AntiDebug::AntiDebugOptions& AntiDebug::getOptions()
{
	return anti_debug_options;
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
	static WinStructs::_PEB64* p_peb{ reinterpret_cast<WinStructs::_PEB64*>(NtCurrentTeb()->ProcessEnvironmentBlock) };
	option.detected = p_peb->BeingDebugged;
}

void AntiDebug::callbackNtGlobalFlag(AntiDebugOption& option)
{
	static WinStructs::_PEB64* p_peb{ reinterpret_cast<WinStructs::_PEB64*>(NtCurrentTeb()->ProcessEnvironmentBlock) };
	option.detected = p_peb->NtGlobalFlag & (0x10 | 0x20 | 0x40); // 0x10: EAP_ENABLE_TAIL_CHECK, 0x20: HEAP_ENABLE_FREE_CHECK, 0x40: HEAP_VALIDATE_PARAMETERS
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
	if (NT_SUCCESS(NtQueryInformationProcess(GetCurrentProcess(), ProcessDebugPort, &debug_port, sizeof(debug_port), nullptr)) && debug_port != 0)
		option.detected = true;
	else
		option.detected = false;
}

void AntiDebug::callbackNtQueryInformationProcessProcessDebugFlags(AntiDebugOption& option)
{
	DWORD debug_flags{};
	if (NT_SUCCESS(NtQueryInformationProcess(GetCurrentProcess(), static_cast<_PROCESSINFOCLASS>(WinStructs::ProcessDebugFlags), &debug_flags, sizeof(debug_flags), nullptr)) && debug_flags == 0)
		option.detected = true;
	else
		option.detected = false;
}

void AntiDebug::callbackNtQueryInformationProcessProcessDebugHandle(AntiDebugOption& option)
{
	HANDLE debug_object{};
	if (NT_SUCCESS(NtQueryInformationProcess(GetCurrentProcess(), static_cast<_PROCESSINFOCLASS>(WinStructs::ProcessDebugObjectHandle), &debug_object, sizeof(debug_object), nullptr)) && debug_object != 0)
		option.detected = true;
	else
		option.detected = false;
}

void AntiDebug::callbackFindWindowByTitle(AntiDebugOption& option)
{
	static const char* titles[] = { "Cheat Engine", "Process Hacker", "x64dbg", "x32dbg", "IDA Pro", "IDA Free", "WinDbg", "Ghidra", "Binary Ninja", "System Informer", nullptr};

	for (int i{}; titles[i]; i++)
	{
		HWND hwnd{};
		char windowText[256];

		while ((hwnd = FindWindowExA(nullptr, hwnd, nullptr, nullptr)) != nullptr)
		{
			if (GetWindowTextA(hwnd, windowText, sizeof(windowText)) > 0 && strstr(windowText, titles[i]) != nullptr)
			{
				option.detected = true;
				return;
			}
		}
	}

	option.detected = false;
}

void AntiDebug::callbackFindWindowByClass(AntiDebugOption& option)
{
	static const char* classes[] = { "OLLYDBG", "WinDbgFrameClass", "ProcessHacker", "PROCMON_WINDOW_CLASS", nullptr };

	for (int i{}; classes[i]; i++)
		if (FindWindowA(classes[i], nullptr)) { option.detected = true; return; }

	option.detected = false;
}

// Checks for hardware breakpoints (this one seems unreliable)
void AntiDebug::callbackGetThreadContext(AntiDebugOption& option)
{
	CONTEXT ctx{};
	ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	option.detected = (ctx.Dr0 != 0) || (ctx.Dr1 != 0) || (ctx.Dr2 != 0) || (ctx.Dr3 != 0) || (ctx.Dr6 != 0) || (ctx.Dr7 & 0xFF);
}

// By kenanwastaken, some turkish kid (unable to make PRs)
void AntiDebug::callbackNtQuerySystemInformation_DebuggerInformation(AntiDebugOption& option)
{
	WinStructs::SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX debugger_info{};

	ULONG len;
	NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)0x95, &debugger_info, sizeof(debugger_info), &len);

	// This is when detection descriptions would be useful: https://github.com/haxo-games/AntiDebug/issues/7
	option.detected = debugger_info.DebuggerEnabled || debugger_info.DebuggerPresent;
}

void AntiDebug::callbackCloseHandle(AntiDebugOption& option)
{
	__try
	{
		CloseHandle(reinterpret_cast<HANDLE>(std::numeric_limits<int>::max()));
		option.detected = false;
	}
	__except (GetExceptionCode() == EXCEPTION_INVALID_HANDLE ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		option.detected = true;
	}
}

void AntiDebug::callbackDbgPrint(AntiDebugOption& option)
{
	__try
	{
		RaiseException(DBG_PRINTEXCEPTION_C, 0, 0, 0);
		option.detected = true;
	}
	__except (GetExceptionCode() == DBG_PRINTEXCEPTION_C)
	{
		option.detected = false;
	}
}

void AntiDebug::callbackEnumDeviceDrivers(AntiDebugOption& option)
{
	static const char* driver_names[] = 
	{ 
		"capcom.sys",     // Vulnerable signed driver historically abused to manual map drivers
		"dbk64.sys",      // DBVM driver
		"procexp152.sys", // Process explorer's driver
	};

	LPVOID drivers[1024];
	DWORD cb_required;

	if (EnumDeviceDrivers(drivers, sizeof(drivers), &cb_required)) 
	{
		int driver_count = cb_required / sizeof(LPVOID);

		for (int i{}; i < driver_count; i++) 
		{
			char driver_name[MAX_PATH];

			if (GetDeviceDriverBaseNameA(drivers[i], driver_name, sizeof(driver_name))) 
			{
				for (int i{}; driver_names[i]; i++)
				{
					if (strcmp(driver_name, driver_names[i]) == 0) 
					{
						option.detected = true;
						return;
					}
				}
			}
		}
	} // May the lord protect us from such sinful brackets
	
	option.detected = false;
}