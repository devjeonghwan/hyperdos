#include "hyperdos/pc_board.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum
{
    HYPERDOS_PC_TEXT_ATTRIBUTE                              = 0x0Fu,
    HYPERDOS_PC_OPEN_BUS_BYTE                               = 0xFFu,
    HYPERDOS_PC_MEMORY_BASE                                 = 0x00000u,
    HYPERDOS_PC_TIMER_INTERRUPT_REQUEST_LINE                = 0u,
    HYPERDOS_PC_KEYBOARD_INTERRUPT_REQUEST_LINE             = 1u,
    HYPERDOS_PC_SLAVE_INTERRUPT_CASCADE_REQUEST_LINE        = 2u,
    HYPERDOS_PC_AUXILIARY_DEVICE_INTERRUPT_REQUEST_LINE     = 4u,
    HYPERDOS_PC_SLAVE_INTERRUPT_VECTOR_BASE                 = 0x70u,
    HYPERDOS_PC_SPEAKER_TIMER_CHANNEL                       = 2u,
    HYPERDOS_PC_INTERVAL_TIMER_CHANNEL_ZERO                 = HYPERDOS_PC_PROGRAMMABLE_INTERVAL_TIMER_PORT,
    HYPERDOS_PC_INTERVAL_TIMER_CHANNEL_TWO                  = HYPERDOS_PC_PROGRAMMABLE_INTERVAL_TIMER_PORT + 2u,
    HYPERDOS_PC_INTERVAL_TIMER_CONTROL_PORT                 = HYPERDOS_PC_PROGRAMMABLE_INTERVAL_TIMER_PORT + 3u,
    HYPERDOS_PC_INTERVAL_TIMER_MAXIMUM_COUNT                = 0x10000u,
    HYPERDOS_PC_INTERVAL_TIMER_CHANNEL_ZERO_DEFAULT_CONTROL = 0x36u,
    HYPERDOS_PC_INTERVAL_TIMER_DEFAULT_COUNT_LOW_BYTE       = 0x00u,
    HYPERDOS_PC_INTERVAL_TIMER_DEFAULT_COUNT_HIGH_BYTE      = 0x00u,
    HYPERDOS_PC_SPEAKER_TIMER_GATE_BIT                      = 0x01u,
    HYPERDOS_PC_SPEAKER_DATA_ENABLE_BIT                     = 0x02u,
    HYPERDOS_PC_BOARD_TRACE_TEXT_CAPACITY                   = 512u
};

static void hyperdos_pc_trace(hyperdos_pc_board_trace_function traceFunction,
                              void*                            traceUserContext,
                              const char*                      format,
                              ...)
{
    char    message[HYPERDOS_PC_BOARD_TRACE_TEXT_CAPACITY];
    va_list arguments;

    if (traceFunction == NULL)
    {
        return;
    }

    va_start(arguments, format);
    (void)vsnprintf(message, sizeof(message), format, arguments);
    va_end(arguments);
    traceFunction(traceUserContext, message);
}

uint32_t hyperdos_pc_get_processor_frequency_hertz(const hyperdos_pc* pc)
{
    if (pc == NULL)
    {
        return 0u;
    }
    return pc->clockGenerator.processorFrequencyHertz;
}

uint32_t hyperdos_pc_get_interval_timer_input_frequency_hertz(const hyperdos_pc* pc)
{
    (void)pc;
    return HYPERDOS_PC_INTERVAL_TIMER_INPUT_FREQUENCY_HERTZ;
}

static uint32_t hyperdos_pc_get_speaker_frequency_hertz(const hyperdos_pc* pc)
{
    uint32_t reloadCount = pc->programmableIntervalTimer.channels[HYPERDOS_PC_SPEAKER_TIMER_CHANNEL].reloadValue;
    uint32_t inputFrequencyHertz = hyperdos_pc_get_interval_timer_input_frequency_hertz(pc);
    uint32_t frequencyHertz      = 0u;

    if (reloadCount == 0u)
    {
        reloadCount = HYPERDOS_PC_INTERVAL_TIMER_MAXIMUM_COUNT;
    }
    frequencyHertz = inputFrequencyHertz / reloadCount;
    return frequencyHertz == 0u ? 1u : frequencyHertz;
}

