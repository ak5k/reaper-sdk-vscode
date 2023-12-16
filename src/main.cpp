#define REAPERAPI_IMPLEMENT
#include <reaper_plugin_functions.h>

#include "my_plugin.hpp"

extern "C"
{
REAPER_PLUGIN_DLL_EXPORT auto REAPER_PLUGIN_ENTRYPOINT(
    REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t* rec) -> int
{
    my_plugin::hInstance = hInstance;
    if (rec != nullptr && REAPERAPI_LoadAPI(rec->GetFunc) == 0)
    {
        my_plugin::Register();
        return 1;
    }
    // quit
    my_plugin::Unregister();
    return 0;
}
}