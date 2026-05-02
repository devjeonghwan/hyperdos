#include "hyperdos/x86_processor.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "hyperdos/hardware.h"

enum
{
    HYPERDOS_X86_TOP_TWO_BIT_COUNT                        = 2u,
    HYPERDOS_X86_NIBBLE_MASK                              = 0x0Fu,
    HYPERDOS_X86_LOW_TWO_BITS_MASK                        = 0x03u,
    HYPERDOS_X86_EXTENDED_WORD_MASK                       = 0x1FFFFu,
    HYPERDOS_X86_BYTE_SIGN_BIT                            = 0x0080u,
    HYPERDOS_X86_WORD_SIGN_BIT                            = 0x8000u,
    HYPERDOS_X86_SEGMENT_BYTE_COUNT                       = 0x10000u,
    HYPERDOS_X86_AUXILIARY_CARRY_BIT                      = 0x0010u,
    HYPERDOS_X86_BYTE_REGISTER_WORD_INDEX_MASK            = 0x03u,
    HYPERDOS_X86_BYTE_REGISTER_HIGH_PART_BIT              = 0x04u,
    HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK              = 0x07u,
    HYPERDOS_X86_MODRM_REGISTER_MEMORY_MASK               = 0x07u,
    HYPERDOS_X86_MODRM_REGISTER_SHIFT                     = 3u,
    HYPERDOS_X86_MODRM_MODE_SHIFT                         = 6u,
    HYPERDOS_X86_MODRM_MODE_MEMORY_NO_DISPLACEMENT        = 0u,
    HYPERDOS_X86_MODRM_MODE_MEMORY_BYTE_DISPLACEMENT      = 1u,
    HYPERDOS_X86_MODRM_MODE_MEMORY_WORD_DISPLACEMENT      = 2u,
    HYPERDOS_X86_MODRM_MODE_REGISTER                      = 3u,
    HYPERDOS_X86_ARITHMETIC_FORM_WORD_BIT                 = 0x01u,
    HYPERDOS_X86_ARITHMETIC_FORM_REGISTER_DESTINATION_BIT = 0x02u,
    HYPERDOS_X86_ARITHMETIC_FORM_REGISTER_MEMORY_MAXIMUM  = 0x03u,
    HYPERDOS_X86_ARITHMETIC_FORM_ACCUMULATOR_BYTE         = 0x04u,
    HYPERDOS_X86_ARITHMETIC_FORM_ACCUMULATOR_WORD         = 0x05u,
    HYPERDOS_X86_OPERATION_CODE_WORD_BIT                  = 0x01u,
    HYPERDOS_X86_OPERATION_CODE_COUNT_IN_COUNTER_BIT      = 0x02u,
    HYPERDOS_X86_PARITY_FOLD_SHIFT                        = 4u,
    HYPERDOS_X86_PARITY_LOOKUP                            = 0x6996u,
    HYPERDOS_X86_INTERRUPT_VECTOR_BYTE_COUNT              = 4u,
    HYPERDOS_X86_FAR_POINTER_SEGMENT_OFFSET               = 2u,
    HYPERDOS_X86_RESET_CODE_SEGMENT                       = 0xFFFFu,
    HYPERDOS_X86_RESET_INSTRUCTION_POINTER                = 0x0000u,
    HYPERDOS_X86_DOS_PROGRAM_MAXIMUM_SIZE                 = 0xFF00u,
    HYPERDOS_X86_DOS_STACK_POINTER                        = 0xFFFEu,
    HYPERDOS_X86_DOS_TERMINATE_STUB_OFFSET                = 0x0000u,
    HYPERDOS_X86_DOS_COMMAND_TAIL_LENGTH_OFFSET           = 0x0080u,
    HYPERDOS_X86_DOS_COMMAND_TAIL_BYTES_OFFSET            = 0x0081u,
    HYPERDOS_X86_DOS_COMMAND_TAIL_MAXIMUM_LENGTH          = 126u,
    HYPERDOS_X86_ASCII_CARRIAGE_RETURN                    = 0x0Du,
    HYPERDOS_X86_INTERRUPT_TYPE_DIVIDE_ERROR              = 0u,
    HYPERDOS_X86_INTERRUPT_TYPE_SINGLE_STEP               = 1u,
    HYPERDOS_X86_INTERRUPT_TYPE_BREAKPOINT                = 3u,
    HYPERDOS_X86_INTERRUPT_TYPE_OVERFLOW                  = 4u,
    HYPERDOS_X86_INTERRUPT_TYPE_BOUND_RANGE_EXCEEDED      = 5u,
    HYPERDOS_X86_INTERRUPT_TYPE_UNUSED_OPERATION_CODE     = 6u,
    HYPERDOS_X86_INTERRUPT_TYPE_ESCAPE_OPERATION_CODE     = 7u,
    HYPERDOS_X86_FLAGS_LOW_BYTE_TRANSFER_MASK             = 0x00D7u,
    HYPERDOS_X86_FLAGS_HIGH_BYTE_MASK                     = 0xFF00u,
    HYPERDOS_X86_FLAGS_WRITABLE_MASK                      = 0x0FD5u,
    HYPERDOS_X86_ENTER_NESTING_LEVEL_MASK                 = 0x1Fu
};

typedef enum hyperdos_x86_operation_code
{
    HYPERDOS_X86_OPERATION_CODE_PUSH_EXTRA_SEGMENT                     = 0x06u,
    HYPERDOS_X86_OPERATION_CODE_POP_EXTRA_SEGMENT                      = 0x07u,
    HYPERDOS_X86_OPERATION_CODE_POP_CODE_SEGMENT                       = 0x0Fu,
    HYPERDOS_X86_OPERATION_CODE_PUSH_CODE_SEGMENT                      = 0x0Eu,
    HYPERDOS_X86_OPERATION_CODE_PUSH_STACK_SEGMENT                     = 0x16u,
    HYPERDOS_X86_OPERATION_CODE_POP_STACK_SEGMENT                      = 0x17u,
    HYPERDOS_X86_OPERATION_CODE_PUSH_DATA_SEGMENT                      = 0x1Eu,
    HYPERDOS_X86_OPERATION_CODE_POP_DATA_SEGMENT                       = 0x1Fu,
    HYPERDOS_X86_OPERATION_CODE_PREFIX_EXTRA_SEGMENT                   = 0x26u,
    HYPERDOS_X86_OPERATION_CODE_DECIMAL_ADJUST_AFTER_ADD               = 0x27u,
    HYPERDOS_X86_OPERATION_CODE_PREFIX_CODE_SEGMENT                    = 0x2Eu,
    HYPERDOS_X86_OPERATION_CODE_DECIMAL_ADJUST_AFTER_SUBTRACT          = 0x2Fu,
    HYPERDOS_X86_OPERATION_CODE_PREFIX_STACK_SEGMENT                   = 0x36u,
    HYPERDOS_X86_OPERATION_CODE_ASCII_ADJUST_AFTER_ADD                 = 0x37u,
    HYPERDOS_X86_OPERATION_CODE_PREFIX_DATA_SEGMENT                    = 0x3Eu,
    HYPERDOS_X86_OPERATION_CODE_ASCII_ADJUST_AFTER_SUBTRACT            = 0x3Fu,
    HYPERDOS_X86_OPERATION_CODE_ARITHMETIC_LOGIC_FIRST                 = 0x00u,
    HYPERDOS_X86_OPERATION_CODE_ARITHMETIC_LOGIC_LAST                  = 0x3Du,
    HYPERDOS_X86_OPERATION_CODE_INCREMENT_REGISTER_FIRST               = 0x40u,
    HYPERDOS_X86_OPERATION_CODE_INCREMENT_REGISTER_LAST                = 0x47u,
    HYPERDOS_X86_OPERATION_CODE_DECREMENT_REGISTER_FIRST               = 0x48u,
    HYPERDOS_X86_OPERATION_CODE_DECREMENT_REGISTER_LAST                = 0x4Fu,
    HYPERDOS_X86_OPERATION_CODE_PUSH_REGISTER_FIRST                    = 0x50u,
    HYPERDOS_X86_OPERATION_CODE_PUSH_REGISTER_LAST                     = 0x57u,
    HYPERDOS_X86_OPERATION_CODE_POP_REGISTER_FIRST                     = 0x58u,
    HYPERDOS_X86_OPERATION_CODE_POP_REGISTER_LAST                      = 0x5Fu,
    HYPERDOS_X86_OPERATION_CODE_PUSH_ALL                               = 0x60u,
    HYPERDOS_X86_OPERATION_CODE_POP_ALL                                = 0x61u,
    HYPERDOS_X86_OPERATION_CODE_BOUND                                  = 0x62u,
    HYPERDOS_X86_OPERATION_CODE_PUSH_IMMEDIATE_WORD                    = 0x68u,
    HYPERDOS_X86_OPERATION_CODE_SIGNED_MULTIPLY_IMMEDIATE_WORD         = 0x69u,
    HYPERDOS_X86_OPERATION_CODE_PUSH_IMMEDIATE_BYTE                    = 0x6Au,
    HYPERDOS_X86_OPERATION_CODE_SIGNED_MULTIPLY_IMMEDIATE_BYTE         = 0x6Bu,
    HYPERDOS_X86_OPERATION_CODE_INPUT_STRING_BYTE                      = 0x6Cu,
    HYPERDOS_X86_OPERATION_CODE_INPUT_STRING_WORD                      = 0x6Du,
    HYPERDOS_X86_OPERATION_CODE_OUTPUT_STRING_BYTE                     = 0x6Eu,
    HYPERDOS_X86_OPERATION_CODE_OUTPUT_STRING_WORD                     = 0x6Fu,
    HYPERDOS_X86_OPERATION_CODE_JUMP_CONDITION_FIRST                   = 0x70u,
    HYPERDOS_X86_OPERATION_CODE_JUMP_CONDITION_LAST                    = 0x7Fu,
    HYPERDOS_X86_OPERATION_CODE_GROUP_ONE_BYTE_IMMEDIATE               = 0x80u,
    HYPERDOS_X86_OPERATION_CODE_GROUP_ONE_WORD_IMMEDIATE               = 0x81u,
    HYPERDOS_X86_OPERATION_CODE_GROUP_ONE_BYTE_IMMEDIATE_ALIAS         = 0x82u,
    HYPERDOS_X86_OPERATION_CODE_GROUP_ONE_SIGN_EXTENDED_IMMEDIATE      = 0x83u,
    HYPERDOS_X86_OPERATION_CODE_TEST_REGISTER_MEMORY_BYTE              = 0x84u,
    HYPERDOS_X86_OPERATION_CODE_TEST_REGISTER_MEMORY_WORD              = 0x85u,
    HYPERDOS_X86_OPERATION_CODE_EXCHANGE_REGISTER_MEMORY_BYTE          = 0x86u,
    HYPERDOS_X86_OPERATION_CODE_EXCHANGE_REGISTER_MEMORY_WORD          = 0x87u,
    HYPERDOS_X86_OPERATION_CODE_MOVE_REGISTER_MEMORY_BYTE_FIRST        = 0x88u,
    HYPERDOS_X86_OPERATION_CODE_MOVE_REGISTER_MEMORY_WORD_LAST         = 0x8Bu,
    HYPERDOS_X86_OPERATION_CODE_MOVE_SEGMENT_TO_REGISTER_MEMORY        = 0x8Cu,
    HYPERDOS_X86_OPERATION_CODE_LOAD_EFFECTIVE_ADDRESS                 = 0x8Du,
    HYPERDOS_X86_OPERATION_CODE_MOVE_REGISTER_MEMORY_TO_SEGMENT        = 0x8Eu,
    HYPERDOS_X86_OPERATION_CODE_POP_REGISTER_MEMORY                    = 0x8Fu,
    HYPERDOS_X86_OPERATION_CODE_EXCHANGE_ACCUMULATOR_FIRST             = 0x90u,
    HYPERDOS_X86_OPERATION_CODE_EXCHANGE_ACCUMULATOR_LAST              = 0x97u,
    HYPERDOS_X86_OPERATION_CODE_CONVERT_BYTE_TO_WORD                   = 0x98u,
    HYPERDOS_X86_OPERATION_CODE_CONVERT_WORD_TO_DOUBLE_WORD            = 0x99u,
    HYPERDOS_X86_OPERATION_CODE_CALL_FAR_IMMEDIATE                     = 0x9Au,
    HYPERDOS_X86_OPERATION_CODE_WAIT                                   = 0x9Bu,
    HYPERDOS_X86_OPERATION_CODE_PUSH_FLAGS                             = 0x9Cu,
    HYPERDOS_X86_OPERATION_CODE_POP_FLAGS                              = 0x9Du,
    HYPERDOS_X86_OPERATION_CODE_STORE_STATUS_TO_FLAGS                  = 0x9Eu,
    HYPERDOS_X86_OPERATION_CODE_LOAD_STATUS_FROM_FLAGS                 = 0x9Fu,
    HYPERDOS_X86_OPERATION_CODE_MOVE_MEMORY_TO_ACCUMULATOR_BYTE        = 0xA0u,
    HYPERDOS_X86_OPERATION_CODE_MOVE_MEMORY_TO_ACCUMULATOR_WORD        = 0xA1u,
    HYPERDOS_X86_OPERATION_CODE_MOVE_ACCUMULATOR_TO_MEMORY_BYTE        = 0xA2u,
    HYPERDOS_X86_OPERATION_CODE_MOVE_ACCUMULATOR_TO_MEMORY_WORD        = 0xA3u,
    HYPERDOS_X86_OPERATION_CODE_MOVE_STRING_BYTE                       = 0xA4u,
    HYPERDOS_X86_OPERATION_CODE_MOVE_STRING_WORD                       = 0xA5u,
    HYPERDOS_X86_OPERATION_CODE_COMPARE_STRING_BYTE                    = 0xA6u,
    HYPERDOS_X86_OPERATION_CODE_COMPARE_STRING_WORD                    = 0xA7u,
    HYPERDOS_X86_OPERATION_CODE_TEST_ACCUMULATOR_BYTE                  = 0xA8u,
    HYPERDOS_X86_OPERATION_CODE_TEST_ACCUMULATOR_WORD                  = 0xA9u,
    HYPERDOS_X86_OPERATION_CODE_STORE_STRING_BYTE                      = 0xAAu,
    HYPERDOS_X86_OPERATION_CODE_STORE_STRING_WORD                      = 0xABu,
    HYPERDOS_X86_OPERATION_CODE_LOAD_STRING_BYTE                       = 0xACu,
    HYPERDOS_X86_OPERATION_CODE_LOAD_STRING_WORD                       = 0xADu,
    HYPERDOS_X86_OPERATION_CODE_SCAN_STRING_BYTE                       = 0xAEu,
    HYPERDOS_X86_OPERATION_CODE_SCAN_STRING_WORD                       = 0xAFu,
    HYPERDOS_X86_OPERATION_CODE_MOVE_IMMEDIATE_TO_BYTE_REGISTER_FIRST  = 0xB0u,
    HYPERDOS_X86_OPERATION_CODE_MOVE_IMMEDIATE_TO_BYTE_REGISTER_LAST   = 0xB7u,
    HYPERDOS_X86_OPERATION_CODE_MOVE_IMMEDIATE_TO_WORD_REGISTER_FIRST  = 0xB8u,
    HYPERDOS_X86_OPERATION_CODE_MOVE_IMMEDIATE_TO_WORD_REGISTER_LAST   = 0xBFu,
    HYPERDOS_X86_OPERATION_CODE_SHIFT_ROTATE_IMMEDIATE_BYTE            = 0xC0u,
    HYPERDOS_X86_OPERATION_CODE_SHIFT_ROTATE_IMMEDIATE_WORD            = 0xC1u,
    HYPERDOS_X86_OPERATION_CODE_RETURN_NEAR_IMMEDIATE                  = 0xC2u,
    HYPERDOS_X86_OPERATION_CODE_RETURN_NEAR                            = 0xC3u,
    HYPERDOS_X86_OPERATION_CODE_LOAD_EXTRA_SEGMENT_POINTER             = 0xC4u,
    HYPERDOS_X86_OPERATION_CODE_LOAD_DATA_SEGMENT_POINTER              = 0xC5u,
    HYPERDOS_X86_OPERATION_CODE_MOVE_IMMEDIATE_TO_REGISTER_MEMORY_BYTE = 0xC6u,
    HYPERDOS_X86_OPERATION_CODE_MOVE_IMMEDIATE_TO_REGISTER_MEMORY_WORD = 0xC7u,
    HYPERDOS_X86_OPERATION_CODE_ENTER                                  = 0xC8u,
    HYPERDOS_X86_OPERATION_CODE_LEAVE                                  = 0xC9u,
    HYPERDOS_X86_OPERATION_CODE_RETURN_FAR_IMMEDIATE                   = 0xCAu,
    HYPERDOS_X86_OPERATION_CODE_RETURN_FAR                             = 0xCBu,
    HYPERDOS_X86_OPERATION_CODE_INTERRUPT_BREAKPOINT                   = 0xCCu,
    HYPERDOS_X86_OPERATION_CODE_INTERRUPT_IMMEDIATE                    = 0xCDu,
    HYPERDOS_X86_OPERATION_CODE_INTERRUPT_ON_OVERFLOW                  = 0xCEu,
    HYPERDOS_X86_OPERATION_CODE_INTERRUPT_RETURN                       = 0xCFu,
    HYPERDOS_X86_OPERATION_CODE_SHIFT_ROTATE_FIRST                     = 0xD0u,
    HYPERDOS_X86_OPERATION_CODE_SHIFT_ROTATE_LAST                      = 0xD3u,
    HYPERDOS_X86_OPERATION_CODE_ASCII_ADJUST_AFTER_MULTIPLY            = 0xD4u,
    HYPERDOS_X86_OPERATION_CODE_ASCII_ADJUST_BEFORE_DIVIDE             = 0xD5u,
    HYPERDOS_X86_OPERATION_CODE_TRANSLATE_BYTE                         = 0xD7u,
    HYPERDOS_X86_OPERATION_CODE_ESCAPE_COPROCESSOR_FIRST               = 0xD8u,
    HYPERDOS_X86_OPERATION_CODE_ESCAPE_COPROCESSOR_LAST                = 0xDFu,
    HYPERDOS_X86_OPERATION_CODE_LOOP_NOT_ZERO_AND_NOT_EQUAL            = 0xE0u,
    HYPERDOS_X86_OPERATION_CODE_LOOP_NOT_ZERO_AND_EQUAL                = 0xE1u,
    HYPERDOS_X86_OPERATION_CODE_LOOP_NOT_ZERO                          = 0xE2u,
    HYPERDOS_X86_OPERATION_CODE_JUMP_IF_COUNTER_ZERO                   = 0xE3u,
    HYPERDOS_X86_OPERATION_CODE_INPUT_IMMEDIATE_BYTE                   = 0xE4u,
    HYPERDOS_X86_OPERATION_CODE_INPUT_IMMEDIATE_WORD                   = 0xE5u,
    HYPERDOS_X86_OPERATION_CODE_OUTPUT_IMMEDIATE_BYTE                  = 0xE6u,
    HYPERDOS_X86_OPERATION_CODE_OUTPUT_IMMEDIATE_WORD                  = 0xE7u,
    HYPERDOS_X86_OPERATION_CODE_CALL_RELATIVE                          = 0xE8u,
    HYPERDOS_X86_OPERATION_CODE_JUMP_RELATIVE                          = 0xE9u,
    HYPERDOS_X86_OPERATION_CODE_JUMP_FAR_IMMEDIATE                     = 0xEAu,
    HYPERDOS_X86_OPERATION_CODE_JUMP_SHORT                             = 0xEBu,
    HYPERDOS_X86_OPERATION_CODE_INPUT_FROM_DATA_BYTE                   = 0xECu,
    HYPERDOS_X86_OPERATION_CODE_INPUT_FROM_DATA_WORD                   = 0xEDu,
    HYPERDOS_X86_OPERATION_CODE_OUTPUT_TO_DATA_BYTE                    = 0xEEu,
    HYPERDOS_X86_OPERATION_CODE_OUTPUT_TO_DATA_WORD                    = 0xEFu,
    HYPERDOS_X86_OPERATION_CODE_PREFIX_LOCK                            = 0xF0u,
    HYPERDOS_X86_OPERATION_CODE_PREFIX_REPEAT_WHILE_NOT_EQUAL          = 0xF2u,
    HYPERDOS_X86_OPERATION_CODE_PREFIX_REPEAT                          = 0xF3u,
    HYPERDOS_X86_OPERATION_CODE_HALT                                   = 0xF4u,
    HYPERDOS_X86_OPERATION_CODE_COMPLEMENT_CARRY                       = 0xF5u,
    HYPERDOS_X86_OPERATION_CODE_GROUP_THREE_BYTE                       = 0xF6u,
    HYPERDOS_X86_OPERATION_CODE_GROUP_THREE_WORD                       = 0xF7u,
    HYPERDOS_X86_OPERATION_CODE_CLEAR_CARRY                            = 0xF8u,
    HYPERDOS_X86_OPERATION_CODE_SET_CARRY                              = 0xF9u,
    HYPERDOS_X86_OPERATION_CODE_CLEAR_INTERRUPT                        = 0xFAu,
    HYPERDOS_X86_OPERATION_CODE_SET_INTERRUPT                          = 0xFBu,
    HYPERDOS_X86_OPERATION_CODE_CLEAR_DIRECTION                        = 0xFCu,
    HYPERDOS_X86_OPERATION_CODE_SET_DIRECTION                          = 0xFDu,
    HYPERDOS_X86_OPERATION_CODE_INCREMENT_DECREMENT_BYTE               = 0xFEu,
    HYPERDOS_X86_OPERATION_CODE_GROUP_FIVE                             = 0xFFu
} hyperdos_x86_operation_code;

typedef enum hyperdos_x86_repeat_mode
{
    HYPERDOS_X86_REPEAT_NONE = 0,
    HYPERDOS_X86_REPEAT_WHILE_EQUAL,
    HYPERDOS_X86_REPEAT_WHILE_NOT_EQUAL
} hyperdos_x86_repeat_mode;

typedef enum hyperdos_x86_arithmetic_logic_operation
{
    HYPERDOS_X86_ARITHMETIC_LOGIC_ADD = 0,
    HYPERDOS_X86_ARITHMETIC_LOGIC_OR  = 1,
    HYPERDOS_X86_ARITHMETIC_LOGIC_ADC = 2,
    HYPERDOS_X86_ARITHMETIC_LOGIC_SBB = 3,
    HYPERDOS_X86_ARITHMETIC_LOGIC_AND = 4,
    HYPERDOS_X86_ARITHMETIC_LOGIC_SUB = 5,
    HYPERDOS_X86_ARITHMETIC_LOGIC_XOR = 6,
    HYPERDOS_X86_ARITHMETIC_LOGIC_CMP = 7
} hyperdos_x86_arithmetic_logic_operation;

typedef enum hyperdos_x86_condition_code
{
    HYPERDOS_X86_CONDITION_OVERFLOW         = 0x0u,
    HYPERDOS_X86_CONDITION_NOT_OVERFLOW     = 0x1u,
    HYPERDOS_X86_CONDITION_CARRY            = 0x2u,
    HYPERDOS_X86_CONDITION_NOT_CARRY        = 0x3u,
    HYPERDOS_X86_CONDITION_ZERO             = 0x4u,
    HYPERDOS_X86_CONDITION_NOT_ZERO         = 0x5u,
    HYPERDOS_X86_CONDITION_BELOW_OR_EQUAL   = 0x6u,
    HYPERDOS_X86_CONDITION_ABOVE            = 0x7u,
    HYPERDOS_X86_CONDITION_SIGN             = 0x8u,
    HYPERDOS_X86_CONDITION_NOT_SIGN         = 0x9u,
    HYPERDOS_X86_CONDITION_PARITY           = 0xAu,
    HYPERDOS_X86_CONDITION_NOT_PARITY       = 0xBu,
    HYPERDOS_X86_CONDITION_LESS             = 0xCu,
    HYPERDOS_X86_CONDITION_GREATER_OR_EQUAL = 0xDu,
    HYPERDOS_X86_CONDITION_LESS_OR_EQUAL    = 0xEu,
    HYPERDOS_X86_CONDITION_GREATER          = 0xFu
} hyperdos_x86_condition_code;