static void hyperdos_pc_update_speaker_state(hyperdos_pc* pc)
{
    uint8_t  speakerControl = 0u;
    uint8_t  speakerEnabled = 0u;
    uint32_t frequencyHertz = 0u;

    if (pc == NULL)
    {
        return;
    }

    speakerControl = pc->programmablePeripheralInterface.portB;
    speakerEnabled = (speakerControl & (HYPERDOS_PC_SPEAKER_TIMER_GATE_BIT | HYPERDOS_PC_SPEAKER_DATA_ENABLE_BIT)) ==
                     (HYPERDOS_PC_SPEAKER_TIMER_GATE_BIT | HYPERDOS_PC_SPEAKER_DATA_ENABLE_BIT);
    if (speakerEnabled)
    {
        frequencyHertz = hyperdos_pc_get_speaker_frequency_hertz(pc);
    }
    if (speakerEnabled == pc->speakerEnabled && frequencyHertz == pc->speakerFrequencyHertz)
    {
        return;
    }

    pc->speakerEnabled        = speakerEnabled;
    pc->speakerFrequencyHertz = frequencyHertz;
    if (pc->speakerStateChange != NULL)
    {
        pc->speakerStateChange(pc->speakerStateUserContext, frequencyHertz, speakerEnabled);
    }
}

static uint8_t hyperdos_pc_read_programmable_interval_timer_byte(void* device, uint16_t port)
{
    hyperdos_pc* pc = (hyperdos_pc*)device;

    if (pc == NULL)
    {
        return HYPERDOS_PC_OPEN_BUS_BYTE;
    }
    return hyperdos_programmable_interval_timer_read_byte(&pc->programmableIntervalTimer, port);
}

static void hyperdos_pc_write_programmable_interval_timer_byte(void* device, uint16_t port, uint8_t value)
{
    hyperdos_pc* pc = (hyperdos_pc*)device;

    if (pc == NULL)
    {
        return;
    }
    hyperdos_programmable_interval_timer_write_byte(&pc->programmableIntervalTimer, port, value);
    if ((port & 0x03u) == (HYPERDOS_PC_INTERVAL_TIMER_CHANNEL_TWO & 0x03u) ||
        (port & 0x03u) == (HYPERDOS_PC_INTERVAL_TIMER_CONTROL_PORT & 0x03u))
    {
        hyperdos_pc_update_speaker_state(pc);
    }
}

static void hyperdos_pc_tick_programmable_interval_timer(void* device, uint64_t elapsedProcessorClockCount)
{
    hyperdos_pc* pc                           = (hyperdos_pc*)device;
    uint64_t     inputClockCountWithRemainder = 0u;
    uint64_t     inputClockCount              = 0u;

    if (pc == NULL)
    {
        return;
    }

    if (pc->programmableIntervalTimerInputClockNumerator == 0u ||
        pc->programmableIntervalTimerInputClockDenominator == 0u)
    {
        pc->programmableIntervalTimerInputClockNumerator   = 1u;
        pc->programmableIntervalTimerInputClockDenominator = 4u;
    }
    inputClockCountWithRemainder = elapsedProcessorClockCount * pc->programmableIntervalTimerInputClockNumerator +
                                   pc->programmableIntervalTimerInputClockRemainder;
    inputClockCount = inputClockCountWithRemainder / pc->programmableIntervalTimerInputClockDenominator;
    pc->programmableIntervalTimerInputClockRemainder = inputClockCountWithRemainder %
                                                       pc->programmableIntervalTimerInputClockDenominator;
    if (inputClockCount != 0u)
    {
        hyperdos_programmable_interval_timer_tick(&pc->programmableIntervalTimer, inputClockCount);
    }
}

