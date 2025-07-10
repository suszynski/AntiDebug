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
		return nullptr; // to change
	}) };

	auto screen{ ScreenInteractive::TerminalOutput() };
	screen.Loop(renderer);

	return 0;
}