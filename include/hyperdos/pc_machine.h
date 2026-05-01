#ifndef HYPERDOS_PC_MACHINE_H
#define HYPERDOS_PC_MACHINE_H

#include <stdint.h>

#include "hyperdos/pc_bios_runtime.h"
#include "hyperdos/pc_board.h"
#include "hyperdos/pc_disk_bios.h"
#include "hyperdos/pc_floppy_controller.h"
#include "hyperdos/pc_keyboard_bios.h"
#include "hyperdos/pc_model.h"
#include "hyperdos/pc_system_bios.h"
#include "hyperdos/pc_video_bios.h"
#include "hyperdos/pc_video_services.h"

typedef struct hyperdos_pc_machine
{
    hyperdos_pc                         pc;
    hyperdos_pc_system_bios             systemBios;
    hyperdos_pc_keyboard_bios           keyboardBios;
    hyperdos_pc_keyboard_bios_interface keyboardBiosInterface;
    hyperdos_pc_disk_bios_interface     diskBiosInterface;
    hyperdos_pc_video_bios_interface    videoBiosInterface;
    hyperdos_pc_bios_runtime            biosRuntime;
    hyperdos_pc_video_services          videoServices;
    hyperdos_pc_floppy_controller       floppyController;
} hyperdos_pc_machine;

typedef struct hyperdos_pc_machine_boot_configuration
{
    void*                                                  userContext;
    hyperdos_x86_16_processor_model                        processorModel;
    hyperdos_pc_model                                      pcModel;
    uint8_t                                                floppyDriveCount;
    uint8_t                                                fixedDiskDriveCount;
    uint8_t                                                coprocessorEnabled;
    uint8_t                                                divideErrorReturnsToFaultingInstruction;
    hyperdos_pc_keyboard_bios_lock_function                lockKeyboard;
    hyperdos_pc_keyboard_bios_lock_function                unlockKeyboard;
    hyperdos_pc_keyboard_bios_refresh_input_state_function refreshKeyboardInputState;
    hyperdos_pc_disk_bios_lock_function                    lockDiskImages;
    hyperdos_pc_disk_bios_lock_function                    unlockDiskImages;
    hyperdos_pc_disk_bios_get_disk_image_function          getDiskImage;
    hyperdos_pc_bios_runtime_drain_keyboard_input_function drainKeyboardInput;
    hyperdos_bus_observe_memory_write                      observeMemoryWrite;
    void*                                                  floppyControllerInputOutputDevice;
    hyperdos_bus_read_input_output_byte                    readFloppyControllerInputOutputByte;
    hyperdos_bus_write_input_output_byte                   writeFloppyControllerInputOutputByte;
    hyperdos_pc_speaker_state_change_function              speakerStateChange;
    hyperdos_pc_floppy_controller_trace_function           traceFloppyController;
    hyperdos_pc_disk_bios_trace_function                   traceDiskBios;
    hyperdos_pc_video_services_trace_function              traceVideoServices;
} hyperdos_pc_machine_boot_configuration;

int hyperdos_pc_machine_initialize_for_boot(hyperdos_pc_machine*                          machine,
                                            const hyperdos_pc_machine_boot_configuration* configuration);

#endif