static void hyperdos_pc_configure_interval_timer_input_clock_ratio(hyperdos_pc* pc)
{
    uint32_t processorFrequencyHertz = 0u;

    if (pc == NULL)
    {
        return;
    }
    processorFrequencyHertz = pc->clockGenerator.processorFrequencyHertz;
    if (processorFrequencyHertz == HYPERDOS_PC_DEFAULT_PROCESSOR_FREQUENCY_HERTZ)
    {
        pc->programmableIntervalTimerInputClockNumerator   = 1u;
        pc->programmableIntervalTimerInputClockDenominator = 4u;
    }
    else
    {
        pc->programmableIntervalTimerInputClockNumerator   = HYPERDOS_PC_8284_CRYSTAL_FREQUENCY_HERTZ;
        pc->programmableIntervalTimerInputClockDenominator = (uint64_t)HYPERDOS_PC_INTERVAL_TIMER_INPUT_CLOCK_DIVISOR *
                                                             processorFrequencyHertz;
    }
    pc->programmableIntervalTimerInputClockRemainder = 0u;
}

static uint8_t hyperdos_pc_read_peripheral_interface_byte(void* device, uint16_t port)
{
    hyperdos_pc* pc = (hyperdos_pc*)device;

    if (pc == NULL)
    {
        return HYPERDOS_PC_OPEN_BUS_BYTE;
    }
    return hyperdos_programmable_peripheral_interface_read_byte(&pc->programmablePeripheralInterface, port);
}

static void hyperdos_pc_write_peripheral_interface_byte(void* device, uint16_t port, uint8_t value)
{
    hyperdos_pc* pc = (hyperdos_pc*)device;

    if (pc == NULL)
    {
        return;
    }
    hyperdos_programmable_peripheral_interface_write_byte(&pc->programmablePeripheralInterface, port, value);
    if ((port & 0x03u) == (HYPERDOS_PC_PROGRAMMABLE_PERIPHERAL_INTERFACE_PORT & 0x03u))
    {
        hyperdos_pc_update_speaker_state(pc);
    }
}

static void hyperdos_pc_initialize_programmable_interval_timer(hyperdos_pc* pc)
{
    hyperdos_programmable_interval_timer_initialize(&pc->programmableIntervalTimer);
    hyperdos_programmable_interval_timer_write_byte(&pc->programmableIntervalTimer,
                                                    HYPERDOS_PC_INTERVAL_TIMER_CONTROL_PORT,
                                                    HYPERDOS_PC_INTERVAL_TIMER_CHANNEL_ZERO_DEFAULT_CONTROL);
    hyperdos_programmable_interval_timer_write_byte(&pc->programmableIntervalTimer,
                                                    HYPERDOS_PC_INTERVAL_TIMER_CHANNEL_ZERO,
                                                    HYPERDOS_PC_INTERVAL_TIMER_DEFAULT_COUNT_LOW_BYTE);
    hyperdos_programmable_interval_timer_write_byte(&pc->programmableIntervalTimer,
                                                    HYPERDOS_PC_INTERVAL_TIMER_CHANNEL_ZERO,
                                                    HYPERDOS_PC_INTERVAL_TIMER_DEFAULT_COUNT_HIGH_BYTE);
}

