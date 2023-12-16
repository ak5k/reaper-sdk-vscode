#include <reaper_plugin_functions.h>

namespace my_plugin
{
extern REAPER_PLUGIN_HINSTANCE hInstance; // used for dialogs, if any
auto Register() -> void;
auto Unregister() -> void;

} // namespace my_plugin
