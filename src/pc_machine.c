#include "hyperdos/pc_machine.h"

static hyperdos_x87_model hyperdos_pc_machine_get_coprocessor_model(
        const hyperdos_pc_machine_boot_configuration* configuration)
{
    if (configuration->coprocessorModel != HYPERDOS_X87_MODEL_NONE)
    {
        return configuration->coprocessorModel;
    }
    if (configuration->coprocessorEnabled == 0u)
    {
        return HYPERDOS_X87_MODEL_NONE;
    }
    if (configuration->processorModel == HYPERDOS_X86_PROCESSOR_MODEL_80286)
    {
        return HYPERDOS_X87_MODEL_80287;
    }
    return HYPERDOS_X87_MODEL_8087;
}

static hyperdos_pc_chipset_profile hyperdos_pc_machine_get_chipset_profile(
        const hyperdos_pc_machine_boot_configuration* configuration)
{
    if (configuration->chipsetProfile != HYPERDOS_PC_CHIPSET_PROFILE_DEFAULT)
    {
        return configuration->chipsetProfile;
    }
    return configuration->pcModel == HYPERDOS_PC_MODEL_AT ? HYPERDOS_PC_CHIPSET_PROFILE_AT_286
                                                          : HYPERDOS_PC_CHIPSET_PROFILE_XT;
}

static uint32_t hyperdos_pc_machine_get_extended_memory_kilobytes(
        const hyperdos_pc_machine_boot_configuration* configuration,
        hyperdos_pc_chipset_profile                   chipsetProfile)
{
    if (configuration->extendedMemoryKilobytes != 0u)
    {
        return configuration->extendedMemoryKilobytes;
    }
    if (chipsetProfile == HYPERDOS_PC_CHIPSET_PROFILE_AT_286 || chipsetProfile == HYPERDOS_PC_CHIPSET_PROFILE_AT_386 ||
        chipsetProfile == HYPERDOS_PC_CHIPSET_PROFILE_AT_486)
    {
        return HYPERDOS_PC_AT_EXTENDED_MEMORY_KILOBYTES;
    }
    return 0u;
}

static int hyperdos_pc_machine_chipset_profile_is_at_compatible(hyperdos_pc_chipset_profile chipsetProfile)
{
    return chipsetProfile == HYPERDOS_PC_CHIPSET_PROFILE_AT_286 ||
           chipsetProfile == HYPERDOS_PC_CHIPSET_PROFILE_AT_386 || chipsetProfile == HYPERDOS_PC_CHIPSET_PROFILE_AT_486;
}

static hyperdos_programmable_interval_timer_model hyperdos_pc_machine_get_interval_timer_model(
        const hyperdos_pc_machine_boot_configuration* configuration,
        hyperdos_pc_chipset_profile                   chipsetProfile)
{
    if (configuration->intervalTimerModel != HYPERDOS_PROGRAMMABLE_INTERVAL_TIMER_MODEL_DEFAULT)
    {
        return configuration->intervalTimerModel;
    }
    if (hyperdos_pc_machine_chipset_profile_is_at_compatible(chipsetProfile))
    {
        return HYPERDOS_PROGRAMMABLE_INTERVAL_TIMER_MODEL_8254;
    }
    return HYPERDOS_PROGRAMMABLE_INTERVAL_TIMER_MODEL_8253;
}

static hyperdos_universal_asynchronous_receiver_transmitter_model hyperdos_pc_machine_get_first_serial_port_model(
        const hyperdos_pc_machine_boot_configuration* configuration)
{
    if (configuration->firstSerialPortModel != HYPERDOS_UNIVERSAL_ASYNCHRONOUS_RECEIVER_TRANSMITTER_MODEL_DEFAULT)
    {
        return configuration->firstSerialPortModel;
    }
    return HYPERDOS_UNIVERSAL_ASYNCHRONOUS_RECEIVER_TRANSMITTER_MODEL_8250;
}

int hyperdos_pc_machine_initialize_for_boot(hyperdos_pc_machine*                          machine,
                                            const hyperdos_pc_machine_boot_configuration* configuration)
{
    hyperdos_x87_model          coprocessorModel = HYPERDOS_X87_MODEL_NONE;
    hyperdos_pc_chipset_profile chipsetProfile   = HYPERDOS_PC_CHIPSET_PROFILE_DEFAULT;

    if (machine == NULL || configuration == NULL)
    {
        return 0;
    }
    coprocessorModel = hyperdos_pc_machine_get_coprocessor_model(configuration);
    chipsetProfile   = hyperdos_pc_machine_get_chipset_profile(configuration);

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

    if (!hyperdos_pc_initialize_with_configuration(&machine->pc,
                                                   chipsetProfile,
                                                   configuration->conventionalMemoryKilobytes,
                                                   hyperdos_pc_machine_get_extended_memory_kilobytes(configuration,
                                                                                                     chipsetProfile),
                                                   hyperdos_pc_machine_get_interval_timer_model(configuration,
                                                                                                chipsetProfile),
                                                   hyperdos_pc_machine_get_first_serial_port_model(configuration)))
    {
        return 0;
    }
    if (configuration->processorFrequencyHertz != 0u)
    {
        hyperdos_pc_set_processor_frequency_hertz(&machine->pc, configuration->processorFrequencyHertz);
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
                                        (uint8_t)(coprocessorModel != HYPERDOS_X87_MODEL_NONE),
                                        configuration->drainKeyboardInput,
                                        configuration->traceDiskBios,
                                        configuration->userContext);

    if (coprocessorModel == HYPERDOS_X87_MODEL_NONE)
    {
        hyperdos_x86_attach_coprocessor(&machine->pc.processor, NULL, NULL, NULL);
    }
    else
    {
        hyperdos_x87_initialize(&machine->pc.floatingPointUnit, coprocessorModel);
        hyperdos_x86_attach_coprocessor(&machine->pc.processor,
                                        hyperdos_x87_wait,
                                        hyperdos_x87_escape,
                                        &machine->pc.floatingPointUnit);
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
