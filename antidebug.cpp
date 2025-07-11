#include "antidebug.h"

//
// [SECTION] Functions
//

void AntiDebug::callbackIsDebuggerPresent(AntiDebugOption& option)
{
	option.detected = IsDebuggerPresent();
}