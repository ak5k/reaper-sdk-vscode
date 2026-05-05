#define REAPERAPI_IMPLEMENT

#include <WDL/wdltypes.h>

#include <reaper_plugin_functions.h>

#include "my_plugin.h"

extern "C"
{
    REAPER_PLUGIN_DLL_EXPORT auto REAPER_PLUGIN_ENTRYPOINT(REAPER_PLUGIN_HINSTANCE hInstance,
                                                           reaper_plugin_info_t* rec) -> int
    {
        PROJECT_NAME::hInstance = hInstance;
        if (rec != nullptr && REAPERAPI_LoadAPI(rec->GetFunc) == 0)
        {
            // Reject duplicate load if the API function is already registered.
            if (rec->GetFunc("ReaScriptAPIFunctionExample"))
                return 0;
            PROJECT_NAME::Register();
            return 1;
        }
        PROJECT_NAME::Unregister();
        return 0;
    }
}