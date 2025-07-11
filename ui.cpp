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

        Elements checkbox_elements;
        for (int i{}; i < options.size(); i++)
        {
            auto status{ options[i].detected ? text(" [DETECTED]") | color(Color::Red) : text("") };
            checkbox_elements.push_back(
                hbox({
                    checkboxes[i]->Render(),
                    status
                    })
            );
        }

        return vbox({
            text("AntiDebug - Haxo Games Inc.") | bold | center,
            separator(),
            vbox(checkbox_elements),
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

ftxui::ScreenInteractive& UI::getScreen()
{
    return screen;
}