#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "pc_monitor.h"

int WINAPI WinMain(HINSTANCE instanceHandle, HINSTANCE previousInstanceHandle, LPSTR commandLine, int showCommand)
{
    (void)previousInstanceHandle;
    return hyperdos_win32_pc_monitor_run(instanceHandle, commandLine, showCommand);
}
