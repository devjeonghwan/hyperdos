#include "hyperdos/pc_bios.h"

#include <stddef.h>

#include "hyperdos/pc_firmware.h"

enum
{
    HYPERDOS_PC_BIOS_INTERRUPT_RETURN_OFFSET_STACK_OFFSET               = 0u,
    HYPERDOS_PC_BIOS_INTERRUPT_RETURN_SEGMENT_STACK_OFFSET              = 2u,
    HYPERDOS_PC_BIOS_INTERRUPT_RETURN_FLAGS_STACK_OFFSET                = 4u,
    HYPERDOS_PC_BIOS_NESTED_INTERRUPT_RETURN_FLAGS_STACK_OFFSET         = 10u,
    HYPERDOS_PC_BIOS_INTERRUPT_VECTOR_BYTE_COUNT                        = 4u,
    HYPERDOS_PC_BIOS_FIRMWARE_STUB_SEGMENT                              = 0xF000u,
    HYPERDOS_PC_BIOS_SYSTEM_SERVICES_KEYBOARD_INTERCEPT_SERVICE         = 0x4Fu,
    HYPERDOS_PC_BIOS_PROGRAMMABLE_INTERRUPT_CONTROLLER_END_OF_INTERRUPT = 0x20u,
    HYPERDOS_PC_BIOS_OPERATION_CODE_PUSH_ACCUMULATOR                    = 0x50u,
    HYPERDOS_PC_BIOS_OPERATION_CODE_POP_ACCUMULATOR                     = 0x58u,
    HYPERDOS_PC_BIOS_OPERATION_CODE_INPUT_ACCUMULATOR_IMMEDIATE         = 0xE4u,
    HYPERDOS_PC_BIOS_OPERATION_CODE_MOVE_HIGH_ACCUMULATOR_IMMEDIATE     = 0xB4u,
    HYPERDOS_PC_BIOS_OPERATION_CODE_MOVE_LOW_ACCUMULATOR_IMMEDIATE      = 0xB0u,
    HYPERDOS_PC_BIOS_OPERATION_CODE_SET_CARRY_FLAG                      = 0xF9u,
    HYPERDOS_PC_BIOS_OPERATION_CODE_CLEAR_INTERRUPT_FLAG                = 0xFAu,
    HYPERDOS_PC_BIOS_OPERATION_CODE_ADD_STACK_POINTER_IMMEDIATE         = 0x83u,
    HYPERDOS_PC_BIOS_OPERATION_CODE_ADD_STACK_POINTER_REGISTER_BYTE     = 0xC4u,
    HYPERDOS_PC_BIOS_OPERATION_CODE_INTERRUPT_IMMEDIATE                 = 0xCDu,
    HYPERDOS_PC_BIOS_OPERATION_CODE_JUMP_IF_NOT_CARRY_SHORT             = 0x73u,
    HYPERDOS_PC_BIOS_OPERATION_CODE_OUTPUT_IMMEDIATE_ACCUMULATOR        = 0xE6u,
    HYPERDOS_PC_BIOS_OPERATION_CODE_RETURN_FAR                          = 0xCBu,
    HYPERDOS_PC_BIOS_OPERATION_CODE_INTERRUPT_RETURN                    = 0xCFu
};

static uint8_t hyperdos_pc_bios_read_guest_memory_byte(hyperdos_pc* pc, uint32_t physicalAddress)
{
    return hyperdos_bus_read_memory_byte_or_open_bus(&pc->bus, physicalAddress & HYPERDOS_X86_ADDRESS_MASK);
}

static uint16_t hyperdos_pc_bios_read_guest_memory_word(hyperdos_pc* pc, uint32_t physicalAddress)
{
    uint16_t lowByte  = hyperdos_pc_bios_read_guest_memory_byte(pc, physicalAddress);
    uint16_t highByte = hyperdos_pc_bios_read_guest_memory_byte(pc, physicalAddress + 1u);

    return (uint16_t)(lowByte | (highByte << HYPERDOS_X86_BYTE_BIT_COUNT));
}

static void hyperdos_pc_bios_write_guest_memory_word(hyperdos_pc* pc, uint32_t physicalAddress, uint16_t value)
{
    hyperdos_bus_write_memory_byte_if_mapped(&pc->bus,
                                             physicalAddress & HYPERDOS_X86_ADDRESS_MASK,
                                             (uint8_t)(value & 0x00FFu));
    hyperdos_bus_write_memory_byte_if_mapped(&pc->bus,
                                             (physicalAddress + 1u) & HYPERDOS_X86_ADDRESS_MASK,
                                             (uint8_t)(value >> HYPERDOS_X86_BYTE_BIT_COUNT));
}

