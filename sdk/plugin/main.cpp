#include "main.h"

#include "../reaper_plugin_functions.h"

int main()
{
  char buf[] = "Hello World\n";
  ShowConsoleMsg(buf);
  std::cout << buf;
  return 0;
}