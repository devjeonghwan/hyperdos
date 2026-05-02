#include "hyperdos/pc_bios_runtime.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "hyperdos/pc_bios_data_area.h"

enum
{
    HYPERDOS_PC_BIOS_RUNTIME_TRACE_TEXT_CAPACITY               = 512u,
    HYPERDOS_PC_BIOS_RUNTIME_DIVIDE_ERROR_INTERRUPT            = 0x00u,
    HYPERDOS_PC_BIOS_RUNTIME_KOREAN_VIDEO_INTERRUPT            = 0x0Au,
    HYPERDOS_PC_BIOS_RUNTIME_EXPANDED_MEMORY_MANAGER_INTERRUPT = 0x67u,
    HYPERDOS_PC_BIOS_RUNTIME_SERVICE_REGISTER_SHIFT            = 8u,
    HYPERDOS_PC_BIOS_RUNTIME_VIDEO_SET_CURSOR_POSITION_SERVICE = 0x02u,
    HYPERDOS_PC_BIOS_RUNTIME_AUXILIARY_PACKET_BYTE_COUNT       = 3u,
    HYPERDOS_PC_BIOS_RUNTIME_AUXILIARY_PACKET_ALWAYS_ONE       = 0x08u,
    HYPERDOS_PC_BIOS_RUNTIME_AUXILIARY_CALLBACK_STATUS_MASK    = 0xFBu,
    HYPERDOS_PC_BIOS_RUNTIME_AUXILIARY_CALLBACK_PADDING_WORD   = 0x0000u
};

static void hyperdos_pc_bios_runtime_trace(hyperdos_pc_bios_runtime* biosRuntime, const char* format, ...)
{
    char    message[HYPERDOS_PC_BIOS_RUNTIME_TRACE_TEXT_CAPACITY];
    va_list arguments;

    if (biosRuntime == NULL || biosRuntime->traceFunction == NULL)
    {
        return;
    }

    va_start(arguments, format);
    (void)vsnprintf(message, sizeof(message), format, arguments);
    va_end(arguments);
    biosRuntime->traceFunction(biosRuntime->userContext, message);
}

static uint8_t hyperdos_pc_bios_runtime_read_guest_memory_byte(const hyperdos_pc_bios_runtime* biosRuntime,
                                                               uint32_t                        physicalAddress)
{
    return hyperdos_bus_read_memory_byte_or_open_bus(&biosRuntime->pc->bus,
                                                     physicalAddress & HYPERDOS_X86_ADDRESS_MASK);
}

static uint8_t hyperdos_pc_bios_runtime_read_guest_instruction_byte(const hyperdos_pc_bios_runtime* biosRuntime,
                                                                    uint16_t                        segment,
                                                                    uint16_t                        offset)
{
    uint32_t physicalAddress = ((((uint32_t)segment) << 4u) + offset) & HYPERDOS_X86_ADDRESS_MASK;

    return hyperdos_pc_bios_runtime_read_guest_memory_byte(biosRuntime, physicalAddress);
}

static uint8_t hyperdos_pc_bios_runtime_get_floppy_drive_count(const hyperdos_pc_bios_runtime* biosRuntime)
{
    if (biosRuntime == NULL || biosRuntime->diskBiosInterface == NULL ||
        biosRuntime->diskBiosInterface->floppyDriveCount == 0u)
    {
        return 1u;
    }
    return biosRuntime->diskBiosInterface->floppyDriveCount;
}

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
                                         void*                                                  userContext)
{
    if (biosRuntime == NULL)
    {
        return;
    }

    memset(biosRuntime, 0, sizeof(*biosRuntime));
    biosRuntime->pc                    = pc;
    biosRuntime->systemBios            = systemBios;
    biosRuntime->keyboardBios          = keyboardBios;
    biosRuntime->keyboardBiosInterface = keyboardBiosInterface;
    biosRuntime->diskBiosInterface     = diskBiosInterface;
    biosRuntime->videoBiosInterface    = videoBiosInterface;
    biosRuntime->coprocessorEnabled    = coprocessorEnabled;
    biosRuntime->drainKeyboardInput    = drainKeyboardInput;
    biosRuntime->traceFunction         = traceFunction;
    biosRuntime->userContext           = userContext;
}

