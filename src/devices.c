#include "hyperdos/devices.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

enum
{
    HYPERDOS_OPEN_BUS_BYTE                                               = 0xFFu,
    HYPERDOS_X86_TWO_BIT_COUNT                                           = 2u,
    HYPERDOS_X86_LOW_TWO_BITS_MASK                                       = 0x03u,
    HYPERDOS_X86_LOW_THREE_BITS_MASK                                     = 0x07u,
    HYPERDOS_INTERRUPT_CONTROLLER_COMMAND_PORT_OFFSET                    = 0u,
    HYPERDOS_INTERRUPT_CONTROLLER_DATA_PORT_OFFSET                       = 1u,
    HYPERDOS_INTERRUPT_CONTROLLER_INITIALIZATION_COMMAND                 = 0x10u,
    HYPERDOS_INTERRUPT_CONTROLLER_INITIALIZATION_NEEDS_WORD_FOUR         = 0x01u,
    HYPERDOS_INTERRUPT_CONTROLLER_OPERATION_COMMAND_THREE                = 0x08u,
    HYPERDOS_INTERRUPT_CONTROLLER_READ_IN_SERVICE_REGISTER               = 0x03u,
    HYPERDOS_INTERRUPT_CONTROLLER_END_OF_INTERRUPT                       = 0x20u,
    HYPERDOS_INTERRUPT_CONTROLLER_SPECIFIC_END_OF_INTERRUPT              = 0x40u,
    HYPERDOS_INTERRUPT_CONTROLLER_AUTO_END_OF_INTERRUPT                  = 0x02u,
    HYPERDOS_INTERRUPT_CONTROLLER_OPERATION_LEVEL_MASK                   = 0x07u,
    HYPERDOS_INTERRUPT_CONTROLLER_REQUEST_LINE_COUNT                     = 8u,
    HYPERDOS_INTERRUPT_CONTROLLER_VECTOR_BASE_MASK                       = 0xF8u,
    HYPERDOS_DIRECT_MEMORY_ACCESS_CHANNEL_COUNT                          = 4u,
    HYPERDOS_DIRECT_MEMORY_ACCESS_CHANNEL_MASK                           = 0x03u,
    HYPERDOS_DIRECT_MEMORY_ACCESS_SINGLE_MASK_SET_BIT                    = 0x04u,
    HYPERDOS_DIRECT_MEMORY_ACCESS_ADDRESS_COUNT_PORT_LAST                = 0x07u,
    HYPERDOS_DIRECT_MEMORY_ACCESS_COMMAND_PORT                           = 0x08u,
    HYPERDOS_DIRECT_MEMORY_ACCESS_REQUEST_PORT                           = 0x09u,
    HYPERDOS_DIRECT_MEMORY_ACCESS_SINGLE_MASK_PORT                       = 0x0Au,
    HYPERDOS_DIRECT_MEMORY_ACCESS_MODE_PORT                              = 0x0Bu,
    HYPERDOS_DIRECT_MEMORY_ACCESS_FLIP_FLOP_RESET_PORT                   = 0x0Cu,
    HYPERDOS_DIRECT_MEMORY_ACCESS_MASTER_RESET_PORT                      = 0x0Du,
    HYPERDOS_DIRECT_MEMORY_ACCESS_MASK_RESET_PORT                        = 0x0Eu,
    HYPERDOS_DIRECT_MEMORY_ACCESS_ALL_MASK_PORT                          = 0x0Fu,
    HYPERDOS_INTERVAL_TIMER_CHANNEL_COUNT                                = 3u,
    HYPERDOS_INTERVAL_TIMER_CONTROL_PORT_OFFSET                          = 3u,
    HYPERDOS_INTERVAL_TIMER_SELECT_CHANNEL_SHIFT                         = 6u,
    HYPERDOS_INTERVAL_TIMER_ACCESS_MODE_SHIFT                            = 4u,
    HYPERDOS_INTERVAL_TIMER_OPERATING_MODE_SHIFT                         = 1u,
    HYPERDOS_INTERVAL_TIMER_ACCESS_MODE_LATCH_COUNT                      = 0u,
    HYPERDOS_INTERVAL_TIMER_ACCESS_MODE_LOW_BYTE                         = 1u,
    HYPERDOS_INTERVAL_TIMER_ACCESS_MODE_HIGH_BYTE                        = 2u,
    HYPERDOS_INTERVAL_TIMER_ACCESS_MODE_LOW_HIGH_BYTE                    = 3u,
    HYPERDOS_INTERVAL_TIMER_MODE_INTERRUPT_ON_TERMINAL_COUNT             = 0u,
    HYPERDOS_INTERVAL_TIMER_MODE_RATE_GENERATOR                          = 2u,
    HYPERDOS_INTERVAL_TIMER_MODE_SQUARE_WAVE                             = 3u,
    HYPERDOS_INTERVAL_TIMER_MODE_SOFTWARE_TRIGGERED_STROBE               = 4u,
    HYPERDOS_INTERVAL_TIMER_MODE_HARDWARE_TRIGGERED_STROBE               = 5u,
    HYPERDOS_INTERVAL_TIMER_MAXIMUM_COUNT                                = 0x10000u,
    HYPERDOS_X86_PERIPHERAL_INTERFACE_CONTROL_PORT_OFFSET                = 3u,
    HYPERDOS_X86_PERIPHERAL_INTERFACE_DEFAULT_CONTROL                    = 0x9Bu,
    HYPERDOS_KEYBOARD_CONTROLLER_DATA_PORT                               = 0x0060u,
    HYPERDOS_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT                     = 0x0064u,
    HYPERDOS_KEYBOARD_CONTROLLER_STATUS_OUTPUT_BUFFER_FULL               = 0x01u,
    HYPERDOS_KEYBOARD_CONTROLLER_STATUS_INPUT_BUFFER_FULL                = 0x02u,
    HYPERDOS_KEYBOARD_CONTROLLER_STATUS_SYSTEM_FLAG                      = 0x04u,
    HYPERDOS_KEYBOARD_CONTROLLER_STATUS_COMMAND_DATA                     = 0x08u,
    HYPERDOS_KEYBOARD_CONTROLLER_STATUS_UNLOCKED                         = 0x10u,
    HYPERDOS_KEYBOARD_CONTROLLER_STATUS_AUXILIARY_OUTPUT_BUFFER          = 0x20u,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_INTERRUPT_ENABLE           = 0x01u,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_AUXILIARY_INTERRUPT_ENABLE = 0x02u,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_SYSTEM_FLAG                = 0x04u,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_KEYBOARD_DISABLED          = 0x10u,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_AUXILIARY_DISABLED         = 0x20u,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_TRANSLATION                = 0x40u,
    HYPERDOS_KEYBOARD_CONTROLLER_DEFAULT_COMMAND_BYTE = HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_INTERRUPT_ENABLE |
                                                        HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_SYSTEM_FLAG |
                                                        HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_TRANSLATION,
    HYPERDOS_KEYBOARD_CONTROLLER_DEFAULT_OUTPUT_PORT                 = 0x01u,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_READ_COMMAND_BYTE           = 0x20u,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_WRITE_COMMAND_BYTE          = 0x60u,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_SELF_TEST                   = 0xAAu,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_TEST_KEYBOARD_INTERFACE     = 0xABu,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_DISABLE_KEYBOARD            = 0xADu,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_ENABLE_KEYBOARD             = 0xAEu,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_DISABLE_AUXILIARY           = 0xA7u,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_ENABLE_AUXILIARY            = 0xA8u,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_TEST_AUXILIARY              = 0xA9u,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_READ_INPUT_PORT             = 0xC0u,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_READ_OUTPUT_PORT            = 0xD0u,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_WRITE_OUTPUT_PORT           = 0xD1u,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_WRITE_KEYBOARD_OUTPUT       = 0xD2u,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_WRITE_AUXILIARY_OUTPUT      = 0xD3u,
    HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_WRITE_AUXILIARY_DEVICE      = 0xD4u,
    HYPERDOS_KEYBOARD_DEVICE_COMMAND_SET_LEDS                        = 0xEDu,
    HYPERDOS_KEYBOARD_DEVICE_COMMAND_ECHO                            = 0xEEu,
    HYPERDOS_KEYBOARD_DEVICE_COMMAND_SET_SCAN_CODE_SET               = 0xF0u,
    HYPERDOS_KEYBOARD_DEVICE_COMMAND_IDENTIFY                        = 0xF2u,
    HYPERDOS_KEYBOARD_DEVICE_COMMAND_SET_TYPEMATIC                   = 0xF3u,
    HYPERDOS_KEYBOARD_DEVICE_COMMAND_ENABLE_SCANNING                 = 0xF4u,
    HYPERDOS_KEYBOARD_DEVICE_COMMAND_DISABLE_SCANNING                = 0xF5u,
    HYPERDOS_KEYBOARD_DEVICE_COMMAND_SET_DEFAULTS                    = 0xF6u,
    HYPERDOS_KEYBOARD_DEVICE_COMMAND_RESEND                          = 0xFEu,
    HYPERDOS_KEYBOARD_DEVICE_COMMAND_RESET                           = 0xFFu,
    HYPERDOS_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE                    = 0xFAu,
    HYPERDOS_KEYBOARD_DEVICE_RESPONSE_RESEND                         = 0xFEu,
    HYPERDOS_KEYBOARD_DEVICE_RESPONSE_SELF_TEST_PASSED               = 0xAAu,
    HYPERDOS_KEYBOARD_DEVICE_RESPONSE_CONTROLLER_TEST_PASSED         = 0x55u,
    HYPERDOS_KEYBOARD_DEVICE_RESPONSE_INTERFACE_TEST_PASSED          = 0x00u,
    HYPERDOS_KEYBOARD_DEVICE_IDENTIFY_FIRST_BYTE                     = 0xABu,
    HYPERDOS_KEYBOARD_DEVICE_IDENTIFY_SECOND_BYTE                    = 0x83u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_SCALING_ONE_TO_ONE   = 0xE6u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_SCALING_TWO_TO_ONE   = 0xE7u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_RESOLUTION           = 0xE8u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_STATUS_REQUEST           = 0xE9u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_STREAM_MODE          = 0xEAu,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_READ_DATA                = 0xEBu,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_RESET_WRAP_MODE          = 0xECu,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_WRAP_MODE            = 0xEEu,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_REMOTE_MODE          = 0xF0u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_IDENTIFY                 = 0xF2u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_SAMPLE_RATE          = 0xF3u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_ENABLE_REPORTING         = 0xF4u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_DISABLE_REPORTING        = 0xF5u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_DEFAULTS             = 0xF6u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_RESEND                   = 0xFEu,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_RESET                    = 0xFFu,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_IDENTIFIER_STANDARD              = 0x00u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_STATUS_RIGHT_BUTTON              = 0x01u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_STATUS_MIDDLE_BUTTON             = 0x02u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_STATUS_LEFT_BUTTON               = 0x04u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_STATUS_SCALING_TWO_TO_ONE        = 0x10u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_STATUS_REPORTING_ENABLED         = 0x20u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_DEFAULT_SAMPLE_RATE              = 100u,
    HYPERDOS_AUXILIARY_MOUSE_DEVICE_DEFAULT_RESOLUTION               = 2u,
    HYPERDOS_AUXILIARY_MOUSE_PACKET_LEFT_BUTTON                      = 0x01u,
    HYPERDOS_AUXILIARY_MOUSE_PACKET_RIGHT_BUTTON                     = 0x02u,
    HYPERDOS_AUXILIARY_MOUSE_PACKET_MIDDLE_BUTTON                    = 0x04u,
    HYPERDOS_AUXILIARY_MOUSE_PACKET_ALWAYS_ONE                       = 0x08u,
    HYPERDOS_AUXILIARY_MOUSE_PACKET_HORIZONTAL_SIGN                  = 0x10u,
    HYPERDOS_AUXILIARY_MOUSE_PACKET_VERTICAL_SIGN                    = 0x20u,
    HYPERDOS_AUXILIARY_MOUSE_PACKET_HORIZONTAL_OVERFLOW              = 0x40u,
    HYPERDOS_AUXILIARY_MOUSE_PACKET_VERTICAL_OVERFLOW                = 0x80u,
    HYPERDOS_UART_RECEIVER_BUFFER_OFFSET                             = 0u,
    HYPERDOS_UART_INTERRUPT_ENABLE_OFFSET                            = 1u,
    HYPERDOS_UART_INTERRUPT_IDENTIFICATION_OFFSET                    = 2u,
    HYPERDOS_UART_LINE_CONTROL_OFFSET                                = 3u,
    HYPERDOS_UART_MODEM_CONTROL_OFFSET                               = 4u,
    HYPERDOS_UART_LINE_STATUS_OFFSET                                 = 5u,
    HYPERDOS_UART_MODEM_STATUS_OFFSET                                = 6u,
    HYPERDOS_UART_PORT_OFFSET_MASK                                   = 0x07u,
    HYPERDOS_UART_DIVISOR_LATCH_ACCESS_BIT                           = 0x80u,
    HYPERDOS_UART_LINE_STATUS_TRANSMITTER_EMPTY                      = 0x60u,
    HYPERDOS_COLOR_GRAPHICS_ADAPTER_INDEX_PORT                       = 0x03D4u,
    HYPERDOS_COLOR_GRAPHICS_ADAPTER_DATA_PORT                        = 0x03D5u,
    HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_CONTROL_PORT                = 0x03D8u,
    HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLOR_SELECT_PORT                = 0x03D9u,
    HYPERDOS_COLOR_GRAPHICS_ADAPTER_STATUS_PORT                      = 0x03DAu,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_MONOCHROME_INDEX_PORT              = 0x03B4u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_MONOCHROME_DATA_PORT               = 0x03B5u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_MONOCHROME_STATUS_PORT             = 0x03BAu,
    HYPERDOS_COLOR_GRAPHICS_ADAPTER_REGISTER_MASK                    = 0x1Fu,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_ADDRESS_PORT             = 0x03C0u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_DATA_PORT                = 0x03C1u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_MISCELLANEOUS_OUTPUT_WRITE_PORT    = 0x03C2u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_INDEX_PORT               = 0x03C4u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_DATA_PORT                = 0x03C5u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_MASK_PORT        = 0x03C6u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_READ_INDEX_PORT  = 0x03C7u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_WRITE_INDEX_PORT = 0x03C8u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_DATA_PORT        = 0x03C9u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_FEATURE_CONTROL_READ_PORT          = 0x03CAu,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_MISCELLANEOUS_OUTPUT_READ_PORT     = 0x03CCu,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_PORT     = 0x03CEu,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_DATA_PORT      = 0x03CFu,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_INDEX_MASK               = 0x1Fu,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_PALETTE_SOURCE_BIT       = 0x20u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_MODE_CONTROL_INDEX       = 0x10u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_COLOR_PLANE_ENABLE_INDEX = 0x12u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_COLOR_SELECT_INDEX       = 0x14u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_GRAPHICS_ENABLE_BIT      = 0x01u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_INDEX_MASK               = 0x07u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_MASK     = 0x0Fu,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MAP_MASK_INDEX           = 0x02u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MEMORY_MODE_INDEX        = 0x04u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_SET_RESET_INDEX           = 0x00u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_ENABLE_SET_RESET_INDEX    = 0x01u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_COLOR_COMPARE_INDEX       = 0x02u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_DATA_ROTATE_INDEX         = 0x03u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_READ_MAP_SELECT_INDEX     = 0x04u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MODE_INDEX                = 0x05u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MISCELLANEOUS_INDEX       = 0x06u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_COLOR_DONT_CARE_INDEX     = 0x07u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_BIT_MASK_INDEX            = 0x08u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_WRITE_MODE_MASK                    = 0x03u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_READ_MODE_BIT                      = 0x08u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_ODD_EVEN_BIT                       = 0x10u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_SHIFT_256_COLOR_BIT                = 0x40u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MODE_BIT                  = 0x01u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_CHAIN_ODD_EVEN_BIT                 = 0x02u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_MAP_SELECT_SHIFT            = 2u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_MAP_SELECT_MASK             = 0x03u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_MAP_A0000_128K              = 0u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_MAP_A0000_64K               = 1u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_MAP_B0000_32K               = 2u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_MAP_B8000_32K               = 3u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_CHAIN_FOUR_BIT           = 0x08u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_ODD_EVEN_DISABLE_BIT     = 0x04u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_TEXT_MEMORY_MAP_VALUE              = 0x0Eu,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_CGA_GRAPHICS_MEMORY_MAP_VALUE      = 0x0Du,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MEMORY_MAP_VALUE          = 0x05u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_CHAIN_FOUR_MEMORY_MODE             = 0x0Eu,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANAR_MEMORY_MODE                 = 0x06u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_TEXT_MEMORY_MODE                   = 0x02u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_DATA_ROTATE_COUNT_MASK             = 0x07u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_LOGICAL_OPERATION_SHIFT            = 3u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_LOGICAL_OPERATION_MASK             = 0x03u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_LOGICAL_OPERATION_REPLACE          = 0u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_LOGICAL_OPERATION_AND              = 1u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_LOGICAL_OPERATION_OR               = 2u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_LOGICAL_OPERATION_XOR              = 3u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_DEFAULT_MISCELLANEOUS_OUTPUT       = 0x63u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_DEFAULT_MAP_MASK                   = 0x0Fu,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_DEFAULT_MEMORY_MODE                = 0x06u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_DEFAULT_GRAPHICS_MISCELLANEOUS     = 0x05u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_DEFAULT_BIT_MASK                   = 0xFFu,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_DEFAULT_DIGITAL_TO_ANALOG_MASK     = 0xFFu,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_CRT_START_ADDRESS_HIGH_INDEX       = 0x0Cu,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_CRT_START_ADDRESS_LOW_INDEX        = 0x0Du,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_CRT_OFFSET_INDEX                   = 0x13u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_DAC_STATE_READ_MODE                = 0x03u,
    HYPERDOS_VIDEO_GRAPHICS_ARRAY_DAC_STATE_WRITE_MODE               = 0x00u,
    HYPERDOS_8087_ESCAPE_D8                                          = 0xD8u,
    HYPERDOS_8087_ESCAPE_D9                                          = 0xD9u,
    HYPERDOS_8087_ESCAPE_DA                                          = 0xDAu,
    HYPERDOS_8087_ESCAPE_DB                                          = 0xDBu,
    HYPERDOS_8087_ESCAPE_DC                                          = 0xDCu,
    HYPERDOS_8087_ESCAPE_DD                                          = 0xDDu,
    HYPERDOS_8087_ESCAPE_DE                                          = 0xDEu,
    HYPERDOS_8087_ESCAPE_DF                                          = 0xDFu,
    HYPERDOS_8087_REGISTER_MEMORY_INITIALIZE                         = 0xE3u,
    HYPERDOS_8087_REGISTER_MEMORY_CLEAR_EXCEPTIONS                   = 0xE2u,
    HYPERDOS_8087_REGISTER_MEMORY_SET_PROTECTED_MODE                 = 0xE4u,
    HYPERDOS_8087_REGISTER_MEMORY_STORE_STATUS_ACCUMULATOR           = 0xE0u,
    HYPERDOS_8087_REGISTER_MEMORY_MASK                               = 0x07u,
    HYPERDOS_8087_REGISTER_MEMORY_GROUP_MASK                         = 0xF8u,
    HYPERDOS_8087_REGISTER_MEMORY_GROUP_LOAD_REGISTER                = 0xC0u,
    HYPERDOS_8087_REGISTER_MEMORY_GROUP_EXCHANGE_REGISTER            = 0xC8u,
    HYPERDOS_8087_REGISTER_MEMORY_GROUP_FREE_REGISTER                = 0xC0u,
    HYPERDOS_8087_REGISTER_MEMORY_GROUP_STORE_REGISTER               = 0xD0u,
    HYPERDOS_8087_REGISTER_MEMORY_GROUP_STORE_POP_REGISTER           = 0xD8u,
    HYPERDOS_8087_STATUS_INVALID_OPERATION                           = 0x0001u,
    HYPERDOS_8087_STATUS_ZERO_DIVIDE                                 = 0x0004u,
    HYPERDOS_8087_STATUS_STACK_FAULT                                 = 0x0040u,
    HYPERDOS_8087_STATUS_ERROR_SUMMARY                               = 0x0080u,
    HYPERDOS_8087_STATUS_CONDITION_ZERO                              = 0x4700u,
    HYPERDOS_8087_STATUS_CONDITION_ZERO_OR_UNORDERED                 = 0x4000u,
    HYPERDOS_8087_STATUS_CONDITION_LESS_THAN                         = 0x0100u,
    HYPERDOS_8087_STATUS_TOP_MASK                                    = 0x3800u,
    HYPERDOS_8087_STATUS_TOP_SHIFT                                   = 11u,
    HYPERDOS_8087_CONTROL_ROUNDING_SHIFT                             = 10u,
    HYPERDOS_8087_CONTROL_ROUNDING_MASK                              = 0x03u,
    HYPERDOS_8087_ROUND_NEAREST                                      = 0u,
    HYPERDOS_8087_ROUND_DOWN                                         = 1u,
    HYPERDOS_8087_ROUND_UP                                           = 2u,
    HYPERDOS_8087_ROUND_TRUNCATE                                     = 3u,
    HYPERDOS_8087_STACK_TAG_VALID                                    = 0u,
    HYPERDOS_8087_STACK_TAG_ZERO                                     = 1u,
    HYPERDOS_8087_STACK_TAG_SPECIAL                                  = 2u,
    HYPERDOS_8087_STACK_TAG_EMPTY                                    = 3u,
    HYPERDOS_8087_ENVIRONMENT_CONTROL_OFFSET                         = 0u,
    HYPERDOS_8087_ENVIRONMENT_STATUS_OFFSET                          = 2u,
    HYPERDOS_8087_ENVIRONMENT_TAG_OFFSET                             = 4u,
    HYPERDOS_8087_ENVIRONMENT_INSTRUCTION_OFFSET                     = 6u,
    HYPERDOS_8087_ENVIRONMENT_INSTRUCTION_SEGMENT                    = 8u,
    HYPERDOS_8087_ENVIRONMENT_OPERAND_OFFSET                         = 10u,
    HYPERDOS_8087_ENVIRONMENT_OPERAND_SEGMENT                        = 12u
};

