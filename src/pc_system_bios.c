#include "hyperdos/pc_system_bios.h"

#include <string.h>

#include "hyperdos/pc_bios_data_area.h"
#include "hyperdos/pc_firmware.h"

enum
{
    HYPERDOS_PC_SYSTEM_BIOS_SERVICE_REGISTER_SHIFT                              = 8u,
    HYPERDOS_PC_SYSTEM_BIOS_CONVENTIONAL_MEMORY_KILOBYTES                       = 640u,
    HYPERDOS_PC_SYSTEM_BIOS_CONFIGURATION_TABLE_SEGMENT                         = 0xF000u,
    HYPERDOS_PC_SYSTEM_BIOS_CONFIGURATION_TABLE_OFFSET                          = 0xE6F5u,
    HYPERDOS_PC_SYSTEM_BIOS_CONFIGURATION_TABLE_BYTE_COUNT                      = 10u,
    HYPERDOS_PC_SYSTEM_BIOS_DATE_PHYSICAL_ADDRESS                               = 0xFFFF5u,
    HYPERDOS_PC_SYSTEM_BIOS_MODEL_IDENTIFIER_PHYSICAL_ADDRESS                   = 0xFFFFEu,
    HYPERDOS_PC_SYSTEM_BIOS_SIGNATURE_PHYSICAL_ADDRESS                          = 0xFFFFFu,
    HYPERDOS_PC_SYSTEM_BIOS_SIGNATURE                                           = 0x55u,
    HYPERDOS_PC_SYSTEM_BIOS_EQUIPMENT_FLAGS_DISKETTE_PRESENT                    = 0x0001u,
    HYPERDOS_PC_SYSTEM_BIOS_EQUIPMENT_FLAGS_COPROCESSOR_PRESENT                 = 0x0002u,
    HYPERDOS_PC_SYSTEM_BIOS_EQUIPMENT_FLAGS_POINTING_DEVICE_PRESENT             = 0x0004u,
    HYPERDOS_PC_SYSTEM_BIOS_EQUIPMENT_FLAGS_DISKETTE_DRIVE_COUNT_SHIFT          = 6u,
    HYPERDOS_PC_SYSTEM_BIOS_EQUIPMENT_FLAGS_MAXIMUM_DISKETTE_DRIVE_COUNT        = 4u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_A20_GATE_SERVICE                    = 0x24u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_EVENT_WAIT_SERVICE                  = 0x83u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_GET_EXTENDED_MEMORY_SIZE_SERVICE    = 0x88u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_WAIT_SERVICE                        = 0x86u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_KEYBOARD_INTERCEPT_SERVICE          = 0x4Fu,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_GET_CONFIGURATION_SERVICE           = 0xC0u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_POINTING_DEVICE_SERVICE             = 0xC2u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_SUCCESS_STATUS                      = 0x00u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_WAIT_BUSY_STATUS                    = 0x83u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_UNSUPPORTED_STATUS                  = 0x86u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_A20_DISABLE_SUBSERVICE              = 0x00u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_A20_ENABLE_SUBSERVICE               = 0x01u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_A20_QUERY_SUBSERVICE                = 0x02u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_A20_SUPPORT_SUBSERVICE              = 0x03u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_EVENT_WAIT_SET_SUBSERVICE           = 0x00u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_EVENT_WAIT_CANCEL_SUBSERVICE        = 0x01u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_EVENT_WAIT_FLAG_MASK                = 0x80u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_A20_KEYBOARD_CONTROLLER_SUPPORT     = 0x0002u,
    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_A20_FAST_GATE_SUPPORT               = 0x0001u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_ENABLE_SUBSERVICE                   = 0x00u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_RESET_SUBSERVICE                    = 0x01u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_SET_SAMPLE_RATE_SUBSERVICE          = 0x02u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_SET_RESOLUTION_SUBSERVICE           = 0x03u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_READ_DEVICE_TYPE_SUBSERVICE         = 0x04u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_INITIALIZE_SUBSERVICE               = 0x05u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_EXTENDED_COMMANDS_SUBSERVICE        = 0x06u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_SET_HANDLER_SUBSERVICE              = 0x07u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_SUCCESS                      = 0x00u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_INVALID_FUNCTION             = 0x01u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_INVALID_INPUT                = 0x02u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_NO_HANDLER                   = 0x05u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_TYPE_STANDARD_MOUSE                 = 0x00u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_RESET_PASSED                        = 0xAAu,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_PACKET_LEFT_BUTTON                  = 0x01u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_PACKET_RIGHT_BUTTON                 = 0x02u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_RIGHT_BUTTON                 = 0x01u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_LEFT_BUTTON                  = 0x04u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_SCALING_TWO_TO_ONE           = 0x10u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_ENABLED                      = 0x20u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_DEFAULT_SAMPLE_RATE                 = 100u,
    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_DEFAULT_RESOLUTION                  = 2u,
    HYPERDOS_PC_SYSTEM_BIOS_SERIAL_INITIALIZE_SERVICE                           = 0x00u,
    HYPERDOS_PC_SYSTEM_BIOS_SERIAL_WRITE_SERVICE                                = 0x01u,
    HYPERDOS_PC_SYSTEM_BIOS_SERIAL_READ_SERVICE                                 = 0x02u,
    HYPERDOS_PC_SYSTEM_BIOS_SERIAL_STATUS_SERVICE                               = 0x03u,
    HYPERDOS_PC_SYSTEM_BIOS_SERIAL_STATUS_TRANSMIT_READY                        = 0x60u,
    HYPERDOS_PC_SYSTEM_BIOS_SERIAL_STATUS_TIMEOUT                               = 0x80u,
    HYPERDOS_PC_SYSTEM_BIOS_PRINTER_WRITE_SERVICE                               = 0x00u,
    HYPERDOS_PC_SYSTEM_BIOS_PRINTER_INITIALIZE_SERVICE                          = 0x01u,
    HYPERDOS_PC_SYSTEM_BIOS_PRINTER_STATUS_SERVICE                              = 0x02u,
    HYPERDOS_PC_SYSTEM_BIOS_PRINTER_STATUS_SELECTED_READY                       = 0x90u,
    HYPERDOS_PC_SYSTEM_BIOS_TIME_TICKS_SERVICE                                  = 0x00u,
    HYPERDOS_PC_SYSTEM_BIOS_TIME_SET_TICKS_SERVICE                              = 0x01u,
    HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_TIME_SERVICE                        = 0x02u,
    HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_SET_TIME_SERVICE                    = 0x03u,
    HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_DATE_SERVICE                        = 0x04u,
    HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_SET_DATE_SERVICE                    = 0x05u,
    HYPERDOS_PC_SYSTEM_BIOS_BINARY_CODED_DECIMAL_TENS_DIVISOR                   = 10u,
    HYPERDOS_PC_SYSTEM_BIOS_BINARY_CODED_DECIMAL_TENS_SHIFT                     = 4u,
    HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_HOUR                                = 12u,
    HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_MINUTE                              = 0u,
    HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_SECOND                              = 0u,
    HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_CENTURY                             = 20u,
    HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_YEAR                                = 26u,
    HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_MONTH                               = 5u,
    HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_DAY                                 = 1u,
    HYPERDOS_PC_SYSTEM_BIOS_TIMER_TICK_PROCESSOR_CLOCK_DIVISOR                  = 262144u,
    HYPERDOS_PC_SYSTEM_BIOS_TIMER_TICKS_PER_DAY                                 = 0x001800B0u,
    HYPERDOS_PC_SYSTEM_BIOS_MICROSECONDS_PER_SECOND                             = 1000000u,
    HYPERDOS_PC_SYSTEM_BIOS_EXPANDED_MEMORY_MANAGER_STATUS_SERVICE              = 0x40u,
    HYPERDOS_PC_SYSTEM_BIOS_EXPANDED_MEMORY_MANAGER_PAGE_FRAME_SERVICE          = 0x41u,
    HYPERDOS_PC_SYSTEM_BIOS_EXPANDED_MEMORY_MANAGER_PAGE_COUNTS_SERVICE         = 0x42u,
    HYPERDOS_PC_SYSTEM_BIOS_EXPANDED_MEMORY_MANAGER_VERSION_SERVICE             = 0x46u,
    HYPERDOS_PC_SYSTEM_BIOS_EXPANDED_MEMORY_MANAGER_STATUS_NOT_AVAILABLE        = 0x80u,
    HYPERDOS_PC_SYSTEM_BIOS_EXPANDED_MEMORY_MANAGER_STATUS_UNSUPPORTED_FUNCTION = 0x84u
};

