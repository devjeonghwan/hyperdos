#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <stdint.h>
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
#include "hyperdos/x86_16_processor.h"

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
    TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_OFFSET                        = 0x0123u,
    TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_SEGMENT                       = 0x2345u,
    TEST_DIVIDE_ERROR_RETURN_OFFSET                                  = 0x010Bu,
    TEST_DIVIDE_ERROR_FAULT_OFFSET                                   = 0x0109u,
    TEST_REPEATED_MOVE_SOURCE_OFFSET                                 = 0x0120u,
    TEST_REPEATED_MOVE_DESTINATION_OFFSET                            = 0x0130u,
    TEST_SHIFT_ROTATE_MEMORY_OFFSET                                  = 0x0200u,
    TEST_SHIFT_ROTATE_BYTE_RESULT                                    = 0x0Cu,
    TEST_SHIFT_ROTATE_WORD_RESULT                                    = 0x0010u,
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

static hyperdos_x86_16_execution_result test_dos_interrupt_handler(hyperdos_x86_16_processor* processor,
                                                                   uint8_t                    interruptNumber,
                                                                   void*                      userContext)
{
    test_dos_context* dosContext = (test_dos_context*)userContext;

    if (interruptNumber == HYPERDOS_X86_16_DOS_INTERRUPT_TERMINATE_PROGRAM)
    {
        dosContext->exitCode = 0;
        hyperdos_x86_16_stop_processor(processor);
        return HYPERDOS_X86_16_EXECUTION_OK;
    }

    if (interruptNumber == HYPERDOS_X86_16_DOS_INTERRUPT_FUNCTION_DISPATCH)
    {
        uint8_t serviceNumber = (uint8_t)(processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] >>
                                          TEST_DOS_SERVICE_REGISTER_SHIFT);
        if (serviceNumber == HYPERDOS_X86_16_DOS_SERVICE_PRINT_STRING)
        {
            uint16_t offset = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA];
            for (;;)
            {
                uint8_t value = 0;
                assert(hyperdos_x86_16_read_memory_byte(processor,
                                                        HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                                        offset,
                                                        &value) == HYPERDOS_X86_16_EXECUTION_OK);
                if (value == HYPERDOS_X86_16_DOS_PRINT_STRING_TERMINATOR)
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
            return HYPERDOS_X86_16_EXECUTION_OK;
        }

        if (serviceNumber == HYPERDOS_X86_16_DOS_SERVICE_TERMINATE_PROCESS)
        {
            dosContext->exitCode = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] &
                                   TEST_DOS_EXIT_CODE_MASK;
            hyperdos_x86_16_stop_processor(processor);
            return HYPERDOS_X86_16_EXECUTION_OK;
        }
    }

    return HYPERDOS_X86_16_EXECUTION_INTERRUPT_NOT_HANDLED;
}

static void test_dos_program_output(void)
{
    uint8_t*                         memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor        processor;
    test_dos_context                 dosContext;
    hyperdos_x86_16_execution_result result = HYPERDOS_X86_16_EXECUTION_OK;

    memset(&dosContext, 0, sizeof(dosContext));
    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    hyperdos_x86_16_set_interrupt_handler(&processor, test_dos_interrupt_handler, &dosContext);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            sampleDosProgram,
                                            sizeof(sampleDosProgram),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);

    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
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
    uint8_t*                         memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor        processor;
    hyperdos_x86_16_execution_result result = HYPERDOS_X86_16_EXECUTION_OK;

    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            program,
                                            sizeof(program),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);

    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert(hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR) == 5u);
    assert(hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER) == 0u);
    free(memory);
}

static void test_shift_rotate_8086_forms(void)
{
    static const uint8_t program[] = {
        0xC6u, 0x06u, 0x00u, 0x02u, 0x03u, 0xD0u, 0x26u, 0x00u, 0x02u, 0xD0u, 0x26u, 0x00u,
        0x02u, 0xA0u, 0x00u, 0x02u, 0xBBu, 0x01u, 0x00u, 0xB1u, 0x04u, 0xD3u, 0xE3u, 0xF4u,
    };
    uint8_t*                         memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor        processor;
    hyperdos_x86_16_execution_result result     = HYPERDOS_X86_16_EXECUTION_OK;
    uint8_t                          memoryByte = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            program,
                                            sizeof(program),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);

    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert((hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR) &
            TEST_BYTE_MASK) == TEST_SHIFT_ROTATE_BYTE_RESULT);
    assert(hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_BASE) ==
           TEST_SHIFT_ROTATE_WORD_RESULT);
    assert(hyperdos_x86_16_read_memory_byte(&processor,
                                            HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                            TEST_SHIFT_ROTATE_MEMORY_OFFSET,
                                            &memoryByte) == HYPERDOS_X86_16_EXECUTION_OK);
    assert(memoryByte == TEST_SHIFT_ROTATE_BYTE_RESULT);
    free(memory);
}