int hyperdos_pc_initialize(hyperdos_pc* pc)
{
    if (pc == NULL)
    {
        return 0;
    }

    memset(pc, 0, sizeof(*pc));
    hyperdos_bus_initialize(&pc->bus);
    hyperdos_random_access_memory_initialize(&pc->randomAccessMemory,
                                             pc->processorMemory,
                                             HYPERDOS_CONVENTIONAL_MEMORY_SIZE,
                                             HYPERDOS_PC_MEMORY_BASE);
    hyperdos_read_only_memory_initialize(&pc->basicInputOutputSystemReadOnlyMemory,
                                         pc->processorMemory + HYPERDOS_PC_BIOS_READ_ONLY_MEMORY_BASE,
                                         HYPERDOS_PC_BIOS_READ_ONLY_MEMORY_SIZE,
                                         HYPERDOS_PC_BIOS_READ_ONLY_MEMORY_BASE);
    hyperdos_color_graphics_adapter_initialize(&pc->colorGraphicsAdapter);
    hyperdos_programmable_interrupt_controller_initialize(&pc->programmableInterruptController,
                                                          HYPERDOS_PC_DEFAULT_INTERRUPT_VECTOR_BASE);
    pc->programmableInterruptController.interruptMaskRegister = HYPERDOS_PC_DEFAULT_INTERRUPT_MASK;
    hyperdos_programmable_interrupt_controller_initialize(&pc->slaveProgrammableInterruptController,
                                                          HYPERDOS_PC_SLAVE_INTERRUPT_VECTOR_BASE);
    hyperdos_direct_memory_access_controller_initialize(&pc->directMemoryAccessController);
    hyperdos_pc_initialize_programmable_interval_timer(pc);
    hyperdos_programmable_peripheral_interface_initialize(&pc->programmablePeripheralInterface);
    hyperdos_pc_cmos_initialize(&pc->realTimeClock);
    hyperdos_intel_8042_keyboard_controller_initialize(&pc->keyboardController);
    hyperdos_universal_asynchronous_receiver_transmitter_initialize(&pc->firstSerialPort);
    hyperdos_8087_initialize(&pc->floatingPointUnit);
    hyperdos_intel_8284_clock_generator_initialize(&pc->clockGenerator, HYPERDOS_PC_8284_CRYSTAL_FREQUENCY_HERTZ);
    hyperdos_pc_configure_interval_timer_input_clock_ratio(pc);
    hyperdos_intel_8288_bus_controller_initialize(&pc->busController);
    hyperdos_intel_8282_address_latch_initialize(&pc->addressLatch);
    hyperdos_intel_8286_bus_transceiver_initialize(&pc->dataBusTransceiver);

    if (hyperdos_bus_map_memory(&pc->bus,
                                HYPERDOS_PC_MEMORY_BASE,
                                HYPERDOS_CONVENTIONAL_MEMORY_SIZE,
                                &pc->randomAccessMemory,
                                hyperdos_random_access_memory_read_byte,
                                hyperdos_random_access_memory_write_byte) != HYPERDOS_BUS_ACCESS_OK)
    {
        return 0;
    }
    if (hyperdos_bus_map_memory(&pc->bus,
                                HYPERDOS_PC_BIOS_READ_ONLY_MEMORY_BASE,
                                HYPERDOS_PC_BIOS_READ_ONLY_MEMORY_SIZE,
                                &pc->basicInputOutputSystemReadOnlyMemory,
                                hyperdos_read_only_memory_read_byte,
                                hyperdos_read_only_memory_write_byte) != HYPERDOS_BUS_ACCESS_OK)
    {
        return 0;
    }
    if (hyperdos_bus_map_memory(&pc->bus,
                                HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_APERTURE_ADDRESS,
                                HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_APERTURE_SIZE,
                                &pc->colorGraphicsAdapter,
                                hyperdos_color_graphics_adapter_read_memory_byte,
                                hyperdos_color_graphics_adapter_write_memory_byte) != HYPERDOS_BUS_ACCESS_OK)
    {
        return 0;
    }
    hyperdos_bus_set_memory_mapping_observer_old_value_read_enabled(
            &pc->bus,
            HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_APERTURE_ADDRESS,
            HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_APERTURE_SIZE,
            0);
    if (hyperdos_bus_map_input_output(&pc->bus,
                                      HYPERDOS_PC_DIRECT_MEMORY_ACCESS_CONTROLLER_PORT,
                                      HYPERDOS_PC_DIRECT_MEMORY_ACCESS_CONTROLLER_PORT_COUNT,
                                      &pc->directMemoryAccessController,
                                      hyperdos_direct_memory_access_controller_read_byte,
                                      hyperdos_direct_memory_access_controller_write_byte) != HYPERDOS_BUS_ACCESS_OK)
    {
        return 0;
    }
    if (hyperdos_bus_map_input_output(&pc->bus,
                                      HYPERDOS_PC_PROGRAMMABLE_INTERRUPT_CONTROLLER_PORT,
                                      HYPERDOS_PC_INTERRUPT_CONTROLLER_PORT_COUNT,
                                      &pc->programmableInterruptController,
                                      hyperdos_programmable_interrupt_controller_read_byte,
                                      hyperdos_programmable_interrupt_controller_write_byte) != HYPERDOS_BUS_ACCESS_OK)
    {
        return 0;
    }
    if (hyperdos_bus_map_input_output(&pc->bus,
                                      HYPERDOS_PC_PROGRAMMABLE_INTERVAL_TIMER_PORT,
                                      HYPERDOS_PC_INTERVAL_TIMER_PORT_COUNT,
                                      pc,
                                      hyperdos_pc_read_programmable_interval_timer_byte,
                                      hyperdos_pc_write_programmable_interval_timer_byte) != HYPERDOS_BUS_ACCESS_OK)
    {
        return 0;
    }
    if (hyperdos_bus_attach_clock_device(&pc->bus, pc, hyperdos_pc_tick_programmable_interval_timer) !=
        HYPERDOS_BUS_ACCESS_OK)
    {
        return 0;
    }
    if (hyperdos_bus_map_input_output(&pc->bus,
                                      HYPERDOS_PC_KEYBOARD_CONTROLLER_DATA_PORT,
                                      HYPERDOS_PC_KEYBOARD_CONTROLLER_PORT_COUNT,
                                      &pc->keyboardController,
                                      hyperdos_intel_8042_keyboard_controller_read_byte,
                                      hyperdos_intel_8042_keyboard_controller_write_byte) != HYPERDOS_BUS_ACCESS_OK)
    {
        return 0;
    }
    if (hyperdos_bus_map_input_output(&pc->bus,
                                      HYPERDOS_PC_KEYBOARD_CONTROLLER_COMMAND_PORT,
                                      HYPERDOS_PC_KEYBOARD_CONTROLLER_COMMAND_PORT_COUNT,
                                      &pc->keyboardController,
                                      hyperdos_intel_8042_keyboard_controller_read_byte,
                                      hyperdos_intel_8042_keyboard_controller_write_byte) != HYPERDOS_BUS_ACCESS_OK)
    {
        return 0;
    }
    if (hyperdos_bus_map_input_output(&pc->bus,
                                      HYPERDOS_PC_PROGRAMMABLE_PERIPHERAL_INTERFACE_PORT,
                                      HYPERDOS_PC_PERIPHERAL_INTERFACE_PORT_COUNT,
                                      pc,
                                      hyperdos_pc_read_peripheral_interface_byte,
                                      hyperdos_pc_write_peripheral_interface_byte) != HYPERDOS_BUS_ACCESS_OK)
    {
        return 0;
    }
    if (hyperdos_bus_map_input_output(&pc->bus,
                                      HYPERDOS_PC_FIRST_SERIAL_PORT,
                                      HYPERDOS_PC_SERIAL_PORT_COUNT,
                                      &pc->firstSerialPort,
                                      hyperdos_universal_asynchronous_receiver_transmitter_read_byte,
                                      hyperdos_universal_asynchronous_receiver_transmitter_write_byte) !=
        HYPERDOS_BUS_ACCESS_OK)
    {
        return 0;
    }
    if (hyperdos_bus_map_input_output(&pc->bus,
                                      HYPERDOS_PC_COLOR_GRAPHICS_ADAPTER_PORT,
                                      HYPERDOS_PC_COLOR_GRAPHICS_ADAPTER_PORT_COUNT,
                                      &pc->colorGraphicsAdapter,
                                      hyperdos_color_graphics_adapter_read_input_output_byte,
                                      hyperdos_color_graphics_adapter_write_input_output_byte) !=
        HYPERDOS_BUS_ACCESS_OK)
    {
        return 0;
    }
    if (hyperdos_bus_map_input_output(&pc->bus,
                                      HYPERDOS_PC_VIDEO_MONOCHROME_COMPATIBILITY_PORT,
                                      HYPERDOS_PC_VIDEO_MONOCHROME_COMPATIBILITY_PORT_COUNT,
                                      &pc->colorGraphicsAdapter,
                                      hyperdos_color_graphics_adapter_read_input_output_byte,
                                      hyperdos_color_graphics_adapter_write_input_output_byte) !=
        HYPERDOS_BUS_ACCESS_OK)
    {
        return 0;
    }

    if (hyperdos_x86_initialize_processor(&pc->processor, pc->processorMemory, sizeof(pc->processorMemory)) !=
        HYPERDOS_X86_EXECUTION_OK)
    {
        return 0;
    }
    hyperdos_x86_attach_bus(&pc->processor, &pc->bus);
    hyperdos_x86_attach_coprocessor(&pc->processor, hyperdos_x87_wait, hyperdos_x87_escape, &pc->floatingPointUnit);
    return 1;
}