static void hyperdos_pc_system_bios_set_carry_flag(hyperdos_x86_16_processor* processor, int carry)
{
    if (carry)
    {
        processor->flags |= HYPERDOS_X86_16_FLAG_CARRY;
    }
    else
    {
        processor->flags &= (uint16_t)~HYPERDOS_X86_16_FLAG_CARRY;
    }
    processor->flags |= HYPERDOS_X86_16_FLAG_RESERVED;
}

static uint8_t hyperdos_pc_system_bios_make_binary_coded_decimal(uint8_t value)
{
    return (uint8_t)(((value / HYPERDOS_PC_SYSTEM_BIOS_BINARY_CODED_DECIMAL_TENS_DIVISOR)
                      << HYPERDOS_PC_SYSTEM_BIOS_BINARY_CODED_DECIMAL_TENS_SHIFT) |
                     (value % HYPERDOS_PC_SYSTEM_BIOS_BINARY_CODED_DECIMAL_TENS_DIVISOR));
}

static void hyperdos_pc_system_bios_set_system_services_status(hyperdos_x86_16_processor* processor,
                                                               uint8_t                    status,
                                                               int                        carry)
{
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] =
            (uint16_t)(((uint16_t)status << HYPERDOS_PC_SYSTEM_BIOS_SERVICE_REGISTER_SHIFT) |
                       (processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] &
                        HYPERDOS_X86_16_LOW_BYTE_MASK));
    hyperdos_pc_system_bios_set_carry_flag(processor, carry);
}

static uint32_t hyperdos_pc_system_bios_get_system_services_wait_microsecond_count(
        const hyperdos_x86_16_processor* processor)
{
    return ((uint32_t)processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER]
            << HYPERDOS_X86_16_WORD_BIT_COUNT) |
           processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA];
}

static uint64_t hyperdos_pc_system_bios_get_system_services_wait_clock_count(const hyperdos_pc*               pc,
                                                                             const hyperdos_x86_16_processor* processor)
{
    uint32_t microsecondCount   = hyperdos_pc_system_bios_get_system_services_wait_microsecond_count(processor);
    uint64_t processorFrequency = 0u;
    uint64_t clockCount         = 0u;

    if (pc == NULL || microsecondCount == 0u)
    {
        return 0u;
    }
    processorFrequency = pc->clockGenerator.processorFrequencyHertz;
    clockCount = ((uint64_t)microsecondCount * processorFrequency + HYPERDOS_PC_SYSTEM_BIOS_MICROSECONDS_PER_SECOND -
                  1u) /
                 HYPERDOS_PC_SYSTEM_BIOS_MICROSECONDS_PER_SECOND;
    return clockCount == 0u ? 1u : clockCount;
}

static void hyperdos_pc_system_bios_complete_wait_event(hyperdos_pc_system_bios* systemBios, hyperdos_pc* pc)
{
    uint8_t flagValue = hyperdos_bus_read_memory_byte_or_open_bus(&pc->bus, systemBios->waitEventFlagPhysicalAddress);

    hyperdos_bus_write_memory_byte_if_mapped(&pc->bus,
                                             systemBios->waitEventFlagPhysicalAddress,
                                             (uint8_t)(flagValue |
                                                       HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_EVENT_WAIT_FLAG_MASK));
    systemBios->waitEventActive = 0u;
}