static uint8_t hyperdos_pc_bios_runtime_pointing_device_is_present(const hyperdos_pc_bios_runtime* biosRuntime)
{
    if (biosRuntime == NULL || biosRuntime->pc == NULL || biosRuntime->systemBios == NULL)
    {
        return 0u;
    }
    if (!hyperdos_pc_system_bios_model_is_at_compatible(
                hyperdos_pc_system_bios_get_model_identifier(biosRuntime->systemBios)))
    {
        return 0u;
    }
    return biosRuntime->pc->slaveProgrammableInterruptControllerEnabled != 0u ? 1u : 0u;
}

void hyperdos_pc_bios_runtime_initialize_data_area(hyperdos_pc_bios_runtime*     biosRuntime,
                                                   const hyperdos_pc_disk_image* activeFloppyDisk,
                                                   uint8_t                       fixedDiskCount)
{
    uint16_t equipmentFlags = 0u;

    if (biosRuntime == NULL || biosRuntime->pc == NULL || biosRuntime->keyboardBios == NULL)
    {
        return;
    }

    equipmentFlags = hyperdos_pc_system_bios_get_equipment_flags(biosRuntime->coprocessorEnabled,
                                                                 hyperdos_pc_bios_runtime_get_floppy_drive_count(
                                                                         biosRuntime),
                                                                 hyperdos_pc_bios_runtime_pointing_device_is_present(
                                                                         biosRuntime));
    hyperdos_pc_bios_data_area_write_word(biosRuntime->pc,
                                          HYPERDOS_PC_BIOS_DATA_AREA_EQUIPMENT_FLAGS_OFFSET,
                                          equipmentFlags);
    hyperdos_pc_cmos_set_equipment_flags(&biosRuntime->pc->realTimeClock, equipmentFlags);
    hyperdos_pc_bios_data_area_write_word(biosRuntime->pc,
                                          HYPERDOS_PC_BIOS_DATA_AREA_CONVENTIONAL_MEMORY_SIZE_OFFSET,
                                          hyperdos_pc_system_bios_get_conventional_memory_size_kilobytes());
    hyperdos_pc_system_bios_initialize_data_area(biosRuntime->pc);
    hyperdos_pc_keyboard_bios_initialize_data_area(biosRuntime->keyboardBios, biosRuntime->pc);
    hyperdos_pc_disk_bios_initialize_data_area(biosRuntime->pc, activeFloppyDisk, fixedDiskCount);
    if (biosRuntime->videoBiosInterface != NULL && biosRuntime->videoBiosInterface->setVideoMode != NULL)
    {
        biosRuntime->videoBiosInterface->setVideoMode(biosRuntime->videoBiosInterface->userContext, 0x03u);
    }
}

static hyperdos_x86_execution_result hyperdos_pc_bios_runtime_handle_korean_video_interrupt(
        hyperdos_x86_processor*   processor,
        hyperdos_pc_bios_runtime* biosRuntime,
        uint8_t                   serviceNumber)
{
    if (serviceNumber == HYPERDOS_PC_BIOS_RUNTIME_VIDEO_SET_CURSOR_POSITION_SERVICE)
    {
        return hyperdos_pc_video_bios_handle_interrupt(processor,
                                                       biosRuntime->videoBiosInterface,
                                                       HYPERDOS_PC_BIOS_RUNTIME_VIDEO_SET_CURSOR_POSITION_SERVICE);
    }

    hyperdos_pc_bios_set_carry_flag(processor, 0);
    return HYPERDOS_X86_EXECUTION_OK;
}

static void hyperdos_pc_bios_runtime_push_processor_word(hyperdos_x86_processor* processor, uint16_t value)
{
    uint16_t stackPointer =
            (uint16_t)(hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER) -
                       HYPERDOS_X86_WORD_SIZE);

    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER, stackPointer);
    (void)hyperdos_x86_write_memory_byte(processor,
                                         HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                         stackPointer,
                                         (uint8_t)(value & HYPERDOS_X86_LOW_BYTE_MASK));
    (void)hyperdos_x86_write_memory_byte(processor,
                                         HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                         (uint16_t)(stackPointer + 1u),
                                         (uint8_t)(value >> HYPERDOS_X86_BYTE_BIT_COUNT));
}

