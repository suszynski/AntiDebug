#pragma once

#include <array>
#include <atomic>
#include <string>

#include <ftxui/component/component.hpp>
#include <windows.h>
#include <winternl.h>

namespace AntiDebug
{
	//
	// [SECTION] Defines
	//

	constexpr int OPTIONS_COUNT{ 8 };

	//
	// [SECTION] Types
	//

	struct AntiDebugOption;
	using AntiDebugCallback = std::function<void(AntiDebugOption&)>;

	struct AntiDebugOption
	{
		std::string name;
		std::atomic<bool> enabled{ true };
		std::atomic<bool> detected{ false };
		bool was_enabled{ false };
		AntiDebugCallback callback;
		ftxui::Component* p_button;

		AntiDebugOption(const std::string& n, bool e, AntiDebugCallback callback)
			: name(n), enabled(e), callback(callback), detected(false), p_button(nullptr)
		{}
	};

	using AntiDebugOptions = std::array<AntiDebugOption, OPTIONS_COUNT>;

	//
	// [SECTION] Functions
	//

	void callbackIsDebuggerPresent(AntiDebugOption& option);
	void callbackBeingDebugged(AntiDebugOption& option);
	void callbackCheckRemoteDebuggerPresent(AntiDebugOption& option);
	void callbackNtQueryInformationProcessProcessDebugPort(AntiDebugOption& option);
	void callbackNtQueryInformationProcessProcessDebugFlags(AntiDebugOption& option);
	void callbackNtQueryInformationProcessProcessDebugHandle(AntiDebugOption& option);
	void callbackFindWindowByTitleAndClass(AntiDebugOption& option);
	void callbackGetThreadContext(AntiDebugOption& option);
}