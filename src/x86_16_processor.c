#include "hyperdos/x86_16_processor.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "hyperdos/hardware.h"

enum
{
    HYPERDOS_X86_16_TOP_TWO_BIT_COUNT                        = 2u,
    HYPERDOS_X86_16_NIBBLE_MASK                              = 0x0Fu,
    HYPERDOS_X86_16_LOW_TWO_BITS_MASK                        = 0x03u,
    HYPERDOS_X86_16_EXTENDED_WORD_MASK                       = 0x1FFFFu,
    HYPERDOS_X86_16_BYTE_SIGN_BIT                            = 0x0080u,
    HYPERDOS_X86_16_WORD_SIGN_BIT                            = 0x8000u,
    HYPERDOS_X86_16_SEGMENT_BYTE_COUNT                       = 0x10000u,
    HYPERDOS_X86_16_AUXILIARY_CARRY_BIT                      = 0x0010u,
    HYPERDOS_X86_16_BYTE_REGISTER_WORD_INDEX_MASK            = 0x03u,
    HYPERDOS_X86_16_BYTE_REGISTER_HIGH_PART_BIT              = 0x04u,
    HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK              = 0x07u,
    HYPERDOS_X86_16_MODRM_REGISTER_MEMORY_MASK               = 0x07u,
    HYPERDOS_X86_16_MODRM_REGISTER_SHIFT                     = 3u,
    HYPERDOS_X86_16_MODRM_MODE_SHIFT                         = 6u,
    HYPERDOS_X86_16_MODRM_MODE_MEMORY_NO_DISPLACEMENT        = 0u,
    HYPERDOS_X86_16_MODRM_MODE_MEMORY_BYTE_DISPLACEMENT      = 1u,
    HYPERDOS_X86_16_MODRM_MODE_MEMORY_WORD_DISPLACEMENT      = 2u,
    HYPERDOS_X86_16_MODRM_MODE_REGISTER                      = 3u,
    HYPERDOS_X86_16_ARITHMETIC_FORM_WORD_BIT                 = 0x01u,
    HYPERDOS_X86_16_ARITHMETIC_FORM_REGISTER_DESTINATION_BIT = 0x02u,
    HYPERDOS_X86_16_ARITHMETIC_FORM_REGISTER_MEMORY_MAXIMUM  = 0x03u,
    HYPERDOS_X86_16_ARITHMETIC_FORM_ACCUMULATOR_BYTE         = 0x04u,
    HYPERDOS_X86_16_ARITHMETIC_FORM_ACCUMULATOR_WORD         = 0x05u,
    HYPERDOS_X86_16_OPERATION_CODE_WORD_BIT                  = 0x01u,
    HYPERDOS_X86_16_OPERATION_CODE_COUNT_IN_COUNTER_BIT      = 0x02u,
    HYPERDOS_X86_16_PARITY_FOLD_SHIFT                        = 4u,
    HYPERDOS_X86_16_PARITY_LOOKUP                            = 0x6996u,
    HYPERDOS_X86_16_INTERRUPT_VECTOR_BYTE_COUNT              = 4u,
    HYPERDOS_X86_16_FAR_POINTER_SEGMENT_OFFSET               = 2u,
    HYPERDOS_X86_16_RESET_CODE_SEGMENT                       = 0xFFFFu,
    HYPERDOS_X86_16_RESET_INSTRUCTION_POINTER                = 0x0000u,
    HYPERDOS_X86_16_DOS_PROGRAM_MAXIMUM_SIZE                 = 0xFF00u,
    HYPERDOS_X86_16_DOS_STACK_POINTER                        = 0xFFFEu,
    HYPERDOS_X86_16_DOS_TERMINATE_STUB_OFFSET                = 0x0000u,
    HYPERDOS_X86_16_DOS_COMMAND_TAIL_LENGTH_OFFSET           = 0x0080u,
    HYPERDOS_X86_16_DOS_COMMAND_TAIL_BYTES_OFFSET            = 0x0081u,
    HYPERDOS_X86_16_DOS_COMMAND_TAIL_MAXIMUM_LENGTH          = 126u,
    HYPERDOS_X86_16_ASCII_CARRIAGE_RETURN                    = 0x0Du,
    HYPERDOS_X86_16_INTERRUPT_TYPE_DIVIDE_ERROR              = 0u,
    HYPERDOS_X86_16_INTERRUPT_TYPE_SINGLE_STEP               = 1u,
    HYPERDOS_X86_16_INTERRUPT_TYPE_BREAKPOINT                = 3u,
    HYPERDOS_X86_16_INTERRUPT_TYPE_OVERFLOW                  = 4u,
    HYPERDOS_X86_16_FLAGS_LOW_BYTE_TRANSFER_MASK             = 0x00D7u,
    HYPERDOS_X86_16_FLAGS_HIGH_BYTE_MASK                     = 0xFF00u,
    HYPERDOS_X86_16_FLAGS_WRITABLE_MASK                      = 0x0FD5u
};

typedef enum hyperdos_x86_16_operation_code
{
    HYPERDOS_X86_16_OPERATION_CODE_PUSH_EXTRA_SEGMENT                     = 0x06u,
    HYPERDOS_X86_16_OPERATION_CODE_POP_EXTRA_SEGMENT                      = 0x07u,
    HYPERDOS_X86_16_OPERATION_CODE_PUSH_CODE_SEGMENT                      = 0x0Eu,
    HYPERDOS_X86_16_OPERATION_CODE_PUSH_STACK_SEGMENT                     = 0x16u,
    HYPERDOS_X86_16_OPERATION_CODE_POP_STACK_SEGMENT                      = 0x17u,
    HYPERDOS_X86_16_OPERATION_CODE_PUSH_DATA_SEGMENT                      = 0x1Eu,
    HYPERDOS_X86_16_OPERATION_CODE_POP_DATA_SEGMENT                       = 0x1Fu,
    HYPERDOS_X86_16_OPERATION_CODE_PREFIX_EXTRA_SEGMENT                   = 0x26u,
    HYPERDOS_X86_16_OPERATION_CODE_DECIMAL_ADJUST_AFTER_ADD               = 0x27u,
    HYPERDOS_X86_16_OPERATION_CODE_PREFIX_CODE_SEGMENT                    = 0x2Eu,
    HYPERDOS_X86_16_OPERATION_CODE_DECIMAL_ADJUST_AFTER_SUBTRACT          = 0x2Fu,
    HYPERDOS_X86_16_OPERATION_CODE_PREFIX_STACK_SEGMENT                   = 0x36u,
    HYPERDOS_X86_16_OPERATION_CODE_ASCII_ADJUST_AFTER_ADD                 = 0x37u,
    HYPERDOS_X86_16_OPERATION_CODE_PREFIX_DATA_SEGMENT                    = 0x3Eu,
    HYPERDOS_X86_16_OPERATION_CODE_ASCII_ADJUST_AFTER_SUBTRACT            = 0x3Fu,
    HYPERDOS_X86_16_OPERATION_CODE_ARITHMETIC_LOGIC_FIRST                 = 0x00u,
    HYPERDOS_X86_16_OPERATION_CODE_ARITHMETIC_LOGIC_LAST                  = 0x3Du,
    HYPERDOS_X86_16_OPERATION_CODE_INCREMENT_REGISTER_FIRST               = 0x40u,
    HYPERDOS_X86_16_OPERATION_CODE_INCREMENT_REGISTER_LAST                = 0x47u,
    HYPERDOS_X86_16_OPERATION_CODE_DECREMENT_REGISTER_FIRST               = 0x48u,
    HYPERDOS_X86_16_OPERATION_CODE_DECREMENT_REGISTER_LAST                = 0x4Fu,
    HYPERDOS_X86_16_OPERATION_CODE_PUSH_REGISTER_FIRST                    = 0x50u,
    HYPERDOS_X86_16_OPERATION_CODE_PUSH_REGISTER_LAST                     = 0x57u,
    HYPERDOS_X86_16_OPERATION_CODE_POP_REGISTER_FIRST                     = 0x58u,
    HYPERDOS_X86_16_OPERATION_CODE_POP_REGISTER_LAST                      = 0x5Fu,
    HYPERDOS_X86_16_OPERATION_CODE_JUMP_CONDITION_FIRST                   = 0x70u,
    HYPERDOS_X86_16_OPERATION_CODE_JUMP_CONDITION_LAST                    = 0x7Fu,
    HYPERDOS_X86_16_OPERATION_CODE_GROUP_ONE_BYTE_IMMEDIATE               = 0x80u,
    HYPERDOS_X86_16_OPERATION_CODE_GROUP_ONE_WORD_IMMEDIATE               = 0x81u,
    HYPERDOS_X86_16_OPERATION_CODE_GROUP_ONE_BYTE_IMMEDIATE_ALIAS         = 0x82u,
    HYPERDOS_X86_16_OPERATION_CODE_GROUP_ONE_SIGN_EXTENDED_IMMEDIATE      = 0x83u,
    HYPERDOS_X86_16_OPERATION_CODE_TEST_REGISTER_MEMORY_BYTE              = 0x84u,
    HYPERDOS_X86_16_OPERATION_CODE_TEST_REGISTER_MEMORY_WORD              = 0x85u,
    HYPERDOS_X86_16_OPERATION_CODE_EXCHANGE_REGISTER_MEMORY_BYTE          = 0x86u,
    HYPERDOS_X86_16_OPERATION_CODE_EXCHANGE_REGISTER_MEMORY_WORD          = 0x87u,
    HYPERDOS_X86_16_OPERATION_CODE_MOVE_REGISTER_MEMORY_BYTE_FIRST        = 0x88u,
    HYPERDOS_X86_16_OPERATION_CODE_MOVE_REGISTER_MEMORY_WORD_LAST         = 0x8Bu,
    HYPERDOS_X86_16_OPERATION_CODE_MOVE_SEGMENT_TO_REGISTER_MEMORY        = 0x8Cu,
    HYPERDOS_X86_16_OPERATION_CODE_LOAD_EFFECTIVE_ADDRESS                 = 0x8Du,
    HYPERDOS_X86_16_OPERATION_CODE_MOVE_REGISTER_MEMORY_TO_SEGMENT        = 0x8Eu,
    HYPERDOS_X86_16_OPERATION_CODE_POP_REGISTER_MEMORY                    = 0x8Fu,
    HYPERDOS_X86_16_OPERATION_CODE_EXCHANGE_ACCUMULATOR_FIRST             = 0x90u,
    HYPERDOS_X86_16_OPERATION_CODE_EXCHANGE_ACCUMULATOR_LAST              = 0x97u,
    HYPERDOS_X86_16_OPERATION_CODE_CONVERT_BYTE_TO_WORD                   = 0x98u,
    HYPERDOS_X86_16_OPERATION_CODE_CONVERT_WORD_TO_DOUBLE_WORD            = 0x99u,
    HYPERDOS_X86_16_OPERATION_CODE_CALL_FAR_IMMEDIATE                     = 0x9Au,
    HYPERDOS_X86_16_OPERATION_CODE_WAIT                                   = 0x9Bu,
    HYPERDOS_X86_16_OPERATION_CODE_PUSH_FLAGS                             = 0x9Cu,
    HYPERDOS_X86_16_OPERATION_CODE_POP_FLAGS                              = 0x9Du,
    HYPERDOS_X86_16_OPERATION_CODE_STORE_STATUS_TO_FLAGS                  = 0x9Eu,
    HYPERDOS_X86_16_OPERATION_CODE_LOAD_STATUS_FROM_FLAGS                 = 0x9Fu,
    HYPERDOS_X86_16_OPERATION_CODE_MOVE_MEMORY_TO_ACCUMULATOR_BYTE        = 0xA0u,
    HYPERDOS_X86_16_OPERATION_CODE_MOVE_MEMORY_TO_ACCUMULATOR_WORD        = 0xA1u,
    HYPERDOS_X86_16_OPERATION_CODE_MOVE_ACCUMULATOR_TO_MEMORY_BYTE        = 0xA2u,
    HYPERDOS_X86_16_OPERATION_CODE_MOVE_ACCUMULATOR_TO_MEMORY_WORD        = 0xA3u,
    HYPERDOS_X86_16_OPERATION_CODE_MOVE_STRING_BYTE                       = 0xA4u,
    HYPERDOS_X86_16_OPERATION_CODE_MOVE_STRING_WORD                       = 0xA5u,
    HYPERDOS_X86_16_OPERATION_CODE_COMPARE_STRING_BYTE                    = 0xA6u,
    HYPERDOS_X86_16_OPERATION_CODE_COMPARE_STRING_WORD                    = 0xA7u,
    HYPERDOS_X86_16_OPERATION_CODE_TEST_ACCUMULATOR_BYTE                  = 0xA8u,
    HYPERDOS_X86_16_OPERATION_CODE_TEST_ACCUMULATOR_WORD                  = 0xA9u,
    HYPERDOS_X86_16_OPERATION_CODE_STORE_STRING_BYTE                      = 0xAAu,
    HYPERDOS_X86_16_OPERATION_CODE_STORE_STRING_WORD                      = 0xABu,
    HYPERDOS_X86_16_OPERATION_CODE_LOAD_STRING_BYTE                       = 0xACu,
    HYPERDOS_X86_16_OPERATION_CODE_LOAD_STRING_WORD                       = 0xADu,
    HYPERDOS_X86_16_OPERATION_CODE_SCAN_STRING_BYTE                       = 0xAEu,
    HYPERDOS_X86_16_OPERATION_CODE_SCAN_STRING_WORD                       = 0xAFu,
    HYPERDOS_X86_16_OPERATION_CODE_MOVE_IMMEDIATE_TO_BYTE_REGISTER_FIRST  = 0xB0u,
    HYPERDOS_X86_16_OPERATION_CODE_MOVE_IMMEDIATE_TO_BYTE_REGISTER_LAST   = 0xB7u,
    HYPERDOS_X86_16_OPERATION_CODE_MOVE_IMMEDIATE_TO_WORD_REGISTER_FIRST  = 0xB8u,
    HYPERDOS_X86_16_OPERATION_CODE_MOVE_IMMEDIATE_TO_WORD_REGISTER_LAST   = 0xBFu,
    HYPERDOS_X86_16_OPERATION_CODE_SHIFT_ROTATE_IMMEDIATE_BYTE            = 0xC0u,
    HYPERDOS_X86_16_OPERATION_CODE_SHIFT_ROTATE_IMMEDIATE_WORD            = 0xC1u,
    HYPERDOS_X86_16_OPERATION_CODE_RETURN_NEAR_IMMEDIATE                  = 0xC2u,
    HYPERDOS_X86_16_OPERATION_CODE_RETURN_NEAR                            = 0xC3u,
    HYPERDOS_X86_16_OPERATION_CODE_LOAD_EXTRA_SEGMENT_POINTER             = 0xC4u,
    HYPERDOS_X86_16_OPERATION_CODE_LOAD_DATA_SEGMENT_POINTER              = 0xC5u,
    HYPERDOS_X86_16_OPERATION_CODE_MOVE_IMMEDIATE_TO_REGISTER_MEMORY_BYTE = 0xC6u,
    HYPERDOS_X86_16_OPERATION_CODE_MOVE_IMMEDIATE_TO_REGISTER_MEMORY_WORD = 0xC7u,
    HYPERDOS_X86_16_OPERATION_CODE_RETURN_FAR_IMMEDIATE                   = 0xCAu,
    HYPERDOS_X86_16_OPERATION_CODE_RETURN_FAR                             = 0xCBu,
    HYPERDOS_X86_16_OPERATION_CODE_INTERRUPT_BREAKPOINT                   = 0xCCu,
    HYPERDOS_X86_16_OPERATION_CODE_INTERRUPT_IMMEDIATE                    = 0xCDu,
    HYPERDOS_X86_16_OPERATION_CODE_INTERRUPT_ON_OVERFLOW                  = 0xCEu,
    HYPERDOS_X86_16_OPERATION_CODE_INTERRUPT_RETURN                       = 0xCFu,
    HYPERDOS_X86_16_OPERATION_CODE_SHIFT_ROTATE_FIRST                     = 0xD0u,
    HYPERDOS_X86_16_OPERATION_CODE_SHIFT_ROTATE_LAST                      = 0xD3u,
    HYPERDOS_X86_16_OPERATION_CODE_ASCII_ADJUST_AFTER_MULTIPLY            = 0xD4u,
    HYPERDOS_X86_16_OPERATION_CODE_ASCII_ADJUST_BEFORE_DIVIDE             = 0xD5u,
    HYPERDOS_X86_16_OPERATION_CODE_TRANSLATE_BYTE                         = 0xD7u,
    HYPERDOS_X86_16_OPERATION_CODE_ESCAPE_COPROCESSOR_FIRST               = 0xD8u,
    HYPERDOS_X86_16_OPERATION_CODE_ESCAPE_COPROCESSOR_LAST                = 0xDFu,
    HYPERDOS_X86_16_OPERATION_CODE_LOOP_NOT_ZERO_AND_NOT_EQUAL            = 0xE0u,
    HYPERDOS_X86_16_OPERATION_CODE_LOOP_NOT_ZERO_AND_EQUAL                = 0xE1u,
    HYPERDOS_X86_16_OPERATION_CODE_LOOP_NOT_ZERO                          = 0xE2u,
    HYPERDOS_X86_16_OPERATION_CODE_JUMP_IF_COUNTER_ZERO                   = 0xE3u,
    HYPERDOS_X86_16_OPERATION_CODE_INPUT_IMMEDIATE_BYTE                   = 0xE4u,
    HYPERDOS_X86_16_OPERATION_CODE_INPUT_IMMEDIATE_WORD                   = 0xE5u,
    HYPERDOS_X86_16_OPERATION_CODE_OUTPUT_IMMEDIATE_BYTE                  = 0xE6u,
    HYPERDOS_X86_16_OPERATION_CODE_OUTPUT_IMMEDIATE_WORD                  = 0xE7u,
    HYPERDOS_X86_16_OPERATION_CODE_CALL_RELATIVE                          = 0xE8u,
    HYPERDOS_X86_16_OPERATION_CODE_JUMP_RELATIVE                          = 0xE9u,
    HYPERDOS_X86_16_OPERATION_CODE_JUMP_FAR_IMMEDIATE                     = 0xEAu,
    HYPERDOS_X86_16_OPERATION_CODE_JUMP_SHORT                             = 0xEBu,
    HYPERDOS_X86_16_OPERATION_CODE_INPUT_FROM_DATA_BYTE                   = 0xECu,
    HYPERDOS_X86_16_OPERATION_CODE_INPUT_FROM_DATA_WORD                   = 0xEDu,
    HYPERDOS_X86_16_OPERATION_CODE_OUTPUT_TO_DATA_BYTE                    = 0xEEu,
    HYPERDOS_X86_16_OPERATION_CODE_OUTPUT_TO_DATA_WORD                    = 0xEFu,
    HYPERDOS_X86_16_OPERATION_CODE_PREFIX_LOCK                            = 0xF0u,
    HYPERDOS_X86_16_OPERATION_CODE_PREFIX_REPEAT_WHILE_NOT_EQUAL          = 0xF2u,
    HYPERDOS_X86_16_OPERATION_CODE_PREFIX_REPEAT                          = 0xF3u,
    HYPERDOS_X86_16_OPERATION_CODE_HALT                                   = 0xF4u,
    HYPERDOS_X86_16_OPERATION_CODE_COMPLEMENT_CARRY                       = 0xF5u,
    HYPERDOS_X86_16_OPERATION_CODE_GROUP_THREE_BYTE                       = 0xF6u,
    HYPERDOS_X86_16_OPERATION_CODE_GROUP_THREE_WORD                       = 0xF7u,
    HYPERDOS_X86_16_OPERATION_CODE_CLEAR_CARRY                            = 0xF8u,
    HYPERDOS_X86_16_OPERATION_CODE_SET_CARRY                              = 0xF9u,
    HYPERDOS_X86_16_OPERATION_CODE_CLEAR_INTERRUPT                        = 0xFAu,
    HYPERDOS_X86_16_OPERATION_CODE_SET_INTERRUPT                          = 0xFBu,
    HYPERDOS_X86_16_OPERATION_CODE_CLEAR_DIRECTION                        = 0xFCu,
    HYPERDOS_X86_16_OPERATION_CODE_SET_DIRECTION                          = 0xFDu,
    HYPERDOS_X86_16_OPERATION_CODE_INCREMENT_DECREMENT_BYTE               = 0xFEu,
    HYPERDOS_X86_16_OPERATION_CODE_GROUP_FIVE                             = 0xFFu
} hyperdos_x86_16_operation_code;