static void hyperdos_pc_bios_runtime_call_pointing_device_handler(hyperdos_x86_processor*  processor,
                                                                  hyperdos_pc_system_bios* systemBios)
{
    uint16_t returnOffset       = (uint16_t)processor->instructionPointer;
    uint16_t returnSegment      = processor->segmentRegisters[HYPERDOS_X86_SEGMENT_REGISTER_CODE];
    uint16_t statusWord         = (uint16_t)(systemBios->pointingDevicePacketBytes[0] &
                                     HYPERDOS_PC_BIOS_RUNTIME_AUXILIARY_CALLBACK_STATUS_MASK);
    uint16_t horizontalDataWord = systemBios->pointingDevicePacketBytes[1];
    uint16_t verticalDataWord   = systemBios->pointingDevicePacketBytes[2];

    hyperdos_pc_bios_runtime_push_processor_word(processor, returnSegment);
    hyperdos_pc_bios_runtime_push_processor_word(processor, returnOffset);
    hyperdos_pc_bios_runtime_push_processor_word(processor, statusWord);
    hyperdos_pc_bios_runtime_push_processor_word(processor, horizontalDataWord);
    hyperdos_pc_bios_runtime_push_processor_word(processor, verticalDataWord);
    hyperdos_pc_bios_runtime_push_processor_word(processor, HYPERDOS_PC_BIOS_RUNTIME_AUXILIARY_CALLBACK_PADDING_WORD);
    hyperdos_pc_bios_runtime_push_processor_word(processor,
                                                 HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_CALLBACK_CLEANUP_STUB_SEGMENT);
    hyperdos_pc_bios_runtime_push_processor_word(processor,
                                                 HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_CALLBACK_CLEANUP_STUB_OFFSET);
    hyperdos_x86_set_segment_register(processor,
                                      HYPERDOS_X86_SEGMENT_REGISTER_CODE,
                                      systemBios->pointingDeviceHandlerSegment);
    processor->instructionPointer = systemBios->pointingDeviceHandlerOffset;
}