typedef enum hyperdos_x86_shift_rotate_operation
{
    HYPERDOS_X86_SHIFT_ROTATE_ROL    = 0u,
    HYPERDOS_X86_SHIFT_ROTATE_ROR    = 1u,
    HYPERDOS_X86_SHIFT_ROTATE_RCL    = 2u,
    HYPERDOS_X86_SHIFT_ROTATE_RCR    = 3u,
    HYPERDOS_X86_SHIFT_ROTATE_SHL    = 4u,
    HYPERDOS_X86_SHIFT_ROTATE_SHR    = 5u,
    HYPERDOS_X86_SHIFT_ROTATE_UNUSED = 6u,
    HYPERDOS_X86_SHIFT_ROTATE_SAR    = 7u
} hyperdos_x86_shift_rotate_operation;

typedef enum hyperdos_x86_group_three_operation
{
    HYPERDOS_X86_GROUP_THREE_TEST              = 0u,
    HYPERDOS_X86_GROUP_THREE_NOT               = 2u,
    HYPERDOS_X86_GROUP_THREE_NEGATE            = 3u,
    HYPERDOS_X86_GROUP_THREE_MULTIPLY_UNSIGNED = 4u,
    HYPERDOS_X86_GROUP_THREE_MULTIPLY_SIGNED   = 5u,
    HYPERDOS_X86_GROUP_THREE_DIVIDE_UNSIGNED   = 6u,
    HYPERDOS_X86_GROUP_THREE_DIVIDE_SIGNED     = 7u
} hyperdos_x86_group_three_operation;

typedef enum hyperdos_x86_group_four_operation
{
    HYPERDOS_X86_GROUP_FOUR_INCREMENT = 0u,
    HYPERDOS_X86_GROUP_FOUR_DECREMENT = 1u,
    HYPERDOS_X86_GROUP_FOUR_UNUSED    = 7u
} hyperdos_x86_group_four_operation;

typedef enum hyperdos_x86_group_five_operation
{
    HYPERDOS_X86_GROUP_FIVE_INCREMENT = 0u,
    HYPERDOS_X86_GROUP_FIVE_DECREMENT = 1u,
    HYPERDOS_X86_GROUP_FIVE_CALL_NEAR = 2u,
    HYPERDOS_X86_GROUP_FIVE_CALL_FAR  = 3u,
    HYPERDOS_X86_GROUP_FIVE_JUMP_NEAR = 4u,
    HYPERDOS_X86_GROUP_FIVE_JUMP_FAR  = 5u,
    HYPERDOS_X86_GROUP_FIVE_PUSH      = 6u,
    HYPERDOS_X86_GROUP_FIVE_UNUSED    = 7u
} hyperdos_x86_group_five_operation;

typedef enum hyperdos_x86_architecture_generation
{
    HYPERDOS_X86_ARCHITECTURE_GENERATION_8086 = 0u,
    HYPERDOS_X86_ARCHITECTURE_GENERATION_80186
} hyperdos_x86_architecture_generation;

typedef enum hyperdos_x86_execution_mode
{
    HYPERDOS_X86_EXECUTION_MODE_REAL = 0u
} hyperdos_x86_execution_mode;

typedef enum hyperdos_x86_external_bus_profile
{
    HYPERDOS_X86_EXTERNAL_BUS_PROFILE_8086 = 0u,
    HYPERDOS_X86_EXTERNAL_BUS_PROFILE_8088,
    HYPERDOS_X86_EXTERNAL_BUS_PROFILE_80186,
    HYPERDOS_X86_EXTERNAL_BUS_PROFILE_80188
} hyperdos_x86_external_bus_profile;

typedef enum hyperdos_x86_operand_address_profile
{
    HYPERDOS_X86_OPERAND_ADDRESS_PROFILE_REAL_MODE_16_BIT = 0u
} hyperdos_x86_operand_address_profile;

typedef enum hyperdos_x86_exception_model
{
    HYPERDOS_X86_EXCEPTION_MODEL_8086 = 0u,
    HYPERDOS_X86_EXCEPTION_MODEL_80186
} hyperdos_x86_exception_model;

typedef enum hyperdos_x86_shift_rotate_count_policy
{
    HYPERDOS_X86_SHIFT_ROTATE_COUNT_POLICY_RAW = 0u,
    HYPERDOS_X86_SHIFT_ROTATE_COUNT_POLICY_MASK_TO_FIVE_BITS
} hyperdos_x86_shift_rotate_count_policy;

typedef enum hyperdos_x86_signed_divide_quotient_policy
{
    HYPERDOS_X86_SIGNED_DIVIDE_QUOTIENT_POLICY_EXCLUDE_MINIMUM_NEGATIVE = 0u,
    HYPERDOS_X86_SIGNED_DIVIDE_QUOTIENT_POLICY_ALLOW_MINIMUM_NEGATIVE
} hyperdos_x86_signed_divide_quotient_policy;

typedef enum hyperdos_x86_segment_boundary_access_policy
{
    HYPERDOS_X86_SEGMENT_BOUNDARY_ACCESS_POLICY_WRAP_OFFSET = 0u,
    HYPERDOS_X86_SEGMENT_BOUNDARY_ACCESS_POLICY_LINEAR_NEXT_BYTE
} hyperdos_x86_segment_boundary_access_policy;

typedef enum hyperdos_x86_push_stack_pointer_policy
{
    HYPERDOS_X86_PUSH_STACK_POINTER_POLICY_PUSH_DECREMENTED_VALUE = 0u,
    HYPERDOS_X86_PUSH_STACK_POINTER_POLICY_PUSH_ORIGINAL_VALUE
} hyperdos_x86_push_stack_pointer_policy;

typedef enum hyperdos_x86_operation_code_validity_action
{
    HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE = 0u,
    HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNSUPPORTED_RESULT,
    HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNUSED_OPERATION_CODE_INTERRUPT,
    HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_POP_CODE_SEGMENT
} hyperdos_x86_operation_code_validity_action;

typedef struct hyperdos_x86_processor_descriptor
{
    hyperdos_x86_architecture_generation        architectureGeneration;
    hyperdos_x86_execution_mode                 executionMode;
    hyperdos_x86_external_bus_profile           externalBusProfile;
    hyperdos_x86_operand_address_profile        operandAddressProfile;
    hyperdos_x86_exception_model                exceptionModel;
    hyperdos_x86_shift_rotate_count_policy      shiftRotateCountPolicy;
    hyperdos_x86_signed_divide_quotient_policy  signedDivideQuotientPolicy;
    hyperdos_x86_segment_boundary_access_policy segmentBoundaryAccessPolicy;
    hyperdos_x86_push_stack_pointer_policy      pushStackPointerPolicy;
} hyperdos_x86_processor_descriptor;

typedef struct hyperdos_x86_operation_code_validity_rule
{
    uint8_t                                     operationCode;
    hyperdos_x86_operation_code_validity_action action8086;
    hyperdos_x86_operation_code_validity_action action80186;
} hyperdos_x86_operation_code_validity_rule;

typedef struct hyperdos_x86_decoded_operand
{
    uint8_t                             isRegister;
    uint8_t                             registerIndex;
    hyperdos_x86_segment_register_index segmentRegister;
    uint16_t                            offset;
} hyperdos_x86_decoded_operand;

// clang-format off
static const hyperdos_x86_processor_descriptor hyperdos_x86_processor_descriptors[] = {
    [HYPERDOS_X86_PROCESSOR_MODEL_8086] = {
        HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
        HYPERDOS_X86_EXECUTION_MODE_REAL,
        HYPERDOS_X86_EXTERNAL_BUS_PROFILE_8086,
        HYPERDOS_X86_OPERAND_ADDRESS_PROFILE_REAL_MODE_16_BIT,
        HYPERDOS_X86_EXCEPTION_MODEL_8086,
        HYPERDOS_X86_SHIFT_ROTATE_COUNT_POLICY_RAW,
        HYPERDOS_X86_SIGNED_DIVIDE_QUOTIENT_POLICY_EXCLUDE_MINIMUM_NEGATIVE,
        HYPERDOS_X86_SEGMENT_BOUNDARY_ACCESS_POLICY_WRAP_OFFSET,
        HYPERDOS_X86_PUSH_STACK_POINTER_POLICY_PUSH_DECREMENTED_VALUE,
    },
    [HYPERDOS_X86_PROCESSOR_MODEL_8088] = {
        HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
        HYPERDOS_X86_EXECUTION_MODE_REAL,
        HYPERDOS_X86_EXTERNAL_BUS_PROFILE_8088,
        HYPERDOS_X86_OPERAND_ADDRESS_PROFILE_REAL_MODE_16_BIT,
        HYPERDOS_X86_EXCEPTION_MODEL_8086,
        HYPERDOS_X86_SHIFT_ROTATE_COUNT_POLICY_RAW,
        HYPERDOS_X86_SIGNED_DIVIDE_QUOTIENT_POLICY_EXCLUDE_MINIMUM_NEGATIVE,
        HYPERDOS_X86_SEGMENT_BOUNDARY_ACCESS_POLICY_WRAP_OFFSET,
        HYPERDOS_X86_PUSH_STACK_POINTER_POLICY_PUSH_DECREMENTED_VALUE,
    },
    [HYPERDOS_X86_PROCESSOR_MODEL_80186] = {
        HYPERDOS_X86_ARCHITECTURE_GENERATION_80186,
        HYPERDOS_X86_EXECUTION_MODE_REAL,
        HYPERDOS_X86_EXTERNAL_BUS_PROFILE_80186,
        HYPERDOS_X86_OPERAND_ADDRESS_PROFILE_REAL_MODE_16_BIT,
        HYPERDOS_X86_EXCEPTION_MODEL_80186,
        HYPERDOS_X86_SHIFT_ROTATE_COUNT_POLICY_MASK_TO_FIVE_BITS,
        HYPERDOS_X86_SIGNED_DIVIDE_QUOTIENT_POLICY_ALLOW_MINIMUM_NEGATIVE,
        HYPERDOS_X86_SEGMENT_BOUNDARY_ACCESS_POLICY_LINEAR_NEXT_BYTE,
        HYPERDOS_X86_PUSH_STACK_POINTER_POLICY_PUSH_ORIGINAL_VALUE,
    },
    [HYPERDOS_X86_PROCESSOR_MODEL_80188] = {
        HYPERDOS_X86_ARCHITECTURE_GENERATION_80186,
        HYPERDOS_X86_EXECUTION_MODE_REAL,
        HYPERDOS_X86_EXTERNAL_BUS_PROFILE_80188,
        HYPERDOS_X86_OPERAND_ADDRESS_PROFILE_REAL_MODE_16_BIT,
        HYPERDOS_X86_EXCEPTION_MODEL_80186,
        HYPERDOS_X86_SHIFT_ROTATE_COUNT_POLICY_MASK_TO_FIVE_BITS,
        HYPERDOS_X86_SIGNED_DIVIDE_QUOTIENT_POLICY_ALLOW_MINIMUM_NEGATIVE,
        HYPERDOS_X86_SEGMENT_BOUNDARY_ACCESS_POLICY_LINEAR_NEXT_BYTE,
        HYPERDOS_X86_PUSH_STACK_POINTER_POLICY_PUSH_ORIGINAL_VALUE,
    },
};
// clang-format on

static const hyperdos_x86_operation_code_validity_rule hyperdos_x86_operation_code_validity_rules[] = {
    {HYPERDOS_X86_OPERATION_CODE_POP_CODE_SEGMENT,
     HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_POP_CODE_SEGMENT,   HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNUSED_OPERATION_CODE_INTERRUPT},
    {0x63u,
     HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNSUPPORTED_RESULT, HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNUSED_OPERATION_CODE_INTERRUPT},
    {0x64u,
     HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNSUPPORTED_RESULT, HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNUSED_OPERATION_CODE_INTERRUPT},
    {0x65u,
     HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNSUPPORTED_RESULT, HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNUSED_OPERATION_CODE_INTERRUPT},
    {0x66u,
     HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNSUPPORTED_RESULT, HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNUSED_OPERATION_CODE_INTERRUPT},
    {0x67u,
     HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNSUPPORTED_RESULT, HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNUSED_OPERATION_CODE_INTERRUPT},
    {0xF1u,
     HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNSUPPORTED_RESULT, HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNUSED_OPERATION_CODE_INTERRUPT},
};

static inline int hyperdos_x86_operation_code_is_in_range(uint8_t operationCode,
                                                          uint8_t firstOperationCode,
                                                          uint8_t lastOperationCode)
{
    return (uint8_t)(operationCode - firstOperationCode) <= (uint8_t)(lastOperationCode - firstOperationCode);
}

static inline uint16_t hyperdos_x86_operand_value_mask(int isWord)
{
    return isWord ? HYPERDOS_X86_WORD_MASK : HYPERDOS_X86_BYTE_MASK;
}

static inline uint16_t hyperdos_x86_operand_sign_bit(int isWord)
{
    return isWord ? HYPERDOS_X86_WORD_SIGN_BIT : HYPERDOS_X86_BYTE_SIGN_BIT;
}

static inline int hyperdos_x86_operation_code_is_string(uint8_t operationCode)
{
    return hyperdos_x86_operation_code_is_in_range(operationCode,
                                                   HYPERDOS_X86_OPERATION_CODE_MOVE_STRING_BYTE,
                                                   HYPERDOS_X86_OPERATION_CODE_COMPARE_STRING_WORD) ||
           hyperdos_x86_operation_code_is_in_range(operationCode,
                                                   HYPERDOS_X86_OPERATION_CODE_STORE_STRING_BYTE,
                                                   HYPERDOS_X86_OPERATION_CODE_SCAN_STRING_WORD);
}

static inline int hyperdos_x86_operation_code_is_input_output_string(uint8_t operationCode)
{
    return hyperdos_x86_operation_code_is_in_range(operationCode,
                                                   HYPERDOS_X86_OPERATION_CODE_INPUT_STRING_BYTE,
                                                   HYPERDOS_X86_OPERATION_CODE_OUTPUT_STRING_WORD);
}

static inline uint16_t hyperdos_x86_normalize_flags(uint16_t flags)
{
    return (uint16_t)((flags & HYPERDOS_X86_FLAGS_WRITABLE_MASK) | HYPERDOS_X86_FLAG_RESERVED);
}

static inline size_t hyperdos_x86_processor_descriptor_count(void)
{
    return sizeof(hyperdos_x86_processor_descriptors) / sizeof(hyperdos_x86_processor_descriptors[0]);
}

static inline const hyperdos_x86_processor_descriptor* hyperdos_x86_get_processor_descriptor(
        const hyperdos_x86_processor* processor)
{
    if (processor == NULL || processor->processorModel >= hyperdos_x86_processor_descriptor_count())
    {
        return &hyperdos_x86_processor_descriptors[HYPERDOS_X86_PROCESSOR_MODEL_8086];
    }
    return &hyperdos_x86_processor_descriptors[processor->processorModel];
}

static inline int hyperdos_x86_supports_80186_instructions(const hyperdos_x86_processor* processor)
{
    const hyperdos_x86_processor_descriptor* processorDescriptor = hyperdos_x86_get_processor_descriptor(processor);
    return processorDescriptor->architectureGeneration >= HYPERDOS_X86_ARCHITECTURE_GENERATION_80186;
}

static hyperdos_x86_operation_code_validity_action hyperdos_x86_find_operation_code_validity_action(
        const hyperdos_x86_processor* processor,
        uint8_t                       operationCode)
{
    const hyperdos_x86_processor_descriptor* processorDescriptor = hyperdos_x86_get_processor_descriptor(processor);
    size_t                                   ruleIndex           = 0u;

    for (ruleIndex = 0u; ruleIndex < sizeof(hyperdos_x86_operation_code_validity_rules) /
                                             sizeof(hyperdos_x86_operation_code_validity_rules[0]);
         ++ruleIndex)
    {
        if (hyperdos_x86_operation_code_validity_rules[ruleIndex].operationCode == operationCode)
        {
            return processorDescriptor->architectureGeneration >= HYPERDOS_X86_ARCHITECTURE_GENERATION_80186
                           ? hyperdos_x86_operation_code_validity_rules[ruleIndex].action80186
                           : hyperdos_x86_operation_code_validity_rules[ruleIndex].action8086;
        }
    }
    return HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE;
}

static inline uint8_t hyperdos_x86_apply_shift_rotate_count_policy(const hyperdos_x86_processor* processor,
                                                                   uint8_t                       count)
{
    const hyperdos_x86_processor_descriptor* processorDescriptor = hyperdos_x86_get_processor_descriptor(processor);
    if (processorDescriptor->shiftRotateCountPolicy == HYPERDOS_X86_SHIFT_ROTATE_COUNT_POLICY_MASK_TO_FIVE_BITS)
    {
        return (uint8_t)(count & 0x1Fu);
    }
    return count;
}

static inline int hyperdos_x86_signed_divide_quotient_is_out_of_range(const hyperdos_x86_processor* processor,
                                                                      int32_t                       quotient,
                                                                      int                           isWord)
{
    const hyperdos_x86_processor_descriptor* processorDescriptor = hyperdos_x86_get_processor_descriptor(processor);
    int32_t                                  minimumQuotient     = isWord ? INT16_MIN : INT8_MIN;
    int32_t                                  maximumQuotient     = isWord ? INT16_MAX : INT8_MAX;

    if (processorDescriptor->signedDivideQuotientPolicy ==
        HYPERDOS_X86_SIGNED_DIVIDE_QUOTIENT_POLICY_EXCLUDE_MINIMUM_NEGATIVE)
    {
        minimumQuotient += 1;
    }
    return quotient < minimumQuotient || quotient > maximumQuotient;
}

static inline int hyperdos_x86_word_access_wraps_at_segment_boundary(const hyperdos_x86_processor* processor,
                                                                     uint16_t                      offset)
{
    const hyperdos_x86_processor_descriptor* processorDescriptor = hyperdos_x86_get_processor_descriptor(processor);
    return offset == HYPERDOS_X86_WORD_MASK &&
           processorDescriptor->segmentBoundaryAccessPolicy == HYPERDOS_X86_SEGMENT_BOUNDARY_ACCESS_POLICY_WRAP_OFFSET;
}

static inline uint32_t hyperdos_x86_physical_address_from_base(uint32_t segmentBase, uint16_t offset)
{
    return (segmentBase + offset) & HYPERDOS_X86_ADDRESS_MASK;
}

static inline void hyperdos_x86_store_segment_base(hyperdos_x86_processor*             processor,
                                                   hyperdos_x86_segment_register_index segmentRegister)
{
    processor->segmentBases[segmentRegister] = ((uint32_t)processor->segmentRegisters[segmentRegister]
                                                << HYPERDOS_X86_SEGMENT_SHIFT) &
                                               HYPERDOS_X86_ADDRESS_MASK;
}

static inline uint16_t hyperdos_x86_read_general_register_word(const hyperdos_x86_processor*       processor,
                                                               hyperdos_x86_general_register_index registerIndex)
{
    return (uint16_t)(processor->generalRegisters[registerIndex] & HYPERDOS_X86_WORD_MASK);
}

static inline void hyperdos_x86_write_general_register_word(hyperdos_x86_processor*             processor,
                                                            hyperdos_x86_general_register_index registerIndex,
                                                            uint16_t                            value)
{
    processor->generalRegisters[registerIndex] = (processor->generalRegisters[registerIndex] & 0xFFFF0000u) | value;
}

static inline uint16_t hyperdos_x86_read_instruction_pointer_word(const hyperdos_x86_processor* processor)
{
    return (uint16_t)(processor->instructionPointer & HYPERDOS_X86_WORD_MASK);
}

static inline void hyperdos_x86_write_instruction_pointer_word(hyperdos_x86_processor* processor, uint16_t value)
{
    processor->instructionPointer = value;
}

static inline uint16_t hyperdos_x86_read_flags_word(const hyperdos_x86_processor* processor)
{
    return (uint16_t)(processor->flags & HYPERDOS_X86_WORD_MASK);
}

static inline void hyperdos_x86_write_flags_word(hyperdos_x86_processor* processor, uint16_t value)
{
    processor->flags = value;
}

static inline uint8_t hyperdos_x86_read_physical_byte(const hyperdos_x86_processor* processor, uint32_t physicalAddress)
{
    if (processor->bus != NULL)
    {
        return hyperdos_bus_read_memory_byte_or_open_bus(processor->bus, physicalAddress & HYPERDOS_X86_ADDRESS_MASK);
    }
    return processor->memory[physicalAddress & HYPERDOS_X86_ADDRESS_MASK];
}

static inline void hyperdos_x86_write_physical_byte(hyperdos_x86_processor* processor,
                                                    uint32_t                physicalAddress,
                                                    uint8_t                 value)
{
    if (processor->bus != NULL)
    {
        hyperdos_bus_write_memory_byte_if_mapped(processor->bus, physicalAddress & HYPERDOS_X86_ADDRESS_MASK, value);
        return;
    }
    processor->memory[physicalAddress & HYPERDOS_X86_ADDRESS_MASK] = value;
}

static inline uint16_t hyperdos_x86_read_physical_word(const hyperdos_x86_processor* processor,
                                                       uint32_t                      physicalAddress)
{
    uint16_t lowByte  = hyperdos_x86_read_physical_byte(processor, physicalAddress);
    uint16_t highByte = hyperdos_x86_read_physical_byte(processor, physicalAddress + 1u);
    return (uint16_t)(lowByte | (highByte << HYPERDOS_X86_BYTE_BIT_COUNT));
}

static inline void hyperdos_x86_write_physical_word(hyperdos_x86_processor* processor,
                                                    uint32_t                physicalAddress,
                                                    uint16_t                value)
{
    hyperdos_x86_write_physical_byte(processor, physicalAddress, (uint8_t)(value & HYPERDOS_X86_BYTE_MASK));
    hyperdos_x86_write_physical_byte(processor, physicalAddress + 1u, (uint8_t)(value >> HYPERDOS_X86_BYTE_BIT_COUNT));
}

static inline uint8_t hyperdos_x86_read_memory_byte_fast(const hyperdos_x86_processor*       processor,
                                                         hyperdos_x86_segment_register_index segmentRegister,
                                                         uint16_t                            offset)
{
    uint32_t physicalAddress = hyperdos_x86_physical_address_from_base(processor->segmentBases[segmentRegister],
                                                                       offset);
    return hyperdos_x86_read_physical_byte(processor, physicalAddress);
}

static inline void hyperdos_x86_write_memory_byte_fast(hyperdos_x86_processor*             processor,
                                                       hyperdos_x86_segment_register_index segmentRegister,
                                                       uint16_t                            offset,
                                                       uint8_t                             value)
{
    uint32_t physicalAddress = hyperdos_x86_physical_address_from_base(processor->segmentBases[segmentRegister],
                                                                       offset);
    hyperdos_x86_write_physical_byte(processor, physicalAddress, value);
}

static inline uint16_t hyperdos_x86_read_memory_word_fast(const hyperdos_x86_processor*       processor,
                                                          hyperdos_x86_segment_register_index segmentRegister,
                                                          uint16_t                            offset)
{
    if (hyperdos_x86_word_access_wraps_at_segment_boundary(processor, offset))
    {
        uint16_t lowByte  = hyperdos_x86_read_memory_byte_fast(processor, segmentRegister, offset);
        uint16_t highByte = hyperdos_x86_read_memory_byte_fast(processor, segmentRegister, 0u);
        return (uint16_t)(lowByte | (highByte << HYPERDOS_X86_BYTE_BIT_COUNT));
    }

    uint32_t physicalAddress = hyperdos_x86_physical_address_from_base(processor->segmentBases[segmentRegister],
                                                                       offset);
    return hyperdos_x86_read_physical_word(processor, physicalAddress);
}