typedef enum hyperdos_x86_16_repeat_mode
{
    HYPERDOS_X86_16_REPEAT_NONE = 0,
    HYPERDOS_X86_16_REPEAT_WHILE_EQUAL,
    HYPERDOS_X86_16_REPEAT_WHILE_NOT_EQUAL
} hyperdos_x86_16_repeat_mode;

typedef enum hyperdos_x86_16_arithmetic_logic_operation
{
    HYPERDOS_X86_16_ARITHMETIC_LOGIC_ADD = 0,
    HYPERDOS_X86_16_ARITHMETIC_LOGIC_OR  = 1,
    HYPERDOS_X86_16_ARITHMETIC_LOGIC_ADC = 2,
    HYPERDOS_X86_16_ARITHMETIC_LOGIC_SBB = 3,
    HYPERDOS_X86_16_ARITHMETIC_LOGIC_AND = 4,
    HYPERDOS_X86_16_ARITHMETIC_LOGIC_SUB = 5,
    HYPERDOS_X86_16_ARITHMETIC_LOGIC_XOR = 6,
    HYPERDOS_X86_16_ARITHMETIC_LOGIC_CMP = 7
} hyperdos_x86_16_arithmetic_logic_operation;

typedef enum hyperdos_x86_16_condition_code
{
    HYPERDOS_X86_16_CONDITION_OVERFLOW         = 0x0u,
    HYPERDOS_X86_16_CONDITION_NOT_OVERFLOW     = 0x1u,
    HYPERDOS_X86_16_CONDITION_CARRY            = 0x2u,
    HYPERDOS_X86_16_CONDITION_NOT_CARRY        = 0x3u,
    HYPERDOS_X86_16_CONDITION_ZERO             = 0x4u,
    HYPERDOS_X86_16_CONDITION_NOT_ZERO         = 0x5u,
    HYPERDOS_X86_16_CONDITION_BELOW_OR_EQUAL   = 0x6u,
    HYPERDOS_X86_16_CONDITION_ABOVE            = 0x7u,
    HYPERDOS_X86_16_CONDITION_SIGN             = 0x8u,
    HYPERDOS_X86_16_CONDITION_NOT_SIGN         = 0x9u,
    HYPERDOS_X86_16_CONDITION_PARITY           = 0xAu,
    HYPERDOS_X86_16_CONDITION_NOT_PARITY       = 0xBu,
    HYPERDOS_X86_16_CONDITION_LESS             = 0xCu,
    HYPERDOS_X86_16_CONDITION_GREATER_OR_EQUAL = 0xDu,
    HYPERDOS_X86_16_CONDITION_LESS_OR_EQUAL    = 0xEu,
    HYPERDOS_X86_16_CONDITION_GREATER          = 0xFu
} hyperdos_x86_16_condition_code;

typedef enum hyperdos_x86_16_shift_rotate_operation
{
    HYPERDOS_X86_16_SHIFT_ROTATE_ROL    = 0u,
    HYPERDOS_X86_16_SHIFT_ROTATE_ROR    = 1u,
    HYPERDOS_X86_16_SHIFT_ROTATE_RCL    = 2u,
    HYPERDOS_X86_16_SHIFT_ROTATE_RCR    = 3u,
    HYPERDOS_X86_16_SHIFT_ROTATE_SHL    = 4u,
    HYPERDOS_X86_16_SHIFT_ROTATE_SHR    = 5u,
    HYPERDOS_X86_16_SHIFT_ROTATE_UNUSED = 6u,
    HYPERDOS_X86_16_SHIFT_ROTATE_SAR    = 7u
} hyperdos_x86_16_shift_rotate_operation;

typedef enum hyperdos_x86_16_group_three_operation
{
    HYPERDOS_X86_16_GROUP_THREE_TEST              = 0u,
    HYPERDOS_X86_16_GROUP_THREE_NOT               = 2u,
    HYPERDOS_X86_16_GROUP_THREE_NEGATE            = 3u,
    HYPERDOS_X86_16_GROUP_THREE_MULTIPLY_UNSIGNED = 4u,
    HYPERDOS_X86_16_GROUP_THREE_MULTIPLY_SIGNED   = 5u,
    HYPERDOS_X86_16_GROUP_THREE_DIVIDE_UNSIGNED   = 6u,
    HYPERDOS_X86_16_GROUP_THREE_DIVIDE_SIGNED     = 7u
} hyperdos_x86_16_group_three_operation;

typedef enum hyperdos_x86_16_group_five_operation
{
    HYPERDOS_X86_16_GROUP_FIVE_INCREMENT = 0u,
    HYPERDOS_X86_16_GROUP_FIVE_DECREMENT = 1u,
    HYPERDOS_X86_16_GROUP_FIVE_CALL_NEAR = 2u,
    HYPERDOS_X86_16_GROUP_FIVE_CALL_FAR  = 3u,
    HYPERDOS_X86_16_GROUP_FIVE_JUMP_NEAR = 4u,
    HYPERDOS_X86_16_GROUP_FIVE_JUMP_FAR  = 5u,
    HYPERDOS_X86_16_GROUP_FIVE_PUSH      = 6u
} hyperdos_x86_16_group_five_operation;

typedef struct hyperdos_x86_16_decoded_operand
{
    uint8_t                                isRegister;
    uint8_t                                registerIndex;
    hyperdos_x86_16_segment_register_index segmentRegister;
    uint16_t                               offset;
} hyperdos_x86_16_decoded_operand;

static int hyperdos_x86_16_operation_code_is_in_range(uint8_t operationCode,
                                                      uint8_t firstOperationCode,
                                                      uint8_t lastOperationCode)
{
    return (uint8_t)(operationCode - firstOperationCode) <= (uint8_t)(lastOperationCode - firstOperationCode);
}

static uint16_t hyperdos_x86_16_operand_value_mask(int isWord)
{
    return isWord ? HYPERDOS_X86_16_WORD_MASK : HYPERDOS_X86_16_BYTE_MASK;
}

static uint16_t hyperdos_x86_16_operand_sign_bit(int isWord)
{
    return isWord ? HYPERDOS_X86_16_WORD_SIGN_BIT : HYPERDOS_X86_16_BYTE_SIGN_BIT;
}

static int hyperdos_x86_16_operation_code_is_string(uint8_t operationCode)
{
    return hyperdos_x86_16_operation_code_is_in_range(operationCode,
                                                      HYPERDOS_X86_16_OPERATION_CODE_MOVE_STRING_BYTE,
                                                      HYPERDOS_X86_16_OPERATION_CODE_COMPARE_STRING_WORD) ||
           hyperdos_x86_16_operation_code_is_in_range(operationCode,
                                                      HYPERDOS_X86_16_OPERATION_CODE_STORE_STRING_BYTE,
                                                      HYPERDOS_X86_16_OPERATION_CODE_SCAN_STRING_WORD);
}

static uint16_t hyperdos_x86_16_normalize_flags(uint16_t flags)
{
    return (uint16_t)((flags & HYPERDOS_X86_16_FLAGS_WRITABLE_MASK) | HYPERDOS_X86_16_FLAG_RESERVED);
}

static int hyperdos_x86_16_supports_80186_instructions(const hyperdos_x86_16_processor* processor)
{
    return processor != NULL && processor->processorModel >= HYPERDOS_X86_16_PROCESSOR_MODEL_80186;
}

static uint32_t hyperdos_x86_16_physical_address_from_base(uint32_t segmentBase, uint16_t offset)
{
    return (segmentBase + offset) & HYPERDOS_X86_16_ADDRESS_MASK;
}

static void hyperdos_x86_16_store_segment_base(hyperdos_x86_16_processor*             processor,
                                               hyperdos_x86_16_segment_register_index segmentRegister)
{
    processor->segmentBases[segmentRegister] = ((uint32_t)processor->segmentRegisters[segmentRegister]
                                                << HYPERDOS_X86_16_SEGMENT_SHIFT) &
                                               HYPERDOS_X86_16_ADDRESS_MASK;
}

static uint8_t hyperdos_x86_16_read_physical_byte(const hyperdos_x86_16_processor* processor, uint32_t physicalAddress)
{
    if (processor->bus != NULL)
    {
        return hyperdos_bus_read_memory_byte_or_open_bus(processor->bus,
                                                         physicalAddress & HYPERDOS_X86_16_ADDRESS_MASK);
    }
    return processor->memory[physicalAddress & HYPERDOS_X86_16_ADDRESS_MASK];
}

static void hyperdos_x86_16_write_physical_byte(hyperdos_x86_16_processor* processor,
                                                uint32_t                   physicalAddress,
                                                uint8_t                    value)
{
    if (processor->bus != NULL)
    {
        hyperdos_bus_write_memory_byte_if_mapped(processor->bus, physicalAddress & HYPERDOS_X86_16_ADDRESS_MASK, value);
        return;
    }
    processor->memory[physicalAddress & HYPERDOS_X86_16_ADDRESS_MASK] = value;
}

static uint16_t hyperdos_x86_16_read_physical_word(const hyperdos_x86_16_processor* processor, uint32_t physicalAddress)
{
    uint16_t lowByte  = hyperdos_x86_16_read_physical_byte(processor, physicalAddress);
    uint16_t highByte = hyperdos_x86_16_read_physical_byte(processor, physicalAddress + 1u);
    return (uint16_t)(lowByte | (highByte << HYPERDOS_X86_16_BYTE_BIT_COUNT));
}

static void hyperdos_x86_16_write_physical_word(hyperdos_x86_16_processor* processor,
                                                uint32_t                   physicalAddress,
                                                uint16_t                   value)
{
    hyperdos_x86_16_write_physical_byte(processor, physicalAddress, (uint8_t)(value & HYPERDOS_X86_16_BYTE_MASK));
    hyperdos_x86_16_write_physical_byte(processor,
                                        physicalAddress + 1u,
                                        (uint8_t)(value >> HYPERDOS_X86_16_BYTE_BIT_COUNT));
}

static uint8_t hyperdos_x86_16_read_memory_byte_fast(const hyperdos_x86_16_processor*       processor,
                                                     hyperdos_x86_16_segment_register_index segmentRegister,
                                                     uint16_t                               offset)
{
    uint32_t physicalAddress = hyperdos_x86_16_physical_address_from_base(processor->segmentBases[segmentRegister],
                                                                          offset);
    return hyperdos_x86_16_read_physical_byte(processor, physicalAddress);
}

static void hyperdos_x86_16_write_memory_byte_fast(hyperdos_x86_16_processor*             processor,
                                                   hyperdos_x86_16_segment_register_index segmentRegister,
                                                   uint16_t                               offset,
                                                   uint8_t                                value)
{
    uint32_t physicalAddress = hyperdos_x86_16_physical_address_from_base(processor->segmentBases[segmentRegister],
                                                                          offset);
    hyperdos_x86_16_write_physical_byte(processor, physicalAddress, value);
}

static uint16_t hyperdos_x86_16_read_memory_word_fast(const hyperdos_x86_16_processor*       processor,
                                                      hyperdos_x86_16_segment_register_index segmentRegister,
                                                      uint16_t                               offset)
{
    uint32_t physicalAddress = hyperdos_x86_16_physical_address_from_base(processor->segmentBases[segmentRegister],
                                                                          offset);
    return hyperdos_x86_16_read_physical_word(processor, physicalAddress);
}

static void hyperdos_x86_16_write_memory_word_fast(hyperdos_x86_16_processor*             processor,
                                                   hyperdos_x86_16_segment_register_index segmentRegister,
                                                   uint16_t                               offset,
                                                   uint16_t                               value)
{
    uint32_t physicalAddress = hyperdos_x86_16_physical_address_from_base(processor->segmentBases[segmentRegister],
                                                                          offset);
    hyperdos_x86_16_write_physical_word(processor, physicalAddress, value);
}

static uint8_t hyperdos_x86_16_fetch_instruction_byte(hyperdos_x86_16_processor* processor)
{
    uint8_t value                 = hyperdos_x86_16_read_memory_byte_fast(processor,
                                                          HYPERDOS_X86_16_SEGMENT_REGISTER_CODE,
                                                          processor->instructionPointer);
    processor->instructionPointer = (uint16_t)(processor->instructionPointer + 1u);
    return value;
}

static uint16_t hyperdos_x86_16_fetch_instruction_word(hyperdos_x86_16_processor* processor)
{
    uint16_t value                = hyperdos_x86_16_read_memory_word_fast(processor,
                                                           HYPERDOS_X86_16_SEGMENT_REGISTER_CODE,
                                                           processor->instructionPointer);
    processor->instructionPointer = (uint16_t)(processor->instructionPointer + 2u);
    return value;
}

static uint8_t hyperdos_x86_16_read_byte_register(const hyperdos_x86_16_processor* processor, uint8_t byteRegisterIndex)
{
    uint16_t value = processor->generalRegisters[byteRegisterIndex & HYPERDOS_X86_16_BYTE_REGISTER_WORD_INDEX_MASK];
    if ((byteRegisterIndex & HYPERDOS_X86_16_BYTE_REGISTER_HIGH_PART_BIT) != 0u)
    {
        return (uint8_t)(value >> HYPERDOS_X86_16_BYTE_BIT_COUNT);
    }
    return (uint8_t)(value & HYPERDOS_X86_16_BYTE_MASK);
}

static void hyperdos_x86_16_write_byte_register(hyperdos_x86_16_processor* processor,
                                                uint8_t                    byteRegisterIndex,
                                                uint8_t                    value)
{
    uint16_t* generalRegister = &processor->generalRegisters[byteRegisterIndex &
                                                             HYPERDOS_X86_16_BYTE_REGISTER_WORD_INDEX_MASK];
    if ((byteRegisterIndex & HYPERDOS_X86_16_BYTE_REGISTER_HIGH_PART_BIT) != 0u)
    {
        *generalRegister = (uint16_t)((*generalRegister & HYPERDOS_X86_16_BYTE_MASK) |
                                      ((uint16_t)value << HYPERDOS_X86_16_BYTE_BIT_COUNT));
    }
    else
    {
        *generalRegister = (uint16_t)((*generalRegister & HYPERDOS_X86_16_HIGH_BYTE_MASK) | value);
    }
}

static uint16_t hyperdos_x86_16_sign_extend_byte(uint8_t value)
{
    return (uint16_t)(int16_t)(int8_t)value;
}

static int hyperdos_x86_16_has_even_parity(uint8_t value)
{
    value ^= (uint8_t)(value >> HYPERDOS_X86_16_PARITY_FOLD_SHIFT);
    value &= HYPERDOS_X86_16_NIBBLE_MASK;
    return ((HYPERDOS_X86_16_PARITY_LOOKUP >> value) & 1u) == 0u;
}

static void hyperdos_x86_16_replace_status_flags(hyperdos_x86_16_processor* processor, uint16_t statusFlags)
{
    const uint16_t statusMask = HYPERDOS_X86_16_FLAG_CARRY | HYPERDOS_X86_16_FLAG_PARITY |
                                HYPERDOS_X86_16_FLAG_AUXILIARY_CARRY | HYPERDOS_X86_16_FLAG_ZERO |
                                HYPERDOS_X86_16_FLAG_SIGN | HYPERDOS_X86_16_FLAG_OVERFLOW;
    processor->flags = (uint16_t)((processor->flags & ~statusMask) | statusFlags | HYPERDOS_X86_16_FLAG_RESERVED);
}

static uint16_t hyperdos_x86_16_sign_zero_parity_flags(uint16_t value, uint16_t valueMask, uint16_t signBit)
{
    uint16_t statusFlags  = 0;
    value                &= valueMask;

    if (value == 0u)
    {
        statusFlags |= HYPERDOS_X86_16_FLAG_ZERO;
    }
    if ((value & signBit) != 0u)
    {
        statusFlags |= HYPERDOS_X86_16_FLAG_SIGN;
    }
    if (hyperdos_x86_16_has_even_parity((uint8_t)value))
    {
        statusFlags |= HYPERDOS_X86_16_FLAG_PARITY;
    }
    return statusFlags;
}

static uint16_t hyperdos_x86_16_execute_arithmetic_logic_operation(hyperdos_x86_16_processor*                 processor,
                                                                   hyperdos_x86_16_arithmetic_logic_operation operation,
                                                                   uint16_t                                   leftValue,
                                                                   uint16_t rightValue,
                                                                   uint16_t valueMask,
                                                                   uint16_t signBit)
{
    uint32_t maskedLeftValue  = leftValue & valueMask;
    uint32_t maskedRightValue = rightValue & valueMask;
    uint32_t fullResult       = 0;
    uint16_t result           = 0;
    uint16_t statusFlags      = 0;

    switch (operation)
    {
    case HYPERDOS_X86_16_ARITHMETIC_LOGIC_ADD:
    case HYPERDOS_X86_16_ARITHMETIC_LOGIC_ADC:
    {
        uint32_t carryValue = (operation == HYPERDOS_X86_16_ARITHMETIC_LOGIC_ADC &&
                               (processor->flags & HYPERDOS_X86_16_FLAG_CARRY) != 0u)
                                      ? 1u
                                      : 0u;
        fullResult          = maskedLeftValue + maskedRightValue + carryValue;
        result              = (uint16_t)(fullResult & valueMask);
        statusFlags         = hyperdos_x86_16_sign_zero_parity_flags(result, valueMask, signBit);
        if (fullResult > valueMask)
        {
            statusFlags |= HYPERDOS_X86_16_FLAG_CARRY;
        }
        if (((maskedLeftValue ^ maskedRightValue ^ result) & HYPERDOS_X86_16_AUXILIARY_CARRY_BIT) != 0u)
        {
            statusFlags |= HYPERDOS_X86_16_FLAG_AUXILIARY_CARRY;
        }
        if (((~(maskedLeftValue ^ maskedRightValue) & (maskedLeftValue ^ result)) & signBit) != 0u)
        {
            statusFlags |= HYPERDOS_X86_16_FLAG_OVERFLOW;
        }
        hyperdos_x86_16_replace_status_flags(processor, statusFlags);
        return result;
    }

    case HYPERDOS_X86_16_ARITHMETIC_LOGIC_SUB:
    case HYPERDOS_X86_16_ARITHMETIC_LOGIC_CMP:
    case HYPERDOS_X86_16_ARITHMETIC_LOGIC_SBB:
    {
        uint32_t borrowValue     = (operation == HYPERDOS_X86_16_ARITHMETIC_LOGIC_SBB &&
                                (processor->flags & HYPERDOS_X86_16_FLAG_CARRY) != 0u)
                                           ? 1u
                                           : 0u;
        uint32_t subtrahendValue = maskedRightValue + borrowValue;
        fullResult               = (maskedLeftValue - subtrahendValue) & HYPERDOS_X86_16_EXTENDED_WORD_MASK;
        result                   = (uint16_t)(fullResult & valueMask);
        statusFlags              = hyperdos_x86_16_sign_zero_parity_flags(result, valueMask, signBit);
        if (maskedLeftValue < subtrahendValue)
        {
            statusFlags |= HYPERDOS_X86_16_FLAG_CARRY;
        }
        if (((maskedLeftValue ^ maskedRightValue ^ result) & HYPERDOS_X86_16_AUXILIARY_CARRY_BIT) != 0u)
        {
            statusFlags |= HYPERDOS_X86_16_FLAG_AUXILIARY_CARRY;
        }
        if ((((maskedLeftValue ^ maskedRightValue) & (maskedLeftValue ^ result)) & signBit) != 0u)
        {
            statusFlags |= HYPERDOS_X86_16_FLAG_OVERFLOW;
        }
        hyperdos_x86_16_replace_status_flags(processor, statusFlags);
        return result;
    }

    case HYPERDOS_X86_16_ARITHMETIC_LOGIC_AND:
        result = (uint16_t)((maskedLeftValue & maskedRightValue) & valueMask);
        hyperdos_x86_16_replace_status_flags(processor,
                                             hyperdos_x86_16_sign_zero_parity_flags(result, valueMask, signBit));
        return result;

    case HYPERDOS_X86_16_ARITHMETIC_LOGIC_OR:
        result = (uint16_t)((maskedLeftValue | maskedRightValue) & valueMask);
        hyperdos_x86_16_replace_status_flags(processor,
                                             hyperdos_x86_16_sign_zero_parity_flags(result, valueMask, signBit));
        return result;

    case HYPERDOS_X86_16_ARITHMETIC_LOGIC_XOR:
        result = (uint16_t)((maskedLeftValue ^ maskedRightValue) & valueMask);
        hyperdos_x86_16_replace_status_flags(processor,
                                             hyperdos_x86_16_sign_zero_parity_flags(result, valueMask, signBit));
        return result;
    }

    return 0;
}

