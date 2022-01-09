#include "my_plugin.hpp"
#include <reaper_plugin_functions.h>

// names
auto COMMAND = "MYTAG_MY_COMMAND";
auto ACTION = "MyTag: My action";

// our "main function" in this example
void MainFunctionOfMyPlugin()
{
    char buf[] = "Hello World\n";
    ShowConsoleMsg(buf);
    return;
}

// commandID declaration and
// C++11 style initialization
int commandID {0};

// initial toggle on/off state
// after plugin is loaded
bool state {false};

// enable reaper.deferlike behavior,
// more like timed while true loop over main function
// MainFunctionOfMyPlugin will be registered on timer
bool defer {false};

// set action to register,
// 0 = main action,
// see reaper_plugin.h
custom_action_register_t action = {0, COMMAND, ACTION, NULL};

// returns current toggle on/off state,
// see reaper_plugin.h
int ToggleActionCallback(int command)
{
    if (command != commandID) {
        return -1;
    }
    else if (state) {
        return 1;
    }
    return 0;
}

// runs the main function on command,
// see reaper_plugin_functions.h
bool OnAction(
    KbdSectionInfo* sec, int command, int val, int valhw, int relmode,
    HWND hwnd)
{
    // treat unused variables 'pedantically'
    (void)sec;
    (void)val;
    (void)valhw;
    (void)relmode;
    (void)hwnd;

    // check command
    if (command != commandID) {
        return false;
    }

    // depending on state,
    // register main function to timer
    if (defer) {
        // flip state on/off
        state = !state;

        if (state) {
            // "reaper.defer(main)"
            plugin_register("timer", (void*)MainFunctionOfMyPlugin);
        }
        else {
            // "reaper.atexit(shutdown)"
            plugin_register("-timer", (void*)MainFunctionOfMyPlugin);
            // shutdown stuff
        }
    }
    else {
        // call main function once
        MainFunctionOfMyPlugin();
    }

    return true;
}

// add menu entry under Extensions
void MenuHook(const char* menuidstr, HMENU menu, int flag)
{
    if (strcmp(menuidstr, "Main extensions") || flag != 0)
        return;

    if (!menu) {
        menu = CreatePopupMenu();
    }

    int pos = GetMenuItemCount(menu);

    MENUITEMINFO mii;
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_TYPE | MIIM_ID;
    mii.fType = MFT_STRING;
    // menu name
    mii.dwTypeData = (char*)ACTION;
    // menu command
    mii.wID = commandID;
    // insert as next menu item
    InsertMenuItem(menu, pos++, true, &mii);
    return;
}

void RegisterMyPlugin()
{
    // register action name and get command_id
    commandID = plugin_register("custom_action", &action);

    // register action on/off state and callback function
    if (defer) {
        plugin_register("toggleaction", (void*)ToggleActionCallback);
    }

    // register run action/command
    plugin_register("hookcommand2", (void*)OnAction);

    // register menu
    plugin_register("hookcustommenu", (void*)MenuHook);
    AddExtensionsMainMenu();
}

void UnregisterMyPlugin()
{
    plugin_register("-custom_action", &action);
    plugin_register("-toggleaction", (void*)ToggleActionCallback);
    plugin_register("-hookcommand2", (void*)OnAction);
    plugin_register("-hookcustommenu", (void*)MenuHook);
    return;
}
