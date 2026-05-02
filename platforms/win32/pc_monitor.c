#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <commdlg.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hyperdos/pc_bios.h"
#include "hyperdos/pc_bios_data_area.h"
#include "hyperdos/pc_bios_runtime.h"
#include "hyperdos/pc_board.h"
#include "hyperdos/pc_disk_bios.h"
#include "hyperdos/pc_disk_image.h"
#include "hyperdos/pc_floppy_controller.h"
#include "hyperdos/pc_keyboard_bios.h"
#include "hyperdos/pc_machine.h"
#include "hyperdos/pc_storage.h"
#include "hyperdos/pc_text.h"
#include "hyperdos/pc_video_services.h"

#ifndef VK_HANGUL
#define VK_HANGUL 0x15
#endif

#ifndef VK_HANJA
#define VK_HANJA 0x19
#endif

enum
{
    HYPERDOS_MONITOR_CHARACTER_WIDTH                      = 9,
    HYPERDOS_MONITOR_CHARACTER_HEIGHT                     = 16,
    HYPERDOS_MONITOR_CHARACTER_COUNT                      = 256u,
    HYPERDOS_MONITOR_WINDOW_EXTRA_WIDTH                   = 32,
    HYPERDOS_MONITOR_DISPLAY_BORDER_THICKNESS             = 1,
    HYPERDOS_MONITOR_RENDER_TIMER_IDENTIFIER              = 1u,
    HYPERDOS_MONITOR_RENDER_TIMER_PERIOD_MILLISECONDS     = 16u,
    HYPERDOS_MONITOR_TEXT_SCREEN_DUMP_PERIOD_MILLISECONDS = 250u,
    HYPERDOS_MONITOR_VIDEO_STATE_DUMP_PERIOD_MILLISECONDS = 250u,
    HYPERDOS_MONITOR_VIDEO_PREVIEW_WIDTH                  = 80u,
    HYPERDOS_MONITOR_VIDEO_PREVIEW_HEIGHT                 = 50u,
    HYPERDOS_MONITOR_INSTRUCTIONS_PER_SLICE               = 32768u,
    HYPERDOS_MONITOR_HOST_SCAN_CODE_QUEUE_CAPACITY        = 512u,
    HYPERDOS_MONITOR_HOST_MOUSE_EVENT_QUEUE_CAPACITY      = 512u,
    HYPERDOS_MONITOR_HOST_MOUSE_PACKET_MOVEMENT_MINIMUM   = -127,
    HYPERDOS_MONITOR_HOST_MOUSE_PACKET_MOVEMENT_MAXIMUM   = 127,
    HYPERDOS_MONITOR_HOST_KEY_STATE_CAPACITY              = 512u,
    HYPERDOS_MONITOR_KEYBOARD_SCAN_CODE_SEQUENCE_CAPACITY = 8u,
    HYPERDOS_MONITOR_CPU_TRACE_ENTRY_COUNT                = 262144u,
    HYPERDOS_MONITOR_CPU_TRACE_BYTE_COUNT                 = 6u,
    HYPERDOS_MONITOR_CPU_TRACE_STACK_WORD_COUNT           = 8u,
    HYPERDOS_MONITOR_UTF8_MAXIMUM_BYTE_COUNT              = 4u,
    HYPERDOS_MONITOR_MEMORY_WATCH_CAPACITY                = 32u,
    HYPERDOS_MONITOR_MEMORY_WATCH_TEXT_CAPACITY           = 64u,
    HYPERDOS_MONITOR_STATUS_TEXT_CAPACITY                 = 256u,
    HYPERDOS_MONITOR_PATH_CAPACITY                        = 1024u,
    HYPERDOS_MONITOR_FLOPPY_DRIVE_COUNT                   = 1u,
    HYPERDOS_MONITOR_MAXIMUM_FLOPPY_DRIVE_COUNT           = 4u,
    HYPERDOS_MONITOR_FIXED_DISK_DRIVE_COUNT               = 1u,
    HYPERDOS_MONITOR_GRAPHICS_PIXEL_CAPACITY              = HYPERDOS_VIDEO_GRAPHICS_ARRAY_MAXIMUM_DISPLAY_WIDTH *
                                               HYPERDOS_VIDEO_GRAPHICS_ARRAY_MAXIMUM_DISPLAY_HEIGHT,
    HYPERDOS_MONITOR_KOREAN_WINDOWS_CODE_PAGE                        = 949u,
    HYPERDOS_MONITOR_USER_RENDER_MESSAGE                             = WM_APP + 1u,
    HYPERDOS_MONITOR_USER_RESET_MESSAGE                              = WM_APP + 2u,
    HYPERDOS_MONITOR_COMMAND_INSERT_FLOPPY                           = 1001u,
    HYPERDOS_MONITOR_COMMAND_EJECT_FLOPPY                            = 1002u,
    HYPERDOS_MONITOR_COMMAND_ATTACH_HARD_DISK                        = 1003u,
    HYPERDOS_MONITOR_COMMAND_FLUSH_DISKS                             = 1004u,
    HYPERDOS_MONITOR_COMMAND_RESET_PC                                = 2001u,
    HYPERDOS_MONITOR_COMMAND_START_CPU_TRACE                         = 2002u,
    HYPERDOS_MONITOR_COMMAND_PROCESSOR_MODEL_8086                    = 2101u,
    HYPERDOS_MONITOR_COMMAND_PROCESSOR_MODEL_80186                   = 2102u,
    HYPERDOS_MONITOR_COMMAND_PC_MODEL_XT                             = 2201u,
    HYPERDOS_MONITOR_COMMAND_PC_MODEL_AT                             = 2202u,
    HYPERDOS_MONITOR_COMMAND_TEXT_CHARACTER_SET_CODE_PAGE_437        = 3001u,
    HYPERDOS_MONITOR_COMMAND_TEXT_CHARACTER_SET_KOREAN_CODE_PAGE_949 = 3002u,
    HYPERDOS_MONITOR_COMMAND_TOGGLE_MOUSE_CAPTURE                    = 3003u,
    HYPERDOS_MONITOR_COMMAND_TOGGLE_HOST_MOUSE_CURSOR_CONFINEMENT    = 3004u,
    HYPERDOS_MONITOR_COMMAND_TOGGLE_HOST_MOUSE_CURSOR_HIDING         = 3005u,
    HYPERDOS_MONITOR_COMMAND_DISPLAY_RESIZE_MODE_ORIGINAL_SIZE       = 3006u,
    HYPERDOS_MONITOR_COMMAND_DISPLAY_RESIZE_MODE_INTEGER_SCALE       = 3007u,
    HYPERDOS_MONITOR_COMMAND_DISPLAY_RESIZE_MODE_FIT_TO_WINDOW       = 3008u,
    HYPERDOS_MONITOR_HOST_EXTENDED_KEY_STATE_OFFSET                  = 0x100u,
    HYPERDOS_MONITOR_HOST_KEY_STATE_NONE                             = 0xFFFFu,
    HYPERDOS_MONITOR_RAW_INPUT_USAGE_PAGE_GENERIC                    = 0x01u,
    HYPERDOS_MONITOR_RAW_INPUT_USAGE_MOUSE                           = 0x02u,
    HYPERDOS_MONITOR_RAW_INPUT_DEVICE_INPUT_SINK                     = 0x00000100u,
    HYPERDOS_MONITOR_MOUSE_LEFT_BUTTON                               = 0x01u,
    HYPERDOS_MONITOR_MOUSE_RIGHT_BUTTON                              = 0x02u,
    HYPERDOS_MONITOR_MOUSE_MIDDLE_BUTTON                             = 0x04u
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

typedef enum hyperdos_monitor_text_character_set
{
    HYPERDOS_MONITOR_TEXT_CHARACTER_SET_CODE_PAGE_437 = 0,
    HYPERDOS_MONITOR_TEXT_CHARACTER_SET_KOREAN_CODE_PAGE_949
} hyperdos_monitor_text_character_set;

typedef enum hyperdos_monitor_display_resize_mode
{
    HYPERDOS_MONITOR_DISPLAY_RESIZE_MODE_ORIGINAL_SIZE = 0,
    HYPERDOS_MONITOR_DISPLAY_RESIZE_MODE_INTEGER_SCALE,
    HYPERDOS_MONITOR_DISPLAY_RESIZE_MODE_FIT_TO_WINDOW
} hyperdos_monitor_display_resize_mode;

typedef struct hyperdos_monitor_memory_watch
{
    uint32_t firstPhysicalAddress;
    uint32_t lastPhysicalAddress;
    char     text[HYPERDOS_MONITOR_MEMORY_WATCH_TEXT_CAPACITY];
} hyperdos_monitor_memory_watch;

typedef struct hyperdos_monitor_mouse_event
{
    int16_t horizontalMovement;
    int16_t verticalMovement;
    uint8_t buttonMask;
} hyperdos_monitor_mouse_event;

typedef struct hyperdos_win32_boot_state
{
    hyperdos_pc_machine                  machine;
    hyperdos_pc_storage_floppy_drive     floppyDrives[HYPERDOS_MONITOR_MAXIMUM_FLOPPY_DRIVE_COUNT];
    size_t                               floppyDriveCount;
    hyperdos_pc_storage_fixed_disk_drive fixedDiskDrives[HYPERDOS_MONITOR_FIXED_DISK_DRIVE_COUNT];
    uint8_t                              hostScanCodeQueue[HYPERDOS_MONITOR_HOST_SCAN_CODE_QUEUE_CAPACITY];
    size_t                               hostScanCodeQueueReadIndex;
    size_t                               hostScanCodeQueueWriteIndex;
    hyperdos_monitor_mouse_event         hostMouseEventQueue[HYPERDOS_MONITOR_HOST_MOUSE_EVENT_QUEUE_CAPACITY];
    size_t                               hostMouseEventQueueReadIndex;
    size_t                               hostMouseEventQueueWriteIndex;
    size_t                               hostMouseEventQueueCount;
    uint8_t                              hostScanCodePressed[HYPERDOS_MONITOR_HOST_KEY_STATE_CAPACITY];
    uint64_t                             keyboardDataAreaBufferWriteSequence;
    uint64_t                             keyboardDataAreaShiftWriteSequence;
    uint8_t                              hostKeyboardShiftFlags;
    CRITICAL_SECTION                     keyboardCriticalSection;
    CRITICAL_SECTION                     diskCriticalSection;
    HANDLE                               keyboardEventHandle;
    HANDLE                               emulationThreadHandle;
    HWND                                 windowHandle;
    volatile LONG                        stopRequested;
    volatile LONG                        isRunning;
    volatile LONG                        isWaitingForKeyboard;
    volatile LONG                        cpuTraceEnabled;
    ULONGLONG                            lastTextScreenDumpTick;
    ULONGLONG                            lastVideoStateDumpTick;
    hyperdos_monitor_cpu_trace_entry     cpuTraceEntries[HYPERDOS_MONITOR_CPU_TRACE_ENTRY_COUNT];
    hyperdos_x86_16_execution_result     executionResult;
    char                                 statusText[HYPERDOS_MONITOR_STATUS_TEXT_CAPACITY];
    int                                  mouseCaptureLastClientHorizontalPosition;
    int                                  mouseCaptureLastClientVerticalPosition;
    uint8_t                              mouseButtonMask;
    uint8_t                              hostMouseCaptureActive;
    uint8_t                              hostMouseRawInputRegistered;
    uint8_t                              hostMouseRawMovementObserved;
    uint8_t                              hostMouseCursorConfinementEnabled;
    uint8_t                              hostMouseCursorHidingEnabled;
    uint8_t                              mouseCaptureLastClientPositionValid;
    uint8_t                              hostMouseCaptureToggleKeyDown;
} hyperdos_win32_boot_state;

static hyperdos_win32_boot_state globalBootState;
static char   globalFloppyImagePaths[HYPERDOS_MONITOR_MAXIMUM_FLOPPY_DRIVE_COUNT][HYPERDOS_MONITOR_PATH_CAPACITY];
static size_t globalFloppyImagePathCount;
static char   globalHardDiskImagePath[HYPERDOS_MONITOR_PATH_CAPACITY];
static char   globalDiskTracePath[HYPERDOS_MONITOR_PATH_CAPACITY];
static char   globalCpuTracePath[HYPERDOS_MONITOR_PATH_CAPACITY];
static char   globalMemoryTracePath[HYPERDOS_MONITOR_PATH_CAPACITY];
static char   globalGuestMemoryDumpPath[HYPERDOS_MONITOR_PATH_CAPACITY];
static char   globalTextScreenDumpPath[HYPERDOS_MONITOR_PATH_CAPACITY];
static char   globalVideoStateDumpPath[HYPERDOS_MONITOR_PATH_CAPACITY];
static hyperdos_monitor_memory_watch   globalMemoryWatches[HYPERDOS_MONITOR_MEMORY_WATCH_CAPACITY];
static size_t                          globalMemoryWatchCount;
static uint32_t                        globalMemoryStopPhysicalAddress;
static uint8_t                         globalMemoryStopByteValue;
static int                             globalMemoryStopByteEnabled;
static int                             globalBootHardDiskWithoutFloppy;
static int                             globalCoprocessorEnabled;
static hyperdos_x86_16_processor_model globalProcessorModel;
static hyperdos_pc_model               globalPcModel;
static int                             globalDivideErrorReturnsToFaultingInstruction;
static int                             globalCpuTraceStartsEnabled;
static FILE*                           globalDiskTraceFile;
static FILE*                           globalMemoryTraceFile;
static CRITICAL_SECTION                globalDiskTraceCriticalSection;
static int                             globalDiskTraceCriticalSectionInitialized;
static HFONT                           globalTextFontHandle;
static HFONT                           globalCodePage437TextFontHandle;
static uint16_t globalCodePage437GlyphRows[HYPERDOS_MONITOR_CHARACTER_COUNT][HYPERDOS_MONITOR_CHARACTER_HEIGHT];
static int      globalCodePage437GlyphRowsInitialized;
static hyperdos_monitor_text_character_set globalTextCharacterSet = HYPERDOS_MONITOR_TEXT_CHARACTER_SET_CODE_PAGE_437;
static hyperdos_monitor_display_resize_mode
                globalDisplayResizeMode = HYPERDOS_MONITOR_DISPLAY_RESIZE_MODE_INTEGER_SCALE;
static uint32_t globalGraphicsPixels[HYPERDOS_MONITOR_GRAPHICS_PIXEL_CAPACITY];

static void write_text_screen_dump_file(const hyperdos_win32_boot_state* bootState);

static void maybe_write_text_screen_dump_file(hyperdos_win32_boot_state* bootState);

static void write_video_state_dump_file(hyperdos_win32_boot_state* bootState);

static void maybe_write_video_state_dump_file(hyperdos_win32_boot_state* bootState);

static void set_status_text(hyperdos_win32_boot_state* bootState, const char* format, ...)
{
    va_list arguments;

    va_start(arguments, format);
    vsnprintf(bootState->statusText, sizeof(bootState->statusText), format, arguments);
    va_end(arguments);
}

static void trace_disk_event(hyperdos_win32_boot_state* bootState, const char* format, ...)
{
    va_list arguments;

    if (globalDiskTraceFile == NULL)
    {
        return;
    }
    if (globalDiskTraceCriticalSectionInitialized)
    {
        EnterCriticalSection(&globalDiskTraceCriticalSection);
    }
    if (bootState != NULL)
    {
        hyperdos_x86_16_processor* processor = &bootState->machine.pc.processor;

        fprintf(globalDiskTraceFile,
                "%04X:%04X AX=%04X BX=%04X CX=%04X DX=%04X SP=%04X FLAGS=%04X ",
                processor->lastInstructionSegment,
                processor->lastInstructionOffset,
                processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR],
                processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE],
                processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER],
                processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA],
                processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER],
                processor->flags);
    }
    va_start(arguments, format);
    vfprintf(globalDiskTraceFile, format, arguments);
    va_end(arguments);
    fputc('\n', globalDiskTraceFile);
    fflush(globalDiskTraceFile);
    if (globalDiskTraceCriticalSectionInitialized)
    {
        LeaveCriticalSection(&globalDiskTraceCriticalSection);
    }
}

static void trace_monitor_event(void* userContext, const char* message)
{
    trace_disk_event((hyperdos_win32_boot_state*)userContext, "%s", message);
}

static int write_binary_file(const char* path, const uint8_t* bytes, size_t byteCount)
{
    FILE* file = fopen(path, "wb");

    if (file == NULL)
    {
        return 0;
    }
    if (byteCount != 0u && fwrite(bytes, 1u, byteCount, file) != byteCount)
    {
        fclose(file);
        return 0;
    }
    fclose(file);
    return 1;
}

static void copy_string_to_buffer(char* destination, size_t destinationSize, const char* source)
{
    if (destinationSize == 0u)
    {
        return;
    }
    if (source == NULL)
    {
        destination[0] = '\0';
        return;
    }
    snprintf(destination, destinationSize, "%s", source);
}

static int parse_memory_watch_address(const char* text, hyperdos_monitor_memory_watch* memoryWatch)
{
    char*         endPointer        = NULL;
    unsigned long parsedFirstValue  = 0u;
    unsigned long parsedSecondValue = 0u;
    unsigned long byteCount         = HYPERDOS_X86_16_WORD_SIZE;
    uint32_t      physicalAddress   = 0u;

    if (text == NULL || memoryWatch == NULL || text[0] == '\0')
    {
        return 0;
    }

    parsedFirstValue = strtoul(text, &endPointer, 16);
    if (endPointer == text)
    {
        return 0;
    }
    if (*endPointer == ':')
    {
        const char* offsetText = endPointer + 1;
        parsedSecondValue      = strtoul(offsetText, &endPointer, 16);
        if (endPointer == offsetText)
        {
            return 0;
        }
        physicalAddress = (uint32_t)((parsedFirstValue << HYPERDOS_X86_16_SEGMENT_SHIFT) +
                                     (parsedSecondValue & 0xFFFFu));
    }
    else
    {
        physicalAddress = (uint32_t)parsedFirstValue;
    }
    if (*endPointer == '+')
    {
        const char* byteCountText = endPointer + 1;
        byteCount                 = strtoul(byteCountText, &endPointer, 16);
        if (endPointer == byteCountText || byteCount == 0u)
        {
            return 0;
        }
    }
    if (*endPointer != '\0')
    {
        return 0;
    }

    memoryWatch->firstPhysicalAddress = physicalAddress & HYPERDOS_X86_16_ADDRESS_MASK;
    memoryWatch->lastPhysicalAddress  = (memoryWatch->firstPhysicalAddress + (uint32_t)(byteCount - 1u)) &
                                       HYPERDOS_X86_16_ADDRESS_MASK;
    copy_string_to_buffer(memoryWatch->text, sizeof(memoryWatch->text), text);
    return 1;
}

static void add_memory_watch_argument(const char* text)
{
    if (globalMemoryWatchCount >= HYPERDOS_MONITOR_MEMORY_WATCH_CAPACITY)
    {
        return;
    }
    if (parse_memory_watch_address(text, &globalMemoryWatches[globalMemoryWatchCount]))
    {
        ++globalMemoryWatchCount;
    }
}

static int parse_memory_stop_byte_argument(const char* text)
{
    char                          physicalAddressText[HYPERDOS_MONITOR_MEMORY_WATCH_TEXT_CAPACITY];
    hyperdos_monitor_memory_watch memoryWatch;
    const char*                   byteValueText             = NULL;
    char*                         endPointer                = NULL;
    unsigned long                 byteValue                 = 0u;
    size_t                        physicalAddressTextLength = 0u;

    if (text == NULL)
    {
        return 0;
    }
    byteValueText = strchr(text, '=');
    if (byteValueText == NULL || byteValueText == text)
    {
        return 0;
    }
    physicalAddressTextLength = (size_t)(byteValueText - text);
    if (physicalAddressTextLength >= sizeof(physicalAddressText))
    {
        return 0;
    }
    memcpy(physicalAddressText, text, physicalAddressTextLength);
    physicalAddressText[physicalAddressTextLength] = '\0';

    ++byteValueText;
    byteValue = strtoul(byteValueText, &endPointer, 16);
    if (endPointer == byteValueText || *endPointer != '\0' || byteValue > 0xFFu)
    {
        return 0;
    }
    if (!parse_memory_watch_address(physicalAddressText, &memoryWatch))
    {
        return 0;
    }

    globalMemoryStopPhysicalAddress = memoryWatch.firstPhysicalAddress;
    globalMemoryStopByteValue       = (uint8_t)byteValue;
    globalMemoryStopByteEnabled     = 1;
    return 1;
}

static const char* find_file_name_from_path(const char* path)
{
    const char* fileName  = path;
    const char* character = path;

    while (character != NULL && *character != '\0')
    {
        if (*character == '\\' || *character == '/')
        {
            fileName = character + 1;
        }
        ++character;
    }
    return fileName;
}

static uint8_t read_floppy_controller_input_output_byte(void* device, uint16_t port)
{
    hyperdos_win32_boot_state* bootState = (hyperdos_win32_boot_state*)device;
    uint8_t                    value     = 0xFFu;

    if (bootState == NULL)
    {
        return 0xFFu;
    }
    EnterCriticalSection(&bootState->diskCriticalSection);
    value = hyperdos_pc_floppy_controller_read_input_output_byte(&bootState->machine.floppyController, port);
    LeaveCriticalSection(&bootState->diskCriticalSection);
    return value;
}

static void write_floppy_controller_input_output_byte(void* device, uint16_t port, uint8_t value)
{
    hyperdos_win32_boot_state* bootState = (hyperdos_win32_boot_state*)device;

    if (bootState == NULL)
    {
        return;
    }
    EnterCriticalSection(&bootState->diskCriticalSection);
    hyperdos_pc_floppy_controller_write_input_output_byte(&bootState->machine.floppyController, port, value);
    LeaveCriticalSection(&bootState->diskCriticalSection);
}

static void initialize_storage_context_for_boot_state(hyperdos_win32_boot_state*   bootState,
                                                      hyperdos_pc_storage_context* storageContext)
{
    hyperdos_pc_storage_context_initialize(storageContext,
                                           &bootState->machine.pc,
                                           &bootState->machine.floppyController,
                                           bootState->floppyDrives,
                                           bootState->floppyDriveCount,
                                           bootState->fixedDiskDrives,
                                           HYPERDOS_MONITOR_FIXED_DISK_DRIVE_COUNT,
                                           trace_monitor_event,
                                           bootState);
}

static int flush_all_disk_images(hyperdos_win32_boot_state* bootState)
{
    hyperdos_pc_storage_context storageContext;
    int                         flushed = 1;

    initialize_storage_context_for_boot_state(bootState, &storageContext);
    EnterCriticalSection(&bootState->diskCriticalSection);
    flushed = hyperdos_pc_storage_flush_all_disk_images(&storageContext);
    LeaveCriticalSection(&bootState->diskCriticalSection);
    return flushed;
}

static hyperdos_pc_disk_image* get_floppy_disk_locked(hyperdos_win32_boot_state* bootState, uint8_t driveNumber)
{
    hyperdos_pc_storage_context storageContext;

    initialize_storage_context_for_boot_state(bootState, &storageContext);
    return hyperdos_pc_storage_get_floppy_disk_for_drive_number(&storageContext, driveNumber);
}

static hyperdos_pc_disk_image* get_disk_for_drive_number_locked(hyperdos_win32_boot_state* bootState,
                                                                uint8_t                    driveNumber)
{
    hyperdos_pc_storage_context storageContext;

    initialize_storage_context_for_boot_state(bootState, &storageContext);
    return hyperdos_pc_storage_get_disk_for_bios_drive_number(&storageContext, driveNumber);
}

static void lock_disk_images_for_bios(void* userContext)
{
    EnterCriticalSection(&((hyperdos_win32_boot_state*)userContext)->diskCriticalSection);
}

static void unlock_disk_images_for_bios(void* userContext)
{
    LeaveCriticalSection(&((hyperdos_win32_boot_state*)userContext)->diskCriticalSection);
}

static hyperdos_pc_disk_image* get_disk_image_for_bios(void* userContext, uint8_t driveNumber)
{
    return get_disk_for_drive_number_locked((hyperdos_win32_boot_state*)userContext, driveNumber);
}

static int push_host_scan_code(hyperdos_win32_boot_state* bootState, uint8_t scanCode)
{
    size_t nextWriteIndex = 0u;
    int    pushed         = 0;

    EnterCriticalSection(&bootState->keyboardCriticalSection);
    nextWriteIndex = (bootState->hostScanCodeQueueWriteIndex + 1u) % HYPERDOS_MONITOR_HOST_SCAN_CODE_QUEUE_CAPACITY;
    if (nextWriteIndex != bootState->hostScanCodeQueueReadIndex)
    {
        bootState->hostScanCodeQueue[bootState->hostScanCodeQueueWriteIndex] = scanCode;
        bootState->hostScanCodeQueueWriteIndex                               = nextWriteIndex;
        pushed                                                               = 1;
    }
    LeaveCriticalSection(&bootState->keyboardCriticalSection);

    if (pushed)
    {
        InterlockedExchange(&bootState->isWaitingForKeyboard, 0);
        SetEvent(bootState->keyboardEventHandle);
    }
    return pushed;
}

static int peek_host_scan_code(hyperdos_win32_boot_state* bootState, uint8_t* keyboardScanCode)
{
    int found = 0;

    if (keyboardScanCode == NULL)
    {
        return 0;
    }

    EnterCriticalSection(&bootState->keyboardCriticalSection);
    if (bootState->hostScanCodeQueueReadIndex != bootState->hostScanCodeQueueWriteIndex)
    {
        *keyboardScanCode = bootState->hostScanCodeQueue[bootState->hostScanCodeQueueReadIndex];
        found             = 1;
    }
    LeaveCriticalSection(&bootState->keyboardCriticalSection);
    return found;
}