static inline void hyperdos_x86_write_memory_word_fast(hyperdos_x86_processor*             processor,
                                                       hyperdos_x86_segment_register_index segmentRegister,
                                                       uint16_t                            offset,
                                                       uint16_t                            value)
{
    if (hyperdos_x86_word_access_wraps_at_segment_boundary(processor, offset))
    {
        hyperdos_x86_write_memory_byte_fast(processor,
                                            segmentRegister,
                                            offset,
                                            (uint8_t)(value & HYPERDOS_X86_BYTE_MASK));
        hyperdos_x86_write_memory_byte_fast(processor,
                                            segmentRegister,
                                            0u,
                                            (uint8_t)(value >> HYPERDOS_X86_BYTE_BIT_COUNT));
        return;
    }

    uint32_t physicalAddress = hyperdos_x86_physical_address_from_base(processor->segmentBases[segmentRegister],
                                                                       offset);
    hyperdos_x86_write_physical_word(processor, physicalAddress, value);
}

static inline uint8_t hyperdos_x86_fetch_instruction_byte(hyperdos_x86_processor* processor)
{
    uint16_t instructionOffset = hyperdos_x86_read_instruction_pointer_word(processor);
    uint8_t  value             = hyperdos_x86_read_memory_byte_fast(processor,
                                                       HYPERDOS_X86_SEGMENT_REGISTER_CODE,
                                                       instructionOffset);
    hyperdos_x86_write_instruction_pointer_word(processor, (uint16_t)(instructionOffset + 1u));
    return value;
}

static inline uint16_t hyperdos_x86_fetch_instruction_word(hyperdos_x86_processor* processor)
{
    uint16_t instructionOffset = hyperdos_x86_read_instruction_pointer_word(processor);
    uint16_t value             = hyperdos_x86_read_memory_word_fast(processor,
                                                        HYPERDOS_X86_SEGMENT_REGISTER_CODE,
                                                        instructionOffset);
    hyperdos_x86_write_instruction_pointer_word(processor, (uint16_t)(instructionOffset + 2u));
    return value;
}

static inline uint8_t hyperdos_x86_read_byte_register(const hyperdos_x86_processor* processor,
                                                      uint8_t                       byteRegisterIndex)
{
    uint16_t value = hyperdos_x86_read_general_register_word(
            processor,
            (hyperdos_x86_general_register_index)(byteRegisterIndex & HYPERDOS_X86_BYTE_REGISTER_WORD_INDEX_MASK));
    if ((byteRegisterIndex & HYPERDOS_X86_BYTE_REGISTER_HIGH_PART_BIT) != 0u)
    {
        return (uint8_t)(value >> HYPERDOS_X86_BYTE_BIT_COUNT);
    }
    return (uint8_t)(value & HYPERDOS_X86_BYTE_MASK);
}

static inline void hyperdos_x86_write_byte_register(hyperdos_x86_processor* processor,
                                                    uint8_t                 byteRegisterIndex,
                                                    uint8_t                 value)
{
    hyperdos_x86_general_register_index
             registerIndex   = (hyperdos_x86_general_register_index)(byteRegisterIndex &
                                                                  HYPERDOS_X86_BYTE_REGISTER_WORD_INDEX_MASK);
    uint16_t generalRegister = hyperdos_x86_read_general_register_word(processor, registerIndex);
    if ((byteRegisterIndex & HYPERDOS_X86_BYTE_REGISTER_HIGH_PART_BIT) != 0u)
    {
        generalRegister = (uint16_t)((generalRegister & HYPERDOS_X86_BYTE_MASK) |
                                     ((uint16_t)value << HYPERDOS_X86_BYTE_BIT_COUNT));
    }
    else
    {
        generalRegister = (uint16_t)((generalRegister & HYPERDOS_X86_HIGH_BYTE_MASK) | value);
    }
    hyperdos_x86_write_general_register_word(processor, registerIndex, generalRegister);
}

static inline uint16_t hyperdos_x86_sign_extend_byte(uint8_t value)
{
    return (uint16_t)(int16_t)(int8_t)value;
}

static inline int hyperdos_x86_has_even_parity(uint8_t value)
{
    value ^= (uint8_t)(value >> HYPERDOS_X86_PARITY_FOLD_SHIFT);
    value &= HYPERDOS_X86_NIBBLE_MASK;
    return ((HYPERDOS_X86_PARITY_LOOKUP >> value) & 1u) == 0u;
}

static inline void hyperdos_x86_replace_status_flags(hyperdos_x86_processor* processor, uint16_t statusFlags)
{
    const uint16_t statusMask = HYPERDOS_X86_FLAG_CARRY | HYPERDOS_X86_FLAG_PARITY | HYPERDOS_X86_FLAG_AUXILIARY_CARRY |
                                HYPERDOS_X86_FLAG_ZERO | HYPERDOS_X86_FLAG_SIGN | HYPERDOS_X86_FLAG_OVERFLOW;
    processor->flags = (uint16_t)((processor->flags & ~statusMask) | statusFlags | HYPERDOS_X86_FLAG_RESERVED);
}

static inline uint16_t hyperdos_x86_sign_zero_parity_flags(uint16_t value, uint16_t valueMask, uint16_t signBit)
{
    uint16_t statusFlags  = 0;
    value                &= valueMask;

    if (value == 0u)
    {
        statusFlags |= HYPERDOS_X86_FLAG_ZERO;
    }
    if ((value & signBit) != 0u)
    {
        statusFlags |= HYPERDOS_X86_FLAG_SIGN;
    }
    if (hyperdos_x86_has_even_parity((uint8_t)value))
    {
        statusFlags |= HYPERDOS_X86_FLAG_PARITY;
    }
    return statusFlags;
}

static uint16_t hyperdos_x86_execute_arithmetic_logic_operation(hyperdos_x86_processor*                 processor,
                                                                hyperdos_x86_arithmetic_logic_operation operation,
                                                                uint16_t                                leftValue,
                                                                uint16_t                                rightValue,
                                                                uint16_t                                valueMask,
                                                                uint16_t                                signBit)
{
    uint32_t maskedLeftValue  = leftValue & valueMask;
    uint32_t maskedRightValue = rightValue & valueMask;
    uint32_t fullResult       = 0;
    uint16_t result           = 0;
    uint16_t statusFlags      = 0;

    switch (operation)
    {
    case HYPERDOS_X86_ARITHMETIC_LOGIC_ADD:
    case HYPERDOS_X86_ARITHMETIC_LOGIC_ADC:
    {
        uint32_t carryValue = (operation == HYPERDOS_X86_ARITHMETIC_LOGIC_ADC &&
                               (processor->flags & HYPERDOS_X86_FLAG_CARRY) != 0u)
                                      ? 1u
                                      : 0u;
        fullResult          = maskedLeftValue + maskedRightValue + carryValue;
        result              = (uint16_t)(fullResult & valueMask);
        statusFlags         = hyperdos_x86_sign_zero_parity_flags(result, valueMask, signBit);
        if (fullResult > valueMask)
        {
            statusFlags |= HYPERDOS_X86_FLAG_CARRY;
        }
        if (((maskedLeftValue ^ maskedRightValue ^ result) & HYPERDOS_X86_AUXILIARY_CARRY_BIT) != 0u)
        {
            statusFlags |= HYPERDOS_X86_FLAG_AUXILIARY_CARRY;
        }
        if (((~(maskedLeftValue ^ maskedRightValue) & (maskedLeftValue ^ result)) & signBit) != 0u)
        {
            statusFlags |= HYPERDOS_X86_FLAG_OVERFLOW;
        }
        hyperdos_x86_replace_status_flags(processor, statusFlags);
        return result;
    }

    case HYPERDOS_X86_ARITHMETIC_LOGIC_SUB:
    case HYPERDOS_X86_ARITHMETIC_LOGIC_CMP:
    case HYPERDOS_X86_ARITHMETIC_LOGIC_SBB:
    {
        uint32_t borrowValue     = (operation == HYPERDOS_X86_ARITHMETIC_LOGIC_SBB &&
                                (processor->flags & HYPERDOS_X86_FLAG_CARRY) != 0u)
                                           ? 1u
                                           : 0u;
        uint32_t subtrahendValue = maskedRightValue + borrowValue;
        fullResult               = (maskedLeftValue - subtrahendValue) & HYPERDOS_X86_EXTENDED_WORD_MASK;
        result                   = (uint16_t)(fullResult & valueMask);
        statusFlags              = hyperdos_x86_sign_zero_parity_flags(result, valueMask, signBit);
        if (maskedLeftValue < subtrahendValue)
        {
            statusFlags |= HYPERDOS_X86_FLAG_CARRY;
        }
        if (((maskedLeftValue ^ maskedRightValue ^ result) & HYPERDOS_X86_AUXILIARY_CARRY_BIT) != 0u)
        {
            statusFlags |= HYPERDOS_X86_FLAG_AUXILIARY_CARRY;
        }
        if ((((maskedLeftValue ^ maskedRightValue) & (maskedLeftValue ^ result)) & signBit) != 0u)
        {
            statusFlags |= HYPERDOS_X86_FLAG_OVERFLOW;
        }
        hyperdos_x86_replace_status_flags(processor, statusFlags);
        return result;
    }

    case HYPERDOS_X86_ARITHMETIC_LOGIC_AND:
        result = (uint16_t)((maskedLeftValue & maskedRightValue) & valueMask);
        hyperdos_x86_replace_status_flags(processor, hyperdos_x86_sign_zero_parity_flags(result, valueMask, signBit));
        return result;

    case HYPERDOS_X86_ARITHMETIC_LOGIC_OR:
        result = (uint16_t)((maskedLeftValue | maskedRightValue) & valueMask);
        hyperdos_x86_replace_status_flags(processor, hyperdos_x86_sign_zero_parity_flags(result, valueMask, signBit));
        return result;

    case HYPERDOS_X86_ARITHMETIC_LOGIC_XOR:
        result = (uint16_t)((maskedLeftValue ^ maskedRightValue) & valueMask);
        hyperdos_x86_replace_status_flags(processor, hyperdos_x86_sign_zero_parity_flags(result, valueMask, signBit));
        return result;
    }

    return 0;
}

static void hyperdos_x86_update_increment_decrement_flags(hyperdos_x86_processor*                 processor,
                                                          hyperdos_x86_arithmetic_logic_operation operation,
                                                          uint16_t                                value,
                                                          uint16_t                                valueMask,
                                                          uint16_t                                signBit)
{
    uint16_t carryFlag = (uint16_t)(processor->flags & HYPERDOS_X86_FLAG_CARRY);
    (void)hyperdos_x86_execute_arithmetic_logic_operation(processor, operation, value, 1u, valueMask, signBit);
    processor->flags = (uint16_t)((processor->flags & ~HYPERDOS_X86_FLAG_CARRY) | carryFlag |
                                  HYPERDOS_X86_FLAG_RESERVED);
}

static hyperdos_x86_decoded_operand hyperdos_x86_decode_register_memory_operand(
        hyperdos_x86_processor*             processor,
        uint8_t                             registerMemoryByte,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride)
{
    hyperdos_x86_decoded_operand operand;
    uint8_t                      addressMode = (uint8_t)(registerMemoryByte >> HYPERDOS_X86_MODRM_MODE_SHIFT);
    uint8_t  registerMemoryIndex             = (uint8_t)(registerMemoryByte & HYPERDOS_X86_MODRM_REGISTER_MEMORY_MASK);
    uint16_t baseOffset                      = 0;
    int16_t  displacement                    = 0;

    operand.isRegister      = (uint8_t)(addressMode == HYPERDOS_X86_MODRM_MODE_REGISTER);
    operand.registerIndex   = registerMemoryIndex;
    operand.segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_DATA;
    operand.offset          = 0;

    if (operand.isRegister)
    {
        return operand;
    }

    switch (registerMemoryIndex)
    {
    case 0:
        baseOffset = (uint16_t)(hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE) +
                                hyperdos_x86_read_general_register_word(processor,
                                                                        HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX));
        operand.segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_DATA;
        break;
    case 1:
        baseOffset =
                (uint16_t)(hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE) +
                           hyperdos_x86_read_general_register_word(processor,
                                                                   HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX));
        operand.segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_DATA;
        break;
    case 2:
        baseOffset              = (uint16_t)(hyperdos_x86_read_general_register_word(processor,
                                                                        HYPERDOS_X86_GENERAL_REGISTER_BASE_POINTER) +
                                hyperdos_x86_read_general_register_word(processor,
                                                                        HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX));
        operand.segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_STACK;
        break;
    case 3:
        baseOffset =
                (uint16_t)(hyperdos_x86_read_general_register_word(processor,
                                                                   HYPERDOS_X86_GENERAL_REGISTER_BASE_POINTER) +
                           hyperdos_x86_read_general_register_word(processor,
                                                                   HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX));
        operand.segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_STACK;
        break;
    case 4:
        baseOffset = hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX);
        operand.segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_DATA;
        break;
    case 5:
        baseOffset              = hyperdos_x86_read_general_register_word(processor,
                                                             HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX);
        operand.segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_DATA;
        break;
    case 6:
        if (addressMode == HYPERDOS_X86_MODRM_MODE_MEMORY_NO_DISPLACEMENT)
        {
            operand.offset          = hyperdos_x86_fetch_instruction_word(processor);
            operand.segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_DATA;
            if (hasSegmentOverride)
            {
                operand.segmentRegister = segmentOverride;
            }
            return operand;
        }
        baseOffset = hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE_POINTER);
        operand.segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_STACK;
        break;
    case 7:
        baseOffset = hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE);
        operand.segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_DATA;
        break;
    }

    if (addressMode == HYPERDOS_X86_MODRM_MODE_MEMORY_BYTE_DISPLACEMENT)
    {
        displacement = (int16_t)(int8_t)hyperdos_x86_fetch_instruction_byte(processor);
    }
    else if (addressMode == HYPERDOS_X86_MODRM_MODE_MEMORY_WORD_DISPLACEMENT)
    {
        displacement = (int16_t)hyperdos_x86_fetch_instruction_word(processor);
    }

    operand.offset = (uint16_t)(baseOffset + displacement);
    if (hasSegmentOverride)
    {
        operand.segmentRegister = segmentOverride;
    }
    return operand;
}

static uint16_t hyperdos_x86_read_operand_value(const hyperdos_x86_processor*       processor,
                                                const hyperdos_x86_decoded_operand* operand,
                                                int                                 isWord)
{
    if (operand->isRegister)
    {
        if (isWord)
        {
            return hyperdos_x86_read_general_register_word(processor,
                                                           (hyperdos_x86_general_register_index)operand->registerIndex);
        }
        return hyperdos_x86_read_byte_register(processor, operand->registerIndex);
    }

    if (isWord)
    {
        return hyperdos_x86_read_memory_word_fast(processor, operand->segmentRegister, operand->offset);
    }
    return hyperdos_x86_read_memory_byte_fast(processor, operand->segmentRegister, operand->offset);
}

static void hyperdos_x86_write_operand_value(hyperdos_x86_processor*             processor,
                                             const hyperdos_x86_decoded_operand* operand,
                                             int                                 isWord,
                                             uint16_t                            value)
{
    if (operand->isRegister)
    {
        if (isWord)
        {
            hyperdos_x86_write_general_register_word(processor,
                                                     (hyperdos_x86_general_register_index)operand->registerIndex,
                                                     value);
        }
        else
        {
            hyperdos_x86_write_byte_register(processor, operand->registerIndex, (uint8_t)value);
        }
        return;
    }

    if (isWord)
    {
        hyperdos_x86_write_memory_word_fast(processor, operand->segmentRegister, operand->offset, value);
    }
    else
    {
        hyperdos_x86_write_memory_byte_fast(processor, operand->segmentRegister, operand->offset, (uint8_t)value);
    }
}

static inline uint16_t hyperdos_x86_get_stack_pointer_push_value(const hyperdos_x86_processor* processor,
                                                                 uint16_t                      stackPointer)
{
    const hyperdos_x86_processor_descriptor* processorDescriptor = hyperdos_x86_get_processor_descriptor(processor);
    if (processorDescriptor->pushStackPointerPolicy == HYPERDOS_X86_PUSH_STACK_POINTER_POLICY_PUSH_DECREMENTED_VALUE)
    {
        return (uint16_t)(stackPointer - HYPERDOS_X86_WORD_SIZE);
    }
    return stackPointer;
}

static inline uint16_t hyperdos_x86_adjust_push_source_word(const hyperdos_x86_processor*       processor,
                                                            const hyperdos_x86_decoded_operand* operand,
                                                            uint16_t                            value)
{
    if (operand->isRegister && operand->registerIndex == HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER)
    {
        return hyperdos_x86_get_stack_pointer_push_value(processor, value);
    }
    return value;
}

static inline void hyperdos_x86_push_word(hyperdos_x86_processor* processor, uint16_t value)
{
    uint16_t stackPointer =
            (uint16_t)(hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER) -
                       HYPERDOS_X86_WORD_SIZE);
    hyperdos_x86_write_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER, stackPointer);
    hyperdos_x86_write_memory_word_fast(processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, stackPointer, value);
}

static inline uint16_t hyperdos_x86_pop_word(hyperdos_x86_processor* processor)
{
    uint16_t stackPointer = hyperdos_x86_read_general_register_word(processor,
                                                                    HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    uint16_t value = hyperdos_x86_read_memory_word_fast(processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, stackPointer);
    hyperdos_x86_write_general_register_word(processor,
                                             HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                             (uint16_t)(stackPointer + HYPERDOS_X86_WORD_SIZE));
    return value;
}

static void hyperdos_x86_execute_push_all_instruction(hyperdos_x86_processor* processor)
{
    uint16_t
            originalStackPointer = hyperdos_x86_read_general_register_word(processor,
                                                                           HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);

    hyperdos_x86_push_word(processor,
                           hyperdos_x86_read_general_register_word(processor,
                                                                   HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR));
    hyperdos_x86_push_word(processor,
                           hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER));
    hyperdos_x86_push_word(processor,
                           hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA));
    hyperdos_x86_push_word(processor,
                           hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE));
    hyperdos_x86_push_word(processor, originalStackPointer);
    hyperdos_x86_push_word(processor,
                           hyperdos_x86_read_general_register_word(processor,
                                                                   HYPERDOS_X86_GENERAL_REGISTER_BASE_POINTER));
    hyperdos_x86_push_word(processor,
                           hyperdos_x86_read_general_register_word(processor,
                                                                   HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX));
    hyperdos_x86_push_word(processor,
                           hyperdos_x86_read_general_register_word(processor,
                                                                   HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX));
}

static void hyperdos_x86_execute_pop_all_instruction(hyperdos_x86_processor* processor)
{
    hyperdos_x86_write_general_register_word(processor,
                                             HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX,
                                             hyperdos_x86_pop_word(processor));
    hyperdos_x86_write_general_register_word(processor,
                                             HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX,
                                             hyperdos_x86_pop_word(processor));
    hyperdos_x86_write_general_register_word(processor,
                                             HYPERDOS_X86_GENERAL_REGISTER_BASE_POINTER,
                                             hyperdos_x86_pop_word(processor));
    (void)hyperdos_x86_pop_word(processor);
    hyperdos_x86_write_general_register_word(processor,
                                             HYPERDOS_X86_GENERAL_REGISTER_BASE,
                                             hyperdos_x86_pop_word(processor));
    hyperdos_x86_write_general_register_word(processor,
                                             HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                             hyperdos_x86_pop_word(processor));
    hyperdos_x86_write_general_register_word(processor,
                                             HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
                                             hyperdos_x86_pop_word(processor));
    hyperdos_x86_write_general_register_word(processor,
                                             HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                             hyperdos_x86_pop_word(processor));
}

static void hyperdos_x86_execute_enter_instruction(hyperdos_x86_processor* processor)
{
    uint16_t localStorageByteCount = hyperdos_x86_fetch_instruction_word(processor);
    uint8_t  nestingLevel          = (uint8_t)(hyperdos_x86_fetch_instruction_byte(processor) &
                                     HYPERDOS_X86_ENTER_NESTING_LEVEL_MASK);
    uint16_t framePointer          = hyperdos_x86_read_general_register_word(processor,
                                                                    HYPERDOS_X86_GENERAL_REGISTER_BASE_POINTER);
    uint8_t  nestingIndex          = 0u;

    hyperdos_x86_push_word(processor, framePointer);
    framePointer = hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    if (nestingLevel != 0u)
    {
        for (nestingIndex = 1u; nestingIndex < nestingLevel; ++nestingIndex)
        {
            uint16_t previousFramePointer =
                    (uint16_t)(hyperdos_x86_read_general_register_word(processor,
                                                                       HYPERDOS_X86_GENERAL_REGISTER_BASE_POINTER) -
                               HYPERDOS_X86_WORD_SIZE);
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_BASE_POINTER,
                                                     previousFramePointer);
            hyperdos_x86_push_word(processor,
                                   hyperdos_x86_read_memory_word_fast(processor,
                                                                      HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                                                      previousFramePointer));
        }
        hyperdos_x86_push_word(processor, framePointer);
    }

    hyperdos_x86_write_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE_POINTER, framePointer);
    hyperdos_x86_write_general_register_word(
            processor,
            HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
            (uint16_t)(hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER) -
                       localStorageByteCount));
}

static hyperdos_x86_execution_result hyperdos_x86_execute_immediate_signed_multiply_instruction(
        hyperdos_x86_processor*             processor,
        uint8_t                             operationCode,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride)
{
    uint8_t registerMemoryByte           = hyperdos_x86_fetch_instruction_byte(processor);
    uint8_t destinationRegisterIndex     = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                                 HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
    hyperdos_x86_decoded_operand operand = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                       registerMemoryByte,
                                                                                       hasSegmentOverride,
                                                                                       segmentOverride);
    int32_t sourceValue                  = (int32_t)(int16_t)hyperdos_x86_read_operand_value(processor, &operand, 1);
    int32_t immediateValue               = operationCode == HYPERDOS_X86_OPERATION_CODE_SIGNED_MULTIPLY_IMMEDIATE_WORD
                                                   ? (int32_t)(int16_t)hyperdos_x86_fetch_instruction_word(processor)
                                                   : (int32_t)(int8_t)hyperdos_x86_fetch_instruction_byte(processor);
    int32_t fullResult                   = sourceValue * immediateValue;

    hyperdos_x86_write_general_register_word(processor,
                                             (hyperdos_x86_general_register_index)destinationRegisterIndex,
                                             (uint16_t)fullResult);
    if (fullResult < INT16_MIN || fullResult > INT16_MAX)
    {
        processor->flags |= HYPERDOS_X86_FLAG_CARRY | HYPERDOS_X86_FLAG_OVERFLOW;
    }
    else
    {
        processor->flags &= (uint16_t)~(HYPERDOS_X86_FLAG_CARRY | HYPERDOS_X86_FLAG_OVERFLOW);
    }
    processor->flags |= HYPERDOS_X86_FLAG_RESERVED;
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_interrupt_with_return_address(
        hyperdos_x86_processor* processor,
        uint8_t                 interruptNumber,
        uint16_t                returnSegment,
        uint16_t                returnOffset);

static hyperdos_x86_execution_result hyperdos_x86_execute_unused_operation_code_interrupt(
        hyperdos_x86_processor* processor)
{
    return hyperdos_x86_execute_interrupt_with_return_address(processor,
                                                              HYPERDOS_X86_INTERRUPT_TYPE_UNUSED_OPERATION_CODE,
                                                              processor->lastInstructionSegment,
                                                              (uint16_t)processor->lastInstructionOffset);
}

static hyperdos_x86_execution_result hyperdos_x86_execute_escape_operation_code_interrupt(
        hyperdos_x86_processor* processor)
{
    return hyperdos_x86_execute_interrupt_with_return_address(processor,
                                                              HYPERDOS_X86_INTERRUPT_TYPE_ESCAPE_OPERATION_CODE,
                                                              processor->lastInstructionSegment,
                                                              (uint16_t)processor->lastInstructionOffset);
}