static void test_80186_only_operation_codes_are_unsupported(void)
{
    static const uint8_t unsupportedOperationCodes[] = {
        0xC0u,
        0xC1u,
        0xC8u,
        0xC9u,
    };
    size_t operationCodeIndex = 0u;

    for (operationCodeIndex = 0u;
         operationCodeIndex < sizeof(unsupportedOperationCodes) / sizeof(unsupportedOperationCodes[0]);
         ++operationCodeIndex)
    {
        uint8_t                          program[] = {unsupportedOperationCodes[operationCodeIndex], 0xF4u};
        uint8_t*                         memory    = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
        hyperdos_x86_16_processor        processor;
        hyperdos_x86_16_execution_result result = HYPERDOS_X86_16_EXECUTION_OK;

        assert(memory != NULL);
        assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
               HYPERDOS_X86_16_EXECUTION_OK);
        assert(hyperdos_x86_16_load_dos_program(&processor,
                                                program,
                                                sizeof(program),
                                                HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                                "",
                                                0u) == HYPERDOS_X86_16_EXECUTION_OK);

        result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
        assert(result == HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION);
        free(memory);
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
    uint8_t*                         memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor        processor;
    hyperdos_x86_16_execution_result result = HYPERDOS_X86_16_EXECUTION_OK;

    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    hyperdos_x86_16_set_processor_model(&processor, HYPERDOS_X86_16_PROCESSOR_MODEL_80186);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            program,
                                            sizeof(program),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);

    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert(hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_DATA) == 0x0004u);
    free(memory);
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
    uint8_t*                  memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor processor;

    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            program,
                                            sizeof(program),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);

    assert(hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert(hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR) ==
           HYPERDOS_X86_16_FLAG_RESERVED);
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
    uint8_t*                         memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor        processor;
    hyperdos_x86_16_execution_result result = HYPERDOS_X86_16_EXECUTION_OK;

    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            program,
                                            sizeof(program),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_write_memory_byte(&processor,
                                             HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                             TEST_LOAD_SEGMENT_POINTER_MEMORY_OFFSET,
                                             (uint8_t)(TEST_LOAD_SEGMENT_POINTER_OFFSET_VALUE & TEST_BYTE_MASK)) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_write_memory_byte(&processor,
                                             HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                             (uint16_t)(TEST_LOAD_SEGMENT_POINTER_MEMORY_OFFSET + 1u),
                                             (uint8_t)(TEST_LOAD_SEGMENT_POINTER_OFFSET_VALUE >>
                                                       TEST_DOS_SERVICE_REGISTER_SHIFT)) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_write_memory_byte(&processor,
                                             HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                             (uint16_t)(TEST_LOAD_SEGMENT_POINTER_MEMORY_OFFSET + TEST_WORD_BYTE_COUNT),
                                             (uint8_t)(TEST_LOAD_SEGMENT_POINTER_SEGMENT_VALUE & TEST_BYTE_MASK)) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_write_memory_byte(&processor,
                                             HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                             (uint16_t)(TEST_LOAD_SEGMENT_POINTER_MEMORY_OFFSET + TEST_WORD_BYTE_COUNT +
                                                        1u),
                                             (uint8_t)(TEST_LOAD_SEGMENT_POINTER_SEGMENT_VALUE >>
                                                       TEST_DOS_SERVICE_REGISTER_SHIFT)) ==
           HYPERDOS_X86_16_EXECUTION_OK);

    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert(hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_BASE) ==
           TEST_LOAD_SEGMENT_POINTER_OFFSET_VALUE);
    assert(hyperdos_x86_16_get_segment_register(&processor, HYPERDOS_X86_16_SEGMENT_REGISTER_DATA) ==
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
    static const uint8_t             sourceBytes[] = {'A', 'B', 'C', 'D', 'E'};
    uint8_t*                         memory        = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor        processor;
    hyperdos_x86_16_execution_result result    = HYPERDOS_X86_16_EXECUTION_OK;
    size_t                           byteIndex = 0;

    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            program,
                                            sizeof(program),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);

    for (byteIndex = 0; byteIndex < sizeof(sourceBytes); ++byteIndex)
    {
        assert(hyperdos_x86_16_write_memory_byte(&processor,
                                                 HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                                 (uint16_t)(TEST_REPEATED_MOVE_SOURCE_OFFSET + byteIndex),
                                                 sourceBytes[byteIndex]) == HYPERDOS_X86_16_EXECUTION_OK);
    }

    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);

    for (byteIndex = 0; byteIndex < sizeof(sourceBytes); ++byteIndex)
    {
        uint8_t value = 0;
        assert(hyperdos_x86_16_read_memory_byte(&processor,
                                                HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA,
                                                (uint16_t)(TEST_REPEATED_MOVE_DESTINATION_OFFSET + byteIndex),
                                                &value) == HYPERDOS_X86_16_EXECUTION_OK);
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
    uint8_t*                                   memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor                  processor;
    hyperdos_bus                               bus;
    hyperdos_random_access_memory              randomAccessMemory;
    hyperdos_programmable_peripheral_interface peripheralInterface;
    hyperdos_x86_16_execution_result           result = HYPERDOS_X86_16_EXECUTION_OK;

    assert(memory != NULL);
    hyperdos_bus_initialize(&bus);
    hyperdos_random_access_memory_initialize(&randomAccessMemory, memory, HYPERDOS_X86_16_MEMORY_SIZE, 0u);
    hyperdos_programmable_peripheral_interface_initialize(&peripheralInterface);
    assert(hyperdos_bus_map_memory(&bus,
                                   0u,
                                   HYPERDOS_X86_16_MEMORY_SIZE,
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
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    hyperdos_x86_16_attach_bus(&processor, &bus);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            program,
                                            sizeof(program),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);

    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert((hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR) &
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
    hyperdos_x86_16_processor*             processor = NULL;
    hyperdos_x86_16_execution_result       result    = HYPERDOS_X86_16_EXECUTION_OK;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    configuration.pcModel = HYPERDOS_PC_MODEL_AT;
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));
    processor = &machine->pc.processor;

    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = 0xC207u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE]        = 0x5678u;
    hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA, 0x1234u);
    result = hyperdos_pc_system_bios_handle_system_services_interrupt(processor,
                                                                      &machine->pc,
                                                                      &machine->systemBios,
                                                                      0xC2u);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_CARRY) == 0u);
    assert(machine->systemBios.pointingDeviceHandlerOffset == 0x5678u);
    assert(machine->systemBios.pointingDeviceHandlerSegment == 0x1234u);

    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = 0xC200u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE]        = 0x0100u;
    result = hyperdos_pc_system_bios_handle_system_services_interrupt(processor,
                                                                      &machine->pc,
                                                                      &machine->systemBios,
                                                                      0xC2u);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_CARRY) == 0u);
    assert(machine->systemBios.pointingDeviceEnabled != 0u);
    assert(machine->pc.keyboardController.auxiliaryDeviceReportingEnabled != 0u);
    assert((machine->pc.programmableInterruptController.interruptMaskRegister & TEST_MASTER_CASCADE_INTERRUPT_MASK) ==
           0u);
    assert((machine->pc.slaveProgrammableInterruptController.interruptMaskRegister &
            TEST_AUXILIARY_DEVICE_INTERRUPT_MASK) == 0u);

    machine->pc.keyboardController.auxiliaryDeviceButtonMask                  = TEST_AUXILIARY_MOUSE_LEFT_BUTTON;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = 0xC206u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE]        = 0x0000u;
    result = hyperdos_pc_system_bios_handle_system_services_interrupt(processor,
                                                                      &machine->pc,
                                                                      &machine->systemBios,
                                                                      0xC2u);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_CARRY) == 0u);
    assert((processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE] &
            TEST_AUXILIARY_MOUSE_STATUS_LEFT_BUTTON) != 0u);
    assert((processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE] & TEST_AUXILIARY_MOUSE_LEFT_BUTTON) ==
           0u);

    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = 0xC200u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE]        = 0x0000u;
    result = hyperdos_pc_system_bios_handle_system_services_interrupt(processor,
                                                                      &machine->pc,
                                                                      &machine->systemBios,
                                                                      0xC2u);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_CARRY) == 0u);
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
    hyperdos_x86_16_processor*             processor = NULL;
    hyperdos_x86_16_execution_result       result    = HYPERDOS_X86_16_EXECUTION_OK;
    uint32_t programPhysicalAddress                  = (uint32_t)TEST_PROGRAM_SEGMENT << HYPERDOS_X86_16_SEGMENT_SHIFT;
    uint32_t callbackPhysicalAddress                 = (uint32_t)TEST_CALLBACK_SEGMENT << HYPERDOS_X86_16_SEGMENT_SHIFT;
    uint32_t resultPhysicalAddress                   = ((uint32_t)TEST_DATA_SEGMENT << HYPERDOS_X86_16_SEGMENT_SHIFT) +
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
    hyperdos_x86_16_set_interrupt_handler(processor, hyperdos_pc_bios_runtime_handle_interrupt, &machine->biosRuntime);
    hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_CODE, TEST_PROGRAM_SEGMENT);
    hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_DATA, TEST_DATA_SEGMENT);
    hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_STACK, TEST_STACK_SEGMENT);
    processor->instructionPointer                                               = 0u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER] = TEST_PROGRAM_STACK_POINTER;
    processor->flags = HYPERDOS_X86_16_FLAG_RESERVED | HYPERDOS_X86_16_FLAG_INTERRUPT_ENABLE;
    memcpy(&machine->pc.processorMemory[programPhysicalAddress], programBytes, sizeof(programBytes));
    memcpy(&machine->pc.processorMemory[callbackPhysicalAddress], callbackBytes, sizeof(callbackBytes));

    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = 0xC207u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE]        = 0u;
    hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA, TEST_CALLBACK_SEGMENT);
    result = hyperdos_pc_system_bios_handle_system_services_interrupt(processor,
                                                                      &machine->pc,
                                                                      &machine->systemBios,
                                                                      0xC2u);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_CARRY) == 0u);

    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = 0xC200u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE]        = 0x0100u;
    result = hyperdos_pc_system_bios_handle_system_services_interrupt(processor,
                                                                      &machine->pc,
                                                                      &machine->systemBios,
                                                                      0xC2u);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_CARRY) == 0u);
    hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA, 0u);

    assert(hyperdos_intel_8042_keyboard_controller_receive_auxiliary_mouse_packet(&machine->pc.keyboardController,
                                                                                  4,
                                                                                  -2,
                                                                                  TEST_AUXILIARY_MOUSE_LEFT_BUTTON));
    for (packetByteIndex = 0u; packetByteIndex < TEST_AUXILIARY_PACKET_BYTE_COUNT; ++packetByteIndex)
    {
        result = hyperdos_pc_bios_runtime_service_pending_hardware_interrupts(&machine->biosRuntime);
        assert(result == HYPERDOS_X86_16_EXECUTION_OK);
        result = hyperdos_x86_16_execute(processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
        assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
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

static void test_keyboard_bios_status_return_flags(void)
{
    hyperdos_pc_machine*                   machine = NULL;
    hyperdos_pc_machine_boot_configuration configuration;
    hyperdos_x86_16_processor*             processor            = NULL;
    hyperdos_x86_16_execution_result       result               = HYPERDOS_X86_16_EXECUTION_OK;
    uint32_t                               flagsPhysicalAddress = 0u;
    uint16_t                               flags                = 0u;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));
    hyperdos_pc_bios_runtime_initialize_data_area(&machine->biosRuntime, NULL, 1u);

    processor = &machine->pc.processor;
    hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_STACK, 0u);
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER] = TEST_INTERRUPT_RETURN_STACK_POINTER;
    flagsPhysicalAddress = TEST_INTERRUPT_RETURN_STACK_POINTER + TEST_INTERRUPT_RETURN_FLAGS_STACK_OFFSET;

    processor->generalRegisters
            [HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = HYPERDOS_PC_KEYBOARD_BIOS_STATUS_SERVICE
                                                             << HYPERDOS_PC_KEYBOARD_BIOS_SERVICE_REGISTER_SHIFT;
    processor->flags                                       = HYPERDOS_X86_16_FLAG_CARRY;
    machine->pc.processorMemory[flagsPhysicalAddress]      = HYPERDOS_X86_16_FLAG_CARRY;
    machine->pc.processorMemory[flagsPhysicalAddress + 1u] = 0u;
    result                                                 = hyperdos_pc_bios_runtime_handle_interrupt(processor,
                                                       HYPERDOS_PC_BIOS_KEYBOARD_SOFTWARE_SERVICE_INTERRUPT,
                                                       &machine->biosRuntime);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    flags = (uint16_t)(machine->pc.processorMemory[flagsPhysicalAddress] |
                       ((uint16_t)machine->pc.processorMemory[flagsPhysicalAddress + 1u]
                        << HYPERDOS_X86_16_BYTE_BIT_COUNT));
    assert((flags & HYPERDOS_X86_16_FLAG_ZERO) != 0u);
    assert((flags & HYPERDOS_X86_16_FLAG_CARRY) != 0u);
    assert((flags & HYPERDOS_X86_16_FLAG_INTERRUPT_ENABLE) != 0u);

    assert(hyperdos_pc_keyboard_bios_push_key_word(&machine->keyboardBios,
                                                   &machine->keyboardBiosInterface,
                                                   &machine->pc,
                                                   TEST_KEYBOARD_FUNCTION_KEY_SIX_WORD));
    processor->generalRegisters
            [HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = HYPERDOS_PC_KEYBOARD_BIOS_STATUS_SERVICE
                                                             << HYPERDOS_PC_KEYBOARD_BIOS_SERVICE_REGISTER_SHIFT;
    processor->flags                                       = 0u;
    machine->pc.processorMemory[flagsPhysicalAddress]      = HYPERDOS_X86_16_FLAG_ZERO;
    machine->pc.processorMemory[flagsPhysicalAddress + 1u] = 0u;
    result                                                 = hyperdos_pc_bios_runtime_handle_interrupt(processor,
                                                       HYPERDOS_PC_BIOS_KEYBOARD_SOFTWARE_SERVICE_INTERRUPT,
                                                       &machine->biosRuntime);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert(processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] ==
           TEST_KEYBOARD_FUNCTION_KEY_SIX_WORD);
    flags = (uint16_t)(machine->pc.processorMemory[flagsPhysicalAddress] |
                       ((uint16_t)machine->pc.processorMemory[flagsPhysicalAddress + 1u]
                        << HYPERDOS_X86_16_BYTE_BIT_COUNT));
    assert((flags & HYPERDOS_X86_16_FLAG_ZERO) == 0u);
    assert((flags & HYPERDOS_X86_16_FLAG_CARRY) == 0u);
    assert((flags & HYPERDOS_X86_16_FLAG_INTERRUPT_ENABLE) != 0u);

    processor->generalRegisters
            [HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_STATUS_SERVICE
                                                             << HYPERDOS_PC_KEYBOARD_BIOS_SERVICE_REGISTER_SHIFT;
    processor->flags                                       = 0u;
    machine->pc.processorMemory[flagsPhysicalAddress]      = HYPERDOS_X86_16_FLAG_CARRY | HYPERDOS_X86_16_FLAG_ZERO;
    machine->pc.processorMemory[flagsPhysicalAddress + 1u] = 0u;
    result                                                 = hyperdos_pc_bios_runtime_handle_interrupt(processor,
                                                       HYPERDOS_PC_BIOS_KEYBOARD_SOFTWARE_SERVICE_INTERRUPT,
                                                       &machine->biosRuntime);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    flags = (uint16_t)(machine->pc.processorMemory[flagsPhysicalAddress] |
                       ((uint16_t)machine->pc.processorMemory[flagsPhysicalAddress + 1u]
                        << HYPERDOS_X86_16_BYTE_BIT_COUNT));
    assert((flags & HYPERDOS_X86_16_FLAG_ZERO) == 0u);
    assert((flags & HYPERDOS_X86_16_FLAG_CARRY) != 0u);
    assert((flags & HYPERDOS_X86_16_FLAG_INTERRUPT_ENABLE) != 0u);

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
    hyperdos_x86_16_processor*             processor = NULL;
    hyperdos_x86_16_execution_result       result    = HYPERDOS_X86_16_EXECUTION_OK;
    uint32_t programPhysicalAddress                  = (uint32_t)TEST_PROGRAM_SEGMENT << HYPERDOS_X86_16_SEGMENT_SHIFT;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));
    hyperdos_pc_bios_runtime_initialize_data_area(&machine->biosRuntime, NULL, 1u);
    hyperdos_pc_bios_install_interrupt_vector_stubs(&machine->pc);

    processor = &machine->pc.processor;
    hyperdos_x86_16_set_interrupt_handler(processor, hyperdos_pc_bios_runtime_handle_interrupt, &machine->biosRuntime);
    hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_CODE, TEST_PROGRAM_SEGMENT);
    hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_STACK, TEST_PROGRAM_STACK_SEGMENT);
    processor->instructionPointer                                               = 0u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER] = TEST_PROGRAM_STACK_POINTER;
    memcpy(&machine->pc.processorMemory[programPhysicalAddress], programBytes, sizeof(programBytes));
    assert(hyperdos_pc_keyboard_bios_push_key_word(&machine->keyboardBios,
                                                   &machine->keyboardBiosInterface,
                                                   &machine->pc,
                                                   TEST_KEYBOARD_ENTER_WORD));

    result = hyperdos_x86_16_execute(processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert(processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] == TEST_KEYBOARD_ENTER_WORD);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_ZERO) == 0u);

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
    hyperdos_x86_16_processor*             processor = NULL;
    hyperdos_x86_16_execution_result       result    = HYPERDOS_X86_16_EXECUTION_OK;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));
    hyperdos_pc_bios_runtime_initialize_data_area(&machine->biosRuntime, NULL, 1u);

    processor                        = &machine->pc.processor;
    processor->lastInstructionOffset = TEST_KEYBOARD_SOFTWARE_SERVICE_STUB_OFFSET;
    processor->instructionPointer    = TEST_KEYBOARD_SOFTWARE_SERVICE_STUB_OFFSET + 2u;
    processor->generalRegisters
            [HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = HYPERDOS_PC_KEYBOARD_BIOS_READ_SERVICE
                                                             << HYPERDOS_PC_KEYBOARD_BIOS_SERVICE_REGISTER_SHIFT;
    processor->flags = HYPERDOS_X86_16_FLAG_RESERVED;

    result = hyperdos_pc_bios_runtime_handle_interrupt(processor,
                                                       HYPERDOS_PC_BIOS_KEYBOARD_SOFTWARE_SERVICE_INTERRUPT,
                                                       &machine->biosRuntime);
    assert(result == HYPERDOS_X86_16_EXECUTION_STEP_LIMIT_REACHED);
    assert(processor->instructionPointer == TEST_KEYBOARD_SOFTWARE_SERVICE_STUB_OFFSET);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_INTERRUPT_ENABLE) != 0u);

    assert(hyperdos_pc_keyboard_bios_push_key_word(&machine->keyboardBios,
                                                   &machine->keyboardBiosInterface,
                                                   &machine->pc,
                                                   TEST_KEYBOARD_FUNCTION_KEY_SIX_WORD));
    processor->instructionPointer = TEST_KEYBOARD_SOFTWARE_SERVICE_STUB_OFFSET + 2u;
    processor->flags              = HYPERDOS_X86_16_FLAG_RESERVED;
    result                        = hyperdos_pc_bios_runtime_handle_interrupt(processor,
                                                       HYPERDOS_PC_BIOS_KEYBOARD_SOFTWARE_SERVICE_INTERRUPT,
                                                       &machine->biosRuntime);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert(processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] ==
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
    uint8_t*                  memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor processor;
    uint32_t                  vectorAddress = (uint32_t)TEST_EXTERNAL_INTERRUPT_NUMBER * 4u;

    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    memory[vectorAddress]      = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET & 0x00FFu);
    memory[vectorAddress + 1u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET >> 8u);
    memory[vectorAddress + 2u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT & 0x00FFu);
    memory[vectorAddress + 3u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT >> 8u);

    assert(hyperdos_x86_16_request_maskable_interrupt(&processor, TEST_EXTERNAL_INTERRUPT_NUMBER) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_get_segment_register(&processor, HYPERDOS_X86_16_SEGMENT_REGISTER_CODE) ==
           TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT);
    assert(processor.instructionPointer == TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET);
    assert((processor.flags & HYPERDOS_X86_16_FLAG_INTERRUPT_ENABLE) == 0u);
    free(memory);
}