static void consume_host_scan_code(hyperdos_win32_boot_state* bootState)
{
    EnterCriticalSection(&bootState->keyboardCriticalSection);
    if (bootState->hostScanCodeQueueReadIndex != bootState->hostScanCodeQueueWriteIndex)
    {
        bootState->hostScanCodeQueueReadIndex = (bootState->hostScanCodeQueueReadIndex + 1u) %
                                                HYPERDOS_MONITOR_HOST_SCAN_CODE_QUEUE_CAPACITY;
    }
    LeaveCriticalSection(&bootState->keyboardCriticalSection);
}

static int push_host_mouse_event(hyperdos_win32_boot_state* bootState,
                                 int16_t                    horizontalMovement,
                                 int16_t                    verticalMovement,
                                 uint8_t                    buttonMask,
                                 int                        forcePacket)
{
    hyperdos_monitor_mouse_event mouseEvent;
    int                          eventQueued     = 0;
    int                          movementSkipped = 0;

    mouseEvent.horizontalMovement = horizontalMovement;
    mouseEvent.verticalMovement   = verticalMovement;
    mouseEvent.buttonMask         = buttonMask;
    EnterCriticalSection(&bootState->keyboardCriticalSection);
    if (forcePacket == 0 && bootState->hostMouseEventQueueCount != 0u)
    {
        movementSkipped = 1;
    }
    else if (bootState->hostMouseEventQueueCount < HYPERDOS_MONITOR_HOST_MOUSE_EVENT_QUEUE_CAPACITY)
    {
        bootState->hostMouseEventQueue[bootState->hostMouseEventQueueWriteIndex] = mouseEvent;
        bootState->hostMouseEventQueueWriteIndex = (bootState->hostMouseEventQueueWriteIndex + 1u) %
                                                   HYPERDOS_MONITOR_HOST_MOUSE_EVENT_QUEUE_CAPACITY;
        ++bootState->hostMouseEventQueueCount;
        eventQueued = 1;
    }
    LeaveCriticalSection(&bootState->keyboardCriticalSection);

    if (movementSkipped)
    {
        return 1;
    }
    if (!eventQueued)
    {
        return 0;
    }
    InterlockedExchange(&bootState->isWaitingForKeyboard, 0);
    SetEvent(bootState->keyboardEventHandle);
    return 1;
}

static int peek_host_mouse_event(hyperdos_win32_boot_state* bootState, hyperdos_monitor_mouse_event* mouseEvent)
{
    int found = 0;

    if (mouseEvent == NULL)
    {
        return 0;
    }

    EnterCriticalSection(&bootState->keyboardCriticalSection);
    if (bootState->hostMouseEventQueueCount != 0u)
    {
        *mouseEvent = bootState->hostMouseEventQueue[bootState->hostMouseEventQueueReadIndex];
        found       = 1;
    }
    LeaveCriticalSection(&bootState->keyboardCriticalSection);
    return found;
}

static void consume_host_mouse_event(hyperdos_win32_boot_state* bootState)
{
    EnterCriticalSection(&bootState->keyboardCriticalSection);
    if (bootState->hostMouseEventQueueCount != 0u)
    {
        bootState->hostMouseEventQueueReadIndex = (bootState->hostMouseEventQueueReadIndex + 1u) %
                                                  HYPERDOS_MONITOR_HOST_MOUSE_EVENT_QUEUE_CAPACITY;
        --bootState->hostMouseEventQueueCount;
    }
    LeaveCriticalSection(&bootState->keyboardCriticalSection);
}

static void update_host_keyboard_shift_flags_from_scan_code(hyperdos_win32_boot_state* bootState,
                                                            uint8_t                    keyboardScanCode)
{
    bootState->hostKeyboardShiftFlags =
            hyperdos_pc_keyboard_bios_update_shift_flags_from_scan_code(bootState->hostKeyboardShiftFlags,
                                                                        keyboardScanCode);
}

static uint8_t make_keyboard_scan_code_from_virtual_key(WPARAM virtualKey)
{
    if (virtualKey >= 'A' && virtualKey <= 'Z')
    {
        static const uint8_t letterScanCodes[26] = {0x1Eu, 0x30u, 0x2Eu, 0x20u, 0x12u, 0x21u, 0x22u, 0x23u, 0x17u,
                                                    0x24u, 0x25u, 0x26u, 0x32u, 0x31u, 0x18u, 0x19u, 0x10u, 0x13u,
                                                    0x1Fu, 0x14u, 0x16u, 0x2Fu, 0x11u, 0x2Du, 0x15u, 0x2Cu};
        return letterScanCodes[virtualKey - 'A'];
    }
    if (virtualKey >= '0' && virtualKey <= '9')
    {
        static const uint8_t digitScanCodes[10] =
                {0x0Bu, 0x02u, 0x03u, 0x04u, 0x05u, 0x06u, 0x07u, 0x08u, 0x09u, 0x0Au};
        return digitScanCodes[virtualKey - '0'];
    }
    if (virtualKey >= VK_F1 && virtualKey <= VK_F10)
    {
        return (uint8_t)(0x3Bu + (virtualKey - VK_F1));
    }
    if (virtualKey == VK_F11)
    {
        return 0x57u;
    }
    if (virtualKey == VK_F12)
    {
        return 0x58u;
    }

    switch (virtualKey)
    {
    case VK_ESCAPE:
        return 0x01u;
    case VK_BACK:
        return 0x0Eu;
    case VK_TAB:
        return 0x0Fu;
    case VK_RETURN:
        return 0x1Cu;
    case VK_CONTROL:
    case VK_LCONTROL:
    case VK_RCONTROL:
        return 0x1Du;
    case VK_SHIFT:
    case VK_LSHIFT:
        return 0x2Au;
    case VK_RSHIFT:
        return 0x36u;
    case VK_MENU:
    case VK_LMENU:
    case VK_RMENU:
        return 0x38u;
    case VK_SPACE:
        return 0x39u;
    case VK_CAPITAL:
        return 0x3Au;
    case VK_NUMLOCK:
        return 0x45u;
    case VK_SCROLL:
        return 0x46u;
    case VK_HOME:
        return 0x47u;
    case VK_UP:
        return 0x48u;
    case VK_PRIOR:
        return 0x49u;
    case VK_LEFT:
        return 0x4Bu;
    case VK_RIGHT:
        return 0x4Du;
    case VK_END:
        return 0x4Fu;
    case VK_DOWN:
        return 0x50u;
    case VK_NEXT:
        return 0x51u;
    case VK_INSERT:
        return 0x52u;
    case VK_DELETE:
        return 0x53u;
    case VK_OEM_3:
        return 0x29u;
    case VK_OEM_MINUS:
        return 0x0Cu;
    case VK_OEM_PLUS:
        return 0x0Du;
    case VK_OEM_4:
        return 0x1Au;
    case VK_OEM_6:
        return 0x1Bu;
    case VK_OEM_5:
        return 0x2Bu;
    case VK_OEM_1:
        return 0x27u;
    case VK_OEM_7:
        return 0x28u;
    case VK_OEM_COMMA:
        return 0x33u;
    case VK_OEM_PERIOD:
        return 0x34u;
    case VK_OEM_2:
        return 0x35u;
    default:
        break;
    }
    return 0u;
}

static int keyboard_scan_code_sequence_is_repeatable(const uint8_t* scanCodeBytes, size_t scanCodeByteCount)
{
    size_t  scanCodeByteIndex = 0u;
    uint8_t keyboardScanCode  = 0u;

    if (scanCodeBytes == NULL || scanCodeByteCount == 0u || scanCodeByteCount > 2u ||
        scanCodeBytes[0] == HYPERDOS_PC_KEYBOARD_BIOS_PAUSE_SCAN_CODE_PREFIX)
    {
        return 0;
    }
    for (scanCodeByteIndex = 0u; scanCodeByteIndex < scanCodeByteCount; ++scanCodeByteIndex)
    {
        if (scanCodeBytes[scanCodeByteIndex] != HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_SCAN_CODE_PREFIX &&
            scanCodeBytes[scanCodeByteIndex] != HYPERDOS_PC_KEYBOARD_BIOS_PAUSE_SCAN_CODE_PREFIX)
        {
            keyboardScanCode = scanCodeBytes[scanCodeByteIndex];
        }
    }
    return keyboardScanCode != 0u && !hyperdos_pc_keyboard_bios_scan_code_is_modifier_or_toggle(keyboardScanCode);
}

static void update_host_keyboard_shift_flags_from_scan_code_sequence(hyperdos_win32_boot_state* bootState,
                                                                     const uint8_t*             scanCodeBytes,
                                                                     size_t                     scanCodeByteCount)
{
    size_t scanCodeByteIndex = 0u;

    for (scanCodeByteIndex = 0u; scanCodeByteIndex < scanCodeByteCount; ++scanCodeByteIndex)
    {
        if (scanCodeBytes[scanCodeByteIndex] != HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_SCAN_CODE_PREFIX &&
            scanCodeBytes[scanCodeByteIndex] != HYPERDOS_PC_KEYBOARD_BIOS_PAUSE_SCAN_CODE_PREFIX)
        {
            update_host_keyboard_shift_flags_from_scan_code(bootState, scanCodeBytes[scanCodeByteIndex]);
        }
    }
}

static size_t write_keyboard_scan_code_sequence(uint8_t  keyboardScanCode,
                                                int      extendedKey,
                                                int      released,
                                                uint8_t* scanCodeBytes,
                                                size_t   scanCodeByteCapacity)
{
    size_t scanCodeByteCount = extendedKey ? 2u : 1u;

    if (scanCodeBytes == NULL || scanCodeByteCapacity < scanCodeByteCount)
    {
        return 0u;
    }
    if (extendedKey)
    {
        scanCodeBytes[0] = HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_SCAN_CODE_PREFIX;
        scanCodeBytes[1] = keyboardScanCode;
    }
    else
    {
        scanCodeBytes[0] = keyboardScanCode;
    }
    if (released)
    {
        scanCodeBytes[scanCodeByteCount - 1u] = (uint8_t)(scanCodeBytes[scanCodeByteCount - 1u] |
                                                          HYPERDOS_PC_KEYBOARD_BIOS_RELEASE_MASK);
    }
    return scanCodeByteCount;
}

static size_t write_print_screen_scan_code_sequence(int released, uint8_t* scanCodeBytes, size_t scanCodeByteCapacity)
{
    if (scanCodeBytes == NULL || scanCodeByteCapacity < 4u)
    {
        return 0u;
    }
    scanCodeBytes[0] = HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_SCAN_CODE_PREFIX;
    scanCodeBytes[1] = (uint8_t)(released ? 0xB7u : 0x2Au);
    scanCodeBytes[2] = HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_SCAN_CODE_PREFIX;
    scanCodeBytes[3] = (uint8_t)(released ? 0xAAu : 0x37u);
    return 4u;
}

static size_t make_keyboard_scan_code_sequence_from_host_key_state_index(uint16_t hostKeyStateIndex,
                                                                         uint8_t* scanCodeBytes,
                                                                         size_t   scanCodeByteCapacity)
{
    int     extendedKey      = hostKeyStateIndex >= HYPERDOS_MONITOR_HOST_EXTENDED_KEY_STATE_OFFSET;
    uint8_t keyboardScanCode = (uint8_t)(hostKeyStateIndex & HYPERDOS_X86_16_LOW_BYTE_MASK);

    if (extendedKey && keyboardScanCode == 0x37u)
    {
        return write_print_screen_scan_code_sequence(1, scanCodeBytes, scanCodeByteCapacity);
    }
    return write_keyboard_scan_code_sequence(keyboardScanCode, extendedKey, 1, scanCodeBytes, scanCodeByteCapacity);
}

static uint16_t make_host_key_state_index(uint8_t keyboardScanCode, int extendedKey)
{
    return (uint16_t)(keyboardScanCode + (extendedKey ? HYPERDOS_MONITOR_HOST_EXTENDED_KEY_STATE_OFFSET : 0u));
}

static size_t make_keyboard_scan_code_sequence_from_window_key_message(WPARAM    virtualKey,
                                                                       LPARAM    longParameter,
                                                                       int       released,
                                                                       uint8_t*  scanCodeBytes,
                                                                       size_t    scanCodeByteCapacity,
                                                                       uint16_t* hostKeyStateIndex)
{
    uint8_t keyboardScanCode = (uint8_t)(((uint32_t)longParameter >> 16u) & 0xFFu);
    int     extendedKey      = (((uint32_t)longParameter & 0x01000000u) != 0u);

    if (hostKeyStateIndex != NULL)
    {
        *hostKeyStateIndex = HYPERDOS_MONITOR_HOST_KEY_STATE_NONE;
    }
    if (scanCodeBytes == NULL)
    {
        return 0u;
    }
    if (virtualKey == VK_PAUSE)
    {
        if (released || scanCodeByteCapacity < 6u)
        {
            return 0u;
        }
        scanCodeBytes[0] = HYPERDOS_PC_KEYBOARD_BIOS_PAUSE_SCAN_CODE_PREFIX;
        scanCodeBytes[1] = 0x1Du;
        scanCodeBytes[2] = 0x45u;
        scanCodeBytes[3] = HYPERDOS_PC_KEYBOARD_BIOS_PAUSE_SCAN_CODE_PREFIX;
        scanCodeBytes[4] = 0x9Du;
        scanCodeBytes[5] = 0xC5u;
        return 6u;
    }
    if (virtualKey == VK_SNAPSHOT)
    {
        if (hostKeyStateIndex != NULL)
        {
            *hostKeyStateIndex = make_host_key_state_index(0x37u, 1);
        }
        return write_print_screen_scan_code_sequence(released, scanCodeBytes, scanCodeByteCapacity);
    }
    if (virtualKey == VK_HANGUL)
    {
        keyboardScanCode = 0x38u;
        extendedKey      = 1;
    }
    else if (virtualKey == VK_HANJA)
    {
        keyboardScanCode = 0x1Du;
        extendedKey      = 1;
    }
    else if (keyboardScanCode == 0u || keyboardScanCode >= HYPERDOS_PC_KEYBOARD_BIOS_RELEASE_MASK)
    {
        keyboardScanCode = make_keyboard_scan_code_from_virtual_key(virtualKey);
        extendedKey      = 0;
    }
    if (keyboardScanCode == 0u || keyboardScanCode >= HYPERDOS_PC_KEYBOARD_BIOS_RELEASE_MASK)
    {
        return 0u;
    }
    if (hostKeyStateIndex != NULL)
    {
        *hostKeyStateIndex = make_host_key_state_index(keyboardScanCode, extendedKey);
    }
    return write_keyboard_scan_code_sequence(keyboardScanCode,
                                             extendedKey,
                                             released,
                                             scanCodeBytes,
                                             scanCodeByteCapacity);
}

static void push_host_scan_code_sequence(hyperdos_win32_boot_state* bootState,
                                         const uint8_t*             scanCodeBytes,
                                         size_t                     scanCodeByteCount)
{
    size_t scanCodeByteIndex = 0u;

    for (scanCodeByteIndex = 0u; scanCodeByteIndex < scanCodeByteCount; ++scanCodeByteIndex)
    {
        push_host_scan_code(bootState, scanCodeBytes[scanCodeByteIndex]);
    }
}

static void push_host_key_press(hyperdos_win32_boot_state* bootState,
                                const uint8_t*             scanCodeBytes,
                                size_t                     scanCodeByteCount,
                                uint16_t                   hostKeyStateIndex,
                                int                        previousKeyWasDown)
{
    int keyAlreadyPressed = 0;
    int repeatable        = keyboard_scan_code_sequence_is_repeatable(scanCodeBytes, scanCodeByteCount);

    if (scanCodeBytes == NULL || scanCodeByteCount == 0u)
    {
        return;
    }
    if (hostKeyStateIndex < HYPERDOS_MONITOR_HOST_KEY_STATE_CAPACITY)
    {
        keyAlreadyPressed = bootState->hostScanCodePressed[hostKeyStateIndex] != 0u;
        if ((keyAlreadyPressed || previousKeyWasDown) && !repeatable)
        {
            trace_disk_event(bootState,
                             "host keyboard ignored repeated press key-state=%04X already-pressed=%u previous-down=%u",
                             hostKeyStateIndex,
                             keyAlreadyPressed != 0,
                             previousKeyWasDown != 0);
            return;
        }
        bootState->hostScanCodePressed[hostKeyStateIndex] = 1u;
    }
    else if (previousKeyWasDown && !repeatable)
    {
        trace_disk_event(bootState, "host keyboard ignored repeated press key-state=none previous-down=1");
        return;
    }
    update_host_keyboard_shift_flags_from_scan_code_sequence(bootState, scanCodeBytes, scanCodeByteCount);
    trace_disk_event(bootState,
                     "host keyboard press key-state=%04X byte-count=%zu",
                     hostKeyStateIndex,
                     scanCodeByteCount);
    push_host_scan_code_sequence(bootState, scanCodeBytes, scanCodeByteCount);
}

static void push_host_key_release(hyperdos_win32_boot_state* bootState,
                                  const uint8_t*             scanCodeBytes,
                                  size_t                     scanCodeByteCount,
                                  uint16_t                   hostKeyStateIndex)
{
    if (scanCodeBytes == NULL || scanCodeByteCount == 0u)
    {
        return;
    }
    if (hostKeyStateIndex < HYPERDOS_MONITOR_HOST_KEY_STATE_CAPACITY)
    {
        bootState->hostScanCodePressed[hostKeyStateIndex] = 0u;
    }
    update_host_keyboard_shift_flags_from_scan_code_sequence(bootState, scanCodeBytes, scanCodeByteCount);
    trace_disk_event(bootState,
                     "host keyboard release key-state=%04X byte-count=%zu",
                     hostKeyStateIndex,
                     scanCodeByteCount);
    push_host_scan_code_sequence(bootState, scanCodeBytes, scanCodeByteCount);
}

static void release_pressed_host_keys(hyperdos_win32_boot_state* bootState)
{
    uint16_t pressedKeyStateIndexes[HYPERDOS_MONITOR_HOST_KEY_STATE_CAPACITY];
    size_t   pressedKeyStateCount = 0u;
    size_t   hostKeyStateIndex    = 0u;
    size_t   pressedKeyStateIndex = 0u;

    for (hostKeyStateIndex = 0u; hostKeyStateIndex < HYPERDOS_MONITOR_HOST_KEY_STATE_CAPACITY; ++hostKeyStateIndex)
    {
        if (bootState->hostScanCodePressed[hostKeyStateIndex] != 0u)
        {
            pressedKeyStateIndexes[pressedKeyStateCount] = (uint16_t)hostKeyStateIndex;
            ++pressedKeyStateCount;
            bootState->hostScanCodePressed[hostKeyStateIndex] = 0u;
        }
    }
    for (pressedKeyStateIndex = 0u; pressedKeyStateIndex < pressedKeyStateCount; ++pressedKeyStateIndex)
    {
        uint8_t scanCodeBytes[HYPERDOS_MONITOR_KEYBOARD_SCAN_CODE_SEQUENCE_CAPACITY];
        size_t  scanCodeByteCount =
                make_keyboard_scan_code_sequence_from_host_key_state_index(pressedKeyStateIndexes[pressedKeyStateIndex],
                                                                           scanCodeBytes,
                                                                           sizeof(scanCodeBytes));

        update_host_keyboard_shift_flags_from_scan_code_sequence(bootState, scanCodeBytes, scanCodeByteCount);
        push_host_scan_code_sequence(bootState, scanCodeBytes, scanCodeByteCount);
    }
    if (pressedKeyStateCount != 0u)
    {
        trace_disk_event(bootState, "keyboard focus lost released-pressed-count=%zu", pressedKeyStateCount);
    }
}

static void drain_host_scan_codes_to_keyboard_controller(hyperdos_win32_boot_state* bootState)
{
    uint8_t keyboardScanCode = 0u;

    while (peek_host_scan_code(bootState, &keyboardScanCode))
    {
        if (!hyperdos_intel_8042_keyboard_controller_can_receive_scan_code(&bootState->machine.pc.keyboardController))
        {
            break;
        }
        if (!hyperdos_intel_8042_keyboard_controller_receive_scan_code(&bootState->machine.pc.keyboardController,
                                                                       keyboardScanCode))
        {
            trace_disk_event(bootState,
                             "keyboard controller rejected scan-code=%02X command-byte=%02X scanning=%u "
                             "output-queue=%zu",
                             keyboardScanCode,
                             bootState->machine.pc.keyboardController.commandByte,
                             bootState->machine.pc.keyboardController.keyboardScanningEnabled,
                             bootState->machine.pc.keyboardController.outputQueueCount);
            break;
        }
        consume_host_scan_code(bootState);
        trace_disk_event(bootState,
                         "keyboard controller received scan-code=%02X command-byte=%02X output-queue=%zu "
                         "interrupt-pending=%u",
                         keyboardScanCode,
                         bootState->machine.pc.keyboardController.commandByte,
                         bootState->machine.pc.keyboardController.outputQueueCount,
                         bootState->machine.pc.keyboardController.keyboardInterruptRequestPending);
    }
}

static void drain_host_mouse_events_to_keyboard_controller(hyperdos_win32_boot_state* bootState)
{
    hyperdos_intel_8042_keyboard_controller* keyboardController = &bootState->machine.pc.keyboardController;
    hyperdos_monitor_mouse_event             mouseEvent;

    while (peek_host_mouse_event(bootState, &mouseEvent))
    {
        if (keyboardController->outputQueueCount != 0u)
        {
            break;
        }
        if (!hyperdos_intel_8042_keyboard_controller_receive_auxiliary_mouse_packet(keyboardController,
                                                                                    mouseEvent.horizontalMovement,
                                                                                    mouseEvent.verticalMovement,
                                                                                    mouseEvent.buttonMask))
        {
            break;
        }
        consume_host_mouse_event(bootState);
    }
}

static void drain_keyboard_input_for_bios_runtime(void* userContext)
{
    hyperdos_win32_boot_state* bootState = (hyperdos_win32_boot_state*)userContext;

    drain_host_scan_codes_to_keyboard_controller(bootState);
    drain_host_mouse_events_to_keyboard_controller(bootState);
}

static void refresh_keyboard_input_state(hyperdos_win32_boot_state* bootState)
{
    drain_host_scan_codes_to_keyboard_controller(bootState);
    drain_host_mouse_events_to_keyboard_controller(bootState);
    if (hyperdos_pc_bios_keyboard_hardware_interrupt_vector_is_default(&bootState->machine.pc))
    {
        (void)hyperdos_pc_keyboard_bios_service_hardware_byte(&bootState->machine.keyboardBios,
                                                              &bootState->machine.keyboardBiosInterface,
                                                              &bootState->machine.pc,
                                                              0u,
                                                              0,
                                                              trace_monitor_event,
                                                              bootState);
    }
}

static uint8_t read_guest_memory_byte(hyperdos_win32_boot_state* bootState, uint32_t physicalAddress)
{
    return hyperdos_bus_read_memory_byte_or_open_bus(&bootState->machine.pc.bus,
                                                     physicalAddress & HYPERDOS_X86_16_ADDRESS_MASK);
}

static uint8_t read_guest_instruction_byte(hyperdos_win32_boot_state* bootState, uint16_t segment, uint16_t offset)
{
    uint32_t physicalAddress = (((uint32_t)segment << HYPERDOS_X86_16_SEGMENT_SHIFT) + offset) &
                               HYPERDOS_X86_16_ADDRESS_MASK;

    return read_guest_memory_byte(bootState, physicalAddress);
}

static uint16_t read_guest_memory_word(hyperdos_win32_boot_state* bootState, uint32_t physicalAddress)
{
    uint16_t lowByte  = read_guest_memory_byte(bootState, physicalAddress);
    uint16_t highByte = read_guest_memory_byte(bootState, physicalAddress + 1u);

    return (uint16_t)(lowByte | (highByte << HYPERDOS_X86_16_BYTE_BIT_COUNT));
}

static int memory_watch_contains_address(const hyperdos_monitor_memory_watch* memoryWatch, uint32_t physicalAddress)
{
    if (memoryWatch->firstPhysicalAddress <= memoryWatch->lastPhysicalAddress)
    {
        return physicalAddress >= memoryWatch->firstPhysicalAddress &&
               physicalAddress <= memoryWatch->lastPhysicalAddress;
    }
    return physicalAddress >= memoryWatch->firstPhysicalAddress || physicalAddress <= memoryWatch->lastPhysicalAddress;
}

static int physical_address_is_keyboard_shift_data_area(uint32_t physicalAddress)
{
    uint32_t keyboardDataAreaStart = HYPERDOS_PC_BIOS_DATA_AREA_BASE_ADDRESS +
                                     HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_SHIFT_FLAGS_OFFSET;
    uint32_t keyboardDataAreaEnd = keyboardDataAreaStart + 1u;

    return physicalAddress >= keyboardDataAreaStart && physicalAddress <= keyboardDataAreaEnd;
}

static int physical_address_is_keyboard_buffer_data_area(uint32_t physicalAddress)
{
    uint32_t keyboardDataAreaStart = HYPERDOS_PC_BIOS_DATA_AREA_BASE_ADDRESS +
                                     HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_HEAD_OFFSET;
    uint32_t keyboardDataAreaEnd = HYPERDOS_PC_BIOS_DATA_AREA_BASE_ADDRESS +
                                   HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_BUFFER_END - 1u;

    return physicalAddress >= keyboardDataAreaStart && physicalAddress <= keyboardDataAreaEnd;
}

