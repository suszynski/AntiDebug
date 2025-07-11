#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "antidebug.h"

namespace UI
{
    //
    // [SECTION] Variables
    //

    inline std::atomic<bool> running{ true };

    //
    // [SECTION] Functions
    //

    void routine(AntiDebug::AntiDebugOptions& options);
    void triggerUpdate();
}