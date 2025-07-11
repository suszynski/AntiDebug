#include <string>
#include <thread>
#include <chrono>
#include <array>
#include <iostream>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

//
// [SECTION] Types
//

struct AntiDebugOption
{
	std::string name;
    std::atomic<bool> enabled{ true };
    std::atomic<bool> detected{ false };
	ftxui::Component* p_button;

	AntiDebugOption(const std::string& n, bool e = false, bool d = false)
		: name(n), enabled(e), detected(d), p_button(nullptr)
	{}
};

#define ANTIDBG_OPTS_COUNT 4
using AntiDebugOptions = std::array<AntiDebugOption, ANTIDBG_OPTS_COUNT>;

//
// [SECTION] Defines
//

static std::atomic<bool> running{true};

//
// [SECTION] Functions
//

void uiRoutine(AntiDebugOptions& options)
{
    using namespace ftxui;

    std::array<bool, ANTIDBG_OPTS_COUNT> checkbox_states;
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
            return true;
        }

        return false;
    });

    auto screen{ ScreenInteractive::Fullscreen() };
    screen.Loop(component);
}

int main()
{
	AntiDebugOptions options{
		AntiDebugOption("IsDebuggerPresent", true),
        AntiDebugOption("BeingDebugged", true),
        AntiDebugOption("CheckRemoteDebuggerPresent", true),
        AntiDebugOption("NtQueryInformationProcess", true),
	};

    std::thread ui_thread([&options] { uiRoutine(options); });

    while (running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }

	return 0;
}