void hyperdos_pc_system_bios_initialize(hyperdos_pc_system_bios* systemBios)
{
    hyperdos_pc_system_bios_reset(systemBios);
}

void hyperdos_pc_system_bios_reset(hyperdos_pc_system_bios* systemBios)
{
    if (systemBios == NULL)
    {
        return;
    }
    memset(systemBios, 0, sizeof(*systemBios));
    systemBios->modelIdentifier          = HYPERDOS_PC_SYSTEM_BIOS_MODEL_IDENTIFIER_AT;
    systemBios->pointingDeviceSampleRate = HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_DEFAULT_SAMPLE_RATE;
    systemBios->pointingDeviceResolution = HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_DEFAULT_RESOLUTION;
}

void hyperdos_pc_system_bios_set_model_identifier(hyperdos_pc_system_bios* systemBios, uint8_t modelIdentifier)
{
    if (systemBios == NULL)
    {
        return;
    }
    systemBios->modelIdentifier = modelIdentifier != 0u ? modelIdentifier : HYPERDOS_PC_SYSTEM_BIOS_MODEL_IDENTIFIER_AT;
}

uint8_t hyperdos_pc_system_bios_get_model_identifier(const hyperdos_pc_system_bios* systemBios)
{
    if (systemBios == NULL || systemBios->modelIdentifier == 0u)
    {
        return HYPERDOS_PC_SYSTEM_BIOS_MODEL_IDENTIFIER_AT;
    }
    return systemBios->modelIdentifier;
}

int hyperdos_pc_system_bios_model_is_at_compatible(uint8_t modelIdentifier)
{
    return modelIdentifier == HYPERDOS_PC_SYSTEM_BIOS_MODEL_IDENTIFIER_AT;
}

void hyperdos_pc_system_bios_install_configuration_table(hyperdos_pc* pc, const hyperdos_pc_system_bios* systemBios)
{
    uint8_t tableBytes[HYPERDOS_PC_SYSTEM_BIOS_CONFIGURATION_TABLE_BYTE_COUNT] =
            {8u, 0u, HYPERDOS_PC_SYSTEM_BIOS_MODEL_IDENTIFIER_AT, 0u, 1u, 0x70u, 0x40u, 0u, 0u, 0u};
    size_t   byteIndex            = 0u;
    uint32_t tablePhysicalAddress = ((uint32_t)HYPERDOS_PC_SYSTEM_BIOS_CONFIGURATION_TABLE_SEGMENT << 4u) +
                                    HYPERDOS_PC_SYSTEM_BIOS_CONFIGURATION_TABLE_OFFSET;

    if (pc == NULL)
    {
        return;
    }
    if (!hyperdos_pc_system_bios_model_is_at_compatible(hyperdos_pc_system_bios_get_model_identifier(systemBios)))
    {
        return;
    }
    tableBytes[2] = hyperdos_pc_system_bios_get_model_identifier(systemBios);
    for (byteIndex = 0u; byteIndex < HYPERDOS_PC_SYSTEM_BIOS_CONFIGURATION_TABLE_BYTE_COUNT; ++byteIndex)
    {
        hyperdos_pc_firmware_write_byte(pc, tablePhysicalAddress + (uint32_t)byteIndex, tableBytes[byteIndex]);
    }
}

void hyperdos_pc_system_bios_install_identification(hyperdos_pc* pc, const hyperdos_pc_system_bios* systemBios)
{
    static const char dateString[]   = "01/01/92";
    size_t            characterIndex = 0u;

    if (pc == NULL)
    {
        return;
    }
    for (characterIndex = 0u; dateString[characterIndex] != '\0'; ++characterIndex)
    {
        hyperdos_pc_firmware_write_byte(pc,
                                        HYPERDOS_PC_SYSTEM_BIOS_DATE_PHYSICAL_ADDRESS + (uint32_t)characterIndex,
                                        (uint8_t)dateString[characterIndex]);
    }
    hyperdos_pc_firmware_write_byte(pc,
                                    HYPERDOS_PC_SYSTEM_BIOS_MODEL_IDENTIFIER_PHYSICAL_ADDRESS,
                                    hyperdos_pc_system_bios_get_model_identifier(systemBios));
    hyperdos_pc_firmware_write_byte(pc,
                                    HYPERDOS_PC_SYSTEM_BIOS_SIGNATURE_PHYSICAL_ADDRESS,
                                    HYPERDOS_PC_SYSTEM_BIOS_SIGNATURE);
}

void hyperdos_pc_system_bios_initialize_data_area(hyperdos_pc* pc)
{
    if (pc == NULL)
    {
        return;
    }
    hyperdos_pc_bios_data_area_write_double_word(pc, HYPERDOS_PC_BIOS_DATA_AREA_TIMER_TICK_COUNT_OFFSET, 0u);
    hyperdos_pc_bios_data_area_write_byte(pc, HYPERDOS_PC_BIOS_DATA_AREA_TIMER_MIDNIGHT_FLAG_OFFSET, 0u);
}

void hyperdos_pc_system_bios_service_wait_event(hyperdos_pc_system_bios* systemBios, hyperdos_pc* pc)
{
    if (systemBios == NULL || pc == NULL || !systemBios->waitEventActive ||
        pc->bus.clockCount < systemBios->waitEventCompletionClockCount)
    {
        return;
    }
    hyperdos_pc_system_bios_complete_wait_event(systemBios, pc);
}

