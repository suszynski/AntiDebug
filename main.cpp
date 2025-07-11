#include <thread>
#include <chrono>
#include <array>

#include "ui.h"
#include "antidebug.h"

int main()
{
    using namespace AntiDebug;

	AntiDebugOptions options{
		AntiDebugOption("IsDebuggerPresent", true, callbackIsDebuggerPresent),
        AntiDebugOption("BeingDebugged", true, callbackIsDebuggerPresent),
        AntiDebugOption("CheckRemoteDebuggerPresent", true, callbackIsDebuggerPresent),
        AntiDebugOption("NtQueryInformationProcess", true, callbackIsDebuggerPresent),
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
                    UI::screen.PostEvent(ftxui::Event::Custom);
            }
            else
                option.detected = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }

	return 0;
}