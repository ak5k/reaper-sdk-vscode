#include "my_plugin.h"
#include "reaper_vararg.h"
#include <algorithm>
#include <cstring>

#define STRINGIZE_DEF(x) #x
#define STRINGIZE(x) STRINGIZE_DEF(x)
#define API_ID MYAPI

namespace PROJECT_NAME
{

// --- State ------------------------------------------------------------------

// Our mighty program state
struct ActionState
{
    int command_id;
    bool is_active;
};

ActionState action_state{0, false};

char command_name[] = STRINGIZE(PROJECT_VENDOR) "_" STRINGIZE(PROJECT_NAME) "_COMMAND";
char action_name[] = STRINGIZE(PROJECT_VENDOR) ": " STRINGIZE(PROJECT_NAME);
custom_action_register_t action = {0, command_name, action_name, nullptr};

REAPER_PLUGIN_HINSTANCE hInstance{nullptr}; // used for dialogs, if any

// --- Business logic ---------------------------------------------------------

// Build-time switch: run business logic on every REAPER timer tick (true),
// or execute once per action trigger (false).
enum
{
    RUN_ON_TIMER = 1
};

// Called on every timer tick when the action is active, or once per trigger.
void MainFunctionOfMyPlugin()
{
    ShowConsoleMsg("hello, world\n");
}

// --- REAPER action callbacks ------------------------------------------------

// Called when any action is triggered. Returns true if we handled it.
bool OnAction(KbdSectionInfo* sec, int command, int val, int val2, int relmode, HWND hwnd)
{
    (void)sec;
    (void)val;
    (void)val2;
    (void)relmode;
    (void)hwnd;

    if (command != action_state.command_id)
        return false;

    if (RUN_ON_TIMER)
    {
        action_state.is_active = !action_state.is_active;
        if (action_state.is_active)
            plugin_register("timer", (void*)MainFunctionOfMyPlugin); // reaper.defer(main)
        else
            plugin_register("-timer", (void*)MainFunctionOfMyPlugin); // reaper.atexit(shutdown)
    }
    else
    {
        MainFunctionOfMyPlugin();
    }

    return true;
}

// Called by REAPER to query the toggle state of an action.
// Returns 1 (active), 0 (inactive), or -1 (not our command).
int ToggleActionCallback(int command)
{
    if (command != action_state.command_id)
        return -1;
    return action_state.is_active ? 1 : 0;
}

// --- Exported ReaScript API functions ---------------------------------------

// Returns the square of whole_number.
// If input_parameterInOptional is provided, also writes the sum of all numeric
// inputs to return_valueOutOptional.
// If boolean_value is true, copies string_of_text into return_stringOutOptional.
char reascript_api_function_example_defstring[] =
    "int"                                                          // return type
    "\0"                                                           // delimiter
    "int,bool,double,const char*,int,const int*,double*,char*,int" // parameter types
    "\0"
    "whole_number,boolean_value,decimal_number,string_of_text,"
    "string_of_text_sz,input_parameterInOptional,"
    "return_valueOutOptional,"
    "return_stringOutOptional,return_stringOutsz" // parameter names
    "\0"
    "help text for myfunction\n"
    "If optional input parameter is provided, produces optional return value.\n"
    "If boolean is true, copies input string to optional output string.\n";

int ReaScriptAPIFunctionExample(int whole_number, bool boolean_value, double decimal_number,
                                char* string_of_text, int string_of_text_sz,
                                int* input_parameterInOptional, double* return_valueOutOptional,
                                char* return_stringOutOptional, int return_string_sz)
{
    if (input_parameterInOptional != nullptr)
        *return_valueOutOptional = (*input_parameterInOptional + whole_number + decimal_number);

    if (boolean_value)
    {
        // *_sz is the C-string buffer size including the null terminator.
        memcpy(return_stringOutOptional, string_of_text,
               std::min<int>(return_string_sz, string_of_text_sz) * sizeof(char));
    }

    return whole_number * whole_number;
}

// Returns version numbers of this plugin.
char defstring_GetVersion[] =
    "void"                                    // return type
    "\0"                                      // delimiter
    "int*,int*,int*,int*,char*,int,char*,int" // parameter types
    "\0"
    "majorOut,minorOut,patchOut,tweakOut,commitOut,commitOut_sz,branchOut,branchOut_sz"
    "\0"
    "Returns version numbers of this plugin.\n";

void GetVersion(int* majorOut, int* minorOut, int* patchOut, int* tweakOut, char* commitOut,
                int commitOut_sz, char* branchOut, int branchOut_sz)
{
    *majorOut = PROJECT_VERSION_MAJOR;
    *minorOut = PROJECT_VERSION_MINOR;
    *patchOut = PROJECT_VERSION_PATCH;
    *tweakOut = PROJECT_VERSION_TWEAK;

    int commit_len = std::min<int>(commitOut_sz - 1, (int)strlen(PROJECT_VERSION_COMMIT));
    memcpy(commitOut, PROJECT_VERSION_COMMIT, commit_len * sizeof(char));
    commitOut[commit_len] = '\0';

    int branch_len = std::min<int>(branchOut_sz - 1, (int)strlen(PROJECT_VERSION_BRANCH));
    memcpy(branchOut, PROJECT_VERSION_BRANCH, branch_len * sizeof(char));
    branchOut[branch_len] = '\0';
}

// --- Plugin lifecycle -------------------------------------------------------

// Called on load.
void Register()
{
    action_state.command_id = plugin_register("custom_action", &action);

    if (RUN_ON_TIMER)
        plugin_register("toggleaction", (void*)ToggleActionCallback);

    plugin_register("hookcommand2", (void*)OnAction);

    plugin_register("API_" STRINGIZE(API_ID) "_ReaScriptAPIFunctionExample",
                                     (void*)ReaScriptAPIFunctionExample);
    plugin_register("APIdef_" STRINGIZE(API_ID) "_ReaScriptAPIFunctionExample",
                                        (void*)reascript_api_function_example_defstring);
    plugin_register(
        "APIvararg_" STRINGIZE(API_ID) "_ReaScriptAPIFunctionExample",
                               (void*)&InvokeReaScriptAPI<&ReaScriptAPIFunctionExample>);

    plugin_register("API_" STRINGIZE(API_ID) "_GetVersion", (void*)GetVersion);
    plugin_register("APIdef_" STRINGIZE(API_ID) "_GetVersion", (void*)defstring_GetVersion);
    plugin_register("APIvararg_" STRINGIZE(API_ID) "_GetVersion",
                                           (void*)&InvokeReaScriptAPI<&GetVersion>);
}

// Called on unload.
void Unregister()
{
    plugin_register("-custom_action", &action);
    plugin_register("-toggleaction", (void*)ToggleActionCallback);
    plugin_register("-hookcommand2", (void*)OnAction);
}

} // namespace PROJECT_NAME