static void hyperdos_x86_16_update_increment_decrement_flags(hyperdos_x86_16_processor*                 processor,
                                                             hyperdos_x86_16_arithmetic_logic_operation operation,
                                                             uint16_t                                   value,
                                                             uint16_t                                   valueMask,
                                                             uint16_t                                   signBit)
{
    uint16_t carryFlag = (uint16_t)(processor->flags & HYPERDOS_X86_16_FLAG_CARRY);
    (void)hyperdos_x86_16_execute_arithmetic_logic_operation(processor, operation, value, 1u, valueMask, signBit);
    processor->flags = (uint16_t)((processor->flags & ~HYPERDOS_X86_16_FLAG_CARRY) | carryFlag |
                                  HYPERDOS_X86_16_FLAG_RESERVED);
}

static hyperdos_x86_16_decoded_operand hyperdos_x86_16_decode_register_memory_operand(
        hyperdos_x86_16_processor*             processor,
        uint8_t                                registerMemoryByte,
        int                                    hasSegmentOverride,
        hyperdos_x86_16_segment_register_index segmentOverride)
{
    hyperdos_x86_16_decoded_operand operand;
    uint8_t                         addressMode = (uint8_t)(registerMemoryByte >> HYPERDOS_X86_16_MODRM_MODE_SHIFT);
    uint8_t  registerMemoryIndex = (uint8_t)(registerMemoryByte & HYPERDOS_X86_16_MODRM_REGISTER_MEMORY_MASK);
    uint16_t baseOffset          = 0;
    int16_t  displacement        = 0;

    operand.isRegister      = (uint8_t)(addressMode == HYPERDOS_X86_16_MODRM_MODE_REGISTER);
    operand.registerIndex   = registerMemoryIndex;
    operand.segmentRegister = HYPERDOS_X86_16_SEGMENT_REGISTER_DATA;
    operand.offset          = 0;

    if (operand.isRegister)
    {
        return operand;
    }

    switch (registerMemoryIndex)
    {
    case 0:
        baseOffset              = (uint16_t)(processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE] +
                                processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_SOURCE_INDEX]);
        operand.segmentRegister = HYPERDOS_X86_16_SEGMENT_REGISTER_DATA;
        break;
    case 1:
        baseOffset              = (uint16_t)(processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE] +
                                processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DESTINATION_INDEX]);
        operand.segmentRegister = HYPERDOS_X86_16_SEGMENT_REGISTER_DATA;
        break;
    case 2:
        baseOffset = (uint16_t)(processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE_POINTER] +
                                processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_SOURCE_INDEX]);
        operand.segmentRegister = HYPERDOS_X86_16_SEGMENT_REGISTER_STACK;
        break;
    case 3:
        baseOffset = (uint16_t)(processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE_POINTER] +
                                processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DESTINATION_INDEX]);
        operand.segmentRegister = HYPERDOS_X86_16_SEGMENT_REGISTER_STACK;
        break;
    case 4:
        baseOffset              = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_SOURCE_INDEX];
        operand.segmentRegister = HYPERDOS_X86_16_SEGMENT_REGISTER_DATA;
        break;
    case 5:
        baseOffset              = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DESTINATION_INDEX];
        operand.segmentRegister = HYPERDOS_X86_16_SEGMENT_REGISTER_DATA;
        break;
    case 6:
        if (addressMode == HYPERDOS_X86_16_MODRM_MODE_MEMORY_NO_DISPLACEMENT)
        {
            operand.offset          = hyperdos_x86_16_fetch_instruction_word(processor);
            operand.segmentRegister = HYPERDOS_X86_16_SEGMENT_REGISTER_DATA;
            if (hasSegmentOverride)
            {
                operand.segmentRegister = segmentOverride;
            }
            return operand;
        }
        baseOffset              = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE_POINTER];
        operand.segmentRegister = HYPERDOS_X86_16_SEGMENT_REGISTER_STACK;
        break;
    case 7:
        baseOffset              = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_BASE];
        operand.segmentRegister = HYPERDOS_X86_16_SEGMENT_REGISTER_DATA;
        break;
    }

    if (addressMode == HYPERDOS_X86_16_MODRM_MODE_MEMORY_BYTE_DISPLACEMENT)
    {
        displacement = (int16_t)(int8_t)hyperdos_x86_16_fetch_instruction_byte(processor);
    }
    else if (addressMode == HYPERDOS_X86_16_MODRM_MODE_MEMORY_WORD_DISPLACEMENT)
    {
        displacement = (int16_t)hyperdos_x86_16_fetch_instruction_word(processor);
    }

    operand.offset = (uint16_t)(baseOffset + displacement);
    if (hasSegmentOverride)
    {
        operand.segmentRegister = segmentOverride;
    }
    return operand;
}

static uint16_t hyperdos_x86_16_read_operand_value(const hyperdos_x86_16_processor*       processor,
                                                   const hyperdos_x86_16_decoded_operand* operand,
                                                   int                                    isWord)
{
    if (operand->isRegister)
    {
        if (isWord)
        {
            return processor->generalRegisters[operand->registerIndex];
        }
        return hyperdos_x86_16_read_byte_register(processor, operand->registerIndex);
    }

    if (isWord)
    {
        return hyperdos_x86_16_read_memory_word_fast(processor, operand->segmentRegister, operand->offset);
    }
    return hyperdos_x86_16_read_memory_byte_fast(processor, operand->segmentRegister, operand->offset);
}

static void hyperdos_x86_16_write_operand_value(hyperdos_x86_16_processor*             processor,
                                                const hyperdos_x86_16_decoded_operand* operand,
                                                int                                    isWord,
                                                uint16_t                               value)
{
    if (operand->isRegister)
    {
        if (isWord)
        {
            processor->generalRegisters[operand->registerIndex] = value;
        }
        else
        {
            hyperdos_x86_16_write_byte_register(processor, operand->registerIndex, (uint8_t)value);
        }
        return;
    }

    if (isWord)
    {
        hyperdos_x86_16_write_memory_word_fast(processor, operand->segmentRegister, operand->offset, value);
    }
    else
    {
        hyperdos_x86_16_write_memory_byte_fast(processor, operand->segmentRegister, operand->offset, (uint8_t)value);
    }
}

static void hyperdos_x86_16_push_word(hyperdos_x86_16_processor* processor, uint16_t value)
{
    uint16_t stackPointer = (uint16_t)(processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER] -
                                       HYPERDOS_X86_16_WORD_SIZE);
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER] = stackPointer;
    hyperdos_x86_16_write_memory_word_fast(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_STACK, stackPointer, value);
}

static uint16_t hyperdos_x86_16_pop_word(hyperdos_x86_16_processor* processor)
{
    uint16_t stackPointer = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER];
    uint16_t value        = hyperdos_x86_16_read_memory_word_fast(processor,
                                                           HYPERDOS_X86_16_SEGMENT_REGISTER_STACK,
                                                           stackPointer);
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER] = (uint16_t)(stackPointer +
                                                                                             HYPERDOS_X86_16_WORD_SIZE);
    return value;
}

static void hyperdos_x86_16_jump_relative(hyperdos_x86_16_processor* processor, int16_t displacement)
{
    processor->instructionPointer = (uint16_t)(processor->instructionPointer + displacement);
}

static int hyperdos_x86_16_evaluate_condition(const hyperdos_x86_16_processor* processor, uint8_t conditionCode)
{
    int overflow = (processor->flags & HYPERDOS_X86_16_FLAG_OVERFLOW) != 0u;
    int carry    = (processor->flags & HYPERDOS_X86_16_FLAG_CARRY) != 0u;
    int zero     = (processor->flags & HYPERDOS_X86_16_FLAG_ZERO) != 0u;
    int sign     = (processor->flags & HYPERDOS_X86_16_FLAG_SIGN) != 0u;
    int parity   = (processor->flags & HYPERDOS_X86_16_FLAG_PARITY) != 0u;

    switch (conditionCode & HYPERDOS_X86_16_NIBBLE_MASK)
    {
    case HYPERDOS_X86_16_CONDITION_OVERFLOW:
        return overflow;
    case HYPERDOS_X86_16_CONDITION_NOT_OVERFLOW:
        return !overflow;
    case HYPERDOS_X86_16_CONDITION_CARRY:
        return carry;
    case HYPERDOS_X86_16_CONDITION_NOT_CARRY:
        return !carry;
    case HYPERDOS_X86_16_CONDITION_ZERO:
        return zero;
    case HYPERDOS_X86_16_CONDITION_NOT_ZERO:
        return !zero;
    case HYPERDOS_X86_16_CONDITION_BELOW_OR_EQUAL:
        return carry || zero;
    case HYPERDOS_X86_16_CONDITION_ABOVE:
        return !carry && !zero;
    case HYPERDOS_X86_16_CONDITION_SIGN:
        return sign;
    case HYPERDOS_X86_16_CONDITION_NOT_SIGN:
        return !sign;
    case HYPERDOS_X86_16_CONDITION_PARITY:
        return parity;
    case HYPERDOS_X86_16_CONDITION_NOT_PARITY:
        return !parity;
    case HYPERDOS_X86_16_CONDITION_LESS:
        return sign != overflow;
    case HYPERDOS_X86_16_CONDITION_GREATER_OR_EQUAL:
        return sign == overflow;
    case HYPERDOS_X86_16_CONDITION_LESS_OR_EQUAL:
        return zero || (sign != overflow);
    case HYPERDOS_X86_16_CONDITION_GREATER:
        return !zero && (sign == overflow);
    }
    return 0;
}

static int hyperdos_x86_16_can_use_contiguous_physical_range(const hyperdos_x86_16_processor*       processor,
                                                             hyperdos_x86_16_segment_register_index segmentRegister,
                                                             uint16_t                               offset,
                                                             size_t                                 byteCount,
                                                             uint32_t*                              physicalAddress)
{
    *physicalAddress = hyperdos_x86_16_physical_address_from_base(processor->segmentBases[segmentRegister], offset);
    if (byteCount == 0u)
    {
        return 1;
    }
    if (byteCount > (size_t)(HYPERDOS_X86_16_SEGMENT_BYTE_COUNT - offset))
    {
        return 0;
    }
    if (*physicalAddress > HYPERDOS_X86_16_ADDRESS_MASK)
    {
        return 0;
    }
    if (byteCount > (size_t)(HYPERDOS_X86_16_MEMORY_SIZE - *physicalAddress))
    {
        return 0;
    }
    return 1;
}

static int hyperdos_x86_16_ranges_do_not_overlap(uint32_t firstAddress, uint32_t secondAddress, size_t byteCount)
{
    return firstAddress + byteCount <= secondAddress || secondAddress + byteCount <= firstAddress;
}

static hyperdos_x86_16_execution_result hyperdos_x86_16_execute_interrupt_with_return_address(
        hyperdos_x86_16_processor* processor,
        uint8_t                    interruptNumber,
        uint16_t                   returnSegment,
        uint16_t                   returnOffset)
{
    if (processor->interruptHandler != NULL)
    {
        hyperdos_x86_16_execution_result handledResult = processor->interruptHandler(processor,
                                                                                     interruptNumber,
                                                                                     processor->userContext);
        if (handledResult != HYPERDOS_X86_16_EXECUTION_INTERRUPT_NOT_HANDLED)
        {
            return handledResult;
        }
    }

    {
        uint32_t vectorAddress = (uint32_t)interruptNumber * HYPERDOS_X86_16_INTERRUPT_VECTOR_BYTE_COUNT;
        uint16_t vectorOffset  = hyperdos_x86_16_read_physical_word(processor, vectorAddress);
        uint16_t vectorSegment = hyperdos_x86_16_read_physical_word(processor,
                                                                    vectorAddress +
                                                                            HYPERDOS_X86_16_FAR_POINTER_SEGMENT_OFFSET);
        if (vectorOffset == 0u && vectorSegment == 0u)
        {
            return HYPERDOS_X86_16_EXECUTION_INTERRUPT_NOT_HANDLED;
        }

        hyperdos_x86_16_push_word(processor, processor->flags);
        hyperdos_x86_16_push_word(processor, returnSegment);
        hyperdos_x86_16_push_word(processor, returnOffset);
        processor->flags = (uint16_t)((processor->flags &
                                       ~(HYPERDOS_X86_16_FLAG_TRAP | HYPERDOS_X86_16_FLAG_INTERRUPT_ENABLE)) |
                                      HYPERDOS_X86_16_FLAG_RESERVED);
        hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_CODE, vectorSegment);
        processor->instructionPointer = vectorOffset;
    }

    return HYPERDOS_X86_16_EXECUTION_OK;
}

static hyperdos_x86_16_execution_result hyperdos_x86_16_execute_interrupt(hyperdos_x86_16_processor* processor,
                                                                          uint8_t                    interruptNumber)
{
    return hyperdos_x86_16_execute_interrupt_with_return_address(processor,
                                                                 interruptNumber,
                                                                 processor->segmentRegisters
                                                                         [HYPERDOS_X86_16_SEGMENT_REGISTER_CODE],
                                                                 processor->instructionPointer);
}

static hyperdos_x86_16_execution_result hyperdos_x86_16_execute_arithmetic_logic_instruction(
        hyperdos_x86_16_processor*             processor,
        uint8_t                                operationCode,
        int                                    hasSegmentOverride,
        hyperdos_x86_16_segment_register_index segmentOverride)
{
    uint8_t instructionForm = (uint8_t)(operationCode & HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
    hyperdos_x86_16_arithmetic_logic_operation
             operation = (hyperdos_x86_16_arithmetic_logic_operation)((operationCode >>
                                                                      HYPERDOS_X86_16_MODRM_REGISTER_SHIFT) &
                                                                     HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
    int      isWord    = (instructionForm & HYPERDOS_X86_16_ARITHMETIC_FORM_WORD_BIT) != 0u;
    uint16_t valueMask = hyperdos_x86_16_operand_value_mask(isWord);
    uint16_t signBit   = hyperdos_x86_16_operand_sign_bit(isWord);

    if (instructionForm <= HYPERDOS_X86_16_ARITHMETIC_FORM_REGISTER_MEMORY_MAXIMUM)
    {
        uint8_t registerMemoryByte = hyperdos_x86_16_fetch_instruction_byte(processor);
        uint8_t registerIndex      = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_16_MODRM_REGISTER_SHIFT) &
                                          HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
        hyperdos_x86_16_decoded_operand operand    = hyperdos_x86_16_decode_register_memory_operand(processor,
                                                                                                 registerMemoryByte,
                                                                                                 hasSegmentOverride,
                                                                                                 segmentOverride);
        uint16_t                        leftValue  = 0;
        uint16_t                        rightValue = 0;
        uint16_t                        result     = 0;

        if ((instructionForm & HYPERDOS_X86_16_ARITHMETIC_FORM_REGISTER_DESTINATION_BIT) != 0u)
        {
            leftValue  = isWord ? processor->generalRegisters[registerIndex]
                                : hyperdos_x86_16_read_byte_register(processor, registerIndex);
            rightValue = hyperdos_x86_16_read_operand_value(processor, &operand, isWord);
            result     = hyperdos_x86_16_execute_arithmetic_logic_operation(processor,
                                                                        operation,
                                                                        leftValue,
                                                                        rightValue,
                                                                        valueMask,
                                                                        signBit);
            if (operation != HYPERDOS_X86_16_ARITHMETIC_LOGIC_CMP)
            {
                if (isWord)
                {
                    processor->generalRegisters[registerIndex] = result;
                }
                else
                {
                    hyperdos_x86_16_write_byte_register(processor, registerIndex, (uint8_t)result);
                }
            }
        }
        else
        {
            leftValue  = hyperdos_x86_16_read_operand_value(processor, &operand, isWord);
            rightValue = isWord ? processor->generalRegisters[registerIndex]
                                : hyperdos_x86_16_read_byte_register(processor, registerIndex);
            result     = hyperdos_x86_16_execute_arithmetic_logic_operation(processor,
                                                                        operation,
                                                                        leftValue,
                                                                        rightValue,
                                                                        valueMask,
                                                                        signBit);
            if (operation != HYPERDOS_X86_16_ARITHMETIC_LOGIC_CMP)
            {
                hyperdos_x86_16_write_operand_value(processor, &operand, isWord, result);
            }
        }
        return HYPERDOS_X86_16_EXECUTION_OK;
    }

    if (instructionForm == HYPERDOS_X86_16_ARITHMETIC_FORM_ACCUMULATOR_BYTE)
    {
        uint16_t leftValue  = hyperdos_x86_16_read_byte_register(processor, 0u);
        uint16_t rightValue = hyperdos_x86_16_fetch_instruction_byte(processor);
        uint16_t result     = hyperdos_x86_16_execute_arithmetic_logic_operation(processor,
                                                                             operation,
                                                                             leftValue,
                                                                             rightValue,
                                                                             HYPERDOS_X86_16_BYTE_MASK,
                                                                             HYPERDOS_X86_16_BYTE_SIGN_BIT);
        if (operation != HYPERDOS_X86_16_ARITHMETIC_LOGIC_CMP)
        {
            hyperdos_x86_16_write_byte_register(processor, 0u, (uint8_t)result);
        }
        return HYPERDOS_X86_16_EXECUTION_OK;
    }

    if (instructionForm == HYPERDOS_X86_16_ARITHMETIC_FORM_ACCUMULATOR_WORD)
    {
        uint16_t leftValue  = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR];
        uint16_t rightValue = hyperdos_x86_16_fetch_instruction_word(processor);
        uint16_t result     = hyperdos_x86_16_execute_arithmetic_logic_operation(processor,
                                                                             operation,
                                                                             leftValue,
                                                                             rightValue,
                                                                             HYPERDOS_X86_16_WORD_MASK,
                                                                             HYPERDOS_X86_16_WORD_SIGN_BIT);
        if (operation != HYPERDOS_X86_16_ARITHMETIC_LOGIC_CMP)
        {
            processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = result;
        }
        return HYPERDOS_X86_16_EXECUTION_OK;
    }

    return HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION;
}