static hyperdos_x86_execution_result hyperdos_pc_bios_runtime_handle_auxiliary_device_hardware_interrupt(
        hyperdos_x86_processor*   processor,
        hyperdos_pc_bios_runtime* biosRuntime)
{
    hyperdos_pc_system_bios* systemBios = biosRuntime->systemBios;
    uint8_t packetByte = hyperdos_intel_8042_keyboard_controller_read_byte(&biosRuntime->pc->keyboardController,
                                                                           HYPERDOS_PC_KEYBOARD_CONTROLLER_DATA_PORT);

    if (systemBios->pointingDevicePacketByteCount == 0u &&
        (packetByte & HYPERDOS_PC_BIOS_RUNTIME_AUXILIARY_PACKET_ALWAYS_ONE) == 0u)
    {
        return HYPERDOS_X86_EXECUTION_OK;
    }

    systemBios->pointingDevicePacketBytes[systemBios->pointingDevicePacketByteCount] = packetByte;
    ++systemBios->pointingDevicePacketByteCount;
    if (systemBios->pointingDevicePacketByteCount < HYPERDOS_PC_BIOS_RUNTIME_AUXILIARY_PACKET_BYTE_COUNT)
    {
        return HYPERDOS_X86_EXECUTION_OK;
    }

    systemBios->pointingDevicePacketByteCount = 0u;
    if (systemBios->pointingDeviceEnabled != 0u &&
        (systemBios->pointingDeviceHandlerOffset != 0u || systemBios->pointingDeviceHandlerSegment != 0u))
    {
        hyperdos_pc_bios_runtime_call_pointing_device_handler(processor, systemBios);
    }
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_pc_bios_runtime_handle_keyboard_hardware_interrupt(
        hyperdos_x86_processor*   processor,
        hyperdos_pc_bios_runtime* biosRuntime)
{
    uint8_t scanCode = (uint8_t)(hyperdos_x86_get_general_register_word(processor,
                                                                        HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) &
                                 HYPERDOS_X86_LOW_BYTE_MASK);

    (void)hyperdos_pc_keyboard_bios_service_hardware_byte(biosRuntime->keyboardBios,
                                                          biosRuntime->keyboardBiosInterface,
                                                          biosRuntime->pc,
                                                          scanCode,
                                                          1,
                                                          biosRuntime->traceFunction,
                                                          biosRuntime->userContext);
    return HYPERDOS_X86_EXECUTION_OK;
}

static int hyperdos_pc_bios_runtime_is_legacy_bios_interrupt(uint8_t interruptNumber)
{
    return interruptNumber == HYPERDOS_PC_BIOS_TIMER_HARDWARE_INTERRUPT ||
           interruptNumber == HYPERDOS_PC_BIOS_KEYBOARD_HARDWARE_INTERRUPT ||
           interruptNumber == HYPERDOS_PC_BIOS_VIDEO_INTERRUPT ||
           interruptNumber == HYPERDOS_PC_BIOS_EQUIPMENT_INTERRUPT ||
           interruptNumber == HYPERDOS_PC_BIOS_MEMORY_SIZE_INTERRUPT ||
           interruptNumber == HYPERDOS_PC_BIOS_DISK_INTERRUPT || interruptNumber == HYPERDOS_PC_BIOS_SERIAL_INTERRUPT ||
           interruptNumber == HYPERDOS_PC_BIOS_SYSTEM_SERVICES_INTERRUPT ||
           interruptNumber == HYPERDOS_PC_BIOS_KEYBOARD_INTERRUPT ||
           interruptNumber == HYPERDOS_PC_BIOS_PRINTER_INTERRUPT ||
           interruptNumber == HYPERDOS_PC_BIOS_TIME_INTERRUPT ||
           interruptNumber == HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_HARDWARE_INTERRUPT ||
           interruptNumber == HYPERDOS_PC_BIOS_USER_TIMER_TICK_INTERRUPT;
}

hyperdos_x86_execution_result hyperdos_pc_bios_runtime_handle_interrupt(hyperdos_x86_processor* processor,
                                                                        uint8_t                 interruptNumber,
                                                                        void*                   userContext)
{
    hyperdos_pc_bios_runtime* biosRuntime   = (hyperdos_pc_bios_runtime*)userContext;
    uint8_t                   serviceNumber = 0u;

    if (processor == NULL || biosRuntime == NULL || biosRuntime->pc == NULL)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }

    serviceNumber = (uint8_t)(hyperdos_x86_get_general_register_word(processor,
                                                                     HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) >>
                              HYPERDOS_PC_BIOS_RUNTIME_SERVICE_REGISTER_SHIFT);
    if (interruptNumber == HYPERDOS_PC_BIOS_RUNTIME_DIVIDE_ERROR_INTERRUPT)
    {
        uint16_t instructionSegment   = processor->lastInstructionSegment;
        uint16_t instructionOffset    = (uint16_t)processor->lastInstructionOffset;
        uint8_t  firstInstructionByte = hyperdos_pc_bios_runtime_read_guest_instruction_byte(biosRuntime,
                                                                                            instructionSegment,
                                                                                            instructionOffset);
        uint8_t  secondInstructionByte =
                hyperdos_pc_bios_runtime_read_guest_instruction_byte(biosRuntime,
                                                                     instructionSegment,
                                                                     (uint16_t)(instructionOffset + 1u));
        uint8_t thirdInstructionByte =
                hyperdos_pc_bios_runtime_read_guest_instruction_byte(biosRuntime,
                                                                     instructionSegment,
                                                                     (uint16_t)(instructionOffset + 2u));
        uint8_t fourthInstructionByte =
                hyperdos_pc_bios_runtime_read_guest_instruction_byte(biosRuntime,
                                                                     instructionSegment,
                                                                     (uint16_t)(instructionOffset + 3u));

        hyperdos_pc_bios_runtime_trace(biosRuntime,
                                       "divide error fault=%04X:%04X bytes=%02X %02X %02X %02X",
                                       instructionSegment,
                                       instructionOffset,
                                       firstInstructionByte,
                                       secondInstructionByte,
                                       thirdInstructionByte,
                                       fourthInstructionByte);
        return HYPERDOS_X86_EXECUTION_INTERRUPT_NOT_HANDLED;
    }
    if (interruptNumber == HYPERDOS_PC_BIOS_RUNTIME_KOREAN_VIDEO_INTERRUPT)
    {
        return hyperdos_pc_bios_runtime_handle_korean_video_interrupt(processor, biosRuntime, serviceNumber);
    }
    if (interruptNumber == HYPERDOS_PC_BIOS_KEYBOARD_SERVICE_INTERRUPT)
    {
        return hyperdos_pc_bios_runtime_handle_keyboard_hardware_interrupt(processor, biosRuntime);
    }
    if (interruptNumber == HYPERDOS_PC_BIOS_TIMER_SERVICE_INTERRUPT)
    {
        hyperdos_pc_system_bios_advance_timer_tick(biosRuntime->pc);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (interruptNumber == HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_SERVICE_INTERRUPT)
    {
        return hyperdos_pc_bios_runtime_handle_auxiliary_device_hardware_interrupt(processor, biosRuntime);
    }
    if (interruptNumber == HYPERDOS_PC_BIOS_KEYBOARD_SOFTWARE_SERVICE_INTERRUPT)
    {
        hyperdos_x86_execution_result
                result = hyperdos_pc_keyboard_bios_handle_interrupt(processor,
                                                                    biosRuntime->keyboardBios,
                                                                    biosRuntime->keyboardBiosInterface,
                                                                    biosRuntime->pc,
                                                                    serviceNumber);

        if (result == HYPERDOS_X86_EXECUTION_OK)
        {
            if (serviceNumber == HYPERDOS_PC_KEYBOARD_BIOS_STATUS_SERVICE)
            {
                hyperdos_pc_bios_synchronize_interrupt_return_flag(biosRuntime->pc,
                                                                   processor,
                                                                   HYPERDOS_X86_FLAG_ZERO |
                                                                           HYPERDOS_X86_FLAG_INTERRUPT_ENABLE);
            }
            else if (serviceNumber == HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_STATUS_SERVICE)
            {
                hyperdos_pc_bios_synchronize_interrupt_return_flag(biosRuntime->pc,
                                                                   processor,
                                                                   HYPERDOS_X86_FLAG_ZERO |
                                                                           HYPERDOS_X86_FLAG_INTERRUPT_ENABLE);
            }
            else if (serviceNumber != HYPERDOS_PC_KEYBOARD_BIOS_READ_SERVICE &&
                     serviceNumber != HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_READ_SERVICE &&
                     serviceNumber != HYPERDOS_PC_KEYBOARD_BIOS_SHIFT_STATUS_SERVICE &&
                     serviceNumber != HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_SHIFT_STATUS_SERVICE)
            {
                hyperdos_pc_bios_synchronize_interrupt_return_carry_flag(biosRuntime->pc, processor);
            }
        }
        return result;
    }
    if (interruptNumber == HYPERDOS_PC_BIOS_VIDEO_SERVICE_INTERRUPT)
    {
        hyperdos_x86_execution_result result = hyperdos_pc_video_bios_handle_interrupt(processor,
                                                                                       biosRuntime->videoBiosInterface,
                                                                                       serviceNumber);

        if (result == HYPERDOS_X86_EXECUTION_OK)
        {
            hyperdos_pc_bios_synchronize_interrupt_return_carry_flag(biosRuntime->pc, processor);
        }
        return result;
    }
    if (interruptNumber == HYPERDOS_PC_BIOS_DISK_SERVICE_INTERRUPT)
    {
        hyperdos_x86_execution_result result = hyperdos_pc_disk_bios_handle_interrupt(processor,
                                                                                      biosRuntime->diskBiosInterface);

        if (result == HYPERDOS_X86_EXECUTION_OK)
        {
            hyperdos_pc_bios_synchronize_interrupt_return_carry_flag(biosRuntime->pc, processor);
        }
        return result;
    }
    if (interruptNumber == HYPERDOS_PC_BIOS_EQUIPMENT_SERVICE_INTERRUPT)
    {
        uint8_t pointingDevicePresent = hyperdos_pc_bios_runtime_pointing_device_is_present(biosRuntime);
        uint8_t floppyDriveCount      = hyperdos_pc_bios_runtime_get_floppy_drive_count(biosRuntime);

        return hyperdos_pc_system_bios_handle_equipment_interrupt(processor,
                                                                  biosRuntime->coprocessorEnabled,
                                                                  floppyDriveCount,
                                                                  pointingDevicePresent);
    }
    if (interruptNumber == HYPERDOS_PC_BIOS_MEMORY_SIZE_SERVICE_INTERRUPT)
    {
        return hyperdos_pc_system_bios_handle_memory_size_interrupt(processor);
    }
    if (interruptNumber == HYPERDOS_PC_BIOS_SERIAL_SERVICE_INTERRUPT)
    {
        hyperdos_x86_execution_result result = hyperdos_pc_system_bios_handle_serial_interrupt(processor,
                                                                                               serviceNumber);

        if (result == HYPERDOS_X86_EXECUTION_INTERRUPT_NOT_HANDLED)
        {
            hyperdos_pc_bios_set_carry_flag(processor, 1);
            result = HYPERDOS_X86_EXECUTION_OK;
        }
        if (result == HYPERDOS_X86_EXECUTION_OK)
        {
            hyperdos_pc_bios_synchronize_interrupt_return_carry_flag(biosRuntime->pc, processor);
        }
        return result;
    }
    if (interruptNumber == HYPERDOS_PC_BIOS_SYSTEM_SERVICES_SERVICE_INTERRUPT)
    {
        hyperdos_x86_execution_result
                result = hyperdos_pc_system_bios_handle_system_services_interrupt(processor,
                                                                                  biosRuntime->pc,
                                                                                  biosRuntime->systemBios,
                                                                                  serviceNumber);

        if (result == HYPERDOS_X86_EXECUTION_OK)
        {
            hyperdos_pc_bios_synchronize_interrupt_return_carry_flag(biosRuntime->pc, processor);
        }
        return result;
    }
    if (interruptNumber == HYPERDOS_PC_BIOS_PRINTER_SERVICE_INTERRUPT)
    {
        hyperdos_x86_execution_result result = hyperdos_pc_system_bios_handle_printer_interrupt(processor,
                                                                                                serviceNumber);

        if (result == HYPERDOS_X86_EXECUTION_INTERRUPT_NOT_HANDLED)
        {
            hyperdos_pc_bios_set_carry_flag(processor, 1);
            result = HYPERDOS_X86_EXECUTION_OK;
        }
        if (result == HYPERDOS_X86_EXECUTION_OK)
        {
            hyperdos_pc_bios_synchronize_interrupt_return_carry_flag(biosRuntime->pc, processor);
        }
        return result;
    }
    if (interruptNumber == HYPERDOS_PC_BIOS_TIME_SERVICE_INTERRUPT)
    {
        hyperdos_x86_execution_result result = hyperdos_pc_system_bios_handle_time_interrupt(processor,
                                                                                             biosRuntime->pc,
                                                                                             serviceNumber);

        if (result == HYPERDOS_X86_EXECUTION_INTERRUPT_NOT_HANDLED)
        {
            hyperdos_pc_bios_set_carry_flag(processor, 1);
            result = HYPERDOS_X86_EXECUTION_OK;
        }
        if (result == HYPERDOS_X86_EXECUTION_OK)
        {
            hyperdos_pc_bios_synchronize_interrupt_return_carry_flag(biosRuntime->pc, processor);
        }
        return result;
    }
    if (hyperdos_pc_bios_runtime_is_legacy_bios_interrupt(interruptNumber))
    {
        return HYPERDOS_X86_EXECUTION_INTERRUPT_NOT_HANDLED;
    }
    if (interruptNumber == HYPERDOS_PC_BIOS_RUNTIME_EXPANDED_MEMORY_MANAGER_INTERRUPT)
    {
        return hyperdos_pc_system_bios_handle_expanded_memory_manager_interrupt(processor, serviceNumber);
    }

    return HYPERDOS_X86_EXECUTION_INTERRUPT_NOT_HANDLED;
}

hyperdos_x86_execution_result hyperdos_pc_bios_runtime_execute_processor_slice(hyperdos_pc_bios_runtime* biosRuntime,
                                                                               uint64_t  instructionLimit,
                                                                               uint64_t* executedInstructionCount)
{
    hyperdos_x86_execution_result executionResult                 = HYPERDOS_X86_EXECUTION_OK;
    uint64_t                      previousInstructionCount        = 0u;
    uint64_t                      currentExecutedInstructionCount = 0u;

    if (executedInstructionCount != NULL)
    {
        *executedInstructionCount = 0u;
    }
    if (biosRuntime == NULL || biosRuntime->pc == NULL)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }

    executionResult = hyperdos_pc_bios_runtime_service_pending_hardware_interrupts(biosRuntime);
    if (executionResult != HYPERDOS_X86_EXECUTION_OK)
    {
        return executionResult;
    }

    previousInstructionCount        = biosRuntime->pc->processor.executedInstructionCount;
    executionResult                 = hyperdos_x86_execute(&biosRuntime->pc->processor, instructionLimit);
    currentExecutedInstructionCount = biosRuntime->pc->processor.executedInstructionCount - previousInstructionCount;
    if (currentExecutedInstructionCount != 0u)
    {
        hyperdos_intel_8284_clock_generator_step(&biosRuntime->pc->clockGenerator,
                                                 &biosRuntime->pc->bus,
                                                 currentExecutedInstructionCount);
    }
    if (executedInstructionCount != NULL)
    {
        *executedInstructionCount = currentExecutedInstructionCount;
    }
    return executionResult;
}

