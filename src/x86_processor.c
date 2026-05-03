#include "hyperdos/x86_processor.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "hyperdos/hardware.h"

enum
{
    HYPERDOS_X86_TOP_TWO_BIT_COUNT                               = 2u,
    HYPERDOS_X86_NIBBLE_MASK                                     = 0x0Fu,
    HYPERDOS_X86_LOW_TWO_BITS_MASK                               = 0x03u,
    HYPERDOS_X86_EXTENDED_WORD_MASK                              = 0x1FFFFu,
    HYPERDOS_X86_BYTE_SIGN_BIT                                   = 0x0080u,
    HYPERDOS_X86_WORD_SIGN_BIT                                   = 0x8000u,
    HYPERDOS_X86_SEGMENT_BYTE_COUNT                              = 0x10000u,
    HYPERDOS_X86_AUXILIARY_CARRY_BIT                             = 0x0010u,
    HYPERDOS_X86_BYTE_REGISTER_WORD_INDEX_MASK                   = 0x03u,
    HYPERDOS_X86_BYTE_REGISTER_HIGH_PART_BIT                     = 0x04u,
    HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK                     = 0x07u,
    HYPERDOS_X86_MODRM_REGISTER_MEMORY_MASK                      = 0x07u,
    HYPERDOS_X86_MODRM_REGISTER_SHIFT                            = 3u,
    HYPERDOS_X86_MODRM_MODE_SHIFT                                = 6u,
    HYPERDOS_X86_MODRM_MODE_MEMORY_NO_DISPLACEMENT               = 0u,
    HYPERDOS_X86_MODRM_MODE_MEMORY_BYTE_DISPLACEMENT             = 1u,
    HYPERDOS_X86_MODRM_MODE_MEMORY_WORD_DISPLACEMENT             = 2u,
    HYPERDOS_X86_MODRM_MODE_REGISTER                             = 3u,
    HYPERDOS_X86_ARITHMETIC_FORM_WORD_BIT                        = 0x01u,
    HYPERDOS_X86_ARITHMETIC_FORM_REGISTER_DESTINATION_BIT        = 0x02u,
    HYPERDOS_X86_ARITHMETIC_FORM_REGISTER_MEMORY_MAXIMUM         = 0x03u,
    HYPERDOS_X86_ARITHMETIC_FORM_ACCUMULATOR_BYTE                = 0x04u,
    HYPERDOS_X86_ARITHMETIC_FORM_ACCUMULATOR_WORD                = 0x05u,
    HYPERDOS_X86_OPERATION_CODE_WORD_BIT                         = 0x01u,
    HYPERDOS_X86_OPERATION_CODE_COUNT_IN_COUNTER_BIT             = 0x02u,
    HYPERDOS_X86_PARITY_FOLD_SHIFT                               = 4u,
    HYPERDOS_X86_PARITY_LOOKUP                                   = 0x6996u,
    HYPERDOS_X86_INTERRUPT_VECTOR_BYTE_COUNT                     = 4u,
    HYPERDOS_X86_FAR_POINTER_SEGMENT_OFFSET                      = 2u,
    HYPERDOS_X86_RESET_CODE_SEGMENT                              = 0xFFFFu,
    HYPERDOS_X86_RESET_INSTRUCTION_POINTER                       = 0x0000u,
    HYPERDOS_X86_REAL_MODE_SEGMENT_LIMIT                         = 0xFFFFu,
    HYPERDOS_X86_REAL_MODE_SEGMENT_ATTRIBUTES                    = 0x0000u,
    HYPERDOS_X86_DOS_PROGRAM_MAXIMUM_SIZE                        = 0xFF00u,
    HYPERDOS_X86_DOS_STACK_POINTER                               = 0xFFFEu,
    HYPERDOS_X86_DOS_TERMINATE_STUB_OFFSET                       = 0x0000u,
    HYPERDOS_X86_DOS_COMMAND_TAIL_LENGTH_OFFSET                  = 0x0080u,
    HYPERDOS_X86_DOS_COMMAND_TAIL_BYTES_OFFSET                   = 0x0081u,
    HYPERDOS_X86_DOS_COMMAND_TAIL_MAXIMUM_LENGTH                 = 126u,
    HYPERDOS_X86_ASCII_CARRIAGE_RETURN                           = 0x0Du,
    HYPERDOS_X86_INTERRUPT_TYPE_DIVIDE_ERROR                     = 0u,
    HYPERDOS_X86_INTERRUPT_TYPE_SINGLE_STEP                      = 1u,
    HYPERDOS_X86_INTERRUPT_TYPE_BREAKPOINT                       = 3u,
    HYPERDOS_X86_INTERRUPT_TYPE_OVERFLOW                         = 4u,
    HYPERDOS_X86_INTERRUPT_TYPE_BOUND_RANGE_EXCEEDED             = 5u,
    HYPERDOS_X86_INTERRUPT_TYPE_UNUSED_OPERATION_CODE            = 6u,
    HYPERDOS_X86_INTERRUPT_TYPE_ESCAPE_OPERATION_CODE            = 7u,
    HYPERDOS_X86_INTERRUPT_TYPE_DOUBLE_FAULT                     = 8u,
    HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT       = 10u,
    HYPERDOS_X86_INTERRUPT_TYPE_SEGMENT_NOT_PRESENT              = 11u,
    HYPERDOS_X86_INTERRUPT_TYPE_STACK_SEGMENT_FAULT              = 12u,
    HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT         = 13u,
    HYPERDOS_X86_FLAGS_LOW_BYTE_TRANSFER_MASK                    = 0x00D7u,
    HYPERDOS_X86_FLAGS_HIGH_BYTE_MASK                            = 0xFF00u,
    HYPERDOS_X86_FLAGS_WRITABLE_MASK                             = 0x0FD5u,
    HYPERDOS_X86_FLAGS_80286_IO_PRIVILEGE_LEVEL_MASK             = 0x3000u,
    HYPERDOS_X86_FLAGS_80286_NESTED_TASK                         = 0x4000u,
    HYPERDOS_X86_FLAGS_80286_PROTECTED_WRITABLE_MASK             = 0x7FD5u,
    HYPERDOS_X86_ENTER_NESTING_LEVEL_MASK                        = 0x1Fu,
    HYPERDOS_X86_80286_PHYSICAL_ADDRESS_MASK                     = 0xFFFFFFu,
    HYPERDOS_X86_80286_DESCRIPTOR_TABLE_STORED_HIGH_BYTE         = 0xFFu,
    HYPERDOS_X86_80286_DESCRIPTOR_TABLE_BASE_MASK                = 0x00FFFFFFu,
    HYPERDOS_X86_80286_DESCRIPTOR_TABLE_RESET_LIMIT              = 0x03FFu,
    HYPERDOS_X86_80286_DESCRIPTOR_BYTE_COUNT                     = 8u,
    HYPERDOS_X86_SELECTOR_REQUESTED_PRIVILEGE_MASK               = 0x0003u,
    HYPERDOS_X86_SELECTOR_TABLE_INDICATOR                        = 0x0004u,
    HYPERDOS_X86_SELECTOR_INDEX_SHIFT                            = 3u,
    HYPERDOS_X86_80286_ACCESS_ACCESSED                           = 0x01u,
    HYPERDOS_X86_80286_ACCESS_READ_WRITE                         = 0x02u,
    HYPERDOS_X86_80286_ACCESS_EXPAND_DOWN_CONFORMING             = 0x04u,
    HYPERDOS_X86_80286_ACCESS_EXECUTABLE                         = 0x08u,
    HYPERDOS_X86_80286_ACCESS_CODE_DATA                          = 0x10u,
    HYPERDOS_X86_80286_ACCESS_PRIVILEGE_SHIFT                    = 5u,
    HYPERDOS_X86_80286_ACCESS_PRESENT                            = 0x80u,
    HYPERDOS_X86_80286_SYSTEM_DESCRIPTOR_TYPE_MASK               = 0x1Fu,
    HYPERDOS_X86_80286_AVAILABLE_TASK_STATE_SEGMENT              = 0x01u,
    HYPERDOS_X86_80286_LOCAL_DESCRIPTOR_TABLE                    = 0x02u,
    HYPERDOS_X86_80286_BUSY_TASK_STATE_SEGMENT                   = 0x03u,
    HYPERDOS_X86_80286_CALL_GATE                                 = 0x04u,
    HYPERDOS_X86_80286_TASK_GATE                                 = 0x05u,
    HYPERDOS_X86_80286_INTERRUPT_GATE                            = 0x06u,
    HYPERDOS_X86_80286_TRAP_GATE                                 = 0x07u,
    HYPERDOS_X86_80286_CALL_GATE_PARAMETER_WORD_COUNT_MASK       = 0x1Fu,
    HYPERDOS_X86_80286_TASK_STATE_SEGMENT_MINIMUM_LIMIT          = 0x002Bu,
    HYPERDOS_X86_80286_TASK_STATE_SEGMENT_PREVIOUS_TASK          = 0x0000u,
    HYPERDOS_X86_80286_TASK_STATE_SEGMENT_STACK_POINTER_ZERO     = 0x0002u,
    HYPERDOS_X86_80286_TASK_STATE_SEGMENT_INSTRUCTION_POINTER    = 0x000Eu,
    HYPERDOS_X86_80286_TASK_STATE_SEGMENT_FLAGS                  = 0x0010u,
    HYPERDOS_X86_80286_TASK_STATE_SEGMENT_ACCUMULATOR            = 0x0012u,
    HYPERDOS_X86_80286_TASK_STATE_SEGMENT_EXTRA_SEGMENT          = 0x0022u,
    HYPERDOS_X86_80286_TASK_STATE_SEGMENT_LOCAL_DESCRIPTOR_TABLE = 0x002Au,
    HYPERDOS_X86_80286_ERROR_CODE_EXTERNAL_EVENT                 = 0x0001u,
    HYPERDOS_X86_80286_ERROR_CODE_INTERRUPT_DESCRIPTOR_TABLE     = 0x0002u,
    HYPERDOS_X86_80186_RELOCATION_REGISTER_RESET_VALUE           = 0x20FFu,
    HYPERDOS_X86_80186_RELOCATION_REGISTER_ESCAPE_TRAP           = 0x4000u,
    HYPERDOS_X86_MACHINE_STATUS_WORD_PROTECTION_ENABLE           = 0x0001u,
    HYPERDOS_X86_MACHINE_STATUS_WORD_MONITOR_PROCESSOR           = 0x0002u,
    HYPERDOS_X86_MACHINE_STATUS_WORD_EMULATE_PROCESSOR           = 0x0004u,
    HYPERDOS_X86_MACHINE_STATUS_WORD_TASK_SWITCHED               = 0x0008u,
    HYPERDOS_X86_MACHINE_STATUS_WORD_80286_MASK                  = 0x000Fu
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
    HYPERDOS_X86_OPERATION_CODE_ADJUST_REQUESTED_PRIVILEGE_LEVEL       = 0x63u,
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
    HYPERDOS_X86_ARCHITECTURE_GENERATION_80186,
    HYPERDOS_X86_ARCHITECTURE_GENERATION_80286
} hyperdos_x86_architecture_generation;

typedef enum hyperdos_x86_execution_mode_profile
{
    HYPERDOS_X86_EXECUTION_MODE_PROFILE_REAL_ONLY = 0u,
    HYPERDOS_X86_EXECUTION_MODE_PROFILE_REAL_AND_PROTECTED
} hyperdos_x86_execution_mode_profile;

typedef enum hyperdos_x86_external_bus_profile
{
    HYPERDOS_X86_EXTERNAL_BUS_PROFILE_8086 = 0u,
    HYPERDOS_X86_EXTERNAL_BUS_PROFILE_8088,
    HYPERDOS_X86_EXTERNAL_BUS_PROFILE_80186,
    HYPERDOS_X86_EXTERNAL_BUS_PROFILE_80188,
    HYPERDOS_X86_EXTERNAL_BUS_PROFILE_80286
} hyperdos_x86_external_bus_profile;

typedef enum hyperdos_x86_operand_address_profile
{
    HYPERDOS_X86_OPERAND_ADDRESS_PROFILE_16_BIT = 0u
} hyperdos_x86_operand_address_profile;

typedef enum hyperdos_x86_exception_model
{
    HYPERDOS_X86_EXCEPTION_MODEL_8086 = 0u,
    HYPERDOS_X86_EXCEPTION_MODEL_80186,
    HYPERDOS_X86_EXCEPTION_MODEL_80286
} hyperdos_x86_exception_model;

typedef enum hyperdos_x86_flags_high_bits_policy
{
    HYPERDOS_X86_FLAGS_HIGH_BITS_POLICY_FORCE_ONE = 0u,
    HYPERDOS_X86_FLAGS_HIGH_BITS_POLICY_FORCE_ZERO
} hyperdos_x86_flags_high_bits_policy;

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
    HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_POP_CODE_SEGMENT,
    HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_80286_SYSTEM_OPERATION_CODE_ESCAPE
} hyperdos_x86_operation_code_validity_action;

typedef enum hyperdos_x86_operation_prefix_rule
{
    HYPERDOS_X86_OPERATION_PREFIX_RULE_NONE = 0u,
    HYPERDOS_X86_OPERATION_PREFIX_RULE_EXTRA_SEGMENT,
    HYPERDOS_X86_OPERATION_PREFIX_RULE_CODE_SEGMENT,
    HYPERDOS_X86_OPERATION_PREFIX_RULE_STACK_SEGMENT,
    HYPERDOS_X86_OPERATION_PREFIX_RULE_DATA_SEGMENT,
    HYPERDOS_X86_OPERATION_PREFIX_RULE_REPEAT_WHILE_NOT_EQUAL,
    HYPERDOS_X86_OPERATION_PREFIX_RULE_REPEAT,
    HYPERDOS_X86_OPERATION_PREFIX_RULE_LOCK
} hyperdos_x86_operation_prefix_rule;

typedef enum hyperdos_x86_mod_register_memory_extension_validity_rule
{
    HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE = 0u,
    HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_GROUP_FOUR,
    HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_GROUP_FIVE
} hyperdos_x86_mod_register_memory_extension_validity_rule;

typedef struct hyperdos_x86_processor_descriptor
{
    hyperdos_x86_architecture_generation        architectureGeneration;
    hyperdos_x86_execution_mode_profile         executionModeProfile;
    hyperdos_x86_external_bus_profile           externalBusProfile;
    hyperdos_x86_operand_address_profile        operandAddressProfile;
    hyperdos_x86_exception_model                exceptionModel;
    uint32_t                                    physicalAddressMask;
    hyperdos_x86_flags_high_bits_policy         flagsHighBitsPolicy;
    hyperdos_x86_shift_rotate_count_policy      shiftRotateCountPolicy;
    hyperdos_x86_signed_divide_quotient_policy  signedDivideQuotientPolicy;
    hyperdos_x86_segment_boundary_access_policy segmentBoundaryAccessPolicy;
    hyperdos_x86_push_stack_pointer_policy      pushStackPointerPolicy;
} hyperdos_x86_processor_descriptor;

typedef struct hyperdos_x86_operation_code_entry
{
    uint8_t                                                  firstOperationCode;
    uint8_t                                                  lastOperationCode;
    hyperdos_x86_architecture_generation                     minimumArchitectureGeneration;
    hyperdos_x86_operation_prefix_rule                       prefixRule;
    hyperdos_x86_mod_register_memory_extension_validity_rule modRegisterMemoryExtensionValidityRule;
    hyperdos_x86_operation_code_validity_action              action8086ExceptionModel;
    hyperdos_x86_operation_code_validity_action              action80186ExceptionModel;
    hyperdos_x86_operation_code_validity_action              action80286ExceptionModel;
} hyperdos_x86_operation_code_entry;

typedef struct hyperdos_x86_external_bus_descriptor
{
    uint8_t dataBusByteCount;
    uint8_t prefetchQueueByteCount;
} hyperdos_x86_external_bus_descriptor;

typedef struct hyperdos_x86_decoded_instruction
{
    uint8_t                             operationCode;
    uint8_t                             operationMap;
    uint8_t                             hasRegisterMemoryByte;
    uint8_t                             registerMemoryByte;
    uint8_t                             operandSize;
    uint8_t                             addressSize;
    uint8_t                             segmentOverrideActive;
    hyperdos_x86_segment_register_index segmentOverride;
    hyperdos_x86_repeat_mode            repeatMode;
    uint8_t                             lockPrefixActive;
} hyperdos_x86_decoded_instruction;

typedef struct hyperdos_x86_decoded_operand
{
    uint8_t                             isRegister;
    uint8_t                             registerIndex;
    hyperdos_x86_segment_register_index segmentRegister;
    uint16_t                            offset;
} hyperdos_x86_decoded_operand;

typedef struct hyperdos_x86_80286_segment_descriptor
{
    uint32_t base;
    uint32_t limit;
    uint8_t  access;
} hyperdos_x86_80286_segment_descriptor;

typedef enum hyperdos_x86_memory_access_type
{
    HYPERDOS_X86_MEMORY_ACCESS_READ = 0u,
    HYPERDOS_X86_MEMORY_ACCESS_WRITE,
    HYPERDOS_X86_MEMORY_ACCESS_EXECUTE
} hyperdos_x86_memory_access_type;

typedef enum hyperdos_x86_80286_task_switch_type
{
    HYPERDOS_X86_80286_TASK_SWITCH_JUMP = 0u,
    HYPERDOS_X86_80286_TASK_SWITCH_CALL,
    HYPERDOS_X86_80286_TASK_SWITCH_INTERRUPT,
    HYPERDOS_X86_80286_TASK_SWITCH_RETURN
} hyperdos_x86_80286_task_switch_type;

typedef struct hyperdos_x86_80286_task_state
{
    uint16_t previousTaskLink;
    uint16_t instructionPointer;
    uint16_t flags;
    uint16_t generalRegisters[8];
    uint16_t segmentSelectors[4];
    uint16_t localDescriptorTableSelector;
} hyperdos_x86_80286_task_state;

static hyperdos_x86_execution_result hyperdos_x86_load_80286_local_descriptor_table(hyperdos_x86_processor* processor,
                                                                                    uint16_t                selector,
                                                                                    int* instructionCompleted);

// clang-format off
static const hyperdos_x86_processor_descriptor hyperdos_x86_processor_descriptors[] = {
    [HYPERDOS_X86_PROCESSOR_MODEL_8086] = {
        .architectureGeneration        = HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
        .executionModeProfile          = HYPERDOS_X86_EXECUTION_MODE_PROFILE_REAL_ONLY,
        .externalBusProfile            = HYPERDOS_X86_EXTERNAL_BUS_PROFILE_8086,
        .operandAddressProfile         = HYPERDOS_X86_OPERAND_ADDRESS_PROFILE_16_BIT,
        .exceptionModel                = HYPERDOS_X86_EXCEPTION_MODEL_8086,
        .physicalAddressMask           = HYPERDOS_X86_ADDRESS_MASK,
        .flagsHighBitsPolicy           = HYPERDOS_X86_FLAGS_HIGH_BITS_POLICY_FORCE_ONE,
        .shiftRotateCountPolicy        = HYPERDOS_X86_SHIFT_ROTATE_COUNT_POLICY_RAW,
        .signedDivideQuotientPolicy    = HYPERDOS_X86_SIGNED_DIVIDE_QUOTIENT_POLICY_EXCLUDE_MINIMUM_NEGATIVE,
        .segmentBoundaryAccessPolicy   = HYPERDOS_X86_SEGMENT_BOUNDARY_ACCESS_POLICY_WRAP_OFFSET,
        .pushStackPointerPolicy        = HYPERDOS_X86_PUSH_STACK_POINTER_POLICY_PUSH_DECREMENTED_VALUE,
    },
    [HYPERDOS_X86_PROCESSOR_MODEL_8088] = {
        .architectureGeneration        = HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
        .executionModeProfile          = HYPERDOS_X86_EXECUTION_MODE_PROFILE_REAL_ONLY,
        .externalBusProfile            = HYPERDOS_X86_EXTERNAL_BUS_PROFILE_8088,
        .operandAddressProfile         = HYPERDOS_X86_OPERAND_ADDRESS_PROFILE_16_BIT,
        .exceptionModel                = HYPERDOS_X86_EXCEPTION_MODEL_8086,
        .physicalAddressMask           = HYPERDOS_X86_ADDRESS_MASK,
        .flagsHighBitsPolicy           = HYPERDOS_X86_FLAGS_HIGH_BITS_POLICY_FORCE_ONE,
        .shiftRotateCountPolicy        = HYPERDOS_X86_SHIFT_ROTATE_COUNT_POLICY_RAW,
        .signedDivideQuotientPolicy    = HYPERDOS_X86_SIGNED_DIVIDE_QUOTIENT_POLICY_EXCLUDE_MINIMUM_NEGATIVE,
        .segmentBoundaryAccessPolicy   = HYPERDOS_X86_SEGMENT_BOUNDARY_ACCESS_POLICY_WRAP_OFFSET,
        .pushStackPointerPolicy        = HYPERDOS_X86_PUSH_STACK_POINTER_POLICY_PUSH_DECREMENTED_VALUE,
    },
    [HYPERDOS_X86_PROCESSOR_MODEL_80186] = {
        .architectureGeneration        = HYPERDOS_X86_ARCHITECTURE_GENERATION_80186,
        .executionModeProfile          = HYPERDOS_X86_EXECUTION_MODE_PROFILE_REAL_ONLY,
        .externalBusProfile            = HYPERDOS_X86_EXTERNAL_BUS_PROFILE_80186,
        .operandAddressProfile         = HYPERDOS_X86_OPERAND_ADDRESS_PROFILE_16_BIT,
        .exceptionModel                = HYPERDOS_X86_EXCEPTION_MODEL_80186,
        .physicalAddressMask           = HYPERDOS_X86_ADDRESS_MASK,
        .flagsHighBitsPolicy           = HYPERDOS_X86_FLAGS_HIGH_BITS_POLICY_FORCE_ONE,
        .shiftRotateCountPolicy        = HYPERDOS_X86_SHIFT_ROTATE_COUNT_POLICY_MASK_TO_FIVE_BITS,
        .signedDivideQuotientPolicy    = HYPERDOS_X86_SIGNED_DIVIDE_QUOTIENT_POLICY_ALLOW_MINIMUM_NEGATIVE,
        .segmentBoundaryAccessPolicy   = HYPERDOS_X86_SEGMENT_BOUNDARY_ACCESS_POLICY_LINEAR_NEXT_BYTE,
        .pushStackPointerPolicy        = HYPERDOS_X86_PUSH_STACK_POINTER_POLICY_PUSH_DECREMENTED_VALUE,
    },
    [HYPERDOS_X86_PROCESSOR_MODEL_80188] = {
        .architectureGeneration        = HYPERDOS_X86_ARCHITECTURE_GENERATION_80186,
        .executionModeProfile          = HYPERDOS_X86_EXECUTION_MODE_PROFILE_REAL_ONLY,
        .externalBusProfile            = HYPERDOS_X86_EXTERNAL_BUS_PROFILE_80188,
        .operandAddressProfile         = HYPERDOS_X86_OPERAND_ADDRESS_PROFILE_16_BIT,
        .exceptionModel                = HYPERDOS_X86_EXCEPTION_MODEL_80186,
        .physicalAddressMask           = HYPERDOS_X86_ADDRESS_MASK,
        .flagsHighBitsPolicy           = HYPERDOS_X86_FLAGS_HIGH_BITS_POLICY_FORCE_ONE,
        .shiftRotateCountPolicy        = HYPERDOS_X86_SHIFT_ROTATE_COUNT_POLICY_MASK_TO_FIVE_BITS,
        .signedDivideQuotientPolicy    = HYPERDOS_X86_SIGNED_DIVIDE_QUOTIENT_POLICY_ALLOW_MINIMUM_NEGATIVE,
        .segmentBoundaryAccessPolicy   = HYPERDOS_X86_SEGMENT_BOUNDARY_ACCESS_POLICY_LINEAR_NEXT_BYTE,
        .pushStackPointerPolicy        = HYPERDOS_X86_PUSH_STACK_POINTER_POLICY_PUSH_DECREMENTED_VALUE,
    },
    [HYPERDOS_X86_PROCESSOR_MODEL_80286] = {
        .architectureGeneration        = HYPERDOS_X86_ARCHITECTURE_GENERATION_80286,
        .executionModeProfile          = HYPERDOS_X86_EXECUTION_MODE_PROFILE_REAL_AND_PROTECTED,
        .externalBusProfile            = HYPERDOS_X86_EXTERNAL_BUS_PROFILE_80286,
        .operandAddressProfile         = HYPERDOS_X86_OPERAND_ADDRESS_PROFILE_16_BIT,
        .exceptionModel                = HYPERDOS_X86_EXCEPTION_MODEL_80286,
        .physicalAddressMask           = HYPERDOS_X86_80286_PHYSICAL_ADDRESS_MASK,
        .flagsHighBitsPolicy           = HYPERDOS_X86_FLAGS_HIGH_BITS_POLICY_FORCE_ZERO,
        .shiftRotateCountPolicy        = HYPERDOS_X86_SHIFT_ROTATE_COUNT_POLICY_MASK_TO_FIVE_BITS,
        .signedDivideQuotientPolicy    = HYPERDOS_X86_SIGNED_DIVIDE_QUOTIENT_POLICY_ALLOW_MINIMUM_NEGATIVE,
        .segmentBoundaryAccessPolicy   = HYPERDOS_X86_SEGMENT_BOUNDARY_ACCESS_POLICY_LINEAR_NEXT_BYTE,
        .pushStackPointerPolicy        = HYPERDOS_X86_PUSH_STACK_POINTER_POLICY_PUSH_ORIGINAL_VALUE,
    },
};
// clang-format on

static const hyperdos_x86_operation_code_entry hyperdos_x86_default_operation_code_entry = {
    .firstOperationCode                     = 0x00u,
    .lastOperationCode                      = 0xFFu,
    .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
    .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_NONE,
    .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE,
    .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
    .action80186ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
    .action80286ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
};