static void test_maskable_interrupt_waits_for_interrupt_shadow(void)
{
    static const uint8_t program[] = {
        0xFBu,
        0x90u,
        0xF4u,
    };
    uint8_t*                  memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor processor;
    uint32_t                  vectorAddress                       = (uint32_t)TEST_EXTERNAL_INTERRUPT_NUMBER * 4u;
    uint16_t                  instructionPointerAfterSetInterrupt = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            program,
                                            sizeof(program),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);
    processor.flags            = HYPERDOS_X86_16_FLAG_RESERVED;
    memory[vectorAddress]      = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET & 0x00FFu);
    memory[vectorAddress + 1u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET >> 8u);
    memory[vectorAddress + 2u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT & 0x00FFu);
    memory[vectorAddress + 3u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT >> 8u);

    assert(hyperdos_x86_16_execute(&processor, 1u) == HYPERDOS_X86_16_EXECUTION_STEP_LIMIT_REACHED);
    instructionPointerAfterSetInterrupt = processor.instructionPointer;
    assert(!hyperdos_x86_16_processor_accepts_maskable_interrupt(&processor));
    assert(hyperdos_x86_16_request_maskable_interrupt(&processor, TEST_EXTERNAL_INTERRUPT_NUMBER) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(processor.instructionPointer == instructionPointerAfterSetInterrupt);

    assert(hyperdos_x86_16_execute(&processor, 2u) == HYPERDOS_X86_16_EXECUTION_STEP_LIMIT_REACHED);
    assert(hyperdos_x86_16_processor_accepts_maskable_interrupt(&processor));
    assert(hyperdos_x86_16_request_maskable_interrupt(&processor, TEST_EXTERNAL_INTERRUPT_NUMBER) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_get_segment_register(&processor, HYPERDOS_X86_16_SEGMENT_REGISTER_CODE) ==
           TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT);
    assert(processor.instructionPointer == TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET);
    free(memory);
}