static void hyperdos_8087_update_tag_word(hyperdos_8087* coprocessor)
{
    uint16_t tagWord       = 0;
    size_t   registerIndex = 0;

    for (registerIndex = 0; registerIndex < HYPERDOS_8087_REGISTER_COUNT; ++registerIndex)
    {
        tagWord |= (uint16_t)((coprocessor->registerTags[registerIndex] & HYPERDOS_8087_REGISTER_MEMORY_MASK)
                              << (registerIndex * 2u));
    }
    coprocessor->tagWord = tagWord;
}

static void hyperdos_8087_update_status_top(hyperdos_8087* coprocessor)
{
    coprocessor->statusWord = (uint16_t)((coprocessor->statusWord & ~HYPERDOS_8087_STATUS_TOP_MASK) |
                                         ((uint16_t)(coprocessor->stackTop & HYPERDOS_8087_REGISTER_MEMORY_MASK)
                                          << HYPERDOS_8087_STATUS_TOP_SHIFT));
}

static void hyperdos_8087_update_environment_words(hyperdos_8087* coprocessor)
{
    hyperdos_8087_update_tag_word(coprocessor);
    hyperdos_8087_update_status_top(coprocessor);
}

static void hyperdos_8087_initialize_programming_state(hyperdos_8087* coprocessor)
{
    size_t registerIndex = 0;

    memset(coprocessor->registers, 0, sizeof(coprocessor->registers));
    for (registerIndex = 0; registerIndex < HYPERDOS_8087_REGISTER_COUNT; ++registerIndex)
    {
        coprocessor->registerTags[registerIndex] = HYPERDOS_8087_STACK_TAG_EMPTY;
    }
    coprocessor->controlWord            = HYPERDOS_8087_CONTROL_WORD_DEFAULT;
    coprocessor->statusWord             = HYPERDOS_8087_STATUS_WORD_DEFAULT;
    coprocessor->tagWord                = HYPERDOS_8087_TAG_WORD_DEFAULT;
    coprocessor->lastInstructionOffset  = 0u;
    coprocessor->lastInstructionSegment = 0u;
    coprocessor->lastOperandOffset      = 0u;
    coprocessor->lastOperandSegment     = 0u;
    coprocessor->stackTop               = 0u;
    hyperdos_8087_update_environment_words(coprocessor);
}

static size_t hyperdos_8087_physical_stack_register(const hyperdos_8087* coprocessor, uint8_t stackIndex)
{
    return (size_t)((coprocessor->stackTop + stackIndex) & HYPERDOS_8087_REGISTER_MEMORY_MASK);
}

static uint8_t hyperdos_8087_tag_for_value(long double value)
{
    return value == 0.0L ? HYPERDOS_8087_STACK_TAG_ZERO : HYPERDOS_8087_STACK_TAG_VALID;
}

static void hyperdos_8087_mark_error(hyperdos_8087* coprocessor, uint16_t errorMask)
{
    coprocessor->statusWord |= (uint16_t)(errorMask | HYPERDOS_8087_STATUS_ERROR_SUMMARY);
}

static long double hyperdos_8087_read_stack_value(const hyperdos_8087* coprocessor, uint8_t stackIndex)
{
    return coprocessor->registers[hyperdos_8087_physical_stack_register(coprocessor, stackIndex)];
}

static void hyperdos_8087_write_stack_value(hyperdos_8087* coprocessor, uint8_t stackIndex, long double value)
{
    size_t physicalRegister                     = hyperdos_8087_physical_stack_register(coprocessor, stackIndex);
    coprocessor->registers[physicalRegister]    = value;
    coprocessor->registerTags[physicalRegister] = hyperdos_8087_tag_for_value(value);
    hyperdos_8087_update_environment_words(coprocessor);
}

static void hyperdos_8087_push_stack_value(hyperdos_8087* coprocessor, long double value)
{
    uint8_t newStackTop = (uint8_t)((coprocessor->stackTop - 1u) & HYPERDOS_8087_REGISTER_MEMORY_MASK);

    if (coprocessor->registerTags[newStackTop] != HYPERDOS_8087_STACK_TAG_EMPTY)
    {
        hyperdos_8087_mark_error(coprocessor,
                                 HYPERDOS_8087_STATUS_INVALID_OPERATION | HYPERDOS_8087_STATUS_STACK_FAULT);
        return;
    }

    coprocessor->stackTop                  = newStackTop;
    coprocessor->registers[newStackTop]    = value;
    coprocessor->registerTags[newStackTop] = hyperdos_8087_tag_for_value(value);
    hyperdos_8087_update_environment_words(coprocessor);
}

static void hyperdos_8087_pop_stack_value(hyperdos_8087* coprocessor)
{
    size_t physicalRegister = hyperdos_8087_physical_stack_register(coprocessor, 0u);

    coprocessor->registerTags[physicalRegister] = HYPERDOS_8087_STACK_TAG_EMPTY;
    coprocessor->registers[physicalRegister]    = 0.0L;
    coprocessor->stackTop = (uint8_t)((coprocessor->stackTop + 1u) & HYPERDOS_8087_REGISTER_MEMORY_MASK);
    hyperdos_8087_update_environment_words(coprocessor);
}

static uint16_t hyperdos_8087_read_memory_word(const hyperdos_x86_processor*       processor,
                                               hyperdos_x86_segment_register_index segmentRegister,
                                               uint16_t                            offset)
{
    uint8_t lowByte  = 0;
    uint8_t highByte = 0;

    (void)hyperdos_x86_read_memory_byte(processor, segmentRegister, offset, &lowByte);
    (void)hyperdos_x86_read_memory_byte(processor, segmentRegister, (uint16_t)(offset + 1u), &highByte);
    return (uint16_t)(lowByte | ((uint16_t)highByte << HYPERDOS_X86_BYTE_BIT_COUNT));
}

static void hyperdos_8087_write_memory_word(hyperdos_x86_processor*             processor,
                                            hyperdos_x86_segment_register_index segmentRegister,
                                            uint16_t                            offset,
                                            uint16_t                            value)
{
    (void)hyperdos_x86_write_memory_byte(processor, segmentRegister, offset, (uint8_t)(value & HYPERDOS_X86_BYTE_MASK));
    (void)hyperdos_x86_write_memory_byte(processor,
                                         segmentRegister,
                                         (uint16_t)(offset + 1u),
                                         (uint8_t)(value >> HYPERDOS_X86_BYTE_BIT_COUNT));
}

static uint32_t hyperdos_8087_read_memory_double_word(const hyperdos_x86_processor*       processor,
                                                      hyperdos_x86_segment_register_index segmentRegister,
                                                      uint16_t                            offset)
{
    uint32_t lowWord  = hyperdos_8087_read_memory_word(processor, segmentRegister, offset);
    uint32_t highWord = hyperdos_8087_read_memory_word(processor, segmentRegister, (uint16_t)(offset + 2u));
    return lowWord | (highWord << HYPERDOS_X86_WORD_BIT_COUNT);
}

static void hyperdos_8087_write_memory_double_word(hyperdos_x86_processor*             processor,
                                                   hyperdos_x86_segment_register_index segmentRegister,
                                                   uint16_t                            offset,
                                                   uint32_t                            value)
{
    hyperdos_8087_write_memory_word(processor, segmentRegister, offset, (uint16_t)value);
    hyperdos_8087_write_memory_word(processor,
                                    segmentRegister,
                                    (uint16_t)(offset + 2u),
                                    (uint16_t)(value >> HYPERDOS_X86_WORD_BIT_COUNT));
}

static uint64_t hyperdos_8087_read_memory_quad_word(const hyperdos_x86_processor*       processor,
                                                    hyperdos_x86_segment_register_index segmentRegister,
                                                    uint16_t                            offset)
{
    uint64_t lowDoubleWord  = hyperdos_8087_read_memory_double_word(processor, segmentRegister, offset);
    uint64_t highDoubleWord = hyperdos_8087_read_memory_double_word(processor,
                                                                    segmentRegister,
                                                                    (uint16_t)(offset + 4u));
    return lowDoubleWord | (highDoubleWord << 32u);
}

static void hyperdos_8087_write_memory_quad_word(hyperdos_x86_processor*             processor,
                                                 hyperdos_x86_segment_register_index segmentRegister,
                                                 uint16_t                            offset,
                                                 uint64_t                            value)
{
    hyperdos_8087_write_memory_double_word(processor, segmentRegister, offset, (uint32_t)value);
    hyperdos_8087_write_memory_double_word(processor,
                                           segmentRegister,
                                           (uint16_t)(offset + 4u),
                                           (uint32_t)(value >> 32u));
}

static long double hyperdos_8087_read_single_precision_real(const hyperdos_x86_processor*       processor,
                                                            hyperdos_x86_segment_register_index segmentRegister,
                                                            uint16_t                            offset)
{
    uint32_t binaryValue = hyperdos_8087_read_memory_double_word(processor, segmentRegister, offset);
    float    realValue   = 0.0f;

    memcpy(&realValue, &binaryValue, sizeof(realValue));
    return realValue;
}

static void hyperdos_8087_write_single_precision_real(hyperdos_x86_processor*             processor,
                                                      hyperdos_x86_segment_register_index segmentRegister,
                                                      uint16_t                            offset,
                                                      long double                         value)
{
    float    realValue   = (float)value;
    uint32_t binaryValue = 0;

    memcpy(&binaryValue, &realValue, sizeof(binaryValue));
    hyperdos_8087_write_memory_double_word(processor, segmentRegister, offset, binaryValue);
}

static long double hyperdos_8087_read_double_precision_real(const hyperdos_x86_processor*       processor,
                                                            hyperdos_x86_segment_register_index segmentRegister,
                                                            uint16_t                            offset)
{
    uint64_t binaryValue = hyperdos_8087_read_memory_quad_word(processor, segmentRegister, offset);
    double   realValue   = 0.0;

    memcpy(&realValue, &binaryValue, sizeof(realValue));
    return realValue;
}

static void hyperdos_8087_write_double_precision_real(hyperdos_x86_processor*             processor,
                                                      hyperdos_x86_segment_register_index segmentRegister,
                                                      uint16_t                            offset,
                                                      long double                         value)
{
    double   realValue   = (double)value;
    uint64_t binaryValue = 0;

    memcpy(&binaryValue, &realValue, sizeof(binaryValue));
    hyperdos_8087_write_memory_quad_word(processor, segmentRegister, offset, binaryValue);
}

static int64_t hyperdos_8087_round_to_integer(const hyperdos_8087* coprocessor, long double value)
{
    uint16_t roundingControl = (uint16_t)((coprocessor->controlWord >> HYPERDOS_8087_CONTROL_ROUNDING_SHIFT) &
                                          HYPERDOS_8087_CONTROL_ROUNDING_MASK);
    int64_t  integerValue    = (int64_t)value;

    if (roundingControl == HYPERDOS_8087_ROUND_NEAREST)
    {
        return value >= 0.0L ? (int64_t)(value + 0.5L) : (int64_t)(value - 0.5L);
    }
    if (roundingControl == HYPERDOS_8087_ROUND_DOWN && value < (long double)integerValue)
    {
        return integerValue - 1;
    }
    if (roundingControl == HYPERDOS_8087_ROUND_UP && value > (long double)integerValue)
    {
        return integerValue + 1;
    }
    return integerValue;
}

static void hyperdos_8087_compare_values(hyperdos_8087* coprocessor, long double leftValue, long double rightValue)
{
    coprocessor->statusWord &= (uint16_t)~HYPERDOS_8087_STATUS_CONDITION_ZERO;
    if (leftValue < rightValue)
    {
        coprocessor->statusWord |= HYPERDOS_8087_STATUS_CONDITION_LESS_THAN;
    }
    else if (leftValue == rightValue)
    {
        coprocessor->statusWord |= HYPERDOS_8087_STATUS_CONDITION_ZERO_OR_UNORDERED;
    }
    hyperdos_8087_update_environment_words(coprocessor);
}

static void hyperdos_8087_execute_stack_operation(hyperdos_8087* coprocessor,
                                                  uint8_t        operationIndex,
                                                  long double    rightValue)
{
    long double leftValue = hyperdos_8087_read_stack_value(coprocessor, 0u);

    switch (operationIndex)
    {
    case 0u:
        hyperdos_8087_write_stack_value(coprocessor, 0u, leftValue + rightValue);
        break;
    case 1u:
        hyperdos_8087_write_stack_value(coprocessor, 0u, leftValue * rightValue);
        break;
    case 2u:
        hyperdos_8087_compare_values(coprocessor, leftValue, rightValue);
        break;
    case 3u:
        hyperdos_8087_compare_values(coprocessor, leftValue, rightValue);
        hyperdos_8087_pop_stack_value(coprocessor);
        break;
    case 4u:
        hyperdos_8087_write_stack_value(coprocessor, 0u, leftValue - rightValue);
        break;
    case 5u:
        hyperdos_8087_write_stack_value(coprocessor, 0u, rightValue - leftValue);
        break;
    case 6u:
        if (rightValue == 0.0L)
        {
            hyperdos_8087_mark_error(coprocessor, HYPERDOS_8087_STATUS_ZERO_DIVIDE);
            break;
        }
        hyperdos_8087_write_stack_value(coprocessor, 0u, leftValue / rightValue);
        break;
    case 7u:
        if (leftValue == 0.0L)
        {
            hyperdos_8087_mark_error(coprocessor, HYPERDOS_8087_STATUS_ZERO_DIVIDE);
            break;
        }
        hyperdos_8087_write_stack_value(coprocessor, 0u, rightValue / leftValue);
        break;
    }
}

static void hyperdos_8087_execute_register_operation(hyperdos_8087* coprocessor,
                                                     uint8_t        operationIndex,
                                                     uint8_t        stackIndex)
{
    hyperdos_8087_execute_stack_operation(coprocessor,
                                          operationIndex,
                                          hyperdos_8087_read_stack_value(coprocessor, stackIndex));
}

static void hyperdos_8087_execute_reversed_register_operation(hyperdos_8087* coprocessor,
                                                              uint8_t        operationIndex,
                                                              uint8_t        stackIndex,
                                                              int            popAfterOperation)
{
    long double leftValue  = hyperdos_8087_read_stack_value(coprocessor, stackIndex);
    long double rightValue = hyperdos_8087_read_stack_value(coprocessor, 0u);

    switch (operationIndex)
    {
    case 0u:
        hyperdos_8087_write_stack_value(coprocessor, stackIndex, leftValue + rightValue);
        break;
    case 1u:
        hyperdos_8087_write_stack_value(coprocessor, stackIndex, leftValue * rightValue);
        break;
    case 4u:
        hyperdos_8087_write_stack_value(coprocessor, stackIndex, rightValue - leftValue);
        break;
    case 5u:
        hyperdos_8087_write_stack_value(coprocessor, stackIndex, leftValue - rightValue);
        break;
    case 6u:
        if (leftValue == 0.0L)
        {
            hyperdos_8087_mark_error(coprocessor, HYPERDOS_8087_STATUS_ZERO_DIVIDE);
            break;
        }
        hyperdos_8087_write_stack_value(coprocessor, stackIndex, rightValue / leftValue);
        break;
    case 7u:
        if (rightValue == 0.0L)
        {
            hyperdos_8087_mark_error(coprocessor, HYPERDOS_8087_STATUS_ZERO_DIVIDE);
            break;
        }
        hyperdos_8087_write_stack_value(coprocessor, stackIndex, leftValue / rightValue);
        break;
    }

    if (popAfterOperation)
    {
        hyperdos_8087_pop_stack_value(coprocessor);
    }
}

static void hyperdos_8087_store_environment(hyperdos_x86_processor*                     processor,
                                            const hyperdos_x86_coprocessor_instruction* instruction,
                                            const hyperdos_8087*                        coprocessor)
{
    hyperdos_8087_write_memory_word(processor,
                                    instruction->segmentRegister,
                                    (uint16_t)(instruction->offset + HYPERDOS_8087_ENVIRONMENT_CONTROL_OFFSET),
                                    coprocessor->controlWord);
    hyperdos_8087_write_memory_word(processor,
                                    instruction->segmentRegister,
                                    (uint16_t)(instruction->offset + HYPERDOS_8087_ENVIRONMENT_STATUS_OFFSET),
                                    coprocessor->statusWord);
    hyperdos_8087_write_memory_word(processor,
                                    instruction->segmentRegister,
                                    (uint16_t)(instruction->offset + HYPERDOS_8087_ENVIRONMENT_TAG_OFFSET),
                                    coprocessor->tagWord);
    hyperdos_8087_write_memory_word(processor,
                                    instruction->segmentRegister,
                                    (uint16_t)(instruction->offset + HYPERDOS_8087_ENVIRONMENT_INSTRUCTION_OFFSET),
                                    coprocessor->lastInstructionOffset);
    hyperdos_8087_write_memory_word(processor,
                                    instruction->segmentRegister,
                                    (uint16_t)(instruction->offset + HYPERDOS_8087_ENVIRONMENT_INSTRUCTION_SEGMENT),
                                    coprocessor->lastInstructionSegment);
    hyperdos_8087_write_memory_word(processor,
                                    instruction->segmentRegister,
                                    (uint16_t)(instruction->offset + HYPERDOS_8087_ENVIRONMENT_OPERAND_OFFSET),
                                    coprocessor->lastOperandOffset);
    hyperdos_8087_write_memory_word(processor,
                                    instruction->segmentRegister,
                                    (uint16_t)(instruction->offset + HYPERDOS_8087_ENVIRONMENT_OPERAND_SEGMENT),
                                    coprocessor->lastOperandSegment);
}