static int hyperdos_x86_execute_operation_code_validity_action(hyperdos_x86_processor*        processor,
                                                               uint8_t                        operationCode,
                                                               hyperdos_x86_execution_result* result)
{
    hyperdos_x86_operation_code_validity_action
            action = hyperdos_x86_find_operation_code_validity_action(processor, operationCode);

    switch (action)
    {
    case HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE:
        return 0;
    case HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNSUPPORTED_RESULT:
        *result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
        return 1;
    case HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNUSED_OPERATION_CODE_INTERRUPT:
        *result = hyperdos_x86_execute_unused_operation_code_interrupt(processor);
        return 1;
    case HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_POP_CODE_SEGMENT:
        hyperdos_x86_set_segment_register(processor,
                                          HYPERDOS_X86_SEGMENT_REGISTER_CODE,
                                          hyperdos_x86_pop_word(processor));
        *result = HYPERDOS_X86_EXECUTION_OK;
        return 1;
    }
    return 0;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_bound_instruction(
        hyperdos_x86_processor*             processor,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride)
{
    uint8_t                      registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
    uint8_t                      registerIndex = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                      HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
    hyperdos_x86_decoded_operand operand       = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                       registerMemoryByte,
                                                                                       hasSegmentOverride,
                                                                                       segmentOverride);
    int16_t                      value         = (int16_t)
            hyperdos_x86_read_general_register_word(processor, (hyperdos_x86_general_register_index)registerIndex);
    int16_t  lowerBound  = 0;
    int16_t  upperBound  = 0;
    uint16_t upperOffset = 0u;

    if (operand.isRegister)
    {
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }

    upperOffset = (uint16_t)(operand.offset + HYPERDOS_X86_WORD_SIZE);
    lowerBound  = (int16_t)hyperdos_x86_read_memory_word_fast(processor, operand.segmentRegister, operand.offset);
    upperBound  = (int16_t)hyperdos_x86_read_memory_word_fast(processor, operand.segmentRegister, upperOffset);
    if (value < lowerBound || value > upperBound)
    {
        return hyperdos_x86_execute_interrupt_with_return_address(processor,
                                                                  HYPERDOS_X86_INTERRUPT_TYPE_BOUND_RANGE_EXCEEDED,
                                                                  processor->lastInstructionSegment,
                                                                  (uint16_t)processor->lastInstructionOffset);
    }
    return HYPERDOS_X86_EXECUTION_OK;
}

static inline void hyperdos_x86_jump_relative(hyperdos_x86_processor* processor, int16_t displacement)
{
    hyperdos_x86_write_instruction_pointer_word(processor,
                                                (uint16_t)(hyperdos_x86_read_instruction_pointer_word(processor) +
                                                           displacement));
}

static int hyperdos_x86_evaluate_condition(const hyperdos_x86_processor* processor, uint8_t conditionCode)
{
    int overflow = (processor->flags & HYPERDOS_X86_FLAG_OVERFLOW) != 0u;
    int carry    = (processor->flags & HYPERDOS_X86_FLAG_CARRY) != 0u;
    int zero     = (processor->flags & HYPERDOS_X86_FLAG_ZERO) != 0u;
    int sign     = (processor->flags & HYPERDOS_X86_FLAG_SIGN) != 0u;
    int parity   = (processor->flags & HYPERDOS_X86_FLAG_PARITY) != 0u;

    switch (conditionCode & HYPERDOS_X86_NIBBLE_MASK)
    {
    case HYPERDOS_X86_CONDITION_OVERFLOW:
        return overflow;
    case HYPERDOS_X86_CONDITION_NOT_OVERFLOW:
        return !overflow;
    case HYPERDOS_X86_CONDITION_CARRY:
        return carry;
    case HYPERDOS_X86_CONDITION_NOT_CARRY:
        return !carry;
    case HYPERDOS_X86_CONDITION_ZERO:
        return zero;
    case HYPERDOS_X86_CONDITION_NOT_ZERO:
        return !zero;
    case HYPERDOS_X86_CONDITION_BELOW_OR_EQUAL:
        return carry || zero;
    case HYPERDOS_X86_CONDITION_ABOVE:
        return !carry && !zero;
    case HYPERDOS_X86_CONDITION_SIGN:
        return sign;
    case HYPERDOS_X86_CONDITION_NOT_SIGN:
        return !sign;
    case HYPERDOS_X86_CONDITION_PARITY:
        return parity;
    case HYPERDOS_X86_CONDITION_NOT_PARITY:
        return !parity;
    case HYPERDOS_X86_CONDITION_LESS:
        return sign != overflow;
    case HYPERDOS_X86_CONDITION_GREATER_OR_EQUAL:
        return sign == overflow;
    case HYPERDOS_X86_CONDITION_LESS_OR_EQUAL:
        return zero || (sign != overflow);
    case HYPERDOS_X86_CONDITION_GREATER:
        return !zero && (sign == overflow);
    }
    return 0;
}

static int hyperdos_x86_can_use_contiguous_physical_range(const hyperdos_x86_processor*       processor,
                                                          hyperdos_x86_segment_register_index segmentRegister,
                                                          uint16_t                            offset,
                                                          size_t                              byteCount,
                                                          uint32_t*                           physicalAddress)
{
    *physicalAddress = hyperdos_x86_physical_address_from_base(processor->segmentBases[segmentRegister], offset);
    if (byteCount == 0u)
    {
        return 1;
    }
    if (byteCount > (size_t)(HYPERDOS_X86_SEGMENT_BYTE_COUNT - offset))
    {
        return 0;
    }
    if (*physicalAddress > HYPERDOS_X86_ADDRESS_MASK)
    {
        return 0;
    }
    if (byteCount > (size_t)(HYPERDOS_X86_MEMORY_SIZE - *physicalAddress))
    {
        return 0;
    }
    return 1;
}

