#ifndef HYPERDOS_PC_BIOS_RUNTIME_H
#define HYPERDOS_PC_BIOS_RUNTIME_H

#include <stdint.h>

#include "hyperdos/pc_bios.h"
#include "hyperdos/pc_disk_bios.h"
#include "hyperdos/pc_keyboard_bios.h"
#include "hyperdos/pc_system_bios.h"
#include "hyperdos/pc_video_bios.h"

typedef void (*hyperdos_pc_bios_runtime_trace_function)(void* userContext, const char* message);

typedef void (*hyperdos_pc_bios_runtime_drain_keyboard_input_function)(void* userContext);

typedef struct hyperdos_pc_bios_runtime
{
    void*                                                  userContext;
    hyperdos_pc*                                           pc;
    hyperdos_pc_system_bios*                               systemBios;
    hyperdos_pc_keyboard_bios*                             keyboardBios;
    const hyperdos_pc_keyboard_bios_interface*             keyboardBiosInterface;
    const hyperdos_pc_disk_bios_interface*                 diskBiosInterface;
    const hyperdos_pc_video_bios_interface*                videoBiosInterface;
    hyperdos_pc_bios_runtime_trace_function                traceFunction;
    hyperdos_pc_bios_runtime_drain_keyboard_input_function drainKeyboardInput;
    uint8_t                                                coprocessorEnabled;
} hyperdos_pc_bios_runtime;

void hyperdos_pc_bios_runtime_initialize(hyperdos_pc_bios_runtime*                              biosRuntime,
                                         hyperdos_pc*                                           pc,
                                         hyperdos_pc_system_bios*                               systemBios,
                                         hyperdos_pc_keyboard_bios*                             keyboardBios,
                                         const hyperdos_pc_keyboard_bios_interface*             keyboardBiosInterface,
                                         const hyperdos_pc_disk_bios_interface*                 diskBiosInterface,
                                         const hyperdos_pc_video_bios_interface*                videoBiosInterface,
                                         uint8_t                                                coprocessorEnabled,
                                         hyperdos_pc_bios_runtime_drain_keyboard_input_function drainKeyboardInput,
                                         hyperdos_pc_bios_runtime_trace_function                traceFunction,
                                         void*                                                  userContext);

hyperdos_x86_16_execution_result hyperdos_pc_bios_runtime_handle_interrupt(hyperdos_x86_16_processor* processor,
                                                                           uint8_t                    interruptNumber,
                                                                           void*                      userContext);

void hyperdos_pc_bios_runtime_initialize_data_area(hyperdos_pc_bios_runtime*     biosRuntime,
                                                   const hyperdos_pc_disk_image* activeFloppyDisk,
                                                   uint8_t                       fixedDiskCount);

hyperdos_x86_16_execution_result hyperdos_pc_bios_runtime_execute_processor_slice(hyperdos_pc_bios_runtime* biosRuntime,
                                                                                  uint64_t  instructionLimit,
                                                                                  uint64_t* executedInstructionCount);

int hyperdos_pc_bios_runtime_prepare_boot_from_disk_image(hyperdos_pc_bios_runtime*     biosRuntime,
                                                          const hyperdos_pc_disk_image* bootDisk,
                                                          const hyperdos_pc_disk_image* activeFloppyDisk,
                                                          uint8_t                       fixedDiskCount);

hyperdos_x86_16_execution_result hyperdos_pc_bios_runtime_service_pending_hardware_interrupts(
        hyperdos_pc_bios_runtime* biosRuntime);

#endif