static hyperdos_x86_16_execution_result hyperdos_x86_16_execute_group_one_instruction(
        hyperdos_x86_16_processor*             processor,
        uint8_t                                operationCode,
        int                                    hasSegmentOverride,
        hyperdos_x86_16_segment_register_index segmentOverride)
{
    uint8_t registerMemoryByte = hyperdos_x86_16_fetch_instruction_byte(processor);
    hyperdos_x86_16_arithmetic_logic_operation
            operation = (hyperdos_x86_16_arithmetic_logic_operation)((registerMemoryByte >>
                                                                      HYPERDOS_X86_16_MODRM_REGISTER_SHIFT) &
                                                                     HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
    int     isWord    = operationCode != HYPERDOS_X86_16_OPERATION_CODE_GROUP_ONE_BYTE_IMMEDIATE &&
                 operationCode != HYPERDOS_X86_16_OPERATION_CODE_GROUP_ONE_BYTE_IMMEDIATE_ALIAS;
    uint16_t                        valueMask  = hyperdos_x86_16_operand_value_mask(isWord);
    uint16_t                        signBit    = hyperdos_x86_16_operand_sign_bit(isWord);
    hyperdos_x86_16_decoded_operand operand    = hyperdos_x86_16_decode_register_memory_operand(processor,
                                                                                             registerMemoryByte,
                                                                                             hasSegmentOverride,
                                                                                             segmentOverride);
    uint16_t                        leftValue  = hyperdos_x86_16_read_operand_value(processor, &operand, isWord);
    uint16_t                        rightValue = 0;
    uint16_t                        result     = 0;

    if (operationCode == HYPERDOS_X86_16_OPERATION_CODE_GROUP_ONE_SIGN_EXTENDED_IMMEDIATE)
    {
        rightValue = hyperdos_x86_16_sign_extend_byte(hyperdos_x86_16_fetch_instruction_byte(processor));
    }
    else if (isWord)
    {
        rightValue = hyperdos_x86_16_fetch_instruction_word(processor);
    }
    else
    {
        rightValue = hyperdos_x86_16_fetch_instruction_byte(processor);
    }

    result = hyperdos_x86_16_execute_arithmetic_logic_operation(processor,
                                                                operation,
                                                                leftValue,
                                                                rightValue,
                                                                valueMask,
                                                                signBit);
    if (operation != HYPERDOS_X86_16_ARITHMETIC_LOGIC_CMP)
    {
        hyperdos_x86_16_write_operand_value(processor, &operand, isWord, result);
    }
    return HYPERDOS_X86_16_EXECUTION_OK;
}

static void hyperdos_x86_16_test_values(hyperdos_x86_16_processor* processor,
                                        uint16_t                   leftValue,
                                        uint16_t                   rightValue,
                                        uint16_t                   valueMask,
                                        uint16_t                   signBit)
{
    uint16_t result = (uint16_t)((leftValue & rightValue) & valueMask);
    hyperdos_x86_16_replace_status_flags(processor, hyperdos_x86_16_sign_zero_parity_flags(result, valueMask, signBit));
}

static hyperdos_x86_16_execution_result hyperdos_x86_16_execute_move_instruction(
        hyperdos_x86_16_processor*             processor,
        uint8_t                                operationCode,
        int                                    hasSegmentOverride,
        hyperdos_x86_16_segment_register_index segmentOverride)
{
    uint8_t registerMemoryByte              = hyperdos_x86_16_fetch_instruction_byte(processor);
    uint8_t registerIndex                   = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_16_MODRM_REGISTER_SHIFT) &
                                      HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
    int     isWord                          = (operationCode & HYPERDOS_X86_16_OPERATION_CODE_WORD_BIT) != 0u;
    hyperdos_x86_16_decoded_operand operand = hyperdos_x86_16_decode_register_memory_operand(processor,
                                                                                             registerMemoryByte,
                                                                                             hasSegmentOverride,
                                                                                             segmentOverride);

    if ((operationCode & HYPERDOS_X86_16_ARITHMETIC_FORM_REGISTER_DESTINATION_BIT) != 0u)
    {
        uint16_t value = hyperdos_x86_16_read_operand_value(processor, &operand, isWord);
        if (isWord)
        {
            processor->generalRegisters[registerIndex] = value;
        }
        else
        {
            hyperdos_x86_16_write_byte_register(processor, registerIndex, (uint8_t)value);
        }
    }
    else
    {
        uint16_t value = isWord ? processor->generalRegisters[registerIndex]
                                : hyperdos_x86_16_read_byte_register(processor, registerIndex);
        hyperdos_x86_16_write_operand_value(processor, &operand, isWord, value);
    }
    return HYPERDOS_X86_16_EXECUTION_OK;
}

static hyperdos_x86_16_execution_result hyperdos_x86_16_execute_lea_instruction(
        hyperdos_x86_16_processor*             processor,
        int                                    hasSegmentOverride,
        hyperdos_x86_16_segment_register_index segmentOverride)
{
    uint8_t registerMemoryByte              = hyperdos_x86_16_fetch_instruction_byte(processor);
    uint8_t registerIndex                   = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_16_MODRM_REGISTER_SHIFT) &
                                      HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
    hyperdos_x86_16_decoded_operand operand = hyperdos_x86_16_decode_register_memory_operand(processor,
                                                                                             registerMemoryByte,
                                                                                             hasSegmentOverride,
                                                                                             segmentOverride);
    if (operand.isRegister)
    {
        return HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }
    processor->generalRegisters[registerIndex] = operand.offset;
    return HYPERDOS_X86_16_EXECUTION_OK;
}

static hyperdos_x86_16_execution_result hyperdos_x86_16_execute_string_instruction(
        hyperdos_x86_16_processor*             processor,
        uint8_t                                operationCode,
        hyperdos_x86_16_repeat_mode            repeatMode,
        int                                    hasSegmentOverride,
        hyperdos_x86_16_segment_register_index segmentOverride)
{
    int       isWord              = (operationCode & HYPERDOS_X86_16_OPERATION_CODE_WORD_BIT) != 0u;
    uint16_t  elementSize         = isWord ? HYPERDOS_X86_16_WORD_SIZE : HYPERDOS_X86_16_BYTE_SIZE;
    int16_t   directionStep       = (processor->flags & HYPERDOS_X86_16_FLAG_DIRECTION) != 0u ? (int16_t)-elementSize
                                                                                              : (int16_t)elementSize;
    uint16_t* sourceIndexRegister = &processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_SOURCE_INDEX];
    uint16_t*
            destinationIndexRegister = &processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DESTINATION_INDEX];
    uint16_t* counterRegister        = &processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER];
    uint16_t  repeatCount            = repeatMode == HYPERDOS_X86_16_REPEAT_NONE ? 1u : *counterRegister;
    hyperdos_x86_16_segment_register_index sourceSegment = hasSegmentOverride ? segmentOverride
                                                                              : HYPERDOS_X86_16_SEGMENT_REGISTER_DATA;

    if (repeatCount == 0u)
    {
        return HYPERDOS_X86_16_EXECUTION_OK;
    }

    if ((operationCode == HYPERDOS_X86_16_OPERATION_CODE_MOVE_STRING_BYTE ||
         operationCode == HYPERDOS_X86_16_OPERATION_CODE_MOVE_STRING_WORD) &&
        repeatMode != HYPERDOS_X86_16_REPEAT_NONE && processor->bus == NULL &&
        (processor->flags & HYPERDOS_X86_16_FLAG_DIRECTION) == 0u)
    {
        size_t   byteCount                  = (size_t)repeatCount * elementSize;
        uint32_t sourcePhysicalAddress      = 0;
        uint32_t destinationPhysicalAddress = 0;
        if (hyperdos_x86_16_can_use_contiguous_physical_range(processor,
                                                              sourceSegment,
                                                              *sourceIndexRegister,
                                                              byteCount,
                                                              &sourcePhysicalAddress) &&
            hyperdos_x86_16_can_use_contiguous_physical_range(processor,
                                                              HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA,
                                                              *destinationIndexRegister,
                                                              byteCount,
                                                              &destinationPhysicalAddress) &&
            hyperdos_x86_16_ranges_do_not_overlap(sourcePhysicalAddress, destinationPhysicalAddress, byteCount))
        {
            memcpy(processor->memory + destinationPhysicalAddress,
                   processor->memory + sourcePhysicalAddress,
                   byteCount);
            *sourceIndexRegister      = (uint16_t)(*sourceIndexRegister + byteCount);
            *destinationIndexRegister = (uint16_t)(*destinationIndexRegister + byteCount);
            *counterRegister          = 0;
            return HYPERDOS_X86_16_EXECUTION_OK;
        }
    }

    if (operationCode == HYPERDOS_X86_16_OPERATION_CODE_STORE_STRING_BYTE &&
        repeatMode != HYPERDOS_X86_16_REPEAT_NONE && processor->bus == NULL &&
        (processor->flags & HYPERDOS_X86_16_FLAG_DIRECTION) == 0u)
    {
        size_t   byteCount                  = repeatCount;
        uint32_t destinationPhysicalAddress = 0;
        if (hyperdos_x86_16_can_use_contiguous_physical_range(processor,
                                                              HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA,
                                                              *destinationIndexRegister,
                                                              byteCount,
                                                              &destinationPhysicalAddress))
        {
            memset(processor->memory + destinationPhysicalAddress,
                   hyperdos_x86_16_read_byte_register(processor, 0u),
                   byteCount);
            *destinationIndexRegister = (uint16_t)(*destinationIndexRegister + byteCount);
            *counterRegister          = 0;
            return HYPERDOS_X86_16_EXECUTION_OK;
        }
    }

    while (repeatCount > 0u)
    {
        switch (operationCode)
        {
        case HYPERDOS_X86_16_OPERATION_CODE_MOVE_STRING_BYTE:
        case HYPERDOS_X86_16_OPERATION_CODE_MOVE_STRING_WORD:
        {
            uint16_t value = isWord ? hyperdos_x86_16_read_memory_word_fast(processor,
                                                                            sourceSegment,
                                                                            *sourceIndexRegister)
                                    : hyperdos_x86_16_read_memory_byte_fast(processor,
                                                                            sourceSegment,
                                                                            *sourceIndexRegister);
            if (isWord)
            {
                hyperdos_x86_16_write_memory_word_fast(processor,
                                                       HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA,
                                                       *destinationIndexRegister,
                                                       value);
            }
            else
            {
                hyperdos_x86_16_write_memory_byte_fast(processor,
                                                       HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA,
                                                       *destinationIndexRegister,
                                                       (uint8_t)value);
            }
            *sourceIndexRegister      = (uint16_t)(*sourceIndexRegister + directionStep);
            *destinationIndexRegister = (uint16_t)(*destinationIndexRegister + directionStep);
            break;
        }

        case HYPERDOS_X86_16_OPERATION_CODE_COMPARE_STRING_BYTE:
        case HYPERDOS_X86_16_OPERATION_CODE_COMPARE_STRING_WORD:
        {
            uint16_t sourceValue = isWord ? hyperdos_x86_16_read_memory_word_fast(processor,
                                                                                  sourceSegment,
                                                                                  *sourceIndexRegister)
                                          : hyperdos_x86_16_read_memory_byte_fast(processor,
                                                                                  sourceSegment,
                                                                                  *sourceIndexRegister);
            uint16_t destinationValue =
                    isWord ? hyperdos_x86_16_read_memory_word_fast(processor,
                                                                   HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA,
                                                                   *destinationIndexRegister)
                           : hyperdos_x86_16_read_memory_byte_fast(processor,
                                                                   HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA,
                                                                   *destinationIndexRegister);
            (void)hyperdos_x86_16_execute_arithmetic_logic_operation(processor,
                                                                     HYPERDOS_X86_16_ARITHMETIC_LOGIC_CMP,
                                                                     sourceValue,
                                                                     destinationValue,
                                                                     hyperdos_x86_16_operand_value_mask(isWord),
                                                                     hyperdos_x86_16_operand_sign_bit(isWord));
            *sourceIndexRegister      = (uint16_t)(*sourceIndexRegister + directionStep);
            *destinationIndexRegister = (uint16_t)(*destinationIndexRegister + directionStep);
            break;
        }

        case HYPERDOS_X86_16_OPERATION_CODE_STORE_STRING_BYTE:
        case HYPERDOS_X86_16_OPERATION_CODE_STORE_STRING_WORD:
        {
            uint16_t value = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR];
            if (isWord)
            {
                hyperdos_x86_16_write_memory_word_fast(processor,
                                                       HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA,
                                                       *destinationIndexRegister,
                                                       value);
            }
            else
            {
                hyperdos_x86_16_write_memory_byte_fast(processor,
                                                       HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA,
                                                       *destinationIndexRegister,
                                                       (uint8_t)value);
            }
            *destinationIndexRegister = (uint16_t)(*destinationIndexRegister + directionStep);
            break;
        }

        case HYPERDOS_X86_16_OPERATION_CODE_LOAD_STRING_BYTE:
        case HYPERDOS_X86_16_OPERATION_CODE_LOAD_STRING_WORD:
        {
            uint16_t value = isWord ? hyperdos_x86_16_read_memory_word_fast(processor,
                                                                            sourceSegment,
                                                                            *sourceIndexRegister)
                                    : hyperdos_x86_16_read_memory_byte_fast(processor,
                                                                            sourceSegment,
                                                                            *sourceIndexRegister);
            if (isWord)
            {
                processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = value;
            }
            else
            {
                hyperdos_x86_16_write_byte_register(processor, 0u, (uint8_t)value);
            }
            *sourceIndexRegister = (uint16_t)(*sourceIndexRegister + directionStep);
            break;
        }

        case HYPERDOS_X86_16_OPERATION_CODE_SCAN_STRING_BYTE:
        case HYPERDOS_X86_16_OPERATION_CODE_SCAN_STRING_WORD:
        {
            uint16_t accumulatorValue = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR];
            uint16_t destinationValue =
                    isWord ? hyperdos_x86_16_read_memory_word_fast(processor,
                                                                   HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA,
                                                                   *destinationIndexRegister)
                           : hyperdos_x86_16_read_memory_byte_fast(processor,
                                                                   HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA,
                                                                   *destinationIndexRegister);
            (void)hyperdos_x86_16_execute_arithmetic_logic_operation(processor,
                                                                     HYPERDOS_X86_16_ARITHMETIC_LOGIC_CMP,
                                                                     accumulatorValue,
                                                                     destinationValue,
                                                                     hyperdos_x86_16_operand_value_mask(isWord),
                                                                     hyperdos_x86_16_operand_sign_bit(isWord));
            *destinationIndexRegister = (uint16_t)(*destinationIndexRegister + directionStep);
            break;
        }

        default:
            return HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION;
        }

        --repeatCount;
        if (repeatMode != HYPERDOS_X86_16_REPEAT_NONE)
        {
            *counterRegister = repeatCount;
            if ((operationCode == HYPERDOS_X86_16_OPERATION_CODE_COMPARE_STRING_BYTE ||
                 operationCode == HYPERDOS_X86_16_OPERATION_CODE_COMPARE_STRING_WORD ||
                 operationCode == HYPERDOS_X86_16_OPERATION_CODE_SCAN_STRING_BYTE ||
                 operationCode == HYPERDOS_X86_16_OPERATION_CODE_SCAN_STRING_WORD))
            {
                int zero = (processor->flags & HYPERDOS_X86_16_FLAG_ZERO) != 0u;
                if (repeatMode == HYPERDOS_X86_16_REPEAT_WHILE_EQUAL && !zero)
                {
                    break;
                }
                if (repeatMode == HYPERDOS_X86_16_REPEAT_WHILE_NOT_EQUAL && zero)
                {
                    break;
                }
            }
        }
        else
        {
            break;
        }
    }

    return HYPERDOS_X86_16_EXECUTION_OK;
}

static hyperdos_x86_16_execution_result hyperdos_x86_16_execute_shift_rotate_instruction(
        hyperdos_x86_16_processor*             processor,
        uint8_t                                operationCode,
        int                                    hasSegmentOverride,
        hyperdos_x86_16_segment_register_index segmentOverride)
{
    uint8_t  registerMemoryByte = hyperdos_x86_16_fetch_instruction_byte(processor);
    uint8_t  operation          = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_16_MODRM_REGISTER_SHIFT) &
                                  HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
    int      isWord             = (operationCode & HYPERDOS_X86_16_OPERATION_CODE_WORD_BIT) != 0u;
    uint16_t valueMask          = hyperdos_x86_16_operand_value_mask(isWord);
    uint16_t signBit            = hyperdos_x86_16_operand_sign_bit(isWord);
    uint8_t  bitCount           = isWord ? HYPERDOS_X86_16_WORD_BIT_COUNT : HYPERDOS_X86_16_BYTE_BIT_COUNT;
    uint8_t  shiftCount         = 1u;
    int      usesImmediateCount = operationCode == HYPERDOS_X86_16_OPERATION_CODE_SHIFT_ROTATE_IMMEDIATE_BYTE ||
                             operationCode == HYPERDOS_X86_16_OPERATION_CODE_SHIFT_ROTATE_IMMEDIATE_WORD;
    hyperdos_x86_16_decoded_operand operand       = hyperdos_x86_16_decode_register_memory_operand(processor,
                                                                                             registerMemoryByte,
                                                                                             hasSegmentOverride,
                                                                                             segmentOverride);
    uint16_t                        value         = hyperdos_x86_16_read_operand_value(processor, &operand, isWord);
    uint16_t                        originalValue = value;
    uint8_t                         shiftIndex    = 0;

    if ((operationCode & HYPERDOS_X86_16_OPERATION_CODE_COUNT_IN_COUNTER_BIT) != 0u)
    {
        shiftCount = hyperdos_x86_16_read_byte_register(processor, 1u);
    }
    else if (usesImmediateCount)
    {
        shiftCount = hyperdos_x86_16_fetch_instruction_byte(processor);
    }
    if (hyperdos_x86_16_supports_80186_instructions(processor))
    {
        shiftCount = (uint8_t)(shiftCount & 0x1Fu);
    }

    if (shiftCount == 0u)
    {
        return HYPERDOS_X86_16_EXECUTION_OK;
    }
    if (operation == HYPERDOS_X86_16_SHIFT_ROTATE_UNUSED)
    {
        return HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }

    value &= valueMask;
    for (shiftIndex = 0; shiftIndex < shiftCount; ++shiftIndex)
    {
        uint16_t carryFlag = 0;
        switch (operation)
        {
        case HYPERDOS_X86_16_SHIFT_ROTATE_ROL:
            carryFlag        = (uint16_t)((value & signBit) != 0u ? HYPERDOS_X86_16_FLAG_CARRY : 0u);
            value            = (uint16_t)(((value << 1u) | (carryFlag != 0u ? 1u : 0u)) & valueMask);
            processor->flags = (uint16_t)((processor->flags & ~HYPERDOS_X86_16_FLAG_CARRY) | carryFlag |
                                          HYPERDOS_X86_16_FLAG_RESERVED);
            break;
        case HYPERDOS_X86_16_SHIFT_ROTATE_ROR:
            carryFlag        = (uint16_t)((value & 1u) != 0u ? HYPERDOS_X86_16_FLAG_CARRY : 0u);
            value            = (uint16_t)((value >> 1u) | (carryFlag != 0u ? signBit : 0u));
            processor->flags = (uint16_t)((processor->flags & ~HYPERDOS_X86_16_FLAG_CARRY) | carryFlag |
                                          HYPERDOS_X86_16_FLAG_RESERVED);
            break;
        case HYPERDOS_X86_16_SHIFT_ROTATE_RCL:
        {
            uint16_t oldCarry = (processor->flags & HYPERDOS_X86_16_FLAG_CARRY) != 0u ? 1u : 0u;
            carryFlag         = (uint16_t)((value & signBit) != 0u ? HYPERDOS_X86_16_FLAG_CARRY : 0u);
            value             = (uint16_t)(((value << 1u) | oldCarry) & valueMask);
            processor->flags  = (uint16_t)((processor->flags & ~HYPERDOS_X86_16_FLAG_CARRY) | carryFlag |
                                          HYPERDOS_X86_16_FLAG_RESERVED);
            break;
        }
        case HYPERDOS_X86_16_SHIFT_ROTATE_RCR:
        {
            uint16_t oldCarry = (processor->flags & HYPERDOS_X86_16_FLAG_CARRY) != 0u ? signBit : 0u;
            carryFlag         = (uint16_t)((value & 1u) != 0u ? HYPERDOS_X86_16_FLAG_CARRY : 0u);
            value             = (uint16_t)((value >> 1u) | oldCarry);
            processor->flags  = (uint16_t)((processor->flags & ~HYPERDOS_X86_16_FLAG_CARRY) | carryFlag |
                                          HYPERDOS_X86_16_FLAG_RESERVED);
            break;
        }
        case HYPERDOS_X86_16_SHIFT_ROTATE_SHL:
            carryFlag        = (uint16_t)((value & signBit) != 0u ? HYPERDOS_X86_16_FLAG_CARRY : 0u);
            value            = (uint16_t)((value << 1u) & valueMask);
            processor->flags = (uint16_t)((processor->flags & ~HYPERDOS_X86_16_FLAG_CARRY) | carryFlag |
                                          HYPERDOS_X86_16_FLAG_RESERVED);
            break;
        case HYPERDOS_X86_16_SHIFT_ROTATE_SHR:
            carryFlag        = (uint16_t)((value & 1u) != 0u ? HYPERDOS_X86_16_FLAG_CARRY : 0u);
            value            = (uint16_t)(value >> 1u);
            processor->flags = (uint16_t)((processor->flags & ~HYPERDOS_X86_16_FLAG_CARRY) | carryFlag |
                                          HYPERDOS_X86_16_FLAG_RESERVED);
            break;
        case HYPERDOS_X86_16_SHIFT_ROTATE_SAR:
            carryFlag        = (uint16_t)((value & 1u) != 0u ? HYPERDOS_X86_16_FLAG_CARRY : 0u);
            value            = (uint16_t)((value >> 1u) | (value & signBit));
            processor->flags = (uint16_t)((processor->flags & ~HYPERDOS_X86_16_FLAG_CARRY) | carryFlag |
                                          HYPERDOS_X86_16_FLAG_RESERVED);
            break;
        }
    }

    if (operation >= HYPERDOS_X86_16_SHIFT_ROTATE_SHL)
    {
        uint16_t statusFlags  = (uint16_t)(processor->flags & HYPERDOS_X86_16_FLAG_CARRY);
        statusFlags          |= hyperdos_x86_16_sign_zero_parity_flags(value, valueMask, signBit);
        if (shiftCount == 1u)
        {
            if (operation == HYPERDOS_X86_16_SHIFT_ROTATE_SHL)
            {
                int signAfter  = (value & signBit) != 0u;
                int carryAfter = (processor->flags & HYPERDOS_X86_16_FLAG_CARRY) != 0u;
                if (signAfter != carryAfter)
                {
                    statusFlags |= HYPERDOS_X86_16_FLAG_OVERFLOW;
                }
            }
            else if (operation == HYPERDOS_X86_16_SHIFT_ROTATE_SHR)
            {
                if ((originalValue & signBit) != 0u)
                {
                    statusFlags |= HYPERDOS_X86_16_FLAG_OVERFLOW;
                }
            }
        }
        hyperdos_x86_16_replace_status_flags(processor, statusFlags);
    }
    else if (shiftCount == 1u)
    {
        uint16_t overflowFlag = 0;
        if (operation == HYPERDOS_X86_16_SHIFT_ROTATE_ROL || operation == HYPERDOS_X86_16_SHIFT_ROTATE_RCL)
        {
            int signAfter  = (value & signBit) != 0u;
            int carryAfter = (processor->flags & HYPERDOS_X86_16_FLAG_CARRY) != 0u;
            overflowFlag   = signAfter != carryAfter ? HYPERDOS_X86_16_FLAG_OVERFLOW : 0u;
        }
        else if (operation == HYPERDOS_X86_16_SHIFT_ROTATE_ROR)
        {
            uint16_t topTwoBits = (uint16_t)((value >> (bitCount - HYPERDOS_X86_16_TOP_TWO_BIT_COUNT)) &
                                             HYPERDOS_X86_16_LOW_TWO_BITS_MASK);
            overflowFlag        = (topTwoBits == 1u || topTwoBits == HYPERDOS_X86_16_TOP_TWO_BIT_COUNT)
                                          ? HYPERDOS_X86_16_FLAG_OVERFLOW
                                          : 0u;
        }
        processor->flags = (uint16_t)((processor->flags & ~HYPERDOS_X86_16_FLAG_OVERFLOW) | overflowFlag |
                                      HYPERDOS_X86_16_FLAG_RESERVED);
    }

    hyperdos_x86_16_write_operand_value(processor, &operand, isWord, value);
    return HYPERDOS_X86_16_EXECUTION_OK;
}