static void hyperdos_8087_load_environment(const hyperdos_x86_processor*               processor,
                                           const hyperdos_x86_coprocessor_instruction* instruction,
                                           hyperdos_8087*                              coprocessor)
{
    size_t registerIndex = 0;

    coprocessor->controlWord           = hyperdos_8087_read_memory_word(processor,
                                                              instruction->segmentRegister,
                                                              (uint16_t)(instruction->offset +
                                                                         HYPERDOS_8087_ENVIRONMENT_CONTROL_OFFSET));
    coprocessor->statusWord            = hyperdos_8087_read_memory_word(processor,
                                                             instruction->segmentRegister,
                                                             (uint16_t)(instruction->offset +
                                                                        HYPERDOS_8087_ENVIRONMENT_STATUS_OFFSET));
    coprocessor->tagWord               = hyperdos_8087_read_memory_word(processor,
                                                          instruction->segmentRegister,
                                                          (uint16_t)(instruction->offset +
                                                                     HYPERDOS_8087_ENVIRONMENT_TAG_OFFSET));
    coprocessor->lastInstructionOffset = hyperdos_8087_read_memory_word(
            processor,
            instruction->segmentRegister,
            (uint16_t)(instruction->offset + HYPERDOS_8087_ENVIRONMENT_INSTRUCTION_OFFSET));
    coprocessor->lastInstructionSegment = hyperdos_8087_read_memory_word(
            processor,
            instruction->segmentRegister,
            (uint16_t)(instruction->offset + HYPERDOS_8087_ENVIRONMENT_INSTRUCTION_SEGMENT));
    coprocessor->lastOperandOffset =
            hyperdos_8087_read_memory_word(processor,
                                           instruction->segmentRegister,
                                           (uint16_t)(instruction->offset + HYPERDOS_8087_ENVIRONMENT_OPERAND_OFFSET));
    coprocessor->lastOperandSegment =
            hyperdos_8087_read_memory_word(processor,
                                           instruction->segmentRegister,
                                           (uint16_t)(instruction->offset + HYPERDOS_8087_ENVIRONMENT_OPERAND_SEGMENT));
    coprocessor->stackTop = (uint8_t)((coprocessor->statusWord & HYPERDOS_8087_STATUS_TOP_MASK) >>
                                      HYPERDOS_8087_STATUS_TOP_SHIFT);

    for (registerIndex = 0; registerIndex < HYPERDOS_8087_REGISTER_COUNT; ++registerIndex)
    {
        coprocessor->registerTags[registerIndex] = (uint8_t)((coprocessor->tagWord >> (registerIndex * 2u)) &
                                                             HYPERDOS_8087_REGISTER_MEMORY_MASK);
    }
    hyperdos_8087_update_environment_words(coprocessor);
}

static void hyperdos_8087_execute_memory_instruction(hyperdos_x86_processor*                     processor,
                                                     const hyperdos_x86_coprocessor_instruction* instruction,
                                                     hyperdos_8087*                              coprocessor)
{
    long double stackValue   = hyperdos_8087_read_stack_value(coprocessor, 0u);
    int64_t     integerValue = 0;

    switch (instruction->operationCode)
    {
    case HYPERDOS_8087_ESCAPE_D8:
        hyperdos_8087_execute_stack_operation(coprocessor,
                                              instruction->operationIndex,
                                              hyperdos_8087_read_single_precision_real(processor,
                                                                                       instruction->segmentRegister,
                                                                                       instruction->offset));
        break;
    case HYPERDOS_8087_ESCAPE_D9:
        if (instruction->operationIndex == 0u)
        {
            hyperdos_8087_push_stack_value(coprocessor,
                                           hyperdos_8087_read_single_precision_real(processor,
                                                                                    instruction->segmentRegister,
                                                                                    instruction->offset));
        }
        else if (instruction->operationIndex == 2u)
        {
            hyperdos_8087_write_single_precision_real(processor,
                                                      instruction->segmentRegister,
                                                      instruction->offset,
                                                      stackValue);
        }
        else if (instruction->operationIndex == 3u)
        {
            hyperdos_8087_write_single_precision_real(processor,
                                                      instruction->segmentRegister,
                                                      instruction->offset,
                                                      stackValue);
            hyperdos_8087_pop_stack_value(coprocessor);
        }
        else if (instruction->operationIndex == 4u)
        {
            hyperdos_8087_load_environment(processor, instruction, coprocessor);
        }
        else if (instruction->operationIndex == 5u)
        {
            coprocessor->controlWord = hyperdos_8087_read_memory_word(processor,
                                                                      instruction->segmentRegister,
                                                                      instruction->offset);
        }
        else if (instruction->operationIndex == 6u)
        {
            hyperdos_8087_store_environment(processor, instruction, coprocessor);
        }
        else if (instruction->operationIndex == 7u)
        {
            hyperdos_8087_write_memory_word(processor,
                                            instruction->segmentRegister,
                                            instruction->offset,
                                            coprocessor->controlWord);
        }
        break;
    case HYPERDOS_8087_ESCAPE_DA:
        hyperdos_8087_execute_stack_operation(
                coprocessor,
                instruction->operationIndex,
                (long double)(int32_t)hyperdos_8087_read_memory_double_word(processor,
                                                                            instruction->segmentRegister,
                                                                            instruction->offset));
        break;
    case HYPERDOS_8087_ESCAPE_DB:
        if (instruction->operationIndex == 0u)
        {
            hyperdos_8087_push_stack_value(coprocessor,
                                           (long double)(int32_t)
                                                   hyperdos_8087_read_memory_double_word(processor,
                                                                                         instruction->segmentRegister,
                                                                                         instruction->offset));
        }
        else if (instruction->operationIndex == 2u || instruction->operationIndex == 3u)
        {
            integerValue = hyperdos_8087_round_to_integer(coprocessor, stackValue);
            hyperdos_8087_write_memory_double_word(processor,
                                                   instruction->segmentRegister,
                                                   instruction->offset,
                                                   (uint32_t)(int32_t)integerValue);
            if (instruction->operationIndex == 3u)
            {
                hyperdos_8087_pop_stack_value(coprocessor);
            }
        }
        break;
    case HYPERDOS_8087_ESCAPE_DC:
        hyperdos_8087_execute_stack_operation(coprocessor,
                                              instruction->operationIndex,
                                              hyperdos_8087_read_double_precision_real(processor,
                                                                                       instruction->segmentRegister,
                                                                                       instruction->offset));
        break;
    case HYPERDOS_8087_ESCAPE_DD:
        if (instruction->operationIndex == 0u)
        {
            hyperdos_8087_push_stack_value(coprocessor,
                                           hyperdos_8087_read_double_precision_real(processor,
                                                                                    instruction->segmentRegister,
                                                                                    instruction->offset));
        }
        else if (instruction->operationIndex == 2u)
        {
            hyperdos_8087_write_double_precision_real(processor,
                                                      instruction->segmentRegister,
                                                      instruction->offset,
                                                      stackValue);
        }
        else if (instruction->operationIndex == 3u)
        {
            hyperdos_8087_write_double_precision_real(processor,
                                                      instruction->segmentRegister,
                                                      instruction->offset,
                                                      stackValue);
            hyperdos_8087_pop_stack_value(coprocessor);
        }
        else if (instruction->operationIndex == 4u)
        {
            hyperdos_8087_load_environment(processor, instruction, coprocessor);
        }
        else if (instruction->operationIndex == 6u)
        {
            hyperdos_8087_store_environment(processor, instruction, coprocessor);
            hyperdos_8087_initialize_programming_state(coprocessor);
        }
        else if (instruction->operationIndex == 7u)
        {
            hyperdos_8087_write_memory_word(processor,
                                            instruction->segmentRegister,
                                            instruction->offset,
                                            coprocessor->statusWord);
        }
        break;
    case HYPERDOS_8087_ESCAPE_DE:
        hyperdos_8087_execute_stack_operation(coprocessor,
                                              instruction->operationIndex,
                                              (long double)(int16_t)
                                                      hyperdos_8087_read_memory_word(processor,
                                                                                     instruction->segmentRegister,
                                                                                     instruction->offset));
        break;
    case HYPERDOS_8087_ESCAPE_DF:
        if (instruction->operationIndex == 0u)
        {
            hyperdos_8087_push_stack_value(coprocessor,
                                           (long double)(int16_t)
                                                   hyperdos_8087_read_memory_word(processor,
                                                                                  instruction->segmentRegister,
                                                                                  instruction->offset));
        }
        else if (instruction->operationIndex == 2u || instruction->operationIndex == 3u)
        {
            integerValue = hyperdos_8087_round_to_integer(coprocessor, stackValue);
            hyperdos_8087_write_memory_word(processor,
                                            instruction->segmentRegister,
                                            instruction->offset,
                                            (uint16_t)(int16_t)integerValue);
            if (instruction->operationIndex == 3u)
            {
                hyperdos_8087_pop_stack_value(coprocessor);
            }
        }
        else if (instruction->operationIndex == 5u)
        {
            hyperdos_8087_push_stack_value(coprocessor,
                                           (long double)(int64_t)
                                                   hyperdos_8087_read_memory_quad_word(processor,
                                                                                       instruction->segmentRegister,
                                                                                       instruction->offset));
        }
        else if (instruction->operationIndex == 7u)
        {
            integerValue = hyperdos_8087_round_to_integer(coprocessor, stackValue);
            hyperdos_8087_write_memory_quad_word(processor,
                                                 instruction->segmentRegister,
                                                 instruction->offset,
                                                 (uint64_t)integerValue);
            hyperdos_8087_pop_stack_value(coprocessor);
        }
        break;
    }
}

static void hyperdos_8087_execute_register_instruction(hyperdos_x86_processor*                     processor,
                                                       const hyperdos_x86_coprocessor_instruction* instruction,
                                                       hyperdos_8087*                              coprocessor)
{
    uint8_t registerGroup = (uint8_t)(instruction->registerMemoryByte & HYPERDOS_8087_REGISTER_MEMORY_GROUP_MASK);
    uint8_t stackIndex    = (uint8_t)(instruction->registerMemoryByte & HYPERDOS_8087_REGISTER_MEMORY_MASK);

    switch (instruction->operationCode)
    {
    case HYPERDOS_8087_ESCAPE_D8:
        hyperdos_8087_execute_register_operation(coprocessor, instruction->operationIndex, stackIndex);
        break;
    case HYPERDOS_8087_ESCAPE_D9:
        if (registerGroup == HYPERDOS_8087_REGISTER_MEMORY_GROUP_LOAD_REGISTER)
        {
            hyperdos_8087_push_stack_value(coprocessor, hyperdos_8087_read_stack_value(coprocessor, stackIndex));
        }
        else if (registerGroup == HYPERDOS_8087_REGISTER_MEMORY_GROUP_EXCHANGE_REGISTER)
        {
            long double firstValue  = hyperdos_8087_read_stack_value(coprocessor, 0u);
            long double secondValue = hyperdos_8087_read_stack_value(coprocessor, stackIndex);
            hyperdos_8087_write_stack_value(coprocessor, 0u, secondValue);
            hyperdos_8087_write_stack_value(coprocessor, stackIndex, firstValue);
        }
        else if (instruction->registerMemoryByte == 0xD0u)
        {
        }
        else if (instruction->registerMemoryByte == 0xE0u)
        {
            hyperdos_8087_write_stack_value(coprocessor, 0u, -hyperdos_8087_read_stack_value(coprocessor, 0u));
        }
        else if (instruction->registerMemoryByte == 0xE1u)
        {
            long double value = hyperdos_8087_read_stack_value(coprocessor, 0u);
            hyperdos_8087_write_stack_value(coprocessor, 0u, value < 0.0L ? -value : value);
        }
        else if (instruction->registerMemoryByte == 0xE4u)
        {
            hyperdos_8087_compare_values(coprocessor, hyperdos_8087_read_stack_value(coprocessor, 0u), 0.0L);
        }
        else if (instruction->registerMemoryByte == 0xE8u)
        {
            hyperdos_8087_push_stack_value(coprocessor, 1.0L);
        }
        else if (instruction->registerMemoryByte == 0xEBu)
        {
            hyperdos_8087_push_stack_value(coprocessor, 3.14159265358979323846264338327950288L);
        }
        else if (instruction->registerMemoryByte == 0xEEu)
        {
            hyperdos_8087_push_stack_value(coprocessor, 0.0L);
        }
        break;
    case HYPERDOS_8087_ESCAPE_DB:
        if (instruction->registerMemoryByte == HYPERDOS_8087_REGISTER_MEMORY_CLEAR_EXCEPTIONS)
        {
            coprocessor->statusWord &= (uint16_t)~(HYPERDOS_8087_STATUS_INVALID_OPERATION |
                                                   HYPERDOS_8087_STATUS_ZERO_DIVIDE | HYPERDOS_8087_STATUS_STACK_FAULT |
                                                   HYPERDOS_8087_STATUS_ERROR_SUMMARY);
        }
        else if (instruction->registerMemoryByte == HYPERDOS_8087_REGISTER_MEMORY_INITIALIZE)
        {
            hyperdos_8087_initialize_programming_state(coprocessor);
        }
        else if (instruction->registerMemoryByte == HYPERDOS_8087_REGISTER_MEMORY_SET_PROTECTED_MODE &&
                 coprocessor->model == HYPERDOS_X87_MODEL_80287)
        {
            coprocessor->protectedModeEnabled = 1u;
        }
        break;
    case HYPERDOS_8087_ESCAPE_DC:
        hyperdos_8087_execute_reversed_register_operation(coprocessor, instruction->operationIndex, stackIndex, 0);
        break;
    case HYPERDOS_8087_ESCAPE_DD:
        if (registerGroup == HYPERDOS_8087_REGISTER_MEMORY_GROUP_FREE_REGISTER)
        {
            coprocessor->registerTags
                    [hyperdos_8087_physical_stack_register(coprocessor, stackIndex)] = HYPERDOS_8087_STACK_TAG_EMPTY;
            hyperdos_8087_update_environment_words(coprocessor);
        }
        else if (registerGroup == HYPERDOS_8087_REGISTER_MEMORY_GROUP_STORE_REGISTER)
        {
            hyperdos_8087_write_stack_value(coprocessor, stackIndex, hyperdos_8087_read_stack_value(coprocessor, 0u));
        }
        else if (registerGroup == HYPERDOS_8087_REGISTER_MEMORY_GROUP_STORE_POP_REGISTER)
        {
            hyperdos_8087_write_stack_value(coprocessor, stackIndex, hyperdos_8087_read_stack_value(coprocessor, 0u));
            hyperdos_8087_pop_stack_value(coprocessor);
        }
        break;
    case HYPERDOS_8087_ESCAPE_DE:
        if (instruction->registerMemoryByte == 0xD9u)
        {
            hyperdos_8087_compare_values(coprocessor,
                                         hyperdos_8087_read_stack_value(coprocessor, 0u),
                                         hyperdos_8087_read_stack_value(coprocessor, 1u));
            hyperdos_8087_pop_stack_value(coprocessor);
            hyperdos_8087_pop_stack_value(coprocessor);
        }
        else
        {
            hyperdos_8087_execute_reversed_register_operation(coprocessor, instruction->operationIndex, stackIndex, 1);
        }
        break;
    case HYPERDOS_8087_ESCAPE_DF:
        if (instruction->registerMemoryByte == HYPERDOS_8087_REGISTER_MEMORY_STORE_STATUS_ACCUMULATOR)
        {
            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                   coprocessor->statusWord);
        }
        break;
    }
}

void hyperdos_random_access_memory_initialize(hyperdos_random_access_memory* memory,
                                              uint8_t*                       bytes,
                                              size_t                         byteCount,
                                              uint32_t                       firstAddress)
{
    if (memory == NULL)
    {
        return;
    }
    memory->bytes        = bytes;
    memory->byteCount    = byteCount;
    memory->firstAddress = firstAddress;
}

void hyperdos_read_only_memory_initialize(hyperdos_read_only_memory* memory,
                                          const uint8_t*             bytes,
                                          size_t                     byteCount,
                                          uint32_t                   firstAddress)
{
    if (memory == NULL)
    {
        return;
    }
    memory->bytes        = bytes;
    memory->byteCount    = byteCount;
    memory->firstAddress = firstAddress;
}

uint8_t hyperdos_random_access_memory_read_byte(void* device, uint32_t physicalAddress)
{
    hyperdos_random_access_memory* memory          = (hyperdos_random_access_memory*)device;
    uint32_t                       relativeAddress = 0;

    if (memory == NULL || memory->bytes == NULL)
    {
        return HYPERDOS_OPEN_BUS_BYTE;
    }
    relativeAddress = physicalAddress - memory->firstAddress;
    if (relativeAddress >= memory->byteCount)
    {
        return HYPERDOS_OPEN_BUS_BYTE;
    }
    return memory->bytes[relativeAddress];
}

void hyperdos_random_access_memory_write_byte(void* device, uint32_t physicalAddress, uint8_t value)
{
    hyperdos_random_access_memory* memory          = (hyperdos_random_access_memory*)device;
    uint32_t                       relativeAddress = 0;

    if (memory == NULL || memory->bytes == NULL)
    {
        return;
    }
    relativeAddress = physicalAddress - memory->firstAddress;
    if (relativeAddress >= memory->byteCount)
    {
        return;
    }
    memory->bytes[relativeAddress] = value;
}

uint8_t hyperdos_read_only_memory_read_byte(void* device, uint32_t physicalAddress)
{
    hyperdos_read_only_memory* memory          = (hyperdos_read_only_memory*)device;
    uint32_t                   relativeAddress = 0;

    if (memory == NULL || memory->bytes == NULL)
    {
        return HYPERDOS_OPEN_BUS_BYTE;
    }
    relativeAddress = physicalAddress - memory->firstAddress;
    if (relativeAddress >= memory->byteCount)
    {
        return HYPERDOS_OPEN_BUS_BYTE;
    }
    return memory->bytes[relativeAddress];
}

void hyperdos_read_only_memory_write_byte(void* device, uint32_t physicalAddress, uint8_t value)
{
    (void)device;
    (void)physicalAddress;
    (void)value;
}

void hyperdos_programmable_interrupt_controller_initialize(hyperdos_programmable_interrupt_controller* controller,
                                                           uint8_t                                     vectorBase)
{
    if (controller == NULL)
    {
        return;
    }
    memset(controller, 0, sizeof(*controller));
    controller->interruptMaskRegister = HYPERDOS_OPEN_BUS_BYTE;
    controller->vectorBase            = vectorBase;
}

uint8_t hyperdos_programmable_interrupt_controller_read_byte(void* device, uint16_t port)
{
    hyperdos_programmable_interrupt_controller* controller = (hyperdos_programmable_interrupt_controller*)device;
    uint16_t portOffset = (uint16_t)(port & HYPERDOS_INTERRUPT_CONTROLLER_DATA_PORT_OFFSET);

    if (controller == NULL)
    {
        return HYPERDOS_OPEN_BUS_BYTE;
    }
    if (portOffset == HYPERDOS_INTERRUPT_CONTROLLER_COMMAND_PORT_OFFSET)
    {
        return controller->readInServiceRegister != 0u ? controller->inServiceRegister
                                                       : controller->interruptRequestRegister;
    }
    return controller->interruptMaskRegister;
}

