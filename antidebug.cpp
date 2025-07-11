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
	// TODO
}

void AntiDebug::callbackNtQueryInformationProcess(AntiDebugOption& option)
{
	// TODO
}