static const hyperdos_x86_operation_code_entry hyperdos_x86_operation_code_entries[] = {
    {
     .firstOperationCode                     = HYPERDOS_X86_OPERATION_CODE_PREFIX_EXTRA_SEGMENT,
     .lastOperationCode                      = HYPERDOS_X86_OPERATION_CODE_PREFIX_EXTRA_SEGMENT,
     .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
     .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_EXTRA_SEGMENT,
     .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE,
     .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80186ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80286ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     },
    {
     .firstOperationCode                     = HYPERDOS_X86_OPERATION_CODE_PREFIX_CODE_SEGMENT,
     .lastOperationCode                      = HYPERDOS_X86_OPERATION_CODE_PREFIX_CODE_SEGMENT,
     .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
     .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_CODE_SEGMENT,
     .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE,
     .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80186ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80286ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     },
    {
     .firstOperationCode                     = HYPERDOS_X86_OPERATION_CODE_PREFIX_STACK_SEGMENT,
     .lastOperationCode                      = HYPERDOS_X86_OPERATION_CODE_PREFIX_STACK_SEGMENT,
     .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
     .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_STACK_SEGMENT,
     .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE,
     .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80186ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80286ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     },
    {
     .firstOperationCode                     = HYPERDOS_X86_OPERATION_CODE_PREFIX_DATA_SEGMENT,
     .lastOperationCode                      = HYPERDOS_X86_OPERATION_CODE_PREFIX_DATA_SEGMENT,
     .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
     .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_DATA_SEGMENT,
     .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE,
     .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80186ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80286ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     },
    {
     .firstOperationCode                     = HYPERDOS_X86_OPERATION_CODE_POP_CODE_SEGMENT,
     .lastOperationCode                      = HYPERDOS_X86_OPERATION_CODE_POP_CODE_SEGMENT,
     .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
     .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_NONE,
     .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE,
     .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_POP_CODE_SEGMENT,
     .action80186ExceptionModel = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNUSED_OPERATION_CODE_INTERRUPT,
     .action80286ExceptionModel = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_80286_SYSTEM_OPERATION_CODE_ESCAPE,
     },
    {
     .firstOperationCode                     = 0x60u,
     .lastOperationCode                      = 0x62u,
     .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_80186,
     .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_NONE,
     .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE,
     .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80186ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80286ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     },
    {
     .firstOperationCode                     = HYPERDOS_X86_OPERATION_CODE_ADJUST_REQUESTED_PRIVILEGE_LEVEL,
     .lastOperationCode                      = HYPERDOS_X86_OPERATION_CODE_ADJUST_REQUESTED_PRIVILEGE_LEVEL,
     .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
     .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_NONE,
     .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE,
     .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNSUPPORTED_RESULT,
     .action80186ExceptionModel = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNUSED_OPERATION_CODE_INTERRUPT,
     .action80286ExceptionModel = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     },
    {
     .firstOperationCode                     = 0x64u,
     .lastOperationCode                      = 0x67u,
     .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
     .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_NONE,
     .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE,
     .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNSUPPORTED_RESULT,
     .action80186ExceptionModel = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNUSED_OPERATION_CODE_INTERRUPT,
     .action80286ExceptionModel = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNUSED_OPERATION_CODE_INTERRUPT,
     },
    {
     .firstOperationCode                     = HYPERDOS_X86_OPERATION_CODE_PUSH_IMMEDIATE_WORD,
     .lastOperationCode                      = HYPERDOS_X86_OPERATION_CODE_OUTPUT_STRING_WORD,
     .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_80186,
     .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_NONE,
     .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE,
     .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80186ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80286ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     },
    {
     .firstOperationCode                     = HYPERDOS_X86_OPERATION_CODE_SHIFT_ROTATE_IMMEDIATE_BYTE,
     .lastOperationCode                      = HYPERDOS_X86_OPERATION_CODE_SHIFT_ROTATE_IMMEDIATE_WORD,
     .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_80186,
     .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_NONE,
     .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE,
     .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80186ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80286ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     },
    {
     .firstOperationCode                     = HYPERDOS_X86_OPERATION_CODE_ENTER,
     .lastOperationCode                      = HYPERDOS_X86_OPERATION_CODE_LEAVE,
     .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_80186,
     .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_NONE,
     .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE,
     .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80186ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80286ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     },
    {
     .firstOperationCode                     = HYPERDOS_X86_OPERATION_CODE_PREFIX_LOCK,
     .lastOperationCode                      = HYPERDOS_X86_OPERATION_CODE_PREFIX_LOCK,
     .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
     .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_LOCK,
     .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE,
     .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80186ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80286ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     },
    {
     .firstOperationCode                     = 0xF1u,
     .lastOperationCode                      = 0xF1u,
     .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
     .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_NONE,
     .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE,
     .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNSUPPORTED_RESULT,
     .action80186ExceptionModel = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNUSED_OPERATION_CODE_INTERRUPT,
     .action80286ExceptionModel = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNUSED_OPERATION_CODE_INTERRUPT,
     },
    {
     .firstOperationCode                     = HYPERDOS_X86_OPERATION_CODE_PREFIX_REPEAT_WHILE_NOT_EQUAL,
     .lastOperationCode                      = HYPERDOS_X86_OPERATION_CODE_PREFIX_REPEAT_WHILE_NOT_EQUAL,
     .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
     .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_REPEAT_WHILE_NOT_EQUAL,
     .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE,
     .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80186ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80286ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     },
    {
     .firstOperationCode                     = HYPERDOS_X86_OPERATION_CODE_PREFIX_REPEAT,
     .lastOperationCode                      = HYPERDOS_X86_OPERATION_CODE_PREFIX_REPEAT,
     .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
     .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_REPEAT,
     .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE,
     .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80186ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80286ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     },
    {
     .firstOperationCode                     = HYPERDOS_X86_OPERATION_CODE_INCREMENT_DECREMENT_BYTE,
     .lastOperationCode                      = HYPERDOS_X86_OPERATION_CODE_INCREMENT_DECREMENT_BYTE,
     .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
     .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_NONE,
     .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_GROUP_FOUR,
     .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80186ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80286ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     },
    {
     .firstOperationCode                     = HYPERDOS_X86_OPERATION_CODE_GROUP_FIVE,
     .lastOperationCode                      = HYPERDOS_X86_OPERATION_CODE_GROUP_FIVE,
     .minimumArchitectureGeneration          = HYPERDOS_X86_ARCHITECTURE_GENERATION_8086,
     .prefixRule                             = HYPERDOS_X86_OPERATION_PREFIX_RULE_NONE,
     .modRegisterMemoryExtensionValidityRule = HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_GROUP_FIVE,
     .action8086ExceptionModel               = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80186ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     .action80286ExceptionModel              = HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_EXECUTE,
     },
};

static const hyperdos_x86_external_bus_descriptor hyperdos_x86_external_bus_descriptors[] = {
    [HYPERDOS_X86_EXTERNAL_BUS_PROFILE_8086] =
            {
                                                      .dataBusByteCount       = 2u,
                                                      .prefetchQueueByteCount = 6u,
                                                      },
    [HYPERDOS_X86_EXTERNAL_BUS_PROFILE_8088] =
            {
                                                      .dataBusByteCount       = 1u,
                                                      .prefetchQueueByteCount = 4u,
                                                      },
    [HYPERDOS_X86_EXTERNAL_BUS_PROFILE_80186] =
            {
                                                      .dataBusByteCount       = 2u,
                                                      .prefetchQueueByteCount = 6u,
                                                      },
    [HYPERDOS_X86_EXTERNAL_BUS_PROFILE_80188] =
            {
                                                      .dataBusByteCount       = 1u,
                                                      .prefetchQueueByteCount = 4u,
                                                      },
    [HYPERDOS_X86_EXTERNAL_BUS_PROFILE_80286] =
            {
                                                      .dataBusByteCount       = 2u,
                                                      .prefetchQueueByteCount = 6u,
                                                      },
};

static inline int hyperdos_x86_operation_code_is_in_range(uint8_t operationCode,
                                                          uint8_t firstOperationCode,
                                                          uint8_t lastOperationCode)
{
    return (uint8_t)(operationCode - firstOperationCode) <= (uint8_t)(lastOperationCode - firstOperationCode);
}

static const hyperdos_x86_operation_code_entry* hyperdos_x86_find_operation_code_entry(uint8_t operationCode)
{
    size_t operationCodeEntryIndex = 0u;

    for (operationCodeEntryIndex = 0u; operationCodeEntryIndex < sizeof(hyperdos_x86_operation_code_entries) /
                                                                         sizeof(hyperdos_x86_operation_code_entries[0]);
         ++operationCodeEntryIndex)
    {
        if (hyperdos_x86_operation_code_is_in_range(
                    operationCode,
                    hyperdos_x86_operation_code_entries[operationCodeEntryIndex].firstOperationCode,
                    hyperdos_x86_operation_code_entries[operationCodeEntryIndex].lastOperationCode))
        {
            return &hyperdos_x86_operation_code_entries[operationCodeEntryIndex];
        }
    }
    return &hyperdos_x86_default_operation_code_entry;
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

static inline uint8_t hyperdos_x86_fetch_instruction_byte(hyperdos_x86_processor* processor);

static inline void hyperdos_x86_push_word(hyperdos_x86_processor* processor, uint16_t value);

static inline const hyperdos_x86_processor_descriptor* hyperdos_x86_get_processor_descriptor(
        const hyperdos_x86_processor* processor);

static inline uint8_t hyperdos_x86_default_operand_address_size(const hyperdos_x86_processor* processor)
{
    const hyperdos_x86_processor_descriptor* processorDescriptor = hyperdos_x86_get_processor_descriptor(processor);
    switch (processorDescriptor->operandAddressProfile)
    {
    case HYPERDOS_X86_OPERAND_ADDRESS_PROFILE_16_BIT:
        return HYPERDOS_X86_WORD_BIT_COUNT;
    }
    return HYPERDOS_X86_WORD_BIT_COUNT;
}

static void hyperdos_x86_initialize_decoded_instruction(const hyperdos_x86_processor*     processor,
                                                        hyperdos_x86_decoded_instruction* instruction)
{
    memset(instruction, 0, sizeof(*instruction));
    instruction->operandSize     = hyperdos_x86_default_operand_address_size(processor);
    instruction->addressSize     = hyperdos_x86_default_operand_address_size(processor);
    instruction->segmentOverride = HYPERDOS_X86_SEGMENT_REGISTER_DATA;
    instruction->repeatMode      = HYPERDOS_X86_REPEAT_NONE;
}

static hyperdos_x86_execution_result hyperdos_x86_decode_next_instruction(hyperdos_x86_processor*           processor,
                                                                          hyperdos_x86_decoded_instruction* instruction)
{
    if (processor == NULL || instruction == NULL)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }

    hyperdos_x86_initialize_decoded_instruction(processor, instruction);
    for (;;)
    {
        const hyperdos_x86_operation_code_entry* operationCodeEntry = NULL;
        instruction->operationCode                                  = hyperdos_x86_fetch_instruction_byte(processor);
        operationCodeEntry = hyperdos_x86_find_operation_code_entry(instruction->operationCode);

        switch (operationCodeEntry->prefixRule)
        {
        case HYPERDOS_X86_OPERATION_PREFIX_RULE_EXTRA_SEGMENT:
            instruction->segmentOverrideActive = 1u;
            instruction->segmentOverride       = HYPERDOS_X86_SEGMENT_REGISTER_EXTRA;
            continue;
        case HYPERDOS_X86_OPERATION_PREFIX_RULE_CODE_SEGMENT:
            instruction->segmentOverrideActive = 1u;
            instruction->segmentOverride       = HYPERDOS_X86_SEGMENT_REGISTER_CODE;
            continue;
        case HYPERDOS_X86_OPERATION_PREFIX_RULE_STACK_SEGMENT:
            instruction->segmentOverrideActive = 1u;
            instruction->segmentOverride       = HYPERDOS_X86_SEGMENT_REGISTER_STACK;
            continue;
        case HYPERDOS_X86_OPERATION_PREFIX_RULE_DATA_SEGMENT:
            instruction->segmentOverrideActive = 1u;
            instruction->segmentOverride       = HYPERDOS_X86_SEGMENT_REGISTER_DATA;
            continue;
        case HYPERDOS_X86_OPERATION_PREFIX_RULE_REPEAT_WHILE_NOT_EQUAL:
            instruction->repeatMode = HYPERDOS_X86_REPEAT_WHILE_NOT_EQUAL;
            continue;
        case HYPERDOS_X86_OPERATION_PREFIX_RULE_REPEAT:
            instruction->repeatMode = HYPERDOS_X86_REPEAT_WHILE_EQUAL;
            continue;
        case HYPERDOS_X86_OPERATION_PREFIX_RULE_LOCK:
            instruction->lockPrefixActive = 1u;
            continue;
        case HYPERDOS_X86_OPERATION_PREFIX_RULE_NONE:
            break;
        }
        break;
    }
    return HYPERDOS_X86_EXECUTION_OK;
}

static inline const hyperdos_x86_processor_descriptor* hyperdos_x86_get_processor_descriptor(
        const hyperdos_x86_processor* processor);

static inline uint16_t hyperdos_x86_normalize_flags(const hyperdos_x86_processor* processor, uint16_t flags)
{
    const hyperdos_x86_processor_descriptor* processorDescriptor = hyperdos_x86_get_processor_descriptor(processor);
    uint16_t                                 fixedBits           = HYPERDOS_X86_FLAG_RESERVED;
    uint16_t                                 writableMask        = HYPERDOS_X86_FLAGS_WRITABLE_MASK;
    if (processorDescriptor->flagsHighBitsPolicy == HYPERDOS_X86_FLAGS_HIGH_BITS_POLICY_FORCE_ZERO)
    {
        fixedBits = 0x0002u;
        if (processorDescriptor->executionModeProfile == HYPERDOS_X86_EXECUTION_MODE_PROFILE_REAL_AND_PROTECTED &&
            processor != NULL &&
            (processor->machineStatusWord & HYPERDOS_X86_MACHINE_STATUS_WORD_PROTECTION_ENABLE) != 0u)
        {
            writableMask = HYPERDOS_X86_FLAGS_80286_PROTECTED_WRITABLE_MASK;
        }
    }
    return (uint16_t)((flags & writableMask) | fixedBits);
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

static inline int hyperdos_x86_supports_80286_instructions(const hyperdos_x86_processor* processor)
{
    const hyperdos_x86_processor_descriptor* processorDescriptor = hyperdos_x86_get_processor_descriptor(processor);
    return processorDescriptor->architectureGeneration >= HYPERDOS_X86_ARCHITECTURE_GENERATION_80286;
}

static inline int hyperdos_x86_supports_protected_mode(const hyperdos_x86_processor* processor)
{
    const hyperdos_x86_processor_descriptor* processorDescriptor = hyperdos_x86_get_processor_descriptor(processor);
    return processorDescriptor->executionModeProfile == HYPERDOS_X86_EXECUTION_MODE_PROFILE_REAL_AND_PROTECTED;
}

static inline int hyperdos_x86_processor_is_protected_mode_active(const hyperdos_x86_processor* processor)
{
    return hyperdos_x86_supports_protected_mode(processor) &&
           (processor->machineStatusWord & HYPERDOS_X86_MACHINE_STATUS_WORD_PROTECTION_ENABLE) != 0u;
}

static inline int hyperdos_x86_uses_80186_relocation_register(const hyperdos_x86_processor* processor)
{
    const hyperdos_x86_processor_descriptor* processorDescriptor = hyperdos_x86_get_processor_descriptor(processor);
    return processorDescriptor->architectureGeneration == HYPERDOS_X86_ARCHITECTURE_GENERATION_80186;
}

static inline uint16_t hyperdos_x86_model_relocation_register_reset_value(const hyperdos_x86_processor* processor)
{
    return hyperdos_x86_uses_80186_relocation_register(processor) ? HYPERDOS_X86_80186_RELOCATION_REGISTER_RESET_VALUE
                                                                  : 0u;
}

static inline int hyperdos_x86_80186_escape_trap_is_active(const hyperdos_x86_processor* processor)
{
    return hyperdos_x86_uses_80186_relocation_register(processor) &&
           (processor->relocationRegister & HYPERDOS_X86_80186_RELOCATION_REGISTER_ESCAPE_TRAP) != 0u;
}

static inline int hyperdos_x86_80286_processor_extension_trap_is_active(const hyperdos_x86_processor* processor,
                                                                        int                           isWaitInstruction)
{
    if (!hyperdos_x86_supports_80286_instructions(processor))
    {
        return 0;
    }
    if (!isWaitInstruction && (processor->machineStatusWord & HYPERDOS_X86_MACHINE_STATUS_WORD_EMULATE_PROCESSOR) != 0u)
    {
        return 1;
    }
    if ((processor->machineStatusWord & HYPERDOS_X86_MACHINE_STATUS_WORD_TASK_SWITCHED) == 0u)
    {
        return 0;
    }
    return !isWaitInstruction ||
           (processor->machineStatusWord & HYPERDOS_X86_MACHINE_STATUS_WORD_MONITOR_PROCESSOR) != 0u;
}

static hyperdos_x86_operation_code_validity_action hyperdos_x86_find_operation_code_validity_action(
        const hyperdos_x86_processor* processor,
        uint8_t                       operationCode)
{
    const hyperdos_x86_processor_descriptor* processorDescriptor = hyperdos_x86_get_processor_descriptor(processor);
    const hyperdos_x86_operation_code_entry* operationCodeEntry = hyperdos_x86_find_operation_code_entry(operationCode);

    if (processorDescriptor->architectureGeneration < operationCodeEntry->minimumArchitectureGeneration)
    {
        return HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_UNSUPPORTED_RESULT;
    }
    if (processorDescriptor->exceptionModel >= HYPERDOS_X86_EXCEPTION_MODEL_80286)
    {
        return operationCodeEntry->action80286ExceptionModel;
    }
    if (processorDescriptor->exceptionModel >= HYPERDOS_X86_EXCEPTION_MODEL_80186)
    {
        return operationCodeEntry->action80186ExceptionModel;
    }
    return operationCodeEntry->action8086ExceptionModel;
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

static inline uint32_t hyperdos_x86_get_physical_address_mask(const hyperdos_x86_processor* processor)
{
    const hyperdos_x86_processor_descriptor* processorDescriptor = hyperdos_x86_get_processor_descriptor(processor);
    return processorDescriptor->physicalAddressMask;
}

static inline uint32_t hyperdos_x86_physical_address_from_base(const hyperdos_x86_processor* processor,
                                                               uint32_t                      segmentBase,
                                                               uint16_t                      offset)
{
    return (segmentBase + offset) & hyperdos_x86_get_physical_address_mask(processor);
}

static inline uint32_t hyperdos_x86_real_mode_segment_base(uint16_t selector)
{
    return (uint32_t)selector << HYPERDOS_X86_SEGMENT_SHIFT;
}

static inline size_t hyperdos_x86_external_bus_descriptor_count(void)
{
    return sizeof(hyperdos_x86_external_bus_descriptors) / sizeof(hyperdos_x86_external_bus_descriptors[0]);
}

static inline const hyperdos_x86_external_bus_descriptor* hyperdos_x86_get_external_bus_descriptor(
        const hyperdos_x86_processor* processor)
{
    const hyperdos_x86_processor_descriptor* processorDescriptor = hyperdos_x86_get_processor_descriptor(processor);
    if ((size_t)processorDescriptor->externalBusProfile >= hyperdos_x86_external_bus_descriptor_count())
    {
        return &hyperdos_x86_external_bus_descriptors[HYPERDOS_X86_EXTERNAL_BUS_PROFILE_8086];
    }
    return &hyperdos_x86_external_bus_descriptors[processorDescriptor->externalBusProfile];
}

static inline void hyperdos_x86_observe_external_bus_access(const hyperdos_x86_processor* processor,
                                                            uint32_t                      physicalAddress,
                                                            size_t                        byteCount)
{
    const hyperdos_x86_external_bus_descriptor* externalBusDescriptor = NULL;
    uint64_t                                    busCycleCount         = 0u;

    if (processor == NULL || byteCount == 0u)
    {
        return;
    }

    externalBusDescriptor = hyperdos_x86_get_external_bus_descriptor(processor);
    if (externalBusDescriptor->dataBusByteCount <= 1u)
    {
        busCycleCount = byteCount;
    }
    else
    {
        size_t unalignedLeadingByteCount = physicalAddress & (uint32_t)(externalBusDescriptor->dataBusByteCount - 1u);
        busCycleCount = (unalignedLeadingByteCount + byteCount + externalBusDescriptor->dataBusByteCount - 1u) /
                        externalBusDescriptor->dataBusByteCount;
    }
    ((hyperdos_x86_processor*)processor)->externalBusCycleCount += busCycleCount;
}

static inline void hyperdos_x86_load_real_mode_segment(hyperdos_x86_processor*             processor,
                                                       hyperdos_x86_segment_register_index segmentRegister,
                                                       uint16_t                            selector)
{
    processor->segmentStates[segmentRegister].selector   = selector;
    processor->segmentStates[segmentRegister].base       = hyperdos_x86_real_mode_segment_base(selector);
    processor->segmentStates[segmentRegister].limit      = HYPERDOS_X86_REAL_MODE_SEGMENT_LIMIT;
    processor->segmentStates[segmentRegister].attributes = HYPERDOS_X86_REAL_MODE_SEGMENT_ATTRIBUTES;
}

static void hyperdos_x86_seed_real_address_segment_attributes_for_protected_mode(hyperdos_x86_processor* processor)
{
    processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_EXTRA].attributes = HYPERDOS_X86_80286_ACCESS_PRESENT |
                                                                               HYPERDOS_X86_80286_ACCESS_CODE_DATA |
                                                                               HYPERDOS_X86_80286_ACCESS_READ_WRITE;
    processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].attributes = HYPERDOS_X86_80286_ACCESS_PRESENT |
                                                                              HYPERDOS_X86_80286_ACCESS_CODE_DATA |
                                                                              HYPERDOS_X86_80286_ACCESS_EXECUTABLE |
                                                                              HYPERDOS_X86_80286_ACCESS_READ_WRITE;
    processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].attributes = HYPERDOS_X86_80286_ACCESS_PRESENT |
                                                                               HYPERDOS_X86_80286_ACCESS_CODE_DATA |
                                                                               HYPERDOS_X86_80286_ACCESS_READ_WRITE;
    processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_DATA].attributes = HYPERDOS_X86_80286_ACCESS_PRESENT |
                                                                              HYPERDOS_X86_80286_ACCESS_CODE_DATA |
                                                                              HYPERDOS_X86_80286_ACCESS_READ_WRITE;
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
    processor->flags = hyperdos_x86_normalize_flags(processor, value);
}

static inline void hyperdos_x86_normalize_current_flags(hyperdos_x86_processor* processor)
{
    hyperdos_x86_write_flags_word(processor, hyperdos_x86_read_flags_word(processor));
}

static inline uint8_t hyperdos_x86_read_physical_byte_unobserved(const hyperdos_x86_processor* processor,
                                                                 uint32_t                      physicalAddress)
{
    physicalAddress &= hyperdos_x86_get_physical_address_mask(processor);
    if (processor->bus != NULL)
    {
        return hyperdos_bus_read_memory_byte_or_open_bus(processor->bus, physicalAddress);
    }
    if (physicalAddress >= processor->memorySize)
    {
        return HYPERDOS_X86_BYTE_MASK;
    }
    return processor->memory[physicalAddress];
}

static inline uint8_t hyperdos_x86_read_physical_byte(const hyperdos_x86_processor* processor, uint32_t physicalAddress)
{
    physicalAddress &= hyperdos_x86_get_physical_address_mask(processor);
    hyperdos_x86_observe_external_bus_access(processor, physicalAddress, HYPERDOS_X86_BYTE_SIZE);
    return hyperdos_x86_read_physical_byte_unobserved(processor, physicalAddress);
}

static inline void hyperdos_x86_write_physical_byte_unobserved(hyperdos_x86_processor* processor,
                                                               uint32_t                physicalAddress,
                                                               uint8_t                 value)
{
    physicalAddress &= hyperdos_x86_get_physical_address_mask(processor);
    if (processor->bus != NULL)
    {
        hyperdos_bus_write_memory_byte_if_mapped(processor->bus, physicalAddress, value);
        return;
    }
    if (physicalAddress >= processor->memorySize)
    {
        return;
    }
    processor->memory[physicalAddress] = value;
}

static inline void hyperdos_x86_write_physical_byte(hyperdos_x86_processor* processor,
                                                    uint32_t                physicalAddress,
                                                    uint8_t                 value)
{
    physicalAddress &= hyperdos_x86_get_physical_address_mask(processor);
    hyperdos_x86_observe_external_bus_access(processor, physicalAddress, HYPERDOS_X86_BYTE_SIZE);
    hyperdos_x86_write_physical_byte_unobserved(processor, physicalAddress, value);
}

static inline uint16_t hyperdos_x86_read_physical_word(const hyperdos_x86_processor* processor,
                                                       uint32_t                      physicalAddress)
{
    uint16_t lowByte  = 0u;
    uint16_t highByte = 0u;

    physicalAddress &= hyperdos_x86_get_physical_address_mask(processor);
    hyperdos_x86_observe_external_bus_access(processor, physicalAddress, HYPERDOS_X86_WORD_SIZE);
    lowByte  = hyperdos_x86_read_physical_byte_unobserved(processor, physicalAddress);
    highByte = hyperdos_x86_read_physical_byte_unobserved(processor, physicalAddress + 1u);
    return (uint16_t)(lowByte | (highByte << HYPERDOS_X86_BYTE_BIT_COUNT));
}

static inline void hyperdos_x86_write_physical_word(hyperdos_x86_processor* processor,
                                                    uint32_t                physicalAddress,
                                                    uint16_t                value)
{
    physicalAddress &= hyperdos_x86_get_physical_address_mask(processor);
    hyperdos_x86_observe_external_bus_access(processor, physicalAddress, HYPERDOS_X86_WORD_SIZE);
    hyperdos_x86_write_physical_byte_unobserved(processor, physicalAddress, (uint8_t)(value & HYPERDOS_X86_BYTE_MASK));
    hyperdos_x86_write_physical_byte_unobserved(processor,
                                                physicalAddress + 1u,
                                                (uint8_t)(value >> HYPERDOS_X86_BYTE_BIT_COUNT));
}

static inline int hyperdos_x86_exception_is_80286_contributory(uint8_t exceptionType)
{
    switch (exceptionType)
    {
    case HYPERDOS_X86_INTERRUPT_TYPE_DIVIDE_ERROR:
    case HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT:
    case HYPERDOS_X86_INTERRUPT_TYPE_SEGMENT_NOT_PRESENT:
    case HYPERDOS_X86_INTERRUPT_TYPE_STACK_SEGMENT_FAULT:
    case HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT:
        return 1;
    default:
        return 0;
    }
}

static inline int hyperdos_x86_exceptions_create_80286_double_fault(uint8_t firstExceptionType,
                                                                    uint8_t secondExceptionType)
{
    return hyperdos_x86_exception_is_80286_contributory(firstExceptionType) &&
           hyperdos_x86_exception_is_80286_contributory(secondExceptionType);
}

static void hyperdos_x86_set_pending_exception(const hyperdos_x86_processor* processor,
                                               uint8_t                       exceptionType,
                                               int                           hasErrorCode,
                                               uint16_t                      errorCode)
{
    hyperdos_x86_processor* mutableProcessor = (hyperdos_x86_processor*)processor;

    if (mutableProcessor == NULL || mutableProcessor->processorShutdownActive != 0u)
    {
        return;
    }
    if (hyperdos_x86_processor_is_protected_mode_active(mutableProcessor))
    {
        if (mutableProcessor->exceptionDeliveryActive != 0u)
        {
            if (mutableProcessor->exceptionDeliveryType == HYPERDOS_X86_INTERRUPT_TYPE_DOUBLE_FAULT)
            {
                mutableProcessor->pendingExceptionActive       = 0u;
                mutableProcessor->pendingExceptionType         = 0u;
                mutableProcessor->pendingExceptionHasErrorCode = 0u;
                mutableProcessor->pendingExceptionErrorCode    = 0u;
                mutableProcessor->processorShutdownActive      = 1u;
                return;
            }
            if (hyperdos_x86_exceptions_create_80286_double_fault(mutableProcessor->exceptionDeliveryType,
                                                                  exceptionType))
            {
                exceptionType = HYPERDOS_X86_INTERRUPT_TYPE_DOUBLE_FAULT;
                hasErrorCode  = 1;
                errorCode     = 0u;
            }
        }
        else if (mutableProcessor->pendingExceptionActive != 0u)
        {
            if (hyperdos_x86_exceptions_create_80286_double_fault(mutableProcessor->pendingExceptionType,
                                                                  exceptionType))
            {
                exceptionType = HYPERDOS_X86_INTERRUPT_TYPE_DOUBLE_FAULT;
                hasErrorCode  = 1;
                errorCode     = 0u;
            }
            else
            {
                return;
            }
        }
    }
    else if (mutableProcessor->pendingExceptionActive != 0u)
    {
        return;
    }

    mutableProcessor->pendingExceptionActive       = 1u;
    mutableProcessor->pendingExceptionType         = exceptionType;
    mutableProcessor->pendingExceptionHasErrorCode = hasErrorCode != 0 ? 1u : 0u;
    mutableProcessor->pendingExceptionErrorCode    = errorCode;
}

static inline uint8_t hyperdos_x86_selector_requested_privilege_level(uint16_t selector)
{
    return (uint8_t)(selector & HYPERDOS_X86_SELECTOR_REQUESTED_PRIVILEGE_MASK);
}

static inline uint8_t hyperdos_x86_current_privilege_level(const hyperdos_x86_processor* processor)
{
    if (!hyperdos_x86_processor_is_protected_mode_active(processor))
    {
        return 0u;
    }
    return hyperdos_x86_selector_requested_privilege_level(
            processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].selector);
}

