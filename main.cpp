#include <thread>
#include <chrono>
#include <array>
#include <iostream>

#include "ui.h"
#include "antidebug.h"

int main()
{
    using namespace AntiDebug;

    AntiDebugOptions options{
        AntiDebugOption("IsDebuggerPresent", true, callbackIsDebuggerPresent),
        AntiDebugOption("BeingDebugged", true, callbackBeingDebugged),
        AntiDebugOption("CheckRemoteDebuggerPresent", true, callbackCheckRemoteDebuggerPresent),
        AntiDebugOption("NtQueryInformationProcess_ProcessDebugPort", true, callbackNtQueryInformationProcessProcessDebugPort),
        AntiDebugOption("NtQueryInformationProcess_ProcessDebugFlags", true, callbackNtQueryInformationProcessProcessDebugFlags),
        AntiDebugOption("NtQueryInformationProcess_ProcessDebugHandle", true, callbackNtQueryInformationProcessProcessDebugHandle),
    };

    SetConsoleTitleA("AntiDebug");

    std::thread ui_thread([&options] { UI::routine(options); });

    while (UI::running)
    {
        for (auto& option : options)
        {
            if (option.enabled)
            {
                bool was_detected{ option.detected };
                option.callback(option);
                bool is_detected{ option.detected };

                if (is_detected && !was_detected || !is_detected && was_detected)
                    UI::triggerUpdate();

                option.was_enabled = true;
            }
            else
            {
                /* Black magic to ensure UI update when disables */
                if (option.was_enabled)
                {
                    UI::triggerUpdate();
                    option.was_enabled = false;
                }

                option.detected = false;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }

    ui_thread.join();

	return 0;
}