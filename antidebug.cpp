#include "antidebug.h"

//
// [SECTION] Functions
//

void AntiDebug::callbackIsDebuggerPresent(AntiDebugOption& option)
{
	option.detected = IsDebuggerPresent();
}

void AntiDebug::callbackBeingDebugged(AntiDebugOption& option)
{
	// TODO
}

void AntiDebug::callbackCheckRemoteDebuggerPresent(AntiDebugOption& option)
{
	BOOL is_debugged{};
	CheckRemoteDebuggerPresent(GetCurrentProcess(), &is_debugged);

	option.detected = is_debugged;
}

void AntiDebug::callbackNtQueryInformationProcess(AntiDebugOption& option)
{
	// TODO
}