void hyperdos_programmable_interrupt_controller_write_byte(void* device, uint16_t port, uint8_t value)
{
    hyperdos_programmable_interrupt_controller* controller = (hyperdos_programmable_interrupt_controller*)device;
    uint16_t portOffset = (uint16_t)(port & HYPERDOS_INTERRUPT_CONTROLLER_DATA_PORT_OFFSET);

    if (controller == NULL)
    {
        return;
    }

    if (portOffset == HYPERDOS_INTERRUPT_CONTROLLER_COMMAND_PORT_OFFSET)
    {
        if ((value & HYPERDOS_INTERRUPT_CONTROLLER_INITIALIZATION_COMMAND) != 0u)
        {
            controller->initializationCommandWordStep = 2u;
            controller->expectsInitializationCommandWordFour =
                    (value & HYPERDOS_INTERRUPT_CONTROLLER_INITIALIZATION_NEEDS_WORD_FOUR) != 0u;
            controller->interruptMaskRegister = 0u;
            return;
        }
        if ((value & HYPERDOS_INTERRUPT_CONTROLLER_END_OF_INTERRUPT) != 0u)
        {
            uint8_t requestLine = 0;
            if ((value & HYPERDOS_INTERRUPT_CONTROLLER_SPECIFIC_END_OF_INTERRUPT) != 0u)
            {
                requestLine                    = (uint8_t)(value & HYPERDOS_INTERRUPT_CONTROLLER_OPERATION_LEVEL_MASK);
                controller->inServiceRegister &= (uint8_t)~(1u << requestLine);
            }
            else
            {
                for (requestLine = 0; requestLine < HYPERDOS_INTERRUPT_CONTROLLER_REQUEST_LINE_COUNT; ++requestLine)
                {
                    uint8_t requestMask = (uint8_t)(1u << requestLine);
                    if ((controller->inServiceRegister & requestMask) != 0u)
                    {
                        controller->inServiceRegister &= (uint8_t)~requestMask;
                        break;
                    }
                }
            }
            return;
        }
        if ((value & HYPERDOS_INTERRUPT_CONTROLLER_OPERATION_COMMAND_THREE) != 0u)
        {
            controller->readInServiceRegister = (value & HYPERDOS_INTERRUPT_CONTROLLER_READ_IN_SERVICE_REGISTER) ==
                                                HYPERDOS_INTERRUPT_CONTROLLER_READ_IN_SERVICE_REGISTER;
        }
        return;
    }

    if (controller->initializationCommandWordStep == 2u)
    {
        controller->vectorBase                    = (uint8_t)(value & HYPERDOS_INTERRUPT_CONTROLLER_VECTOR_BASE_MASK);
        controller->initializationCommandWordStep = 3u;
        return;
    }
    if (controller->initializationCommandWordStep == 3u)
    {
        controller->initializationCommandWordStep = controller->expectsInitializationCommandWordFour != 0u ? 4u : 0u;
        return;
    }
    if (controller->initializationCommandWordStep == 4u)
    {
        controller->autoEndOfInterrupt            = (value & HYPERDOS_INTERRUPT_CONTROLLER_AUTO_END_OF_INTERRUPT) != 0u;
        controller->initializationCommandWordStep = 0u;
        return;
    }
    controller->interruptMaskRegister = value;
}

void hyperdos_programmable_interrupt_controller_raise_request(hyperdos_programmable_interrupt_controller* controller,
                                                              uint8_t                                     requestLine)
{
    if (controller == NULL || requestLine >= HYPERDOS_INTERRUPT_CONTROLLER_REQUEST_LINE_COUNT)
    {
        return;
    }
    controller->interruptRequestRegister |= (uint8_t)(1u << requestLine);
}

static int hyperdos_programmable_interrupt_controller_request_can_preempt(
        const hyperdos_programmable_interrupt_controller* controller,
        uint8_t                                           requestLine)
{
    uint8_t inServiceLine = 0;

    for (inServiceLine = 0; inServiceLine < HYPERDOS_INTERRUPT_CONTROLLER_REQUEST_LINE_COUNT; ++inServiceLine)
    {
        uint8_t inServiceMask = (uint8_t)(1u << inServiceLine);
        if ((controller->inServiceRegister & inServiceMask) != 0u)
        {
            return requestLine < inServiceLine;
        }
    }
    return 1;
}

int hyperdos_programmable_interrupt_controller_has_pending_unmasked_request(
        const hyperdos_programmable_interrupt_controller* controller)
{
    uint8_t requestLine = 0;

    if (controller == NULL)
    {
        return 0;
    }

    for (requestLine = 0; requestLine < HYPERDOS_INTERRUPT_CONTROLLER_REQUEST_LINE_COUNT; ++requestLine)
    {
        uint8_t requestMask = (uint8_t)(1u << requestLine);
        if ((controller->interruptRequestRegister & requestMask) != 0u &&
            (controller->interruptMaskRegister & requestMask) == 0u &&
            hyperdos_programmable_interrupt_controller_request_can_preempt(controller, requestLine))
        {
            return 1;
        }
    }
    return 0;
}

int hyperdos_programmable_interrupt_controller_acknowledge_request(
        hyperdos_programmable_interrupt_controller* controller,
        uint8_t*                                    requestLine,
        uint8_t*                                    interruptNumber)
{
    uint8_t currentRequestLine = 0;

    if (controller == NULL || requestLine == NULL || interruptNumber == NULL)
    {
        return 0;
    }

    for (currentRequestLine = 0; currentRequestLine < HYPERDOS_INTERRUPT_CONTROLLER_REQUEST_LINE_COUNT;
         ++currentRequestLine)
    {
        uint8_t requestMask = (uint8_t)(1u << currentRequestLine);
        if ((controller->interruptRequestRegister & requestMask) != 0u &&
            (controller->interruptMaskRegister & requestMask) == 0u &&
            hyperdos_programmable_interrupt_controller_request_can_preempt(controller, currentRequestLine))
        {
            controller->interruptRequestRegister &= (uint8_t)~requestMask;
            if (controller->autoEndOfInterrupt == 0u)
            {
                controller->inServiceRegister |= requestMask;
            }
            *requestLine     = currentRequestLine;
            *interruptNumber = (uint8_t)(controller->vectorBase + currentRequestLine);
            return 1;
        }
    }
    return 0;
}

int hyperdos_programmable_interrupt_controller_acknowledge(hyperdos_programmable_interrupt_controller* controller,
                                                           uint8_t*                                    interruptNumber)
{
    uint8_t requestLine = 0u;

    return hyperdos_programmable_interrupt_controller_acknowledge_request(controller, &requestLine, interruptNumber);
}

void hyperdos_direct_memory_access_controller_initialize(hyperdos_direct_memory_access_controller* controller)
{
    size_t channelIndex = 0;

    if (controller == NULL)
    {
        return;
    }
    memset(controller, 0, sizeof(*controller));
    for (channelIndex = 0; channelIndex < HYPERDOS_DIRECT_MEMORY_ACCESS_CHANNEL_COUNT; ++channelIndex)
    {
        controller->channels[channelIndex].masked = 1u;
    }
}

uint8_t hyperdos_direct_memory_access_controller_read_byte(void* device, uint16_t port)
{
    hyperdos_direct_memory_access_controller* controller = (hyperdos_direct_memory_access_controller*)device;
    uint16_t                               portOffset = (uint16_t)(port & HYPERDOS_DIRECT_MEMORY_ACCESS_ALL_MASK_PORT);
    hyperdos_direct_memory_access_channel* channel    = NULL;
    uint8_t                                value      = 0;

    if (controller == NULL)
    {
        return HYPERDOS_OPEN_BUS_BYTE;
    }
    if (portOffset <= HYPERDOS_DIRECT_MEMORY_ACCESS_ADDRESS_COUNT_PORT_LAST)
    {
        uint16_t channelIndex = (uint16_t)(portOffset >> 1u);
        channel               = &controller->channels[channelIndex];
        if ((portOffset & 1u) == 0u)
        {
            value = controller->firstLastFlipFlop == 0u
                            ? (uint8_t)(channel->currentAddress & HYPERDOS_X86_BYTE_MASK)
                            : (uint8_t)(channel->currentAddress >> HYPERDOS_X86_BYTE_BIT_COUNT);
        }
        else
        {
            value = controller->firstLastFlipFlop == 0u
                            ? (uint8_t)(channel->currentCount & HYPERDOS_X86_BYTE_MASK)
                            : (uint8_t)(channel->currentCount >> HYPERDOS_X86_BYTE_BIT_COUNT);
        }
        controller->firstLastFlipFlop ^= 1u;
        return value;
    }
    if (portOffset == HYPERDOS_DIRECT_MEMORY_ACCESS_COMMAND_PORT)
    {
        return controller->status;
    }
    if (portOffset == HYPERDOS_DIRECT_MEMORY_ACCESS_REQUEST_PORT)
    {
        return controller->temporary;
    }
    return HYPERDOS_OPEN_BUS_BYTE;
}

void hyperdos_direct_memory_access_controller_write_byte(void* device, uint16_t port, uint8_t value)
{
    hyperdos_direct_memory_access_controller* controller = (hyperdos_direct_memory_access_controller*)device;
    uint16_t                               portOffset = (uint16_t)(port & HYPERDOS_DIRECT_MEMORY_ACCESS_ALL_MASK_PORT);
    hyperdos_direct_memory_access_channel* channel    = NULL;

    if (controller == NULL)
    {
        return;
    }
    if (portOffset <= HYPERDOS_DIRECT_MEMORY_ACCESS_ADDRESS_COUNT_PORT_LAST)
    {
        uint16_t channelIndex = (uint16_t)(portOffset >> 1u);
        channel               = &controller->channels[channelIndex];
        if ((portOffset & 1u) == 0u)
        {
            channel->currentAddress = controller->firstLastFlipFlop == 0u
                                              ? (uint16_t)((channel->currentAddress & HYPERDOS_X86_HIGH_BYTE_MASK) |
                                                           value)
                                              : (uint16_t)((channel->currentAddress & HYPERDOS_X86_BYTE_MASK) |
                                                           ((uint16_t)value << HYPERDOS_X86_BYTE_BIT_COUNT));
            channel->baseAddress    = channel->currentAddress;
        }
        else
        {
            channel->currentCount = controller->firstLastFlipFlop == 0u
                                            ? (uint16_t)((channel->currentCount & HYPERDOS_X86_HIGH_BYTE_MASK) | value)
                                            : (uint16_t)((channel->currentCount & HYPERDOS_X86_BYTE_MASK) |
                                                         ((uint16_t)value << HYPERDOS_X86_BYTE_BIT_COUNT));
            channel->baseCount    = channel->currentCount;
        }
        controller->firstLastFlipFlop ^= 1u;
        return;
    }

    switch (portOffset)
    {
    case HYPERDOS_DIRECT_MEMORY_ACCESS_COMMAND_PORT:
        controller->command = value;
        break;
    case HYPERDOS_DIRECT_MEMORY_ACCESS_REQUEST_PORT:
        controller->request = value;
        break;
    case HYPERDOS_DIRECT_MEMORY_ACCESS_SINGLE_MASK_PORT:
    {
        uint8_t channelIndex = (uint8_t)(value & HYPERDOS_DIRECT_MEMORY_ACCESS_CHANNEL_MASK);
        if (channelIndex < HYPERDOS_DIRECT_MEMORY_ACCESS_CHANNEL_COUNT)
        {
            controller->channels[channelIndex].masked = (value & HYPERDOS_DIRECT_MEMORY_ACCESS_SINGLE_MASK_SET_BIT) !=
                                                        0u;
        }
        break;
    }
    case HYPERDOS_DIRECT_MEMORY_ACCESS_MODE_PORT:
    {
        uint8_t channelIndex = (uint8_t)(value & HYPERDOS_DIRECT_MEMORY_ACCESS_CHANNEL_MASK);
        if (channelIndex < HYPERDOS_DIRECT_MEMORY_ACCESS_CHANNEL_COUNT)
        {
            controller->channels[channelIndex].mode = value;
        }
        break;
    }
    case HYPERDOS_DIRECT_MEMORY_ACCESS_FLIP_FLOP_RESET_PORT:
        controller->firstLastFlipFlop = 0u;
        break;
    case HYPERDOS_DIRECT_MEMORY_ACCESS_MASTER_RESET_PORT:
        hyperdos_direct_memory_access_controller_initialize(controller);
        break;
    case HYPERDOS_DIRECT_MEMORY_ACCESS_MASK_RESET_PORT:
    {
        size_t channelIndex = 0;
        for (channelIndex = 0; channelIndex < HYPERDOS_DIRECT_MEMORY_ACCESS_CHANNEL_COUNT; ++channelIndex)
        {
            controller->channels[channelIndex].masked = 0u;
        }
        break;
    }
    case HYPERDOS_DIRECT_MEMORY_ACCESS_ALL_MASK_PORT:
    {
        size_t channelIndex = 0;
        for (channelIndex = 0; channelIndex < HYPERDOS_DIRECT_MEMORY_ACCESS_CHANNEL_COUNT; ++channelIndex)
        {
            controller->channels[channelIndex].masked = (value >> channelIndex) & 1u;
        }
        break;
    }
    }
}

void hyperdos_programmable_interval_timer_initialize(hyperdos_programmable_interval_timer* timer)
{
    size_t channelIndex = 0;

    if (timer == NULL)
    {
        return;
    }
    memset(timer, 0, sizeof(*timer));
    for (channelIndex = 0; channelIndex < HYPERDOS_INTERVAL_TIMER_CHANNEL_COUNT; ++channelIndex)
    {
        timer->channels[channelIndex].accessMode    = HYPERDOS_INTERVAL_TIMER_ACCESS_MODE_LOW_HIGH_BYTE;
        timer->channels[channelIndex].operatingMode = HYPERDOS_INTERVAL_TIMER_MODE_SQUARE_WAVE;
        timer->channels[channelIndex].outputLevel   = 1u;
    }
}

static uint32_t hyperdos_x86_interval_timer_effective_count(uint16_t reloadValue)
{
    return reloadValue == 0u ? HYPERDOS_INTERVAL_TIMER_MAXIMUM_COUNT : reloadValue;
}

static uint16_t hyperdos_x86_interval_timer_visible_count(const hyperdos_x86_interval_timer_channel* channel)
{
    if (channel->currentCounter == HYPERDOS_INTERVAL_TIMER_MAXIMUM_COUNT)
    {
        return 0u;
    }
    return (uint16_t)channel->currentCounter;
}

static void hyperdos_x86_interval_timer_load_count(hyperdos_x86_interval_timer_channel* channel)
{
    channel->reloadCounter  = hyperdos_x86_interval_timer_effective_count(channel->reloadValue);
    channel->currentCounter = channel->reloadCounter;
    channel->currentValue   = hyperdos_x86_interval_timer_visible_count(channel);
    channel->outputLevel = channel->operatingMode == HYPERDOS_INTERVAL_TIMER_MODE_INTERRUPT_ON_TERMINAL_COUNT ? 0u : 1u;
}

uint8_t hyperdos_programmable_interval_timer_read_byte(void* device, uint16_t port)
{
    hyperdos_programmable_interval_timer* timer        = (hyperdos_programmable_interval_timer*)device;
    uint16_t                              channelIndex = (uint16_t)(port & HYPERDOS_INTERVAL_TIMER_CONTROL_PORT_OFFSET);
    hyperdos_x86_interval_timer_channel*  channel      = NULL;
    uint16_t                              readValue    = 0u;
    uint8_t                               value        = 0;

    if (timer == NULL || channelIndex >= HYPERDOS_INTERVAL_TIMER_CHANNEL_COUNT)
    {
        return HYPERDOS_OPEN_BUS_BYTE;
    }

    channel   = &timer->channels[channelIndex];
    readValue = channel->countLatched != 0u ? channel->latchedValue
                                            : hyperdos_x86_interval_timer_visible_count(channel);
    if (channel->accessMode == HYPERDOS_INTERVAL_TIMER_ACCESS_MODE_LOW_BYTE)
    {
        value                 = (uint8_t)(readValue & HYPERDOS_X86_BYTE_MASK);
        channel->countLatched = 0u;
    }
    else if (channel->accessMode == HYPERDOS_INTERVAL_TIMER_ACCESS_MODE_HIGH_BYTE)
    {
        value                 = (uint8_t)(readValue >> HYPERDOS_X86_BYTE_BIT_COUNT);
        channel->countLatched = 0u;
    }
    else
    {
        uint8_t* readLowByte = channel->countLatched != 0u ? &channel->latchedReadLowByte : &channel->readLatchLowByte;
        if (*readLowByte == 0u)
        {
            value        = (uint8_t)(readValue & HYPERDOS_X86_BYTE_MASK);
            *readLowByte = 1u;
        }
        else
        {
            value                 = (uint8_t)(readValue >> HYPERDOS_X86_BYTE_BIT_COUNT);
            *readLowByte          = 0u;
            channel->countLatched = 0u;
        }
    }
    return value;
}

static void hyperdos_x86_interval_timer_write_reload_byte(hyperdos_x86_interval_timer_channel* channel, uint8_t value)
{
    if (channel->accessMode == HYPERDOS_INTERVAL_TIMER_ACCESS_MODE_LOW_BYTE)
    {
        channel->reloadValue = (uint16_t)((channel->reloadValue & HYPERDOS_X86_HIGH_BYTE_MASK) | value);
        hyperdos_x86_interval_timer_load_count(channel);
        return;
    }
    if (channel->accessMode == HYPERDOS_INTERVAL_TIMER_ACCESS_MODE_HIGH_BYTE)
    {
        channel->reloadValue = (uint16_t)((channel->reloadValue & HYPERDOS_X86_BYTE_MASK) |
                                          ((uint16_t)value << HYPERDOS_X86_BYTE_BIT_COUNT));
        hyperdos_x86_interval_timer_load_count(channel);
        return;
    }
    if (channel->writeLatchLowByte == 0u)
    {
        channel->reloadValue       = (uint16_t)((channel->reloadValue & HYPERDOS_X86_HIGH_BYTE_MASK) | value);
        channel->writeLatchLowByte = 1u;
    }
    else
    {
        channel->reloadValue       = (uint16_t)((channel->reloadValue & HYPERDOS_X86_BYTE_MASK) |
                                          ((uint16_t)value << HYPERDOS_X86_BYTE_BIT_COUNT));
        channel->writeLatchLowByte = 0u;
        hyperdos_x86_interval_timer_load_count(channel);
    }
}

