#pragma once

#include <array>
#include <atomic>
#include <string>

#include <ftxui/component/component.hpp>
#include <Windows.h>

namespace AntiDebug
{
	//
	// [SECTION] Types
	//

	#define ANTIDBG_OPTS_COUNT 4

	struct AntiDebugOption;
	using AntiDebugOptions = std::array<AntiDebugOption, ANTIDBG_OPTS_COUNT>;
	using AntiDebugCallback = std::function<void(AntiDebugOption&)>;

	struct AntiDebugOption
	{
		std::string name;
		std::atomic<bool> enabled{ true };
		std::atomic<bool> detected{ false };
		AntiDebugCallback callback;
		ftxui::Component* p_button;

		AntiDebugOption(const std::string& n, bool e, AntiDebugCallback callback)
			: name(n), enabled(e), callback(callback), detected(false), p_button(nullptr)
		{}
	};

	//
	// [SECTION] Callbacks
	//

	void callbackIsDebuggerPresent(AntiDebugOption& option)
	{
		option.detected = IsDebuggerPresent();
	}
}