static void observe_guest_memory_write(void*    observerContext,
                                       uint32_t physicalAddress,
                                       uint8_t  oldValue,
                                       uint8_t  newValue)
{
    hyperdos_win32_boot_state* bootState        = (hyperdos_win32_boot_state*)observerContext;
    hyperdos_x86_16_processor* processor        = NULL;
    size_t                     memoryWatchIndex = 0u;

    if (bootState == NULL)
    {
        return;
    }
    processor        = &bootState->machine.pc.processor;
    physicalAddress &= HYPERDOS_X86_16_ADDRESS_MASK;
    if (physical_address_is_keyboard_shift_data_area(physicalAddress))
    {
        ++bootState->keyboardDataAreaShiftWriteSequence;
        trace_disk_event(bootState,
                         "keyboard shift data area write physical=%05X old=%02X new=%02X sequence=%llu",
                         physicalAddress,
                         oldValue,
                         newValue,
                         (unsigned long long)bootState->keyboardDataAreaShiftWriteSequence);
    }
    if (physical_address_is_keyboard_buffer_data_area(physicalAddress))
    {
        ++bootState->keyboardDataAreaBufferWriteSequence;
        trace_disk_event(bootState,
                         "keyboard buffer data area write physical=%05X old=%02X new=%02X sequence=%llu",
                         physicalAddress,
                         oldValue,
                         newValue,
                         (unsigned long long)bootState->keyboardDataAreaBufferWriteSequence);
    }
    if (globalMemoryTraceFile != NULL && globalMemoryWatchCount != 0u)
    {
        for (memoryWatchIndex = 0u; memoryWatchIndex < globalMemoryWatchCount; ++memoryWatchIndex)
        {
            const hyperdos_monitor_memory_watch* memoryWatch = &globalMemoryWatches[memoryWatchIndex];
            if (!memory_watch_contains_address(memoryWatch, physicalAddress))
            {
                continue;
            }

            fprintf(globalMemoryTraceFile,
                    "%llu %04X:%04X opcode=%02X write physical=%05X old=%02X new=%02X "
                    "watch=%s word0=%04X word2=%04X "
                    "AX=%04X BX=%04X CX=%04X DX=%04X BP=%04X SI=%04X DI=%04X "
                    "DS=%04X ES=%04X SS:SP=%04X:%04X source-physical=%05X destination-physical=%05X FLAGS=%04X "
                    "SR02=%02X SR04=%02X GC00=%02X GC01=%02X GC03=%02X GC04=%02X GC05=%02X GC06=%02X GC08=%02X\n",
                    (unsigned long long)processor->executedInstructionCount,
                    processor->lastInstructionSegment,
                    processor->lastInstructionOffset,
                    processor->lastOperationCode,
                    physicalAddress,
                    oldValue,
                    newValue,
                    memoryWatch->text,
                    read_guest_memory_word(bootState, memoryWatch->firstPhysicalAddress),
                    read_guest_memory_word(bootState, memoryWatch->firstPhysicalAddress + HYPERDOS_X86_16_WORD_SIZE),
                    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR],
                    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE],
                    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER],
                    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA],
                    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE_POINTER],
                    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_SOURCE_INDEX],
                    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DESTINATION_INDEX],
                    processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_DATA],
                    processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA],
                    processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_STACK],
                    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER],
                    (((uint32_t)processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_DATA]
                      << HYPERDOS_X86_16_SEGMENT_SHIFT) +
                     processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_SOURCE_INDEX]) &
                            HYPERDOS_X86_16_ADDRESS_MASK,
                    (((uint32_t)processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA]
                      << HYPERDOS_X86_16_SEGMENT_SHIFT) +
                     processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DESTINATION_INDEX]) &
                            HYPERDOS_X86_16_ADDRESS_MASK,
                    processor->flags,
                    bootState->machine.pc.colorGraphicsAdapter.sequencerRegisters[2],
                    bootState->machine.pc.colorGraphicsAdapter.sequencerRegisters[4],
                    bootState->machine.pc.colorGraphicsAdapter.graphicsControllerRegisters[0],
                    bootState->machine.pc.colorGraphicsAdapter.graphicsControllerRegisters[1],
                    bootState->machine.pc.colorGraphicsAdapter.graphicsControllerRegisters[3],
                    bootState->machine.pc.colorGraphicsAdapter.graphicsControllerRegisters[4],
                    bootState->machine.pc.colorGraphicsAdapter.graphicsControllerRegisters[5],
                    bootState->machine.pc.colorGraphicsAdapter.graphicsControllerRegisters[6],
                    bootState->machine.pc.colorGraphicsAdapter.graphicsControllerRegisters[8]);
            fflush(globalMemoryTraceFile);
        }
    }
    if (globalMemoryStopByteEnabled && physicalAddress == globalMemoryStopPhysicalAddress &&
        newValue == globalMemoryStopByteValue)
    {
        set_status_text(bootState,
                        "memory stop at %04X:%04X write %05X=%02X",
                        processor->lastInstructionSegment,
                        processor->lastInstructionOffset,
                        physicalAddress,
                        newValue);
        InterlockedExchange(&bootState->stopRequested, 1);
        InterlockedExchange(&bootState->isRunning, 0);
    }
}

static void capture_cpu_trace_entry(hyperdos_win32_boot_state* bootState)
{
    hyperdos_x86_16_processor*        processor      = &bootState->machine.pc.processor;
    hyperdos_monitor_cpu_trace_entry* traceEntry     = &bootState->cpuTraceEntries[processor->executedInstructionCount %
                                                                               HYPERDOS_MONITOR_CPU_TRACE_ENTRY_COUNT];
    size_t                            byteIndex      = 0u;
    size_t                            stackWordIndex = 0u;

    traceEntry->instructionCount   = processor->executedInstructionCount;
    traceEntry->codeSegment        = processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_CODE];
    traceEntry->instructionPointer = processor->instructionPointer;
    traceEntry->stackSegment       = processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_STACK];
    traceEntry->stackPointer       = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER];
    traceEntry->dataSegment        = processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_DATA];
    traceEntry->extraSegment       = processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA];
    traceEntry->accumulator        = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR];
    traceEntry->base               = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE];
    traceEntry->counter            = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER];
    traceEntry->data               = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA];
    traceEntry->basePointer        = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE_POINTER];
    traceEntry->sourceIndex        = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_SOURCE_INDEX];
    traceEntry->destinationIndex   = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DESTINATION_INDEX];
    traceEntry->flags              = processor->flags;
    for (byteIndex = 0u; byteIndex < HYPERDOS_MONITOR_CPU_TRACE_BYTE_COUNT; ++byteIndex)
    {
        traceEntry->instructionBytes
                [byteIndex] = read_guest_instruction_byte(bootState,
                                                          traceEntry->codeSegment,
                                                          (uint16_t)(traceEntry->instructionPointer + byteIndex));
    }
    for (stackWordIndex = 0u; stackWordIndex < HYPERDOS_MONITOR_CPU_TRACE_STACK_WORD_COUNT; ++stackWordIndex)
    {
        uint32_t physicalAddress = (((uint32_t)traceEntry->stackSegment << HYPERDOS_X86_16_SEGMENT_SHIFT) +
                                    (uint16_t)(traceEntry->stackPointer + stackWordIndex * HYPERDOS_X86_16_WORD_SIZE)) &
                                   HYPERDOS_X86_16_ADDRESS_MASK;
        traceEntry->stackWords[stackWordIndex] = read_guest_memory_word(bootState, physicalAddress);
    }
}

static void write_cpu_trace_file(hyperdos_win32_boot_state* bootState)
{
    hyperdos_x86_16_processor* processor                  = &bootState->machine.pc.processor;
    uint64_t                   traceStartInstructionCount = 0u;
    uint64_t                   traceInstructionCount      = 0u;
    FILE*                      traceFile                  = NULL;

    if (globalCpuTracePath[0] == '\0' || InterlockedCompareExchange(&bootState->cpuTraceEnabled, 0, 0) == 0)
    {
        return;
    }
    traceFile = fopen(globalCpuTracePath, "wb");
    if (traceFile == NULL)
    {
        return;
    }
    fprintf(traceFile, "HyperDOS CPU trace\n");
    traceStartInstructionCount = processor->executedInstructionCount > HYPERDOS_MONITOR_CPU_TRACE_ENTRY_COUNT
                                         ? processor->executedInstructionCount - HYPERDOS_MONITOR_CPU_TRACE_ENTRY_COUNT
                                         : 0u;
    for (traceInstructionCount = traceStartInstructionCount;
         traceInstructionCount <= processor->executedInstructionCount;
         ++traceInstructionCount)
    {
        hyperdos_monitor_cpu_trace_entry*
                traceEntry = &bootState->cpuTraceEntries[traceInstructionCount %
                                                         HYPERDOS_MONITOR_CPU_TRACE_ENTRY_COUNT];
        if (traceEntry->instructionCount != traceInstructionCount)
        {
            continue;
        }
        fprintf(traceFile,
                "%llu %04X:%04X bytes %02X %02X %02X %02X %02X %02X "
                "SS:SP=%04X:%04X DS=%04X ES=%04X "
                "AX=%04X BX=%04X CX=%04X DX=%04X BP=%04X SI=%04X DI=%04X FLAGS=%04X "
                "stack=%04X,%04X,%04X,%04X,%04X,%04X,%04X,%04X\n",
                (unsigned long long)traceEntry->instructionCount,
                traceEntry->codeSegment,
                traceEntry->instructionPointer,
                traceEntry->instructionBytes[0],
                traceEntry->instructionBytes[1],
                traceEntry->instructionBytes[2],
                traceEntry->instructionBytes[3],
                traceEntry->instructionBytes[4],
                traceEntry->instructionBytes[5],
                traceEntry->stackSegment,
                traceEntry->stackPointer,
                traceEntry->dataSegment,
                traceEntry->extraSegment,
                traceEntry->accumulator,
                traceEntry->base,
                traceEntry->counter,
                traceEntry->data,
                traceEntry->basePointer,
                traceEntry->sourceIndex,
                traceEntry->destinationIndex,
                traceEntry->flags,
                traceEntry->stackWords[0],
                traceEntry->stackWords[1],
                traceEntry->stackWords[2],
                traceEntry->stackWords[3],
                traceEntry->stackWords[4],
                traceEntry->stackWords[5],
                traceEntry->stackWords[6],
                traceEntry->stackWords[7]);
    }
    fclose(traceFile);
}

static void write_guest_memory_dump_file(hyperdos_win32_boot_state* bootState)
{
    if (globalGuestMemoryDumpPath[0] == '\0')
    {
        return;
    }
    (void)write_binary_file(globalGuestMemoryDumpPath,
                            bootState->machine.pc.processorMemory,
                            sizeof(bootState->machine.pc.processorMemory));
}

static void lock_keyboard_for_bios(void* userContext)
{
    EnterCriticalSection(&((hyperdos_win32_boot_state*)userContext)->keyboardCriticalSection);
}

static void unlock_keyboard_for_bios(void* userContext)
{
    LeaveCriticalSection(&((hyperdos_win32_boot_state*)userContext)->keyboardCriticalSection);
}

static void refresh_keyboard_input_state_for_bios(void* userContext)
{
    refresh_keyboard_input_state((hyperdos_win32_boot_state*)userContext);
}

static void handle_speaker_state_change(void* userContext, uint32_t frequencyHertz, uint8_t enabled)
{
    hyperdos_win32_boot_state* bootState = (hyperdos_win32_boot_state*)userContext;

    trace_disk_event(bootState, "pc speaker enabled=%u frequency=%lu", enabled != 0u, (unsigned long)frequencyHertz);
    if (enabled)
    {
        MessageBeep(MB_OK);
    }
}

static void free_loaded_disk_images(hyperdos_win32_boot_state* bootState)
{
    hyperdos_pc_storage_context storageContext;

    initialize_storage_context_for_boot_state(bootState, &storageContext);
    hyperdos_pc_storage_free_all_disk_images(&storageContext);
}

static int initialize_boot_from_disk_images(hyperdos_win32_boot_state* bootState)
{
    size_t                                 floppyDriveIndex            = 0;
    hyperdos_pc_disk_image*                bootDisk                    = NULL;
    const hyperdos_pc_disk_image*          activeFloppyDiskForDataArea = NULL;
    const char*                            floppyStatusText            = "not inserted";
    const char*                            hardDiskStatusText          = "not attached";
    uint8_t                                fixedDiskCount              = 0u;
    hyperdos_pc_storage_context            storageContext;
    hyperdos_pc_machine_boot_configuration machineConfiguration;

    bootState->executionResult = HYPERDOS_X86_16_EXECUTION_OK;
    InterlockedExchange(&bootState->stopRequested, 0);
    InterlockedExchange(&bootState->isRunning, 0);
    InterlockedExchange(&bootState->isWaitingForKeyboard, 0);
    InterlockedExchange(&bootState->cpuTraceEnabled, 0);

    if (!flush_all_disk_images(bootState))
    {
        set_status_text(bootState, "failed to flush disk image before reboot");
        return 0;
    }
    free_loaded_disk_images(bootState);
    memset(bootState->floppyDrives, 0, sizeof(bootState->floppyDrives));
    memset(bootState->fixedDiskDrives, 0, sizeof(bootState->fixedDiskDrives));
    bootState->floppyDriveCount = globalFloppyImagePathCount > HYPERDOS_MONITOR_FLOPPY_DRIVE_COUNT
                                          ? globalFloppyImagePathCount
                                          : HYPERDOS_MONITOR_FLOPPY_DRIVE_COUNT;
    if (bootState->floppyDriveCount > HYPERDOS_MONITOR_MAXIMUM_FLOPPY_DRIVE_COUNT)
    {
        bootState->floppyDriveCount = HYPERDOS_MONITOR_MAXIMUM_FLOPPY_DRIVE_COUNT;
    }
    bootState->hostScanCodeQueueReadIndex               = 0u;
    bootState->hostScanCodeQueueWriteIndex              = 0u;
    bootState->hostMouseEventQueueReadIndex             = 0u;
    bootState->hostMouseEventQueueWriteIndex            = 0u;
    bootState->hostMouseEventQueueCount                 = 0u;
    bootState->mouseCaptureLastClientHorizontalPosition = 0;
    bootState->mouseCaptureLastClientVerticalPosition   = 0;
    bootState->hostMouseRawMovementObserved             = 0u;
    bootState->mouseCaptureLastClientPositionValid      = 0u;
    bootState->mouseButtonMask                          = 0u;
    bootState->hostMouseCaptureActive                   = 0u;
    bootState->hostMouseCaptureToggleKeyDown            = 0u;
    memset(bootState->hostScanCodePressed, 0, sizeof(bootState->hostScanCodePressed));
    bootState->lastTextScreenDumpTick = 0u;
    bootState->lastVideoStateDumpTick = 0u;

    memset(&machineConfiguration, 0, sizeof(machineConfiguration));
    machineConfiguration.userContext                             = bootState;
    machineConfiguration.processorModel                          = globalProcessorModel;
    machineConfiguration.pcModel                                 = globalPcModel;
    machineConfiguration.floppyDriveCount                        = (uint8_t)bootState->floppyDriveCount;
    machineConfiguration.fixedDiskDriveCount                     = HYPERDOS_MONITOR_FIXED_DISK_DRIVE_COUNT;
    machineConfiguration.coprocessorEnabled                      = (uint8_t)globalCoprocessorEnabled;
    machineConfiguration.divideErrorReturnsToFaultingInstruction = (uint8_t)
            globalDivideErrorReturnsToFaultingInstruction;
    machineConfiguration.lockKeyboard                         = lock_keyboard_for_bios;
    machineConfiguration.unlockKeyboard                       = unlock_keyboard_for_bios;
    machineConfiguration.refreshKeyboardInputState            = refresh_keyboard_input_state_for_bios;
    machineConfiguration.lockDiskImages                       = lock_disk_images_for_bios;
    machineConfiguration.unlockDiskImages                     = unlock_disk_images_for_bios;
    machineConfiguration.getDiskImage                         = get_disk_image_for_bios;
    machineConfiguration.drainKeyboardInput                   = drain_keyboard_input_for_bios_runtime;
    machineConfiguration.observeMemoryWrite                   = observe_guest_memory_write;
    machineConfiguration.floppyControllerInputOutputDevice    = bootState;
    machineConfiguration.readFloppyControllerInputOutputByte  = read_floppy_controller_input_output_byte;
    machineConfiguration.writeFloppyControllerInputOutputByte = write_floppy_controller_input_output_byte;
    machineConfiguration.speakerStateChange                   = handle_speaker_state_change;
    machineConfiguration.traceFloppyController                = trace_monitor_event;
    machineConfiguration.traceDiskBios                        = trace_monitor_event;
    machineConfiguration.traceVideoServices                   = trace_monitor_event;
    if (!hyperdos_pc_machine_initialize_for_boot(&bootState->machine, &machineConfiguration))
    {
        set_status_text(bootState, "failed to initialize PC machine");
        return 0;
    }
    initialize_storage_context_for_boot_state(bootState, &storageContext);

    for (floppyDriveIndex = 0; floppyDriveIndex < bootState->floppyDriveCount; ++floppyDriveIndex)
    {
        hyperdos_pc_storage_install_floppy_drive(&storageContext, (uint8_t)floppyDriveIndex);
    }
    for (floppyDriveIndex = 0;
         floppyDriveIndex < globalFloppyImagePathCount && floppyDriveIndex < bootState->floppyDriveCount;
         ++floppyDriveIndex)
    {
        hyperdos_pc_disk_image* diskImage = &bootState->floppyDrives[floppyDriveIndex].diskImage;

        if (!hyperdos_pc_disk_image_load_floppy(diskImage, globalFloppyImagePaths[floppyDriveIndex]))
        {
            set_status_text(bootState, "failed to read floppy image: %s", globalFloppyImagePaths[floppyDriveIndex]);
            free_loaded_disk_images(bootState);
            return 0;
        }
        diskImage->driveNumber = (uint8_t)floppyDriveIndex;
    }

    if (globalHardDiskImagePath[0] != '\0' &&
        !hyperdos_pc_disk_image_load_hard_disk(&bootState->fixedDiskDrives[0].diskImage, globalHardDiskImagePath))
    {
        set_status_text(bootState, "failed to attach hard disk image: %s", globalHardDiskImagePath);
        free_loaded_disk_images(bootState);
        return 0;
    }
    if (bootState->fixedDiskDrives[0].diskImage.inserted)
    {
        hyperdos_pc_storage_install_fixed_disk_drive(&storageContext, 0u);
    }
    fixedDiskCount = hyperdos_pc_storage_count_inserted_fixed_disks(&storageContext);

    if (globalBootHardDiskWithoutFloppy && bootState->fixedDiskDrives[0].diskImage.inserted)
    {
        bootDisk = &bootState->fixedDiskDrives[0].diskImage;
    }
    else
    {
        bootDisk = get_floppy_disk_locked(bootState, 0u);
        if (bootDisk == NULL && bootState->fixedDiskDrives[0].diskImage.inserted)
        {
            bootDisk = &bootState->fixedDiskDrives[0].diskImage;
        }
    }
    if (bootDisk == NULL)
    {
        set_status_text(bootState, "no boot disk selected");
        free_loaded_disk_images(bootState);
        return 0;
    }
    activeFloppyDiskForDataArea = get_floppy_disk_locked(bootState, 0u);
    if (!hyperdos_pc_bios_runtime_prepare_boot_from_disk_image(&bootState->machine.biosRuntime,
                                                               bootDisk,
                                                               activeFloppyDiskForDataArea,
                                                               fixedDiskCount))
    {
        set_status_text(bootState, "disk image is smaller than a boot sector");
        free_loaded_disk_images(bootState);
        return 0;
    }

    if (globalCpuTraceStartsEnabled)
    {
        memset(bootState->cpuTraceEntries, 0, sizeof(bootState->cpuTraceEntries));
        InterlockedExchange(&bootState->cpuTraceEnabled, 1);
    }
    InterlockedExchange(&bootState->isRunning, 1);
    if (bootState->floppyDriveCount != 0u)
    {
        hyperdos_pc_disk_image* activeFloppyDisk = get_floppy_disk_locked(bootState, 0u);
        if (activeFloppyDisk != NULL)
        {
            floppyStatusText = find_file_name_from_path(activeFloppyDisk->path);
        }
    }
    if (bootState->fixedDiskDrives[0].diskImage.inserted)
    {
        hardDiskStatusText = find_file_name_from_path(bootState->fixedDiskDrives[0].diskImage.path);
    }
    set_status_text(bootState,
                    "boot %c:, A: %s, C: %s",
                    bootDisk->driveNumber == HYPERDOS_PC_DISK_BIOS_HARD_DISK_DRIVE_NUMBER ? 'C' : 'A',
                    floppyStatusText,
                    hardDiskStatusText);
    return 1;
}

static int initialize_emulation_pacing(LARGE_INTEGER*                   performanceCounterFrequency,
                                       LARGE_INTEGER*                   hostPerformanceCounterBase,
                                       uint64_t*                        guestClockBase,
                                       const hyperdos_win32_boot_state* bootState);

static void pace_emulation_to_guest_clock(const hyperdos_win32_boot_state* bootState,
                                          const LARGE_INTEGER*             performanceCounterFrequency,
                                          const LARGE_INTEGER*             hostPerformanceCounterBase,
                                          uint64_t                         guestClockBase);

static DWORD WINAPI emulation_thread_main(void* parameter)
{
    hyperdos_win32_boot_state* bootState                = (hyperdos_win32_boot_state*)parameter;
    uint64_t                   previousInstructionCount = 0;
    uint64_t                   instructionLimit         = 0;
    ULONGLONG                  lastRenderTick           = GetTickCount64();
    LARGE_INTEGER              performanceCounterFrequency;
    LARGE_INTEGER              hostPerformanceCounterBase;
    uint64_t                   guestClockBase         = 0u;
    int                        emulationPacingEnabled = 0;

    emulationPacingEnabled = initialize_emulation_pacing(&performanceCounterFrequency,
                                                         &hostPerformanceCounterBase,
                                                         &guestClockBase,
                                                         bootState);

    while (InterlockedCompareExchange(&bootState->stopRequested, 0, 0) == 0 &&
           InterlockedCompareExchange(&bootState->isRunning, 0, 0) != 0)
    {
        previousInstructionCount = bootState->machine.pc.processor.executedInstructionCount;
        if (globalCpuTracePath[0] != '\0' && InterlockedCompareExchange(&bootState->cpuTraceEnabled, 0, 0) != 0)
        {
            capture_cpu_trace_entry(bootState);
            instructionLimit = previousInstructionCount + 1u;
        }
        else
        {
            instructionLimit = previousInstructionCount + HYPERDOS_MONITOR_INSTRUCTIONS_PER_SLICE;
        }
        bootState->executionResult = hyperdos_pc_bios_runtime_execute_processor_slice(&bootState->machine.biosRuntime,
                                                                                      instructionLimit,
                                                                                      NULL);
        maybe_write_text_screen_dump_file(bootState);
        maybe_write_video_state_dump_file(bootState);
        if (bootState->executionResult == HYPERDOS_X86_16_EXECUTION_HALTED)
        {
            (void)hyperdos_pc_step_halted_processor_clock(&bootState->machine.pc,
                                                          HYPERDOS_MONITOR_RENDER_TIMER_PERIOD_MILLISECONDS,
                                                          trace_monitor_event,
                                                          bootState);
            InterlockedExchange(&bootState->isWaitingForKeyboard, 1);
            WaitForSingleObject(bootState->keyboardEventHandle, HYPERDOS_MONITOR_RENDER_TIMER_PERIOD_MILLISECONDS);
            InterlockedExchange(&bootState->isWaitingForKeyboard, 0);
            if (InterlockedCompareExchange(&bootState->stopRequested, 0, 0) != 0)
            {
                break;
            }
            continue;
        }
        if (bootState->executionResult != HYPERDOS_X86_16_EXECUTION_STEP_LIMIT_REACHED)
        {
            InterlockedExchange(&bootState->isRunning, 0);
            write_cpu_trace_file(bootState);
            write_guest_memory_dump_file(bootState);
            if (bootState->executionResult == HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION &&
                hyperdos_x86_16_processor_is_at_bios_reset_vector(&bootState->machine.pc.processor))
            {
                set_status_text(bootState, "guest requested reset");
                PostMessageA(bootState->windowHandle, HYPERDOS_MONITOR_USER_RESET_MESSAGE, 0, 0);
                break;
            }
            if (bootState->executionResult != HYPERDOS_X86_16_EXECUTION_HALTED)
            {
                uint16_t instructionSegment = bootState->machine.pc.processor.lastInstructionSegment;
                uint16_t instructionOffset  = bootState->machine.pc.processor.lastInstructionOffset;
                uint8_t  firstByte  = read_guest_instruction_byte(bootState, instructionSegment, instructionOffset);
                uint8_t  secondByte = read_guest_instruction_byte(bootState,
                                                                 instructionSegment,
                                                                 (uint16_t)(instructionOffset + 1u));
                uint8_t  thirdByte  = read_guest_instruction_byte(bootState,
                                                                instructionSegment,
                                                                (uint16_t)(instructionOffset + 2u));
                uint8_t  fourthByte = read_guest_instruction_byte(bootState,
                                                                 instructionSegment,
                                                                 (uint16_t)(instructionOffset + 3u));
                set_status_text(bootState,
                                "%s at %04X:%04X opcode %02X bytes %02X %02X %02X %02X",
                                hyperdos_x86_16_execution_result_name(bootState->executionResult),
                                instructionSegment,
                                instructionOffset,
                                bootState->machine.pc.processor.lastOperationCode,
                                firstByte,
                                secondByte,
                                thirdByte,
                                fourthByte);
            }
            PostMessageA(bootState->windowHandle, HYPERDOS_MONITOR_USER_RENDER_MESSAGE, 0, 0);
            break;
        }
        if (GetTickCount64() - lastRenderTick >= HYPERDOS_MONITOR_RENDER_TIMER_PERIOD_MILLISECONDS)
        {
            PostMessageA(bootState->windowHandle, HYPERDOS_MONITOR_USER_RENDER_MESSAGE, 0, 0);
            lastRenderTick = GetTickCount64();
        }
        if (emulationPacingEnabled)
        {
            pace_emulation_to_guest_clock(bootState,
                                          &performanceCounterFrequency,
                                          &hostPerformanceCounterBase,
                                          guestClockBase);
        }
        Sleep(0);
    }
    write_text_screen_dump_file(bootState);
    write_video_state_dump_file(bootState);
    write_cpu_trace_file(bootState);
    write_guest_memory_dump_file(bootState);
    return 0;
}