static void test_halt_resumes_through_maskable_interrupt(void)
{
    static const uint8_t program[] = {
        0xF4u,
        0x90u,
    };
    uint8_t*                  memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor processor;
    uint32_t                  vectorAddress = (uint32_t)TEST_EXTERNAL_INTERRUPT_NUMBER * 4u;

    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            program,
                                            sizeof(program),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);
    memory[vectorAddress]      = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET & 0x00FFu);
    memory[vectorAddress + 1u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET >> 8u);
    memory[vectorAddress + 2u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT & 0x00FFu);
    memory[vectorAddress + 3u] = (uint8_t)(TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT >> 8u);

    assert(hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert(processor.halted != 0u);
    assert(hyperdos_x86_16_request_maskable_interrupt(&processor, TEST_EXTERNAL_INTERRUPT_NUMBER) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(processor.halted == 0u);
    assert(hyperdos_x86_16_get_segment_register(&processor, HYPERDOS_X86_16_SEGMENT_REGISTER_CODE) ==
           TEST_EXTERNAL_INTERRUPT_VECTOR_SEGMENT);
    assert(processor.instructionPointer == TEST_EXTERNAL_INTERRUPT_VECTOR_OFFSET);
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
    uint8_t*                  memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor processor;
    uint32_t                  handlerAddress = ((uint32_t)TEST_SINGLE_STEP_INTERRUPT_VECTOR_SEGMENT << 4u) +
                              TEST_SINGLE_STEP_INTERRUPT_VECTOR_OFFSET;
    size_t byteIndex = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            program,
                                            sizeof(program),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);
    processor.flags |= HYPERDOS_X86_16_FLAG_TRAP;
    memory[4]        = (uint8_t)(TEST_SINGLE_STEP_INTERRUPT_VECTOR_OFFSET & 0x00FFu);
    memory[5]        = (uint8_t)(TEST_SINGLE_STEP_INTERRUPT_VECTOR_OFFSET >> 8u);
    memory[6]        = (uint8_t)(TEST_SINGLE_STEP_INTERRUPT_VECTOR_SEGMENT & 0x00FFu);
    memory[7]        = (uint8_t)(TEST_SINGLE_STEP_INTERRUPT_VECTOR_SEGMENT >> 8u);
    for (byteIndex = 0u; byteIndex < sizeof(singleStepHandler); ++byteIndex)
    {
        memory[handlerAddress + byteIndex] = singleStepHandler[byteIndex];
    }

    assert(hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert(hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR) ==
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
    uint8_t*                  memory = NULL;
    hyperdos_x86_16_processor processor;

    memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            decimalAdjustProgram,
                                            sizeof(decimalAdjustProgram),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert((hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR) &
            TEST_BYTE_MASK) == 0x01u);
    assert((processor.flags & HYPERDOS_X86_16_FLAG_CARRY) != 0u);
    assert((processor.flags & HYPERDOS_X86_16_FLAG_AUXILIARY_CARRY) != 0u);
    free(memory);

    memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            asciiAdjustProgram,
                                            sizeof(asciiAdjustProgram),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT) == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert(hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR) == 0x0100u);
    assert((processor.flags & HYPERDOS_X86_16_FLAG_CARRY) != 0u);
    assert((processor.flags & HYPERDOS_X86_16_FLAG_AUXILIARY_CARRY) != 0u);
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
    uint8_t*                         memory = NULL;
    hyperdos_x86_16_processor        processor;
    hyperdos_x86_16_execution_result result = HYPERDOS_X86_16_EXECUTION_OK;

    memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            multiplyAdjustProgram,
                                            sizeof(multiplyAdjustProgram),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);
    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert(hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR) == 0x0309u);
    free(memory);

    memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            divideAdjustProgram,
                                            sizeof(divideAdjustProgram),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);
    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert(hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR) == 0x0025u);
    free(memory);

    memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            divideErrorProgram,
                                            sizeof(divideErrorProgram),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);
    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_DIVIDE_ERROR);
    free(memory);
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
    uint8_t*                         memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor        processor;
    hyperdos_x86_16_execution_result result         = HYPERDOS_X86_16_EXECUTION_OK;
    uint32_t                         handlerAddress = ((uint32_t)TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_SEGMENT << 4u) +
                              TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_OFFSET;
    size_t byteIndex = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            divideErrorInterruptProgram,
                                            sizeof(divideErrorInterruptProgram),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);

    memory[0] = (uint8_t)(TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_OFFSET & 0x00FFu);
    memory[1] = (uint8_t)(TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_OFFSET >> 8u);
    memory[2] = (uint8_t)(TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_SEGMENT & 0x00FFu);
    memory[3] = (uint8_t)(TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_SEGMENT >> 8u);
    for (byteIndex = 0u; byteIndex < sizeof(divideErrorInterruptHandler); ++byteIndex)
    {
        memory[handlerAddress + byteIndex] = divideErrorInterruptHandler[byteIndex];
    }

    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert(hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR) ==
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
    uint8_t*                         memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor        processor;
    hyperdos_x86_16_execution_result result         = HYPERDOS_X86_16_EXECUTION_OK;
    uint32_t                         handlerAddress = ((uint32_t)TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_SEGMENT << 4u) +
                              TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_OFFSET;
    size_t byteIndex = 0u;

    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    processor.divideErrorReturnsToFaultingInstruction = 1u;
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            divideErrorInterruptProgram,
                                            sizeof(divideErrorInterruptProgram),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);

    memory[0] = (uint8_t)(TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_OFFSET & 0x00FFu);
    memory[1] = (uint8_t)(TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_OFFSET >> 8u);
    memory[2] = (uint8_t)(TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_SEGMENT & 0x00FFu);
    memory[3] = (uint8_t)(TEST_DIVIDE_ERROR_INTERRUPT_VECTOR_SEGMENT >> 8u);
    for (byteIndex = 0u; byteIndex < sizeof(divideErrorInterruptHandler); ++byteIndex)
    {
        memory[handlerAddress + byteIndex] = divideErrorInterruptHandler[byteIndex];
    }

    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert(hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR) ==
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
    uint8_t*                         memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor        processor;
    hyperdos_x86_16_execution_result result = HYPERDOS_X86_16_EXECUTION_OK;

    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            program,
                                            sizeof(program),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);
    hyperdos_x86_16_set_segment_register(&processor,
                                         HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                         HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT + 1u);
    assert(hyperdos_x86_16_write_memory_byte(&processor, HYPERDOS_X86_16_SEGMENT_REGISTER_CODE, 0x0203u, 0x5Au) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_write_memory_byte(&processor, HYPERDOS_X86_16_SEGMENT_REGISTER_DATA, 0x0203u, 0x11u) ==
           HYPERDOS_X86_16_EXECUTION_OK);

    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert((hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR) &
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
    uint8_t*                         memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor        processor;
    hyperdos_bus                     bus;
    hyperdos_random_access_memory    randomAccessMemory;
    hyperdos_color_graphics_adapter  colorGraphicsAdapter;
    hyperdos_x86_16_execution_result result    = HYPERDOS_X86_16_EXECUTION_OK;
    size_t                           byteIndex = 0;

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
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    hyperdos_x86_16_attach_bus(&processor, &bus);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            program,
                                            sizeof(program),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);

    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
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
    static const uint8_t             sourceBytes[] = {'H', '8', '0', '8', '6'};
    uint8_t*                         memory        = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor        processor;
    hyperdos_bus                     bus;
    hyperdos_random_access_memory    randomAccessMemory;
    hyperdos_color_graphics_adapter  colorGraphicsAdapter;
    hyperdos_x86_16_execution_result result    = HYPERDOS_X86_16_EXECUTION_OK;
    size_t                           byteIndex = 0;

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
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    hyperdos_x86_16_attach_bus(&processor, &bus);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            program,
                                            sizeof(program),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);

    for (byteIndex = 0; byteIndex < sizeof(sourceBytes); ++byteIndex)
    {
        assert(hyperdos_x86_16_write_memory_byte(&processor,
                                                 HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                                 (uint16_t)(TEST_REPEATED_MOVE_SOURCE_OFFSET + byteIndex),
                                                 sourceBytes[byteIndex]) == HYPERDOS_X86_16_EXECUTION_OK);
    }

    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
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
    return (((uint32_t)segment << 4u) + offset) & HYPERDOS_X86_16_ADDRESS_MASK;
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
    uint8_t*                         memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    hyperdos_x86_16_processor        processor;
    hyperdos_x86_16_execution_result result                 = HYPERDOS_X86_16_EXECUTION_OK;
    size_t                           byteIndex              = 0;
    uint32_t                         programPhysicalAddress = test_physical_address(TEST_FAR_CALL_INITIAL_SEGMENT,
                                                            HYPERDOS_X86_16_DOS_PROGRAM_OFFSET);
    uint32_t pointerPhysicalAddress = test_physical_address(TEST_FAR_CALL_INITIAL_SEGMENT, TEST_FAR_POINTER_OFFSET);
    uint32_t expectedTargetPhysicalAddress  = test_physical_address(TEST_FAR_CALL_TARGET_SEGMENT,
                                                                   TEST_FAR_CALL_TARGET_OFFSET);
    uint32_t incorrectTargetPhysicalAddress = test_physical_address(TEST_FAR_CALL_TARGET_SEGMENT,
                                                                    TEST_FAR_CALL_INCORRECT_OFFSET);

    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    hyperdos_x86_16_set_segment_register(&processor,
                                         HYPERDOS_X86_16_SEGMENT_REGISTER_CODE,
                                         TEST_FAR_CALL_INITIAL_SEGMENT);
    hyperdos_x86_16_set_segment_register(&processor,
                                         HYPERDOS_X86_16_SEGMENT_REGISTER_STACK,
                                         TEST_FAR_CALL_INITIAL_SEGMENT);
    hyperdos_x86_16_set_segment_register(&processor,
                                         HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                         TEST_FAR_CALL_INITIAL_SEGMENT);
    hyperdos_x86_16_set_segment_register(&processor,
                                         HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA,
                                         TEST_FAR_CALL_INITIAL_SEGMENT);
    hyperdos_x86_16_set_general_register(&processor,
                                         HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER,
                                         TEST_FAR_CALL_STACK_POINTER);
    processor.instructionPointer = HYPERDOS_X86_16_DOS_PROGRAM_OFFSET;

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

    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert(hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR) ==
           TEST_FAR_CALL_EXPECTED_ACCUMULATOR);
    assert(hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR) !=
           TEST_FAR_CALL_INCORRECT_ACCUMULATOR);
    free(memory);
}