static inline uint8_t hyperdos_x86_current_io_privilege_level(const hyperdos_x86_processor* processor)
{
    return (uint8_t)((hyperdos_x86_read_flags_word(processor) & HYPERDOS_X86_FLAGS_80286_IO_PRIVILEGE_LEVEL_MASK) >>
                     12u);
}

static inline int hyperdos_x86_io_privilege_allows_current_level(const hyperdos_x86_processor* processor)
{
    return !hyperdos_x86_processor_is_protected_mode_active(processor) ||
           hyperdos_x86_current_privilege_level(processor) <= hyperdos_x86_current_io_privilege_level(processor);
}

static inline uint16_t hyperdos_x86_stack_flags_restore_mask(const hyperdos_x86_processor* processor)
{
    uint16_t writableMask = HYPERDOS_X86_FLAGS_WRITABLE_MASK;

    if (!hyperdos_x86_processor_is_protected_mode_active(processor))
    {
        return writableMask;
    }

    writableMask = HYPERDOS_X86_FLAGS_80286_PROTECTED_WRITABLE_MASK;
    if (hyperdos_x86_current_privilege_level(processor) != 0u)
    {
        writableMask = (uint16_t)(writableMask & (uint16_t)~HYPERDOS_X86_FLAGS_80286_IO_PRIVILEGE_LEVEL_MASK);
    }
    if (!hyperdos_x86_io_privilege_allows_current_level(processor))
    {
        writableMask = (uint16_t)(writableMask & (uint16_t)~HYPERDOS_X86_FLAG_INTERRUPT_ENABLE);
    }
    return writableMask;
}

static inline void hyperdos_x86_restore_flags_word_from_stack(hyperdos_x86_processor* processor, uint16_t value)
{
    uint16_t writableMask   = hyperdos_x86_stack_flags_restore_mask(processor);
    uint16_t currentFlags   = hyperdos_x86_read_flags_word(processor);
    uint16_t preservedFlags = (uint16_t)(currentFlags & (uint16_t)~writableMask);

    hyperdos_x86_write_flags_word(processor, (uint16_t)((value & writableMask) | preservedFlags));
}

static hyperdos_x86_execution_result hyperdos_x86_raise_general_protection_fault(
        const hyperdos_x86_processor* processor,
        uint16_t                      errorCode,
        int*                          instructionCompleted)
{
    if (instructionCompleted != NULL)
    {
        *instructionCompleted = 0;
    }
    hyperdos_x86_set_pending_exception(processor, HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT, 1, errorCode);
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_raise_protected_mode_exception(
        const hyperdos_x86_processor* processor,
        uint8_t                       exceptionType,
        uint16_t                      errorCode,
        int*                          instructionCompleted)
{
    if (instructionCompleted != NULL)
    {
        *instructionCompleted = 0;
    }
    hyperdos_x86_set_pending_exception(processor, exceptionType, 1, errorCode);
    return HYPERDOS_X86_EXECUTION_OK;
}

static inline uint8_t hyperdos_x86_80286_descriptor_privilege_level(
        const hyperdos_x86_80286_segment_descriptor* descriptor)
{
    return (uint8_t)((descriptor->access >> HYPERDOS_X86_80286_ACCESS_PRIVILEGE_SHIFT) &
                     HYPERDOS_X86_SELECTOR_REQUESTED_PRIVILEGE_MASK);
}

static inline int hyperdos_x86_80286_descriptor_is_present(const hyperdos_x86_80286_segment_descriptor* descriptor)
{
    return (descriptor->access & HYPERDOS_X86_80286_ACCESS_PRESENT) != 0u;
}

static inline int hyperdos_x86_80286_descriptor_is_code_or_data(const hyperdos_x86_80286_segment_descriptor* descriptor)
{
    return (descriptor->access & HYPERDOS_X86_80286_ACCESS_CODE_DATA) != 0u;
}

static inline int hyperdos_x86_80286_descriptor_is_executable(const hyperdos_x86_80286_segment_descriptor* descriptor)
{
    return (descriptor->access & HYPERDOS_X86_80286_ACCESS_EXECUTABLE) != 0u;
}

static inline int hyperdos_x86_80286_descriptor_is_expand_down(const hyperdos_x86_80286_segment_descriptor* descriptor)
{
    return !hyperdos_x86_80286_descriptor_is_executable(descriptor) &&
           (descriptor->access & HYPERDOS_X86_80286_ACCESS_EXPAND_DOWN_CONFORMING) != 0u;
}

static inline int hyperdos_x86_80286_descriptor_is_readable_or_writable(
        const hyperdos_x86_80286_segment_descriptor* descriptor)
{
    return (descriptor->access & HYPERDOS_X86_80286_ACCESS_READ_WRITE) != 0u;
}

static inline int hyperdos_x86_80286_descriptor_is_conforming_code(
        const hyperdos_x86_80286_segment_descriptor* descriptor)
{
    return hyperdos_x86_80286_descriptor_is_executable(descriptor) &&
           (descriptor->access & HYPERDOS_X86_80286_ACCESS_EXPAND_DOWN_CONFORMING) != 0u;
}

static inline int hyperdos_x86_privilege_level_allows_data_access(uint8_t currentPrivilegeLevel,
                                                                  uint8_t requestedPrivilegeLevel,
                                                                  uint8_t descriptorPrivilegeLevel)
{
    uint8_t effectivePrivilegeLevel = currentPrivilegeLevel > requestedPrivilegeLevel ? currentPrivilegeLevel
                                                                                      : requestedPrivilegeLevel;
    return effectivePrivilegeLevel <= descriptorPrivilegeLevel;
}

static int hyperdos_x86_protected_segment_range_is_valid(const hyperdos_x86_segment_state* segmentState,
                                                         uint16_t                          offset,
                                                         size_t                            byteCount)
{
    hyperdos_x86_80286_segment_descriptor descriptor;
    uint32_t                              highestOffset = 0u;

    if (segmentState == NULL || byteCount == 0u)
    {
        return 1;
    }
    if (byteCount > HYPERDOS_X86_SEGMENT_BYTE_COUNT)
    {
        return 0;
    }

    highestOffset = (uint32_t)offset + (uint32_t)byteCount - 1u;
    if (highestOffset > HYPERDOS_X86_WORD_MASK)
    {
        return 0;
    }

    descriptor.base   = segmentState->base;
    descriptor.limit  = segmentState->limit;
    descriptor.access = (uint8_t)segmentState->attributes;
    if (hyperdos_x86_80286_descriptor_is_expand_down(&descriptor))
    {
        return offset > descriptor.limit;
    }
    return highestOffset <= descriptor.limit;
}

static int hyperdos_x86_protected_segment_allows_access(const hyperdos_x86_segment_state* segmentState,
                                                        hyperdos_x86_memory_access_type   accessType)
{
    hyperdos_x86_80286_segment_descriptor descriptor;

    if (segmentState == NULL)
    {
        return 0;
    }
    descriptor.base   = segmentState->base;
    descriptor.limit  = segmentState->limit;
    descriptor.access = (uint8_t)segmentState->attributes;

    if (accessType == HYPERDOS_X86_MEMORY_ACCESS_EXECUTE)
    {
        return hyperdos_x86_80286_descriptor_is_executable(&descriptor);
    }
    if (!hyperdos_x86_80286_descriptor_is_code_or_data(&descriptor))
    {
        return 0;
    }
    if (accessType == HYPERDOS_X86_MEMORY_ACCESS_WRITE)
    {
        return !hyperdos_x86_80286_descriptor_is_executable(&descriptor) &&
               hyperdos_x86_80286_descriptor_is_readable_or_writable(&descriptor);
    }
    return !hyperdos_x86_80286_descriptor_is_executable(&descriptor) ||
           hyperdos_x86_80286_descriptor_is_readable_or_writable(&descriptor);
}

static int hyperdos_x86_80286_stack_state_can_push_words(const hyperdos_x86_segment_state* stackState,
                                                         uint16_t                          stackPointer,
                                                         size_t                            wordCount)
{
    uint16_t firstWrittenOffset = 0u;
    size_t   byteCount          = wordCount * HYPERDOS_X86_WORD_SIZE;

    if (wordCount == 0u)
    {
        return 1;
    }
    if (wordCount > HYPERDOS_X86_SEGMENT_BYTE_COUNT / HYPERDOS_X86_WORD_SIZE)
    {
        return 0;
    }

    firstWrittenOffset = (uint16_t)(stackPointer - (uint16_t)byteCount);
    return hyperdos_x86_protected_segment_allows_access(stackState, HYPERDOS_X86_MEMORY_ACCESS_WRITE) &&
           hyperdos_x86_protected_segment_range_is_valid(stackState, firstWrittenOffset, byteCount);
}

static int hyperdos_x86_80286_current_stack_can_push_words(const hyperdos_x86_processor* processor, size_t wordCount)
{
    return hyperdos_x86_80286_stack_state_can_push_words(
            &processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK],
            hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER),
            wordCount);
}

static int hyperdos_x86_80286_stack_descriptor_can_push_words(const hyperdos_x86_80286_segment_descriptor* descriptor,
                                                              uint16_t                                     selector,
                                                              uint16_t                                     stackPointer,
                                                              size_t                                       wordCount)
{
    hyperdos_x86_segment_state stackState;

    stackState.selector   = selector;
    stackState.base       = descriptor->base & HYPERDOS_X86_80286_DESCRIPTOR_TABLE_BASE_MASK;
    stackState.limit      = descriptor->limit;
    stackState.attributes = descriptor->access;
    return hyperdos_x86_80286_stack_state_can_push_words(&stackState, stackPointer, wordCount);
}

static int hyperdos_x86_protected_memory_access_is_valid(const hyperdos_x86_processor*       processor,
                                                         hyperdos_x86_segment_register_index segmentRegister,
                                                         uint16_t                            offset,
                                                         size_t                              byteCount,
                                                         hyperdos_x86_memory_access_type     accessType)
{
    const hyperdos_x86_segment_state* segmentState  = NULL;
    uint8_t                           exceptionType = segmentRegister == HYPERDOS_X86_SEGMENT_REGISTER_STACK
                                                              ? HYPERDOS_X86_INTERRUPT_TYPE_STACK_SEGMENT_FAULT
                                                              : HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT;

    if (!hyperdos_x86_processor_is_protected_mode_active(processor))
    {
        return 1;
    }
    segmentState = &processor->segmentStates[segmentRegister];
    if (!hyperdos_x86_protected_segment_allows_access(segmentState, accessType) ||
        !hyperdos_x86_protected_segment_range_is_valid(segmentState, offset, byteCount))
    {
        hyperdos_x86_set_pending_exception(processor, exceptionType, 1, 0u);
        return 0;
    }
    return 1;
}

static hyperdos_x86_execution_result hyperdos_x86_get_80286_descriptor_address(const hyperdos_x86_processor* processor,
                                                                               uint16_t                      selector,
                                                                               uint32_t* descriptorAddress)
{
    const hyperdos_x86_descriptor_table_state* descriptorTable = NULL;
    uint32_t                                   descriptorIndex = selector >> HYPERDOS_X86_SELECTOR_INDEX_SHIFT;
    uint32_t descriptorOffset = descriptorIndex * HYPERDOS_X86_80286_DESCRIPTOR_BYTE_COUNT;

    if (processor == NULL || descriptorAddress == NULL || descriptorIndex == 0u)
    {
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }

    descriptorTable = (selector & HYPERDOS_X86_SELECTOR_TABLE_INDICATOR) != 0u ? &processor->localDescriptorTable
                                                                               : &processor->globalDescriptorTable;
    if (descriptorOffset > descriptorTable->limit ||
        descriptorOffset + HYPERDOS_X86_80286_DESCRIPTOR_BYTE_COUNT - 1u > descriptorTable->limit)
    {
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }

    *descriptorAddress = descriptorTable->base + descriptorOffset;
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_read_80286_segment_descriptor(
        const hyperdos_x86_processor*          processor,
        uint16_t                               selector,
        hyperdos_x86_80286_segment_descriptor* descriptor)
{
    uint32_t descriptorAddress = 0u;

    if (processor == NULL || descriptor == NULL)
    {
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }

    if (hyperdos_x86_get_80286_descriptor_address(processor, selector, &descriptorAddress) != HYPERDOS_X86_EXECUTION_OK)
    {
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }

    descriptor->limit = hyperdos_x86_read_physical_word(processor, descriptorAddress);
    descriptor->base  = (uint32_t)hyperdos_x86_read_physical_word(processor, descriptorAddress + 2u) |
                       ((uint32_t)hyperdos_x86_read_physical_byte(processor, descriptorAddress + 4u) << 16u);
    descriptor->access = hyperdos_x86_read_physical_byte(processor, descriptorAddress + 5u);
    return HYPERDOS_X86_EXECUTION_OK;
}

static inline uint16_t hyperdos_x86_selector_with_requested_privilege_level(uint16_t selector,
                                                                            uint8_t  requestedPrivilegeLevel)
{
    return (uint16_t)((selector & (uint16_t)~HYPERDOS_X86_SELECTOR_REQUESTED_PRIVILEGE_MASK) |
                      (requestedPrivilegeLevel & HYPERDOS_X86_SELECTOR_REQUESTED_PRIVILEGE_MASK));
}

static inline uint16_t hyperdos_x86_80286_selector_error_code(uint16_t selector, int externalEvent)
{
    uint16_t errorCode = (uint16_t)((selector & 0xFFF8u) | (selector & HYPERDOS_X86_SELECTOR_TABLE_INDICATOR));
    if (externalEvent)
    {
        errorCode = (uint16_t)(errorCode | HYPERDOS_X86_80286_ERROR_CODE_EXTERNAL_EVENT);
    }
    return errorCode;
}

static inline uint16_t hyperdos_x86_80286_interrupt_descriptor_error_code(uint8_t interruptNumber, int externalEvent)
{
    uint16_t errorCode = (uint16_t)((uint16_t)interruptNumber * HYPERDOS_X86_80286_DESCRIPTOR_BYTE_COUNT);
    errorCode          = (uint16_t)(errorCode | HYPERDOS_X86_80286_ERROR_CODE_INTERRUPT_DESCRIPTOR_TABLE);
    if (externalEvent)
    {
        errorCode = (uint16_t)(errorCode | HYPERDOS_X86_80286_ERROR_CODE_EXTERNAL_EVENT);
    }
    return errorCode;
}

static void hyperdos_x86_load_80286_segment_state_from_descriptor(
        hyperdos_x86_processor*                      processor,
        hyperdos_x86_segment_register_index          segmentRegister,
        uint16_t                                     selector,
        const hyperdos_x86_80286_segment_descriptor* descriptor)
{
    uint8_t  access            = descriptor->access;
    uint32_t descriptorAddress = 0u;

    if (hyperdos_x86_80286_descriptor_is_code_or_data(descriptor) &&
        (access & HYPERDOS_X86_80286_ACCESS_ACCESSED) == 0u &&
        hyperdos_x86_get_80286_descriptor_address(processor, selector, &descriptorAddress) == HYPERDOS_X86_EXECUTION_OK)
    {
        access = (uint8_t)(access | HYPERDOS_X86_80286_ACCESS_ACCESSED);
        hyperdos_x86_write_physical_byte(processor, descriptorAddress + 5u, access);
    }

    processor->segmentStates[segmentRegister].selector = selector;
    processor->segmentStates[segmentRegister].base  = descriptor->base & HYPERDOS_X86_80286_DESCRIPTOR_TABLE_BASE_MASK;
    processor->segmentStates[segmentRegister].limit = descriptor->limit;
    processor->segmentStates[segmentRegister].attributes = access;
}

static int hyperdos_x86_read_80286_task_stack(const hyperdos_x86_processor* processor,
                                              uint8_t                       privilegeLevel,
                                              uint16_t*                     stackPointer,
                                              uint16_t*                     stackSelector)
{
    uint32_t stackPointerOffset = 2u + (uint32_t)privilegeLevel * 4u;

    if (processor == NULL || stackPointer == NULL || stackSelector == NULL ||
        privilegeLevel > HYPERDOS_X86_SELECTOR_REQUESTED_PRIVILEGE_MASK || processor->taskRegisterSelector == 0u ||
        processor->taskRegister.limit < stackPointerOffset + 3u)
    {
        return 0;
    }

    *stackPointer  = hyperdos_x86_read_physical_word(processor, processor->taskRegister.base + stackPointerOffset);
    *stackSelector = hyperdos_x86_read_physical_word(processor, processor->taskRegister.base + stackPointerOffset + 2u);
    return 1;
}

static int hyperdos_x86_validate_80286_stack_segment_for_privilege(hyperdos_x86_processor* processor,
                                                                   uint16_t                selector,
                                                                   uint8_t                 privilegeLevel,
                                                                   hyperdos_x86_80286_segment_descriptor* descriptor)
{
    if ((selector & 0xFFF8u) == 0u || hyperdos_x86_selector_requested_privilege_level(selector) != privilegeLevel ||
        hyperdos_x86_read_80286_segment_descriptor(processor, selector, descriptor) != HYPERDOS_X86_EXECUTION_OK ||
        hyperdos_x86_80286_descriptor_is_executable(descriptor) ||
        !hyperdos_x86_80286_descriptor_is_code_or_data(descriptor) ||
        !hyperdos_x86_80286_descriptor_is_readable_or_writable(descriptor) ||
        hyperdos_x86_80286_descriptor_privilege_level(descriptor) != privilegeLevel ||
        !hyperdos_x86_80286_descriptor_is_present(descriptor))
    {
        return 0;
    }
    return 1;
}

static int hyperdos_x86_load_80286_stack_segment_for_privilege(hyperdos_x86_processor* processor,
                                                               uint16_t                selector,
                                                               uint8_t                 privilegeLevel)
{
    hyperdos_x86_80286_segment_descriptor descriptor;

    if (!hyperdos_x86_validate_80286_stack_segment_for_privilege(processor, selector, privilegeLevel, &descriptor))
    {
        return 0;
    }
    hyperdos_x86_load_80286_segment_state_from_descriptor(processor,
                                                          HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                                          selector,
                                                          &descriptor);
    return 1;
}

static inline uint8_t hyperdos_x86_80286_system_descriptor_type(const hyperdos_x86_80286_segment_descriptor* descriptor)
{
    return (uint8_t)(descriptor->access & HYPERDOS_X86_80286_SYSTEM_DESCRIPTOR_TYPE_MASK);
}

static inline int hyperdos_x86_80286_descriptor_is_task_state_segment(
        const hyperdos_x86_80286_segment_descriptor* descriptor)
{
    uint8_t descriptorType = hyperdos_x86_80286_system_descriptor_type(descriptor);
    return descriptorType == HYPERDOS_X86_80286_AVAILABLE_TASK_STATE_SEGMENT ||
           descriptorType == HYPERDOS_X86_80286_BUSY_TASK_STATE_SEGMENT;
}

static void hyperdos_x86_clear_80286_segment_state(hyperdos_x86_processor*             processor,
                                                   hyperdos_x86_segment_register_index segmentRegister)
{
    processor->segmentStates[segmentRegister].selector   = 0u;
    processor->segmentStates[segmentRegister].base       = 0u;
    processor->segmentStates[segmentRegister].limit      = 0u;
    processor->segmentStates[segmentRegister].attributes = 0u;
}

static hyperdos_x86_execution_result hyperdos_x86_get_80286_descriptor_address_from_tables(
        const hyperdos_x86_processor*              processor,
        uint16_t                                   selector,
        const hyperdos_x86_descriptor_table_state* localDescriptorTable,
        uint32_t*                                  descriptorAddress)
{
    const hyperdos_x86_descriptor_table_state* descriptorTable = NULL;
    uint32_t                                   descriptorIndex = selector >> HYPERDOS_X86_SELECTOR_INDEX_SHIFT;
    uint32_t descriptorOffset = descriptorIndex * HYPERDOS_X86_80286_DESCRIPTOR_BYTE_COUNT;

    if (processor == NULL || descriptorAddress == NULL || descriptorIndex == 0u)
    {
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }

    if ((selector & HYPERDOS_X86_SELECTOR_TABLE_INDICATOR) != 0u)
    {
        if (localDescriptorTable == NULL)
        {
            return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
        }
        descriptorTable = localDescriptorTable;
    }
    else
    {
        descriptorTable = &processor->globalDescriptorTable;
    }

    if (descriptorOffset > descriptorTable->limit ||
        descriptorOffset + HYPERDOS_X86_80286_DESCRIPTOR_BYTE_COUNT - 1u > descriptorTable->limit)
    {
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }

    *descriptorAddress = descriptorTable->base + descriptorOffset;
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_read_80286_segment_descriptor_from_tables(
        const hyperdos_x86_processor*              processor,
        uint16_t                                   selector,
        const hyperdos_x86_descriptor_table_state* localDescriptorTable,
        hyperdos_x86_80286_segment_descriptor*     descriptor)
{
    uint32_t descriptorAddress = 0u;

    if (descriptor == NULL ||
        hyperdos_x86_get_80286_descriptor_address_from_tables(processor,
                                                              selector,
                                                              localDescriptorTable,
                                                              &descriptorAddress) != HYPERDOS_X86_EXECUTION_OK)
    {
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }

    descriptor->limit = hyperdos_x86_read_physical_word(processor, descriptorAddress);
    descriptor->base  = (uint32_t)hyperdos_x86_read_physical_word(processor, descriptorAddress + 2u) |
                       ((uint32_t)hyperdos_x86_read_physical_byte(processor, descriptorAddress + 4u) << 16u);
    descriptor->access = hyperdos_x86_read_physical_byte(processor, descriptorAddress + 5u);
    return HYPERDOS_X86_EXECUTION_OK;
}

static int hyperdos_x86_read_80286_local_descriptor_table_state(
        const hyperdos_x86_processor*        processor,
        uint16_t                             selector,
        hyperdos_x86_descriptor_table_state* localDescriptorTable)
{
    hyperdos_x86_80286_segment_descriptor descriptor;

    if (localDescriptorTable == NULL)
    {
        return 0;
    }
    if ((selector & 0xFFF8u) == 0u)
    {
        localDescriptorTable->base  = 0u;
        localDescriptorTable->limit = 0u;
        return 1;
    }
    if ((selector & HYPERDOS_X86_SELECTOR_TABLE_INDICATOR) != 0u ||
        hyperdos_x86_read_80286_segment_descriptor_from_tables(processor, selector, NULL, &descriptor) !=
                HYPERDOS_X86_EXECUTION_OK ||
        hyperdos_x86_80286_system_descriptor_type(&descriptor) != HYPERDOS_X86_80286_LOCAL_DESCRIPTOR_TABLE ||
        !hyperdos_x86_80286_descriptor_is_present(&descriptor))
    {
        return 0;
    }

    localDescriptorTable->base  = descriptor.base & HYPERDOS_X86_80286_DESCRIPTOR_TABLE_BASE_MASK;
    localDescriptorTable->limit = (uint16_t)descriptor.limit;
    return 1;
}

static int hyperdos_x86_validate_80286_data_segment_for_privilege(
        const hyperdos_x86_80286_segment_descriptor* descriptor,
        uint16_t                                     selector,
        uint8_t                                      privilegeLevel)
{
    if (!hyperdos_x86_80286_descriptor_is_code_or_data(descriptor))
    {
        return 0;
    }
    if (hyperdos_x86_80286_descriptor_is_executable(descriptor) &&
        !hyperdos_x86_80286_descriptor_is_readable_or_writable(descriptor))
    {
        return 0;
    }
    return hyperdos_x86_privilege_level_allows_data_access(privilegeLevel,
                                                           hyperdos_x86_selector_requested_privilege_level(selector),
                                                           hyperdos_x86_80286_descriptor_privilege_level(descriptor));
}

static void hyperdos_x86_clear_80286_inaccessible_data_segments(hyperdos_x86_processor* processor,
                                                                uint8_t                 privilegeLevel)
{
    hyperdos_x86_segment_register_index segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_EXTRA;

    for (segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_EXTRA; segmentRegister <= HYPERDOS_X86_SEGMENT_REGISTER_DATA;
         ++segmentRegister)
    {
        hyperdos_x86_80286_segment_descriptor descriptor;
        uint16_t                              selector = processor->segmentStates[segmentRegister].selector;

        if (segmentRegister == HYPERDOS_X86_SEGMENT_REGISTER_CODE ||
            segmentRegister == HYPERDOS_X86_SEGMENT_REGISTER_STACK || (selector & 0xFFF8u) == 0u)
        {
            continue;
        }
        if (hyperdos_x86_read_80286_segment_descriptor(processor, selector, &descriptor) != HYPERDOS_X86_EXECUTION_OK ||
            !hyperdos_x86_80286_descriptor_is_present(&descriptor) ||
            !hyperdos_x86_validate_80286_data_segment_for_privilege(&descriptor, selector, privilegeLevel))
        {
            hyperdos_x86_clear_80286_segment_state(processor, segmentRegister);
        }
    }
}

static inline uint16_t hyperdos_x86_read_80286_task_state_word(const hyperdos_x86_processor* processor,
                                                               uint32_t                      taskStateSegmentBase,
                                                               uint16_t                      taskStateSegmentOffset)
{
    return hyperdos_x86_read_physical_word(processor, taskStateSegmentBase + taskStateSegmentOffset);
}

static inline void hyperdos_x86_write_80286_task_state_word(hyperdos_x86_processor* processor,
                                                            uint32_t                taskStateSegmentBase,
                                                            uint16_t                taskStateSegmentOffset,
                                                            uint16_t                value)
{
    hyperdos_x86_write_physical_word(processor, taskStateSegmentBase + taskStateSegmentOffset, value);
}

static int hyperdos_x86_read_80286_task_state(const hyperdos_x86_processor*                processor,
                                              const hyperdos_x86_80286_segment_descriptor* descriptor,
                                              hyperdos_x86_80286_task_state*               taskState)
{
    hyperdos_x86_general_register_index generalRegister = HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR;
    hyperdos_x86_segment_register_index segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_EXTRA;
    uint32_t taskStateSegmentBase = descriptor->base & HYPERDOS_X86_80286_DESCRIPTOR_TABLE_BASE_MASK;

    if (descriptor->limit < HYPERDOS_X86_80286_TASK_STATE_SEGMENT_MINIMUM_LIMIT || taskState == NULL)
    {
        return 0;
    }

    taskState->previousTaskLink =
            hyperdos_x86_read_80286_task_state_word(processor,
                                                    taskStateSegmentBase,
                                                    HYPERDOS_X86_80286_TASK_STATE_SEGMENT_PREVIOUS_TASK);
    taskState->instructionPointer =
            hyperdos_x86_read_80286_task_state_word(processor,
                                                    taskStateSegmentBase,
                                                    HYPERDOS_X86_80286_TASK_STATE_SEGMENT_INSTRUCTION_POINTER);
    taskState->flags = hyperdos_x86_read_80286_task_state_word(processor,
                                                               taskStateSegmentBase,
                                                               HYPERDOS_X86_80286_TASK_STATE_SEGMENT_FLAGS);
    for (generalRegister = HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR;
         generalRegister <= HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX;
         ++generalRegister)
    {
        taskState->generalRegisters[generalRegister] =
                hyperdos_x86_read_80286_task_state_word(processor,
                                                        taskStateSegmentBase,
                                                        (uint16_t)(HYPERDOS_X86_80286_TASK_STATE_SEGMENT_ACCUMULATOR +
                                                                   generalRegister * HYPERDOS_X86_WORD_SIZE));
    }
    for (segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_EXTRA; segmentRegister <= HYPERDOS_X86_SEGMENT_REGISTER_DATA;
         ++segmentRegister)
    {
        taskState->segmentSelectors[segmentRegister] =
                hyperdos_x86_read_80286_task_state_word(processor,
                                                        taskStateSegmentBase,
                                                        (uint16_t)(HYPERDOS_X86_80286_TASK_STATE_SEGMENT_EXTRA_SEGMENT +
                                                                   segmentRegister * HYPERDOS_X86_WORD_SIZE));
    }
    taskState->localDescriptorTableSelector =
            hyperdos_x86_read_80286_task_state_word(processor,
                                                    taskStateSegmentBase,
                                                    HYPERDOS_X86_80286_TASK_STATE_SEGMENT_LOCAL_DESCRIPTOR_TABLE);
    return 1;
}

static void hyperdos_x86_save_80286_current_task_state(hyperdos_x86_processor* processor,
                                                       uint16_t                instructionPointer,
                                                       uint16_t                flags)
{
    hyperdos_x86_general_register_index generalRegister = HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR;
    hyperdos_x86_segment_register_index segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_EXTRA;
    uint32_t taskStateSegmentBase = processor->taskRegister.base & HYPERDOS_X86_80286_DESCRIPTOR_TABLE_BASE_MASK;

    if (processor->taskRegisterSelector == 0u ||
        processor->taskRegister.limit < HYPERDOS_X86_80286_TASK_STATE_SEGMENT_MINIMUM_LIMIT)
    {
        return;
    }

    hyperdos_x86_write_80286_task_state_word(processor,
                                             taskStateSegmentBase,
                                             HYPERDOS_X86_80286_TASK_STATE_SEGMENT_INSTRUCTION_POINTER,
                                             instructionPointer);
    hyperdos_x86_write_80286_task_state_word(processor,
                                             taskStateSegmentBase,
                                             HYPERDOS_X86_80286_TASK_STATE_SEGMENT_FLAGS,
                                             flags);
    for (generalRegister = HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR;
         generalRegister <= HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX;
         ++generalRegister)
    {
        hyperdos_x86_write_80286_task_state_word(processor,
                                                 taskStateSegmentBase,
                                                 (uint16_t)(HYPERDOS_X86_80286_TASK_STATE_SEGMENT_ACCUMULATOR +
                                                            generalRegister * HYPERDOS_X86_WORD_SIZE),
                                                 hyperdos_x86_read_general_register_word(processor, generalRegister));
    }
    for (segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_EXTRA; segmentRegister <= HYPERDOS_X86_SEGMENT_REGISTER_DATA;
         ++segmentRegister)
    {
        hyperdos_x86_write_80286_task_state_word(processor,
                                                 taskStateSegmentBase,
                                                 (uint16_t)(HYPERDOS_X86_80286_TASK_STATE_SEGMENT_EXTRA_SEGMENT +
                                                            segmentRegister * HYPERDOS_X86_WORD_SIZE),
                                                 processor->segmentStates[segmentRegister].selector);
    }
    hyperdos_x86_write_80286_task_state_word(processor,
                                             taskStateSegmentBase,
                                             HYPERDOS_X86_80286_TASK_STATE_SEGMENT_LOCAL_DESCRIPTOR_TABLE,
                                             processor->localDescriptorTableSelector);
}

static hyperdos_x86_execution_result hyperdos_x86_set_80286_task_state_segment_busy(hyperdos_x86_processor* processor,
                                                                                    uint16_t                selector,
                                                                                    int                     busy)
{
    uint32_t descriptorAddress = 0u;
    uint8_t  access            = 0u;

    if ((selector & HYPERDOS_X86_SELECTOR_TABLE_INDICATOR) != 0u ||
        hyperdos_x86_get_80286_descriptor_address(processor, selector, &descriptorAddress) != HYPERDOS_X86_EXECUTION_OK)
    {
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }
    access = hyperdos_x86_read_physical_byte(processor, descriptorAddress + 5u);
    access = (uint8_t)((access & 0xE0u) | (busy ? HYPERDOS_X86_80286_BUSY_TASK_STATE_SEGMENT
                                                : HYPERDOS_X86_80286_AVAILABLE_TASK_STATE_SEGMENT));
    hyperdos_x86_write_physical_byte(processor, descriptorAddress + 5u, access);
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_validate_80286_task_state_segments(
        const hyperdos_x86_processor*              processor,
        uint16_t                                   taskSelector,
        const hyperdos_x86_80286_task_state*       taskState,
        const hyperdos_x86_descriptor_table_state* localDescriptorTable,
        hyperdos_x86_80286_segment_descriptor*     segmentDescriptors,
        uint8_t*                                   segmentDescriptorLoaded,
        int                                        externalEvent,
        int*                                       instructionCompleted)
{
    hyperdos_x86_segment_register_index segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_EXTRA;
    uint16_t                            codeSelector = taskState->segmentSelectors[HYPERDOS_X86_SEGMENT_REGISTER_CODE];
    uint16_t stackSelector                           = taskState->segmentSelectors[HYPERDOS_X86_SEGMENT_REGISTER_STACK];
    uint8_t  newPrivilegeLevel                       = hyperdos_x86_selector_requested_privilege_level(codeSelector);
    hyperdos_x86_80286_segment_descriptor* codeDescriptor  = &segmentDescriptors[HYPERDOS_X86_SEGMENT_REGISTER_CODE];
    hyperdos_x86_80286_segment_descriptor* stackDescriptor = &segmentDescriptors[HYPERDOS_X86_SEGMENT_REGISTER_STACK];

    memset(segmentDescriptorLoaded, 0, sizeof(uint8_t) * 4u);

    if ((codeSelector & 0xFFF8u) == 0u ||
        hyperdos_x86_read_80286_segment_descriptor_from_tables(processor,
                                                               codeSelector,
                                                               localDescriptorTable,
                                                               codeDescriptor) != HYPERDOS_X86_EXECUTION_OK ||
        !hyperdos_x86_80286_descriptor_is_code_or_data(codeDescriptor) ||
        !hyperdos_x86_80286_descriptor_is_executable(codeDescriptor))
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT,
                                                           hyperdos_x86_80286_selector_error_code(codeSelector,
                                                                                                  externalEvent),
                                                           instructionCompleted);
    }
    if (!hyperdos_x86_80286_descriptor_is_present(codeDescriptor))
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_SEGMENT_NOT_PRESENT,
                                                           hyperdos_x86_80286_selector_error_code(codeSelector,
                                                                                                  externalEvent),
                                                           instructionCompleted);
    }
    if (taskState->instructionPointer > codeDescriptor->limit)
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT,
                                                           hyperdos_x86_80286_selector_error_code(taskSelector,
                                                                                                  externalEvent),
                                                           instructionCompleted);
    }
    if (hyperdos_x86_80286_descriptor_is_conforming_code(codeDescriptor))
    {
        if (hyperdos_x86_80286_descriptor_privilege_level(codeDescriptor) > newPrivilegeLevel)
        {
            return hyperdos_x86_raise_protected_mode_exception(processor,
                                                               HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT,
                                                               hyperdos_x86_80286_selector_error_code(codeSelector,
                                                                                                      externalEvent),
                                                               instructionCompleted);
        }
    }
    else if (hyperdos_x86_80286_descriptor_privilege_level(codeDescriptor) != newPrivilegeLevel)
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT,
                                                           hyperdos_x86_80286_selector_error_code(codeSelector,
                                                                                                  externalEvent),
                                                           instructionCompleted);
    }
    segmentDescriptorLoaded[HYPERDOS_X86_SEGMENT_REGISTER_CODE] = 1u;

    if ((stackSelector & 0xFFF8u) == 0u ||
        hyperdos_x86_selector_requested_privilege_level(stackSelector) != newPrivilegeLevel ||
        hyperdos_x86_read_80286_segment_descriptor_from_tables(processor,
                                                               stackSelector,
                                                               localDescriptorTable,
                                                               stackDescriptor) != HYPERDOS_X86_EXECUTION_OK ||
        !hyperdos_x86_80286_descriptor_is_code_or_data(stackDescriptor) ||
        hyperdos_x86_80286_descriptor_is_executable(stackDescriptor) ||
        !hyperdos_x86_80286_descriptor_is_readable_or_writable(stackDescriptor) ||
        hyperdos_x86_80286_descriptor_privilege_level(stackDescriptor) != newPrivilegeLevel)
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT,
                                                           hyperdos_x86_80286_selector_error_code(stackSelector,
                                                                                                  externalEvent),
                                                           instructionCompleted);
    }
    if (!hyperdos_x86_80286_descriptor_is_present(stackDescriptor))
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_STACK_SEGMENT_FAULT,
                                                           hyperdos_x86_80286_selector_error_code(stackSelector,
                                                                                                  externalEvent),
                                                           instructionCompleted);
    }
    segmentDescriptorLoaded[HYPERDOS_X86_SEGMENT_REGISTER_STACK] = 1u;

    for (segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_EXTRA; segmentRegister <= HYPERDOS_X86_SEGMENT_REGISTER_DATA;
         ++segmentRegister)
    {
        uint16_t selector = taskState->segmentSelectors[segmentRegister];

        if (segmentRegister == HYPERDOS_X86_SEGMENT_REGISTER_CODE ||
            segmentRegister == HYPERDOS_X86_SEGMENT_REGISTER_STACK || (selector & 0xFFF8u) == 0u)
        {
            continue;
        }
        if (hyperdos_x86_read_80286_segment_descriptor_from_tables(processor,
                                                                   selector,
                                                                   localDescriptorTable,
                                                                   &segmentDescriptors[segmentRegister]) !=
                    HYPERDOS_X86_EXECUTION_OK ||
            !hyperdos_x86_validate_80286_data_segment_for_privilege(&segmentDescriptors[segmentRegister],
                                                                    selector,
                                                                    newPrivilegeLevel))
        {
            return hyperdos_x86_raise_protected_mode_exception(processor,
                                                               HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT,
                                                               hyperdos_x86_80286_selector_error_code(selector,
                                                                                                      externalEvent),
                                                               instructionCompleted);
        }
        if (!hyperdos_x86_80286_descriptor_is_present(&segmentDescriptors[segmentRegister]))
        {
            return hyperdos_x86_raise_protected_mode_exception(processor,
                                                               HYPERDOS_X86_INTERRUPT_TYPE_SEGMENT_NOT_PRESENT,
                                                               hyperdos_x86_80286_selector_error_code(selector,
                                                                                                      externalEvent),
                                                               instructionCompleted);
        }
        segmentDescriptorLoaded[segmentRegister] = 1u;
    }

    return HYPERDOS_X86_EXECUTION_OK;
}