void hyperdos_pc_set_processor_frequency_hertz(hyperdos_pc* pc, uint32_t processorFrequencyHertz)
{
    if (pc == NULL || processorFrequencyHertz == 0u)
    {
        return;
    }

    hyperdos_intel_8284_clock_generator_set_processor_frequency_hertz(&pc->clockGenerator, processorFrequencyHertz);
    hyperdos_pc_configure_interval_timer_input_clock_ratio(pc);
    hyperdos_pc_update_speaker_state(pc);
}

void hyperdos_pc_set_speaker_state_change_function(hyperdos_pc*                              pc,
                                                   hyperdos_pc_speaker_state_change_function speakerStateChange,
                                                   void*                                     userContext)
{
    if (pc == NULL)
    {
        return;
    }
    pc->speakerStateChange      = speakerStateChange;
    pc->speakerStateUserContext = userContext;
}

void hyperdos_pc_render_text_message(hyperdos_pc* pc, const char* message)
{
    size_t characterIndex = 0;

    if (pc == NULL || message == NULL)
    {
        return;
    }

    while (message[characterIndex] != '\0' &&
           characterIndex < HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT * HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT)
    {
        size_t memoryIndex                           = characterIndex * HYPERDOS_COLOR_GRAPHICS_ADAPTER_CELL_BYTE_COUNT;
        pc->colorGraphicsAdapter.memory[memoryIndex] = (uint8_t)message[characterIndex];
        pc->colorGraphicsAdapter.memory[memoryIndex + 1u] = HYPERDOS_PC_TEXT_ATTRIBUTE;
        ++characterIndex;
    }
}

