#define WIN32_LEAN_AND_MEAN

#include "pc_monitor_runtime.h"

#include <stdio.h>
#include <string.h>

void hyperdos_win32_pc_monitor_runtime_initialize(hyperdos_win32_pc_monitor_runtime*                 runtime,
                                                  const hyperdos_win32_pc_monitor_runtime_callbacks* callbacks,
                                                  void*                                              callbackContext)
{
    if (runtime == NULL)
    {
        return;
    }

    memset(runtime, 0, sizeof(*runtime));
    runtime->callbackContext                   = callbackContext;
    runtime->hostMouseCursorConfinementEnabled = 1u;
    runtime->hostMouseCursorHidingEnabled      = 1u;
    if (callbacks != NULL)
    {
        runtime->callbacks = *callbacks;
    }
    InitializeCriticalSection(&runtime->keyboardCriticalSection);
    InitializeCriticalSection(&runtime->diskCriticalSection);
    runtime->keyboardEventHandle = CreateEventA(NULL, FALSE, FALSE, NULL);
}

void hyperdos_win32_pc_monitor_runtime_close_emulation_thread_handle(hyperdos_win32_pc_monitor_runtime* runtime)
{
    if (runtime == NULL || runtime->emulationThreadHandle == NULL)
    {
        return;
    }

    WaitForSingleObject(runtime->emulationThreadHandle, 2000u);
    CloseHandle(runtime->emulationThreadHandle);
    runtime->emulationThreadHandle = NULL;
}

void hyperdos_win32_pc_monitor_runtime_stop_emulation_thread(hyperdos_win32_pc_monitor_runtime* runtime)
{
    if (runtime == NULL)
    {
        return;
    }

    InterlockedExchange(&runtime->stopRequested, 1);
    InterlockedExchange(&runtime->isRunning, 0);
    InterlockedExchange(&runtime->isWaitingForKeyboard, 0);
    if (runtime->keyboardEventHandle != NULL)
    {
        SetEvent(runtime->keyboardEventHandle);
    }
    hyperdos_win32_pc_monitor_runtime_close_emulation_thread_handle(runtime);
}

void hyperdos_win32_pc_monitor_runtime_destroy(hyperdos_win32_pc_monitor_runtime* runtime)
{
    if (runtime == NULL)
    {
        return;
    }

    hyperdos_win32_pc_monitor_runtime_stop_emulation_thread(runtime);
    if (runtime->keyboardEventHandle != NULL)
    {
        CloseHandle(runtime->keyboardEventHandle);
        runtime->keyboardEventHandle = NULL;
    }
    DeleteCriticalSection(&runtime->diskCriticalSection);
    DeleteCriticalSection(&runtime->keyboardCriticalSection);
}

int hyperdos_win32_pc_monitor_runtime_start_emulation_thread(hyperdos_win32_pc_monitor_runtime* runtime,
                                                             LPTHREAD_START_ROUTINE             threadProcedure)
{
    DWORD threadIdentifier = 0;

    if (runtime == NULL || threadProcedure == NULL)
    {
        return 0;
    }

    runtime->emulationThreadHandle = CreateThread(NULL, 0u, threadProcedure, runtime, 0u, &threadIdentifier);
    return runtime->emulationThreadHandle != NULL;
}

void hyperdos_win32_pc_monitor_runtime_request_render(hyperdos_win32_pc_monitor_runtime* runtime)
{
    if (runtime == NULL || runtime->callbacks.requestRender == NULL)
    {
        return;
    }
    runtime->callbacks.requestRender(runtime->callbackContext);
}

void hyperdos_win32_pc_monitor_runtime_request_reset(hyperdos_win32_pc_monitor_runtime* runtime)
{
    if (runtime == NULL || runtime->callbacks.requestReset == NULL)
    {
        return;
    }
    runtime->callbacks.requestReset(runtime->callbackContext);
}

void hyperdos_win32_pc_monitor_runtime_notify_execution_stopped(hyperdos_win32_pc_monitor_runtime* runtime)
{
    if (runtime == NULL || runtime->callbacks.notifyExecutionStopped == NULL)
    {
        return;
    }
    runtime->callbacks.notifyExecutionStopped(runtime->callbackContext);
}

void hyperdos_win32_pc_monitor_runtime_set_host_notification_from_arguments(
        hyperdos_win32_pc_monitor_runtime*          runtime,
        hyperdos_monitor_host_notification_severity severity,
        hyperdos_monitor_host_notification_category category,
        const char*                                 format,
        va_list                                     arguments)
{
    if (runtime == NULL)
    {
        return;
    }
    runtime->hostNotification.severity = severity;
    runtime->hostNotification.category = category;
    vsnprintf(runtime->hostNotification.text, sizeof(runtime->hostNotification.text), format, arguments);
}

void hyperdos_win32_pc_monitor_runtime_set_host_notification(hyperdos_win32_pc_monitor_runtime*          runtime,
                                                             hyperdos_monitor_host_notification_severity severity,
                                                             hyperdos_monitor_host_notification_category category,
                                                             const char*                                 format,
                                                             ...)
{
    va_list arguments;

    va_start(arguments, format);
    hyperdos_win32_pc_monitor_runtime_set_host_notification_from_arguments(runtime,
                                                                           severity,
                                                                           category,
                                                                           format,
                                                                           arguments);
    va_end(arguments);
}

void hyperdos_win32_pc_monitor_runtime_clear_host_notification(hyperdos_win32_pc_monitor_runtime* runtime)
{
    if (runtime == NULL)
    {
        return;
    }
    runtime->hostNotification.severity = HYPERDOS_MONITOR_HOST_NOTIFICATION_SEVERITY_INFORMATION;
    runtime->hostNotification.category = HYPERDOS_MONITOR_HOST_NOTIFICATION_CATEGORY_RUNTIME;
    runtime->hostNotification.text[0]  = '\0';
}
