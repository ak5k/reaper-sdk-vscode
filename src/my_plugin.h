#pragma once
#include <config.h>
#include <reaper_plugin_functions.h>

namespace PROJECT_NAME
{
extern REAPER_PLUGIN_HINSTANCE hInstance; // used for dialogs, if any
void Register();
void Unregister();

} // namespace PROJECT_NAME