int hyperdos_pc_load_boot_sector(hyperdos_pc* pc, const uint8_t* bootSectorBytes, size_t bootSectorByteCount)
{
    size_t byteIndex = 0u;

    if (pc == NULL || bootSectorBytes == NULL || bootSectorByteCount < HYPERDOS_PC_BOOT_SECTOR_BYTE_COUNT)
    {
        return 0;
    }

    for (byteIndex = 0u; byteIndex < HYPERDOS_PC_BOOT_SECTOR_BYTE_COUNT; ++byteIndex)
    {
        hyperdos_bus_write_memory_byte_if_mapped(&pc->bus,
                                                 HYPERDOS_PC_BOOT_SECTOR_ADDRESS + (uint32_t)byteIndex,
                                                 bootSectorBytes[byteIndex]);
    }
    return 1;
}

void hyperdos_pc_prepare_boot_sector_execution(hyperdos_pc* pc, uint8_t bootDriveNumber)
{
    if (pc == NULL)
    {
        return;
    }

    hyperdos_x86_set_segment_register(&pc->processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE, 0u);
    hyperdos_x86_set_segment_register(&pc->processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, 0u);
    hyperdos_x86_set_segment_register(&pc->processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0u);
    hyperdos_x86_set_segment_register(&pc->processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0u);
    hyperdos_x86_set_general_register(&pc->processor,
                                      HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                      HYPERDOS_PC_BOOT_STACK_POINTER);
    hyperdos_x86_set_general_register(&pc->processor, HYPERDOS_X86_GENERAL_REGISTER_DATA, bootDriveNumber);
    hyperdos_x86_set_instruction_pointer_word(&pc->processor, HYPERDOS_PC_BOOT_SECTOR_ADDRESS);
}

int hyperdos_x86_processor_is_at_bios_reset_vector(const hyperdos_x86_processor* processor)
{
    return processor != NULL && processor->lastInstructionSegment == HYPERDOS_PC_BIOS_RESET_VECTOR_SEGMENT &&
           processor->lastInstructionOffset == HYPERDOS_PC_BIOS_RESET_VECTOR_OFFSET;
}

