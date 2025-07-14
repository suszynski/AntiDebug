#pragma once

#include <array>
#include <mutex>
#include <string>

#include <ftxui/component/component.hpp>
#define NOMINMAX
#include <windows.h>
#include <winternl.h>

namespace AntiDebug
{
	//
	// [SECTION] Types
	//

	struct AntiDebugOption
	{
		using AntiDebugCallback = std::function<void(AntiDebugOption&)>;

		std::string name;
		bool enabled;
		bool detected;
		bool was_enabled;
		AntiDebugCallback callback;

		AntiDebugOption(const std::string& n, bool e, AntiDebugCallback callback)
			: name(n), enabled(e), callback(callback), detected(false), was_enabled(e)
		{}
	};

	using AntiDebugOptions = std::array<AntiDebugOption, 13>;

	//
	// [SECTION] Variables
	//

	inline std::mutex options_mutex;

	//
	// [SECTION] Functions
	//

	AntiDebugOptions& getOptions();

	void callbackIsDebuggerPresent(AntiDebugOption& option);
	void callbackBeingDebugged(AntiDebugOption& option);
	void callbackNtGlobalFlag(AntiDebugOption& option);
	void callbackCheckRemoteDebuggerPresent(AntiDebugOption& option);
	void callbackNtQueryInformationProcessProcessDebugPort(AntiDebugOption& option);
	void callbackNtQueryInformationProcessProcessDebugFlags(AntiDebugOption& option);
	void callbackNtQueryInformationProcessProcessDebugHandle(AntiDebugOption& option);
	void callbackFindWindowByTitle(AntiDebugOption& option);
	void callbackFindWindowByClass(AntiDebugOption& option);
	void callbackGetThreadContext(AntiDebugOption& option);
	void callbackNtQuerySystemInformation_DebuggerInformation(AntiDebugOption& option);
	void callbackCloseHandle(AntiDebugOption& option);
	void callbackDbgPrint(AntiDebugOption& option);
}