#include "hyperdos/pc_machine.h"

enum
{
    HYPERDOS_PC_MACHINE_SLAVE_INTERRUPT_CASCADE_REQUEST_LINE = 2u
};

int hyperdos_pc_machine_initialize_for_boot(hyperdos_pc_machine*                          machine,
                                            const hyperdos_pc_machine_boot_configuration* configuration)
{
    if (machine == NULL || configuration == NULL)
    {
        return 0;
    }

    hyperdos_pc_keyboard_bios_reset(&machine->keyboardBios);
    hyperdos_pc_system_bios_reset(&machine->systemBios);
    hyperdos_pc_system_bios_set_model_identifier(&machine->systemBios,
                                                 hyperdos_pc_model_get_system_bios_model_identifier(
                                                         configuration->pcModel));
    hyperdos_pc_floppy_controller_reset(&machine->floppyController);
    hyperdos_pc_floppy_controller_set_drive_count(&machine->floppyController,
                                                  configuration->floppyDriveCount != 0u
                                                          ? configuration->floppyDriveCount
                                                          : 1u);
    hyperdos_pc_floppy_controller_set_trace_function(&machine->floppyController,
                                                     configuration->traceFloppyController,
                                                     configuration->userContext);

    if (!hyperdos_pc_initialize(&machine->pc))
    {
        return 0;
    }
    if (configuration->processorFrequencyHertz != 0u)
    {
        hyperdos_pc_set_processor_frequency_hertz(&machine->pc, configuration->processorFrequencyHertz);
    }
    if (configuration->pcModel == HYPERDOS_PC_MODEL_AT &&
        hyperdos_bus_map_input_output(&machine->pc.bus,
                                      HYPERDOS_PC_CMOS_ADDRESS_PORT,
                                      HYPERDOS_PC_CMOS_PORT_COUNT,
                                      &machine->pc.realTimeClock,
                                      hyperdos_pc_cmos_read_input_output_byte,
                                      hyperdos_pc_cmos_write_input_output_byte) != HYPERDOS_BUS_ACCESS_OK)
    {
        return 0;
    }
    if (configuration->pcModel == HYPERDOS_PC_MODEL_AT &&
        hyperdos_bus_map_input_output(&machine->pc.bus,
                                      HYPERDOS_PC_SLAVE_PROGRAMMABLE_INTERRUPT_CONTROLLER_PORT,
                                      HYPERDOS_PC_SLAVE_INTERRUPT_CONTROLLER_PORT_COUNT,
                                      &machine->pc.slaveProgrammableInterruptController,
                                      hyperdos_programmable_interrupt_controller_read_byte,
                                      hyperdos_programmable_interrupt_controller_write_byte) != HYPERDOS_BUS_ACCESS_OK)
    {
        return 0;
    }
    machine->pc.slaveProgrammableInterruptControllerEnabled = configuration->pcModel == HYPERDOS_PC_MODEL_AT ? 1u : 0u;
    if (machine->pc.slaveProgrammableInterruptControllerEnabled != 0u)
    {
        machine->pc.programmableInterruptController.interruptMaskRegister =
                (uint8_t)(machine->pc.programmableInterruptController.interruptMaskRegister &
                          ~(1u << HYPERDOS_PC_MACHINE_SLAVE_INTERRUPT_CASCADE_REQUEST_LINE));
    }
    hyperdos_x86_set_processor_model(&machine->pc.processor, configuration->processorModel);
    hyperdos_pc_set_speaker_state_change_function(&machine->pc,
                                                  configuration->speakerStateChange,
                                                  configuration->userContext);
    hyperdos_pc_video_services_initialize(&machine->videoServices,
                                          &machine->pc,
                                          configuration->traceVideoServices,
                                          configuration->userContext);
    machine->keyboardBiosInterface.userContext       = configuration->userContext;
    machine->keyboardBiosInterface.lockKeyboard      = configuration->lockKeyboard;
    machine->keyboardBiosInterface.unlockKeyboard    = configuration->unlockKeyboard;
    machine->keyboardBiosInterface.refreshInputState = configuration->refreshKeyboardInputState;

    machine->diskBiosInterface.userContext                   = configuration->userContext;
    machine->diskBiosInterface.pc                            = &machine->pc;
    machine->diskBiosInterface.floppyController              = &machine->floppyController;
    machine->diskBiosInterface.floppyDriveCount              = configuration->floppyDriveCount != 0u
                                                                       ? configuration->floppyDriveCount
                                                                       : 1u;
    machine->diskBiosInterface.fixedDiskDriveCount           = configuration->fixedDiskDriveCount;
    machine->diskBiosInterface.floppyDiskChangeLineSupported = 1u;
    machine->diskBiosInterface.lockDiskImages                = configuration->lockDiskImages;
    machine->diskBiosInterface.unlockDiskImages              = configuration->unlockDiskImages;
    machine->diskBiosInterface.getDiskImage                  = configuration->getDiskImage;
    machine->diskBiosInterface.traceFunction                 = configuration->traceDiskBios;

    hyperdos_pc_video_services_connect_bios_interface(&machine->videoServices, &machine->videoBiosInterface);
    hyperdos_pc_bios_runtime_initialize(&machine->biosRuntime,
                                        &machine->pc,
                                        &machine->systemBios,
                                        &machine->keyboardBios,
                                        &machine->keyboardBiosInterface,
                                        &machine->diskBiosInterface,
                                        &machine->videoBiosInterface,
                                        configuration->coprocessorEnabled,
                                        configuration->drainKeyboardInput,
                                        configuration->traceDiskBios,
                                        configuration->userContext);

    if (!configuration->coprocessorEnabled)
    {
        hyperdos_x86_attach_coprocessor(&machine->pc.processor, NULL, NULL, NULL);
    }
    machine->pc.processor.divideErrorReturnsToFaultingInstruction =
            configuration->divideErrorReturnsToFaultingInstruction;
    hyperdos_bus_set_memory_write_observer(&machine->pc.bus,
                                           configuration->observeMemoryWrite,
                                           configuration->userContext);

    if (configuration->floppyControllerInputOutputDevice != NULL &&
        configuration->readFloppyControllerInputOutputByte != NULL &&
        configuration->writeFloppyControllerInputOutputByte != NULL &&
        hyperdos_bus_map_input_output(&machine->pc.bus,
                                      HYPERDOS_PC_FLOPPY_CONTROLLER_PORT,
                                      HYPERDOS_PC_FLOPPY_CONTROLLER_PORT_COUNT,
                                      configuration->floppyControllerInputOutputDevice,
                                      configuration->readFloppyControllerInputOutputByte,
                                      configuration->writeFloppyControllerInputOutputByte) != HYPERDOS_BUS_ACCESS_OK)
    {
        return 0;
    }
    return 1;
}
