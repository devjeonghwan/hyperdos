#ifndef HYPERDOS_WIN32_PC_MONITOR_WINDOW_H
#define HYPERDOS_WIN32_PC_MONITOR_WINDOW_H

#include <windows.h>

#include <stddef.h>

int hyperdos_win32_pc_monitor_configure_from_command_line(const char* commandLine,
                                                          char*       errorText,
                                                          size_t      errorTextSize);

void hyperdos_win32_pc_monitor_open_trace_files(void);

void hyperdos_win32_pc_monitor_close_trace_files(void);

int hyperdos_win32_pc_monitor_get_initial_client_width(void);

int hyperdos_win32_pc_monitor_get_initial_client_height(void);

int hyperdos_win32_pc_monitor_get_status_bar_height(void);

HMENU hyperdos_win32_pc_monitor_create_menu(void);

LRESULT CALLBACK hyperdos_win32_pc_monitor_window_procedure(HWND   windowHandle,
                                                            UINT   message,
                                                            WPARAM wordParameter,
                                                            LPARAM longParameter);

#endif