void hyperdos_pc_system_bios_advance_timer_tick(hyperdos_pc* pc)
{
    uint32_t timerTickCount = 0u;

    if (pc == NULL)
    {
        return;
    }

    timerTickCount = hyperdos_pc_bios_data_area_read_double_word(pc,
                                                                 HYPERDOS_PC_BIOS_DATA_AREA_TIMER_TICK_COUNT_OFFSET);
    ++timerTickCount;
    if (timerTickCount >= HYPERDOS_PC_SYSTEM_BIOS_TIMER_TICKS_PER_DAY)
    {
        uint8_t midnightFlag =
                hyperdos_pc_bios_data_area_read_byte(pc, HYPERDOS_PC_BIOS_DATA_AREA_TIMER_MIDNIGHT_FLAG_OFFSET);

        timerTickCount = 0u;
        hyperdos_pc_bios_data_area_write_byte(pc,
                                              HYPERDOS_PC_BIOS_DATA_AREA_TIMER_MIDNIGHT_FLAG_OFFSET,
                                              (uint8_t)(midnightFlag + 1u));
    }
    hyperdos_pc_bios_data_area_write_double_word(pc,
                                                 HYPERDOS_PC_BIOS_DATA_AREA_TIMER_TICK_COUNT_OFFSET,
                                                 timerTickCount);
}

uint16_t hyperdos_pc_system_bios_get_equipment_flags(uint8_t coprocessorEnabled,
                                                     uint8_t floppyDriveCount,
                                                     uint8_t pointingDevicePresent)
{
    uint16_t equipmentFlags = 0u;

    if (floppyDriveCount > HYPERDOS_PC_SYSTEM_BIOS_EQUIPMENT_FLAGS_MAXIMUM_DISKETTE_DRIVE_COUNT)
    {
        floppyDriveCount = HYPERDOS_PC_SYSTEM_BIOS_EQUIPMENT_FLAGS_MAXIMUM_DISKETTE_DRIVE_COUNT;
    }
    if (floppyDriveCount != 0u)
    {
        equipmentFlags |= HYPERDOS_PC_SYSTEM_BIOS_EQUIPMENT_FLAGS_DISKETTE_PRESENT;
        equipmentFlags |= (uint16_t)((floppyDriveCount - 1u)
                                     << HYPERDOS_PC_SYSTEM_BIOS_EQUIPMENT_FLAGS_DISKETTE_DRIVE_COUNT_SHIFT);
    }

    if (coprocessorEnabled)
    {
        equipmentFlags |= HYPERDOS_PC_SYSTEM_BIOS_EQUIPMENT_FLAGS_COPROCESSOR_PRESENT;
    }
    if (pointingDevicePresent)
    {
        equipmentFlags |= HYPERDOS_PC_SYSTEM_BIOS_EQUIPMENT_FLAGS_POINTING_DEVICE_PRESENT;
    }
    return equipmentFlags;
}

uint16_t hyperdos_pc_system_bios_get_conventional_memory_size_kilobytes(void)
{
    return HYPERDOS_PC_SYSTEM_BIOS_CONVENTIONAL_MEMORY_KILOBYTES;
}

hyperdos_x86_16_execution_result hyperdos_pc_system_bios_handle_equipment_interrupt(
        hyperdos_x86_16_processor* processor,
        uint8_t                    coprocessorEnabled,
        uint8_t                    floppyDriveCount,
        uint8_t                    pointingDevicePresent)
{
    if (processor == NULL)
    {
        return HYPERDOS_X86_16_EXECUTION_INVALID_ARGUMENT;
    }
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] =
            hyperdos_pc_system_bios_get_equipment_flags(coprocessorEnabled, floppyDriveCount, pointingDevicePresent);
    return HYPERDOS_X86_16_EXECUTION_OK;
}

hyperdos_x86_16_execution_result hyperdos_pc_system_bios_handle_memory_size_interrupt(
        hyperdos_x86_16_processor* processor)
{
    if (processor == NULL)
    {
        return HYPERDOS_X86_16_EXECUTION_INVALID_ARGUMENT;
    }
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] =
            hyperdos_pc_system_bios_get_conventional_memory_size_kilobytes();
    return HYPERDOS_X86_16_EXECUTION_OK;
}

hyperdos_x86_16_execution_result hyperdos_pc_system_bios_handle_serial_interrupt(hyperdos_x86_16_processor* processor,
                                                                                 uint8_t serviceNumber)
{
    uint16_t accumulator = 0u;

    if (processor == NULL)
    {
        return HYPERDOS_X86_16_EXECUTION_INVALID_ARGUMENT;
    }
    accumulator = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR];
    if (serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_SERIAL_INITIALIZE_SERVICE ||
        serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_SERIAL_WRITE_SERVICE ||
        serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_SERIAL_STATUS_SERVICE)
    {
        processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] =
                (uint16_t)((HYPERDOS_PC_SYSTEM_BIOS_SERIAL_STATUS_TRANSMIT_READY
                            << HYPERDOS_PC_SYSTEM_BIOS_SERVICE_REGISTER_SHIFT) |
                           (accumulator & HYPERDOS_X86_16_LOW_BYTE_MASK));
        hyperdos_pc_system_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_16_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_SERIAL_READ_SERVICE)
    {
        processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] =
                (uint16_t)(HYPERDOS_PC_SYSTEM_BIOS_SERIAL_STATUS_TIMEOUT
                           << HYPERDOS_PC_SYSTEM_BIOS_SERVICE_REGISTER_SHIFT);
        hyperdos_pc_system_bios_set_carry_flag(processor, 1);
        return HYPERDOS_X86_16_EXECUTION_OK;
    }
    return HYPERDOS_X86_16_EXECUTION_INTERRUPT_NOT_HANDLED;
}

static void hyperdos_pc_system_bios_set_pointing_device_defaults(hyperdos_pc_system_bios* systemBios)
{
    systemBios->pointingDeviceEnabled         = 0u;
    systemBios->pointingDeviceSampleRate      = HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_DEFAULT_SAMPLE_RATE;
    systemBios->pointingDeviceResolution      = HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_DEFAULT_RESOLUTION;
    systemBios->pointingDeviceScalingTwoToOne = 0u;
    systemBios->pointingDevicePacketByteCount = 0u;
}