static void stop_emulation_thread(hyperdos_win32_boot_state* bootState)
{
    InterlockedExchange(&bootState->stopRequested, 1);
    InterlockedExchange(&bootState->isRunning, 0);
    InterlockedExchange(&bootState->isWaitingForKeyboard, 0);
    if (bootState->keyboardEventHandle != NULL)
    {
        SetEvent(bootState->keyboardEventHandle);
    }
    if (bootState->emulationThreadHandle != NULL)
    {
        WaitForSingleObject(bootState->emulationThreadHandle, 2000u);
        CloseHandle(bootState->emulationThreadHandle);
        bootState->emulationThreadHandle = NULL;
    }
}

static void shutdown_boot_state(hyperdos_win32_boot_state* bootState)
{
    stop_emulation_thread(bootState);
    if (bootState->keyboardEventHandle != NULL)
    {
        CloseHandle(bootState->keyboardEventHandle);
        bootState->keyboardEventHandle = NULL;
    }
    flush_all_disk_images(bootState);
    free_loaded_disk_images(bootState);
    DeleteCriticalSection(&bootState->diskCriticalSection);
    DeleteCriticalSection(&bootState->keyboardCriticalSection);
}

static int try_convert_korean_code_page_character(uint8_t firstCharacterByte,
                                                  uint8_t secondCharacterByte,
                                                  WCHAR*  character)
{
    char multiByteCharacters[2];

    multiByteCharacters[0] = (char)firstCharacterByte;
    multiByteCharacters[1] = (char)secondCharacterByte;
    return MultiByteToWideChar(HYPERDOS_MONITOR_KOREAN_WINDOWS_CODE_PAGE,
                               MB_ERR_INVALID_CHARS,
                               multiByteCharacters,
                               2,
                               character,
                               1) == 1;
}

static int try_read_korean_code_page_text_character(const hyperdos_win32_boot_state* bootState,
                                                    uint32_t                         row,
                                                    uint32_t                         column,
                                                    uint8_t                          firstCharacterByte,
                                                    WCHAR*                           character)
{
    uint8_t secondCharacterByte = 0u;
    size_t  nextMemoryIndex     = 0u;

    if (globalTextCharacterSet != HYPERDOS_MONITOR_TEXT_CHARACTER_SET_KOREAN_CODE_PAGE_949 ||
        !hyperdos_pc_text_korean_code_page_949_is_lead_byte(firstCharacterByte) ||
        column + 1u >= HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT)
    {
        return 0;
    }
    nextMemoryIndex =
            hyperdos_pc_video_services_get_text_page_cell_memory_index(&bootState->machine.videoServices,
                                                                       bootState->machine.videoServices.activeVideoPage,
                                                                       (uint16_t)row,
                                                                       (uint16_t)(column + 1u));
    secondCharacterByte = hyperdos_pc_video_services_read_text_memory_byte(&bootState->machine.videoServices,
                                                                           nextMemoryIndex);

    return hyperdos_pc_text_korean_code_page_949_is_trail_byte(secondCharacterByte) &&
           try_convert_korean_code_page_character(firstCharacterByte, secondCharacterByte, character);
}

static int code_page_437_source_pixel_is_set(HDC deviceContext,
                                             int firstSourceColumn,
                                             int lastSourceColumn,
                                             int firstSourceRow,
                                             int lastSourceRow)
{
    int sourceRow    = 0;
    int sourceColumn = 0;

    for (sourceRow = firstSourceRow; sourceRow < lastSourceRow; ++sourceRow)
    {
        for (sourceColumn = firstSourceColumn; sourceColumn < lastSourceColumn; ++sourceColumn)
        {
            COLORREF pixel = GetPixel(deviceContext, sourceColumn, sourceRow);
            if ((pixel & 0x00FFFFFFu) != 0x00FFFFFFu)
            {
                return 1;
            }
        }
    }
    return 0;
}

static int initialize_code_page_437_glyph_rows(HDC referenceDeviceContext)
{
    enum
    {
        GLYPH_BITMAP_WIDTH  = 32,
        GLYPH_BITMAP_HEIGHT = 32
    };
    HDC     glyphDeviceContext = NULL;
    HBITMAP glyphBitmap        = NULL;
    HGDIOBJ previousBitmap     = NULL;
    HFONT   previousFont       = NULL;
    RECT    glyphRectangle     = {0, 0, GLYPH_BITMAP_WIDTH, GLYPH_BITMAP_HEIGHT};
    size_t  characterIndex     = 0u;

    if (referenceDeviceContext == NULL || globalCodePage437TextFontHandle == NULL)
    {
        return 0;
    }
    glyphDeviceContext = CreateCompatibleDC(referenceDeviceContext);
    if (glyphDeviceContext == NULL)
    {
        return 0;
    }
    glyphBitmap = CreateCompatibleBitmap(referenceDeviceContext, GLYPH_BITMAP_WIDTH, GLYPH_BITMAP_HEIGHT);
    if (glyphBitmap == NULL)
    {
        DeleteDC(glyphDeviceContext);
        return 0;
    }

    previousBitmap = SelectObject(glyphDeviceContext, glyphBitmap);
    previousFont   = SelectObject(glyphDeviceContext, globalCodePage437TextFontHandle);
    SetBkMode(glyphDeviceContext, TRANSPARENT);
    SetTextColor(glyphDeviceContext, RGB(0, 0, 0));

    for (characterIndex = 0u; characterIndex < HYPERDOS_MONITOR_CHARACTER_COUNT; ++characterIndex)
    {
        WCHAR character    = (WCHAR)hyperdos_pc_text_code_page_437_unicode_character((uint8_t)characterIndex);
        SIZE  glyphSize    = {HYPERDOS_MONITOR_CHARACTER_WIDTH, HYPERDOS_MONITOR_CHARACTER_HEIGHT};
        int   sourceWidth  = HYPERDOS_MONITOR_CHARACTER_WIDTH;
        int   sourceHeight = HYPERDOS_MONITOR_CHARACTER_HEIGHT;
        int   targetRow    = 0;

        FillRect(glyphDeviceContext, &glyphRectangle, (HBRUSH)GetStockObject(WHITE_BRUSH));
        (void)GetTextExtentPoint32W(glyphDeviceContext, &character, 1, &glyphSize);
        if (glyphSize.cx > 0 && glyphSize.cx <= GLYPH_BITMAP_WIDTH)
        {
            sourceWidth = glyphSize.cx;
        }
        if (glyphSize.cy > 0 && glyphSize.cy <= GLYPH_BITMAP_HEIGHT)
        {
            sourceHeight = glyphSize.cy;
        }
        TextOutW(glyphDeviceContext, 0, 0, &character, 1);

        for (targetRow = 0; targetRow < HYPERDOS_MONITOR_CHARACTER_HEIGHT; ++targetRow)
        {
            uint16_t rowBits        = 0u;
            int      firstSourceRow = (targetRow * sourceHeight) / HYPERDOS_MONITOR_CHARACTER_HEIGHT;
            int      lastSourceRow  = ((targetRow + 1) * sourceHeight + HYPERDOS_MONITOR_CHARACTER_HEIGHT - 1) /
                                HYPERDOS_MONITOR_CHARACTER_HEIGHT;
            int targetColumn = 0;

            if (lastSourceRow <= firstSourceRow)
            {
                lastSourceRow = firstSourceRow + 1;
            }
            for (targetColumn = 0; targetColumn < HYPERDOS_MONITOR_CHARACTER_WIDTH; ++targetColumn)
            {
                int firstSourceColumn = (targetColumn * sourceWidth) / HYPERDOS_MONITOR_CHARACTER_WIDTH;
                int lastSourceColumn  = ((targetColumn + 1) * sourceWidth + HYPERDOS_MONITOR_CHARACTER_WIDTH - 1) /
                                       HYPERDOS_MONITOR_CHARACTER_WIDTH;

                if (lastSourceColumn <= firstSourceColumn)
                {
                    lastSourceColumn = firstSourceColumn + 1;
                }
                if (code_page_437_source_pixel_is_set(glyphDeviceContext,
                                                      firstSourceColumn,
                                                      lastSourceColumn,
                                                      firstSourceRow,
                                                      lastSourceRow))
                {
                    rowBits |= (uint16_t)(1u << targetColumn);
                }
            }
            globalCodePage437GlyphRows[characterIndex][targetRow] = rowBits;
        }
    }

    SelectObject(glyphDeviceContext, previousFont);
    SelectObject(glyphDeviceContext, previousBitmap);
    DeleteObject(glyphBitmap);
    DeleteDC(glyphDeviceContext);
    globalCodePage437GlyphRowsInitialized = 1;
    return 1;
}

static void render_code_page_437_character_cell(HDC      deviceContext,
                                                LONG     cellLeft,
                                                LONG     cellTop,
                                                uint8_t  characterByte,
                                                COLORREF foregroundColor)
{
    HBRUSH foregroundBrush = (HBRUSH)GetStockObject(DC_BRUSH);
    int    targetRow       = 0;

    SetDCBrushColor(deviceContext, foregroundColor);
    for (targetRow = 0; targetRow < HYPERDOS_MONITOR_CHARACTER_HEIGHT; ++targetRow)
    {
        uint16_t rowBits      = globalCodePage437GlyphRows[characterByte][targetRow];
        int      targetColumn = 0;

        while (targetColumn < HYPERDOS_MONITOR_CHARACTER_WIDTH)
        {
            RECT runRectangle;
            int  runStartColumn = targetColumn;

            while (targetColumn < HYPERDOS_MONITOR_CHARACTER_WIDTH && (rowBits & (uint16_t)(1u << targetColumn)) == 0u)
            {
                ++targetColumn;
            }
            runStartColumn = targetColumn;
            while (targetColumn < HYPERDOS_MONITOR_CHARACTER_WIDTH && (rowBits & (uint16_t)(1u << targetColumn)) != 0u)
            {
                ++targetColumn;
            }
            if (runStartColumn == targetColumn)
            {
                continue;
            }
            runRectangle.left   = cellLeft + runStartColumn;
            runRectangle.top    = cellTop + targetRow;
            runRectangle.right  = cellLeft + targetColumn;
            runRectangle.bottom = runRectangle.top + 1;
            FillRect(deviceContext, &runRectangle, foregroundBrush);
        }
    }
}

static size_t append_wide_character_as_utf8(char* line, size_t lineCapacity, size_t lineLength, WCHAR character)
{
    char utf8Bytes[HYPERDOS_MONITOR_UTF8_MAXIMUM_BYTE_COUNT];
    int  byteCount = WideCharToMultiByte(CP_UTF8, 0, &character, 1, utf8Bytes, sizeof(utf8Bytes), NULL, NULL);

    if (byteCount <= 0)
    {
        if (lineLength + 1u < lineCapacity)
        {
            line[lineLength] = '?';
            ++lineLength;
        }
        return lineLength;
    }
    if (lineLength + (size_t)byteCount >= lineCapacity)
    {
        return lineLength;
    }
    memcpy(line + lineLength, utf8Bytes, (size_t)byteCount);
    return lineLength + (size_t)byteCount;
}

static void write_text_screen_dump_file(const hyperdos_win32_boot_state* bootState)
{
    static const uint8_t utf8ByteOrderMark[] = {0xEFu, 0xBBu, 0xBFu};
    FILE*                textScreenDumpFile  = NULL;
    uint32_t             row                 = 0u;

    if (globalTextScreenDumpPath[0] == '\0')
    {
        return;
    }

    textScreenDumpFile = fopen(globalTextScreenDumpPath, "wb");
    if (textScreenDumpFile == NULL)
    {
        return;
    }
    fwrite(utf8ByteOrderMark, 1u, sizeof(utf8ByteOrderMark), textScreenDumpFile);

    for (row = 0u; row < HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT; ++row)
    {
        char     line[HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT * HYPERDOS_MONITOR_UTF8_MAXIMUM_BYTE_COUNT + 1u];
        size_t   lineLength = 0u;
        uint32_t column     = 0u;

        for (column = 0u; column < HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT; ++column)
        {
            size_t memoryIndex = hyperdos_pc_video_services_get_text_page_cell_memory_index(
                    &bootState->machine.videoServices,
                    bootState->machine.videoServices.activeVideoPage,
                    (uint16_t)row,
                    (uint16_t)column);
            uint8_t characterByte = hyperdos_pc_video_services_read_text_memory_byte(&bootState->machine.videoServices,
                                                                                     memoryIndex);
            WCHAR   wideCharacter = L'?';

            if (try_read_korean_code_page_text_character(bootState, row, column, characterByte, &wideCharacter))
            {
                lineLength = append_wide_character_as_utf8(line, sizeof(line), lineLength, wideCharacter);
                ++column;
                continue;
            }

            lineLength = append_wide_character_as_utf8(line,
                                                       sizeof(line),
                                                       lineLength,
                                                       (WCHAR)hyperdos_pc_text_code_page_437_unicode_character(
                                                               characterByte));
        }

        while (lineLength > 0u && line[lineLength - 1u] == ' ')
        {
            --lineLength;
        }
        fwrite(line, 1u, lineLength, textScreenDumpFile);
        fputc('\n', textScreenDumpFile);
    }

    fclose(textScreenDumpFile);
}

static void maybe_write_text_screen_dump_file(hyperdos_win32_boot_state* bootState)
{
    ULONGLONG currentTick = 0u;

    if (globalTextScreenDumpPath[0] == '\0')
    {
        return;
    }

    currentTick = GetTickCount64();
    if (bootState->lastTextScreenDumpTick == 0u ||
        currentTick - bootState->lastTextScreenDumpTick >= HYPERDOS_MONITOR_TEXT_SCREEN_DUMP_PERIOD_MILLISECONDS)
    {
        write_text_screen_dump_file(bootState);
        bootState->lastTextScreenDumpTick = currentTick;
    }
}

static void write_video_register_dump_line(FILE*          videoStateDumpFile,
                                           const char*    name,
                                           const uint8_t* registers,
                                           size_t         registerCount)
{
    size_t registerIndex = 0u;

    fprintf(videoStateDumpFile, "%s=", name);
    for (registerIndex = 0u; registerIndex < registerCount; ++registerIndex)
    {
        fprintf(videoStateDumpFile, "%s%02X", registerIndex == 0u ? "" : " ", registers[registerIndex]);
    }
    fputc('\n', videoStateDumpFile);
}