static uint32_t hyperdos_pc_bios_get_stack_physical_address(const hyperdos_x86_processor* processor,
                                                            uint16_t                      stackOffset)
{
    uint16_t stackPointer    = hyperdos_x86_get_general_register_word(processor,
                                                                   HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    uint32_t physicalAddress = 0u;

    (void)hyperdos_x86_translate_logical_to_physical_address(processor,
                                                             HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                                             (uint16_t)(stackPointer + stackOffset),
                                                             &physicalAddress);
    return physicalAddress;
}

static int hyperdos_pc_bios_monitor_service_returns_to_interrupt_return_instruction(
        hyperdos_pc*                  pc,
        const hyperdos_x86_processor* processor)
{
    uint32_t returnOffsetPhysicalAddress =
            hyperdos_pc_bios_get_stack_physical_address(processor,
                                                        HYPERDOS_PC_BIOS_INTERRUPT_RETURN_OFFSET_STACK_OFFSET);
    uint32_t returnSegmentPhysicalAddress =
            hyperdos_pc_bios_get_stack_physical_address(processor,
                                                        HYPERDOS_PC_BIOS_INTERRUPT_RETURN_SEGMENT_STACK_OFFSET);
    uint16_t returnOffset          = hyperdos_pc_bios_read_guest_memory_word(pc, returnOffsetPhysicalAddress);
    uint16_t returnSegment         = hyperdos_pc_bios_read_guest_memory_word(pc, returnSegmentPhysicalAddress);
    uint32_t returnPhysicalAddress = (((uint32_t)returnSegment << HYPERDOS_X86_SEGMENT_SHIFT) + returnOffset) &
                                     HYPERDOS_X86_ADDRESS_MASK;

    return returnSegment == HYPERDOS_PC_BIOS_FIRMWARE_STUB_SEGMENT &&
           hyperdos_pc_bios_read_guest_memory_byte(pc, returnPhysicalAddress) ==
                   HYPERDOS_PC_BIOS_OPERATION_CODE_INTERRUPT_RETURN;
}

static void hyperdos_pc_bios_synchronize_interrupt_return_flag_at_stack_offset(hyperdos_pc*            pc,
                                                                               hyperdos_x86_processor* processor,
                                                                               uint16_t                stackOffset,
                                                                               uint16_t                flagMask)
{
    uint32_t flagsPhysicalAddress = hyperdos_pc_bios_get_stack_physical_address(processor, stackOffset);
    uint16_t flags                = hyperdos_pc_bios_read_guest_memory_word(pc, flagsPhysicalAddress);

    flags  = (uint16_t)((flags & (uint16_t)~flagMask) | (hyperdos_x86_get_flags_word(processor) & flagMask));
    flags |= HYPERDOS_X86_FLAG_RESERVED;
    hyperdos_pc_bios_write_guest_memory_word(pc, flagsPhysicalAddress, flags);
}

static void hyperdos_pc_bios_install_interrupt_vector(hyperdos_pc* pc,
                                                      uint8_t      interruptNumber,
                                                      uint16_t     segment,
                                                      uint16_t     offset)
{
    uint32_t interruptVectorAddress = (uint32_t)interruptNumber * HYPERDOS_PC_BIOS_INTERRUPT_VECTOR_BYTE_COUNT;

    hyperdos_pc_bios_write_guest_memory_word(pc, interruptVectorAddress, offset);
    hyperdos_pc_bios_write_guest_memory_word(pc, interruptVectorAddress + HYPERDOS_X86_WORD_SIZE, segment);
}

static void hyperdos_pc_bios_install_timer_hardware_interrupt_vector_stub(hyperdos_pc* pc)
{
    static const uint8_t timerHardwareInterruptStubBytes[] =
            {HYPERDOS_PC_BIOS_OPERATION_CODE_PUSH_ACCUMULATOR,
             HYPERDOS_PC_BIOS_OPERATION_CODE_INTERRUPT_IMMEDIATE,
             HYPERDOS_PC_BIOS_TIMER_SERVICE_INTERRUPT,
             HYPERDOS_PC_BIOS_OPERATION_CODE_INTERRUPT_IMMEDIATE,
             HYPERDOS_PC_BIOS_USER_TIMER_TICK_INTERRUPT,
             HYPERDOS_PC_BIOS_OPERATION_CODE_CLEAR_INTERRUPT_FLAG,
             HYPERDOS_PC_BIOS_OPERATION_CODE_MOVE_LOW_ACCUMULATOR_IMMEDIATE,
             HYPERDOS_PC_BIOS_PROGRAMMABLE_INTERRUPT_CONTROLLER_END_OF_INTERRUPT,
             HYPERDOS_PC_BIOS_OPERATION_CODE_OUTPUT_IMMEDIATE_ACCUMULATOR,
             HYPERDOS_PC_PROGRAMMABLE_INTERRUPT_CONTROLLER_PORT,
             HYPERDOS_PC_BIOS_OPERATION_CODE_POP_ACCUMULATOR,
             HYPERDOS_PC_BIOS_OPERATION_CODE_INTERRUPT_RETURN};
    uint32_t stubPhysicalAddress = ((uint32_t)HYPERDOS_PC_BIOS_TIMER_HARDWARE_STUB_SEGMENT
                                    << HYPERDOS_X86_SEGMENT_SHIFT) +
                                   HYPERDOS_PC_BIOS_TIMER_HARDWARE_STUB_OFFSET;
    size_t byteIndex = 0u;

    for (byteIndex = 0u; byteIndex < sizeof(timerHardwareInterruptStubBytes); ++byteIndex)
    {
        hyperdos_pc_firmware_write_byte(pc,
                                        stubPhysicalAddress + (uint32_t)byteIndex,
                                        timerHardwareInterruptStubBytes[byteIndex]);
    }
    hyperdos_pc_bios_install_interrupt_vector(pc,
                                              HYPERDOS_PC_BIOS_TIMER_HARDWARE_INTERRUPT,
                                              HYPERDOS_PC_BIOS_TIMER_HARDWARE_STUB_SEGMENT,
                                              HYPERDOS_PC_BIOS_TIMER_HARDWARE_STUB_OFFSET);
}

static void hyperdos_pc_bios_install_user_timer_tick_interrupt_vector_stub(hyperdos_pc* pc)
{
    uint32_t stubPhysicalAddress = ((uint32_t)HYPERDOS_PC_BIOS_USER_TIMER_TICK_STUB_SEGMENT
                                    << HYPERDOS_X86_SEGMENT_SHIFT) +
                                   HYPERDOS_PC_BIOS_USER_TIMER_TICK_STUB_OFFSET;

    hyperdos_pc_firmware_write_byte(pc, stubPhysicalAddress, HYPERDOS_PC_BIOS_OPERATION_CODE_INTERRUPT_RETURN);
    hyperdos_pc_bios_install_interrupt_vector(pc,
                                              HYPERDOS_PC_BIOS_USER_TIMER_TICK_INTERRUPT,
                                              HYPERDOS_PC_BIOS_USER_TIMER_TICK_STUB_SEGMENT,
                                              HYPERDOS_PC_BIOS_USER_TIMER_TICK_STUB_OFFSET);
}

static void hyperdos_pc_bios_install_keyboard_hardware_interrupt_vector_stub(hyperdos_pc* pc)
{
    static const uint8_t keyboardHardwareInterruptStubBytes[] =
            {HYPERDOS_PC_BIOS_OPERATION_CODE_PUSH_ACCUMULATOR,
             HYPERDOS_PC_BIOS_OPERATION_CODE_INPUT_ACCUMULATOR_IMMEDIATE,
             HYPERDOS_PC_KEYBOARD_CONTROLLER_DATA_PORT,
             HYPERDOS_PC_BIOS_OPERATION_CODE_MOVE_HIGH_ACCUMULATOR_IMMEDIATE,
             HYPERDOS_PC_BIOS_SYSTEM_SERVICES_KEYBOARD_INTERCEPT_SERVICE,
             HYPERDOS_PC_BIOS_OPERATION_CODE_SET_CARRY_FLAG,
             HYPERDOS_PC_BIOS_OPERATION_CODE_INTERRUPT_IMMEDIATE,
             HYPERDOS_PC_BIOS_SYSTEM_SERVICES_INTERRUPT,
             HYPERDOS_PC_BIOS_OPERATION_CODE_JUMP_IF_NOT_CARRY_SHORT,
             0x02u,
             HYPERDOS_PC_BIOS_OPERATION_CODE_INTERRUPT_IMMEDIATE,
             HYPERDOS_PC_BIOS_KEYBOARD_SERVICE_INTERRUPT,
             HYPERDOS_PC_BIOS_OPERATION_CODE_CLEAR_INTERRUPT_FLAG,
             HYPERDOS_PC_BIOS_OPERATION_CODE_MOVE_LOW_ACCUMULATOR_IMMEDIATE,
             HYPERDOS_PC_BIOS_PROGRAMMABLE_INTERRUPT_CONTROLLER_END_OF_INTERRUPT,
             HYPERDOS_PC_BIOS_OPERATION_CODE_OUTPUT_IMMEDIATE_ACCUMULATOR,
             HYPERDOS_PC_PROGRAMMABLE_INTERRUPT_CONTROLLER_PORT,
             HYPERDOS_PC_BIOS_OPERATION_CODE_POP_ACCUMULATOR,
             HYPERDOS_PC_BIOS_OPERATION_CODE_INTERRUPT_RETURN};
    uint32_t stubPhysicalAddress = ((uint32_t)HYPERDOS_PC_BIOS_KEYBOARD_HARDWARE_STUB_SEGMENT
                                    << HYPERDOS_X86_SEGMENT_SHIFT) +
                                   HYPERDOS_PC_BIOS_KEYBOARD_HARDWARE_STUB_OFFSET;
    size_t byteIndex = 0u;

    for (byteIndex = 0u; byteIndex < sizeof(keyboardHardwareInterruptStubBytes); ++byteIndex)
    {
        hyperdos_pc_firmware_write_byte(pc,
                                        stubPhysicalAddress + (uint32_t)byteIndex,
                                        keyboardHardwareInterruptStubBytes[byteIndex]);
    }
    hyperdos_pc_bios_install_interrupt_vector(pc,
                                              HYPERDOS_PC_BIOS_KEYBOARD_HARDWARE_INTERRUPT,
                                              HYPERDOS_PC_BIOS_KEYBOARD_HARDWARE_STUB_SEGMENT,
                                              HYPERDOS_PC_BIOS_KEYBOARD_HARDWARE_STUB_OFFSET);
}

static void hyperdos_pc_bios_install_auxiliary_device_hardware_interrupt_vector_stub(hyperdos_pc* pc)
{
    static const uint8_t auxiliaryDeviceHardwareInterruptStubBytes[] =
            {HYPERDOS_PC_BIOS_OPERATION_CODE_PUSH_ACCUMULATOR,
             HYPERDOS_PC_BIOS_OPERATION_CODE_INTERRUPT_IMMEDIATE,
             HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_SERVICE_INTERRUPT,
             HYPERDOS_PC_BIOS_OPERATION_CODE_CLEAR_INTERRUPT_FLAG,
             HYPERDOS_PC_BIOS_OPERATION_CODE_MOVE_LOW_ACCUMULATOR_IMMEDIATE,
             HYPERDOS_PC_BIOS_PROGRAMMABLE_INTERRUPT_CONTROLLER_END_OF_INTERRUPT,
             HYPERDOS_PC_BIOS_OPERATION_CODE_OUTPUT_IMMEDIATE_ACCUMULATOR,
             HYPERDOS_PC_SLAVE_PROGRAMMABLE_INTERRUPT_CONTROLLER_PORT,
             HYPERDOS_PC_BIOS_OPERATION_CODE_OUTPUT_IMMEDIATE_ACCUMULATOR,
             HYPERDOS_PC_PROGRAMMABLE_INTERRUPT_CONTROLLER_PORT,
             HYPERDOS_PC_BIOS_OPERATION_CODE_POP_ACCUMULATOR,
             HYPERDOS_PC_BIOS_OPERATION_CODE_INTERRUPT_RETURN};
    static const uint8_t auxiliaryDeviceCallbackCleanupStubBytes[] =
            {HYPERDOS_PC_BIOS_OPERATION_CODE_ADD_STACK_POINTER_IMMEDIATE,
             HYPERDOS_PC_BIOS_OPERATION_CODE_ADD_STACK_POINTER_REGISTER_BYTE,
             0x08u,
             HYPERDOS_PC_BIOS_OPERATION_CODE_RETURN_FAR};
    uint32_t stubPhysicalAddress = ((uint32_t)HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_HARDWARE_STUB_SEGMENT
                                    << HYPERDOS_X86_SEGMENT_SHIFT) +
                                   HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_HARDWARE_STUB_OFFSET;
    uint32_t cleanupStubPhysicalAddress = ((uint32_t)HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_CALLBACK_CLEANUP_STUB_SEGMENT
                                           << HYPERDOS_X86_SEGMENT_SHIFT) +
                                          HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_CALLBACK_CLEANUP_STUB_OFFSET;
    size_t byteIndex = 0u;

    for (byteIndex = 0u; byteIndex < sizeof(auxiliaryDeviceHardwareInterruptStubBytes); ++byteIndex)
    {
        hyperdos_pc_firmware_write_byte(pc,
                                        stubPhysicalAddress + (uint32_t)byteIndex,
                                        auxiliaryDeviceHardwareInterruptStubBytes[byteIndex]);
    }
    for (byteIndex = 0u; byteIndex < sizeof(auxiliaryDeviceCallbackCleanupStubBytes); ++byteIndex)
    {
        hyperdos_pc_firmware_write_byte(pc,
                                        cleanupStubPhysicalAddress + (uint32_t)byteIndex,
                                        auxiliaryDeviceCallbackCleanupStubBytes[byteIndex]);
    }
    hyperdos_pc_bios_install_interrupt_vector(pc,
                                              HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_HARDWARE_INTERRUPT,
                                              HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_HARDWARE_STUB_SEGMENT,
                                              HYPERDOS_PC_BIOS_AUXILIARY_DEVICE_HARDWARE_STUB_OFFSET);
}

void hyperdos_pc_bios_install_interrupt_vector_stubs(hyperdos_pc* pc)
{
    hyperdos_pc_bios_install_timer_hardware_interrupt_vector_stub(pc);
    hyperdos_pc_bios_install_user_timer_tick_interrupt_vector_stub(pc);
    hyperdos_pc_bios_install_keyboard_hardware_interrupt_vector_stub(pc);
    hyperdos_pc_bios_install_auxiliary_device_hardware_interrupt_vector_stub(pc);
    hyperdos_pc_firmware_install_interrupt_vector_stub(pc,
                                                       HYPERDOS_PC_BIOS_VIDEO_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_VIDEO_SERVICE_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_VIDEO_STUB_SEGMENT,
                                                       HYPERDOS_PC_BIOS_VIDEO_STUB_OFFSET);
    hyperdos_pc_firmware_install_interrupt_vector_stub(pc,
                                                       HYPERDOS_PC_BIOS_EQUIPMENT_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_EQUIPMENT_SERVICE_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_EQUIPMENT_STUB_SEGMENT,
                                                       HYPERDOS_PC_BIOS_EQUIPMENT_STUB_OFFSET);
    hyperdos_pc_firmware_install_interrupt_vector_stub(pc,
                                                       HYPERDOS_PC_BIOS_MEMORY_SIZE_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_MEMORY_SIZE_SERVICE_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_MEMORY_SIZE_STUB_SEGMENT,
                                                       HYPERDOS_PC_BIOS_MEMORY_SIZE_STUB_OFFSET);
    hyperdos_pc_firmware_install_interrupt_vector_stub(pc,
                                                       HYPERDOS_PC_BIOS_DISK_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_DISK_SERVICE_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_DISK_STUB_SEGMENT,
                                                       HYPERDOS_PC_BIOS_DISK_STUB_OFFSET);
    hyperdos_pc_firmware_install_interrupt_vector_stub(pc,
                                                       HYPERDOS_PC_BIOS_SERIAL_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_SERIAL_SERVICE_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_SERIAL_STUB_SEGMENT,
                                                       HYPERDOS_PC_BIOS_SERIAL_STUB_OFFSET);
    hyperdos_pc_firmware_install_interrupt_vector_stub(pc,
                                                       HYPERDOS_PC_BIOS_SYSTEM_SERVICES_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_SYSTEM_SERVICES_SERVICE_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_SYSTEM_SERVICES_STUB_SEGMENT,
                                                       HYPERDOS_PC_BIOS_SYSTEM_SERVICES_STUB_OFFSET);
    hyperdos_pc_firmware_install_interrupt_vector_stub(pc,
                                                       HYPERDOS_PC_BIOS_KEYBOARD_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_KEYBOARD_SOFTWARE_SERVICE_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_KEYBOARD_SOFTWARE_STUB_SEGMENT,
                                                       HYPERDOS_PC_BIOS_KEYBOARD_SOFTWARE_STUB_OFFSET);
    hyperdos_pc_firmware_install_interrupt_vector_stub(pc,
                                                       HYPERDOS_PC_BIOS_PRINTER_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_PRINTER_SERVICE_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_PRINTER_STUB_SEGMENT,
                                                       HYPERDOS_PC_BIOS_PRINTER_STUB_OFFSET);
    hyperdos_pc_firmware_install_interrupt_vector_stub(pc,
                                                       HYPERDOS_PC_BIOS_TIME_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_TIME_SERVICE_INTERRUPT,
                                                       HYPERDOS_PC_BIOS_TIME_STUB_SEGMENT,
                                                       HYPERDOS_PC_BIOS_TIME_STUB_OFFSET);
}

void hyperdos_pc_bios_set_carry_flag(hyperdos_x86_processor* processor, int carry)
{
    if (processor == NULL)
    {
        return;
    }
    hyperdos_x86_set_flag(processor, HYPERDOS_X86_FLAG_CARRY, carry);
}

void hyperdos_pc_bios_synchronize_interrupt_return_flag(hyperdos_pc*            pc,
                                                        hyperdos_x86_processor* processor,
                                                        uint16_t                flagMask)
{
    if (pc == NULL || processor == NULL)
    {
        return;
    }

    hyperdos_pc_bios_synchronize_interrupt_return_flag_at_stack_offset(
            pc,
            processor,
            HYPERDOS_PC_BIOS_INTERRUPT_RETURN_FLAGS_STACK_OFFSET,
            flagMask);
    if (hyperdos_pc_bios_monitor_service_returns_to_interrupt_return_instruction(pc, processor))
    {
        hyperdos_pc_bios_synchronize_interrupt_return_flag_at_stack_offset(
                pc,
                processor,
                HYPERDOS_PC_BIOS_NESTED_INTERRUPT_RETURN_FLAGS_STACK_OFFSET,
                flagMask);
    }
}

void hyperdos_pc_bios_synchronize_interrupt_return_carry_flag(hyperdos_pc* pc, hyperdos_x86_processor* processor)
{
    hyperdos_pc_bios_synchronize_interrupt_return_flag(pc, processor, HYPERDOS_X86_FLAG_CARRY);
}

void hyperdos_pc_bios_synchronize_interrupt_return_zero_flag(hyperdos_pc* pc, hyperdos_x86_processor* processor)
{
    hyperdos_pc_bios_synchronize_interrupt_return_flag(pc, processor, HYPERDOS_X86_FLAG_ZERO);
}

int hyperdos_pc_bios_interrupt_vector_matches(hyperdos_pc* pc,
                                              uint8_t      interruptNumber,
                                              uint16_t     expectedSegment,
                                              uint16_t     expectedOffset)
{
    uint32_t vectorAddress = (uint32_t)interruptNumber * HYPERDOS_PC_BIOS_INTERRUPT_VECTOR_BYTE_COUNT;
    uint16_t vectorOffset  = 0u;
    uint16_t vectorSegment = 0u;

    if (pc == NULL)
    {
        return 0;
    }

    vectorOffset  = hyperdos_pc_bios_read_guest_memory_word(pc, vectorAddress);
    vectorSegment = hyperdos_pc_bios_read_guest_memory_word(pc, vectorAddress + HYPERDOS_X86_WORD_SIZE);
    return vectorOffset == expectedOffset && vectorSegment == expectedSegment;
}

int hyperdos_pc_bios_interrupt_vector_is_empty(hyperdos_pc* pc, uint8_t interruptNumber)
{
    return hyperdos_pc_bios_interrupt_vector_matches(pc, interruptNumber, 0u, 0u);
}

int hyperdos_pc_bios_keyboard_hardware_interrupt_vector_is_default(hyperdos_pc* pc)
{
    return hyperdos_pc_bios_interrupt_vector_matches(pc,
                                                     HYPERDOS_PC_BIOS_KEYBOARD_HARDWARE_INTERRUPT,
                                                     HYPERDOS_PC_BIOS_KEYBOARD_HARDWARE_STUB_SEGMENT,
                                                     HYPERDOS_PC_BIOS_KEYBOARD_HARDWARE_STUB_OFFSET);
}
