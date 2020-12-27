#define REAPERAPI_IMPLEMENT

#include "main.cpp"

#include "plugin.h"

// command_id declaration
int command_id;

// initial toggle on/off state
// after plugin is loaded
bool state = false;

// enable reaper.deferlike behavior,
// more like timed while true loop over main function
bool defer = false;

// set action to register,
// 0 = main action,
// see reaper_plugin.h
custom_action_register_t action = {0, COMMAND_NAME, ACTION_NAME};

// returns current toggle on/off state,
// see reaper_plugin.h
int toggleactioncallback(int command)
{
    if (command != command_id)
    {
        return -1;
    }
    else if (state)
    {
        return 1;
    }
    return 0;
}

// runs the main function on command,
// see reaper_plugin_functions.h
bool onAction(KbdSectionInfo *sec, int command, int val, int valhw, int relmode, HWND hwnd)
{
    // check command
    if (command != command_id)
    {
        return false;
    }

    if (defer)
    {
        // flip state on/off
        state = !state;

        if (state)
        {
            // "reaper.defer(main)"
            plugin_register("timer", (void *)main);
        }
        else
        {
            // "reaper.atexit(shutdown)"
            plugin_register("-timer", (void *)main);
            // shutdown stuff
        }
    }
    else
    {
        // call main function once
        (void)main();
    }

    return true;
}

// add menu entry under Extensions
void menuhook(const char *menuidstr, HMENU menu, int flag)
{
    if (strcmp(menuidstr, "Main extensions") || flag != 0)
        return;

    if (!menu)
    {
        menu = CreatePopupMenu();
    }

    int pos = GetMenuItemCount(menu);

    MENUITEMINFO mii;
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_TYPE | MIIM_ID;
    mii.fType = MFT_STRING;
    // menu name
    mii.dwTypeData = (char*)ACTION_NAME;
    // menu command
    mii.wID = command_id;
    // insert as last
    InsertMenuItem(menu, pos++, true, &mii);
    return;
}

REAPER_PLUGIN_HINSTANCE g_hInst; // used for dialogs, if any

// plugin entrypoint
extern "C"
{
    REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t *rec)
    {
        g_hInst = hInstance;

        // fail or unload
        // unregister stuff
        if (!rec)
        {
            plugin_register("-custom_action", &action);
            plugin_register("-toggleaction", (void *)toggleactioncallback);
            plugin_register("-hookcommand2", (void *)onAction);
            plugin_register("-hookcustommenu", (void *)menuhook);
            return 0;
        }

        // load Reaper API functions or fail
        if (REAPERAPI_LoadAPI(rec->GetFunc) != 0)
        {
            return 0;
        }

        // register action name and get command_id
        command_id = plugin_register("custom_action", &action);

        // register action on/off state and callback function
        if (defer)
        {
            plugin_register("toggleaction", (void *)toggleactioncallback);
        }

        // register run action/command
        plugin_register("hookcommand2", (void *)onAction);
        plugin_register("hookcustommenu", (void *)menuhook);
        AddExtensionsMainMenu();

        return 1;
    }
};