static void hyperdos_x86_load_80286_task_state(hyperdos_x86_processor*                      processor,
                                               const hyperdos_x86_80286_task_state*         taskState,
                                               const hyperdos_x86_80286_segment_descriptor* segmentDescriptors,
                                               const uint8_t*                               segmentDescriptorLoaded)
{
    hyperdos_x86_general_register_index generalRegister = HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR;
    hyperdos_x86_segment_register_index segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_EXTRA;

    for (generalRegister = HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR;
         generalRegister <= HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX;
         ++generalRegister)
    {
        hyperdos_x86_write_general_register_word(processor,
                                                 generalRegister,
                                                 taskState->generalRegisters[generalRegister]);
    }
    for (segmentRegister = HYPERDOS_X86_SEGMENT_REGISTER_EXTRA; segmentRegister <= HYPERDOS_X86_SEGMENT_REGISTER_DATA;
         ++segmentRegister)
    {
        if (segmentDescriptorLoaded[segmentRegister] != 0u)
        {
            hyperdos_x86_load_80286_segment_state_from_descriptor(processor,
                                                                  segmentRegister,
                                                                  taskState->segmentSelectors[segmentRegister],
                                                                  &segmentDescriptors[segmentRegister]);
        }
        else
        {
            hyperdos_x86_clear_80286_segment_state(processor, segmentRegister);
        }
    }
    hyperdos_x86_write_instruction_pointer_word(processor, taskState->instructionPointer);
}