static inline int hyperdos_x86_ranges_do_not_overlap(uint32_t firstAddress, uint32_t secondAddress, size_t byteCount)
{
    return firstAddress + byteCount <= secondAddress || secondAddress + byteCount <= firstAddress;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_interrupt_with_return_address(
        hyperdos_x86_processor* processor,
        uint8_t                 interruptNumber,
        uint16_t                returnSegment,
        uint16_t                returnOffset)
{
    if (processor->interruptHandler != NULL)
    {
        hyperdos_x86_execution_result handledResult = processor->interruptHandler(processor,
                                                                                  interruptNumber,
                                                                                  processor->userContext);
        if (handledResult != HYPERDOS_X86_EXECUTION_INTERRUPT_NOT_HANDLED)
        {
            return handledResult;
        }
    }

    {
        uint32_t vectorAddress = (uint32_t)interruptNumber * HYPERDOS_X86_INTERRUPT_VECTOR_BYTE_COUNT;
        uint16_t vectorOffset  = hyperdos_x86_read_physical_word(processor, vectorAddress);
        uint16_t vectorSegment = hyperdos_x86_read_physical_word(processor,
                                                                 vectorAddress +
                                                                         HYPERDOS_X86_FAR_POINTER_SEGMENT_OFFSET);
        if (vectorOffset == 0u && vectorSegment == 0u)
        {
            return HYPERDOS_X86_EXECUTION_INTERRUPT_NOT_HANDLED;
        }

        hyperdos_x86_push_word(processor, hyperdos_x86_read_flags_word(processor));
        hyperdos_x86_push_word(processor, returnSegment);
        hyperdos_x86_push_word(processor, returnOffset);
        hyperdos_x86_write_flags_word(processor,
                                      (uint16_t)((hyperdos_x86_read_flags_word(processor) &
                                                  ~(HYPERDOS_X86_FLAG_TRAP | HYPERDOS_X86_FLAG_INTERRUPT_ENABLE)) |
                                                 HYPERDOS_X86_FLAG_RESERVED));
        hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE, vectorSegment);
        hyperdos_x86_write_instruction_pointer_word(processor, vectorOffset);
    }

    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_interrupt(hyperdos_x86_processor* processor,
                                                                    uint8_t                 interruptNumber)
{
    return hyperdos_x86_execute_interrupt_with_return_address(processor,
                                                              interruptNumber,
                                                              processor->segmentRegisters
                                                                      [HYPERDOS_X86_SEGMENT_REGISTER_CODE],
                                                              hyperdos_x86_read_instruction_pointer_word(processor));
}

static hyperdos_x86_execution_result hyperdos_x86_execute_arithmetic_logic_instruction(
        hyperdos_x86_processor*             processor,
        uint8_t                             operationCode,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride)
{
    uint8_t instructionForm = (uint8_t)(operationCode & HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
    hyperdos_x86_arithmetic_logic_operation
            operation = (hyperdos_x86_arithmetic_logic_operation)((operationCode >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                                                  HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
    int     isWord    = (instructionForm & HYPERDOS_X86_ARITHMETIC_FORM_WORD_BIT) != 0u;
    uint16_t valueMask = hyperdos_x86_operand_value_mask(isWord);
    uint16_t signBit   = hyperdos_x86_operand_sign_bit(isWord);

    if (instructionForm <= HYPERDOS_X86_ARITHMETIC_FORM_REGISTER_MEMORY_MAXIMUM)
    {
        uint8_t registerMemoryByte              = hyperdos_x86_fetch_instruction_byte(processor);
        uint8_t registerIndex                   = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                          HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
        hyperdos_x86_decoded_operand operand    = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                           registerMemoryByte,
                                                                                           hasSegmentOverride,
                                                                                           segmentOverride);
        uint16_t                     leftValue  = 0;
        uint16_t                     rightValue = 0;
        uint16_t                     result     = 0;

        if ((instructionForm & HYPERDOS_X86_ARITHMETIC_FORM_REGISTER_DESTINATION_BIT) != 0u)
        {
            leftValue  = isWord ? hyperdos_x86_read_general_register_word(processor,
                                                                         (hyperdos_x86_general_register_index)
                                                                                 registerIndex)
                                : hyperdos_x86_read_byte_register(processor, registerIndex);
            rightValue = hyperdos_x86_read_operand_value(processor, &operand, isWord);
            result     = hyperdos_x86_execute_arithmetic_logic_operation(processor,
                                                                     operation,
                                                                     leftValue,
                                                                     rightValue,
                                                                     valueMask,
                                                                     signBit);
            if (operation != HYPERDOS_X86_ARITHMETIC_LOGIC_CMP)
            {
                if (isWord)
                {
                    hyperdos_x86_write_general_register_word(processor,
                                                             (hyperdos_x86_general_register_index)registerIndex,
                                                             result);
                }
                else
                {
                    hyperdos_x86_write_byte_register(processor, registerIndex, (uint8_t)result);
                }
            }
        }
        else
        {
            leftValue  = hyperdos_x86_read_operand_value(processor, &operand, isWord);
            rightValue = isWord ? hyperdos_x86_read_general_register_word(processor,
                                                                          (hyperdos_x86_general_register_index)
                                                                                  registerIndex)
                                : hyperdos_x86_read_byte_register(processor, registerIndex);
            result     = hyperdos_x86_execute_arithmetic_logic_operation(processor,
                                                                     operation,
                                                                     leftValue,
                                                                     rightValue,
                                                                     valueMask,
                                                                     signBit);
            if (operation != HYPERDOS_X86_ARITHMETIC_LOGIC_CMP)
            {
                hyperdos_x86_write_operand_value(processor, &operand, isWord, result);
            }
        }
        return HYPERDOS_X86_EXECUTION_OK;
    }

    if (instructionForm == HYPERDOS_X86_ARITHMETIC_FORM_ACCUMULATOR_BYTE)
    {
        uint16_t leftValue  = hyperdos_x86_read_byte_register(processor, 0u);
        uint16_t rightValue = hyperdos_x86_fetch_instruction_byte(processor);
        uint16_t result     = hyperdos_x86_execute_arithmetic_logic_operation(processor,
                                                                          operation,
                                                                          leftValue,
                                                                          rightValue,
                                                                          HYPERDOS_X86_BYTE_MASK,
                                                                          HYPERDOS_X86_BYTE_SIGN_BIT);
        if (operation != HYPERDOS_X86_ARITHMETIC_LOGIC_CMP)
        {
            hyperdos_x86_write_byte_register(processor, 0u, (uint8_t)result);
        }
        return HYPERDOS_X86_EXECUTION_OK;
    }

    if (instructionForm == HYPERDOS_X86_ARITHMETIC_FORM_ACCUMULATOR_WORD)
    {
        uint16_t leftValue  = hyperdos_x86_read_general_register_word(processor,
                                                                     HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
        uint16_t rightValue = hyperdos_x86_fetch_instruction_word(processor);
        uint16_t result     = hyperdos_x86_execute_arithmetic_logic_operation(processor,
                                                                          operation,
                                                                          leftValue,
                                                                          rightValue,
                                                                          HYPERDOS_X86_WORD_MASK,
                                                                          HYPERDOS_X86_WORD_SIGN_BIT);
        if (operation != HYPERDOS_X86_ARITHMETIC_LOGIC_CMP)
        {
            hyperdos_x86_write_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, result);
        }
        return HYPERDOS_X86_EXECUTION_OK;
    }

    return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_group_one_instruction(
        hyperdos_x86_processor*             processor,
        uint8_t                             operationCode,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride)
{
    uint8_t registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
    hyperdos_x86_arithmetic_logic_operation
            operation = (hyperdos_x86_arithmetic_logic_operation)((registerMemoryByte >>
                                                                   HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                                                  HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
    int     isWord    = operationCode != HYPERDOS_X86_OPERATION_CODE_GROUP_ONE_BYTE_IMMEDIATE &&
                 operationCode != HYPERDOS_X86_OPERATION_CODE_GROUP_ONE_BYTE_IMMEDIATE_ALIAS;
    uint16_t                     valueMask  = hyperdos_x86_operand_value_mask(isWord);
    uint16_t                     signBit    = hyperdos_x86_operand_sign_bit(isWord);
    hyperdos_x86_decoded_operand operand    = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                       registerMemoryByte,
                                                                                       hasSegmentOverride,
                                                                                       segmentOverride);
    uint16_t                     leftValue  = hyperdos_x86_read_operand_value(processor, &operand, isWord);
    uint16_t                     rightValue = 0;
    uint16_t                     result     = 0;

    if (operationCode == HYPERDOS_X86_OPERATION_CODE_GROUP_ONE_SIGN_EXTENDED_IMMEDIATE)
    {
        rightValue = hyperdos_x86_sign_extend_byte(hyperdos_x86_fetch_instruction_byte(processor));
    }
    else if (isWord)
    {
        rightValue = hyperdos_x86_fetch_instruction_word(processor);
    }
    else
    {
        rightValue = hyperdos_x86_fetch_instruction_byte(processor);
    }

    result = hyperdos_x86_execute_arithmetic_logic_operation(processor,
                                                             operation,
                                                             leftValue,
                                                             rightValue,
                                                             valueMask,
                                                             signBit);
    if (operation != HYPERDOS_X86_ARITHMETIC_LOGIC_CMP)
    {
        hyperdos_x86_write_operand_value(processor, &operand, isWord, result);
    }
    return HYPERDOS_X86_EXECUTION_OK;
}

static void hyperdos_x86_test_values(hyperdos_x86_processor* processor,
                                     uint16_t                leftValue,
                                     uint16_t                rightValue,
                                     uint16_t                valueMask,
                                     uint16_t                signBit)
{
    uint16_t result = (uint16_t)((leftValue & rightValue) & valueMask);
    hyperdos_x86_replace_status_flags(processor, hyperdos_x86_sign_zero_parity_flags(result, valueMask, signBit));
}

static hyperdos_x86_execution_result hyperdos_x86_execute_move_instruction(
        hyperdos_x86_processor*             processor,
        uint8_t                             operationCode,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride)
{
    uint8_t                      registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
    uint8_t                      registerIndex = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                      HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
    int                          isWord        = (operationCode & HYPERDOS_X86_OPERATION_CODE_WORD_BIT) != 0u;
    hyperdos_x86_decoded_operand operand       = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                       registerMemoryByte,
                                                                                       hasSegmentOverride,
                                                                                       segmentOverride);

    if ((operationCode & HYPERDOS_X86_ARITHMETIC_FORM_REGISTER_DESTINATION_BIT) != 0u)
    {
        uint16_t value = hyperdos_x86_read_operand_value(processor, &operand, isWord);
        if (isWord)
        {
            hyperdos_x86_write_general_register_word(processor,
                                                     (hyperdos_x86_general_register_index)registerIndex,
                                                     value);
        }
        else
        {
            hyperdos_x86_write_byte_register(processor, registerIndex, (uint8_t)value);
        }
    }
    else
    {
        uint16_t value = isWord ? hyperdos_x86_read_general_register_word(processor,
                                                                          (hyperdos_x86_general_register_index)
                                                                                  registerIndex)
                                : hyperdos_x86_read_byte_register(processor, registerIndex);
        hyperdos_x86_write_operand_value(processor, &operand, isWord, value);
    }
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_lea_instruction(
        hyperdos_x86_processor*             processor,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride)
{
    uint8_t                      registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
    uint8_t                      registerIndex = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                      HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
    hyperdos_x86_decoded_operand operand       = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                       registerMemoryByte,
                                                                                       hasSegmentOverride,
                                                                                       segmentOverride);
    if (operand.isRegister)
    {
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }
    hyperdos_x86_write_general_register_word(processor,
                                             (hyperdos_x86_general_register_index)registerIndex,
                                             operand.offset);
    return HYPERDOS_X86_EXECUTION_OK;
}

static inline uint16_t hyperdos_x86_read_input_output_word(hyperdos_x86_processor* processor, uint16_t port)
{
    uint16_t lowByte  = hyperdos_x86_read_input_output_byte(processor, port);
    uint16_t highByte = hyperdos_x86_read_input_output_byte(processor, (uint16_t)(port + HYPERDOS_X86_BYTE_SIZE));
    return (uint16_t)(lowByte | (highByte << HYPERDOS_X86_BYTE_BIT_COUNT));
}

static inline void hyperdos_x86_write_input_output_word(hyperdos_x86_processor* processor,
                                                        uint16_t                port,
                                                        uint16_t                value)
{
    hyperdos_x86_write_input_output_byte(processor, port, (uint8_t)(value & HYPERDOS_X86_BYTE_MASK));
    hyperdos_x86_write_input_output_byte(processor,
                                         (uint16_t)(port + HYPERDOS_X86_BYTE_SIZE),
                                         (uint8_t)(value >> HYPERDOS_X86_BYTE_BIT_COUNT));
}

static hyperdos_x86_execution_result hyperdos_x86_execute_input_output_string_instruction(
        hyperdos_x86_processor*             processor,
        uint8_t                             operationCode,
        hyperdos_x86_repeat_mode            repeatMode,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride)
{
    int      isWord        = (operationCode & HYPERDOS_X86_OPERATION_CODE_WORD_BIT) != 0u;
    uint16_t elementSize   = isWord ? HYPERDOS_X86_WORD_SIZE : HYPERDOS_X86_BYTE_SIZE;
    int16_t  directionStep = (processor->flags & HYPERDOS_X86_FLAG_DIRECTION) != 0u ? (int16_t)-elementSize
                                                                                    : (int16_t)elementSize;
    uint16_t repeatCount   = repeatMode == HYPERDOS_X86_REPEAT_NONE
                                     ? 1u
                                     : hyperdos_x86_read_general_register_word(processor,
                                                                             HYPERDOS_X86_GENERAL_REGISTER_COUNTER);
    uint16_t port          = hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA);
    hyperdos_x86_segment_register_index sourceSegment = hasSegmentOverride ? segmentOverride
                                                                           : HYPERDOS_X86_SEGMENT_REGISTER_DATA;

    if (repeatCount == 0u)
    {
        return HYPERDOS_X86_EXECUTION_OK;
    }

    while (repeatCount > 0u)
    {
        switch (operationCode)
        {
        case HYPERDOS_X86_OPERATION_CODE_INPUT_STRING_BYTE:
        {
            uint16_t destinationOffset =
                    hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX);
            hyperdos_x86_write_memory_byte_fast(processor,
                                                HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                destinationOffset,
                                                hyperdos_x86_read_input_output_byte(processor, port));
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX,
                                                     (uint16_t)(destinationOffset + directionStep));
            break;
        }

        case HYPERDOS_X86_OPERATION_CODE_INPUT_STRING_WORD:
        {
            uint16_t destinationOffset =
                    hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX);
            hyperdos_x86_write_memory_word_fast(processor,
                                                HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                destinationOffset,
                                                hyperdos_x86_read_input_output_word(processor, port));
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX,
                                                     (uint16_t)(destinationOffset + directionStep));
            break;
        }

        case HYPERDOS_X86_OPERATION_CODE_OUTPUT_STRING_BYTE:
        {
            uint16_t sourceOffset = hyperdos_x86_read_general_register_word(processor,
                                                                            HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX);
            hyperdos_x86_write_input_output_byte(processor,
                                                 port,
                                                 hyperdos_x86_read_memory_byte_fast(processor,
                                                                                    sourceSegment,
                                                                                    sourceOffset));
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX,
                                                     (uint16_t)(sourceOffset + directionStep));
            break;
        }

        case HYPERDOS_X86_OPERATION_CODE_OUTPUT_STRING_WORD:
        {
            uint16_t sourceOffset = hyperdos_x86_read_general_register_word(processor,
                                                                            HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX);
            hyperdos_x86_write_input_output_word(processor,
                                                 port,
                                                 hyperdos_x86_read_memory_word_fast(processor,
                                                                                    sourceSegment,
                                                                                    sourceOffset));
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX,
                                                     (uint16_t)(sourceOffset + directionStep));
            break;
        }

        default:
            return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
        }

        --repeatCount;
        if (repeatMode != HYPERDOS_X86_REPEAT_NONE)
        {
            hyperdos_x86_write_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER, repeatCount);
        }
        else
        {
            break;
        }
    }
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_string_instruction(
        hyperdos_x86_processor*             processor,
        uint8_t                             operationCode,
        hyperdos_x86_repeat_mode            repeatMode,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride)
{
    int      isWord        = (operationCode & HYPERDOS_X86_OPERATION_CODE_WORD_BIT) != 0u;
    uint16_t elementSize   = isWord ? HYPERDOS_X86_WORD_SIZE : HYPERDOS_X86_BYTE_SIZE;
    int16_t  directionStep = (processor->flags & HYPERDOS_X86_FLAG_DIRECTION) != 0u ? (int16_t)-elementSize
                                                                                    : (int16_t)elementSize;
    uint16_t repeatCount   = repeatMode == HYPERDOS_X86_REPEAT_NONE
                                     ? 1u
                                     : hyperdos_x86_read_general_register_word(processor,
                                                                             HYPERDOS_X86_GENERAL_REGISTER_COUNTER);
    hyperdos_x86_segment_register_index sourceSegment = hasSegmentOverride ? segmentOverride
                                                                           : HYPERDOS_X86_SEGMENT_REGISTER_DATA;

    if (repeatCount == 0u)
    {
        return HYPERDOS_X86_EXECUTION_OK;
    }

    if ((operationCode == HYPERDOS_X86_OPERATION_CODE_MOVE_STRING_BYTE ||
         operationCode == HYPERDOS_X86_OPERATION_CODE_MOVE_STRING_WORD) &&
        repeatMode != HYPERDOS_X86_REPEAT_NONE && processor->bus == NULL &&
        (processor->flags & HYPERDOS_X86_FLAG_DIRECTION) == 0u)
    {
        size_t   byteCount    = (size_t)repeatCount * elementSize;
        uint16_t sourceOffset = hyperdos_x86_read_general_register_word(processor,
                                                                        HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX);
        uint16_t destinationOffset =
                hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX);
        uint32_t sourcePhysicalAddress      = 0;
        uint32_t destinationPhysicalAddress = 0;
        if (hyperdos_x86_can_use_contiguous_physical_range(processor,
                                                           sourceSegment,
                                                           sourceOffset,
                                                           byteCount,
                                                           &sourcePhysicalAddress) &&
            hyperdos_x86_can_use_contiguous_physical_range(processor,
                                                           HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                           destinationOffset,
                                                           byteCount,
                                                           &destinationPhysicalAddress) &&
            hyperdos_x86_ranges_do_not_overlap(sourcePhysicalAddress, destinationPhysicalAddress, byteCount))
        {
            memcpy(processor->memory + destinationPhysicalAddress,
                   processor->memory + sourcePhysicalAddress,
                   byteCount);
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX,
                                                     (uint16_t)(sourceOffset + byteCount));
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX,
                                                     (uint16_t)(destinationOffset + byteCount));
            hyperdos_x86_write_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER, 0u);
            return HYPERDOS_X86_EXECUTION_OK;
        }
    }

    if (operationCode == HYPERDOS_X86_OPERATION_CODE_STORE_STRING_BYTE && repeatMode != HYPERDOS_X86_REPEAT_NONE &&
        processor->bus == NULL && (processor->flags & HYPERDOS_X86_FLAG_DIRECTION) == 0u)
    {
        size_t   byteCount = repeatCount;
        uint16_t destinationOffset =
                hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX);
        uint32_t destinationPhysicalAddress = 0;
        if (hyperdos_x86_can_use_contiguous_physical_range(processor,
                                                           HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                           destinationOffset,
                                                           byteCount,
                                                           &destinationPhysicalAddress))
        {
            memset(processor->memory + destinationPhysicalAddress,
                   hyperdos_x86_read_byte_register(processor, 0u),
                   byteCount);
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX,
                                                     (uint16_t)(destinationOffset + byteCount));
            hyperdos_x86_write_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER, 0u);
            return HYPERDOS_X86_EXECUTION_OK;
        }
    }

    while (repeatCount > 0u)
    {
        switch (operationCode)
        {
        case HYPERDOS_X86_OPERATION_CODE_MOVE_STRING_BYTE:
        case HYPERDOS_X86_OPERATION_CODE_MOVE_STRING_WORD:
        {
            uint16_t sourceOffset = hyperdos_x86_read_general_register_word(processor,
                                                                            HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX);
            uint16_t destinationOffset =
                    hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX);
            uint16_t value = isWord ? hyperdos_x86_read_memory_word_fast(processor, sourceSegment, sourceOffset)
                                    : hyperdos_x86_read_memory_byte_fast(processor, sourceSegment, sourceOffset);
            if (isWord)
            {
                hyperdos_x86_write_memory_word_fast(processor,
                                                    HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                    destinationOffset,
                                                    value);
            }
            else
            {
                hyperdos_x86_write_memory_byte_fast(processor,
                                                    HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                    destinationOffset,
                                                    (uint8_t)value);
            }
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX,
                                                     (uint16_t)(sourceOffset + directionStep));
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX,
                                                     (uint16_t)(destinationOffset + directionStep));
            break;
        }

        case HYPERDOS_X86_OPERATION_CODE_COMPARE_STRING_BYTE:
        case HYPERDOS_X86_OPERATION_CODE_COMPARE_STRING_WORD:
        {
            uint16_t sourceOffset = hyperdos_x86_read_general_register_word(processor,
                                                                            HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX);
            uint16_t destinationOffset =
                    hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX);
            uint16_t sourceValue = isWord ? hyperdos_x86_read_memory_word_fast(processor, sourceSegment, sourceOffset)
                                          : hyperdos_x86_read_memory_byte_fast(processor, sourceSegment, sourceOffset);
            uint16_t destinationValue = isWord ? hyperdos_x86_read_memory_word_fast(processor,
                                                                                    HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                                                    destinationOffset)
                                               : hyperdos_x86_read_memory_byte_fast(processor,
                                                                                    HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                                                    destinationOffset);
            (void)hyperdos_x86_execute_arithmetic_logic_operation(processor,
                                                                  HYPERDOS_X86_ARITHMETIC_LOGIC_CMP,
                                                                  sourceValue,
                                                                  destinationValue,
                                                                  hyperdos_x86_operand_value_mask(isWord),
                                                                  hyperdos_x86_operand_sign_bit(isWord));
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX,
                                                     (uint16_t)(sourceOffset + directionStep));
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX,
                                                     (uint16_t)(destinationOffset + directionStep));
            break;
        }

        case HYPERDOS_X86_OPERATION_CODE_STORE_STRING_BYTE:
        case HYPERDOS_X86_OPERATION_CODE_STORE_STRING_WORD:
        {
            uint16_t destinationOffset =
                    hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX);
            uint16_t value = hyperdos_x86_read_general_register_word(processor,
                                                                     HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
            if (isWord)
            {
                hyperdos_x86_write_memory_word_fast(processor,
                                                    HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                    destinationOffset,
                                                    value);
            }
            else
            {
                hyperdos_x86_write_memory_byte_fast(processor,
                                                    HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                    destinationOffset,
                                                    (uint8_t)value);
            }
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX,
                                                     (uint16_t)(destinationOffset + directionStep));
            break;
        }

        case HYPERDOS_X86_OPERATION_CODE_LOAD_STRING_BYTE:
        case HYPERDOS_X86_OPERATION_CODE_LOAD_STRING_WORD:
        {
            uint16_t sourceOffset = hyperdos_x86_read_general_register_word(processor,
                                                                            HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX);
            uint16_t value        = isWord ? hyperdos_x86_read_memory_word_fast(processor, sourceSegment, sourceOffset)
                                           : hyperdos_x86_read_memory_byte_fast(processor, sourceSegment, sourceOffset);
            if (isWord)
            {
                hyperdos_x86_write_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, value);
            }
            else
            {
                hyperdos_x86_write_byte_register(processor, 0u, (uint8_t)value);
            }
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX,
                                                     (uint16_t)(sourceOffset + directionStep));
            break;
        }

        case HYPERDOS_X86_OPERATION_CODE_SCAN_STRING_BYTE:
        case HYPERDOS_X86_OPERATION_CODE_SCAN_STRING_WORD:
        {
            uint16_t destinationOffset =
                    hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX);
            uint16_t accumulatorValue =
                    hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
            uint16_t destinationValue = isWord ? hyperdos_x86_read_memory_word_fast(processor,
                                                                                    HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                                                    destinationOffset)
                                               : hyperdos_x86_read_memory_byte_fast(processor,
                                                                                    HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                                                    destinationOffset);
            (void)hyperdos_x86_execute_arithmetic_logic_operation(processor,
                                                                  HYPERDOS_X86_ARITHMETIC_LOGIC_CMP,
                                                                  accumulatorValue,
                                                                  destinationValue,
                                                                  hyperdos_x86_operand_value_mask(isWord),
                                                                  hyperdos_x86_operand_sign_bit(isWord));
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX,
                                                     (uint16_t)(destinationOffset + directionStep));
            break;
        }

        default:
            return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
        }

        --repeatCount;
        if (repeatMode != HYPERDOS_X86_REPEAT_NONE)
        {
            hyperdos_x86_write_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER, repeatCount);
            if ((operationCode == HYPERDOS_X86_OPERATION_CODE_COMPARE_STRING_BYTE ||
                 operationCode == HYPERDOS_X86_OPERATION_CODE_COMPARE_STRING_WORD ||
                 operationCode == HYPERDOS_X86_OPERATION_CODE_SCAN_STRING_BYTE ||
                 operationCode == HYPERDOS_X86_OPERATION_CODE_SCAN_STRING_WORD))
            {
                int zero = (processor->flags & HYPERDOS_X86_FLAG_ZERO) != 0u;
                if (repeatMode == HYPERDOS_X86_REPEAT_WHILE_EQUAL && !zero)
                {
                    break;
                }
                if (repeatMode == HYPERDOS_X86_REPEAT_WHILE_NOT_EQUAL && zero)
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

    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_shift_rotate_instruction(
        hyperdos_x86_processor*             processor,
        uint8_t                             operationCode,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride)
{
    uint8_t  registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
    uint8_t  operation          = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                  HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
    int      isWord             = (operationCode & HYPERDOS_X86_OPERATION_CODE_WORD_BIT) != 0u;
    uint16_t valueMask          = hyperdos_x86_operand_value_mask(isWord);
    uint16_t signBit            = hyperdos_x86_operand_sign_bit(isWord);
    uint8_t  bitCount           = isWord ? HYPERDOS_X86_WORD_BIT_COUNT : HYPERDOS_X86_BYTE_BIT_COUNT;
    uint8_t  shiftCount         = 1u;
    int      usesImmediateCount = operationCode == HYPERDOS_X86_OPERATION_CODE_SHIFT_ROTATE_IMMEDIATE_BYTE ||
                             operationCode == HYPERDOS_X86_OPERATION_CODE_SHIFT_ROTATE_IMMEDIATE_WORD;
    hyperdos_x86_decoded_operand operand       = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                       registerMemoryByte,
                                                                                       hasSegmentOverride,
                                                                                       segmentOverride);
    uint16_t                     value         = hyperdos_x86_read_operand_value(processor, &operand, isWord);
    uint16_t                     originalValue = value;
    uint8_t                      shiftIndex    = 0;

    if ((operationCode & HYPERDOS_X86_OPERATION_CODE_COUNT_IN_COUNTER_BIT) != 0u)
    {
        shiftCount = hyperdos_x86_read_byte_register(processor, 1u);
    }
    else if (usesImmediateCount)
    {
        shiftCount = hyperdos_x86_fetch_instruction_byte(processor);
    }
    shiftCount = hyperdos_x86_apply_shift_rotate_count_policy(processor, shiftCount);

    if (shiftCount == 0u)
    {
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (operation == HYPERDOS_X86_SHIFT_ROTATE_UNUSED)
    {
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }

    value &= valueMask;
    for (shiftIndex = 0; shiftIndex < shiftCount; ++shiftIndex)
    {
        uint16_t carryFlag = 0;
        switch (operation)
        {
        case HYPERDOS_X86_SHIFT_ROTATE_ROL:
            carryFlag        = (uint16_t)((value & signBit) != 0u ? HYPERDOS_X86_FLAG_CARRY : 0u);
            value            = (uint16_t)(((value << 1u) | (carryFlag != 0u ? 1u : 0u)) & valueMask);
            processor->flags = (uint16_t)((processor->flags & ~HYPERDOS_X86_FLAG_CARRY) | carryFlag |
                                          HYPERDOS_X86_FLAG_RESERVED);
            break;
        case HYPERDOS_X86_SHIFT_ROTATE_ROR:
            carryFlag        = (uint16_t)((value & 1u) != 0u ? HYPERDOS_X86_FLAG_CARRY : 0u);
            value            = (uint16_t)((value >> 1u) | (carryFlag != 0u ? signBit : 0u));
            processor->flags = (uint16_t)((processor->flags & ~HYPERDOS_X86_FLAG_CARRY) | carryFlag |
                                          HYPERDOS_X86_FLAG_RESERVED);
            break;
        case HYPERDOS_X86_SHIFT_ROTATE_RCL:
        {
            uint16_t oldCarry = (processor->flags & HYPERDOS_X86_FLAG_CARRY) != 0u ? 1u : 0u;
            carryFlag         = (uint16_t)((value & signBit) != 0u ? HYPERDOS_X86_FLAG_CARRY : 0u);
            value             = (uint16_t)(((value << 1u) | oldCarry) & valueMask);
            processor->flags  = (uint16_t)((processor->flags & ~HYPERDOS_X86_FLAG_CARRY) | carryFlag |
                                          HYPERDOS_X86_FLAG_RESERVED);
            break;
        }
        case HYPERDOS_X86_SHIFT_ROTATE_RCR:
        {
            uint16_t oldCarry = (processor->flags & HYPERDOS_X86_FLAG_CARRY) != 0u ? signBit : 0u;
            carryFlag         = (uint16_t)((value & 1u) != 0u ? HYPERDOS_X86_FLAG_CARRY : 0u);
            value             = (uint16_t)((value >> 1u) | oldCarry);
            processor->flags  = (uint16_t)((processor->flags & ~HYPERDOS_X86_FLAG_CARRY) | carryFlag |
                                          HYPERDOS_X86_FLAG_RESERVED);
            break;
        }
        case HYPERDOS_X86_SHIFT_ROTATE_SHL:
            carryFlag        = (uint16_t)((value & signBit) != 0u ? HYPERDOS_X86_FLAG_CARRY : 0u);
            value            = (uint16_t)((value << 1u) & valueMask);
            processor->flags = (uint16_t)((processor->flags & ~HYPERDOS_X86_FLAG_CARRY) | carryFlag |
                                          HYPERDOS_X86_FLAG_RESERVED);
            break;
        case HYPERDOS_X86_SHIFT_ROTATE_SHR:
            carryFlag        = (uint16_t)((value & 1u) != 0u ? HYPERDOS_X86_FLAG_CARRY : 0u);
            value            = (uint16_t)(value >> 1u);
            processor->flags = (uint16_t)((processor->flags & ~HYPERDOS_X86_FLAG_CARRY) | carryFlag |
                                          HYPERDOS_X86_FLAG_RESERVED);
            break;
        case HYPERDOS_X86_SHIFT_ROTATE_SAR:
            carryFlag        = (uint16_t)((value & 1u) != 0u ? HYPERDOS_X86_FLAG_CARRY : 0u);
            value            = (uint16_t)((value >> 1u) | (value & signBit));
            processor->flags = (uint16_t)((processor->flags & ~HYPERDOS_X86_FLAG_CARRY) | carryFlag |
                                          HYPERDOS_X86_FLAG_RESERVED);
            break;
        }
    }

    if (operation >= HYPERDOS_X86_SHIFT_ROTATE_SHL)
    {
        uint16_t statusFlags  = (uint16_t)(processor->flags & HYPERDOS_X86_FLAG_CARRY);
        statusFlags          |= hyperdos_x86_sign_zero_parity_flags(value, valueMask, signBit);
        if (shiftCount == 1u)
        {
            if (operation == HYPERDOS_X86_SHIFT_ROTATE_SHL)
            {
                int signAfter  = (value & signBit) != 0u;
                int carryAfter = (processor->flags & HYPERDOS_X86_FLAG_CARRY) != 0u;
                if (signAfter != carryAfter)
                {
                    statusFlags |= HYPERDOS_X86_FLAG_OVERFLOW;
                }
            }
            else if (operation == HYPERDOS_X86_SHIFT_ROTATE_SHR)
            {
                if ((originalValue & signBit) != 0u)
                {
                    statusFlags |= HYPERDOS_X86_FLAG_OVERFLOW;
                }
            }
        }
        hyperdos_x86_replace_status_flags(processor, statusFlags);
    }
    else if (shiftCount == 1u)
    {
        uint16_t overflowFlag = 0;
        if (operation == HYPERDOS_X86_SHIFT_ROTATE_ROL || operation == HYPERDOS_X86_SHIFT_ROTATE_RCL)
        {
            int signAfter  = (value & signBit) != 0u;
            int carryAfter = (processor->flags & HYPERDOS_X86_FLAG_CARRY) != 0u;
            overflowFlag   = signAfter != carryAfter ? HYPERDOS_X86_FLAG_OVERFLOW : 0u;
        }
        else if (operation == HYPERDOS_X86_SHIFT_ROTATE_ROR)
        {
            uint16_t topTwoBits = (uint16_t)((value >> (bitCount - HYPERDOS_X86_TOP_TWO_BIT_COUNT)) &
                                             HYPERDOS_X86_LOW_TWO_BITS_MASK);
            overflowFlag        = (topTwoBits == 1u || topTwoBits == HYPERDOS_X86_TOP_TWO_BIT_COUNT)
                                          ? HYPERDOS_X86_FLAG_OVERFLOW
                                          : 0u;
        }
        processor->flags = (uint16_t)((processor->flags & ~HYPERDOS_X86_FLAG_OVERFLOW) | overflowFlag |
                                      HYPERDOS_X86_FLAG_RESERVED);
    }

    hyperdos_x86_write_operand_value(processor, &operand, isWord, value);
    return HYPERDOS_X86_EXECUTION_OK;
}

static void hyperdos_x86_replace_decimal_adjust_flags(hyperdos_x86_processor* processor,
                                                      uint8_t                 accumulatorLow,
                                                      uint16_t                carryFlag,
                                                      uint16_t                auxiliaryCarryFlag)
{
    const uint16_t statusMask = HYPERDOS_X86_FLAG_CARRY | HYPERDOS_X86_FLAG_PARITY | HYPERDOS_X86_FLAG_AUXILIARY_CARRY |
                                HYPERDOS_X86_FLAG_ZERO | HYPERDOS_X86_FLAG_SIGN;
    uint16_t statusFlags = hyperdos_x86_sign_zero_parity_flags(accumulatorLow,
                                                               HYPERDOS_X86_BYTE_MASK,
                                                               HYPERDOS_X86_BYTE_SIGN_BIT);

    statusFlags      |= carryFlag | auxiliaryCarryFlag;
    processor->flags  = (uint16_t)((processor->flags & ~statusMask) | statusFlags | HYPERDOS_X86_FLAG_RESERVED);
}

static void hyperdos_x86_execute_decimal_adjust_after_add_instruction(hyperdos_x86_processor* processor)
{
    uint8_t  accumulatorLow     = hyperdos_x86_read_byte_register(processor, 0u);
    uint8_t  oldAccumulatorLow  = accumulatorLow;
    uint16_t carryFlag          = 0u;
    uint16_t auxiliaryCarryFlag = 0u;

    if ((accumulatorLow & HYPERDOS_X86_NIBBLE_MASK) > 9u ||
        (processor->flags & HYPERDOS_X86_FLAG_AUXILIARY_CARRY) != 0u)
    {
        accumulatorLow     = (uint8_t)(accumulatorLow + 0x06u);
        auxiliaryCarryFlag = HYPERDOS_X86_FLAG_AUXILIARY_CARRY;
    }
    if (oldAccumulatorLow > 0x99u || (processor->flags & HYPERDOS_X86_FLAG_CARRY) != 0u)
    {
        accumulatorLow = (uint8_t)(accumulatorLow + 0x60u);
        carryFlag      = HYPERDOS_X86_FLAG_CARRY;
    }

    hyperdos_x86_write_byte_register(processor, 0u, accumulatorLow);
    hyperdos_x86_replace_decimal_adjust_flags(processor, accumulatorLow, carryFlag, auxiliaryCarryFlag);
}

static void hyperdos_x86_execute_decimal_adjust_after_subtract_instruction(hyperdos_x86_processor* processor)
{
    uint8_t  accumulatorLow     = hyperdos_x86_read_byte_register(processor, 0u);
    uint8_t  oldAccumulatorLow  = accumulatorLow;
    uint16_t carryFlag          = 0u;
    uint16_t auxiliaryCarryFlag = 0u;

    if ((accumulatorLow & HYPERDOS_X86_NIBBLE_MASK) > 9u ||
        (processor->flags & HYPERDOS_X86_FLAG_AUXILIARY_CARRY) != 0u)
    {
        accumulatorLow     = (uint8_t)(accumulatorLow - 0x06u);
        auxiliaryCarryFlag = HYPERDOS_X86_FLAG_AUXILIARY_CARRY;
    }
    if (oldAccumulatorLow > 0x99u || (processor->flags & HYPERDOS_X86_FLAG_CARRY) != 0u)
    {
        accumulatorLow = (uint8_t)(accumulatorLow - 0x60u);
        carryFlag      = HYPERDOS_X86_FLAG_CARRY;
    }

    hyperdos_x86_write_byte_register(processor, 0u, accumulatorLow);
    hyperdos_x86_replace_decimal_adjust_flags(processor, accumulatorLow, carryFlag, auxiliaryCarryFlag);
}

static void hyperdos_x86_execute_ascii_adjust_add_subtract_instruction(hyperdos_x86_processor* processor, int subtract)
{
    uint8_t  accumulatorLow      = hyperdos_x86_read_byte_register(processor, 0u);
    uint8_t  accumulatorHigh     = hyperdos_x86_read_byte_register(processor, 4u);
    uint16_t carryAuxiliaryFlags = 0u;

    if ((accumulatorLow & HYPERDOS_X86_NIBBLE_MASK) > 9u ||
        (processor->flags & HYPERDOS_X86_FLAG_AUXILIARY_CARRY) != 0u)
    {
        accumulatorLow      = subtract ? (uint8_t)(accumulatorLow - 0x06u) : (uint8_t)(accumulatorLow + 0x06u);
        accumulatorHigh     = subtract ? (uint8_t)(accumulatorHigh - 1u) : (uint8_t)(accumulatorHigh + 1u);
        carryAuxiliaryFlags = HYPERDOS_X86_FLAG_CARRY | HYPERDOS_X86_FLAG_AUXILIARY_CARRY;
    }

    accumulatorLow &= HYPERDOS_X86_NIBBLE_MASK;
    hyperdos_x86_write_byte_register(processor, 0u, accumulatorLow);
    hyperdos_x86_write_byte_register(processor, 4u, accumulatorHigh);
    processor->flags = (uint16_t)((processor->flags & ~(HYPERDOS_X86_FLAG_CARRY | HYPERDOS_X86_FLAG_AUXILIARY_CARRY)) |
                                  carryAuxiliaryFlags | HYPERDOS_X86_FLAG_RESERVED);
}

static hyperdos_x86_execution_result hyperdos_x86_execute_ascii_adjust_multiply_divide_instruction(
        hyperdos_x86_processor* processor,
        uint8_t                 operationCode)
{
    uint8_t radix           = hyperdos_x86_fetch_instruction_byte(processor);
    uint8_t accumulatorLow  = hyperdos_x86_read_byte_register(processor, 0u);
    uint8_t accumulatorHigh = hyperdos_x86_read_byte_register(processor, 4u);

    if (operationCode == HYPERDOS_X86_OPERATION_CODE_ASCII_ADJUST_AFTER_MULTIPLY)
    {
        if (radix == 0u)
        {
            return HYPERDOS_X86_EXECUTION_DIVIDE_ERROR;
        }
        accumulatorHigh = (uint8_t)(accumulatorLow / radix);
        accumulatorLow  = (uint8_t)(accumulatorLow % radix);
    }
    else
    {
        accumulatorLow  = (uint8_t)(accumulatorLow + accumulatorHigh * radix);
        accumulatorHigh = 0u;
    }

    hyperdos_x86_write_byte_register(processor, 0u, accumulatorLow);
    hyperdos_x86_write_byte_register(processor, 4u, accumulatorHigh);
    hyperdos_x86_replace_status_flags(processor,
                                      hyperdos_x86_sign_zero_parity_flags(accumulatorLow,
                                                                          HYPERDOS_X86_BYTE_MASK,
                                                                          HYPERDOS_X86_BYTE_SIGN_BIT));
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_escape_coprocessor_instruction(
        hyperdos_x86_processor*             processor,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride)
{
    uint8_t                              registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
    hyperdos_x86_decoded_operand         operand            = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                       registerMemoryByte,
                                                                                       hasSegmentOverride,
                                                                                       segmentOverride);
    hyperdos_x86_coprocessor_instruction instruction;

    instruction.operationCode      = processor->lastOperationCode;
    instruction.registerMemoryByte = registerMemoryByte;
    instruction.isRegister         = operand.isRegister;
    instruction.registerIndex      = operand.registerIndex;
    instruction.operationIndex     = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                           HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
    instruction.segmentRegister    = operand.segmentRegister;
    instruction.offset             = operand.offset;

    if (processor->coprocessorEscapeHandler != NULL)
    {
        return processor->coprocessorEscapeHandler(processor, &instruction, processor->coprocessorContext);
    }
    if (processor->escapeTrapEnabled && hyperdos_x86_supports_80186_instructions(processor))
    {
        return hyperdos_x86_execute_escape_operation_code_interrupt(processor);
    }
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_multiply_divide_instruction(
        hyperdos_x86_processor*             processor,
        uint8_t                             operationCode,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride)
{
    uint8_t                      registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
    uint8_t                      operation = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                  HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
    int                          isWord    = operationCode == HYPERDOS_X86_OPERATION_CODE_GROUP_THREE_WORD;
    hyperdos_x86_decoded_operand operand   = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                       registerMemoryByte,
                                                                                       hasSegmentOverride,
                                                                                       segmentOverride);
    uint16_t                     value     = hyperdos_x86_read_operand_value(processor, &operand, isWord);

    switch (operation)
    {
    case HYPERDOS_X86_GROUP_THREE_TEST:
    {
        uint16_t immediateValue = isWord ? hyperdos_x86_fetch_instruction_word(processor)
                                         : hyperdos_x86_fetch_instruction_byte(processor);
        hyperdos_x86_test_values(processor,
                                 value,
                                 immediateValue,
                                 hyperdos_x86_operand_value_mask(isWord),
                                 hyperdos_x86_operand_sign_bit(isWord));
        return HYPERDOS_X86_EXECUTION_OK;
    }

    case HYPERDOS_X86_GROUP_THREE_NOT:
        hyperdos_x86_write_operand_value(processor, &operand, isWord, (uint16_t)(~value));
        return HYPERDOS_X86_EXECUTION_OK;

    case HYPERDOS_X86_GROUP_THREE_NEGATE:
    {
        uint16_t result = hyperdos_x86_execute_arithmetic_logic_operation(processor,
                                                                          HYPERDOS_X86_ARITHMETIC_LOGIC_SUB,
                                                                          0u,
                                                                          value,
                                                                          hyperdos_x86_operand_value_mask(isWord),
                                                                          hyperdos_x86_operand_sign_bit(isWord));
        if ((value & hyperdos_x86_operand_value_mask(isWord)) != 0u)
        {
            processor->flags |= HYPERDOS_X86_FLAG_CARRY;
        }
        else
        {
            processor->flags &= (uint16_t)~HYPERDOS_X86_FLAG_CARRY;
        }
        processor->flags |= HYPERDOS_X86_FLAG_RESERVED;
        hyperdos_x86_write_operand_value(processor, &operand, isWord, result);
        return HYPERDOS_X86_EXECUTION_OK;
    }

    case HYPERDOS_X86_GROUP_THREE_MULTIPLY_UNSIGNED:
        if (isWord)
        {
            uint32_t result =
                    (uint32_t)hyperdos_x86_read_general_register_word(processor,
                                                                      HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) *
                    value;
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                     (uint16_t)result);
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                                     (uint16_t)(result >> HYPERDOS_X86_WORD_BIT_COUNT));
            if ((result >> HYPERDOS_X86_WORD_BIT_COUNT) != 0u)
            {
                processor->flags |= HYPERDOS_X86_FLAG_CARRY | HYPERDOS_X86_FLAG_OVERFLOW;
            }
            else
            {
                processor->flags &= (uint16_t)~(HYPERDOS_X86_FLAG_CARRY | HYPERDOS_X86_FLAG_OVERFLOW);
            }
        }
        else
        {
            uint16_t result = (uint16_t)hyperdos_x86_read_byte_register(processor, 0u) * (uint8_t)value;
            hyperdos_x86_write_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, result);
            if ((result & HYPERDOS_X86_HIGH_BYTE_MASK) != 0u)
            {
                processor->flags |= HYPERDOS_X86_FLAG_CARRY | HYPERDOS_X86_FLAG_OVERFLOW;
            }
            else
            {
                processor->flags &= (uint16_t)~(HYPERDOS_X86_FLAG_CARRY | HYPERDOS_X86_FLAG_OVERFLOW);
            }
        }
        processor->flags |= HYPERDOS_X86_FLAG_RESERVED;
        return HYPERDOS_X86_EXECUTION_OK;

    case HYPERDOS_X86_GROUP_THREE_MULTIPLY_SIGNED:
        if (isWord)
        {
            int32_t result = (int32_t)(int16_t)hyperdos_x86_read_general_register_word(
                                     processor,
                                     HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) *
                             (int32_t)(int16_t)value;
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                     (uint16_t)result);
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                                     (uint16_t)((uint32_t)result >> HYPERDOS_X86_WORD_BIT_COUNT));
            if (result < INT16_MIN || result > INT16_MAX)
            {
                processor->flags |= HYPERDOS_X86_FLAG_CARRY | HYPERDOS_X86_FLAG_OVERFLOW;
            }
            else
            {
                processor->flags &= (uint16_t)~(HYPERDOS_X86_FLAG_CARRY | HYPERDOS_X86_FLAG_OVERFLOW);
            }
        }
        else
        {
            int16_t result = (int16_t)((int16_t)(int8_t)hyperdos_x86_read_byte_register(processor, 0u) *
                                       (int16_t)(int8_t)value);
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                     (uint16_t)result);
            if (result < INT8_MIN || result > INT8_MAX)
            {
                processor->flags |= HYPERDOS_X86_FLAG_CARRY | HYPERDOS_X86_FLAG_OVERFLOW;
            }
            else
            {
                processor->flags &= (uint16_t)~(HYPERDOS_X86_FLAG_CARRY | HYPERDOS_X86_FLAG_OVERFLOW);
            }
        }
        processor->flags |= HYPERDOS_X86_FLAG_RESERVED;
        return HYPERDOS_X86_EXECUTION_OK;

    case HYPERDOS_X86_GROUP_THREE_DIVIDE_UNSIGNED:
        if (value == 0u)
        {
            return HYPERDOS_X86_EXECUTION_DIVIDE_ERROR;
        }
        if (isWord)
        {
            uint32_t dividend = ((uint32_t)hyperdos_x86_read_general_register_word(processor,
                                                                                   HYPERDOS_X86_GENERAL_REGISTER_DATA)
                                 << HYPERDOS_X86_WORD_BIT_COUNT) |
                                hyperdos_x86_read_general_register_word(processor,
                                                                        HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
            uint32_t quotient  = dividend / value;
            uint32_t remainder = dividend % value;
            if (quotient > HYPERDOS_X86_WORD_MASK)
            {
                return HYPERDOS_X86_EXECUTION_DIVIDE_ERROR;
            }
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                     (uint16_t)quotient);
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                                     (uint16_t)remainder);
        }
        else
        {
            uint16_t dividend  = hyperdos_x86_read_general_register_word(processor,
                                                                        HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
            uint16_t quotient  = (uint16_t)(dividend / (uint8_t)value);
            uint16_t remainder = (uint16_t)(dividend % (uint8_t)value);
            if (quotient > HYPERDOS_X86_BYTE_MASK)
            {
                return HYPERDOS_X86_EXECUTION_DIVIDE_ERROR;
            }
            hyperdos_x86_write_byte_register(processor, 0u, (uint8_t)quotient);
            hyperdos_x86_write_byte_register(processor, 4u, (uint8_t)remainder);
        }
        return HYPERDOS_X86_EXECUTION_OK;

    case HYPERDOS_X86_GROUP_THREE_DIVIDE_SIGNED:
        if (value == 0u)
        {
            return HYPERDOS_X86_EXECUTION_DIVIDE_ERROR;
        }
        if (isWord)
        {
            int32_t dividend =
                    (int32_t)(int16_t)hyperdos_x86_read_general_register_word(processor,
                                                                              HYPERDOS_X86_GENERAL_REGISTER_DATA) *
                            (int32_t)HYPERDOS_X86_SEGMENT_BYTE_COUNT +
                    (int32_t)hyperdos_x86_read_general_register_word(processor,
                                                                     HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
            int32_t divisor   = (int32_t)(int16_t)value;
            int32_t quotient  = 0;
            int32_t remainder = 0;

            if (dividend == INT32_MIN && divisor == -1)
            {
                return HYPERDOS_X86_EXECUTION_DIVIDE_ERROR;
            }
            quotient  = dividend / divisor;
            remainder = dividend % divisor;
            if (hyperdos_x86_signed_divide_quotient_is_out_of_range(processor, quotient, 1))
            {
                return HYPERDOS_X86_EXECUTION_DIVIDE_ERROR;
            }
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                     (uint16_t)quotient);
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                                     (uint16_t)remainder);
        }
        else
        {
            int32_t dividend = (int32_t)(int16_t)
                    hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
            int32_t divisor   = (int32_t)(int8_t)value;
            int32_t quotient  = dividend / divisor;
            int32_t remainder = dividend % divisor;
            if (hyperdos_x86_signed_divide_quotient_is_out_of_range(processor, quotient, 0))
            {
                return HYPERDOS_X86_EXECUTION_DIVIDE_ERROR;
            }
            hyperdos_x86_write_byte_register(processor, 0u, (uint8_t)quotient);
            hyperdos_x86_write_byte_register(processor, 4u, (uint8_t)remainder);
        }
        return HYPERDOS_X86_EXECUTION_OK;
    }

    return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_group_five_instruction(
        hyperdos_x86_processor*             processor,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride)
{
    uint8_t                      registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
    uint8_t                      operation = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                  HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
    hyperdos_x86_decoded_operand operand;
    uint16_t                     value = 0u;

    if (operation == HYPERDOS_X86_GROUP_FIVE_UNUSED && hyperdos_x86_supports_80186_instructions(processor))
    {
        return hyperdos_x86_execute_unused_operation_code_interrupt(processor);
    }

    operand = hyperdos_x86_decode_register_memory_operand(processor,
                                                          registerMemoryByte,
                                                          hasSegmentOverride,
                                                          segmentOverride);
    value   = hyperdos_x86_read_operand_value(processor, &operand, 1);

    switch (operation)
    {
    case HYPERDOS_X86_GROUP_FIVE_INCREMENT:
        hyperdos_x86_update_increment_decrement_flags(processor,
                                                      HYPERDOS_X86_ARITHMETIC_LOGIC_ADD,
                                                      value,
                                                      HYPERDOS_X86_WORD_MASK,
                                                      HYPERDOS_X86_WORD_SIGN_BIT);
        hyperdos_x86_write_operand_value(processor, &operand, 1, (uint16_t)(value + 1u));
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_X86_GROUP_FIVE_DECREMENT:
        hyperdos_x86_update_increment_decrement_flags(processor,
                                                      HYPERDOS_X86_ARITHMETIC_LOGIC_SUB,
                                                      value,
                                                      HYPERDOS_X86_WORD_MASK,
                                                      HYPERDOS_X86_WORD_SIGN_BIT);
        hyperdos_x86_write_operand_value(processor, &operand, 1, (uint16_t)(value - 1u));
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_X86_GROUP_FIVE_CALL_NEAR:
        hyperdos_x86_push_word(processor, hyperdos_x86_read_instruction_pointer_word(processor));
        hyperdos_x86_write_instruction_pointer_word(processor, value);
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_X86_GROUP_FIVE_CALL_FAR:
    {
        uint16_t segment = 0;
        if (operand.isRegister)
        {
            return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
        }
        segment = hyperdos_x86_read_memory_word_fast(processor,
                                                     operand.segmentRegister,
                                                     (uint16_t)(operand.offset +
                                                                HYPERDOS_X86_FAR_POINTER_SEGMENT_OFFSET));
        hyperdos_x86_push_word(processor, processor->segmentRegisters[HYPERDOS_X86_SEGMENT_REGISTER_CODE]);
        hyperdos_x86_push_word(processor, hyperdos_x86_read_instruction_pointer_word(processor));
        hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE, segment);
        hyperdos_x86_write_instruction_pointer_word(processor, value);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    case HYPERDOS_X86_GROUP_FIVE_JUMP_NEAR:
        hyperdos_x86_write_instruction_pointer_word(processor, value);
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_X86_GROUP_FIVE_JUMP_FAR:
    {
        uint16_t segment = 0;
        if (operand.isRegister)
        {
            return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
        }
        segment = hyperdos_x86_read_memory_word_fast(processor,
                                                     operand.segmentRegister,
                                                     (uint16_t)(operand.offset +
                                                                HYPERDOS_X86_FAR_POINTER_SEGMENT_OFFSET));
        hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE, segment);
        hyperdos_x86_write_instruction_pointer_word(processor, value);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    case HYPERDOS_X86_GROUP_FIVE_PUSH:
        hyperdos_x86_push_word(processor, hyperdos_x86_adjust_push_source_word(processor, &operand, value));
        return HYPERDOS_X86_EXECUTION_OK;
    }

    return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
}

