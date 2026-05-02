#ifndef HYPERDOS_PC_BOARD_H
#define HYPERDOS_PC_BOARD_H

#include <stddef.h>
#include <stdint.h>

#include "hyperdos/chipset.h"
#include "hyperdos/devices.h"
#include "hyperdos/pc_cmos.h"
#include "hyperdos/x86_16_processor.h"

enum
{
    HYPERDOS_PC_PROGRAMMABLE_INTERRUPT_CONTROLLER_PORT       = 0x0020u,
    HYPERDOS_PC_SLAVE_PROGRAMMABLE_INTERRUPT_CONTROLLER_PORT = 0x00A0u,
    HYPERDOS_PC_DIRECT_MEMORY_ACCESS_CONTROLLER_PORT         = 0x0000u,
    HYPERDOS_PC_PROGRAMMABLE_INTERVAL_TIMER_PORT             = 0x0040u,
    HYPERDOS_PC_KEYBOARD_CONTROLLER_DATA_PORT                = 0x0060u,
    HYPERDOS_PC_KEYBOARD_CONTROLLER_COMMAND_PORT             = 0x0064u,
    HYPERDOS_PC_PROGRAMMABLE_PERIPHERAL_INTERFACE_PORT       = 0x0061u,
    HYPERDOS_PC_FIRST_SERIAL_PORT                            = 0x03F8u,
    HYPERDOS_PC_VIDEO_MONOCHROME_COMPATIBILITY_PORT          = 0x03B0u,
    HYPERDOS_PC_COLOR_GRAPHICS_ADAPTER_PORT                  = HYPERDOS_COLOR_GRAPHICS_ADAPTER_INPUT_OUTPUT_PORT,
    HYPERDOS_PC_COLOR_GRAPHICS_ADAPTER_PORT_COUNT            = HYPERDOS_COLOR_GRAPHICS_ADAPTER_INPUT_OUTPUT_PORT_COUNT,
    HYPERDOS_PC_DIRECT_MEMORY_ACCESS_CONTROLLER_PORT_COUNT   = 16u,
    HYPERDOS_PC_SERIAL_PORT_COUNT                            = 8u,
    HYPERDOS_PC_VIDEO_MONOCHROME_COMPATIBILITY_PORT_COUNT    = 16u,
    HYPERDOS_PC_KEYBOARD_CONTROLLER_PORT_COUNT               = 1u,
    HYPERDOS_PC_KEYBOARD_CONTROLLER_COMMAND_PORT_COUNT       = 1u,
    HYPERDOS_PC_PERIPHERAL_INTERFACE_PORT_COUNT              = 3u,
    HYPERDOS_PC_INTERVAL_TIMER_PORT_COUNT                    = 4u,
    HYPERDOS_PC_INTERRUPT_CONTROLLER_PORT_COUNT              = 2u,
    HYPERDOS_PC_SLAVE_INTERRUPT_CONTROLLER_PORT_COUNT        = 2u,
    HYPERDOS_PC_DEFAULT_INTERRUPT_VECTOR_BASE                = 0x08u,
    HYPERDOS_PC_DEFAULT_INTERRUPT_MASK                       = 0xFCu,
    HYPERDOS_PC_BOOT_SECTOR_ADDRESS                          = 0x7C00u,
    HYPERDOS_PC_BOOT_SECTOR_BYTE_COUNT                       = 512u,
    HYPERDOS_PC_BOOT_STACK_POINTER                           = 0x7C00u,
    HYPERDOS_PC_BIOS_RESET_VECTOR_SEGMENT                    = 0xFFFFu,
    HYPERDOS_PC_BIOS_RESET_VECTOR_OFFSET                     = 0x0000u,
    HYPERDOS_PC_BIOS_READ_ONLY_MEMORY_BASE                   = 0xF0000u,
    HYPERDOS_PC_BIOS_READ_ONLY_MEMORY_SIZE                   = 0x10000u,
    HYPERDOS_PC_8284_CRYSTAL_FREQUENCY_HERTZ                 = 14318181u,
    HYPERDOS_PC_MILLISECONDS_PER_SECOND                      = 1000u
};