static hyperdos_x86_execution_result hyperdos_x86_execute_80286_task_switch(
        hyperdos_x86_processor*             processor,
        uint16_t                            targetSelector,
        hyperdos_x86_80286_task_switch_type taskSwitchType,
        uint16_t                            oldInstructionPointer,
        int                                 checkTargetDescriptorPrivilege,
        int                                 externalEvent,
        int*                                instructionCompleted)
{
    hyperdos_x86_80286_segment_descriptor targetDescriptor;
    hyperdos_x86_80286_segment_descriptor segmentDescriptors[4];
    uint8_t                               segmentDescriptorLoaded[4];
    hyperdos_x86_80286_task_state         targetTaskState;
    hyperdos_x86_descriptor_table_state   targetLocalDescriptorTable;
    uint8_t                               descriptorType        = 0u;
    uint8_t                               currentPrivilegeLevel = hyperdos_x86_current_privilege_level(processor);
    uint8_t  requestedPrivilegeLevel = hyperdos_x86_selector_requested_privilege_level(targetSelector);
    uint16_t currentTaskSelector     = processor->taskRegisterSelector;
    uint16_t savedFlags              = hyperdos_x86_read_flags_word(processor);
    uint16_t loadedFlags             = 0u;

    if ((targetSelector & 0xFFF8u) == 0u || (targetSelector & HYPERDOS_X86_SELECTOR_TABLE_INDICATOR) != 0u ||
        hyperdos_x86_read_80286_segment_descriptor(processor, targetSelector, &targetDescriptor) !=
                HYPERDOS_X86_EXECUTION_OK ||
        !hyperdos_x86_80286_descriptor_is_task_state_segment(&targetDescriptor))
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT,
                                                           hyperdos_x86_80286_selector_error_code(targetSelector,
                                                                                                  externalEvent),
                                                           instructionCompleted);
    }
    if (!hyperdos_x86_80286_descriptor_is_present(&targetDescriptor))
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_SEGMENT_NOT_PRESENT,
                                                           hyperdos_x86_80286_selector_error_code(targetSelector,
                                                                                                  externalEvent),
                                                           instructionCompleted);
    }
    if (targetDescriptor.limit < HYPERDOS_X86_80286_TASK_STATE_SEGMENT_MINIMUM_LIMIT)
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT,
                                                           hyperdos_x86_80286_selector_error_code(targetSelector,
                                                                                                  externalEvent),
                                                           instructionCompleted);
    }
    if (checkTargetDescriptorPrivilege &&
        !hyperdos_x86_privilege_level_allows_data_access(currentPrivilegeLevel,
                                                         requestedPrivilegeLevel,
                                                         hyperdos_x86_80286_descriptor_privilege_level(
                                                                 &targetDescriptor)))
    {
        return hyperdos_x86_raise_general_protection_fault(processor,
                                                           hyperdos_x86_80286_selector_error_code(targetSelector,
                                                                                                  externalEvent),
                                                           instructionCompleted);
    }

    descriptorType = hyperdos_x86_80286_system_descriptor_type(&targetDescriptor);
    if (taskSwitchType == HYPERDOS_X86_80286_TASK_SWITCH_RETURN)
    {
        if (descriptorType != HYPERDOS_X86_80286_BUSY_TASK_STATE_SEGMENT)
        {
            return hyperdos_x86_raise_protected_mode_exception(processor,
                                                               HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT,
                                                               hyperdos_x86_80286_selector_error_code(targetSelector,
                                                                                                      externalEvent),
                                                               instructionCompleted);
        }
        savedFlags = (uint16_t)(savedFlags & (uint16_t)~HYPERDOS_X86_FLAGS_80286_NESTED_TASK);
    }
    else if (descriptorType != HYPERDOS_X86_80286_AVAILABLE_TASK_STATE_SEGMENT)
    {
        return hyperdos_x86_raise_general_protection_fault(processor,
                                                           hyperdos_x86_80286_selector_error_code(targetSelector,
                                                                                                  externalEvent),
                                                           instructionCompleted);
    }

    if (!hyperdos_x86_read_80286_task_state(processor, &targetDescriptor, &targetTaskState))
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT,
                                                           hyperdos_x86_80286_selector_error_code(targetSelector,
                                                                                                  externalEvent),
                                                           instructionCompleted);
    }
    if (!hyperdos_x86_read_80286_local_descriptor_table_state(processor,
                                                              targetTaskState.localDescriptorTableSelector,
                                                              &targetLocalDescriptorTable))
    {
        return hyperdos_x86_raise_protected_mode_exception(
                processor,
                HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT,
                hyperdos_x86_80286_selector_error_code(targetTaskState.localDescriptorTableSelector, externalEvent),
                instructionCompleted);
    }
    if (hyperdos_x86_validate_80286_task_state_segments(processor,
                                                        targetSelector,
                                                        &targetTaskState,
                                                        &targetLocalDescriptorTable,
                                                        segmentDescriptors,
                                                        segmentDescriptorLoaded,
                                                        externalEvent,
                                                        instructionCompleted) != HYPERDOS_X86_EXECUTION_OK ||
        processor->pendingExceptionActive != 0u)
    {
        return HYPERDOS_X86_EXECUTION_OK;
    }

    if (currentTaskSelector != 0u)
    {
        hyperdos_x86_save_80286_current_task_state(processor, oldInstructionPointer, savedFlags);
    }
    if ((taskSwitchType == HYPERDOS_X86_80286_TASK_SWITCH_JUMP ||
         taskSwitchType == HYPERDOS_X86_80286_TASK_SWITCH_RETURN) &&
        currentTaskSelector != 0u)
    {
        (void)hyperdos_x86_set_80286_task_state_segment_busy(processor, currentTaskSelector, 0);
    }
    if (taskSwitchType != HYPERDOS_X86_80286_TASK_SWITCH_RETURN)
    {
        (void)hyperdos_x86_set_80286_task_state_segment_busy(processor, targetSelector, 1);
        targetDescriptor.access = (uint8_t)((targetDescriptor.access & 0xE0u) |
                                            HYPERDOS_X86_80286_BUSY_TASK_STATE_SEGMENT);
    }
    if (taskSwitchType == HYPERDOS_X86_80286_TASK_SWITCH_CALL ||
        taskSwitchType == HYPERDOS_X86_80286_TASK_SWITCH_INTERRUPT)
    {
        uint32_t taskStateSegmentBase = targetDescriptor.base & HYPERDOS_X86_80286_DESCRIPTOR_TABLE_BASE_MASK;
        hyperdos_x86_write_80286_task_state_word(processor,
                                                 taskStateSegmentBase,
                                                 HYPERDOS_X86_80286_TASK_STATE_SEGMENT_PREVIOUS_TASK,
                                                 currentTaskSelector);
        targetTaskState.previousTaskLink = currentTaskSelector;
    }

    processor->taskRegisterSelector         = targetSelector;
    processor->taskRegister.selector        = targetSelector;
    processor->taskRegister.base            = targetDescriptor.base & HYPERDOS_X86_80286_DESCRIPTOR_TABLE_BASE_MASK;
    processor->taskRegister.limit           = targetDescriptor.limit;
    processor->taskRegister.attributes      = targetDescriptor.access;
    processor->localDescriptorTableSelector = targetTaskState.localDescriptorTableSelector;
    processor->localDescriptorTable         = targetLocalDescriptorTable;

    loadedFlags = targetTaskState.flags;
    if (taskSwitchType == HYPERDOS_X86_80286_TASK_SWITCH_CALL ||
        taskSwitchType == HYPERDOS_X86_80286_TASK_SWITCH_INTERRUPT)
    {
        loadedFlags = (uint16_t)(loadedFlags | HYPERDOS_X86_FLAGS_80286_NESTED_TASK);
    }
    else if (taskSwitchType == HYPERDOS_X86_80286_TASK_SWITCH_JUMP)
    {
        loadedFlags = (uint16_t)(loadedFlags & (uint16_t)~HYPERDOS_X86_FLAGS_80286_NESTED_TASK);
    }
    targetTaskState.flags = loadedFlags;
    hyperdos_x86_load_80286_task_state(processor, &targetTaskState, segmentDescriptors, segmentDescriptorLoaded);
    hyperdos_x86_write_flags_word(processor, loadedFlags);
    processor->machineStatusWord = (uint16_t)(processor->machineStatusWord |
                                              HYPERDOS_X86_MACHINE_STATUS_WORD_TASK_SWITCHED);
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_load_protected_mode_segment(
        hyperdos_x86_processor*             processor,
        hyperdos_x86_segment_register_index segmentRegister,
        uint16_t                            selector)
{
    hyperdos_x86_80286_segment_descriptor descriptor;
    uint8_t                               currentPrivilegeLevel = hyperdos_x86_current_privilege_level(processor);
    uint8_t requestedPrivilegeLevel  = hyperdos_x86_selector_requested_privilege_level(selector);
    uint8_t descriptorPrivilegeLevel = 0u;

    if ((selector & 0xFFF8u) == 0u)
    {
        if (segmentRegister == HYPERDOS_X86_SEGMENT_REGISTER_CODE ||
            segmentRegister == HYPERDOS_X86_SEGMENT_REGISTER_STACK)
        {
            hyperdos_x86_set_pending_exception(processor, HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT, 1, 0u);
            return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
        }
        processor->segmentStates[segmentRegister].selector   = selector;
        processor->segmentStates[segmentRegister].base       = 0u;
        processor->segmentStates[segmentRegister].limit      = 0u;
        processor->segmentStates[segmentRegister].attributes = 0u;
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (hyperdos_x86_read_80286_segment_descriptor(processor, selector, &descriptor) != HYPERDOS_X86_EXECUTION_OK)
    {
        hyperdos_x86_set_pending_exception(processor,
                                           HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT,
                                           1,
                                           hyperdos_x86_80286_selector_error_code(selector, 0));
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }
    if (!hyperdos_x86_80286_descriptor_is_present(&descriptor))
    {
        hyperdos_x86_set_pending_exception(processor,
                                           segmentRegister == HYPERDOS_X86_SEGMENT_REGISTER_STACK
                                                   ? HYPERDOS_X86_INTERRUPT_TYPE_STACK_SEGMENT_FAULT
                                                   : HYPERDOS_X86_INTERRUPT_TYPE_SEGMENT_NOT_PRESENT,
                                           1,
                                           hyperdos_x86_80286_selector_error_code(selector, 0));
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }
    if (!hyperdos_x86_80286_descriptor_is_code_or_data(&descriptor))
    {
        hyperdos_x86_set_pending_exception(processor,
                                           HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT,
                                           1,
                                           hyperdos_x86_80286_selector_error_code(selector, 0));
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }

    descriptorPrivilegeLevel = hyperdos_x86_80286_descriptor_privilege_level(&descriptor);
    if (segmentRegister == HYPERDOS_X86_SEGMENT_REGISTER_CODE)
    {
        if (!hyperdos_x86_80286_descriptor_is_executable(&descriptor))
        {
            hyperdos_x86_set_pending_exception(processor,
                                               HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT,
                                               1,
                                               hyperdos_x86_80286_selector_error_code(selector, 0));
            return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
        }
        if ((descriptor.access & HYPERDOS_X86_80286_ACCESS_EXPAND_DOWN_CONFORMING) != 0u)
        {
            if (descriptorPrivilegeLevel > currentPrivilegeLevel)
            {
                hyperdos_x86_set_pending_exception(processor,
                                                   HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT,
                                                   1,
                                                   hyperdos_x86_80286_selector_error_code(selector, 0));
                return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
            }
        }
        else if (descriptorPrivilegeLevel != currentPrivilegeLevel || requestedPrivilegeLevel > currentPrivilegeLevel)
        {
            hyperdos_x86_set_pending_exception(processor,
                                               HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT,
                                               1,
                                               hyperdos_x86_80286_selector_error_code(selector, 0));
            return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
        }
    }
    else if (segmentRegister == HYPERDOS_X86_SEGMENT_REGISTER_STACK)
    {
        if (hyperdos_x86_80286_descriptor_is_executable(&descriptor) ||
            !hyperdos_x86_80286_descriptor_is_readable_or_writable(&descriptor) ||
            descriptorPrivilegeLevel != currentPrivilegeLevel || requestedPrivilegeLevel != currentPrivilegeLevel)
        {
            hyperdos_x86_set_pending_exception(processor,
                                               HYPERDOS_X86_INTERRUPT_TYPE_STACK_SEGMENT_FAULT,
                                               1,
                                               hyperdos_x86_80286_selector_error_code(selector, 0));
            return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
        }
    }
    else if (hyperdos_x86_80286_descriptor_is_executable(&descriptor) &&
             !hyperdos_x86_80286_descriptor_is_readable_or_writable(&descriptor))
    {
        hyperdos_x86_set_pending_exception(processor,
                                           HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT,
                                           1,
                                           hyperdos_x86_80286_selector_error_code(selector, 0));
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }
    else if (!hyperdos_x86_privilege_level_allows_data_access(currentPrivilegeLevel,
                                                              requestedPrivilegeLevel,
                                                              descriptorPrivilegeLevel))
    {
        hyperdos_x86_set_pending_exception(processor,
                                           HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT,
                                           1,
                                           hyperdos_x86_80286_selector_error_code(selector, 0));
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }

    hyperdos_x86_load_80286_segment_state_from_descriptor(processor, segmentRegister, selector, &descriptor);
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_80286_protected_interrupt_with_return_address(
        hyperdos_x86_processor* processor,
        uint8_t                 interruptNumber,
        uint16_t                returnSegment,
        uint16_t                returnOffset,
        int                     hasErrorCode,
        uint16_t                errorCode,
        int                     softwareInterrupt,
        int                     externalEvent)
{
    enum
    {
        HYPERDOS_X86_80286_INTERRUPT_TASK_GATE_TYPE = 0x05u,
        HYPERDOS_X86_80286_INTERRUPT_GATE_TYPE      = 0x06u,
        HYPERDOS_X86_80286_TRAP_GATE_TYPE           = 0x07u
    };
    uint32_t gateOffsetAddress              = (uint32_t)interruptNumber * HYPERDOS_X86_80286_DESCRIPTOR_BYTE_COUNT;
    uint32_t gateAddress                    = 0u;
    uint16_t targetOffset                   = 0u;
    uint16_t targetSelector                 = 0u;
    uint8_t  gateAccess                     = 0u;
    uint8_t  gateType                       = 0u;
    uint8_t  gatePrivilegeLevel             = 0u;
    uint8_t  currentPrivilegeLevel          = hyperdos_x86_current_privilege_level(processor);
    uint8_t  targetPrivilegeLevel           = currentPrivilegeLevel;
    uint8_t  targetDescriptorPrivilegeLevel = 0u;
    int      stackSwitchRequired            = 0;
    uint16_t oldStackSelector               = processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].selector;
    uint16_t oldStackPointer                = hyperdos_x86_read_general_register_word(processor,
                                                                       HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    uint16_t newStackSelector               = 0u;
    uint16_t newStackPointer                = 0u;
    hyperdos_x86_80286_segment_descriptor targetDescriptor;
    hyperdos_x86_80286_segment_descriptor newStackDescriptor;
    uint16_t interruptGateErrorCode  = hyperdos_x86_80286_interrupt_descriptor_error_code(interruptNumber,
                                                                                         externalEvent);
    size_t   interruptFrameWordCount = hasErrorCode ? 4u : 3u;

    if (gateOffsetAddress > processor->interruptDescriptorTable.limit ||
        gateOffsetAddress + HYPERDOS_X86_80286_DESCRIPTOR_BYTE_COUNT - 1u > processor->interruptDescriptorTable.limit)
    {
        hyperdos_x86_set_pending_exception(processor,
                                           HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT,
                                           1,
                                           interruptGateErrorCode);
        return HYPERDOS_X86_EXECUTION_OK;
    }

    gateAddress        = processor->interruptDescriptorTable.base + gateOffsetAddress;
    targetOffset       = hyperdos_x86_read_physical_word(processor, gateAddress);
    targetSelector     = hyperdos_x86_read_physical_word(processor, gateAddress + 2u);
    gateAccess         = hyperdos_x86_read_physical_byte(processor, gateAddress + 5u);
    gateType           = (uint8_t)(gateAccess & 0x1Fu);
    gatePrivilegeLevel = (uint8_t)((gateAccess >> HYPERDOS_X86_80286_ACCESS_PRIVILEGE_SHIFT) &
                                   HYPERDOS_X86_SELECTOR_REQUESTED_PRIVILEGE_MASK);

    if ((gateAccess & HYPERDOS_X86_80286_ACCESS_PRESENT) == 0u)
    {
        hyperdos_x86_set_pending_exception(processor,
                                           HYPERDOS_X86_INTERRUPT_TYPE_SEGMENT_NOT_PRESENT,
                                           1,
                                           interruptGateErrorCode);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (gateType != HYPERDOS_X86_80286_INTERRUPT_TASK_GATE_TYPE && gateType != HYPERDOS_X86_80286_INTERRUPT_GATE_TYPE &&
        gateType != HYPERDOS_X86_80286_TRAP_GATE_TYPE)
    {
        hyperdos_x86_set_pending_exception(processor,
                                           HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT,
                                           1,
                                           interruptGateErrorCode);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (softwareInterrupt && gatePrivilegeLevel < hyperdos_x86_current_privilege_level(processor))
    {
        hyperdos_x86_set_pending_exception(processor,
                                           HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT,
                                           1,
                                           interruptGateErrorCode);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (gateType == HYPERDOS_X86_80286_INTERRUPT_TASK_GATE_TYPE)
    {
        hyperdos_x86_execution_result
                taskSwitchResult = hyperdos_x86_execute_80286_task_switch(processor,
                                                                          targetSelector,
                                                                          HYPERDOS_X86_80286_TASK_SWITCH_INTERRUPT,
                                                                          returnOffset,
                                                                          0,
                                                                          externalEvent,
                                                                          NULL);
        if (taskSwitchResult != HYPERDOS_X86_EXECUTION_OK || processor->pendingExceptionActive != 0u)
        {
            return taskSwitchResult;
        }
        if (hasErrorCode)
        {
            hyperdos_x86_push_word(processor, errorCode);
            if (processor->pendingExceptionActive != 0u)
            {
                return HYPERDOS_X86_EXECUTION_OK;
            }
        }
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if ((targetSelector & 0xFFF8u) == 0u ||
        hyperdos_x86_read_80286_segment_descriptor(processor, targetSelector, &targetDescriptor) !=
                HYPERDOS_X86_EXECUTION_OK ||
        !hyperdos_x86_80286_descriptor_is_code_or_data(&targetDescriptor) ||
        !hyperdos_x86_80286_descriptor_is_executable(&targetDescriptor))
    {
        hyperdos_x86_set_pending_exception(processor,
                                           HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT,
                                           1,
                                           hyperdos_x86_80286_selector_error_code(targetSelector, externalEvent));
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (!hyperdos_x86_80286_descriptor_is_present(&targetDescriptor))
    {
        hyperdos_x86_set_pending_exception(processor,
                                           HYPERDOS_X86_INTERRUPT_TYPE_SEGMENT_NOT_PRESENT,
                                           1,
                                           hyperdos_x86_80286_selector_error_code(targetSelector, externalEvent));
        return HYPERDOS_X86_EXECUTION_OK;
    }

    targetDescriptorPrivilegeLevel = hyperdos_x86_80286_descriptor_privilege_level(&targetDescriptor);
    if ((targetDescriptor.access & HYPERDOS_X86_80286_ACCESS_EXPAND_DOWN_CONFORMING) != 0u)
    {
        if (targetDescriptorPrivilegeLevel > currentPrivilegeLevel)
        {
            hyperdos_x86_set_pending_exception(processor,
                                               HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT,
                                               1,
                                               hyperdos_x86_80286_selector_error_code(targetSelector, externalEvent));
            return HYPERDOS_X86_EXECUTION_OK;
        }
    }
    else
    {
        if (targetDescriptorPrivilegeLevel > currentPrivilegeLevel)
        {
            hyperdos_x86_set_pending_exception(processor,
                                               HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT,
                                               1,
                                               hyperdos_x86_80286_selector_error_code(targetSelector, externalEvent));
            return HYPERDOS_X86_EXECUTION_OK;
        }
        targetPrivilegeLevel = targetDescriptorPrivilegeLevel;
        stackSwitchRequired  = targetPrivilegeLevel < currentPrivilegeLevel;
    }

    if (stackSwitchRequired)
    {
        if (!hyperdos_x86_read_80286_task_stack(processor, targetPrivilegeLevel, &newStackPointer, &newStackSelector))
        {
            hyperdos_x86_set_pending_exception(processor,
                                               HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT,
                                               1,
                                               hyperdos_x86_80286_selector_error_code(processor->taskRegisterSelector,
                                                                                      externalEvent));
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (!hyperdos_x86_validate_80286_stack_segment_for_privilege(processor,
                                                                     newStackSelector,
                                                                     targetPrivilegeLevel,
                                                                     &newStackDescriptor))
        {
            hyperdos_x86_set_pending_exception(processor,
                                               HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT,
                                               1,
                                               hyperdos_x86_80286_selector_error_code(newStackSelector, externalEvent));
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (!hyperdos_x86_80286_stack_descriptor_can_push_words(&newStackDescriptor,
                                                                newStackSelector,
                                                                newStackPointer,
                                                                interruptFrameWordCount + 2u))
        {
            hyperdos_x86_set_pending_exception(processor, HYPERDOS_X86_INTERRUPT_TYPE_STACK_SEGMENT_FAULT, 1, 0u);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        hyperdos_x86_load_80286_segment_state_from_descriptor(processor,
                                                              HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                                              newStackSelector,
                                                              &newStackDescriptor);
        hyperdos_x86_write_general_register_word(processor,
                                                 HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                                 newStackPointer);
        hyperdos_x86_push_word(processor, oldStackSelector);
        hyperdos_x86_push_word(processor, oldStackPointer);
    }
    else if (!hyperdos_x86_80286_current_stack_can_push_words(processor, interruptFrameWordCount))
    {
        hyperdos_x86_set_pending_exception(processor, HYPERDOS_X86_INTERRUPT_TYPE_STACK_SEGMENT_FAULT, 1, 0u);
        return HYPERDOS_X86_EXECUTION_OK;
    }

    hyperdos_x86_push_word(processor, hyperdos_x86_read_flags_word(processor));
    hyperdos_x86_push_word(processor, returnSegment);
    hyperdos_x86_push_word(processor, returnOffset);
    if (hasErrorCode)
    {
        hyperdos_x86_push_word(processor, errorCode);
    }
    hyperdos_x86_load_80286_segment_state_from_descriptor(
            processor,
            HYPERDOS_X86_SEGMENT_REGISTER_CODE,
            hyperdos_x86_selector_with_requested_privilege_level(targetSelector, targetPrivilegeLevel),
            &targetDescriptor);
    hyperdos_x86_write_instruction_pointer_word(processor, targetOffset);
    processor->flags &= (uint16_t)~HYPERDOS_X86_FLAG_TRAP;
    if (gateType == HYPERDOS_X86_80286_INTERRUPT_GATE_TYPE)
    {
        processor->flags &= (uint16_t)~HYPERDOS_X86_FLAG_INTERRUPT_ENABLE;
    }
    hyperdos_x86_normalize_current_flags(processor);
    return HYPERDOS_X86_EXECUTION_OK;
}

static inline uint8_t hyperdos_x86_read_memory_byte_fast(const hyperdos_x86_processor*       processor,
                                                         hyperdos_x86_segment_register_index segmentRegister,
                                                         uint16_t                            offset)
{
    uint32_t physicalAddress = hyperdos_x86_physical_address_from_base(processor,
                                                                       processor->segmentStates[segmentRegister].base,
                                                                       offset);
    if (!hyperdos_x86_protected_memory_access_is_valid(processor,
                                                       segmentRegister,
                                                       offset,
                                                       HYPERDOS_X86_BYTE_SIZE,
                                                       HYPERDOS_X86_MEMORY_ACCESS_READ))
    {
        return 0u;
    }
    return hyperdos_x86_read_physical_byte(processor, physicalAddress);
}

static inline void hyperdos_x86_write_memory_byte_fast(hyperdos_x86_processor*             processor,
                                                       hyperdos_x86_segment_register_index segmentRegister,
                                                       uint16_t                            offset,
                                                       uint8_t                             value)
{
    uint32_t physicalAddress = hyperdos_x86_physical_address_from_base(processor,
                                                                       processor->segmentStates[segmentRegister].base,
                                                                       offset);
    if (!hyperdos_x86_protected_memory_access_is_valid(processor,
                                                       segmentRegister,
                                                       offset,
                                                       HYPERDOS_X86_BYTE_SIZE,
                                                       HYPERDOS_X86_MEMORY_ACCESS_WRITE))
    {
        return;
    }
    hyperdos_x86_write_physical_byte(processor, physicalAddress, value);
}

static inline uint16_t hyperdos_x86_read_memory_word_fast(const hyperdos_x86_processor*       processor,
                                                          hyperdos_x86_segment_register_index segmentRegister,
                                                          uint16_t                            offset)
{
    if (!hyperdos_x86_protected_memory_access_is_valid(processor,
                                                       segmentRegister,
                                                       offset,
                                                       HYPERDOS_X86_WORD_SIZE,
                                                       HYPERDOS_X86_MEMORY_ACCESS_READ))
    {
        return 0u;
    }
    if (hyperdos_x86_word_access_wraps_at_segment_boundary(processor, offset))
    {
        uint16_t lowByte  = hyperdos_x86_read_memory_byte_fast(processor, segmentRegister, offset);
        uint16_t highByte = hyperdos_x86_read_memory_byte_fast(processor, segmentRegister, 0u);
        return (uint16_t)(lowByte | (highByte << HYPERDOS_X86_BYTE_BIT_COUNT));
    }

    uint32_t physicalAddress = hyperdos_x86_physical_address_from_base(processor,
                                                                       processor->segmentStates[segmentRegister].base,
                                                                       offset);
    return hyperdos_x86_read_physical_word(processor, physicalAddress);
}

static inline void hyperdos_x86_write_memory_word_fast(hyperdos_x86_processor*             processor,
                                                       hyperdos_x86_segment_register_index segmentRegister,
                                                       uint16_t                            offset,
                                                       uint16_t                            value)
{
    if (!hyperdos_x86_protected_memory_access_is_valid(processor,
                                                       segmentRegister,
                                                       offset,
                                                       HYPERDOS_X86_WORD_SIZE,
                                                       HYPERDOS_X86_MEMORY_ACCESS_WRITE))
    {
        return;
    }
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

    uint32_t physicalAddress = hyperdos_x86_physical_address_from_base(processor,
                                                                       processor->segmentStates[segmentRegister].base,
                                                                       offset);
    hyperdos_x86_write_physical_word(processor, physicalAddress, value);
}

static inline uint8_t hyperdos_x86_fetch_instruction_byte(hyperdos_x86_processor* processor)
{
    uint16_t instructionOffset = hyperdos_x86_read_instruction_pointer_word(processor);
    uint8_t  value             = 0u;

    if (!hyperdos_x86_protected_memory_access_is_valid(processor,
                                                       HYPERDOS_X86_SEGMENT_REGISTER_CODE,
                                                       instructionOffset,
                                                       HYPERDOS_X86_BYTE_SIZE,
                                                       HYPERDOS_X86_MEMORY_ACCESS_EXECUTE))
    {
        return 0u;
    }
    value = hyperdos_x86_read_physical_byte(
            processor,
            hyperdos_x86_physical_address_from_base(processor,
                                                    processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].base,
                                                    instructionOffset));
    hyperdos_x86_write_instruction_pointer_word(processor, (uint16_t)(instructionOffset + 1u));
    return value;
}

static inline uint16_t hyperdos_x86_fetch_instruction_word(hyperdos_x86_processor* processor)
{
    uint16_t instructionOffset = hyperdos_x86_read_instruction_pointer_word(processor);
    uint16_t value             = 0u;

    if (!hyperdos_x86_protected_memory_access_is_valid(processor,
                                                       HYPERDOS_X86_SEGMENT_REGISTER_CODE,
                                                       instructionOffset,
                                                       HYPERDOS_X86_WORD_SIZE,
                                                       HYPERDOS_X86_MEMORY_ACCESS_EXECUTE))
    {
        return 0u;
    }
    value = hyperdos_x86_read_physical_word(
            processor,
            hyperdos_x86_physical_address_from_base(processor,
                                                    processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].base,
                                                    instructionOffset));
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
    hyperdos_x86_write_flags_word(processor, (uint16_t)((processor->flags & ~statusMask) | statusFlags));
}

static inline void hyperdos_x86_replace_carry_flag(hyperdos_x86_processor* processor, uint16_t carryFlag)
{
    hyperdos_x86_write_flags_word(processor,
                                  (uint16_t)((hyperdos_x86_read_flags_word(processor) &
                                              (uint16_t)~HYPERDOS_X86_FLAG_CARRY) |
                                             carryFlag));
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
    hyperdos_x86_write_flags_word(processor,
                                  (uint16_t)((processor->flags & (uint16_t)~HYPERDOS_X86_FLAG_CARRY) | carryFlag));
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
    if (hyperdos_x86_processor_is_protected_mode_active(processor) &&
        !hyperdos_x86_protected_memory_access_is_valid(processor,
                                                       HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                                       stackPointer,
                                                       HYPERDOS_X86_WORD_SIZE,
                                                       HYPERDOS_X86_MEMORY_ACCESS_WRITE))
    {
        return;
    }
    hyperdos_x86_write_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER, stackPointer);
    hyperdos_x86_write_memory_word_fast(processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, stackPointer, value);
}

static inline uint16_t hyperdos_x86_pop_word(hyperdos_x86_processor* processor)
{
    uint16_t stackPointer = hyperdos_x86_read_general_register_word(processor,
                                                                    HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    uint16_t value        = 0u;

    if (hyperdos_x86_processor_is_protected_mode_active(processor) &&
        !hyperdos_x86_protected_memory_access_is_valid(processor,
                                                       HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                                       stackPointer,
                                                       HYPERDOS_X86_WORD_SIZE,
                                                       HYPERDOS_X86_MEMORY_ACCESS_READ))
    {
        return 0u;
    }
    value = hyperdos_x86_read_memory_word_fast(processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, stackPointer);
    hyperdos_x86_write_general_register_word(processor,
                                             HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                             (uint16_t)(stackPointer + HYPERDOS_X86_WORD_SIZE));
    return value;
}

static hyperdos_x86_execution_result hyperdos_x86_validate_80286_return_code_segment(
        const hyperdos_x86_processor*          processor,
        uint16_t                               selector,
        uint16_t                               offset,
        hyperdos_x86_80286_segment_descriptor* descriptor,
        int*                                   instructionCompleted)
{
    uint8_t currentPrivilegeLevel    = hyperdos_x86_current_privilege_level(processor);
    uint8_t returnPrivilegeLevel     = hyperdos_x86_selector_requested_privilege_level(selector);
    uint8_t descriptorPrivilegeLevel = 0u;

    if (returnPrivilegeLevel < currentPrivilegeLevel)
    {
        return hyperdos_x86_raise_general_protection_fault(processor,
                                                           hyperdos_x86_80286_selector_error_code(selector, 0),
                                                           instructionCompleted);
    }
    if ((selector & 0xFFF8u) == 0u ||
        hyperdos_x86_read_80286_segment_descriptor(processor, selector, descriptor) != HYPERDOS_X86_EXECUTION_OK ||
        !hyperdos_x86_80286_descriptor_is_code_or_data(descriptor) ||
        !hyperdos_x86_80286_descriptor_is_executable(descriptor))
    {
        return hyperdos_x86_raise_general_protection_fault(processor,
                                                           hyperdos_x86_80286_selector_error_code(selector, 0),
                                                           instructionCompleted);
    }
    if (!hyperdos_x86_80286_descriptor_is_present(descriptor))
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_SEGMENT_NOT_PRESENT,
                                                           hyperdos_x86_80286_selector_error_code(selector, 0),
                                                           instructionCompleted);
    }
    if (offset > descriptor->limit)
    {
        return hyperdos_x86_raise_general_protection_fault(processor, 0u, instructionCompleted);
    }

    descriptorPrivilegeLevel = hyperdos_x86_80286_descriptor_privilege_level(descriptor);
    if (hyperdos_x86_80286_descriptor_is_conforming_code(descriptor))
    {
        if (descriptorPrivilegeLevel > returnPrivilegeLevel)
        {
            return hyperdos_x86_raise_general_protection_fault(processor,
                                                               hyperdos_x86_80286_selector_error_code(selector, 0),
                                                               instructionCompleted);
        }
    }
    else if (descriptorPrivilegeLevel != returnPrivilegeLevel)
    {
        return hyperdos_x86_raise_general_protection_fault(processor,
                                                           hyperdos_x86_80286_selector_error_code(selector, 0),
                                                           instructionCompleted);
    }
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_validate_80286_return_stack_segment(
        const hyperdos_x86_processor*          processor,
        uint16_t                               selector,
        uint8_t                                privilegeLevel,
        hyperdos_x86_80286_segment_descriptor* descriptor,
        int*                                   instructionCompleted)
{
    if ((selector & 0xFFF8u) == 0u || hyperdos_x86_selector_requested_privilege_level(selector) != privilegeLevel ||
        hyperdos_x86_read_80286_segment_descriptor(processor, selector, descriptor) != HYPERDOS_X86_EXECUTION_OK ||
        !hyperdos_x86_80286_descriptor_is_code_or_data(descriptor) ||
        hyperdos_x86_80286_descriptor_is_executable(descriptor) ||
        !hyperdos_x86_80286_descriptor_is_readable_or_writable(descriptor) ||
        hyperdos_x86_80286_descriptor_privilege_level(descriptor) != privilegeLevel)
    {
        return hyperdos_x86_raise_general_protection_fault(processor,
                                                           hyperdos_x86_80286_selector_error_code(selector, 0),
                                                           instructionCompleted);
    }
    if (!hyperdos_x86_80286_descriptor_is_present(descriptor))
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_STACK_SEGMENT_FAULT,
                                                           hyperdos_x86_80286_selector_error_code(selector, 0),
                                                           instructionCompleted);
    }
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_80286_protected_far_return(hyperdos_x86_processor* processor,
                                                                                     uint16_t stackByteCount,
                                                                                     int      restoreFlags,
                                                                                     int*     instructionCompleted)
{
    uint16_t                              stackPointer          = hyperdos_x86_read_general_register_word(processor,
                                                                    HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    uint16_t                              frameByteCount        = restoreFlags ? 6u : 4u;
    uint16_t                              returnOffset          = 0u;
    uint16_t                              returnSelector        = 0u;
    uint16_t                              returnFlags           = 0u;
    uint16_t                              outerStackPointer     = 0u;
    uint16_t                              outerStackSelector    = 0u;
    uint8_t                               currentPrivilegeLevel = hyperdos_x86_current_privilege_level(processor);
    uint8_t                               returnPrivilegeLevel  = 0u;
    hyperdos_x86_80286_segment_descriptor codeDescriptor;
    hyperdos_x86_80286_segment_descriptor stackDescriptor;
    hyperdos_x86_execution_result         validationResult = HYPERDOS_X86_EXECUTION_OK;

    if (!hyperdos_x86_protected_memory_access_is_valid(processor,
                                                       HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                                       stackPointer,
                                                       frameByteCount,
                                                       HYPERDOS_X86_MEMORY_ACCESS_READ))
    {
        if (instructionCompleted != NULL)
        {
            *instructionCompleted = 0;
        }
        return HYPERDOS_X86_EXECUTION_OK;
    }
    returnOffset   = hyperdos_x86_read_memory_word_fast(processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, stackPointer);
    returnSelector = hyperdos_x86_read_memory_word_fast(processor,
                                                        HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                                        (uint16_t)(stackPointer + 2u));
    returnFlags    = restoreFlags ? hyperdos_x86_read_memory_word_fast(processor,
                                                                    HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                                                    (uint16_t)(stackPointer + 4u))
                                  : 0u;
    returnPrivilegeLevel = hyperdos_x86_selector_requested_privilege_level(returnSelector);
    validationResult     = hyperdos_x86_validate_80286_return_code_segment(processor,
                                                                       returnSelector,
                                                                       returnOffset,
                                                                       &codeDescriptor,
                                                                       instructionCompleted);
    if (processor->pendingExceptionActive != 0u)
    {
        if (instructionCompleted != NULL)
        {
            *instructionCompleted = 0;
        }
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (validationResult != HYPERDOS_X86_EXECUTION_OK || processor->pendingExceptionActive != 0u)
    {
        return validationResult;
    }

    if (returnPrivilegeLevel > currentPrivilegeLevel)
    {
        if (!hyperdos_x86_protected_memory_access_is_valid(processor,
                                                           HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                                           (uint16_t)(stackPointer + frameByteCount + stackByteCount),
                                                           4u,
                                                           HYPERDOS_X86_MEMORY_ACCESS_READ))
        {
            if (instructionCompleted != NULL)
            {
                *instructionCompleted = 0;
            }
            return HYPERDOS_X86_EXECUTION_OK;
        }
        outerStackPointer  = hyperdos_x86_read_memory_word_fast(processor,
                                                               HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                                               (uint16_t)(stackPointer + frameByteCount +
                                                                          stackByteCount));
        outerStackSelector = hyperdos_x86_read_memory_word_fast(processor,
                                                                HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                                                (uint16_t)(stackPointer + frameByteCount +
                                                                           stackByteCount + HYPERDOS_X86_WORD_SIZE));
        if (processor->pendingExceptionActive != 0u)
        {
            if (instructionCompleted != NULL)
            {
                *instructionCompleted = 0;
            }
            return HYPERDOS_X86_EXECUTION_OK;
        }
        validationResult = hyperdos_x86_validate_80286_return_stack_segment(processor,
                                                                            outerStackSelector,
                                                                            returnPrivilegeLevel,
                                                                            &stackDescriptor,
                                                                            instructionCompleted);
        if (validationResult != HYPERDOS_X86_EXECUTION_OK || processor->pendingExceptionActive != 0u)
        {
            return validationResult;
        }
    }

    if (restoreFlags)
    {
        hyperdos_x86_restore_flags_word_from_stack(processor, returnFlags);
    }
    hyperdos_x86_load_80286_segment_state_from_descriptor(processor,
                                                          HYPERDOS_X86_SEGMENT_REGISTER_CODE,
                                                          returnSelector,
                                                          &codeDescriptor);
    hyperdos_x86_write_instruction_pointer_word(processor, returnOffset);
    if (returnPrivilegeLevel > currentPrivilegeLevel)
    {
        hyperdos_x86_load_80286_segment_state_from_descriptor(processor,
                                                              HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                                              outerStackSelector,
                                                              &stackDescriptor);
        hyperdos_x86_write_general_register_word(processor,
                                                 HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                                 (uint16_t)(outerStackPointer + stackByteCount));
        hyperdos_x86_clear_80286_inaccessible_data_segments(processor, returnPrivilegeLevel);
    }
    else
    {
        hyperdos_x86_write_general_register_word(processor,
                                                 HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                                 (uint16_t)(stackPointer + frameByteCount + stackByteCount));
    }
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_validate_80286_direct_code_transfer(
        const hyperdos_x86_processor*                processor,
        uint16_t                                     selector,
        uint16_t                                     offset,
        const hyperdos_x86_80286_segment_descriptor* descriptor,
        int*                                         instructionCompleted)
{
    uint8_t currentPrivilegeLevel    = hyperdos_x86_current_privilege_level(processor);
    uint8_t requestedPrivilegeLevel  = hyperdos_x86_selector_requested_privilege_level(selector);
    uint8_t descriptorPrivilegeLevel = hyperdos_x86_80286_descriptor_privilege_level(descriptor);

    if (!hyperdos_x86_80286_descriptor_is_code_or_data(descriptor) ||
        !hyperdos_x86_80286_descriptor_is_executable(descriptor))
    {
        return hyperdos_x86_raise_general_protection_fault(processor,
                                                           hyperdos_x86_80286_selector_error_code(selector, 0),
                                                           instructionCompleted);
    }
    if (!hyperdos_x86_80286_descriptor_is_present(descriptor))
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_SEGMENT_NOT_PRESENT,
                                                           hyperdos_x86_80286_selector_error_code(selector, 0),
                                                           instructionCompleted);
    }
    if (offset > descriptor->limit)
    {
        return hyperdos_x86_raise_general_protection_fault(processor, 0u, instructionCompleted);
    }

    if (hyperdos_x86_80286_descriptor_is_conforming_code(descriptor))
    {
        if (descriptorPrivilegeLevel > currentPrivilegeLevel)
        {
            return hyperdos_x86_raise_general_protection_fault(processor,
                                                               hyperdos_x86_80286_selector_error_code(selector, 0),
                                                               instructionCompleted);
        }
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (descriptorPrivilegeLevel != currentPrivilegeLevel || requestedPrivilegeLevel > currentPrivilegeLevel)
    {
        return hyperdos_x86_raise_general_protection_fault(processor,
                                                           hyperdos_x86_80286_selector_error_code(selector, 0),
                                                           instructionCompleted);
    }
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_80286_direct_code_transfer(
        hyperdos_x86_processor*                      processor,
        uint16_t                                     selector,
        uint16_t                                     offset,
        const hyperdos_x86_80286_segment_descriptor* descriptor,
        int                                          pushReturnAddress,
        int*                                         instructionCompleted)
{
    hyperdos_x86_execution_result
            validationResult      = hyperdos_x86_validate_80286_direct_code_transfer(processor,
                                                                                selector,
                                                                                offset,
                                                                                descriptor,
                                                                                instructionCompleted);
    uint8_t currentPrivilegeLevel = hyperdos_x86_current_privilege_level(processor);

    if (validationResult != HYPERDOS_X86_EXECUTION_OK || processor->pendingExceptionActive != 0u)
    {
        return validationResult;
    }
    if (pushReturnAddress && !hyperdos_x86_80286_current_stack_can_push_words(processor, 2u))
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_STACK_SEGMENT_FAULT,
                                                           0u,
                                                           instructionCompleted);
    }
    if (pushReturnAddress)
    {
        hyperdos_x86_push_word(processor, processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].selector);
        hyperdos_x86_push_word(processor, hyperdos_x86_read_instruction_pointer_word(processor));
    }
    hyperdos_x86_load_80286_segment_state_from_descriptor(
            processor,
            HYPERDOS_X86_SEGMENT_REGISTER_CODE,
            hyperdos_x86_selector_with_requested_privilege_level(selector, currentPrivilegeLevel),
            descriptor);
    hyperdos_x86_write_instruction_pointer_word(processor, offset);
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_80286_call_gate_transfer(
        hyperdos_x86_processor*                      processor,
        uint16_t                                     gateSelector,
        const hyperdos_x86_80286_segment_descriptor* gateDescriptor,
        int                                          pushReturnAddress,
        int*                                         instructionCompleted)
{
    uint32_t gateAddress             = 0u;
    uint16_t targetOffset            = 0u;
    uint16_t targetSelector          = 0u;
    uint8_t  parameterWordCount      = 0u;
    uint8_t  currentPrivilegeLevel   = hyperdos_x86_current_privilege_level(processor);
    uint8_t  requestedPrivilegeLevel = hyperdos_x86_selector_requested_privilege_level(gateSelector);
    uint8_t  gatePrivilegeLevel      = hyperdos_x86_80286_descriptor_privilege_level(gateDescriptor);
    hyperdos_x86_80286_segment_descriptor targetDescriptor;
    uint8_t                               targetPrivilegeLevel = 0u;
    int                                   stackSwitchRequired  = 0;
    uint16_t                              copiedParameterWords[HYPERDOS_X86_80286_CALL_GATE_PARAMETER_WORD_COUNT_MASK];
    uint8_t                               parameterWordIndex = 0u;
    uint16_t oldStackSelector = processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].selector;
    uint16_t oldStackPointer  = hyperdos_x86_read_general_register_word(processor,
                                                                       HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    uint16_t newStackSelector = 0u;
    uint16_t newStackPointer  = 0u;
    hyperdos_x86_80286_segment_descriptor newStackDescriptor;

    if (!hyperdos_x86_80286_descriptor_is_present(gateDescriptor))
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_SEGMENT_NOT_PRESENT,
                                                           hyperdos_x86_80286_selector_error_code(gateSelector, 0),
                                                           instructionCompleted);
    }
    if (!hyperdos_x86_privilege_level_allows_data_access(currentPrivilegeLevel,
                                                         requestedPrivilegeLevel,
                                                         gatePrivilegeLevel))
    {
        return hyperdos_x86_raise_general_protection_fault(processor,
                                                           hyperdos_x86_80286_selector_error_code(gateSelector, 0),
                                                           instructionCompleted);
    }
    if (hyperdos_x86_get_80286_descriptor_address(processor, gateSelector, &gateAddress) != HYPERDOS_X86_EXECUTION_OK)
    {
        return hyperdos_x86_raise_general_protection_fault(processor,
                                                           hyperdos_x86_80286_selector_error_code(gateSelector, 0),
                                                           instructionCompleted);
    }

    targetOffset       = hyperdos_x86_read_physical_word(processor, gateAddress);
    targetSelector     = hyperdos_x86_read_physical_word(processor, gateAddress + 2u);
    parameterWordCount = (uint8_t)(hyperdos_x86_read_physical_byte(processor, gateAddress + 4u) &
                                   HYPERDOS_X86_80286_CALL_GATE_PARAMETER_WORD_COUNT_MASK);

    if ((targetSelector & 0xFFF8u) == 0u ||
        hyperdos_x86_read_80286_segment_descriptor(processor, targetSelector, &targetDescriptor) !=
                HYPERDOS_X86_EXECUTION_OK ||
        !hyperdos_x86_80286_descriptor_is_code_or_data(&targetDescriptor) ||
        !hyperdos_x86_80286_descriptor_is_executable(&targetDescriptor))
    {
        return hyperdos_x86_raise_general_protection_fault(processor,
                                                           hyperdos_x86_80286_selector_error_code(targetSelector, 0),
                                                           instructionCompleted);
    }
    if (!hyperdos_x86_80286_descriptor_is_present(&targetDescriptor))
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_SEGMENT_NOT_PRESENT,
                                                           hyperdos_x86_80286_selector_error_code(targetSelector, 0),
                                                           instructionCompleted);
    }
    if (targetOffset > targetDescriptor.limit)
    {
        return hyperdos_x86_raise_general_protection_fault(processor, 0u, instructionCompleted);
    }

    targetPrivilegeLevel = hyperdos_x86_80286_descriptor_privilege_level(&targetDescriptor);
    if (hyperdos_x86_80286_descriptor_is_conforming_code(&targetDescriptor))
    {
        if (targetPrivilegeLevel > currentPrivilegeLevel)
        {
            return hyperdos_x86_raise_general_protection_fault(processor,
                                                               hyperdos_x86_80286_selector_error_code(targetSelector,
                                                                                                      0),
                                                               instructionCompleted);
        }
        targetPrivilegeLevel = currentPrivilegeLevel;
    }
    else
    {
        if (targetPrivilegeLevel > currentPrivilegeLevel)
        {
            return hyperdos_x86_raise_general_protection_fault(processor,
                                                               hyperdos_x86_80286_selector_error_code(targetSelector,
                                                                                                      0),
                                                               instructionCompleted);
        }
        stackSwitchRequired = targetPrivilegeLevel < currentPrivilegeLevel;
    }
    if (stackSwitchRequired && !pushReturnAddress)
    {
        return hyperdos_x86_raise_general_protection_fault(processor,
                                                           hyperdos_x86_80286_selector_error_code(targetSelector, 0),
                                                           instructionCompleted);
    }

    if (stackSwitchRequired)
    {
        for (parameterWordIndex = 0u; parameterWordIndex < parameterWordCount; ++parameterWordIndex)
        {
            copiedParameterWords[parameterWordIndex] = hyperdos_x86_read_memory_word_fast(
                    processor,
                    HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                    (uint16_t)(oldStackPointer + parameterWordIndex * HYPERDOS_X86_WORD_SIZE));
            if (processor->pendingExceptionActive != 0u)
            {
                if (instructionCompleted != NULL)
                {
                    *instructionCompleted = 0;
                }
                return HYPERDOS_X86_EXECUTION_OK;
            }
        }
        if (!hyperdos_x86_read_80286_task_stack(processor, targetPrivilegeLevel, &newStackPointer, &newStackSelector))
        {
            return hyperdos_x86_raise_protected_mode_exception(
                    processor,
                    HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT,
                    hyperdos_x86_80286_selector_error_code(processor->taskRegisterSelector, 0),
                    instructionCompleted);
        }
        if (!hyperdos_x86_validate_80286_stack_segment_for_privilege(processor,
                                                                     newStackSelector,
                                                                     targetPrivilegeLevel,
                                                                     &newStackDescriptor))
        {
            return hyperdos_x86_raise_protected_mode_exception(processor,
                                                               HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT,
                                                               hyperdos_x86_80286_selector_error_code(newStackSelector,
                                                                                                      0),
                                                               instructionCompleted);
        }
        if (!hyperdos_x86_80286_stack_descriptor_can_push_words(&newStackDescriptor,
                                                                newStackSelector,
                                                                newStackPointer,
                                                                4u + parameterWordCount))
        {
            return hyperdos_x86_raise_protected_mode_exception(processor,
                                                               HYPERDOS_X86_INTERRUPT_TYPE_STACK_SEGMENT_FAULT,
                                                               0u,
                                                               instructionCompleted);
        }
        hyperdos_x86_load_80286_segment_state_from_descriptor(processor,
                                                              HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                                              newStackSelector,
                                                              &newStackDescriptor);
        hyperdos_x86_write_general_register_word(processor,
                                                 HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                                 newStackPointer);
        hyperdos_x86_push_word(processor, oldStackSelector);
        hyperdos_x86_push_word(processor, oldStackPointer);
        while (parameterWordIndex > 0u)
        {
            --parameterWordIndex;
            hyperdos_x86_push_word(processor, copiedParameterWords[parameterWordIndex]);
        }
    }
    else if (pushReturnAddress && !hyperdos_x86_80286_current_stack_can_push_words(processor, 2u))
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_STACK_SEGMENT_FAULT,
                                                           0u,
                                                           instructionCompleted);
    }

    if (pushReturnAddress)
    {
        hyperdos_x86_push_word(processor, processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].selector);
        hyperdos_x86_push_word(processor, hyperdos_x86_read_instruction_pointer_word(processor));
    }
    hyperdos_x86_load_80286_segment_state_from_descriptor(
            processor,
            HYPERDOS_X86_SEGMENT_REGISTER_CODE,
            hyperdos_x86_selector_with_requested_privilege_level(targetSelector, targetPrivilegeLevel),
            &targetDescriptor);
    hyperdos_x86_write_instruction_pointer_word(processor, targetOffset);
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_80286_task_state_segment_transfer(
        hyperdos_x86_processor*                      processor,
        uint16_t                                     selector,
        const hyperdos_x86_80286_segment_descriptor* descriptor,
        int                                          pushReturnAddress,
        int*                                         instructionCompleted)
{
    uint8_t currentPrivilegeLevel    = hyperdos_x86_current_privilege_level(processor);
    uint8_t requestedPrivilegeLevel  = hyperdos_x86_selector_requested_privilege_level(selector);
    uint8_t descriptorPrivilegeLevel = hyperdos_x86_80286_descriptor_privilege_level(descriptor);

    if (!hyperdos_x86_80286_descriptor_is_present(descriptor))
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_SEGMENT_NOT_PRESENT,
                                                           hyperdos_x86_80286_selector_error_code(selector, 0),
                                                           instructionCompleted);
    }
    if (!hyperdos_x86_privilege_level_allows_data_access(currentPrivilegeLevel,
                                                         requestedPrivilegeLevel,
                                                         descriptorPrivilegeLevel))
    {
        return hyperdos_x86_raise_general_protection_fault(processor,
                                                           hyperdos_x86_80286_selector_error_code(selector, 0),
                                                           instructionCompleted);
    }
    return hyperdos_x86_execute_80286_task_switch(processor,
                                                  selector,
                                                  pushReturnAddress ? HYPERDOS_X86_80286_TASK_SWITCH_CALL
                                                                    : HYPERDOS_X86_80286_TASK_SWITCH_JUMP,
                                                  hyperdos_x86_read_instruction_pointer_word(processor),
                                                  1,
                                                  0,
                                                  instructionCompleted);
}