static uint16_t test_read_memory_word(const hyperdos_x86_16_processor*       processor,
                                      hyperdos_x86_16_segment_register_index segmentRegister,
                                      uint16_t                               offset)
{
    uint8_t lowByte  = 0;
    uint8_t highByte = 0;

    assert(hyperdos_x86_16_read_memory_byte(processor, segmentRegister, offset, &lowByte) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_read_memory_byte(processor, segmentRegister, (uint16_t)(offset + 1u), &highByte) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    return (uint16_t)(lowByte | ((uint16_t)highByte << 8u));
}

static uint8_t test_read_cmos_register(hyperdos_pc* pc, uint8_t registerNumber)
{
    hyperdos_bus_write_input_output_byte_if_mapped(&pc->bus, HYPERDOS_PC_CMOS_ADDRESS_PORT, registerNumber);
    return hyperdos_bus_read_input_output_byte_or_open_bus(&pc->bus, HYPERDOS_PC_CMOS_DATA_PORT);
}

static void test_write_memory_double_word(hyperdos_x86_16_processor*             processor,
                                          hyperdos_x86_16_segment_register_index segmentRegister,
                                          uint16_t                               offset,
                                          uint32_t                               value)
{
    size_t byteIndex = 0;

    for (byteIndex = 0; byteIndex < 4u; ++byteIndex)
    {
        assert(hyperdos_x86_16_write_memory_byte(processor,
                                                 segmentRegister,
                                                 (uint16_t)(offset + byteIndex),
                                                 (uint8_t)(value >> (byteIndex * 8u))) == HYPERDOS_X86_16_EXECUTION_OK);
    }
}

static uint32_t test_read_memory_double_word(const hyperdos_x86_16_processor*       processor,
                                             hyperdos_x86_16_segment_register_index segmentRegister,
                                             uint16_t                               offset)
{
    uint32_t value     = 0;
    size_t   byteIndex = 0;

    for (byteIndex = 0; byteIndex < 4u; ++byteIndex)
    {
        uint8_t byteValue = 0;
        assert(hyperdos_x86_16_read_memory_byte(processor,
                                                segmentRegister,
                                                (uint16_t)(offset + byteIndex),
                                                &byteValue) == HYPERDOS_X86_16_EXECUTION_OK);
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
    uint8_t*                         memory = NULL;
    hyperdos_x86_16_processor        processor;
    hyperdos_x86_16_execution_result result = HYPERDOS_X86_16_EXECUTION_OK;

    memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            escapeProgram,
                                            sizeof(escapeProgram),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);

    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert(hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR) == 0x1234u);
    assert(processor.instructionPointer == HYPERDOS_X86_16_DOS_PROGRAM_OFFSET + sizeof(escapeProgram));
    free(memory);
}