hyperdos_x86_execution_result hyperdos_x86_initialize_processor(hyperdos_x86_processor* processor,
                                                                uint8_t*                memory,
                                                                size_t                  memorySize)
{
    if (processor == NULL || memory == NULL || memorySize < HYPERDOS_X86_MEMORY_SIZE)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }

    memset(processor, 0, sizeof(*processor));
    processor->memory     = memory;
    processor->memorySize = memorySize;
    hyperdos_x86_reset_processor(processor);
    return HYPERDOS_X86_EXECUTION_OK;
}

void hyperdos_x86_reset_processor(hyperdos_x86_processor* processor)
{
    uint8_t*                                memory                                  = NULL;
    size_t                                  memorySize                              = 0u;
    struct hyperdos_bus*                    bus                                     = NULL;
    hyperdos_x86_interrupt_handler          interruptHandler                        = NULL;
    void*                                   userContext                             = NULL;
    hyperdos_x86_coprocessor_wait_handler   coprocessorWaitHandler                  = NULL;
    hyperdos_x86_coprocessor_escape_handler coprocessorEscapeHandler                = NULL;
    void*                                   coprocessorContext                      = NULL;
    uint8_t                                 processorModel                          = 0u;
    uint8_t                                 divideErrorReturnsToFaultingInstruction = 0u;

    if (processor == NULL)
    {
        return;
    }

    memory                                  = processor->memory;
    memorySize                              = processor->memorySize;
    bus                                     = processor->bus;
    interruptHandler                        = processor->interruptHandler;
    userContext                             = processor->userContext;
    coprocessorWaitHandler                  = processor->coprocessorWaitHandler;
    coprocessorEscapeHandler                = processor->coprocessorEscapeHandler;
    coprocessorContext                      = processor->coprocessorContext;
    processorModel                          = processor->processorModel;
    divideErrorReturnsToFaultingInstruction = processor->divideErrorReturnsToFaultingInstruction;

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
    hyperdos_x86_write_flags_word(processor, hyperdos_x86_normalize_flags(HYPERDOS_X86_FLAG_INTERRUPT_ENABLE));
    processor->segmentRegisters[HYPERDOS_X86_SEGMENT_REGISTER_CODE] = HYPERDOS_X86_RESET_CODE_SEGMENT;
    hyperdos_x86_write_instruction_pointer_word(processor, HYPERDOS_X86_RESET_INSTRUCTION_POINTER);
    hyperdos_x86_store_segment_base(processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA);
    hyperdos_x86_store_segment_base(processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE);
    hyperdos_x86_store_segment_base(processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK);
    hyperdos_x86_store_segment_base(processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA);
}

void hyperdos_x86_set_processor_model(hyperdos_x86_processor* processor, hyperdos_x86_processor_model model)
{
    if (processor == NULL || (size_t)model >= hyperdos_x86_processor_descriptor_count())
    {
        return;
    }
    processor->processorModel = (uint8_t)model;
}

void hyperdos_x86_set_interrupt_handler(hyperdos_x86_processor*        processor,
                                        hyperdos_x86_interrupt_handler interruptHandler,
                                        void*                          userContext)
{
    if (processor == NULL)
    {
        return;
    }
    processor->interruptHandler = interruptHandler;
    processor->userContext      = userContext;
}

void hyperdos_x86_attach_coprocessor(hyperdos_x86_processor*                 processor,
                                     hyperdos_x86_coprocessor_wait_handler   waitHandler,
                                     hyperdos_x86_coprocessor_escape_handler escapeHandler,
                                     void*                                   userContext)
{
    if (processor == NULL)
    {
        return;
    }
    processor->coprocessorWaitHandler   = waitHandler;
    processor->coprocessorEscapeHandler = escapeHandler;
    processor->coprocessorContext       = userContext;
}

void hyperdos_x86_set_escape_trap_enabled(hyperdos_x86_processor* processor, int enabled)
{
    if (processor == NULL)
    {
        return;
    }
    processor->escapeTrapEnabled = (uint8_t)(enabled != 0);
}

void hyperdos_x86_attach_bus(hyperdos_x86_processor* processor, struct hyperdos_bus* bus)
{
    if (processor == NULL)
    {
        return;
    }
    processor->bus = bus;
}

hyperdos_x86_execution_result hyperdos_x86_load_dos_program(hyperdos_x86_processor* processor,
                                                            const uint8_t*          programBytes,
                                                            size_t                  programSize,
                                                            uint16_t                programSegment,
                                                            const char*             commandTail,
                                                            size_t                  commandTailLength)
{
    uint32_t programBase          = 0;
    uint32_t requestedProgramBase = ((uint32_t)programSegment << HYPERDOS_X86_SEGMENT_SHIFT) &
                                    HYPERDOS_X86_ADDRESS_MASK;
    size_t commandTailByteCount = commandTailLength;

    if (processor == NULL || processor->memory == NULL || processor->memorySize < HYPERDOS_X86_MEMORY_SIZE ||
        programBytes == NULL)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }
    if (programSize > HYPERDOS_X86_DOS_PROGRAM_MAXIMUM_SIZE)
    {
        return HYPERDOS_X86_EXECUTION_PROGRAM_TOO_LARGE;
    }
    if (requestedProgramBase > HYPERDOS_X86_MEMORY_SIZE - HYPERDOS_X86_SEGMENT_BYTE_COUNT)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }
    if (commandTailByteCount > HYPERDOS_X86_DOS_COMMAND_TAIL_MAXIMUM_LENGTH)
    {
        commandTailByteCount = HYPERDOS_X86_DOS_COMMAND_TAIL_MAXIMUM_LENGTH;
    }

    hyperdos_x86_reset_processor(processor);
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, programSegment);
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE, programSegment);
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, programSegment);
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, programSegment);
    hyperdos_x86_write_instruction_pointer_word(processor, HYPERDOS_X86_DOS_PROGRAM_OFFSET);
    hyperdos_x86_write_general_register_word(processor,
                                             HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                             HYPERDOS_X86_DOS_STACK_POINTER);

    programBase = processor->segmentBases[HYPERDOS_X86_SEGMENT_REGISTER_CODE];
    memset(processor->memory + programBase, 0, HYPERDOS_X86_SEGMENT_BYTE_COUNT);
    hyperdos_x86_write_physical_byte(processor,
                                     programBase + HYPERDOS_X86_DOS_TERMINATE_STUB_OFFSET,
                                     HYPERDOS_X86_OPERATION_CODE_INTERRUPT_IMMEDIATE);
    hyperdos_x86_write_physical_byte(processor,
                                     programBase + HYPERDOS_X86_DOS_TERMINATE_STUB_OFFSET + HYPERDOS_X86_BYTE_SIZE,
                                     HYPERDOS_X86_DOS_INTERRUPT_TERMINATE_PROGRAM);
    hyperdos_x86_write_physical_byte(processor,
                                     programBase + HYPERDOS_X86_DOS_COMMAND_TAIL_LENGTH_OFFSET,
                                     (uint8_t)commandTailByteCount);
    if (commandTail != NULL && commandTailByteCount > 0u)
    {
        memcpy(processor->memory + programBase + HYPERDOS_X86_DOS_COMMAND_TAIL_BYTES_OFFSET,
               commandTail,
               commandTailByteCount);
    }
    hyperdos_x86_write_physical_byte(processor,
                                     (uint32_t)(programBase + HYPERDOS_X86_DOS_COMMAND_TAIL_BYTES_OFFSET +
                                                commandTailByteCount),
                                     HYPERDOS_X86_ASCII_CARRIAGE_RETURN);
    memcpy(processor->memory + programBase + HYPERDOS_X86_DOS_PROGRAM_OFFSET, programBytes, programSize);
    return HYPERDOS_X86_EXECUTION_OK;
}

