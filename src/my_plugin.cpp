#include "my_plugin.hpp"
#include <reaper_plugin_functions.h>

// enable reaper.defer like behavior,
// like timed while true loop over main function
// MainFunctionOfMyPlugin will be registered on REAPER main timer
bool defer {true};

// names
auto COMMAND = "AK5K_MY_COMMAND";
auto ACTION = "ak5k: my plugin";

// our "main function" in this example
void MainFunctionOfMyPlugin()
{
    char buf[] = "hello, world\n";
    ShowConsoleMsg(buf);
    return;
}

// commandID declaration and
// C++11 style initialization
int commandID {0};

// initial toggle on/off state
// after plugin is loaded
bool state {false};

// set action,
// 0 = main action,
// see reaper_plugin.h
custom_action_register_t action = {0, COMMAND, ACTION, NULL};

// returns current toggle on/off state,
// see reaper_plugin.h
int ToggleActionCallback(int command)
{
    if (command != commandID)
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
bool OnAction(KbdSectionInfo* sec, int command, int val, int valhw, int relmode,
              HWND hwnd)
{
    // treat unused variables 'pedantically'
    (void)sec;
    (void)val;
    (void)valhw;
    (void)relmode;
    (void)hwnd;

    // check command
    if (command != commandID)
    {
        return false;
    }

    // depending on state,
    // register main function to timer
    if (defer)
    {
        // flip state on/off
        state = !state;

        if (state)
        {
            // "reaper.defer(main)"
            plugin_register("timer", (void*)MainFunctionOfMyPlugin);
        }
        else
        {
            // "reaper.atexit(shutdown)"
            plugin_register("-timer", (void*)MainFunctionOfMyPlugin);
            // shutdown stuff
        }
    }
    else
    {
        // call main function once
        MainFunctionOfMyPlugin();
    }

    return true;
}

void RegisterMyPlugin()
{
    // register action name and get command_id
    commandID = plugin_register("custom_action", &action);

    // register action on/off state and callback function
    if (defer)
    {
        plugin_register("toggleaction", (void*)ToggleActionCallback);
    }

    // register run action/command
    plugin_register("hookcommand2", (void*)OnAction);
}

void UnregisterMyPlugin()
{
    plugin_register("-custom_action", &action);
    plugin_register("-toggleaction", (void*)ToggleActionCallback);
    plugin_register("-hookcommand2", (void*)OnAction);
    return;
}