static hyperdos_x86_execution_result hyperdos_x86_execute_80286_task_gate_transfer(
        hyperdos_x86_processor*                      processor,
        uint16_t                                     gateSelector,
        const hyperdos_x86_80286_segment_descriptor* gateDescriptor,
        hyperdos_x86_80286_task_switch_type          taskSwitchType,
        int*                                         instructionCompleted)
{
    uint32_t gateAddress             = 0u;
    uint16_t targetSelector          = 0u;
    uint8_t  currentPrivilegeLevel   = hyperdos_x86_current_privilege_level(processor);
    uint8_t  requestedPrivilegeLevel = hyperdos_x86_selector_requested_privilege_level(gateSelector);
    uint8_t  gatePrivilegeLevel      = hyperdos_x86_80286_descriptor_privilege_level(gateDescriptor);

    if (!hyperdos_x86_80286_descriptor_is_present(gateDescriptor))
    {
        return hyperdos_x86_raise_protected_mode_exception(processor,
                                                           HYPERDOS_X86_INTERRUPT_TYPE_SEGMENT_NOT_PRESENT,
                                                           hyperdos_x86_80286_selector_error_code(gateSelector, 0),
                                                           instructionCompleted);
    }
    if (!hyperdos_x86_privilege_level_allows_data_access(currentPrivilegeLevel,
                                                         requestedPrivilegeLevel,
                                                         gatePrivilegeLevel))
    {
        return hyperdos_x86_raise_general_protection_fault(processor,
                                                           hyperdos_x86_80286_selector_error_code(gateSelector, 0),
                                                           instructionCompleted);
    }
    if (hyperdos_x86_get_80286_descriptor_address(processor, gateSelector, &gateAddress) != HYPERDOS_X86_EXECUTION_OK)
    {
        return hyperdos_x86_raise_general_protection_fault(processor,
                                                           hyperdos_x86_80286_selector_error_code(gateSelector, 0),
                                                           instructionCompleted);
    }

    targetSelector = hyperdos_x86_read_physical_word(processor, gateAddress + 2u);
    return hyperdos_x86_execute_80286_task_switch(processor,
                                                  targetSelector,
                                                  taskSwitchType,
                                                  hyperdos_x86_read_instruction_pointer_word(processor),
                                                  0,
                                                  0,
                                                  instructionCompleted);
}

static hyperdos_x86_execution_result hyperdos_x86_execute_far_control_transfer(hyperdos_x86_processor* processor,
                                                                               uint16_t                selector,
                                                                               uint16_t                offset,
                                                                               int  pushReturnAddress,
                                                                               int* instructionCompleted)
{
    hyperdos_x86_80286_segment_descriptor descriptor;
    uint8_t                               descriptorType = 0u;

    if (!hyperdos_x86_processor_is_protected_mode_active(processor))
    {
        if (pushReturnAddress)
        {
            hyperdos_x86_push_word(processor, processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].selector);
            hyperdos_x86_push_word(processor, hyperdos_x86_read_instruction_pointer_word(processor));
        }
        hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE, selector);
        hyperdos_x86_write_instruction_pointer_word(processor, offset);
        return HYPERDOS_X86_EXECUTION_OK;
    }

    if ((selector & 0xFFF8u) == 0u ||
        hyperdos_x86_read_80286_segment_descriptor(processor, selector, &descriptor) != HYPERDOS_X86_EXECUTION_OK)
    {
        return hyperdos_x86_raise_general_protection_fault(processor,
                                                           hyperdos_x86_80286_selector_error_code(selector, 0),
                                                           instructionCompleted);
    }

    if (hyperdos_x86_80286_descriptor_is_code_or_data(&descriptor))
    {
        return hyperdos_x86_execute_80286_direct_code_transfer(processor,
                                                               selector,
                                                               offset,
                                                               &descriptor,
                                                               pushReturnAddress,
                                                               instructionCompleted);
    }

    descriptorType = (uint8_t)(descriptor.access & HYPERDOS_X86_80286_SYSTEM_DESCRIPTOR_TYPE_MASK);
    if (descriptorType == HYPERDOS_X86_80286_CALL_GATE)
    {
        return hyperdos_x86_execute_80286_call_gate_transfer(processor,
                                                             selector,
                                                             &descriptor,
                                                             pushReturnAddress,
                                                             instructionCompleted);
    }
    if (descriptorType == HYPERDOS_X86_80286_TASK_GATE)
    {
        return hyperdos_x86_execute_80286_task_gate_transfer(processor,
                                                             selector,
                                                             &descriptor,
                                                             pushReturnAddress ? HYPERDOS_X86_80286_TASK_SWITCH_CALL
                                                                               : HYPERDOS_X86_80286_TASK_SWITCH_JUMP,
                                                             instructionCompleted);
    }
    if (descriptorType == HYPERDOS_X86_80286_AVAILABLE_TASK_STATE_SEGMENT ||
        descriptorType == HYPERDOS_X86_80286_BUSY_TASK_STATE_SEGMENT)
    {
        return hyperdos_x86_execute_80286_task_state_segment_transfer(processor,
                                                                      selector,
                                                                      &descriptor,
                                                                      pushReturnAddress,
                                                                      instructionCompleted);
    }
    return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
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
    hyperdos_x86_normalize_current_flags(processor);
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_interrupt_with_return_address(
        hyperdos_x86_processor* processor,
        uint8_t                 interruptNumber,
        uint16_t                returnSegment,
        uint16_t                returnOffset);

static hyperdos_x86_execution_result hyperdos_x86_execute_software_interrupt(hyperdos_x86_processor* processor,
                                                                             uint8_t                 interruptNumber);

static hyperdos_x86_execution_result hyperdos_x86_execute_80286_system_operation_code_escape(
        hyperdos_x86_processor*             processor,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride,
        int*                                instructionCompleted);

static inline void hyperdos_x86_set_zero_flag_from_condition(hyperdos_x86_processor* processor, int condition);

static void hyperdos_x86_mark_instruction_faulted(int* instructionCompleted)
{
    if (instructionCompleted != NULL)
    {
        *instructionCompleted = 0;
    }
}

static hyperdos_x86_execution_result hyperdos_x86_execute_unused_operation_code_interrupt(
        hyperdos_x86_processor* processor,
        int*                    instructionCompleted)
{
    hyperdos_x86_mark_instruction_faulted(instructionCompleted);
    return hyperdos_x86_execute_interrupt_with_return_address(processor,
                                                              HYPERDOS_X86_INTERRUPT_TYPE_UNUSED_OPERATION_CODE,
                                                              processor->lastInstructionSegment,
                                                              (uint16_t)processor->lastInstructionOffset);
}

static hyperdos_x86_execution_result hyperdos_x86_execute_escape_operation_code_interrupt(
        hyperdos_x86_processor* processor,
        int*                    instructionCompleted)
{
    hyperdos_x86_mark_instruction_faulted(instructionCompleted);
    return hyperdos_x86_execute_interrupt_with_return_address(processor,
                                                              HYPERDOS_X86_INTERRUPT_TYPE_ESCAPE_OPERATION_CODE,
                                                              processor->lastInstructionSegment,
                                                              (uint16_t)processor->lastInstructionOffset);
}

static int hyperdos_x86_execute_operation_code_validity_action(hyperdos_x86_processor*             processor,
                                                               uint8_t                             operationCode,
                                                               int                                 hasSegmentOverride,
                                                               hyperdos_x86_segment_register_index segmentOverride,
                                                               hyperdos_x86_execution_result*      result,
                                                               int*                                instructionCompleted)
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
        *result = hyperdos_x86_execute_unused_operation_code_interrupt(processor, instructionCompleted);
        return 1;
    case HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_POP_CODE_SEGMENT:
        hyperdos_x86_set_segment_register(processor,
                                          HYPERDOS_X86_SEGMENT_REGISTER_CODE,
                                          hyperdos_x86_pop_word(processor));
        *result = HYPERDOS_X86_EXECUTION_OK;
        return 1;
    case HYPERDOS_X86_OPERATION_CODE_VALIDITY_ACTION_80286_SYSTEM_OPERATION_CODE_ESCAPE:
        *result = hyperdos_x86_execute_80286_system_operation_code_escape(processor,
                                                                          hasSegmentOverride,
                                                                          segmentOverride,
                                                                          instructionCompleted);
        return 1;
    }
    return 0;
}

static hyperdos_x86_execution_result hyperdos_x86_validate_mod_register_memory_extension(
        hyperdos_x86_processor* processor,
        uint8_t                 operationCode,
        uint8_t                 registerMemoryByte,
        int*                    instructionCompleted,
        int*                    handled)
{
    const hyperdos_x86_operation_code_entry* operationCodeEntry = hyperdos_x86_find_operation_code_entry(operationCode);
    uint8_t operation = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                  HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);

    if (handled != NULL)
    {
        *handled = 0;
    }

    switch (operationCodeEntry->modRegisterMemoryExtensionValidityRule)
    {
    case HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_GROUP_FOUR:
        if (operation != HYPERDOS_X86_GROUP_FOUR_UNUSED)
        {
            return HYPERDOS_X86_EXECUTION_OK;
        }
        break;
    case HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_GROUP_FIVE:
        if (operation != HYPERDOS_X86_GROUP_FIVE_UNUSED)
        {
            return HYPERDOS_X86_EXECUTION_OK;
        }
        break;
    case HYPERDOS_X86_MOD_REGISTER_MEMORY_EXTENSION_VALIDITY_RULE_NONE:
        return HYPERDOS_X86_EXECUTION_OK;
    }

    if (handled != NULL)
    {
        *handled = 1;
    }
    if (hyperdos_x86_supports_80186_instructions(processor))
    {
        return hyperdos_x86_execute_unused_operation_code_interrupt(processor, instructionCompleted);
    }
    return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_bound_instruction(
        hyperdos_x86_processor*             processor,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride,
        int*                                instructionCompleted)
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
        hyperdos_x86_mark_instruction_faulted(instructionCompleted);
        return hyperdos_x86_execute_interrupt_with_return_address(processor,
                                                                  HYPERDOS_X86_INTERRUPT_TYPE_BOUND_RANGE_EXCEEDED,
                                                                  processor->lastInstructionSegment,
                                                                  (uint16_t)processor->lastInstructionOffset);
    }
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_adjust_requested_privilege_level_instruction(
        hyperdos_x86_processor*             processor,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride,
        int*                                instructionCompleted)
{
    uint8_t                      registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
    uint8_t                      registerIndex = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                      HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
    hyperdos_x86_decoded_operand operand       = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                       registerMemoryByte,
                                                                                       hasSegmentOverride,
                                                                                       segmentOverride);
    uint16_t                     destinationSelector = hyperdos_x86_read_operand_value(processor, &operand, 1);
    uint16_t                     sourceSelector      = hyperdos_x86_read_general_register_word(processor,
                                                                      (hyperdos_x86_general_register_index)
                                                                              registerIndex);
    uint16_t sourceRequestedPrivilegeLevel           = sourceSelector & HYPERDOS_X86_SELECTOR_REQUESTED_PRIVILEGE_MASK;
    uint16_t destinationRequestedPrivilegeLevel = destinationSelector & HYPERDOS_X86_SELECTOR_REQUESTED_PRIVILEGE_MASK;

    if (!hyperdos_x86_processor_is_protected_mode_active(processor))
    {
        return hyperdos_x86_execute_unused_operation_code_interrupt(processor, instructionCompleted);
    }
    if ((sourceSelector & 0xFFF8u) == 0u)
    {
        hyperdos_x86_mark_instruction_faulted(instructionCompleted);
        hyperdos_x86_set_pending_exception(processor, HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT, 1, 0u);
        return HYPERDOS_X86_EXECUTION_OK;
    }

    if (destinationRequestedPrivilegeLevel < sourceRequestedPrivilegeLevel)
    {
        destinationSelector = (uint16_t)((destinationSelector &
                                          (uint16_t)~HYPERDOS_X86_SELECTOR_REQUESTED_PRIVILEGE_MASK) |
                                         sourceRequestedPrivilegeLevel);
        hyperdos_x86_write_operand_value(processor, &operand, 1, destinationSelector);
        hyperdos_x86_set_zero_flag_from_condition(processor, 1);
    }
    else
    {
        hyperdos_x86_set_zero_flag_from_condition(processor, 0);
    }
    return HYPERDOS_X86_EXECUTION_OK;
}

static void hyperdos_x86_store_80286_descriptor_table_state(hyperdos_x86_processor*                    processor,
                                                            const hyperdos_x86_decoded_operand*        operand,
                                                            const hyperdos_x86_descriptor_table_state* tableState)
{
    hyperdos_x86_write_memory_word_fast(processor, operand->segmentRegister, operand->offset, tableState->limit);
    hyperdos_x86_write_memory_byte_fast(processor,
                                        operand->segmentRegister,
                                        (uint16_t)(operand->offset + 2u),
                                        (uint8_t)(tableState->base & HYPERDOS_X86_BYTE_MASK));
    hyperdos_x86_write_memory_byte_fast(processor,
                                        operand->segmentRegister,
                                        (uint16_t)(operand->offset + 3u),
                                        (uint8_t)((tableState->base >> 8u) & HYPERDOS_X86_BYTE_MASK));
    hyperdos_x86_write_memory_byte_fast(processor,
                                        operand->segmentRegister,
                                        (uint16_t)(operand->offset + 4u),
                                        (uint8_t)((tableState->base >> 16u) & HYPERDOS_X86_BYTE_MASK));
    hyperdos_x86_write_memory_byte_fast(processor,
                                        operand->segmentRegister,
                                        (uint16_t)(operand->offset + 5u),
                                        HYPERDOS_X86_80286_DESCRIPTOR_TABLE_STORED_HIGH_BYTE);
}

static void hyperdos_x86_load_80286_descriptor_table_state(const hyperdos_x86_processor*        processor,
                                                           const hyperdos_x86_decoded_operand*  operand,
                                                           hyperdos_x86_descriptor_table_state* tableState)
{
    uint32_t baseLowByte    = hyperdos_x86_read_memory_byte_fast(processor,
                                                              operand->segmentRegister,
                                                              (uint16_t)(operand->offset + 2u));
    uint32_t baseMiddleByte = hyperdos_x86_read_memory_byte_fast(processor,
                                                                 operand->segmentRegister,
                                                                 (uint16_t)(operand->offset + 3u));
    uint32_t baseHighByte   = hyperdos_x86_read_memory_byte_fast(processor,
                                                               operand->segmentRegister,
                                                               (uint16_t)(operand->offset + 4u));

    tableState->limit = hyperdos_x86_read_memory_word_fast(processor, operand->segmentRegister, operand->offset);
    tableState->base  = (baseLowByte | (baseMiddleByte << 8u) | (baseHighByte << 16u)) &
                       HYPERDOS_X86_80286_DESCRIPTOR_TABLE_BASE_MASK;
}

static hyperdos_x86_execution_result hyperdos_x86_load_80286_machine_status_word(hyperdos_x86_processor* processor,
                                                                                 uint16_t                value,
                                                                                 int* instructionCompleted)
{
    uint16_t newMachineStatusWord = (uint16_t)(value & HYPERDOS_X86_MACHINE_STATUS_WORD_80286_MASK);

    if (hyperdos_x86_processor_is_protected_mode_active(processor) &&
        hyperdos_x86_current_privilege_level(processor) != 0u)
    {
        return hyperdos_x86_raise_general_protection_fault(processor, 0u, instructionCompleted);
    }

    if ((processor->machineStatusWord & HYPERDOS_X86_MACHINE_STATUS_WORD_PROTECTION_ENABLE) != 0u)
    {
        newMachineStatusWord |= HYPERDOS_X86_MACHINE_STATUS_WORD_PROTECTION_ENABLE;
    }
    else if ((newMachineStatusWord & HYPERDOS_X86_MACHINE_STATUS_WORD_PROTECTION_ENABLE) != 0u)
    {
        if (!hyperdos_x86_supports_protected_mode(processor))
        {
            return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
        }
        hyperdos_x86_seed_real_address_segment_attributes_for_protected_mode(processor);
    }

    processor->machineStatusWord = newMachineStatusWord;
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_80286_group_seven_instruction(
        hyperdos_x86_processor*             processor,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride,
        int*                                instructionCompleted)
{
    uint8_t                      registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
    uint8_t                      operation = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                  HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
    hyperdos_x86_decoded_operand operand   = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                       registerMemoryByte,
                                                                                       hasSegmentOverride,
                                                                                       segmentOverride);

    switch (operation)
    {
    case 0u:
        if (operand.isRegister)
        {
            return hyperdos_x86_execute_unused_operation_code_interrupt(processor, instructionCompleted);
        }
        hyperdos_x86_store_80286_descriptor_table_state(processor, &operand, &processor->globalDescriptorTable);
        return HYPERDOS_X86_EXECUTION_OK;
    case 1u:
        if (operand.isRegister)
        {
            return hyperdos_x86_execute_unused_operation_code_interrupt(processor, instructionCompleted);
        }
        hyperdos_x86_store_80286_descriptor_table_state(processor, &operand, &processor->interruptDescriptorTable);
        return HYPERDOS_X86_EXECUTION_OK;
    case 2u:
        if (operand.isRegister)
        {
            return hyperdos_x86_execute_unused_operation_code_interrupt(processor, instructionCompleted);
        }
        if (hyperdos_x86_processor_is_protected_mode_active(processor) &&
            hyperdos_x86_current_privilege_level(processor) != 0u)
        {
            return hyperdos_x86_raise_general_protection_fault(processor, 0u, instructionCompleted);
        }
        hyperdos_x86_load_80286_descriptor_table_state(processor, &operand, &processor->globalDescriptorTable);
        return HYPERDOS_X86_EXECUTION_OK;
    case 3u:
        if (operand.isRegister)
        {
            return hyperdos_x86_execute_unused_operation_code_interrupt(processor, instructionCompleted);
        }
        if (hyperdos_x86_processor_is_protected_mode_active(processor) &&
            hyperdos_x86_current_privilege_level(processor) != 0u)
        {
            return hyperdos_x86_raise_general_protection_fault(processor, 0u, instructionCompleted);
        }
        hyperdos_x86_load_80286_descriptor_table_state(processor, &operand, &processor->interruptDescriptorTable);
        return HYPERDOS_X86_EXECUTION_OK;
    case 4u:
        hyperdos_x86_write_operand_value(processor, &operand, 1, processor->machineStatusWord);
        return HYPERDOS_X86_EXECUTION_OK;
    case 6u:
        return hyperdos_x86_load_80286_machine_status_word(processor,
                                                           hyperdos_x86_read_operand_value(processor, &operand, 1),
                                                           instructionCompleted);
    default:
        return hyperdos_x86_execute_unused_operation_code_interrupt(processor, instructionCompleted);
    }
}

static inline void hyperdos_x86_set_zero_flag_from_condition(hyperdos_x86_processor* processor, int condition)
{
    if (condition)
    {
        processor->flags |= HYPERDOS_X86_FLAG_ZERO;
    }
    else
    {
        processor->flags &= (uint16_t)~HYPERDOS_X86_FLAG_ZERO;
    }
    hyperdos_x86_normalize_current_flags(processor);
}

static int hyperdos_x86_80286_descriptor_is_visible_for_current_privilege(
        const hyperdos_x86_processor*                processor,
        uint16_t                                     selector,
        const hyperdos_x86_80286_segment_descriptor* descriptor)
{
    uint8_t currentPrivilegeLevel    = hyperdos_x86_current_privilege_level(processor);
    uint8_t requestedPrivilegeLevel  = hyperdos_x86_selector_requested_privilege_level(selector);
    uint8_t descriptorPrivilegeLevel = hyperdos_x86_80286_descriptor_privilege_level(descriptor);

    if (!hyperdos_x86_80286_descriptor_is_present(descriptor))
    {
        return 0;
    }
    if (!hyperdos_x86_80286_descriptor_is_code_or_data(descriptor))
    {
        return 1;
    }
    if (hyperdos_x86_80286_descriptor_is_executable(descriptor) &&
        (descriptor->access & HYPERDOS_X86_80286_ACCESS_EXPAND_DOWN_CONFORMING) != 0u)
    {
        return descriptorPrivilegeLevel <= currentPrivilegeLevel;
    }
    return hyperdos_x86_privilege_level_allows_data_access(currentPrivilegeLevel,
                                                           requestedPrivilegeLevel,
                                                           descriptorPrivilegeLevel);
}

