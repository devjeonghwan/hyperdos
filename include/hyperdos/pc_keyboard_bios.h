#ifndef HYPERDOS_PC_KEYBOARD_BIOS_H
#define HYPERDOS_PC_KEYBOARD_BIOS_H

#include <stddef.h>
#include <stdint.h>

#include "hyperdos/pc_board.h"

enum
{
    HYPERDOS_PC_KEYBOARD_BIOS_QUEUE_CAPACITY                = 256u,
    HYPERDOS_PC_KEYBOARD_BIOS_RELEASE_MASK                  = 0x80u,
    HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_SCAN_CODE_PREFIX     = 0xE0u,
    HYPERDOS_PC_KEYBOARD_BIOS_PAUSE_SCAN_CODE_PREFIX        = 0xE1u,
    HYPERDOS_PC_KEYBOARD_BIOS_RIGHT_SHIFT_FLAG              = 0x01u,
    HYPERDOS_PC_KEYBOARD_BIOS_LEFT_SHIFT_FLAG               = 0x02u,
    HYPERDOS_PC_KEYBOARD_BIOS_CONTROL_FLAG                  = 0x04u,
    HYPERDOS_PC_KEYBOARD_BIOS_ALT_FLAG                      = 0x08u,
    HYPERDOS_PC_KEYBOARD_BIOS_SCROLL_LOCK_FLAG              = 0x10u,
    HYPERDOS_PC_KEYBOARD_BIOS_NUM_LOCK_FLAG                 = 0x20u,
    HYPERDOS_PC_KEYBOARD_BIOS_CAPS_LOCK_FLAG                = 0x40u,
    HYPERDOS_PC_KEYBOARD_BIOS_INSERT_FLAG                   = 0x80u,
    HYPERDOS_PC_KEYBOARD_BIOS_SERVICE_REGISTER_SHIFT        = 8u,
    HYPERDOS_PC_KEYBOARD_BIOS_READ_SERVICE                  = 0x00u,
    HYPERDOS_PC_KEYBOARD_BIOS_STATUS_SERVICE                = 0x01u,
    HYPERDOS_PC_KEYBOARD_BIOS_SHIFT_STATUS_SERVICE          = 0x02u,
    HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_READ_SERVICE         = 0x10u,
    HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_STATUS_SERVICE       = 0x11u,
    HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_SHIFT_STATUS_SERVICE = 0x12u
};

typedef void (*hyperdos_pc_keyboard_bios_lock_function)(void* userContext);

typedef void (*hyperdos_pc_keyboard_bios_refresh_input_state_function)(void* userContext);

typedef void (*hyperdos_pc_keyboard_bios_trace_function)(void* userContext, const char* message);

typedef struct hyperdos_pc_keyboard_bios_interface
{
    void*                                                  userContext;
    hyperdos_pc_keyboard_bios_lock_function                lockKeyboard;
    hyperdos_pc_keyboard_bios_lock_function                unlockKeyboard;
    hyperdos_pc_keyboard_bios_refresh_input_state_function refreshInputState;
} hyperdos_pc_keyboard_bios_interface;

typedef struct hyperdos_pc_keyboard_bios
{
    uint16_t keyWords[HYPERDOS_PC_KEYBOARD_BIOS_QUEUE_CAPACITY];
    size_t   keyWordReadIndex;
    size_t   keyWordWriteIndex;
    uint8_t  shiftFlags;
    uint8_t  extendedScanCodePrefix;
} hyperdos_pc_keyboard_bios;

void hyperdos_pc_keyboard_bios_initialize(hyperdos_pc_keyboard_bios* keyboardBios);

void hyperdos_pc_keyboard_bios_reset(hyperdos_pc_keyboard_bios* keyboardBios);

void hyperdos_pc_keyboard_bios_initialize_data_area(hyperdos_pc_keyboard_bios* keyboardBios, hyperdos_pc* pc);

int hyperdos_pc_keyboard_bios_push_key_word(hyperdos_pc_keyboard_bios*                 keyboardBios,
                                            const hyperdos_pc_keyboard_bios_interface* keyboardBiosInterface,
                                            hyperdos_pc*                               pc,
                                            uint16_t                                   keyWord);

int hyperdos_pc_keyboard_bios_pop_key_word(hyperdos_pc_keyboard_bios*                 keyboardBios,
                                           const hyperdos_pc_keyboard_bios_interface* keyboardBiosInterface,
                                           hyperdos_pc*                               pc,
                                           uint16_t*                                  keyWord);

int hyperdos_pc_keyboard_bios_peek_key_word(hyperdos_pc_keyboard_bios*                 keyboardBios,
                                            const hyperdos_pc_keyboard_bios_interface* keyboardBiosInterface,
                                            hyperdos_pc*                               pc,
                                            uint16_t*                                  keyWord);

uint8_t hyperdos_pc_keyboard_bios_update_shift_flags_from_scan_code(uint8_t shiftFlags, uint8_t scanCode);

int hyperdos_pc_keyboard_bios_scan_code_is_modifier_or_toggle(uint8_t scanCode);

void hyperdos_pc_keyboard_bios_process_scan_code(hyperdos_pc_keyboard_bios*                 keyboardBios,
                                                 const hyperdos_pc_keyboard_bios_interface* keyboardBiosInterface,
                                                 hyperdos_pc*                               pc,
                                                 uint8_t                                    scanCode);

int hyperdos_pc_keyboard_bios_service_hardware_byte(hyperdos_pc_keyboard_bios*                 keyboardBios,
                                                    const hyperdos_pc_keyboard_bios_interface* keyboardBiosInterface,
                                                    hyperdos_pc*                               pc,
                                                    uint8_t                                    preloadedScanCode,
                                                    int                                        hasPreloadedScanCode,
                                                    hyperdos_pc_keyboard_bios_trace_function   traceFunction,
                                                    void*                                      traceUserContext);

hyperdos_x86_execution_result hyperdos_pc_keyboard_bios_handle_interrupt(
        hyperdos_x86_processor*                    processor,
        hyperdos_pc_keyboard_bios*                 keyboardBios,
        const hyperdos_pc_keyboard_bios_interface* keyboardBiosInterface,
        hyperdos_pc*                               pc,
        uint8_t                                    serviceNumber);

#endif