typedef void (*hyperdos_pc_speaker_state_change_function)(void* userContext, uint32_t frequencyHertz, uint8_t enabled);

typedef struct hyperdos_pc
{
    uint8_t                                              processorMemory[HYPERDOS_X86_16_MEMORY_SIZE];
    hyperdos_x86_16_processor                            processor;
    hyperdos_bus                                         bus;
    hyperdos_random_access_memory                        randomAccessMemory;
    hyperdos_read_only_memory                            basicInputOutputSystemReadOnlyMemory;
    hyperdos_color_graphics_adapter                      colorGraphicsAdapter;
    hyperdos_programmable_interrupt_controller           programmableInterruptController;
    hyperdos_programmable_interrupt_controller           slaveProgrammableInterruptController;
    hyperdos_direct_memory_access_controller             directMemoryAccessController;
    hyperdos_programmable_interval_timer                 programmableIntervalTimer;
    hyperdos_programmable_peripheral_interface           programmablePeripheralInterface;
    hyperdos_pc_cmos                                     realTimeClock;
    hyperdos_intel_8042_keyboard_controller              keyboardController;
    hyperdos_universal_asynchronous_receiver_transmitter firstSerialPort;
    hyperdos_8087                                        floatingPointUnit;
    hyperdos_intel_8284_clock_generator                  clockGenerator;
    hyperdos_intel_8288_bus_controller                   busController;
    hyperdos_intel_8282_address_latch                    addressLatch;
    hyperdos_intel_8286_bus_transceiver                  dataBusTransceiver;
    hyperdos_pc_speaker_state_change_function            speakerStateChange;
    void*                                                speakerStateUserContext;
    uint64_t                                             programmableIntervalTimerInputClockRemainder;
    uint32_t                                             speakerFrequencyHertz;
    uint8_t                                              speakerEnabled;
    uint8_t                                              slaveProgrammableInterruptControllerEnabled;
} hyperdos_pc;

typedef void (*hyperdos_pc_board_trace_function)(void* userContext, const char* message);

int hyperdos_pc_initialize(hyperdos_pc* pc);

void hyperdos_pc_set_speaker_state_change_function(hyperdos_pc*                              pc,
                                                   hyperdos_pc_speaker_state_change_function speakerStateChange,
                                                   void*                                     userContext);

void hyperdos_pc_render_text_message(hyperdos_pc* pc, const char* message);

int hyperdos_pc_load_boot_sector(hyperdos_pc* pc, const uint8_t* bootSectorBytes, size_t bootSectorByteCount);

void hyperdos_pc_prepare_boot_sector_execution(hyperdos_pc* pc, uint8_t bootDriveNumber);

int hyperdos_x86_16_processor_is_at_bios_reset_vector(const hyperdos_x86_16_processor* processor);

void hyperdos_pc_raise_keyboard_controller_interrupt_request(hyperdos_pc*                     pc,
                                                             hyperdos_pc_board_trace_function traceFunction,
                                                             void*                            traceUserContext);

void hyperdos_pc_raise_auxiliary_device_interrupt_request(hyperdos_pc*                     pc,
                                                          hyperdos_pc_board_trace_function traceFunction,
                                                          void*                            traceUserContext);

void hyperdos_pc_set_auxiliary_device_interrupt_request_enabled(hyperdos_pc* pc, uint8_t enabled);

void hyperdos_pc_raise_interval_timer_interrupt_request(hyperdos_pc*                     pc,
                                                        hyperdos_pc_board_trace_function traceFunction,
                                                        void*                            traceUserContext);

int hyperdos_pc_acknowledge_interrupt_request(hyperdos_pc* pc, uint8_t* interruptNumber);

uint64_t hyperdos_pc_step_halted_processor_clock(hyperdos_pc*                     pc,
                                                 uint32_t                         milliseconds,
                                                 hyperdos_pc_board_trace_function traceFunction,
                                                 void*                            traceUserContext);

#endif
