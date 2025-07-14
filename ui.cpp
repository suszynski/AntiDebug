#include <mutex>

#include "ui.h"

//
// [SECTION] Variables
//

static auto screen{ ftxui::ScreenInteractive::Fullscreen() };

//
// [SECTION] Functions
//

void UI::routine()
{
    using namespace ftxui;

    std::vector<Component> checkboxes;

    /* Lock guard scope */
    {
        std::lock_guard<std::mutex> guard(AntiDebug::options_mutex);
        for (auto& option : AntiDebug::options)
            checkboxes.push_back(Checkbox(option.name, &option.enabled));
    }

    auto checkbox_container{ Container::Vertical(checkboxes) };
    auto component = Renderer(checkbox_container, [&] {
        int detections_count{};
        Elements checkbox_elements;

        /* Lock guard scope */
        {
            std::lock_guard<std::mutex> guard(AntiDebug::options_mutex);
            for (int i{}; i < AntiDebug::options.size(); i++)
            {
                if (AntiDebug::options[i].detected)
                    detections_count++;

                auto status{ AntiDebug::options[i].detected ? text(" [DETECTED]") | color(Color::Red) : text("") };
                checkbox_elements.push_back(
                    hbox({
                        checkboxes[i]->Render(),
                        status
                        })
                );
            }
        }

        bool is_detected{ detections_count > 0 };
        std::string detection_text("Debugging is currently ");

        if (is_detected)
            detection_text += "detected " + std::to_string(detections_count) + (detections_count > 1 ? " times." : " time."); // needs to say times
        else
            detection_text += "not detected.";

        return vbox({
            text("AntiDebug - Haxo Games Inc.") | bold | center,
            text(detection_text) | (is_detected ? color(Color::Red) : color(Color::Green)) | center,
            separator(),
            vbox(checkbox_elements) | flex | vscroll_indicator | frame,
            separator(),
            text("Use arrow keys to navigate, Space to toggle, Q to quit") | dim
        }) | border;
    });

    component |= CatchEvent([&](Event event) {
        if (event == Event::Character('q') || event == Event::Character('Q'))
        {
            running = false;
            screen.ExitLoopClosure()();
            return true;
        }

        return false;
    });

    screen.Loop(component);
}

void UI::triggerUpdate()
{
    screen.PostEvent(ftxui::Event::Custom);
}