static void hyperdos_x86_16_replace_decimal_adjust_flags(hyperdos_x86_16_processor* processor,
                                                         uint8_t                    accumulatorLow,
                                                         uint16_t                   carryFlag,
                                                         uint16_t                   auxiliaryCarryFlag)
{
    const uint16_t statusMask = HYPERDOS_X86_16_FLAG_CARRY | HYPERDOS_X86_16_FLAG_PARITY |
                                HYPERDOS_X86_16_FLAG_AUXILIARY_CARRY | HYPERDOS_X86_16_FLAG_ZERO |
                                HYPERDOS_X86_16_FLAG_SIGN;
    uint16_t statusFlags = hyperdos_x86_16_sign_zero_parity_flags(accumulatorLow,
                                                                  HYPERDOS_X86_16_BYTE_MASK,
                                                                  HYPERDOS_X86_16_BYTE_SIGN_BIT);

    statusFlags      |= carryFlag | auxiliaryCarryFlag;
    processor->flags  = (uint16_t)((processor->flags & ~statusMask) | statusFlags | HYPERDOS_X86_16_FLAG_RESERVED);
}

static void hyperdos_x86_16_execute_decimal_adjust_after_add_instruction(hyperdos_x86_16_processor* processor)
{
    uint8_t  accumulatorLow     = hyperdos_x86_16_read_byte_register(processor, 0u);
    uint8_t  oldAccumulatorLow  = accumulatorLow;
    uint16_t carryFlag          = 0u;
    uint16_t auxiliaryCarryFlag = 0u;

    if ((accumulatorLow & HYPERDOS_X86_16_NIBBLE_MASK) > 9u ||
        (processor->flags & HYPERDOS_X86_16_FLAG_AUXILIARY_CARRY) != 0u)
    {
        accumulatorLow     = (uint8_t)(accumulatorLow + 0x06u);
        auxiliaryCarryFlag = HYPERDOS_X86_16_FLAG_AUXILIARY_CARRY;
    }
    if (oldAccumulatorLow > 0x99u || (processor->flags & HYPERDOS_X86_16_FLAG_CARRY) != 0u)
    {
        accumulatorLow = (uint8_t)(accumulatorLow + 0x60u);
        carryFlag      = HYPERDOS_X86_16_FLAG_CARRY;
    }

    hyperdos_x86_16_write_byte_register(processor, 0u, accumulatorLow);
    hyperdos_x86_16_replace_decimal_adjust_flags(processor, accumulatorLow, carryFlag, auxiliaryCarryFlag);
}

static void hyperdos_x86_16_execute_decimal_adjust_after_subtract_instruction(hyperdos_x86_16_processor* processor)
{
    uint8_t  accumulatorLow     = hyperdos_x86_16_read_byte_register(processor, 0u);
    uint8_t  oldAccumulatorLow  = accumulatorLow;
    uint16_t carryFlag          = 0u;
    uint16_t auxiliaryCarryFlag = 0u;

    if ((accumulatorLow & HYPERDOS_X86_16_NIBBLE_MASK) > 9u ||
        (processor->flags & HYPERDOS_X86_16_FLAG_AUXILIARY_CARRY) != 0u)
    {
        accumulatorLow     = (uint8_t)(accumulatorLow - 0x06u);
        auxiliaryCarryFlag = HYPERDOS_X86_16_FLAG_AUXILIARY_CARRY;
    }
    if (oldAccumulatorLow > 0x99u || (processor->flags & HYPERDOS_X86_16_FLAG_CARRY) != 0u)
    {
        accumulatorLow = (uint8_t)(accumulatorLow - 0x60u);
        carryFlag      = HYPERDOS_X86_16_FLAG_CARRY;
    }

    hyperdos_x86_16_write_byte_register(processor, 0u, accumulatorLow);
    hyperdos_x86_16_replace_decimal_adjust_flags(processor, accumulatorLow, carryFlag, auxiliaryCarryFlag);
}

static void hyperdos_x86_16_execute_ascii_adjust_add_subtract_instruction(hyperdos_x86_16_processor* processor,
                                                                          int                        subtract)
{
    uint8_t  accumulatorLow      = hyperdos_x86_16_read_byte_register(processor, 0u);
    uint8_t  accumulatorHigh     = hyperdos_x86_16_read_byte_register(processor, 4u);
    uint16_t carryAuxiliaryFlags = 0u;

    if ((accumulatorLow & HYPERDOS_X86_16_NIBBLE_MASK) > 9u ||
        (processor->flags & HYPERDOS_X86_16_FLAG_AUXILIARY_CARRY) != 0u)
    {
        accumulatorLow      = subtract ? (uint8_t)(accumulatorLow - 0x06u) : (uint8_t)(accumulatorLow + 0x06u);
        accumulatorHigh     = subtract ? (uint8_t)(accumulatorHigh - 1u) : (uint8_t)(accumulatorHigh + 1u);
        carryAuxiliaryFlags = HYPERDOS_X86_16_FLAG_CARRY | HYPERDOS_X86_16_FLAG_AUXILIARY_CARRY;
    }

    accumulatorLow &= HYPERDOS_X86_16_NIBBLE_MASK;
    hyperdos_x86_16_write_byte_register(processor, 0u, accumulatorLow);
    hyperdos_x86_16_write_byte_register(processor, 4u, accumulatorHigh);
    processor->flags = (uint16_t)((processor->flags &
                                   ~(HYPERDOS_X86_16_FLAG_CARRY | HYPERDOS_X86_16_FLAG_AUXILIARY_CARRY)) |
                                  carryAuxiliaryFlags | HYPERDOS_X86_16_FLAG_RESERVED);
}

static hyperdos_x86_16_execution_result hyperdos_x86_16_execute_ascii_adjust_multiply_divide_instruction(
        hyperdos_x86_16_processor* processor,
        uint8_t                    operationCode)
{
    uint8_t radix           = hyperdos_x86_16_fetch_instruction_byte(processor);
    uint8_t accumulatorLow  = hyperdos_x86_16_read_byte_register(processor, 0u);
    uint8_t accumulatorHigh = hyperdos_x86_16_read_byte_register(processor, 4u);

    if (operationCode == HYPERDOS_X86_16_OPERATION_CODE_ASCII_ADJUST_AFTER_MULTIPLY)
    {
        if (radix == 0u)
        {
            return HYPERDOS_X86_16_EXECUTION_DIVIDE_ERROR;
        }
        accumulatorHigh = (uint8_t)(accumulatorLow / radix);
        accumulatorLow  = (uint8_t)(accumulatorLow % radix);
    }
    else
    {
        accumulatorLow  = (uint8_t)(accumulatorLow + accumulatorHigh * radix);
        accumulatorHigh = 0u;
    }

    hyperdos_x86_16_write_byte_register(processor, 0u, accumulatorLow);
    hyperdos_x86_16_write_byte_register(processor, 4u, accumulatorHigh);
    hyperdos_x86_16_replace_status_flags(processor,
                                         hyperdos_x86_16_sign_zero_parity_flags(accumulatorLow,
                                                                                HYPERDOS_X86_16_BYTE_MASK,
                                                                                HYPERDOS_X86_16_BYTE_SIGN_BIT));
    return HYPERDOS_X86_16_EXECUTION_OK;
}

static hyperdos_x86_16_execution_result hyperdos_x86_16_execute_escape_coprocessor_instruction(
        hyperdos_x86_16_processor*             processor,
        int                                    hasSegmentOverride,
        hyperdos_x86_16_segment_register_index segmentOverride)
{
    uint8_t                                 registerMemoryByte = hyperdos_x86_16_fetch_instruction_byte(processor);
    hyperdos_x86_16_decoded_operand         operand = hyperdos_x86_16_decode_register_memory_operand(processor,
                                                                                             registerMemoryByte,
                                                                                             hasSegmentOverride,
                                                                                             segmentOverride);
    hyperdos_x86_16_coprocessor_instruction instruction;

    instruction.operationCode      = processor->lastOperationCode;
    instruction.registerMemoryByte = registerMemoryByte;
    instruction.isRegister         = operand.isRegister;
    instruction.registerIndex      = operand.registerIndex;
    instruction.operationIndex     = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_16_MODRM_REGISTER_SHIFT) &
                                           HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
    instruction.segmentRegister    = operand.segmentRegister;
    instruction.offset             = operand.offset;

    if (processor->coprocessorEscapeHandler != NULL)
    {
        return processor->coprocessorEscapeHandler(processor, &instruction, processor->coprocessorContext);
    }
    return HYPERDOS_X86_16_EXECUTION_OK;
}

static hyperdos_x86_16_execution_result hyperdos_x86_16_execute_multiply_divide_instruction(
        hyperdos_x86_16_processor*             processor,
        uint8_t                                operationCode,
        int                                    hasSegmentOverride,
        hyperdos_x86_16_segment_register_index segmentOverride)
{
    uint8_t                         registerMemoryByte = hyperdos_x86_16_fetch_instruction_byte(processor);
    uint8_t                         operation = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_16_MODRM_REGISTER_SHIFT) &
                                  HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
    int                             isWord    = operationCode == HYPERDOS_X86_16_OPERATION_CODE_GROUP_THREE_WORD;
    hyperdos_x86_16_decoded_operand operand   = hyperdos_x86_16_decode_register_memory_operand(processor,
                                                                                             registerMemoryByte,
                                                                                             hasSegmentOverride,
                                                                                             segmentOverride);
    uint16_t                        value     = hyperdos_x86_16_read_operand_value(processor, &operand, isWord);

    switch (operation)
    {
    case HYPERDOS_X86_16_GROUP_THREE_TEST:
    {
        uint16_t immediateValue = isWord ? hyperdos_x86_16_fetch_instruction_word(processor)
                                         : hyperdos_x86_16_fetch_instruction_byte(processor);
        hyperdos_x86_16_test_values(processor,
                                    value,
                                    immediateValue,
                                    hyperdos_x86_16_operand_value_mask(isWord),
                                    hyperdos_x86_16_operand_sign_bit(isWord));
        return HYPERDOS_X86_16_EXECUTION_OK;
    }

    case HYPERDOS_X86_16_GROUP_THREE_NOT:
        hyperdos_x86_16_write_operand_value(processor, &operand, isWord, (uint16_t)(~value));
        return HYPERDOS_X86_16_EXECUTION_OK;

    case HYPERDOS_X86_16_GROUP_THREE_NEGATE:
    {
        uint16_t result = hyperdos_x86_16_execute_arithmetic_logic_operation(processor,
                                                                             HYPERDOS_X86_16_ARITHMETIC_LOGIC_SUB,
                                                                             0u,
                                                                             value,
                                                                             hyperdos_x86_16_operand_value_mask(isWord),
                                                                             hyperdos_x86_16_operand_sign_bit(isWord));
        if ((value & hyperdos_x86_16_operand_value_mask(isWord)) != 0u)
        {
            processor->flags |= HYPERDOS_X86_16_FLAG_CARRY;
        }
        else
        {
            processor->flags &= (uint16_t)~HYPERDOS_X86_16_FLAG_CARRY;
        }
        processor->flags |= HYPERDOS_X86_16_FLAG_RESERVED;
        hyperdos_x86_16_write_operand_value(processor, &operand, isWord, result);
        return HYPERDOS_X86_16_EXECUTION_OK;
    }

    case HYPERDOS_X86_16_GROUP_THREE_MULTIPLY_UNSIGNED:
        if (isWord)
        {
            uint32_t result = (uint32_t)processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] *
                              value;
            processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = (uint16_t)result;
            processor->generalRegisters
                    [HYPERDOS_X86_16_GENERAL_REGISTER_DATA] = (uint16_t)(result >> HYPERDOS_X86_16_WORD_BIT_COUNT);
            if ((result >> HYPERDOS_X86_16_WORD_BIT_COUNT) != 0u)
            {
                processor->flags |= HYPERDOS_X86_16_FLAG_CARRY | HYPERDOS_X86_16_FLAG_OVERFLOW;
            }
            else
            {
                processor->flags &= (uint16_t)~(HYPERDOS_X86_16_FLAG_CARRY | HYPERDOS_X86_16_FLAG_OVERFLOW);
            }
        }
        else
        {
            uint16_t result = (uint16_t)hyperdos_x86_16_read_byte_register(processor, 0u) * (uint8_t)value;
            processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = result;
            if ((result & HYPERDOS_X86_16_HIGH_BYTE_MASK) != 0u)
            {
                processor->flags |= HYPERDOS_X86_16_FLAG_CARRY | HYPERDOS_X86_16_FLAG_OVERFLOW;
            }
            else
            {
                processor->flags &= (uint16_t)~(HYPERDOS_X86_16_FLAG_CARRY | HYPERDOS_X86_16_FLAG_OVERFLOW);
            }
        }
        processor->flags |= HYPERDOS_X86_16_FLAG_RESERVED;
        return HYPERDOS_X86_16_EXECUTION_OK;

    case HYPERDOS_X86_16_GROUP_THREE_MULTIPLY_SIGNED:
        if (isWord)
        {
            int32_t result = (int32_t)(int16_t)
                                     processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] *
                             (int32_t)(int16_t)value;
            processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = (uint16_t)result;
            processor->generalRegisters
                    [HYPERDOS_X86_16_GENERAL_REGISTER_DATA] = (uint16_t)((uint32_t)result >>
                                                                         HYPERDOS_X86_16_WORD_BIT_COUNT);
            if (result < INT16_MIN || result > INT16_MAX)
            {
                processor->flags |= HYPERDOS_X86_16_FLAG_CARRY | HYPERDOS_X86_16_FLAG_OVERFLOW;
            }
            else
            {
                processor->flags &= (uint16_t)~(HYPERDOS_X86_16_FLAG_CARRY | HYPERDOS_X86_16_FLAG_OVERFLOW);
            }
        }
        else
        {
            int16_t result = (int16_t)((int16_t)(int8_t)hyperdos_x86_16_read_byte_register(processor, 0u) *
                                       (int16_t)(int8_t)value);
            processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = (uint16_t)result;
            if (result < INT8_MIN || result > INT8_MAX)
            {
                processor->flags |= HYPERDOS_X86_16_FLAG_CARRY | HYPERDOS_X86_16_FLAG_OVERFLOW;
            }
            else
            {
                processor->flags &= (uint16_t)~(HYPERDOS_X86_16_FLAG_CARRY | HYPERDOS_X86_16_FLAG_OVERFLOW);
            }
        }
        processor->flags |= HYPERDOS_X86_16_FLAG_RESERVED;
        return HYPERDOS_X86_16_EXECUTION_OK;

    case HYPERDOS_X86_16_GROUP_THREE_DIVIDE_UNSIGNED:
        if (value == 0u)
        {
            return HYPERDOS_X86_16_EXECUTION_DIVIDE_ERROR;
        }
        if (isWord)
        {
            uint32_t dividend = ((uint32_t)processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA]
                                 << HYPERDOS_X86_16_WORD_BIT_COUNT) |
                                processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR];
            uint32_t quotient  = dividend / value;
            uint32_t remainder = dividend % value;
            if (quotient > HYPERDOS_X86_16_WORD_MASK)
            {
                return HYPERDOS_X86_16_EXECUTION_DIVIDE_ERROR;
            }
            processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = (uint16_t)quotient;
            processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA]        = (uint16_t)remainder;
        }
        else
        {
            uint16_t dividend  = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR];
            uint16_t quotient  = (uint16_t)(dividend / (uint8_t)value);
            uint16_t remainder = (uint16_t)(dividend % (uint8_t)value);
            if (quotient > HYPERDOS_X86_16_BYTE_MASK)
            {
                return HYPERDOS_X86_16_EXECUTION_DIVIDE_ERROR;
            }
            hyperdos_x86_16_write_byte_register(processor, 0u, (uint8_t)quotient);
            hyperdos_x86_16_write_byte_register(processor, 4u, (uint8_t)remainder);
        }
        return HYPERDOS_X86_16_EXECUTION_OK;

    case HYPERDOS_X86_16_GROUP_THREE_DIVIDE_SIGNED:
        if (value == 0u)
        {
            return HYPERDOS_X86_16_EXECUTION_DIVIDE_ERROR;
        }
        if (isWord)
        {
            int32_t dividend = (int32_t)(int16_t)processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA] *
                                       (int32_t)HYPERDOS_X86_16_SEGMENT_BYTE_COUNT +
                               (int32_t)processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR];
            int32_t divisor   = (int32_t)(int16_t)value;
            int32_t quotient  = 0;
            int32_t remainder = 0;

            if (dividend == INT32_MIN && divisor == -1)
            {
                return HYPERDOS_X86_16_EXECUTION_DIVIDE_ERROR;
            }
            quotient  = dividend / divisor;
            remainder = dividend % divisor;
            if (quotient < INT16_MIN || quotient > INT16_MAX)
            {
                return HYPERDOS_X86_16_EXECUTION_DIVIDE_ERROR;
            }
            processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = (uint16_t)quotient;
            processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA]        = (uint16_t)remainder;
        }
        else
        {
            int32_t dividend = (int32_t)(int16_t)
                                       processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR];
            int32_t divisor   = (int32_t)(int8_t)value;
            int32_t quotient  = dividend / divisor;
            int32_t remainder = dividend % divisor;
            if (quotient < INT8_MIN || quotient > INT8_MAX)
            {
                return HYPERDOS_X86_16_EXECUTION_DIVIDE_ERROR;
            }
            hyperdos_x86_16_write_byte_register(processor, 0u, (uint8_t)quotient);
            hyperdos_x86_16_write_byte_register(processor, 4u, (uint8_t)remainder);
        }
        return HYPERDOS_X86_16_EXECUTION_OK;
    }

    return HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION;
}

