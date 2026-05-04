#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#if defined(_MSC_VER)
#include <crtdbg.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hyperdos/devices.h"
#include "hyperdos/hardware.h"
#include "hyperdos/pc_bios.h"
#include "hyperdos/pc_bios_data_area.h"
#include "hyperdos/pc_machine.h"
#include "hyperdos/pc_storage.h"
#include "hyperdos/pc_text.h"
#include "hyperdos/pc_video_bios.h"
#include "hyperdos/pc_video_services.h"
#include "hyperdos/x86_processor.h"

enum
{
    TEST_DOS_SERVICE_REGISTER_SHIFT                                  = 8u,
    TEST_DOS_EXIT_CODE_MASK                                          = 0x00FFu,
    TEST_BYTE_MASK                                                   = 0x00FFu,
    TEST_WORD_BYTE_COUNT                                             = 2u,
    TEST_DEFAULT_INSTRUCTION_LIMIT                                   = 1000u,
    TEST_PERIPHERAL_INTERFACE_PORT                                   = 0x0060u,
    TEST_PERIPHERAL_INTERFACE_PORT_COUNT                             = 4u,
    TEST_PERIPHERAL_INTERFACE_VALUE                                  = 0x5Au,
    TEST_KEYBOARD_CONTROLLER_DATA_PORT                               = 0x0060u,
    TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT                     = 0x0064u,
    TEST_KEYBOARD_CONTROLLER_STATUS_OUTPUT_BUFFER_FULL               = 0x01u,
    TEST_KEYBOARD_CONTROLLER_STATUS_AUXILIARY_OUTPUT_BUFFER          = 0x20u,
    TEST_KEYBOARD_CONTROLLER_COMMAND_READ_COMMAND_BYTE               = 0x20u,
    TEST_KEYBOARD_CONTROLLER_COMMAND_WRITE_COMMAND_BYTE              = 0x60u,
    TEST_KEYBOARD_CONTROLLER_COMMAND_DISABLE_KEYBOARD                = 0xADu,
    TEST_KEYBOARD_CONTROLLER_COMMAND_ENABLE_KEYBOARD                 = 0xAEu,
    TEST_KEYBOARD_CONTROLLER_COMMAND_ENABLE_AUXILIARY                = 0xA8u,
    TEST_KEYBOARD_CONTROLLER_COMMAND_WRITE_KEYBOARD_OUTPUT           = 0xD2u,
    TEST_KEYBOARD_CONTROLLER_COMMAND_WRITE_AUXILIARY_DEVICE          = 0xD4u,
    TEST_KEYBOARD_CONTROLLER_COMMAND_BYTE_DEFAULT                    = 0x45u,
    TEST_KEYBOARD_CONTROLLER_COMMAND_BYTE_AUXILIARY_INTERRUPT_ENABLE = 0x02u,
    TEST_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_SCALING_ONE_TO_ONE       = 0xE6u,
    TEST_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_SCALING_TWO_TO_ONE       = 0xE7u,
    TEST_KEYBOARD_DEVICE_COMMAND_SET_SCAN_CODE_SET                   = 0xF0u,
    TEST_AUXILIARY_MOUSE_DEVICE_COMMAND_STATUS_REQUEST               = 0xE9u,
    TEST_AUXILIARY_MOUSE_DEVICE_COMMAND_ENABLE_REPORTING             = 0xF4u,
    TEST_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE                        = 0xFAu,
    TEST_KEYBOARD_SCAN_CODE_A                                        = 0x1Eu,
    TEST_KEYBOARD_SCAN_CODE_B                                        = 0x30u,
    TEST_AUXILIARY_MOUSE_LEFT_BUTTON                                 = 0x01u,
    TEST_AUXILIARY_MOUSE_STATUS_LEFT_BUTTON                          = 0x04u,
    TEST_AUXILIARY_MOUSE_STATUS_SCALING_TWO_TO_ONE                   = 0x10u,
    TEST_AUXILIARY_MOUSE_STATUS_REPORTING_ENABLED                    = 0x20u,
    TEST_AUXILIARY_MOUSE_DEFAULT_RESOLUTION                          = 2u,
    TEST_AUXILIARY_MOUSE_DEFAULT_SAMPLE_RATE                         = 100u,
    TEST_AUXILIARY_MOUSE_PACKET_ALWAYS_ONE                           = 0x08u,
    TEST_AUXILIARY_MOUSE_PACKET_HORIZONTAL_SIGN                      = 0x10u,
    TEST_AUXILIARY_MOUSE_PACKET_VERTICAL_SIGN                        = 0x20u,
    TEST_PC_EQUIPMENT_FLAGS_POINTING_DEVICE_PRESENT                  = 0x0004u,
    TEST_MASTER_CASCADE_INTERRUPT_MASK                               = 0x04u,
    TEST_AUXILIARY_DEVICE_INTERRUPT_MASK                             = 0x10u,
    TEST_KEYBOARD_FUNCTION_KEY_SIX_WORD                              = 0x4000u,
    TEST_PC_INTERVAL_TIMER_CONTROL_PORT                              = 0x0043u,
    TEST_PC_INTERVAL_TIMER_CHANNEL_TWO_PORT                          = 0x0042u,
    TEST_PC_INTERVAL_TIMER_CHANNEL_TWO_SQUARE_WAVE_LOW_HIGH          = 0xB6u,
    TEST_PC_INTERVAL_TIMER_CHANNEL_TWO_RELOAD_LOW_BYTE               = 0x33u,
    TEST_PC_INTERVAL_TIMER_CHANNEL_TWO_RELOAD_HIGH_BYTE              = 0x05u,
    TEST_PC_PERIPHERAL_INTERFACE_PORT_B                              = 0x0061u,
    TEST_PC_SPEAKER_ENABLE_BITS                                      = 0x03u,
    TEST_PC_DEFAULT_TIMER_INTERRUPT_CLOCK_COUNT                      = 262144u,
    TEST_PC_TIMER_TICKS_PER_DAY                                      = 0x001800B0u,
    TEST_INTERRUPT_RETURN_STACK_POINTER                              = 0x0100u,
    TEST_INTERRUPT_RETURN_FLAGS_STACK_OFFSET                         = 4u,
    TEST_EXTERNAL_INTERRUPT_NUMBER                                   = 0x30u,
    TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET                            = 0x0123u,
    TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT                           = 0x2345u,
    TEST_SINGLE_STEP_INTERRUPT_VECTOR_OFFSET                         = 0x0234u,
    TEST_SINGLE_STEP_INTERRUPT_VECTOR_SEGMENT                        = 0x3456u,
    TEST_SINGLE_STEP_EXPECTED_ACCUMULATOR                            = 0x1234u,
    TEST_BOUND_INTERRUPT_VECTOR_OFFSET                               = 0x0456u,
    TEST_BOUND_INTERRUPT_VECTOR_SEGMENT                              = 0x4567u,
    TEST_BOUND_FAULT_OFFSET                                          = 0x010Fu,
    TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_OFFSET                        = 0x0123u,
    TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_SEGMENT                       = 0x2345u,
    TEST_DIVIDE_ERROR_RETURN_OFFSET                                  = 0x010Bu,
    TEST_DIVIDE_ERROR_FAULT_OFFSET                                   = 0x0109u,
    TEST_REPEATED_MOVE_SOURCE_OFFSET                                 = 0x0120u,
    TEST_REPEATED_MOVE_DESTINATION_OFFSET                            = 0x0130u,
    TEST_SHIFT_ROTATE_MEMORY_OFFSET                                  = 0x0200u,
    TEST_SHIFT_ROTATE_BYTE_RESULT                                    = 0x0Cu,
    TEST_SHIFT_ROTATE_WORD_RESULT                                    = 0x0010u,
    TEST_SEGMENT_BOUNDARY_LOW_OFFSET                                 = 0xFFFFu,
    TEST_SEGMENT_BOUNDARY_LINEAR_HIGH_OFFSET                         = 0x10000u,
    TEST_SEGMENT_BOUNDARY_LOW_BYTE                                   = 0x34u,
    TEST_SEGMENT_BOUNDARY_HIGH_BYTE                                  = 0x12u,
    TEST_LOAD_SEGMENT_POINTER_MEMORY_OFFSET                          = 0x0220u,
    TEST_LOAD_SEGMENT_POINTER_OFFSET_VALUE                           = 0x3456u,
    TEST_LOAD_SEGMENT_POINTER_SEGMENT_VALUE                          = 0x789Au,
    TEST_FAR_POINTER_OFFSET                                          = 0x0200u,
    TEST_FAR_CALL_TARGET_OFFSET                                      = 0x0100u,
    TEST_FAR_CALL_INCORRECT_OFFSET                                   = 0x0300u,
    TEST_FAR_CALL_INITIAL_SEGMENT                                    = 0x1000u,
    TEST_FAR_CALL_TARGET_SEGMENT                                     = 0x1234u,
    TEST_FAR_CALL_STACK_POINTER                                      = 0xFFEEu,
    TEST_FAR_CALL_EXPECTED_ACCUMULATOR                               = 0xABCDu,
    TEST_FAR_CALL_INCORRECT_ACCUMULATOR                              = 0x1234u,
    TEST_8087_CONTROL_WORD_OFFSET                                    = 0x0200u,
    TEST_8087_STORED_CONTROL_WORD_OFFSET                             = 0x0204u,
    TEST_8087_FIRST_REAL_OFFSET                                      = 0x0210u,
    TEST_8087_SECOND_REAL_OFFSET                                     = 0x0214u,
    TEST_8087_RESULT_REAL_OFFSET                                     = 0x0218u,
    TEST_VIDEO_WRITE_BYTE_COUNT                                      = 5u
};

typedef struct test_dos_context
{
    char   output[128];
    size_t outputLength;
    int    exitCode;
} test_dos_context;

typedef struct test_speaker_context
{
    uint32_t frequencyHertz;
    uint8_t  enabled;
    size_t   changeCount;
} test_speaker_context;

typedef struct test_disk_bios_context
{
    hyperdos_pc_disk_image* diskImage;
} test_disk_bios_context;

typedef struct test_string_input_output_device
{
    uint8_t  readBytes[32];
    uint16_t readPorts[32];
    size_t   readByteCount;
    size_t   readByteIndex;
    uint8_t  writtenBytes[32];
    uint16_t writtenPorts[32];
    size_t   writtenByteCount;
} test_string_input_output_device;

typedef struct test_coprocessor_escape_context
{
    size_t escapeCallCount;
} test_coprocessor_escape_context;

typedef struct test_coprocessor_wait_context
{
    size_t waitCallCount;
} test_coprocessor_wait_context;

static void test_configure_assertion_reporting(void)
{
#if defined(_MSC_VER)
    _set_error_mode(_OUT_TO_STDERR);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif
}

static uint8_t test_string_input_output_read_byte(void* device, uint16_t port)
{
    test_string_input_output_device* inputOutputDevice = (test_string_input_output_device*)device;
    if (inputOutputDevice == NULL || inputOutputDevice->readByteIndex >= inputOutputDevice->readByteCount)
    {
        return 0xFFu;
    }
    inputOutputDevice->readPorts[inputOutputDevice->readByteIndex] = port;
    return inputOutputDevice->readBytes[inputOutputDevice->readByteIndex++];
}

static void test_string_input_output_write_byte(void* device, uint16_t port, uint8_t value)
{
    test_string_input_output_device* inputOutputDevice = (test_string_input_output_device*)device;
    if (inputOutputDevice == NULL || inputOutputDevice->writtenByteCount >= sizeof(inputOutputDevice->writtenBytes))
    {
        return;
    }
    inputOutputDevice->writtenPorts[inputOutputDevice->writtenByteCount]   = port;
    inputOutputDevice->writtenBytes[inputOutputDevice->writtenByteCount++] = value;
}

static hyperdos_x86_execution_result test_coprocessor_escape_records_call(
        hyperdos_x86_processor*                     processor,
        const hyperdos_x86_coprocessor_instruction* instruction,
        void*                                       userContext)
{
    test_coprocessor_escape_context* context = (test_coprocessor_escape_context*)userContext;

    (void)processor;
    (void)instruction;
    if (context != NULL)
    {
        ++context->escapeCallCount;
    }
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result test_coprocessor_wait_records_call(hyperdos_x86_processor* processor,
                                                                        void*                   userContext)
{
    test_coprocessor_wait_context* context = (test_coprocessor_wait_context*)userContext;

    (void)processor;
    if (context != NULL)
    {
        ++context->waitCallCount;
    }
    return HYPERDOS_X86_EXECUTION_OK;
}

static const uint8_t sampleDosProgram[] = {
    0xBAu, 0x0Du, 0x01u, 0xB4u, 0x09u, 0xCDu, 0x21u, 0xB4u, 0x4Cu, 0x30u, 0xC0u, 0xCDu, 0x21u,
    'H',   'y',   'p',   'e',   'r',   'D',   'O',   'S',   ' ',   'D',   'O',   'S',   ' ',
    'C',   'O',   'M',   ' ',   's',   'a',   'm',   'p',   'l',   'e',   0x0Du, 0x0Au, '$',
};

static hyperdos_pc_disk_image* test_get_disk_bios_disk_image(void* userContext, uint8_t driveNumber)
{
    test_disk_bios_context* diskBiosContext = (test_disk_bios_context*)userContext;

    if (diskBiosContext == NULL || driveNumber != HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER)
    {
        return NULL;
    }
    return diskBiosContext->diskImage;
}

static hyperdos_pc_disk_image* test_get_storage_disk_bios_disk_image(void* userContext, uint8_t driveNumber)
{
    return hyperdos_pc_storage_get_disk_for_bios_drive_number((hyperdos_pc_storage_context*)userContext, driveNumber);
}

static hyperdos_x86_execution_result test_dos_interrupt_handler(hyperdos_x86_processor* processor,
                                                                uint8_t                 interruptNumber,
                                                                void*                   userContext)
{
    test_dos_context* dosContext = (test_dos_context*)userContext;

    if (interruptNumber == HYPERDOS_X86_DOS_INTERRUPT_TERMINATE_PROGRAM)
    {
        dosContext->exitCode = 0;
        hyperdos_x86_stop_processor(processor);
        return HYPERDOS_X86_EXECUTION_OK;
    }

    if (interruptNumber == HYPERDOS_X86_DOS_INTERRUPT_FUNCTION_DISPATCH)
    {
        uint8_t serviceNumber =
                (uint8_t)(hyperdos_x86_get_general_register_word(processor,
                                                                 HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) >>
                          TEST_DOS_SERVICE_REGISTER_SHIFT);
        if (serviceNumber == HYPERDOS_X86_DOS_SERVICE_PRINT_STRING)
        {
            uint16_t offset = hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA);
            for (;;)
            {
                uint8_t value = 0;
                assert(hyperdos_x86_read_memory_byte(processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, offset, &value) ==
                       HYPERDOS_X86_EXECUTION_OK);
                if (value == HYPERDOS_X86_DOS_PRINT_STRING_TERMINATOR)
                {
                    break;
                }
                if (dosContext->outputLength + 1u < sizeof(dosContext->output))
                {
                    dosContext->output[dosContext->outputLength] = (char)value;
                    ++dosContext->outputLength;
                    dosContext->output[dosContext->outputLength] = '\0';
                }
                offset = (uint16_t)(offset + 1u);
            }
            return HYPERDOS_X86_EXECUTION_OK;
        }

        if (serviceNumber == HYPERDOS_X86_DOS_SERVICE_TERMINATE_PROCESS)
        {
            dosContext->exitCode = hyperdos_x86_get_general_register_word(processor,
                                                                          HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) &
                                   TEST_DOS_EXIT_CODE_MASK;
            hyperdos_x86_stop_processor(processor);
            return HYPERDOS_X86_EXECUTION_OK;
        }
    }

    return HYPERDOS_X86_EXECUTION_INTERRUPT_NOT_HANDLED;
}

static void test_dos_program_output(void)
{
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    test_dos_context              dosContext;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    memset(&dosContext, 0, sizeof(dosContext));
    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_interrupt_handler(&processor, test_dos_interrupt_handler, &dosContext);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         sampleDosProgram,
                                         sizeof(sampleDosProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(dosContext.exitCode == 0);
    assert(strcmp(dosContext.output, "HyperDOS DOS COM sample\r\n") == 0);
    free(memory);
}

static void test_loop_and_arithmetic(void)
{
    static const uint8_t program[] = {
        0xB9u,
        0x05u,
        0x00u,
        0x31u,
        0xC0u,
        0x05u,
        0x01u,
        0x00u,
        0xE2u,
        0xFBu,
        0xF4u,
    };
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 5u);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER) == 0u);
    free(memory);
}

static void test_shift_rotate_8086_forms(void)
{
    static const uint8_t program[] = {
        0xC6u, 0x06u, 0x00u, 0x02u, 0x03u, 0xD0u, 0x26u, 0x00u, 0x02u, 0xD0u, 0x26u, 0x00u,
        0x02u, 0xA0u, 0x00u, 0x02u, 0xBBu, 0x01u, 0x00u, 0xB1u, 0x04u, 0xD3u, 0xE3u, 0xF4u,
    };
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result     = HYPERDOS_X86_EXECUTION_OK;
    uint8_t                       memoryByte = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert((hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) &
            TEST_BYTE_MASK) == TEST_SHIFT_ROTATE_BYTE_RESULT);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_BASE) ==
           TEST_SHIFT_ROTATE_WORD_RESULT);
    assert(hyperdos_x86_read_memory_byte(&processor,
                                         HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                         TEST_SHIFT_ROTATE_MEMORY_OFFSET,
                                         &memoryByte) == HYPERDOS_X86_EXECUTION_OK);
    assert(memoryByte == TEST_SHIFT_ROTATE_BYTE_RESULT);
    free(memory);
}

static void test_80186_only_operation_codes_are_unsupported(void)
{
    static const uint8_t unsupportedOperationCodes[] = {
        0x60u,
        0x61u,
        0x62u,
        0x68u,
        0x69u,
        0x6Au,
        0x6Bu,
        0x6Cu,
        0x6Du,
        0x6Eu,
        0x6Fu,
        0xC0u,
        0xC1u,
        0xC8u,
        0xC9u,
    };
    static const hyperdos_x86_processor_model unsupportedProcessorModels[] = {
        HYPERDOS_X86_PROCESSOR_MODEL_8086,
        HYPERDOS_X86_PROCESSOR_MODEL_8088,
    };
    size_t processorModelIndex = 0u;
    size_t operationCodeIndex  = 0u;

    for (processorModelIndex = 0u;
         processorModelIndex < sizeof(unsupportedProcessorModels) / sizeof(unsupportedProcessorModels[0]);
         ++processorModelIndex)
    {
        for (operationCodeIndex = 0u;
             operationCodeIndex < sizeof(unsupportedOperationCodes) / sizeof(unsupportedOperationCodes[0]);
             ++operationCodeIndex)
        {
            uint8_t                       program[] = {unsupportedOperationCodes[operationCodeIndex], 0xF4u};
            uint8_t*                      memory    = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
            hyperdos_x86_processor        processor;
            hyperdos_x86_execution_result result         = HYPERDOS_X86_EXECUTION_OK;
            hyperdos_x86_processor_model  processorModel = unsupportedProcessorModels[processorModelIndex];

            assert(memory != NULL);
            assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
                   HYPERDOS_X86_EXECUTION_OK);
            hyperdos_x86_set_processor_model(&processor, processorModel);
            assert(hyperdos_x86_load_dos_program(&processor,
                                                 program,
                                                 sizeof(program),
                                                 HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                                 "",
                                                 0u) == HYPERDOS_X86_EXECUTION_OK);

            result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
            assert(result == HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION);
            free(memory);
        }
    }
}

static void test_80186_shift_rotate_immediate_count(void)
{
    static const uint8_t program[] = {
        0xBAu,
        0x23u,
        0x00u,
        0xC1u,
        0xEAu,
        0x03u,
        0xF4u,
    };
    static const hyperdos_x86_processor_model supportedProcessorModels[] = {
        HYPERDOS_X86_PROCESSOR_MODEL_80186,
        HYPERDOS_X86_PROCESSOR_MODEL_80188,
        HYPERDOS_X86_PROCESSOR_MODEL_80286,
    };
    size_t processorModelIndex = 0u;

    for (processorModelIndex = 0u;
         processorModelIndex < sizeof(supportedProcessorModels) / sizeof(supportedProcessorModels[0]);
         ++processorModelIndex)
    {
        uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
        hyperdos_x86_processor        processor;
        hyperdos_x86_execution_result result         = HYPERDOS_X86_EXECUTION_OK;
        hyperdos_x86_processor_model  processorModel = supportedProcessorModels[processorModelIndex];

        assert(memory != NULL);
        assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
               HYPERDOS_X86_EXECUTION_OK);
        hyperdos_x86_set_processor_model(&processor, processorModel);
        assert(hyperdos_x86_load_dos_program(&processor,
                                             program,
                                             sizeof(program),
                                             HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                             "",
                                             0u) == HYPERDOS_X86_EXECUTION_OK);

        result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
        assert(result == HYPERDOS_X86_EXECUTION_HALTED);
        assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_DATA) == 0x0004u);
        free(memory);
    }
}

static uint16_t test_80186_shift_rotate_reference_value(uint16_t originalValue,
                                                        uint8_t  operationIndex,
                                                        uint8_t  count,
                                                        int      isWordOperand,
                                                        int      initialCarry,
                                                        int*     finalCarry)
{
    uint16_t valueMask  = isWordOperand ? HYPERDOS_X86_WORD_MASK : HYPERDOS_X86_BYTE_MASK;
    uint16_t signBit    = isWordOperand ? 0x8000u : 0x0080u;
    uint16_t value      = (uint16_t)(originalValue & valueMask);
    uint8_t  shiftCount = (uint8_t)(count & 0x1Fu);
    int      carry      = initialCarry != 0;
    uint8_t  shiftIndex = 0u;

    for (shiftIndex = 0u; shiftIndex < shiftCount; ++shiftIndex)
    {
        switch (operationIndex)
        {
        case 0u:
            carry = (value & signBit) != 0u;
            value = (uint16_t)(((value << 1u) | (carry ? 1u : 0u)) & valueMask);
            break;
        case 1u:
            carry = (value & 1u) != 0u;
            value = (uint16_t)((value >> 1u) | (carry ? signBit : 0u));
            break;
        case 2u:
        {
            int oldCarry = carry;
            carry        = (value & signBit) != 0u;
            value        = (uint16_t)(((value << 1u) | (oldCarry ? 1u : 0u)) & valueMask);
            break;
        }
        case 3u:
        {
            int oldCarry = carry;
            carry        = (value & 1u) != 0u;
            value        = (uint16_t)((value >> 1u) | (oldCarry ? signBit : 0u));
            break;
        }
        case 4u:
            carry = (value & signBit) != 0u;
            value = (uint16_t)((value << 1u) & valueMask);
            break;
        case 5u:
            carry = (value & 1u) != 0u;
            value = (uint16_t)(value >> 1u);
            break;
        case 7u:
            carry = (value & 1u) != 0u;
            value = (uint16_t)((value >> 1u) | (value & signBit));
            break;
        }
    }

    *finalCarry = carry;
    return (uint16_t)(value & valueMask);
}

static uint16_t test_80186_shift_rotate_initial_value(uint8_t operationIndex, int isWordOperand)
{
    if (operationIndex == 2u)
    {
        return isWordOperand ? 0x8000u : 0x0080u;
    }
    if (operationIndex == 3u)
    {
        return 0x0001u;
    }
    return isWordOperand ? 0x8001u : 0x0081u;
}

static void test_80186_shift_rotate_immediate_case(hyperdos_x86_processor_model processorModel,
                                                   uint8_t                      operationIndex,
                                                   int                          isWordOperand,
                                                   int                          usesMemoryOperand,
                                                   uint8_t                      count)
{
    uint8_t                       program[16];
    size_t                        programByteCount = 0u;
    uint16_t                      initialValue  = test_80186_shift_rotate_initial_value(operationIndex, isWordOperand);
    int                           initialCarry  = 1;
    int                           expectedCarry = 0;
    uint16_t                      expectedValue = test_80186_shift_rotate_reference_value(initialValue,
                                                                     operationIndex,
                                                                     count,
                                                                     isWordOperand,
                                                                     initialCarry,
                                                                     &expectedCarry);
    uint8_t*                      memory        = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result      = HYPERDOS_X86_EXECUTION_OK;
    uint16_t                      actualValue = 0u;
    uint8_t                       lowByte     = 0u;
    uint8_t                       highByte    = 0u;

    if (usesMemoryOperand)
    {
        if (isWordOperand)
        {
            program[programByteCount++] = 0xC7u;
            program[programByteCount++] = 0x06u;
            program[programByteCount++] = 0x00u;
            program[programByteCount++] = 0x02u;
            program[programByteCount++] = (uint8_t)(initialValue & TEST_BYTE_MASK);
            program[programByteCount++] = (uint8_t)(initialValue >> HYPERDOS_X86_BYTE_BIT_COUNT);
        }
        else
        {
            program[programByteCount++] = 0xC6u;
            program[programByteCount++] = 0x06u;
            program[programByteCount++] = 0x00u;
            program[programByteCount++] = 0x02u;
            program[programByteCount++] = (uint8_t)(initialValue & TEST_BYTE_MASK);
        }
    }
    else if (isWordOperand)
    {
        program[programByteCount++] = 0xB8u;
        program[programByteCount++] = (uint8_t)(initialValue & TEST_BYTE_MASK);
        program[programByteCount++] = (uint8_t)(initialValue >> HYPERDOS_X86_BYTE_BIT_COUNT);
    }
    else
    {
        program[programByteCount++] = 0xB0u;
        program[programByteCount++] = (uint8_t)(initialValue & TEST_BYTE_MASK);
    }

    program[programByteCount++] = initialCarry ? 0xF9u : 0xF8u;
    program[programByteCount++] = isWordOperand ? 0xC1u : 0xC0u;
    program[programByteCount++] = (uint8_t)((operationIndex << 3u) | (usesMemoryOperand ? 0x06u : 0xC0u));
    if (usesMemoryOperand)
    {
        program[programByteCount++] = 0x00u;
        program[programByteCount++] = 0x02u;
    }
    program[programByteCount++] = count;
    program[programByteCount++] = 0xF4u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         programByteCount,
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    if (usesMemoryOperand)
    {
        assert(hyperdos_x86_read_memory_byte(&processor,
                                             HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                             TEST_SHIFT_ROTATE_MEMORY_OFFSET,
                                             &lowByte) == HYPERDOS_X86_EXECUTION_OK);
        if (isWordOperand)
        {
            assert(hyperdos_x86_read_memory_byte(&processor,
                                                 HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                                 (uint16_t)(TEST_SHIFT_ROTATE_MEMORY_OFFSET + 1u),
                                                 &highByte) == HYPERDOS_X86_EXECUTION_OK);
            actualValue = (uint16_t)(lowByte | ((uint16_t)highByte << HYPERDOS_X86_BYTE_BIT_COUNT));
        }
        else
        {
            actualValue = lowByte;
        }
    }
    else
    {
        actualValue = hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
        if (!isWordOperand)
        {
            actualValue &= TEST_BYTE_MASK;
        }
    }

    assert(actualValue == expectedValue);
    assert(((hyperdos_x86_get_flags_word(&processor) & HYPERDOS_X86_FLAG_CARRY) != 0u) == expectedCarry);
    free(memory);
}

static void test_80186_shift_rotate_immediate_matrix_for_model(hyperdos_x86_processor_model processorModel)
{
    static const uint8_t operationIndices[] = {0u, 1u, 2u, 3u, 4u, 5u, 7u};
    static const uint8_t counts[]           = {0u, 1u, 33u};
    size_t               operationIndex     = 0u;
    size_t               countIndex         = 0u;
    int                  isWordOperand      = 0;
    int                  usesMemoryOperand  = 0;

    for (operationIndex = 0u; operationIndex < sizeof(operationIndices) / sizeof(operationIndices[0]); ++operationIndex)
    {
        for (isWordOperand = 0; isWordOperand <= 1; ++isWordOperand)
        {
            for (usesMemoryOperand = 0; usesMemoryOperand <= 1; ++usesMemoryOperand)
            {
                for (countIndex = 0u; countIndex < sizeof(counts) / sizeof(counts[0]); ++countIndex)
                {
                    test_80186_shift_rotate_immediate_case(processorModel,
                                                           operationIndices[operationIndex],
                                                           isWordOperand,
                                                           usesMemoryOperand,
                                                           counts[countIndex]);
                }
            }
        }
    }
}

static void test_80186_shift_rotate_immediate_matrix(void)
{
    test_80186_shift_rotate_immediate_matrix_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80186);
    test_80186_shift_rotate_immediate_matrix_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80188);
    test_80186_shift_rotate_immediate_matrix_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80286);
}

static void test_segment_boundary_word_write(hyperdos_x86_processor_model processorModel, int wrapsAtSegmentBoundary)
{
    static const uint8_t program[] = {
        0xB8u,
        TEST_SEGMENT_BOUNDARY_LOW_BYTE,
        TEST_SEGMENT_BOUNDARY_HIGH_BYTE,
        0xA3u,
        0xFFu,
        0xFFu,
        0xF4u,
    };
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;
    uint32_t segmentBase                 = ((uint32_t)HYPERDOS_X86_DEFAULT_DOS_SEGMENT << HYPERDOS_X86_SEGMENT_SHIFT) &
                           HYPERDOS_X86_ADDRESS_MASK;
    uint32_t lowPhysicalAddress         = (segmentBase + TEST_SEGMENT_BOUNDARY_LOW_OFFSET) & HYPERDOS_X86_ADDRESS_MASK;
    uint32_t wrappedHighPhysicalAddress = segmentBase & HYPERDOS_X86_ADDRESS_MASK;
    uint32_t linearHighPhysicalAddress  = (segmentBase + TEST_SEGMENT_BOUNDARY_LINEAR_HIGH_OFFSET) &
                                         HYPERDOS_X86_ADDRESS_MASK;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(memory[lowPhysicalAddress] == TEST_SEGMENT_BOUNDARY_LOW_BYTE);
    if (wrapsAtSegmentBoundary)
    {
        assert(memory[wrappedHighPhysicalAddress] == TEST_SEGMENT_BOUNDARY_HIGH_BYTE);
        assert(memory[linearHighPhysicalAddress] != TEST_SEGMENT_BOUNDARY_HIGH_BYTE);
    }
    else
    {
        assert(memory[wrappedHighPhysicalAddress] != TEST_SEGMENT_BOUNDARY_HIGH_BYTE);
        assert(memory[linearHighPhysicalAddress] == TEST_SEGMENT_BOUNDARY_HIGH_BYTE);
    }
    free(memory);
}

static void test_segment_boundary_word_write_policy(void)
{
    test_segment_boundary_word_write(HYPERDOS_X86_PROCESSOR_MODEL_8086, 1);
    test_segment_boundary_word_write(HYPERDOS_X86_PROCESSOR_MODEL_8088, 1);
    test_segment_boundary_word_write(HYPERDOS_X86_PROCESSOR_MODEL_80186, 0);
    test_segment_boundary_word_write(HYPERDOS_X86_PROCESSOR_MODEL_80188, 0);
    test_segment_boundary_word_write(HYPERDOS_X86_PROCESSOR_MODEL_80286, 0);
}

static void test_push_stack_pointer_program(hyperdos_x86_processor_model processorModel,
                                            const uint8_t*               program,
                                            size_t                       programSize,
                                            uint16_t                     expectedAccumulator)
{
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_load_dos_program(&processor, program, programSize, HYPERDOS_X86_DEFAULT_DOS_SEGMENT, "", 0u) ==
           HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           expectedAccumulator);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER) == 0x0200u);
    free(memory);
}

static void test_push_stack_pointer_for_model(hyperdos_x86_processor_model processorModel, uint16_t expectedAccumulator)
{
    static const uint8_t shortPushStackPointerProgram[] = {
        0xBCu,
        0x00u,
        0x02u,
        0x54u,
        0x58u,
        0xF4u,
    };
    static const uint8_t groupFivePushStackPointerProgram[] = {
        0xBCu,
        0x00u,
        0x02u,
        0xFFu,
        0xF4u,
        0x58u,
        0xF4u,
    };

    test_push_stack_pointer_program(processorModel,
                                    shortPushStackPointerProgram,
                                    sizeof(shortPushStackPointerProgram),
                                    expectedAccumulator);
    test_push_stack_pointer_program(processorModel,
                                    groupFivePushStackPointerProgram,
                                    sizeof(groupFivePushStackPointerProgram),
                                    expectedAccumulator);
}

static void test_push_stack_pointer_model_policy(void)
{
    test_push_stack_pointer_for_model(HYPERDOS_X86_PROCESSOR_MODEL_8086, 0x01FEu);
    test_push_stack_pointer_for_model(HYPERDOS_X86_PROCESSOR_MODEL_8088, 0x01FEu);
    test_push_stack_pointer_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80186, 0x01FEu);
    test_push_stack_pointer_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80188, 0x01FEu);
    test_push_stack_pointer_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80286, 0x0200u);
}

static void test_80186_basic_stack_instructions_for_model(hyperdos_x86_processor_model processorModel)
{
    static const uint8_t program[] = {
        0xB8u, 0x11u, 0x11u, 0xB9u, 0x22u, 0x22u, 0xBAu, 0x33u, 0x33u, 0xBBu, 0x44u, 0x44u, 0xBCu, 0x00u, 0x02u,
        0xBDu, 0x66u, 0x66u, 0xBEu, 0x77u, 0x77u, 0xBFu, 0x88u, 0x88u, 0x60u, 0xB8u, 0xAAu, 0xAAu, 0xB9u, 0xBBu,
        0xBBu, 0xBAu, 0xCCu, 0xCCu, 0xBBu, 0xDDu, 0xDDu, 0xBDu, 0xEEu, 0xEEu, 0xBEu, 0x99u, 0x99u, 0xBFu, 0x55u,
        0x55u, 0x61u, 0x6Au, 0x80u, 0x68u, 0x34u, 0x12u, 0xBDu, 0xFEu, 0x01u, 0xC9u, 0xF4u,
    };
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;
    uint32_t                      stackBase;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x1111u);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER) == 0x2222u);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_DATA) == 0x3333u);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_BASE) == 0x4444u);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER) == 0x0200u);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_BASE_POINTER) == 0xFF80u);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX) == 0x7777u);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX) == 0x8888u);

    stackBase = hyperdos_x86_get_segment_base(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK);
    assert(memory[(stackBase + 0x01F6u) & HYPERDOS_X86_ADDRESS_MASK] == 0x00u);
    assert(memory[(stackBase + 0x01F7u) & HYPERDOS_X86_ADDRESS_MASK] == 0x02u);
    assert(memory[(stackBase + 0x01FCu) & HYPERDOS_X86_ADDRESS_MASK] == 0x34u);
    assert(memory[(stackBase + 0x01FDu) & HYPERDOS_X86_ADDRESS_MASK] == 0x12u);
    free(memory);
}

static void test_80186_basic_stack_instructions(void)
{
    test_80186_basic_stack_instructions_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80186);
    test_80186_basic_stack_instructions_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80188);
    test_80186_basic_stack_instructions_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80286);
}

static void test_80186_immediate_signed_multiply_program(hyperdos_x86_processor_model        processorModel,
                                                         const uint8_t*                      program,
                                                         size_t                              programSize,
                                                         hyperdos_x86_general_register_index destinationRegister,
                                                         uint16_t                            expectedDestinationValue,
                                                         int                                 expectedCarryOverflow)
{
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_load_dos_program(&processor, program, programSize, HYPERDOS_X86_DEFAULT_DOS_SEGMENT, "", 0u) ==
           HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, destinationRegister) == expectedDestinationValue);
    if (expectedCarryOverflow)
    {
        assert((hyperdos_x86_get_flags_word(&processor) & HYPERDOS_X86_FLAG_CARRY) != 0u);
        assert((hyperdos_x86_get_flags_word(&processor) & HYPERDOS_X86_FLAG_OVERFLOW) != 0u);
    }
    else
    {
        assert((hyperdos_x86_get_flags_word(&processor) & HYPERDOS_X86_FLAG_CARRY) == 0u);
        assert((hyperdos_x86_get_flags_word(&processor) & HYPERDOS_X86_FLAG_OVERFLOW) == 0u);
    }
    free(memory);
}

static void test_80186_immediate_signed_multiply_for_model(hyperdos_x86_processor_model processorModel)
{
    static const uint8_t wordImmediateRegisterProgram[] = {
        0xB8u,
        0x03u,
        0x00u,
        0x69u,
        0xC0u,
        0x04u,
        0x00u,
        0xF4u,
    };
    static const uint8_t byteImmediateRegisterOverflowProgram[] = {
        0xBBu,
        0x00u,
        0x40u,
        0x6Bu,
        0xDBu,
        0x02u,
        0xF4u,
    };
    static const uint8_t wordImmediateMemoryProgram[] = {
        0xC7u,
        0x06u,
        0x00u,
        0x02u,
        0xFEu,
        0xFFu,
        0x69u,
        0x0Eu,
        0x00u,
        0x02u,
        0x03u,
        0x00u,
        0xF4u,
    };
    static const uint8_t byteImmediateMemoryProgram[] = {
        0xC7u,
        0x06u,
        0x00u,
        0x02u,
        0xFEu,
        0xFFu,
        0x6Bu,
        0x16u,
        0x00u,
        0x02u,
        0x80u,
        0xF4u,
    };

    test_80186_immediate_signed_multiply_program(processorModel,
                                                 wordImmediateRegisterProgram,
                                                 sizeof(wordImmediateRegisterProgram),
                                                 HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                 0x000Cu,
                                                 0);
    test_80186_immediate_signed_multiply_program(processorModel,
                                                 byteImmediateRegisterOverflowProgram,
                                                 sizeof(byteImmediateRegisterOverflowProgram),
                                                 HYPERDOS_X86_GENERAL_REGISTER_BASE,
                                                 0x8000u,
                                                 1);
    test_80186_immediate_signed_multiply_program(processorModel,
                                                 wordImmediateMemoryProgram,
                                                 sizeof(wordImmediateMemoryProgram),
                                                 HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
                                                 0xFFFAu,
                                                 0);
    test_80186_immediate_signed_multiply_program(processorModel,
                                                 byteImmediateMemoryProgram,
                                                 sizeof(byteImmediateMemoryProgram),
                                                 HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                                 0x0100u,
                                                 0);
}

static void test_80186_immediate_signed_multiply(void)
{
    test_80186_immediate_signed_multiply_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80186);
    test_80186_immediate_signed_multiply_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80188);
    test_80186_immediate_signed_multiply_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80286);
}

static void test_80186_bound_passes_for_model(hyperdos_x86_processor_model processorModel)
{
    static const uint8_t program[] = {
        0xC7u, 0x06u, 0x00u, 0x02u, 0xFEu, 0xFFu, 0xC7u, 0x06u, 0x02u, 0x02u, 0x03u, 0x00u, 0xB9u, 0xFEu,
        0xFFu, 0x62u, 0x0Eu, 0x00u, 0x02u, 0xBAu, 0x03u, 0x00u, 0x62u, 0x16u, 0x00u, 0x02u, 0xF4u,
    };
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER) == 0xFFFEu);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_DATA) == 0x0003u);
    free(memory);
}

static void test_80186_bound_failure_program_uses_interrupt_vector(hyperdos_x86_processor_model processorModel,
                                                                   const uint8_t*               program,
                                                                   size_t                       programSize)
{
    static const uint8_t boundInterruptHandler[] = {
        0x8Bu,
        0xECu,
        0x8Bu,
        0x46u,
        0x00u,
        0xF4u,
    };
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result        = HYPERDOS_X86_EXECUTION_OK;
    uint32_t                      vectorAddress = 5u * 4u;
    uint32_t handlerAddress = ((uint32_t)TEST_BOUND_INTERRUPT_VECTOR_SEGMENT << HYPERDOS_X86_SEGMENT_SHIFT) +
                              TEST_BOUND_INTERRUPT_VECTOR_OFFSET;
    size_t byteIndex = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_load_dos_program(&processor, program, programSize, HYPERDOS_X86_DEFAULT_DOS_SEGMENT, "", 0u) ==
           HYPERDOS_X86_EXECUTION_OK);

    memory[vectorAddress]      = (uint8_t)(TEST_BOUND_INTERRUPT_VECTOR_OFFSET & 0x00FFu);
    memory[vectorAddress + 1u] = (uint8_t)(TEST_BOUND_INTERRUPT_VECTOR_OFFSET >> HYPERDOS_X86_BYTE_BIT_COUNT);
    memory[vectorAddress + 2u] = (uint8_t)(TEST_BOUND_INTERRUPT_VECTOR_SEGMENT & 0x00FFu);
    memory[vectorAddress + 3u] = (uint8_t)(TEST_BOUND_INTERRUPT_VECTOR_SEGMENT >> HYPERDOS_X86_BYTE_BIT_COUNT);
    for (byteIndex = 0u; byteIndex < sizeof(boundInterruptHandler); ++byteIndex)
    {
        memory[handlerAddress + byteIndex] = boundInterruptHandler[byteIndex];
    }

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           TEST_BOUND_FAULT_OFFSET);
    free(memory);
}

static void test_80186_bound_failure_uses_interrupt_vector_for_model(hyperdos_x86_processor_model processorModel)
{
    static const uint8_t lowerFailureProgram[] = {
        0xC7u, 0x06u, 0x00u, 0x02u, 0xFEu, 0xFFu, 0xC7u, 0x06u, 0x02u, 0x02u,
        0x03u, 0x00u, 0xB8u, 0xFDu, 0xFFu, 0x62u, 0x06u, 0x00u, 0x02u, 0xF4u,
    };
    static const uint8_t upperFailureProgram[] = {
        0xC7u, 0x06u, 0x00u, 0x02u, 0xFEu, 0xFFu, 0xC7u, 0x06u, 0x02u, 0x02u,
        0x03u, 0x00u, 0xB8u, 0x04u, 0x00u, 0x62u, 0x06u, 0x00u, 0x02u, 0xF4u,
    };

    test_80186_bound_failure_program_uses_interrupt_vector(processorModel,
                                                           lowerFailureProgram,
                                                           sizeof(lowerFailureProgram));
    test_80186_bound_failure_program_uses_interrupt_vector(processorModel,
                                                           upperFailureProgram,
                                                           sizeof(upperFailureProgram));
}

static void test_80186_bound_instruction(void)
{
    test_80186_bound_passes_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80186);
    test_80186_bound_passes_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80188);
    test_80186_bound_passes_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80286);
    test_80186_bound_failure_uses_interrupt_vector_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80186);
    test_80186_bound_failure_uses_interrupt_vector_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80188);
    test_80186_bound_failure_uses_interrupt_vector_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80286);
}

static void test_80186_enter_for_model(hyperdos_x86_processor_model processorModel)
{
    static const uint8_t levelZeroProgram[] = {
        0xBCu,
        0x00u,
        0x02u,
        0xBDu,
        0x00u,
        0x03u,
        0xC8u,
        0x04u,
        0x00u,
        0x00u,
        0xF4u,
    };
    static const uint8_t levelOneProgram[] = {
        0xBCu,
        0x00u,
        0x02u,
        0xBDu,
        0x00u,
        0x03u,
        0xC8u,
        0x02u,
        0x00u,
        0x01u,
        0xF4u,
    };
    static const uint8_t levelTwoProgram[] = {
        0xBCu,
        0x00u,
        0x02u,
        0xBDu,
        0x00u,
        0x03u,
        0xC8u,
        0x00u,
        0x00u,
        0x02u,
        0xF4u,
    };
    static const struct
    {
        const uint8_t* program;
        size_t         programSize;
        uint16_t       expectedStackPointer;
    } testCases[] = {
        {
         .program              = levelZeroProgram,
         .programSize          = sizeof(levelZeroProgram),
         .expectedStackPointer = 0x01FAu,
         },
        {
         .program              = levelOneProgram,
         .programSize          = sizeof(levelOneProgram),
         .expectedStackPointer = 0x01FAu,
         },
        {
         .program              = levelTwoProgram,
         .programSize          = sizeof(levelTwoProgram),
         .expectedStackPointer = 0x01FAu,
         },
    };
    size_t testCaseIndex = 0u;

    for (testCaseIndex = 0u; testCaseIndex < sizeof(testCases) / sizeof(testCases[0]); ++testCaseIndex)
    {
        uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
        hyperdos_x86_processor        processor;
        hyperdos_x86_execution_result result    = HYPERDOS_X86_EXECUTION_OK;
        uint32_t                      stackBase = 0u;

        assert(memory != NULL);
        assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
               HYPERDOS_X86_EXECUTION_OK);
        hyperdos_x86_set_processor_model(&processor, processorModel);
        assert(hyperdos_x86_load_dos_program(&processor,
                                             testCases[testCaseIndex].program,
                                             testCases[testCaseIndex].programSize,
                                             HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                             "",
                                             0u) == HYPERDOS_X86_EXECUTION_OK);
        if (testCaseIndex == 2u)
        {
            assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, 0x02FEu, 0xEFu) ==
                   HYPERDOS_X86_EXECUTION_OK);
            assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, 0x02FFu, 0xBEu) ==
                   HYPERDOS_X86_EXECUTION_OK);
        }

        result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
        assert(result == HYPERDOS_X86_EXECUTION_HALTED);
        assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_BASE_POINTER) == 0x01FEu);
        assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER) ==
               testCases[testCaseIndex].expectedStackPointer);

        stackBase = hyperdos_x86_get_segment_base(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK);
        assert(memory[(stackBase + 0x01FEu) & HYPERDOS_X86_ADDRESS_MASK] == 0x00u);
        assert(memory[(stackBase + 0x01FFu) & HYPERDOS_X86_ADDRESS_MASK] == 0x03u);
        if (testCaseIndex == 1u)
        {
            assert(memory[(stackBase + 0x01FCu) & HYPERDOS_X86_ADDRESS_MASK] == 0xFEu);
            assert(memory[(stackBase + 0x01FDu) & HYPERDOS_X86_ADDRESS_MASK] == 0x01u);
        }
        if (testCaseIndex == 2u)
        {
            assert(memory[(stackBase + 0x01FCu) & HYPERDOS_X86_ADDRESS_MASK] == 0xEFu);
            assert(memory[(stackBase + 0x01FDu) & HYPERDOS_X86_ADDRESS_MASK] == 0xBEu);
            assert(memory[(stackBase + 0x01FAu) & HYPERDOS_X86_ADDRESS_MASK] == 0xFEu);
            assert(memory[(stackBase + 0x01FBu) & HYPERDOS_X86_ADDRESS_MASK] == 0x01u);
        }
        free(memory);
    }
}

static void test_80186_enter_instruction(void)
{
    test_80186_enter_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80186);
    test_80186_enter_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80188);
    test_80186_enter_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80286);
}

static void test_80186_input_string_for_model(hyperdos_x86_processor_model processorModel)
{
    static const uint8_t program[] = {
        0xB8u, 0x00u, 0x20u, 0x8Eu, 0xC0u, 0xBAu, 0x20u, 0x00u, 0xBFu, 0x00u, 0x02u, 0xB9u, 0x02u, 0x00u,
        0x3Eu, 0xF3u, 0x6Cu, 0xBFu, 0x10u, 0x02u, 0xB9u, 0x02u, 0x00u, 0xFDu, 0xF3u, 0x6Du, 0xF4u,
    };
    uint8_t*                        memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor          processor;
    hyperdos_bus                    bus;
    hyperdos_random_access_memory   randomAccessMemory;
    test_string_input_output_device inputOutputDevice;
    hyperdos_x86_execution_result   result = HYPERDOS_X86_EXECUTION_OK;
    uint8_t                         value  = 0u;

    assert(memory != NULL);
    memset(&inputOutputDevice, 0, sizeof(inputOutputDevice));
    inputOutputDevice.readBytes[0]  = 0xAAu;
    inputOutputDevice.readBytes[1]  = 0xBBu;
    inputOutputDevice.readBytes[2]  = 0x34u;
    inputOutputDevice.readBytes[3]  = 0x12u;
    inputOutputDevice.readBytes[4]  = 0x78u;
    inputOutputDevice.readBytes[5]  = 0x56u;
    inputOutputDevice.readByteCount = 6u;

    hyperdos_bus_initialize(&bus);
    hyperdos_random_access_memory_initialize(&randomAccessMemory, memory, HYPERDOS_X86_MEMORY_SIZE, 0u);
    assert(hyperdos_bus_map_memory(&bus,
                                   0u,
                                   HYPERDOS_X86_MEMORY_SIZE,
                                   &randomAccessMemory,
                                   hyperdos_random_access_memory_read_byte,
                                   hyperdos_random_access_memory_write_byte) == HYPERDOS_BUS_ACCESS_OK);
    assert(hyperdos_bus_map_input_output(&bus,
                                         0x0020u,
                                         2u,
                                         &inputOutputDevice,
                                         test_string_input_output_read_byte,
                                         test_string_input_output_write_byte) == HYPERDOS_BUS_ACCESS_OK);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    hyperdos_x86_attach_bus(&processor, &bus);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(inputOutputDevice.readByteIndex == inputOutputDevice.readByteCount);
    assert(inputOutputDevice.readPorts[0] == 0x0020u);
    assert(inputOutputDevice.readPorts[1] == 0x0020u);
    assert(inputOutputDevice.readPorts[2] == 0x0020u);
    assert(inputOutputDevice.readPorts[3] == 0x0021u);
    assert(inputOutputDevice.readPorts[4] == 0x0020u);
    assert(inputOutputDevice.readPorts[5] == 0x0021u);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER) == 0u);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX) == 0x020Cu);

    assert(hyperdos_x86_read_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0x0200u, &value) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(value == 0xAAu);
    assert(hyperdos_x86_read_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0x0201u, &value) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(value == 0xBBu);
    assert(hyperdos_x86_read_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0x0200u, &value) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(value != 0xAAu);
    assert(hyperdos_x86_read_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0x0210u, &value) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(value == 0x34u);
    assert(hyperdos_x86_read_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0x0211u, &value) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(value == 0x12u);
    assert(hyperdos_x86_read_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0x020Eu, &value) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(value == 0x78u);
    assert(hyperdos_x86_read_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0x020Fu, &value) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(value == 0x56u);
    free(memory);
}

static void test_80186_output_string_for_model(hyperdos_x86_processor_model processorModel)
{
    static const uint8_t program[] = {
        0xB8u, 0x00u, 0x20u, 0x8Eu, 0xC0u, 0xBAu, 0x30u, 0x00u, 0xBEu, 0x00u, 0x02u, 0xB9u, 0x03u, 0x00u,
        0x26u, 0xF3u, 0x6Eu, 0xFDu, 0xBEu, 0x10u, 0x02u, 0xB9u, 0x02u, 0x00u, 0x26u, 0xF3u, 0x6Fu, 0xF4u,
    };
    uint8_t*                        memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor          processor;
    hyperdos_bus                    bus;
    hyperdos_random_access_memory   randomAccessMemory;
    test_string_input_output_device inputOutputDevice;
    hyperdos_x86_execution_result   result = HYPERDOS_X86_EXECUTION_OK;

    assert(memory != NULL);
    memset(&inputOutputDevice, 0, sizeof(inputOutputDevice));
    hyperdos_bus_initialize(&bus);
    hyperdos_random_access_memory_initialize(&randomAccessMemory, memory, HYPERDOS_X86_MEMORY_SIZE, 0u);
    assert(hyperdos_bus_map_memory(&bus,
                                   0u,
                                   HYPERDOS_X86_MEMORY_SIZE,
                                   &randomAccessMemory,
                                   hyperdos_random_access_memory_read_byte,
                                   hyperdos_random_access_memory_write_byte) == HYPERDOS_BUS_ACCESS_OK);
    assert(hyperdos_bus_map_input_output(&bus,
                                         0x0030u,
                                         2u,
                                         &inputOutputDevice,
                                         test_string_input_output_read_byte,
                                         test_string_input_output_write_byte) == HYPERDOS_BUS_ACCESS_OK);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    hyperdos_x86_attach_bus(&processor, &bus);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0x2000u);

    assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0x0200u, 0xA1u) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0x0201u, 0xA2u) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0x0202u, 0xA3u) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0x0210u, 0x44u) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0x0211u, 0x33u) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0x020Eu, 0x66u) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0x020Fu, 0x55u) ==
           HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(inputOutputDevice.writtenByteCount == 7u);
    assert(inputOutputDevice.writtenPorts[0] == 0x0030u && inputOutputDevice.writtenBytes[0] == 0xA1u);
    assert(inputOutputDevice.writtenPorts[1] == 0x0030u && inputOutputDevice.writtenBytes[1] == 0xA2u);
    assert(inputOutputDevice.writtenPorts[2] == 0x0030u && inputOutputDevice.writtenBytes[2] == 0xA3u);
    assert(inputOutputDevice.writtenPorts[3] == 0x0030u && inputOutputDevice.writtenBytes[3] == 0x44u);
    assert(inputOutputDevice.writtenPorts[4] == 0x0031u && inputOutputDevice.writtenBytes[4] == 0x33u);
    assert(inputOutputDevice.writtenPorts[5] == 0x0030u && inputOutputDevice.writtenBytes[5] == 0x66u);
    assert(inputOutputDevice.writtenPorts[6] == 0x0031u && inputOutputDevice.writtenBytes[6] == 0x55u);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER) == 0u);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX) == 0x020Cu);
    free(memory);
}

static void test_80186_input_output_string_instructions(void)
{
    test_80186_input_string_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80186);
    test_80186_input_string_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80188);
    test_80186_input_string_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80286);
    test_80186_output_string_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80186);
    test_80186_output_string_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80188);
    test_80186_output_string_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80286);
}

static void test_install_offset_capture_interrupt_handler(uint8_t* memory, uint8_t interruptNumber)
{
    static const uint8_t interruptHandler[] = {
        0x8Bu,
        0xECu,
        0x8Bu,
        0x46u,
        0x00u,
        0xF4u,
    };
    enum
    {
        TEST_OFFSET_CAPTURE_INTERRUPT_VECTOR_OFFSET  = 0x0550u,
        TEST_OFFSET_CAPTURE_INTERRUPT_VECTOR_SEGMENT = 0x5678u
    };
    uint32_t vectorAddress  = (uint32_t)interruptNumber * 4u;
    uint32_t handlerAddress = ((uint32_t)TEST_OFFSET_CAPTURE_INTERRUPT_VECTOR_SEGMENT << HYPERDOS_X86_SEGMENT_SHIFT) +
                              TEST_OFFSET_CAPTURE_INTERRUPT_VECTOR_OFFSET;
    size_t byteIndex = 0u;

    memory[vectorAddress]      = (uint8_t)((uint16_t)TEST_OFFSET_CAPTURE_INTERRUPT_VECTOR_OFFSET & 0x00FFu);
    memory[vectorAddress + 1u] = (uint8_t)((uint16_t)TEST_OFFSET_CAPTURE_INTERRUPT_VECTOR_OFFSET >> 8u);
    memory[vectorAddress + 2u] = (uint8_t)((uint16_t)TEST_OFFSET_CAPTURE_INTERRUPT_VECTOR_SEGMENT & 0x00FFu);
    memory[vectorAddress + 3u] = (uint8_t)((uint16_t)TEST_OFFSET_CAPTURE_INTERRUPT_VECTOR_SEGMENT >> 8u);
    for (byteIndex = 0u; byteIndex < sizeof(interruptHandler); ++byteIndex)
    {
        memory[handlerAddress + byteIndex] = interruptHandler[byteIndex];
    }
}

static void test_fault_interrupt_does_not_dispatch_single_step_for_program(hyperdos_x86_processor_model processorModel,
                                                                           uint8_t                      interruptNumber,
                                                                           const uint8_t*               program,
                                                                           size_t                       programSize,
                                                                           uint16_t expectedReturnOffset,
                                                                           int      escapeTrapEnabled)
{
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_load_dos_program(&processor, program, programSize, HYPERDOS_X86_DEFAULT_DOS_SEGMENT, "", 0u) ==
           HYPERDOS_X86_EXECUTION_OK);
    test_install_offset_capture_interrupt_handler(memory, interruptNumber);
    hyperdos_x86_set_escape_trap_enabled(&processor, escapeTrapEnabled);
    hyperdos_x86_set_flags_word(&processor, HYPERDOS_X86_FLAG_RESERVED | HYPERDOS_X86_FLAG_TRAP);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    if (result != HYPERDOS_X86_EXECUTION_HALTED)
    {
        fprintf(stderr,
                "fault interrupt single-step test failed for model %d interrupt %u with result %s\n",
                (int)processorModel,
                (unsigned int)interruptNumber,
                hyperdos_x86_execution_result_name(result));
    }
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           expectedReturnOffset);
    free(memory);
}

static void test_80186_fault_interrupts_do_not_dispatch_single_step_for_model(
        hyperdos_x86_processor_model processorModel)
{
    static const uint8_t unusedOperationCodeProgram[] = {
        0x63u,
        0xF4u,
    };
    static const uint8_t escapeTrapProgram[] = {
        0xD8u,
        0xC0u,
        0xF4u,
    };

    test_fault_interrupt_does_not_dispatch_single_step_for_program(processorModel,
                                                                   6u,
                                                                   unusedOperationCodeProgram,
                                                                   sizeof(unusedOperationCodeProgram),
                                                                   HYPERDOS_X86_DOS_PROGRAM_OFFSET,
                                                                   0);
    test_fault_interrupt_does_not_dispatch_single_step_for_program(processorModel,
                                                                   7u,
                                                                   escapeTrapProgram,
                                                                   sizeof(escapeTrapProgram),
                                                                   HYPERDOS_X86_DOS_PROGRAM_OFFSET,
                                                                   1);
}

static void test_80186_bound_fault_interrupt_does_not_dispatch_single_step_for_model(
        hyperdos_x86_processor_model processorModel)
{
    static const uint8_t boundFailureProgram[] = {
        0x62u,
        0x06u,
        0x00u,
        0x02u,
        0xF4u,
    };
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         boundFailureProgram,
                                         sizeof(boundFailureProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    test_install_offset_capture_interrupt_handler(memory, 5u);
    hyperdos_x86_set_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, (uint16_t)(0xFFFDu));
    assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0x0200u, 0xFEu) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0x0201u, 0xFFu) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0x0202u, 0x03u) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0x0203u, 0x00u) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_flags_word(&processor, HYPERDOS_X86_FLAG_RESERVED | HYPERDOS_X86_FLAG_TRAP);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           HYPERDOS_X86_DOS_PROGRAM_OFFSET);
    free(memory);
}

static void test_80186_fault_interrupts_do_not_dispatch_single_step(void)
{
    static const uint8_t unusedOperationCodeProgram[] = {
        0x63u,
        0xF4u,
    };

    test_80186_fault_interrupts_do_not_dispatch_single_step_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80186);
    test_80186_fault_interrupts_do_not_dispatch_single_step_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80188);
    test_fault_interrupt_does_not_dispatch_single_step_for_program(HYPERDOS_X86_PROCESSOR_MODEL_80286,
                                                                   6u,
                                                                   unusedOperationCodeProgram,
                                                                   sizeof(unusedOperationCodeProgram),
                                                                   HYPERDOS_X86_DOS_PROGRAM_OFFSET,
                                                                   0);
    test_80186_bound_fault_interrupt_does_not_dispatch_single_step_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80186);
    test_80186_bound_fault_interrupt_does_not_dispatch_single_step_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80188);
    test_80186_bound_fault_interrupt_does_not_dispatch_single_step_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80286);
}

static void test_8086_pop_code_segment_operation_code_for_model(hyperdos_x86_processor_model processorModel)
{
    static const uint8_t program[] = {
        0xB8u,
        0x34u,
        0x12u,
        0x50u,
        0x0Fu,
        0xF4u,
    };
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result                = HYPERDOS_X86_EXECUTION_OK;
    uint32_t                      targetPhysicalAddress = (((uint32_t)0x1234u << HYPERDOS_X86_SEGMENT_SHIFT) +
                                      HYPERDOS_X86_DOS_PROGRAM_OFFSET + 5u) &
                                     HYPERDOS_X86_ADDRESS_MASK;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    memory[targetPhysicalAddress] = 0xF4u;

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE) == 0x1234u);
    free(memory);
}

static void test_8086_pop_code_segment_operation_code(void)
{
    test_8086_pop_code_segment_operation_code_for_model(HYPERDOS_X86_PROCESSOR_MODEL_8086);
    test_8086_pop_code_segment_operation_code_for_model(HYPERDOS_X86_PROCESSOR_MODEL_8088);
}

static void test_80186_unused_operation_code_interrupt_for_program(hyperdos_x86_processor_model processorModel,
                                                                   const uint8_t*               program,
                                                                   size_t                       programSize)
{
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_load_dos_program(&processor, program, programSize, HYPERDOS_X86_DEFAULT_DOS_SEGMENT, "", 0u) ==
           HYPERDOS_X86_EXECUTION_OK);
    test_install_offset_capture_interrupt_handler(memory, 6u);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           HYPERDOS_X86_DOS_PROGRAM_OFFSET);
    free(memory);
}

static void test_80186_unused_operation_code_interrupt_for_model(hyperdos_x86_processor_model processorModel)
{
    static const uint8_t operationCode0FProgram[]           = {0x0Fu, 0xF4u};
    static const uint8_t operationCode63Program[]           = {0x63u, 0xF4u};
    static const uint8_t operationCode64Program[]           = {0x64u, 0xF4u};
    static const uint8_t operationCode65Program[]           = {0x65u, 0xF4u};
    static const uint8_t operationCode66Program[]           = {0x66u, 0xF4u};
    static const uint8_t operationCode67Program[]           = {0x67u, 0xF4u};
    static const uint8_t operationCodeF1Program[]           = {0xF1u, 0xF4u};
    static const uint8_t operationCodeFEGroupSevenProgram[] = {0xFEu, 0xF8u, 0xF4u};
    static const uint8_t operationCodeFFGroupSevenProgram[] = {0xFFu, 0xF8u, 0xF4u};
    static const struct
    {
        const uint8_t* program;
        size_t         programSize;
    } programs[] = {
        {
         .program     = operationCode0FProgram,
         .programSize = sizeof(operationCode0FProgram),
         },
        {
         .program     = operationCode63Program,
         .programSize = sizeof(operationCode63Program),
         },
        {
         .program     = operationCode64Program,
         .programSize = sizeof(operationCode64Program),
         },
        {
         .program     = operationCode65Program,
         .programSize = sizeof(operationCode65Program),
         },
        {
         .program     = operationCode66Program,
         .programSize = sizeof(operationCode66Program),
         },
        {
         .program     = operationCode67Program,
         .programSize = sizeof(operationCode67Program),
         },
        {
         .program     = operationCodeF1Program,
         .programSize = sizeof(operationCodeF1Program),
         },
        {
         .program     = operationCodeFEGroupSevenProgram,
         .programSize = sizeof(operationCodeFEGroupSevenProgram),
         },
        {
         .program     = operationCodeFFGroupSevenProgram,
         .programSize = sizeof(operationCodeFFGroupSevenProgram),
         },
    };
    size_t programIndex = 0u;

    for (programIndex = 0u; programIndex < sizeof(programs) / sizeof(programs[0]); ++programIndex)
    {
        test_80186_unused_operation_code_interrupt_for_program(processorModel,
                                                               programs[programIndex].program,
                                                               programs[programIndex].programSize);
    }
}

static void test_80186_unused_operation_code_interrupt(void)
{
    test_80186_unused_operation_code_interrupt_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80186);
    test_80186_unused_operation_code_interrupt_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80188);
    test_80186_unused_operation_code_interrupt_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80286);
}

static void test_80186_escape_trap_interrupt_for_model(hyperdos_x86_processor_model processorModel)
{
    static const uint8_t escapeTrapProgram[] = {
        0x26u,
        0xD8u,
        0x06u,
        0x00u,
        0x02u,
        0xF4u,
    };
    static const uint8_t escapeTrapDisabledProgram[] = {
        0xD8u,
        0xC0u,
        0xF4u,
    };
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         escapeTrapDisabledProgram,
                                         sizeof(escapeTrapDisabledProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    free(memory);

    memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         escapeTrapProgram,
                                         sizeof(escapeTrapProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    test_install_offset_capture_interrupt_handler(memory, 7u);
    hyperdos_x86_set_escape_trap_enabled(&processor, 1);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           HYPERDOS_X86_DOS_PROGRAM_OFFSET);
    free(memory);
}

static void test_80186_escape_trap_precedes_coprocessor_handler_for_model(hyperdos_x86_processor_model processorModel)
{
    static const uint8_t escapeTrapProgram[] = {
        0xD8u,
        0xC0u,
        0xF4u,
    };
    uint8_t*                        memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor          processor;
    test_coprocessor_escape_context context;
    hyperdos_x86_execution_result   result = HYPERDOS_X86_EXECUTION_OK;

    memset(&context, 0, sizeof(context));
    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         escapeTrapProgram,
                                         sizeof(escapeTrapProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    test_install_offset_capture_interrupt_handler(memory, 7u);
    hyperdos_x86_attach_coprocessor(&processor, NULL, test_coprocessor_escape_records_call, &context);
    hyperdos_x86_set_escape_trap_enabled(&processor, 1);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(context.escapeCallCount == 0u);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           HYPERDOS_X86_DOS_PROGRAM_OFFSET);
    free(memory);
}

static void test_80186_escape_trap_precedes_coprocessor_handler(void)
{
    test_80186_escape_trap_precedes_coprocessor_handler_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80186);
    test_80186_escape_trap_precedes_coprocessor_handler_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80188);
}

static void test_80186_escape_trap_interrupt(void)
{
    test_80186_escape_trap_interrupt_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80186);
    test_80186_escape_trap_interrupt_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80188);
    test_80186_escape_trap_precedes_coprocessor_handler();
}

static void test_80186_relocation_register_escape_trap_bit_for_model(hyperdos_x86_processor_model processorModel)
{
    enum
    {
        TEST_80186_RELOCATION_REGISTER_RESET_VALUE = 0x20FFu,
        TEST_80186_RELOCATION_REGISTER_ESCAPE_TRAP = 0x4000u
    };
    static const uint8_t escapeTrapProgram[] = {
        0xD8u,
        0xC0u,
        0xF4u,
    };
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_get_relocation_register(&processor) == TEST_80186_RELOCATION_REGISTER_RESET_VALUE);

    hyperdos_x86_set_escape_trap_enabled(&processor, 1);
    assert((hyperdos_x86_get_relocation_register(&processor) & TEST_80186_RELOCATION_REGISTER_ESCAPE_TRAP) != 0u);
    hyperdos_x86_set_escape_trap_enabled(&processor, 0);
    assert((hyperdos_x86_get_relocation_register(&processor) & TEST_80186_RELOCATION_REGISTER_ESCAPE_TRAP) == 0u);

    assert(hyperdos_x86_load_dos_program(&processor,
                                         escapeTrapProgram,
                                         sizeof(escapeTrapProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    test_install_offset_capture_interrupt_handler(memory, 7u);
    hyperdos_x86_set_relocation_register(&processor,
                                         (uint16_t)(TEST_80186_RELOCATION_REGISTER_RESET_VALUE |
                                                    TEST_80186_RELOCATION_REGISTER_ESCAPE_TRAP));

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           HYPERDOS_X86_DOS_PROGRAM_OFFSET);
    free(memory);
}

static void test_80186_relocation_register_escape_trap_bit(void)
{
    test_80186_relocation_register_escape_trap_bit_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80186);
    test_80186_relocation_register_escape_trap_bit_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80188);
}

static void test_8086_flags_reserved_bits_are_fixed(void)
{
    static const uint8_t program[] = {
        0xB8u,
        0x28u,
        0x00u,
        0x50u,
        0x9Du,
        0x9Cu,
        0x58u,
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           HYPERDOS_X86_FLAG_RESERVED);
    free(memory);
}

static void test_80286_flags_high_bits_are_zero(void)
{
    static const uint8_t program[] = {
        0xB8u,
        0xD5u,
        0xF0u,
        0x50u,
        0x9Du,
        0x9Cu,
        0x58u,
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x00D7u);
    free(memory);
}

static void test_80286_real_mode_physical_address_uses_24_bits(void)
{
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    uint32_t                      physicalAddress = 0u;
    hyperdos_x86_execution_result result          = HYPERDOS_X86_EXECUTION_OK;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    hyperdos_x86_set_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0xFFFFu);

    result = hyperdos_x86_translate_logical_to_physical_address(&processor,
                                                                HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                                                0xFFFFu,
                                                                &physicalAddress);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert(physicalAddress == 0x10FFEFu);
    free(memory);
}

static void test_80286_descriptor_table_instructions(void)
{
    static const uint8_t program[] = {
        0xC7u, 0x06u, 0x00u, 0x02u, 0x34u, 0x12u, 0xC6u, 0x06u, 0x02u, 0x02u, 0x78u, 0xC6u, 0x06u, 0x03u, 0x02u,
        0x56u, 0xC6u, 0x06u, 0x04u, 0x02u, 0x34u, 0xC6u, 0x06u, 0x05u, 0x02u, 0xAAu, 0x0Fu, 0x01u, 0x16u, 0x00u,
        0x02u, 0x0Fu, 0x01u, 0x06u, 0x08u, 0x02u, 0xC7u, 0x06u, 0x10u, 0x02u, 0xBCu, 0x9Au, 0xC6u, 0x06u, 0x12u,
        0x02u, 0xF0u, 0xC6u, 0x06u, 0x13u, 0x02u, 0xDEu, 0xC6u, 0x06u, 0x14u, 0x02u, 0xBCu, 0xC6u, 0x06u, 0x15u,
        0x02u, 0x11u, 0x0Fu, 0x01u, 0x1Eu, 0x10u, 0x02u, 0x0Fu, 0x01u, 0x0Eu, 0x18u, 0x02u, 0xF4u,
    };
    static const uint8_t   expectedGlobalDescriptorBytes[]    = {0x34u, 0x12u, 0x78u, 0x56u, 0x34u, 0xFFu};
    static const uint8_t   expectedInterruptDescriptorBytes[] = {0xBCu, 0x9Au, 0xF0u, 0xDEu, 0xBCu, 0xFFu};
    uint8_t*               memory                             = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    size_t                 byteIndex = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    for (byteIndex = 0u; byteIndex < sizeof(expectedGlobalDescriptorBytes); ++byteIndex)
    {
        uint8_t byteValue = 0u;
        assert(hyperdos_x86_read_memory_byte(&processor,
                                             HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                             (uint16_t)(0x0208u + byteIndex),
                                             &byteValue) == HYPERDOS_X86_EXECUTION_OK);
        assert(byteValue == expectedGlobalDescriptorBytes[byteIndex]);
    }
    for (byteIndex = 0u; byteIndex < sizeof(expectedInterruptDescriptorBytes); ++byteIndex)
    {
        uint8_t byteValue = 0u;
        assert(hyperdos_x86_read_memory_byte(&processor,
                                             HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                             (uint16_t)(0x0218u + byteIndex),
                                             &byteValue) == HYPERDOS_X86_EXECUTION_OK);
        assert(byteValue == expectedInterruptDescriptorBytes[byteIndex]);
    }
    free(memory);
}

static void test_80286_descriptor_table_instruction_segment_override(void)
{
    static const uint8_t program[] = {
        0x26u,
        0x0Fu,
        0x01u,
        0x16u,
        0x00u,
        0x02u,
        0x0Fu,
        0x01u,
        0x06u,
        0x08u,
        0x02u,
        0xF4u,
    };
    static const uint8_t   sourceDescriptorBytes[]   = {0x78u, 0x56u, 0x34u, 0x12u, 0xF0u, 0x00u};
    static const uint8_t   expectedDescriptorBytes[] = {0x78u, 0x56u, 0x34u, 0x12u, 0xF0u, 0xFFu};
    uint8_t*               memory                    = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    size_t                 byteIndex = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0x2000u);
    for (byteIndex = 0u; byteIndex < sizeof(sourceDescriptorBytes); ++byteIndex)
    {
        assert(hyperdos_x86_write_memory_byte(&processor,
                                              HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                              (uint16_t)(0x0200u + byteIndex),
                                              sourceDescriptorBytes[byteIndex]) == HYPERDOS_X86_EXECUTION_OK);
    }

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    for (byteIndex = 0u; byteIndex < sizeof(expectedDescriptorBytes); ++byteIndex)
    {
        uint8_t byteValue = 0u;
        assert(hyperdos_x86_read_memory_byte(&processor,
                                             HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                             (uint16_t)(0x0208u + byteIndex),
                                             &byteValue) == HYPERDOS_X86_EXECUTION_OK);
        assert(byteValue == expectedDescriptorBytes[byteIndex]);
    }
    free(memory);
}

static void test_80286_machine_status_word_instructions(void)
{
    static const uint8_t program[] = {
        0xB8u,
        0x0Eu,
        0x00u,
        0x0Fu,
        0x01u,
        0xF0u,
        0x0Fu,
        0x06u,
        0x0Fu,
        0x01u,
        0xE3u,
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_BASE) == 0x0006u);
    free(memory);
}

static void test_write_80286_segment_descriptor(uint8_t* memory,
                                                uint32_t descriptorAddress,
                                                uint32_t baseAddress,
                                                uint16_t limit,
                                                uint8_t  access)
{
    memory[descriptorAddress + 0u] = (uint8_t)(limit & HYPERDOS_X86_BYTE_MASK);
    memory[descriptorAddress + 1u] = (uint8_t)(limit >> HYPERDOS_X86_BYTE_BIT_COUNT);
    memory[descriptorAddress + 2u] = (uint8_t)(baseAddress & HYPERDOS_X86_BYTE_MASK);
    memory[descriptorAddress + 3u] = (uint8_t)((baseAddress >> HYPERDOS_X86_BYTE_BIT_COUNT) & HYPERDOS_X86_BYTE_MASK);
    memory[descriptorAddress + 4u] = (uint8_t)((baseAddress >> HYPERDOS_X86_WORD_BIT_COUNT) & HYPERDOS_X86_BYTE_MASK);
    memory[descriptorAddress + 5u] = access;
}

static void test_write_80286_interrupt_gate(uint8_t* memory,
                                            uint32_t gateAddress,
                                            uint16_t targetOffset,
                                            uint16_t targetSelector,
                                            uint8_t  access)
{
    memory[gateAddress + 0u] = (uint8_t)(targetOffset & HYPERDOS_X86_BYTE_MASK);
    memory[gateAddress + 1u] = (uint8_t)(targetOffset >> HYPERDOS_X86_BYTE_BIT_COUNT);
    memory[gateAddress + 2u] = (uint8_t)(targetSelector & HYPERDOS_X86_BYTE_MASK);
    memory[gateAddress + 3u] = (uint8_t)(targetSelector >> HYPERDOS_X86_BYTE_BIT_COUNT);
    memory[gateAddress + 5u] = access;
}

static void test_write_80286_call_gate(uint8_t* memory,
                                       uint32_t gateAddress,
                                       uint16_t targetOffset,
                                       uint16_t targetSelector,
                                       uint8_t  parameterWordCount,
                                       uint8_t  access)
{
    memory[gateAddress + 0u] = (uint8_t)(targetOffset & HYPERDOS_X86_BYTE_MASK);
    memory[gateAddress + 1u] = (uint8_t)(targetOffset >> HYPERDOS_X86_BYTE_BIT_COUNT);
    memory[gateAddress + 2u] = (uint8_t)(targetSelector & HYPERDOS_X86_BYTE_MASK);
    memory[gateAddress + 3u] = (uint8_t)(targetSelector >> HYPERDOS_X86_BYTE_BIT_COUNT);
    memory[gateAddress + 4u] = parameterWordCount;
    memory[gateAddress + 5u] = access;
}

static void test_write_80286_task_gate(uint8_t* memory, uint32_t gateAddress, uint16_t targetSelector, uint8_t access)
{
    memory[gateAddress + 2u] = (uint8_t)(targetSelector & HYPERDOS_X86_BYTE_MASK);
    memory[gateAddress + 3u] = (uint8_t)(targetSelector >> HYPERDOS_X86_BYTE_BIT_COUNT);
    memory[gateAddress + 5u] = access;
}

static void test_write_80286_task_state_word(uint8_t* memory,
                                             uint32_t taskStateSegmentAddress,
                                             uint16_t taskStateSegmentOffset,
                                             uint16_t value)
{
    memory[taskStateSegmentAddress + taskStateSegmentOffset]      = (uint8_t)(value & HYPERDOS_X86_BYTE_MASK);
    memory[taskStateSegmentAddress + taskStateSegmentOffset + 1u] = (uint8_t)(value >> HYPERDOS_X86_BYTE_BIT_COUNT);
}

static void test_write_80286_minimal_task_state(uint8_t* memory,
                                                uint32_t taskStateSegmentAddress,
                                                uint16_t instructionPointer,
                                                uint16_t flags,
                                                uint16_t stackPointer,
                                                uint16_t codeSelector,
                                                uint16_t stackSelector,
                                                uint16_t dataSelector)
{
    test_write_80286_task_state_word(memory, taskStateSegmentAddress, 0x000Eu, instructionPointer);
    test_write_80286_task_state_word(memory, taskStateSegmentAddress, 0x0010u, flags);
    test_write_80286_task_state_word(memory, taskStateSegmentAddress, 0x001Au, stackPointer);
    test_write_80286_task_state_word(memory, taskStateSegmentAddress, 0x0022u, 0u);
    test_write_80286_task_state_word(memory, taskStateSegmentAddress, 0x0024u, codeSelector);
    test_write_80286_task_state_word(memory, taskStateSegmentAddress, 0x0026u, stackSelector);
    test_write_80286_task_state_word(memory, taskStateSegmentAddress, 0x0028u, dataSelector);
    test_write_80286_task_state_word(memory, taskStateSegmentAddress, 0x002Au, 0u);
}

static uint16_t test_read_memory_word(const hyperdos_x86_processor*       processor,
                                      hyperdos_x86_segment_register_index segmentRegister,
                                      uint16_t                            offset);

static void test_80286_machine_status_word_can_enter_protected_mode(void)
{
    static const uint8_t program[] = {
        0xB8u,
        0x01u,
        0x00u,
        0x0Fu,
        0x01u,
        0xF0u,
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert((processor.machineStatusWord & 0x0001u) != 0u);
    free(memory);
}

static void test_80286_protected_mode_segment_descriptor_loads(void)
{
    enum
    {
        TEST_GLOBAL_DESCRIPTOR_TABLE_BASE    = 0x0300u,
        TEST_DATA_DESCRIPTOR_ADDRESS         = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0008u,
        TEST_DATA_DESCRIPTOR_BASE            = 0x1234u,
        TEST_DATA_DESCRIPTOR_LIMIT           = 0x000Fu,
        TEST_DATA_DESCRIPTOR_ACCESS          = 0x92u,
        TEST_DATA_DESCRIPTOR_ACCESSED_ACCESS = 0x93u,
        TEST_DATA_SELECTOR                   = 0x0008u
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint32_t               linearAddress = 0u;
    uint8_t                byteValue     = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    hyperdos_x86_set_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE, 0u);
    processor.globalDescriptorTable.base  = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE;
    processor.globalDescriptorTable.limit = 0x000Fu;
    processor.machineStatusWord           = 0x0001u;
    test_write_80286_segment_descriptor(memory,
                                        TEST_DATA_DESCRIPTOR_ADDRESS,
                                        TEST_DATA_DESCRIPTOR_BASE,
                                        TEST_DATA_DESCRIPTOR_LIMIT,
                                        TEST_DATA_DESCRIPTOR_ACCESS);

    hyperdos_x86_set_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, TEST_DATA_SELECTOR);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA) == TEST_DATA_SELECTOR);
    assert(hyperdos_x86_get_segment_base(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA) == TEST_DATA_DESCRIPTOR_BASE);
    assert(hyperdos_x86_get_segment_limit(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA) ==
           TEST_DATA_DESCRIPTOR_LIMIT);
    assert(hyperdos_x86_get_segment_attributes(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA) ==
           TEST_DATA_DESCRIPTOR_ACCESSED_ACCESS);
    assert(memory[TEST_DATA_DESCRIPTOR_ADDRESS + 5u] == TEST_DATA_DESCRIPTOR_ACCESSED_ACCESS);

    assert(hyperdos_x86_translate_logical_to_linear_address(&processor,
                                                            HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                                            TEST_DATA_DESCRIPTOR_LIMIT,
                                                            &linearAddress) == HYPERDOS_X86_EXECUTION_OK);
    assert(linearAddress == TEST_DATA_DESCRIPTOR_BASE + TEST_DATA_DESCRIPTOR_LIMIT);
    assert(hyperdos_x86_translate_logical_to_linear_address(&processor,
                                                            HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                                            TEST_DATA_DESCRIPTOR_LIMIT + 1u,
                                                            &linearAddress) ==
           HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION);
    assert(hyperdos_x86_write_memory_byte(&processor,
                                          HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                          TEST_DATA_DESCRIPTOR_LIMIT,
                                          0x5Au) == HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_read_memory_byte(&processor,
                                         HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                         TEST_DATA_DESCRIPTOR_LIMIT,
                                         &byteValue) == HYPERDOS_X86_EXECUTION_OK);
    assert(byteValue == 0x5Au);
    assert(hyperdos_x86_write_memory_byte(&processor,
                                          HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                          TEST_DATA_DESCRIPTOR_LIMIT + 1u,
                                          0xA5u) == HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION);
    free(memory);
}

static void test_80286_protected_mode_interrupt_gate_dispatch(void)
{
    enum
    {
        TEST_GLOBAL_DESCRIPTOR_TABLE_BASE    = 0x0300u,
        TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE = 0x0400u,
        TEST_CODE_DESCRIPTOR_ADDRESS         = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0008u,
        TEST_INTERRUPT_GATE_ADDRESS          = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE + 3u * 8u,
        TEST_PROTECTED_CODE_SELECTOR         = 0x0008u,
        TEST_PROTECTED_HANDLER_OFFSET        = 0x0200u,
        TEST_CODE_DESCRIPTOR_ACCESS          = 0x9Au,
        TEST_INTERRUPT_GATE_ACCESS           = 0x86u
    };
    static const uint8_t program[] = {
        0xB8u,
        0x01u,
        0x00u,
        0x0Fu,
        0x01u,
        0xF0u,
        0xCCu,
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor,
                                          HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                          TEST_PROTECTED_HANDLER_OFFSET,
                                          0xF4u) == HYPERDOS_X86_EXECUTION_OK);

    processor.globalDescriptorTable.base     = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE;
    processor.globalDescriptorTable.limit    = 0x000Fu;
    processor.interruptDescriptorTable.base  = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE;
    processor.interruptDescriptorTable.limit = 3u * 8u + 7u;
    test_write_80286_segment_descriptor(memory,
                                        TEST_CODE_DESCRIPTOR_ADDRESS,
                                        HYPERDOS_X86_DEFAULT_DOS_SEGMENT << HYPERDOS_X86_SEGMENT_SHIFT,
                                        HYPERDOS_X86_WORD_MASK,
                                        TEST_CODE_DESCRIPTOR_ACCESS);
    test_write_80286_interrupt_gate(memory,
                                    TEST_INTERRUPT_GATE_ADDRESS,
                                    TEST_PROTECTED_HANDLER_OFFSET,
                                    TEST_PROTECTED_CODE_SELECTOR,
                                    TEST_INTERRUPT_GATE_ACCESS);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE) ==
           TEST_PROTECTED_CODE_SELECTOR);
    assert(hyperdos_x86_get_instruction_pointer_word(&processor) == TEST_PROTECTED_HANDLER_OFFSET + 1u);
    assert((hyperdos_x86_get_flags_word(&processor) & HYPERDOS_X86_FLAG_INTERRUPT_ENABLE) == 0u);
    free(memory);
}

static void test_80286_protected_system_instructions(void)
{
    enum
    {
        TEST_GLOBAL_DESCRIPTOR_TABLE_BASE   = 0x0300u,
        TEST_DATA_DESCRIPTOR_ADDRESS        = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0008u,
        TEST_LOCAL_TABLE_DESCRIPTOR_ADDRESS = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0010u,
        TEST_TASK_DESCRIPTOR_ADDRESS        = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0018u,
        TEST_DATA_SELECTOR                  = 0x0008u,
        TEST_LOCAL_TABLE_SELECTOR           = 0x0010u,
        TEST_TASK_SELECTOR                  = 0x0018u,
        TEST_DATA_DESCRIPTOR_LIMIT          = 0x0042u,
        TEST_DATA_DESCRIPTOR_ACCESS         = 0x92u,
        TEST_LOCAL_TABLE_DESCRIPTOR_ACCESS  = 0x82u,
        TEST_TASK_DESCRIPTOR_ACCESS         = 0x81u
    };
    static const uint8_t program[] = {
        0xB8u, 0x01u, 0x00u, 0x0Fu, 0x01u, 0xF0u, 0xB8u, 0x10u, 0x00u, 0x0Fu, 0x00u, 0xD0u, 0xB8u, 0x18u, 0x00u, 0x0Fu,
        0x00u, 0xD8u, 0x0Fu, 0x00u, 0xC0u, 0x89u, 0x06u, 0x00u, 0x02u, 0x0Fu, 0x00u, 0xC8u, 0x89u, 0x06u, 0x02u, 0x02u,
        0xB8u, 0x08u, 0x00u, 0x0Fu, 0x00u, 0xE0u, 0x9Cu, 0x58u, 0x89u, 0x06u, 0x04u, 0x02u, 0xB8u, 0x08u, 0x00u, 0x0Fu,
        0x00u, 0xE8u, 0x9Cu, 0x58u, 0x89u, 0x06u, 0x06u, 0x02u, 0xB8u, 0x08u, 0x00u, 0x0Fu, 0x02u, 0xC0u, 0x89u, 0x06u,
        0x08u, 0x02u, 0xB8u, 0x08u, 0x00u, 0x0Fu, 0x03u, 0xC0u, 0x89u, 0x06u, 0x0Au, 0x02u, 0xB8u, 0x08u, 0x00u, 0xBBu,
        0x0Bu, 0x00u, 0x63u, 0xD8u, 0x89u, 0x06u, 0x0Cu, 0x02u, 0x9Cu, 0x58u, 0x89u, 0x06u, 0x0Eu, 0x02u, 0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    processor.globalDescriptorTable.base  = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE;
    processor.globalDescriptorTable.limit = 0x001Fu;
    test_write_80286_segment_descriptor(memory,
                                        TEST_DATA_DESCRIPTOR_ADDRESS,
                                        0x1234u,
                                        TEST_DATA_DESCRIPTOR_LIMIT,
                                        TEST_DATA_DESCRIPTOR_ACCESS);
    test_write_80286_segment_descriptor(memory,
                                        TEST_LOCAL_TABLE_DESCRIPTOR_ADDRESS,
                                        0x0500u,
                                        0x0017u,
                                        TEST_LOCAL_TABLE_DESCRIPTOR_ACCESS);
    test_write_80286_segment_descriptor(memory,
                                        TEST_TASK_DESCRIPTOR_ADDRESS,
                                        0x0600u,
                                        0x002Bu,
                                        TEST_TASK_DESCRIPTOR_ACCESS);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0x0200u) == TEST_LOCAL_TABLE_SELECTOR);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0x0202u) == TEST_TASK_SELECTOR);
    assert((test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0x0204u) & HYPERDOS_X86_FLAG_ZERO) !=
           0u);
    assert((test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0x0206u) & HYPERDOS_X86_FLAG_ZERO) !=
           0u);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0x0208u) ==
           (uint16_t)(TEST_DATA_DESCRIPTOR_ACCESS << HYPERDOS_X86_BYTE_BIT_COUNT));
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0x020Au) ==
           TEST_DATA_DESCRIPTOR_LIMIT);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0x020Cu) == 0x000Bu);
    assert((test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0x020Eu) & HYPERDOS_X86_FLAG_ZERO) !=
           0u);
    assert(memory[TEST_TASK_DESCRIPTOR_ADDRESS + 5u] == 0x83u);
    free(memory);
}

static void test_prepare_80286_protected_execution_state(hyperdos_x86_processor* processor,
                                                         uint8_t*                memory,
                                                         uint16_t                codeSelector,
                                                         uint8_t                 codeAccess,
                                                         uint16_t                stackSelector,
                                                         uint8_t                 stackAccess)
{
    enum
    {
        TEST_GLOBAL_DESCRIPTOR_TABLE_BASE = 0x0300u,
        TEST_CODE_DESCRIPTOR_ADDRESS      = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0008u,
        TEST_STACK_DESCRIPTOR_ADDRESS     = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0010u,
        TEST_CODE_BASE                    = 0x2000u,
        TEST_STACK_BASE                   = 0x4000u
    };

    processor->globalDescriptorTable.base  = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE;
    processor->globalDescriptorTable.limit = 0x0017u;
    test_write_80286_segment_descriptor(memory,
                                        TEST_CODE_DESCRIPTOR_ADDRESS,
                                        TEST_CODE_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        codeAccess);
    test_write_80286_segment_descriptor(memory,
                                        TEST_STACK_DESCRIPTOR_ADDRESS,
                                        TEST_STACK_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        stackAccess);
    processor->machineStatusWord                                             = 0x0001u;
    processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].selector    = codeSelector;
    processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].base        = TEST_CODE_BASE;
    processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].limit       = HYPERDOS_X86_WORD_MASK;
    processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].attributes  = codeAccess;
    processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].selector   = stackSelector;
    processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].base       = TEST_STACK_BASE;
    processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].limit      = HYPERDOS_X86_WORD_MASK;
    processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].attributes = stackAccess;
    hyperdos_x86_set_instruction_pointer_word(processor, 0u);
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER, 0x0800u);
}

static void test_80286_protected_mode_pop_flags_preserves_system_bits_at_privilege_zero(void)
{
    enum
    {
        TEST_CODE_BASE      = 0x2000u,
        TEST_CODE_SELECTOR  = 0x0008u,
        TEST_STACK_SELECTOR = 0x0010u
    };
    static const uint8_t program[] = {
        0xB8u,
        0xD5u,
        0x70u,
        0x50u,
        0x9Du,
        0x9Cu,
        0x58u,
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    test_prepare_80286_protected_execution_state(&processor,
                                                 memory,
                                                 TEST_CODE_SELECTOR,
                                                 0x9Au,
                                                 TEST_STACK_SELECTOR,
                                                 0x92u);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x70D7u);
    free(memory);
}

static void test_80286_direct_far_jump_uses_protected_code_descriptor(void)
{
    enum
    {
        TEST_CODE_BASE            = 0x2000u,
        TEST_SOURCE_CODE_SELECTOR = 0x0008u,
        TEST_STACK_SELECTOR       = 0x0010u,
        TEST_TARGET_CODE_SELECTOR = 0x0018u,
        TEST_TARGET_DESCRIPTOR    = 0x0300u + 0x0018u,
        TEST_TARGET_OFFSET        = 0x0100u
    };
    static const uint8_t program[] = {
        0xEAu,
        (uint8_t)(TEST_TARGET_OFFSET & 0x00FFu),
        (uint8_t)(TEST_TARGET_OFFSET / 0x0100u),
        (uint8_t)(TEST_TARGET_CODE_SELECTOR & 0x00FFu),
        (uint8_t)(TEST_TARGET_CODE_SELECTOR / 0x0100u),
    };
    static const uint8_t handlerProgram[] = {
        0xB8u,
        0x34u,
        0x12u,
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    memcpy(memory + TEST_CODE_BASE + TEST_TARGET_OFFSET, handlerProgram, sizeof(handlerProgram));
    test_prepare_80286_protected_execution_state(&processor,
                                                 memory,
                                                 TEST_SOURCE_CODE_SELECTOR,
                                                 0x9Au,
                                                 TEST_STACK_SELECTOR,
                                                 0x92u);
    processor.globalDescriptorTable.limit = 0x001Fu;
    test_write_80286_segment_descriptor(memory, TEST_TARGET_DESCRIPTOR, TEST_CODE_BASE, HYPERDOS_X86_WORD_MASK, 0x9Au);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE) ==
           TEST_TARGET_CODE_SELECTOR);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x1234u);
    free(memory);
}

static void test_80286_direct_far_call_pushes_protected_return_address(void)
{
    enum
    {
        TEST_CODE_BASE            = 0x2000u,
        TEST_SOURCE_CODE_SELECTOR = 0x0008u,
        TEST_STACK_SELECTOR       = 0x0010u,
        TEST_TARGET_CODE_SELECTOR = 0x0018u,
        TEST_TARGET_DESCRIPTOR    = 0x0300u + 0x0018u,
        TEST_TARGET_OFFSET        = 0x0100u,
        TEST_RETURN_OFFSET        = 0x0005u
    };
    static const uint8_t program[] = {
        0x9Au,
        (uint8_t)(TEST_TARGET_OFFSET & 0x00FFu),
        (uint8_t)(TEST_TARGET_OFFSET / 0x0100u),
        (uint8_t)(TEST_TARGET_CODE_SELECTOR & 0x00FFu),
        (uint8_t)(TEST_TARGET_CODE_SELECTOR / 0x0100u),
    };
    static const uint8_t handlerProgram[] = {
        0xB8u,
        0x78u,
        0x56u,
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint16_t               stackPointer = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    memcpy(memory + TEST_CODE_BASE + TEST_TARGET_OFFSET, handlerProgram, sizeof(handlerProgram));
    test_prepare_80286_protected_execution_state(&processor,
                                                 memory,
                                                 TEST_SOURCE_CODE_SELECTOR,
                                                 0x9Au,
                                                 TEST_STACK_SELECTOR,
                                                 0x92u);
    processor.globalDescriptorTable.limit = 0x001Fu;
    test_write_80286_segment_descriptor(memory, TEST_TARGET_DESCRIPTOR, TEST_CODE_BASE, HYPERDOS_X86_WORD_MASK, 0x9Au);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x5678u);
    stackPointer = hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, stackPointer) == TEST_RETURN_OFFSET);
    assert(test_read_memory_word(&processor,
                                 HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                 (uint16_t)(stackPointer + HYPERDOS_X86_WORD_SIZE)) == TEST_SOURCE_CODE_SELECTOR);
    free(memory);
}

static void test_80286_call_gate_transfers_to_same_privilege_code(void)
{
    enum
    {
        TEST_CODE_BASE            = 0x2000u,
        TEST_CODE_SELECTOR        = 0x0008u,
        TEST_STACK_SELECTOR       = 0x0010u,
        TEST_CALL_GATE_SELECTOR   = 0x0018u,
        TEST_CALL_GATE_DESCRIPTOR = 0x0300u + 0x0018u,
        TEST_TARGET_OFFSET        = 0x0100u,
        TEST_RETURN_OFFSET        = 0x0005u
    };
    static const uint8_t program[] = {
        0x9Au,
        0x00u,
        0x00u,
        (uint8_t)(TEST_CALL_GATE_SELECTOR & 0x00FFu),
        (uint8_t)(TEST_CALL_GATE_SELECTOR / 0x0100u),
    };
    static const uint8_t handlerProgram[] = {
        0xB8u,
        0xADu,
        0xDEu,
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint16_t               stackPointer = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    memcpy(memory + TEST_CODE_BASE + TEST_TARGET_OFFSET, handlerProgram, sizeof(handlerProgram));
    test_prepare_80286_protected_execution_state(&processor,
                                                 memory,
                                                 TEST_CODE_SELECTOR,
                                                 0x9Au,
                                                 TEST_STACK_SELECTOR,
                                                 0x92u);
    processor.globalDescriptorTable.limit = 0x001Fu;
    test_write_80286_call_gate(memory, TEST_CALL_GATE_DESCRIPTOR, TEST_TARGET_OFFSET, TEST_CODE_SELECTOR, 0u, 0x84u);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0xDEADu);
    stackPointer = hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, stackPointer) == TEST_RETURN_OFFSET);
    assert(test_read_memory_word(&processor,
                                 HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                 (uint16_t)(stackPointer + HYPERDOS_X86_WORD_SIZE)) == TEST_CODE_SELECTOR);
    free(memory);
}

static void test_80286_io_privilege_violation_dispatches_general_protection_fault(void)
{
    enum
    {
        TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE = 0x0500u,
        TEST_CODE_BASE                       = 0x2000u,
        TEST_CODE_SELECTOR                   = 0x000Bu,
        TEST_STACK_SELECTOR                  = 0x0013u,
        TEST_HANDLER_OFFSET                  = 0x0100u,
        TEST_GENERAL_PROTECTION_GATE_ADDRESS = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE + 13u * 8u
    };
    static const uint8_t program[] = {
        0xFAu,
    };
    static const uint8_t handlerProgram[] = {
        0xB8u,
        0xCDu,
        0xABu,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint16_t               stackPointer = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    memcpy(memory + TEST_CODE_BASE + TEST_HANDLER_OFFSET, handlerProgram, sizeof(handlerProgram));
    test_prepare_80286_protected_execution_state(&processor,
                                                 memory,
                                                 TEST_CODE_SELECTOR,
                                                 0xFAu,
                                                 TEST_STACK_SELECTOR,
                                                 0xF2u);
    processor.interruptDescriptorTable.base  = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE;
    processor.interruptDescriptorTable.limit = 13u * 8u + 7u;
    test_write_80286_interrupt_gate(memory,
                                    TEST_GENERAL_PROTECTION_GATE_ADDRESS,
                                    TEST_HANDLER_OFFSET,
                                    TEST_CODE_SELECTOR,
                                    0xE6u);
    hyperdos_x86_set_flags_word(&processor, 0x0002u);

    assert(hyperdos_x86_execute(&processor, 2u) == HYPERDOS_X86_EXECUTION_STEP_LIMIT_REACHED);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0xABCDu);
    stackPointer = hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, stackPointer) == 0x0000u);
    free(memory);
}

static void test_80286_interrupt_to_inner_privilege_switches_stack(void)
{
    enum
    {
        TEST_GLOBAL_DESCRIPTOR_TABLE_BASE    = 0x0300u,
        TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE = 0x0500u,
        TEST_CODE_BASE                       = 0x2000u,
        TEST_RING_THREE_STACK_BASE           = 0x4000u,
        TEST_RING_ZERO_STACK_BASE            = 0x5000u,
        TEST_TASK_STATE_SEGMENT_BASE         = 0x0600u,
        TEST_RING_THREE_CODE_DESCRIPTOR      = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0008u,
        TEST_RING_THREE_STACK_DESCRIPTOR     = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0010u,
        TEST_RING_ZERO_CODE_DESCRIPTOR       = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0018u,
        TEST_RING_ZERO_STACK_DESCRIPTOR      = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0020u,
        TEST_TASK_STATE_SEGMENT_DESCRIPTOR   = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0028u,
        TEST_RING_THREE_CODE_SELECTOR        = 0x000Bu,
        TEST_RING_THREE_STACK_SELECTOR       = 0x0013u,
        TEST_RING_ZERO_CODE_SELECTOR         = 0x0018u,
        TEST_RING_ZERO_STACK_SELECTOR        = 0x0020u,
        TEST_TASK_STATE_SEGMENT_SELECTOR     = 0x0028u,
        TEST_RING_THREE_STACK_POINTER        = 0x0800u,
        TEST_RING_ZERO_STACK_POINTER         = 0x0900u,
        TEST_HANDLER_OFFSET                  = 0x0100u,
        TEST_GENERAL_PROTECTION_GATE_ADDRESS = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE + 13u * 8u
    };
    static const uint8_t program[] = {
        0xFAu,
    };
    static const uint8_t handlerProgram[] = {
        0xB8u,
        0x57u,
        0x13u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint16_t               stackPointer = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    memcpy(memory + TEST_CODE_BASE + TEST_HANDLER_OFFSET, handlerProgram, sizeof(handlerProgram));
    processor.globalDescriptorTable.base     = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE;
    processor.globalDescriptorTable.limit    = 0x002Fu;
    processor.interruptDescriptorTable.base  = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE;
    processor.interruptDescriptorTable.limit = 13u * 8u + 7u;
    processor.machineStatusWord              = 0x0001u;
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_THREE_CODE_DESCRIPTOR,
                                        TEST_CODE_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0xFAu);
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_THREE_STACK_DESCRIPTOR,
                                        TEST_RING_THREE_STACK_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0xF2u);
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_ZERO_CODE_DESCRIPTOR,
                                        TEST_CODE_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0x9Au);
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_ZERO_STACK_DESCRIPTOR,
                                        TEST_RING_ZERO_STACK_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0x92u);
    test_write_80286_segment_descriptor(memory,
                                        TEST_TASK_STATE_SEGMENT_DESCRIPTOR,
                                        TEST_TASK_STATE_SEGMENT_BASE,
                                        0x002Bu,
                                        0x81u);
    test_write_80286_interrupt_gate(memory,
                                    TEST_GENERAL_PROTECTION_GATE_ADDRESS,
                                    TEST_HANDLER_OFFSET,
                                    TEST_RING_ZERO_CODE_SELECTOR,
                                    0x86u);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 2u] = (uint8_t)(TEST_RING_ZERO_STACK_POINTER & 0x00FFu);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 3u] = (uint8_t)(TEST_RING_ZERO_STACK_POINTER / 0x0100u);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 4u] = (uint8_t)(TEST_RING_ZERO_STACK_SELECTOR & 0x00FFu);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 5u] = (uint8_t)(TEST_RING_ZERO_STACK_SELECTOR / 0x0100u);
    processor.taskRegisterSelector            = TEST_TASK_STATE_SEGMENT_SELECTOR;
    processor.taskRegister.selector           = TEST_TASK_STATE_SEGMENT_SELECTOR;
    processor.taskRegister.base               = TEST_TASK_STATE_SEGMENT_BASE;
    processor.taskRegister.limit              = 0x002Bu;
    processor.taskRegister.attributes         = 0x81u;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].selector    = TEST_RING_THREE_CODE_SELECTOR;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].base        = TEST_CODE_BASE;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].limit       = HYPERDOS_X86_WORD_MASK;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].attributes  = 0xFAu;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].selector   = TEST_RING_THREE_STACK_SELECTOR;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].base       = TEST_RING_THREE_STACK_BASE;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].limit      = HYPERDOS_X86_WORD_MASK;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].attributes = 0xF2u;
    hyperdos_x86_set_instruction_pointer_word(&processor, 0u);
    hyperdos_x86_set_general_register_word(&processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                           TEST_RING_THREE_STACK_POINTER);
    hyperdos_x86_set_flags_word(&processor, 0x0002u);

    assert(hyperdos_x86_execute(&processor, 2u) == HYPERDOS_X86_EXECUTION_STEP_LIMIT_REACHED);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x1357u);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE) ==
           TEST_RING_ZERO_CODE_SELECTOR);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK) ==
           TEST_RING_ZERO_STACK_SELECTOR);
    stackPointer = hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    assert(stackPointer == TEST_RING_ZERO_STACK_POINTER - 12u);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, stackPointer) == 0x0000u);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, (uint16_t)(stackPointer + 10u)) ==
           TEST_RING_THREE_STACK_SELECTOR);
    free(memory);
}

static void test_80286_call_gate_to_inner_privilege_switches_stack_and_copies_parameters(void)
{
    enum
    {
        TEST_GLOBAL_DESCRIPTOR_TABLE_BASE  = 0x0300u,
        TEST_CODE_BASE                     = 0x2000u,
        TEST_RING_THREE_STACK_BASE         = 0x4000u,
        TEST_RING_ZERO_STACK_BASE          = 0x5000u,
        TEST_TASK_STATE_SEGMENT_BASE       = 0x0600u,
        TEST_RING_THREE_CODE_DESCRIPTOR    = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0008u,
        TEST_RING_THREE_STACK_DESCRIPTOR   = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0010u,
        TEST_RING_ZERO_CODE_DESCRIPTOR     = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0018u,
        TEST_RING_ZERO_STACK_DESCRIPTOR    = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0020u,
        TEST_TASK_STATE_SEGMENT_DESCRIPTOR = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0028u,
        TEST_CALL_GATE_DESCRIPTOR          = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0030u,
        TEST_RING_THREE_CODE_SELECTOR      = 0x000Bu,
        TEST_RING_THREE_STACK_SELECTOR     = 0x0013u,
        TEST_RING_ZERO_CODE_SELECTOR       = 0x0018u,
        TEST_RING_ZERO_STACK_SELECTOR      = 0x0020u,
        TEST_TASK_STATE_SEGMENT_SELECTOR   = 0x0028u,
        TEST_CALL_GATE_SELECTOR            = 0x0033u,
        TEST_RING_THREE_STACK_POINTER      = 0x0800u,
        TEST_RING_ZERO_STACK_POINTER       = 0x0900u,
        TEST_HANDLER_OFFSET                = 0x0100u,
        TEST_RETURN_OFFSET                 = 0x000Bu
    };
    static const uint8_t program[] = {
        0x68u,
        0x11u,
        0x11u,
        0x68u,
        0x22u,
        0x22u,
        0x9Au,
        0x00u,
        0x00u,
        (uint8_t)(TEST_CALL_GATE_SELECTOR & 0x00FFu),
        (uint8_t)(TEST_CALL_GATE_SELECTOR / 0x0100u),
    };
    static const uint8_t handlerProgram[] = {
        0xB8u,
        0x9Au,
        0xBCu,
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint16_t               stackPointer = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    memcpy(memory + TEST_CODE_BASE + TEST_HANDLER_OFFSET, handlerProgram, sizeof(handlerProgram));
    processor.globalDescriptorTable.base  = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE;
    processor.globalDescriptorTable.limit = 0x0037u;
    processor.machineStatusWord           = 0x0001u;
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_THREE_CODE_DESCRIPTOR,
                                        TEST_CODE_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0xFAu);
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_THREE_STACK_DESCRIPTOR,
                                        TEST_RING_THREE_STACK_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0xF2u);
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_ZERO_CODE_DESCRIPTOR,
                                        TEST_CODE_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0x9Au);
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_ZERO_STACK_DESCRIPTOR,
                                        TEST_RING_ZERO_STACK_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0x92u);
    test_write_80286_segment_descriptor(memory,
                                        TEST_TASK_STATE_SEGMENT_DESCRIPTOR,
                                        TEST_TASK_STATE_SEGMENT_BASE,
                                        0x002Bu,
                                        0x81u);
    test_write_80286_call_gate(memory,
                               TEST_CALL_GATE_DESCRIPTOR,
                               TEST_HANDLER_OFFSET,
                               TEST_RING_ZERO_CODE_SELECTOR,
                               2u,
                               0xE4u);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 2u] = (uint8_t)(TEST_RING_ZERO_STACK_POINTER & 0x00FFu);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 3u] = (uint8_t)(TEST_RING_ZERO_STACK_POINTER / 0x0100u);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 4u] = (uint8_t)(TEST_RING_ZERO_STACK_SELECTOR & 0x00FFu);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 5u] = (uint8_t)(TEST_RING_ZERO_STACK_SELECTOR / 0x0100u);
    processor.taskRegisterSelector            = TEST_TASK_STATE_SEGMENT_SELECTOR;
    processor.taskRegister.selector           = TEST_TASK_STATE_SEGMENT_SELECTOR;
    processor.taskRegister.base               = TEST_TASK_STATE_SEGMENT_BASE;
    processor.taskRegister.limit              = 0x002Bu;
    processor.taskRegister.attributes         = 0x81u;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].selector    = TEST_RING_THREE_CODE_SELECTOR;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].base        = TEST_CODE_BASE;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].limit       = HYPERDOS_X86_WORD_MASK;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].attributes  = 0xFAu;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].selector   = TEST_RING_THREE_STACK_SELECTOR;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].base       = TEST_RING_THREE_STACK_BASE;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].limit      = HYPERDOS_X86_WORD_MASK;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].attributes = 0xF2u;
    hyperdos_x86_set_instruction_pointer_word(&processor, 0u);
    hyperdos_x86_set_general_register_word(&processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                           TEST_RING_THREE_STACK_POINTER);
    hyperdos_x86_set_flags_word(&processor, 0x0002u);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0xBC9Au);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE) ==
           TEST_RING_ZERO_CODE_SELECTOR);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK) ==
           TEST_RING_ZERO_STACK_SELECTOR);
    stackPointer = hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    assert(stackPointer == TEST_RING_ZERO_STACK_POINTER - 12u);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, stackPointer) == TEST_RETURN_OFFSET);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, (uint16_t)(stackPointer + 2u)) ==
           TEST_RING_THREE_CODE_SELECTOR);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, (uint16_t)(stackPointer + 4u)) ==
           0x2222u);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, (uint16_t)(stackPointer + 6u)) ==
           0x1111u);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, (uint16_t)(stackPointer + 8u)) ==
           TEST_RING_THREE_STACK_POINTER - 4u);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, (uint16_t)(stackPointer + 10u)) ==
           TEST_RING_THREE_STACK_SELECTOR);
    free(memory);
}

static void test_80286_far_return_to_same_privilege_uses_protected_descriptor(void)
{
    enum
    {
        TEST_CODE_BASE            = 0x2000u,
        TEST_CODE_SELECTOR        = 0x0008u,
        TEST_STACK_SELECTOR       = 0x0010u,
        TEST_TARGET_CODE_SELECTOR = 0x0018u,
        TEST_TARGET_DESCRIPTOR    = 0x0300u + 0x0018u,
        TEST_TARGET_OFFSET        = 0x0100u
    };
    static const uint8_t program[] = {
        0x9Au,
        (uint8_t)(TEST_TARGET_OFFSET & 0x00FFu),
        (uint8_t)(TEST_TARGET_OFFSET / 0x0100u),
        (uint8_t)(TEST_TARGET_CODE_SELECTOR & 0x00FFu),
        (uint8_t)(TEST_TARGET_CODE_SELECTOR / 0x0100u),
        0xB8u,
        0x34u,
        0x12u,
        0xF4u,
    };
    static const uint8_t handlerProgram[] = {
        0xB8u,
        0xADu,
        0xDEu,
        0xCBu,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    memcpy(memory + TEST_CODE_BASE + TEST_TARGET_OFFSET, handlerProgram, sizeof(handlerProgram));
    test_prepare_80286_protected_execution_state(&processor,
                                                 memory,
                                                 TEST_CODE_SELECTOR,
                                                 0x9Au,
                                                 TEST_STACK_SELECTOR,
                                                 0x92u);
    processor.globalDescriptorTable.limit = 0x001Fu;
    test_write_80286_segment_descriptor(memory, TEST_TARGET_DESCRIPTOR, TEST_CODE_BASE, HYPERDOS_X86_WORD_MASK, 0x9Au);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x1234u);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE) == TEST_CODE_SELECTOR);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER) == 0x0800u);
    free(memory);
}

static void test_80286_far_return_to_outer_privilege_restores_stack(void)
{
    enum
    {
        TEST_GLOBAL_DESCRIPTOR_TABLE_BASE  = 0x0300u,
        TEST_CODE_BASE                     = 0x2000u,
        TEST_RING_THREE_STACK_BASE         = 0x4000u,
        TEST_RING_ZERO_STACK_BASE          = 0x5000u,
        TEST_TASK_STATE_SEGMENT_BASE       = 0x0600u,
        TEST_RING_THREE_CODE_DESCRIPTOR    = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0008u,
        TEST_RING_THREE_STACK_DESCRIPTOR   = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0010u,
        TEST_RING_ZERO_CODE_DESCRIPTOR     = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0018u,
        TEST_RING_ZERO_STACK_DESCRIPTOR    = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0020u,
        TEST_TASK_STATE_SEGMENT_DESCRIPTOR = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0028u,
        TEST_CALL_GATE_DESCRIPTOR          = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0030u,
        TEST_RING_THREE_CODE_SELECTOR      = 0x000Bu,
        TEST_RING_THREE_STACK_SELECTOR     = 0x0013u,
        TEST_RING_ZERO_CODE_SELECTOR       = 0x0018u,
        TEST_RING_ZERO_STACK_SELECTOR      = 0x0020u,
        TEST_TASK_STATE_SEGMENT_SELECTOR   = 0x0028u,
        TEST_CALL_GATE_SELECTOR            = 0x0033u,
        TEST_RING_THREE_STACK_POINTER      = 0x0800u,
        TEST_RING_ZERO_STACK_POINTER       = 0x0900u,
        TEST_HANDLER_OFFSET                = 0x0100u
    };
    static const uint8_t program[] = {
        0x68u,
        0x11u,
        0x11u,
        0x68u,
        0x22u,
        0x22u,
        0x9Au,
        0x00u,
        0x00u,
        (uint8_t)(TEST_CALL_GATE_SELECTOR & 0x00FFu),
        (uint8_t)(TEST_CALL_GATE_SELECTOR / 0x0100u),
        0xB8u,
        0x68u,
        0x24u,
        0xF4u,
    };
    static const uint8_t handlerProgram[] = {
        0xCAu,
        0x04u,
        0x00u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    memcpy(memory + TEST_CODE_BASE + TEST_HANDLER_OFFSET, handlerProgram, sizeof(handlerProgram));
    processor.globalDescriptorTable.base  = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE;
    processor.globalDescriptorTable.limit = 0x0037u;
    processor.machineStatusWord           = 0x0001u;
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_THREE_CODE_DESCRIPTOR,
                                        TEST_CODE_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0xFAu);
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_THREE_STACK_DESCRIPTOR,
                                        TEST_RING_THREE_STACK_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0xF2u);
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_ZERO_CODE_DESCRIPTOR,
                                        TEST_CODE_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0x9Au);
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_ZERO_STACK_DESCRIPTOR,
                                        TEST_RING_ZERO_STACK_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0x92u);
    test_write_80286_segment_descriptor(memory,
                                        TEST_TASK_STATE_SEGMENT_DESCRIPTOR,
                                        TEST_TASK_STATE_SEGMENT_BASE,
                                        0x002Bu,
                                        0x81u);
    test_write_80286_call_gate(memory,
                               TEST_CALL_GATE_DESCRIPTOR,
                               TEST_HANDLER_OFFSET,
                               TEST_RING_ZERO_CODE_SELECTOR,
                               2u,
                               0xE4u);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 2u] = (uint8_t)(TEST_RING_ZERO_STACK_POINTER & 0x00FFu);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 3u] = (uint8_t)(TEST_RING_ZERO_STACK_POINTER / 0x0100u);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 4u] = (uint8_t)(TEST_RING_ZERO_STACK_SELECTOR & 0x00FFu);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 5u] = (uint8_t)(TEST_RING_ZERO_STACK_SELECTOR / 0x0100u);
    processor.taskRegisterSelector            = TEST_TASK_STATE_SEGMENT_SELECTOR;
    processor.taskRegister.selector           = TEST_TASK_STATE_SEGMENT_SELECTOR;
    processor.taskRegister.base               = TEST_TASK_STATE_SEGMENT_BASE;
    processor.taskRegister.limit              = 0x002Bu;
    processor.taskRegister.attributes         = 0x81u;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].selector    = TEST_RING_THREE_CODE_SELECTOR;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].base        = TEST_CODE_BASE;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].limit       = HYPERDOS_X86_WORD_MASK;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].attributes  = 0xFAu;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].selector   = TEST_RING_THREE_STACK_SELECTOR;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].base       = TEST_RING_THREE_STACK_BASE;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].limit      = HYPERDOS_X86_WORD_MASK;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].attributes = 0xF2u;
    hyperdos_x86_set_instruction_pointer_word(&processor, 0u);
    hyperdos_x86_set_general_register_word(&processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                           TEST_RING_THREE_STACK_POINTER);
    hyperdos_x86_set_flags_word(&processor, 0x0002u);

    assert(hyperdos_x86_execute(&processor, 5u) == HYPERDOS_X86_EXECUTION_STEP_LIMIT_REACHED);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x2468u);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE) ==
           TEST_RING_THREE_CODE_SELECTOR);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK) ==
           TEST_RING_THREE_STACK_SELECTOR);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER) ==
           TEST_RING_THREE_STACK_POINTER);
    free(memory);
}

static void test_80286_interrupt_return_to_outer_privilege_restores_stack(void)
{
    enum
    {
        TEST_GLOBAL_DESCRIPTOR_TABLE_BASE    = 0x0300u,
        TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE = 0x0500u,
        TEST_CODE_BASE                       = 0x2000u,
        TEST_RING_THREE_STACK_BASE           = 0x4000u,
        TEST_RING_ZERO_STACK_BASE            = 0x5000u,
        TEST_TASK_STATE_SEGMENT_BASE         = 0x0600u,
        TEST_RING_THREE_CODE_DESCRIPTOR      = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0008u,
        TEST_RING_THREE_STACK_DESCRIPTOR     = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0010u,
        TEST_RING_ZERO_CODE_DESCRIPTOR       = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0018u,
        TEST_RING_ZERO_STACK_DESCRIPTOR      = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0020u,
        TEST_TASK_STATE_SEGMENT_DESCRIPTOR   = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0028u,
        TEST_RING_THREE_CODE_SELECTOR        = 0x000Bu,
        TEST_RING_THREE_STACK_SELECTOR       = 0x0013u,
        TEST_RING_ZERO_CODE_SELECTOR         = 0x0018u,
        TEST_RING_ZERO_STACK_SELECTOR        = 0x0020u,
        TEST_TASK_STATE_SEGMENT_SELECTOR     = 0x0028u,
        TEST_RING_THREE_STACK_POINTER        = 0x0800u,
        TEST_RING_ZERO_STACK_POINTER         = 0x0900u,
        TEST_HANDLER_OFFSET                  = 0x0100u,
        TEST_SOFTWARE_INTERRUPT_NUMBER       = 0x30u,
        TEST_SOFTWARE_INTERRUPT_GATE_ADDRESS = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE +
                                               TEST_SOFTWARE_INTERRUPT_NUMBER * 8u
    };
    static const uint8_t program[] = {
        0xCDu,
        TEST_SOFTWARE_INTERRUPT_NUMBER,
        0xB8u,
        0x78u,
        0x56u,
        0xF4u,
    };
    static const uint8_t handlerProgram[] = {
        0xCFu,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    memcpy(memory + TEST_CODE_BASE + TEST_HANDLER_OFFSET, handlerProgram, sizeof(handlerProgram));
    processor.globalDescriptorTable.base     = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE;
    processor.globalDescriptorTable.limit    = 0x002Fu;
    processor.interruptDescriptorTable.base  = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE;
    processor.interruptDescriptorTable.limit = TEST_SOFTWARE_INTERRUPT_NUMBER * 8u + 7u;
    processor.machineStatusWord              = 0x0001u;
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_THREE_CODE_DESCRIPTOR,
                                        TEST_CODE_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0xFAu);
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_THREE_STACK_DESCRIPTOR,
                                        TEST_RING_THREE_STACK_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0xF2u);
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_ZERO_CODE_DESCRIPTOR,
                                        TEST_CODE_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0x9Au);
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_ZERO_STACK_DESCRIPTOR,
                                        TEST_RING_ZERO_STACK_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0x92u);
    test_write_80286_segment_descriptor(memory,
                                        TEST_TASK_STATE_SEGMENT_DESCRIPTOR,
                                        TEST_TASK_STATE_SEGMENT_BASE,
                                        0x002Bu,
                                        0x81u);
    test_write_80286_interrupt_gate(memory,
                                    TEST_SOFTWARE_INTERRUPT_GATE_ADDRESS,
                                    TEST_HANDLER_OFFSET,
                                    TEST_RING_ZERO_CODE_SELECTOR,
                                    0xE6u);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 2u] = (uint8_t)(TEST_RING_ZERO_STACK_POINTER & 0x00FFu);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 3u] = (uint8_t)(TEST_RING_ZERO_STACK_POINTER / 0x0100u);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 4u] = (uint8_t)(TEST_RING_ZERO_STACK_SELECTOR & 0x00FFu);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 5u] = (uint8_t)(TEST_RING_ZERO_STACK_SELECTOR / 0x0100u);
    processor.taskRegisterSelector            = TEST_TASK_STATE_SEGMENT_SELECTOR;
    processor.taskRegister.selector           = TEST_TASK_STATE_SEGMENT_SELECTOR;
    processor.taskRegister.base               = TEST_TASK_STATE_SEGMENT_BASE;
    processor.taskRegister.limit              = 0x002Bu;
    processor.taskRegister.attributes         = 0x81u;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].selector    = TEST_RING_THREE_CODE_SELECTOR;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].base        = TEST_CODE_BASE;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].limit       = HYPERDOS_X86_WORD_MASK;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].attributes  = 0xFAu;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].selector   = TEST_RING_THREE_STACK_SELECTOR;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].base       = TEST_RING_THREE_STACK_BASE;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].limit      = HYPERDOS_X86_WORD_MASK;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].attributes = 0xF2u;
    hyperdos_x86_set_instruction_pointer_word(&processor, 0u);
    hyperdos_x86_set_general_register_word(&processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                           TEST_RING_THREE_STACK_POINTER);
    hyperdos_x86_set_flags_word(&processor, 0x0002u);

    assert(hyperdos_x86_execute(&processor, 3u) == HYPERDOS_X86_EXECUTION_STEP_LIMIT_REACHED);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x5678u);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE) ==
           TEST_RING_THREE_CODE_SELECTOR);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK) ==
           TEST_RING_THREE_STACK_SELECTOR);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER) ==
           TEST_RING_THREE_STACK_POINTER);
    free(memory);
}

static void test_80286_far_jump_to_task_state_segment_switches_task(void)
{
    enum
    {
        TEST_GLOBAL_DESCRIPTOR_TABLE_BASE = 0x0300u,
        TEST_CODE_BASE                    = 0x2000u,
        TEST_STACK_BASE                   = 0x4000u,
        TEST_CURRENT_TASK_STATE_BASE      = 0x0600u,
        TEST_TARGET_TASK_STATE_BASE       = 0x0700u,
        TEST_CODE_DESCRIPTOR              = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0008u,
        TEST_STACK_DESCRIPTOR             = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0010u,
        TEST_CURRENT_TASK_DESCRIPTOR      = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0018u,
        TEST_TARGET_TASK_DESCRIPTOR       = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0020u,
        TEST_CODE_SELECTOR                = 0x0008u,
        TEST_STACK_SELECTOR               = 0x0010u,
        TEST_CURRENT_TASK_SELECTOR        = 0x0018u,
        TEST_TARGET_TASK_SELECTOR         = 0x0020u,
        TEST_TARGET_OFFSET                = 0x0100u,
        TEST_TARGET_STACK_POINTER         = 0x0900u
    };
    static const uint8_t program[] = {
        0xEAu,
        0x00u,
        0x00u,
        (uint8_t)(TEST_TARGET_TASK_SELECTOR & 0x00FFu),
        (uint8_t)(TEST_TARGET_TASK_SELECTOR / 0x0100u),
    };
    static const uint8_t targetProgram[] = {
        0xB8u,
        0x57u,
        0x13u,
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    memcpy(memory + TEST_CODE_BASE + TEST_TARGET_OFFSET, targetProgram, sizeof(targetProgram));
    test_prepare_80286_protected_execution_state(&processor,
                                                 memory,
                                                 TEST_CODE_SELECTOR,
                                                 0x9Au,
                                                 TEST_STACK_SELECTOR,
                                                 0x92u);
    processor.globalDescriptorTable.limit = 0x0027u;
    test_write_80286_segment_descriptor(memory,
                                        TEST_CURRENT_TASK_DESCRIPTOR,
                                        TEST_CURRENT_TASK_STATE_BASE,
                                        0x002Bu,
                                        0x83u);
    test_write_80286_segment_descriptor(memory,
                                        TEST_TARGET_TASK_DESCRIPTOR,
                                        TEST_TARGET_TASK_STATE_BASE,
                                        0x002Bu,
                                        0x81u);
    test_write_80286_minimal_task_state(memory,
                                        TEST_TARGET_TASK_STATE_BASE,
                                        TEST_TARGET_OFFSET,
                                        0x0002u,
                                        TEST_TARGET_STACK_POINTER,
                                        TEST_CODE_SELECTOR,
                                        TEST_STACK_SELECTOR,
                                        TEST_STACK_SELECTOR);
    processor.taskRegisterSelector                                        = TEST_CURRENT_TASK_SELECTOR;
    processor.taskRegister.selector                                       = TEST_CURRENT_TASK_SELECTOR;
    processor.taskRegister.base                                           = TEST_CURRENT_TASK_STATE_BASE;
    processor.taskRegister.limit                                          = 0x002Bu;
    processor.taskRegister.attributes                                     = 0x83u;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_EXTRA].selector = 0u;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_DATA].selector  = 0u;

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x1357u);
    assert(processor.taskRegisterSelector == TEST_TARGET_TASK_SELECTOR);
    assert(memory[TEST_CURRENT_TASK_DESCRIPTOR + 5u] == 0x81u);
    assert(memory[TEST_TARGET_TASK_DESCRIPTOR + 5u] == 0x83u);
    assert(memory[TEST_CURRENT_TASK_STATE_BASE + 0x000Eu] == 0x05u);
    free(memory);
}

static void test_80286_task_gate_call_and_nested_task_return(void)
{
    enum
    {
        TEST_GLOBAL_DESCRIPTOR_TABLE_BASE = 0x0300u,
        TEST_CODE_BASE                    = 0x2000u,
        TEST_STACK_BASE                   = 0x4000u,
        TEST_CURRENT_TASK_STATE_BASE      = 0x0600u,
        TEST_TARGET_TASK_STATE_BASE       = 0x0700u,
        TEST_CODE_DESCRIPTOR              = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0008u,
        TEST_STACK_DESCRIPTOR             = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0010u,
        TEST_CURRENT_TASK_DESCRIPTOR      = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0018u,
        TEST_TARGET_TASK_DESCRIPTOR       = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0020u,
        TEST_TASK_GATE_DESCRIPTOR         = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0028u,
        TEST_CODE_SELECTOR                = 0x0008u,
        TEST_STACK_SELECTOR               = 0x0010u,
        TEST_CURRENT_TASK_SELECTOR        = 0x0018u,
        TEST_TARGET_TASK_SELECTOR         = 0x0020u,
        TEST_TASK_GATE_SELECTOR           = 0x0028u,
        TEST_TARGET_OFFSET                = 0x0100u,
        TEST_TARGET_STACK_POINTER         = 0x0900u
    };
    static const uint8_t program[] = {
        0x9Au,
        0x00u,
        0x00u,
        (uint8_t)(TEST_TASK_GATE_SELECTOR & 0x00FFu),
        (uint8_t)(TEST_TASK_GATE_SELECTOR / 0x0100u),
        0xB8u,
        0xFEu,
        0xCAu,
        0xF4u,
    };
    static const uint8_t targetProgram[] = {
        0xCFu,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    memcpy(memory + TEST_CODE_BASE + TEST_TARGET_OFFSET, targetProgram, sizeof(targetProgram));
    test_prepare_80286_protected_execution_state(&processor,
                                                 memory,
                                                 TEST_CODE_SELECTOR,
                                                 0x9Au,
                                                 TEST_STACK_SELECTOR,
                                                 0x92u);
    processor.globalDescriptorTable.limit = 0x002Fu;
    test_write_80286_segment_descriptor(memory,
                                        TEST_CURRENT_TASK_DESCRIPTOR,
                                        TEST_CURRENT_TASK_STATE_BASE,
                                        0x002Bu,
                                        0x83u);
    test_write_80286_segment_descriptor(memory,
                                        TEST_TARGET_TASK_DESCRIPTOR,
                                        TEST_TARGET_TASK_STATE_BASE,
                                        0x002Bu,
                                        0x81u);
    test_write_80286_task_gate(memory, TEST_TASK_GATE_DESCRIPTOR, TEST_TARGET_TASK_SELECTOR, 0x85u);
    test_write_80286_minimal_task_state(memory,
                                        TEST_TARGET_TASK_STATE_BASE,
                                        TEST_TARGET_OFFSET,
                                        0x0002u,
                                        TEST_TARGET_STACK_POINTER,
                                        TEST_CODE_SELECTOR,
                                        TEST_STACK_SELECTOR,
                                        TEST_STACK_SELECTOR);
    processor.taskRegisterSelector                                        = TEST_CURRENT_TASK_SELECTOR;
    processor.taskRegister.selector                                       = TEST_CURRENT_TASK_SELECTOR;
    processor.taskRegister.base                                           = TEST_CURRENT_TASK_STATE_BASE;
    processor.taskRegister.limit                                          = 0x002Bu;
    processor.taskRegister.attributes                                     = 0x83u;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_EXTRA].selector = 0u;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_DATA].selector  = 0u;

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0xCAFEu);
    assert(processor.taskRegisterSelector == TEST_CURRENT_TASK_SELECTOR);
    assert(memory[TEST_CURRENT_TASK_DESCRIPTOR + 5u] == 0x83u);
    assert(memory[TEST_TARGET_TASK_DESCRIPTOR + 5u] == 0x81u);
    assert(memory[TEST_TARGET_TASK_STATE_BASE + 0x0000u] ==
           (uint8_t)((uint16_t)TEST_CURRENT_TASK_SELECTOR & HYPERDOS_X86_BYTE_MASK));
    assert(memory[TEST_CURRENT_TASK_STATE_BASE + 0x000Eu] == 0x05u);
    free(memory);
}

static void test_80286_software_interrupt_checks_gate_privilege(void)
{
    enum
    {
        TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE = 0x0500u,
        TEST_CODE_BASE                       = 0x2000u,
        TEST_CODE_SELECTOR                   = 0x000Bu,
        TEST_STACK_SELECTOR                  = 0x0013u,
        TEST_HANDLER_OFFSET                  = 0x0100u,
        TEST_SOFTWARE_INTERRUPT_NUMBER       = 0x30u,
        TEST_SOFTWARE_INTERRUPT_GATE_ADDRESS = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE +
                                               TEST_SOFTWARE_INTERRUPT_NUMBER * 8u,
        TEST_GENERAL_PROTECTION_GATE_ADDRESS = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE + 13u * 8u
    };
    static const uint8_t program[] = {
        0xCDu,
        TEST_SOFTWARE_INTERRUPT_NUMBER,
    };
    static const uint8_t handlerProgram[] = {
        0xB8u,
        0xEFu,
        0xBEu,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint16_t               stackPointer = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    memcpy(memory + TEST_CODE_BASE + TEST_HANDLER_OFFSET, handlerProgram, sizeof(handlerProgram));
    test_prepare_80286_protected_execution_state(&processor,
                                                 memory,
                                                 TEST_CODE_SELECTOR,
                                                 0xFAu,
                                                 TEST_STACK_SELECTOR,
                                                 0xF2u);
    processor.interruptDescriptorTable.base  = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE;
    processor.interruptDescriptorTable.limit = TEST_SOFTWARE_INTERRUPT_NUMBER * 8u + 7u;
    test_write_80286_interrupt_gate(memory, TEST_SOFTWARE_INTERRUPT_GATE_ADDRESS, 0x0200u, TEST_CODE_SELECTOR, 0x86u);
    test_write_80286_interrupt_gate(memory,
                                    TEST_GENERAL_PROTECTION_GATE_ADDRESS,
                                    TEST_HANDLER_OFFSET,
                                    TEST_CODE_SELECTOR,
                                    0xE6u);
    hyperdos_x86_set_flags_word(&processor, 0x0002u);

    assert(hyperdos_x86_execute(&processor, 2u) == HYPERDOS_X86_EXECUTION_STEP_LIMIT_REACHED);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0xBEEFu);
    stackPointer = hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, stackPointer) ==
           (uint16_t)(TEST_SOFTWARE_INTERRUPT_NUMBER * 8u + 2u));
    free(memory);
}

static void test_80286_protected_data_limit_fault_pushes_error_code(void)
{
    enum
    {
        TEST_GLOBAL_DESCRIPTOR_TABLE_BASE    = 0x0300u,
        TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE = 0x0400u,
        TEST_DATA_DESCRIPTOR_ADDRESS         = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0008u,
        TEST_CODE_DESCRIPTOR_ADDRESS         = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0010u,
        TEST_GENERAL_PROTECTION_GATE_ADDRESS = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE + 13u * 8u,
        TEST_DATA_SELECTOR                   = 0x0008u,
        TEST_CODE_SELECTOR                   = 0x0010u,
        TEST_HANDLER_OFFSET                  = 0x0300u
    };
    static const uint8_t program[] = {
        0xB8u,
        0x01u,
        0x00u,
        0x0Fu,
        0x01u,
        0xF0u,
        0xB8u,
        0x08u,
        0x00u,
        0x8Eu,
        0xD8u,
        0xA0u,
        0x02u,
        0x00u,
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint16_t               stackPointer = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, TEST_HANDLER_OFFSET, 0xF4u) ==
           HYPERDOS_X86_EXECUTION_OK);
    processor.globalDescriptorTable.base     = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE;
    processor.globalDescriptorTable.limit    = 0x0017u;
    processor.interruptDescriptorTable.base  = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE;
    processor.interruptDescriptorTable.limit = 13u * 8u + 7u;
    test_write_80286_segment_descriptor(memory,
                                        TEST_DATA_DESCRIPTOR_ADDRESS,
                                        HYPERDOS_X86_DEFAULT_DOS_SEGMENT << HYPERDOS_X86_SEGMENT_SHIFT,
                                        0x0001u,
                                        0x92u);
    test_write_80286_segment_descriptor(memory,
                                        TEST_CODE_DESCRIPTOR_ADDRESS,
                                        HYPERDOS_X86_DEFAULT_DOS_SEGMENT << HYPERDOS_X86_SEGMENT_SHIFT,
                                        HYPERDOS_X86_WORD_MASK,
                                        0x9Au);
    test_write_80286_interrupt_gate(memory,
                                    TEST_GENERAL_PROTECTION_GATE_ADDRESS,
                                    TEST_HANDLER_OFFSET,
                                    TEST_CODE_SELECTOR,
                                    0x86u);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    stackPointer = hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, stackPointer) == 0x0000u);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE) == TEST_CODE_SELECTOR);
    assert(hyperdos_x86_get_instruction_pointer_word(&processor) == TEST_HANDLER_OFFSET + 1u);
    free(memory);
}

static void test_80286_exception_delivery_fault_dispatches_double_fault(void)
{
    enum
    {
        TEST_GLOBAL_DESCRIPTOR_TABLE_BASE    = 0x0300u,
        TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE = 0x0500u,
        TEST_CODE_BASE                       = 0x2000u,
        TEST_RING_THREE_STACK_BASE           = 0x4000u,
        TEST_RING_ZERO_STACK_BASE            = 0x5000u,
        TEST_TASK_STATE_SEGMENT_BASE         = 0x0600u,
        TEST_RING_THREE_CODE_DESCRIPTOR      = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0008u,
        TEST_RING_THREE_STACK_DESCRIPTOR     = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0010u,
        TEST_RING_ZERO_CODE_DESCRIPTOR       = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0018u,
        TEST_RING_ZERO_STACK_DESCRIPTOR      = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0020u,
        TEST_TASK_STATE_SEGMENT_DESCRIPTOR   = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0028u,
        TEST_RING_THREE_CODE_SELECTOR        = 0x000Bu,
        TEST_RING_THREE_STACK_SELECTOR       = 0x0013u,
        TEST_RING_ZERO_CODE_SELECTOR         = 0x0018u,
        TEST_RING_ZERO_STACK_SELECTOR        = 0x0020u,
        TEST_TASK_STATE_SEGMENT_SELECTOR     = 0x0028u,
        TEST_RING_THREE_STACK_POINTER        = 0x0800u,
        TEST_RING_ZERO_STACK_POINTER         = 0x0900u,
        TEST_DOUBLE_FAULT_HANDLER_OFFSET     = 0x0100u,
        TEST_DOUBLE_FAULT_GATE_ADDRESS       = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE + 8u * 8u,
        TEST_GENERAL_PROTECTION_GATE_ADDRESS = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE + 13u * 8u
    };
    static const uint8_t program[] = {
        0xFAu,
    };
    static const uint8_t handlerProgram[] = {
        0xB8u,
        0xDFu,
        0xDFu,
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint16_t               stackPointer = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    memcpy(memory + TEST_CODE_BASE + TEST_DOUBLE_FAULT_HANDLER_OFFSET, handlerProgram, sizeof(handlerProgram));
    processor.globalDescriptorTable.base     = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE;
    processor.globalDescriptorTable.limit    = 0x002Fu;
    processor.interruptDescriptorTable.base  = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE;
    processor.interruptDescriptorTable.limit = 13u * 8u + 7u;
    processor.machineStatusWord              = 0x0001u;
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_THREE_CODE_DESCRIPTOR,
                                        TEST_CODE_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0xFAu);
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_THREE_STACK_DESCRIPTOR,
                                        TEST_RING_THREE_STACK_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0xF2u);
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_ZERO_CODE_DESCRIPTOR,
                                        TEST_CODE_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0x9Au);
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_ZERO_STACK_DESCRIPTOR,
                                        TEST_RING_ZERO_STACK_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0x92u);
    test_write_80286_segment_descriptor(memory,
                                        TEST_TASK_STATE_SEGMENT_DESCRIPTOR,
                                        TEST_TASK_STATE_SEGMENT_BASE,
                                        0x002Bu,
                                        0x81u);
    test_write_80286_interrupt_gate(memory, TEST_GENERAL_PROTECTION_GATE_ADDRESS, 0u, 0u, 0x86u);
    test_write_80286_interrupt_gate(memory,
                                    TEST_DOUBLE_FAULT_GATE_ADDRESS,
                                    TEST_DOUBLE_FAULT_HANDLER_OFFSET,
                                    TEST_RING_ZERO_CODE_SELECTOR,
                                    0x86u);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 2u] = (uint8_t)(TEST_RING_ZERO_STACK_POINTER & 0x00FFu);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 3u] = (uint8_t)(TEST_RING_ZERO_STACK_POINTER / 0x0100u);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 4u] = (uint8_t)(TEST_RING_ZERO_STACK_SELECTOR & 0x00FFu);
    memory[TEST_TASK_STATE_SEGMENT_BASE + 5u] = (uint8_t)(TEST_RING_ZERO_STACK_SELECTOR / 0x0100u);
    processor.taskRegisterSelector            = TEST_TASK_STATE_SEGMENT_SELECTOR;
    processor.taskRegister.selector           = TEST_TASK_STATE_SEGMENT_SELECTOR;
    processor.taskRegister.base               = TEST_TASK_STATE_SEGMENT_BASE;
    processor.taskRegister.limit              = 0x002Bu;
    processor.taskRegister.attributes         = 0x81u;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].selector    = TEST_RING_THREE_CODE_SELECTOR;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].base        = TEST_CODE_BASE;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].limit       = HYPERDOS_X86_WORD_MASK;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].attributes  = 0xFAu;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].selector   = TEST_RING_THREE_STACK_SELECTOR;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].base       = TEST_RING_THREE_STACK_BASE;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].limit      = HYPERDOS_X86_WORD_MASK;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].attributes = 0xF2u;
    hyperdos_x86_set_instruction_pointer_word(&processor, 0u);
    hyperdos_x86_set_general_register_word(&processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                           TEST_RING_THREE_STACK_POINTER);
    hyperdos_x86_set_flags_word(&processor, 0x0002u);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0xDFDFu);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE) ==
           TEST_RING_ZERO_CODE_SELECTOR);
    stackPointer = hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    assert(stackPointer == TEST_RING_ZERO_STACK_POINTER - 6u * HYPERDOS_X86_WORD_SIZE);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, stackPointer) == 0x0000u);
    free(memory);
}

static void test_80286_double_fault_delivery_fault_shuts_down_processor(void)
{
    enum
    {
        TEST_GLOBAL_DESCRIPTOR_TABLE_BASE    = 0x0300u,
        TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE = 0x0500u,
        TEST_CODE_BASE                       = 0x2000u,
        TEST_RING_THREE_STACK_BASE           = 0x4000u,
        TEST_RING_THREE_CODE_DESCRIPTOR      = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0008u,
        TEST_RING_THREE_STACK_DESCRIPTOR     = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE + 0x0010u,
        TEST_RING_THREE_CODE_SELECTOR        = 0x000Bu,
        TEST_RING_THREE_STACK_SELECTOR       = 0x0013u,
        TEST_RING_THREE_STACK_POINTER        = 0x0800u,
        TEST_GENERAL_PROTECTION_GATE_ADDRESS = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE + 13u * 8u
    };
    static const uint8_t program[] = {
        0xFAu,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    processor.globalDescriptorTable.base     = TEST_GLOBAL_DESCRIPTOR_TABLE_BASE;
    processor.globalDescriptorTable.limit    = 0x0017u;
    processor.interruptDescriptorTable.base  = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE;
    processor.interruptDescriptorTable.limit = 13u * 8u + 7u;
    processor.machineStatusWord              = 0x0001u;
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_THREE_CODE_DESCRIPTOR,
                                        TEST_CODE_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0xFAu);
    test_write_80286_segment_descriptor(memory,
                                        TEST_RING_THREE_STACK_DESCRIPTOR,
                                        TEST_RING_THREE_STACK_BASE,
                                        HYPERDOS_X86_WORD_MASK,
                                        0xF2u);
    test_write_80286_interrupt_gate(memory, TEST_GENERAL_PROTECTION_GATE_ADDRESS, 0u, 0u, 0x86u);
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].selector    = TEST_RING_THREE_CODE_SELECTOR;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].base        = TEST_CODE_BASE;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].limit       = HYPERDOS_X86_WORD_MASK;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].attributes  = 0xFAu;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].selector   = TEST_RING_THREE_STACK_SELECTOR;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].base       = TEST_RING_THREE_STACK_BASE;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].limit      = HYPERDOS_X86_WORD_MASK;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].attributes = 0xF2u;
    hyperdos_x86_set_instruction_pointer_word(&processor, 0u);
    hyperdos_x86_set_general_register_word(&processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                           TEST_RING_THREE_STACK_POINTER);
    hyperdos_x86_set_flags_word(&processor, 0x0002u);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) ==
           HYPERDOS_X86_EXECUTION_PROCESSOR_SHUTDOWN);
    assert(processor.processorShutdownActive != 0u);
    free(memory);
}

static void test_80286_not_present_segment_error_code_masks_requested_privilege_level(void)
{
    enum
    {
        TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE  = 0x0500u,
        TEST_CODE_BASE                        = 0x2000u,
        TEST_CODE_SELECTOR                    = 0x0008u,
        TEST_STACK_SELECTOR                   = 0x0010u,
        TEST_DATA_SELECTOR                    = 0x001Bu,
        TEST_DATA_DESCRIPTOR_ADDRESS          = 0x0300u + 0x0018u,
        TEST_HANDLER_OFFSET                   = 0x0100u,
        TEST_SEGMENT_NOT_PRESENT_GATE_ADDRESS = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE + 11u * 8u
    };
    static const uint8_t program[] = {
        0xB8u,
        (uint8_t)(TEST_DATA_SELECTOR & 0x00FFu),
        (uint8_t)(TEST_DATA_SELECTOR / 0x0100u),
        0x8Eu,
        0xD8u,
        0xF4u,
    };
    static const uint8_t handlerProgram[] = {
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint16_t               stackPointer = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    memcpy(memory + TEST_CODE_BASE + TEST_HANDLER_OFFSET, handlerProgram, sizeof(handlerProgram));
    test_prepare_80286_protected_execution_state(&processor,
                                                 memory,
                                                 TEST_CODE_SELECTOR,
                                                 0x9Au,
                                                 TEST_STACK_SELECTOR,
                                                 0x92u);
    processor.globalDescriptorTable.limit    = 0x001Fu;
    processor.interruptDescriptorTable.base  = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE;
    processor.interruptDescriptorTable.limit = 11u * 8u + 7u;
    test_write_80286_segment_descriptor(memory, TEST_DATA_DESCRIPTOR_ADDRESS, 0x6000u, HYPERDOS_X86_WORD_MASK, 0x12u);
    test_write_80286_interrupt_gate(memory,
                                    TEST_SEGMENT_NOT_PRESENT_GATE_ADDRESS,
                                    TEST_HANDLER_OFFSET,
                                    TEST_CODE_SELECTOR,
                                    0x86u);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    stackPointer = hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, stackPointer) == 0x0018u);
    free(memory);
}

static void test_80286_external_interrupt_descriptor_fault_sets_external_error_code(void)
{
    enum
    {
        TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE = 0x0500u,
        TEST_CODE_BASE                       = 0x2000u,
        TEST_CODE_SELECTOR                   = 0x0008u,
        TEST_STACK_SELECTOR                  = 0x0010u,
        TEST_EXTERNAL_INTERRUPT_NUMBER       = 0x30u,
        TEST_HANDLER_OFFSET                  = 0x0100u,
        TEST_EXTERNAL_INTERRUPT_GATE_ADDRESS = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE +
                                               TEST_EXTERNAL_INTERRUPT_NUMBER * 8u,
        TEST_SEGMENT_NOT_PRESENT_GATE_ADDRESS = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE + 11u * 8u,
        TEST_EXPECTED_ERROR_CODE              = TEST_EXTERNAL_INTERRUPT_NUMBER * 8u + 0x0003u
    };
    static const uint8_t program[] = {
        0x90u,
    };
    static const uint8_t handlerProgram[] = {
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint16_t               stackPointer = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    memcpy(memory + TEST_CODE_BASE + TEST_HANDLER_OFFSET, handlerProgram, sizeof(handlerProgram));
    test_prepare_80286_protected_execution_state(&processor,
                                                 memory,
                                                 TEST_CODE_SELECTOR,
                                                 0x9Au,
                                                 TEST_STACK_SELECTOR,
                                                 0x92u);
    processor.interruptDescriptorTable.base  = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE;
    processor.interruptDescriptorTable.limit = TEST_EXTERNAL_INTERRUPT_NUMBER * 8u + 7u;
    test_write_80286_interrupt_gate(memory, TEST_EXTERNAL_INTERRUPT_GATE_ADDRESS, 0u, TEST_CODE_SELECTOR, 0x06u);
    test_write_80286_interrupt_gate(memory,
                                    TEST_SEGMENT_NOT_PRESENT_GATE_ADDRESS,
                                    TEST_HANDLER_OFFSET,
                                    TEST_CODE_SELECTOR,
                                    0x86u);
    hyperdos_x86_set_flags_word(&processor, HYPERDOS_X86_FLAG_RESERVED | HYPERDOS_X86_FLAG_INTERRUPT_ENABLE);

    assert(hyperdos_x86_request_maskable_interrupt(&processor, TEST_EXTERNAL_INTERRUPT_NUMBER) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    stackPointer = hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, stackPointer) ==
           TEST_EXPECTED_ERROR_CODE);
    free(memory);
}

static void test_80286_direct_far_call_stack_fault_keeps_stack_pointer(void)
{
    enum
    {
        TEST_CODE_BASE            = 0x2000u,
        TEST_CODE_SELECTOR        = 0x0008u,
        TEST_STACK_SELECTOR       = 0x0010u,
        TEST_TARGET_CODE_SELECTOR = 0x0018u,
        TEST_TARGET_DESCRIPTOR    = 0x0300u + 0x0018u,
        TEST_TARGET_OFFSET        = 0x0100u,
        TEST_STACK_POINTER        = 0x0002u
    };
    static const uint8_t program[] = {
        0x9Au,
        (uint8_t)(TEST_TARGET_OFFSET & 0x00FFu),
        (uint8_t)(TEST_TARGET_OFFSET / 0x0100u),
        (uint8_t)(TEST_TARGET_CODE_SELECTOR & 0x00FFu),
        (uint8_t)(TEST_TARGET_CODE_SELECTOR / 0x0100u),
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    test_prepare_80286_protected_execution_state(&processor,
                                                 memory,
                                                 TEST_CODE_SELECTOR,
                                                 0x9Au,
                                                 TEST_STACK_SELECTOR,
                                                 0x92u);
    processor.globalDescriptorTable.limit                              = 0x001Fu;
    processor.segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].limit = 0x0003u;
    hyperdos_x86_set_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER, TEST_STACK_POINTER);
    test_write_80286_segment_descriptor(memory, TEST_TARGET_DESCRIPTOR, TEST_CODE_BASE, HYPERDOS_X86_WORD_MASK, 0x9Au);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) ==
           HYPERDOS_X86_EXECUTION_PROCESSOR_SHUTDOWN);
    assert(hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER) ==
           TEST_STACK_POINTER);
    free(memory);
}

static void test_80286_task_register_invalid_selector_dispatches_general_protection_fault(void)
{
    enum
    {
        TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE = 0x0500u,
        TEST_CODE_BASE                       = 0x2000u,
        TEST_CODE_SELECTOR                   = 0x0008u,
        TEST_STACK_SELECTOR                  = 0x0010u,
        TEST_HANDLER_OFFSET                  = 0x0100u,
        TEST_GENERAL_PROTECTION_GATE_ADDRESS = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE + 13u * 8u
    };
    static const uint8_t program[] = {
        0xB8u,
        (uint8_t)(TEST_STACK_SELECTOR & 0x00FFu),
        (uint8_t)(TEST_STACK_SELECTOR / 0x0100u),
        0x0Fu,
        0x00u,
        0xD8u,
        0xF4u,
    };
    static const uint8_t handlerProgram[] = {
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint16_t               stackPointer = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    memcpy(memory + TEST_CODE_BASE + TEST_HANDLER_OFFSET, handlerProgram, sizeof(handlerProgram));
    test_prepare_80286_protected_execution_state(&processor,
                                                 memory,
                                                 TEST_CODE_SELECTOR,
                                                 0x9Au,
                                                 TEST_STACK_SELECTOR,
                                                 0x92u);
    processor.interruptDescriptorTable.base  = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE;
    processor.interruptDescriptorTable.limit = 13u * 8u + 7u;
    test_write_80286_interrupt_gate(memory,
                                    TEST_GENERAL_PROTECTION_GATE_ADDRESS,
                                    TEST_HANDLER_OFFSET,
                                    TEST_CODE_SELECTOR,
                                    0x86u);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    stackPointer = hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, stackPointer) == TEST_STACK_SELECTOR);
    free(memory);
}

static void test_80286_local_descriptor_table_not_present_dispatches_segment_not_present_fault(void)
{
    enum
    {
        TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE   = 0x0500u,
        TEST_CODE_BASE                         = 0x2000u,
        TEST_CODE_SELECTOR                     = 0x0008u,
        TEST_STACK_SELECTOR                    = 0x0010u,
        TEST_LOCAL_DESCRIPTOR_TABLE_SELECTOR   = 0x0018u,
        TEST_LOCAL_DESCRIPTOR_TABLE_DESCRIPTOR = 0x0300u + 0x0018u,
        TEST_HANDLER_OFFSET                    = 0x0100u,
        TEST_SEGMENT_NOT_PRESENT_GATE_ADDRESS  = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE + 11u * 8u
    };
    static const uint8_t program[] = {
        0xB8u,
        (uint8_t)(TEST_LOCAL_DESCRIPTOR_TABLE_SELECTOR & 0x00FFu),
        (uint8_t)(TEST_LOCAL_DESCRIPTOR_TABLE_SELECTOR / 0x0100u),
        0x0Fu,
        0x00u,
        0xD0u,
        0xF4u,
    };
    static const uint8_t handlerProgram[] = {
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint16_t               stackPointer = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    memcpy(memory + TEST_CODE_BASE, program, sizeof(program));
    memcpy(memory + TEST_CODE_BASE + TEST_HANDLER_OFFSET, handlerProgram, sizeof(handlerProgram));
    test_prepare_80286_protected_execution_state(&processor,
                                                 memory,
                                                 TEST_CODE_SELECTOR,
                                                 0x9Au,
                                                 TEST_STACK_SELECTOR,
                                                 0x92u);
    processor.globalDescriptorTable.limit    = 0x001Fu;
    processor.interruptDescriptorTable.base  = TEST_INTERRUPT_DESCRIPTOR_TABLE_BASE;
    processor.interruptDescriptorTable.limit = 11u * 8u + 7u;
    test_write_80286_segment_descriptor(memory, TEST_LOCAL_DESCRIPTOR_TABLE_DESCRIPTOR, 0x6000u, 0x0017u, 0x02u);
    test_write_80286_interrupt_gate(memory,
                                    TEST_SEGMENT_NOT_PRESENT_GATE_ADDRESS,
                                    TEST_HANDLER_OFFSET,
                                    TEST_CODE_SELECTOR,
                                    0x86u);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    stackPointer = hyperdos_x86_get_general_register_word(&processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, stackPointer) ==
           TEST_LOCAL_DESCRIPTOR_TABLE_SELECTOR);
    free(memory);
}

static void test_80286_processor_extension_fault_for_program(uint16_t       machineStatusWord,
                                                             const uint8_t* program,
                                                             size_t         programSize)
{
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    assert(hyperdos_x86_load_dos_program(&processor, program, programSize, HYPERDOS_X86_DEFAULT_DOS_SEGMENT, "", 0u) ==
           HYPERDOS_X86_EXECUTION_OK);
    test_install_offset_capture_interrupt_handler(memory, 7u);
    processor.machineStatusWord = machineStatusWord;
    hyperdos_x86_set_flags_word(&processor, HYPERDOS_X86_FLAG_RESERVED | HYPERDOS_X86_FLAG_TRAP);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           HYPERDOS_X86_DOS_PROGRAM_OFFSET);
    free(memory);
}

static void test_80286_processor_extension_faults(void)
{
    enum
    {
        TEST_80286_MACHINE_STATUS_WORD_EMULATE_PROCESSOR             = 0x0004u,
        TEST_80286_MACHINE_STATUS_WORD_MONITOR_PROCESSOR_TASK_SWITCH = 0x000Au
    };
    static const uint8_t escapeTrapProgram[] = {
        0xD8u,
        0xC0u,
        0xF4u,
    };
    static const uint8_t waitTrapProgram[] = {
        0x9Bu,
        0xF4u,
    };

    test_80286_processor_extension_fault_for_program(TEST_80286_MACHINE_STATUS_WORD_EMULATE_PROCESSOR,
                                                     escapeTrapProgram,
                                                     sizeof(escapeTrapProgram));
    test_80286_processor_extension_fault_for_program(TEST_80286_MACHINE_STATUS_WORD_MONITOR_PROCESSOR_TASK_SWITCH,
                                                     waitTrapProgram,
                                                     sizeof(waitTrapProgram));
}

static void test_80286_processor_extension_trap_precedes_coprocessor_handler(void)
{
    enum
    {
        TEST_80286_MACHINE_STATUS_WORD_EMULATE_PROCESSOR = 0x0004u
    };
    static const uint8_t program[] = {
        0xD8u,
        0xC0u,
        0xF4u,
    };
    uint8_t*                        memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor          processor;
    test_coprocessor_escape_context context;

    assert(memory != NULL);
    memset(&context, 0, sizeof(context));
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    test_install_offset_capture_interrupt_handler(memory, 7u);
    processor.machineStatusWord = TEST_80286_MACHINE_STATUS_WORD_EMULATE_PROCESSOR;
    hyperdos_x86_attach_coprocessor(&processor, NULL, test_coprocessor_escape_records_call, &context);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(context.escapeCallCount == 0u);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           HYPERDOS_X86_DOS_PROGRAM_OFFSET);
    free(memory);
}

static void test_80286_wait_uses_coprocessor_when_task_switched_without_monitor_processor(void)
{
    enum
    {
        TEST_80286_MACHINE_STATUS_WORD_TASK_SWITCHED = 0x0008u
    };
    static const uint8_t program[] = {
        0x9Bu,
        0xF4u,
    };
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    test_coprocessor_wait_context context;

    assert(memory != NULL);
    memset(&context, 0, sizeof(context));
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    processor.machineStatusWord = TEST_80286_MACHINE_STATUS_WORD_TASK_SWITCHED;
    hyperdos_x86_attach_coprocessor(&processor, test_coprocessor_wait_records_call, NULL, &context);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(context.waitCallCount == 1u);
    free(memory);
}

static uint64_t test_external_bus_word_read_cycle_count_for_model(hyperdos_x86_processor_model processorModel)
{
    static const uint8_t program[] = {
        0xA1u,
        0x00u,
        0x02u,
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint64_t               externalBusCycleCount = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0x0200u, 0x34u) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0x0201u, 0x12u) ==
           HYPERDOS_X86_EXECUTION_OK);

    hyperdos_x86_reset_external_bus_cycle_count(&processor);
    assert(hyperdos_x86_get_external_bus_cycle_count(&processor) == 0u);
    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    externalBusCycleCount = hyperdos_x86_get_external_bus_cycle_count(&processor);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x1234u);
    free(memory);
    return externalBusCycleCount;
}

static void test_external_bus_cycle_count_profiles(void)
{
    uint64_t processor8086CycleCount = test_external_bus_word_read_cycle_count_for_model(
            HYPERDOS_X86_PROCESSOR_MODEL_8086);
    uint64_t processor8088CycleCount = test_external_bus_word_read_cycle_count_for_model(
            HYPERDOS_X86_PROCESSOR_MODEL_8088);
    uint64_t processor80186CycleCount = test_external_bus_word_read_cycle_count_for_model(
            HYPERDOS_X86_PROCESSOR_MODEL_80186);
    uint64_t processor80188CycleCount = test_external_bus_word_read_cycle_count_for_model(
            HYPERDOS_X86_PROCESSOR_MODEL_80188);

    assert(processor8086CycleCount < processor8088CycleCount);
    assert(processor80186CycleCount < processor80188CycleCount);
    assert(processor8086CycleCount == processor80186CycleCount);
    assert(processor8088CycleCount == processor80188CycleCount);
}

static void test_x86_general_register_word_operations_preserve_upper_word(void)
{
    static const uint8_t byteRegisterProgram[] = {
        0xB4u,
        0x9Au,
        0xB0u,
        0xBCu,
        0xF4u,
    };
    static const uint8_t wordRegisterProgram[] = {
        0xB8u,
        0x78u,
        0x56u,
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         byteRegisterProgram,
                                         sizeof(byteRegisterProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, 0x12340000u);
    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x12349ABCu);

    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         wordRegisterProgram,
                                         sizeof(wordRegisterProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, 0x12340000u);
    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x12345678u);
    free(memory);
}

static void test_load_data_segment_pointer(void)
{
    static const uint8_t program[] = {
        0xC5u,
        0x1Eu,
        0x20u,
        0x02u,
        0xF4u,
    };
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor,
                                          HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                          TEST_LOAD_SEGMENT_POINTER_MEMORY_OFFSET,
                                          (uint8_t)(TEST_LOAD_SEGMENT_POINTER_OFFSET_VALUE & TEST_BYTE_MASK)) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor,
                                          HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                          (uint16_t)(TEST_LOAD_SEGMENT_POINTER_MEMORY_OFFSET + 1u),
                                          (uint8_t)(TEST_LOAD_SEGMENT_POINTER_OFFSET_VALUE >>
                                                    TEST_DOS_SERVICE_REGISTER_SHIFT)) == HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor,
                                          HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                          (uint16_t)(TEST_LOAD_SEGMENT_POINTER_MEMORY_OFFSET + TEST_WORD_BYTE_COUNT),
                                          (uint8_t)(TEST_LOAD_SEGMENT_POINTER_SEGMENT_VALUE & TEST_BYTE_MASK)) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor,
                                          HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                          (uint16_t)(TEST_LOAD_SEGMENT_POINTER_MEMORY_OFFSET + TEST_WORD_BYTE_COUNT +
                                                     1u),
                                          (uint8_t)(TEST_LOAD_SEGMENT_POINTER_SEGMENT_VALUE >>
                                                    TEST_DOS_SERVICE_REGISTER_SHIFT)) == HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_BASE) ==
           TEST_LOAD_SEGMENT_POINTER_OFFSET_VALUE);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA) ==
           TEST_LOAD_SEGMENT_POINTER_SEGMENT_VALUE);
    free(memory);
}

static void test_repeated_byte_move(void)
{
    static const uint8_t program[] = {
        0xBEu,
        0x20u,
        0x01u,
        0xBFu,
        0x30u,
        0x01u,
        0xB9u,
        0x05u,
        0x00u,
        0xF3u,
        0xA4u,
        0xF4u,
    };
    static const uint8_t          sourceBytes[] = {'A', 'B', 'C', 'D', 'E'};
    uint8_t*                      memory        = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result    = HYPERDOS_X86_EXECUTION_OK;
    size_t                        byteIndex = 0;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    for (byteIndex = 0; byteIndex < sizeof(sourceBytes); ++byteIndex)
    {
        assert(hyperdos_x86_write_memory_byte(&processor,
                                              HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                              (uint16_t)(TEST_REPEATED_MOVE_SOURCE_OFFSET + byteIndex),
                                              sourceBytes[byteIndex]) == HYPERDOS_X86_EXECUTION_OK);
    }

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);

    for (byteIndex = 0; byteIndex < sizeof(sourceBytes); ++byteIndex)
    {
        uint8_t value = 0;
        assert(hyperdos_x86_read_memory_byte(&processor,
                                             HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                             (uint16_t)(TEST_REPEATED_MOVE_DESTINATION_OFFSET + byteIndex),
                                             &value) == HYPERDOS_X86_EXECUTION_OK);
        assert(value == sourceBytes[byteIndex]);
    }

    free(memory);
}

static void test_bus_connected_input_output(void)
{
    static const uint8_t program[] = {
        0xBAu,
        0x60u,
        0x00u,
        0xB0u,
        0x5Au,
        0xEEu,
        0xECu,
        0xF4u,
    };
    uint8_t*                                   memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor                     processor;
    hyperdos_bus                               bus;
    hyperdos_random_access_memory              randomAccessMemory;
    hyperdos_programmable_peripheral_interface peripheralInterface;
    hyperdos_x86_execution_result              result = HYPERDOS_X86_EXECUTION_OK;

    assert(memory != NULL);
    hyperdos_bus_initialize(&bus);
    hyperdos_random_access_memory_initialize(&randomAccessMemory, memory, HYPERDOS_X86_MEMORY_SIZE, 0u);
    hyperdos_programmable_peripheral_interface_initialize(&peripheralInterface);
    assert(hyperdos_bus_map_memory(&bus,
                                   0u,
                                   HYPERDOS_X86_MEMORY_SIZE,
                                   &randomAccessMemory,
                                   hyperdos_random_access_memory_read_byte,
                                   hyperdos_random_access_memory_write_byte) == HYPERDOS_BUS_ACCESS_OK);
    assert(hyperdos_bus_map_input_output(&bus,
                                         TEST_PERIPHERAL_INTERFACE_PORT,
                                         TEST_PERIPHERAL_INTERFACE_PORT_COUNT,
                                         &peripheralInterface,
                                         hyperdos_programmable_peripheral_interface_read_byte,
                                         hyperdos_programmable_peripheral_interface_write_byte) ==
           HYPERDOS_BUS_ACCESS_OK);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_attach_bus(&processor, &bus);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert((hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) &
            TEST_DOS_EXIT_CODE_MASK) == TEST_PERIPHERAL_INTERFACE_VALUE);
    assert(peripheralInterface.portA == TEST_PERIPHERAL_INTERFACE_VALUE);
    free(memory);
}

static void test_intel_8042_keyboard_controller(void)
{
    hyperdos_intel_8042_keyboard_controller controller;
    uint8_t                                 statusRegister = 0u;
    uint8_t                                 value          = 0u;

    hyperdos_intel_8042_keyboard_controller_initialize(&controller);
    assert(!hyperdos_intel_8042_keyboard_controller_has_interrupt_request(&controller));
    assert(hyperdos_intel_8042_keyboard_controller_receive_scan_code(&controller, TEST_KEYBOARD_SCAN_CODE_A));
    assert(hyperdos_intel_8042_keyboard_controller_can_receive_scan_code(&controller));
    assert(hyperdos_intel_8042_keyboard_controller_receive_scan_code(&controller, TEST_KEYBOARD_SCAN_CODE_B));
    assert(hyperdos_intel_8042_keyboard_controller_has_interrupt_request(&controller));
    hyperdos_intel_8042_keyboard_controller_clear_interrupt_request(&controller);
    assert(!hyperdos_intel_8042_keyboard_controller_has_interrupt_request(&controller));

    statusRegister = hyperdos_intel_8042_keyboard_controller_read_byte(&controller,
                                                                       TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT);
    assert((statusRegister & TEST_KEYBOARD_CONTROLLER_STATUS_OUTPUT_BUFFER_FULL) != 0u);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_KEYBOARD_SCAN_CODE_A);
    assert(hyperdos_intel_8042_keyboard_controller_has_interrupt_request(&controller));
    hyperdos_intel_8042_keyboard_controller_clear_interrupt_request(&controller);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_KEYBOARD_SCAN_CODE_B);
    statusRegister = hyperdos_intel_8042_keyboard_controller_read_byte(&controller,
                                                                       TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT);
    assert((statusRegister & TEST_KEYBOARD_CONTROLLER_STATUS_OUTPUT_BUFFER_FULL) == 0u);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_KEYBOARD_SCAN_CODE_B);

    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT,
                                                       TEST_KEYBOARD_CONTROLLER_COMMAND_READ_COMMAND_BYTE);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_KEYBOARD_CONTROLLER_COMMAND_BYTE_DEFAULT);

    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT,
                                                       TEST_KEYBOARD_CONTROLLER_COMMAND_DISABLE_KEYBOARD);
    assert(!hyperdos_intel_8042_keyboard_controller_receive_scan_code(&controller, TEST_KEYBOARD_SCAN_CODE_B));
    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT,
                                                       TEST_KEYBOARD_CONTROLLER_COMMAND_ENABLE_KEYBOARD);
    assert(hyperdos_intel_8042_keyboard_controller_receive_scan_code(&controller, TEST_KEYBOARD_SCAN_CODE_B));

    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_KEYBOARD_SCAN_CODE_B);

    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT,
                                                       TEST_KEYBOARD_CONTROLLER_COMMAND_WRITE_KEYBOARD_OUTPUT);
    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_DATA_PORT,
                                                       TEST_KEYBOARD_SCAN_CODE_A);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_KEYBOARD_SCAN_CODE_A);

    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_DATA_PORT,
                                                       TEST_KEYBOARD_DEVICE_COMMAND_SET_SCAN_CODE_SET);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
    hyperdos_intel_8042_keyboard_controller_write_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT, 0u);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == 1u);
}

static void test_intel_8042_auxiliary_mouse(void)
{
    hyperdos_intel_8042_keyboard_controller controller;
    uint8_t                                 commandByte    = 0u;
    uint8_t                                 statusRegister = 0u;
    uint8_t                                 value          = 0u;

    hyperdos_intel_8042_keyboard_controller_initialize(&controller);
    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT,
                                                       TEST_KEYBOARD_CONTROLLER_COMMAND_ENABLE_AUXILIARY);
    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT,
                                                       TEST_KEYBOARD_CONTROLLER_COMMAND_READ_COMMAND_BYTE);
    commandByte = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT,
                                                       TEST_KEYBOARD_CONTROLLER_COMMAND_WRITE_COMMAND_BYTE);
    hyperdos_intel_8042_keyboard_controller_write_byte(
            &controller,
            TEST_KEYBOARD_CONTROLLER_DATA_PORT,
            (uint8_t)(commandByte | TEST_KEYBOARD_CONTROLLER_COMMAND_BYTE_AUXILIARY_INTERRUPT_ENABLE));

    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT,
                                                       TEST_KEYBOARD_CONTROLLER_COMMAND_WRITE_AUXILIARY_DEVICE);
    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_DATA_PORT,
                                                       TEST_AUXILIARY_MOUSE_DEVICE_COMMAND_ENABLE_REPORTING);
    assert(hyperdos_intel_8042_keyboard_controller_has_auxiliary_device_interrupt_request(&controller));
    statusRegister = hyperdos_intel_8042_keyboard_controller_read_byte(&controller,
                                                                       TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT);
    assert((statusRegister & TEST_KEYBOARD_CONTROLLER_STATUS_OUTPUT_BUFFER_FULL) != 0u);
    assert((statusRegister & TEST_KEYBOARD_CONTROLLER_STATUS_AUXILIARY_OUTPUT_BUFFER) != 0u);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);

    assert(hyperdos_intel_8042_keyboard_controller_receive_auxiliary_mouse_packet(&controller,
                                                                                  5,
                                                                                  -3,
                                                                                  TEST_AUXILIARY_MOUSE_LEFT_BUTTON));
    assert(!hyperdos_intel_8042_keyboard_controller_receive_auxiliary_mouse_packet(&controller,
                                                                                   12,
                                                                                   7,
                                                                                   TEST_AUXILIARY_MOUSE_LEFT_BUTTON));
    statusRegister = hyperdos_intel_8042_keyboard_controller_read_byte(&controller,
                                                                       TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT);
    assert((statusRegister & TEST_KEYBOARD_CONTROLLER_STATUS_AUXILIARY_OUTPUT_BUFFER) != 0u);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == (TEST_AUXILIARY_MOUSE_PACKET_ALWAYS_ONE | TEST_AUXILIARY_MOUSE_PACKET_VERTICAL_SIGN |
                     TEST_AUXILIARY_MOUSE_LEFT_BUTTON));
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == 5u);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == 0xFDu);

    assert(hyperdos_intel_8042_keyboard_controller_receive_auxiliary_mouse_packet(&controller,
                                                                                  -3,
                                                                                  0,
                                                                                  TEST_AUXILIARY_MOUSE_LEFT_BUTTON));
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == (TEST_AUXILIARY_MOUSE_PACKET_ALWAYS_ONE | TEST_AUXILIARY_MOUSE_PACKET_HORIZONTAL_SIGN |
                     TEST_AUXILIARY_MOUSE_LEFT_BUTTON));
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == 0xFDu);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == 0u);
    assert(hyperdos_intel_8042_keyboard_controller_receive_auxiliary_mouse_packet(&controller,
                                                                                  4,
                                                                                  0,
                                                                                  TEST_AUXILIARY_MOUSE_LEFT_BUTTON));
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == (TEST_AUXILIARY_MOUSE_PACKET_ALWAYS_ONE | TEST_AUXILIARY_MOUSE_LEFT_BUTTON));
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == 4u);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == 0u);

    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT,
                                                       TEST_KEYBOARD_CONTROLLER_COMMAND_WRITE_AUXILIARY_DEVICE);
    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_DATA_PORT,
                                                       TEST_AUXILIARY_MOUSE_DEVICE_COMMAND_STATUS_REQUEST);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert((value & TEST_AUXILIARY_MOUSE_STATUS_LEFT_BUTTON) != 0u);
    assert((value & TEST_AUXILIARY_MOUSE_LEFT_BUTTON) == 0u);
    assert((value & TEST_AUXILIARY_MOUSE_STATUS_REPORTING_ENABLED) != 0u);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_AUXILIARY_MOUSE_DEFAULT_RESOLUTION);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_AUXILIARY_MOUSE_DEFAULT_SAMPLE_RATE);

    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT,
                                                       TEST_KEYBOARD_CONTROLLER_COMMAND_WRITE_AUXILIARY_DEVICE);
    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_DATA_PORT,
                                                       TEST_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_SCALING_TWO_TO_ONE);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT,
                                                       TEST_KEYBOARD_CONTROLLER_COMMAND_WRITE_AUXILIARY_DEVICE);
    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_DATA_PORT,
                                                       TEST_AUXILIARY_MOUSE_DEVICE_COMMAND_STATUS_REQUEST);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert((value & TEST_AUXILIARY_MOUSE_STATUS_SCALING_TWO_TO_ONE) != 0u);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_AUXILIARY_MOUSE_DEFAULT_RESOLUTION);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_AUXILIARY_MOUSE_DEFAULT_SAMPLE_RATE);

    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT,
                                                       TEST_KEYBOARD_CONTROLLER_COMMAND_WRITE_AUXILIARY_DEVICE);
    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_DATA_PORT,
                                                       TEST_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_SCALING_ONE_TO_ONE);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT,
                                                       TEST_KEYBOARD_CONTROLLER_COMMAND_WRITE_AUXILIARY_DEVICE);
    hyperdos_intel_8042_keyboard_controller_write_byte(&controller,
                                                       TEST_KEYBOARD_CONTROLLER_DATA_PORT,
                                                       TEST_AUXILIARY_MOUSE_DEVICE_COMMAND_STATUS_REQUEST);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert((value & TEST_AUXILIARY_MOUSE_STATUS_SCALING_TWO_TO_ONE) == 0u);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_AUXILIARY_MOUSE_DEFAULT_RESOLUTION);
    value = hyperdos_intel_8042_keyboard_controller_read_byte(&controller, TEST_KEYBOARD_CONTROLLER_DATA_PORT);
    assert(value == TEST_AUXILIARY_MOUSE_DEFAULT_SAMPLE_RATE);
}

static void test_cascaded_programmable_interrupt_controller(void)
{
    hyperdos_pc* pc              = NULL;
    uint8_t      interruptNumber = 0u;

    pc = (hyperdos_pc*)calloc(1u, sizeof(*pc));
    assert(pc != NULL);
    assert(hyperdos_pc_initialize(pc));
    pc->slaveProgrammableInterruptControllerEnabled                 = 1u;
    pc->programmableInterruptController.interruptMaskRegister      &= (uint8_t)~TEST_MASTER_CASCADE_INTERRUPT_MASK;
    pc->slaveProgrammableInterruptController.interruptMaskRegister &= (uint8_t)~TEST_AUXILIARY_DEVICE_INTERRUPT_MASK;
    hyperdos_programmable_interrupt_controller_raise_request(&pc->slaveProgrammableInterruptController, 4u);
    assert(hyperdos_pc_acknowledge_interrupt_request(pc, &interruptNumber));
    assert(interruptNumber == 0x74u);
    free(pc);
}

static void test_pointing_device_bios_services(void)
{
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;
    hyperdos_x86_processor*                processor = NULL;
    hyperdos_x86_execution_result          result    = HYPERDOS_X86_EXECUTION_OK;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    configuration.pcModel = HYPERDOS_PC_MODEL_AT;
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));
    processor = &machine->pc.processor;

    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, (uint16_t)(0xC207u));
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE, (uint16_t)(0x5678u));
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0x1234u);
    result = hyperdos_pc_system_bios_handle_system_services_interrupt(processor,
                                                                      &machine->pc,
                                                                      &machine->systemBios,
                                                                      0xC2u);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) == 0u);
    assert(machine->systemBios.pointingDeviceHandlerOffset == 0x5678u);
    assert(machine->systemBios.pointingDeviceHandlerSegment == 0x1234u);

    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, (uint16_t)(0xC200u));
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE, (uint16_t)(0x0100u));
    result = hyperdos_pc_system_bios_handle_system_services_interrupt(processor,
                                                                      &machine->pc,
                                                                      &machine->systemBios,
                                                                      0xC2u);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) == 0u);
    assert(machine->systemBios.pointingDeviceEnabled != 0u);
    assert(machine->pc.keyboardController.auxiliaryDeviceReportingEnabled != 0u);
    assert((machine->pc.programmableInterruptController.interruptMaskRegister & TEST_MASTER_CASCADE_INTERRUPT_MASK) ==
           0u);
    assert((machine->pc.slaveProgrammableInterruptController.interruptMaskRegister &
            TEST_AUXILIARY_DEVICE_INTERRUPT_MASK) == 0u);

    machine->pc.keyboardController.auxiliaryDeviceButtonMask = TEST_AUXILIARY_MOUSE_LEFT_BUTTON;
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, (uint16_t)(0xC206u));
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE, (uint16_t)(0x0000u));
    result = hyperdos_pc_system_bios_handle_system_services_interrupt(processor,
                                                                      &machine->pc,
                                                                      &machine->systemBios,
                                                                      0xC2u);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) == 0u);
    assert((hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE) &
            TEST_AUXILIARY_MOUSE_STATUS_LEFT_BUTTON) != 0u);
    assert((hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE) &
            TEST_AUXILIARY_MOUSE_LEFT_BUTTON) == 0u);

    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, (uint16_t)(0xC200u));
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE, (uint16_t)(0x0000u));
    result = hyperdos_pc_system_bios_handle_system_services_interrupt(processor,
                                                                      &machine->pc,
                                                                      &machine->systemBios,
                                                                      0xC2u);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) == 0u);
    assert(machine->systemBios.pointingDeviceEnabled == 0u);
    assert(machine->pc.keyboardController.auxiliaryDeviceReportingEnabled == 0u);
    assert((machine->pc.slaveProgrammableInterruptController.interruptMaskRegister &
            TEST_AUXILIARY_DEVICE_INTERRUPT_MASK) != 0u);
    hyperdos_pc_bios_runtime_initialize_data_area(&machine->biosRuntime, NULL, 1u);
    assert((hyperdos_pc_bios_data_area_read_word(&machine->pc, HYPERDOS_PC_BIOS_DATA_AREA_EQUIPMENT_FLAGS_OFFSET) &
            TEST_PC_EQUIPMENT_FLAGS_POINTING_DEVICE_PRESENT) != 0u);
    free(machine);
}

static void test_pointing_device_bios_callback_from_auxiliary_interrupt(void)
{
    enum
    {
        TEST_PROGRAM_SEGMENT             = 0x2000u,
        TEST_CALLBACK_SEGMENT            = 0x2100u,
        TEST_DATA_SEGMENT                = 0x2200u,
        TEST_STACK_SEGMENT               = 0x2300u,
        TEST_PROGRAM_STACK_POINTER       = 0x0100u,
        TEST_CALLBACK_RESULT_OFFSET      = 0x0040u,
        TEST_CALLBACK_HORIZONTAL_OFFSET  = 0x0042u,
        TEST_CALLBACK_VERTICAL_OFFSET    = 0x0044u,
        TEST_CALLBACK_PADDING_OFFSET     = 0x0046u,
        TEST_CALLBACK_MARKER_OFFSET      = 0x0048u,
        TEST_CALLBACK_RESULT_WORD        = 0xCAFEu,
        TEST_AUXILIARY_PACKET_BYTE_COUNT = 3u
    };
    static const uint8_t programBytes[]  = {0xF4u, 0xF4u, 0xF4u, 0xF4u};
    static const uint8_t callbackBytes[] = {0x55u, 0x8Bu, 0xECu, 0x8Bu, 0x46u, 0x0Cu, 0xA3u, 0x40u, 0x00u,
                                            0x8Bu, 0x46u, 0x0Au, 0xA3u, 0x42u, 0x00u, 0x8Bu, 0x46u, 0x08u,
                                            0xA3u, 0x44u, 0x00u, 0x8Bu, 0x46u, 0x06u, 0xA3u, 0x46u, 0x00u,
                                            0xB8u, 0xFEu, 0xCAu, 0xA3u, 0x48u, 0x00u, 0x5Du, 0xCBu};
    hyperdos_pc_machine* machine         = NULL;
    hyperdos_pc_machine_boot_configuration configuration;
    hyperdos_x86_processor*                processor = NULL;
    hyperdos_x86_execution_result          result    = HYPERDOS_X86_EXECUTION_OK;
    uint32_t programPhysicalAddress                  = (uint32_t)TEST_PROGRAM_SEGMENT << HYPERDOS_X86_SEGMENT_SHIFT;
    uint32_t callbackPhysicalAddress                 = (uint32_t)TEST_CALLBACK_SEGMENT << HYPERDOS_X86_SEGMENT_SHIFT;
    uint32_t resultPhysicalAddress                   = ((uint32_t)TEST_DATA_SEGMENT << HYPERDOS_X86_SEGMENT_SHIFT) +
                                     TEST_CALLBACK_RESULT_OFFSET;
    uint8_t packetByteIndex = 0u;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    configuration.pcModel = HYPERDOS_PC_MODEL_AT;
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));
    hyperdos_pc_bios_runtime_initialize_data_area(&machine->biosRuntime, NULL, 1u);
    hyperdos_pc_bios_install_interrupt_vector_stubs(&machine->pc);

    processor = &machine->pc.processor;
    hyperdos_x86_set_interrupt_handler(processor, hyperdos_pc_bios_runtime_handle_interrupt, &machine->biosRuntime);
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE, TEST_PROGRAM_SEGMENT);
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, TEST_DATA_SEGMENT);
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, TEST_STACK_SEGMENT);
    hyperdos_x86_set_instruction_pointer_word(processor, (uint16_t)(0u));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                           (uint16_t)(TEST_PROGRAM_STACK_POINTER));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(HYPERDOS_X86_FLAG_RESERVED | HYPERDOS_X86_FLAG_INTERRUPT_ENABLE));
    memcpy(&machine->pc.processorMemory[programPhysicalAddress], programBytes, sizeof(programBytes));
    memcpy(&machine->pc.processorMemory[callbackPhysicalAddress], callbackBytes, sizeof(callbackBytes));

    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, (uint16_t)(0xC207u));
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE, (uint16_t)(0u));
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, TEST_CALLBACK_SEGMENT);
    result = hyperdos_pc_system_bios_handle_system_services_interrupt(processor,
                                                                      &machine->pc,
                                                                      &machine->systemBios,
                                                                      0xC2u);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) == 0u);

    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, (uint16_t)(0xC200u));
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE, (uint16_t)(0x0100u));
    result = hyperdos_pc_system_bios_handle_system_services_interrupt(processor,
                                                                      &machine->pc,
                                                                      &machine->systemBios,
                                                                      0xC2u);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) == 0u);
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0u);

    assert(hyperdos_intel_8042_keyboard_controller_receive_auxiliary_mouse_packet(&machine->pc.keyboardController,
                                                                                  4,
                                                                                  -2,
                                                                                  TEST_AUXILIARY_MOUSE_LEFT_BUTTON));
    for (packetByteIndex = 0u; packetByteIndex < TEST_AUXILIARY_PACKET_BYTE_COUNT; ++packetByteIndex)
    {
        result = hyperdos_pc_bios_runtime_service_pending_hardware_interrupts(&machine->biosRuntime);
        assert(result == HYPERDOS_X86_EXECUTION_OK);
        result = hyperdos_x86_execute(processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
        assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    }

    assert((machine->pc.processorMemory[resultPhysicalAddress] & TEST_AUXILIARY_MOUSE_LEFT_BUTTON) != 0u);
    assert(machine->pc.processorMemory[resultPhysicalAddress + 1u] == 0u);
    assert(machine->pc.processorMemory[resultPhysicalAddress + TEST_CALLBACK_HORIZONTAL_OFFSET -
                                       TEST_CALLBACK_RESULT_OFFSET] == 4u);
    assert(machine->pc.processorMemory[resultPhysicalAddress + TEST_CALLBACK_HORIZONTAL_OFFSET -
                                       TEST_CALLBACK_RESULT_OFFSET + 1u] == 0u);
    assert(machine->pc.processorMemory[resultPhysicalAddress + TEST_CALLBACK_VERTICAL_OFFSET -
                                       TEST_CALLBACK_RESULT_OFFSET] == 0xFEu);
    assert(machine->pc.processorMemory[resultPhysicalAddress + TEST_CALLBACK_VERTICAL_OFFSET -
                                       TEST_CALLBACK_RESULT_OFFSET + 1u] == 0u);
    assert(machine->pc.processorMemory[resultPhysicalAddress + TEST_CALLBACK_PADDING_OFFSET -
                                       TEST_CALLBACK_RESULT_OFFSET] == 0u);
    assert(machine->pc.processorMemory[resultPhysicalAddress + TEST_CALLBACK_PADDING_OFFSET -
                                       TEST_CALLBACK_RESULT_OFFSET + 1u] == 0u);
    assert(machine->pc.processorMemory[resultPhysicalAddress + TEST_CALLBACK_MARKER_OFFSET -
                                       TEST_CALLBACK_RESULT_OFFSET] ==
           (uint8_t)(((uint16_t)TEST_CALLBACK_RESULT_WORD) & (uint16_t)TEST_BYTE_MASK));
    assert(machine->pc.processorMemory[resultPhysicalAddress + TEST_CALLBACK_MARKER_OFFSET -
                                       TEST_CALLBACK_RESULT_OFFSET + 1u] ==
           (uint8_t)(((uint16_t)TEST_CALLBACK_RESULT_WORD) >> (unsigned)TEST_DOS_SERVICE_REGISTER_SHIFT));
    free(machine);
}

static void test_bios_does_not_install_mouse_software_interrupt(void)
{
    enum
    {
        TEST_MOUSE_SOFTWARE_INTERRUPT = 0x33u
    };
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    configuration.pcModel = HYPERDOS_PC_MODEL_AT;
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));
    hyperdos_pc_bios_install_interrupt_vector_stubs(&machine->pc);
    assert(hyperdos_pc_bios_interrupt_vector_is_empty(&machine->pc, TEST_MOUSE_SOFTWARE_INTERRUPT));
    free(machine);
}

static void test_keyboard_bios_status_return_flags(void)
{
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;
    hyperdos_x86_processor*                processor            = NULL;
    hyperdos_x86_execution_result          result               = HYPERDOS_X86_EXECUTION_OK;
    uint32_t                               flagsPhysicalAddress = 0u;
    uint16_t                               flags                = 0u;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));
    hyperdos_pc_bios_runtime_initialize_data_area(&machine->biosRuntime, NULL, 1u);

    processor = &machine->pc.processor;
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, 0u);
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                           (uint16_t)(TEST_INTERRUPT_RETURN_STACK_POINTER));
    flagsPhysicalAddress = TEST_INTERRUPT_RETURN_STACK_POINTER + TEST_INTERRUPT_RETURN_FLAGS_STACK_OFFSET;

    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)(HYPERDOS_PC_KEYBOARD_BIOS_STATUS_SERVICE
                                                      << HYPERDOS_PC_KEYBOARD_BIOS_SERVICE_REGISTER_SHIFT));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(HYPERDOS_X86_FLAG_CARRY));
    machine->pc.processorMemory[flagsPhysicalAddress]      = HYPERDOS_X86_FLAG_CARRY;
    machine->pc.processorMemory[flagsPhysicalAddress + 1u] = 0u;
    result                                                 = hyperdos_pc_bios_runtime_handle_interrupt(processor,
                                                       HYPERDOS_PC_BIOS_KEYBOARD_SOFTWARE_SERVICE_INTERRUPT,
                                                       &machine->biosRuntime);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    flags = (uint16_t)(machine->pc.processorMemory[flagsPhysicalAddress] |
                       ((uint16_t)machine->pc.processorMemory[flagsPhysicalAddress + 1u]
                        << HYPERDOS_X86_BYTE_BIT_COUNT));
    assert((flags & HYPERDOS_X86_FLAG_ZERO) != 0u);
    assert((flags & HYPERDOS_X86_FLAG_CARRY) != 0u);
    assert((flags & HYPERDOS_X86_FLAG_INTERRUPT_ENABLE) != 0u);

    assert(hyperdos_pc_keyboard_bios_push_key_word(&machine->keyboardBios,
                                                   &machine->keyboardBiosInterface,
                                                   &machine->pc,
                                                   TEST_KEYBOARD_FUNCTION_KEY_SIX_WORD));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)(HYPERDOS_PC_KEYBOARD_BIOS_STATUS_SERVICE
                                                      << HYPERDOS_PC_KEYBOARD_BIOS_SERVICE_REGISTER_SHIFT));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(0u));
    machine->pc.processorMemory[flagsPhysicalAddress]      = HYPERDOS_X86_FLAG_ZERO;
    machine->pc.processorMemory[flagsPhysicalAddress + 1u] = 0u;
    result                                                 = hyperdos_pc_bios_runtime_handle_interrupt(processor,
                                                       HYPERDOS_PC_BIOS_KEYBOARD_SOFTWARE_SERVICE_INTERRUPT,
                                                       &machine->biosRuntime);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           TEST_KEYBOARD_FUNCTION_KEY_SIX_WORD);
    flags = (uint16_t)(machine->pc.processorMemory[flagsPhysicalAddress] |
                       ((uint16_t)machine->pc.processorMemory[flagsPhysicalAddress + 1u]
                        << HYPERDOS_X86_BYTE_BIT_COUNT));
    assert((flags & HYPERDOS_X86_FLAG_ZERO) == 0u);
    assert((flags & HYPERDOS_X86_FLAG_CARRY) == 0u);
    assert((flags & HYPERDOS_X86_FLAG_INTERRUPT_ENABLE) != 0u);

    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)(HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_STATUS_SERVICE
                                                      << HYPERDOS_PC_KEYBOARD_BIOS_SERVICE_REGISTER_SHIFT));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(0u));
    machine->pc.processorMemory[flagsPhysicalAddress]      = HYPERDOS_X86_FLAG_CARRY | HYPERDOS_X86_FLAG_ZERO;
    machine->pc.processorMemory[flagsPhysicalAddress + 1u] = 0u;
    result                                                 = hyperdos_pc_bios_runtime_handle_interrupt(processor,
                                                       HYPERDOS_PC_BIOS_KEYBOARD_SOFTWARE_SERVICE_INTERRUPT,
                                                       &machine->biosRuntime);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    flags = (uint16_t)(machine->pc.processorMemory[flagsPhysicalAddress] |
                       ((uint16_t)machine->pc.processorMemory[flagsPhysicalAddress + 1u]
                        << HYPERDOS_X86_BYTE_BIT_COUNT));
    assert((flags & HYPERDOS_X86_FLAG_ZERO) == 0u);
    assert((flags & HYPERDOS_X86_FLAG_CARRY) != 0u);
    assert((flags & HYPERDOS_X86_FLAG_INTERRUPT_ENABLE) != 0u);

    free(machine);
}

static void test_keyboard_bios_status_return_flags_through_firmware_stub(void)
{
    enum
    {
        TEST_PROGRAM_SEGMENT       = 0x2000u,
        TEST_PROGRAM_STACK_SEGMENT = 0x3000u,
        TEST_PROGRAM_STACK_POINTER = 0x0100u,
        TEST_KEYBOARD_ENTER_WORD   = 0x1C0Du
    };
    static const uint8_t                   programBytes[] = {0x31u,
                                                             0xC0u,
                                                             0xB4u,
                                                             HYPERDOS_PC_KEYBOARD_BIOS_STATUS_SERVICE,
                                                             0xCDu,
                                                             HYPERDOS_PC_BIOS_KEYBOARD_INTERRUPT,
                                                             0xF4u};
    hyperdos_pc_machine*                   machine        = NULL;
    hyperdos_pc_machine_boot_configuration configuration;
    hyperdos_x86_processor*                processor = NULL;
    hyperdos_x86_execution_result          result    = HYPERDOS_X86_EXECUTION_OK;
    uint32_t programPhysicalAddress                  = (uint32_t)TEST_PROGRAM_SEGMENT << HYPERDOS_X86_SEGMENT_SHIFT;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));
    hyperdos_pc_bios_runtime_initialize_data_area(&machine->biosRuntime, NULL, 1u);
    hyperdos_pc_bios_install_interrupt_vector_stubs(&machine->pc);

    processor = &machine->pc.processor;
    hyperdos_x86_set_interrupt_handler(processor, hyperdos_pc_bios_runtime_handle_interrupt, &machine->biosRuntime);
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE, TEST_PROGRAM_SEGMENT);
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, TEST_PROGRAM_STACK_SEGMENT);
    hyperdos_x86_set_instruction_pointer_word(processor, (uint16_t)(0u));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                           (uint16_t)(TEST_PROGRAM_STACK_POINTER));
    memcpy(&machine->pc.processorMemory[programPhysicalAddress], programBytes, sizeof(programBytes));
    assert(hyperdos_pc_keyboard_bios_push_key_word(&machine->keyboardBios,
                                                   &machine->keyboardBiosInterface,
                                                   &machine->pc,
                                                   TEST_KEYBOARD_ENTER_WORD));

    result = hyperdos_x86_execute(processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           TEST_KEYBOARD_ENTER_WORD);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_ZERO) == 0u);

    free(machine);
}

static void test_keyboard_bios_read_waits_with_interrupts_enabled(void)
{
    enum
    {
        TEST_KEYBOARD_SOFTWARE_SERVICE_STUB_OFFSET = 0x0138u
    };
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;
    hyperdos_x86_processor*                processor = NULL;
    hyperdos_x86_execution_result          result    = HYPERDOS_X86_EXECUTION_OK;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));
    hyperdos_pc_bios_runtime_initialize_data_area(&machine->biosRuntime, NULL, 1u);

    processor                        = &machine->pc.processor;
    processor->lastInstructionOffset = TEST_KEYBOARD_SOFTWARE_SERVICE_STUB_OFFSET;
    hyperdos_x86_set_instruction_pointer_word(processor, (uint16_t)(TEST_KEYBOARD_SOFTWARE_SERVICE_STUB_OFFSET + 2u));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)(HYPERDOS_PC_KEYBOARD_BIOS_READ_SERVICE
                                                      << HYPERDOS_PC_KEYBOARD_BIOS_SERVICE_REGISTER_SHIFT));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(HYPERDOS_X86_FLAG_RESERVED));

    result = hyperdos_pc_bios_runtime_handle_interrupt(processor,
                                                       HYPERDOS_PC_BIOS_KEYBOARD_SOFTWARE_SERVICE_INTERRUPT,
                                                       &machine->biosRuntime);
    assert(result == HYPERDOS_X86_EXECUTION_STEP_LIMIT_REACHED);
    assert(hyperdos_x86_get_instruction_pointer_word(processor) == TEST_KEYBOARD_SOFTWARE_SERVICE_STUB_OFFSET);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_INTERRUPT_ENABLE) != 0u);

    assert(hyperdos_pc_keyboard_bios_push_key_word(&machine->keyboardBios,
                                                   &machine->keyboardBiosInterface,
                                                   &machine->pc,
                                                   TEST_KEYBOARD_FUNCTION_KEY_SIX_WORD));
    hyperdos_x86_set_instruction_pointer_word(processor, (uint16_t)(TEST_KEYBOARD_SOFTWARE_SERVICE_STUB_OFFSET + 2u));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(HYPERDOS_X86_FLAG_RESERVED));
    result = hyperdos_pc_bios_runtime_handle_interrupt(processor,
                                                       HYPERDOS_PC_BIOS_KEYBOARD_SOFTWARE_SERVICE_INTERRUPT,
                                                       &machine->biosRuntime);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           TEST_KEYBOARD_FUNCTION_KEY_SIX_WORD);
    free(machine);
}

static void test_programmable_interrupt_controller_blocks_same_priority_reentry(void)
{
    enum
    {
        TEST_INTERRUPT_CONTROLLER_VECTOR_BASE              = 0x08u,
        TEST_INTERRUPT_CONTROLLER_KEYBOARD_REQUEST_LINE    = 1u,
        TEST_INTERRUPT_CONTROLLER_TIMER_REQUEST_LINE       = 0u,
        TEST_INTERRUPT_CONTROLLER_KEYBOARD_VECTOR          = 0x09u,
        TEST_INTERRUPT_CONTROLLER_TIMER_VECTOR             = 0x08u,
        TEST_INTERRUPT_CONTROLLER_COMMAND_PORT             = 0x20u,
        TEST_INTERRUPT_CONTROLLER_END_OF_INTERRUPT_COMMAND = 0x20u
    };
    hyperdos_programmable_interrupt_controller controller;
    uint8_t                                    interruptNumber = 0u;

    hyperdos_programmable_interrupt_controller_initialize(&controller, TEST_INTERRUPT_CONTROLLER_VECTOR_BASE);
    controller.interruptMaskRegister = 0u;

    hyperdos_programmable_interrupt_controller_raise_request(&controller,
                                                             TEST_INTERRUPT_CONTROLLER_KEYBOARD_REQUEST_LINE);
    assert(hyperdos_programmable_interrupt_controller_acknowledge(&controller, &interruptNumber));
    assert(interruptNumber == TEST_INTERRUPT_CONTROLLER_KEYBOARD_VECTOR);

    hyperdos_programmable_interrupt_controller_raise_request(&controller,
                                                             TEST_INTERRUPT_CONTROLLER_KEYBOARD_REQUEST_LINE);
    assert(!hyperdos_programmable_interrupt_controller_acknowledge(&controller, &interruptNumber));

    hyperdos_programmable_interrupt_controller_raise_request(&controller, TEST_INTERRUPT_CONTROLLER_TIMER_REQUEST_LINE);
    assert(hyperdos_programmable_interrupt_controller_acknowledge(&controller, &interruptNumber));
    assert(interruptNumber == TEST_INTERRUPT_CONTROLLER_TIMER_VECTOR);

    hyperdos_programmable_interrupt_controller_write_byte(&controller,
                                                          TEST_INTERRUPT_CONTROLLER_COMMAND_PORT,
                                                          TEST_INTERRUPT_CONTROLLER_END_OF_INTERRUPT_COMMAND);
    hyperdos_programmable_interrupt_controller_write_byte(&controller,
                                                          TEST_INTERRUPT_CONTROLLER_COMMAND_PORT,
                                                          TEST_INTERRUPT_CONTROLLER_END_OF_INTERRUPT_COMMAND);
    assert(hyperdos_programmable_interrupt_controller_acknowledge(&controller, &interruptNumber));
    assert(interruptNumber == TEST_INTERRUPT_CONTROLLER_KEYBOARD_VECTOR);
}

static void test_programmable_interrupt_controller_specific_end_of_interrupt(void)
{
    enum
    {
        TEST_INTERRUPT_CONTROLLER_VECTOR_BASE                       = 0x08u,
        TEST_INTERRUPT_CONTROLLER_KEYBOARD_REQUEST_LINE             = 1u,
        TEST_INTERRUPT_CONTROLLER_KEYBOARD_VECTOR                   = 0x09u,
        TEST_INTERRUPT_CONTROLLER_COMMAND_PORT                      = 0x20u,
        TEST_INTERRUPT_CONTROLLER_SPECIFIC_END_OF_INTERRUPT_COMMAND = 0x61u
    };
    hyperdos_programmable_interrupt_controller controller;
    uint8_t                                    interruptNumber = 0u;

    hyperdos_programmable_interrupt_controller_initialize(&controller, TEST_INTERRUPT_CONTROLLER_VECTOR_BASE);
    controller.interruptMaskRegister = 0u;

    hyperdos_programmable_interrupt_controller_raise_request(&controller,
                                                             TEST_INTERRUPT_CONTROLLER_KEYBOARD_REQUEST_LINE);
    assert(hyperdos_programmable_interrupt_controller_acknowledge(&controller, &interruptNumber));
    assert(interruptNumber == TEST_INTERRUPT_CONTROLLER_KEYBOARD_VECTOR);
    assert((controller.inServiceRegister & (1u << TEST_INTERRUPT_CONTROLLER_KEYBOARD_REQUEST_LINE)) != 0u);

    hyperdos_programmable_interrupt_controller_write_byte(&controller,
                                                          TEST_INTERRUPT_CONTROLLER_COMMAND_PORT,
                                                          TEST_INTERRUPT_CONTROLLER_SPECIFIC_END_OF_INTERRUPT_COMMAND);
    assert((controller.inServiceRegister & (1u << TEST_INTERRUPT_CONTROLLER_KEYBOARD_REQUEST_LINE)) == 0u);
}

static void test_programmable_interval_timer_rate_generator_transition(void)
{
    enum
    {
        TEST_INTERVAL_TIMER_CHANNEL_ZERO_PORT                    = 0x40u,
        TEST_INTERVAL_TIMER_CONTROL_PORT                         = 0x43u,
        TEST_INTERVAL_TIMER_CHANNEL_ZERO_RATE_GENERATOR_LOW_HIGH = 0x34u,
        TEST_INTERVAL_TIMER_RELOAD_VALUE                         = 4u
    };
    hyperdos_programmable_interval_timer timer;

    hyperdos_programmable_interval_timer_initialize(&timer);
    hyperdos_programmable_interval_timer_write_byte(&timer,
                                                    TEST_INTERVAL_TIMER_CONTROL_PORT,
                                                    TEST_INTERVAL_TIMER_CHANNEL_ZERO_RATE_GENERATOR_LOW_HIGH);
    hyperdos_programmable_interval_timer_write_byte(&timer, TEST_INTERVAL_TIMER_CHANNEL_ZERO_PORT, 4u);
    hyperdos_programmable_interval_timer_write_byte(&timer, TEST_INTERVAL_TIMER_CHANNEL_ZERO_PORT, 0u);

    hyperdos_programmable_interval_timer_tick(&timer, TEST_INTERVAL_TIMER_RELOAD_VALUE - 1u);
    assert(!hyperdos_programmable_interval_timer_get_and_clear_output_transition(&timer, 0u));
    hyperdos_programmable_interval_timer_tick(&timer, 1u);
    assert(hyperdos_programmable_interval_timer_get_and_clear_output_transition(&timer, 0u));
    assert(!hyperdos_programmable_interval_timer_get_and_clear_output_transition(&timer, 0u));
}

static void test_pc_speaker_state_change(void* userContext, uint32_t frequencyHertz, uint8_t enabled)
{
    test_speaker_context* speakerContext = (test_speaker_context*)userContext;

    speakerContext->frequencyHertz = frequencyHertz;
    speakerContext->enabled        = enabled;
    ++speakerContext->changeCount;
}

static void test_pc_default_interval_timer_transition(void)
{
    hyperdos_pc* pc = NULL;

    pc = (hyperdos_pc*)calloc(1u, sizeof(*pc));
    assert(pc != NULL);
    assert(hyperdos_pc_initialize(pc));
    hyperdos_intel_8284_clock_generator_step(&pc->clockGenerator,
                                             &pc->bus,
                                             TEST_PC_DEFAULT_TIMER_INTERRUPT_CLOCK_COUNT - 1u);
    assert(!hyperdos_programmable_interval_timer_get_and_clear_output_transition(&pc->programmableIntervalTimer, 0u));
    hyperdos_intel_8284_clock_generator_step(&pc->clockGenerator, &pc->bus, 1u);
    assert(hyperdos_programmable_interval_timer_get_and_clear_output_transition(&pc->programmableIntervalTimer, 0u));
    free(pc);
}

static void test_pc_interval_timer_input_frequency_is_independent_from_processor_frequency(void)
{
    enum
    {
        TEST_INTERVAL_TIMER_FULL_RELOAD_INPUT_CLOCK_COUNT = 0x10000u,
        TEST_TURBO_PROCESSOR_FREQUENCY_HERTZ              = 8000000u
    };

    hyperdos_pc* pc                                 = NULL;
    uint64_t     timerTransitionProcessorClockCount = 0u;

    pc = (hyperdos_pc*)calloc(1u, sizeof(*pc));
    assert(pc != NULL);
    assert(hyperdos_pc_initialize(pc));
    hyperdos_pc_set_processor_frequency_hertz(pc, TEST_TURBO_PROCESSOR_FREQUENCY_HERTZ);
    assert(hyperdos_pc_get_processor_frequency_hertz(pc) == TEST_TURBO_PROCESSOR_FREQUENCY_HERTZ);
    assert(hyperdos_pc_get_interval_timer_input_frequency_hertz(pc) ==
           HYPERDOS_PC_INTERVAL_TIMER_INPUT_FREQUENCY_HERTZ);

    timerTransitionProcessorClockCount = ((uint64_t)TEST_INTERVAL_TIMER_FULL_RELOAD_INPUT_CLOCK_COUNT *
                                                  HYPERDOS_PC_INTERVAL_TIMER_INPUT_CLOCK_DIVISOR *
                                                  TEST_TURBO_PROCESSOR_FREQUENCY_HERTZ +
                                          HYPERDOS_PC_8284_CRYSTAL_FREQUENCY_HERTZ - 1u) /
                                         HYPERDOS_PC_8284_CRYSTAL_FREQUENCY_HERTZ;
    hyperdos_intel_8284_clock_generator_step(&pc->clockGenerator, &pc->bus, timerTransitionProcessorClockCount - 1u);
    assert(!hyperdos_programmable_interval_timer_get_and_clear_output_transition(&pc->programmableIntervalTimer, 0u));
    hyperdos_intel_8284_clock_generator_step(&pc->clockGenerator, &pc->bus, 1u);
    assert(hyperdos_programmable_interval_timer_get_and_clear_output_transition(&pc->programmableIntervalTimer, 0u));
    free(pc);
}

static void test_pc_speaker_callback_from_port_control(void)
{
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;
    test_speaker_context                   speakerContext;

    memset(&configuration, 0, sizeof(configuration));
    memset(&speakerContext, 0, sizeof(speakerContext));
    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);

    configuration.userContext        = &speakerContext;
    configuration.speakerStateChange = test_pc_speaker_state_change;
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));

    hyperdos_bus_write_input_output_byte_if_mapped(&machine->pc.bus,
                                                   TEST_PC_INTERVAL_TIMER_CONTROL_PORT,
                                                   TEST_PC_INTERVAL_TIMER_CHANNEL_TWO_SQUARE_WAVE_LOW_HIGH);
    hyperdos_bus_write_input_output_byte_if_mapped(&machine->pc.bus,
                                                   TEST_PC_INTERVAL_TIMER_CHANNEL_TWO_PORT,
                                                   TEST_PC_INTERVAL_TIMER_CHANNEL_TWO_RELOAD_LOW_BYTE);
    hyperdos_bus_write_input_output_byte_if_mapped(&machine->pc.bus,
                                                   TEST_PC_INTERVAL_TIMER_CHANNEL_TWO_PORT,
                                                   TEST_PC_INTERVAL_TIMER_CHANNEL_TWO_RELOAD_HIGH_BYTE);
    assert(speakerContext.changeCount == 0u);

    hyperdos_bus_write_input_output_byte_if_mapped(&machine->pc.bus,
                                                   TEST_PC_PERIPHERAL_INTERFACE_PORT_B,
                                                   TEST_PC_SPEAKER_ENABLE_BITS);
    assert(speakerContext.changeCount == 1u);
    assert(speakerContext.enabled != 0u);
    assert(speakerContext.frequencyHertz != 0u);

    hyperdos_bus_write_input_output_byte_if_mapped(&machine->pc.bus, TEST_PC_PERIPHERAL_INTERFACE_PORT_B, 0u);
    assert(speakerContext.changeCount == 2u);
    assert(speakerContext.enabled == 0u);
    assert(speakerContext.frequencyHertz == 0u);
    free(machine);
}

static void test_pc_system_bios_timer_tick_data_area(void)
{
    hyperdos_pc* pc = NULL;

    pc = (hyperdos_pc*)calloc(1u, sizeof(*pc));
    assert(pc != NULL);
    assert(hyperdos_pc_initialize(pc));
    hyperdos_pc_system_bios_initialize_data_area(pc);
    assert(hyperdos_pc_bios_data_area_read_double_word(pc, HYPERDOS_PC_BIOS_DATA_AREA_TIMER_TICK_COUNT_OFFSET) == 0u);

    hyperdos_pc_system_bios_advance_timer_tick(pc);
    assert(hyperdos_pc_bios_data_area_read_double_word(pc, HYPERDOS_PC_BIOS_DATA_AREA_TIMER_TICK_COUNT_OFFSET) == 1u);

    hyperdos_pc_bios_data_area_write_double_word(pc,
                                                 HYPERDOS_PC_BIOS_DATA_AREA_TIMER_TICK_COUNT_OFFSET,
                                                 TEST_PC_TIMER_TICKS_PER_DAY - 1u);
    hyperdos_pc_system_bios_advance_timer_tick(pc);
    assert(hyperdos_pc_bios_data_area_read_double_word(pc, HYPERDOS_PC_BIOS_DATA_AREA_TIMER_TICK_COUNT_OFFSET) == 0u);
    assert(hyperdos_pc_bios_data_area_read_byte(pc, HYPERDOS_PC_BIOS_DATA_AREA_TIMER_MIDNIGHT_FLAG_OFFSET) == 1u);
    free(pc);
}

static void test_maskable_external_interrupt_request(void)
{
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint32_t               vectorAddress = (uint32_t)TEST_EXTERNAL_INTERRUPT_NUMBER * 4u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    memory[vectorAddress]      = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET & 0x00FFu);
    memory[vectorAddress + 1u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET >> 8u);
    memory[vectorAddress + 2u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT & 0x00FFu);
    memory[vectorAddress + 3u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT >> 8u);

    assert(hyperdos_x86_request_maskable_interrupt(&processor, TEST_EXTERNAL_INTERRUPT_NUMBER) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE) ==
           TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT);
    assert(hyperdos_x86_get_instruction_pointer_word(&processor) == TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET);
    assert((hyperdos_x86_get_flags_word(&processor) & HYPERDOS_X86_FLAG_INTERRUPT_ENABLE) == 0u);
    free(memory);
}

static void test_maskable_interrupt_waits_for_interrupt_shadow(void)
{
    static const uint8_t program[] = {
        0xFBu,
        0x90u,
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint32_t               vectorAddress                       = (uint32_t)TEST_EXTERNAL_INTERRUPT_NUMBER * 4u;
    uint16_t               instructionPointerAfterSetInterrupt = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_flags_word(&processor, (uint16_t)(HYPERDOS_X86_FLAG_RESERVED));
    memory[vectorAddress]      = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET & 0x00FFu);
    memory[vectorAddress + 1u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET >> 8u);
    memory[vectorAddress + 2u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT & 0x00FFu);
    memory[vectorAddress + 3u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT >> 8u);

    assert(hyperdos_x86_execute(&processor, 1u) == HYPERDOS_X86_EXECUTION_STEP_LIMIT_REACHED);
    instructionPointerAfterSetInterrupt = hyperdos_x86_get_instruction_pointer_word(&processor);
    assert(!hyperdos_x86_processor_accepts_maskable_interrupt(&processor));
    assert(hyperdos_x86_request_maskable_interrupt(&processor, TEST_EXTERNAL_INTERRUPT_NUMBER) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_get_instruction_pointer_word(&processor) == instructionPointerAfterSetInterrupt);

    assert(hyperdos_x86_execute(&processor, 2u) == HYPERDOS_X86_EXECUTION_STEP_LIMIT_REACHED);
    assert(hyperdos_x86_processor_accepts_maskable_interrupt(&processor));
    assert(hyperdos_x86_request_maskable_interrupt(&processor, TEST_EXTERNAL_INTERRUPT_NUMBER) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE) ==
           TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT);
    assert(hyperdos_x86_get_instruction_pointer_word(&processor) == TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET);
    free(memory);
}

static void test_halt_resumes_through_maskable_interrupt(void)
{
    static const uint8_t program[] = {
        0xF4u,
        0x90u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint32_t               vectorAddress = (uint32_t)TEST_EXTERNAL_INTERRUPT_NUMBER * 4u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    memory[vectorAddress]      = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET & 0x00FFu);
    memory[vectorAddress + 1u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET >> 8u);
    memory[vectorAddress + 2u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT & 0x00FFu);
    memory[vectorAddress + 3u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT >> 8u);

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(processor.halted != 0u);
    assert(hyperdos_x86_request_maskable_interrupt(&processor, TEST_EXTERNAL_INTERRUPT_NUMBER) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(processor.halted == 0u);
    assert(hyperdos_x86_get_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE) ==
           TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT);
    assert(hyperdos_x86_get_instruction_pointer_word(&processor) == TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET);
    free(memory);
}

static void test_trap_flag_dispatches_single_step_interrupt(void)
{
    static const uint8_t program[] = {
        0x90u,
        0xF4u,
    };
    static const uint8_t singleStepHandler[] = {
        0xB8u,
        0x34u,
        0x12u,
        0xF4u,
    };
    uint8_t*               memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor processor;
    uint32_t               handlerAddress = ((uint32_t)TEST_SINGLE_STEP_INTERRUPT_VECTOR_SEGMENT << 4u) +
                              TEST_SINGLE_STEP_INTERRUPT_VECTOR_OFFSET;
    size_t byteIndex = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_update_flags_word(&processor, (uint16_t)(HYPERDOS_X86_FLAG_TRAP), (uint16_t)(HYPERDOS_X86_FLAG_TRAP));
    memory[4] = (uint8_t)(TEST_SINGLE_STEP_INTERRUPT_VECTOR_OFFSET & 0x00FFu);
    memory[5] = (uint8_t)(TEST_SINGLE_STEP_INTERRUPT_VECTOR_OFFSET >> 8u);
    memory[6] = (uint8_t)(TEST_SINGLE_STEP_INTERRUPT_VECTOR_SEGMENT & 0x00FFu);
    memory[7] = (uint8_t)(TEST_SINGLE_STEP_INTERRUPT_VECTOR_SEGMENT >> 8u);
    for (byteIndex = 0u; byteIndex < sizeof(singleStepHandler); ++byteIndex)
    {
        memory[handlerAddress + byteIndex] = singleStepHandler[byteIndex];
    }

    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           TEST_SINGLE_STEP_EXPECTED_ACCUMULATOR);
    free(memory);
}

static void test_decimal_and_ascii_adjust_instructions(void)
{
    static const uint8_t decimalAdjustProgram[] = {
        0xB0u,
        0x9Bu,
        0x27u,
        0xF4u,
    };
    static const uint8_t asciiAdjustProgram[] = {
        0xB8u,
        0x0Au,
        0x00u,
        0x37u,
        0xF4u,
    };
    uint8_t*               memory = NULL;
    hyperdos_x86_processor processor;

    memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         decimalAdjustProgram,
                                         sizeof(decimalAdjustProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert((hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) &
            TEST_BYTE_MASK) == 0x01u);
    assert((hyperdos_x86_get_flags_word(&processor) & HYPERDOS_X86_FLAG_CARRY) != 0u);
    assert((hyperdos_x86_get_flags_word(&processor) & HYPERDOS_X86_FLAG_AUXILIARY_CARRY) != 0u);
    free(memory);

    memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         asciiAdjustProgram,
                                         sizeof(asciiAdjustProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x0100u);
    assert((hyperdos_x86_get_flags_word(&processor) & HYPERDOS_X86_FLAG_CARRY) != 0u);
    assert((hyperdos_x86_get_flags_word(&processor) & HYPERDOS_X86_FLAG_AUXILIARY_CARRY) != 0u);
    free(memory);
}

static void test_ascii_adjust_multiply_divide(void)
{
    static const uint8_t multiplyAdjustProgram[] = {
        0xB8u,
        0x27u,
        0x00u,
        0xD4u,
        0x0Au,
        0xF4u,
    };
    static const uint8_t divideAdjustProgram[] = {
        0xB8u,
        0x07u,
        0x03u,
        0xD5u,
        0x0Au,
        0xF4u,
    };
    static const uint8_t divideErrorProgram[] = {
        0xB8u,
        0x27u,
        0x00u,
        0xD4u,
        0x00u,
        0xF4u,
    };
    uint8_t*                      memory = NULL;
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         multiplyAdjustProgram,
                                         sizeof(multiplyAdjustProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x0309u);
    free(memory);

    memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         divideAdjustProgram,
                                         sizeof(divideAdjustProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x0025u);
    free(memory);

    memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         divideErrorProgram,
                                         sizeof(divideErrorProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_DIVIDE_ERROR);
    free(memory);
}

static void test_signed_divide_minimum_negative_quotient_for_model(
        hyperdos_x86_processor_model  processorModel,
        hyperdos_x86_execution_result expectedExecutionResult)
{
    static const uint8_t byteDivideProgram[] = {
        0xB8u,
        0x80u,
        0xFFu,
        0xB3u,
        0x01u,
        0xF6u,
        0xFBu,
        0xF4u,
    };
    static const uint8_t wordDivideProgram[] = {
        0xB8u,
        0x00u,
        0x80u,
        0xBAu,
        0xFFu,
        0xFFu,
        0xBBu,
        0x01u,
        0x00u,
        0xF7u,
        0xFBu,
        0xF4u,
    };
    uint8_t*                      memory = NULL;
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         byteDivideProgram,
                                         sizeof(byteDivideProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == expectedExecutionResult);
    if (expectedExecutionResult == HYPERDOS_X86_EXECUTION_HALTED)
    {
        assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x0080u);
    }
    free(memory);

    memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, processorModel);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         wordDivideProgram,
                                         sizeof(wordDivideProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == expectedExecutionResult);
    if (expectedExecutionResult == HYPERDOS_X86_EXECUTION_HALTED)
    {
        assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x8000u);
        assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_DATA) == 0x0000u);
    }
    free(memory);
}

static void test_signed_divide_minimum_negative_quotient_policy(void)
{
    test_signed_divide_minimum_negative_quotient_for_model(HYPERDOS_X86_PROCESSOR_MODEL_8086,
                                                           HYPERDOS_X86_EXECUTION_DIVIDE_ERROR);
    test_signed_divide_minimum_negative_quotient_for_model(HYPERDOS_X86_PROCESSOR_MODEL_8088,
                                                           HYPERDOS_X86_EXECUTION_DIVIDE_ERROR);
    test_signed_divide_minimum_negative_quotient_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80186,
                                                           HYPERDOS_X86_EXECUTION_HALTED);
    test_signed_divide_minimum_negative_quotient_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80188,
                                                           HYPERDOS_X86_EXECUTION_HALTED);
    test_signed_divide_minimum_negative_quotient_for_model(HYPERDOS_X86_PROCESSOR_MODEL_80286,
                                                           HYPERDOS_X86_EXECUTION_HALTED);
}

static void test_divide_error_uses_interrupt_vector(void)
{
    static const uint8_t divideErrorInterruptProgram[] = {
        0xB8u,
        0x01u,
        0x00u,
        0xBAu,
        0x00u,
        0x00u,
        0xBBu,
        0x00u,
        0x00u,
        0xF7u,
        0xF3u,
        0xB9u,
        0xFEu,
        0xCAu,
        0xF4u,
    };
    static const uint8_t divideErrorInterruptHandler[] = {
        0x8Bu,
        0xECu,
        0x8Bu,
        0x46u,
        0x00u,
        0xF4u,
    };
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result         = HYPERDOS_X86_EXECUTION_OK;
    uint32_t                      handlerAddress = ((uint32_t)TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_SEGMENT << 4u) +
                              TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_OFFSET;
    size_t byteIndex = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         divideErrorInterruptProgram,
                                         sizeof(divideErrorInterruptProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    memory[0] = (uint8_t)(TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_OFFSET & 0x00FFu);
    memory[1] = (uint8_t)(TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_OFFSET >> 8u);
    memory[2] = (uint8_t)(TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_SEGMENT & 0x00FFu);
    memory[3] = (uint8_t)(TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_SEGMENT >> 8u);
    for (byteIndex = 0u; byteIndex < sizeof(divideErrorInterruptHandler); ++byteIndex)
    {
        memory[handlerAddress + byteIndex] = divideErrorInterruptHandler[byteIndex];
    }

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           TEST_DIVIDE_ERROR_RETURN_OFFSET);
    free(memory);
}

static void test_divide_error_can_return_to_faulting_instruction(void)
{
    static const uint8_t divideErrorInterruptProgram[] = {
        0xB8u,
        0x01u,
        0x00u,
        0xBAu,
        0x00u,
        0x00u,
        0xBBu,
        0x00u,
        0x00u,
        0xF7u,
        0xF3u,
        0xB9u,
        0xFEu,
        0xCAu,
        0xF4u,
    };
    static const uint8_t divideErrorInterruptHandler[] = {
        0x8Bu,
        0xECu,
        0x8Bu,
        0x46u,
        0x00u,
        0xF4u,
    };
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result         = HYPERDOS_X86_EXECUTION_OK;
    uint32_t                      handlerAddress = ((uint32_t)TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_SEGMENT << 4u) +
                              TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_OFFSET;
    size_t byteIndex = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    processor.divideErrorReturnsToFaultingInstruction = 1u;
    assert(hyperdos_x86_load_dos_program(&processor,
                                         divideErrorInterruptProgram,
                                         sizeof(divideErrorInterruptProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    memory[0] = (uint8_t)(TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_OFFSET & 0x00FFu);
    memory[1] = (uint8_t)(TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_OFFSET >> 8u);
    memory[2] = (uint8_t)(TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_SEGMENT & 0x00FFu);
    memory[3] = (uint8_t)(TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_SEGMENT >> 8u);
    for (byteIndex = 0u; byteIndex < sizeof(divideErrorInterruptHandler); ++byteIndex)
    {
        memory[handlerAddress + byteIndex] = divideErrorInterruptHandler[byteIndex];
    }

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           TEST_DIVIDE_ERROR_FAULT_OFFSET);
    free(memory);
}

static void test_translate_byte_respects_segment_override(void)
{
    static const uint8_t program[] = {
        0xBBu,
        0x00u,
        0x02u,
        0xB0u,
        0x03u,
        0x2Eu,
        0xD7u,
        0xF4u,
    };
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_segment_register(&processor,
                                      HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                      HYPERDOS_X86_DEFAULT_DOS_SEGMENT + 1u);
    assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE, 0x0203u, 0x5Au) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_write_memory_byte(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0x0203u, 0x11u) ==
           HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert((hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) &
            TEST_DOS_EXIT_CODE_MASK) == 0x5Au);
    free(memory);
}

static void test_repeated_store_respects_memory_mapped_video(void)
{
    static const uint8_t program[] = {
        0xB8u,
        0x00u,
        0xB8u,
        0x8Eu,
        0xC0u,
        0x31u,
        0xFFu,
        0xB9u,
        0x05u,
        0x00u,
        0xB0u,
        0x5Au,
        0xF3u,
        0xAAu,
        0xF4u,
    };
    uint8_t*                        memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor          processor;
    hyperdos_bus                    bus;
    hyperdos_random_access_memory   randomAccessMemory;
    hyperdos_color_graphics_adapter colorGraphicsAdapter;
    hyperdos_x86_execution_result   result    = HYPERDOS_X86_EXECUTION_OK;
    size_t                          byteIndex = 0;

    assert(memory != NULL);
    hyperdos_bus_initialize(&bus);
    hyperdos_random_access_memory_initialize(&randomAccessMemory, memory, HYPERDOS_CONVENTIONAL_MEMORY_SIZE, 0u);
    hyperdos_color_graphics_adapter_initialize(&colorGraphicsAdapter);
    assert(hyperdos_bus_map_memory(&bus,
                                   0u,
                                   HYPERDOS_CONVENTIONAL_MEMORY_SIZE,
                                   &randomAccessMemory,
                                   hyperdos_random_access_memory_read_byte,
                                   hyperdos_random_access_memory_write_byte) == HYPERDOS_BUS_ACCESS_OK);
    assert(hyperdos_bus_map_memory(&bus,
                                   HYPERDOS_COLOR_GRAPHICS_ADAPTER_TEXT_MEMORY_ADDRESS,
                                   HYPERDOS_COLOR_GRAPHICS_ADAPTER_TEXT_MEMORY_SIZE,
                                   &colorGraphicsAdapter,
                                   hyperdos_color_graphics_adapter_read_memory_byte,
                                   hyperdos_color_graphics_adapter_write_memory_byte) == HYPERDOS_BUS_ACCESS_OK);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_attach_bus(&processor, &bus);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    for (byteIndex = 0; byteIndex < TEST_VIDEO_WRITE_BYTE_COUNT; ++byteIndex)
    {
        assert(colorGraphicsAdapter.memory[byteIndex] == 'Z');
    }
    free(memory);
}

static void test_repeated_move_respects_memory_mapped_video(void)
{
    static const uint8_t program[] = {
        0xBEu,
        0x20u,
        0x01u,
        0xB8u,
        0x00u,
        0xB8u,
        0x8Eu,
        0xC0u,
        0x31u,
        0xFFu,
        0xB9u,
        0x05u,
        0x00u,
        0xF3u,
        0xA4u,
        0xF4u,
    };
    static const uint8_t            sourceBytes[] = {'H', '8', '0', '8', '6'};
    uint8_t*                        memory        = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor          processor;
    hyperdos_bus                    bus;
    hyperdos_random_access_memory   randomAccessMemory;
    hyperdos_color_graphics_adapter colorGraphicsAdapter;
    hyperdos_x86_execution_result   result    = HYPERDOS_X86_EXECUTION_OK;
    size_t                          byteIndex = 0;

    assert(memory != NULL);
    hyperdos_bus_initialize(&bus);
    hyperdos_random_access_memory_initialize(&randomAccessMemory, memory, HYPERDOS_CONVENTIONAL_MEMORY_SIZE, 0u);
    hyperdos_color_graphics_adapter_initialize(&colorGraphicsAdapter);
    assert(hyperdos_bus_map_memory(&bus,
                                   0u,
                                   HYPERDOS_CONVENTIONAL_MEMORY_SIZE,
                                   &randomAccessMemory,
                                   hyperdos_random_access_memory_read_byte,
                                   hyperdos_random_access_memory_write_byte) == HYPERDOS_BUS_ACCESS_OK);
    assert(hyperdos_bus_map_memory(&bus,
                                   HYPERDOS_COLOR_GRAPHICS_ADAPTER_TEXT_MEMORY_ADDRESS,
                                   HYPERDOS_COLOR_GRAPHICS_ADAPTER_TEXT_MEMORY_SIZE,
                                   &colorGraphicsAdapter,
                                   hyperdos_color_graphics_adapter_read_memory_byte,
                                   hyperdos_color_graphics_adapter_write_memory_byte) == HYPERDOS_BUS_ACCESS_OK);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_attach_bus(&processor, &bus);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         program,
                                         sizeof(program),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    for (byteIndex = 0; byteIndex < sizeof(sourceBytes); ++byteIndex)
    {
        assert(hyperdos_x86_write_memory_byte(&processor,
                                              HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                              (uint16_t)(TEST_REPEATED_MOVE_SOURCE_OFFSET + byteIndex),
                                              sourceBytes[byteIndex]) == HYPERDOS_X86_EXECUTION_OK);
    }

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    for (byteIndex = 0; byteIndex < sizeof(sourceBytes); ++byteIndex)
    {
        assert(colorGraphicsAdapter.memory[byteIndex] == sourceBytes[byteIndex]);
    }
    free(memory);
}

static void test_video_graphics_array_plane_memory(void)
{
    enum
    {
        TEST_VIDEO_GRAPHICS_ARRAY_SEQUENCER_INDEX_PORT           = 0x03C4u,
        TEST_VIDEO_GRAPHICS_ARRAY_SEQUENCER_DATA_PORT            = 0x03C5u,
        TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_PORT = 0x03CEu,
        TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_DATA_PORT  = 0x03CFu,
        TEST_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MAP_MASK_INDEX       = 0x02u,
        TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_READ_MAP_SELECT_INDEX = 0x04u,
        TEST_VIDEO_GRAPHICS_ARRAY_FIRST_AND_THIRD_PLANE_MASK     = 0x05u,
        TEST_VIDEO_GRAPHICS_ARRAY_MEMORY_OFFSET                  = 0x1234u,
        TEST_VIDEO_GRAPHICS_ARRAY_WRITTEN_BYTE                   = 0xA5u
    };
    hyperdos_color_graphics_adapter colorGraphicsAdapter;

    hyperdos_color_graphics_adapter_initialize(&colorGraphicsAdapter);
    hyperdos_color_graphics_adapter_set_bios_video_mode(&colorGraphicsAdapter, 0x12u);
    hyperdos_color_graphics_adapter_write_input_output_byte(&colorGraphicsAdapter,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_SEQUENCER_INDEX_PORT,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MAP_MASK_INDEX);
    hyperdos_color_graphics_adapter_write_input_output_byte(&colorGraphicsAdapter,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_SEQUENCER_DATA_PORT,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_FIRST_AND_THIRD_PLANE_MASK);
    hyperdos_color_graphics_adapter_write_memory_byte(&colorGraphicsAdapter,
                                                      HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_ADDRESS +
                                                              TEST_VIDEO_GRAPHICS_ARRAY_MEMORY_OFFSET,
                                                      TEST_VIDEO_GRAPHICS_ARRAY_WRITTEN_BYTE);

    assert(colorGraphicsAdapter.graphicsPlanes[0][TEST_VIDEO_GRAPHICS_ARRAY_MEMORY_OFFSET] ==
           TEST_VIDEO_GRAPHICS_ARRAY_WRITTEN_BYTE);
    assert(colorGraphicsAdapter.graphicsPlanes[1][TEST_VIDEO_GRAPHICS_ARRAY_MEMORY_OFFSET] == 0u);
    assert(colorGraphicsAdapter.graphicsPlanes[2][TEST_VIDEO_GRAPHICS_ARRAY_MEMORY_OFFSET] ==
           TEST_VIDEO_GRAPHICS_ARRAY_WRITTEN_BYTE);
    assert(colorGraphicsAdapter.graphicsPlanes[3][TEST_VIDEO_GRAPHICS_ARRAY_MEMORY_OFFSET] == 0u);

    hyperdos_color_graphics_adapter_write_input_output_byte(&colorGraphicsAdapter,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_PORT,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_READ_MAP_SELECT_INDEX);
    hyperdos_color_graphics_adapter_write_input_output_byte(&colorGraphicsAdapter,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_DATA_PORT,
                                                            2u);
    assert(hyperdos_color_graphics_adapter_read_memory_byte(&colorGraphicsAdapter,
                                                            HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_ADDRESS +
                                                                    TEST_VIDEO_GRAPHICS_ARRAY_MEMORY_OFFSET) ==
           TEST_VIDEO_GRAPHICS_ARRAY_WRITTEN_BYTE);
}

static void test_video_graphics_array_write_mode_one_copies_latches(void)
{
    enum
    {
        TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_PORT = 0x03CEu,
        TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_DATA_PORT  = 0x03CFu,
        TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MODE_INDEX            = 0x05u,
        TEST_VIDEO_GRAPHICS_ARRAY_SOURCE_MEMORY_OFFSET           = 0x0100u,
        TEST_VIDEO_GRAPHICS_ARRAY_DESTINATION_MEMORY_OFFSET      = 0x0200u,
        TEST_VIDEO_GRAPHICS_ARRAY_WRITE_MODE_ONE                 = 0x01u,
        TEST_VIDEO_GRAPHICS_ARRAY_IGNORED_PROCESSOR_BYTE         = 0x00u
    };
    static const uint8_t sourcePlaneBytes[HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT] = {0x11u, 0x22u, 0x44u, 0x88u};
    hyperdos_color_graphics_adapter colorGraphicsAdapter;
    size_t                          planeIndex = 0u;

    hyperdos_color_graphics_adapter_initialize(&colorGraphicsAdapter);
    hyperdos_color_graphics_adapter_set_bios_video_mode(&colorGraphicsAdapter, 0x12u);

    for (planeIndex = 0u; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
    {
        colorGraphicsAdapter.graphicsPlanes
                [planeIndex][TEST_VIDEO_GRAPHICS_ARRAY_SOURCE_MEMORY_OFFSET] = sourcePlaneBytes[planeIndex];
        colorGraphicsAdapter.graphicsPlanes[planeIndex][TEST_VIDEO_GRAPHICS_ARRAY_DESTINATION_MEMORY_OFFSET] = 0xF0u;
    }

    (void)hyperdos_color_graphics_adapter_read_memory_byte(&colorGraphicsAdapter,
                                                           HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_ADDRESS +
                                                                   TEST_VIDEO_GRAPHICS_ARRAY_SOURCE_MEMORY_OFFSET);
    hyperdos_color_graphics_adapter_write_input_output_byte(&colorGraphicsAdapter,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_PORT,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MODE_INDEX);
    hyperdos_color_graphics_adapter_write_input_output_byte(&colorGraphicsAdapter,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_DATA_PORT,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_WRITE_MODE_ONE);
    hyperdos_color_graphics_adapter_write_memory_byte(&colorGraphicsAdapter,
                                                      HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_ADDRESS +
                                                              TEST_VIDEO_GRAPHICS_ARRAY_DESTINATION_MEMORY_OFFSET,
                                                      TEST_VIDEO_GRAPHICS_ARRAY_IGNORED_PROCESSOR_BYTE);

    for (planeIndex = 0u; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
    {
        assert(colorGraphicsAdapter.graphicsPlanes[planeIndex][TEST_VIDEO_GRAPHICS_ARRAY_DESTINATION_MEMORY_OFFSET] ==
               sourcePlaneBytes[planeIndex]);
    }
}

static void test_video_graphics_array_write_mode_zero_uses_existing_latches(void)
{
    enum
    {
        TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_PORT = 0x03CEu,
        TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_DATA_PORT  = 0x03CFu,
        TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_BIT_MASK_INDEX        = 0x08u,
        TEST_VIDEO_GRAPHICS_ARRAY_SOURCE_MEMORY_OFFSET           = 0x0500u,
        TEST_VIDEO_GRAPHICS_ARRAY_DESTINATION_MEMORY_OFFSET      = 0x0600u,
        TEST_VIDEO_GRAPHICS_ARRAY_ZERO_BIT_MASK                  = 0x00u,
        TEST_VIDEO_GRAPHICS_ARRAY_IGNORED_PROCESSOR_BYTE         = 0xA5u
    };
    static const uint8_t sourcePlaneBytes[HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT] = {0x11u, 0x22u, 0x44u, 0x88u};
    static const uint8_t destinationPlaneBytes[HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT] = {0xAAu,
                                                                                             0xBBu,
                                                                                             0xCCu,
                                                                                             0xDDu};
    hyperdos_color_graphics_adapter colorGraphicsAdapter;
    size_t                          planeIndex = 0u;

    hyperdos_color_graphics_adapter_initialize(&colorGraphicsAdapter);
    hyperdos_color_graphics_adapter_set_bios_video_mode(&colorGraphicsAdapter, 0x12u);

    for (planeIndex = 0u; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
    {
        colorGraphicsAdapter.graphicsPlanes
                [planeIndex][TEST_VIDEO_GRAPHICS_ARRAY_SOURCE_MEMORY_OFFSET] = sourcePlaneBytes[planeIndex];
        colorGraphicsAdapter.graphicsPlanes
                [planeIndex][TEST_VIDEO_GRAPHICS_ARRAY_DESTINATION_MEMORY_OFFSET] = destinationPlaneBytes[planeIndex];
    }

    (void)hyperdos_color_graphics_adapter_read_memory_byte(&colorGraphicsAdapter,
                                                           HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_ADDRESS +
                                                                   TEST_VIDEO_GRAPHICS_ARRAY_SOURCE_MEMORY_OFFSET);
    hyperdos_color_graphics_adapter_write_input_output_byte(&colorGraphicsAdapter,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_PORT,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_BIT_MASK_INDEX);
    hyperdos_color_graphics_adapter_write_input_output_byte(&colorGraphicsAdapter,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_DATA_PORT,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_ZERO_BIT_MASK);
    hyperdos_color_graphics_adapter_write_memory_byte(&colorGraphicsAdapter,
                                                      HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_ADDRESS +
                                                              TEST_VIDEO_GRAPHICS_ARRAY_DESTINATION_MEMORY_OFFSET,
                                                      TEST_VIDEO_GRAPHICS_ARRAY_IGNORED_PROCESSOR_BYTE);

    for (planeIndex = 0u; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
    {
        assert(colorGraphicsAdapter.graphicsPlanes[planeIndex][TEST_VIDEO_GRAPHICS_ARRAY_DESTINATION_MEMORY_OFFSET] ==
               sourcePlaneBytes[planeIndex]);
    }
}

typedef struct test_memory_write_observer_context
{
    size_t  writeCount;
    uint8_t oldValue;
    uint8_t newValue;
} test_memory_write_observer_context;

static void test_observe_memory_write(void*    observerContext,
                                      uint32_t physicalAddress,
                                      uint8_t  oldValue,
                                      uint8_t  newValue)
{
    test_memory_write_observer_context* context = (test_memory_write_observer_context*)observerContext;

    (void)physicalAddress;
    ++context->writeCount;
    context->oldValue = oldValue;
    context->newValue = newValue;
}

static void test_video_graphics_array_observer_does_not_reload_latches(void)
{
    enum
    {
        TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_PORT = 0x03CEu,
        TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_DATA_PORT  = 0x03CFu,
        TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MODE_INDEX            = 0x05u,
        TEST_VIDEO_GRAPHICS_ARRAY_SOURCE_MEMORY_OFFSET           = 0x0300u,
        TEST_VIDEO_GRAPHICS_ARRAY_DESTINATION_MEMORY_OFFSET      = 0x0400u,
        TEST_VIDEO_GRAPHICS_ARRAY_WRITE_MODE_ONE                 = 0x01u,
        TEST_VIDEO_GRAPHICS_ARRAY_IGNORED_PROCESSOR_BYTE         = 0x00u,
        TEST_VIDEO_GRAPHICS_ARRAY_UNOBSERVED_OLD_VALUE           = 0xFFu
    };
    static const uint8_t sourcePlaneBytes[HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT] = {0x12u, 0x34u, 0x56u, 0x78u};
    hyperdos_bus         bus;
    hyperdos_color_graphics_adapter    colorGraphicsAdapter;
    test_memory_write_observer_context observerContext;
    size_t                             planeIndex = 0u;

    memset(&observerContext, 0, sizeof(observerContext));
    hyperdos_bus_initialize(&bus);
    hyperdos_color_graphics_adapter_initialize(&colorGraphicsAdapter);
    hyperdos_color_graphics_adapter_set_bios_video_mode(&colorGraphicsAdapter, 0x12u);
    assert(hyperdos_bus_map_memory(&bus,
                                   HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_APERTURE_ADDRESS,
                                   HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_APERTURE_SIZE,
                                   &colorGraphicsAdapter,
                                   hyperdos_color_graphics_adapter_read_memory_byte,
                                   hyperdos_color_graphics_adapter_write_memory_byte) == HYPERDOS_BUS_ACCESS_OK);
    hyperdos_bus_set_memory_mapping_observer_old_value_read_enabled(
            &bus,
            HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_APERTURE_ADDRESS,
            HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_APERTURE_SIZE,
            0);
    hyperdos_bus_set_memory_write_observer(&bus, test_observe_memory_write, &observerContext);

    for (planeIndex = 0u; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
    {
        colorGraphicsAdapter.graphicsPlanes
                [planeIndex][TEST_VIDEO_GRAPHICS_ARRAY_SOURCE_MEMORY_OFFSET] = sourcePlaneBytes[planeIndex];
        colorGraphicsAdapter.graphicsPlanes[planeIndex][TEST_VIDEO_GRAPHICS_ARRAY_DESTINATION_MEMORY_OFFSET] = 0xF0u;
    }

    (void)hyperdos_bus_read_memory_byte_or_open_bus(&bus,
                                                    HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_ADDRESS +
                                                            TEST_VIDEO_GRAPHICS_ARRAY_SOURCE_MEMORY_OFFSET);
    hyperdos_color_graphics_adapter_write_input_output_byte(&colorGraphicsAdapter,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_PORT,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MODE_INDEX);
    hyperdos_color_graphics_adapter_write_input_output_byte(&colorGraphicsAdapter,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_DATA_PORT,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_WRITE_MODE_ONE);
    hyperdos_bus_write_memory_byte_if_mapped(&bus,
                                             HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_ADDRESS +
                                                     TEST_VIDEO_GRAPHICS_ARRAY_DESTINATION_MEMORY_OFFSET,
                                             TEST_VIDEO_GRAPHICS_ARRAY_IGNORED_PROCESSOR_BYTE);

    assert(observerContext.writeCount == 1u);
    assert(observerContext.oldValue == TEST_VIDEO_GRAPHICS_ARRAY_UNOBSERVED_OLD_VALUE);
    assert(observerContext.newValue == TEST_VIDEO_GRAPHICS_ARRAY_IGNORED_PROCESSOR_BYTE);
    for (planeIndex = 0u; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
    {
        assert(colorGraphicsAdapter.graphicsPlanes[planeIndex][TEST_VIDEO_GRAPHICS_ARRAY_DESTINATION_MEMORY_OFFSET] ==
               sourcePlaneBytes[planeIndex]);
    }
}

static uint8_t test_expand_video_digital_to_analog_converter_component(uint8_t component)
{
    component = (uint8_t)(component & 0x3Fu);
    return (uint8_t)((component << 2u) | (component >> 4u));
}

static uint32_t test_pack_video_color(uint8_t red, uint8_t green, uint8_t blue)
{
    return (uint32_t)test_expand_video_digital_to_analog_converter_component(red) |
           ((uint32_t)test_expand_video_digital_to_analog_converter_component(green) << 8u) |
           ((uint32_t)test_expand_video_digital_to_analog_converter_component(blue) << 16u);
}

static void test_video_graphics_array_attribute_palette_uses_internal_high_bits(void)
{
    enum
    {
        TEST_VIDEO_GRAPHICS_ARRAY_SOURCE_WIDTH                       = 640u,
        TEST_VIDEO_GRAPHICS_ARRAY_SOURCE_HEIGHT                      = 480u,
        TEST_VIDEO_GRAPHICS_ARRAY_FIRST_PIXEL_MASK                   = 0x80u,
        TEST_VIDEO_GRAPHICS_ARRAY_COLOR_INDEX_FOUR                   = 4u,
        TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_MODE_CONTROL_INDEX       = 0x10u,
        TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_COLOR_PLANE_ENABLE_INDEX = 0x12u,
        TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_COLOR_SELECT_INDEX       = 0x14u,
        TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_COLOR_SELECT_MODE_BIT    = 0x80u,
        TEST_VIDEO_GRAPHICS_ARRAY_INTERNAL_PALETTE_VALUE             = 0x31u,
        TEST_VIDEO_GRAPHICS_ARRAY_COLOR_SELECT_PALETTE_VALUE         = 0x01u,
        TEST_VIDEO_GRAPHICS_ARRAY_INTERNAL_PALETTE_RED               = 0x05u,
        TEST_VIDEO_GRAPHICS_ARRAY_INTERNAL_PALETTE_GREEN             = 0x06u,
        TEST_VIDEO_GRAPHICS_ARRAY_INTERNAL_PALETTE_BLUE              = 0x07u,
        TEST_VIDEO_GRAPHICS_ARRAY_COLOR_SELECT_RED                   = 0x11u,
        TEST_VIDEO_GRAPHICS_ARRAY_COLOR_SELECT_GREEN                 = 0x12u,
        TEST_VIDEO_GRAPHICS_ARRAY_COLOR_SELECT_BLUE                  = 0x13u
    };
    hyperdos_color_graphics_adapter colorGraphicsAdapter;
    uint32_t*                       pixels = NULL;

    pixels = (uint32_t*)calloc(TEST_VIDEO_GRAPHICS_ARRAY_SOURCE_WIDTH * TEST_VIDEO_GRAPHICS_ARRAY_SOURCE_HEIGHT,
                               sizeof(*pixels));
    assert(pixels != NULL);
    hyperdos_color_graphics_adapter_initialize(&colorGraphicsAdapter);
    hyperdos_color_graphics_adapter_set_bios_video_mode(&colorGraphicsAdapter, 0x12u);
    colorGraphicsAdapter.graphicsPlanes[2][0] = TEST_VIDEO_GRAPHICS_ARRAY_FIRST_PIXEL_MASK;
    colorGraphicsAdapter.attributeControllerRegisters
            [TEST_VIDEO_GRAPHICS_ARRAY_COLOR_INDEX_FOUR] = TEST_VIDEO_GRAPHICS_ARRAY_INTERNAL_PALETTE_VALUE;
    colorGraphicsAdapter.attributeControllerRegisters
            [TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_COLOR_PLANE_ENABLE_INDEX] = 0x0Fu;
    colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_GRAPHICS_ARRAY_INTERNAL_PALETTE_VALUE]
                                                        [0] = TEST_VIDEO_GRAPHICS_ARRAY_INTERNAL_PALETTE_RED;
    colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_GRAPHICS_ARRAY_INTERNAL_PALETTE_VALUE]
                                                        [1] = TEST_VIDEO_GRAPHICS_ARRAY_INTERNAL_PALETTE_GREEN;
    colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_GRAPHICS_ARRAY_INTERNAL_PALETTE_VALUE]
                                                        [2] = TEST_VIDEO_GRAPHICS_ARRAY_INTERNAL_PALETTE_BLUE;
    colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_GRAPHICS_ARRAY_COLOR_SELECT_PALETTE_VALUE]
                                                        [0] = TEST_VIDEO_GRAPHICS_ARRAY_COLOR_SELECT_RED;
    colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_GRAPHICS_ARRAY_COLOR_SELECT_PALETTE_VALUE]
                                                        [1] = TEST_VIDEO_GRAPHICS_ARRAY_COLOR_SELECT_GREEN;
    colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_GRAPHICS_ARRAY_COLOR_SELECT_PALETTE_VALUE]
                                                        [2] = TEST_VIDEO_GRAPHICS_ARRAY_COLOR_SELECT_BLUE;

    hyperdos_pc_video_services_render_planar_video_graphics_array_pixels(&colorGraphicsAdapter,
                                                                         0x12u,
                                                                         TEST_VIDEO_GRAPHICS_ARRAY_SOURCE_WIDTH,
                                                                         TEST_VIDEO_GRAPHICS_ARRAY_SOURCE_HEIGHT,
                                                                         pixels);
    assert(pixels[0] == test_pack_video_color(TEST_VIDEO_GRAPHICS_ARRAY_INTERNAL_PALETTE_RED,
                                              TEST_VIDEO_GRAPHICS_ARRAY_INTERNAL_PALETTE_GREEN,
                                              TEST_VIDEO_GRAPHICS_ARRAY_INTERNAL_PALETTE_BLUE));

    colorGraphicsAdapter.attributeControllerRegisters[TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_MODE_CONTROL_INDEX] =
            TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_COLOR_SELECT_MODE_BIT;
    colorGraphicsAdapter.attributeControllerRegisters[TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_COLOR_SELECT_INDEX] = 0u;
    hyperdos_pc_video_services_render_planar_video_graphics_array_pixels(&colorGraphicsAdapter,
                                                                         0x12u,
                                                                         TEST_VIDEO_GRAPHICS_ARRAY_SOURCE_WIDTH,
                                                                         TEST_VIDEO_GRAPHICS_ARRAY_SOURCE_HEIGHT,
                                                                         pixels);
    assert(pixels[0] == test_pack_video_color(TEST_VIDEO_GRAPHICS_ARRAY_COLOR_SELECT_RED,
                                              TEST_VIDEO_GRAPHICS_ARRAY_COLOR_SELECT_GREEN,
                                              TEST_VIDEO_GRAPHICS_ARRAY_COLOR_SELECT_BLUE));

    free(pixels);
}

static void test_video_graphics_array_default_palette_is_ega_compatible(void)
{
    enum
    {
        TEST_VIDEO_GRAPHICS_ARRAY_BROWN_ATTRIBUTE_INDEX = 0x06u,
        TEST_VIDEO_GRAPHICS_ARRAY_BROWN_DAC_INDEX       = 0x14u,
        TEST_VIDEO_GRAPHICS_ARRAY_SAMPLE_DAC_INDEX      = 0x31u
    };
    hyperdos_color_graphics_adapter colorGraphicsAdapter;

    hyperdos_color_graphics_adapter_initialize(&colorGraphicsAdapter);
    assert(colorGraphicsAdapter.attributeControllerRegisters[TEST_VIDEO_GRAPHICS_ARRAY_BROWN_ATTRIBUTE_INDEX] ==
           TEST_VIDEO_GRAPHICS_ARRAY_BROWN_DAC_INDEX);
    assert(colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_GRAPHICS_ARRAY_BROWN_DAC_INDEX][0] == 42u);
    assert(colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_GRAPHICS_ARRAY_BROWN_DAC_INDEX][1] == 21u);
    assert(colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_GRAPHICS_ARRAY_BROWN_DAC_INDEX][2] == 0u);
    assert(colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_GRAPHICS_ARRAY_SAMPLE_DAC_INDEX][0] == 21u);
    assert(colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_GRAPHICS_ARRAY_SAMPLE_DAC_INDEX][1] == 21u);
    assert(colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_GRAPHICS_ARRAY_SAMPLE_DAC_INDEX][2] == 42u);
}

static void test_video_graphics_array_memory_map_select(void)
{
    enum
    {
        TEST_VIDEO_GRAPHICS_ARRAY_TEXT_MEMORY_VALUE         = 0x5Au,
        TEST_VIDEO_GRAPHICS_ARRAY_IGNORED_TEXT_MEMORY_VALUE = 0xA5u
    };
    hyperdos_color_graphics_adapter colorGraphicsAdapter;

    hyperdos_color_graphics_adapter_initialize(&colorGraphicsAdapter);
    hyperdos_color_graphics_adapter_set_bios_video_mode(&colorGraphicsAdapter, 0x03u);
    hyperdos_color_graphics_adapter_write_memory_byte(&colorGraphicsAdapter,
                                                      HYPERDOS_COLOR_GRAPHICS_ADAPTER_TEXT_MEMORY_ADDRESS,
                                                      TEST_VIDEO_GRAPHICS_ARRAY_TEXT_MEMORY_VALUE);
    assert(colorGraphicsAdapter.memory[0] == TEST_VIDEO_GRAPHICS_ARRAY_TEXT_MEMORY_VALUE);

    hyperdos_color_graphics_adapter_set_bios_video_mode(&colorGraphicsAdapter, 0x12u);
    hyperdos_color_graphics_adapter_write_memory_byte(&colorGraphicsAdapter,
                                                      HYPERDOS_COLOR_GRAPHICS_ADAPTER_TEXT_MEMORY_ADDRESS,
                                                      TEST_VIDEO_GRAPHICS_ARRAY_IGNORED_TEXT_MEMORY_VALUE);
    assert(colorGraphicsAdapter.memory[0] == TEST_VIDEO_GRAPHICS_ARRAY_TEXT_MEMORY_VALUE);
}

static void test_video_graphics_array_chain_four_memory(void)
{
    enum
    {
        TEST_VIDEO_GRAPHICS_ARRAY_CHAIN_FOUR_MEMORY_OFFSET = 0x0005u,
        TEST_VIDEO_GRAPHICS_ARRAY_CHAIN_FOUR_PLANE_OFFSET  = 0x0001u,
        TEST_VIDEO_GRAPHICS_ARRAY_CHAIN_FOUR_PLANE_INDEX   = 1u,
        TEST_VIDEO_GRAPHICS_ARRAY_CHAIN_FOUR_VALUE         = 0x7Bu
    };
    hyperdos_color_graphics_adapter colorGraphicsAdapter;

    hyperdos_color_graphics_adapter_initialize(&colorGraphicsAdapter);
    hyperdos_color_graphics_adapter_set_bios_video_mode(&colorGraphicsAdapter, 0x13u);
    hyperdos_color_graphics_adapter_write_memory_byte(&colorGraphicsAdapter,
                                                      HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_ADDRESS +
                                                              TEST_VIDEO_GRAPHICS_ARRAY_CHAIN_FOUR_MEMORY_OFFSET,
                                                      TEST_VIDEO_GRAPHICS_ARRAY_CHAIN_FOUR_VALUE);

    assert(colorGraphicsAdapter.graphicsPlanes[TEST_VIDEO_GRAPHICS_ARRAY_CHAIN_FOUR_PLANE_INDEX]
                                              [TEST_VIDEO_GRAPHICS_ARRAY_CHAIN_FOUR_PLANE_OFFSET] ==
           TEST_VIDEO_GRAPHICS_ARRAY_CHAIN_FOUR_VALUE);
    assert(colorGraphicsAdapter.graphicsPlanes[0][TEST_VIDEO_GRAPHICS_ARRAY_CHAIN_FOUR_PLANE_OFFSET] == 0u);
    assert(colorGraphicsAdapter.graphicsPlanes[2][TEST_VIDEO_GRAPHICS_ARRAY_CHAIN_FOUR_PLANE_OFFSET] == 0u);
    assert(colorGraphicsAdapter.graphicsPlanes[3][TEST_VIDEO_GRAPHICS_ARRAY_CHAIN_FOUR_PLANE_OFFSET] == 0u);
    assert(hyperdos_color_graphics_adapter_read_memory_byte(
                   &colorGraphicsAdapter,
                   HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_ADDRESS + TEST_VIDEO_GRAPHICS_ARRAY_CHAIN_FOUR_MEMORY_OFFSET) ==
           TEST_VIDEO_GRAPHICS_ARRAY_CHAIN_FOUR_VALUE);
}

static void test_video_bios_set_mode_no_clear_bit_preserves_video_memory(void)
{
    enum
    {
        TEST_VIDEO_GRAPHICS_ARRAY_MODE_TWELVE_WITHOUT_CLEAR = 0x92u,
        TEST_VIDEO_GRAPHICS_ARRAY_MODE_TWELVE               = 0x12u,
        TEST_VIDEO_GRAPHICS_ARRAY_MEMORY_OFFSET             = 0x1234u,
        TEST_VIDEO_GRAPHICS_ARRAY_PLANE_ZERO_VALUE          = 0x11u,
        TEST_VIDEO_GRAPHICS_ARRAY_PLANE_ONE_VALUE           = 0x22u,
        TEST_VIDEO_GRAPHICS_ARRAY_PLANE_TWO_VALUE           = 0x44u,
        TEST_VIDEO_GRAPHICS_ARRAY_PLANE_THREE_VALUE         = 0x88u
    };
    static const uint8_t planeValues[HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT] = {
        TEST_VIDEO_GRAPHICS_ARRAY_PLANE_ZERO_VALUE,
        TEST_VIDEO_GRAPHICS_ARRAY_PLANE_ONE_VALUE,
        TEST_VIDEO_GRAPHICS_ARRAY_PLANE_TWO_VALUE,
        TEST_VIDEO_GRAPHICS_ARRAY_PLANE_THREE_VALUE,
    };
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;
    size_t                                 planeIndex = 0u;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));

    hyperdos_pc_video_services_set_video_mode(&machine->videoServices, TEST_VIDEO_GRAPHICS_ARRAY_MODE_TWELVE);
    for (planeIndex = 0u; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
    {
        machine->pc.colorGraphicsAdapter.graphicsPlanes
                [planeIndex][TEST_VIDEO_GRAPHICS_ARRAY_MEMORY_OFFSET] = planeValues[planeIndex];
    }

    hyperdos_pc_video_services_set_video_mode(&machine->videoServices,
                                              TEST_VIDEO_GRAPHICS_ARRAY_MODE_TWELVE_WITHOUT_CLEAR);
    assert(machine->videoServices.videoMode == TEST_VIDEO_GRAPHICS_ARRAY_MODE_TWELVE);
    for (planeIndex = 0u; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
    {
        assert(machine->pc.colorGraphicsAdapter.graphicsPlanes[planeIndex][TEST_VIDEO_GRAPHICS_ARRAY_MEMORY_OFFSET] ==
               planeValues[planeIndex]);
    }

    hyperdos_pc_video_services_set_video_mode(&machine->videoServices, TEST_VIDEO_GRAPHICS_ARRAY_MODE_TWELVE);
    for (planeIndex = 0u; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
    {
        assert(machine->pc.colorGraphicsAdapter.graphicsPlanes[planeIndex][TEST_VIDEO_GRAPHICS_ARRAY_MEMORY_OFFSET] ==
               0u);
    }

    free(machine);
}

static void test_video_graphics_array_standard_mode_registers(void)
{
    enum
    {
        TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_PORT  = 0x03CEu,
        TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_DATA_PORT   = 0x03CFu,
        TEST_VIDEO_GRAPHICS_ARRAY_COLOR_CRT_CONTROLLER_INDEX_PORT = 0x03D4u,
        TEST_VIDEO_GRAPHICS_ARRAY_COLOR_CRT_CONTROLLER_DATA_PORT  = 0x03D5u,
        TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MODE_INDEX             = 0x05u,
        TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MISCELLANEOUS_INDEX    = 0x06u,
        TEST_VIDEO_GRAPHICS_ARRAY_CRT_OFFSET_INDEX                = 0x13u,
        TEST_VIDEO_GRAPHICS_ARRAY_TEXT_MEMORY_MAP                 = 0x0Eu,
        TEST_VIDEO_GRAPHICS_ARRAY_CGA_GRAPHICS_MEMORY_MAP         = 0x0Du,
        TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MEMORY_MAP             = 0x05u,
        TEST_VIDEO_GRAPHICS_ARRAY_256_COLOR_SHIFT_MODE            = 0x40u,
        TEST_VIDEO_GRAPHICS_ARRAY_320_PIXEL_STRIDE_REGISTER       = 0x14u,
        TEST_VIDEO_GRAPHICS_ARRAY_640_PIXEL_STRIDE_REGISTER       = 0x28u
    };
    hyperdos_color_graphics_adapter colorGraphicsAdapter;

    hyperdos_color_graphics_adapter_initialize(&colorGraphicsAdapter);
    hyperdos_color_graphics_adapter_set_bios_video_mode(&colorGraphicsAdapter, 0x03u);
    hyperdos_color_graphics_adapter_write_input_output_byte(&colorGraphicsAdapter,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_PORT,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MISCELLANEOUS_INDEX);
    assert(hyperdos_color_graphics_adapter_read_input_output_byte(
                   &colorGraphicsAdapter,
                   TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_DATA_PORT) ==
           TEST_VIDEO_GRAPHICS_ARRAY_TEXT_MEMORY_MAP);
    assert(!hyperdos_color_graphics_adapter_graphics_mode_is_enabled(&colorGraphicsAdapter));

    hyperdos_color_graphics_adapter_set_bios_video_mode(&colorGraphicsAdapter, 0x04u);
    assert(hyperdos_color_graphics_adapter_read_input_output_byte(
                   &colorGraphicsAdapter,
                   TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_DATA_PORT) ==
           TEST_VIDEO_GRAPHICS_ARRAY_CGA_GRAPHICS_MEMORY_MAP);
    assert(hyperdos_color_graphics_adapter_graphics_mode_is_enabled(&colorGraphicsAdapter));

    hyperdos_color_graphics_adapter_set_bios_video_mode(&colorGraphicsAdapter, 0x12u);
    assert(hyperdos_color_graphics_adapter_read_input_output_byte(
                   &colorGraphicsAdapter,
                   TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_DATA_PORT) ==
           TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MEMORY_MAP);
    hyperdos_color_graphics_adapter_write_input_output_byte(&colorGraphicsAdapter,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_COLOR_CRT_CONTROLLER_INDEX_PORT,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_CRT_OFFSET_INDEX);
    assert(hyperdos_color_graphics_adapter_read_input_output_byte(
                   &colorGraphicsAdapter,
                   TEST_VIDEO_GRAPHICS_ARRAY_COLOR_CRT_CONTROLLER_DATA_PORT) ==
           TEST_VIDEO_GRAPHICS_ARRAY_640_PIXEL_STRIDE_REGISTER);
    assert(hyperdos_color_graphics_adapter_get_display_stride_byte_count(&colorGraphicsAdapter) == 80u);

    hyperdos_color_graphics_adapter_set_bios_video_mode(&colorGraphicsAdapter, 0x0Du);
    hyperdos_color_graphics_adapter_write_input_output_byte(&colorGraphicsAdapter,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_COLOR_CRT_CONTROLLER_INDEX_PORT,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_CRT_OFFSET_INDEX);
    assert(hyperdos_color_graphics_adapter_read_input_output_byte(
                   &colorGraphicsAdapter,
                   TEST_VIDEO_GRAPHICS_ARRAY_COLOR_CRT_CONTROLLER_DATA_PORT) ==
           TEST_VIDEO_GRAPHICS_ARRAY_320_PIXEL_STRIDE_REGISTER);

    hyperdos_color_graphics_adapter_set_bios_video_mode(&colorGraphicsAdapter, 0x13u);
    hyperdos_color_graphics_adapter_write_input_output_byte(&colorGraphicsAdapter,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_PORT,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MODE_INDEX);
    assert(hyperdos_color_graphics_adapter_read_input_output_byte(
                   &colorGraphicsAdapter,
                   TEST_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_DATA_PORT) ==
           TEST_VIDEO_GRAPHICS_ARRAY_256_COLOR_SHIFT_MODE);
}

static void test_video_graphics_array_compatibility_ports(void)
{
    enum
    {
        TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_ADDRESS_PORT               = 0x03C0u,
        TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_PALETTE_SOURCE_BIT         = 0x20u,
        TEST_VIDEO_GRAPHICS_ARRAY_MONOCHROME_CRT_CONTROLLER_INDEX_PORT = 0x03B4u,
        TEST_VIDEO_GRAPHICS_ARRAY_MONOCHROME_CRT_CONTROLLER_DATA_PORT  = 0x03B5u,
        TEST_VIDEO_GRAPHICS_ARRAY_MONOCHROME_STATUS_PORT               = 0x03BAu,
        TEST_VIDEO_GRAPHICS_ARRAY_COLOR_CRT_CONTROLLER_INDEX_PORT      = 0x03D4u,
        TEST_VIDEO_GRAPHICS_ARRAY_COLOR_CRT_CONTROLLER_DATA_PORT       = 0x03D5u,
        TEST_VIDEO_GRAPHICS_ARRAY_CRT_START_HIGH_INDEX                 = 0x0Cu,
        TEST_VIDEO_GRAPHICS_ARRAY_CRT_START_HIGH_VALUE                 = 0x12u,
        TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_COLOR_PLANE_ENABLE_INDEX   = 0x12u
    };
    hyperdos_color_graphics_adapter colorGraphicsAdapter;

    hyperdos_color_graphics_adapter_initialize(&colorGraphicsAdapter);
    hyperdos_color_graphics_adapter_write_input_output_byte(
            &colorGraphicsAdapter,
            TEST_VIDEO_GRAPHICS_ARRAY_MONOCHROME_CRT_CONTROLLER_INDEX_PORT,
            TEST_VIDEO_GRAPHICS_ARRAY_CRT_START_HIGH_INDEX);
    hyperdos_color_graphics_adapter_write_input_output_byte(
            &colorGraphicsAdapter,
            TEST_VIDEO_GRAPHICS_ARRAY_MONOCHROME_CRT_CONTROLLER_DATA_PORT,
            TEST_VIDEO_GRAPHICS_ARRAY_CRT_START_HIGH_VALUE);
    hyperdos_color_graphics_adapter_write_input_output_byte(&colorGraphicsAdapter,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_COLOR_CRT_CONTROLLER_INDEX_PORT,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_CRT_START_HIGH_INDEX);
    assert(hyperdos_color_graphics_adapter_read_input_output_byte(
                   &colorGraphicsAdapter,
                   TEST_VIDEO_GRAPHICS_ARRAY_COLOR_CRT_CONTROLLER_DATA_PORT) ==
           TEST_VIDEO_GRAPHICS_ARRAY_CRT_START_HIGH_VALUE);

    hyperdos_color_graphics_adapter_write_input_output_byte(&colorGraphicsAdapter,
                                                            TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_ADDRESS_PORT,
                                                            0u);
    (void)hyperdos_color_graphics_adapter_read_input_output_byte(&colorGraphicsAdapter,
                                                                 TEST_VIDEO_GRAPHICS_ARRAY_MONOCHROME_STATUS_PORT);
    hyperdos_color_graphics_adapter_write_input_output_byte(
            &colorGraphicsAdapter,
            TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_ADDRESS_PORT,
            TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_PALETTE_SOURCE_BIT |
                    TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_COLOR_PLANE_ENABLE_INDEX);
    assert(hyperdos_color_graphics_adapter_read_input_output_byte(&colorGraphicsAdapter,
                                                                  TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_ADDRESS_PORT) ==
           (TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_PALETTE_SOURCE_BIT |
            TEST_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_COLOR_PLANE_ENABLE_INDEX));
}

static uint32_t test_physical_address(uint16_t segment, uint16_t offset)
{
    return (((uint32_t)segment << 4u) + offset) & HYPERDOS_X86_ADDRESS_MASK;
}

static void test_code_segment_relative_far_call_pointer(void)
{
    static const uint8_t program[] = {
        0xBBu,
        0x00u,
        0x02u,
        0x2Eu,
        0xFFu,
        0x1Fu,
        0xF4u,
    };
    static const uint8_t expectedTargetProgram[] = {
        0xB8u,
        0xCDu,
        0xABu,
        0xCBu,
    };
    static const uint8_t incorrectTargetProgram[] = {
        0xB8u,
        0x34u,
        0x12u,
        0xCBu,
    };
    uint8_t*                      memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result                 = HYPERDOS_X86_EXECUTION_OK;
    size_t                        byteIndex              = 0;
    uint32_t                      programPhysicalAddress = test_physical_address(TEST_FAR_CALL_INITIAL_SEGMENT,
                                                            HYPERDOS_X86_DOS_PROGRAM_OFFSET);
    uint32_t pointerPhysicalAddress = test_physical_address(TEST_FAR_CALL_INITIAL_SEGMENT, TEST_FAR_POINTER_OFFSET);
    uint32_t expectedTargetPhysicalAddress  = test_physical_address(TEST_FAR_CALL_TARGET_SEGMENT,
                                                                   TEST_FAR_CALL_TARGET_OFFSET);
    uint32_t incorrectTargetPhysicalAddress = test_physical_address(TEST_FAR_CALL_TARGET_SEGMENT,
                                                                    TEST_FAR_CALL_INCORRECT_OFFSET);

    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE, TEST_FAR_CALL_INITIAL_SEGMENT);
    hyperdos_x86_set_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, TEST_FAR_CALL_INITIAL_SEGMENT);
    hyperdos_x86_set_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, TEST_FAR_CALL_INITIAL_SEGMENT);
    hyperdos_x86_set_segment_register(&processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, TEST_FAR_CALL_INITIAL_SEGMENT);
    hyperdos_x86_set_general_register(&processor,
                                      HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                      TEST_FAR_CALL_STACK_POINTER);
    hyperdos_x86_set_instruction_pointer_word(&processor, (uint16_t)(HYPERDOS_X86_DOS_PROGRAM_OFFSET));

    for (byteIndex = 0; byteIndex < sizeof(program); ++byteIndex)
    {
        memory[programPhysicalAddress + byteIndex] = program[byteIndex];
    }
    memory[pointerPhysicalAddress]      = (uint8_t)(TEST_FAR_CALL_TARGET_OFFSET & TEST_DOS_EXIT_CODE_MASK);
    memory[pointerPhysicalAddress + 1u] = (uint8_t)(TEST_FAR_CALL_TARGET_OFFSET >> TEST_DOS_SERVICE_REGISTER_SHIFT);
    memory[pointerPhysicalAddress + 2u] = (uint8_t)(TEST_FAR_CALL_TARGET_SEGMENT & TEST_DOS_EXIT_CODE_MASK);
    memory[pointerPhysicalAddress + 3u] = (uint8_t)(TEST_FAR_CALL_TARGET_SEGMENT >> TEST_DOS_SERVICE_REGISTER_SHIFT);

    for (byteIndex = 0; byteIndex < sizeof(expectedTargetProgram); ++byteIndex)
    {
        memory[expectedTargetPhysicalAddress + byteIndex] = expectedTargetProgram[byteIndex];
    }
    for (byteIndex = 0; byteIndex < sizeof(incorrectTargetProgram); ++byteIndex)
    {
        memory[incorrectTargetPhysicalAddress + byteIndex] = incorrectTargetProgram[byteIndex];
    }
    memory[test_physical_address(TEST_FAR_CALL_TARGET_SEGMENT,
                                 TEST_FAR_POINTER_OFFSET)] = (uint8_t)(TEST_FAR_CALL_INCORRECT_OFFSET &
                                                                       TEST_DOS_EXIT_CODE_MASK);
    memory[test_physical_address(TEST_FAR_CALL_TARGET_SEGMENT, TEST_FAR_POINTER_OFFSET) +
           1u] = (uint8_t)(TEST_FAR_CALL_INCORRECT_OFFSET >> TEST_DOS_SERVICE_REGISTER_SHIFT);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           TEST_FAR_CALL_EXPECTED_ACCUMULATOR);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) !=
           TEST_FAR_CALL_INCORRECT_ACCUMULATOR);
    free(memory);
}

static uint16_t test_read_memory_word(const hyperdos_x86_processor*       processor,
                                      hyperdos_x86_segment_register_index segmentRegister,
                                      uint16_t                            offset)
{
    uint8_t lowByte  = 0;
    uint8_t highByte = 0;

    assert(hyperdos_x86_read_memory_byte(processor, segmentRegister, offset, &lowByte) == HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_read_memory_byte(processor, segmentRegister, (uint16_t)(offset + 1u), &highByte) ==
           HYPERDOS_X86_EXECUTION_OK);
    return (uint16_t)(lowByte | ((uint16_t)highByte << 8u));
}

static uint8_t test_read_cmos_register(hyperdos_pc* pc, uint8_t registerNumber)
{
    hyperdos_bus_write_input_output_byte_if_mapped(&pc->bus, HYPERDOS_PC_CMOS_ADDRESS_PORT, registerNumber);
    return hyperdos_bus_read_input_output_byte_or_open_bus(&pc->bus, HYPERDOS_PC_CMOS_DATA_PORT);
}

static void test_write_memory_double_word(hyperdos_x86_processor*             processor,
                                          hyperdos_x86_segment_register_index segmentRegister,
                                          uint16_t                            offset,
                                          uint32_t                            value)
{
    size_t byteIndex = 0;

    for (byteIndex = 0; byteIndex < 4u; ++byteIndex)
    {
        assert(hyperdos_x86_write_memory_byte(processor,
                                              segmentRegister,
                                              (uint16_t)(offset + byteIndex),
                                              (uint8_t)(value >> (byteIndex * 8u))) == HYPERDOS_X86_EXECUTION_OK);
    }
}

static uint32_t test_read_memory_double_word(const hyperdos_x86_processor*       processor,
                                             hyperdos_x86_segment_register_index segmentRegister,
                                             uint16_t                            offset)
{
    uint32_t value     = 0;
    size_t   byteIndex = 0;

    for (byteIndex = 0; byteIndex < 4u; ++byteIndex)
    {
        uint8_t byteValue = 0;
        assert(hyperdos_x86_read_memory_byte(processor, segmentRegister, (uint16_t)(offset + byteIndex), &byteValue) ==
               HYPERDOS_X86_EXECUTION_OK);
        value |= (uint32_t)byteValue << (byteIndex * 8u);
    }
    return value;
}

static void test_wait_and_escape_coprocessor(void)
{
    static const uint8_t escapeProgram[] = {
        0xB8u,
        0x34u,
        0x12u,
        0x9Bu,
        0xD8u,
        0x06u,
        0x00u,
        0x02u,
        0xD9u,
        0xC0u,
        0xF4u,
    };
    uint8_t*                      memory = NULL;
    hyperdos_x86_processor        processor;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         escapeProgram,
                                         sizeof(escapeProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) == 0x1234u);
    assert(hyperdos_x86_get_instruction_pointer_word(&processor) ==
           HYPERDOS_X86_DOS_PROGRAM_OFFSET + sizeof(escapeProgram));
    free(memory);
}

static void test_8087_environment_instructions(void)
{
    static const uint8_t environmentProgram[] = {
        0xDBu, 0xE3u, 0xD9u, 0x3Eu, 0x00u, 0x02u, 0xC7u, 0x06u, 0x02u, 0x02u, 0x7Fu, 0x02u,
        0xD9u, 0x2Eu, 0x02u, 0x02u, 0xD9u, 0x3Eu, 0x04u, 0x02u, 0xDFu, 0xE0u, 0xF4u,
    };
    uint8_t*                      memory = NULL;
    hyperdos_x86_processor        processor;
    hyperdos_8087                 floatingPointUnit;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_8087_initialize(&floatingPointUnit);
    hyperdos_x86_attach_coprocessor(&processor, hyperdos_8087_wait, hyperdos_8087_escape, &floatingPointUnit);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         environmentProgram,
                                         sizeof(environmentProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, TEST_8087_CONTROL_WORD_OFFSET) ==
           HYPERDOS_8087_CONTROL_WORD_DEFAULT);
    assert(test_read_memory_word(&processor,
                                 HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                 TEST_8087_STORED_CONTROL_WORD_OFFSET) == 0x027Fu);
    assert(floatingPointUnit.controlWord == 0x027Fu);
    assert(floatingPointUnit.tagWord == HYPERDOS_8087_TAG_WORD_DEFAULT);
    assert(hyperdos_x86_get_general_register(&processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) ==
           HYPERDOS_8087_STATUS_WORD_DEFAULT);
    free(memory);
}

static void test_8087_single_precision_add(void)
{
    static const uint8_t arithmeticProgram[] = {
        0xD9u,
        0x06u,
        0x10u,
        0x02u,
        0xD8u,
        0x06u,
        0x14u,
        0x02u,
        0xD9u,
        0x1Eu,
        0x18u,
        0x02u,
        0xF4u,
    };
    uint8_t*                      memory = NULL;
    hyperdos_x86_processor        processor;
    hyperdos_8087                 floatingPointUnit;
    hyperdos_x86_execution_result result            = HYPERDOS_X86_EXECUTION_OK;
    float                         firstRealValue    = 1.5f;
    float                         secondRealValue   = 2.25f;
    float                         resultRealValue   = 0.0f;
    uint32_t                      firstBinaryValue  = 0;
    uint32_t                      secondBinaryValue = 0;
    uint32_t                      resultBinaryValue = 0;

    memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_8087_initialize(&floatingPointUnit);
    hyperdos_x86_attach_coprocessor(&processor, hyperdos_8087_wait, hyperdos_8087_escape, &floatingPointUnit);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         arithmeticProgram,
                                         sizeof(arithmeticProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    memcpy(&firstBinaryValue, &firstRealValue, sizeof(firstBinaryValue));
    memcpy(&secondBinaryValue, &secondRealValue, sizeof(secondBinaryValue));
    test_write_memory_double_word(&processor,
                                  HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                  TEST_8087_FIRST_REAL_OFFSET,
                                  firstBinaryValue);
    test_write_memory_double_word(&processor,
                                  HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                  TEST_8087_SECOND_REAL_OFFSET,
                                  secondBinaryValue);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    resultBinaryValue = test_read_memory_double_word(&processor,
                                                     HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                                     TEST_8087_RESULT_REAL_OFFSET);
    memcpy(&resultRealValue, &resultBinaryValue, sizeof(resultRealValue));
    assert(resultRealValue == 3.75f);
    assert(floatingPointUnit.tagWord == HYPERDOS_8087_TAG_WORD_DEFAULT);
    free(memory);
}

static void test_80287_set_protected_mode_instruction(void)
{
    static const uint8_t setProtectedModeProgram[] = {
        0xDBu,
        0xE4u,
        0xF4u,
    };
    uint8_t*                      memory = NULL;
    hyperdos_x86_processor        processor;
    hyperdos_8087                 floatingPointUnit;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    hyperdos_x87_initialize(&floatingPointUnit, HYPERDOS_X87_MODEL_80287);
    hyperdos_x86_attach_coprocessor(&processor, hyperdos_x87_wait, hyperdos_x87_escape, &floatingPointUnit);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         setProtectedModeProgram,
                                         sizeof(setProtectedModeProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x87_get_model(&floatingPointUnit) == HYPERDOS_X87_MODEL_80287);
    assert(hyperdos_x87_is_protected_mode_enabled(&floatingPointUnit));
    free(memory);
}

static void test_80287_initialize_instruction_preserves_protected_mode(void)
{
    static const uint8_t initializeProgram[] = {
        0xDBu,
        0xE4u,
        0xDBu,
        0xE3u,
        0xF4u,
    };
    uint8_t*                      memory = NULL;
    hyperdos_x86_processor        processor;
    hyperdos_8087                 floatingPointUnit;
    hyperdos_x86_execution_result result = HYPERDOS_X86_EXECUTION_OK;

    memory = (uint8_t*)calloc(HYPERDOS_X86_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_initialize_processor(&processor, memory, HYPERDOS_X86_MEMORY_SIZE) ==
           HYPERDOS_X86_EXECUTION_OK);
    hyperdos_x86_set_processor_model(&processor, HYPERDOS_X86_PROCESSOR_MODEL_80286);
    hyperdos_x87_initialize(&floatingPointUnit, HYPERDOS_X87_MODEL_80287);
    hyperdos_x86_attach_coprocessor(&processor, hyperdos_x87_wait, hyperdos_x87_escape, &floatingPointUnit);
    assert(hyperdos_x86_load_dos_program(&processor,
                                         initializeProgram,
                                         sizeof(initializeProgram),
                                         HYPERDOS_X86_DEFAULT_DOS_SEGMENT,
                                         "",
                                         0u) == HYPERDOS_X86_EXECUTION_OK);

    result = hyperdos_x86_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_EXECUTION_HALTED);
    assert(hyperdos_x87_is_protected_mode_enabled(&floatingPointUnit));
    assert(floatingPointUnit.controlWord == HYPERDOS_8087_CONTROL_WORD_DEFAULT);
    assert(floatingPointUnit.statusWord == HYPERDOS_8087_STATUS_WORD_DEFAULT);
    assert(floatingPointUnit.tagWord == HYPERDOS_8087_TAG_WORD_DEFAULT);
    hyperdos_x87_initialize(&floatingPointUnit, HYPERDOS_X87_MODEL_80287);
    assert(!hyperdos_x87_is_protected_mode_enabled(&floatingPointUnit));
    free(memory);
}

static void test_pc_text_code_pages(void)
{
    assert(hyperdos_pc_text_code_page_437_unicode_character('A') == 'A');
    assert(hyperdos_pc_text_code_page_437_unicode_character(0x05u) == 0x2663u);
    assert(hyperdos_pc_text_code_page_437_unicode_character(0xB3u) == 0x2502u);
    assert(hyperdos_pc_text_code_page_437_unicode_character(0xDBu) == 0x2588u);
    assert(!hyperdos_pc_text_korean_code_page_949_is_lead_byte(0x80u));
    assert(hyperdos_pc_text_korean_code_page_949_is_lead_byte(0x81u));
    assert(hyperdos_pc_text_korean_code_page_949_is_lead_byte(0xFEu));
    assert(!hyperdos_pc_text_korean_code_page_949_is_trail_byte(0x40u));
    assert(hyperdos_pc_text_korean_code_page_949_is_trail_byte(0x41u));
    assert(hyperdos_pc_text_korean_code_page_949_is_trail_byte(0xFEu));
}

static void test_pc_machine_initializes_core_devices(void)
{
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));
    assert(machine->diskBiosInterface.pc == &machine->pc);
    assert(machine->diskBiosInterface.floppyController == &machine->floppyController);
    assert(machine->videoServices.pc == &machine->pc);
    assert(machine->videoBiosInterface.pc == &machine->pc);
    assert(machine->biosRuntime.pc == &machine->pc);
    assert(machine->biosRuntime.systemBios == &machine->systemBios);
    assert(machine->biosRuntime.keyboardBios == &machine->keyboardBios);
    assert(machine->biosRuntime.diskBiosInterface == &machine->diskBiosInterface);
    assert(machine->biosRuntime.videoBiosInterface == &machine->videoBiosInterface);
    assert(hyperdos_pc_system_bios_get_model_identifier(&machine->systemBios) ==
           HYPERDOS_PC_SYSTEM_BIOS_MODEL_IDENTIFIER_XT);
    free(machine);
}

static void test_pc_machine_selects_80287_for_80286_coprocessor(void)
{
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    configuration.processorModel     = HYPERDOS_X86_PROCESSOR_MODEL_80286;
    configuration.pcModel            = HYPERDOS_PC_MODEL_AT;
    configuration.coprocessorEnabled = 1u;
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));
    assert(hyperdos_x87_get_model(&machine->pc.floatingPointUnit) == HYPERDOS_X87_MODEL_80287);
    assert(machine->biosRuntime.coprocessorEnabled != 0u);
    free(machine);
}

static void test_pc_machine_uses_explicit_coprocessor_model(void)
{
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    configuration.processorModel   = HYPERDOS_X86_PROCESSOR_MODEL_80286;
    configuration.pcModel          = HYPERDOS_PC_MODEL_AT;
    configuration.coprocessorModel = HYPERDOS_X87_MODEL_8087;
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));
    assert(hyperdos_x87_get_model(&machine->pc.floatingPointUnit) == HYPERDOS_X87_MODEL_8087);
    assert(machine->biosRuntime.coprocessorEnabled != 0u);
    free(machine);
}

static void test_pc_system_bios_identity_can_disable_at_services(void)
{
    enum
    {
        TEST_SYSTEM_BIOS_MODEL_IDENTIFIER_ADDRESS = 0xFFFFEu,
        TEST_SYSTEM_BIOS_CONFIGURATION_SERVICE    = 0xC0u,
        TEST_SYSTEM_BIOS_UNSUPPORTED_STATUS       = 0x86u
    };
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;
    hyperdos_x86_processor*                processor       = NULL;
    hyperdos_x86_execution_result          result          = HYPERDOS_X86_EXECUTION_OK;
    uint8_t                                modelIdentifier = 0u;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    configuration.pcModel = HYPERDOS_PC_MODEL_AT;
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));

    hyperdos_pc_system_bios_install_identification(&machine->pc, &machine->systemBios);
    modelIdentifier = hyperdos_bus_read_memory_byte_or_open_bus(&machine->pc.bus,
                                                                TEST_SYSTEM_BIOS_MODEL_IDENTIFIER_ADDRESS);
    assert(modelIdentifier == HYPERDOS_PC_SYSTEM_BIOS_MODEL_IDENTIFIER_AT);

    hyperdos_pc_system_bios_set_model_identifier(&machine->systemBios, HYPERDOS_PC_SYSTEM_BIOS_MODEL_IDENTIFIER_XT);
    hyperdos_pc_system_bios_install_identification(&machine->pc, &machine->systemBios);
    modelIdentifier = hyperdos_bus_read_memory_byte_or_open_bus(&machine->pc.bus,
                                                                TEST_SYSTEM_BIOS_MODEL_IDENTIFIER_ADDRESS);
    assert(modelIdentifier == HYPERDOS_PC_SYSTEM_BIOS_MODEL_IDENTIFIER_XT);

    processor = &machine->pc.processor;
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)(TEST_SYSTEM_BIOS_CONFIGURATION_SERVICE
                                                      << TEST_DOS_SERVICE_REGISTER_SHIFT));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(HYPERDOS_X86_FLAG_RESERVED));
    result = hyperdos_pc_system_bios_handle_system_services_interrupt(processor,
                                                                      &machine->pc,
                                                                      &machine->systemBios,
                                                                      TEST_SYSTEM_BIOS_CONFIGURATION_SERVICE);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) != 0u);
    assert((hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) >>
            TEST_DOS_SERVICE_REGISTER_SHIFT) == TEST_SYSTEM_BIOS_UNSUPPORTED_STATUS);

    free(machine);
}

static void test_pc_system_bios_configuration_table_survives_interrupt_vector_stubs(void)
{
    enum
    {
        TEST_SYSTEM_BIOS_CONFIGURATION_SERVICE      = 0xC0u,
        TEST_SYSTEM_BIOS_CONFIGURATION_TABLE_LENGTH = 0x08u,
        TEST_SYSTEM_BIOS_CONFIGURATION_MODEL_OFFSET = 0x02u
    };
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;
    hyperdos_x86_processor*                processor                         = NULL;
    hyperdos_x86_execution_result          result                            = HYPERDOS_X86_EXECUTION_OK;
    uint32_t                               configurationTablePhysicalAddress = 0u;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    configuration.pcModel = HYPERDOS_PC_MODEL_AT;
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));

    hyperdos_pc_system_bios_install_configuration_table(&machine->pc, &machine->systemBios);
    hyperdos_pc_bios_install_interrupt_vector_stubs(&machine->pc);

    processor = &machine->pc.processor;
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)(TEST_SYSTEM_BIOS_CONFIGURATION_SERVICE
                                                      << TEST_DOS_SERVICE_REGISTER_SHIFT));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(HYPERDOS_X86_FLAG_RESERVED | HYPERDOS_X86_FLAG_CARRY));
    result = hyperdos_pc_system_bios_handle_system_services_interrupt(processor,
                                                                      &machine->pc,
                                                                      &machine->systemBios,
                                                                      TEST_SYSTEM_BIOS_CONFIGURATION_SERVICE);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) == 0u);

    assert(hyperdos_x86_translate_logical_to_physical_address(
                   processor,
                   HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                   hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE),
                   &configurationTablePhysicalAddress) == HYPERDOS_X86_EXECUTION_OK);
    assert(hyperdos_bus_read_memory_byte_or_open_bus(&machine->pc.bus, configurationTablePhysicalAddress) ==
           TEST_SYSTEM_BIOS_CONFIGURATION_TABLE_LENGTH);
    assert(hyperdos_bus_read_memory_byte_or_open_bus(&machine->pc.bus,
                                                     configurationTablePhysicalAddress +
                                                             TEST_SYSTEM_BIOS_CONFIGURATION_MODEL_OFFSET) ==
           HYPERDOS_PC_SYSTEM_BIOS_MODEL_IDENTIFIER_AT);

    free(machine);
}

static void test_pc_storage_maps_configured_bios_drive_numbers(void)
{
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;
    hyperdos_pc_storage_floppy_drive       floppyDrives[2];
    hyperdos_pc_storage_fixed_disk_drive   fixedDiskDrives[2];
    hyperdos_pc_storage_context            storageContext;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    memset(floppyDrives, 0, sizeof(floppyDrives));
    memset(fixedDiskDrives, 0, sizeof(fixedDiskDrives));
    configuration.floppyDriveCount    = 2u;
    configuration.fixedDiskDriveCount = 2u;
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));

    hyperdos_pc_storage_context_initialize(&storageContext,
                                           &machine->pc,
                                           &machine->floppyController,
                                           floppyDrives,
                                           2u,
                                           fixedDiskDrives,
                                           2u,
                                           NULL,
                                           NULL);
    hyperdos_pc_storage_install_floppy_drive(&storageContext, 0u);
    hyperdos_pc_storage_install_floppy_drive(&storageContext, 1u);
    hyperdos_pc_storage_install_fixed_disk_drive(&storageContext, 1u);

    floppyDrives[0].diskImage.inserted       = 1u;
    floppyDrives[0].diskImage.driveNumber    = 0x00u;
    floppyDrives[1].diskImage.inserted       = 1u;
    floppyDrives[1].diskImage.driveNumber    = 0x01u;
    fixedDiskDrives[1].diskImage.inserted    = 1u;
    fixedDiskDrives[1].diskImage.isHardDisk  = 1u;
    fixedDiskDrives[1].diskImage.driveNumber = 0x81u;

    assert(hyperdos_pc_storage_count_installed_floppy_drives(&storageContext) == 2u);
    assert(hyperdos_pc_storage_count_inserted_fixed_disks(&storageContext) == 1u);
    assert(hyperdos_pc_storage_get_disk_for_bios_drive_number(&storageContext, 0x00u) == &floppyDrives[0].diskImage);
    assert(hyperdos_pc_storage_get_disk_for_bios_drive_number(&storageContext, 0x01u) == &floppyDrives[1].diskImage);
    assert(hyperdos_pc_storage_get_disk_for_bios_drive_number(&storageContext, 0x81u) == &fixedDiskDrives[1].diskImage);
    assert(hyperdos_pc_storage_get_disk_for_bios_drive_number(&storageContext, 0x82u) == NULL);
    free(machine);
}

static void test_disk_bios_reports_only_available_fixed_disks(void)
{
    enum
    {
        TEST_DISK_BIOS_DRIVE_PARAMETERS_SERVICE = 0x08u,
        TEST_DISK_BIOS_STATUS_INVALID_FUNCTION  = 0x01u,
        TEST_FIXED_DISK_BYTES_PER_SECTOR        = 512u,
        TEST_FIXED_DISK_SECTORS_PER_TRACK       = 17u,
        TEST_FIXED_DISK_HEAD_COUNT              = 4u,
        TEST_FIXED_DISK_CYLINDER_COUNT          = 615u,
        TEST_FIXED_DISK_SECTOR_COUNT            = TEST_FIXED_DISK_SECTORS_PER_TRACK * TEST_FIXED_DISK_HEAD_COUNT *
                                       TEST_FIXED_DISK_CYLINDER_COUNT
    };
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;
    hyperdos_pc_storage_floppy_drive       floppyDrives[1];
    hyperdos_pc_storage_fixed_disk_drive   fixedDiskDrives[2];
    hyperdos_pc_storage_context            storageContext;
    hyperdos_x86_processor*                processor = NULL;
    hyperdos_x86_execution_result          result    = HYPERDOS_X86_EXECUTION_OK;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    memset(floppyDrives, 0, sizeof(floppyDrives));
    memset(fixedDiskDrives, 0, sizeof(fixedDiskDrives));
    memset(&storageContext, 0, sizeof(storageContext));
    configuration.userContext         = &storageContext;
    configuration.floppyDriveCount    = 1u;
    configuration.fixedDiskDriveCount = 2u;
    configuration.getDiskImage        = test_get_storage_disk_bios_disk_image;
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));

    hyperdos_pc_storage_context_initialize(&storageContext,
                                           &machine->pc,
                                           &machine->floppyController,
                                           floppyDrives,
                                           1u,
                                           fixedDiskDrives,
                                           2u,
                                           NULL,
                                           NULL);
    fixedDiskDrives[0].diskImage.inserted        = 1u;
    fixedDiskDrives[0].diskImage.isHardDisk      = 1u;
    fixedDiskDrives[0].diskImage.driveNumber     = HYPERDOS_PC_DISK_BIOS_HARD_DISK_DRIVE_NUMBER;
    fixedDiskDrives[0].diskImage.bytesPerSector  = TEST_FIXED_DISK_BYTES_PER_SECTOR;
    fixedDiskDrives[0].diskImage.sectorsPerTrack = TEST_FIXED_DISK_SECTORS_PER_TRACK;
    fixedDiskDrives[0].diskImage.headCount       = TEST_FIXED_DISK_HEAD_COUNT;
    fixedDiskDrives[0].diskImage.cylinderCount   = TEST_FIXED_DISK_CYLINDER_COUNT;
    fixedDiskDrives[0].diskImage.sectorCount     = TEST_FIXED_DISK_SECTOR_COUNT;
    hyperdos_pc_storage_install_fixed_disk_drive(&storageContext, 0u);

    processor = &machine->pc.processor;
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)(TEST_DISK_BIOS_DRIVE_PARAMETERS_SERVICE
                                                      << TEST_DOS_SERVICE_REGISTER_SHIFT));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                           (uint16_t)(HYPERDOS_PC_DISK_BIOS_HARD_DISK_DRIVE_NUMBER));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(HYPERDOS_X86_FLAG_RESERVED | HYPERDOS_X86_FLAG_CARRY));
    result = hyperdos_pc_disk_bios_handle_interrupt(processor, &machine->diskBiosInterface);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) == 0u);
    assert((hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA) & TEST_BYTE_MASK) ==
           1u);

    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)(TEST_DISK_BIOS_DRIVE_PARAMETERS_SERVICE
                                                      << TEST_DOS_SERVICE_REGISTER_SHIFT));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                           (uint16_t)(HYPERDOS_PC_DISK_BIOS_HARD_DISK_DRIVE_NUMBER + 1u));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(HYPERDOS_X86_FLAG_RESERVED));
    result = hyperdos_pc_disk_bios_handle_interrupt(processor, &machine->diskBiosInterface);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) != 0u);
    assert((hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) >>
            TEST_DOS_SERVICE_REGISTER_SHIFT) == TEST_DISK_BIOS_STATUS_INVALID_FUNCTION);

    free(machine);
}

static void test_pc_storage_updates_at_cmos_drive_configuration(void)
{
    enum
    {
        TEST_CMOS_FLOPPY_TYPE_REGISTER                         = 0x10u,
        TEST_CMOS_FIXED_DISK_TYPE_REGISTER                     = 0x12u,
        TEST_CMOS_EQUIPMENT_REGISTER                           = 0x14u,
        TEST_CMOS_FIRST_FIXED_DISK_EXTENDED_TYPE_REGISTER      = 0x19u,
        TEST_CMOS_SECOND_FIXED_DISK_EXTENDED_TYPE_REGISTER     = 0x1Au,
        TEST_CMOS_FIRST_FIXED_DISK_CYLINDER_LOW_REGISTER       = 0x1Bu,
        TEST_CMOS_FIRST_FIXED_DISK_CYLINDER_HIGH_REGISTER      = 0x1Cu,
        TEST_CMOS_FIRST_FIXED_DISK_HEAD_COUNT_REGISTER         = 0x1Du,
        TEST_CMOS_FIRST_FIXED_DISK_CONTROL_REGISTER            = 0x20u,
        TEST_CMOS_FIRST_FIXED_DISK_SECTORS_PER_TRACK_REGISTER  = 0x23u,
        TEST_CMOS_SECOND_FIXED_DISK_CYLINDER_LOW_REGISTER      = 0x24u,
        TEST_CMOS_SECOND_FIXED_DISK_CYLINDER_HIGH_REGISTER     = 0x25u,
        TEST_CMOS_SECOND_FIXED_DISK_HEAD_COUNT_REGISTER        = 0x26u,
        TEST_CMOS_SECOND_FIXED_DISK_CONTROL_REGISTER           = 0x29u,
        TEST_CMOS_SECOND_FIXED_DISK_SECTORS_PER_TRACK_REGISTER = 0x2Cu
    };
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;
    hyperdos_pc_storage_floppy_drive       floppyDrives[2];
    hyperdos_pc_storage_fixed_disk_drive   fixedDiskDrives[2];
    hyperdos_pc_storage_context            storageContext;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    memset(floppyDrives, 0, sizeof(floppyDrives));
    memset(fixedDiskDrives, 0, sizeof(fixedDiskDrives));
    configuration.pcModel             = HYPERDOS_PC_MODEL_AT;
    configuration.floppyDriveCount    = 2u;
    configuration.fixedDiskDriveCount = 2u;
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));

    hyperdos_pc_storage_context_initialize(&storageContext,
                                           &machine->pc,
                                           &machine->floppyController,
                                           floppyDrives,
                                           2u,
                                           fixedDiskDrives,
                                           2u,
                                           NULL,
                                           NULL);

    floppyDrives[0].diskImage.inserted        = 1u;
    floppyDrives[0].diskImage.headCount       = 2u;
    floppyDrives[0].diskImage.sectorsPerTrack = 18u;
    floppyDrives[0].diskImage.cylinderCount   = 80u;
    floppyDrives[1].diskImage.inserted        = 1u;
    floppyDrives[1].diskImage.headCount       = 2u;
    floppyDrives[1].diskImage.sectorsPerTrack = 9u;
    floppyDrives[1].diskImage.cylinderCount   = 40u;
    hyperdos_pc_storage_install_floppy_drive(&storageContext, 0u);
    hyperdos_pc_storage_install_floppy_drive(&storageContext, 1u);
    assert(test_read_cmos_register(&machine->pc, TEST_CMOS_FLOPPY_TYPE_REGISTER) == 0x41u);

    fixedDiskDrives[0].diskImage.inserted        = 1u;
    fixedDiskDrives[0].diskImage.isHardDisk      = 1u;
    fixedDiskDrives[0].diskImage.cylinderCount   = 615u;
    fixedDiskDrives[0].diskImage.headCount       = 4u;
    fixedDiskDrives[0].diskImage.sectorsPerTrack = 17u;
    fixedDiskDrives[1].diskImage.inserted        = 1u;
    fixedDiskDrives[1].diskImage.isHardDisk      = 1u;
    fixedDiskDrives[1].diskImage.cylinderCount   = 1024u;
    fixedDiskDrives[1].diskImage.headCount       = 16u;
    fixedDiskDrives[1].diskImage.sectorsPerTrack = 63u;
    hyperdos_pc_storage_install_fixed_disk_drive(&storageContext, 0u);
    hyperdos_pc_storage_install_fixed_disk_drive(&storageContext, 1u);

    assert(test_read_cmos_register(&machine->pc, TEST_CMOS_FIXED_DISK_TYPE_REGISTER) == 0xFFu);
    assert(test_read_cmos_register(&machine->pc, TEST_CMOS_FIRST_FIXED_DISK_EXTENDED_TYPE_REGISTER) ==
           HYPERDOS_PC_CMOS_USER_DEFINED_FIXED_DISK_TYPE);
    assert(test_read_cmos_register(&machine->pc, TEST_CMOS_SECOND_FIXED_DISK_EXTENDED_TYPE_REGISTER) ==
           HYPERDOS_PC_CMOS_USER_DEFINED_FIXED_DISK_TYPE);
    assert(test_read_cmos_register(&machine->pc, TEST_CMOS_FIRST_FIXED_DISK_CYLINDER_LOW_REGISTER) == 0x67u);
    assert(test_read_cmos_register(&machine->pc, TEST_CMOS_FIRST_FIXED_DISK_CYLINDER_HIGH_REGISTER) == 0x02u);
    assert(test_read_cmos_register(&machine->pc, TEST_CMOS_FIRST_FIXED_DISK_HEAD_COUNT_REGISTER) == 4u);
    assert(test_read_cmos_register(&machine->pc, TEST_CMOS_FIRST_FIXED_DISK_CONTROL_REGISTER) == 0xC0u);
    assert(test_read_cmos_register(&machine->pc, TEST_CMOS_FIRST_FIXED_DISK_SECTORS_PER_TRACK_REGISTER) == 17u);
    assert(test_read_cmos_register(&machine->pc, TEST_CMOS_SECOND_FIXED_DISK_CYLINDER_LOW_REGISTER) == 0x00u);
    assert(test_read_cmos_register(&machine->pc, TEST_CMOS_SECOND_FIXED_DISK_CYLINDER_HIGH_REGISTER) == 0x04u);
    assert(test_read_cmos_register(&machine->pc, TEST_CMOS_SECOND_FIXED_DISK_HEAD_COUNT_REGISTER) == 16u);
    assert(test_read_cmos_register(&machine->pc, TEST_CMOS_SECOND_FIXED_DISK_CONTROL_REGISTER) == 0xC8u);
    assert(test_read_cmos_register(&machine->pc, TEST_CMOS_SECOND_FIXED_DISK_SECTORS_PER_TRACK_REGISTER) == 63u);

    hyperdos_pc_bios_runtime_initialize_data_area(&machine->biosRuntime,
                                                  &floppyDrives[0].diskImage,
                                                  hyperdos_pc_storage_count_inserted_fixed_disks(&storageContext));
    assert(test_read_cmos_register(&machine->pc, TEST_CMOS_EQUIPMENT_REGISTER) == 0x45u);
    free(machine);
}

static void test_disk_bios_reset_preserves_floppy_media_change_until_read(void)
{
    enum
    {
        TEST_DISK_IMAGE_BYTES_PER_SECTOR  = 512u,
        TEST_DISK_IMAGE_SECTORS_PER_TRACK = 9u,
        TEST_DISK_IMAGE_HEAD_COUNT        = 2u,
        TEST_DISK_IMAGE_CYLINDER_COUNT    = 1u,
        TEST_DISK_TRANSFER_OFFSET         = 0x0500u,
        TEST_DISK_SAMPLE_BYTE             = 0xA5u,
        TEST_DISK_BIOS_STATUS_SUCCESS     = 0x00u,
        TEST_DISK_BIOS_STATUS_CHANGED     = 0x06u
    };
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;
    hyperdos_pc_disk_image                 diskImage;
    test_disk_bios_context                 diskBiosContext;
    hyperdos_x86_processor*                processor          = NULL;
    size_t                                 diskImageByteCount = 0u;
    uint8_t*                               diskImageBytes     = NULL;
    hyperdos_x86_execution_result          result             = HYPERDOS_X86_EXECUTION_OK;

    memset(&diskImage, 0, sizeof(diskImage));
    memset(&diskBiosContext, 0, sizeof(diskBiosContext));
    diskImageByteCount = TEST_DISK_IMAGE_BYTES_PER_SECTOR * TEST_DISK_IMAGE_SECTORS_PER_TRACK *
                         TEST_DISK_IMAGE_HEAD_COUNT * TEST_DISK_IMAGE_CYLINDER_COUNT;
    diskImageBytes = (uint8_t*)calloc(diskImageByteCount, 1u);
    assert(diskImageBytes != NULL);
    diskImageBytes[0] = TEST_DISK_SAMPLE_BYTE;
    assert(hyperdos_pc_disk_image_initialize_memory_floppy(&diskImage,
                                                           "test-floppy.img",
                                                           diskImageBytes,
                                                           diskImageByteCount,
                                                           0u));
    diskImage.sectorsPerTrack     = TEST_DISK_IMAGE_SECTORS_PER_TRACK;
    diskImage.headCount           = TEST_DISK_IMAGE_HEAD_COUNT;
    diskImage.cylinderCount       = TEST_DISK_IMAGE_CYLINDER_COUNT;
    diskImage.mediaChanged        = 1u;
    diskImage.mediaChangeReported = 0u;
    diskBiosContext.diskImage     = &diskImage;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    configuration.userContext  = &diskBiosContext;
    configuration.getDiskImage = test_get_disk_bios_disk_image;
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));
    hyperdos_pc_bios_runtime_initialize_data_area(&machine->biosRuntime, &diskImage, 0u);
    hyperdos_pc_floppy_controller_notify_disk_changed(&machine->floppyController);

    processor = &machine->pc.processor;
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, (uint16_t)(0x0000u));
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA, (uint16_t)(0x0000u));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(HYPERDOS_X86_FLAG_CARRY));
    result = hyperdos_pc_disk_bios_handle_interrupt(processor, &machine->diskBiosInterface);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) == 0u);
    assert(diskImage.mediaChanged == 1u);
    assert(diskImage.mediaChangeReported == 0u);
    assert(hyperdos_pc_floppy_controller_drive_disk_changed(&machine->floppyController,
                                                            HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER));
    assert(hyperdos_pc_disk_bios_get_disk_operation_status(&machine->pc, HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER) ==
           TEST_DISK_BIOS_STATUS_SUCCESS);

    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, (uint16_t)(0x0201u));
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER, (uint16_t)(0x0001u));
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA, (uint16_t)(0x0000u));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_BASE,
                                           (uint16_t)(TEST_DISK_TRANSFER_OFFSET));
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, (uint16_t)(0u));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(HYPERDOS_X86_FLAG_CARRY));
    result = hyperdos_pc_disk_bios_handle_interrupt(processor, &machine->diskBiosInterface);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) != 0u);
    assert((hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) >>
            TEST_DOS_SERVICE_REGISTER_SHIFT) == TEST_DISK_BIOS_STATUS_CHANGED);
    assert(diskImage.mediaChanged == 0u);
    assert(diskImage.mediaChangeReported == 1u);
    assert(!hyperdos_pc_floppy_controller_drive_disk_changed(&machine->floppyController,
                                                             HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER));

    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, (uint16_t)(0x0201u));
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER, (uint16_t)(0x0001u));
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA, (uint16_t)(0x0000u));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_BASE,
                                           (uint16_t)(TEST_DISK_TRANSFER_OFFSET));
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, (uint16_t)(0u));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(HYPERDOS_X86_FLAG_CARRY));
    result = hyperdos_pc_disk_bios_handle_interrupt(processor, &machine->diskBiosInterface);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) == 0u);
    assert(machine->pc.processorMemory[TEST_DISK_TRANSFER_OFFSET] == TEST_DISK_SAMPLE_BYTE);

    free(machine);
    hyperdos_pc_disk_image_free(&diskImage);
}

static void test_disk_bios_xt_reports_floppy_change_line_for_disk_swaps(void)
{
    enum
    {
        TEST_DISK_BIOS_SERVICE_REGISTER_SHIFT  = 8u,
        TEST_DISK_BIOS_GET_TYPE_SERVICE        = 0x15u,
        TEST_DISK_BIOS_CHANGE_LINE_SERVICE     = 0x16u,
        TEST_DISK_BIOS_READ_SECTORS_SERVICE    = 0x02u,
        TEST_DISK_BIOS_CHANGED_STATUS          = 0x06u,
        TEST_DISK_BIOS_FLOPPY_WITH_CHANGE_LINE = 0x02u,
        TEST_DISK_IMAGE_BYTES_PER_SECTOR       = 512u,
        TEST_DISK_IMAGE_SECTORS_PER_TRACK      = 9u,
        TEST_DISK_IMAGE_HEAD_COUNT             = 2u,
        TEST_DISK_IMAGE_CYLINDER_COUNT         = 1u,
        TEST_DISK_TRANSFER_OFFSET              = 0x0500u,
        TEST_DISK_SAMPLE_BYTE                  = 0x5Au
    };
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;
    hyperdos_pc_disk_image                 diskImage;
    test_disk_bios_context                 diskBiosContext;
    hyperdos_x86_processor*                processor          = NULL;
    size_t                                 diskImageByteCount = 0u;
    uint8_t*                               diskImageBytes     = NULL;
    hyperdos_x86_execution_result          result             = HYPERDOS_X86_EXECUTION_OK;

    memset(&diskImage, 0, sizeof(diskImage));
    memset(&diskBiosContext, 0, sizeof(diskBiosContext));
    diskImageByteCount = TEST_DISK_IMAGE_BYTES_PER_SECTOR * TEST_DISK_IMAGE_SECTORS_PER_TRACK *
                         TEST_DISK_IMAGE_HEAD_COUNT * TEST_DISK_IMAGE_CYLINDER_COUNT;
    diskImageBytes = (uint8_t*)calloc(diskImageByteCount, 1u);
    assert(diskImageBytes != NULL);
    diskImageBytes[0] = TEST_DISK_SAMPLE_BYTE;
    assert(hyperdos_pc_disk_image_initialize_memory_floppy(&diskImage,
                                                           "test-floppy.img",
                                                           diskImageBytes,
                                                           diskImageByteCount,
                                                           0u));
    diskImage.sectorsPerTrack     = TEST_DISK_IMAGE_SECTORS_PER_TRACK;
    diskImage.headCount           = TEST_DISK_IMAGE_HEAD_COUNT;
    diskImage.cylinderCount       = TEST_DISK_IMAGE_CYLINDER_COUNT;
    diskImage.mediaChanged        = 1u;
    diskImage.mediaChangeReported = 0u;
    diskBiosContext.diskImage     = &diskImage;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    configuration.userContext  = &diskBiosContext;
    configuration.getDiskImage = test_get_disk_bios_disk_image;
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));
    hyperdos_pc_bios_runtime_initialize_data_area(&machine->biosRuntime, &diskImage, 0u);
    hyperdos_pc_floppy_controller_notify_disk_changed(&machine->floppyController);
    assert(machine->diskBiosInterface.floppyDiskChangeLineSupported != 0u);

    processor = &machine->pc.processor;
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)(TEST_DISK_BIOS_GET_TYPE_SERVICE
                                                      << TEST_DISK_BIOS_SERVICE_REGISTER_SHIFT));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                           (uint16_t)(HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(HYPERDOS_X86_FLAG_RESERVED));
    result = hyperdos_pc_disk_bios_handle_interrupt(processor, &machine->diskBiosInterface);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) == 0u);
    assert((hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) >>
            TEST_DISK_BIOS_SERVICE_REGISTER_SHIFT) == TEST_DISK_BIOS_FLOPPY_WITH_CHANGE_LINE);

    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)(TEST_DISK_BIOS_CHANGE_LINE_SERVICE
                                                      << TEST_DISK_BIOS_SERVICE_REGISTER_SHIFT));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                           (uint16_t)(HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(HYPERDOS_X86_FLAG_RESERVED));
    result = hyperdos_pc_disk_bios_handle_interrupt(processor, &machine->diskBiosInterface);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) != 0u);
    assert((hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) >>
            TEST_DISK_BIOS_SERVICE_REGISTER_SHIFT) == TEST_DISK_BIOS_CHANGED_STATUS);
    assert(diskImage.mediaChanged == 0u);
    assert(diskImage.mediaChangeReported == 1u);
    assert(!hyperdos_pc_floppy_controller_drive_disk_changed(&machine->floppyController,
                                                             HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER));

    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)((TEST_DISK_BIOS_READ_SECTORS_SERVICE
                                                       << TEST_DISK_BIOS_SERVICE_REGISTER_SHIFT) |
                                                      1u));
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER, (uint16_t)(0x0001u));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                           (uint16_t)(HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_BASE,
                                           (uint16_t)(TEST_DISK_TRANSFER_OFFSET));
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, (uint16_t)(0u));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(HYPERDOS_X86_FLAG_CARRY));
    result = hyperdos_pc_disk_bios_handle_interrupt(processor, &machine->diskBiosInterface);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) == 0u);
    assert(machine->pc.processorMemory[TEST_DISK_TRANSFER_OFFSET] == TEST_DISK_SAMPLE_BYTE);

    free(machine);
    hyperdos_pc_disk_image_free(&diskImage);
}

static void test_video_bios_palette_services_update_video_graphics_array_state(void)
{
    enum
    {
        TEST_VIDEO_BIOS_SERVICE_REGISTER_SHIFT            = 8u,
        TEST_VIDEO_BIOS_PALETTE_SERVICE                   = 0x10u,
        TEST_VIDEO_BIOS_PALETTE_SET_ALL_REGISTERS         = 0x02u,
        TEST_VIDEO_BIOS_PALETTE_SET_SINGLE_DAC_REGISTER   = 0x10u,
        TEST_VIDEO_BIOS_PALETTE_SET_DAC_REGISTER_BLOCK    = 0x12u,
        TEST_VIDEO_BIOS_PALETTE_TABLE_OFFSET              = 0x0500u,
        TEST_VIDEO_BIOS_DAC_TABLE_OFFSET                  = 0x0600u,
        TEST_VIDEO_BIOS_PALETTE_REGISTER_COUNT            = 16u,
        TEST_VIDEO_BIOS_ATTRIBUTE_OVERSCAN_COLOR_REGISTER = 0x11u,
        TEST_VIDEO_BIOS_FIRST_DAC_REGISTER                = 2u,
        TEST_VIDEO_BIOS_SINGLE_DAC_REGISTER               = 4u,
        TEST_VIDEO_BIOS_SINGLE_DAC_RED                    = 0x15u,
        TEST_VIDEO_BIOS_SINGLE_DAC_GREEN                  = 0x25u,
        TEST_VIDEO_BIOS_SINGLE_DAC_BLUE                   = 0x35u
    };
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;
    hyperdos_x86_processor*                processor     = NULL;
    size_t                                 registerIndex = 0u;
    hyperdos_x86_execution_result          result        = HYPERDOS_X86_EXECUTION_OK;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));
    processor = &machine->pc.processor;
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0u);

    for (registerIndex = 0u; registerIndex < TEST_VIDEO_BIOS_PALETTE_REGISTER_COUNT + 1u; ++registerIndex)
    {
        machine->pc.processorMemory[TEST_VIDEO_BIOS_PALETTE_TABLE_OFFSET + registerIndex] = (uint8_t)(0x20u +
                                                                                                      registerIndex);
    }
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)((TEST_VIDEO_BIOS_PALETTE_SERVICE
                                                       << TEST_VIDEO_BIOS_SERVICE_REGISTER_SHIFT) |
                                                      TEST_VIDEO_BIOS_PALETTE_SET_ALL_REGISTERS));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                           (uint16_t)(TEST_VIDEO_BIOS_PALETTE_TABLE_OFFSET));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(HYPERDOS_X86_FLAG_CARRY));
    result = hyperdos_pc_video_bios_handle_interrupt(processor,
                                                     &machine->videoBiosInterface,
                                                     TEST_VIDEO_BIOS_PALETTE_SERVICE);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) == 0u);
    for (registerIndex = 0u; registerIndex < TEST_VIDEO_BIOS_PALETTE_REGISTER_COUNT; ++registerIndex)
    {
        assert(machine->pc.colorGraphicsAdapter.attributeControllerRegisters[registerIndex] ==
               (uint8_t)(0x20u + registerIndex));
    }
    assert(machine->pc.colorGraphicsAdapter.attributeControllerRegisters
                   [TEST_VIDEO_BIOS_ATTRIBUTE_OVERSCAN_COLOR_REGISTER] == 0x30u);

    machine->pc.processorMemory[TEST_VIDEO_BIOS_DAC_TABLE_OFFSET]      = 0x01u;
    machine->pc.processorMemory[TEST_VIDEO_BIOS_DAC_TABLE_OFFSET + 1u] = 0x02u;
    machine->pc.processorMemory[TEST_VIDEO_BIOS_DAC_TABLE_OFFSET + 2u] = 0x03u;
    machine->pc.processorMemory[TEST_VIDEO_BIOS_DAC_TABLE_OFFSET + 3u] = 0x04u;
    machine->pc.processorMemory[TEST_VIDEO_BIOS_DAC_TABLE_OFFSET + 4u] = 0x05u;
    machine->pc.processorMemory[TEST_VIDEO_BIOS_DAC_TABLE_OFFSET + 5u] = 0x06u;
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)((TEST_VIDEO_BIOS_PALETTE_SERVICE
                                                       << TEST_VIDEO_BIOS_SERVICE_REGISTER_SHIFT) |
                                                      TEST_VIDEO_BIOS_PALETTE_SET_DAC_REGISTER_BLOCK));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_BASE,
                                           (uint16_t)(TEST_VIDEO_BIOS_FIRST_DAC_REGISTER));
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER, (uint16_t)(2u));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                           (uint16_t)(TEST_VIDEO_BIOS_DAC_TABLE_OFFSET));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(HYPERDOS_X86_FLAG_CARRY));
    result = hyperdos_pc_video_bios_handle_interrupt(processor,
                                                     &machine->videoBiosInterface,
                                                     TEST_VIDEO_BIOS_PALETTE_SERVICE);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) == 0u);
    assert(machine->pc.colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_BIOS_FIRST_DAC_REGISTER][0] ==
           0x01u);
    assert(machine->pc.colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_BIOS_FIRST_DAC_REGISTER][1] ==
           0x02u);
    assert(machine->pc.colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_BIOS_FIRST_DAC_REGISTER][2] ==
           0x03u);
    assert(machine->pc.colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_BIOS_FIRST_DAC_REGISTER + 1u]
                                                                           [0] == 0x04u);
    assert(machine->pc.colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_BIOS_FIRST_DAC_REGISTER + 1u]
                                                                           [1] == 0x05u);
    assert(machine->pc.colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_BIOS_FIRST_DAC_REGISTER + 1u]
                                                                           [2] == 0x06u);

    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)((TEST_VIDEO_BIOS_PALETTE_SERVICE
                                                       << TEST_VIDEO_BIOS_SERVICE_REGISTER_SHIFT) |
                                                      TEST_VIDEO_BIOS_PALETTE_SET_SINGLE_DAC_REGISTER));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_BASE,
                                           (uint16_t)(TEST_VIDEO_BIOS_SINGLE_DAC_REGISTER));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
                                           (uint16_t)((TEST_VIDEO_BIOS_SINGLE_DAC_GREEN
                                                       << HYPERDOS_X86_BYTE_BIT_COUNT) |
                                                      TEST_VIDEO_BIOS_SINGLE_DAC_BLUE));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                           (uint16_t)(TEST_VIDEO_BIOS_SINGLE_DAC_RED << HYPERDOS_X86_BYTE_BIT_COUNT));
    hyperdos_x86_set_flags_word(processor, (uint16_t)(HYPERDOS_X86_FLAG_CARRY));
    result = hyperdos_pc_video_bios_handle_interrupt(processor,
                                                     &machine->videoBiosInterface,
                                                     TEST_VIDEO_BIOS_PALETTE_SERVICE);
    assert(result == HYPERDOS_X86_EXECUTION_OK);
    assert((hyperdos_x86_get_flags_word(processor) & HYPERDOS_X86_FLAG_CARRY) == 0u);
    assert(machine->pc.colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_BIOS_SINGLE_DAC_REGISTER][0] ==
           TEST_VIDEO_BIOS_SINGLE_DAC_RED);
    assert(machine->pc.colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_BIOS_SINGLE_DAC_REGISTER][1] ==
           TEST_VIDEO_BIOS_SINGLE_DAC_GREEN);
    assert(machine->pc.colorGraphicsAdapter.digitalToAnalogConverterPalette[TEST_VIDEO_BIOS_SINGLE_DAC_REGISTER][2] ==
           TEST_VIDEO_BIOS_SINGLE_DAC_BLUE);

    free(machine);
}

int main(void)
{
    test_configure_assertion_reporting();

    test_dos_program_output();
    test_loop_and_arithmetic();
    test_shift_rotate_8086_forms();
    test_80186_only_operation_codes_are_unsupported();
    test_80186_shift_rotate_immediate_count();
    test_80186_shift_rotate_immediate_matrix();
    test_segment_boundary_word_write_policy();
    test_push_stack_pointer_model_policy();
    test_80186_basic_stack_instructions();
    test_80186_immediate_signed_multiply();
    test_80186_bound_instruction();
    test_80186_enter_instruction();
    test_80186_input_output_string_instructions();
    test_8086_pop_code_segment_operation_code();
    test_80186_unused_operation_code_interrupt();
    test_80186_escape_trap_interrupt();
    test_80186_relocation_register_escape_trap_bit();
    test_80186_fault_interrupts_do_not_dispatch_single_step();
    test_8086_flags_reserved_bits_are_fixed();
    test_80286_flags_high_bits_are_zero();
    test_80286_real_mode_physical_address_uses_24_bits();
    test_80286_descriptor_table_instructions();
    test_80286_descriptor_table_instruction_segment_override();
    test_80286_machine_status_word_instructions();
    test_80286_machine_status_word_can_enter_protected_mode();
    test_80286_protected_mode_segment_descriptor_loads();
    test_80286_protected_mode_interrupt_gate_dispatch();
    test_80286_protected_system_instructions();
    test_80286_protected_mode_pop_flags_preserves_system_bits_at_privilege_zero();
    test_80286_direct_far_jump_uses_protected_code_descriptor();
    test_80286_direct_far_call_pushes_protected_return_address();
    test_80286_call_gate_transfers_to_same_privilege_code();
    test_80286_io_privilege_violation_dispatches_general_protection_fault();
    test_80286_interrupt_to_inner_privilege_switches_stack();
    test_80286_call_gate_to_inner_privilege_switches_stack_and_copies_parameters();
    test_80286_far_return_to_same_privilege_uses_protected_descriptor();
    test_80286_far_return_to_outer_privilege_restores_stack();
    test_80286_interrupt_return_to_outer_privilege_restores_stack();
    test_80286_far_jump_to_task_state_segment_switches_task();
    test_80286_task_gate_call_and_nested_task_return();
    test_80286_software_interrupt_checks_gate_privilege();
    test_80286_protected_data_limit_fault_pushes_error_code();
    test_80286_exception_delivery_fault_dispatches_double_fault();
    test_80286_double_fault_delivery_fault_shuts_down_processor();
    test_80286_not_present_segment_error_code_masks_requested_privilege_level();
    test_80286_external_interrupt_descriptor_fault_sets_external_error_code();
    test_80286_direct_far_call_stack_fault_keeps_stack_pointer();
    test_80286_task_register_invalid_selector_dispatches_general_protection_fault();
    test_80286_local_descriptor_table_not_present_dispatches_segment_not_present_fault();
    test_80286_processor_extension_faults();
    test_80286_processor_extension_trap_precedes_coprocessor_handler();
    test_80286_wait_uses_coprocessor_when_task_switched_without_monitor_processor();
    test_external_bus_cycle_count_profiles();
    test_x86_general_register_word_operations_preserve_upper_word();
    test_load_data_segment_pointer();
    test_repeated_byte_move();
    test_bus_connected_input_output();
    test_intel_8042_keyboard_controller();
    test_intel_8042_auxiliary_mouse();
    test_cascaded_programmable_interrupt_controller();
    test_pointing_device_bios_services();
    test_pointing_device_bios_callback_from_auxiliary_interrupt();
    test_bios_does_not_install_mouse_software_interrupt();
    test_keyboard_bios_status_return_flags();
    test_keyboard_bios_status_return_flags_through_firmware_stub();
    test_keyboard_bios_read_waits_with_interrupts_enabled();
    test_programmable_interrupt_controller_blocks_same_priority_reentry();
    test_programmable_interrupt_controller_specific_end_of_interrupt();
    test_programmable_interval_timer_rate_generator_transition();
    test_pc_default_interval_timer_transition();
    test_pc_interval_timer_input_frequency_is_independent_from_processor_frequency();
    test_pc_speaker_callback_from_port_control();
    test_pc_system_bios_timer_tick_data_area();
    test_pc_system_bios_identity_can_disable_at_services();
    test_pc_system_bios_configuration_table_survives_interrupt_vector_stubs();
    test_pc_storage_maps_configured_bios_drive_numbers();
    test_disk_bios_reports_only_available_fixed_disks();
    test_pc_storage_updates_at_cmos_drive_configuration();
    test_disk_bios_reset_preserves_floppy_media_change_until_read();
    test_disk_bios_xt_reports_floppy_change_line_for_disk_swaps();
    test_video_bios_palette_services_update_video_graphics_array_state();
    test_maskable_external_interrupt_request();
    test_maskable_interrupt_waits_for_interrupt_shadow();
    test_halt_resumes_through_maskable_interrupt();
    test_trap_flag_dispatches_single_step_interrupt();
    test_decimal_and_ascii_adjust_instructions();
    test_ascii_adjust_multiply_divide();
    test_signed_divide_minimum_negative_quotient_policy();
    test_divide_error_uses_interrupt_vector();
    test_divide_error_can_return_to_faulting_instruction();
    test_translate_byte_respects_segment_override();
    test_repeated_store_respects_memory_mapped_video();
    test_repeated_move_respects_memory_mapped_video();
    test_video_graphics_array_plane_memory();
    test_video_graphics_array_write_mode_one_copies_latches();
    test_video_graphics_array_write_mode_zero_uses_existing_latches();
    test_video_graphics_array_observer_does_not_reload_latches();
    test_video_graphics_array_attribute_palette_uses_internal_high_bits();
    test_video_graphics_array_default_palette_is_ega_compatible();
    test_video_graphics_array_memory_map_select();
    test_video_graphics_array_chain_four_memory();
    test_video_bios_set_mode_no_clear_bit_preserves_video_memory();
    test_video_graphics_array_standard_mode_registers();
    test_video_graphics_array_compatibility_ports();
    test_code_segment_relative_far_call_pointer();
    test_wait_and_escape_coprocessor();
    test_8087_environment_instructions();
    test_8087_single_precision_add();
    test_80287_set_protected_mode_instruction();
    test_80287_initialize_instruction_preserves_protected_mode();
    test_pc_text_code_pages();
    test_pc_machine_initializes_core_devices();
    test_pc_machine_selects_80287_for_80286_coprocessor();
    test_pc_machine_uses_explicit_coprocessor_model();
    return 0;
}
