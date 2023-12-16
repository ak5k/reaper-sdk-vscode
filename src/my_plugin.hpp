// void RegisterMyPlugin();
// void UnregisterMyPlugin();
#include <reaper_plugin_functions.h>

// register the main function on timer
// true or false
#define RUN_ON_TIMER true

// MyPlugin as singleton object
class MyPlugin
{
  public:
    static auto GetInstance() -> MyPlugin&
    {
        // initialized only once
        static MyPlugin instance;
        return instance;
    }

    // delete copy
    MyPlugin(const MyPlugin&) = delete;
    // delete copy assignment
    auto operator=(const MyPlugin&) -> MyPlugin& = delete;

    // delete move
    MyPlugin(MyPlugin&&) = delete;
    // delete move assignment
    auto operator=(MyPlugin&&) -> MyPlugin& = delete;

    ~MyPlugin() = default;

    auto SetReaperPluginInstance(REAPER_PLUGIN_HINSTANCE param)
    {
        hInstance = param;
        return;
    }

    auto GetCommandId()
    {
        return command_id;
    }

    auto GetToggleState()
    {
        return toggle_state;
    }

    auto SetCommandId(int param)
    {
        command_id = param;
        return;
    }

    auto SetToggleState(bool param)
    {
        toggle_state = param;
        return;
    }

    auto Register()
    {
        // register action name and get command_id
        command_id = plugin_register("custom_action", &action);

        // register action on/off state and callback function
        if (RUN_ON_TIMER)
        {
            plugin_register("toggleaction", (void*)ToggleActionCallback);
        }

        // register run action/command
        plugin_register("hookcommand2", (void*)OnAction);
        return;
    }

    auto Unregister()
    {
        plugin_register("-custom_action", &action);
        plugin_register("-toggleaction", (void*)ToggleActionCallback);
        plugin_register("-hookcommand2", (void*)OnAction);
        return;
    }

  private:
    // hidden private constructor
    MyPlugin() = default;
    int command_id {0};
    bool toggle_state {false};

    REAPER_PLUGIN_HINSTANCE hInstance {nullptr}; // used for dialogs, if any

    custom_action_register_t action = {0, "AK5K_MY_COMMAND", "ak5k: my plugin",
                                       nullptr};

    static auto MainFunctionOfMyPlugin()
    {
        ShowConsoleMsg("hello, world\n");
        return;
    }

    static auto ToggleActionCallback(int command) -> int
    {
        MyPlugin& my_plugin_instance = MyPlugin::GetInstance();

        if (command != my_plugin_instance.GetCommandId())
            return -1;

        if (my_plugin_instance.GetToggleState())
            return 1;

        return 0;
    }

    static auto OnAction(KbdSectionInfo* sec, int command, int val, int valhw,
                         int relmode, HWND hwnd) -> bool
    {
        // treat unused variables 'pedantically'
        (void)sec;
        (void)val;
        (void)valhw;
        (void)relmode;
        (void)hwnd;

        MyPlugin& my_plugin_instance = MyPlugin::GetInstance();

        // check command
        if (command != my_plugin_instance.GetCommandId())
            return false;

        // depending on state,
        // register main function to timer
        if (RUN_ON_TIMER)
        {
            // flip state on/off
            my_plugin_instance.SetToggleState(
                !my_plugin_instance.GetToggleState());

            if (my_plugin_instance.GetToggleState())
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
};
