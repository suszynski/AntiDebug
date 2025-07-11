#include "ui.h"

//
// [SECTION] Variables
//

static auto screen{ ftxui::ScreenInteractive::Fullscreen() };

//
// [SECTION] Functions
//

void UI::routine(AntiDebug::AntiDebugOptions& options)
{
    using namespace ftxui;

    std::array<bool, AntiDebug::OPTIONS_COUNT> checkbox_states;
    std::vector<Component> checkboxes;

    for (int i{}; i < options.size(); i++)
    {
        checkbox_states[i] = options[i].enabled.load();

        CheckboxOption option;
        option.label = options[i].name;
        option.checked = &checkbox_states[i];
        checkboxes.push_back(Checkbox(option));
    }

    auto checkbox_container{ Container::Vertical(checkboxes) };
    auto component = Renderer(checkbox_container, [&] {
        for (int i{}; i < options.size(); i++)
            options[i].enabled.store(checkbox_states[i]);

        int detections_count{};
        Elements checkbox_elements;
        for (int i{}; i < options.size(); i++)
        {
            if (options[i].detected)
                detections_count++;

            auto status{ options[i].detected ? text(" [DETECTED]") | color(Color::Red) : text("") };
            checkbox_elements.push_back(
                hbox({
                    checkboxes[i]->Render() | (options[i].detected ? color(Color::Red) : color(Color::Default)),
                    status
                })
            );
        }

        bool is_detected{ detections_count > 0 };
        std::string detection_text("Debugging is currently ");

        if (is_detected)
            detection_text += "detected " + std::to_string(detections_count) + (detections_count > 1 ? "times." : " time."); // needs to say times
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