static hyperdos_x86_16_execution_result hyperdos_x86_16_execute_group_five_instruction(
        hyperdos_x86_16_processor*             processor,
        int                                    hasSegmentOverride,
        hyperdos_x86_16_segment_register_index segmentOverride)
{
    uint8_t                         registerMemoryByte = hyperdos_x86_16_fetch_instruction_byte(processor);
    uint8_t                         operation = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_16_MODRM_REGISTER_SHIFT) &
                                  HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
    hyperdos_x86_16_decoded_operand operand   = hyperdos_x86_16_decode_register_memory_operand(processor,
                                                                                             registerMemoryByte,
                                                                                             hasSegmentOverride,
                                                                                             segmentOverride);
    uint16_t                        value     = hyperdos_x86_16_read_operand_value(processor, &operand, 1);

    switch (operation)
    {
    case HYPERDOS_X86_16_GROUP_FIVE_INCREMENT:
        hyperdos_x86_16_update_increment_decrement_flags(processor,
                                                         HYPERDOS_X86_16_ARITHMETIC_LOGIC_ADD,
                                                         value,
                                                         HYPERDOS_X86_16_WORD_MASK,
                                                         HYPERDOS_X86_16_WORD_SIGN_BIT);
        hyperdos_x86_16_write_operand_value(processor, &operand, 1, (uint16_t)(value + 1u));
        return HYPERDOS_X86_16_EXECUTION_OK;
    case HYPERDOS_X86_16_GROUP_FIVE_DECREMENT:
        hyperdos_x86_16_update_increment_decrement_flags(processor,
                                                         HYPERDOS_X86_16_ARITHMETIC_LOGIC_SUB,
                                                         value,
                                                         HYPERDOS_X86_16_WORD_MASK,
                                                         HYPERDOS_X86_16_WORD_SIGN_BIT);
        hyperdos_x86_16_write_operand_value(processor, &operand, 1, (uint16_t)(value - 1u));
        return HYPERDOS_X86_16_EXECUTION_OK;
    case HYPERDOS_X86_16_GROUP_FIVE_CALL_NEAR:
        hyperdos_x86_16_push_word(processor, processor->instructionPointer);
        processor->instructionPointer = value;
        return HYPERDOS_X86_16_EXECUTION_OK;
    case HYPERDOS_X86_16_GROUP_FIVE_CALL_FAR:
    {
        uint16_t segment = 0;
        if (operand.isRegister)
        {
            return HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION;
        }
        segment = hyperdos_x86_16_read_memory_word_fast(processor,
                                                        operand.segmentRegister,
                                                        (uint16_t)(operand.offset +
                                                                   HYPERDOS_X86_16_FAR_POINTER_SEGMENT_OFFSET));
        hyperdos_x86_16_push_word(processor, processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_CODE]);
        hyperdos_x86_16_push_word(processor, processor->instructionPointer);
        hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_CODE, segment);
        processor->instructionPointer = value;
        return HYPERDOS_X86_16_EXECUTION_OK;
    }
    case HYPERDOS_X86_16_GROUP_FIVE_JUMP_NEAR:
        processor->instructionPointer = value;
        return HYPERDOS_X86_16_EXECUTION_OK;
    case HYPERDOS_X86_16_GROUP_FIVE_JUMP_FAR:
    {
        uint16_t segment = 0;
        if (operand.isRegister)
        {
            return HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION;
        }
        segment = hyperdos_x86_16_read_memory_word_fast(processor,
                                                        operand.segmentRegister,
                                                        (uint16_t)(operand.offset +
                                                                   HYPERDOS_X86_16_FAR_POINTER_SEGMENT_OFFSET));
        hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_CODE, segment);
        processor->instructionPointer = value;
        return HYPERDOS_X86_16_EXECUTION_OK;
    }
    case HYPERDOS_X86_16_GROUP_FIVE_PUSH:
        hyperdos_x86_16_push_word(processor, value);
        return HYPERDOS_X86_16_EXECUTION_OK;
    }

    return HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION;
}

hyperdos_x86_16_execution_result hyperdos_x86_16_initialize_processor(hyperdos_x86_16_processor* processor,
                                                                      uint8_t*                   memory,
                                                                      size_t                     memorySize)
{
    if (processor == NULL || memory == NULL || memorySize < HYPERDOS_X86_16_MEMORY_SIZE)
    {
        return HYPERDOS_X86_16_EXECUTION_INVALID_ARGUMENT;
    }

    memset(processor, 0, sizeof(*processor));
    processor->memory     = memory;
    processor->memorySize = memorySize;
    hyperdos_x86_16_reset_processor(processor);
    return HYPERDOS_X86_16_EXECUTION_OK;
}

void hyperdos_x86_16_reset_processor(hyperdos_x86_16_processor* processor)
{
    uint8_t*                                   memory                   = processor->memory;
    size_t                                     memorySize               = processor->memorySize;
    struct hyperdos_bus*                       bus                      = processor->bus;
    hyperdos_x86_16_interrupt_handler          interruptHandler         = processor->interruptHandler;
    void*                                      userContext              = processor->userContext;
    hyperdos_x86_16_coprocessor_wait_handler   coprocessorWaitHandler   = processor->coprocessorWaitHandler;
    hyperdos_x86_16_coprocessor_escape_handler coprocessorEscapeHandler = processor->coprocessorEscapeHandler;
    void*                                      coprocessorContext       = processor->coprocessorContext;
    uint8_t                                    processorModel           = processor->processorModel;
    uint8_t divideErrorReturnsToFaultingInstruction = processor->divideErrorReturnsToFaultingInstruction;

    memset(processor, 0, sizeof(*processor));
    processor->memory                                  = memory;
    processor->memorySize                              = memorySize;
    processor->bus                                     = bus;
    processor->interruptHandler                        = interruptHandler;
    processor->userContext                             = userContext;
    processor->coprocessorWaitHandler                  = coprocessorWaitHandler;
    processor->coprocessorEscapeHandler                = coprocessorEscapeHandler;
    processor->coprocessorContext                      = coprocessorContext;
    processor->processorModel                          = processorModel;
    processor->divideErrorReturnsToFaultingInstruction = divideErrorReturnsToFaultingInstruction;
    processor->flags = hyperdos_x86_16_normalize_flags(HYPERDOS_X86_16_FLAG_INTERRUPT_ENABLE);
    processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_CODE] = HYPERDOS_X86_16_RESET_CODE_SEGMENT;
    processor->instructionPointer                                      = HYPERDOS_X86_16_RESET_INSTRUCTION_POINTER;
    hyperdos_x86_16_store_segment_base(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA);
    hyperdos_x86_16_store_segment_base(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_CODE);
    hyperdos_x86_16_store_segment_base(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_STACK);
    hyperdos_x86_16_store_segment_base(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_DATA);
}

void hyperdos_x86_16_set_processor_model(hyperdos_x86_16_processor* processor, hyperdos_x86_16_processor_model model)
{
    if (processor == NULL)
    {
        return;
    }
    processor->processorModel = (uint8_t)model;
}

void hyperdos_x86_16_set_interrupt_handler(hyperdos_x86_16_processor*        processor,
                                           hyperdos_x86_16_interrupt_handler interruptHandler,
                                           void*                             userContext)
{
    processor->interruptHandler = interruptHandler;
    processor->userContext      = userContext;
}

void hyperdos_x86_16_attach_coprocessor(hyperdos_x86_16_processor*                 processor,
                                        hyperdos_x86_16_coprocessor_wait_handler   waitHandler,
                                        hyperdos_x86_16_coprocessor_escape_handler escapeHandler,
                                        void*                                      userContext)
{
    if (processor == NULL)
    {
        return;
    }
    processor->coprocessorWaitHandler   = waitHandler;
    processor->coprocessorEscapeHandler = escapeHandler;
    processor->coprocessorContext       = userContext;
}

void hyperdos_x86_16_attach_bus(hyperdos_x86_16_processor* processor, struct hyperdos_bus* bus)
{
    if (processor == NULL)
    {
        return;
    }
    processor->bus = bus;
}

hyperdos_x86_16_execution_result hyperdos_x86_16_load_dos_program(hyperdos_x86_16_processor* processor,
                                                                  const uint8_t*             programBytes,
                                                                  size_t                     programSize,
                                                                  uint16_t                   programSegment,
                                                                  const char*                commandTail,
                                                                  size_t                     commandTailLength)
{
    uint32_t programBase          = 0;
    uint32_t requestedProgramBase = ((uint32_t)programSegment << HYPERDOS_X86_16_SEGMENT_SHIFT) &
                                    HYPERDOS_X86_16_ADDRESS_MASK;
    size_t commandTailByteCount = commandTailLength;

    if (processor == NULL || programBytes == NULL)
    {
        return HYPERDOS_X86_16_EXECUTION_INVALID_ARGUMENT;
    }
    if (programSize > HYPERDOS_X86_16_DOS_PROGRAM_MAXIMUM_SIZE)
    {
        return HYPERDOS_X86_16_EXECUTION_PROGRAM_TOO_LARGE;
    }
    if (requestedProgramBase > HYPERDOS_X86_16_MEMORY_SIZE - HYPERDOS_X86_16_SEGMENT_BYTE_COUNT)
    {
        return HYPERDOS_X86_16_EXECUTION_INVALID_ARGUMENT;
    }
    if (commandTailByteCount > HYPERDOS_X86_16_DOS_COMMAND_TAIL_MAXIMUM_LENGTH)
    {
        commandTailByteCount = HYPERDOS_X86_16_DOS_COMMAND_TAIL_MAXIMUM_LENGTH;
    }

    hyperdos_x86_16_reset_processor(processor);
    hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA, programSegment);
    hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_CODE, programSegment);
    hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_STACK, programSegment);
    hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_DATA, programSegment);
    processor->instructionPointer                                               = HYPERDOS_X86_16_DOS_PROGRAM_OFFSET;
    processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER] = HYPERDOS_X86_16_DOS_STACK_POINTER;

    programBase = processor->segmentBases[HYPERDOS_X86_16_SEGMENT_REGISTER_CODE];
    memset(processor->memory + programBase, 0, HYPERDOS_X86_16_SEGMENT_BYTE_COUNT);
    hyperdos_x86_16_write_physical_byte(processor,
                                        programBase + HYPERDOS_X86_16_DOS_TERMINATE_STUB_OFFSET,
                                        HYPERDOS_X86_16_OPERATION_CODE_INTERRUPT_IMMEDIATE);
    hyperdos_x86_16_write_physical_byte(processor,
                                        programBase + HYPERDOS_X86_16_DOS_TERMINATE_STUB_OFFSET +
                                                HYPERDOS_X86_16_BYTE_SIZE,
                                        HYPERDOS_X86_16_DOS_INTERRUPT_TERMINATE_PROGRAM);
    hyperdos_x86_16_write_physical_byte(processor,
                                        programBase + HYPERDOS_X86_16_DOS_COMMAND_TAIL_LENGTH_OFFSET,
                                        (uint8_t)commandTailByteCount);
    if (commandTail != NULL && commandTailByteCount > 0u)
    {
        memcpy(processor->memory + programBase + HYPERDOS_X86_16_DOS_COMMAND_TAIL_BYTES_OFFSET,
               commandTail,
               commandTailByteCount);
    }
    hyperdos_x86_16_write_physical_byte(processor,
                                        (uint32_t)(programBase + HYPERDOS_X86_16_DOS_COMMAND_TAIL_BYTES_OFFSET +
                                                   commandTailByteCount),
                                        HYPERDOS_X86_16_ASCII_CARRIAGE_RETURN);
    memcpy(processor->memory + programBase + HYPERDOS_X86_16_DOS_PROGRAM_OFFSET, programBytes, programSize);
    return HYPERDOS_X86_16_EXECUTION_OK;
}

