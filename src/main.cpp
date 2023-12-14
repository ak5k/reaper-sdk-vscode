#include "my_plugin.hpp"
#define REAPERAPI_IMPLEMENT
#include <reaper_plugin_functions.h>

REAPER_PLUGIN_HINSTANCE g_hInst; // used for dialogs, if any

extern "C"
{
    REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(
        REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t* rec)
    {
        g_hInst = hInstance;
        if (rec && REAPERAPI_LoadAPI(rec->GetFunc) == 0)
        {
            RegisterMyPlugin();
            return 1;
        }
        else
        {
            UnregisterMyPlugin();
            return 0;
        }
    }
}