void hyperdos_programmable_interval_timer_write_byte(void* device, uint16_t port, uint8_t value)
{
    hyperdos_programmable_interval_timer* timer      = (hyperdos_programmable_interval_timer*)device;
    uint16_t                              portOffset = (uint16_t)(port & HYPERDOS_INTERVAL_TIMER_CONTROL_PORT_OFFSET);
    hyperdos_x86_interval_timer_channel*  channel    = NULL;

    if (timer == NULL)
    {
        return;
    }
    if (portOffset == HYPERDOS_INTERVAL_TIMER_CONTROL_PORT_OFFSET)
    {
        uint8_t channelIndex = (uint8_t)(value >> HYPERDOS_INTERVAL_TIMER_SELECT_CHANNEL_SHIFT);
        uint8_t accessMode   = (uint8_t)((value >> HYPERDOS_INTERVAL_TIMER_ACCESS_MODE_SHIFT) &
                                       HYPERDOS_X86_LOW_TWO_BITS_MASK);
        if (channelIndex < HYPERDOS_INTERVAL_TIMER_CHANNEL_COUNT)
        {
            channel = &timer->channels[channelIndex];
            if (accessMode == HYPERDOS_INTERVAL_TIMER_ACCESS_MODE_LATCH_COUNT)
            {
                channel->latchedValue       = hyperdos_x86_interval_timer_visible_count(channel);
                channel->countLatched       = 1u;
                channel->latchedReadLowByte = 0u;
                return;
            }
            channel->accessMode    = accessMode;
            channel->operatingMode = (uint8_t)((value >> HYPERDOS_INTERVAL_TIMER_OPERATING_MODE_SHIFT) &
                                               HYPERDOS_X86_LOW_THREE_BITS_MASK);
            if (channel->operatingMode > HYPERDOS_INTERVAL_TIMER_MODE_HARDWARE_TRIGGERED_STROBE)
            {
                channel->operatingMode = (uint8_t)(channel->operatingMode -
                                                   HYPERDOS_INTERVAL_TIMER_MODE_SOFTWARE_TRIGGERED_STROBE);
            }
            channel->writeLatchLowByte = 0u;
            channel->readLatchLowByte  = 0u;
            channel->countLatched      = 0u;
        }
        return;
    }

    channel = &timer->channels[portOffset];
    hyperdos_x86_interval_timer_write_reload_byte(channel, value);
}

void hyperdos_programmable_interval_timer_tick(void* device, uint64_t elapsedClockCount)
{
    hyperdos_programmable_interval_timer* timer        = (hyperdos_programmable_interval_timer*)device;
    size_t                                channelIndex = 0;

    if (timer == NULL)
    {
        return;
    }

    for (channelIndex = 0; channelIndex < HYPERDOS_INTERVAL_TIMER_CHANNEL_COUNT; ++channelIndex)
    {
        hyperdos_x86_interval_timer_channel* channel             = &timer->channels[channelIndex];
        uint64_t                             remainingClockCount = elapsedClockCount;
        if (channel->reloadCounter == 0u)
        {
            continue;
        }
        if (channel->operatingMode == HYPERDOS_INTERVAL_TIMER_MODE_INTERRUPT_ON_TERMINAL_COUNT)
        {
            if (channel->currentCounter == 0u)
            {
                continue;
            }
            if (remainingClockCount >= channel->currentCounter)
            {
                channel->currentCounter = 0u;
                channel->currentValue   = 0u;
                if (channel->outputLevel == 0u)
                {
                    channel->outputLevel             = 1u;
                    channel->outputTransitionPending = 1u;
                }
            }
            else
            {
                channel->currentCounter -= (uint32_t)remainingClockCount;
                channel->currentValue    = hyperdos_x86_interval_timer_visible_count(channel);
            }
            continue;
        }

        while (remainingClockCount >= channel->currentCounter && channel->currentCounter != 0u)
        {
            remainingClockCount              -= channel->currentCounter;
            channel->currentCounter           = channel->reloadCounter;
            channel->outputLevel              = 1u;
            channel->outputTransitionPending  = 1u;
        }
        if (remainingClockCount != 0u && channel->currentCounter != 0u)
        {
            channel->currentCounter -= (uint32_t)remainingClockCount;
            if (channel->currentCounter == 0u)
            {
                channel->currentCounter          = channel->reloadCounter;
                channel->outputTransitionPending = 1u;
            }
        }
        channel->currentValue = hyperdos_x86_interval_timer_visible_count(channel);
    }
}

int hyperdos_programmable_interval_timer_get_and_clear_output_transition(hyperdos_programmable_interval_timer* timer,
                                                                         uint8_t channelIndex)
{
    hyperdos_x86_interval_timer_channel* channel = NULL;

    if (timer == NULL || channelIndex >= HYPERDOS_INTERVAL_TIMER_CHANNEL_COUNT)
    {
        return 0;
    }
    channel = &timer->channels[channelIndex];
    if (channel->outputTransitionPending == 0u)
    {
        return 0;
    }
    channel->outputTransitionPending = 0u;
    return 1;
}

void hyperdos_programmable_peripheral_interface_initialize(hyperdos_programmable_peripheral_interface* interface)
{
    if (interface == NULL)
    {
        return;
    }
    memset(interface, 0, sizeof(*interface));
    interface->control = HYPERDOS_X86_PERIPHERAL_INTERFACE_DEFAULT_CONTROL;
}

uint8_t hyperdos_programmable_peripheral_interface_read_byte(void* device, uint16_t port)
{
    hyperdos_programmable_peripheral_interface* interface = (hyperdos_programmable_peripheral_interface*)device;

    if (interface == NULL)
    {
        return HYPERDOS_OPEN_BUS_BYTE;
    }

    switch (port & HYPERDOS_X86_PERIPHERAL_INTERFACE_CONTROL_PORT_OFFSET)
    {
    case 0u:
        return interface->portA;
    case 1u:
        return interface->portB;
    case 2u:
        return interface->portC;
    case HYPERDOS_X86_PERIPHERAL_INTERFACE_CONTROL_PORT_OFFSET:
        return interface->control;
    }
    return HYPERDOS_OPEN_BUS_BYTE;
}

void hyperdos_programmable_peripheral_interface_write_byte(void* device, uint16_t port, uint8_t value)
{
    hyperdos_programmable_peripheral_interface* interface = (hyperdos_programmable_peripheral_interface*)device;

    if (interface == NULL)
    {
        return;
    }

    switch (port & HYPERDOS_X86_PERIPHERAL_INTERFACE_CONTROL_PORT_OFFSET)
    {
    case 0u:
        interface->portA = value;
        break;
    case 1u:
        interface->portB = value;
        break;
    case 2u:
        interface->portC = value;
        break;
    case HYPERDOS_X86_PERIPHERAL_INTERFACE_CONTROL_PORT_OFFSET:
        interface->control = value;
        break;
    }
}

static void hyperdos_intel_8042_keyboard_controller_update_status(hyperdos_intel_8042_keyboard_controller* controller)
{
    uint8_t statusRegister = HYPERDOS_KEYBOARD_CONTROLLER_STATUS_UNLOCKED;

    if (controller->outputQueueCount != 0u)
    {
        statusRegister |= HYPERDOS_KEYBOARD_CONTROLLER_STATUS_OUTPUT_BUFFER_FULL;
        if (controller->outputQueueAuxiliaryDevice[controller->outputQueueReadIndex] != 0u)
        {
            statusRegister |= HYPERDOS_KEYBOARD_CONTROLLER_STATUS_AUXILIARY_OUTPUT_BUFFER;
        }
    }
    if ((controller->commandByte & HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_SYSTEM_FLAG) != 0u)
    {
        statusRegister |= HYPERDOS_KEYBOARD_CONTROLLER_STATUS_SYSTEM_FLAG;
    }
    controller->statusRegister = (uint8_t)((controller->statusRegister &
                                            HYPERDOS_KEYBOARD_CONTROLLER_STATUS_COMMAND_DATA) |
                                           statusRegister);
}

static void hyperdos_intel_8042_keyboard_controller_request_interrupt_if_needed(
        hyperdos_intel_8042_keyboard_controller* controller)
{
    if (controller->outputQueueCount == 0u)
    {
        return;
    }

    if (controller->outputQueueAuxiliaryDevice[controller->outputQueueReadIndex] != 0u)
    {
        if ((controller->commandByte & HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_AUXILIARY_INTERRUPT_ENABLE) != 0u &&
            controller->auxiliaryDeviceDisabled == 0u &&
            (controller->commandByte & HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_AUXILIARY_DISABLED) == 0u)
        {
            controller->auxiliaryDeviceInterruptRequestPending = 1u;
        }
        return;
    }

    if ((controller->commandByte & HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_INTERRUPT_ENABLE) != 0u &&
        (controller->commandByte & HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_KEYBOARD_DISABLED) == 0u)
    {
        controller->keyboardInterruptRequestPending = 1u;
    }
}

static int hyperdos_intel_8042_keyboard_controller_enqueue_output_byte(
        hyperdos_intel_8042_keyboard_controller* controller,
        uint8_t                                  value,
        uint8_t                                  auxiliaryDevice)
{
    if (controller->outputQueueCount >= HYPERDOS_KEYBOARD_CONTROLLER_OUTPUT_QUEUE_CAPACITY)
    {
        return 0;
    }

    controller->outputQueue[controller->outputQueueWriteIndex]                = value;
    controller->outputQueueAuxiliaryDevice[controller->outputQueueWriteIndex] = auxiliaryDevice != 0u ? 1u : 0u;
    controller->outputQueueWriteIndex = (controller->outputQueueWriteIndex + 1u) %
                                        HYPERDOS_KEYBOARD_CONTROLLER_OUTPUT_QUEUE_CAPACITY;
    ++controller->outputQueueCount;
    hyperdos_intel_8042_keyboard_controller_update_status(controller);
    hyperdos_intel_8042_keyboard_controller_request_interrupt_if_needed(controller);
    return 1;
}

static int hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
        hyperdos_intel_8042_keyboard_controller* controller,
        uint8_t                                  value)
{
    return hyperdos_intel_8042_keyboard_controller_enqueue_output_byte(controller, value, 0u);
}

static int hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_output_byte(
        hyperdos_intel_8042_keyboard_controller* controller,
        uint8_t                                  value)
{
    return hyperdos_intel_8042_keyboard_controller_enqueue_output_byte(controller, value, 1u);
}

static void hyperdos_intel_8042_keyboard_controller_set_auxiliary_defaults(
        hyperdos_intel_8042_keyboard_controller* controller)
{
    controller->auxiliaryDeviceSampleRate       = HYPERDOS_AUXILIARY_MOUSE_DEVICE_DEFAULT_SAMPLE_RATE;
    controller->auxiliaryDeviceResolution       = HYPERDOS_AUXILIARY_MOUSE_DEVICE_DEFAULT_RESOLUTION;
    controller->auxiliaryDeviceScalingTwoToOne  = 0u;
    controller->auxiliaryDeviceReportingEnabled = 0u;
    controller->pendingAuxiliaryDeviceCommand   = 0u;
}

static int16_t hyperdos_intel_8042_keyboard_controller_clamp_auxiliary_mouse_movement(int32_t  movement,
                                                                                      uint8_t* overflow)
{
    if (movement < -256)
    {
        *overflow = 1u;
        return -256;
    }
    if (movement > 255)
    {
        *overflow = 1u;
        return 255;
    }
    *overflow = movement == -256 || movement == 255 ? 1u : 0u;
    return (int16_t)movement;
}

static uint8_t hyperdos_intel_8042_keyboard_controller_get_auxiliary_mouse_status_byte(
        const hyperdos_intel_8042_keyboard_controller* controller)
{
    uint8_t statusByte = 0u;

    if ((controller->auxiliaryDeviceButtonMask & HYPERDOS_AUXILIARY_MOUSE_PACKET_RIGHT_BUTTON) != 0u)
    {
        statusByte |= HYPERDOS_AUXILIARY_MOUSE_DEVICE_STATUS_RIGHT_BUTTON;
    }
    if ((controller->auxiliaryDeviceButtonMask & HYPERDOS_AUXILIARY_MOUSE_PACKET_MIDDLE_BUTTON) != 0u)
    {
        statusByte |= HYPERDOS_AUXILIARY_MOUSE_DEVICE_STATUS_MIDDLE_BUTTON;
    }
    if ((controller->auxiliaryDeviceButtonMask & HYPERDOS_AUXILIARY_MOUSE_PACKET_LEFT_BUTTON) != 0u)
    {
        statusByte |= HYPERDOS_AUXILIARY_MOUSE_DEVICE_STATUS_LEFT_BUTTON;
    }
    if (controller->auxiliaryDeviceScalingTwoToOne != 0u)
    {
        statusByte |= HYPERDOS_AUXILIARY_MOUSE_DEVICE_STATUS_SCALING_TWO_TO_ONE;
    }
    if (controller->auxiliaryDeviceReportingEnabled != 0u)
    {
        statusByte |= HYPERDOS_AUXILIARY_MOUSE_DEVICE_STATUS_REPORTING_ENABLED;
    }
    return statusByte;
}

static int hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_mouse_packet(
        hyperdos_intel_8042_keyboard_controller* controller,
        int32_t                                  horizontalMovement,
        int32_t                                  verticalMovement,
        uint8_t                                  buttonMask)
{
    uint8_t horizontalOverflow = 0u;
    uint8_t verticalOverflow   = 0u;
    uint8_t firstByte          = HYPERDOS_AUXILIARY_MOUSE_PACKET_ALWAYS_ONE;
    int16_t clampedHorizontalMovement =
            hyperdos_intel_8042_keyboard_controller_clamp_auxiliary_mouse_movement(horizontalMovement,
                                                                                   &horizontalOverflow);
    int16_t clampedVerticalMovement =
            hyperdos_intel_8042_keyboard_controller_clamp_auxiliary_mouse_movement(verticalMovement, &verticalOverflow);

    if (controller->outputQueueCount + 3u > HYPERDOS_KEYBOARD_CONTROLLER_OUTPUT_QUEUE_CAPACITY)
    {
        return 0;
    }

    controller->auxiliaryDeviceButtonMask  = buttonMask & (HYPERDOS_AUXILIARY_MOUSE_PACKET_LEFT_BUTTON |
                                                          HYPERDOS_AUXILIARY_MOUSE_PACKET_RIGHT_BUTTON |
                                                          HYPERDOS_AUXILIARY_MOUSE_PACKET_MIDDLE_BUTTON);
    firstByte                             |= controller->auxiliaryDeviceButtonMask;
    if (clampedHorizontalMovement < 0)
    {
        firstByte |= HYPERDOS_AUXILIARY_MOUSE_PACKET_HORIZONTAL_SIGN;
    }
    if (clampedVerticalMovement < 0)
    {
        firstByte |= HYPERDOS_AUXILIARY_MOUSE_PACKET_VERTICAL_SIGN;
    }
    if (horizontalOverflow != 0u)
    {
        firstByte |= HYPERDOS_AUXILIARY_MOUSE_PACKET_HORIZONTAL_OVERFLOW;
    }
    if (verticalOverflow != 0u)
    {
        firstByte |= HYPERDOS_AUXILIARY_MOUSE_PACKET_VERTICAL_OVERFLOW;
    }

    return hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_output_byte(controller, firstByte) &&
           hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_output_byte(controller,
                                                                                 (uint8_t)clampedHorizontalMovement) &&
           hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_output_byte(controller,
                                                                                 (uint8_t)clampedVerticalMovement);
}

void hyperdos_intel_8042_keyboard_controller_initialize(hyperdos_intel_8042_keyboard_controller* controller)
{
    if (controller == NULL)
    {
        return;
    }
    memset(controller, 0, sizeof(*controller));
    controller->commandByte             = HYPERDOS_KEYBOARD_CONTROLLER_DEFAULT_COMMAND_BYTE;
    controller->inputPort               = 0xFFu;
    controller->outputPort              = HYPERDOS_KEYBOARD_CONTROLLER_DEFAULT_OUTPUT_PORT;
    controller->keyboardScanCodeSet     = 1u;
    controller->keyboardScanningEnabled = 1u;
    controller->auxiliaryDeviceDisabled = 1u;
    hyperdos_intel_8042_keyboard_controller_set_auxiliary_defaults(controller);
    hyperdos_intel_8042_keyboard_controller_update_status(controller);
}

uint8_t hyperdos_intel_8042_keyboard_controller_read_byte(void* device, uint16_t port)
{
    hyperdos_intel_8042_keyboard_controller* controller = (hyperdos_intel_8042_keyboard_controller*)device;
    uint8_t                                  value      = HYPERDOS_OPEN_BUS_BYTE;

    if (controller == NULL)
    {
        return HYPERDOS_OPEN_BUS_BYTE;
    }

    if (port == HYPERDOS_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT)
    {
        hyperdos_intel_8042_keyboard_controller_update_status(controller);
        return controller->statusRegister;
    }
    if (port != HYPERDOS_KEYBOARD_CONTROLLER_DATA_PORT)
    {
        return HYPERDOS_OPEN_BUS_BYTE;
    }
    if (controller->outputQueueCount != 0u)
    {
        uint8_t auxiliaryDevice          = controller->outputQueueAuxiliaryDevice[controller->outputQueueReadIndex];
        value                            = controller->outputQueue[controller->outputQueueReadIndex];
        controller->outputQueueReadIndex = (controller->outputQueueReadIndex + 1u) %
                                           HYPERDOS_KEYBOARD_CONTROLLER_OUTPUT_QUEUE_CAPACITY;
        --controller->outputQueueCount;
        controller->latchedOutputByte          = value;
        controller->latchedOutputByteAvailable = 1u;
        if (auxiliaryDevice != 0u)
        {
            controller->auxiliaryDeviceInterruptRequestPending = 0u;
        }
        else
        {
            controller->keyboardInterruptRequestPending = 0u;
        }
    }
    else if (controller->latchedOutputByteAvailable != 0u)
    {
        value = controller->latchedOutputByte;
    }
    controller->statusRegister &= (uint8_t)~HYPERDOS_KEYBOARD_CONTROLLER_STATUS_COMMAND_DATA;
    hyperdos_intel_8042_keyboard_controller_update_status(controller);
    hyperdos_intel_8042_keyboard_controller_request_interrupt_if_needed(controller);
    return value;
}

static void hyperdos_intel_8042_keyboard_controller_write_keyboard_command(
        hyperdos_intel_8042_keyboard_controller* controller,
        uint8_t                                  value)
{
    if (controller->pendingKeyboardCommand == HYPERDOS_KEYBOARD_DEVICE_COMMAND_SET_LEDS ||
        controller->pendingKeyboardCommand == HYPERDOS_KEYBOARD_DEVICE_COMMAND_SET_TYPEMATIC)
    {
        (void)value;
        controller->pendingKeyboardCommand = 0u;
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
                controller,
                HYPERDOS_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
        return;
    }
    if (controller->pendingKeyboardCommand == HYPERDOS_KEYBOARD_DEVICE_COMMAND_SET_SCAN_CODE_SET)
    {
        if (value != 0u)
        {
            controller->keyboardScanCodeSet = value;
        }
        controller->pendingKeyboardCommand = 0u;
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
                controller,
                HYPERDOS_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
        if (value == 0u)
        {
            (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(controller,
                                                                                       controller->keyboardScanCodeSet);
        }
        return;
    }

    switch (value)
    {
    case HYPERDOS_KEYBOARD_DEVICE_COMMAND_SET_LEDS:
    case HYPERDOS_KEYBOARD_DEVICE_COMMAND_SET_TYPEMATIC:
        controller->pendingKeyboardCommand = value;
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
                controller,
                HYPERDOS_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
        break;
    case HYPERDOS_KEYBOARD_DEVICE_COMMAND_SET_SCAN_CODE_SET:
        controller->pendingKeyboardCommand = value;
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
                controller,
                HYPERDOS_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
        break;
    case HYPERDOS_KEYBOARD_DEVICE_COMMAND_ECHO:
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(controller, value);
        break;
    case HYPERDOS_KEYBOARD_DEVICE_COMMAND_IDENTIFY:
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
                controller,
                HYPERDOS_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
                controller,
                HYPERDOS_KEYBOARD_DEVICE_IDENTIFY_FIRST_BYTE);
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
                controller,
                HYPERDOS_KEYBOARD_DEVICE_IDENTIFY_SECOND_BYTE);
        break;
    case HYPERDOS_KEYBOARD_DEVICE_COMMAND_ENABLE_SCANNING:
        controller->keyboardScanningEnabled = 1u;
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
                controller,
                HYPERDOS_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
        break;
    case HYPERDOS_KEYBOARD_DEVICE_COMMAND_DISABLE_SCANNING:
        controller->keyboardScanningEnabled = 0u;
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
                controller,
                HYPERDOS_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
        break;
    case HYPERDOS_KEYBOARD_DEVICE_COMMAND_SET_DEFAULTS:
        controller->keyboardScanCodeSet     = 1u;
        controller->keyboardScanningEnabled = 1u;
        controller->pendingKeyboardCommand  = 0u;
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
                controller,
                HYPERDOS_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
        break;
    case HYPERDOS_KEYBOARD_DEVICE_COMMAND_RESEND:
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
                controller,
                HYPERDOS_KEYBOARD_DEVICE_RESPONSE_RESEND);
        break;
    case HYPERDOS_KEYBOARD_DEVICE_COMMAND_RESET:
        controller->keyboardScanningEnabled = 1u;
        controller->pendingKeyboardCommand  = 0u;
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
                controller,
                HYPERDOS_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
                controller,
                HYPERDOS_KEYBOARD_DEVICE_RESPONSE_SELF_TEST_PASSED);
        break;
    default:
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
                controller,
                HYPERDOS_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
        break;
    }
}

