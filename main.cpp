#include <string>
#include <vector>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

struct AntiDebugOption
{
	std::string name;
	bool enabled;
	bool detected;
	ftxui::Component* p_button;

	AntiDebugOption(const std::string& n, bool e = false, bool d = false)
		: name(n), enabled(e), detected(d)
	{}
};

int main()
{
	using namespace ftxui;

	std::vector<AntiDebugOption> options = {
		{ "IsDebuggerPresent", false, false },
		{ "BeingDebugged", false, false },
		{ "CheckRemoteDebuggerPresent", false, false },
		{ "NtQueryInformationProcess", false, false },
	};

	std::vector<Component> button_components;
	for (auto& option : options)
	{
		auto button{ Button(option.name, [&option] {
			option.enabled = !option.enabled;
		}) };

		button_components.push_back(button);
		option.p_button = &button_components.back();
	}

	auto container{ Container::Vertical(button_components) };

	auto renderer{ Renderer(container, [&] {
		auto terminal_info{ Terminal::Size() };

		int button_width{ 36 };
		int columns{ std::max(1, (terminal_info.dimx - 4) / button_width) };

		Elements rows;
		Elements current_row;

		for (int i{}; i < options.size(); i++)
		{
			auto& option{ options[i] };
			
			auto title{ text(option.name) | (option.detected ? color(Color::Red) : color(Color::Default)) | center | bold };
			auto status{ text(option.enabled ? "ENABLED" : "DISABLED") | (option.enabled ? color(Color::Green) : color(Color::Red)) | center };

			auto button_display = vbox({ title, status }) | border | size(WIDTH, EQUAL, button_width - 2) | size(HEIGHT, EQUAL, 4);
			auto interactive_button = button_components[i]->Render() | size(WIDTH, EQUAL, button_width - 2) | size(HEIGHT, EQUAL, 4);
			auto combined = dbox({ button_display, interactive_button });

			current_row.push_back(combined);

			if (current_row.size() == columns || i == options.size() - 1)
			{
				while (current_row.size() < columns && i != options.size() - 1)
					current_row.push_back(text("") | size(WIDTH, EQUAL, button_width - 2));

				rows.push_back(hbox(current_row));
				current_row.clear();
			}
		}

		return vbox(rows) | border | center;
	}) };

	auto screen{ ScreenInteractive::TerminalOutput() };
	screen.Loop(renderer);

	return 0;
}