void hyperdos_pc_raise_keyboard_controller_interrupt_request(hyperdos_pc*                     pc,
                                                             hyperdos_pc_board_trace_function traceFunction,
                                                             void*                            traceUserContext)
{
    if (pc == NULL || !hyperdos_intel_8042_keyboard_controller_has_interrupt_request(&pc->keyboardController))
    {
        return;
    }

    hyperdos_pc_trace(traceFunction,
                      traceUserContext,
                      "keyboard raise irq1 command-byte=%02X pic-mask=%02X pic-request=%02X pic-service=%02X "
                      "vector-base=%02X output-queue=%zu",
                      pc->keyboardController.commandByte,
                      pc->programmableInterruptController.interruptMaskRegister,
                      pc->programmableInterruptController.interruptRequestRegister,
                      pc->programmableInterruptController.inServiceRegister,
                      pc->programmableInterruptController.vectorBase,
                      pc->keyboardController.outputQueueCount);
    hyperdos_programmable_interrupt_controller_raise_request(&pc->programmableInterruptController,
                                                             HYPERDOS_PC_KEYBOARD_INTERRUPT_REQUEST_LINE);
    hyperdos_intel_8042_keyboard_controller_clear_interrupt_request(&pc->keyboardController);
}

void hyperdos_pc_raise_auxiliary_device_interrupt_request(hyperdos_pc*                     pc,
                                                          hyperdos_pc_board_trace_function traceFunction,
                                                          void*                            traceUserContext)
{
    (void)traceFunction;
    (void)traceUserContext;

    if (pc == NULL || pc->slaveProgrammableInterruptControllerEnabled == 0u ||
        !hyperdos_intel_8042_keyboard_controller_has_auxiliary_device_interrupt_request(&pc->keyboardController))
    {
        return;
    }

    hyperdos_pc_raise_auxiliary_device_interrupt_request_line(pc);
    hyperdos_intel_8042_keyboard_controller_clear_auxiliary_device_interrupt_request(&pc->keyboardController);
}

void hyperdos_pc_raise_auxiliary_device_interrupt_request_line(hyperdos_pc* pc)
{
    if (pc == NULL || pc->slaveProgrammableInterruptControllerEnabled == 0u)
    {
        return;
    }

    hyperdos_programmable_interrupt_controller_raise_request(&pc->slaveProgrammableInterruptController,
                                                             HYPERDOS_PC_AUXILIARY_DEVICE_INTERRUPT_REQUEST_LINE);
}

void hyperdos_pc_set_auxiliary_device_interrupt_request_enabled(hyperdos_pc* pc, uint8_t enabled)
{
    uint8_t cascadeInterruptMask         = (uint8_t)(1u << HYPERDOS_PC_SLAVE_INTERRUPT_CASCADE_REQUEST_LINE);
    uint8_t auxiliaryDeviceInterruptMask = (uint8_t)(1u << HYPERDOS_PC_AUXILIARY_DEVICE_INTERRUPT_REQUEST_LINE);

    if (pc == NULL || pc->slaveProgrammableInterruptControllerEnabled == 0u)
    {
        return;
    }

    if (enabled != 0u)
    {
        pc->programmableInterruptController.interruptMaskRegister =
                (uint8_t)(pc->programmableInterruptController.interruptMaskRegister & ~cascadeInterruptMask);
        pc->slaveProgrammableInterruptController.interruptMaskRegister =
                (uint8_t)(pc->slaveProgrammableInterruptController.interruptMaskRegister &
                          ~auxiliaryDeviceInterruptMask);
    }
    else
    {
        pc->slaveProgrammableInterruptController.interruptMaskRegister =
                (uint8_t)(pc->slaveProgrammableInterruptController.interruptMaskRegister |
                          auxiliaryDeviceInterruptMask);
    }
}

