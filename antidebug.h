#pragma once

#include <array>
#include <mutex>
#include <string>

#include <ftxui/component/component.hpp>
#include <windows.h>
#include <winternl.h>

namespace AntiDebug
{
	//
	// [SECTION] Types
	//

	struct AntiDebugOption;
	using AntiDebugCallback = std::function<void(AntiDebugOption&)>;

	struct AntiDebugOption
	{
		std::string name;
		bool enabled;
		bool detected;
		bool was_enabled;
		AntiDebugCallback callback;
		ftxui::Component* p_button;

		AntiDebugOption(const std::string& n, bool e, AntiDebugCallback callback)
			: name(n), enabled(e), callback(callback), detected(false), p_button(nullptr), was_enabled(e)
		{}
	};

	//
	// [SECTION] Functions
	//

	void callbackIsDebuggerPresent(AntiDebugOption& option);
	void callbackBeingDebugged(AntiDebugOption& option);
	void callbackCheckRemoteDebuggerPresent(AntiDebugOption& option);
	void callbackNtQueryInformationProcessProcessDebugPort(AntiDebugOption& option);
	void callbackNtQueryInformationProcessProcessDebugFlags(AntiDebugOption& option);
	void callbackNtQueryInformationProcessProcessDebugHandle(AntiDebugOption& option);

	//
	// [SECTION] Variables (Temp placement)
	//

	inline std::array<AntiDebugOption, 6> options
	{
		AntiDebugOption("IsDebuggerPresent", true, callbackIsDebuggerPresent),
		AntiDebugOption("BeingDebugged", true, callbackBeingDebugged),
		AntiDebugOption("CheckRemoteDebuggerPresent", true, callbackCheckRemoteDebuggerPresent),
		AntiDebugOption("NtQueryInformationProcess_ProcessDebugPort", true, callbackNtQueryInformationProcessProcessDebugPort),
		AntiDebugOption("NtQueryInformationProcess_ProcessDebugFlags", true, callbackNtQueryInformationProcessProcessDebugFlags),
		AntiDebugOption("NtQueryInformationProcess_ProcessDebugHandle", true, callbackNtQueryInformationProcessProcessDebugHandle),
	};

	inline std::mutex options_mutex;
}