static uint8_t hyperdos_pc_system_bios_get_high_register_byte(const hyperdos_x86_16_processor*       processor,
                                                              hyperdos_x86_16_general_register_index registerIndex)
{
    return (uint8_t)(processor->generalRegisters[registerIndex] >> HYPERDOS_X86_16_BYTE_BIT_COUNT);
}

static void hyperdos_pc_system_bios_write_low_register_byte(hyperdos_x86_16_processor*             processor,
                                                            hyperdos_x86_16_general_register_index registerIndex,
                                                            uint8_t                                value)
{
    processor->generalRegisters[registerIndex] = (uint16_t)((processor->generalRegisters[registerIndex] &
                                                             HYPERDOS_X86_16_HIGH_BYTE_MASK) |
                                                            value);
}

static uint8_t hyperdos_pc_system_bios_get_pointing_device_status_byte(const hyperdos_pc*             pc,
                                                                       const hyperdos_pc_system_bios* systemBios)
{
    uint8_t pointingDeviceStatus = 0u;

    if ((pc->keyboardController.auxiliaryDeviceButtonMask &
         HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_PACKET_RIGHT_BUTTON) != 0u)
    {
        pointingDeviceStatus |= HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_RIGHT_BUTTON;
    }
    if ((pc->keyboardController.auxiliaryDeviceButtonMask &
         HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_PACKET_LEFT_BUTTON) != 0u)
    {
        pointingDeviceStatus |= HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_LEFT_BUTTON;
    }
    if (systemBios->pointingDeviceScalingTwoToOne != 0u)
    {
        pointingDeviceStatus |= HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_SCALING_TWO_TO_ONE;
    }
    if (systemBios->pointingDeviceEnabled != 0u)
    {
        pointingDeviceStatus |= HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_ENABLED;
    }
    return pointingDeviceStatus;
}

static hyperdos_x86_16_execution_result hyperdos_pc_system_bios_handle_pointing_device_interrupt(
        hyperdos_x86_16_processor* processor,
        hyperdos_pc*               pc,
        hyperdos_pc_system_bios*   systemBios)
{
    static const uint8_t sampleRates[] = {10u, 20u, 40u, 60u, 80u, 100u, 200u};
    uint8_t subservice   = (uint8_t)(processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] &
                                   HYPERDOS_X86_16_LOW_BYTE_MASK);
    uint8_t baseHighByte = hyperdos_pc_system_bios_get_high_register_byte(processor,
                                                                          HYPERDOS_X86_16_GENERAL_REGISTER_BASE);

    switch (subservice)
    {
    case HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_ENABLE_SUBSERVICE:
        if (baseHighByte > 1u)
        {
            hyperdos_pc_system_bios_set_system_services_status(
                    processor,
                    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_INVALID_INPUT,
                    1);
            return HYPERDOS_X86_16_EXECUTION_OK;
        }
        if (baseHighByte != 0u && systemBios->pointingDeviceHandlerOffset == 0u &&
            systemBios->pointingDeviceHandlerSegment == 0u)
        {
            hyperdos_pc_system_bios_set_system_services_status(
                    processor,
                    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_NO_HANDLER,
                    1);
            return HYPERDOS_X86_16_EXECUTION_OK;
        }
        systemBios->pointingDeviceEnabled = baseHighByte != 0u ? 1u : 0u;
        hyperdos_intel_8042_keyboard_controller_set_auxiliary_mouse_reporting_enabled(
                &pc->keyboardController,
                systemBios->pointingDeviceEnabled);
        hyperdos_pc_set_auxiliary_device_interrupt_request_enabled(pc, systemBios->pointingDeviceEnabled);
        hyperdos_pc_system_bios_set_system_services_status(processor,
                                                           HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_SUCCESS,
                                                           0);
        return HYPERDOS_X86_16_EXECUTION_OK;
    case HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_RESET_SUBSERVICE:
        hyperdos_pc_system_bios_set_pointing_device_defaults(systemBios);
        hyperdos_intel_8042_keyboard_controller_set_auxiliary_mouse_reporting_enabled(&pc->keyboardController, 0u);
        hyperdos_pc_set_auxiliary_device_interrupt_request_enabled(pc, 0u);
        processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE] =
                (uint16_t)((HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_TYPE_STANDARD_MOUSE
                            << HYPERDOS_X86_16_BYTE_BIT_COUNT) |
                           HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_RESET_PASSED);
        hyperdos_pc_system_bios_set_system_services_status(processor,
                                                           HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_SUCCESS,
                                                           0);
        return HYPERDOS_X86_16_EXECUTION_OK;
    case HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_SET_SAMPLE_RATE_SUBSERVICE:
        if (baseHighByte >= sizeof(sampleRates))
        {
            hyperdos_pc_system_bios_set_system_services_status(
                    processor,
                    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_INVALID_INPUT,
                    1);
            return HYPERDOS_X86_16_EXECUTION_OK;
        }
        systemBios->pointingDeviceSampleRate = sampleRates[baseHighByte];
        hyperdos_pc_system_bios_set_system_services_status(processor,
                                                           HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_SUCCESS,
                                                           0);
        return HYPERDOS_X86_16_EXECUTION_OK;
    case HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_SET_RESOLUTION_SUBSERVICE:
        if (baseHighByte > 3u)
        {
            hyperdos_pc_system_bios_set_system_services_status(
                    processor,
                    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_INVALID_INPUT,
                    1);
            return HYPERDOS_X86_16_EXECUTION_OK;
        }
        systemBios->pointingDeviceResolution = baseHighByte;
        hyperdos_pc_system_bios_set_system_services_status(processor,
                                                           HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_SUCCESS,
                                                           0);
        return HYPERDOS_X86_16_EXECUTION_OK;
    case HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_READ_DEVICE_TYPE_SUBSERVICE:
        processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE] =
                (uint16_t)((processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE] &
                            HYPERDOS_X86_16_LOW_BYTE_MASK) |
                           (HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_TYPE_STANDARD_MOUSE
                            << HYPERDOS_X86_16_BYTE_BIT_COUNT));
        hyperdos_pc_system_bios_set_system_services_status(processor,
                                                           HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_SUCCESS,
                                                           0);
        return HYPERDOS_X86_16_EXECUTION_OK;
    case HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_INITIALIZE_SUBSERVICE:
        if (baseHighByte == 0u || baseHighByte > 8u)
        {
            hyperdos_pc_system_bios_set_system_services_status(
                    processor,
                    HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_INVALID_INPUT,
                    1);
            return HYPERDOS_X86_16_EXECUTION_OK;
        }
        hyperdos_pc_system_bios_set_pointing_device_defaults(systemBios);
        hyperdos_intel_8042_keyboard_controller_set_auxiliary_mouse_reporting_enabled(&pc->keyboardController, 0u);
        hyperdos_pc_set_auxiliary_device_interrupt_request_enabled(pc, 0u);
        hyperdos_pc_system_bios_set_system_services_status(processor,
                                                           HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_SUCCESS,
                                                           0);
        return HYPERDOS_X86_16_EXECUTION_OK;
    case HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_EXTENDED_COMMANDS_SUBSERVICE:
        if (baseHighByte == 0u)
        {
            uint8_t pointingDeviceStatus = hyperdos_pc_system_bios_get_pointing_device_status_byte(pc, systemBios);
            hyperdos_pc_system_bios_write_low_register_byte(processor,
                                                            HYPERDOS_X86_16_GENERAL_REGISTER_BASE,
                                                            pointingDeviceStatus);
            hyperdos_pc_system_bios_write_low_register_byte(processor,
                                                            HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER,
                                                            systemBios->pointingDeviceResolution);
            hyperdos_pc_system_bios_write_low_register_byte(processor,
                                                            HYPERDOS_X86_16_GENERAL_REGISTER_DATA,
                                                            systemBios->pointingDeviceSampleRate);
            hyperdos_pc_system_bios_set_system_services_status(processor,
                                                               HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_SUCCESS,
                                                               0);
            return HYPERDOS_X86_16_EXECUTION_OK;
        }
        if (baseHighByte == 1u || baseHighByte == 2u)
        {
            systemBios->pointingDeviceScalingTwoToOne = baseHighByte == 2u ? 1u : 0u;
            hyperdos_pc_system_bios_set_system_services_status(processor,
                                                               HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_SUCCESS,
                                                               0);
            return HYPERDOS_X86_16_EXECUTION_OK;
        }
        hyperdos_pc_system_bios_set_system_services_status(
                processor,
                HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_INVALID_FUNCTION,
                1);
        return HYPERDOS_X86_16_EXECUTION_OK;
    case HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_SET_HANDLER_SUBSERVICE:
        systemBios->pointingDeviceHandlerOffset  = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE];
        systemBios->pointingDeviceHandlerSegment = processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA];
        hyperdos_pc_system_bios_set_system_services_status(processor,
                                                           HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_SUCCESS,
                                                           0);
        return HYPERDOS_X86_16_EXECUTION_OK;
    default:
        hyperdos_pc_system_bios_set_system_services_status(
                processor,
                HYPERDOS_PC_SYSTEM_BIOS_POINTING_DEVICE_STATUS_INVALID_FUNCTION,
                1);
        return HYPERDOS_X86_16_EXECUTION_OK;
    }
}