hyperdos_x86_execution_result hyperdos_x86_execute(hyperdos_x86_processor* processor, uint64_t instructionLimit)
{
    if (processor == NULL || processor->memory == NULL || processor->memorySize < HYPERDOS_X86_MEMORY_SIZE)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }

    while (!processor->halted)
    {
        uint16_t instructionStartOffset            = hyperdos_x86_read_instruction_pointer_word(processor);
        uint16_t instructionStartSegment           = processor->segmentRegisters[HYPERDOS_X86_SEGMENT_REGISTER_CODE];
        uint8_t  operationCode                     = 0;
        int      hasSegmentOverride                = 0;
        int      maskableInterruptInhibitWasActive = processor->maskableInterruptInhibitCount != 0u;
        int      trapWasEnabled                    = (processor->flags & HYPERDOS_X86_FLAG_TRAP) != 0u;
        int      startMaskableInterruptInhibit     = 0;
        int      suppressTrapAfterInstruction      = 0;
        hyperdos_x86_segment_register_index segmentOverride = HYPERDOS_X86_SEGMENT_REGISTER_DATA;
        hyperdos_x86_repeat_mode            repeatMode      = HYPERDOS_X86_REPEAT_NONE;
        hyperdos_x86_execution_result       result          = HYPERDOS_X86_EXECUTION_OK;

        if (instructionLimit != 0u && processor->executedInstructionCount >= instructionLimit)
        {
            return HYPERDOS_X86_EXECUTION_STEP_LIMIT_REACHED;
        }

        for (;;)
        {
            operationCode = hyperdos_x86_fetch_instruction_byte(processor);
            if (operationCode == HYPERDOS_X86_OPERATION_CODE_PREFIX_EXTRA_SEGMENT)
            {
                hasSegmentOverride = 1;
                segmentOverride    = HYPERDOS_X86_SEGMENT_REGISTER_EXTRA;
                continue;
            }
            if (operationCode == HYPERDOS_X86_OPERATION_CODE_PREFIX_CODE_SEGMENT)
            {
                hasSegmentOverride = 1;
                segmentOverride    = HYPERDOS_X86_SEGMENT_REGISTER_CODE;
                continue;
            }
            if (operationCode == HYPERDOS_X86_OPERATION_CODE_PREFIX_STACK_SEGMENT)
            {
                hasSegmentOverride = 1;
                segmentOverride    = HYPERDOS_X86_SEGMENT_REGISTER_STACK;
                continue;
            }
            if (operationCode == HYPERDOS_X86_OPERATION_CODE_PREFIX_DATA_SEGMENT)
            {
                hasSegmentOverride = 1;
                segmentOverride    = HYPERDOS_X86_SEGMENT_REGISTER_DATA;
                continue;
            }
            if (operationCode == HYPERDOS_X86_OPERATION_CODE_PREFIX_REPEAT_WHILE_NOT_EQUAL)
            {
                repeatMode = HYPERDOS_X86_REPEAT_WHILE_NOT_EQUAL;
                continue;
            }
            if (operationCode == HYPERDOS_X86_OPERATION_CODE_PREFIX_REPEAT)
            {
                repeatMode = HYPERDOS_X86_REPEAT_WHILE_EQUAL;
                continue;
            }
            if (operationCode == HYPERDOS_X86_OPERATION_CODE_PREFIX_LOCK)
            {
                continue;
            }
            break;
        }

        processor->lastOperationCode      = operationCode;
        processor->lastInstructionSegment = instructionStartSegment;
        processor->lastInstructionOffset  = instructionStartOffset;

        if (hyperdos_x86_execute_operation_code_validity_action(processor, operationCode, &result))
        {
        }
        else if (hyperdos_x86_operation_code_is_in_range(operationCode,
                                                         HYPERDOS_X86_OPERATION_CODE_ARITHMETIC_LOGIC_FIRST,
                                                         HYPERDOS_X86_OPERATION_CODE_ARITHMETIC_LOGIC_LAST) &&
                 (operationCode & HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK) <=
                         HYPERDOS_X86_ARITHMETIC_FORM_ACCUMULATOR_WORD)
        {
            result = hyperdos_x86_execute_arithmetic_logic_instruction(processor,
                                                                       operationCode,
                                                                       hasSegmentOverride,
                                                                       segmentOverride);
        }
        else if (hyperdos_x86_operation_code_is_in_range(operationCode,
                                                         HYPERDOS_X86_OPERATION_CODE_INCREMENT_REGISTER_FIRST,
                                                         HYPERDOS_X86_OPERATION_CODE_INCREMENT_REGISTER_LAST))
        {
            hyperdos_x86_general_register_index
                     registerIndex = (hyperdos_x86_general_register_index)(operationCode &
                                                                          HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
            uint16_t value         = hyperdos_x86_read_general_register_word(processor, registerIndex);
            hyperdos_x86_update_increment_decrement_flags(processor,
                                                          HYPERDOS_X86_ARITHMETIC_LOGIC_ADD,
                                                          value,
                                                          HYPERDOS_X86_WORD_MASK,
                                                          HYPERDOS_X86_WORD_SIGN_BIT);
            hyperdos_x86_write_general_register_word(processor, registerIndex, (uint16_t)(value + 1u));
        }
        else if (hyperdos_x86_operation_code_is_in_range(operationCode,
                                                         HYPERDOS_X86_OPERATION_CODE_DECREMENT_REGISTER_FIRST,
                                                         HYPERDOS_X86_OPERATION_CODE_DECREMENT_REGISTER_LAST))
        {
            hyperdos_x86_general_register_index
                     registerIndex = (hyperdos_x86_general_register_index)(operationCode &
                                                                          HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
            uint16_t value         = hyperdos_x86_read_general_register_word(processor, registerIndex);
            hyperdos_x86_update_increment_decrement_flags(processor,
                                                          HYPERDOS_X86_ARITHMETIC_LOGIC_SUB,
                                                          value,
                                                          HYPERDOS_X86_WORD_MASK,
                                                          HYPERDOS_X86_WORD_SIGN_BIT);
            hyperdos_x86_write_general_register_word(processor, registerIndex, (uint16_t)(value - 1u));
        }
        else if (hyperdos_x86_operation_code_is_in_range(operationCode,
                                                         HYPERDOS_X86_OPERATION_CODE_PUSH_REGISTER_FIRST,
                                                         HYPERDOS_X86_OPERATION_CODE_PUSH_REGISTER_LAST))
        {
            hyperdos_x86_general_register_index
                     registerIndex = (hyperdos_x86_general_register_index)(operationCode &
                                                                          HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
            uint16_t value         = hyperdos_x86_read_general_register_word(processor, registerIndex);
            if (registerIndex == HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER)
            {
                value = hyperdos_x86_get_stack_pointer_push_value(processor, value);
            }
            hyperdos_x86_push_word(processor, value);
        }
        else if (hyperdos_x86_operation_code_is_in_range(operationCode,
                                                         HYPERDOS_X86_OPERATION_CODE_POP_REGISTER_FIRST,
                                                         HYPERDOS_X86_OPERATION_CODE_POP_REGISTER_LAST))
        {
            hyperdos_x86_general_register_index
                    registerIndex = (hyperdos_x86_general_register_index)(operationCode &
                                                                          HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
            hyperdos_x86_write_general_register_word(processor, registerIndex, hyperdos_x86_pop_word(processor));
        }
        else if (operationCode == HYPERDOS_X86_OPERATION_CODE_PUSH_ALL)
        {
            if (!hyperdos_x86_supports_80186_instructions(processor))
            {
                result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
            }
            else
            {
                hyperdos_x86_execute_push_all_instruction(processor);
            }
        }
        else if (operationCode == HYPERDOS_X86_OPERATION_CODE_POP_ALL)
        {
            if (!hyperdos_x86_supports_80186_instructions(processor))
            {
                result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
            }
            else
            {
                hyperdos_x86_execute_pop_all_instruction(processor);
            }
        }
        else if (operationCode == HYPERDOS_X86_OPERATION_CODE_BOUND)
        {
            if (!hyperdos_x86_supports_80186_instructions(processor))
            {
                result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
            }
            else
            {
                result = hyperdos_x86_execute_bound_instruction(processor, hasSegmentOverride, segmentOverride);
            }
        }
        else if (operationCode == HYPERDOS_X86_OPERATION_CODE_PUSH_IMMEDIATE_WORD)
        {
            if (!hyperdos_x86_supports_80186_instructions(processor))
            {
                result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
            }
            else
            {
                hyperdos_x86_push_word(processor, hyperdos_x86_fetch_instruction_word(processor));
            }
        }
        else if (operationCode == HYPERDOS_X86_OPERATION_CODE_SIGNED_MULTIPLY_IMMEDIATE_WORD)
        {
            if (!hyperdos_x86_supports_80186_instructions(processor))
            {
                result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
            }
            else
            {
                result = hyperdos_x86_execute_immediate_signed_multiply_instruction(processor,
                                                                                    operationCode,
                                                                                    hasSegmentOverride,
                                                                                    segmentOverride);
            }
        }
        else if (operationCode == HYPERDOS_X86_OPERATION_CODE_PUSH_IMMEDIATE_BYTE)
        {
            if (!hyperdos_x86_supports_80186_instructions(processor))
            {
                result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
            }
            else
            {
                uint16_t value = (uint16_t)(int16_t)(int8_t)hyperdos_x86_fetch_instruction_byte(processor);
                hyperdos_x86_push_word(processor, value);
            }
        }
        else if (operationCode == HYPERDOS_X86_OPERATION_CODE_SIGNED_MULTIPLY_IMMEDIATE_BYTE)
        {
            if (!hyperdos_x86_supports_80186_instructions(processor))
            {
                result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
            }
            else
            {
                result = hyperdos_x86_execute_immediate_signed_multiply_instruction(processor,
                                                                                    operationCode,
                                                                                    hasSegmentOverride,
                                                                                    segmentOverride);
            }
        }
        else if (hyperdos_x86_operation_code_is_input_output_string(operationCode))
        {
            if (!hyperdos_x86_supports_80186_instructions(processor))
            {
                result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
            }
            else
            {
                result = hyperdos_x86_execute_input_output_string_instruction(processor,
                                                                              operationCode,
                                                                              repeatMode,
                                                                              hasSegmentOverride,
                                                                              segmentOverride);
            }
        }
        else if (hyperdos_x86_operation_code_is_in_range(operationCode,
                                                         HYPERDOS_X86_OPERATION_CODE_JUMP_CONDITION_FIRST,
                                                         HYPERDOS_X86_OPERATION_CODE_JUMP_CONDITION_LAST))
        {
            int8_t displacement = (int8_t)hyperdos_x86_fetch_instruction_byte(processor);
            if (hyperdos_x86_evaluate_condition(processor, (uint8_t)(operationCode & HYPERDOS_X86_NIBBLE_MASK)))
            {
                hyperdos_x86_jump_relative(processor, displacement);
            }
        }
        else if (hyperdos_x86_operation_code_is_in_range(operationCode,
                                                         HYPERDOS_X86_OPERATION_CODE_GROUP_ONE_BYTE_IMMEDIATE,
                                                         HYPERDOS_X86_OPERATION_CODE_GROUP_ONE_SIGN_EXTENDED_IMMEDIATE))
        {
            result = hyperdos_x86_execute_group_one_instruction(processor,
                                                                operationCode,
                                                                hasSegmentOverride,
                                                                segmentOverride);
        }
        else if (hyperdos_x86_operation_code_is_in_range(operationCode,
                                                         HYPERDOS_X86_OPERATION_CODE_MOVE_REGISTER_MEMORY_BYTE_FIRST,
                                                         HYPERDOS_X86_OPERATION_CODE_MOVE_REGISTER_MEMORY_WORD_LAST))
        {
            result = hyperdos_x86_execute_move_instruction(processor,
                                                           operationCode,
                                                           hasSegmentOverride,
                                                           segmentOverride);
        }
        else if (hyperdos_x86_operation_code_is_in_range(operationCode,
                                                         HYPERDOS_X86_OPERATION_CODE_EXCHANGE_ACCUMULATOR_FIRST,
                                                         HYPERDOS_X86_OPERATION_CODE_EXCHANGE_ACCUMULATOR_LAST))
        {
            hyperdos_x86_general_register_index
                     registerIndex = (hyperdos_x86_general_register_index)(operationCode &
                                                                          HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
            uint16_t value         = hyperdos_x86_read_general_register_word(processor,
                                                                     HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
            hyperdos_x86_write_general_register_word(processor,
                                                     HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                     hyperdos_x86_read_general_register_word(processor, registerIndex));
            hyperdos_x86_write_general_register_word(processor, registerIndex, value);
        }
        else if (hyperdos_x86_operation_code_is_string(operationCode))
        {
            result = hyperdos_x86_execute_string_instruction(processor,
                                                             operationCode,
                                                             repeatMode,
                                                             hasSegmentOverride,
                                                             segmentOverride);
        }
        else if (hyperdos_x86_operation_code_is_in_range(operationCode,
                                                         HYPERDOS_X86_OPERATION_CODE_ESCAPE_COPROCESSOR_FIRST,
                                                         HYPERDOS_X86_OPERATION_CODE_ESCAPE_COPROCESSOR_LAST))
        {
            result = hyperdos_x86_execute_escape_coprocessor_instruction(processor,
                                                                         hasSegmentOverride,
                                                                         segmentOverride);
        }
        else if (hyperdos_x86_operation_code_is_in_range(
                         operationCode,
                         HYPERDOS_X86_OPERATION_CODE_MOVE_IMMEDIATE_TO_BYTE_REGISTER_FIRST,
                         HYPERDOS_X86_OPERATION_CODE_MOVE_IMMEDIATE_TO_BYTE_REGISTER_LAST))
        {
            hyperdos_x86_write_byte_register(processor,
                                             (uint8_t)(operationCode & HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK),
                                             hyperdos_x86_fetch_instruction_byte(processor));
        }
        else if (hyperdos_x86_operation_code_is_in_range(
                         operationCode,
                         HYPERDOS_X86_OPERATION_CODE_MOVE_IMMEDIATE_TO_WORD_REGISTER_FIRST,
                         HYPERDOS_X86_OPERATION_CODE_MOVE_IMMEDIATE_TO_WORD_REGISTER_LAST))
        {
            hyperdos_x86_write_general_register_word(
                    processor,
                    (hyperdos_x86_general_register_index)(operationCode & HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK),
                    hyperdos_x86_fetch_instruction_word(processor));
        }
        else if (hyperdos_x86_operation_code_is_in_range(operationCode,
                                                         HYPERDOS_X86_OPERATION_CODE_SHIFT_ROTATE_FIRST,
                                                         HYPERDOS_X86_OPERATION_CODE_SHIFT_ROTATE_LAST))
        {
            result = hyperdos_x86_execute_shift_rotate_instruction(processor,
                                                                   operationCode,
                                                                   hasSegmentOverride,
                                                                   segmentOverride);
        }
        else if (hyperdos_x86_operation_code_is_in_range(operationCode,
                                                         HYPERDOS_X86_OPERATION_CODE_LOOP_NOT_ZERO_AND_NOT_EQUAL,
                                                         HYPERDOS_X86_OPERATION_CODE_LOOP_NOT_ZERO))
        {
            int8_t   displacement = (int8_t)hyperdos_x86_fetch_instruction_byte(processor);
            uint16_t counterValue =
                    (uint16_t)(hyperdos_x86_read_general_register_word(processor,
                                                                       HYPERDOS_X86_GENERAL_REGISTER_COUNTER) -
                               1u);
            hyperdos_x86_write_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER, counterValue);
            if (counterValue != 0u)
            {
                int zero = (processor->flags & HYPERDOS_X86_FLAG_ZERO) != 0u;
                if (operationCode == HYPERDOS_X86_OPERATION_CODE_LOOP_NOT_ZERO ||
                    (operationCode == HYPERDOS_X86_OPERATION_CODE_LOOP_NOT_ZERO_AND_EQUAL && zero) ||
                    (operationCode == HYPERDOS_X86_OPERATION_CODE_LOOP_NOT_ZERO_AND_NOT_EQUAL && !zero))
                {
                    hyperdos_x86_jump_relative(processor, displacement);
                }
            }
        }
        else
        {
            switch (operationCode)
            {
            case HYPERDOS_X86_OPERATION_CODE_PUSH_EXTRA_SEGMENT:
                hyperdos_x86_push_word(processor, processor->segmentRegisters[HYPERDOS_X86_SEGMENT_REGISTER_EXTRA]);
                break;
            case HYPERDOS_X86_OPERATION_CODE_POP_EXTRA_SEGMENT:
                hyperdos_x86_set_segment_register(processor,
                                                  HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                  hyperdos_x86_pop_word(processor));
                break;
            case HYPERDOS_X86_OPERATION_CODE_PUSH_CODE_SEGMENT:
                hyperdos_x86_push_word(processor, processor->segmentRegisters[HYPERDOS_X86_SEGMENT_REGISTER_CODE]);
                break;
            case HYPERDOS_X86_OPERATION_CODE_PUSH_STACK_SEGMENT:
                hyperdos_x86_push_word(processor, processor->segmentRegisters[HYPERDOS_X86_SEGMENT_REGISTER_STACK]);
                break;
            case HYPERDOS_X86_OPERATION_CODE_POP_STACK_SEGMENT:
                hyperdos_x86_set_segment_register(processor,
                                                  HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                                  hyperdos_x86_pop_word(processor));
                startMaskableInterruptInhibit = 1;
                break;
            case HYPERDOS_X86_OPERATION_CODE_PUSH_DATA_SEGMENT:
                hyperdos_x86_push_word(processor, processor->segmentRegisters[HYPERDOS_X86_SEGMENT_REGISTER_DATA]);
                break;
            case HYPERDOS_X86_OPERATION_CODE_POP_DATA_SEGMENT:
                hyperdos_x86_set_segment_register(processor,
                                                  HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                                  hyperdos_x86_pop_word(processor));
                break;

            case HYPERDOS_X86_OPERATION_CODE_DECIMAL_ADJUST_AFTER_ADD:
                hyperdos_x86_execute_decimal_adjust_after_add_instruction(processor);
                break;

            case HYPERDOS_X86_OPERATION_CODE_DECIMAL_ADJUST_AFTER_SUBTRACT:
                hyperdos_x86_execute_decimal_adjust_after_subtract_instruction(processor);
                break;

            case HYPERDOS_X86_OPERATION_CODE_ASCII_ADJUST_AFTER_ADD:
                hyperdos_x86_execute_ascii_adjust_add_subtract_instruction(processor, 0);
                break;

            case HYPERDOS_X86_OPERATION_CODE_ASCII_ADJUST_AFTER_SUBTRACT:
                hyperdos_x86_execute_ascii_adjust_add_subtract_instruction(processor, 1);
                break;

            case HYPERDOS_X86_OPERATION_CODE_TEST_REGISTER_MEMORY_BYTE:
            case HYPERDOS_X86_OPERATION_CODE_TEST_REGISTER_MEMORY_WORD:
            {
                uint8_t registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
                uint8_t registerIndex      = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                                  HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
                int     isWord             = operationCode == HYPERDOS_X86_OPERATION_CODE_TEST_REGISTER_MEMORY_WORD;
                hyperdos_x86_decoded_operand operand    = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                                   registerMemoryByte,
                                                                                                   hasSegmentOverride,
                                                                                                   segmentOverride);
                uint16_t                     leftValue  = hyperdos_x86_read_operand_value(processor, &operand, isWord);
                uint16_t                     rightValue = isWord ? hyperdos_x86_read_general_register_word(
                                                       processor,
                                                       (hyperdos_x86_general_register_index)registerIndex)
                                                                 : hyperdos_x86_read_byte_register(processor, registerIndex);
                hyperdos_x86_test_values(processor,
                                         leftValue,
                                         rightValue,
                                         hyperdos_x86_operand_value_mask(isWord),
                                         hyperdos_x86_operand_sign_bit(isWord));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_EXCHANGE_REGISTER_MEMORY_BYTE:
            case HYPERDOS_X86_OPERATION_CODE_EXCHANGE_REGISTER_MEMORY_WORD:
            {
                uint8_t registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
                uint8_t registerIndex      = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                                  HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
                int     isWord             = operationCode == HYPERDOS_X86_OPERATION_CODE_EXCHANGE_REGISTER_MEMORY_WORD;
                hyperdos_x86_decoded_operand operand       = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                                   registerMemoryByte,
                                                                                                   hasSegmentOverride,
                                                                                                   segmentOverride);
                uint16_t                     registerValue = isWord ? hyperdos_x86_read_general_register_word(
                                                          processor,
                                                          (hyperdos_x86_general_register_index)registerIndex)
                                                                    : hyperdos_x86_read_byte_register(processor, registerIndex);
                uint16_t operandValue = hyperdos_x86_read_operand_value(processor, &operand, isWord);
                if (isWord)
                {
                    hyperdos_x86_write_general_register_word(processor,
                                                             (hyperdos_x86_general_register_index)registerIndex,
                                                             operandValue);
                }
                else
                {
                    hyperdos_x86_write_byte_register(processor, registerIndex, (uint8_t)operandValue);
                }
                hyperdos_x86_write_operand_value(processor, &operand, isWord, registerValue);
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_MOVE_SEGMENT_TO_REGISTER_MEMORY:
            {
                uint8_t registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
                uint8_t segmentRegister    = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                                    HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
                hyperdos_x86_decoded_operand operand = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                                   registerMemoryByte,
                                                                                                   hasSegmentOverride,
                                                                                                   segmentOverride);
                if (segmentRegister > HYPERDOS_X86_SEGMENT_REGISTER_DATA)
                {
                    result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
                    break;
                }
                hyperdos_x86_write_operand_value(processor, &operand, 1, processor->segmentRegisters[segmentRegister]);
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_LOAD_EFFECTIVE_ADDRESS:
                result = hyperdos_x86_execute_lea_instruction(processor, hasSegmentOverride, segmentOverride);
                break;

            case HYPERDOS_X86_OPERATION_CODE_MOVE_REGISTER_MEMORY_TO_SEGMENT:
            {
                uint8_t registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
                uint8_t segmentRegister    = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                                    HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
                hyperdos_x86_decoded_operand operand = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                                   registerMemoryByte,
                                                                                                   hasSegmentOverride,
                                                                                                   segmentOverride);
                if (segmentRegister > HYPERDOS_X86_SEGMENT_REGISTER_DATA)
                {
                    result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
                    break;
                }
                hyperdos_x86_set_segment_register(processor,
                                                  (hyperdos_x86_segment_register_index)segmentRegister,
                                                  hyperdos_x86_read_operand_value(processor, &operand, 1));
                if (segmentRegister == HYPERDOS_X86_SEGMENT_REGISTER_STACK)
                {
                    startMaskableInterruptInhibit = 1;
                }
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_POP_REGISTER_MEMORY:
            {
                uint8_t registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
                uint8_t operation          = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                              HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
                hyperdos_x86_decoded_operand operand = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                                   registerMemoryByte,
                                                                                                   hasSegmentOverride,
                                                                                                   segmentOverride);
                if (operation != HYPERDOS_X86_GROUP_FIVE_INCREMENT)
                {
                    result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
                    break;
                }
                hyperdos_x86_write_operand_value(processor, &operand, 1, hyperdos_x86_pop_word(processor));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_CONVERT_BYTE_TO_WORD:
            {
                uint8_t accumulatorLow = hyperdos_x86_read_byte_register(processor, 0u);
                hyperdos_x86_write_byte_register(processor,
                                                 HYPERDOS_X86_BYTE_REGISTER_HIGH_PART_BIT,
                                                 (accumulatorLow & HYPERDOS_X86_BYTE_SIGN_BIT) != 0u
                                                         ? HYPERDOS_X86_BYTE_MASK
                                                         : 0x00u);
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_CONVERT_WORD_TO_DOUBLE_WORD:
                hyperdos_x86_write_general_register_word(
                        processor,
                        HYPERDOS_X86_GENERAL_REGISTER_DATA,
                        (hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) &
                         HYPERDOS_X86_WORD_SIGN_BIT) != 0u
                                ? HYPERDOS_X86_WORD_MASK
                                : 0x0000u);
                break;

            case HYPERDOS_X86_OPERATION_CODE_CALL_FAR_IMMEDIATE:
            {
                uint16_t offset  = hyperdos_x86_fetch_instruction_word(processor);
                uint16_t segment = hyperdos_x86_fetch_instruction_word(processor);
                hyperdos_x86_push_word(processor, processor->segmentRegisters[HYPERDOS_X86_SEGMENT_REGISTER_CODE]);
                hyperdos_x86_push_word(processor, hyperdos_x86_read_instruction_pointer_word(processor));
                hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE, segment);
                hyperdos_x86_write_instruction_pointer_word(processor, offset);
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_PUSH_FLAGS:
                hyperdos_x86_write_flags_word(processor,
                                              hyperdos_x86_normalize_flags(hyperdos_x86_read_flags_word(processor)));
                hyperdos_x86_push_word(processor, hyperdos_x86_read_flags_word(processor));
                break;

            case HYPERDOS_X86_OPERATION_CODE_POP_FLAGS:
                hyperdos_x86_write_flags_word(processor,
                                              hyperdos_x86_normalize_flags(hyperdos_x86_pop_word(processor)));
                suppressTrapAfterInstruction = !trapWasEnabled;
                break;

            case HYPERDOS_X86_OPERATION_CODE_STORE_STATUS_TO_FLAGS:
            {
                uint16_t accumulator =
                        hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
                hyperdos_x86_write_flags_word(processor,
                                              hyperdos_x86_normalize_flags(
                                                      (uint16_t)((hyperdos_x86_read_flags_word(processor) &
                                                                  HYPERDOS_X86_FLAGS_HIGH_BYTE_MASK) |
                                                                 ((accumulator >> HYPERDOS_X86_BYTE_BIT_COUNT) &
                                                                  HYPERDOS_X86_FLAGS_LOW_BYTE_TRANSFER_MASK))));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_LOAD_STATUS_FROM_FLAGS:
                hyperdos_x86_write_byte_register(processor,
                                                 HYPERDOS_X86_BYTE_REGISTER_HIGH_PART_BIT,
                                                 (uint8_t)(processor->flags &
                                                           HYPERDOS_X86_FLAGS_LOW_BYTE_TRANSFER_MASK));
                break;

            case HYPERDOS_X86_OPERATION_CODE_WAIT:
                if (processor->coprocessorWaitHandler != NULL)
                {
                    result = processor->coprocessorWaitHandler(processor, processor->coprocessorContext);
                }
                break;

            case HYPERDOS_X86_OPERATION_CODE_MOVE_MEMORY_TO_ACCUMULATOR_BYTE:
            {
                uint16_t offset = hyperdos_x86_fetch_instruction_word(processor);
                hyperdos_x86_write_byte_register(
                        processor,
                        0u,
                        hyperdos_x86_read_memory_byte_fast(processor,
                                                           hasSegmentOverride ? segmentOverride
                                                                              : HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                                           offset));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_MOVE_MEMORY_TO_ACCUMULATOR_WORD:
            {
                uint16_t offset = hyperdos_x86_fetch_instruction_word(processor);
                hyperdos_x86_write_general_register_word(
                        processor,
                        HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                        hyperdos_x86_read_memory_word_fast(processor,
                                                           hasSegmentOverride ? segmentOverride
                                                                              : HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                                           offset));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_MOVE_ACCUMULATOR_TO_MEMORY_BYTE:
            {
                uint16_t offset = hyperdos_x86_fetch_instruction_word(processor);
                hyperdos_x86_write_memory_byte_fast(processor,
                                                    hasSegmentOverride ? segmentOverride
                                                                       : HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                                    offset,
                                                    hyperdos_x86_read_byte_register(processor, 0u));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_MOVE_ACCUMULATOR_TO_MEMORY_WORD:
            {
                uint16_t offset = hyperdos_x86_fetch_instruction_word(processor);
                hyperdos_x86_write_memory_word_fast(
                        processor,
                        hasSegmentOverride ? segmentOverride : HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                        offset,
                        hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_TEST_ACCUMULATOR_BYTE:
                hyperdos_x86_test_values(processor,
                                         hyperdos_x86_read_byte_register(processor, 0u),
                                         hyperdos_x86_fetch_instruction_byte(processor),
                                         HYPERDOS_X86_BYTE_MASK,
                                         HYPERDOS_X86_BYTE_SIGN_BIT);
                break;

            case HYPERDOS_X86_OPERATION_CODE_TEST_ACCUMULATOR_WORD:
                hyperdos_x86_test_values(
                        processor,
                        hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR),
                        hyperdos_x86_fetch_instruction_word(processor),
                        HYPERDOS_X86_WORD_MASK,
                        HYPERDOS_X86_WORD_SIGN_BIT);
                break;

            case HYPERDOS_X86_OPERATION_CODE_SHIFT_ROTATE_IMMEDIATE_BYTE:
            case HYPERDOS_X86_OPERATION_CODE_SHIFT_ROTATE_IMMEDIATE_WORD:
                if (!hyperdos_x86_supports_80186_instructions(processor))
                {
                    result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
                    break;
                }
                result = hyperdos_x86_execute_shift_rotate_instruction(processor,
                                                                       operationCode,
                                                                       hasSegmentOverride,
                                                                       segmentOverride);
                break;

            case HYPERDOS_X86_OPERATION_CODE_ENTER:
                if (!hyperdos_x86_supports_80186_instructions(processor))
                {
                    result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
                    break;
                }
                hyperdos_x86_execute_enter_instruction(processor);
                break;

            case HYPERDOS_X86_OPERATION_CODE_RETURN_NEAR_IMMEDIATE:
            {
                uint16_t stackByteCount = hyperdos_x86_fetch_instruction_word(processor);
                uint16_t stackPointer   = 0u;
                hyperdos_x86_write_instruction_pointer_word(processor, hyperdos_x86_pop_word(processor));
                stackPointer = hyperdos_x86_read_general_register_word(processor,
                                                                       HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
                hyperdos_x86_write_general_register_word(processor,
                                                         HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                                         (uint16_t)(stackPointer + stackByteCount));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_RETURN_NEAR:
                hyperdos_x86_write_instruction_pointer_word(processor, hyperdos_x86_pop_word(processor));
                break;

            case HYPERDOS_X86_OPERATION_CODE_LEAVE:
                if (!hyperdos_x86_supports_80186_instructions(processor))
                {
                    result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
                    break;
                }
                hyperdos_x86_write_general_register_word(
                        processor,
                        HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                        hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE_POINTER));
                hyperdos_x86_write_general_register_word(processor,
                                                         HYPERDOS_X86_GENERAL_REGISTER_BASE_POINTER,
                                                         hyperdos_x86_pop_word(processor));
                break;

            case HYPERDOS_X86_OPERATION_CODE_LOAD_EXTRA_SEGMENT_POINTER:
            case HYPERDOS_X86_OPERATION_CODE_LOAD_DATA_SEGMENT_POINTER:
            {
                uint8_t registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
                uint8_t registerIndex      = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                                  HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
                hyperdos_x86_decoded_operand operand = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                                   registerMemoryByte,
                                                                                                   hasSegmentOverride,
                                                                                                   segmentOverride);
                hyperdos_x86_segment_register_index
                        targetSegment = operationCode == HYPERDOS_X86_OPERATION_CODE_LOAD_EXTRA_SEGMENT_POINTER
                                                ? HYPERDOS_X86_SEGMENT_REGISTER_EXTRA
                                                : HYPERDOS_X86_SEGMENT_REGISTER_DATA;
                if (operand.isRegister)
                {
                    result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
                    break;
                }
                hyperdos_x86_write_general_register_word(processor,
                                                         (hyperdos_x86_general_register_index)registerIndex,
                                                         hyperdos_x86_read_memory_word_fast(processor,
                                                                                            operand.segmentRegister,
                                                                                            operand.offset));
                hyperdos_x86_set_segment_register(
                        processor,
                        targetSegment,
                        hyperdos_x86_read_memory_word_fast(processor,
                                                           operand.segmentRegister,
                                                           (uint16_t)(operand.offset +
                                                                      HYPERDOS_X86_FAR_POINTER_SEGMENT_OFFSET)));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_MOVE_IMMEDIATE_TO_REGISTER_MEMORY_BYTE:
            case HYPERDOS_X86_OPERATION_CODE_MOVE_IMMEDIATE_TO_REGISTER_MEMORY_WORD:
            {
                uint8_t registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
                uint8_t operation          = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                              HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
                int     isWord = operationCode == HYPERDOS_X86_OPERATION_CODE_MOVE_IMMEDIATE_TO_REGISTER_MEMORY_WORD;
                hyperdos_x86_decoded_operand operand = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                                   registerMemoryByte,
                                                                                                   hasSegmentOverride,
                                                                                                   segmentOverride);
                if (operation != HYPERDOS_X86_GROUP_THREE_TEST)
                {
                    result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
                    break;
                }
                hyperdos_x86_write_operand_value(processor,
                                                 &operand,
                                                 isWord,
                                                 isWord ? hyperdos_x86_fetch_instruction_word(processor)
                                                        : hyperdos_x86_fetch_instruction_byte(processor));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_RETURN_FAR_IMMEDIATE:
            {
                uint16_t stackByteCount = hyperdos_x86_fetch_instruction_word(processor);
                uint16_t stackPointer   = 0u;
                hyperdos_x86_write_instruction_pointer_word(processor, hyperdos_x86_pop_word(processor));
                hyperdos_x86_set_segment_register(processor,
                                                  HYPERDOS_X86_SEGMENT_REGISTER_CODE,
                                                  hyperdos_x86_pop_word(processor));
                stackPointer = hyperdos_x86_read_general_register_word(processor,
                                                                       HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
                hyperdos_x86_write_general_register_word(processor,
                                                         HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                                         (uint16_t)(stackPointer + stackByteCount));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_RETURN_FAR:
                hyperdos_x86_write_instruction_pointer_word(processor, hyperdos_x86_pop_word(processor));
                hyperdos_x86_set_segment_register(processor,
                                                  HYPERDOS_X86_SEGMENT_REGISTER_CODE,
                                                  hyperdos_x86_pop_word(processor));
                break;

            case HYPERDOS_X86_OPERATION_CODE_INTERRUPT_BREAKPOINT:
                suppressTrapAfterInstruction = 1;
                result = hyperdos_x86_execute_interrupt(processor, HYPERDOS_X86_INTERRUPT_TYPE_BREAKPOINT);
                break;

            case HYPERDOS_X86_OPERATION_CODE_INTERRUPT_IMMEDIATE:
                suppressTrapAfterInstruction = 1;
                result = hyperdos_x86_execute_interrupt(processor, hyperdos_x86_fetch_instruction_byte(processor));
                break;

            case HYPERDOS_X86_OPERATION_CODE_INTERRUPT_ON_OVERFLOW:
                if ((processor->flags & HYPERDOS_X86_FLAG_OVERFLOW) != 0u)
                {
                    suppressTrapAfterInstruction = 1;
                    result = hyperdos_x86_execute_interrupt(processor, HYPERDOS_X86_INTERRUPT_TYPE_OVERFLOW);
                }
                break;

            case HYPERDOS_X86_OPERATION_CODE_INTERRUPT_RETURN:
                hyperdos_x86_write_instruction_pointer_word(processor, hyperdos_x86_pop_word(processor));
                hyperdos_x86_set_segment_register(processor,
                                                  HYPERDOS_X86_SEGMENT_REGISTER_CODE,
                                                  hyperdos_x86_pop_word(processor));
                hyperdos_x86_write_flags_word(processor,
                                              hyperdos_x86_normalize_flags(hyperdos_x86_pop_word(processor)));
                suppressTrapAfterInstruction = !trapWasEnabled;
                break;

            case HYPERDOS_X86_OPERATION_CODE_ASCII_ADJUST_AFTER_MULTIPLY:
            case HYPERDOS_X86_OPERATION_CODE_ASCII_ADJUST_BEFORE_DIVIDE:
                result = hyperdos_x86_execute_ascii_adjust_multiply_divide_instruction(processor, operationCode);
                break;

            case HYPERDOS_X86_OPERATION_CODE_TRANSLATE_BYTE:
            {
                hyperdos_x86_segment_register_index translateSegment = hasSegmentOverride
                                                                               ? segmentOverride
                                                                               : HYPERDOS_X86_SEGMENT_REGISTER_DATA;
                uint16_t                            translateOffset =
                        (uint16_t)(hyperdos_x86_read_general_register_word(processor,
                                                                           HYPERDOS_X86_GENERAL_REGISTER_BASE) +
                                   hyperdos_x86_read_byte_register(processor, 0u));
                hyperdos_x86_write_byte_register(processor,
                                                 0u,
                                                 hyperdos_x86_read_memory_byte_fast(processor,
                                                                                    translateSegment,
                                                                                    translateOffset));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_JUMP_IF_COUNTER_ZERO:
            {
                int8_t displacement = (int8_t)hyperdos_x86_fetch_instruction_byte(processor);
                if (hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER) == 0u)
                {
                    hyperdos_x86_jump_relative(processor, displacement);
                }
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_INPUT_IMMEDIATE_BYTE:
                hyperdos_x86_write_byte_register(
                        processor,
                        0u,
                        hyperdos_x86_read_input_output_byte(processor, hyperdos_x86_fetch_instruction_byte(processor)));
                break;

            case HYPERDOS_X86_OPERATION_CODE_INPUT_IMMEDIATE_WORD:
            {
                uint16_t port     = hyperdos_x86_fetch_instruction_byte(processor);
                uint16_t lowByte  = hyperdos_x86_read_input_output_byte(processor, port);
                uint16_t highByte = hyperdos_x86_read_input_output_byte(processor,
                                                                        (uint16_t)(port + HYPERDOS_X86_BYTE_SIZE));
                hyperdos_x86_write_general_register_word(processor,
                                                         HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                         (uint16_t)(lowByte |
                                                                    (highByte << HYPERDOS_X86_BYTE_BIT_COUNT)));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_OUTPUT_IMMEDIATE_BYTE:
                hyperdos_x86_write_input_output_byte(processor,
                                                     hyperdos_x86_fetch_instruction_byte(processor),
                                                     hyperdos_x86_read_byte_register(processor, 0u));
                break;

            case HYPERDOS_X86_OPERATION_CODE_OUTPUT_IMMEDIATE_WORD:
            {
                uint16_t port = hyperdos_x86_fetch_instruction_byte(processor);
                uint16_t accumulator =
                        hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
                hyperdos_x86_write_input_output_byte(processor, port, (uint8_t)(accumulator & HYPERDOS_X86_BYTE_MASK));
                hyperdos_x86_write_input_output_byte(processor,
                                                     (uint16_t)(port + HYPERDOS_X86_BYTE_SIZE),
                                                     (uint8_t)(accumulator >> HYPERDOS_X86_BYTE_BIT_COUNT));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_CALL_RELATIVE:
            {
                int16_t displacement = (int16_t)hyperdos_x86_fetch_instruction_word(processor);
                hyperdos_x86_push_word(processor, hyperdos_x86_read_instruction_pointer_word(processor));
                hyperdos_x86_jump_relative(processor, displacement);
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_JUMP_RELATIVE:
                hyperdos_x86_jump_relative(processor, (int16_t)hyperdos_x86_fetch_instruction_word(processor));
                break;

            case HYPERDOS_X86_OPERATION_CODE_JUMP_FAR_IMMEDIATE:
            {
                uint16_t offset  = hyperdos_x86_fetch_instruction_word(processor);
                uint16_t segment = hyperdos_x86_fetch_instruction_word(processor);
                hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE, segment);
                hyperdos_x86_write_instruction_pointer_word(processor, offset);
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_JUMP_SHORT:
                hyperdos_x86_jump_relative(processor, (int8_t)hyperdos_x86_fetch_instruction_byte(processor));
                break;

            case HYPERDOS_X86_OPERATION_CODE_INPUT_FROM_DATA_BYTE:
                hyperdos_x86_write_byte_register(
                        processor,
                        0u,
                        hyperdos_x86_read_input_output_byte(
                                processor,
                                hyperdos_x86_read_general_register_word(processor,
                                                                        HYPERDOS_X86_GENERAL_REGISTER_DATA)));
                break;

            case HYPERDOS_X86_OPERATION_CODE_INPUT_FROM_DATA_WORD:
            {
                uint16_t port = hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA);
                uint16_t lowByte  = hyperdos_x86_read_input_output_byte(processor, port);
                uint16_t highByte = hyperdos_x86_read_input_output_byte(processor,
                                                                        (uint16_t)(port + HYPERDOS_X86_BYTE_SIZE));
                hyperdos_x86_write_general_register_word(processor,
                                                         HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                         (uint16_t)(lowByte |
                                                                    (highByte << HYPERDOS_X86_BYTE_BIT_COUNT)));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_OUTPUT_TO_DATA_BYTE:
                hyperdos_x86_write_input_output_byte(
                        processor,
                        hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA),
                        hyperdos_x86_read_byte_register(processor, 0u));
                break;

            case HYPERDOS_X86_OPERATION_CODE_OUTPUT_TO_DATA_WORD:
            {
                uint16_t port = hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA);
                uint16_t accumulator =
                        hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
                hyperdos_x86_write_input_output_byte(processor, port, (uint8_t)(accumulator & HYPERDOS_X86_BYTE_MASK));
                hyperdos_x86_write_input_output_byte(processor,
                                                     (uint16_t)(port + HYPERDOS_X86_BYTE_SIZE),
                                                     (uint8_t)(accumulator >> HYPERDOS_X86_BYTE_BIT_COUNT));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_HALT:
                processor->halted            = 1u;
                suppressTrapAfterInstruction = 1;
                break;

            case HYPERDOS_X86_OPERATION_CODE_COMPLEMENT_CARRY:
                processor->flags ^= HYPERDOS_X86_FLAG_CARRY;
                processor->flags |= HYPERDOS_X86_FLAG_RESERVED;
                break;

            case HYPERDOS_X86_OPERATION_CODE_GROUP_THREE_BYTE:
            case HYPERDOS_X86_OPERATION_CODE_GROUP_THREE_WORD:
                result = hyperdos_x86_execute_multiply_divide_instruction(processor,
                                                                          operationCode,
                                                                          hasSegmentOverride,
                                                                          segmentOverride);
                break;

            case HYPERDOS_X86_OPERATION_CODE_CLEAR_CARRY:
                processor->flags &= (uint16_t)~HYPERDOS_X86_FLAG_CARRY;
                processor->flags |= HYPERDOS_X86_FLAG_RESERVED;
                break;

            case HYPERDOS_X86_OPERATION_CODE_SET_CARRY:
                processor->flags |= HYPERDOS_X86_FLAG_CARRY | HYPERDOS_X86_FLAG_RESERVED;
                break;

            case HYPERDOS_X86_OPERATION_CODE_CLEAR_INTERRUPT:
                processor->flags &= (uint16_t)~HYPERDOS_X86_FLAG_INTERRUPT_ENABLE;
                processor->flags |= HYPERDOS_X86_FLAG_RESERVED;
                break;

            case HYPERDOS_X86_OPERATION_CODE_SET_INTERRUPT:
                processor->flags              |= HYPERDOS_X86_FLAG_INTERRUPT_ENABLE | HYPERDOS_X86_FLAG_RESERVED;
                startMaskableInterruptInhibit  = 1;
                break;

            case HYPERDOS_X86_OPERATION_CODE_CLEAR_DIRECTION:
                processor->flags &= (uint16_t)~HYPERDOS_X86_FLAG_DIRECTION;
                processor->flags |= HYPERDOS_X86_FLAG_RESERVED;
                break;

            case HYPERDOS_X86_OPERATION_CODE_SET_DIRECTION:
                processor->flags |= HYPERDOS_X86_FLAG_DIRECTION | HYPERDOS_X86_FLAG_RESERVED;
                break;

            case HYPERDOS_X86_OPERATION_CODE_INCREMENT_DECREMENT_BYTE:
            {
                uint8_t registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
                uint8_t operation          = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                              HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
                hyperdos_x86_decoded_operand operand;
                uint16_t                     value = 0u;
                if (operation == HYPERDOS_X86_GROUP_FOUR_UNUSED && hyperdos_x86_supports_80186_instructions(processor))
                {
                    result = hyperdos_x86_execute_unused_operation_code_interrupt(processor);
                    break;
                }
                operand = hyperdos_x86_decode_register_memory_operand(processor,
                                                                      registerMemoryByte,
                                                                      hasSegmentOverride,
                                                                      segmentOverride);
                value   = hyperdos_x86_read_operand_value(processor, &operand, 0);
                if (operation == HYPERDOS_X86_GROUP_FOUR_INCREMENT)
                {
                    hyperdos_x86_update_increment_decrement_flags(processor,
                                                                  HYPERDOS_X86_ARITHMETIC_LOGIC_ADD,
                                                                  value,
                                                                  HYPERDOS_X86_BYTE_MASK,
                                                                  HYPERDOS_X86_BYTE_SIGN_BIT);
                    hyperdos_x86_write_operand_value(processor, &operand, 0, (uint8_t)(value + 1u));
                }
                else if (operation == HYPERDOS_X86_GROUP_FOUR_DECREMENT)
                {
                    hyperdos_x86_update_increment_decrement_flags(processor,
                                                                  HYPERDOS_X86_ARITHMETIC_LOGIC_SUB,
                                                                  value,
                                                                  HYPERDOS_X86_BYTE_MASK,
                                                                  HYPERDOS_X86_BYTE_SIGN_BIT);
                    hyperdos_x86_write_operand_value(processor, &operand, 0, (uint8_t)(value - 1u));
                }
                else
                {
                    result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
                }
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_GROUP_FIVE:
                result = hyperdos_x86_execute_group_five_instruction(processor, hasSegmentOverride, segmentOverride);
                break;

            default:
                result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
                break;
            }
        }

        {
            int instructionCompleted = result != HYPERDOS_X86_EXECUTION_DIVIDE_ERROR;

            if (result == HYPERDOS_X86_EXECUTION_DIVIDE_ERROR)
            {
                hyperdos_x86_execution_result interruptResult =
                        processor->divideErrorReturnsToFaultingInstruction
                                ? hyperdos_x86_execute_interrupt_with_return_address(
                                          processor,
                                          HYPERDOS_X86_INTERRUPT_TYPE_DIVIDE_ERROR,
                                          processor->lastInstructionSegment,
                                          (uint16_t)processor->lastInstructionOffset)
                                : hyperdos_x86_execute_interrupt(processor, HYPERDOS_X86_INTERRUPT_TYPE_DIVIDE_ERROR);
                if (interruptResult == HYPERDOS_X86_EXECUTION_INTERRUPT_NOT_HANDLED)
                {
                    return result;
                }
                result = interruptResult;
            }

            if (instructionCompleted && result == HYPERDOS_X86_EXECUTION_OK)
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
                    result = hyperdos_x86_execute_interrupt(processor, HYPERDOS_X86_INTERRUPT_TYPE_SINGLE_STEP);
                }
            }
        }

        if (result != HYPERDOS_X86_EXECUTION_OK)
        {
            return result;
        }

        ++processor->executedInstructionCount;
    }

    return HYPERDOS_X86_EXECUTION_HALTED;
}

int hyperdos_x86_processor_accepts_maskable_interrupt(const hyperdos_x86_processor* processor)
{
    if (processor == NULL)
    {
        return 0;
    }
    return (processor->flags & HYPERDOS_X86_FLAG_INTERRUPT_ENABLE) != 0u &&
           processor->maskableInterruptInhibitCount == 0u;
}

hyperdos_x86_execution_result hyperdos_x86_request_maskable_interrupt(hyperdos_x86_processor* processor,
                                                                      uint8_t                 interruptNumber)
{
    if (processor == NULL)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }
    if (!hyperdos_x86_processor_accepts_maskable_interrupt(processor))
    {
        return HYPERDOS_X86_EXECUTION_OK;
    }
    processor->halted = 0u;
    return hyperdos_x86_execute_interrupt(processor, interruptNumber);
}

void hyperdos_x86_stop_processor(hyperdos_x86_processor* processor)
{
    if (processor == NULL)
    {
        return;
    }
    processor->halted = 1u;
}

uint32_t hyperdos_x86_get_physical_address(const hyperdos_x86_processor*       processor,
                                           hyperdos_x86_segment_register_index segmentRegister,
                                           uint16_t                            offset)
{
    if (processor == NULL || segmentRegister > HYPERDOS_X86_SEGMENT_REGISTER_DATA)
    {
        return 0;
    }
    return hyperdos_x86_physical_address_from_base(processor->segmentBases[segmentRegister], offset);
}

hyperdos_x86_execution_result hyperdos_x86_read_memory_byte(const hyperdos_x86_processor*       processor,
                                                            hyperdos_x86_segment_register_index segmentRegister,
                                                            uint16_t                            offset,
                                                            uint8_t*                            value)
{
    if (processor == NULL || value == NULL || segmentRegister > HYPERDOS_X86_SEGMENT_REGISTER_DATA)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }
    *value = hyperdos_x86_read_memory_byte_fast(processor, segmentRegister, offset);
    return HYPERDOS_X86_EXECUTION_OK;
}

hyperdos_x86_execution_result hyperdos_x86_write_memory_byte(hyperdos_x86_processor*             processor,
                                                             hyperdos_x86_segment_register_index segmentRegister,
                                                             uint16_t                            offset,
                                                             uint8_t                             value)
{
    if (processor == NULL || segmentRegister > HYPERDOS_X86_SEGMENT_REGISTER_DATA)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }
    hyperdos_x86_write_memory_byte_fast(processor, segmentRegister, offset, value);
    return HYPERDOS_X86_EXECUTION_OK;
}

uint8_t hyperdos_x86_read_input_output_byte(hyperdos_x86_processor* processor, uint16_t port)
{
    if (processor == NULL || processor->bus == NULL)
    {
        return HYPERDOS_X86_BYTE_MASK;
    }
    return hyperdos_bus_read_input_output_byte_or_open_bus(processor->bus, port);
}

void hyperdos_x86_write_input_output_byte(hyperdos_x86_processor* processor, uint16_t port, uint8_t value)
{
    if (processor == NULL || processor->bus == NULL)
    {
        return;
    }
    hyperdos_bus_write_input_output_byte_if_mapped(processor->bus, port, value);
}

uint32_t hyperdos_x86_get_general_register(const hyperdos_x86_processor*       processor,
                                           hyperdos_x86_general_register_index registerIndex)
{
    if (processor == NULL || registerIndex > HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX)
    {
        return 0;
    }
    return processor->generalRegisters[registerIndex];
}

void hyperdos_x86_set_general_register(hyperdos_x86_processor*             processor,
                                       hyperdos_x86_general_register_index registerIndex,
                                       uint32_t                            value)
{
    if (processor == NULL || registerIndex > HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX)
    {
        return;
    }
    processor->generalRegisters[registerIndex] = value;
}

uint16_t hyperdos_x86_get_segment_register(const hyperdos_x86_processor*       processor,
                                           hyperdos_x86_segment_register_index segmentRegister)
{
    if (processor == NULL || segmentRegister > HYPERDOS_X86_SEGMENT_REGISTER_DATA)
    {
        return 0;
    }
    return processor->segmentRegisters[segmentRegister];
}

void hyperdos_x86_set_segment_register(hyperdos_x86_processor*             processor,
                                       hyperdos_x86_segment_register_index segmentRegister,
                                       uint16_t                            value)
{
    if (processor == NULL || segmentRegister > HYPERDOS_X86_SEGMENT_REGISTER_DATA)
    {
        return;
    }
    processor->segmentRegisters[segmentRegister] = value;
    hyperdos_x86_store_segment_base(processor, segmentRegister);
}

const char* hyperdos_x86_execution_result_name(hyperdos_x86_execution_result result)
{
    switch (result)
    {
    case HYPERDOS_X86_EXECUTION_OK:
        return "ok";
    case HYPERDOS_X86_EXECUTION_HALTED:
        return "halted";
    case HYPERDOS_X86_EXECUTION_STEP_LIMIT_REACHED:
        return "step limit reached";
    case HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT:
        return "invalid argument";
    case HYPERDOS_X86_EXECUTION_PROGRAM_TOO_LARGE:
        return "program too large";
    case HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION:
        return "unsupported instruction";
    case HYPERDOS_X86_EXECUTION_INTERRUPT_NOT_HANDLED:
        return "interrupt not handled";
    case HYPERDOS_X86_EXECUTION_DIVIDE_ERROR:
        return "divide error";
    }
    return "unknown";
}