static void hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_acknowledge(
        hyperdos_intel_8042_keyboard_controller* controller)
{
    (void)hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_output_byte(
            controller,
            HYPERDOS_KEYBOARD_DEVICE_RESPONSE_ACKNOWLEDGE);
}

static void hyperdos_intel_8042_keyboard_controller_write_auxiliary_device_command(
        hyperdos_intel_8042_keyboard_controller* controller,
        uint8_t                                  value)
{
    if (controller->pendingAuxiliaryDeviceCommand == HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_SAMPLE_RATE)
    {
        controller->auxiliaryDeviceSampleRate     = value;
        controller->pendingAuxiliaryDeviceCommand = 0u;
        hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_acknowledge(controller);
        return;
    }
    if (controller->pendingAuxiliaryDeviceCommand == HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_RESOLUTION)
    {
        controller->auxiliaryDeviceResolution     = (uint8_t)(value & HYPERDOS_X86_LOW_TWO_BITS_MASK);
        controller->pendingAuxiliaryDeviceCommand = 0u;
        hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_acknowledge(controller);
        return;
    }

    switch (value)
    {
    case HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_SCALING_ONE_TO_ONE:
        controller->auxiliaryDeviceScalingTwoToOne = 0u;
        hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_acknowledge(controller);
        break;
    case HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_SCALING_TWO_TO_ONE:
        controller->auxiliaryDeviceScalingTwoToOne = 1u;
        hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_acknowledge(controller);
        break;
    case HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_RESOLUTION:
    case HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_SAMPLE_RATE:
        controller->pendingAuxiliaryDeviceCommand = value;
        hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_acknowledge(controller);
        break;
    case HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_STATUS_REQUEST:
    {
        uint8_t statusByte = hyperdos_intel_8042_keyboard_controller_get_auxiliary_mouse_status_byte(controller);
        hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_acknowledge(controller);
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_output_byte(controller, statusByte);
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_output_byte(
                controller,
                controller->auxiliaryDeviceResolution);
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_output_byte(
                controller,
                controller->auxiliaryDeviceSampleRate);
        break;
    }
    case HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_STREAM_MODE:
    case HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_RESET_WRAP_MODE:
        hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_acknowledge(controller);
        break;
    case HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_READ_DATA:
        hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_acknowledge(controller);
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_mouse_packet(
                controller,
                0,
                0,
                controller->auxiliaryDeviceButtonMask);
        break;
    case HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_WRAP_MODE:
    case HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_REMOTE_MODE:
        controller->auxiliaryDeviceReportingEnabled = 0u;
        hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_acknowledge(controller);
        break;
    case HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_IDENTIFY:
        hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_acknowledge(controller);
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_output_byte(
                controller,
                HYPERDOS_AUXILIARY_MOUSE_DEVICE_IDENTIFIER_STANDARD);
        break;
    case HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_ENABLE_REPORTING:
        controller->auxiliaryDeviceReportingEnabled = 1u;
        hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_acknowledge(controller);
        break;
    case HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_DISABLE_REPORTING:
        controller->auxiliaryDeviceReportingEnabled = 0u;
        hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_acknowledge(controller);
        break;
    case HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_SET_DEFAULTS:
        hyperdos_intel_8042_keyboard_controller_set_auxiliary_defaults(controller);
        hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_acknowledge(controller);
        break;
    case HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_RESEND:
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_output_byte(
                controller,
                HYPERDOS_KEYBOARD_DEVICE_RESPONSE_RESEND);
        break;
    case HYPERDOS_AUXILIARY_MOUSE_DEVICE_COMMAND_RESET:
        hyperdos_intel_8042_keyboard_controller_set_auxiliary_defaults(controller);
        hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_acknowledge(controller);
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_output_byte(
                controller,
                HYPERDOS_KEYBOARD_DEVICE_RESPONSE_SELF_TEST_PASSED);
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_output_byte(
                controller,
                HYPERDOS_AUXILIARY_MOUSE_DEVICE_IDENTIFIER_STANDARD);
        break;
    default:
        hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_acknowledge(controller);
        break;
    }
}

