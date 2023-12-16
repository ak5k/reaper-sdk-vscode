#include "my_plugin.hpp"
#include "reaper_vararg.hpp"
#include <reaper_plugin_functions.h>

// register main function on timer
// true or false
#define RUN_ON_TIMER true

// confine my plugin to namespace
namespace my_plugin
{

// some global non-const variables
// the necessary 'evil'
int command_id {0};
bool toggle_action_state {false};
constexpr auto command_name = "AK5K_MY_COMMAND";
constexpr auto action_name = "ak5k: my plugin";
custom_action_register_t action = {0, command_name, action_name, nullptr};

// hInstance is declared in header file my_plugin.hpp
// defined here
REAPER_PLUGIN_HINSTANCE hInstance {nullptr}; // used for dialogs, if any

// the main function of my plugin
// gets called via callback or timer
auto MainFunctionOfMyPlugin()
{
    ShowConsoleMsg("hello, world\n");
}

// c++11 trailing return type syntax
// REAPER calls this to check my plugin toggle state
auto ToggleActionCallback(int command) -> int
{
    if (command != command_id)
    {
        // not quite our command_id
        return -1;
    }
    if (toggle_action_state) // if toggle_action_state == true
    {
        return 1;
    }
    return 0;
}

// this gets called when my plugin action is run (e.g. from action list)
auto OnAction(KbdSectionInfo* sec, int command, int val, int valhw, int relmode,
              HWND hwnd) -> bool
{
    // treat unused variables 'pedantically'
    (void)sec;
    (void)val;
    (void)valhw;
    (void)relmode;
    (void)hwnd;

    // check command
    if (command != command_id)
    {
        return false;
    }

    // depending on RUN_ON_TIMER #definition,
    // register my plugins main function to timer
    if (RUN_ON_TIMER) // RUN_ON_TIMER is true or false
    {
        // flip state on/off
        toggle_action_state = !toggle_action_state;

        if (toggle_action_state) // if toggle_action_state == true
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
        // else call main function once
        MainFunctionOfMyPlugin();
    }

    return true;
}

// my plugins simple example data structure
struct MyPlugin
{
    int x = 0;
} my_plugin;

// definition string for example API function
auto reascript_api_function_example_defstring =
    "int" // return type
    "\0"  // delimiter ('separator')
    // input parameter types
    "int,bool,double,const char*,int,const int*,double*,char*,int"
    "\0"
    // input parameter names
    "whole_number,boolean_value,decimal_number,string_of_text,"
    "string_of_text_sz,input_parameterInOptional,"
    "return_valueOutOptional,"
    "return_stringOutOptional,return_stringOutsz"
    "\0"
    "help text for myfunction\n"
    "If optional input parameter is provided, produces optional return "
    "value.\n";

// example api function
auto ReaScriptAPIFunctionExample(
    int whole_number, bool boolean_value, double decimal_number,
    const char* string_of_text, int string_of_text_sz,
    const int* input_parameterInOptional, double* return_valueOutOptional,
    char* return_stringOutOptional, int return_string_sz) -> int
{
    // if optional integer is provided
    if (input_parameterInOptional != nullptr)
    {
        // assign value to deferenced output pointer
        *return_valueOutOptional =
            // by making this awesome calculation
            (*input_parameterInOptional + whole_number + decimal_number);
    }

    // lets conditionally produce optional output string
    if (boolean_value)
    {
        // copy string_of_text to return_stringOutOptional
        // *_sz is length/size of zero terminated string (C-style char array)
        memcpy(return_stringOutOptional, string_of_text,
               std::min(return_string_sz, string_of_text_sz) * sizeof(char));
    }
    return whole_number * whole_number;
};

// when my plugin gets loaded
// function to register my plugins 'stuff' with REAPER
auto Register() -> void
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

    // register the API function example
    // function, definition string and function 'signature'
    plugin_register("API_ReaScriptAPIFunctionExample",
                    (void*)ReaScriptAPIFunctionExample);
    plugin_register("APIdef_ReaScriptAPIFunctionExample",
                    (void*)reascript_api_function_example_defstring);
    plugin_register("APIvararg_ReaScriptAPIFunctionExample",
                    (void*)&InvokeReaScriptAPI<&ReaScriptAPIFunctionExample>);
}

// shutdown, time to exit
auto Unregister() -> void
{
    plugin_register("-custom_action", &action);
    plugin_register("-toggleaction", (void*)ToggleActionCallback);
    plugin_register("-hookcommand2", (void*)OnAction);
}

} // namespace my_plugin