hyperdos_x86_16_execution_result hyperdos_x86_16_execute(hyperdos_x86_16_processor* processor,
                                                         uint64_t                   instructionLimit)
{
    if (processor == NULL || processor->memory == NULL || processor->memorySize < HYPERDOS_X86_16_MEMORY_SIZE)
    {
        return HYPERDOS_X86_16_EXECUTION_INVALID_ARGUMENT;
    }

    while (!processor->halted)
    {
        uint16_t instructionStartOffset            = processor->instructionPointer;
        uint16_t instructionStartSegment           = processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_CODE];
        uint8_t  operationCode                     = 0;
        int      hasSegmentOverride                = 0;
        int      maskableInterruptInhibitWasActive = processor->maskableInterruptInhibitCount != 0u;
        int      trapWasEnabled                    = (processor->flags & HYPERDOS_X86_16_FLAG_TRAP) != 0u;
        int      startMaskableInterruptInhibit     = 0;
        int      suppressTrapAfterInstruction      = 0;
        hyperdos_x86_16_segment_register_index segmentOverride = HYPERDOS_X86_16_SEGMENT_REGISTER_DATA;
        hyperdos_x86_16_repeat_mode            repeatMode      = HYPERDOS_X86_16_REPEAT_NONE;
        hyperdos_x86_16_execution_result       result          = HYPERDOS_X86_16_EXECUTION_OK;

        if (instructionLimit != 0u && processor->executedInstructionCount >= instructionLimit)
        {
            return HYPERDOS_X86_16_EXECUTION_STEP_LIMIT_REACHED;
        }

        for (;;)
        {
            operationCode = hyperdos_x86_16_fetch_instruction_byte(processor);
            if (operationCode == HYPERDOS_X86_16_OPERATION_CODE_PREFIX_EXTRA_SEGMENT)
            {
                hasSegmentOverride = 1;
                segmentOverride    = HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA;
                continue;
            }
            if (operationCode == HYPERDOS_X86_16_OPERATION_CODE_PREFIX_CODE_SEGMENT)
            {
                hasSegmentOverride = 1;
                segmentOverride    = HYPERDOS_X86_16_SEGMENT_REGISTER_CODE;
                continue;
            }
            if (operationCode == HYPERDOS_X86_16_OPERATION_CODE_PREFIX_STACK_SEGMENT)
            {
                hasSegmentOverride = 1;
                segmentOverride    = HYPERDOS_X86_16_SEGMENT_REGISTER_STACK;
                continue;
            }
            if (operationCode == HYPERDOS_X86_16_OPERATION_CODE_PREFIX_DATA_SEGMENT)
            {
                hasSegmentOverride = 1;
                segmentOverride    = HYPERDOS_X86_16_SEGMENT_REGISTER_DATA;
                continue;
            }
            if (operationCode == HYPERDOS_X86_16_OPERATION_CODE_PREFIX_REPEAT_WHILE_NOT_EQUAL)
            {
                repeatMode = HYPERDOS_X86_16_REPEAT_WHILE_NOT_EQUAL;
                continue;
            }
            if (operationCode == HYPERDOS_X86_16_OPERATION_CODE_PREFIX_REPEAT)
            {
                repeatMode = HYPERDOS_X86_16_REPEAT_WHILE_EQUAL;
                continue;
            }
            if (operationCode == HYPERDOS_X86_16_OPERATION_CODE_PREFIX_LOCK)
            {
                continue;
            }
            break;
        }

        processor->lastOperationCode      = operationCode;
        processor->lastInstructionSegment = instructionStartSegment;
        processor->lastInstructionOffset  = instructionStartOffset;

        if (hyperdos_x86_16_operation_code_is_in_range(operationCode,
                                                       HYPERDOS_X86_16_OPERATION_CODE_ARITHMETIC_LOGIC_FIRST,
                                                       HYPERDOS_X86_16_OPERATION_CODE_ARITHMETIC_LOGIC_LAST) &&
            (operationCode & HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK) <=
                    HYPERDOS_X86_16_ARITHMETIC_FORM_ACCUMULATOR_WORD)
        {
            result = hyperdos_x86_16_execute_arithmetic_logic_instruction(processor,
                                                                          operationCode,
                                                                          hasSegmentOverride,
                                                                          segmentOverride);
        }
        else if (hyperdos_x86_16_operation_code_is_in_range(operationCode,
                                                            HYPERDOS_X86_16_OPERATION_CODE_INCREMENT_REGISTER_FIRST,
                                                            HYPERDOS_X86_16_OPERATION_CODE_INCREMENT_REGISTER_LAST))
        {
            uint8_t  registerIndex = (uint8_t)(operationCode & HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
            uint16_t value         = processor->generalRegisters[registerIndex];
            hyperdos_x86_16_update_increment_decrement_flags(processor,
                                                             HYPERDOS_X86_16_ARITHMETIC_LOGIC_ADD,
                                                             value,
                                                             HYPERDOS_X86_16_WORD_MASK,
                                                             HYPERDOS_X86_16_WORD_SIGN_BIT);
            processor->generalRegisters[registerIndex] = (uint16_t)(value + 1u);
        }
        else if (hyperdos_x86_16_operation_code_is_in_range(operationCode,
                                                            HYPERDOS_X86_16_OPERATION_CODE_DECREMENT_REGISTER_FIRST,
                                                            HYPERDOS_X86_16_OPERATION_CODE_DECREMENT_REGISTER_LAST))
        {
            uint8_t  registerIndex = (uint8_t)(operationCode & HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
            uint16_t value         = processor->generalRegisters[registerIndex];
            hyperdos_x86_16_update_increment_decrement_flags(processor,
                                                             HYPERDOS_X86_16_ARITHMETIC_LOGIC_SUB,
                                                             value,
                                                             HYPERDOS_X86_16_WORD_MASK,
                                                             HYPERDOS_X86_16_WORD_SIGN_BIT);
            processor->generalRegisters[registerIndex] = (uint16_t)(value - 1u);
        }
        else if (hyperdos_x86_16_operation_code_is_in_range(operationCode,
                                                            HYPERDOS_X86_16_OPERATION_CODE_PUSH_REGISTER_FIRST,
                                                            HYPERDOS_X86_16_OPERATION_CODE_PUSH_REGISTER_LAST))
        {
            uint8_t  registerIndex = (uint8_t)(operationCode & HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
            uint16_t value         = processor->generalRegisters[registerIndex];
            if (registerIndex == HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER)
            {
                value = (uint16_t)(value - HYPERDOS_X86_16_WORD_SIZE);
            }
            hyperdos_x86_16_push_word(processor, value);
        }
        else if (hyperdos_x86_16_operation_code_is_in_range(operationCode,
                                                            HYPERDOS_X86_16_OPERATION_CODE_POP_REGISTER_FIRST,
                                                            HYPERDOS_X86_16_OPERATION_CODE_POP_REGISTER_LAST))
        {
            uint8_t registerIndex = (uint8_t)(operationCode & HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
            processor->generalRegisters[registerIndex] = hyperdos_x86_16_pop_word(processor);
        }
        else if (hyperdos_x86_16_operation_code_is_in_range(operationCode,
                                                            HYPERDOS_X86_16_OPERATION_CODE_JUMP_CONDITION_FIRST,
                                                            HYPERDOS_X86_16_OPERATION_CODE_JUMP_CONDITION_LAST))
        {
            int8_t displacement = (int8_t)hyperdos_x86_16_fetch_instruction_byte(processor);
            if (hyperdos_x86_16_evaluate_condition(processor, (uint8_t)(operationCode & HYPERDOS_X86_16_NIBBLE_MASK)))
            {
                hyperdos_x86_16_jump_relative(processor, displacement);
            }
        }
        else if (hyperdos_x86_16_operation_code_is_in_range(
                         operationCode,
                         HYPERDOS_X86_16_OPERATION_CODE_GROUP_ONE_BYTE_IMMEDIATE,
                         HYPERDOS_X86_16_OPERATION_CODE_GROUP_ONE_SIGN_EXTENDED_IMMEDIATE))
        {
            result = hyperdos_x86_16_execute_group_one_instruction(processor,
                                                                   operationCode,
                                                                   hasSegmentOverride,
                                                                   segmentOverride);
        }
        else if (hyperdos_x86_16_operation_code_is_in_range(
                         operationCode,
                         HYPERDOS_X86_16_OPERATION_CODE_MOVE_REGISTER_MEMORY_BYTE_FIRST,
                         HYPERDOS_X86_16_OPERATION_CODE_MOVE_REGISTER_MEMORY_WORD_LAST))
        {
            result = hyperdos_x86_16_execute_move_instruction(processor,
                                                              operationCode,
                                                              hasSegmentOverride,
                                                              segmentOverride);
        }
        else if (hyperdos_x86_16_operation_code_is_in_range(operationCode,
                                                            HYPERDOS_X86_16_OPERATION_CODE_EXCHANGE_ACCUMULATOR_FIRST,
                                                            HYPERDOS_X86_16_OPERATION_CODE_EXCHANGE_ACCUMULATOR_LAST))
        {
            uint8_t  registerIndex = (uint8_t)(operationCode & HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
            uint16_t value         = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR];
            processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] = processor->generalRegisters
                                                                                                [registerIndex];
            processor->generalRegisters[registerIndex] = value;
        }
        else if (hyperdos_x86_16_operation_code_is_string(operationCode))
        {
            result = hyperdos_x86_16_execute_string_instruction(processor,
                                                                operationCode,
                                                                repeatMode,
                                                                hasSegmentOverride,
                                                                segmentOverride);
        }
        else if (hyperdos_x86_16_operation_code_is_in_range(operationCode,
                                                            HYPERDOS_X86_16_OPERATION_CODE_ESCAPE_COPROCESSOR_FIRST,
                                                            HYPERDOS_X86_16_OPERATION_CODE_ESCAPE_COPROCESSOR_LAST))
        {
            result = hyperdos_x86_16_execute_escape_coprocessor_instruction(processor,
                                                                            hasSegmentOverride,
                                                                            segmentOverride);
        }
        else if (hyperdos_x86_16_operation_code_is_in_range(
                         operationCode,
                         HYPERDOS_X86_16_OPERATION_CODE_MOVE_IMMEDIATE_TO_BYTE_REGISTER_FIRST,
                         HYPERDOS_X86_16_OPERATION_CODE_MOVE_IMMEDIATE_TO_BYTE_REGISTER_LAST))
        {
            hyperdos_x86_16_write_byte_register(processor,
                                                (uint8_t)(operationCode & HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK),
                                                hyperdos_x86_16_fetch_instruction_byte(processor));
        }
        else if (hyperdos_x86_16_operation_code_is_in_range(
                         operationCode,
                         HYPERDOS_X86_16_OPERATION_CODE_MOVE_IMMEDIATE_TO_WORD_REGISTER_FIRST,
                         HYPERDOS_X86_16_OPERATION_CODE_MOVE_IMMEDIATE_TO_WORD_REGISTER_LAST))
        {
            processor->generalRegisters
                    [operationCode &
                     HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK] = hyperdos_x86_16_fetch_instruction_word(processor);
        }
        else if (hyperdos_x86_16_operation_code_is_in_range(operationCode,
                                                            HYPERDOS_X86_16_OPERATION_CODE_SHIFT_ROTATE_FIRST,
                                                            HYPERDOS_X86_16_OPERATION_CODE_SHIFT_ROTATE_LAST))
        {
            result = hyperdos_x86_16_execute_shift_rotate_instruction(processor,
                                                                      operationCode,
                                                                      hasSegmentOverride,
                                                                      segmentOverride);
        }
        else if (hyperdos_x86_16_operation_code_is_in_range(operationCode,
                                                            HYPERDOS_X86_16_OPERATION_CODE_LOOP_NOT_ZERO_AND_NOT_EQUAL,
                                                            HYPERDOS_X86_16_OPERATION_CODE_LOOP_NOT_ZERO))
        {
            int8_t   displacement = (int8_t)hyperdos_x86_16_fetch_instruction_byte(processor);
            uint16_t counterValue = (uint16_t)(processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER] -
                                               1u);
            processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER] = counterValue;
            if (counterValue != 0u)
            {
                int zero = (processor->flags & HYPERDOS_X86_16_FLAG_ZERO) != 0u;
                if (operationCode == HYPERDOS_X86_16_OPERATION_CODE_LOOP_NOT_ZERO ||
                    (operationCode == HYPERDOS_X86_16_OPERATION_CODE_LOOP_NOT_ZERO_AND_EQUAL && zero) ||
                    (operationCode == HYPERDOS_X86_16_OPERATION_CODE_LOOP_NOT_ZERO_AND_NOT_EQUAL && !zero))
                {
                    hyperdos_x86_16_jump_relative(processor, displacement);
                }
            }
        }
        else
        {
            switch (operationCode)
            {
            case HYPERDOS_X86_16_OPERATION_CODE_PUSH_EXTRA_SEGMENT:
                hyperdos_x86_16_push_word(processor,
                                          processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA]);
                break;
            case HYPERDOS_X86_16_OPERATION_CODE_POP_EXTRA_SEGMENT:
                hyperdos_x86_16_set_segment_register(processor,
                                                     HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA,
                                                     hyperdos_x86_16_pop_word(processor));
                break;
            case HYPERDOS_X86_16_OPERATION_CODE_PUSH_CODE_SEGMENT:
                hyperdos_x86_16_push_word(processor,
                                          processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_CODE]);
                break;
            case HYPERDOS_X86_16_OPERATION_CODE_PUSH_STACK_SEGMENT:
                hyperdos_x86_16_push_word(processor,
                                          processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_STACK]);
                break;
            case HYPERDOS_X86_16_OPERATION_CODE_POP_STACK_SEGMENT:
                hyperdos_x86_16_set_segment_register(processor,
                                                     HYPERDOS_X86_16_SEGMENT_REGISTER_STACK,
                                                     hyperdos_x86_16_pop_word(processor));
                startMaskableInterruptInhibit = 1;
                break;
            case HYPERDOS_X86_16_OPERATION_CODE_PUSH_DATA_SEGMENT:
                hyperdos_x86_16_push_word(processor,
                                          processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_DATA]);
                break;
            case HYPERDOS_X86_16_OPERATION_CODE_POP_DATA_SEGMENT:
                hyperdos_x86_16_set_segment_register(processor,
                                                     HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                                     hyperdos_x86_16_pop_word(processor));
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_DECIMAL_ADJUST_AFTER_ADD:
                hyperdos_x86_16_execute_decimal_adjust_after_add_instruction(processor);
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_DECIMAL_ADJUST_AFTER_SUBTRACT:
                hyperdos_x86_16_execute_decimal_adjust_after_subtract_instruction(processor);
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_ASCII_ADJUST_AFTER_ADD:
                hyperdos_x86_16_execute_ascii_adjust_add_subtract_instruction(processor, 0);
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_ASCII_ADJUST_AFTER_SUBTRACT:
                hyperdos_x86_16_execute_ascii_adjust_add_subtract_instruction(processor, 1);
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_TEST_REGISTER_MEMORY_BYTE:
            case HYPERDOS_X86_16_OPERATION_CODE_TEST_REGISTER_MEMORY_WORD:
            {
                uint8_t registerMemoryByte = hyperdos_x86_16_fetch_instruction_byte(processor);
                uint8_t registerIndex      = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_16_MODRM_REGISTER_SHIFT) &
                                                  HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
                int     isWord             = operationCode == HYPERDOS_X86_16_OPERATION_CODE_TEST_REGISTER_MEMORY_WORD;
                hyperdos_x86_16_decoded_operand
                         operand    = hyperdos_x86_16_decode_register_memory_operand(processor,
                                                                                 registerMemoryByte,
                                                                                 hasSegmentOverride,
                                                                                 segmentOverride);
                uint16_t leftValue  = hyperdos_x86_16_read_operand_value(processor, &operand, isWord);
                uint16_t rightValue = isWord ? processor->generalRegisters[registerIndex]
                                             : hyperdos_x86_16_read_byte_register(processor, registerIndex);
                hyperdos_x86_16_test_values(processor,
                                            leftValue,
                                            rightValue,
                                            hyperdos_x86_16_operand_value_mask(isWord),
                                            hyperdos_x86_16_operand_sign_bit(isWord));
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_EXCHANGE_REGISTER_MEMORY_BYTE:
            case HYPERDOS_X86_16_OPERATION_CODE_EXCHANGE_REGISTER_MEMORY_WORD:
            {
                uint8_t registerMemoryByte = hyperdos_x86_16_fetch_instruction_byte(processor);
                uint8_t registerIndex      = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_16_MODRM_REGISTER_SHIFT) &
                                                  HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
                int     isWord = operationCode == HYPERDOS_X86_16_OPERATION_CODE_EXCHANGE_REGISTER_MEMORY_WORD;
                hyperdos_x86_16_decoded_operand
                         operand       = hyperdos_x86_16_decode_register_memory_operand(processor,
                                                                                 registerMemoryByte,
                                                                                 hasSegmentOverride,
                                                                                 segmentOverride);
                uint16_t registerValue = isWord ? processor->generalRegisters[registerIndex]
                                                : hyperdos_x86_16_read_byte_register(processor, registerIndex);
                uint16_t operandValue  = hyperdos_x86_16_read_operand_value(processor, &operand, isWord);
                if (isWord)
                {
                    processor->generalRegisters[registerIndex] = operandValue;
                }
                else
                {
                    hyperdos_x86_16_write_byte_register(processor, registerIndex, (uint8_t)operandValue);
                }
                hyperdos_x86_16_write_operand_value(processor, &operand, isWord, registerValue);
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_MOVE_SEGMENT_TO_REGISTER_MEMORY:
            {
                uint8_t registerMemoryByte = hyperdos_x86_16_fetch_instruction_byte(processor);
                uint8_t segmentRegister    = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_16_MODRM_REGISTER_SHIFT) &
                                                    HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
                hyperdos_x86_16_decoded_operand
                        operand = hyperdos_x86_16_decode_register_memory_operand(processor,
                                                                                 registerMemoryByte,
                                                                                 hasSegmentOverride,
                                                                                 segmentOverride);
                if (segmentRegister > HYPERDOS_X86_16_SEGMENT_REGISTER_DATA)
                {
                    result = HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION;
                    break;
                }
                hyperdos_x86_16_write_operand_value(processor,
                                                    &operand,
                                                    1,
                                                    processor->segmentRegisters[segmentRegister]);
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_LOAD_EFFECTIVE_ADDRESS:
                result = hyperdos_x86_16_execute_lea_instruction(processor, hasSegmentOverride, segmentOverride);
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_MOVE_REGISTER_MEMORY_TO_SEGMENT:
            {
                uint8_t registerMemoryByte = hyperdos_x86_16_fetch_instruction_byte(processor);
                uint8_t segmentRegister    = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_16_MODRM_REGISTER_SHIFT) &
                                                    HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
                hyperdos_x86_16_decoded_operand
                        operand = hyperdos_x86_16_decode_register_memory_operand(processor,
                                                                                 registerMemoryByte,
                                                                                 hasSegmentOverride,
                                                                                 segmentOverride);
                if (segmentRegister > HYPERDOS_X86_16_SEGMENT_REGISTER_DATA)
                {
                    result = HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION;
                    break;
                }
                hyperdos_x86_16_set_segment_register(processor,
                                                     (hyperdos_x86_16_segment_register_index)segmentRegister,
                                                     hyperdos_x86_16_read_operand_value(processor, &operand, 1));
                if (segmentRegister == HYPERDOS_X86_16_SEGMENT_REGISTER_STACK)
                {
                    startMaskableInterruptInhibit = 1;
                }
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_POP_REGISTER_MEMORY:
            {
                uint8_t registerMemoryByte = hyperdos_x86_16_fetch_instruction_byte(processor);
                uint8_t operation          = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_16_MODRM_REGISTER_SHIFT) &
                                              HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
                hyperdos_x86_16_decoded_operand
                        operand = hyperdos_x86_16_decode_register_memory_operand(processor,
                                                                                 registerMemoryByte,
                                                                                 hasSegmentOverride,
                                                                                 segmentOverride);
                if (operation != HYPERDOS_X86_16_GROUP_FIVE_INCREMENT)
                {
                    result = HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION;
                    break;
                }
                hyperdos_x86_16_write_operand_value(processor, &operand, 1, hyperdos_x86_16_pop_word(processor));
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_CONVERT_BYTE_TO_WORD:
            {
                uint8_t accumulatorLow = hyperdos_x86_16_read_byte_register(processor, 0u);
                hyperdos_x86_16_write_byte_register(processor,
                                                    HYPERDOS_X86_16_BYTE_REGISTER_HIGH_PART_BIT,
                                                    (accumulatorLow & HYPERDOS_X86_16_BYTE_SIGN_BIT) != 0u
                                                            ? HYPERDOS_X86_16_BYTE_MASK
                                                            : 0x00u);
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_CONVERT_WORD_TO_DOUBLE_WORD:
                processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA] =
                        (processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] &
                         HYPERDOS_X86_16_WORD_SIGN_BIT) != 0u
                                ? HYPERDOS_X86_16_WORD_MASK
                                : 0x0000u;
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_CALL_FAR_IMMEDIATE:
            {
                uint16_t offset  = hyperdos_x86_16_fetch_instruction_word(processor);
                uint16_t segment = hyperdos_x86_16_fetch_instruction_word(processor);
                hyperdos_x86_16_push_word(processor,
                                          processor->segmentRegisters[HYPERDOS_X86_16_SEGMENT_REGISTER_CODE]);
                hyperdos_x86_16_push_word(processor, processor->instructionPointer);
                hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_CODE, segment);
                processor->instructionPointer = offset;
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_PUSH_FLAGS:
                processor->flags = hyperdos_x86_16_normalize_flags(processor->flags);
                hyperdos_x86_16_push_word(processor, processor->flags);
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_POP_FLAGS:
                processor->flags             = hyperdos_x86_16_normalize_flags(hyperdos_x86_16_pop_word(processor));
                suppressTrapAfterInstruction = !trapWasEnabled;
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_STORE_STATUS_TO_FLAGS:
            {
                uint16_t accumulator = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR];
                processor->flags     = hyperdos_x86_16_normalize_flags(
                        (uint16_t)((processor->flags & HYPERDOS_X86_16_FLAGS_HIGH_BYTE_MASK) |
                                   ((accumulator >> HYPERDOS_X86_16_BYTE_BIT_COUNT) &
                                    HYPERDOS_X86_16_FLAGS_LOW_BYTE_TRANSFER_MASK)));
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_LOAD_STATUS_FROM_FLAGS:
                hyperdos_x86_16_write_byte_register(processor,
                                                    HYPERDOS_X86_16_BYTE_REGISTER_HIGH_PART_BIT,
                                                    (uint8_t)(processor->flags &
                                                              HYPERDOS_X86_16_FLAGS_LOW_BYTE_TRANSFER_MASK));
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_WAIT:
                if (processor->coprocessorWaitHandler != NULL)
                {
                    result = processor->coprocessorWaitHandler(processor, processor->coprocessorContext);
                }
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_MOVE_MEMORY_TO_ACCUMULATOR_BYTE:
            {
                uint16_t offset = hyperdos_x86_16_fetch_instruction_word(processor);
                hyperdos_x86_16_write_byte_register(processor,
                                                    0u,
                                                    hyperdos_x86_16_read_memory_byte_fast(
                                                            processor,
                                                            hasSegmentOverride ? segmentOverride
                                                                               : HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                                            offset));
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_MOVE_MEMORY_TO_ACCUMULATOR_WORD:
            {
                uint16_t offset = hyperdos_x86_16_fetch_instruction_word(processor);
                processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] =
                        hyperdos_x86_16_read_memory_word_fast(processor,
                                                              hasSegmentOverride
                                                                      ? segmentOverride
                                                                      : HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                                              offset);
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_MOVE_ACCUMULATOR_TO_MEMORY_BYTE:
            {
                uint16_t offset = hyperdos_x86_16_fetch_instruction_word(processor);
                hyperdos_x86_16_write_memory_byte_fast(processor,
                                                       hasSegmentOverride ? segmentOverride
                                                                          : HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                                       offset,
                                                       hyperdos_x86_16_read_byte_register(processor, 0u));
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_MOVE_ACCUMULATOR_TO_MEMORY_WORD:
            {
                uint16_t offset = hyperdos_x86_16_fetch_instruction_word(processor);
                hyperdos_x86_16_write_memory_word_fast(processor,
                                                       hasSegmentOverride ? segmentOverride
                                                                          : HYPERDOS_X86_16_SEGMENT_REGISTER_DATA,
                                                       offset,
                                                       processor->generalRegisters
                                                               [HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR]);
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_TEST_ACCUMULATOR_BYTE:
                hyperdos_x86_16_test_values(processor,
                                            hyperdos_x86_16_read_byte_register(processor, 0u),
                                            hyperdos_x86_16_fetch_instruction_byte(processor),
                                            HYPERDOS_X86_16_BYTE_MASK,
                                            HYPERDOS_X86_16_BYTE_SIGN_BIT);
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_TEST_ACCUMULATOR_WORD:
                hyperdos_x86_16_test_values(processor,
                                            processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR],
                                            hyperdos_x86_16_fetch_instruction_word(processor),
                                            HYPERDOS_X86_16_WORD_MASK,
                                            HYPERDOS_X86_16_WORD_SIGN_BIT);
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_SHIFT_ROTATE_IMMEDIATE_BYTE:
            case HYPERDOS_X86_16_OPERATION_CODE_SHIFT_ROTATE_IMMEDIATE_WORD:
                if (!hyperdos_x86_16_supports_80186_instructions(processor))
                {
                    result = HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION;
                    break;
                }
                result = hyperdos_x86_16_execute_shift_rotate_instruction(processor,
                                                                          operationCode,
                                                                          hasSegmentOverride,
                                                                          segmentOverride);
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_RETURN_NEAR_IMMEDIATE:
            {
                uint16_t stackByteCount       = hyperdos_x86_16_fetch_instruction_word(processor);
                processor->instructionPointer = hyperdos_x86_16_pop_word(processor);
                processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER] =
                        (uint16_t)(processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER] +
                                   stackByteCount);
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_RETURN_NEAR:
                processor->instructionPointer = hyperdos_x86_16_pop_word(processor);
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_LOAD_EXTRA_SEGMENT_POINTER:
            case HYPERDOS_X86_16_OPERATION_CODE_LOAD_DATA_SEGMENT_POINTER:
            {
                uint8_t registerMemoryByte = hyperdos_x86_16_fetch_instruction_byte(processor);
                uint8_t registerIndex      = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_16_MODRM_REGISTER_SHIFT) &
                                                  HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
                hyperdos_x86_16_decoded_operand
                        operand = hyperdos_x86_16_decode_register_memory_operand(processor,
                                                                                 registerMemoryByte,
                                                                                 hasSegmentOverride,
                                                                                 segmentOverride);
                hyperdos_x86_16_segment_register_index
                        targetSegment = operationCode == HYPERDOS_X86_16_OPERATION_CODE_LOAD_EXTRA_SEGMENT_POINTER
                                                ? HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA
                                                : HYPERDOS_X86_16_SEGMENT_REGISTER_DATA;
                if (operand.isRegister)
                {
                    result = HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION;
                    break;
                }
                processor->generalRegisters
                        [registerIndex] = hyperdos_x86_16_read_memory_word_fast(processor,
                                                                                operand.segmentRegister,
                                                                                operand.offset);
                hyperdos_x86_16_set_segment_register(
                        processor,
                        targetSegment,
                        hyperdos_x86_16_read_memory_word_fast(processor,
                                                              operand.segmentRegister,
                                                              (uint16_t)(operand.offset +
                                                                         HYPERDOS_X86_16_FAR_POINTER_SEGMENT_OFFSET)));
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_MOVE_IMMEDIATE_TO_REGISTER_MEMORY_BYTE:
            case HYPERDOS_X86_16_OPERATION_CODE_MOVE_IMMEDIATE_TO_REGISTER_MEMORY_WORD:
            {
                uint8_t registerMemoryByte = hyperdos_x86_16_fetch_instruction_byte(processor);
                uint8_t operation          = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_16_MODRM_REGISTER_SHIFT) &
                                              HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
                int     isWord = operationCode == HYPERDOS_X86_16_OPERATION_CODE_MOVE_IMMEDIATE_TO_REGISTER_MEMORY_WORD;
                hyperdos_x86_16_decoded_operand
                        operand = hyperdos_x86_16_decode_register_memory_operand(processor,
                                                                                 registerMemoryByte,
                                                                                 hasSegmentOverride,
                                                                                 segmentOverride);
                if (operation != HYPERDOS_X86_16_GROUP_THREE_TEST)
                {
                    result = HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION;
                    break;
                }
                hyperdos_x86_16_write_operand_value(processor,
                                                    &operand,
                                                    isWord,
                                                    isWord ? hyperdos_x86_16_fetch_instruction_word(processor)
                                                           : hyperdos_x86_16_fetch_instruction_byte(processor));
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_RETURN_FAR_IMMEDIATE:
            {
                uint16_t stackByteCount       = hyperdos_x86_16_fetch_instruction_word(processor);
                processor->instructionPointer = hyperdos_x86_16_pop_word(processor);
                hyperdos_x86_16_set_segment_register(processor,
                                                     HYPERDOS_X86_16_SEGMENT_REGISTER_CODE,
                                                     hyperdos_x86_16_pop_word(processor));
                processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER] =
                        (uint16_t)(processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER] +
                                   stackByteCount);
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_RETURN_FAR:
                processor->instructionPointer = hyperdos_x86_16_pop_word(processor);
                hyperdos_x86_16_set_segment_register(processor,
                                                     HYPERDOS_X86_16_SEGMENT_REGISTER_CODE,
                                                     hyperdos_x86_16_pop_word(processor));
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_INTERRUPT_BREAKPOINT:
                suppressTrapAfterInstruction = 1;
                result = hyperdos_x86_16_execute_interrupt(processor, HYPERDOS_X86_16_INTERRUPT_TYPE_BREAKPOINT);
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_INTERRUPT_IMMEDIATE:
                suppressTrapAfterInstruction = 1;
                result                       = hyperdos_x86_16_execute_interrupt(processor,
                                                           hyperdos_x86_16_fetch_instruction_byte(processor));
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_INTERRUPT_ON_OVERFLOW:
                if ((processor->flags & HYPERDOS_X86_16_FLAG_OVERFLOW) != 0u)
                {
                    suppressTrapAfterInstruction = 1;
                    result = hyperdos_x86_16_execute_interrupt(processor, HYPERDOS_X86_16_INTERRUPT_TYPE_OVERFLOW);
                }
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_INTERRUPT_RETURN:
                processor->instructionPointer = hyperdos_x86_16_pop_word(processor);
                hyperdos_x86_16_set_segment_register(processor,
                                                     HYPERDOS_X86_16_SEGMENT_REGISTER_CODE,
                                                     hyperdos_x86_16_pop_word(processor));
                processor->flags             = hyperdos_x86_16_normalize_flags(hyperdos_x86_16_pop_word(processor));
                suppressTrapAfterInstruction = !trapWasEnabled;
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_ASCII_ADJUST_AFTER_MULTIPLY:
            case HYPERDOS_X86_16_OPERATION_CODE_ASCII_ADJUST_BEFORE_DIVIDE:
                result = hyperdos_x86_16_execute_ascii_adjust_multiply_divide_instruction(processor, operationCode);
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_TRANSLATE_BYTE:
            {
                hyperdos_x86_16_segment_register_index
                        translateSegment = hasSegmentOverride ? segmentOverride : HYPERDOS_X86_16_SEGMENT_REGISTER_DATA;
                uint16_t translateOffset = (uint16_t)(processor->generalRegisters
                                                              [HYPERDOS_X86_16_GENERAL_REGISTER_BASE] +
                                                      hyperdos_x86_16_read_byte_register(processor, 0u));
                hyperdos_x86_16_write_byte_register(processor,
                                                    0u,
                                                    hyperdos_x86_16_read_memory_byte_fast(processor,
                                                                                          translateSegment,
                                                                                          translateOffset));
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_JUMP_IF_COUNTER_ZERO:
            {
                int8_t displacement = (int8_t)hyperdos_x86_16_fetch_instruction_byte(processor);
                if (processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER] == 0u)
                {
                    hyperdos_x86_16_jump_relative(processor, displacement);
                }
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_INPUT_IMMEDIATE_BYTE:
                hyperdos_x86_16_write_byte_register(
                        processor,
                        0u,
                        hyperdos_x86_16_read_input_output_byte(processor,
                                                               hyperdos_x86_16_fetch_instruction_byte(processor)));
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_INPUT_IMMEDIATE_WORD:
            {
                uint16_t port     = hyperdos_x86_16_fetch_instruction_byte(processor);
                uint16_t lowByte  = hyperdos_x86_16_read_input_output_byte(processor, port);
                uint16_t highByte = hyperdos_x86_16_read_input_output_byte(processor,
                                                                           (uint16_t)(port +
                                                                                      HYPERDOS_X86_16_BYTE_SIZE));
                processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] =
                        (uint16_t)(lowByte | (highByte << HYPERDOS_X86_16_BYTE_BIT_COUNT));
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_OUTPUT_IMMEDIATE_BYTE:
                hyperdos_x86_16_write_input_output_byte(processor,
                                                        hyperdos_x86_16_fetch_instruction_byte(processor),
                                                        hyperdos_x86_16_read_byte_register(processor, 0u));
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_OUTPUT_IMMEDIATE_WORD:
            {
                uint16_t port        = hyperdos_x86_16_fetch_instruction_byte(processor);
                uint16_t accumulator = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR];
                hyperdos_x86_16_write_input_output_byte(processor,
                                                        port,
                                                        (uint8_t)(accumulator & HYPERDOS_X86_16_BYTE_MASK));
                hyperdos_x86_16_write_input_output_byte(processor,
                                                        (uint16_t)(port + HYPERDOS_X86_16_BYTE_SIZE),
                                                        (uint8_t)(accumulator >> HYPERDOS_X86_16_BYTE_BIT_COUNT));
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_CALL_RELATIVE:
            {
                int16_t displacement = (int16_t)hyperdos_x86_16_fetch_instruction_word(processor);
                hyperdos_x86_16_push_word(processor, processor->instructionPointer);
                hyperdos_x86_16_jump_relative(processor, displacement);
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_JUMP_RELATIVE:
                hyperdos_x86_16_jump_relative(processor, (int16_t)hyperdos_x86_16_fetch_instruction_word(processor));
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_JUMP_FAR_IMMEDIATE:
            {
                uint16_t offset  = hyperdos_x86_16_fetch_instruction_word(processor);
                uint16_t segment = hyperdos_x86_16_fetch_instruction_word(processor);
                hyperdos_x86_16_set_segment_register(processor, HYPERDOS_X86_16_SEGMENT_REGISTER_CODE, segment);
                processor->instructionPointer = offset;
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_JUMP_SHORT:
                hyperdos_x86_16_jump_relative(processor, (int8_t)hyperdos_x86_16_fetch_instruction_byte(processor));
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_INPUT_FROM_DATA_BYTE:
                hyperdos_x86_16_write_byte_register(
                        processor,
                        0u,
                        hyperdos_x86_16_read_input_output_byte(processor,
                                                               processor->generalRegisters
                                                                       [HYPERDOS_X86_16_GENERAL_REGISTER_DATA]));
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_INPUT_FROM_DATA_WORD:
            {
                uint16_t port     = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA];
                uint16_t lowByte  = hyperdos_x86_16_read_input_output_byte(processor, port);
                uint16_t highByte = hyperdos_x86_16_read_input_output_byte(processor,
                                                                           (uint16_t)(port +
                                                                                      HYPERDOS_X86_16_BYTE_SIZE));
                processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR] =
                        (uint16_t)(lowByte | (highByte << HYPERDOS_X86_16_BYTE_BIT_COUNT));
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_OUTPUT_TO_DATA_BYTE:
                hyperdos_x86_16_write_input_output_byte(processor,
                                                        processor->generalRegisters
                                                                [HYPERDOS_X86_16_GENERAL_REGISTER_DATA],
                                                        hyperdos_x86_16_read_byte_register(processor, 0u));
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_OUTPUT_TO_DATA_WORD:
            {
                uint16_t port        = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_DATA];
                uint16_t accumulator = processor->generalRegisters[HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR];
                hyperdos_x86_16_write_input_output_byte(processor,
                                                        port,
                                                        (uint8_t)(accumulator & HYPERDOS_X86_16_BYTE_MASK));
                hyperdos_x86_16_write_input_output_byte(processor,
                                                        (uint16_t)(port + HYPERDOS_X86_16_BYTE_SIZE),
                                                        (uint8_t)(accumulator >> HYPERDOS_X86_16_BYTE_BIT_COUNT));
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_HALT:
                processor->halted            = 1u;
                suppressTrapAfterInstruction = 1;
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_COMPLEMENT_CARRY:
                processor->flags ^= HYPERDOS_X86_16_FLAG_CARRY;
                processor->flags |= HYPERDOS_X86_16_FLAG_RESERVED;
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_GROUP_THREE_BYTE:
            case HYPERDOS_X86_16_OPERATION_CODE_GROUP_THREE_WORD:
                result = hyperdos_x86_16_execute_multiply_divide_instruction(processor,
                                                                             operationCode,
                                                                             hasSegmentOverride,
                                                                             segmentOverride);
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_CLEAR_CARRY:
                processor->flags &= (uint16_t)~HYPERDOS_X86_16_FLAG_CARRY;
                processor->flags |= HYPERDOS_X86_16_FLAG_RESERVED;
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_SET_CARRY:
                processor->flags |= HYPERDOS_X86_16_FLAG_CARRY | HYPERDOS_X86_16_FLAG_RESERVED;
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_CLEAR_INTERRUPT:
                processor->flags &= (uint16_t)~HYPERDOS_X86_16_FLAG_INTERRUPT_ENABLE;
                processor->flags |= HYPERDOS_X86_16_FLAG_RESERVED;
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_SET_INTERRUPT:
                processor->flags              |= HYPERDOS_X86_16_FLAG_INTERRUPT_ENABLE | HYPERDOS_X86_16_FLAG_RESERVED;
                startMaskableInterruptInhibit  = 1;
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_CLEAR_DIRECTION:
                processor->flags &= (uint16_t)~HYPERDOS_X86_16_FLAG_DIRECTION;
                processor->flags |= HYPERDOS_X86_16_FLAG_RESERVED;
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_SET_DIRECTION:
                processor->flags |= HYPERDOS_X86_16_FLAG_DIRECTION | HYPERDOS_X86_16_FLAG_RESERVED;
                break;

            case HYPERDOS_X86_16_OPERATION_CODE_INCREMENT_DECREMENT_BYTE:
            {
                uint8_t registerMemoryByte = hyperdos_x86_16_fetch_instruction_byte(processor);
                uint8_t operation          = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_16_MODRM_REGISTER_SHIFT) &
                                              HYPERDOS_X86_16_GENERAL_REGISTER_INDEX_MASK);
                hyperdos_x86_16_decoded_operand
                         operand = hyperdos_x86_16_decode_register_memory_operand(processor,
                                                                                 registerMemoryByte,
                                                                                 hasSegmentOverride,
                                                                                 segmentOverride);
                uint16_t value   = hyperdos_x86_16_read_operand_value(processor, &operand, 0);
                if (operation == HYPERDOS_X86_16_GROUP_FIVE_INCREMENT)
                {
                    hyperdos_x86_16_update_increment_decrement_flags(processor,
                                                                     HYPERDOS_X86_16_ARITHMETIC_LOGIC_ADD,
                                                                     value,
                                                                     HYPERDOS_X86_16_BYTE_MASK,
                                                                     HYPERDOS_X86_16_BYTE_SIGN_BIT);
                    hyperdos_x86_16_write_operand_value(processor, &operand, 0, (uint8_t)(value + 1u));
                }
                else if (operation == HYPERDOS_X86_16_GROUP_FIVE_DECREMENT)
                {
                    hyperdos_x86_16_update_increment_decrement_flags(processor,
                                                                     HYPERDOS_X86_16_ARITHMETIC_LOGIC_SUB,
                                                                     value,
                                                                     HYPERDOS_X86_16_BYTE_MASK,
                                                                     HYPERDOS_X86_16_BYTE_SIGN_BIT);
                    hyperdos_x86_16_write_operand_value(processor, &operand, 0, (uint8_t)(value - 1u));
                }
                else
                {
                    result = HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION;
                }
                break;
            }

            case HYPERDOS_X86_16_OPERATION_CODE_GROUP_FIVE:
                result = hyperdos_x86_16_execute_group_five_instruction(processor, hasSegmentOverride, segmentOverride);
                break;

            default:
                result = HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION;
                break;
            }
        }

        {
            int instructionCompleted = result != HYPERDOS_X86_16_EXECUTION_DIVIDE_ERROR;

            if (result == HYPERDOS_X86_16_EXECUTION_DIVIDE_ERROR)
            {
                hyperdos_x86_16_execution_result interruptResult =
                        processor->divideErrorReturnsToFaultingInstruction
                                ? hyperdos_x86_16_execute_interrupt_with_return_address(
                                          processor,
                                          HYPERDOS_X86_16_INTERRUPT_TYPE_DIVIDE_ERROR,
                                          processor->lastInstructionSegment,
                                          processor->lastInstructionOffset)
                                : hyperdos_x86_16_execute_interrupt(processor,
                                                                    HYPERDOS_X86_16_INTERRUPT_TYPE_DIVIDE_ERROR);
                if (interruptResult == HYPERDOS_X86_16_EXECUTION_INTERRUPT_NOT_HANDLED)
                {
                    return result;
                }
                result = interruptResult;
            }

            if (instructionCompleted && result == HYPERDOS_X86_16_EXECUTION_OK)
            {
                if (maskableInterruptInhibitWasActive && processor->maskableInterruptInhibitCount != 0u)
                {
                    --processor->maskableInterruptInhibitCount;
                }
                if (startMaskableInterruptInhibit)
                {
                    processor->maskableInterruptInhibitCount = 1u;
                }
                if (trapWasEnabled && !suppressTrapAfterInstruction && processor->halted == 0u)
                {
                    result = hyperdos_x86_16_execute_interrupt(processor, HYPERDOS_X86_16_INTERRUPT_TYPE_SINGLE_STEP);
                }
            }
        }

        if (result != HYPERDOS_X86_16_EXECUTION_OK)
        {
            return result;
        }

        ++processor->executedInstructionCount;
    }

    return HYPERDOS_X86_16_EXECUTION_HALTED;
}