hyperdos_x86_16_execution_result hyperdos_pc_system_bios_handle_system_services_interrupt(
        hyperdos_x86_16_processor* processor,
        hyperdos_pc*               pc,
        hyperdos_pc_system_bios*   systemBios,
        uint8_t                    serviceNumber)
{
    uint8_t modelIdentifier = 0u;

    if (processor == NULL || pc == NULL || systemBios == NULL)
    {
        return HYPERDOS_X86_16_EXECUTION_INVALID_ARGUMENT;
    }
    modelIdentifier = hyperdos_pc_system_bios_get_model_identifier(systemBios);
    if (!hyperdos_pc_system_bios_model_is_at_compatible(modelIdentifier) &&
        (serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_A20_GATE_SERVICE ||
         serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_EVENT_WAIT_SERVICE ||
         serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_GET_EXTENDED_MEMORY_SIZE_SERVICE ||
         serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_WAIT_SERVICE ||
         serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_GET_CONFIGURATION_SERVICE ||
         serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_POINTING_DEVICE_SERVICE))
    {
        hyperdos_pc_system_bios_set_system_services_status(processor,
                                                           HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_UNSUPPORTED_STATUS,
                                                           1);
        return HYPERDOS_X86_16_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_KEYBOARD_INTERCEPT_SERVICE)
    {
        hyperdos_pc_system_bios_set_carry_flag(processor, 1);
        return HYPERDOS_X86_16_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_POINTING_DEVICE_SERVICE)
    {
        return hyperdos_pc_system_bios_handle_pointing_device_interrupt(processor, pc, systemBios);
    }
    if (serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_A20_GATE_SERVICE)
    {
        uint8_t subservice = (uint8_t)(processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] &
                                       HYPERDOS_X86_16_LOW_BYTE_MASK);

        if (subservice == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_A20_DISABLE_SUBSERVICE)
        {
            systemBios->a20GateEnabled = 0u;
            hyperdos_pc_system_bios_set_system_services_status(processor,
                                                               HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_SUCCESS_STATUS,
                                                               0);
            return HYPERDOS_X86_16_EXECUTION_OK;
        }
        if (subservice == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_A20_ENABLE_SUBSERVICE)
        {
            systemBios->a20GateEnabled = 1u;
            hyperdos_pc_system_bios_set_system_services_status(processor,
                                                               HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_SUCCESS_STATUS,
                                                               0);
            return HYPERDOS_X86_16_EXECUTION_OK;
        }
        if (subservice == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_A20_QUERY_SUBSERVICE)
        {
            processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] =
                    (uint16_t)((HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_SUCCESS_STATUS
                                << HYPERDOS_PC_SYSTEM_BIOS_SERVICE_REGISTER_SHIFT) |
                               (systemBios->a20GateEnabled ? 1u : 0u));
            hyperdos_pc_system_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_16_EXECUTION_OK;
        }
        if (subservice == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_A20_SUPPORT_SUBSERVICE)
        {
            processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE] =
                    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_A20_KEYBOARD_CONTROLLER_SUPPORT |
                    HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_A20_FAST_GATE_SUPPORT;
            hyperdos_pc_system_bios_set_system_services_status(processor,
                                                               HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_SUCCESS_STATUS,
                                                               0);
            return HYPERDOS_X86_16_EXECUTION_OK;
        }
    }
    if (serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_EVENT_WAIT_SERVICE)
    {
        uint8_t subservice = (uint8_t)(processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] &
                                       HYPERDOS_X86_16_LOW_BYTE_MASK);

        if (subservice == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_EVENT_WAIT_CANCEL_SUBSERVICE)
        {
            systemBios->waitEventActive = 0u;
            hyperdos_pc_system_bios_set_system_services_status(processor,
                                                               HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_SUCCESS_STATUS,
                                                               0);
            return HYPERDOS_X86_16_EXECUTION_OK;
        }
        if (subservice == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_EVENT_WAIT_SET_SUBSERVICE)
        {
            uint64_t clockCount = hyperdos_pc_system_bios_get_system_services_wait_clock_count(pc, processor);

            if (systemBios->waitEventActive)
            {
                hyperdos_pc_system_bios_set_system_services_status(
                        processor,
                        HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_WAIT_BUSY_STATUS,
                        1);
                return HYPERDOS_X86_16_EXECUTION_OK;
            }
            systemBios->waitEventFlagPhysicalAddress =
                    (processor->segmentBases[HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA] +
                     processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE]) &
                    HYPERDOS_X86_16_ADDRESS_MASK;
            systemBios->waitEventCompletionClockCount = pc->bus.clockCount + clockCount;
            systemBios->waitEventActive               = 1u;
            if (clockCount == 0u)
            {
                hyperdos_pc_system_bios_complete_wait_event(systemBios, pc);
            }
            hyperdos_pc_system_bios_set_system_services_status(processor,
                                                               HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_SUCCESS_STATUS,
                                                               0);
            return HYPERDOS_X86_16_EXECUTION_OK;
        }
    }
    if (serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_GET_EXTENDED_MEMORY_SIZE_SERVICE)
    {
        processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = 0u;
        hyperdos_pc_system_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_16_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_WAIT_SERVICE)
    {
        uint64_t clockCount = hyperdos_pc_system_bios_get_system_services_wait_clock_count(pc, processor);

        if (systemBios->waitEventActive)
        {
            hyperdos_pc_system_bios_set_system_services_status(processor,
                                                               HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_WAIT_BUSY_STATUS,
                                                               1);
            return HYPERDOS_X86_16_EXECUTION_OK;
        }
        if (clockCount != 0u)
        {
            hyperdos_intel_8284_clock_generator_step(&pc->clockGenerator, &pc->bus, clockCount);
        }
        hyperdos_pc_system_bios_set_system_services_status(processor,
                                                           HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_SUCCESS_STATUS,
                                                           0);
        return HYPERDOS_X86_16_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_GET_CONFIGURATION_SERVICE)
    {
        processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] =
                (uint16_t)(processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] &
                           HYPERDOS_X86_16_LOW_BYTE_MASK);
        processor->generalRegisters
                [HYPERDOS_X86_16_GENERAL_REGISTER_BASE] = HYPERDOS_PC_SYSTEM_BIOS_CONFIGURATION_TABLE_OFFSET;
        hyperdos_x86_16_set_segment_register(processor,
                                             HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA,
                                             HYPERDOS_PC_SYSTEM_BIOS_CONFIGURATION_TABLE_SEGMENT);
        hyperdos_pc_system_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_16_EXECUTION_OK;
    }

    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] =
            (uint16_t)((HYPERDOS_PC_SYSTEM_BIOS_SYSTEM_SERVICES_UNSUPPORTED_STATUS
                        << HYPERDOS_PC_SYSTEM_BIOS_SERVICE_REGISTER_SHIFT) |
                       (processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] &
                        HYPERDOS_X86_16_LOW_BYTE_MASK));
    hyperdos_pc_system_bios_set_carry_flag(processor, 1);
    return HYPERDOS_X86_16_EXECUTION_OK;
}

