#ifndef HYPERDOS_WIN32_PC_MONITOR_RUNTIME_H
#define HYPERDOS_WIN32_PC_MONITOR_RUNTIME_H

#include <windows.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "hyperdos/pc_machine.h"
#include "hyperdos/pc_storage.h"

enum
{
    HYPERDOS_MONITOR_HOST_SCAN_CODE_QUEUE_CAPACITY   = 512u,
    HYPERDOS_MONITOR_HOST_MOUSE_EVENT_QUEUE_CAPACITY = 512u,
    HYPERDOS_MONITOR_HOST_KEY_STATE_CAPACITY         = 512u,
    HYPERDOS_MONITOR_CPU_TRACE_ENTRY_COUNT           = 262144u,
    HYPERDOS_MONITOR_CPU_TRACE_BYTE_COUNT            = 6u,
    HYPERDOS_MONITOR_CPU_TRACE_STACK_WORD_COUNT      = 8u,
    HYPERDOS_MONITOR_HOST_NOTIFICATION_TEXT_CAPACITY = 256u,
    HYPERDOS_MONITOR_MAXIMUM_FLOPPY_DRIVE_COUNT      = 4u,
    HYPERDOS_MONITOR_FIXED_DISK_DRIVE_COUNT          = 2u
};

typedef struct hyperdos_monitor_cpu_trace_entry
{
    uint64_t instructionCount;
    uint16_t codeSegment;
    uint16_t instructionPointer;
    uint16_t stackSegment;
    uint16_t stackPointer;
    uint16_t dataSegment;
    uint16_t extraSegment;
    uint16_t accumulator;
    uint16_t base;
    uint16_t counter;
    uint16_t data;
    uint16_t basePointer;
    uint16_t sourceIndex;
    uint16_t destinationIndex;
    uint16_t flags;
    uint8_t  instructionBytes[HYPERDOS_MONITOR_CPU_TRACE_BYTE_COUNT];
    uint16_t stackWords[HYPERDOS_MONITOR_CPU_TRACE_STACK_WORD_COUNT];
} hyperdos_monitor_cpu_trace_entry;

typedef enum hyperdos_monitor_host_notification_severity
{
    HYPERDOS_MONITOR_HOST_NOTIFICATION_SEVERITY_INFORMATION = 0,
    HYPERDOS_MONITOR_HOST_NOTIFICATION_SEVERITY_WARNING,
    HYPERDOS_MONITOR_HOST_NOTIFICATION_SEVERITY_ERROR
} hyperdos_monitor_host_notification_severity;

typedef enum hyperdos_monitor_host_notification_category
{
    HYPERDOS_MONITOR_HOST_NOTIFICATION_CATEGORY_BOOT = 0,
    HYPERDOS_MONITOR_HOST_NOTIFICATION_CATEGORY_STORAGE,
    HYPERDOS_MONITOR_HOST_NOTIFICATION_CATEGORY_EXECUTION,
    HYPERDOS_MONITOR_HOST_NOTIFICATION_CATEGORY_MEMORY,
    HYPERDOS_MONITOR_HOST_NOTIFICATION_CATEGORY_RUNTIME
} hyperdos_monitor_host_notification_category;

typedef enum hyperdos_monitor_boot_device_kind
{
    HYPERDOS_MONITOR_BOOT_DEVICE_KIND_NONE = 0,
    HYPERDOS_MONITOR_BOOT_DEVICE_KIND_FLOPPY,
    HYPERDOS_MONITOR_BOOT_DEVICE_KIND_FIXED_DISK
} hyperdos_monitor_boot_device_kind;

typedef struct hyperdos_monitor_host_notification
{
    hyperdos_monitor_host_notification_severity severity;
    hyperdos_monitor_host_notification_category category;
    char                                        text[HYPERDOS_MONITOR_HOST_NOTIFICATION_TEXT_CAPACITY];
} hyperdos_monitor_host_notification;

typedef struct hyperdos_monitor_mouse_event
{
    int16_t horizontalMovement;
    int16_t verticalMovement;
    uint8_t buttonMask;
} hyperdos_monitor_mouse_event;

typedef struct hyperdos_win32_pc_monitor_runtime_callbacks
{
    void (*requestRender)(void* callbackContext);
    void (*requestReset)(void* callbackContext);
    void (*notifyExecutionStopped)(void* callbackContext);
} hyperdos_win32_pc_monitor_runtime_callbacks;