void hyperdos_intel_8042_keyboard_controller_write_byte(void* device, uint16_t port, uint8_t value)
{
    hyperdos_intel_8042_keyboard_controller* controller = (hyperdos_intel_8042_keyboard_controller*)device;

    if (controller == NULL)
    {
        return;
    }

    if (port == HYPERDOS_KEYBOARD_CONTROLLER_STATUS_COMMAND_PORT)
    {
        controller->statusRegister           |= HYPERDOS_KEYBOARD_CONTROLLER_STATUS_COMMAND_DATA;
        controller->pendingControllerCommand  = 0u;
        switch (value)
        {
        case HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_READ_COMMAND_BYTE:
            (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(controller,
                                                                                       controller->commandByte);
            break;
        case HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_WRITE_COMMAND_BYTE:
        case HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_WRITE_OUTPUT_PORT:
        case HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_WRITE_KEYBOARD_OUTPUT:
        case HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_WRITE_AUXILIARY_OUTPUT:
        case HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_WRITE_AUXILIARY_DEVICE:
            controller->pendingControllerCommand = value;
            break;
        case HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_SELF_TEST:
            controller->commandByte |= HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_SYSTEM_FLAG;
            (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
                    controller,
                    HYPERDOS_KEYBOARD_DEVICE_RESPONSE_CONTROLLER_TEST_PASSED);
            break;
        case HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_TEST_KEYBOARD_INTERFACE:
            (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
                    controller,
                    HYPERDOS_KEYBOARD_DEVICE_RESPONSE_INTERFACE_TEST_PASSED);
            break;
        case HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_DISABLE_AUXILIARY:
            controller->auxiliaryDeviceDisabled  = 1u;
            controller->commandByte             |= HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_AUXILIARY_DISABLED;
            break;
        case HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_ENABLE_AUXILIARY:
            controller->auxiliaryDeviceDisabled = 0u;
            controller->commandByte &= (uint8_t)~HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_AUXILIARY_DISABLED;
            hyperdos_intel_8042_keyboard_controller_request_interrupt_if_needed(controller);
            break;
        case HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_TEST_AUXILIARY:
            (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(
                    controller,
                    HYPERDOS_KEYBOARD_DEVICE_RESPONSE_INTERFACE_TEST_PASSED);
            break;
        case HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_DISABLE_KEYBOARD:
            controller->commandByte |= HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_KEYBOARD_DISABLED;
            break;
        case HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_ENABLE_KEYBOARD:
            controller->commandByte &= (uint8_t)~HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_KEYBOARD_DISABLED;
            hyperdos_intel_8042_keyboard_controller_request_interrupt_if_needed(controller);
            break;
        case HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_READ_INPUT_PORT:
            (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(controller,
                                                                                       controller->inputPort);
            break;
        case HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_READ_OUTPUT_PORT:
            (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(controller,
                                                                                       controller->outputPort);
            break;
        default:
            break;
        }
        hyperdos_intel_8042_keyboard_controller_update_status(controller);
        return;
    }

    if (port != HYPERDOS_KEYBOARD_CONTROLLER_DATA_PORT)
    {
        return;
    }
    if (controller->pendingControllerCommand == HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_WRITE_COMMAND_BYTE)
    {
        controller->commandByte             = value;
        controller->auxiliaryDeviceDisabled = (value & HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_AUXILIARY_DISABLED) !=
                                              0u;
        controller->pendingControllerCommand = 0u;
        hyperdos_intel_8042_keyboard_controller_update_status(controller);
        hyperdos_intel_8042_keyboard_controller_request_interrupt_if_needed(controller);
        return;
    }
    if (controller->pendingControllerCommand == HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_WRITE_OUTPUT_PORT)
    {
        controller->outputPort               = value;
        controller->pendingControllerCommand = 0u;
        hyperdos_intel_8042_keyboard_controller_update_status(controller);
        return;
    }
    if (controller->pendingControllerCommand == HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_WRITE_KEYBOARD_OUTPUT)
    {
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(controller, value);
        controller->pendingControllerCommand = 0u;
        hyperdos_intel_8042_keyboard_controller_update_status(controller);
        return;
    }
    if (controller->pendingControllerCommand == HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_WRITE_AUXILIARY_OUTPUT)
    {
        (void)hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_output_byte(controller, value);
        controller->pendingControllerCommand = 0u;
        hyperdos_intel_8042_keyboard_controller_update_status(controller);
        return;
    }
    if (controller->pendingControllerCommand == HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_WRITE_AUXILIARY_DEVICE)
    {
        controller->pendingControllerCommand = 0u;
        hyperdos_intel_8042_keyboard_controller_write_auxiliary_device_command(controller, value);
        hyperdos_intel_8042_keyboard_controller_update_status(controller);
        return;
    }

    hyperdos_intel_8042_keyboard_controller_write_keyboard_command(controller, value);
    hyperdos_intel_8042_keyboard_controller_update_status(controller);
}

int hyperdos_intel_8042_keyboard_controller_receive_scan_code(hyperdos_intel_8042_keyboard_controller* controller,
                                                              uint8_t                                  scanCode)
{
    if (controller == NULL || controller->keyboardScanningEnabled == 0u ||
        (controller->commandByte & HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_KEYBOARD_DISABLED) != 0u ||
        controller->outputQueueCount >= HYPERDOS_KEYBOARD_CONTROLLER_OUTPUT_QUEUE_CAPACITY)
    {
        return 0;
    }
    return hyperdos_intel_8042_keyboard_controller_enqueue_keyboard_output_byte(controller, scanCode);
}

int hyperdos_intel_8042_keyboard_controller_can_receive_scan_code(
        const hyperdos_intel_8042_keyboard_controller* controller)
{
    return controller != NULL && controller->keyboardScanningEnabled != 0u &&
           (controller->commandByte & HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_KEYBOARD_DISABLED) == 0u &&
           controller->outputQueueCount < HYPERDOS_KEYBOARD_CONTROLLER_OUTPUT_QUEUE_CAPACITY;
}

int hyperdos_intel_8042_keyboard_controller_output_queue_is_full(
        const hyperdos_intel_8042_keyboard_controller* controller)
{
    return controller != NULL && controller->outputQueueCount >= HYPERDOS_KEYBOARD_CONTROLLER_OUTPUT_QUEUE_CAPACITY;
}

int hyperdos_intel_8042_keyboard_controller_receive_auxiliary_mouse_packet(
        hyperdos_intel_8042_keyboard_controller* controller,
        int16_t                                  horizontalMovement,
        int16_t                                  verticalMovement,
        uint8_t                                  buttonMask)
{
    uint8_t previousButtonMask   = 0u;
    uint8_t normalizedButtonMask = 0u;
    uint8_t buttonChanged        = 0u;

    if (controller == NULL)
    {
        return 0;
    }
    normalizedButtonMask = buttonMask &
                           (HYPERDOS_AUXILIARY_MOUSE_PACKET_LEFT_BUTTON | HYPERDOS_AUXILIARY_MOUSE_PACKET_RIGHT_BUTTON |
                            HYPERDOS_AUXILIARY_MOUSE_PACKET_MIDDLE_BUTTON);
    previousButtonMask                    = controller->auxiliaryDeviceButtonMask;
    buttonChanged                         = previousButtonMask != normalizedButtonMask ? 1u : 0u;
    controller->auxiliaryDeviceButtonMask = normalizedButtonMask;
    if (controller->auxiliaryDeviceDisabled != 0u ||
        (controller->commandByte & HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_AUXILIARY_DISABLED) != 0u ||
        controller->auxiliaryDeviceReportingEnabled == 0u)
    {
        return 1;
    }
    if (controller->outputQueueCount != 0u)
    {
        return 0;
    }

    if (horizontalMovement == 0 && verticalMovement == 0 && buttonChanged == 0u)
    {
        return 1;
    }
    return hyperdos_intel_8042_keyboard_controller_enqueue_auxiliary_mouse_packet(controller,
                                                                                  horizontalMovement,
                                                                                  verticalMovement,
                                                                                  normalizedButtonMask);
}

void hyperdos_intel_8042_keyboard_controller_set_auxiliary_mouse_reporting_enabled(
        hyperdos_intel_8042_keyboard_controller* controller,
        uint8_t                                  enabled)
{
    if (controller == NULL)
    {
        return;
    }
    controller->auxiliaryDeviceReportingEnabled = enabled != 0u ? 1u : 0u;
    if (enabled != 0u)
    {
        controller->auxiliaryDeviceDisabled  = 0u;
        controller->commandByte             &= (uint8_t)~HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_AUXILIARY_DISABLED;
        controller->commandByte             |= HYPERDOS_KEYBOARD_CONTROLLER_COMMAND_BYTE_AUXILIARY_INTERRUPT_ENABLE;
        hyperdos_intel_8042_keyboard_controller_request_interrupt_if_needed(controller);
    }
}

int hyperdos_intel_8042_keyboard_controller_has_interrupt_request(
        const hyperdos_intel_8042_keyboard_controller* controller)
{
    return controller != NULL && controller->keyboardInterruptRequestPending != 0u;
}

int hyperdos_intel_8042_keyboard_controller_has_auxiliary_device_interrupt_request(
        const hyperdos_intel_8042_keyboard_controller* controller)
{
    return controller != NULL && controller->auxiliaryDeviceInterruptRequestPending != 0u;
}

void hyperdos_intel_8042_keyboard_controller_clear_interrupt_request(
        hyperdos_intel_8042_keyboard_controller* controller)
{
    if (controller != NULL)
    {
        controller->keyboardInterruptRequestPending = 0u;
    }
}

void hyperdos_intel_8042_keyboard_controller_clear_auxiliary_device_interrupt_request(
        hyperdos_intel_8042_keyboard_controller* controller)
{
    if (controller != NULL)
    {
        controller->auxiliaryDeviceInterruptRequestPending = 0u;
    }
}

void hyperdos_universal_asynchronous_receiver_transmitter_initialize(
        hyperdos_universal_asynchronous_receiver_transmitter* transmitter)
{
    if (transmitter == NULL)
    {
        return;
    }
    memset(transmitter, 0, sizeof(*transmitter));
    transmitter->lineStatus              = HYPERDOS_UART_LINE_STATUS_TRANSMITTER_EMPTY;
    transmitter->interruptIdentification = 1u;
}

uint8_t hyperdos_universal_asynchronous_receiver_transmitter_read_byte(void* device, uint16_t port)
{
    hyperdos_universal_asynchronous_receiver_transmitter*
             transmitter = (hyperdos_universal_asynchronous_receiver_transmitter*)device;
    uint16_t portOffset  = (uint16_t)(port & HYPERDOS_UART_PORT_OFFSET_MASK);

    if (transmitter == NULL)
    {
        return HYPERDOS_OPEN_BUS_BYTE;
    }
    if ((transmitter->lineControl & HYPERDOS_UART_DIVISOR_LATCH_ACCESS_BIT) != 0u)
    {
        if (portOffset == HYPERDOS_UART_RECEIVER_BUFFER_OFFSET)
        {
            return transmitter->divisorLowByte;
        }
        if (portOffset == HYPERDOS_UART_INTERRUPT_ENABLE_OFFSET)
        {
            return transmitter->divisorHighByte;
        }
    }

    switch (portOffset)
    {
    case HYPERDOS_UART_RECEIVER_BUFFER_OFFSET:
        return transmitter->receiverBuffer;
    case HYPERDOS_UART_INTERRUPT_ENABLE_OFFSET:
        return transmitter->interruptEnable;
    case HYPERDOS_UART_INTERRUPT_IDENTIFICATION_OFFSET:
        return transmitter->interruptIdentification;
    case HYPERDOS_UART_LINE_CONTROL_OFFSET:
        return transmitter->lineControl;
    case HYPERDOS_UART_MODEM_CONTROL_OFFSET:
        return transmitter->modemControl;
    case HYPERDOS_UART_LINE_STATUS_OFFSET:
        return transmitter->lineStatus;
    case HYPERDOS_UART_MODEM_STATUS_OFFSET:
        return transmitter->modemStatus;
    }
    return HYPERDOS_OPEN_BUS_BYTE;
}

void hyperdos_universal_asynchronous_receiver_transmitter_write_byte(void* device, uint16_t port, uint8_t value)
{
    hyperdos_universal_asynchronous_receiver_transmitter*
             transmitter = (hyperdos_universal_asynchronous_receiver_transmitter*)device;
    uint16_t portOffset  = (uint16_t)(port & HYPERDOS_UART_PORT_OFFSET_MASK);

    if (transmitter == NULL)
    {
        return;
    }
    if ((transmitter->lineControl & HYPERDOS_UART_DIVISOR_LATCH_ACCESS_BIT) != 0u)
    {
        if (portOffset == HYPERDOS_UART_RECEIVER_BUFFER_OFFSET)
        {
            transmitter->divisorLowByte = value;
            return;
        }
        if (portOffset == HYPERDOS_UART_INTERRUPT_ENABLE_OFFSET)
        {
            transmitter->divisorHighByte = value;
            return;
        }
    }

    switch (portOffset)
    {
    case HYPERDOS_UART_RECEIVER_BUFFER_OFFSET:
        transmitter->transmitterHolding  = value;
        transmitter->lineStatus         |= HYPERDOS_UART_LINE_STATUS_TRANSMITTER_EMPTY;
        break;
    case HYPERDOS_UART_INTERRUPT_ENABLE_OFFSET:
        transmitter->interruptEnable = value;
        break;
    case HYPERDOS_UART_LINE_CONTROL_OFFSET:
        transmitter->lineControl = value;
        break;
    case HYPERDOS_UART_MODEM_CONTROL_OFFSET:
        transmitter->modemControl = value;
        break;
    }
}

static uint8_t hyperdos_video_graphics_array_rotate_right(uint8_t value, uint8_t rotationCount)
{
    rotationCount &= HYPERDOS_VIDEO_GRAPHICS_ARRAY_DATA_ROTATE_COUNT_MASK;
    if (rotationCount == 0u)
    {
        return value;
    }
    return (uint8_t)((value >> rotationCount) | (value << (HYPERDOS_X86_BYTE_BIT_COUNT - rotationCount)));
}

static void hyperdos_video_graphics_array_load_latch(hyperdos_color_graphics_adapter* adapter, uint32_t relativeAddress)
{
    size_t planeIndex = 0;

    for (planeIndex = 0; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
    {
        adapter->graphicsLatch[planeIndex] = adapter->graphicsPlanes[planeIndex][relativeAddress];
    }
}

static uint8_t hyperdos_video_graphics_array_apply_logical_operation(uint8_t sourceByte,
                                                                     uint8_t latchByte,
                                                                     uint8_t dataRotateRegister)
{
    uint8_t logicalOperation = (uint8_t)((dataRotateRegister >> HYPERDOS_VIDEO_GRAPHICS_ARRAY_LOGICAL_OPERATION_SHIFT) &
                                         HYPERDOS_VIDEO_GRAPHICS_ARRAY_LOGICAL_OPERATION_MASK);

    switch (logicalOperation)
    {
    case HYPERDOS_VIDEO_GRAPHICS_ARRAY_LOGICAL_OPERATION_AND:
        return (uint8_t)(sourceByte & latchByte);
    case HYPERDOS_VIDEO_GRAPHICS_ARRAY_LOGICAL_OPERATION_OR:
        return (uint8_t)(sourceByte | latchByte);
    case HYPERDOS_VIDEO_GRAPHICS_ARRAY_LOGICAL_OPERATION_XOR:
        return (uint8_t)(sourceByte ^ latchByte);
    case HYPERDOS_VIDEO_GRAPHICS_ARRAY_LOGICAL_OPERATION_REPLACE:
    default:
        return sourceByte;
    }
}

static uint8_t hyperdos_video_graphics_array_merge_with_bit_mask(uint8_t sourceByte, uint8_t latchByte, uint8_t bitMask)
{
    return (uint8_t)((sourceByte & bitMask) | (latchByte & (uint8_t)~bitMask));
}

static uint8_t hyperdos_video_graphics_array_expand_plane_bit(uint8_t value, size_t planeIndex)
{
    return (value & (uint8_t)(1u << planeIndex)) != 0u ? 0xFFu : 0x00u;
}

static uint8_t hyperdos_video_graphics_array_read_memory_byte(hyperdos_color_graphics_adapter* adapter,
                                                              uint32_t                         relativeAddress)
{
    uint8_t graphicsModeRegister = adapter->graphicsControllerRegisters
                                           [HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MODE_INDEX];
    uint8_t readMapSelect = (uint8_t)(adapter->graphicsControllerRegisters
                                              [HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_READ_MAP_SELECT_INDEX] &
                                      HYPERDOS_X86_LOW_TWO_BITS_MASK);

    hyperdos_video_graphics_array_load_latch(adapter, relativeAddress);
    if ((graphicsModeRegister & HYPERDOS_VIDEO_GRAPHICS_ARRAY_READ_MODE_BIT) == 0u)
    {
        return adapter->graphicsPlanes[readMapSelect][relativeAddress];
    }
    else
    {
        uint8_t colorCompare = adapter->graphicsControllerRegisters
                                       [HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_COLOR_COMPARE_INDEX];
        uint8_t colorDontCare = adapter->graphicsControllerRegisters
                                        [HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_COLOR_DONT_CARE_INDEX];
        uint8_t result     = 0xFFu;
        size_t  planeIndex = 0;

        for (planeIndex = 0; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
        {
            uint8_t planeBit = (uint8_t)(1u << planeIndex);
            if ((colorDontCare & planeBit) != 0u)
            {
                uint8_t compareByte = (colorCompare & planeBit) != 0u ? 0xFFu : 0x00u;
                result              = (uint8_t)(result &
                                   (uint8_t)~(adapter->graphicsPlanes[planeIndex][relativeAddress] ^ compareByte));
            }
        }
        return result;
    }
}

static void hyperdos_video_graphics_array_write_memory_byte(hyperdos_color_graphics_adapter* adapter,
                                                            uint32_t                         relativeAddress,
                                                            uint8_t                          value)
{
    uint8_t graphicsModeRegister = adapter->graphicsControllerRegisters
                                           [HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MODE_INDEX];
    uint8_t writeMode = (uint8_t)(graphicsModeRegister & HYPERDOS_VIDEO_GRAPHICS_ARRAY_WRITE_MODE_MASK);
    uint8_t mapMask   = (uint8_t)(adapter->sequencerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MAP_MASK_INDEX] &
                                ((1u << HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT) - 1u));
    uint8_t setResetRegister = adapter->graphicsControllerRegisters
                                       [HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_SET_RESET_INDEX];
    uint8_t enableSetResetRegister = adapter->graphicsControllerRegisters
                                             [HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_ENABLE_SET_RESET_INDEX];
    uint8_t dataRotateRegister = adapter->graphicsControllerRegisters
                                         [HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_DATA_ROTATE_INDEX];
    uint8_t bitMaskRegister = adapter->graphicsControllerRegisters
                                      [HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_BIT_MASK_INDEX];
    uint8_t rotatedValue = hyperdos_video_graphics_array_rotate_right(value, dataRotateRegister);
    size_t  planeIndex   = 0;

    for (planeIndex = 0; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
    {
        uint8_t planeBit    = (uint8_t)(1u << planeIndex);
        uint8_t sourceByte  = rotatedValue;
        uint8_t latchByte   = adapter->graphicsLatch[planeIndex];
        uint8_t writtenByte = latchByte;

        if ((mapMask & planeBit) == 0u)
        {
            continue;
        }

        if (writeMode == 0u)
        {
            if ((enableSetResetRegister & planeBit) != 0u)
            {
                sourceByte = hyperdos_video_graphics_array_expand_plane_bit(setResetRegister, planeIndex);
            }
            sourceByte  = hyperdos_video_graphics_array_apply_logical_operation(sourceByte,
                                                                               latchByte,
                                                                               dataRotateRegister);
            writtenByte = hyperdos_video_graphics_array_merge_with_bit_mask(sourceByte, latchByte, bitMaskRegister);
        }
        else if (writeMode == 1u)
        {
            writtenByte = latchByte;
        }
        else if (writeMode == 2u)
        {
            sourceByte  = hyperdos_video_graphics_array_expand_plane_bit(value, planeIndex);
            sourceByte  = hyperdos_video_graphics_array_apply_logical_operation(sourceByte,
                                                                               latchByte,
                                                                               dataRotateRegister);
            writtenByte = hyperdos_video_graphics_array_merge_with_bit_mask(sourceByte, latchByte, bitMaskRegister);
        }
        else
        {
            uint8_t modeThreeBitMask = (uint8_t)(rotatedValue & bitMaskRegister);
            sourceByte               = hyperdos_video_graphics_array_expand_plane_bit(setResetRegister, planeIndex);
            sourceByte               = hyperdos_video_graphics_array_apply_logical_operation(sourceByte,
                                                                               latchByte,
                                                                               dataRotateRegister);
            writtenByte = hyperdos_video_graphics_array_merge_with_bit_mask(sourceByte, latchByte, modeThreeBitMask);
        }
        adapter->graphicsPlanes[planeIndex][relativeAddress] = writtenByte;
    }
}

static uint8_t hyperdos_video_graphics_array_read_digital_to_analog_converter_data(
        hyperdos_color_graphics_adapter* adapter)
{
    uint8_t value = adapter->digitalToAnalogConverterPalette[adapter->digitalToAnalogConverterReadIndexRegister]
                                                            [adapter->digitalToAnalogConverterReadComponentIndex];

    ++adapter->digitalToAnalogConverterReadComponentIndex;
    if (adapter->digitalToAnalogConverterReadComponentIndex >=
        HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_CONVERTER_COMPONENT_COUNT)
    {
        adapter->digitalToAnalogConverterReadComponentIndex = 0u;
        ++adapter->digitalToAnalogConverterReadIndexRegister;
    }
    return value;
}

static void hyperdos_video_graphics_array_write_digital_to_analog_converter_data(
        hyperdos_color_graphics_adapter* adapter,
        uint8_t                          value)
{
    adapter->digitalToAnalogConverterPalette[adapter->digitalToAnalogConverterWriteIndexRegister]
                                            [adapter->digitalToAnalogConverterWriteComponentIndex] = (uint8_t)(value &
                                                                                                               0x3Fu);
    ++adapter->digitalToAnalogConverterWriteComponentIndex;
    if (adapter->digitalToAnalogConverterWriteComponentIndex >=
        HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_CONVERTER_COMPONENT_COUNT)
    {
        adapter->digitalToAnalogConverterWriteComponentIndex = 0u;
        ++adapter->digitalToAnalogConverterWriteIndexRegister;
    }
}

static uint8_t hyperdos_video_graphics_array_get_memory_map_select(const hyperdos_color_graphics_adapter* adapter)
{
    return (uint8_t)((adapter->graphicsControllerRegisters
                              [HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MISCELLANEOUS_INDEX] >>
                      HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_MAP_SELECT_SHIFT) &
                     HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_MAP_SELECT_MASK);
}

static int hyperdos_x86_unsigned_address_is_in_range(uint32_t address, uint32_t firstAddress, uint32_t byteCount)
{
    return address >= firstAddress && address < firstAddress + byteCount;
}

static int hyperdos_video_graphics_array_translate_memory_address(const hyperdos_color_graphics_adapter* adapter,
                                                                  uint32_t  physicalAddress,
                                                                  uint32_t* relativeAddress)
{
    uint8_t  memoryMapSelect = hyperdos_video_graphics_array_get_memory_map_select(adapter);
    uint32_t firstAddress    = HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_ADDRESS;
    uint32_t byteCount       = HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_SIZE;

    switch (memoryMapSelect)
    {
    case HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_MAP_A0000_128K:
        firstAddress = HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_APERTURE_ADDRESS;
        byteCount    = HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_APERTURE_SIZE;
        break;
    case HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_MAP_A0000_64K:
        firstAddress = HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_ADDRESS;
        byteCount    = HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_SIZE;
        break;
    case HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_MAP_B0000_32K:
        firstAddress = 0xB0000u;
        byteCount    = 0x8000u;
        break;
    case HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_MAP_B8000_32K:
        firstAddress = HYPERDOS_COLOR_GRAPHICS_ADAPTER_MEMORY_ADDRESS;
        byteCount    = 0x8000u;
        break;
    }

    if (!hyperdos_x86_unsigned_address_is_in_range(physicalAddress, firstAddress, byteCount))
    {
        return 0;
    }

    *relativeAddress = (physicalAddress - firstAddress) & (HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_SIZE - 1u);
    return 1;
}

static int hyperdos_video_graphics_array_chain_four_is_enabled(const hyperdos_color_graphics_adapter* adapter)
{
    return (adapter->sequencerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MEMORY_MODE_INDEX] &
            HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_CHAIN_FOUR_BIT) != 0u;
}

static uint8_t hyperdos_video_graphics_array_read_chain_four_memory_byte(hyperdos_color_graphics_adapter* adapter,
                                                                         uint32_t relativeAddress)
{
    size_t   planeIndex   = relativeAddress & HYPERDOS_X86_LOW_TWO_BITS_MASK;
    uint32_t planeAddress = (relativeAddress >> HYPERDOS_X86_TWO_BIT_COUNT) &
                            (HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_SIZE - 1u);

    hyperdos_video_graphics_array_load_latch(adapter, planeAddress);
    return adapter->graphicsPlanes[planeIndex][planeAddress];
}

static void hyperdos_video_graphics_array_write_chain_four_memory_byte(hyperdos_color_graphics_adapter* adapter,
                                                                       uint32_t                         relativeAddress,
                                                                       uint8_t                          value)
{
    size_t   planeIndex   = relativeAddress & HYPERDOS_X86_LOW_TWO_BITS_MASK;
    uint32_t planeAddress = (relativeAddress >> HYPERDOS_X86_TWO_BIT_COUNT) &
                            (HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_SIZE - 1u);
    uint8_t mapMask = (uint8_t)(adapter->sequencerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MAP_MASK_INDEX] &
                                ((1u << HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT) - 1u));

    if ((mapMask & (uint8_t)(1u << planeIndex)) != 0u)
    {
        adapter->graphicsPlanes[planeIndex][planeAddress] = value;
    }
}

static void hyperdos_video_graphics_array_set_memory_map(hyperdos_color_graphics_adapter* adapter,
                                                         uint8_t                          memoryMapValue)
{
    adapter->graphicsControllerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MISCELLANEOUS_INDEX] = memoryMapValue;
}

static void hyperdos_video_graphics_array_set_display_start_address(hyperdos_color_graphics_adapter* adapter,
                                                                    uint16_t displayStartAddress)
{
    adapter->registers
            [HYPERDOS_VIDEO_GRAPHICS_ARRAY_CRT_START_ADDRESS_HIGH_INDEX]          = (uint8_t)(displayStartAddress >>
                                                                                     HYPERDOS_X86_BYTE_BIT_COUNT);
    adapter->registers[HYPERDOS_VIDEO_GRAPHICS_ARRAY_CRT_START_ADDRESS_LOW_INDEX] = (uint8_t)(displayStartAddress &
                                                                                              HYPERDOS_X86_BYTE_MASK);
}

static void hyperdos_video_graphics_array_set_display_stride_byte_count(hyperdos_color_graphics_adapter* adapter,
                                                                        uint16_t displayStrideByteCount)
{
    adapter->registers[HYPERDOS_VIDEO_GRAPHICS_ARRAY_CRT_OFFSET_INDEX] = (uint8_t)(displayStrideByteCount /
                                                                                   HYPERDOS_X86_TWO_BIT_COUNT);
}

static void hyperdos_video_graphics_array_set_attribute_mode(hyperdos_color_graphics_adapter* adapter, int graphicsMode)
{
    if (graphicsMode)
    {
        adapter->attributeControllerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_MODE_CONTROL_INDEX] |=
                HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_GRAPHICS_ENABLE_BIT;
    }
    else
    {
        adapter->attributeControllerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_MODE_CONTROL_INDEX] &=
                (uint8_t)~HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_GRAPHICS_ENABLE_BIT;
    }
}

static void hyperdos_video_graphics_array_initialize_default_palette(hyperdos_color_graphics_adapter* adapter)
{
    static const uint8_t attributePalette[16] = {
        0x00u,
        0x01u,
        0x02u,
        0x03u,
        0x04u,
        0x05u,
        0x14u,
        0x07u,
        0x38u,
        0x39u,
        0x3Au,
        0x3Bu,
        0x3Cu,
        0x3Du,
        0x3Eu,
        0x3Fu,
    };
    size_t colorIndex = 0u;

    for (colorIndex = 0u; colorIndex < 64u; ++colorIndex)
    {
        uint8_t blue  = (uint8_t)(((colorIndex & 0x01u) != 0u ? 42u : 0u) + ((colorIndex & 0x08u) != 0u ? 21u : 0u));
        uint8_t green = (uint8_t)(((colorIndex & 0x02u) != 0u ? 42u : 0u) + ((colorIndex & 0x10u) != 0u ? 21u : 0u));
        uint8_t red   = (uint8_t)(((colorIndex & 0x04u) != 0u ? 42u : 0u) + ((colorIndex & 0x20u) != 0u ? 21u : 0u));

        adapter->digitalToAnalogConverterPalette[colorIndex][0] = red;
        adapter->digitalToAnalogConverterPalette[colorIndex][1] = green;
        adapter->digitalToAnalogConverterPalette[colorIndex][2] = blue;
    }
    for (colorIndex = 0u; colorIndex < 16u; ++colorIndex)
    {
        adapter->attributeControllerRegisters[colorIndex] = attributePalette[colorIndex];
    }
    adapter->attributeControllerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_COLOR_PLANE_ENABLE_INDEX] = 0x0Fu;
    adapter->attributeControllerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_COLOR_SELECT_INDEX]       = 0u;
}

void hyperdos_color_graphics_adapter_initialize(hyperdos_color_graphics_adapter* adapter)
{
    size_t cellIndex  = 0;
    size_t colorIndex = 0;

    if (adapter == NULL)
    {
        return;
    }
    memset(adapter, 0, sizeof(*adapter));
    for (cellIndex = 0;
         cellIndex < HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT * HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT;
         ++cellIndex)
    {
        adapter->memory[cellIndex * HYPERDOS_COLOR_GRAPHICS_ADAPTER_CELL_BYTE_COUNT]      = ' ';
        adapter->memory[cellIndex * HYPERDOS_COLOR_GRAPHICS_ADAPTER_CELL_BYTE_COUNT + 1u] = 0x07u;
    }
    adapter->miscellaneousOutputRegister = HYPERDOS_VIDEO_GRAPHICS_ARRAY_DEFAULT_MISCELLANEOUS_OUTPUT;
    adapter->sequencerRegisters
            [HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MAP_MASK_INDEX] = HYPERDOS_VIDEO_GRAPHICS_ARRAY_DEFAULT_MAP_MASK;
    adapter->sequencerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MEMORY_MODE_INDEX] =
            HYPERDOS_VIDEO_GRAPHICS_ARRAY_TEXT_MEMORY_MODE;
    adapter->graphicsControllerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MISCELLANEOUS_INDEX] =
            HYPERDOS_VIDEO_GRAPHICS_ARRAY_TEXT_MEMORY_MAP_VALUE;
    adapter->graphicsControllerRegisters
            [HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_BIT_MASK_INDEX] = HYPERDOS_VIDEO_GRAPHICS_ARRAY_DEFAULT_BIT_MASK;
    adapter->digitalToAnalogConverterMaskRegister    = HYPERDOS_VIDEO_GRAPHICS_ARRAY_DEFAULT_DIGITAL_TO_ANALOG_MASK;
    adapter->digitalToAnalogConverterState           = HYPERDOS_VIDEO_GRAPHICS_ARRAY_DAC_STATE_WRITE_MODE;
    adapter->attributeControllerPaletteAddressSource = HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_PALETTE_SOURCE_BIT;
    hyperdos_video_graphics_array_set_display_start_address(adapter, 0u);
    hyperdos_video_graphics_array_set_display_stride_byte_count(adapter, 80u);
    for (colorIndex = 0; colorIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_CONVERTER_COLOR_COUNT;
         ++colorIndex)
    {
        uint8_t componentValue                                  = (uint8_t)(colorIndex & 0x3Fu);
        adapter->digitalToAnalogConverterPalette[colorIndex][0] = componentValue;
        adapter->digitalToAnalogConverterPalette[colorIndex][1] = componentValue;
        adapter->digitalToAnalogConverterPalette[colorIndex][2] = componentValue;
    }
    hyperdos_video_graphics_array_initialize_default_palette(adapter);
}

uint8_t hyperdos_color_graphics_adapter_read_memory_byte(void* device, uint32_t physicalAddress)
{
    hyperdos_color_graphics_adapter* adapter         = (hyperdos_color_graphics_adapter*)device;
    uint32_t                         relativeAddress = 0u;

    if (adapter == NULL)
    {
        return HYPERDOS_OPEN_BUS_BYTE;
    }
    if (hyperdos_video_graphics_array_get_memory_map_select(adapter) ==
                HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_MAP_B8000_32K &&
        physicalAddress >= HYPERDOS_COLOR_GRAPHICS_ADAPTER_MEMORY_ADDRESS &&
        physicalAddress < HYPERDOS_COLOR_GRAPHICS_ADAPTER_MEMORY_ADDRESS + HYPERDOS_COLOR_GRAPHICS_ADAPTER_MEMORY_SIZE)
    {
        relativeAddress = physicalAddress - HYPERDOS_COLOR_GRAPHICS_ADAPTER_MEMORY_ADDRESS;
        return adapter->memory[relativeAddress];
    }
    if (hyperdos_video_graphics_array_translate_memory_address(adapter, physicalAddress, &relativeAddress))
    {
        if (hyperdos_video_graphics_array_chain_four_is_enabled(adapter))
        {
            return hyperdos_video_graphics_array_read_chain_four_memory_byte(adapter, relativeAddress);
        }
        return hyperdos_video_graphics_array_read_memory_byte(adapter, relativeAddress);
    }
    return HYPERDOS_OPEN_BUS_BYTE;
}

