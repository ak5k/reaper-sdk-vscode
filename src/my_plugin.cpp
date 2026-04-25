#include "my_plugin.h"
#include "reaper_vararg.hpp"
#include <gsl/gsl>

#define STRINGIZE_DEF(x) #x
#define STRINGIZE(x) STRINGIZE_DEF(x)
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define API_ID MYAPI
#define RUN_ON_TIMER true

namespace PROJECT_NAME
{

// Global state required by REAPER callbacks.
int command_id{0};
bool toggle_action_state{false};
constexpr auto command_name = "AK5K_" STRINGIZE(PROJECT_NAME) "_COMMAND";
constexpr auto action_name = "ak5k: " STRINGIZE(PROJECT_NAME);
custom_action_register_t action = {0, command_name, action_name, nullptr};

// hInstance is declared in my_plugin.h and defined here.
REAPER_PLUGIN_HINSTANCE hInstance{nullptr}; // used for dialogs, if any

// Entry point invoked directly or from REAPER's timer hook.
void MainFunctionOfMyPlugin()
{
    ShowConsoleMsg("hello, world\n");
}

// REAPER calls this to query the action toggle state.
auto ToggleActionCallback(int command) -> int
{
    if (command != command_id)
    {
        // Command is not ours.
        return -1;
    }
    if (toggle_action_state)
        return 1;
    return 0;
}

// Called when the action is triggered from REAPER's action list.
bool OnAction(KbdSectionInfo* sec, int command, int val, int valhw, int relmode, HWND hwnd)
{
    // Unused in this handler.
    (void)sec;
    (void)val;
    (void)valhw;
    (void)relmode;
    (void)hwnd;

    if (command != command_id)
        return false;

    // Toggle timer registration or execute once, based on build-time mode.
    if (RUN_ON_TIMER)
    {
        // Flip action state on or off.
        toggle_action_state = !toggle_action_state;

        if (toggle_action_state)
        {
            // Equivalent to reaper.defer(main).
            plugin_register("timer", (void*)MainFunctionOfMyPlugin);
        }
        else
        {
            // Equivalent to reaper.atexit(shutdown).
            plugin_register("-timer", (void*)MainFunctionOfMyPlugin);
        }
    }
    else
    {
        MainFunctionOfMyPlugin();
    }

    return true;
}

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
    "value.\n"
    "If boolean is true, copies input string to optional output string.\n";

// example api function
int ReaScriptAPIFunctionExample(
    int whole_number,
    bool boolean_value,
    double decimal_number,
    const char* string_of_text,
    int string_of_text_sz,
    const int* input_parameterInOptional,
    double* return_valueOutOptional,
    char* return_stringOutOptional,
    int return_string_sz
)
{
    // if optional integer is provided
    if (input_parameterInOptional != nullptr)
    {
        // Assign computed value to the optional output pointer.
        *return_valueOutOptional =
            (*input_parameterInOptional + whole_number + decimal_number);
    }

    // Conditionally produce optional output string.
    if (boolean_value)
    {
        // *_sz indicates C-string buffer size including the null terminator.
        memcpy(return_stringOutOptional, string_of_text, min(return_string_sz, string_of_text_sz) * sizeof(char));
    }
    return whole_number * whole_number;
}

auto defstring_GetVersion =
    "void" // return type
    "\0"   // delimiter ('separator')
    // input parameter types
    "int*,int*,int*,int*,char*,int"
    "\0"
    // input parameter names
    "majorOut,minorOut,patchOut,tweakOut,commitOut,commitOut_sz"
    "\0"
    "returns version numbers of my plugin\n";

void GetVersion(int* majorOut, int* minorOut, int* patchOut, int* tweakOut, char* commitOut, int commitOut_sz)
{
    *majorOut = PROJECT_VERSION_MAJOR;
    *minorOut = PROJECT_VERSION_MINOR;
    *patchOut = PROJECT_VERSION_PATCH;
    *tweakOut = PROJECT_VERSION_TWEAK;
    const char* commit = STRINGIZE(PROJECT_VERSION_COMMIT);
    std::copy(commit, commit + min(commitOut_sz - 1, (int)strlen(commit)), commitOut);
    commitOut[min(commitOut_sz - 1, (int)strlen(commit))] = '\0'; // Ensure null termination
}

// Register actions and exported API functions with REAPER.
void Register()
{
    // Register action name and get command_id.
    command_id = plugin_register("custom_action", &action);

    // Register action toggle state callback.
    if (RUN_ON_TIMER)
        plugin_register("toggleaction", (void*)ToggleActionCallback);

    // Register action command handler.
    plugin_register("hookcommand2", (void*)OnAction);

    // Register the example API function and metadata.
    plugin_register("API_" STRINGIZE(API_ID)"_ReaScriptAPIFunctionExample", (void*)ReaScriptAPIFunctionExample);
    plugin_register(
        "APIdef_" STRINGIZE(API_ID)"_ReaScriptAPIFunctionExample", (void*)reascript_api_function_example_defstring
    );
    plugin_register("APIvararg_" STRINGIZE(API_ID)"_ReaScriptAPIFunctionExample", (void*)&InvokeReaScriptAPI<&ReaScriptAPIFunctionExample>);

    plugin_register("API_" STRINGIZE(API_ID)"_GetVersion", (void*)GetVersion);
    plugin_register("APIdef_" STRINGIZE(API_ID)"_GetVersion", (void*)defstring_GetVersion);
    plugin_register("APIvararg_" STRINGIZE(API_ID)"_GetVersion", (void*)&InvokeReaScriptAPI<&GetVersion>);
}

// Unregister hooks before plugin unload.
auto Unregister() -> void
{
    plugin_register("-custom_action", &action);
    plugin_register("-toggleaction", (void*)ToggleActionCallback);
    plugin_register("-hookcommand2", (void*)OnAction);
}

} // namespace PROJECT_NAME