int hyperdos_pc_bios_runtime_prepare_boot_from_disk_image(hyperdos_pc_bios_runtime*     biosRuntime,
                                                          const hyperdos_pc_disk_image* bootDisk,
                                                          const hyperdos_pc_disk_image* activeFloppyDisk,
                                                          uint8_t                       fixedDiskCount)
{
    uint8_t bootSectorBytes[HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_SIZE];

    if (biosRuntime == NULL || biosRuntime->pc == NULL || bootDisk == NULL)
    {
        return 0;
    }
    if (bootDisk->bytesPerSector != HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_SIZE ||
        hyperdos_pc_disk_image_read_sectors(bootDisk, 0u, 1u, bootSectorBytes) != HYPERDOS_PC_DISK_TRANSFER_OK)
    {
        return 0;
    }
    if (!hyperdos_pc_load_boot_sector(biosRuntime->pc, bootSectorBytes, sizeof(bootSectorBytes)))
    {
        return 0;
    }

    hyperdos_pc_bios_runtime_initialize_data_area(biosRuntime, activeFloppyDisk, fixedDiskCount);
    if (biosRuntime->videoBiosInterface != NULL &&
        biosRuntime->videoBiosInterface->initializeStaticFunctionalityTable != NULL)
    {
        biosRuntime->videoBiosInterface->initializeStaticFunctionalityTable(
                biosRuntime->videoBiosInterface->userContext);
    }
    hyperdos_pc_system_bios_install_configuration_table(biosRuntime->pc, biosRuntime->systemBios);
    hyperdos_pc_system_bios_install_identification(biosRuntime->pc, biosRuntime->systemBios);
    hyperdos_pc_bios_install_interrupt_vector_stubs(biosRuntime->pc);
    hyperdos_pc_prepare_boot_sector_execution(biosRuntime->pc, bootDisk->driveNumber);
    hyperdos_x86_set_interrupt_handler(&biosRuntime->pc->processor,
                                       hyperdos_pc_bios_runtime_handle_interrupt,
                                       biosRuntime);
    return 1;
}