hyperdos_x86_16_execution_result hyperdos_pc_system_bios_handle_printer_interrupt(hyperdos_x86_16_processor* processor,
                                                                                  uint8_t serviceNumber)
{
    if (processor == NULL)
    {
        return HYPERDOS_X86_16_EXECUTION_INVALID_ARGUMENT;
    }
    if (serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_PRINTER_WRITE_SERVICE ||
        serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_PRINTER_INITIALIZE_SERVICE ||
        serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_PRINTER_STATUS_SERVICE)
    {
        processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] =
                (uint16_t)((HYPERDOS_PC_SYSTEM_BIOS_PRINTER_STATUS_SELECTED_READY
                            << HYPERDOS_PC_SYSTEM_BIOS_SERVICE_REGISTER_SHIFT) |
                           (processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] &
                            HYPERDOS_X86_16_LOW_BYTE_MASK));
        hyperdos_pc_system_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_16_EXECUTION_OK;
    }
    return HYPERDOS_X86_16_EXECUTION_INTERRUPT_NOT_HANDLED;
}

hyperdos_x86_16_execution_result hyperdos_pc_system_bios_handle_time_interrupt(hyperdos_x86_16_processor* processor,
                                                                               hyperdos_pc*               pc,
                                                                               uint8_t                    serviceNumber)
{
    if (processor == NULL || pc == NULL)
    {
        return HYPERDOS_X86_16_EXECUTION_INVALID_ARGUMENT;
    }
    if (serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_TIME_TICKS_SERVICE)
    {
        uint32_t timerTickCount =
                hyperdos_pc_bios_data_area_read_double_word(pc, HYPERDOS_PC_BIOS_DATA_AREA_TIMER_TICK_COUNT_OFFSET);
        processor->generalRegisters
                [HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER]                 = (uint16_t)(timerTickCount >>
                                                                        HYPERDOS_X86_16_WORD_BIT_COUNT);
        processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA] = (uint16_t)timerTickCount;
        processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] =
                hyperdos_pc_bios_data_area_read_byte(pc, HYPERDOS_PC_BIOS_DATA_AREA_TIMER_MIDNIGHT_FLAG_OFFSET);
        hyperdos_pc_system_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_16_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_TIME_SET_TICKS_SERVICE)
    {
        uint32_t timerTickCount = ((uint32_t)processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER]
                                   << HYPERDOS_X86_16_WORD_BIT_COUNT) |
                                  processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA];

        hyperdos_pc_bios_data_area_write_double_word(pc,
                                                     HYPERDOS_PC_BIOS_DATA_AREA_TIMER_TICK_COUNT_OFFSET,
                                                     timerTickCount);
        hyperdos_pc_bios_data_area_write_byte(pc, HYPERDOS_PC_BIOS_DATA_AREA_TIMER_MIDNIGHT_FLAG_OFFSET, 0u);
        processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = 0u;
        hyperdos_pc_system_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_16_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_TIME_SERVICE)
    {
        processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER] =
                (uint16_t)(((uint16_t)hyperdos_pc_system_bios_make_binary_coded_decimal(
                                    HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_HOUR)
                            << HYPERDOS_X86_16_BYTE_BIT_COUNT) |
                           hyperdos_pc_system_bios_make_binary_coded_decimal(
                                   HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_MINUTE));
        processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA]         = (uint16_t)((
                (uint16_t)hyperdos_pc_system_bios_make_binary_coded_decimal(
                        HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_SECOND)
                << HYPERDOS_X86_16_BYTE_BIT_COUNT));
        processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] &= HYPERDOS_X86_16_LOW_BYTE_MASK;
        hyperdos_pc_system_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_16_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_DATE_SERVICE)
    {
        processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER] =
                (uint16_t)(((uint16_t)hyperdos_pc_system_bios_make_binary_coded_decimal(
                                    HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_CENTURY)
                            << HYPERDOS_X86_16_BYTE_BIT_COUNT) |
                           hyperdos_pc_system_bios_make_binary_coded_decimal(
                                   HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_YEAR));
        processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA] =
                (uint16_t)(((uint16_t)hyperdos_pc_system_bios_make_binary_coded_decimal(
                                    HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_MONTH)
                            << HYPERDOS_X86_16_BYTE_BIT_COUNT) |
                           hyperdos_pc_system_bios_make_binary_coded_decimal(
                                   HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_DAY));
        processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] &= HYPERDOS_X86_16_LOW_BYTE_MASK;
        hyperdos_pc_system_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_16_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_SET_TIME_SERVICE ||
        serviceNumber == HYPERDOS_PC_SYSTEM_BIOS_REAL_TIME_CLOCK_SET_DATE_SERVICE)
    {
        processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] &= HYPERDOS_X86_16_LOW_BYTE_MASK;
        hyperdos_pc_system_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_16_EXECUTION_OK;
    }
    return HYPERDOS_X86_16_EXECUTION_INTERRUPT_NOT_HANDLED;
}