typedef struct hyperdos_win32_pc_monitor_runtime
{
    hyperdos_pc_machine                         machine;
    hyperdos_pc_storage_floppy_drive            floppyDrives[HYPERDOS_MONITOR_MAXIMUM_FLOPPY_DRIVE_COUNT];
    size_t                                      floppyDriveCount;
    hyperdos_pc_storage_fixed_disk_drive        fixedDiskDrives[HYPERDOS_MONITOR_FIXED_DISK_DRIVE_COUNT];
    uint8_t                                     hostScanCodeQueue[HYPERDOS_MONITOR_HOST_SCAN_CODE_QUEUE_CAPACITY];
    size_t                                      hostScanCodeQueueReadIndex;
    size_t                                      hostScanCodeQueueWriteIndex;
    hyperdos_monitor_mouse_event                hostMouseEventQueue[HYPERDOS_MONITOR_HOST_MOUSE_EVENT_QUEUE_CAPACITY];
    size_t                                      hostMouseEventQueueReadIndex;
    size_t                                      hostMouseEventQueueWriteIndex;
    size_t                                      hostMouseEventQueueCount;
    uint8_t                                     hostScanCodePressed[HYPERDOS_MONITOR_HOST_KEY_STATE_CAPACITY];
    uint64_t                                    keyboardDataAreaBufferWriteSequence;
    uint64_t                                    keyboardDataAreaShiftWriteSequence;
    uint8_t                                     hostKeyboardShiftFlags;
    CRITICAL_SECTION                            keyboardCriticalSection;
    CRITICAL_SECTION                            diskCriticalSection;
    HANDLE                                      keyboardEventHandle;
    HANDLE                                      emulationThreadHandle;
    volatile LONG                               stopRequested;
    volatile LONG                               isRunning;
    volatile LONG                               isWaitingForKeyboard;
    volatile LONG                               cpuTraceEnabled;
    ULONGLONG                                   lastTextScreenDumpTick;
    ULONGLONG                                   lastVideoStateDumpTick;
    hyperdos_monitor_cpu_trace_entry            cpuTraceEntries[HYPERDOS_MONITOR_CPU_TRACE_ENTRY_COUNT];
    hyperdos_x86_16_execution_result            executionResult;
    hyperdos_monitor_boot_device_kind           bootDeviceKind;
    uint8_t                                     bootDeviceIndex;
    hyperdos_monitor_host_notification          hostNotification;
    int                                         mouseCaptureLastClientHorizontalPosition;
    int                                         mouseCaptureLastClientVerticalPosition;
    uint8_t                                     mouseButtonMask;
    uint8_t                                     hostMouseCaptureActive;
    uint8_t                                     hostMouseRawInputRegistered;
    uint8_t                                     hostMouseRawMovementObserved;
    uint8_t                                     hostMouseCursorConfinementEnabled;
    uint8_t                                     hostMouseCursorHidingEnabled;
    uint8_t                                     mouseCaptureLastClientPositionValid;
    uint8_t                                     hostMouseCaptureToggleKeyDown;
    void*                                       callbackContext;
    hyperdos_win32_pc_monitor_runtime_callbacks callbacks;
} hyperdos_win32_pc_monitor_runtime;

typedef hyperdos_win32_pc_monitor_runtime hyperdos_win32_boot_state;

void hyperdos_win32_pc_monitor_runtime_initialize(hyperdos_win32_pc_monitor_runtime*                 runtime,
                                                  const hyperdos_win32_pc_monitor_runtime_callbacks* callbacks,
                                                  void*                                              callbackContext);

void hyperdos_win32_pc_monitor_runtime_destroy(hyperdos_win32_pc_monitor_runtime* runtime);

int hyperdos_win32_pc_monitor_runtime_start_emulation_thread(hyperdos_win32_pc_monitor_runtime* runtime,
                                                             LPTHREAD_START_ROUTINE             threadProcedure);

void hyperdos_win32_pc_monitor_runtime_stop_emulation_thread(hyperdos_win32_pc_monitor_runtime* runtime);

void hyperdos_win32_pc_monitor_runtime_close_emulation_thread_handle(hyperdos_win32_pc_monitor_runtime* runtime);

void hyperdos_win32_pc_monitor_runtime_request_render(hyperdos_win32_pc_monitor_runtime* runtime);

void hyperdos_win32_pc_monitor_runtime_request_reset(hyperdos_win32_pc_monitor_runtime* runtime);

void hyperdos_win32_pc_monitor_runtime_notify_execution_stopped(hyperdos_win32_pc_monitor_runtime* runtime);

void hyperdos_win32_pc_monitor_runtime_set_host_notification_from_arguments(
        hyperdos_win32_pc_monitor_runtime*          runtime,
        hyperdos_monitor_host_notification_severity severity,
        hyperdos_monitor_host_notification_category category,
        const char*                                 format,
        va_list                                     arguments);

void hyperdos_win32_pc_monitor_runtime_set_host_notification(hyperdos_win32_pc_monitor_runtime*          runtime,
                                                             hyperdos_monitor_host_notification_severity severity,
                                                             hyperdos_monitor_host_notification_category category,
                                                             const char*                                 format,
                                                             ...);

void hyperdos_win32_pc_monitor_runtime_clear_host_notification(hyperdos_win32_pc_monitor_runtime* runtime);

#endif
