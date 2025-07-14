#include <thread>
#include <chrono>
#include <array>
#include <iostream>

#include "ui.h"
#include "antidebug.h"

int main()
{
    SetConsoleTitleA("AntiDebug");

    std::thread ui_thread([] { UI::routine(); });
    auto& options{ AntiDebug::getOptions() };

    while (UI::running)
    {
        AntiDebug::options_mutex.lock();

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

        AntiDebug::options_mutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }

    if (ui_thread.joinable())
        ui_thread.join();

	return 0;
}