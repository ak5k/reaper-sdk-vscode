#pragma once

#include <config.h>

#include <WDL/wdltypes.h>

#include <reaper_plugin_functions.h>

namespace PROJECT_NAME
{
extern REAPER_PLUGIN_HINSTANCE hInstance; // used for dialogs, if any
auto Register() -> void;
auto Unregister() -> void;

} // namespace PROJECT_NAME