hyperdos_x86_execution_result hyperdos_pc_bios_runtime_service_pending_hardware_interrupts(
        hyperdos_pc_bios_runtime* biosRuntime)
{
    uint8_t                 interruptNumber = 0u;
    hyperdos_x86_processor* processor       = NULL;

    if (biosRuntime == NULL || biosRuntime->pc == NULL || biosRuntime->systemBios == NULL)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }

    processor = &biosRuntime->pc->processor;
    hyperdos_pc_system_bios_service_wait_event(biosRuntime->systemBios, biosRuntime->pc);
    if (biosRuntime->drainKeyboardInput != NULL)
    {
        biosRuntime->drainKeyboardInput(biosRuntime->userContext);
    }
    hyperdos_pc_raise_keyboard_controller_interrupt_request(biosRuntime->pc,
                                                            biosRuntime->traceFunction,
                                                            biosRuntime->userContext);
    hyperdos_pc_raise_auxiliary_device_interrupt_request(biosRuntime->pc,
                                                         biosRuntime->traceFunction,
                                                         biosRuntime->userContext);
    hyperdos_pc_raise_interval_timer_interrupt_request(biosRuntime->pc,
                                                       biosRuntime->traceFunction,
                                                       biosRuntime->userContext);

    if (!hyperdos_x86_processor_accepts_maskable_interrupt(processor))
    {
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (!hyperdos_pc_acknowledge_interrupt_request(biosRuntime->pc, &interruptNumber))
    {
        return HYPERDOS_X86_EXECUTION_OK;
    }
    hyperdos_pc_bios_runtime_trace(biosRuntime,
                                   "pic acknowledge interrupt=%02X mask=%02X request=%02X service=%02X "
                                   "vector-base=%02X",
                                   interruptNumber,
                                   biosRuntime->pc->programmableInterruptController.interruptMaskRegister,
                                   biosRuntime->pc->programmableInterruptController.interruptRequestRegister,
                                   biosRuntime->pc->programmableInterruptController.inServiceRegister,
                                   biosRuntime->pc->programmableInterruptController.vectorBase);
    return hyperdos_x86_request_maskable_interrupt(processor, interruptNumber);
}