void hyperdos_pc_raise_interval_timer_interrupt_request(hyperdos_pc*                     pc,
                                                        hyperdos_pc_board_trace_function traceFunction,
                                                        void*                            traceUserContext)
{
    if (pc == NULL ||
        !hyperdos_programmable_interval_timer_get_and_clear_output_transition(&pc->programmableIntervalTimer,
                                                                              HYPERDOS_PC_TIMER_INTERRUPT_REQUEST_LINE))
    {
        return;
    }

    hyperdos_pc_trace(traceFunction,
                      traceUserContext,
                      "timer raise irq0 pic-mask=%02X pic-request=%02X pic-service=%02X vector-base=%02X counter=%04X "
                      "reload=%04X",
                      pc->programmableInterruptController.interruptMaskRegister,
                      pc->programmableInterruptController.interruptRequestRegister,
                      pc->programmableInterruptController.inServiceRegister,
                      pc->programmableInterruptController.vectorBase,
                      pc->programmableIntervalTimer.channels[HYPERDOS_PC_TIMER_INTERRUPT_REQUEST_LINE].currentValue,
                      pc->programmableIntervalTimer.channels[HYPERDOS_PC_TIMER_INTERRUPT_REQUEST_LINE].reloadValue);
    hyperdos_programmable_interrupt_controller_raise_request(&pc->programmableInterruptController,
                                                             HYPERDOS_PC_TIMER_INTERRUPT_REQUEST_LINE);
}

static void hyperdos_pc_raise_slave_interrupt_cascade_request_if_needed(hyperdos_pc* pc)
{
    if (pc == NULL || pc->slaveProgrammableInterruptControllerEnabled == 0u ||
        !hyperdos_programmable_interrupt_controller_has_pending_unmasked_request(
                &pc->slaveProgrammableInterruptController))
    {
        return;
    }

    hyperdos_programmable_interrupt_controller_raise_request(&pc->programmableInterruptController,
                                                             HYPERDOS_PC_SLAVE_INTERRUPT_CASCADE_REQUEST_LINE);
}

int hyperdos_pc_acknowledge_interrupt_request(hyperdos_pc* pc, uint8_t* interruptNumber)
{
    uint8_t masterRequestLine     = 0u;
    uint8_t masterInterruptNumber = 0u;
    uint8_t slaveInterruptNumber  = 0u;
    uint8_t slaveRequestLine      = 0u;

    if (pc == NULL || interruptNumber == NULL)
    {
        return 0;
    }

    hyperdos_pc_raise_slave_interrupt_cascade_request_if_needed(pc);
    if (!hyperdos_programmable_interrupt_controller_acknowledge_request(&pc->programmableInterruptController,
                                                                        &masterRequestLine,
                                                                        &masterInterruptNumber))
    {
        return 0;
    }
    if (pc->slaveProgrammableInterruptControllerEnabled != 0u &&
        masterRequestLine == HYPERDOS_PC_SLAVE_INTERRUPT_CASCADE_REQUEST_LINE &&
        hyperdos_programmable_interrupt_controller_acknowledge_request(&pc->slaveProgrammableInterruptController,
                                                                       &slaveRequestLine,
                                                                       &slaveInterruptNumber))
    {
        (void)slaveRequestLine;
        *interruptNumber = slaveInterruptNumber;
        return 1;
    }

    *interruptNumber = masterInterruptNumber;
    return 1;
}

uint64_t hyperdos_pc_step_halted_processor_clock(hyperdos_pc*                     pc,
                                                 uint32_t                         milliseconds,
                                                 hyperdos_pc_board_trace_function traceFunction,
                                                 void*                            traceUserContext)
{
    uint64_t processorClockCount = 0u;

    if (pc == NULL)
    {
        return 0u;
    }

    processorClockCount = ((uint64_t)pc->clockGenerator.processorFrequencyHertz * milliseconds) /
                          HYPERDOS_PC_MILLISECONDS_PER_SECOND;
    if (processorClockCount == 0u)
    {
        processorClockCount = 1u;
    }
    hyperdos_intel_8284_clock_generator_step(&pc->clockGenerator, &pc->bus, processorClockCount);
    hyperdos_pc_raise_interval_timer_interrupt_request(pc, traceFunction, traceUserContext);
    return processorClockCount;
}