static int hyperdos_x86_80286_selector_is_verifiable(const hyperdos_x86_processor* processor,
                                                     uint16_t                      selector,
                                                     int                           writeAccess)
{
    hyperdos_x86_80286_segment_descriptor descriptor;

    if (hyperdos_x86_read_80286_segment_descriptor(processor, selector, &descriptor) != HYPERDOS_X86_EXECUTION_OK ||
        !hyperdos_x86_80286_descriptor_is_code_or_data(&descriptor) ||
        !hyperdos_x86_80286_descriptor_is_visible_for_current_privilege(processor, selector, &descriptor))
    {
        return 0;
    }
    if (writeAccess)
    {
        return !hyperdos_x86_80286_descriptor_is_executable(&descriptor) &&
               hyperdos_x86_80286_descriptor_is_readable_or_writable(&descriptor);
    }
    return !hyperdos_x86_80286_descriptor_is_executable(&descriptor) ||
           hyperdos_x86_80286_descriptor_is_readable_or_writable(&descriptor);
}

static hyperdos_x86_execution_result hyperdos_x86_load_80286_local_descriptor_table(hyperdos_x86_processor* processor,
                                                                                    uint16_t                selector,
                                                                                    int* instructionCompleted)
{
    hyperdos_x86_80286_segment_descriptor descriptor;
    uint8_t                               descriptorType = 0u;

    if ((selector & 0xFFF8u) == 0u)
    {
        processor->localDescriptorTableSelector = 0u;
        processor->localDescriptorTable.base    = 0u;
        processor->localDescriptorTable.limit   = 0u;
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if ((selector & HYPERDOS_X86_SELECTOR_TABLE_INDICATOR) != 0u ||
        hyperdos_x86_read_80286_segment_descriptor(processor, selector, &descriptor) != HYPERDOS_X86_EXECUTION_OK)
    {
        if (hyperdos_x86_processor_is_protected_mode_active(processor))
        {
            return hyperdos_x86_raise_general_protection_fault(processor,
                                                               hyperdos_x86_80286_selector_error_code(selector, 0),
                                                               instructionCompleted);
        }
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }

    descriptorType = (uint8_t)(descriptor.access & HYPERDOS_X86_80286_SYSTEM_DESCRIPTOR_TYPE_MASK);
    if (descriptorType != HYPERDOS_X86_80286_LOCAL_DESCRIPTOR_TABLE)
    {
        if (hyperdos_x86_processor_is_protected_mode_active(processor))
        {
            return hyperdos_x86_raise_general_protection_fault(processor,
                                                               hyperdos_x86_80286_selector_error_code(selector, 0),
                                                               instructionCompleted);
        }
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }
    if (!hyperdos_x86_80286_descriptor_is_present(&descriptor))
    {
        if (hyperdos_x86_processor_is_protected_mode_active(processor))
        {
            return hyperdos_x86_raise_protected_mode_exception(processor,
                                                               HYPERDOS_X86_INTERRUPT_TYPE_SEGMENT_NOT_PRESENT,
                                                               hyperdos_x86_80286_selector_error_code(selector, 0),
                                                               instructionCompleted);
        }
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }
    processor->localDescriptorTableSelector = selector;
    processor->localDescriptorTable.base    = descriptor.base & HYPERDOS_X86_80286_DESCRIPTOR_TABLE_BASE_MASK;
    processor->localDescriptorTable.limit   = (uint16_t)descriptor.limit;
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_load_80286_task_register(hyperdos_x86_processor* processor,
                                                                           uint16_t                selector,
                                                                           int*                    instructionCompleted)
{
    hyperdos_x86_80286_segment_descriptor descriptor;
    uint32_t                              descriptorAddress = 0u;
    uint8_t                               descriptorType    = 0u;

    if ((selector & 0xFFF8u) == 0u || (selector & HYPERDOS_X86_SELECTOR_TABLE_INDICATOR) != 0u ||
        hyperdos_x86_read_80286_segment_descriptor(processor, selector, &descriptor) != HYPERDOS_X86_EXECUTION_OK)
    {
        if (hyperdos_x86_processor_is_protected_mode_active(processor))
        {
            return hyperdos_x86_raise_general_protection_fault(processor,
                                                               hyperdos_x86_80286_selector_error_code(selector, 0),
                                                               instructionCompleted);
        }
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }
    descriptorType = (uint8_t)(descriptor.access & HYPERDOS_X86_80286_SYSTEM_DESCRIPTOR_TYPE_MASK);
    if (descriptorType != HYPERDOS_X86_80286_AVAILABLE_TASK_STATE_SEGMENT)
    {
        if (hyperdos_x86_processor_is_protected_mode_active(processor))
        {
            return hyperdos_x86_raise_general_protection_fault(processor,
                                                               hyperdos_x86_80286_selector_error_code(selector, 0),
                                                               instructionCompleted);
        }
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }
    if (!hyperdos_x86_80286_descriptor_is_present(&descriptor))
    {
        if (hyperdos_x86_processor_is_protected_mode_active(processor))
        {
            return hyperdos_x86_raise_protected_mode_exception(processor,
                                                               HYPERDOS_X86_INTERRUPT_TYPE_SEGMENT_NOT_PRESENT,
                                                               hyperdos_x86_80286_selector_error_code(selector, 0),
                                                               instructionCompleted);
        }
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }
    if (descriptor.limit < HYPERDOS_X86_80286_TASK_STATE_SEGMENT_MINIMUM_LIMIT)
    {
        if (hyperdos_x86_processor_is_protected_mode_active(processor))
        {
            return hyperdos_x86_raise_protected_mode_exception(processor,
                                                               HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT,
                                                               hyperdos_x86_80286_selector_error_code(selector, 0),
                                                               instructionCompleted);
        }
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }
    processor->taskRegisterSelector    = selector;
    processor->taskRegister.selector   = selector;
    processor->taskRegister.base       = descriptor.base & HYPERDOS_X86_80286_DESCRIPTOR_TABLE_BASE_MASK;
    processor->taskRegister.limit      = descriptor.limit;
    processor->taskRegister.attributes = descriptor.access;
    if (descriptorType == HYPERDOS_X86_80286_AVAILABLE_TASK_STATE_SEGMENT &&
        hyperdos_x86_get_80286_descriptor_address(processor, selector, &descriptorAddress) == HYPERDOS_X86_EXECUTION_OK)
    {
        descriptor.access = (uint8_t)((descriptor.access & 0xE0u) | HYPERDOS_X86_80286_BUSY_TASK_STATE_SEGMENT);
        processor->taskRegister.attributes = descriptor.access;
        hyperdos_x86_write_physical_byte(processor, descriptorAddress + 5u, descriptor.access);
    }
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_80286_group_six_instruction(
        hyperdos_x86_processor*             processor,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride,
        int*                                instructionCompleted)
{
    uint8_t                      registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
    uint8_t                      operation = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                  HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
    hyperdos_x86_decoded_operand operand   = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                       registerMemoryByte,
                                                                                       hasSegmentOverride,
                                                                                       segmentOverride);
    uint16_t                     selector  = 0u;

    switch (operation)
    {
    case 0u:
        hyperdos_x86_write_operand_value(processor, &operand, 1, processor->localDescriptorTableSelector);
        return HYPERDOS_X86_EXECUTION_OK;
    case 1u:
        hyperdos_x86_write_operand_value(processor, &operand, 1, processor->taskRegisterSelector);
        return HYPERDOS_X86_EXECUTION_OK;
    case 2u:
        if (hyperdos_x86_processor_is_protected_mode_active(processor) &&
            hyperdos_x86_current_privilege_level(processor) != 0u)
        {
            return hyperdos_x86_raise_general_protection_fault(processor, 0u, instructionCompleted);
        }
        selector = hyperdos_x86_read_operand_value(processor, &operand, 1);
        return hyperdos_x86_load_80286_local_descriptor_table(processor, selector, instructionCompleted);
    case 3u:
        if (hyperdos_x86_processor_is_protected_mode_active(processor) &&
            hyperdos_x86_current_privilege_level(processor) != 0u)
        {
            return hyperdos_x86_raise_general_protection_fault(processor, 0u, instructionCompleted);
        }
        selector = hyperdos_x86_read_operand_value(processor, &operand, 1);
        return hyperdos_x86_load_80286_task_register(processor, selector, instructionCompleted);
    case 4u:
    case 5u:
        selector = hyperdos_x86_read_operand_value(processor, &operand, 1);
        hyperdos_x86_set_zero_flag_from_condition(processor,
                                                  hyperdos_x86_80286_selector_is_verifiable(processor,
                                                                                            selector,
                                                                                            operation == 5u));
        return HYPERDOS_X86_EXECUTION_OK;
    default:
        return hyperdos_x86_execute_unused_operation_code_interrupt(processor, instructionCompleted);
    }
}

static hyperdos_x86_execution_result hyperdos_x86_execute_80286_load_access_rights_instruction(
        hyperdos_x86_processor*             processor,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride,
        int                                 loadLimit)
{
    uint8_t                      registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
    uint8_t                      registerIndex = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                      HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
    hyperdos_x86_decoded_operand operand       = hyperdos_x86_decode_register_memory_operand(processor,
                                                                                       registerMemoryByte,
                                                                                       hasSegmentOverride,
                                                                                       segmentOverride);
    uint16_t                     selector      = hyperdos_x86_read_operand_value(processor, &operand, 1);
    hyperdos_x86_80286_segment_descriptor descriptor;

    if (hyperdos_x86_read_80286_segment_descriptor(processor, selector, &descriptor) != HYPERDOS_X86_EXECUTION_OK ||
        !hyperdos_x86_80286_descriptor_is_visible_for_current_privilege(processor, selector, &descriptor))
    {
        hyperdos_x86_set_zero_flag_from_condition(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }

    hyperdos_x86_write_general_register_word(processor,
                                             (hyperdos_x86_general_register_index)registerIndex,
                                             loadLimit ? (uint16_t)descriptor.limit
                                                       : (uint16_t)(descriptor.access << 8u));
    hyperdos_x86_set_zero_flag_from_condition(processor, 1);
    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_80286_system_operation_code_escape(
        hyperdos_x86_processor*             processor,
        int                                 hasSegmentOverride,
        hyperdos_x86_segment_register_index segmentOverride,
        int*                                instructionCompleted)
{
    uint8_t systemOperationCode = hyperdos_x86_fetch_instruction_byte(processor);

    switch (systemOperationCode)
    {
    case 0x00u:
        return hyperdos_x86_execute_80286_group_six_instruction(processor,
                                                                hasSegmentOverride,
                                                                segmentOverride,
                                                                instructionCompleted);
    case 0x01u:
        return hyperdos_x86_execute_80286_group_seven_instruction(processor,
                                                                  hasSegmentOverride,
                                                                  segmentOverride,
                                                                  instructionCompleted);
    case 0x02u:
        return hyperdos_x86_execute_80286_load_access_rights_instruction(processor,
                                                                         hasSegmentOverride,
                                                                         segmentOverride,
                                                                         0);
    case 0x03u:
        return hyperdos_x86_execute_80286_load_access_rights_instruction(processor,
                                                                         hasSegmentOverride,
                                                                         segmentOverride,
                                                                         1);
    case 0x06u:
        if (hyperdos_x86_processor_is_protected_mode_active(processor) &&
            hyperdos_x86_current_privilege_level(processor) != 0u)
        {
            return hyperdos_x86_raise_general_protection_fault(processor, 0u, instructionCompleted);
        }
        processor->machineStatusWord = (uint16_t)(processor->machineStatusWord &
                                                  (uint16_t)~HYPERDOS_X86_MACHINE_STATUS_WORD_TASK_SWITCHED);
        return HYPERDOS_X86_EXECUTION_OK;
    default:
        return hyperdos_x86_execute_unused_operation_code_interrupt(processor, instructionCompleted);
    }
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
    *physicalAddress = hyperdos_x86_physical_address_from_base(processor,
                                                               processor->segmentStates[segmentRegister].base,
                                                               offset);
    if (byteCount == 0u)
    {
        return 1;
    }
    if (byteCount > (size_t)(HYPERDOS_X86_SEGMENT_BYTE_COUNT - offset))
    {
        return 0;
    }
    if (*physicalAddress > hyperdos_x86_get_physical_address_mask(processor))
    {
        return 0;
    }
    if (*physicalAddress >= processor->memorySize)
    {
        return 0;
    }
    if (byteCount > processor->memorySize - *physicalAddress)
    {
        return 0;
    }
    return 1;
}

static inline int hyperdos_x86_ranges_do_not_overlap(uint32_t firstAddress, uint32_t secondAddress, size_t byteCount)
{
    return firstAddress + byteCount <= secondAddress || secondAddress + byteCount <= firstAddress;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_interrupt_with_return_address_from_source(
        hyperdos_x86_processor* processor,
        uint8_t                 interruptNumber,
        uint16_t                returnSegment,
        uint16_t                returnOffset,
        int                     softwareInterrupt,
        int                     externalEvent)
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

    if (hyperdos_x86_processor_is_protected_mode_active(processor))
    {
        return hyperdos_x86_execute_80286_protected_interrupt_with_return_address(processor,
                                                                                  interruptNumber,
                                                                                  returnSegment,
                                                                                  returnOffset,
                                                                                  0,
                                                                                  0u,
                                                                                  softwareInterrupt,
                                                                                  externalEvent);
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
                                      (uint16_t)(hyperdos_x86_read_flags_word(processor) &
                                                 ~(HYPERDOS_X86_FLAG_TRAP | HYPERDOS_X86_FLAG_INTERRUPT_ENABLE)));
        hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE, vectorSegment);
        hyperdos_x86_write_instruction_pointer_word(processor, vectorOffset);
    }

    return HYPERDOS_X86_EXECUTION_OK;
}

static hyperdos_x86_execution_result hyperdos_x86_execute_interrupt_with_return_address(
        hyperdos_x86_processor* processor,
        uint8_t                 interruptNumber,
        uint16_t                returnSegment,
        uint16_t                returnOffset)
{
    return hyperdos_x86_execute_interrupt_with_return_address_from_source(processor,
                                                                          interruptNumber,
                                                                          returnSegment,
                                                                          returnOffset,
                                                                          0,
                                                                          0);
}

static hyperdos_x86_execution_result hyperdos_x86_execute_software_interrupt_with_return_address(
        hyperdos_x86_processor* processor,
        uint8_t                 interruptNumber,
        uint16_t                returnSegment,
        uint16_t                returnOffset)
{
    return hyperdos_x86_execute_interrupt_with_return_address_from_source(processor,
                                                                          interruptNumber,
                                                                          returnSegment,
                                                                          returnOffset,
                                                                          1,
                                                                          0);
}

static hyperdos_x86_execution_result hyperdos_x86_execute_external_interrupt_with_return_address(
        hyperdos_x86_processor* processor,
        uint8_t                 interruptNumber,
        uint16_t                returnSegment,
        uint16_t                returnOffset)
{
    return hyperdos_x86_execute_interrupt_with_return_address_from_source(processor,
                                                                          interruptNumber,
                                                                          returnSegment,
                                                                          returnOffset,
                                                                          0,
                                                                          1);
}

static hyperdos_x86_execution_result hyperdos_x86_execute_interrupt(hyperdos_x86_processor* processor,
                                                                    uint8_t                 interruptNumber)
{
    return hyperdos_x86_execute_interrupt_with_return_address(
            processor,
            interruptNumber,
            processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].selector,
            hyperdos_x86_read_instruction_pointer_word(processor));
}

static hyperdos_x86_execution_result hyperdos_x86_execute_software_interrupt(hyperdos_x86_processor* processor,
                                                                             uint8_t                 interruptNumber)
{
    return hyperdos_x86_execute_software_interrupt_with_return_address(
            processor,
            interruptNumber,
            processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].selector,
            hyperdos_x86_read_instruction_pointer_word(processor));
}

static hyperdos_x86_execution_result hyperdos_x86_execute_external_interrupt(hyperdos_x86_processor* processor,
                                                                             uint8_t                 interruptNumber)
{
    return hyperdos_x86_execute_external_interrupt_with_return_address(
            processor,
            interruptNumber,
            processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].selector,
            hyperdos_x86_read_instruction_pointer_word(processor));
}

