#define REAPERAPI_IMPLEMENT
#include <reaper_plugin_functions.h>

#include "my_plugin.hpp"

extern "C"
{
    REAPER_PLUGIN_DLL_EXPORT auto REAPER_PLUGIN_ENTRYPOINT(
        REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t* rec) -> int
    {
        MyPlugin& my_plugin_instance = MyPlugin::GetInstance();
        if (rec != nullptr && REAPERAPI_LoadAPI(rec->GetFunc) == 0)
        {
            my_plugin_instance.SetReaperPluginInstance(hInstance);

            my_plugin_instance.Register();

            return 1;
        }

        // quit
        my_plugin_instance.Unregister();
        return 0;
    }
}