#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <commctrl.h>
#include <objbase.h>
#include <stdint.h>
#include <string.h>

#include "pc_monitor.h"
#include "pc_monitor_window.h"

static void configure_process_display_scale_awareness(void)
{
    typedef BOOL(WINAPI * set_process_display_scale_awareness_context_function)(HANDLE);
    typedef HRESULT(WINAPI * set_process_display_scale_awareness_function)(int);
    typedef BOOL(WINAPI * set_process_display_scale_aware_function)(void);

    HMODULE userInterfaceLibrary = LoadLibraryA("user32.dll");
    HMODULE shellScalingLibrary  = NULL;

    if (userInterfaceLibrary == NULL)
    {
        return;
    }

    {
        set_process_display_scale_awareness_context_function
                setProcessDisplayScaleAwarenessContext = (set_process_display_scale_awareness_context_function)
                        GetProcAddress(userInterfaceLibrary, "SetProcessDpiAwarenessContext");
        if (setProcessDisplayScaleAwarenessContext != NULL &&
            setProcessDisplayScaleAwarenessContext((HANDLE)(intptr_t)-4) != FALSE)
        {
            FreeLibrary(userInterfaceLibrary);
            return;
        }
    }

    shellScalingLibrary = LoadLibraryA("shcore.dll");
    if (shellScalingLibrary != NULL)
    {
        set_process_display_scale_awareness_function
                setProcessDisplayScaleAwareness = (set_process_display_scale_awareness_function)
                        GetProcAddress(shellScalingLibrary, "SetProcessDpiAwareness");
        if (setProcessDisplayScaleAwareness != NULL && setProcessDisplayScaleAwareness(2) >= 0)
        {
            FreeLibrary(shellScalingLibrary);
            FreeLibrary(userInterfaceLibrary);
            return;
        }
        FreeLibrary(shellScalingLibrary);
    }

    {
        set_process_display_scale_aware_function
                setProcessDisplayScaleAware = (set_process_display_scale_aware_function)
                        GetProcAddress(userInterfaceLibrary, "SetProcessDPIAware");
        if (setProcessDisplayScaleAware != NULL)
        {
            (void)setProcessDisplayScaleAware();
        }
    }
    FreeLibrary(userInterfaceLibrary);
}

static int initialize_common_controls(void)
{
    INITCOMMONCONTROLSEX commonControlInitialization;

    memset(&commonControlInitialization, 0, sizeof(commonControlInitialization));
    commonControlInitialization.dwSize = sizeof(commonControlInitialization);
    commonControlInitialization.dwICC  = ICC_BAR_CLASSES;
    return InitCommonControlsEx(&commonControlInitialization) != FALSE;
}

static int register_monitor_window_class(HINSTANCE instanceHandle)
{
    WNDCLASSA windowClass;

    memset(&windowClass, 0, sizeof(windowClass));
    windowClass.style         = CS_DBLCLKS;
    windowClass.lpfnWndProc   = hyperdos_win32_pc_monitor_window_procedure;
    windowClass.hInstance     = instanceHandle;
    windowClass.lpszClassName = "HyperDOSMonitorWindow";
    windowClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    return RegisterClassA(&windowClass) != 0;
}

int hyperdos_win32_pc_monitor_run(HINSTANCE instanceHandle, LPSTR commandLine, int showCommand)
{
    HWND  windowHandle = NULL;
    HMENU menuHandle   = NULL;
    MSG   message;
    DWORD windowStyle = WS_OVERLAPPEDWINDOW;
    RECT  requestedClientRectangle;
    int   windowWidth    = 0;
    int   windowHeight   = 0;
    int   comInitialized = 0;
    char  commandLineErrorText[256];

    configure_process_display_scale_awareness();
    if (!initialize_common_controls())
    {
        return 1;
    }
    comInitialized          = SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED));
    commandLineErrorText[0] = '\0';
    if (!hyperdos_win32_pc_monitor_configure_from_command_line(commandLine,
                                                               commandLineErrorText,
                                                               sizeof(commandLineErrorText)))
    {
        MessageBoxA(NULL, commandLineErrorText, "HyperDOS PC Monitor", MB_OK | MB_ICONERROR);
        if (comInitialized)
        {
            CoUninitialize();
        }
        return 1;
    }

    hyperdos_win32_pc_monitor_open_trace_files();

    requestedClientRectangle.left   = 0;
    requestedClientRectangle.top    = 0;
    requestedClientRectangle.right  = hyperdos_win32_pc_monitor_get_initial_client_width();
    requestedClientRectangle.bottom = hyperdos_win32_pc_monitor_get_initial_client_height();
    AdjustWindowRectEx(&requestedClientRectangle, windowStyle, TRUE, 0);
    windowWidth  = requestedClientRectangle.right - requestedClientRectangle.left;
    windowHeight = requestedClientRectangle.bottom - requestedClientRectangle.top;

    if (!register_monitor_window_class(instanceHandle))
    {
        hyperdos_win32_pc_monitor_close_trace_files();
        if (comInitialized)
        {
            CoUninitialize();
        }
        return 1;
    }

    menuHandle = hyperdos_win32_pc_monitor_create_menu();
    if (menuHandle == NULL)
    {
        hyperdos_win32_pc_monitor_close_trace_files();
        if (comInitialized)
        {
            CoUninitialize();
        }
        return 1;
    }

    windowHandle = CreateWindowExA(0,
                                   "HyperDOSMonitorWindow",
                                   "HyperDOS PC Monitor",
                                   windowStyle,
                                   CW_USEDEFAULT,
                                   CW_USEDEFAULT,
                                   windowWidth,
                                   windowHeight,
                                   NULL,
                                   menuHandle,
                                   instanceHandle,
                                   NULL);
    if (windowHandle == NULL)
    {
        DestroyMenu(menuHandle);
        hyperdos_win32_pc_monitor_close_trace_files();
        if (comInitialized)
        {
            CoUninitialize();
        }
        return 1;
    }
    {
        int statusBarHeight = hyperdos_win32_pc_monitor_get_status_bar_height();
        if (statusBarHeight > 0)
        {
            SetWindowPos(windowHandle,
                         NULL,
                         0,
                         0,
                         windowWidth,
                         windowHeight + statusBarHeight,
                         SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }

    ShowWindow(windowHandle, showCommand);
    UpdateWindow(windowHandle);

    while (GetMessageA(&message, NULL, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    hyperdos_win32_pc_monitor_close_trace_files();
    if (comInitialized)
    {
        CoUninitialize();
    }
    return (int)message.wParam;
}