static void test_8087_environment_instructions(void)
{
    static const uint8_t environmentProgram[] = {
        0xDBu, 0xE3u, 0xD9u, 0x3Eu, 0x00u, 0x02u, 0xC7u, 0x06u, 0x02u, 0x02u, 0x7Fu, 0x02u,
        0xD9u, 0x2Eu, 0x02u, 0x02u, 0xD9u, 0x3Eu, 0x04u, 0x02u, 0xDFu, 0xE0u, 0xF4u,
    };
    uint8_t*                         memory = NULL;
    hyperdos_x86_16_processor        processor;
    hyperdos_8087                    floatingPointUnit;
    hyperdos_x86_16_execution_result result = HYPERDOS_X86_16_EXECUTION_OK;

    memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    hyperdos_8087_initialize(&floatingPointUnit);
    hyperdos_x86_16_attach_coprocessor(&processor, hyperdos_8087_wait, hyperdos_8087_escape, &floatingPointUnit);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            environmentProgram,
                                            sizeof(environmentProgram),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);

    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
    assert(test_read_memory_word(&processor, HYPERDOS_X86_16_SEGMENT_REGISTER_DATA, TEST_8087_CONTROL_WORD_OFFSET) ==
           HYPERDOS_8087_CONTROL_WORD_DEFAULT);
    assert(test_read_memory_word(&processor,
                                 HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                 TEST_8087_STORED_CONTROL_WORD_OFFSET) == 0x027Fu);
    assert(floatingPointUnit.controlWord == 0x027Fu);
    assert(floatingPointUnit.tagWord == HYPERDOS_8087_TAG_WORD_DEFAULT);
    assert(hyperdos_x86_16_get_general_register(&processor, HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR) ==
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
    uint8_t*                         memory = NULL;
    hyperdos_x86_16_processor        processor;
    hyperdos_8087                    floatingPointUnit;
    hyperdos_x86_16_execution_result result            = HYPERDOS_X86_16_EXECUTION_OK;
    float                            firstRealValue    = 1.5f;
    float                            secondRealValue   = 2.25f;
    float                            resultRealValue   = 0.0f;
    uint32_t                         firstBinaryValue  = 0;
    uint32_t                         secondBinaryValue = 0;
    uint32_t                         resultBinaryValue = 0;

    memory = (uint8_t*)calloc(HYPERDOS_X86_16_MEMORY_SIZE, 1u);
    assert(memory != NULL);
    assert(hyperdos_x86_16_initialize_processor(&processor, memory, HYPERDOS_X86_16_MEMORY_SIZE) ==
           HYPERDOS_X86_16_EXECUTION_OK);
    hyperdos_8087_initialize(&floatingPointUnit);
    hyperdos_x86_16_attach_coprocessor(&processor, hyperdos_8087_wait, hyperdos_8087_escape, &floatingPointUnit);
    assert(hyperdos_x86_16_load_dos_program(&processor,
                                            arithmeticProgram,
                                            sizeof(arithmeticProgram),
                                            HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT,
                                            "",
                                            0u) == HYPERDOS_X86_16_EXECUTION_OK);

    memcpy(&firstBinaryValue, &firstRealValue, sizeof(firstBinaryValue));
    memcpy(&secondBinaryValue, &secondRealValue, sizeof(secondBinaryValue));
    test_write_memory_double_word(&processor,
                                  HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                  TEST_8087_FIRST_REAL_OFFSET,
                                  firstBinaryValue);
    test_write_memory_double_word(&processor,
                                  HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                  TEST_8087_SECOND_REAL_OFFSET,
                                  secondBinaryValue);

    result = hyperdos_x86_16_execute(&processor, TEST_DEFAULT_INSTRUCTION_LIMIT);
    assert(result == HYPERDOS_X86_16_EXECUTION_HALTED);
    resultBinaryValue = test_read_memory_double_word(&processor,
                                                     HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                                     TEST_8087_RESULT_REAL_OFFSET);
    memcpy(&resultRealValue, &resultBinaryValue, sizeof(resultRealValue));
    assert(resultRealValue == 3.75f);
    assert(floatingPointUnit.tagWord == HYPERDOS_8087_TAG_WORD_DEFAULT);
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
    hyperdos_x86_16_processor*             processor       = NULL;
    hyperdos_x86_16_execution_result       result          = HYPERDOS_X86_16_EXECUTION_OK;
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

    processor                                                                 = &machine->pc.processor;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = TEST_SYSTEM_BIOS_CONFIGURATION_SERVICE
                                                                                << TEST_DOS_SERVICE_REGISTER_SHIFT;
    processor->flags = HYPERDOS_X86_16_FLAG_RESERVED;
    result           = hyperdos_pc_system_bios_handle_system_services_interrupt(processor,
                                                                      &machine->pc,
                                                                      &machine->systemBios,
                                                                      TEST_SYSTEM_BIOS_CONFIGURATION_SERVICE);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_CARRY) != 0u);
    assert((processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] >>
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
    hyperdos_x86_16_processor*             processor                         = NULL;
    hyperdos_x86_16_execution_result       result                            = HYPERDOS_X86_16_EXECUTION_OK;
    uint32_t                               configurationTablePhysicalAddress = 0u;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    configuration.pcModel = HYPERDOS_PC_MODEL_AT;
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));

    hyperdos_pc_system_bios_install_configuration_table(&machine->pc, &machine->systemBios);
    hyperdos_pc_bios_install_interrupt_vector_stubs(&machine->pc);

    processor                                                                 = &machine->pc.processor;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = TEST_SYSTEM_BIOS_CONFIGURATION_SERVICE
                                                                                << TEST_DOS_SERVICE_REGISTER_SHIFT;
    processor->flags = HYPERDOS_X86_16_FLAG_RESERVED | HYPERDOS_X86_16_FLAG_CARRY;
    result           = hyperdos_pc_system_bios_handle_system_services_interrupt(processor,
                                                                      &machine->pc,
                                                                      &machine->systemBios,
                                                                      TEST_SYSTEM_BIOS_CONFIGURATION_SERVICE);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_CARRY) == 0u);

    configurationTablePhysicalAddress = (processor->segmentBases[HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA] +
                                         processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE]) &
                                        HYPERDOS_X86_16_ADDRESS_MASK;
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
    hyperdos_x86_16_processor*             processor          = NULL;
    size_t                                 diskImageByteCount = 0u;
    uint8_t*                               diskImageBytes     = NULL;
    hyperdos_x86_16_execution_result       result             = HYPERDOS_X86_16_EXECUTION_OK;

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

    processor                                                                 = &machine->pc.processor;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = 0x0000u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA]        = 0x0000u;
    processor->flags                                                          = HYPERDOS_X86_16_FLAG_CARRY;
    result = hyperdos_pc_disk_bios_handle_interrupt(processor, &machine->diskBiosInterface);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_CARRY) == 0u);
    assert(diskImage.mediaChanged == 1u);
    assert(diskImage.mediaChangeReported == 0u);
    assert(hyperdos_pc_floppy_controller_drive_disk_changed(&machine->floppyController,
                                                            HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER));
    assert(hyperdos_pc_disk_bios_get_disk_operation_status(&machine->pc, HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER) ==
           TEST_DISK_BIOS_STATUS_SUCCESS);

    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = 0x0201u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER]     = 0x0001u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA]        = 0x0000u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE]        = TEST_DISK_TRANSFER_OFFSET;
    processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA]       = 0u;
    processor->flags                                                          = HYPERDOS_X86_16_FLAG_CARRY;
    result = hyperdos_pc_disk_bios_handle_interrupt(processor, &machine->diskBiosInterface);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_CARRY) != 0u);
    assert((processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] >>
            TEST_DOS_SERVICE_REGISTER_SHIFT) == TEST_DISK_BIOS_STATUS_CHANGED);
    assert(diskImage.mediaChanged == 0u);
    assert(diskImage.mediaChangeReported == 1u);
    assert(!hyperdos_pc_floppy_controller_drive_disk_changed(&machine->floppyController,
                                                             HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER));

    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = 0x0201u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER]     = 0x0001u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA]        = 0x0000u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE]        = TEST_DISK_TRANSFER_OFFSET;
    processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA]       = 0u;
    processor->flags                                                          = HYPERDOS_X86_16_FLAG_CARRY;
    result = hyperdos_pc_disk_bios_handle_interrupt(processor, &machine->diskBiosInterface);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_CARRY) == 0u);
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
    hyperdos_x86_16_processor*             processor          = NULL;
    size_t                                 diskImageByteCount = 0u;
    uint8_t*                               diskImageBytes     = NULL;
    hyperdos_x86_16_execution_result       result             = HYPERDOS_X86_16_EXECUTION_OK;

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
    processor->generalRegisters
            [HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = TEST_DISK_BIOS_GET_TYPE_SERVICE
                                                             << TEST_DISK_BIOS_SERVICE_REGISTER_SHIFT;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA] = HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER;
    processor->flags                                                   = HYPERDOS_X86_16_FLAG_RESERVED;
    result = hyperdos_pc_disk_bios_handle_interrupt(processor, &machine->diskBiosInterface);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_CARRY) == 0u);
    assert((processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] >>
            TEST_DISK_BIOS_SERVICE_REGISTER_SHIFT) == TEST_DISK_BIOS_FLOPPY_WITH_CHANGE_LINE);

    processor->generalRegisters
            [HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = TEST_DISK_BIOS_CHANGE_LINE_SERVICE
                                                             << TEST_DISK_BIOS_SERVICE_REGISTER_SHIFT;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA] = HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER;
    processor->flags                                                   = HYPERDOS_X86_16_FLAG_RESERVED;
    result = hyperdos_pc_disk_bios_handle_interrupt(processor, &machine->diskBiosInterface);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_CARRY) != 0u);
    assert((processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] >>
            TEST_DISK_BIOS_SERVICE_REGISTER_SHIFT) == TEST_DISK_BIOS_CHANGED_STATUS);
    assert(diskImage.mediaChanged == 0u);
    assert(diskImage.mediaChangeReported == 1u);
    assert(!hyperdos_pc_floppy_controller_drive_disk_changed(&machine->floppyController,
                                                             HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER));

    processor->generalRegisters
            [HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = (TEST_DISK_BIOS_READ_SECTORS_SERVICE
                                                              << TEST_DISK_BIOS_SERVICE_REGISTER_SHIFT) |
                                                             1u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER] = 0x0001u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA]    = HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE]    = TEST_DISK_TRANSFER_OFFSET;
    processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA]   = 0u;
    processor->flags                                                      = HYPERDOS_X86_16_FLAG_CARRY;
    result = hyperdos_pc_disk_bios_handle_interrupt(processor, &machine->diskBiosInterface);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_CARRY) == 0u);
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
    hyperdos_x86_16_processor*             processor     = NULL;
    size_t                                 registerIndex = 0u;
    hyperdos_x86_16_execution_result       result        = HYPERDOS_X86_16_EXECUTION_OK;

    machine = (hyperdos_pc_machine*)calloc(1u, sizeof(*machine));
    assert(machine != NULL);
    memset(&configuration, 0, sizeof(configuration));
    assert(hyperdos_pc_machine_initialize_for_boot(machine, &configuration));
    processor = &machine->pc.processor;
    hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA, 0u);

    for (registerIndex = 0u; registerIndex < TEST_VIDEO_BIOS_PALETTE_REGISTER_COUNT + 1u; ++registerIndex)
    {
        machine->pc.processorMemory[TEST_VIDEO_BIOS_PALETTE_TABLE_OFFSET + registerIndex] = (uint8_t)(0x20u +
                                                                                                      registerIndex);
    }
    processor->generalRegisters
            [HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = (TEST_VIDEO_BIOS_PALETTE_SERVICE
                                                              << TEST_VIDEO_BIOS_SERVICE_REGISTER_SHIFT) |
                                                             TEST_VIDEO_BIOS_PALETTE_SET_ALL_REGISTERS;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA] = TEST_VIDEO_BIOS_PALETTE_TABLE_OFFSET;
    processor->flags                                                   = HYPERDOS_X86_16_FLAG_CARRY;
    result = hyperdos_pc_video_bios_handle_interrupt(processor,
                                                     &machine->videoBiosInterface,
                                                     TEST_VIDEO_BIOS_PALETTE_SERVICE);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_CARRY) == 0u);
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
    processor->generalRegisters
            [HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = (TEST_VIDEO_BIOS_PALETTE_SERVICE
                                                              << TEST_VIDEO_BIOS_SERVICE_REGISTER_SHIFT) |
                                                             TEST_VIDEO_BIOS_PALETTE_SET_DAC_REGISTER_BLOCK;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE]    = TEST_VIDEO_BIOS_FIRST_DAC_REGISTER;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER] = 2u;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA]    = TEST_VIDEO_BIOS_DAC_TABLE_OFFSET;
    processor->flags                                                      = HYPERDOS_X86_16_FLAG_CARRY;
    result = hyperdos_pc_video_bios_handle_interrupt(processor,
                                                     &machine->videoBiosInterface,
                                                     TEST_VIDEO_BIOS_PALETTE_SERVICE);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_CARRY) == 0u);
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

    processor->generalRegisters
            [HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = (TEST_VIDEO_BIOS_PALETTE_SERVICE
                                                              << TEST_VIDEO_BIOS_SERVICE_REGISTER_SHIFT) |
                                                             TEST_VIDEO_BIOS_PALETTE_SET_SINGLE_DAC_REGISTER;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE]    = TEST_VIDEO_BIOS_SINGLE_DAC_REGISTER;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER] = (TEST_VIDEO_BIOS_SINGLE_DAC_GREEN
                                                                             << HYPERDOS_X86_16_BYTE_BIT_COUNT) |
                                                                            TEST_VIDEO_BIOS_SINGLE_DAC_BLUE;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA] = TEST_VIDEO_BIOS_SINGLE_DAC_RED
                                                                         << HYPERDOS_X86_16_BYTE_BIT_COUNT;
    processor->flags = HYPERDOS_X86_16_FLAG_CARRY;
    result           = hyperdos_pc_video_bios_handle_interrupt(processor,
                                                     &machine->videoBiosInterface,
                                                     TEST_VIDEO_BIOS_PALETTE_SERVICE);
    assert(result == HYPERDOS_X86_16_EXECUTION_OK);
    assert((processor->flags & HYPERDOS_X86_16_FLAG_CARRY) == 0u);
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
    test_dos_program_output();
    test_loop_and_arithmetic();
    test_shift_rotate_8086_forms();
    test_80186_only_operation_codes_are_unsupported();
    test_80186_shift_rotate_immediate_count();
    test_8086_flags_reserved_bits_are_fixed();
    test_load_data_segment_pointer();
    test_repeated_byte_move();
    test_bus_connected_input_output();
    test_intel_8042_keyboard_controller();
    test_intel_8042_auxiliary_mouse();
    test_cascaded_programmable_interrupt_controller();
    test_pointing_device_bios_services();
    test_pointing_device_bios_callback_from_auxiliary_interrupt();
    test_keyboard_bios_status_return_flags();
    test_keyboard_bios_status_return_flags_through_firmware_stub();
    test_keyboard_bios_read_waits_with_interrupts_enabled();
    test_programmable_interrupt_controller_blocks_same_priority_reentry();
    test_programmable_interrupt_controller_specific_end_of_interrupt();
    test_programmable_interval_timer_rate_generator_transition();
    test_pc_default_interval_timer_transition();
    test_pc_speaker_callback_from_port_control();
    test_pc_system_bios_timer_tick_data_area();
    test_pc_system_bios_identity_can_disable_at_services();
    test_pc_system_bios_configuration_table_survives_interrupt_vector_stubs();
    test_pc_storage_maps_configured_bios_drive_numbers();
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
    test_pc_text_code_pages();
    test_pc_machine_initializes_core_devices();
    return 0;
}