int hyperdos_x86_16_processor_accepts_maskable_interrupt(const hyperdos_x86_16_processor* processor)
{
    if (processor == NULL)
    {
        return 0;
    }
    return (processor->flags & HYPERDOS_X86_16_FLAG_INTERRUPT_ENABLE) != 0u &&
           processor->maskableInterruptInhibitCount == 0u;
}

hyperdos_x86_16_execution_result hyperdos_x86_16_request_maskable_interrupt(hyperdos_x86_16_processor* processor,
                                                                            uint8_t                    interruptNumber)
{
    if (processor == NULL)
    {
        return HYPERDOS_X86_16_EXECUTION_INVALID_ARGUMENT;
    }
    if (!hyperdos_x86_16_processor_accepts_maskable_interrupt(processor))
    {
        return HYPERDOS_X86_16_EXECUTION_OK;
    }
    processor->halted = 0u;
    return hyperdos_x86_16_execute_interrupt(processor, interruptNumber);
}

void hyperdos_x86_16_stop_processor(hyperdos_x86_16_processor* processor)
{
    processor->halted = 1u;
}

uint32_t hyperdos_x86_16_get_physical_address(const hyperdos_x86_16_processor*       processor,
                                              hyperdos_x86_16_segment_register_index segmentRegister,
                                              uint16_t                               offset)
{
    if (processor == NULL || segmentRegister > HYPERDOS_X86_16_SEGMENT_REGISTER_DATA)
    {
        return 0;
    }
    return hyperdos_x86_16_physical_address_from_base(processor->segmentBases[segmentRegister], offset);
}

hyperdos_x86_16_execution_result hyperdos_x86_16_read_memory_byte(
        const hyperdos_x86_16_processor*       processor,
        hyperdos_x86_16_segment_register_index segmentRegister,
        uint16_t                               offset,
        uint8_t*                               value)
{
    if (processor == NULL || value == NULL || segmentRegister > HYPERDOS_X86_16_SEGMENT_REGISTER_DATA)
    {
        return HYPERDOS_X86_16_EXECUTION_INVALID_ARGUMENT;
    }
    *value = hyperdos_x86_16_read_memory_byte_fast(processor, segmentRegister, offset);
    return HYPERDOS_X86_16_EXECUTION_OK;
}

hyperdos_x86_16_execution_result hyperdos_x86_16_write_memory_byte(
        hyperdos_x86_16_processor*             processor,
        hyperdos_x86_16_segment_register_index segmentRegister,
        uint16_t                               offset,
        uint8_t                                value)
{
    if (processor == NULL || segmentRegister > HYPERDOS_X86_16_SEGMENT_REGISTER_DATA)
    {
        return HYPERDOS_X86_16_EXECUTION_INVALID_ARGUMENT;
    }
    hyperdos_x86_16_write_memory_byte_fast(processor, segmentRegister, offset, value);
    return HYPERDOS_X86_16_EXECUTION_OK;
}

uint8_t hyperdos_x86_16_read_input_output_byte(hyperdos_x86_16_processor* processor, uint16_t port)
{
    if (processor == NULL || processor->bus == NULL)
    {
        return HYPERDOS_X86_16_BYTE_MASK;
    }
    return hyperdos_bus_read_input_output_byte_or_open_bus(processor->bus, port);
}

void hyperdos_x86_16_write_input_output_byte(hyperdos_x86_16_processor* processor, uint16_t port, uint8_t value)
{
    if (processor == NULL || processor->bus == NULL)
    {
        return;
    }
    hyperdos_bus_write_input_output_byte_if_mapped(processor->bus, port, value);
}

uint16_t hyperdos_x86_16_get_general_register(const hyperdos_x86_16_processor*       processor,
                                              hyperdos_x86_16_general_register_index registerIndex)
{
    if (processor == NULL || registerIndex > HYPERDOS_X86_16_GENERAL_REGISTER_DESTINATION_INDEX)
    {
        return 0;
    }
    return processor->generalRegisters[registerIndex];
}

void hyperdos_x86_16_set_general_register(hyperdos_x86_16_processor*             processor,
                                          hyperdos_x86_16_general_register_index registerIndex,
                                          uint16_t                               value)
{
    if (processor == NULL || registerIndex > HYPERDOS_X86_16_GENERAL_REGISTER_DESTINATION_INDEX)
    {
        return;
    }
    processor->generalRegisters[registerIndex] = value;
}

uint16_t hyperdos_x86_16_get_segment_register(const hyperdos_x86_16_processor*       processor,
                                              hyperdos_x86_16_segment_register_index segmentRegister)
{
    if (processor == NULL || segmentRegister > HYPERDOS_X86_16_SEGMENT_REGISTER_DATA)
    {
        return 0;
    }
    return processor->segmentRegisters[segmentRegister];
}

void hyperdos_x86_16_set_segment_register(hyperdos_x86_16_processor*             processor,
                                          hyperdos_x86_16_segment_register_index segmentRegister,
                                          uint16_t                               value)
{
    if (processor == NULL || segmentRegister > HYPERDOS_X86_16_SEGMENT_REGISTER_DATA)
    {
        return;
    }
    processor->segmentRegisters[segmentRegister] = value;
    hyperdos_x86_16_store_segment_base(processor, segmentRegister);
}

const char* hyperdos_x86_16_execution_result_name(hyperdos_x86_16_execution_result result)
{
    switch (result)
    {
    case HYPERDOS_X86_16_EXECUTION_OK:
        return "ok";
    case HYPERDOS_X86_16_EXECUTION_HALTED:
        return "halted";
    case HYPERDOS_X86_16_EXECUTION_STEP_LIMIT_REACHED:
        return "step limit reached";
    case HYPERDOS_X86_16_EXECUTION_INVALID_ARGUMENT:
        return "invalid argument";
    case HYPERDOS_X86_16_EXECUTION_PROGRAM_TOO_LARGE:
        return "program too large";
    case HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION:
        return "unsupported instruction";
    case HYPERDOS_X86_16_EXECUTION_INTERRUPT_NOT_HANDLED:
        return "interrupt not handled";
    case HYPERDOS_X86_16_EXECUTION_DIVIDE_ERROR:
        return "divide error";
    }
    return "unknown";
}