void hyperdos_color_graphics_adapter_write_memory_byte(void* device, uint32_t physicalAddress, uint8_t value)
{
    hyperdos_color_graphics_adapter* adapter         = (hyperdos_color_graphics_adapter*)device;
    uint32_t                         relativeAddress = 0u;

    if (adapter == NULL)
    {
        return;
    }
    if (hyperdos_video_graphics_array_get_memory_map_select(adapter) ==
                HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_MAP_B8000_32K &&
        physicalAddress >= HYPERDOS_COLOR_GRAPHICS_ADAPTER_MEMORY_ADDRESS &&
        physicalAddress < HYPERDOS_COLOR_GRAPHICS_ADAPTER_MEMORY_ADDRESS + HYPERDOS_COLOR_GRAPHICS_ADAPTER_MEMORY_SIZE)
    {
        relativeAddress                  = physicalAddress - HYPERDOS_COLOR_GRAPHICS_ADAPTER_MEMORY_ADDRESS;
        adapter->memory[relativeAddress] = value;
        return;
    }
    if (hyperdos_video_graphics_array_translate_memory_address(adapter, physicalAddress, &relativeAddress))
    {
        if (hyperdos_video_graphics_array_chain_four_is_enabled(adapter))
        {
            hyperdos_video_graphics_array_write_chain_four_memory_byte(adapter, relativeAddress, value);
            return;
        }
        hyperdos_video_graphics_array_write_memory_byte(adapter, relativeAddress, value);
    }
}

void hyperdos_color_graphics_adapter_set_bios_video_mode(hyperdos_color_graphics_adapter* adapter, uint8_t videoMode)
{
    if (adapter == NULL)
    {
        return;
    }

    adapter->sequencerRegisters
            [HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MAP_MASK_INDEX] = HYPERDOS_VIDEO_GRAPHICS_ARRAY_DEFAULT_MAP_MASK;
    adapter->graphicsControllerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MODE_INDEX] = 0u;
    adapter->graphicsControllerRegisters
            [HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_BIT_MASK_INDEX] = HYPERDOS_VIDEO_GRAPHICS_ARRAY_DEFAULT_BIT_MASK;
    adapter->graphicsControllerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_READ_MAP_SELECT_INDEX] = 0u;
    adapter->attributeControllerPaletteAddressSource = HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_PALETTE_SOURCE_BIT;
    hyperdos_video_graphics_array_set_display_start_address(adapter, 0u);

    switch (videoMode)
    {
    case 0x04u:
    case 0x05u:
        adapter->modeControl = HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_VIDEO_ENABLE |
                               HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_GRAPHICS;
        adapter->sequencerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MEMORY_MODE_INDEX] =
                HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANAR_MEMORY_MODE;
        adapter->graphicsControllerRegisters
                [HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MODE_INDEX] = HYPERDOS_VIDEO_GRAPHICS_ARRAY_ODD_EVEN_BIT;
        hyperdos_video_graphics_array_set_memory_map(adapter,
                                                     HYPERDOS_VIDEO_GRAPHICS_ARRAY_CGA_GRAPHICS_MEMORY_MAP_VALUE);
        hyperdos_video_graphics_array_set_attribute_mode(adapter, 1);
        hyperdos_video_graphics_array_set_display_stride_byte_count(adapter, 40u);
        break;
    case 0x06u:
        adapter->modeControl = HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_VIDEO_ENABLE |
                               HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_GRAPHICS |
                               HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_HIGH_RESOLUTION;
        adapter->sequencerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MEMORY_MODE_INDEX] =
                HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANAR_MEMORY_MODE;
        adapter->graphicsControllerRegisters
                [HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MODE_INDEX] = HYPERDOS_VIDEO_GRAPHICS_ARRAY_ODD_EVEN_BIT;
        hyperdos_video_graphics_array_set_memory_map(adapter,
                                                     HYPERDOS_VIDEO_GRAPHICS_ARRAY_CGA_GRAPHICS_MEMORY_MAP_VALUE);
        hyperdos_video_graphics_array_set_attribute_mode(adapter, 1);
        hyperdos_video_graphics_array_set_display_stride_byte_count(adapter, 80u);
        break;
    case 0x0Du:
        adapter->modeControl = HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_VIDEO_ENABLE |
                               HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_GRAPHICS;
        adapter->sequencerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MEMORY_MODE_INDEX] =
                HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANAR_MEMORY_MODE;
        hyperdos_video_graphics_array_set_memory_map(adapter, HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MEMORY_MAP_VALUE);
        hyperdos_video_graphics_array_set_attribute_mode(adapter, 1);
        hyperdos_video_graphics_array_set_display_stride_byte_count(adapter, 40u);
        break;
    case 0x0Eu:
        adapter->modeControl = HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_VIDEO_ENABLE |
                               HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_GRAPHICS |
                               HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_HIGH_RESOLUTION;
        adapter->sequencerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MEMORY_MODE_INDEX] =
                HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANAR_MEMORY_MODE;
        hyperdos_video_graphics_array_set_memory_map(adapter, HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MEMORY_MAP_VALUE);
        hyperdos_video_graphics_array_set_attribute_mode(adapter, 1);
        hyperdos_video_graphics_array_set_display_stride_byte_count(adapter, 80u);
        break;
    case 0x0Fu:
    case 0x10u:
    case 0x11u:
    case 0x12u:
        adapter->modeControl = HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_VIDEO_ENABLE |
                               HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_GRAPHICS |
                               HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_HIGH_RESOLUTION;
        adapter->sequencerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MEMORY_MODE_INDEX] =
                HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANAR_MEMORY_MODE;
        hyperdos_video_graphics_array_set_memory_map(adapter, HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MEMORY_MAP_VALUE);
        hyperdos_video_graphics_array_set_attribute_mode(adapter, 1);
        hyperdos_video_graphics_array_set_display_stride_byte_count(adapter, 80u);
        break;
    case 0x13u:
        adapter->modeControl = HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_VIDEO_ENABLE |
                               HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_GRAPHICS;
        adapter->sequencerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MEMORY_MODE_INDEX] =
                HYPERDOS_VIDEO_GRAPHICS_ARRAY_CHAIN_FOUR_MEMORY_MODE;
        adapter->graphicsControllerRegisters
                [HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MODE_INDEX] = HYPERDOS_VIDEO_GRAPHICS_ARRAY_SHIFT_256_COLOR_BIT;
        hyperdos_video_graphics_array_set_memory_map(adapter, HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MEMORY_MAP_VALUE);
        hyperdos_video_graphics_array_set_attribute_mode(adapter, 1);
        hyperdos_video_graphics_array_set_display_stride_byte_count(adapter, 80u);
        break;
    case 0x00u:
    case 0x01u:
        adapter->modeControl = HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_VIDEO_ENABLE;
        adapter->sequencerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MEMORY_MODE_INDEX] =
                HYPERDOS_VIDEO_GRAPHICS_ARRAY_TEXT_MEMORY_MODE;
        hyperdos_video_graphics_array_set_memory_map(adapter, HYPERDOS_VIDEO_GRAPHICS_ARRAY_TEXT_MEMORY_MAP_VALUE);
        hyperdos_video_graphics_array_set_attribute_mode(adapter, 0);
        hyperdos_video_graphics_array_set_display_stride_byte_count(adapter, 40u);
        break;
    case 0x02u:
    case 0x03u:
    default:
        adapter->modeControl = HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_VIDEO_ENABLE |
                               HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_HIGH_RESOLUTION;
        adapter->sequencerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_MEMORY_MODE_INDEX] =
                HYPERDOS_VIDEO_GRAPHICS_ARRAY_TEXT_MEMORY_MODE;
        hyperdos_video_graphics_array_set_memory_map(adapter, HYPERDOS_VIDEO_GRAPHICS_ARRAY_TEXT_MEMORY_MAP_VALUE);
        hyperdos_video_graphics_array_set_attribute_mode(adapter, 0);
        hyperdos_video_graphics_array_set_display_stride_byte_count(adapter, 80u);
        break;
    }
}

int hyperdos_color_graphics_adapter_graphics_mode_is_enabled(const hyperdos_color_graphics_adapter* adapter)
{
    if (adapter == NULL)
    {
        return 0;
    }
    return (adapter->graphicsControllerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MISCELLANEOUS_INDEX] &
            HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_MODE_BIT) != 0u ||
           (adapter->attributeControllerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_MODE_CONTROL_INDEX] &
            HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_GRAPHICS_ENABLE_BIT) != 0u ||
           (adapter->modeControl & HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_GRAPHICS) != 0u;
}

uint16_t hyperdos_color_graphics_adapter_get_display_start_address(const hyperdos_color_graphics_adapter* adapter)
{
    if (adapter == NULL)
    {
        return 0u;
    }
    return (uint16_t)(((uint16_t)adapter->registers[HYPERDOS_VIDEO_GRAPHICS_ARRAY_CRT_START_ADDRESS_HIGH_INDEX]
                       << HYPERDOS_X86_BYTE_BIT_COUNT) |
                      adapter->registers[HYPERDOS_VIDEO_GRAPHICS_ARRAY_CRT_START_ADDRESS_LOW_INDEX]);
}

uint16_t hyperdos_color_graphics_adapter_get_display_stride_byte_count(const hyperdos_color_graphics_adapter* adapter)
{
    if (adapter == NULL)
    {
        return 0u;
    }
    return (uint16_t)(adapter->registers[HYPERDOS_VIDEO_GRAPHICS_ARRAY_CRT_OFFSET_INDEX] * HYPERDOS_X86_TWO_BIT_COUNT);
}

uint8_t hyperdos_color_graphics_adapter_read_input_output_byte(void* device, uint16_t port)
{
    hyperdos_color_graphics_adapter* adapter = (hyperdos_color_graphics_adapter*)device;

    if (adapter == NULL)
    {
        return HYPERDOS_OPEN_BUS_BYTE;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_ADDRESS_PORT)
    {
        return (uint8_t)(adapter->attributeControllerIndexRegister | adapter->attributeControllerPaletteAddressSource);
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_DATA_PORT)
    {
        return adapter->attributeControllerRegisters[adapter->attributeControllerIndexRegister &
                                                     HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_INDEX_MASK];
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_INDEX_PORT)
    {
        return adapter->sequencerIndexRegister;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_DATA_PORT)
    {
        return adapter->sequencerRegisters[adapter->sequencerIndexRegister &
                                           HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_INDEX_MASK];
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_MASK_PORT)
    {
        return adapter->digitalToAnalogConverterMaskRegister;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_READ_INDEX_PORT)
    {
        return adapter->digitalToAnalogConverterState;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_WRITE_INDEX_PORT)
    {
        return adapter->digitalToAnalogConverterWriteIndexRegister;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_DATA_PORT)
    {
        return hyperdos_video_graphics_array_read_digital_to_analog_converter_data(adapter);
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_FEATURE_CONTROL_READ_PORT)
    {
        return adapter->featureControlRegister;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_MISCELLANEOUS_OUTPUT_READ_PORT)
    {
        return adapter->miscellaneousOutputRegister;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_PORT)
    {
        return adapter->graphicsControllerIndexRegister;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_DATA_PORT)
    {
        return adapter->graphicsControllerRegisters[adapter->graphicsControllerIndexRegister &
                                                    HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_MASK];
    }
    if (port == HYPERDOS_COLOR_GRAPHICS_ADAPTER_INDEX_PORT)
    {
        return adapter->indexRegister;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_MONOCHROME_INDEX_PORT)
    {
        return adapter->indexRegister;
    }
    if (port == HYPERDOS_COLOR_GRAPHICS_ADAPTER_DATA_PORT)
    {
        return adapter->registers[adapter->indexRegister & HYPERDOS_COLOR_GRAPHICS_ADAPTER_REGISTER_MASK];
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_MONOCHROME_DATA_PORT)
    {
        return adapter->registers[adapter->indexRegister & HYPERDOS_COLOR_GRAPHICS_ADAPTER_REGISTER_MASK];
    }
    if (port == HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_CONTROL_PORT)
    {
        return adapter->modeControl;
    }
    if (port == HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLOR_SELECT_PORT)
    {
        return adapter->colorSelect;
    }
    if (port == HYPERDOS_COLOR_GRAPHICS_ADAPTER_STATUS_PORT ||
        port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_MONOCHROME_STATUS_PORT)
    {
        adapter->attributeControllerAddressFlipFlop  = 0u;
        adapter->statusRegister                     ^= 0x09u;
        return adapter->statusRegister;
    }
    return HYPERDOS_OPEN_BUS_BYTE;
}

void hyperdos_color_graphics_adapter_write_input_output_byte(void* device, uint16_t port, uint8_t value)
{
    hyperdos_color_graphics_adapter* adapter = (hyperdos_color_graphics_adapter*)device;

    if (adapter == NULL)
    {
        return;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_ADDRESS_PORT)
    {
        if (adapter->attributeControllerAddressFlipFlop == 0u)
        {
            adapter->attributeControllerIndexRegister = (uint8_t)(value &
                                                                  HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_INDEX_MASK);
            adapter->attributeControllerPaletteAddressSource =
                    (uint8_t)(value & HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_PALETTE_SOURCE_BIT);
        }
        else
        {
            adapter->attributeControllerRegisters[adapter->attributeControllerIndexRegister &
                                                  HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_INDEX_MASK] = value;
        }
        adapter->attributeControllerAddressFlipFlop = (uint8_t)(adapter->attributeControllerAddressFlipFlop ^ 1u);
        return;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_MISCELLANEOUS_OUTPUT_WRITE_PORT)
    {
        adapter->miscellaneousOutputRegister = value;
        return;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_INDEX_PORT)
    {
        adapter->sequencerIndexRegister = (uint8_t)(value & HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_INDEX_MASK);
        return;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_DATA_PORT)
    {
        adapter->sequencerRegisters[adapter->sequencerIndexRegister &
                                    HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_INDEX_MASK] = value;
        return;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_MASK_PORT)
    {
        adapter->digitalToAnalogConverterMaskRegister = value;
        return;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_READ_INDEX_PORT)
    {
        adapter->digitalToAnalogConverterReadIndexRegister  = value;
        adapter->digitalToAnalogConverterReadComponentIndex = 0u;
        adapter->digitalToAnalogConverterState              = HYPERDOS_VIDEO_GRAPHICS_ARRAY_DAC_STATE_READ_MODE;
        return;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_WRITE_INDEX_PORT)
    {
        adapter->digitalToAnalogConverterWriteIndexRegister  = value;
        adapter->digitalToAnalogConverterWriteComponentIndex = 0u;
        adapter->digitalToAnalogConverterState               = HYPERDOS_VIDEO_GRAPHICS_ARRAY_DAC_STATE_WRITE_MODE;
        return;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_DATA_PORT)
    {
        hyperdos_video_graphics_array_write_digital_to_analog_converter_data(adapter, value);
        return;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_PORT)
    {
        adapter->graphicsControllerIndexRegister =
                (uint8_t)(value & HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_MASK);
        return;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_DATA_PORT)
    {
        adapter->graphicsControllerRegisters[adapter->graphicsControllerIndexRegister &
                                             HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_INDEX_MASK] = value;
        return;
    }
    if (port == HYPERDOS_COLOR_GRAPHICS_ADAPTER_INDEX_PORT)
    {
        adapter->indexRegister = value;
        return;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_MONOCHROME_INDEX_PORT)
    {
        adapter->indexRegister = value;
        return;
    }
    if (port == HYPERDOS_COLOR_GRAPHICS_ADAPTER_DATA_PORT)
    {
        adapter->registers[adapter->indexRegister & HYPERDOS_COLOR_GRAPHICS_ADAPTER_REGISTER_MASK] = value;
        return;
    }
    if (port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_MONOCHROME_DATA_PORT)
    {
        adapter->registers[adapter->indexRegister & HYPERDOS_COLOR_GRAPHICS_ADAPTER_REGISTER_MASK] = value;
        return;
    }
    if (port == HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_CONTROL_PORT)
    {
        adapter->modeControl = value;
        return;
    }
    if (port == HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLOR_SELECT_PORT)
    {
        adapter->colorSelect = value;
        return;
    }
    if (port == HYPERDOS_COLOR_GRAPHICS_ADAPTER_STATUS_PORT ||
        port == HYPERDOS_VIDEO_GRAPHICS_ARRAY_MONOCHROME_STATUS_PORT)
    {
        adapter->featureControlRegister = value;
    }
}

const uint8_t* hyperdos_color_graphics_adapter_get_text_memory(const hyperdos_color_graphics_adapter* adapter)
{
    if (adapter == NULL)
    {
        return NULL;
    }
    return adapter->memory;
}

void hyperdos_x87_initialize(hyperdos_8087* coprocessor, hyperdos_x87_model model)
{
    if (coprocessor == NULL)
    {
        return;
    }

    memset(coprocessor, 0, sizeof(*coprocessor));
    coprocessor->model = model;
    hyperdos_8087_initialize_programming_state(coprocessor);
}

hyperdos_x87_model hyperdos_x87_get_model(const hyperdos_8087* coprocessor)
{
    if (coprocessor == NULL)
    {
        return HYPERDOS_X87_MODEL_NONE;
    }
    return coprocessor->model;
}

int hyperdos_x87_is_protected_mode_enabled(const hyperdos_8087* coprocessor)
{
    return coprocessor != NULL && coprocessor->protectedModeEnabled != 0u;
}

void hyperdos_8087_initialize(hyperdos_8087* coprocessor)
{
    hyperdos_x87_initialize(coprocessor, HYPERDOS_X87_MODEL_8087);
}

hyperdos_x86_execution_result hyperdos_x87_wait(hyperdos_x86_processor* processor, void* userContext)
{
    (void)processor;
    (void)userContext;
    return HYPERDOS_X86_EXECUTION_OK;
}

hyperdos_x86_execution_result hyperdos_x87_escape(hyperdos_x86_processor*                     processor,
                                                  const hyperdos_x86_coprocessor_instruction* instruction,
                                                  void*                                       userContext)
{
    hyperdos_8087* coprocessor = (hyperdos_8087*)userContext;

    if (processor == NULL || instruction == NULL || coprocessor == NULL)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }

    coprocessor->lastInstructionSegment = processor->lastInstructionSegment;
    coprocessor->lastInstructionOffset  = (uint16_t)processor->lastInstructionOffset;
    if (!instruction->isRegister)
    {
        coprocessor->lastOperandSegment = hyperdos_x86_get_segment_register(processor, instruction->segmentRegister);
        coprocessor->lastOperandOffset  = instruction->offset;
    }
    hyperdos_8087_update_environment_words(coprocessor);

    if (instruction->isRegister)
    {
        hyperdos_8087_execute_register_instruction(processor, instruction, coprocessor);
    }
    else
    {
        hyperdos_8087_execute_memory_instruction(processor, instruction, coprocessor);
    }

    hyperdos_8087_update_environment_words(coprocessor);
    return HYPERDOS_X86_EXECUTION_OK;
}

hyperdos_x86_execution_result hyperdos_8087_wait(hyperdos_x86_processor* processor, void* userContext)
{
    return hyperdos_x87_wait(processor, userContext);
}

hyperdos_x86_execution_result hyperdos_8087_escape(hyperdos_x86_processor*                     processor,
                                                   const hyperdos_x86_coprocessor_instruction* instruction,
                                                   void*                                       userContext)
{
    return hyperdos_x87_escape(processor, instruction, userContext);
}