static uint8_t get_planar_video_graphics_array_color_index_at_pixel(const hyperdos_color_graphics_adapter* adapter,
                                                                    uint32_t                               sourceWidth,
                                                                    uint32_t                               sourceHeight,
                                                                    uint32_t                               column,
                                                                    uint32_t                               row)
{
    uint32_t displayStartAddress = 0u;
    uint32_t bytesPerScanLine    = 0u;
    uint32_t planeAddress        = 0u;
    uint8_t  bitMask             = 0u;
    uint8_t  colorIndex          = 0u;
    size_t   planeIndex          = 0u;

    if (adapter == NULL || column >= sourceWidth || row >= sourceHeight)
    {
        return 0u;
    }

    displayStartAddress = hyperdos_color_graphics_adapter_get_display_start_address(adapter);
    bytesPerScanLine    = hyperdos_color_graphics_adapter_get_display_stride_byte_count(adapter);
    if (bytesPerScanLine == 0u)
    {
        bytesPerScanLine = (sourceWidth + 7u) / 8u;
    }

    planeAddress = (displayStartAddress + row * bytesPerScanLine + (column >> 3u)) &
                   (HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_SIZE - 1u);
    bitMask = (uint8_t)(0x80u >> (column & 0x07u));
    for (planeIndex = 0u; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
    {
        if ((adapter->graphicsPlanes[planeIndex][planeAddress] & bitMask) != 0u)
        {
            colorIndex |= (uint8_t)(1u << planeIndex);
        }
    }
    return colorIndex;
}

static void write_video_digital_to_analog_converter_palette_dump(FILE* videoStateDumpFile,
                                                                 const hyperdos_color_graphics_adapter* adapter)
{
    size_t colorIndex = 0u;

    if (videoStateDumpFile == NULL || adapter == NULL)
    {
        return;
    }

    fputs("digital-to-analog-converter=", videoStateDumpFile);
    for (colorIndex = 0u; colorIndex < 16u; ++colorIndex)
    {
        fprintf(videoStateDumpFile,
                "%s%02X:%02X,%02X,%02X",
                colorIndex == 0u ? "" : " ",
                (unsigned int)colorIndex,
                adapter->digitalToAnalogConverterPalette[colorIndex][0],
                adapter->digitalToAnalogConverterPalette[colorIndex][1],
                adapter->digitalToAnalogConverterPalette[colorIndex][2]);
    }
    fputc('\n', videoStateDumpFile);
}

static void write_planar_video_graphics_array_color_index_dump(FILE* videoStateDumpFile,
                                                               const hyperdos_color_graphics_adapter* adapter,
                                                               uint32_t                               sourceWidth,
                                                               uint32_t                               sourceHeight)
{
    uint32_t colorIndexCounts[16] = {0u};
    uint32_t row                  = 0u;
    uint32_t sampleIndex          = 0u;
    static const struct
    {
        uint32_t column;
        uint32_t row;
    } samples[] = {
        {16u,  16u },
        {32u,  72u },
        {120u, 120u},
        {240u, 180u},
        {400u, 180u},
        {520u, 220u},
        {80u,  420u},
        {560u, 420u},
    };

    if (videoStateDumpFile == NULL || adapter == NULL || sourceWidth == 0u || sourceHeight == 0u)
    {
        return;
    }

    for (row = 0u; row < sourceHeight; ++row)
    {
        uint32_t column = 0u;
        for (column = 0u; column < sourceWidth; ++column)
        {
            uint8_t colorIndex = get_planar_video_graphics_array_color_index_at_pixel(adapter,
                                                                                      sourceWidth,
                                                                                      sourceHeight,
                                                                                      column,
                                                                                      row);
            ++colorIndexCounts[colorIndex & 0x0Fu];
        }
    }

    fputs("color-index-histogram=", videoStateDumpFile);
    for (sampleIndex = 0u; sampleIndex < 16u; ++sampleIndex)
    {
        fprintf(videoStateDumpFile,
                "%s%02X:%lu",
                sampleIndex == 0u ? "" : " ",
                (unsigned int)sampleIndex,
                (unsigned long)colorIndexCounts[sampleIndex]);
    }
    fputc('\n', videoStateDumpFile);

    fputs("color-index-samples=", videoStateDumpFile);
    for (sampleIndex = 0u; sampleIndex < sizeof(samples) / sizeof(samples[0]); ++sampleIndex)
    {
        uint32_t column     = samples[sampleIndex].column;
        uint32_t sampleRow  = samples[sampleIndex].row;
        uint8_t  colorIndex = get_planar_video_graphics_array_color_index_at_pixel(adapter,
                                                                                  sourceWidth,
                                                                                  sourceHeight,
                                                                                  column,
                                                                                  sampleRow);

        fprintf(videoStateDumpFile,
                "%s%lu,%lu:%02X",
                sampleIndex == 0u ? "" : " ",
                (unsigned long)column,
                (unsigned long)sampleRow,
                colorIndex);
    }
    fputc('\n', videoStateDumpFile);

    fputs("plane-byte-samples=", videoStateDumpFile);
    for (sampleIndex = 0u; sampleIndex < sizeof(samples) / sizeof(samples[0]); ++sampleIndex)
    {
        uint32_t column              = samples[sampleIndex].column;
        uint32_t sampleRow           = samples[sampleIndex].row;
        uint32_t bytesPerScanLine    = hyperdos_color_graphics_adapter_get_display_stride_byte_count(adapter);
        uint32_t displayStartAddress = hyperdos_color_graphics_adapter_get_display_start_address(adapter);
        uint32_t planeAddress        = 0u;

        if (bytesPerScanLine == 0u)
        {
            bytesPerScanLine = (sourceWidth + 7u) / 8u;
        }
        planeAddress = (displayStartAddress + sampleRow * bytesPerScanLine + (column >> 3u)) &
                       (HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_SIZE - 1u);
        fprintf(videoStateDumpFile,
                "%s%lu,%lu@%04lX:%02X,%02X,%02X,%02X",
                sampleIndex == 0u ? "" : " ",
                (unsigned long)column,
                (unsigned long)sampleRow,
                (unsigned long)planeAddress,
                adapter->graphicsPlanes[0][planeAddress],
                adapter->graphicsPlanes[1][planeAddress],
                adapter->graphicsPlanes[2][planeAddress],
                adapter->graphicsPlanes[3][planeAddress]);
    }
    fputc('\n', videoStateDumpFile);
}

static char get_video_preview_character_from_color(COLORREF color)
{
    static const char ramp[]    = " .:-=+*#%@";
    uint32_t          red       = color & 0xFFu;
    uint32_t          green     = (color >> 8u) & 0xFFu;
    uint32_t          blue      = (color >> 16u) & 0xFFu;
    uint32_t          luminance = red * 30u + green * 59u + blue * 11u;
    size_t            rampIndex = (size_t)((luminance * (sizeof(ramp) - 2u)) / (255u * 100u));

    return ramp[rampIndex];
}

static int prepare_video_preview_pixels(const hyperdos_win32_boot_state* bootState, int* sourceWidth, int* sourceHeight)
{
    const hyperdos_color_graphics_adapter* adapter = &bootState->machine.pc.colorGraphicsAdapter;
    int vgaGraphicsMode = hyperdos_pc_video_services_get_video_graphics_array_mode_dimensions(
            bootState->machine.videoServices.videoMode,
            sourceWidth,
            sourceHeight);

    if (!hyperdos_color_graphics_adapter_graphics_mode_is_enabled(adapter))
    {
        return 0;
    }
    if (vgaGraphicsMode && bootState->machine.videoServices.videoMode == 0x13u)
    {
        hyperdos_pc_video_services_render_linear_256_color_video_graphics_array_pixels(adapter,
                                                                                       *sourceWidth,
                                                                                       *sourceHeight,
                                                                                       globalGraphicsPixels);
        return 1;
    }
    if (vgaGraphicsMode)
    {
        hyperdos_pc_video_services_render_planar_video_graphics_array_pixels(adapter,
                                                                             bootState->machine.videoServices.videoMode,
                                                                             *sourceWidth,
                                                                             *sourceHeight,
                                                                             globalGraphicsPixels);
        return 1;
    }
    if (*sourceWidth == HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_WIDTH_HIGH)
    {
        hyperdos_pc_video_services_render_high_resolution_graphics_pixels(adapter, globalGraphicsPixels);
    }
    else
    {
        hyperdos_pc_video_services_render_low_resolution_graphics_pixels(adapter, globalGraphicsPixels);
    }
    return 1;
}

static void write_video_state_dump_file(hyperdos_win32_boot_state* bootState)
{
    FILE*                                  videoStateDumpFile = NULL;
    const hyperdos_color_graphics_adapter* adapter            = &bootState->machine.pc.colorGraphicsAdapter;
    int    sourceWidth  = (adapter->modeControl & HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_HIGH_RESOLUTION) != 0u
                                  ? HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_WIDTH_HIGH
                                  : HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_WIDTH_LOW;
    int    sourceHeight = HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_HEIGHT;
    int    graphicsPreviewAvailable                                          = 0;
    size_t planeNonzeroByteCounts[HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT] = {0u, 0u, 0u, 0u};
    size_t planeIndex                                                        = 0u;

    if (globalVideoStateDumpPath[0] == '\0')
    {
        return;
    }
    videoStateDumpFile = fopen(globalVideoStateDumpPath, "wb");
    if (videoStateDumpFile == NULL)
    {
        return;
    }

    for (planeIndex = 0u; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
    {
        size_t byteIndex = 0u;
        for (byteIndex = 0u; byteIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_SIZE; ++byteIndex)
        {
            if (adapter->graphicsPlanes[planeIndex][byteIndex] != 0u)
            {
                ++planeNonzeroByteCounts[planeIndex];
            }
        }
    }

    graphicsPreviewAvailable = prepare_video_preview_pixels(bootState, &sourceWidth, &sourceHeight);
    fprintf(videoStateDumpFile,
            "mode=%02X graphics=%u width=%d height=%d start=%04X stride-bytes=%u "
            "chain-four=%u plane-nonzero=%zu,%zu,%zu,%zu\n",
            bootState->machine.videoServices.videoMode,
            hyperdos_color_graphics_adapter_graphics_mode_is_enabled(adapter) ? 1u : 0u,
            sourceWidth,
            sourceHeight,
            hyperdos_color_graphics_adapter_get_display_start_address(adapter),
            hyperdos_color_graphics_adapter_get_display_stride_byte_count(adapter),
            (adapter->sequencerRegisters[4] & 0x08u) != 0u ? 1u : 0u,
            planeNonzeroByteCounts[0],
            planeNonzeroByteCounts[1],
            planeNonzeroByteCounts[2],
            planeNonzeroByteCounts[3]);
    write_video_register_dump_line(videoStateDumpFile, "sequencer", adapter->sequencerRegisters, 5u);
    write_video_register_dump_line(videoStateDumpFile, "graphics-controller", adapter->graphicsControllerRegisters, 9u);
    write_video_register_dump_line(videoStateDumpFile, "attribute", adapter->attributeControllerRegisters, 0x15u);
    write_video_register_dump_line(videoStateDumpFile, "crt-controller", adapter->registers, 0x19u);
    write_video_digital_to_analog_converter_palette_dump(videoStateDumpFile, adapter);
    if (graphicsPreviewAvailable && bootState->machine.videoServices.videoMode != 0x13u)
    {
        write_planar_video_graphics_array_color_index_dump(videoStateDumpFile,
                                                           adapter,
                                                           (uint32_t)sourceWidth,
                                                           (uint32_t)sourceHeight);
    }

    if (graphicsPreviewAvailable)
    {
        uint32_t previewRow = 0u;

        fputs("preview:\n", videoStateDumpFile);
        for (previewRow = 0u; previewRow < HYPERDOS_MONITOR_VIDEO_PREVIEW_HEIGHT; ++previewRow)
        {
            uint32_t previewColumn = 0u;
            uint32_t sourceRow     = (previewRow * (uint32_t)sourceHeight) / HYPERDOS_MONITOR_VIDEO_PREVIEW_HEIGHT;
            for (previewColumn = 0u; previewColumn < HYPERDOS_MONITOR_VIDEO_PREVIEW_WIDTH; ++previewColumn)
            {
                uint32_t sourceColumn = (previewColumn * (uint32_t)sourceWidth) / HYPERDOS_MONITOR_VIDEO_PREVIEW_WIDTH;
                COLORREF color        = globalGraphicsPixels[sourceRow * (uint32_t)sourceWidth + sourceColumn];
                fputc(get_video_preview_character_from_color(color), videoStateDumpFile);
            }
            fputc('\n', videoStateDumpFile);
        }
    }
    fclose(videoStateDumpFile);
}

static void maybe_write_video_state_dump_file(hyperdos_win32_boot_state* bootState)
{
    ULONGLONG currentTick = 0u;

    if (globalVideoStateDumpPath[0] == '\0')
    {
        return;
    }

    currentTick = GetTickCount64();
    if (bootState->lastVideoStateDumpTick == 0u ||
        currentTick - bootState->lastVideoStateDumpTick >= HYPERDOS_MONITOR_VIDEO_STATE_DUMP_PERIOD_MILLISECONDS)
    {
        write_video_state_dump_file(bootState);
        bootState->lastVideoStateDumpTick = currentTick;
    }
}

static void render_text_display_native(HDC deviceContext, const hyperdos_win32_boot_state* bootState)
{
    HFONT    previousFontHandle = (HFONT)SelectObject(deviceContext, globalTextFontHandle);
    HBRUSH   backgroundBrush    = (HBRUSH)GetStockObject(DC_BRUSH);
    uint32_t row                = 0;
    uint32_t column             = 0;

    SetTextAlign(deviceContext, TA_LEFT | TA_TOP | TA_NOUPDATECP);
    SetBkMode(deviceContext, TRANSPARENT);
    for (row = 0; row < HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT; ++row)
    {
        for (column = 0; column < HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT; ++column)
        {
            size_t memoryIndex = hyperdos_pc_video_services_get_text_page_cell_memory_index(
                    &bootState->machine.videoServices,
                    bootState->machine.videoServices.activeVideoPage,
                    (uint16_t)row,
                    (uint16_t)column);
            uint8_t  attribute = hyperdos_pc_video_services_read_text_memory_byte(&bootState->machine.videoServices,
                                                                                 memoryIndex + 1u);
            RECT     cellRectangle;
            COLORREF backgroundColor = 0;

            if ((uint16_t)row == bootState->machine.videoServices.cursorRow &&
                (uint16_t)column == bootState->machine.videoServices.cursorColumn)
            {
                backgroundColor = hyperdos_pc_video_services_color_graphics_adapter_color_from_index(
                        (uint8_t)(attribute & 0x0Fu));
            }
            else
            {
                backgroundColor = hyperdos_pc_video_services_color_graphics_adapter_color_from_index(
                        (uint8_t)(attribute >> 4u));
            }
            cellRectangle.left   = (LONG)column * HYPERDOS_MONITOR_CHARACTER_WIDTH;
            cellRectangle.top    = (LONG)row * HYPERDOS_MONITOR_CHARACTER_HEIGHT;
            cellRectangle.right  = cellRectangle.left + HYPERDOS_MONITOR_CHARACTER_WIDTH;
            cellRectangle.bottom = cellRectangle.top + HYPERDOS_MONITOR_CHARACTER_HEIGHT;
            SetDCBrushColor(deviceContext, backgroundColor);
            FillRect(deviceContext, &cellRectangle, backgroundBrush);
        }
    }
    for (row = 0; row < HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT; ++row)
    {
        for (column = 0; column < HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT; ++column)
        {
            size_t memoryIndex = hyperdos_pc_video_services_get_text_page_cell_memory_index(
                    &bootState->machine.videoServices,
                    bootState->machine.videoServices.activeVideoPage,
                    (uint16_t)row,
                    (uint16_t)column);
            uint8_t  characterByte = hyperdos_pc_video_services_read_text_memory_byte(&bootState->machine.videoServices,
                                                                                     memoryIndex);
            uint8_t  attribute     = hyperdos_pc_video_services_read_text_memory_byte(&bootState->machine.videoServices,
                                                                                 memoryIndex + 1u);
            WCHAR    wideCharacter = L' ';
            RECT     characterRectangle;
            COLORREF foregroundColor = 0;

            if ((uint16_t)row == bootState->machine.videoServices.cursorRow &&
                (uint16_t)column == bootState->machine.videoServices.cursorColumn)
            {
                foregroundColor = hyperdos_pc_video_services_color_graphics_adapter_color_from_index(
                        (uint8_t)(attribute >> 4u));
            }
            else
            {
                foregroundColor = hyperdos_pc_video_services_color_graphics_adapter_color_from_index(
                        (uint8_t)(attribute & 0x0Fu));
            }
            SetTextColor(deviceContext, foregroundColor);
            characterRectangle.left   = (LONG)column * HYPERDOS_MONITOR_CHARACTER_WIDTH;
            characterRectangle.top    = (LONG)row * HYPERDOS_MONITOR_CHARACTER_HEIGHT;
            characterRectangle.right  = characterRectangle.left + HYPERDOS_MONITOR_CHARACTER_WIDTH;
            characterRectangle.bottom = characterRectangle.top + HYPERDOS_MONITOR_CHARACTER_HEIGHT;
            if (try_read_korean_code_page_text_character(bootState, row, column, characterByte, &wideCharacter))
            {
                SelectObject(deviceContext, globalTextFontHandle);
                characterRectangle.right = characterRectangle.left + HYPERDOS_MONITOR_CHARACTER_WIDTH * 2;
                ExtTextOutW(deviceContext,
                            characterRectangle.left,
                            characterRectangle.top,
                            ETO_CLIPPED,
                            &characterRectangle,
                            &wideCharacter,
                            1,
                            NULL);
                ++column;
                continue;
            }
            if (globalCodePage437GlyphRowsInitialized)
            {
                render_code_page_437_character_cell(deviceContext,
                                                    characterRectangle.left,
                                                    characterRectangle.top,
                                                    characterByte,
                                                    foregroundColor);
            }
            else
            {
                char codePage437Character = (char)characterByte;
                SelectObject(deviceContext,
                             globalCodePage437TextFontHandle != NULL ? globalCodePage437TextFontHandle
                                                                     : globalTextFontHandle);
                ExtTextOutA(deviceContext,
                            characterRectangle.left,
                            characterRectangle.top,
                            ETO_CLIPPED,
                            &characterRectangle,
                            &codePage437Character,
                            1,
                            NULL);
            }
        }
    }
    SelectObject(deviceContext, previousFontHandle);
}

static uint32_t convert_color_reference_to_device_independent_bitmap_pixel(COLORREF color)
{
    return ((color & 0x000000FFu) << 16u) | (color & 0x0000FF00u) | ((color & 0x00FF0000u) >> 16u);
}

static void convert_graphics_pixels_to_device_independent_bitmap_pixels(uint32_t* pixels,
                                                                        int       sourceWidth,
                                                                        int       sourceHeight)
{
    size_t pixelCount = 0u;
    size_t pixelIndex = 0u;

    if (pixels == NULL || sourceWidth <= 0 || sourceHeight <= 0)
    {
        return;
    }

    pixelCount = (size_t)sourceWidth * (size_t)sourceHeight;
    for (pixelIndex = 0u; pixelIndex < pixelCount; ++pixelIndex)
    {
        pixels[pixelIndex] = convert_color_reference_to_device_independent_bitmap_pixel(pixels[pixelIndex]);
    }
}

static void get_display_source_dimensions(const hyperdos_win32_boot_state* bootState,
                                          int*                             sourceWidth,
                                          int*                             sourceHeight)
{
    const hyperdos_color_graphics_adapter* adapter = &bootState->machine.pc.colorGraphicsAdapter;

    if (hyperdos_color_graphics_adapter_graphics_mode_is_enabled(adapter))
    {
        int graphicsSourceWidth  = (adapter->modeControl & HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_HIGH_RESOLUTION) != 0u
                                           ? HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_WIDTH_HIGH
                                           : HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_WIDTH_LOW;
        int graphicsSourceHeight = HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_HEIGHT;

        (void)hyperdos_pc_video_services_get_video_graphics_array_mode_dimensions(
                bootState->machine.videoServices.videoMode,
                &graphicsSourceWidth,
                &graphicsSourceHeight);
        *sourceWidth  = graphicsSourceWidth;
        *sourceHeight = graphicsSourceHeight;
        return;
    }

    *sourceWidth  = HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT * HYPERDOS_MONITOR_CHARACTER_WIDTH;
    *sourceHeight = HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT * HYPERDOS_MONITOR_CHARACTER_HEIGHT;
}

static void calculate_fit_to_window_display_size(int  sourceWidth,
                                                 int  sourceHeight,
                                                 int  clientWidth,
                                                 int  clientHeight,
                                                 int* destinationWidth,
                                                 int* destinationHeight)
{
    if (sourceWidth <= 0 || sourceHeight <= 0 || clientWidth <= 0 || clientHeight <= 0)
    {
        *destinationWidth  = 0;
        *destinationHeight = 0;
        return;
    }

    if ((int64_t)clientWidth * (int64_t)sourceHeight <= (int64_t)clientHeight * (int64_t)sourceWidth)
    {
        *destinationWidth  = clientWidth;
        *destinationHeight = (clientWidth * sourceHeight) / sourceWidth;
    }
    else
    {
        *destinationHeight = clientHeight;
        *destinationWidth  = (clientHeight * sourceWidth) / sourceHeight;
    }
    if (*destinationWidth <= 0)
    {
        *destinationWidth = 1;
    }
    if (*destinationHeight <= 0)
    {
        *destinationHeight = 1;
    }
}

static void calculate_display_destination_rectangle(const RECT* clientRectangle,
                                                    int         sourceWidth,
                                                    int         sourceHeight,
                                                    RECT*       destinationRectangle)
{
    int clientWidth       = clientRectangle->right - clientRectangle->left;
    int clientHeight      = clientRectangle->bottom - clientRectangle->top;
    int destinationWidth  = sourceWidth;
    int destinationHeight = sourceHeight;

    if (globalDisplayResizeMode == HYPERDOS_MONITOR_DISPLAY_RESIZE_MODE_FIT_TO_WINDOW)
    {
        calculate_fit_to_window_display_size(sourceWidth,
                                             sourceHeight,
                                             clientWidth,
                                             clientHeight,
                                             &destinationWidth,
                                             &destinationHeight);
    }
    else if (globalDisplayResizeMode == HYPERDOS_MONITOR_DISPLAY_RESIZE_MODE_INTEGER_SCALE)
    {
        int horizontalScale = sourceWidth > 0 ? clientWidth / sourceWidth : 0;
        int verticalScale   = sourceHeight > 0 ? clientHeight / sourceHeight : 0;

        if (horizontalScale > 0 && verticalScale > 0)
        {
            int integerScale = horizontalScale < verticalScale ? horizontalScale : verticalScale;

            destinationWidth  = sourceWidth * integerScale;
            destinationHeight = sourceHeight * integerScale;
        }
        else
        {
            calculate_fit_to_window_display_size(sourceWidth,
                                                 sourceHeight,
                                                 clientWidth,
                                                 clientHeight,
                                                 &destinationWidth,
                                                 &destinationHeight);
        }
    }

    destinationRectangle->left   = clientRectangle->left + (clientWidth - destinationWidth) / 2;
    destinationRectangle->top    = clientRectangle->top + (clientHeight - destinationHeight) / 2;
    destinationRectangle->right  = destinationRectangle->left + destinationWidth;
    destinationRectangle->bottom = destinationRectangle->top + destinationHeight;
}

static void get_display_geometry(const hyperdos_win32_boot_state* bootState,
                                 const RECT*                      clientRectangle,
                                 RECT*                            destinationRectangle,
                                 int*                             sourceWidth,
                                 int*                             sourceHeight)
{
    get_display_source_dimensions(bootState, sourceWidth, sourceHeight);
    calculate_display_destination_rectangle(clientRectangle, *sourceWidth, *sourceHeight, destinationRectangle);
}

static void render_text_display(HDC                              deviceContext,
                                const hyperdos_win32_boot_state* bootState,
                                const RECT*                      clientRectangle)
{
    RECT    destinationRectangle;
    int     sourceWidth         = 0;
    int     sourceHeight        = 0;
    HDC     memoryDeviceContext = NULL;
    HBITMAP sourceBitmap        = NULL;
    HGDIOBJ previousBitmap      = NULL;

    get_display_geometry(bootState, clientRectangle, &destinationRectangle, &sourceWidth, &sourceHeight);
    if (sourceWidth <= 0 || sourceHeight <= 0 || destinationRectangle.right <= destinationRectangle.left ||
        destinationRectangle.bottom <= destinationRectangle.top)
    {
        return;
    }

    memoryDeviceContext = CreateCompatibleDC(deviceContext);
    if (memoryDeviceContext == NULL)
    {
        return;
    }
    sourceBitmap = CreateCompatibleBitmap(deviceContext, sourceWidth, sourceHeight);
    if (sourceBitmap == NULL)
    {
        DeleteDC(memoryDeviceContext);
        return;
    }
    previousBitmap = SelectObject(memoryDeviceContext, sourceBitmap);
    render_text_display_native(memoryDeviceContext, bootState);
    SetStretchBltMode(deviceContext, COLORONCOLOR);
    StretchBlt(deviceContext,
               destinationRectangle.left,
               destinationRectangle.top,
               destinationRectangle.right - destinationRectangle.left,
               destinationRectangle.bottom - destinationRectangle.top,
               memoryDeviceContext,
               0,
               0,
               sourceWidth,
               sourceHeight,
               SRCCOPY);
    SelectObject(memoryDeviceContext, previousBitmap);
    DeleteObject(sourceBitmap);
    DeleteDC(memoryDeviceContext);
}

static void render_graphics_display(HDC                              deviceContext,
                                    const hyperdos_win32_boot_state* bootState,
                                    const RECT*                      clientRectangle)
{
    const hyperdos_color_graphics_adapter* adapter = &bootState->machine.pc.colorGraphicsAdapter;
    BITMAPINFO                             bitmapInfo;
    RECT                                   destinationRectangle;
    int sourceWidth     = (adapter->modeControl & HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_HIGH_RESOLUTION) != 0u
                                  ? HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_WIDTH_HIGH
                                  : HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_WIDTH_LOW;
    int sourceHeight    = HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_HEIGHT;
    int vgaGraphicsMode = hyperdos_pc_video_services_get_video_graphics_array_mode_dimensions(
            bootState->machine.videoServices.videoMode,
            &sourceWidth,
            &sourceHeight);

    memset(&bitmapInfo, 0, sizeof(bitmapInfo));
    bitmapInfo.bmiHeader.biSize        = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth       = sourceWidth;
    bitmapInfo.bmiHeader.biHeight      = -sourceHeight;
    bitmapInfo.bmiHeader.biPlanes      = 1;
    bitmapInfo.bmiHeader.biBitCount    = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    if (vgaGraphicsMode && bootState->machine.videoServices.videoMode == 0x13u)
    {
        hyperdos_pc_video_services_render_linear_256_color_video_graphics_array_pixels(adapter,
                                                                                       sourceWidth,
                                                                                       sourceHeight,
                                                                                       globalGraphicsPixels);
    }
    else if (vgaGraphicsMode)
    {
        hyperdos_pc_video_services_render_planar_video_graphics_array_pixels(adapter,
                                                                             bootState->machine.videoServices.videoMode,
                                                                             sourceWidth,
                                                                             sourceHeight,
                                                                             globalGraphicsPixels);
    }
    else if (sourceWidth == HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_WIDTH_HIGH)
    {
        hyperdos_pc_video_services_render_high_resolution_graphics_pixels(adapter, globalGraphicsPixels);
    }
    else
    {
        hyperdos_pc_video_services_render_low_resolution_graphics_pixels(adapter, globalGraphicsPixels);
    }
    convert_graphics_pixels_to_device_independent_bitmap_pixels(globalGraphicsPixels, sourceWidth, sourceHeight);
    get_display_geometry(bootState, clientRectangle, &destinationRectangle, &sourceWidth, &sourceHeight);

    SetStretchBltMode(deviceContext, COLORONCOLOR);
    StretchDIBits(deviceContext,
                  destinationRectangle.left,
                  destinationRectangle.top,
                  destinationRectangle.right - destinationRectangle.left,
                  destinationRectangle.bottom - destinationRectangle.top,
                  0,
                  0,
                  sourceWidth,
                  sourceHeight,
                  globalGraphicsPixels,
                  &bitmapInfo,
                  DIB_RGB_COLORS,
                  SRCCOPY);
}

static void render_display(HDC deviceContext, const hyperdos_win32_boot_state* bootState, const RECT* clientRectangle)
{
    const hyperdos_color_graphics_adapter* adapter = &bootState->machine.pc.colorGraphicsAdapter;

    if (hyperdos_color_graphics_adapter_graphics_mode_is_enabled(adapter))
    {
        render_graphics_display(deviceContext, bootState, clientRectangle);
    }
    else
    {
        render_text_display(deviceContext, bootState, clientRectangle);
    }
}

static COLORREF get_display_padding_color(void)
{
    return RGB(24, 28, 34);
}

static COLORREF get_display_border_color(void)
{
    return RGB(91, 111, 134);
}

static void fill_display_padding(HDC deviceContext, const RECT* clientRectangle)
{
    HBRUSH backgroundBrush = (HBRUSH)GetStockObject(DC_BRUSH);

    SetDCBrushColor(deviceContext, get_display_padding_color());
    FillRect(deviceContext, clientRectangle, backgroundBrush);
}

static void fill_display_border_segment(HDC         deviceContext,
                                        const RECT* clientRectangle,
                                        const RECT* borderRectangle,
                                        HBRUSH      borderBrush)
{
    RECT clippedRectangle;

    if (IntersectRect(&clippedRectangle, clientRectangle, borderRectangle))
    {
        FillRect(deviceContext, &clippedRectangle, borderBrush);
    }
}

static void draw_display_border(HDC                              deviceContext,
                                const hyperdos_win32_boot_state* bootState,
                                const RECT*                      clientRectangle)
{
    RECT   destinationRectangle;
    RECT   borderRectangle;
    int    sourceWidth     = 0;
    int    sourceHeight    = 0;
    HBRUSH borderBrush     = (HBRUSH)GetStockObject(DC_BRUSH);
    LONG   borderThickness = HYPERDOS_MONITOR_DISPLAY_BORDER_THICKNESS;

    get_display_geometry(bootState, clientRectangle, &destinationRectangle, &sourceWidth, &sourceHeight);
    if (sourceWidth <= 0 || sourceHeight <= 0 || destinationRectangle.right <= destinationRectangle.left ||
        destinationRectangle.bottom <= destinationRectangle.top)
    {
        return;
    }

    if (borderBrush == NULL)
    {
        return;
    }

    SetDCBrushColor(deviceContext, get_display_border_color());

    borderRectangle.left   = destinationRectangle.left - borderThickness;
    borderRectangle.top    = destinationRectangle.top - borderThickness;
    borderRectangle.right  = destinationRectangle.right + borderThickness;
    borderRectangle.bottom = destinationRectangle.top;
    fill_display_border_segment(deviceContext, clientRectangle, &borderRectangle, borderBrush);

    borderRectangle.left   = destinationRectangle.left - borderThickness;
    borderRectangle.top    = destinationRectangle.top;
    borderRectangle.right  = destinationRectangle.left;
    borderRectangle.bottom = destinationRectangle.bottom;
    fill_display_border_segment(deviceContext, clientRectangle, &borderRectangle, borderBrush);

    borderRectangle.left   = destinationRectangle.right;
    borderRectangle.top    = destinationRectangle.top;
    borderRectangle.right  = destinationRectangle.right + borderThickness;
    borderRectangle.bottom = destinationRectangle.bottom;
    fill_display_border_segment(deviceContext, clientRectangle, &borderRectangle, borderBrush);

    borderRectangle.left   = destinationRectangle.left - borderThickness;
    borderRectangle.top    = destinationRectangle.bottom;
    borderRectangle.right  = destinationRectangle.right + borderThickness;
    borderRectangle.bottom = destinationRectangle.bottom + borderThickness;
    fill_display_border_segment(deviceContext, clientRectangle, &borderRectangle, borderBrush);
}

static void render_display_surface(HDC                              deviceContext,
                                   const hyperdos_win32_boot_state* bootState,
                                   const RECT*                      clientRectangle)
{
    fill_display_padding(deviceContext, clientRectangle);
    render_display(deviceContext, bootState, clientRectangle);
    draw_display_border(deviceContext, bootState, clientRectangle);
}

static void render_display_to_window(HDC                              windowDeviceContext,
                                     const hyperdos_win32_boot_state* bootState,
                                     const RECT*                      clientRectangle)
{
    int     clientWidth         = clientRectangle->right - clientRectangle->left;
    int     clientHeight        = clientRectangle->bottom - clientRectangle->top;
    HDC     memoryDeviceContext = NULL;
    HBITMAP backBufferBitmap    = NULL;
    HGDIOBJ previousBitmap      = NULL;

    if (clientWidth <= 0 || clientHeight <= 0)
    {
        return;
    }

    memoryDeviceContext = CreateCompatibleDC(windowDeviceContext);
    if (memoryDeviceContext == NULL)
    {
        render_display_surface(windowDeviceContext, bootState, clientRectangle);
        return;
    }
    backBufferBitmap = CreateCompatibleBitmap(windowDeviceContext, clientWidth, clientHeight);
    if (backBufferBitmap == NULL)
    {
        DeleteDC(memoryDeviceContext);
        render_display_surface(windowDeviceContext, bootState, clientRectangle);
        return;
    }
    previousBitmap = SelectObject(memoryDeviceContext, backBufferBitmap);
    render_display_surface(memoryDeviceContext, bootState, clientRectangle);
    BitBlt(windowDeviceContext, 0, 0, clientWidth, clientHeight, memoryDeviceContext, 0, 0, SRCCOPY);
    SelectObject(memoryDeviceContext, previousBitmap);
    DeleteObject(backBufferBitmap);
    DeleteDC(memoryDeviceContext);
}

static int get_mouse_message_horizontal_position(LPARAM longParameter)
{
    return (int)(short)(longParameter & 0xFFFFu);
}

static int get_mouse_message_vertical_position(LPARAM longParameter)
{
    return (int)(short)(((uint32_t)longParameter >> 16u) & 0xFFFFu);
}

static uint8_t get_mouse_button_mask_from_word_parameter(WPARAM wordParameter)
{
    uint8_t buttonMask = 0u;

    if ((wordParameter & MK_LBUTTON) != 0u)
    {
        buttonMask |= HYPERDOS_MONITOR_MOUSE_LEFT_BUTTON;
    }
    if ((wordParameter & MK_RBUTTON) != 0u)
    {
        buttonMask |= HYPERDOS_MONITOR_MOUSE_RIGHT_BUTTON;
    }
    if ((wordParameter & MK_MBUTTON) != 0u)
    {
        buttonMask |= HYPERDOS_MONITOR_MOUSE_MIDDLE_BUTTON;
    }
    return buttonMask;
}

static uint8_t get_mouse_button_mask_from_message(UINT message, WPARAM wordParameter, uint8_t previousButtonMask)
{
    uint8_t buttonMask = get_mouse_button_mask_from_word_parameter(wordParameter);

    if (message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK)
    {
        buttonMask |= HYPERDOS_MONITOR_MOUSE_LEFT_BUTTON;
    }
    else if (message == WM_LBUTTONUP)
    {
        buttonMask &= (uint8_t)~HYPERDOS_MONITOR_MOUSE_LEFT_BUTTON;
    }
    else if (message == WM_RBUTTONDOWN || message == WM_RBUTTONDBLCLK)
    {
        buttonMask |= HYPERDOS_MONITOR_MOUSE_RIGHT_BUTTON;
    }
    else if (message == WM_RBUTTONUP)
    {
        buttonMask &= (uint8_t)~HYPERDOS_MONITOR_MOUSE_RIGHT_BUTTON;
    }
    else if (message == WM_MBUTTONDOWN || message == WM_MBUTTONDBLCLK)
    {
        buttonMask |= HYPERDOS_MONITOR_MOUSE_MIDDLE_BUTTON;
    }
    else if (message == WM_MBUTTONUP)
    {
        buttonMask &= (uint8_t)~HYPERDOS_MONITOR_MOUSE_MIDDLE_BUTTON;
    }
    else if (message != WM_MOUSEMOVE)
    {
        buttonMask = previousButtonMask;
    }
    return buttonMask;
}

static int mouse_button_down_message_is(UINT message)
{
    return message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK || message == WM_RBUTTONDOWN ||
           message == WM_RBUTTONDBLCLK || message == WM_MBUTTONDOWN || message == WM_MBUTTONDBLCLK;
}

static int client_position_is_inside_display(const hyperdos_win32_boot_state* bootState,
                                             HWND                             windowHandle,
                                             int                              clientHorizontalPosition,
                                             int                              clientVerticalPosition)
{
    RECT clientRectangle;
    RECT displayRectangle;
    int  sourceWidth       = 0;
    int  sourceHeight      = 0;
    int  destinationWidth  = 0;
    int  destinationHeight = 0;

    GetClientRect(windowHandle, &clientRectangle);
    get_display_geometry(bootState, &clientRectangle, &displayRectangle, &sourceWidth, &sourceHeight);
    destinationWidth  = displayRectangle.right - displayRectangle.left;
    destinationHeight = displayRectangle.bottom - displayRectangle.top;
    return sourceWidth > 0 && sourceHeight > 0 && destinationWidth > 0 && destinationHeight > 0 &&
           clientHorizontalPosition >= displayRectangle.left && clientVerticalPosition >= displayRectangle.top &&
           clientHorizontalPosition < displayRectangle.right && clientVerticalPosition < displayRectangle.bottom;
}

static int get_display_clipping_rectangle(HWND                             windowHandle,
                                          const hyperdos_win32_boot_state* bootState,
                                          RECT*                            displayClippingRectangle)
{
    RECT  clientRectangle;
    RECT  displayRectangle;
    RECT  visibleDisplayRectangle;
    POINT displayUpperLeftPoint;
    POINT displayLowerRightPoint;
    int   sourceWidth       = 0;
    int   sourceHeight      = 0;
    int   destinationWidth  = 0;
    int   destinationHeight = 0;

    if (bootState == NULL || displayClippingRectangle == NULL)
    {
        return 0;
    }

    GetClientRect(windowHandle, &clientRectangle);
    get_display_geometry(bootState, &clientRectangle, &displayRectangle, &sourceWidth, &sourceHeight);
    destinationWidth  = displayRectangle.right - displayRectangle.left;
    destinationHeight = displayRectangle.bottom - displayRectangle.top;
    if (sourceWidth <= 0 || sourceHeight <= 0 || destinationWidth <= 0 || destinationHeight <= 0)
    {
        return 0;
    }
    if (!IntersectRect(&visibleDisplayRectangle, &displayRectangle, &clientRectangle))
    {
        return 0;
    }

    displayUpperLeftPoint.x  = visibleDisplayRectangle.left;
    displayUpperLeftPoint.y  = visibleDisplayRectangle.top;
    displayLowerRightPoint.x = visibleDisplayRectangle.right;
    displayLowerRightPoint.y = visibleDisplayRectangle.bottom;
    if (!ClientToScreen(windowHandle, &displayUpperLeftPoint) || !ClientToScreen(windowHandle, &displayLowerRightPoint))
    {
        return 0;
    }

    displayClippingRectangle->left   = displayUpperLeftPoint.x;
    displayClippingRectangle->top    = displayUpperLeftPoint.y;
    displayClippingRectangle->right  = displayLowerRightPoint.x;
    displayClippingRectangle->bottom = displayLowerRightPoint.y;
    return displayClippingRectangle->right > displayClippingRectangle->left &&
           displayClippingRectangle->bottom > displayClippingRectangle->top;
}

static int get_display_capture_geometry(HWND                             windowHandle,
                                        const hyperdos_win32_boot_state* bootState,
                                        POINT*                           clientCenterPoint,
                                        POINT*                           screenCenterPoint,
                                        int*                             sourceWidth,
                                        int*                             sourceHeight,
                                        int*                             destinationWidth,
                                        int*                             destinationHeight)
{
    RECT clientRectangle;
    RECT displayRectangle;
    int  currentSourceWidth  = 0;
    int  currentSourceHeight = 0;

    if (clientCenterPoint == NULL || screenCenterPoint == NULL || sourceWidth == NULL || sourceHeight == NULL ||
        destinationWidth == NULL || destinationHeight == NULL)
    {
        return 0;
    }

    GetClientRect(windowHandle, &clientRectangle);
    get_display_geometry(bootState, &clientRectangle, &displayRectangle, &currentSourceWidth, &currentSourceHeight);
    *destinationWidth  = displayRectangle.right - displayRectangle.left;
    *destinationHeight = displayRectangle.bottom - displayRectangle.top;
    if (currentSourceWidth <= 0 || currentSourceHeight <= 0 || *destinationWidth <= 0 || *destinationHeight <= 0 ||
        displayRectangle.right <= displayRectangle.left || displayRectangle.bottom <= displayRectangle.top)
    {
        return 0;
    }

    clientCenterPoint->x = displayRectangle.left + (displayRectangle.right - displayRectangle.left) / 2;
    clientCenterPoint->y = displayRectangle.top + (displayRectangle.bottom - displayRectangle.top) / 2;
    *screenCenterPoint   = *clientCenterPoint;
    *sourceWidth         = currentSourceWidth;
    *sourceHeight        = currentSourceHeight;
    return ClientToScreen(windowHandle, screenCenterPoint) != FALSE;
}

static int center_host_mouse_capture_cursor(HWND windowHandle, const hyperdos_win32_boot_state* bootState)
{
    POINT clientCenterPoint;
    POINT screenCenterPoint;
    int   sourceWidth       = 0;
    int   sourceHeight      = 0;
    int   destinationWidth  = 0;
    int   destinationHeight = 0;

    if (bootState == NULL || bootState->hostMouseCaptureActive == 0u)
    {
        return 1;
    }
    if (!get_display_capture_geometry(windowHandle,
                                      bootState,
                                      &clientCenterPoint,
                                      &screenCenterPoint,
                                      &sourceWidth,
                                      &sourceHeight,
                                      &destinationWidth,
                                      &destinationHeight))
    {
        return 0;
    }
    return SetCursorPos(screenCenterPoint.x, screenCenterPoint.y) != FALSE;
}

static int host_mouse_capture_uses_message_movement(const hyperdos_win32_boot_state* bootState)
{
    return bootState != NULL && bootState->hostMouseRawInputRegistered == 0u;
}

static void update_mouse_capture_menu(HWND windowHandle, const hyperdos_win32_boot_state* bootState)
{
    HMENU menuHandle = GetMenu(windowHandle);

    if (menuHandle == NULL)
    {
        return;
    }
    CheckMenuItem(menuHandle,
                  HYPERDOS_MONITOR_COMMAND_TOGGLE_MOUSE_CAPTURE,
                  MF_BYCOMMAND | (bootState->hostMouseCaptureActive != 0u ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(menuHandle,
                  HYPERDOS_MONITOR_COMMAND_TOGGLE_HOST_MOUSE_CURSOR_CONFINEMENT,
                  MF_BYCOMMAND | (bootState->hostMouseCursorConfinementEnabled != 0u ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(menuHandle,
                  HYPERDOS_MONITOR_COMMAND_TOGGLE_HOST_MOUSE_CURSOR_HIDING,
                  MF_BYCOMMAND | (bootState->hostMouseCursorHidingEnabled != 0u ? MF_CHECKED : MF_UNCHECKED));
    DrawMenuBar(windowHandle);
}

static int mouse_capture_clipping_rectangles_are_equal(const RECT* firstRectangle, const RECT* secondRectangle)
{
    return firstRectangle != NULL && secondRectangle != NULL && firstRectangle->left == secondRectangle->left &&
           firstRectangle->top == secondRectangle->top && firstRectangle->right == secondRectangle->right &&
           firstRectangle->bottom == secondRectangle->bottom;
}

static int host_mouse_capture_clipping_is_current(const RECT* displayClippingRectangle)
{
    RECT currentClippingRectangle;

    if (displayClippingRectangle == NULL)
    {
        return 0;
    }
    if (GetClipCursor(&currentClippingRectangle) == 0)
    {
        return 0;
    }
    return mouse_capture_clipping_rectangles_are_equal(&currentClippingRectangle, displayClippingRectangle);
}

static int update_host_mouse_capture_clipping(HWND windowHandle, hyperdos_win32_boot_state* bootState)
{
    RECT displayClippingRectangle;

    if (bootState == NULL || bootState->hostMouseCaptureActive == 0u ||
        bootState->hostMouseCursorConfinementEnabled == 0u)
    {
        return 1;
    }
    if (!get_display_clipping_rectangle(windowHandle, bootState, &displayClippingRectangle))
    {
        return 0;
    }
    if (host_mouse_capture_clipping_is_current(&displayClippingRectangle))
    {
        return 1;
    }
    if (ClipCursor(&displayClippingRectangle) == 0)
    {
        trace_disk_event(bootState,
                         "host mouse clipping failed left=%ld top=%ld right=%ld bottom=%ld error=%lu",
                         (long)displayClippingRectangle.left,
                         (long)displayClippingRectangle.top,
                         (long)displayClippingRectangle.right,
                         (long)displayClippingRectangle.bottom,
                         GetLastError());
        return 0;
    }
    return 1;
}

static void release_host_mouse_capture_clipping(hyperdos_win32_boot_state* bootState)
{
    if (ClipCursor(NULL) == 0)
    {
        trace_disk_event(bootState, "host mouse clipping release failed error=%lu", GetLastError());
    }
}

static void set_host_mouse_capture_cursor(const hyperdos_win32_boot_state* bootState)
{
    if (bootState == NULL || bootState->hostMouseCursorHidingEnabled == 0u)
    {
        SetCursor(LoadCursorA(NULL, IDC_ARROW));
        return;
    }
    SetCursor(NULL);
}

static void reset_host_mouse_capture_motion_state(hyperdos_win32_boot_state* bootState);

static int capture_host_mouse(HWND windowHandle, hyperdos_win32_boot_state* bootState)
{
    if (bootState == NULL)
    {
        return 0;
    }
    if (bootState->hostMouseCaptureActive != 0u)
    {
        return update_host_mouse_capture_clipping(windowHandle, bootState) &&
               (!host_mouse_capture_uses_message_movement(bootState) ||
                center_host_mouse_capture_cursor(windowHandle, bootState));
    }

    bootState->hostMouseCaptureActive = 1u;
    reset_host_mouse_capture_motion_state(bootState);
    if (!update_host_mouse_capture_clipping(windowHandle, bootState))
    {
        bootState->hostMouseCaptureActive = 0u;
        release_host_mouse_capture_clipping(bootState);
        return 0;
    }
    SetCapture(windowHandle);
    set_host_mouse_capture_cursor(bootState);
    if (host_mouse_capture_uses_message_movement(bootState))
    {
        (void)center_host_mouse_capture_cursor(windowHandle, bootState);
    }
    update_mouse_capture_menu(windowHandle, bootState);
    return 1;
}

static void release_host_mouse_capture(HWND windowHandle, hyperdos_win32_boot_state* bootState)
{
    if (bootState == NULL)
    {
        return;
    }
    if (bootState->hostMouseCaptureActive == 0u)
    {
        return;
    }
    bootState->hostMouseCaptureActive = 0u;
    reset_host_mouse_capture_motion_state(bootState);
    release_host_mouse_capture_clipping(bootState);
    if (GetCapture() == windowHandle)
    {
        ReleaseCapture();
    }
    SetCursor(LoadCursorA(NULL, IDC_ARROW));
    update_mouse_capture_menu(windowHandle, bootState);
}

static int host_mouse_capture_toggle_shortcut_is_down(WPARAM wordParameter)
{
    return wordParameter == VK_F10 &&
           ((GetKeyState(VK_CONTROL) & 0x8000) != 0 || (GetKeyState(VK_LCONTROL) & 0x8000) != 0 ||
            (GetKeyState(VK_RCONTROL) & 0x8000) != 0);
}

static void toggle_host_mouse_capture(HWND windowHandle, hyperdos_win32_boot_state* bootState)
{
    if (bootState->hostMouseCaptureActive != 0u)
    {
        release_host_mouse_capture(windowHandle, bootState);
        return;
    }
    (void)capture_host_mouse(windowHandle, bootState);
}

static void toggle_host_mouse_cursor_confinement(HWND windowHandle, hyperdos_win32_boot_state* bootState)
{
    if (bootState == NULL)
    {
        return;
    }
    bootState->hostMouseCursorConfinementEnabled = bootState->hostMouseCursorConfinementEnabled != 0u ? 0u : 1u;
    if (bootState->hostMouseCursorConfinementEnabled != 0u)
    {
        (void)update_host_mouse_capture_clipping(windowHandle, bootState);
    }
    else
    {
        release_host_mouse_capture_clipping(bootState);
    }
    update_mouse_capture_menu(windowHandle, bootState);
}

static void toggle_host_mouse_cursor_hiding(HWND windowHandle, hyperdos_win32_boot_state* bootState)
{
    if (bootState == NULL)
    {
        return;
    }
    bootState->hostMouseCursorHidingEnabled = bootState->hostMouseCursorHidingEnabled != 0u ? 0u : 1u;
    if (bootState->hostMouseCaptureActive != 0u)
    {
        set_host_mouse_capture_cursor(bootState);
    }
    update_mouse_capture_menu(windowHandle, bootState);
}

static int register_host_mouse_raw_input(HWND windowHandle)
{
    RAWINPUTDEVICE rawInputDevice;

    memset(&rawInputDevice, 0, sizeof(rawInputDevice));
    rawInputDevice.usUsagePage = HYPERDOS_MONITOR_RAW_INPUT_USAGE_PAGE_GENERIC;
    rawInputDevice.usUsage     = HYPERDOS_MONITOR_RAW_INPUT_USAGE_MOUSE;
    rawInputDevice.dwFlags     = HYPERDOS_MONITOR_RAW_INPUT_DEVICE_INPUT_SINK;
    rawInputDevice.hwndTarget  = windowHandle;
    return RegisterRawInputDevices(&rawInputDevice, 1u, sizeof(rawInputDevice)) != FALSE;
}

static int16_t clamp_monitor_mouse_movement(int movement)
{
    if (movement < HYPERDOS_MONITOR_HOST_MOUSE_PACKET_MOVEMENT_MINIMUM)
    {
        return HYPERDOS_MONITOR_HOST_MOUSE_PACKET_MOVEMENT_MINIMUM;
    }
    if (movement > HYPERDOS_MONITOR_HOST_MOUSE_PACKET_MOVEMENT_MAXIMUM)
    {
        return HYPERDOS_MONITOR_HOST_MOUSE_PACKET_MOVEMENT_MAXIMUM;
    }
    return (int16_t)movement;
}

static int scale_mouse_capture_axis_movement(int hostMovement, int sourceLength, int destinationLength)
{
    if (sourceLength <= 0 || destinationLength <= 0)
    {
        return hostMovement;
    }
    return (int)(((int64_t)hostMovement * sourceLength) / destinationLength);
}

static int mouse_capture_message_movement_should_run(const hyperdos_win32_boot_state* bootState)
{
    return bootState != NULL &&
           (bootState->hostMouseRawInputRegistered == 0u || bootState->hostMouseRawMovementObserved == 0u);
}

static int get_current_client_mouse_position(HWND windowHandle,
                                             int* clientHorizontalPosition,
                                             int* clientVerticalPosition)
{
    POINT screenPoint;

    if (clientHorizontalPosition == NULL || clientVerticalPosition == NULL)
    {
        return 0;
    }
    if (!GetCursorPos(&screenPoint) || !ScreenToClient(windowHandle, &screenPoint))
    {
        return 0;
    }

    *clientHorizontalPosition = screenPoint.x;
    *clientVerticalPosition   = screenPoint.y;
    return 1;
}

static void reset_host_mouse_capture_motion_state(hyperdos_win32_boot_state* bootState)
{
    bootState->mouseCaptureLastClientHorizontalPosition = 0;
    bootState->mouseCaptureLastClientVerticalPosition   = 0;
    bootState->mouseCaptureLastClientPositionValid      = 0u;
    bootState->hostMouseRawMovementObserved             = 0u;
}

static void push_host_mouse_movement(hyperdos_win32_boot_state* bootState,
                                     int                        horizontalMovement,
                                     int                        verticalMovement,
                                     uint8_t                    buttonMask,
                                     int                        forcePacket)
{
    while (horizontalMovement != 0 || verticalMovement != 0 || forcePacket)
    {
        int16_t packetHorizontalMovement = clamp_monitor_mouse_movement(horizontalMovement);
        int16_t packetVerticalMovement   = clamp_monitor_mouse_movement(verticalMovement);

        if (!push_host_mouse_event(bootState,
                                   packetHorizontalMovement,
                                   packetVerticalMovement,
                                   buttonMask,
                                   forcePacket))
        {
            return;
        }
        horizontalMovement = 0;
        verticalMovement   = 0;
        forcePacket        = 0;
    }
}

static void handle_host_mouse_input(HWND                       windowHandle,
                                    hyperdos_win32_boot_state* bootState,
                                    UINT                       message,
                                    WPARAM                     wordParameter,
                                    LPARAM                     longParameter)
{
    int     clientHorizontalPosition = get_mouse_message_horizontal_position(longParameter);
    int     clientVerticalPosition   = get_mouse_message_vertical_position(longParameter);
    int     positionInsideDisplay    = client_position_is_inside_display(bootState,
                                                                  windowHandle,
                                                                  clientHorizontalPosition,
                                                                  clientVerticalPosition);
    uint8_t physicalButtonMask = get_mouse_button_mask_from_message(message, wordParameter, bootState->mouseButtonMask);
    uint8_t buttonMask         = physicalButtonMask;
    int     buttonChanged      = 0;
    int     buttonDownMessage  = mouse_button_down_message_is(message);

    if (bootState->hostMouseCaptureActive == 0u)
    {
        bootState->mouseButtonMask = 0u;
        if (!positionInsideDisplay || !buttonDownMessage)
        {
            return;
        }
        if (!capture_host_mouse(windowHandle, bootState))
        {
            return;
        }
        bootState->mouseButtonMask = buttonMask;
        if (buttonMask != 0u)
        {
            push_host_mouse_movement(bootState, 0, 0, buttonMask, 1);
            SetCapture(windowHandle);
        }
        return;
    }

    if (bootState->hostMouseCaptureActive != 0u)
    {
        POINT clientCenterPoint;
        POINT screenCenterPoint;
        int   sourceWidth            = 0;
        int   sourceHeight           = 0;
        int   destinationWidth       = 0;
        int   destinationHeight      = 0;
        int   hostHorizontalMovement = 0;
        int   hostVerticalMovement   = 0;
        int   horizontalMovement     = 0;
        int   verticalMovement       = 0;

        buttonMask    = physicalButtonMask;
        buttonChanged = buttonMask != bootState->mouseButtonMask;
        if (buttonChanged)
        {
            bootState->mouseCaptureLastClientPositionValid = 0u;
        }

        if (get_display_capture_geometry(windowHandle,
                                         bootState,
                                         &clientCenterPoint,
                                         &screenCenterPoint,
                                         &sourceWidth,
                                         &sourceHeight,
                                         &destinationWidth,
                                         &destinationHeight))
        {
            if (message == WM_MOUSEMOVE && mouse_capture_message_movement_should_run(bootState))
            {
                (void)get_current_client_mouse_position(windowHandle,
                                                        &clientHorizontalPosition,
                                                        &clientVerticalPosition);
                if (bootState->mouseCaptureLastClientPositionValid != 0u)
                {
                    hostHorizontalMovement = clientHorizontalPosition -
                                             bootState->mouseCaptureLastClientHorizontalPosition;
                    hostVerticalMovement = bootState->mouseCaptureLastClientVerticalPosition - clientVerticalPosition;
                    horizontalMovement   = scale_mouse_capture_axis_movement(hostHorizontalMovement,
                                                                           sourceWidth,
                                                                           destinationWidth);
                    verticalMovement     = scale_mouse_capture_axis_movement(hostVerticalMovement,
                                                                         sourceHeight,
                                                                         destinationHeight);
                }
                bootState->mouseCaptureLastClientHorizontalPosition = clientHorizontalPosition;
                bootState->mouseCaptureLastClientVerticalPosition   = clientVerticalPosition;
                bootState->mouseCaptureLastClientPositionValid      = 1u;
            }
            if (horizontalMovement != 0 || verticalMovement != 0 || buttonChanged)
            {
                push_host_mouse_movement(bootState, horizontalMovement, verticalMovement, buttonMask, buttonChanged);
            }
        }
        else if (buttonChanged)
        {
            push_host_mouse_movement(bootState, 0, 0, buttonMask, 1);
        }

        bootState->mouseButtonMask = buttonMask;
        if (buttonMask != 0u)
        {
            SetCapture(windowHandle);
        }
        return;
    }
}

static void handle_host_raw_mouse_input(hyperdos_win32_boot_state* bootState, HRAWINPUT rawInputHandle)
{
    RAWINPUT        rawInput;
    UINT            rawInputSize = sizeof(rawInput);
    const RAWMOUSE* rawMouse     = NULL;
    int             rawHorizontalMovement;
    int             rawVerticalMovement;

    if (bootState == NULL || bootState->hostMouseCaptureActive == 0u)
    {
        return;
    }
    if (GetRawInputData(rawInputHandle, RID_INPUT, &rawInput, &rawInputSize, sizeof(RAWINPUTHEADER)) == (UINT)-1)
    {
        return;
    }
    if (rawInput.header.dwType != RIM_TYPEMOUSE)
    {
        return;
    }

    rawMouse = &rawInput.data.mouse;
    if ((rawMouse->usFlags & MOUSE_MOVE_ABSOLUTE) != 0u)
    {
        return;
    }

    rawHorizontalMovement = rawMouse->lLastX;
    rawVerticalMovement   = -rawMouse->lLastY;
    if (rawHorizontalMovement == 0 && rawVerticalMovement == 0)
    {
        return;
    }

    bootState->hostMouseRawMovementObserved        = 1u;
    bootState->mouseCaptureLastClientPositionValid = 0u;
    push_host_mouse_movement(bootState, rawHorizontalMovement, rawVerticalMovement, bootState->mouseButtonMask, 0);
}

static void release_host_mouse_buttons(HWND windowHandle, hyperdos_win32_boot_state* bootState)
{
    if (bootState->mouseButtonMask != 0u)
    {
        push_host_mouse_movement(bootState, 0, 0, 0u, 1);
    }
    bootState->mouseButtonMask = 0u;
    if (bootState->hostMouseCaptureActive == 0u && GetCapture() == windowHandle)
    {
        ReleaseCapture();
    }
}

static void update_window_title(HWND windowHandle, const hyperdos_win32_boot_state* bootState)
{
    char title[HYPERDOS_MONITOR_STATUS_TEXT_CAPACITY + 96u];

    snprintf(title,
             sizeof(title),
             "HyperDOS PC Monitor - %s%s",
             bootState->statusText[0] != '\0' ? bootState->statusText : "running",
             bootState->hostMouseCaptureActive != 0u ? " - mouse captured, Ctrl+F10 releases" : "");
    SetWindowTextA(windowHandle, title);
}

static int start_emulation_thread(hyperdos_win32_boot_state* bootState)
{
    DWORD threadIdentifier = 0;

    bootState->emulationThreadHandle = CreateThread(NULL, 0u, emulation_thread_main, bootState, 0u, &threadIdentifier);
    return bootState->emulationThreadHandle != NULL;
}

static int initialize_emulation_pacing(LARGE_INTEGER*                   performanceCounterFrequency,
                                       LARGE_INTEGER*                   hostPerformanceCounterBase,
                                       uint64_t*                        guestClockBase,
                                       const hyperdos_win32_boot_state* bootState)
{
    if (performanceCounterFrequency == NULL || hostPerformanceCounterBase == NULL || guestClockBase == NULL ||
        bootState == NULL)
    {
        return 0;
    }
    if (!QueryPerformanceFrequency(performanceCounterFrequency) ||
        !QueryPerformanceCounter(hostPerformanceCounterBase) || performanceCounterFrequency->QuadPart <= 0)
    {
        return 0;
    }
    *guestClockBase = bootState->machine.pc.clockGenerator.generatedClockCount;
    return 1;
}

static void pace_emulation_to_guest_clock(const hyperdos_win32_boot_state* bootState,
                                          const LARGE_INTEGER*             performanceCounterFrequency,
                                          const LARGE_INTEGER*             hostPerformanceCounterBase,
                                          uint64_t                         guestClockBase)
{
    LARGE_INTEGER hostPerformanceCounter;
    uint64_t      guestElapsedClockCount  = 0u;
    uint32_t      processorFrequencyHertz = 0u;
    double        hostElapsedSeconds      = 0.0;
    double        guestElapsedSeconds     = 0.0;
    double        guestAheadSeconds       = 0.0;
    DWORD         sleepMilliseconds       = 0u;

    if (bootState == NULL || performanceCounterFrequency == NULL || hostPerformanceCounterBase == NULL ||
        performanceCounterFrequency->QuadPart <= 0)
    {
        return;
    }
    processorFrequencyHertz = bootState->machine.pc.clockGenerator.processorFrequencyHertz;
    if (processorFrequencyHertz == 0u || bootState->machine.pc.clockGenerator.generatedClockCount < guestClockBase ||
        !QueryPerformanceCounter(&hostPerformanceCounter))
    {
        return;
    }

    guestElapsedClockCount = bootState->machine.pc.clockGenerator.generatedClockCount - guestClockBase;
    hostElapsedSeconds     = (double)(hostPerformanceCounter.QuadPart - hostPerformanceCounterBase->QuadPart) /
                         (double)performanceCounterFrequency->QuadPart;
    guestElapsedSeconds = (double)guestElapsedClockCount / (double)processorFrequencyHertz;
    guestAheadSeconds   = guestElapsedSeconds - hostElapsedSeconds;
    if (guestAheadSeconds < 0.002)
    {
        return;
    }

    sleepMilliseconds = (DWORD)(guestAheadSeconds * 1000.0);
    if (sleepMilliseconds > HYPERDOS_MONITOR_RENDER_TIMER_PERIOD_MILLISECONDS)
    {
        sleepMilliseconds = HYPERDOS_MONITOR_RENDER_TIMER_PERIOD_MILLISECONDS;
    }
    if (sleepMilliseconds != 0u)
    {
        Sleep(sleepMilliseconds);
    }
}

static void render_monitor_message(hyperdos_win32_boot_state* bootState, const char* message)
{
    hyperdos_pc_machine_boot_configuration machineConfiguration;

    memset(&machineConfiguration, 0, sizeof(machineConfiguration));
    machineConfiguration.userContext    = bootState;
    machineConfiguration.processorModel = globalProcessorModel;
    machineConfiguration.pcModel        = globalPcModel;
    if (hyperdos_pc_machine_initialize_for_boot(&bootState->machine, &machineConfiguration))
    {
        hyperdos_pc_render_text_message(&bootState->machine.pc, message);
    }
    set_status_text(bootState, "%s", message);
}

static int select_disk_image_file_path(HWND        windowHandle,
                                       const char* title,
                                       char*       selectedPath,
                                       size_t      selectedPathSize)
{
    OPENFILENAMEA     openFileName;
    static const char filter[] = "Disk images (*.img;*.ima;*.bin)\0*.img;*.ima;*.bin\0"
                                 "All files (*.*)\0*.*\0";

    if (selectedPathSize == 0u)
    {
        return 0;
    }
    selectedPath[0] = '\0';
    memset(&openFileName, 0, sizeof(openFileName));
    openFileName.lStructSize = sizeof(openFileName);
    openFileName.hwndOwner   = windowHandle;
    openFileName.lpstrFilter = filter;
    openFileName.lpstrFile   = selectedPath;
    openFileName.nMaxFile    = (DWORD)selectedPathSize;
    openFileName.lpstrTitle  = title;
    openFileName.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    return GetOpenFileNameA(&openFileName) != 0;
}

static void restore_monitor_window_keyboard_focus(HWND windowHandle)
{
    SetForegroundWindow(windowHandle);
    SetActiveWindow(windowHandle);
    SetFocus(windowHandle);
}

static void boot_from_selected_disk_images(HWND windowHandle, hyperdos_win32_boot_state* bootState)
{
    if (initialize_boot_from_disk_images(bootState))
    {
        if (!start_emulation_thread(bootState))
        {
            set_status_text(bootState, "failed to start emulation thread");
        }
    }
    else
    {
        hyperdos_pc_render_text_message(&bootState->machine.pc, bootState->statusText);
    }
    InvalidateRect(windowHandle, NULL, FALSE);
    update_window_title(windowHandle, bootState);
}

static void handle_insert_floppy_command(HWND windowHandle, hyperdos_win32_boot_state* bootState)
{
    char                        selectedPath[HYPERDOS_MONITOR_PATH_CAPACITY];
    hyperdos_pc_disk_image      replacementDisk;
    hyperdos_pc_storage_context storageContext;
    hyperdos_pc_storage_result  storageResult = HYPERDOS_PC_STORAGE_OK;

    if (!select_disk_image_file_path(windowHandle,
                                     "Insert floppy image for drive A:",
                                     selectedPath,
                                     sizeof(selectedPath)))
    {
        restore_monitor_window_keyboard_focus(windowHandle);
        return;
    }
    restore_monitor_window_keyboard_focus(windowHandle);

    copy_string_to_buffer(globalFloppyImagePaths[0], sizeof(globalFloppyImagePaths[0]), selectedPath);
    globalFloppyImagePathCount = 1u;

    if (InterlockedCompareExchange(&bootState->isRunning, 0, 0) == 0)
    {
        boot_from_selected_disk_images(windowHandle, bootState);
        return;
    }

    if (!hyperdos_pc_disk_image_load_floppy(&replacementDisk, selectedPath))
    {
        set_status_text(bootState, "failed to read floppy image: %s", selectedPath);
        update_window_title(windowHandle, bootState);
        restore_monitor_window_keyboard_focus(windowHandle);
        return;
    }
    initialize_storage_context_for_boot_state(bootState, &storageContext);
    EnterCriticalSection(&bootState->diskCriticalSection);
    storageResult = hyperdos_pc_storage_insert_floppy_disk_image(&storageContext, 0u, &replacementDisk);
    LeaveCriticalSection(&bootState->diskCriticalSection);
    if (storageResult != HYPERDOS_PC_STORAGE_OK)
    {
        hyperdos_pc_disk_image_free(&replacementDisk);
        if (storageResult == HYPERDOS_PC_STORAGE_FLUSH_FAILED)
        {
            set_status_text(bootState, "failed to flush previous floppy image");
        }
        else
        {
            set_status_text(bootState, "failed to insert floppy image");
        }
        update_window_title(windowHandle, bootState);
        restore_monitor_window_keyboard_focus(windowHandle);
        return;
    }

    set_status_text(bootState, "A: %s inserted", find_file_name_from_path(selectedPath));
    InvalidateRect(windowHandle, NULL, FALSE);
    update_window_title(windowHandle, bootState);
    restore_monitor_window_keyboard_focus(windowHandle);
}

static void handle_eject_floppy_command(HWND windowHandle, hyperdos_win32_boot_state* bootState)
{
    hyperdos_pc_storage_context storageContext;
    hyperdos_pc_storage_result  storageResult = HYPERDOS_PC_STORAGE_OK;

    initialize_storage_context_for_boot_state(bootState, &storageContext);
    EnterCriticalSection(&bootState->diskCriticalSection);
    storageResult = hyperdos_pc_storage_eject_floppy_disk(&storageContext, 0u);
    LeaveCriticalSection(&bootState->diskCriticalSection);

    if (storageResult == HYPERDOS_PC_STORAGE_FLUSH_FAILED)
    {
        set_status_text(bootState, "failed to flush floppy image before eject");
    }
    else if (storageResult == HYPERDOS_PC_STORAGE_OK)
    {
        globalFloppyImagePathCount   = 0u;
        globalFloppyImagePaths[0][0] = '\0';
        set_status_text(bootState, "A: ejected");
    }
    else
    {
        set_status_text(bootState, "A: no floppy inserted");
    }
    InvalidateRect(windowHandle, NULL, FALSE);
    update_window_title(windowHandle, bootState);
    SetFocus(windowHandle);
}

static void handle_attach_hard_disk_command(HWND windowHandle, hyperdos_win32_boot_state* bootState)
{
    char                        selectedPath[HYPERDOS_MONITOR_PATH_CAPACITY];
    hyperdos_pc_disk_image      replacementDisk;
    hyperdos_pc_storage_context storageContext;
    hyperdos_pc_storage_result  storageResult = HYPERDOS_PC_STORAGE_OK;

    if (!select_disk_image_file_path(windowHandle,
                                     "Attach hard disk image for drive C:",
                                     selectedPath,
                                     sizeof(selectedPath)))
    {
        SetFocus(windowHandle);
        return;
    }
    SetFocus(windowHandle);
    if (!hyperdos_pc_disk_image_load_hard_disk(&replacementDisk, selectedPath))
    {
        set_status_text(bootState, "failed to attach hard disk image: %s", selectedPath);
        update_window_title(windowHandle, bootState);
        SetFocus(windowHandle);
        return;
    }

    initialize_storage_context_for_boot_state(bootState, &storageContext);
    EnterCriticalSection(&bootState->diskCriticalSection);
    storageResult = hyperdos_pc_storage_attach_fixed_disk_image(&storageContext, 0u, &replacementDisk);
    LeaveCriticalSection(&bootState->diskCriticalSection);
    if (storageResult != HYPERDOS_PC_STORAGE_OK)
    {
        hyperdos_pc_disk_image_free(&replacementDisk);
        if (storageResult == HYPERDOS_PC_STORAGE_FLUSH_FAILED)
        {
            set_status_text(bootState, "failed to flush previous hard disk image");
        }
        else
        {
            set_status_text(bootState, "failed to attach hard disk image");
        }
        update_window_title(windowHandle, bootState);
        SetFocus(windowHandle);
        return;
    }

    copy_string_to_buffer(globalHardDiskImagePath, sizeof(globalHardDiskImagePath), selectedPath);
    set_status_text(bootState, "C: %s attached", find_file_name_from_path(selectedPath));
    InvalidateRect(windowHandle, NULL, FALSE);
    update_window_title(windowHandle, bootState);
    SetFocus(windowHandle);
}

static void handle_flush_disks_command(HWND windowHandle, hyperdos_win32_boot_state* bootState)
{
    if (flush_all_disk_images(bootState))
    {
        set_status_text(bootState, "disk images flushed");
    }
    else
    {
        set_status_text(bootState, "failed to flush one or more disk images");
    }
    update_window_title(windowHandle, bootState);
}

static void handle_reset_pc_command(HWND windowHandle, hyperdos_win32_boot_state* bootState)
{
    release_host_mouse_buttons(windowHandle, bootState);
    release_host_mouse_capture(windowHandle, bootState);
    stop_emulation_thread(bootState);
    flush_all_disk_images(bootState);
    boot_from_selected_disk_images(windowHandle, bootState);
}

static void handle_start_cpu_trace_command(HWND windowHandle, hyperdos_win32_boot_state* bootState)
{
    memset(bootState->cpuTraceEntries, 0, sizeof(bootState->cpuTraceEntries));
    InterlockedExchange(&bootState->cpuTraceEnabled, 1);
    if (globalCpuTracePath[0] != '\0')
    {
        set_status_text(bootState, "CPU trace armed: %s", globalCpuTracePath);
    }
    else
    {
        set_status_text(bootState, "CPU trace path not configured");
    }
    update_window_title(windowHandle, bootState);
    SetFocus(windowHandle);
}

static void update_machine_model_menu(HWND windowHandle)
{
    HMENU menuHandle = GetMenu(windowHandle);

    if (menuHandle == NULL)
    {
        return;
    }
    CheckMenuRadioItem(menuHandle,
                       HYPERDOS_MONITOR_COMMAND_PROCESSOR_MODEL_8086,
                       HYPERDOS_MONITOR_COMMAND_PROCESSOR_MODEL_80186,
                       globalProcessorModel == HYPERDOS_X86_16_PROCESSOR_MODEL_8086
                               ? HYPERDOS_MONITOR_COMMAND_PROCESSOR_MODEL_8086
                               : HYPERDOS_MONITOR_COMMAND_PROCESSOR_MODEL_80186,
                       MF_BYCOMMAND);
    CheckMenuRadioItem(menuHandle,
                       HYPERDOS_MONITOR_COMMAND_PC_MODEL_XT,
                       HYPERDOS_MONITOR_COMMAND_PC_MODEL_AT,
                       globalPcModel == HYPERDOS_PC_MODEL_XT ? HYPERDOS_MONITOR_COMMAND_PC_MODEL_XT
                                                             : HYPERDOS_MONITOR_COMMAND_PC_MODEL_AT,
                       MF_BYCOMMAND);
}

static void set_processor_model(HWND                            windowHandle,
                                hyperdos_win32_boot_state*      bootState,
                                hyperdos_x86_16_processor_model processorModel)
{
    if (globalProcessorModel != processorModel)
    {
        globalProcessorModel = processorModel;
        update_machine_model_menu(windowHandle);
        handle_reset_pc_command(windowHandle, bootState);
        return;
    }
    SetFocus(windowHandle);
}

static void set_pc_model(HWND windowHandle, hyperdos_win32_boot_state* bootState, hyperdos_pc_model pcModel)
{
    if (globalPcModel != pcModel)
    {
        globalPcModel = pcModel;
        update_machine_model_menu(windowHandle);
        handle_reset_pc_command(windowHandle, bootState);
        return;
    }
    SetFocus(windowHandle);
}

static void update_text_character_set_menu(HWND windowHandle)
{
    HMENU menuHandle = GetMenu(windowHandle);

    if (menuHandle == NULL)
    {
        return;
    }
    CheckMenuRadioItem(menuHandle,
                       HYPERDOS_MONITOR_COMMAND_TEXT_CHARACTER_SET_CODE_PAGE_437,
                       HYPERDOS_MONITOR_COMMAND_TEXT_CHARACTER_SET_KOREAN_CODE_PAGE_949,
                       globalTextCharacterSet == HYPERDOS_MONITOR_TEXT_CHARACTER_SET_CODE_PAGE_437
                               ? HYPERDOS_MONITOR_COMMAND_TEXT_CHARACTER_SET_CODE_PAGE_437
                               : HYPERDOS_MONITOR_COMMAND_TEXT_CHARACTER_SET_KOREAN_CODE_PAGE_949,
                       MF_BYCOMMAND);
}

static void set_text_character_set(HWND windowHandle, hyperdos_monitor_text_character_set textCharacterSet)
{
    globalTextCharacterSet = textCharacterSet;
    update_text_character_set_menu(windowHandle);
    InvalidateRect(windowHandle, NULL, FALSE);
    SetFocus(windowHandle);
}

static void update_display_resize_mode_menu(HWND windowHandle)
{
    HMENU menuHandle     = GetMenu(windowHandle);
    UINT  checkedCommand = HYPERDOS_MONITOR_COMMAND_DISPLAY_RESIZE_MODE_FIT_TO_WINDOW;

    if (menuHandle == NULL)
    {
        return;
    }
    if (globalDisplayResizeMode == HYPERDOS_MONITOR_DISPLAY_RESIZE_MODE_ORIGINAL_SIZE)
    {
        checkedCommand = HYPERDOS_MONITOR_COMMAND_DISPLAY_RESIZE_MODE_ORIGINAL_SIZE;
    }
    else if (globalDisplayResizeMode == HYPERDOS_MONITOR_DISPLAY_RESIZE_MODE_INTEGER_SCALE)
    {
        checkedCommand = HYPERDOS_MONITOR_COMMAND_DISPLAY_RESIZE_MODE_INTEGER_SCALE;
    }
    CheckMenuRadioItem(menuHandle,
                       HYPERDOS_MONITOR_COMMAND_DISPLAY_RESIZE_MODE_ORIGINAL_SIZE,
                       HYPERDOS_MONITOR_COMMAND_DISPLAY_RESIZE_MODE_FIT_TO_WINDOW,
                       checkedCommand,
                       MF_BYCOMMAND);
}

static void set_display_resize_mode(HWND                                 windowHandle,
                                    hyperdos_win32_boot_state*           bootState,
                                    hyperdos_monitor_display_resize_mode displayResizeMode)
{
    globalDisplayResizeMode = displayResizeMode;
    update_display_resize_mode_menu(windowHandle);
    (void)update_host_mouse_capture_clipping(windowHandle, bootState);
    InvalidateRect(windowHandle, NULL, FALSE);
    SetFocus(windowHandle);
}

static HMENU create_monitor_menu(void)
{
    HMENU menuHandle                  = CreateMenu();
    HMENU machineMenuHandle           = CreatePopupMenu();
    HMENU processorModelMenuHandle    = CreatePopupMenu();
    HMENU pcModelMenuHandle           = CreatePopupMenu();
    HMENU diskMenuHandle              = CreatePopupMenu();
    HMENU viewMenuHandle              = CreatePopupMenu();
    HMENU textCharacterSetMenuHandle  = CreatePopupMenu();
    HMENU displayResizeModeMenuHandle = CreatePopupMenu();

    AppendMenuA(processorModelMenuHandle, MF_STRING, HYPERDOS_MONITOR_COMMAND_PROCESSOR_MODEL_8086, "8086 / 8088");
    AppendMenuA(processorModelMenuHandle, MF_STRING, HYPERDOS_MONITOR_COMMAND_PROCESSOR_MODEL_80186, "80186 / 80188");
    AppendMenuA(machineMenuHandle, MF_POPUP, (UINT_PTR)processorModelMenuHandle, "Processor");
    AppendMenuA(pcModelMenuHandle, MF_STRING, HYPERDOS_MONITOR_COMMAND_PC_MODEL_XT, "XT");
    AppendMenuA(pcModelMenuHandle, MF_STRING, HYPERDOS_MONITOR_COMMAND_PC_MODEL_AT, "AT");
    AppendMenuA(machineMenuHandle, MF_POPUP, (UINT_PTR)pcModelMenuHandle, "PC Model");
    AppendMenuA(machineMenuHandle, MF_SEPARATOR, 0u, NULL);
    AppendMenuA(machineMenuHandle, MF_STRING, HYPERDOS_MONITOR_COMMAND_RESET_PC, "Reset PC");
    AppendMenuA(machineMenuHandle, MF_STRING, HYPERDOS_MONITOR_COMMAND_START_CPU_TRACE, "Start CPU Trace");
    AppendMenuA(menuHandle, MF_POPUP, (UINT_PTR)machineMenuHandle, "Machine");

    AppendMenuA(diskMenuHandle, MF_STRING, HYPERDOS_MONITOR_COMMAND_INSERT_FLOPPY, "Insert Floppy A...");
    AppendMenuA(diskMenuHandle, MF_STRING, HYPERDOS_MONITOR_COMMAND_EJECT_FLOPPY, "Eject Floppy A");
    AppendMenuA(diskMenuHandle, MF_SEPARATOR, 0u, NULL);
    AppendMenuA(diskMenuHandle, MF_STRING, HYPERDOS_MONITOR_COMMAND_ATTACH_HARD_DISK, "Attach Hard Disk C...");
    AppendMenuA(diskMenuHandle, MF_SEPARATOR, 0u, NULL);
    AppendMenuA(diskMenuHandle, MF_STRING, HYPERDOS_MONITOR_COMMAND_FLUSH_DISKS, "Flush Disk Images");
    AppendMenuA(menuHandle, MF_POPUP, (UINT_PTR)diskMenuHandle, "Disk");

    AppendMenuA(textCharacterSetMenuHandle,
                MF_STRING,
                HYPERDOS_MONITOR_COMMAND_TEXT_CHARACTER_SET_CODE_PAGE_437,
                "CP437");
    AppendMenuA(textCharacterSetMenuHandle,
                MF_STRING,
                HYPERDOS_MONITOR_COMMAND_TEXT_CHARACTER_SET_KOREAN_CODE_PAGE_949,
                "Korean CP949");
    AppendMenuA(displayResizeModeMenuHandle,
                MF_STRING,
                HYPERDOS_MONITOR_COMMAND_DISPLAY_RESIZE_MODE_ORIGINAL_SIZE,
                "Original Size");
    AppendMenuA(displayResizeModeMenuHandle,
                MF_STRING,
                HYPERDOS_MONITOR_COMMAND_DISPLAY_RESIZE_MODE_INTEGER_SCALE,
                "Integer Scale");
    AppendMenuA(displayResizeModeMenuHandle,
                MF_STRING,
                HYPERDOS_MONITOR_COMMAND_DISPLAY_RESIZE_MODE_FIT_TO_WINDOW,
                "Fit to Window");
    AppendMenuA(viewMenuHandle, MF_POPUP, (UINT_PTR)displayResizeModeMenuHandle, "Display Resize");
    AppendMenuA(viewMenuHandle, MF_POPUP, (UINT_PTR)textCharacterSetMenuHandle, "Text Charset");
    AppendMenuA(viewMenuHandle, MF_SEPARATOR, 0u, NULL);
    AppendMenuA(viewMenuHandle, MF_STRING, HYPERDOS_MONITOR_COMMAND_TOGGLE_MOUSE_CAPTURE, "Capture Mouse\tCtrl+F10");
    AppendMenuA(viewMenuHandle,
                MF_STRING | MF_CHECKED,
                HYPERDOS_MONITOR_COMMAND_TOGGLE_HOST_MOUSE_CURSOR_CONFINEMENT,
                "Confine Host Cursor");
    AppendMenuA(viewMenuHandle,
                MF_STRING | MF_CHECKED,
                HYPERDOS_MONITOR_COMMAND_TOGGLE_HOST_MOUSE_CURSOR_HIDING,
                "Hide Host Cursor");
    AppendMenuA(menuHandle, MF_POPUP, (UINT_PTR)viewMenuHandle, "View");
    return menuHandle;
}

static LRESULT CALLBACK monitor_window_procedure(HWND   windowHandle,
                                                 UINT   message,
                                                 WPARAM wordParameter,
                                                 LPARAM longParameter)
{
    switch (message)
    {
    case WM_CREATE:
        memset(&globalBootState, 0, sizeof(globalBootState));
        globalBootState.windowHandle                      = windowHandle;
        globalBootState.hostMouseCursorConfinementEnabled = 1u;
        globalBootState.hostMouseCursorHidingEnabled      = 1u;
        InitializeCriticalSection(&globalBootState.keyboardCriticalSection);
        InitializeCriticalSection(&globalBootState.diskCriticalSection);
        globalBootState.keyboardEventHandle         = CreateEventA(NULL, FALSE, FALSE, NULL);
        globalBootState.hostMouseRawInputRegistered = (uint8_t)register_host_mouse_raw_input(windowHandle);
        globalTextFontHandle                        = CreateFontA(HYPERDOS_MONITOR_CHARACTER_HEIGHT,
                                           HYPERDOS_MONITOR_CHARACTER_WIDTH,
                                           0,
                                           0,
                                           FW_NORMAL,
                                           FALSE,
                                           FALSE,
                                           FALSE,
                                           HANGEUL_CHARSET,
                                           OUT_DEFAULT_PRECIS,
                                           CLIP_DEFAULT_PRECIS,
                                           NONANTIALIASED_QUALITY,
                                           FIXED_PITCH | FF_MODERN,
                                           "GulimChe");
        globalCodePage437TextFontHandle             = CreateFontA(HYPERDOS_MONITOR_CHARACTER_HEIGHT,
                                                      HYPERDOS_MONITOR_CHARACTER_WIDTH,
                                                      0,
                                                      0,
                                                      FW_NORMAL,
                                                      FALSE,
                                                      FALSE,
                                                      FALSE,
                                                      DEFAULT_CHARSET,
                                                      OUT_DEFAULT_PRECIS,
                                                      CLIP_DEFAULT_PRECIS,
                                                      NONANTIALIASED_QUALITY,
                                                      FIXED_PITCH | FF_MODERN,
                                                      "Lucida Console");
        {
            HDC deviceContext = GetDC(windowHandle);
            if (deviceContext != NULL)
            {
                globalCodePage437GlyphRowsInitialized = initialize_code_page_437_glyph_rows(deviceContext);
                ReleaseDC(windowHandle, deviceContext);
            }
        }

        if (globalFloppyImagePathCount == 0u && !globalBootHardDiskWithoutFloppy)
        {
            char selectedPath[HYPERDOS_MONITOR_PATH_CAPACITY];
            if (select_disk_image_file_path(windowHandle,
                                            "Select boot floppy image",
                                            selectedPath,
                                            sizeof(selectedPath)))
            {
                copy_string_to_buffer(globalFloppyImagePaths[0], sizeof(globalFloppyImagePaths[0]), selectedPath);
                globalFloppyImagePathCount = 1u;
            }
        }

        if (globalFloppyImagePathCount == 0u && !globalBootHardDiskWithoutFloppy)
        {
            render_monitor_message(&globalBootState, "Use Disk > Insert Floppy A... to boot.");
        }
        else if (initialize_boot_from_disk_images(&globalBootState))
        {
            if (!start_emulation_thread(&globalBootState))
            {
                set_status_text(&globalBootState, "failed to start emulation thread");
            }
        }
        else
        {
            hyperdos_pc_render_text_message(&globalBootState.machine.pc, globalBootState.statusText);
        }
        SetTimer(windowHandle,
                 HYPERDOS_MONITOR_RENDER_TIMER_IDENTIFIER,
                 HYPERDOS_MONITOR_RENDER_TIMER_PERIOD_MILLISECONDS,
                 NULL);
        update_machine_model_menu(windowHandle);
        update_display_resize_mode_menu(windowHandle);
        update_text_character_set_menu(windowHandle);
        update_mouse_capture_menu(windowHandle, &globalBootState);
        update_window_title(windowHandle, &globalBootState);
        SetFocus(windowHandle);
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wordParameter))
        {
        case HYPERDOS_MONITOR_COMMAND_RESET_PC:
            handle_reset_pc_command(windowHandle, &globalBootState);
            return 0;
        case HYPERDOS_MONITOR_COMMAND_START_CPU_TRACE:
            handle_start_cpu_trace_command(windowHandle, &globalBootState);
            return 0;
        case HYPERDOS_MONITOR_COMMAND_PROCESSOR_MODEL_8086:
            set_processor_model(windowHandle, &globalBootState, HYPERDOS_X86_16_PROCESSOR_MODEL_8086);
            return 0;
        case HYPERDOS_MONITOR_COMMAND_PROCESSOR_MODEL_80186:
            set_processor_model(windowHandle, &globalBootState, HYPERDOS_X86_16_PROCESSOR_MODEL_80186);
            return 0;
        case HYPERDOS_MONITOR_COMMAND_PC_MODEL_XT:
            set_pc_model(windowHandle, &globalBootState, HYPERDOS_PC_MODEL_XT);
            return 0;
        case HYPERDOS_MONITOR_COMMAND_PC_MODEL_AT:
            set_pc_model(windowHandle, &globalBootState, HYPERDOS_PC_MODEL_AT);
            return 0;
        case HYPERDOS_MONITOR_COMMAND_TEXT_CHARACTER_SET_CODE_PAGE_437:
            set_text_character_set(windowHandle, HYPERDOS_MONITOR_TEXT_CHARACTER_SET_CODE_PAGE_437);
            return 0;
        case HYPERDOS_MONITOR_COMMAND_TEXT_CHARACTER_SET_KOREAN_CODE_PAGE_949:
            set_text_character_set(windowHandle, HYPERDOS_MONITOR_TEXT_CHARACTER_SET_KOREAN_CODE_PAGE_949);
            return 0;
        case HYPERDOS_MONITOR_COMMAND_DISPLAY_RESIZE_MODE_ORIGINAL_SIZE:
            set_display_resize_mode(windowHandle, &globalBootState, HYPERDOS_MONITOR_DISPLAY_RESIZE_MODE_ORIGINAL_SIZE);
            return 0;
        case HYPERDOS_MONITOR_COMMAND_DISPLAY_RESIZE_MODE_INTEGER_SCALE:
            set_display_resize_mode(windowHandle, &globalBootState, HYPERDOS_MONITOR_DISPLAY_RESIZE_MODE_INTEGER_SCALE);
            return 0;
        case HYPERDOS_MONITOR_COMMAND_DISPLAY_RESIZE_MODE_FIT_TO_WINDOW:
            set_display_resize_mode(windowHandle, &globalBootState, HYPERDOS_MONITOR_DISPLAY_RESIZE_MODE_FIT_TO_WINDOW);
            return 0;
        case HYPERDOS_MONITOR_COMMAND_TOGGLE_MOUSE_CAPTURE:
            toggle_host_mouse_capture(windowHandle, &globalBootState);
            update_window_title(windowHandle, &globalBootState);
            SetFocus(windowHandle);
            return 0;
        case HYPERDOS_MONITOR_COMMAND_TOGGLE_HOST_MOUSE_CURSOR_CONFINEMENT:
            toggle_host_mouse_cursor_confinement(windowHandle, &globalBootState);
            SetFocus(windowHandle);
            return 0;
        case HYPERDOS_MONITOR_COMMAND_TOGGLE_HOST_MOUSE_CURSOR_HIDING:
            toggle_host_mouse_cursor_hiding(windowHandle, &globalBootState);
            SetFocus(windowHandle);
            return 0;
        case HYPERDOS_MONITOR_COMMAND_INSERT_FLOPPY:
            handle_insert_floppy_command(windowHandle, &globalBootState);
            return 0;
        case HYPERDOS_MONITOR_COMMAND_EJECT_FLOPPY:
            handle_eject_floppy_command(windowHandle, &globalBootState);
            return 0;
        case HYPERDOS_MONITOR_COMMAND_ATTACH_HARD_DISK:
            handle_attach_hard_disk_command(windowHandle, &globalBootState);
            return 0;
        case HYPERDOS_MONITOR_COMMAND_FLUSH_DISKS:
            handle_flush_disks_command(windowHandle, &globalBootState);
            return 0;
        default:
            break;
        }
        break;

    case WM_SYSCOMMAND:
        if ((wordParameter & 0xFFF0u) == SC_KEYMENU)
        {
            return 0;
        }
        break;

    case WM_MOVE:
    case WM_SIZE:
    case WM_DISPLAYCHANGE:
        update_host_mouse_capture_clipping(windowHandle, &globalBootState);
        if (host_mouse_capture_uses_message_movement(&globalBootState))
        {
            center_host_mouse_capture_cursor(windowHandle, &globalBootState);
        }
        InvalidateRect(windowHandle, NULL, FALSE);
        break;

    case WM_SETCURSOR:
        if (globalBootState.hostMouseCaptureActive != 0u && LOWORD(longParameter) == HTCLIENT)
        {
            set_host_mouse_capture_cursor(&globalBootState);
            return TRUE;
        }
        break;

    case WM_CAPTURECHANGED:
        if (globalBootState.hostMouseCaptureActive != 0u && (HWND)longParameter != windowHandle)
        {
            if (GetForegroundWindow() == windowHandle)
            {
                SetCapture(windowHandle);
                (void)update_host_mouse_capture_clipping(windowHandle, &globalBootState);
                set_host_mouse_capture_cursor(&globalBootState);
            }
            else
            {
                release_host_mouse_capture(windowHandle, &globalBootState);
            }
            update_window_title(windowHandle, &globalBootState);
        }
        break;

    case WM_ACTIVATEAPP:
        if (wordParameter == FALSE)
        {
            globalBootState.hostMouseCaptureToggleKeyDown = 0u;
            release_pressed_host_keys(&globalBootState);
            release_host_mouse_buttons(windowHandle, &globalBootState);
            release_host_mouse_capture(windowHandle, &globalBootState);
            update_window_title(windowHandle, &globalBootState);
        }
        break;

    case WM_KILLFOCUS:
        globalBootState.hostMouseCaptureToggleKeyDown = 0u;
        release_pressed_host_keys(&globalBootState);
        release_host_mouse_buttons(windowHandle, &globalBootState);
        release_host_mouse_capture(windowHandle, &globalBootState);
        update_window_title(windowHandle, &globalBootState);
        break;

    case WM_TIMER:
        if (wordParameter == HYPERDOS_MONITOR_RENDER_TIMER_IDENTIFIER)
        {
            InvalidateRect(windowHandle, NULL, FALSE);
            update_window_title(windowHandle, &globalBootState);
            return 0;
        }
        break;

    case HYPERDOS_MONITOR_USER_RENDER_MESSAGE:
        InvalidateRect(windowHandle, NULL, FALSE);
        update_window_title(windowHandle, &globalBootState);
        return 0;

    case HYPERDOS_MONITOR_USER_RESET_MESSAGE:
        release_host_mouse_buttons(windowHandle, &globalBootState);
        release_host_mouse_capture(windowHandle, &globalBootState);
        if (globalBootState.emulationThreadHandle != NULL)
        {
            WaitForSingleObject(globalBootState.emulationThreadHandle, 2000u);
            CloseHandle(globalBootState.emulationThreadHandle);
            globalBootState.emulationThreadHandle = NULL;
        }
        boot_from_selected_disk_images(windowHandle, &globalBootState);
        return 0;

    case WM_CHAR:
        return 0;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
    {
        uint8_t  scanCodeBytes[HYPERDOS_MONITOR_KEYBOARD_SCAN_CODE_SEQUENCE_CAPACITY];
        size_t   scanCodeByteCount  = 0u;
        uint16_t hostKeyStateIndex  = HYPERDOS_MONITOR_HOST_KEY_STATE_NONE;
        int      previousKeyWasDown = (((uint32_t)longParameter & 0x40000000u) != 0u);

        if (host_mouse_capture_toggle_shortcut_is_down(wordParameter))
        {
            globalBootState.hostMouseCaptureToggleKeyDown = 1u;
            if (!previousKeyWasDown)
            {
                release_pressed_host_keys(&globalBootState);
                release_host_mouse_buttons(windowHandle, &globalBootState);
                toggle_host_mouse_capture(windowHandle, &globalBootState);
                update_window_title(windowHandle, &globalBootState);
            }
            return 0;
        }

        scanCodeByteCount = make_keyboard_scan_code_sequence_from_window_key_message(wordParameter,
                                                                                     longParameter,
                                                                                     0,
                                                                                     scanCodeBytes,
                                                                                     sizeof(scanCodeBytes),
                                                                                     &hostKeyStateIndex);
        if (scanCodeByteCount != 0u)
        {
            push_host_key_press(&globalBootState,
                                scanCodeBytes,
                                scanCodeByteCount,
                                hostKeyStateIndex,
                                previousKeyWasDown);
            return 0;
        }
        if (wordParameter == VK_SHIFT || wordParameter == VK_LSHIFT || wordParameter == VK_RSHIFT ||
            wordParameter == VK_CONTROL || wordParameter == VK_LCONTROL || wordParameter == VK_RCONTROL ||
            wordParameter == VK_MENU || wordParameter == VK_LMENU || wordParameter == VK_RMENU)
        {
            return 0;
        }
        break;
    }

    case WM_SYSKEYUP:
    case WM_KEYUP:
    {
        uint8_t  scanCodeBytes[HYPERDOS_MONITOR_KEYBOARD_SCAN_CODE_SEQUENCE_CAPACITY];
        size_t   scanCodeByteCount = 0u;
        uint16_t hostKeyStateIndex = HYPERDOS_MONITOR_HOST_KEY_STATE_NONE;

        if (wordParameter == VK_F10 && globalBootState.hostMouseCaptureToggleKeyDown != 0u)
        {
            globalBootState.hostMouseCaptureToggleKeyDown = 0u;
            return 0;
        }

        scanCodeByteCount = make_keyboard_scan_code_sequence_from_window_key_message(wordParameter,
                                                                                     longParameter,
                                                                                     1,
                                                                                     scanCodeBytes,
                                                                                     sizeof(scanCodeBytes),
                                                                                     &hostKeyStateIndex);
        if (scanCodeByteCount != 0u)
        {
            push_host_key_release(&globalBootState, scanCodeBytes, scanCodeByteCount, hostKeyStateIndex);
            return 0;
        }
    }
        if (wordParameter == VK_F10 || wordParameter == VK_SHIFT || wordParameter == VK_LSHIFT ||
            wordParameter == VK_RSHIFT || wordParameter == VK_CONTROL || wordParameter == VK_LCONTROL ||
            wordParameter == VK_RCONTROL || wordParameter == VK_MENU || wordParameter == VK_LMENU ||
            wordParameter == VK_RMENU)
        {
            return 0;
        }
        break;

    case WM_SYSCHAR:
        return 0;

    case WM_ERASEBKGND:
        return 1;

    case WM_INPUT:
        handle_host_raw_mouse_input(&globalBootState, (HRAWINPUT)longParameter);
        return 0;

    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
    case WM_MBUTTONUP:
        SetFocus(windowHandle);
        handle_host_mouse_input(windowHandle, &globalBootState, message, wordParameter, longParameter);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT paintStructure;
        RECT        clientRectangle;
        HDC         deviceContext = BeginPaint(windowHandle, &paintStructure);
        GetClientRect(windowHandle, &clientRectangle);
        render_display_to_window(deviceContext, &globalBootState, &clientRectangle);
        EndPaint(windowHandle, &paintStructure);
        return 0;
    }

    case WM_DESTROY:
        KillTimer(windowHandle, HYPERDOS_MONITOR_RENDER_TIMER_IDENTIFIER);
        release_host_mouse_buttons(windowHandle, &globalBootState);
        release_host_mouse_capture(windowHandle, &globalBootState);
        shutdown_boot_state(&globalBootState);
        if (globalTextFontHandle != NULL)
        {
            DeleteObject(globalTextFontHandle);
            globalTextFontHandle = NULL;
        }
        if (globalCodePage437TextFontHandle != NULL)
        {
            DeleteObject(globalCodePage437TextFontHandle);
            globalCodePage437TextFontHandle = NULL;
        }
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(windowHandle, message, wordParameter, longParameter);
}

static int copy_next_command_line_argument(const char* commandLine,
                                           size_t      sourceOffset,
                                           char*       destination,
                                           size_t      destinationSize,
                                           size_t*     nextSourceOffset)
{
    size_t sourceIndex      = sourceOffset;
    size_t destinationIndex = 0;
    int    quoted           = 0;

    if (destinationSize == 0u)
    {
        return 0;
    }
    destination[0] = '\0';
    while (commandLine[sourceIndex] == ' ' || commandLine[sourceIndex] == '\t')
    {
        ++sourceIndex;
    }
    if (commandLine[sourceIndex] == '\0')
    {
        if (nextSourceOffset != NULL)
        {
            *nextSourceOffset = sourceIndex;
        }
        return 0;
    }
    if (commandLine[sourceIndex] == '"')
    {
        quoted = 1;
        ++sourceIndex;
    }
    while (commandLine[sourceIndex] != '\0' && destinationIndex + 1u < destinationSize)
    {
        if (quoted)
        {
            if (commandLine[sourceIndex] == '"')
            {
                break;
            }
        }
        else if (commandLine[sourceIndex] == ' ' || commandLine[sourceIndex] == '\t')
        {
            break;
        }
        destination[destinationIndex] = commandLine[sourceIndex];
        ++destinationIndex;
        ++sourceIndex;
    }
    destination[destinationIndex] = '\0';
    if (quoted && commandLine[sourceIndex] == '"')
    {
        ++sourceIndex;
    }
    if (nextSourceOffset != NULL)
    {
        *nextSourceOffset = sourceIndex;
    }
    return destinationIndex != 0u;
}

static void parse_command_line_disk_paths(const char* commandLine)
{
    char              argument[HYPERDOS_MONITOR_PATH_CAPACITY];
    size_t            sourceOffset               = 0u;
    static const char defaultHardDiskImagePath[] = "images\\harddisk.img";

    globalFloppyImagePathCount      = 0u;
    globalDiskTracePath[0]          = '\0';
    globalCpuTracePath[0]           = '\0';
    globalMemoryTracePath[0]        = '\0';
    globalGuestMemoryDumpPath[0]    = '\0';
    globalTextScreenDumpPath[0]     = '\0';
    globalVideoStateDumpPath[0]     = '\0';
    globalMemoryWatchCount          = 0u;
    globalMemoryStopPhysicalAddress = 0u;
    globalMemoryStopByteValue       = 0u;
    globalMemoryStopByteEnabled     = 0;
    globalBootHardDiskWithoutFloppy = 0;
    globalCoprocessorEnabled        = 0;
    globalProcessorModel            = HYPERDOS_X86_16_PROCESSOR_MODEL_80186;
    globalPcModel                   = HYPERDOS_PC_MODEL_AT;
    globalCpuTraceStartsEnabled     = 0;
    copy_string_to_buffer(globalHardDiskImagePath, sizeof(globalHardDiskImagePath), defaultHardDiskImagePath);

    while (copy_next_command_line_argument(commandLine, sourceOffset, argument, sizeof(argument), &sourceOffset))
    {
        if (strcmp(argument, "--hard-disk") == 0)
        {
            if (copy_next_command_line_argument(commandLine,
                                                sourceOffset,
                                                globalHardDiskImagePath,
                                                sizeof(globalHardDiskImagePath),
                                                &sourceOffset))
            {
                globalBootHardDiskWithoutFloppy = 1;
                continue;
            }
            globalHardDiskImagePath[0] = '\0';
            continue;
        }
        if (strncmp(argument, "--hard-disk=", 12u) == 0)
        {
            copy_string_to_buffer(globalHardDiskImagePath, sizeof(globalHardDiskImagePath), argument + 12u);
            globalBootHardDiskWithoutFloppy = 1;
            continue;
        }
        if (strcmp(argument, "--no-hard-disk") == 0)
        {
            globalHardDiskImagePath[0]      = '\0';
            globalBootHardDiskWithoutFloppy = 0;
            continue;
        }
        if (strcmp(argument, "--disk-trace") == 0)
        {
            (void)copy_next_command_line_argument(commandLine,
                                                  sourceOffset,
                                                  globalDiskTracePath,
                                                  sizeof(globalDiskTracePath),
                                                  &sourceOffset);
            continue;
        }
        if (strncmp(argument, "--disk-trace=", 13u) == 0)
        {
            copy_string_to_buffer(globalDiskTracePath, sizeof(globalDiskTracePath), argument + 13u);
            continue;
        }
        if (strcmp(argument, "--cpu-trace") == 0)
        {
            (void)copy_next_command_line_argument(commandLine,
                                                  sourceOffset,
                                                  globalCpuTracePath,
                                                  sizeof(globalCpuTracePath),
                                                  &sourceOffset);
            continue;
        }
        if (strncmp(argument, "--cpu-trace=", 12u) == 0)
        {
            copy_string_to_buffer(globalCpuTracePath, sizeof(globalCpuTracePath), argument + 12u);
            continue;
        }
        if (strcmp(argument, "--cpu-trace-start-enabled") == 0)
        {
            globalCpuTraceStartsEnabled = 1;
            continue;
        }
        if (strcmp(argument, "--memory-trace") == 0)
        {
            (void)copy_next_command_line_argument(commandLine,
                                                  sourceOffset,
                                                  globalMemoryTracePath,
                                                  sizeof(globalMemoryTracePath),
                                                  &sourceOffset);
            continue;
        }
        if (strncmp(argument, "--memory-trace=", 15u) == 0)
        {
            copy_string_to_buffer(globalMemoryTracePath, sizeof(globalMemoryTracePath), argument + 15u);
            continue;
        }
        if (strcmp(argument, "--guest-memory-dump") == 0)
        {
            (void)copy_next_command_line_argument(commandLine,
                                                  sourceOffset,
                                                  globalGuestMemoryDumpPath,
                                                  sizeof(globalGuestMemoryDumpPath),
                                                  &sourceOffset);
            continue;
        }
        if (strncmp(argument, "--guest-memory-dump=", 20u) == 0)
        {
            copy_string_to_buffer(globalGuestMemoryDumpPath, sizeof(globalGuestMemoryDumpPath), argument + 20u);
            continue;
        }
        if (strcmp(argument, "--text-screen-dump") == 0)
        {
            (void)copy_next_command_line_argument(commandLine,
                                                  sourceOffset,
                                                  globalTextScreenDumpPath,
                                                  sizeof(globalTextScreenDumpPath),
                                                  &sourceOffset);
            continue;
        }
        if (strncmp(argument, "--text-screen-dump=", 19u) == 0)
        {
            copy_string_to_buffer(globalTextScreenDumpPath, sizeof(globalTextScreenDumpPath), argument + 19u);
            continue;
        }
        if (strcmp(argument, "--video-state-dump") == 0)
        {
            (void)copy_next_command_line_argument(commandLine,
                                                  sourceOffset,
                                                  globalVideoStateDumpPath,
                                                  sizeof(globalVideoStateDumpPath),
                                                  &sourceOffset);
            continue;
        }
        if (strncmp(argument, "--video-state-dump=", 19u) == 0)
        {
            copy_string_to_buffer(globalVideoStateDumpPath, sizeof(globalVideoStateDumpPath), argument + 19u);
            continue;
        }
        if (strcmp(argument, "--memory-watch") == 0)
        {
            char memoryWatchText[HYPERDOS_MONITOR_MEMORY_WATCH_TEXT_CAPACITY];
            if (copy_next_command_line_argument(commandLine,
                                                sourceOffset,
                                                memoryWatchText,
                                                sizeof(memoryWatchText),
                                                &sourceOffset))
            {
                add_memory_watch_argument(memoryWatchText);
            }
            continue;
        }
        if (strncmp(argument, "--memory-watch=", 15u) == 0)
        {
            add_memory_watch_argument(argument + 15u);
            continue;
        }
        if (strcmp(argument, "--memory-stop-byte") == 0)
        {
            char memoryStopText[HYPERDOS_MONITOR_MEMORY_WATCH_TEXT_CAPACITY];
            if (copy_next_command_line_argument(commandLine,
                                                sourceOffset,
                                                memoryStopText,
                                                sizeof(memoryStopText),
                                                &sourceOffset))
            {
                (void)parse_memory_stop_byte_argument(memoryStopText);
            }
            continue;
        }
        if (strncmp(argument, "--memory-stop-byte=", 19u) == 0)
        {
            (void)parse_memory_stop_byte_argument(argument + 19u);
            continue;
        }
        if (strcmp(argument, "--boot-hard-disk") == 0)
        {
            globalBootHardDiskWithoutFloppy = 1;
            continue;
        }
        if (strcmp(argument, "--8087") == 0)
        {
            globalCoprocessorEnabled = 1;
            continue;
        }
        if (strcmp(argument, "--no-8087") == 0)
        {
            globalCoprocessorEnabled = 0;
            continue;
        }
        if (strcmp(argument, "--divide-error-returns-to-faulting-instruction") == 0)
        {
            globalDivideErrorReturnsToFaultingInstruction = 1;
            continue;
        }
        if (strcmp(argument, "--divide-error-returns-to-following-instruction") == 0)
        {
            globalDivideErrorReturnsToFaultingInstruction = 0;
            continue;
        }
        if (globalFloppyImagePathCount < HYPERDOS_MONITOR_MAXIMUM_FLOPPY_DRIVE_COUNT)
        {
            copy_string_to_buffer(globalFloppyImagePaths[globalFloppyImagePathCount],
                                  sizeof(globalFloppyImagePaths[globalFloppyImagePathCount]),
                                  argument);
            ++globalFloppyImagePathCount;
        }
    }
}

static void configure_process_display_scale_awareness(void)
{
    typedef BOOL(WINAPI * hyperdos_monitor_set_process_display_scale_awareness_context_function)(HANDLE);
    typedef HRESULT(WINAPI * hyperdos_monitor_set_process_display_scale_awareness_function)(int);
    typedef BOOL(WINAPI * hyperdos_monitor_set_process_display_scale_aware_function)(void);

    HMODULE userInterfaceLibrary = LoadLibraryA("user32.dll");
    HMODULE shellScalingLibrary  = NULL;

    if (userInterfaceLibrary == NULL)
    {
        return;
    }

    hyperdos_monitor_set_process_display_scale_awareness_context_function setProcessDisplayScaleAwarenessContext =
            (hyperdos_monitor_set_process_display_scale_awareness_context_function)
                    GetProcAddress(userInterfaceLibrary, "SetProcessDpiAwarenessContext");
    if (setProcessDisplayScaleAwarenessContext != NULL &&
        setProcessDisplayScaleAwarenessContext((HANDLE)(intptr_t)-4) != FALSE)
    {
        FreeLibrary(userInterfaceLibrary);
        return;
    }

    shellScalingLibrary = LoadLibraryA("shcore.dll");
    if (shellScalingLibrary != NULL)
    {
        hyperdos_monitor_set_process_display_scale_awareness_function
                setProcessDisplayScaleAwareness = (hyperdos_monitor_set_process_display_scale_awareness_function)
                        GetProcAddress(shellScalingLibrary, "SetProcessDpiAwareness");
        if (setProcessDisplayScaleAwareness != NULL && setProcessDisplayScaleAwareness(2) >= 0)
        {
            FreeLibrary(shellScalingLibrary);
            FreeLibrary(userInterfaceLibrary);
            return;
        }
        FreeLibrary(shellScalingLibrary);
    }

    hyperdos_monitor_set_process_display_scale_aware_function
            setProcessDisplayScaleAware = (hyperdos_monitor_set_process_display_scale_aware_function)
                    GetProcAddress(userInterfaceLibrary, "SetProcessDPIAware");
    if (setProcessDisplayScaleAware != NULL)
    {
        (void)setProcessDisplayScaleAware();
    }
    FreeLibrary(userInterfaceLibrary);
}

int WINAPI WinMain(HINSTANCE instanceHandle, HINSTANCE previousInstanceHandle, LPSTR commandLine, int showCommand)
{
    WNDCLASSA windowClass;
    HWND      windowHandle = NULL;
    HMENU     menuHandle   = NULL;
    MSG       message;
    DWORD     windowStyle = WS_OVERLAPPEDWINDOW;
    RECT      requestedClientRectangle;
    int       windowWidth  = 0;
    int       windowHeight = 0;

    (void)previousInstanceHandle;
    configure_process_display_scale_awareness();
    parse_command_line_disk_paths(commandLine);
    InitializeCriticalSection(&globalDiskTraceCriticalSection);
    globalDiskTraceCriticalSectionInitialized = 1;
    if (globalDiskTracePath[0] != '\0')
    {
        globalDiskTraceFile = fopen(globalDiskTracePath, "wb");
        if (globalDiskTraceFile != NULL)
        {
            fprintf(globalDiskTraceFile, "HyperDOS disk trace\n");
            fflush(globalDiskTraceFile);
        }
    }
    if (globalMemoryTracePath[0] != '\0')
    {
        globalMemoryTraceFile = fopen(globalMemoryTracePath, "wb");
        if (globalMemoryTraceFile != NULL)
        {
            fprintf(globalMemoryTraceFile, "HyperDOS memory trace\n");
            fflush(globalMemoryTraceFile);
        }
    }
    requestedClientRectangle.left  = 0;
    requestedClientRectangle.top   = 0;
    requestedClientRectangle.right = HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT * HYPERDOS_MONITOR_CHARACTER_WIDTH +
                                     HYPERDOS_MONITOR_WINDOW_EXTRA_WIDTH;
    requestedClientRectangle.bottom = HYPERDOS_VIDEO_GRAPHICS_ARRAY_MAXIMUM_DISPLAY_HEIGHT;
    AdjustWindowRectEx(&requestedClientRectangle, windowStyle, TRUE, 0);
    windowWidth  = requestedClientRectangle.right - requestedClientRectangle.left;
    windowHeight = requestedClientRectangle.bottom - requestedClientRectangle.top;
    memset(&windowClass, 0, sizeof(windowClass));
    windowClass.style         = CS_DBLCLKS;
    windowClass.lpfnWndProc   = monitor_window_procedure;
    windowClass.hInstance     = instanceHandle;
    windowClass.lpszClassName = "HyperDOSMonitorWindow";
    windowClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    if (!RegisterClassA(&windowClass))
    {
        return 1;
    }

    menuHandle   = create_monitor_menu();
    windowHandle = CreateWindowExA(0,
                                   windowClass.lpszClassName,
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
        return 1;
    }

    ShowWindow(windowHandle, showCommand);
    UpdateWindow(windowHandle);

    while (GetMessageA(&message, NULL, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    if (globalDiskTraceFile != NULL)
    {
        fclose(globalDiskTraceFile);
        globalDiskTraceFile = NULL;
    }
    if (globalMemoryTraceFile != NULL)
    {
        fclose(globalMemoryTraceFile);
        globalMemoryTraceFile = NULL;
    }
    if (globalDiskTraceCriticalSectionInitialized)
    {
        DeleteCriticalSection(&globalDiskTraceCriticalSection);
        globalDiskTraceCriticalSectionInitialized = 0;
    }
    return (int)message.wParam;
}