static hyperdos_x86_execution_result hyperdos_x86_dispatch_pending_exception(hyperdos_x86_processor* processor,
                                                                             int* instructionCompleted)
{
    uint8_t                       exceptionType = processor->pendingExceptionType;
    int                           hasErrorCode  = processor->pendingExceptionHasErrorCode != 0u;
    uint16_t                      errorCode     = processor->pendingExceptionErrorCode;
    uint16_t                      returnSegment = processor->lastInstructionSegment;
    uint16_t                      returnOffset  = (uint16_t)processor->lastInstructionOffset;
    hyperdos_x86_execution_result result        = HYPERDOS_X86_EXECUTION_OK;

    if (processor->processorShutdownActive != 0u)
    {
        return HYPERDOS_X86_EXECUTION_PROCESSOR_SHUTDOWN;
    }

    processor->pendingExceptionActive       = 0u;
    processor->pendingExceptionType         = 0u;
    processor->pendingExceptionHasErrorCode = 0u;
    processor->pendingExceptionErrorCode    = 0u;
    hyperdos_x86_mark_instruction_faulted(instructionCompleted);

    if (hyperdos_x86_processor_is_protected_mode_active(processor))
    {
        processor->exceptionDeliveryActive = 1u;
        processor->exceptionDeliveryType   = exceptionType;
        result = hyperdos_x86_execute_80286_protected_interrupt_with_return_address(processor,
                                                                                    exceptionType,
                                                                                    returnSegment,
                                                                                    returnOffset,
                                                                                    hasErrorCode,
                                                                                    errorCode,
                                                                                    0,
                                                                                    0);
        processor->exceptionDeliveryActive = 0u;
        processor->exceptionDeliveryType   = 0u;
        if (processor->processorShutdownActive != 0u)
        {
            return HYPERDOS_X86_EXECUTION_PROCESSOR_SHUTDOWN;
        }
        return result;
    }
    return hyperdos_x86_execute_interrupt_with_return_address(processor, exceptionType, returnSegment, returnOffset);
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
    if (!hyperdos_x86_io_privilege_allows_current_level(processor))
    {
        hyperdos_x86_set_pending_exception(processor, HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT, 1, 0u);
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
            carryFlag = (uint16_t)((value & signBit) != 0u ? HYPERDOS_X86_FLAG_CARRY : 0u);
            value     = (uint16_t)(((value << 1u) | (carryFlag != 0u ? 1u : 0u)) & valueMask);
            hyperdos_x86_replace_carry_flag(processor, carryFlag);
            break;
        case HYPERDOS_X86_SHIFT_ROTATE_ROR:
            carryFlag = (uint16_t)((value & 1u) != 0u ? HYPERDOS_X86_FLAG_CARRY : 0u);
            value     = (uint16_t)((value >> 1u) | (carryFlag != 0u ? signBit : 0u));
            hyperdos_x86_replace_carry_flag(processor, carryFlag);
            break;
        case HYPERDOS_X86_SHIFT_ROTATE_RCL:
        {
            uint16_t oldCarry = (processor->flags & HYPERDOS_X86_FLAG_CARRY) != 0u ? 1u : 0u;
            carryFlag         = (uint16_t)((value & signBit) != 0u ? HYPERDOS_X86_FLAG_CARRY : 0u);
            value             = (uint16_t)(((value << 1u) | oldCarry) & valueMask);
            hyperdos_x86_replace_carry_flag(processor, carryFlag);
            break;
        }
        case HYPERDOS_X86_SHIFT_ROTATE_RCR:
        {
            uint16_t oldCarry = (processor->flags & HYPERDOS_X86_FLAG_CARRY) != 0u ? signBit : 0u;
            carryFlag         = (uint16_t)((value & 1u) != 0u ? HYPERDOS_X86_FLAG_CARRY : 0u);
            value             = (uint16_t)((value >> 1u) | oldCarry);
            hyperdos_x86_replace_carry_flag(processor, carryFlag);
            break;
        }
        case HYPERDOS_X86_SHIFT_ROTATE_SHL:
            carryFlag = (uint16_t)((value & signBit) != 0u ? HYPERDOS_X86_FLAG_CARRY : 0u);
            value     = (uint16_t)((value << 1u) & valueMask);
            hyperdos_x86_replace_carry_flag(processor, carryFlag);
            break;
        case HYPERDOS_X86_SHIFT_ROTATE_SHR:
            carryFlag = (uint16_t)((value & 1u) != 0u ? HYPERDOS_X86_FLAG_CARRY : 0u);
            value     = (uint16_t)(value >> 1u);
            hyperdos_x86_replace_carry_flag(processor, carryFlag);
            break;
        case HYPERDOS_X86_SHIFT_ROTATE_SAR:
            carryFlag = (uint16_t)((value & 1u) != 0u ? HYPERDOS_X86_FLAG_CARRY : 0u);
            value     = (uint16_t)((value >> 1u) | (value & signBit));
            hyperdos_x86_replace_carry_flag(processor, carryFlag);
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
        hyperdos_x86_write_flags_word(processor,
                                      (uint16_t)((processor->flags & (uint16_t)~HYPERDOS_X86_FLAG_OVERFLOW) |
                                                 overflowFlag));
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

    statusFlags |= carryFlag | auxiliaryCarryFlag;
    hyperdos_x86_write_flags_word(processor, (uint16_t)((processor->flags & ~statusMask) | statusFlags));
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
    hyperdos_x86_write_flags_word(processor,
                                  (uint16_t)((processor->flags &
                                              ~(HYPERDOS_X86_FLAG_CARRY | HYPERDOS_X86_FLAG_AUXILIARY_CARRY)) |
                                             carryAuxiliaryFlags));
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
        hyperdos_x86_segment_register_index segmentOverride,
        int*                                instructionCompleted)
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

    if (hyperdos_x86_80286_processor_extension_trap_is_active(processor, 0) ||
        hyperdos_x86_80186_escape_trap_is_active(processor))
    {
        return hyperdos_x86_execute_escape_operation_code_interrupt(processor, instructionCompleted);
    }
    if (processor->coprocessorEscapeHandler != NULL)
    {
        return processor->coprocessorEscapeHandler(processor, &instruction, processor->coprocessorContext);
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
        hyperdos_x86_normalize_current_flags(processor);
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
        hyperdos_x86_normalize_current_flags(processor);
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
        hyperdos_x86_normalize_current_flags(processor);
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
        hyperdos_x86_segment_register_index segmentOverride,
        int*                                instructionCompleted)
{
    uint8_t                       registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
    uint8_t                       operation = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                  HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
    hyperdos_x86_decoded_operand  operand;
    uint16_t                      value                   = 0u;
    int                           invalidExtensionHandled = 0;
    hyperdos_x86_execution_result validationResult        = HYPERDOS_X86_EXECUTION_OK;

    validationResult = hyperdos_x86_validate_mod_register_memory_extension(processor,
                                                                           HYPERDOS_X86_OPERATION_CODE_GROUP_FIVE,
                                                                           registerMemoryByte,
                                                                           instructionCompleted,
                                                                           &invalidExtensionHandled);
    if (validationResult != HYPERDOS_X86_EXECUTION_OK || invalidExtensionHandled)
    {
        return validationResult;
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
        return hyperdos_x86_execute_far_control_transfer(processor, segment, value, 1, instructionCompleted);
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
        return hyperdos_x86_execute_far_control_transfer(processor, segment, value, 0, instructionCompleted);
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
    processor->interruptDescriptorTable.limit          = HYPERDOS_X86_80286_DESCRIPTOR_TABLE_RESET_LIMIT;
    processor->relocationRegister                      = hyperdos_x86_model_relocation_register_reset_value(processor);
    hyperdos_x86_write_flags_word(processor, HYPERDOS_X86_FLAG_INTERRUPT_ENABLE);
    hyperdos_x86_load_real_mode_segment(processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE, HYPERDOS_X86_RESET_CODE_SEGMENT);
    hyperdos_x86_write_instruction_pointer_word(processor, HYPERDOS_X86_RESET_INSTRUCTION_POINTER);
    hyperdos_x86_load_real_mode_segment(processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0u);
    hyperdos_x86_load_real_mode_segment(processor, HYPERDOS_X86_SEGMENT_REGISTER_STACK, 0u);
    hyperdos_x86_load_real_mode_segment(processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA, 0u);
}

void hyperdos_x86_set_processor_model(hyperdos_x86_processor* processor, hyperdos_x86_processor_model model)
{
    if (processor == NULL || (size_t)model >= hyperdos_x86_processor_descriptor_count())
    {
        return;
    }
    processor->processorModel     = (uint8_t)model;
    processor->relocationRegister = hyperdos_x86_model_relocation_register_reset_value(processor);
    hyperdos_x86_normalize_current_flags(processor);
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
    if (enabled)
    {
        processor->relocationRegister = (uint16_t)(processor->relocationRegister |
                                                   HYPERDOS_X86_80186_RELOCATION_REGISTER_ESCAPE_TRAP);
    }
    else
    {
        processor->relocationRegister = (uint16_t)(processor->relocationRegister &
                                                   (uint16_t)~HYPERDOS_X86_80186_RELOCATION_REGISTER_ESCAPE_TRAP);
    }
}

uint16_t hyperdos_x86_get_relocation_register(const hyperdos_x86_processor* processor)
{
    if (processor == NULL)
    {
        return 0u;
    }
    return processor->relocationRegister;
}

void hyperdos_x86_set_relocation_register(hyperdos_x86_processor* processor, uint16_t value)
{
    if (processor == NULL)
    {
        return;
    }
    processor->relocationRegister = value;
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

    programBase = processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].base;
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
    if (processor->processorShutdownActive != 0u)
    {
        return HYPERDOS_X86_EXECUTION_PROCESSOR_SHUTDOWN;
    }

    while (!processor->halted)
    {
        uint16_t instructionStartOffset  = hyperdos_x86_read_instruction_pointer_word(processor);
        uint16_t instructionStartSegment = processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].selector;
        hyperdos_x86_decoded_instruction    decodedInstruction;
        uint8_t                             operationCode      = 0;
        int                                 hasSegmentOverride = 0;
        hyperdos_x86_segment_register_index segmentOverride    = HYPERDOS_X86_SEGMENT_REGISTER_DATA;
        hyperdos_x86_repeat_mode            repeatMode         = HYPERDOS_X86_REPEAT_NONE;
        int maskableInterruptInhibitWasActive                  = processor->maskableInterruptInhibitCount != 0u;
        int trapWasEnabled                                     = (processor->flags & HYPERDOS_X86_FLAG_TRAP) != 0u;
        int startMaskableInterruptInhibit                      = 0;
        int suppressTrapAfterInstruction                       = 0;
        int instructionCompleted                               = 1;
        hyperdos_x86_execution_result result                   = HYPERDOS_X86_EXECUTION_OK;

        if (instructionLimit != 0u && processor->executedInstructionCount >= instructionLimit)
        {
            return HYPERDOS_X86_EXECUTION_STEP_LIMIT_REACHED;
        }
        if (processor->processorShutdownActive != 0u)
        {
            return HYPERDOS_X86_EXECUTION_PROCESSOR_SHUTDOWN;
        }

        result = hyperdos_x86_decode_next_instruction(processor, &decodedInstruction);
        if (result != HYPERDOS_X86_EXECUTION_OK)
        {
            return result;
        }
        operationCode      = decodedInstruction.operationCode;
        hasSegmentOverride = decodedInstruction.segmentOverrideActive != 0u;
        segmentOverride    = decodedInstruction.segmentOverride;
        repeatMode         = decodedInstruction.repeatMode;

        processor->lastOperationCode      = operationCode;
        processor->lastInstructionSegment = instructionStartSegment;
        processor->lastInstructionOffset  = instructionStartOffset;

        if (decodedInstruction.lockPrefixActive != 0u && !hyperdos_x86_io_privilege_allows_current_level(processor))
        {
            result = hyperdos_x86_raise_general_protection_fault(processor, 0u, &instructionCompleted);
        }
        else if (hyperdos_x86_execute_operation_code_validity_action(processor,
                                                                     operationCode,
                                                                     hasSegmentOverride,
                                                                     segmentOverride,
                                                                     &result,
                                                                     &instructionCompleted))
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
                result = hyperdos_x86_execute_bound_instruction(processor,
                                                                hasSegmentOverride,
                                                                segmentOverride,
                                                                &instructionCompleted);
            }
        }
        else if (operationCode == HYPERDOS_X86_OPERATION_CODE_ADJUST_REQUESTED_PRIVILEGE_LEVEL)
        {
            result = hyperdos_x86_execute_adjust_requested_privilege_level_instruction(processor,
                                                                                       hasSegmentOverride,
                                                                                       segmentOverride,
                                                                                       &instructionCompleted);
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
                                                                         segmentOverride,
                                                                         &instructionCompleted);
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
                hyperdos_x86_push_word(processor,
                                       processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_EXTRA].selector);
                break;
            case HYPERDOS_X86_OPERATION_CODE_POP_EXTRA_SEGMENT:
                hyperdos_x86_set_segment_register(processor,
                                                  HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                  hyperdos_x86_pop_word(processor));
                break;
            case HYPERDOS_X86_OPERATION_CODE_PUSH_CODE_SEGMENT:
                hyperdos_x86_push_word(processor,
                                       processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_CODE].selector);
                break;
            case HYPERDOS_X86_OPERATION_CODE_PUSH_STACK_SEGMENT:
                hyperdos_x86_push_word(processor,
                                       processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_STACK].selector);
                break;
            case HYPERDOS_X86_OPERATION_CODE_POP_STACK_SEGMENT:
                hyperdos_x86_set_segment_register(processor,
                                                  HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                                  hyperdos_x86_pop_word(processor));
                startMaskableInterruptInhibit = 1;
                break;
            case HYPERDOS_X86_OPERATION_CODE_PUSH_DATA_SEGMENT:
                hyperdos_x86_push_word(processor,
                                       processor->segmentStates[HYPERDOS_X86_SEGMENT_REGISTER_DATA].selector);
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
                hyperdos_x86_write_operand_value(processor,
                                                 &operand,
                                                 1,
                                                 processor->segmentStates[segmentRegister].selector);
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
                result           = hyperdos_x86_execute_far_control_transfer(processor,
                                                                   segment,
                                                                   offset,
                                                                   1,
                                                                   &instructionCompleted);
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_PUSH_FLAGS:
                hyperdos_x86_normalize_current_flags(processor);
                hyperdos_x86_push_word(processor, hyperdos_x86_read_flags_word(processor));
                break;

            case HYPERDOS_X86_OPERATION_CODE_POP_FLAGS:
                hyperdos_x86_restore_flags_word_from_stack(processor, hyperdos_x86_pop_word(processor));
                suppressTrapAfterInstruction = !trapWasEnabled;
                break;

            case HYPERDOS_X86_OPERATION_CODE_STORE_STATUS_TO_FLAGS:
            {
                uint16_t accumulator =
                        hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
                hyperdos_x86_write_flags_word(processor,
                                              (uint16_t)((hyperdos_x86_read_flags_word(processor) &
                                                          HYPERDOS_X86_FLAGS_HIGH_BYTE_MASK) |
                                                         ((accumulator >> HYPERDOS_X86_BYTE_BIT_COUNT) &
                                                          HYPERDOS_X86_FLAGS_LOW_BYTE_TRANSFER_MASK)));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_LOAD_STATUS_FROM_FLAGS:
                hyperdos_x86_write_byte_register(processor,
                                                 HYPERDOS_X86_BYTE_REGISTER_HIGH_PART_BIT,
                                                 (uint8_t)(processor->flags &
                                                           HYPERDOS_X86_FLAGS_LOW_BYTE_TRANSFER_MASK));
                break;

            case HYPERDOS_X86_OPERATION_CODE_WAIT:
                if (hyperdos_x86_80286_processor_extension_trap_is_active(processor, 1))
                {
                    result = hyperdos_x86_execute_escape_operation_code_interrupt(processor, &instructionCompleted);
                    break;
                }
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
                if (hyperdos_x86_processor_is_protected_mode_active(processor))
                {
                    result = hyperdos_x86_execute_80286_protected_far_return(processor,
                                                                             stackByteCount,
                                                                             0,
                                                                             &instructionCompleted);
                    break;
                }
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
                if (hyperdos_x86_processor_is_protected_mode_active(processor))
                {
                    result = hyperdos_x86_execute_80286_protected_far_return(processor, 0u, 0, &instructionCompleted);
                    break;
                }
                hyperdos_x86_write_instruction_pointer_word(processor, hyperdos_x86_pop_word(processor));
                hyperdos_x86_set_segment_register(processor,
                                                  HYPERDOS_X86_SEGMENT_REGISTER_CODE,
                                                  hyperdos_x86_pop_word(processor));
                break;

            case HYPERDOS_X86_OPERATION_CODE_INTERRUPT_BREAKPOINT:
                suppressTrapAfterInstruction = 1;
                result = hyperdos_x86_execute_software_interrupt(processor, HYPERDOS_X86_INTERRUPT_TYPE_BREAKPOINT);
                break;

            case HYPERDOS_X86_OPERATION_CODE_INTERRUPT_IMMEDIATE:
                suppressTrapAfterInstruction = 1;
                result                       = hyperdos_x86_execute_software_interrupt(processor,
                                                                 hyperdos_x86_fetch_instruction_byte(processor));
                break;

            case HYPERDOS_X86_OPERATION_CODE_INTERRUPT_ON_OVERFLOW:
                if ((processor->flags & HYPERDOS_X86_FLAG_OVERFLOW) != 0u)
                {
                    suppressTrapAfterInstruction = 1;
                    result = hyperdos_x86_execute_software_interrupt(processor, HYPERDOS_X86_INTERRUPT_TYPE_OVERFLOW);
                }
                break;

            case HYPERDOS_X86_OPERATION_CODE_INTERRUPT_RETURN:
                if (hyperdos_x86_processor_is_protected_mode_active(processor) &&
                    (hyperdos_x86_read_flags_word(processor) & HYPERDOS_X86_FLAGS_80286_NESTED_TASK) != 0u)
                {
                    uint16_t previousTaskSelector = 0u;
                    if (processor->taskRegisterSelector == 0u ||
                        processor->taskRegister.limit < HYPERDOS_X86_80286_TASK_STATE_SEGMENT_MINIMUM_LIMIT)
                    {
                        result = hyperdos_x86_raise_protected_mode_exception(
                                processor,
                                HYPERDOS_X86_INTERRUPT_TYPE_INVALID_TASK_STATE_SEGMENT,
                                processor->taskRegisterSelector,
                                &instructionCompleted);
                        break;
                    }
                    previousTaskSelector = hyperdos_x86_read_80286_task_state_word(
                            processor,
                            processor->taskRegister.base,
                            HYPERDOS_X86_80286_TASK_STATE_SEGMENT_PREVIOUS_TASK);
                    result                       = hyperdos_x86_execute_80286_task_switch(processor,
                                                                    previousTaskSelector,
                                                                    HYPERDOS_X86_80286_TASK_SWITCH_RETURN,
                                                                    hyperdos_x86_read_instruction_pointer_word(
                                                                            processor),
                                                                    0,
                                                                    0,
                                                                    &instructionCompleted);
                    suppressTrapAfterInstruction = 1;
                    break;
                }
                if (hyperdos_x86_processor_is_protected_mode_active(processor))
                {
                    result = hyperdos_x86_execute_80286_protected_far_return(processor, 0u, 1, &instructionCompleted);
                    suppressTrapAfterInstruction = !trapWasEnabled;
                    break;
                }
                hyperdos_x86_write_instruction_pointer_word(processor, hyperdos_x86_pop_word(processor));
                hyperdos_x86_set_segment_register(processor,
                                                  HYPERDOS_X86_SEGMENT_REGISTER_CODE,
                                                  hyperdos_x86_pop_word(processor));
                hyperdos_x86_restore_flags_word_from_stack(processor, hyperdos_x86_pop_word(processor));
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
                if (!hyperdos_x86_io_privilege_allows_current_level(processor))
                {
                    result = hyperdos_x86_raise_general_protection_fault(processor, 0u, &instructionCompleted);
                    break;
                }
                hyperdos_x86_write_byte_register(
                        processor,
                        0u,
                        hyperdos_x86_read_input_output_byte(processor, hyperdos_x86_fetch_instruction_byte(processor)));
                break;

            case HYPERDOS_X86_OPERATION_CODE_INPUT_IMMEDIATE_WORD:
            {
                uint16_t port     = hyperdos_x86_fetch_instruction_byte(processor);
                uint16_t lowByte  = 0u;
                uint16_t highByte = 0u;
                if (!hyperdos_x86_io_privilege_allows_current_level(processor))
                {
                    result = hyperdos_x86_raise_general_protection_fault(processor, 0u, &instructionCompleted);
                    break;
                }
                lowByte  = hyperdos_x86_read_input_output_byte(processor, port);
                highByte = hyperdos_x86_read_input_output_byte(processor, (uint16_t)(port + HYPERDOS_X86_BYTE_SIZE));
                hyperdos_x86_write_general_register_word(processor,
                                                         HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                         (uint16_t)(lowByte |
                                                                    (highByte << HYPERDOS_X86_BYTE_BIT_COUNT)));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_OUTPUT_IMMEDIATE_BYTE:
                if (!hyperdos_x86_io_privilege_allows_current_level(processor))
                {
                    result = hyperdos_x86_raise_general_protection_fault(processor, 0u, &instructionCompleted);
                    break;
                }
                hyperdos_x86_write_input_output_byte(processor,
                                                     hyperdos_x86_fetch_instruction_byte(processor),
                                                     hyperdos_x86_read_byte_register(processor, 0u));
                break;

            case HYPERDOS_X86_OPERATION_CODE_OUTPUT_IMMEDIATE_WORD:
            {
                uint16_t port = hyperdos_x86_fetch_instruction_byte(processor);
                uint16_t accumulator =
                        hyperdos_x86_read_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
                if (!hyperdos_x86_io_privilege_allows_current_level(processor))
                {
                    result = hyperdos_x86_raise_general_protection_fault(processor, 0u, &instructionCompleted);
                    break;
                }
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
                result           = hyperdos_x86_execute_far_control_transfer(processor,
                                                                   segment,
                                                                   offset,
                                                                   0,
                                                                   &instructionCompleted);
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_JUMP_SHORT:
                hyperdos_x86_jump_relative(processor, (int8_t)hyperdos_x86_fetch_instruction_byte(processor));
                break;

            case HYPERDOS_X86_OPERATION_CODE_INPUT_FROM_DATA_BYTE:
                if (!hyperdos_x86_io_privilege_allows_current_level(processor))
                {
                    result = hyperdos_x86_raise_general_protection_fault(processor, 0u, &instructionCompleted);
                    break;
                }
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
                uint16_t lowByte  = 0u;
                uint16_t highByte = 0u;
                if (!hyperdos_x86_io_privilege_allows_current_level(processor))
                {
                    result = hyperdos_x86_raise_general_protection_fault(processor, 0u, &instructionCompleted);
                    break;
                }
                lowByte  = hyperdos_x86_read_input_output_byte(processor, port);
                highByte = hyperdos_x86_read_input_output_byte(processor, (uint16_t)(port + HYPERDOS_X86_BYTE_SIZE));
                hyperdos_x86_write_general_register_word(processor,
                                                         HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                         (uint16_t)(lowByte |
                                                                    (highByte << HYPERDOS_X86_BYTE_BIT_COUNT)));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_OUTPUT_TO_DATA_BYTE:
                if (!hyperdos_x86_io_privilege_allows_current_level(processor))
                {
                    result = hyperdos_x86_raise_general_protection_fault(processor, 0u, &instructionCompleted);
                    break;
                }
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
                if (!hyperdos_x86_io_privilege_allows_current_level(processor))
                {
                    result = hyperdos_x86_raise_general_protection_fault(processor, 0u, &instructionCompleted);
                    break;
                }
                hyperdos_x86_write_input_output_byte(processor, port, (uint8_t)(accumulator & HYPERDOS_X86_BYTE_MASK));
                hyperdos_x86_write_input_output_byte(processor,
                                                     (uint16_t)(port + HYPERDOS_X86_BYTE_SIZE),
                                                     (uint8_t)(accumulator >> HYPERDOS_X86_BYTE_BIT_COUNT));
                break;
            }

            case HYPERDOS_X86_OPERATION_CODE_HALT:
                if (hyperdos_x86_processor_is_protected_mode_active(processor) &&
                    hyperdos_x86_current_privilege_level(processor) != 0u)
                {
                    result = hyperdos_x86_raise_general_protection_fault(processor, 0u, &instructionCompleted);
                    break;
                }
                processor->halted            = 1u;
                suppressTrapAfterInstruction = 1;
                break;

            case HYPERDOS_X86_OPERATION_CODE_COMPLEMENT_CARRY:
                processor->flags ^= HYPERDOS_X86_FLAG_CARRY;
                hyperdos_x86_normalize_current_flags(processor);
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
                hyperdos_x86_normalize_current_flags(processor);
                break;

            case HYPERDOS_X86_OPERATION_CODE_SET_CARRY:
                processor->flags |= HYPERDOS_X86_FLAG_CARRY;
                hyperdos_x86_normalize_current_flags(processor);
                break;

            case HYPERDOS_X86_OPERATION_CODE_CLEAR_INTERRUPT:
                if (!hyperdos_x86_io_privilege_allows_current_level(processor))
                {
                    result = hyperdos_x86_raise_general_protection_fault(processor, 0u, &instructionCompleted);
                    break;
                }
                processor->flags &= (uint16_t)~HYPERDOS_X86_FLAG_INTERRUPT_ENABLE;
                hyperdos_x86_normalize_current_flags(processor);
                break;

            case HYPERDOS_X86_OPERATION_CODE_SET_INTERRUPT:
                if (!hyperdos_x86_io_privilege_allows_current_level(processor))
                {
                    result = hyperdos_x86_raise_general_protection_fault(processor, 0u, &instructionCompleted);
                    break;
                }
                processor->flags |= HYPERDOS_X86_FLAG_INTERRUPT_ENABLE;
                hyperdos_x86_normalize_current_flags(processor);
                startMaskableInterruptInhibit = 1;
                break;

            case HYPERDOS_X86_OPERATION_CODE_CLEAR_DIRECTION:
                processor->flags &= (uint16_t)~HYPERDOS_X86_FLAG_DIRECTION;
                hyperdos_x86_normalize_current_flags(processor);
                break;

            case HYPERDOS_X86_OPERATION_CODE_SET_DIRECTION:
                processor->flags |= HYPERDOS_X86_FLAG_DIRECTION;
                hyperdos_x86_normalize_current_flags(processor);
                break;

            case HYPERDOS_X86_OPERATION_CODE_INCREMENT_DECREMENT_BYTE:
            {
                uint8_t registerMemoryByte = hyperdos_x86_fetch_instruction_byte(processor);
                uint8_t operation          = (uint8_t)((registerMemoryByte >> HYPERDOS_X86_MODRM_REGISTER_SHIFT) &
                                              HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK);
                hyperdos_x86_decoded_operand operand;
                uint16_t                     value                   = 0u;
                int                          invalidExtensionHandled = 0;
                result = hyperdos_x86_validate_mod_register_memory_extension(
                        processor,
                        HYPERDOS_X86_OPERATION_CODE_INCREMENT_DECREMENT_BYTE,
                        registerMemoryByte,
                        &instructionCompleted,
                        &invalidExtensionHandled);
                if (result != HYPERDOS_X86_EXECUTION_OK || invalidExtensionHandled)
                {
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
                result = hyperdos_x86_execute_group_five_instruction(processor,
                                                                     hasSegmentOverride,
                                                                     segmentOverride,
                                                                     &instructionCompleted);
                break;

            default:
                result = HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
                break;
            }
        }

        {
            while (processor->pendingExceptionActive &&
                   (result == HYPERDOS_X86_EXECUTION_OK || result == HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION))
            {
                result = hyperdos_x86_dispatch_pending_exception(processor, &instructionCompleted);
            }
            if (processor->processorShutdownActive != 0u)
            {
                return HYPERDOS_X86_EXECUTION_PROCESSOR_SHUTDOWN;
            }

            if (result == HYPERDOS_X86_EXECUTION_DIVIDE_ERROR)
            {
                if (hyperdos_x86_processor_is_protected_mode_active(processor))
                {
                    hyperdos_x86_set_pending_exception(processor, HYPERDOS_X86_INTERRUPT_TYPE_DIVIDE_ERROR, 0, 0u);
                    result = hyperdos_x86_dispatch_pending_exception(processor, &instructionCompleted);
                    while (processor->pendingExceptionActive && result == HYPERDOS_X86_EXECUTION_OK)
                    {
                        result = hyperdos_x86_dispatch_pending_exception(processor, &instructionCompleted);
                    }
                    if (processor->processorShutdownActive != 0u)
                    {
                        return HYPERDOS_X86_EXECUTION_PROCESSOR_SHUTDOWN;
                    }
                }
                else
                {
                    hyperdos_x86_execution_result interruptResult =
                            processor->divideErrorReturnsToFaultingInstruction
                                    ? hyperdos_x86_execute_interrupt_with_return_address(
                                              processor,
                                              HYPERDOS_X86_INTERRUPT_TYPE_DIVIDE_ERROR,
                                              processor->lastInstructionSegment,
                                              (uint16_t)processor->lastInstructionOffset)
                                    : hyperdos_x86_execute_interrupt(processor,
                                                                     HYPERDOS_X86_INTERRUPT_TYPE_DIVIDE_ERROR);
                    if (interruptResult == HYPERDOS_X86_EXECUTION_INTERRUPT_NOT_HANDLED)
                    {
                        return result;
                    }
                    result = interruptResult;
                }
                instructionCompleted = 0;
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

uint64_t hyperdos_x86_get_external_bus_cycle_count(const hyperdos_x86_processor* processor)
{
    if (processor == NULL)
    {
        return 0u;
    }
    return processor->externalBusCycleCount;
}

void hyperdos_x86_reset_external_bus_cycle_count(hyperdos_x86_processor* processor)
{
    if (processor == NULL)
    {
        return;
    }
    processor->externalBusCycleCount = 0u;
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
    return hyperdos_x86_execute_external_interrupt(processor, interruptNumber);
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
    uint32_t physicalAddress = 0u;

    if (hyperdos_x86_translate_logical_to_physical_address(processor, segmentRegister, offset, &physicalAddress) !=
        HYPERDOS_X86_EXECUTION_OK)
    {
        return 0u;
    }
    return physicalAddress;
}

hyperdos_x86_execution_result hyperdos_x86_translate_logical_to_linear_address(
        const hyperdos_x86_processor*       processor,
        hyperdos_x86_segment_register_index segmentRegister,
        uint16_t                            offset,
        uint32_t*                           linearAddress)
{
    if (processor == NULL || segmentRegister > HYPERDOS_X86_SEGMENT_REGISTER_DATA)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }
    if (linearAddress == NULL)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }
    if (offset > processor->segmentStates[segmentRegister].limit)
    {
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }
    *linearAddress = processor->segmentStates[segmentRegister].base + offset;
    return HYPERDOS_X86_EXECUTION_OK;
}

hyperdos_x86_execution_result hyperdos_x86_translate_linear_to_physical_address(const hyperdos_x86_processor* processor,
                                                                                uint32_t  linearAddress,
                                                                                uint32_t* physicalAddress)
{
    if (processor == NULL || physicalAddress == NULL)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }
    *physicalAddress = linearAddress & hyperdos_x86_get_physical_address_mask(processor);
    return HYPERDOS_X86_EXECUTION_OK;
}

hyperdos_x86_execution_result hyperdos_x86_translate_logical_to_physical_address(
        const hyperdos_x86_processor*       processor,
        hyperdos_x86_segment_register_index segmentRegister,
        uint16_t                            offset,
        uint32_t*                           physicalAddress)
{
    uint32_t linearAddress = 0u;

    if (hyperdos_x86_translate_logical_to_linear_address(processor, segmentRegister, offset, &linearAddress) !=
        HYPERDOS_X86_EXECUTION_OK)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }
    return hyperdos_x86_translate_linear_to_physical_address(processor, linearAddress, physicalAddress);
}

hyperdos_x86_execution_result hyperdos_x86_read_memory_byte(const hyperdos_x86_processor*       processor,
                                                            hyperdos_x86_segment_register_index segmentRegister,
                                                            uint16_t                            offset,
                                                            uint8_t*                            value)
{
    uint32_t physicalAddress = 0u;

    if (processor == NULL || value == NULL || segmentRegister > HYPERDOS_X86_SEGMENT_REGISTER_DATA)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }
    if (hyperdos_x86_translate_logical_to_physical_address(processor, segmentRegister, offset, &physicalAddress) !=
        HYPERDOS_X86_EXECUTION_OK)
    {
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }
    *value = hyperdos_x86_read_physical_byte(processor, physicalAddress);
    return HYPERDOS_X86_EXECUTION_OK;
}

hyperdos_x86_execution_result hyperdos_x86_write_memory_byte(hyperdos_x86_processor*             processor,
                                                             hyperdos_x86_segment_register_index segmentRegister,
                                                             uint16_t                            offset,
                                                             uint8_t                             value)
{
    uint32_t physicalAddress = 0u;

    if (processor == NULL || segmentRegister > HYPERDOS_X86_SEGMENT_REGISTER_DATA)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }
    if (hyperdos_x86_translate_logical_to_physical_address(processor, segmentRegister, offset, &physicalAddress) !=
        HYPERDOS_X86_EXECUTION_OK)
    {
        return HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION;
    }
    hyperdos_x86_write_physical_byte(processor, physicalAddress, value);
    return HYPERDOS_X86_EXECUTION_OK;
}

uint8_t hyperdos_x86_read_input_output_byte(hyperdos_x86_processor* processor, uint16_t port)
{
    if (processor == NULL || processor->bus == NULL)
    {
        return HYPERDOS_X86_BYTE_MASK;
    }
    if (!hyperdos_x86_io_privilege_allows_current_level(processor))
    {
        hyperdos_x86_set_pending_exception(processor, HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT, 1, 0u);
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
    if (!hyperdos_x86_io_privilege_allows_current_level(processor))
    {
        hyperdos_x86_set_pending_exception(processor, HYPERDOS_X86_INTERRUPT_TYPE_GENERAL_PROTECTION_FAULT, 1, 0u);
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

uint8_t hyperdos_x86_get_general_register_byte(const hyperdos_x86_processor* processor, uint8_t byteRegisterIndex)
{
    if (processor == NULL || byteRegisterIndex > HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK)
    {
        return 0u;
    }
    return hyperdos_x86_read_byte_register(processor, byteRegisterIndex);
}

void hyperdos_x86_set_general_register_byte(hyperdos_x86_processor* processor, uint8_t byteRegisterIndex, uint8_t value)
{
    if (processor == NULL || byteRegisterIndex > HYPERDOS_X86_GENERAL_REGISTER_INDEX_MASK)
    {
        return;
    }
    hyperdos_x86_write_byte_register(processor, byteRegisterIndex, value);
}

uint16_t hyperdos_x86_get_segment_register(const hyperdos_x86_processor*       processor,
                                           hyperdos_x86_segment_register_index segmentRegister)
{
    if (processor == NULL || segmentRegister > HYPERDOS_X86_SEGMENT_REGISTER_DATA)
    {
        return 0;
    }
    return processor->segmentStates[segmentRegister].selector;
}

void hyperdos_x86_set_segment_register(hyperdos_x86_processor*             processor,
                                       hyperdos_x86_segment_register_index segmentRegister,
                                       uint16_t                            value)
{
    if (processor == NULL || segmentRegister > HYPERDOS_X86_SEGMENT_REGISTER_DATA)
    {
        return;
    }
    if (hyperdos_x86_processor_is_protected_mode_active(processor))
    {
        (void)hyperdos_x86_load_protected_mode_segment(processor, segmentRegister, value);
        return;
    }
    hyperdos_x86_load_real_mode_segment(processor, segmentRegister, value);
}

uint32_t hyperdos_x86_get_segment_base(const hyperdos_x86_processor*       processor,
                                       hyperdos_x86_segment_register_index segmentRegister)
{
    if (processor == NULL || segmentRegister > HYPERDOS_X86_SEGMENT_REGISTER_DATA)
    {
        return 0u;
    }
    return processor->segmentStates[segmentRegister].base;
}

uint32_t hyperdos_x86_get_segment_limit(const hyperdos_x86_processor*       processor,
                                        hyperdos_x86_segment_register_index segmentRegister)
{
    if (processor == NULL || segmentRegister > HYPERDOS_X86_SEGMENT_REGISTER_DATA)
    {
        return 0u;
    }
    return processor->segmentStates[segmentRegister].limit;
}

uint16_t hyperdos_x86_get_segment_attributes(const hyperdos_x86_processor*       processor,
                                             hyperdos_x86_segment_register_index segmentRegister)
{
    if (processor == NULL || segmentRegister > HYPERDOS_X86_SEGMENT_REGISTER_DATA)
    {
        return 0u;
    }
    return processor->segmentStates[segmentRegister].attributes;
}

uint16_t hyperdos_x86_get_instruction_pointer_word(const hyperdos_x86_processor* processor)
{
    if (processor == NULL)
    {
        return 0u;
    }
    return hyperdos_x86_read_instruction_pointer_word(processor);
}

void hyperdos_x86_set_instruction_pointer_word(hyperdos_x86_processor* processor, uint16_t value)
{
    if (processor == NULL)
    {
        return;
    }
    hyperdos_x86_write_instruction_pointer_word(processor, value);
}

uint32_t hyperdos_x86_get_instruction_pointer_dword(const hyperdos_x86_processor* processor)
{
    if (processor == NULL)
    {
        return 0u;
    }
    return processor->instructionPointer;
}

void hyperdos_x86_set_instruction_pointer_dword(hyperdos_x86_processor* processor, uint32_t value)
{
    if (processor == NULL)
    {
        return;
    }
    processor->instructionPointer = value;
}

uint16_t hyperdos_x86_get_flags_word(const hyperdos_x86_processor* processor)
{
    if (processor == NULL)
    {
        return 0u;
    }
    return hyperdos_x86_read_flags_word(processor);
}

void hyperdos_x86_set_flags_word(hyperdos_x86_processor* processor, uint16_t value)
{
    if (processor == NULL)
    {
        return;
    }
    hyperdos_x86_write_flags_word(processor, value);
}

int hyperdos_x86_get_flag(const hyperdos_x86_processor* processor, hyperdos_x86_flag_mask flagMask)
{
    if (processor == NULL)
    {
        return 0;
    }
    return (hyperdos_x86_read_flags_word(processor) & (uint16_t)flagMask) != 0u;
}

void hyperdos_x86_update_flags_word(hyperdos_x86_processor* processor, uint16_t flagMask, uint16_t flagValues)
{
    uint16_t flags = 0u;

    if (processor == NULL)
    {
        return;
    }
    flags = hyperdos_x86_read_flags_word(processor);
    hyperdos_x86_write_flags_word(processor, (uint16_t)((flags & (uint16_t)~flagMask) | (flagValues & flagMask)));
}

void hyperdos_x86_set_flag(hyperdos_x86_processor* processor, hyperdos_x86_flag_mask flagMask, int enabled)
{
    hyperdos_x86_update_flags_word(processor, (uint16_t)flagMask, enabled ? (uint16_t)flagMask : 0u);
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
    case HYPERDOS_X86_EXECUTION_PROCESSOR_SHUTDOWN:
        return "processor shutdown";
    }
    return "unknown";
}