hyperdos_x86_16_execution_result hyperdos_pc_system_bios_handle_expanded_memory_manager_interrupt(
        hyperdos_x86_16_processor* processor,
        uint8_t                    serviceNumber)
{
    uint8_t status = HYPERDOS_PC_SYSTEM_BIOS_EXPANDED_MEMORY_MANAGER_STATUS_NOT_AVAILABLE;

    if (processor == NULL)
    {
        return HYPERDOS_X86_16_EXECUTION_INVALID_ARGUMENT;
    }
    if (serviceNumber != HYPERDOS_PC_SYSTEM_BIOS_EXPANDED_MEMORY_MANAGER_STATUS_SERVICE &&
        serviceNumber != HYPERDOS_PC_SYSTEM_BIOS_EXPANDED_MEMORY_MANAGER_PAGE_FRAME_SERVICE &&
        serviceNumber != HYPERDOS_PC_SYSTEM_BIOS_EXPANDED_MEMORY_MANAGER_PAGE_COUNTS_SERVICE &&
        serviceNumber != HYPERDOS_PC_SYSTEM_BIOS_EXPANDED_MEMORY_MANAGER_VERSION_SERVICE)
    {
        status = HYPERDOS_PC_SYSTEM_BIOS_EXPANDED_MEMORY_MANAGER_STATUS_UNSUPPORTED_FUNCTION;
    }

    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] =
            (uint16_t)((processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] &
                        HYPERDOS_X86_16_LOW_BYTE_MASK) |
                       ((uint16_t)status << HYPERDOS_PC_SYSTEM_BIOS_SERVICE_REGISTER_SHIFT));
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE]    = 0u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER] = 0u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA]    = 0u;
    hyperdos_pc_system_bios_set_carry_flag(processor, 1);
    return HYPERDOS_X86_16_EXECUTION_OK;
}
