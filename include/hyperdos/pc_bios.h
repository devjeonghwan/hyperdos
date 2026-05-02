#ifndef HYPERDOS_PC_BIOS_H
#define HYPERDOS_PC_BIOS_H

#include <stdint.h>

#include "hyperdos/pc_board.h"

enum
{
    HYPERDOS_PC_BIOS_TIMER_HARDWARE_INTERRUPT                       = 0x08u,
    HYPERDOS_PC_BIOS_KEYBOARD_HARDWARE_INTERRUPT                    = 0x09u,
    HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_HARDWARE_INTERRUPT            = 0x74u,
    HYPERDOS_PC_BIOS_VIDEO_INTERRUPT                                = 0x10u,
    HYPERDOS_PC_BIOS_EQUIPMENT_INTERRUPT                            = 0x11u,
    HYPERDOS_PC_BIOS_MEMORY_SIZE_INTERRUPT                          = 0x12u,
    HYPERDOS_PC_BIOS_DISK_INTERRUPT                                 = 0x13u,
    HYPERDOS_PC_BIOS_SERIAL_INTERRUPT                               = 0x14u,
    HYPERDOS_PC_BIOS_SYSTEM_SERVICES_INTERRUPT                      = 0x15u,
    HYPERDOS_PC_BIOS_KEYBOARD_INTERRUPT                             = 0x16u,
    HYPERDOS_PC_BIOS_PRINTER_INTERRUPT                              = 0x17u,
    HYPERDOS_PC_BIOS_TIME_INTERRUPT                                 = 0x1Au,
    HYPERDOS_PC_BIOS_USER_TIMER_TICK_INTERRUPT                      = 0x1Cu,
    HYPERDOS_PC_BIOS_KEYBOARD_SERVICE_INTERRUPT                     = 0xF1u,
    HYPERDOS_PC_BIOS_DISK_SERVICE_INTERRUPT                         = 0xF2u,
    HYPERDOS_PC_BIOS_KEYBOARD_SOFTWARE_SERVICE_INTERRUPT            = 0xF3u,
    HYPERDOS_PC_BIOS_VIDEO_SERVICE_INTERRUPT                        = 0xF4u,
    HYPERDOS_PC_BIOS_EQUIPMENT_SERVICE_INTERRUPT                    = 0xF5u,
    HYPERDOS_PC_BIOS_MEMORY_SIZE_SERVICE_INTERRUPT                  = 0xF6u,
    HYPERDOS_PC_BIOS_SERIAL_SERVICE_INTERRUPT                       = 0xF7u,
    HYPERDOS_PC_BIOS_SYSTEM_SERVICES_SERVICE_INTERRUPT              = 0xF8u,
    HYPERDOS_PC_BIOS_PRINTER_SERVICE_INTERRUPT                      = 0xF9u,
    HYPERDOS_PC_BIOS_TIME_SERVICE_INTERRUPT                         = 0xFAu,
    HYPERDOS_PC_BIOS_TIMER_SERVICE_INTERRUPT                        = 0xFBu,
    HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_SERVICE_INTERRUPT             = 0xFCu,
    HYPERDOS_PC_BIOS_KEYBOARD_HARDWARE_STUB_SEGMENT                 = 0xF000u,
    HYPERDOS_PC_BIOS_KEYBOARD_HARDWARE_STUB_OFFSET                  = 0x0100u,
    HYPERDOS_PC_BIOS_TIMER_HARDWARE_STUB_SEGMENT                    = 0xF000u,
    HYPERDOS_PC_BIOS_TIMER_HARDWARE_STUB_OFFSET                     = 0x0150u,
    HYPERDOS_PC_BIOS_USER_TIMER_TICK_STUB_SEGMENT                   = 0xF000u,
    HYPERDOS_PC_BIOS_USER_TIMER_TICK_STUB_OFFSET                    = 0x0160u,
    HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_HARDWARE_STUB_SEGMENT         = 0xF000u,
    HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_HARDWARE_STUB_OFFSET          = 0x0170u,
    HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_CALLBACK_CLEANUP_STUB_SEGMENT = 0xF000u,
    HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_CALLBACK_CLEANUP_STUB_OFFSET  = 0x0180u,
    HYPERDOS_PC_BIOS_DISK_STUB_SEGMENT                              = 0xF000u,
    HYPERDOS_PC_BIOS_DISK_STUB_OFFSET                               = 0x012Cu,
    HYPERDOS_PC_BIOS_KEYBOARD_SOFTWARE_STUB_SEGMENT                 = 0xF000u,
    HYPERDOS_PC_BIOS_KEYBOARD_SOFTWARE_STUB_OFFSET                  = 0x0138u,
    HYPERDOS_PC_BIOS_VIDEO_STUB_SEGMENT                             = 0xF000u,
    HYPERDOS_PC_BIOS_VIDEO_STUB_OFFSET                              = 0x0120u,
    HYPERDOS_PC_BIOS_EQUIPMENT_STUB_SEGMENT                         = 0xF000u,
    HYPERDOS_PC_BIOS_EQUIPMENT_STUB_OFFSET                          = 0x0124u,
    HYPERDOS_PC_BIOS_MEMORY_SIZE_STUB_SEGMENT                       = 0xF000u,
    HYPERDOS_PC_BIOS_MEMORY_SIZE_STUB_OFFSET                        = 0x0128u,
    HYPERDOS_PC_BIOS_SERIAL_STUB_SEGMENT                            = 0xF000u,
    HYPERDOS_PC_BIOS_SERIAL_STUB_OFFSET                             = 0x0130u,
    HYPERDOS_PC_BIOS_SYSTEM_SERVICES_STUB_SEGMENT                   = 0xF000u,
    HYPERDOS_PC_BIOS_SYSTEM_SERVICES_STUB_OFFSET                    = 0x0134u,
    HYPERDOS_PC_BIOS_PRINTER_STUB_SEGMENT                           = 0xF000u,
    HYPERDOS_PC_BIOS_PRINTER_STUB_OFFSET                            = 0x013Cu,
    HYPERDOS_PC_BIOS_TIME_STUB_SEGMENT                              = 0xF000u,
    HYPERDOS_PC_BIOS_TIME_STUB_OFFSET                               = 0x0140u
};

void hyperdos_pc_bios_install_interrupt_vector_stubs(hyperdos_pc* pc);

void hyperdos_pc_bios_set_carry_flag(hyperdos_x86_16_processor* processor, int carry);

void hyperdos_pc_bios_synchronize_interrupt_return_flag(hyperdos_pc*               pc,
                                                        hyperdos_x86_16_processor* processor,
                                                        uint16_t                   flagMask);

void hyperdos_pc_bios_synchronize_interrupt_return_carry_flag(hyperdos_pc* pc, hyperdos_x86_16_processor* processor);

void hyperdos_pc_bios_synchronize_interrupt_return_zero_flag(hyperdos_pc* pc, hyperdos_x86_16_processor* processor);

int hyperdos_pc_bios_interrupt_vector_matches(hyperdos_pc* pc,
                                              uint8_t      interruptNumber,
                                              uint16_t     expectedSegment,
                                              uint16_t     expectedOffset);

int hyperdos_pc_bios_interrupt_vector_is_empty(hyperdos_pc* pc, uint8_t interruptNumber);

int hyperdos_pc_bios_keyboard_hardware_interrupt_vector_is_default(hyperdos_pc* pc);

#endif
