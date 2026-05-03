#include "hyperdos/pc_video_bios.h"

#include <stdarg.h>
#include <stdio.h>

#include "hyperdos/pc_bios_data_area.h"

enum
{
    HYPERDOS_PC_VIDEO_BIOS_TRACE_TEXT_CAPACITY                                 = 512u,
    HYPERDOS_PC_VIDEO_BIOS_TEXT_ATTRIBUTE                                      = 0x07u,
    HYPERDOS_PC_VIDEO_BIOS_SERVICE_REGISTER_SHIFT                              = 8u,
    HYPERDOS_PC_VIDEO_BIOS_SET_MODE_SERVICE                                    = 0x00u,
    HYPERDOS_PC_VIDEO_BIOS_SET_CURSOR_SHAPE_SERVICE                            = 0x01u,
    HYPERDOS_PC_VIDEO_BIOS_SET_CURSOR_POSITION_SERVICE                         = 0x02u,
    HYPERDOS_PC_VIDEO_BIOS_READ_CURSOR_POSITION_SERVICE                        = 0x03u,
    HYPERDOS_PC_VIDEO_BIOS_SET_ACTIVE_PAGE_SERVICE                             = 0x05u,
    HYPERDOS_PC_VIDEO_BIOS_SCROLL_UP_SERVICE                                   = 0x06u,
    HYPERDOS_PC_VIDEO_BIOS_SCROLL_DOWN_SERVICE                                 = 0x07u,
    HYPERDOS_PC_VIDEO_BIOS_READ_CHARACTER_ATTRIBUTE_SERVICE                    = 0x08u,
    HYPERDOS_PC_VIDEO_BIOS_WRITE_CHARACTER_ATTRIBUTE_SERVICE                   = 0x09u,
    HYPERDOS_PC_VIDEO_BIOS_WRITE_CHARACTER_SERVICE                             = 0x0Au,
    HYPERDOS_PC_VIDEO_BIOS_WRITE_PIXEL_SERVICE                                 = 0x0Cu,
    HYPERDOS_PC_VIDEO_BIOS_READ_PIXEL_SERVICE                                  = 0x0Du,
    HYPERDOS_PC_VIDEO_BIOS_TELETYPE_SERVICE                                    = 0x0Eu,
    HYPERDOS_PC_VIDEO_BIOS_GET_MODE_SERVICE                                    = 0x0Fu,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_SERVICE                                     = 0x10u,
    HYPERDOS_PC_VIDEO_BIOS_FONT_SERVICE                                        = 0x11u,
    HYPERDOS_PC_VIDEO_BIOS_ENHANCED_GRAPHICS_INFORMATION_SERVICE               = 0x12u,
    HYPERDOS_PC_VIDEO_BIOS_WRITE_STRING_SERVICE                                = 0x13u,
    HYPERDOS_PC_VIDEO_BIOS_DISPLAY_COMBINATION_CODE_SERVICE                    = 0x1Au,
    HYPERDOS_PC_VIDEO_BIOS_FUNCTIONALITY_STATE_SERVICE                         = 0x1Bu,
    HYPERDOS_PC_VIDEO_BIOS_SAVE_RESTORE_STATE_SERVICE                          = 0x1Cu,
    HYPERDOS_PC_VIDEO_BIOS_ORIGINAL_EQUIPMENT_MANUFACTURER_EXTENSION_SERVICE   = 0x5Au,
    HYPERDOS_PC_VIDEO_BIOS_ADAPTER_EXTENSION_SERVICE                           = 0x6Fu,
    HYPERDOS_PC_VIDEO_BIOS_KOREAN_EXTENSION_STATUS_SERVICE                     = 0xF8u,
    HYPERDOS_PC_VIDEO_BIOS_KOREAN_EXTENSION_CHARACTER_SHAPE_SERVICE            = 0xFCu,
    HYPERDOS_PC_VIDEO_BIOS_KOREAN_EXTENSION_OUTPUT_SERVICE                     = 0xFEu,
    HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_READ_REGISTER_SERVICE            = 0xF0u,
    HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_WRITE_REGISTER_SERVICE           = 0xF1u,
    HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_READ_REGISTER_RANGE_SERVICE      = 0xF2u,
    HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_WRITE_REGISTER_RANGE_SERVICE     = 0xF3u,
    HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_READ_REGISTER_SET_SERVICE        = 0xF4u,
    HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_WRITE_REGISTER_SET_SERVICE       = 0xF5u,
    HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_GET_VERSION_SERVICE              = 0xFAu,
    HYPERDOS_PC_VIDEO_BIOS_GET_FONT_INFORMATION_SUBSERVICE                     = 0x30u,
    HYPERDOS_PC_VIDEO_BIOS_GET_ENHANCED_GRAPHICS_INFORMATION_SUBSERVICE        = 0x10u,
    HYPERDOS_PC_VIDEO_BIOS_FUNCTIONALITY_STATE_IMPLEMENTATION_TYPE             = 0x0000u,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_SET_SINGLE_REGISTER_SUBSERVICE              = 0x00u,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_SET_BORDER_COLOR_SUBSERVICE                 = 0x01u,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_SET_ALL_REGISTERS_SUBSERVICE                = 0x02u,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_TOGGLE_INTENSITY_BLINKING_SUBSERVICE        = 0x03u,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_READ_SINGLE_REGISTER_SUBSERVICE             = 0x07u,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_READ_BORDER_COLOR_SUBSERVICE                = 0x08u,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_READ_ALL_REGISTERS_SUBSERVICE               = 0x09u,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_SET_SINGLE_DAC_REGISTER_SUBSERVICE          = 0x10u,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_SET_DAC_REGISTER_BLOCK_SUBSERVICE           = 0x12u,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_SET_COLOR_PAGE_STATE_SUBSERVICE             = 0x13u,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_READ_SINGLE_DAC_REGISTER_SUBSERVICE         = 0x15u,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_READ_DAC_REGISTER_BLOCK_SUBSERVICE          = 0x17u,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_SET_DAC_MASK_REGISTER_SUBSERVICE            = 0x18u,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_READ_DAC_MASK_REGISTER_SUBSERVICE           = 0x19u,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_READ_COLOR_PAGE_STATE_SUBSERVICE            = 0x1Au,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_SUM_COLORS_TO_GRAY_SUBSERVICE               = 0x1Bu,
    HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_MODE_CONTROL_BLINK_BIT                    = 0x08u,
    HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_MODE_CONTROL_COLOR_SELECT_MODE_BIT        = 0x80u,
    HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_MODE_CONTROL_REGISTER                     = 0x10u,
    HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_OVERSCAN_COLOR_REGISTER                   = 0x11u,
    HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_COLOR_SELECT_REGISTER                     = 0x14u,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_REGISTER_COUNT                              = 16u,
    HYPERDOS_PC_VIDEO_BIOS_PALETTE_AND_BORDER_REGISTER_COUNT                   = 17u,
    HYPERDOS_PC_VIDEO_BIOS_DAC_COMPONENT_MASK                                  = 0x3Fu,
    HYPERDOS_PC_VIDEO_BIOS_DAC_COLOR_COUNT_MASK                                = 0xFFu,
    HYPERDOS_PC_VIDEO_BIOS_VERTICAL_RESOLUTION_SUBSERVICE                      = 0x30u,
    HYPERDOS_PC_VIDEO_BIOS_DEFAULT_PALETTE_LOADING_SUBSERVICE                  = 0x31u,
    HYPERDOS_PC_VIDEO_BIOS_ADDRESSING_SUBSERVICE                               = 0x32u,
    HYPERDOS_PC_VIDEO_BIOS_GRAY_SCALE_SUMMING_SUBSERVICE                       = 0x33u,
    HYPERDOS_PC_VIDEO_BIOS_CURSOR_EMULATION_SUBSERVICE                         = 0x34u,
    HYPERDOS_PC_VIDEO_BIOS_DISPLAY_SWITCH_INTERFACE_SUBSERVICE                 = 0x35u,
    HYPERDOS_PC_VIDEO_BIOS_SCREEN_REFRESH_SUBSERVICE                           = 0x36u,
    HYPERDOS_PC_VIDEO_BIOS_VERTICAL_RESOLUTION_200_LINES                       = 0x00u,
    HYPERDOS_PC_VIDEO_BIOS_VERTICAL_RESOLUTION_350_LINES                       = 0x01u,
    HYPERDOS_PC_VIDEO_BIOS_VERTICAL_RESOLUTION_400_LINES                       = 0x02u,
    HYPERDOS_PC_VIDEO_BIOS_MODE_SET_CONTROL_200_LINE_BIT                       = 0x80u,
    HYPERDOS_PC_VIDEO_BIOS_MODE_SET_CONTROL_400_LINE_BIT                       = 0x10u,
    HYPERDOS_PC_VIDEO_BIOS_MODE_SET_CONTROL_GRAY_SCALE_SUMMING_DISABLED_BIT    = 0x02u,
    HYPERDOS_PC_VIDEO_BIOS_CONTROL_DEFAULT_PALETTE_LOADING_BIT                 = 0x08u,
    HYPERDOS_PC_VIDEO_BIOS_CONTROL_CURSOR_EMULATION_BIT                        = 0x01u,
    HYPERDOS_PC_VIDEO_BIOS_DISPLAY_COMBINATION_CODE_VIDEO_GRAPHICS_ARRAY_COLOR = 0x08u,
    HYPERDOS_PC_VIDEO_BIOS_SAVE_RESTORE_GET_BUFFER_SIZE                        = 0x00u,
    HYPERDOS_PC_VIDEO_BIOS_SAVE_RESTORE_SAVE_STATE                             = 0x01u,
    HYPERDOS_PC_VIDEO_BIOS_SAVE_RESTORE_RESTORE_STATE                          = 0x02u,
    HYPERDOS_PC_VIDEO_BIOS_SAVE_RESTORE_STATE_MASK                             = 0x0007u,
    HYPERDOS_PC_VIDEO_BIOS_REGISTER_SET_ENTRY_BYTE_COUNT                       = 4u,
    HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_VERSION_SEGMENT                  = 0xF000u,
    HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_VERSION_OFFSET                   = 0x0030u,
    HYPERDOS_PC_VIDEO_BIOS_FONT_SEGMENT                                        = 0xF000u,
    HYPERDOS_PC_VIDEO_BIOS_FONT_OFFSET                                         = 0x0200u,
    HYPERDOS_PC_VIDEO_BIOS_CHARACTER_HEIGHT                                    = 16u,
    HYPERDOS_PC_VIDEO_BIOS_MAXIMUM_TEXT_PAGE_NUMBER                            = 0x07u,
    HYPERDOS_PC_VIDEO_BIOS_KOREAN_EXTENSION_CHARACTER_SHAPE_SUBSERVICE         = 0x02u,
    HYPERDOS_PC_VIDEO_BIOS_UNSUPPORTED_STATUS                                  = 0x86u,
    HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_CURSOR_SHAPE_OFFSET                       = 0x0060u,
    HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_VIDEO_CONTROL_OFFSET                      = 0x0087u,
    HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_VIDEO_SWITCHES_OFFSET                     = 0x0088u,
    HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_VIDEO_MODE_SET_CONTROL_OFFSET             = 0x0089u,
    HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_DISPLAY_COMBINATION_CODE_INDEX_OFFSET     = 0x008Au
};

static void hyperdos_pc_video_bios_set_carry_flag(hyperdos_x86_processor* processor, int carry)
{
    hyperdos_x86_set_flag(processor, HYPERDOS_X86_FLAG_CARRY, carry);
}

static uint32_t hyperdos_pc_video_bios_get_extra_segment_physical_address(const hyperdos_x86_processor* processor,
                                                                          uint16_t                      offset)
{
    uint32_t physicalAddress = 0u;

    (void)hyperdos_x86_translate_logical_to_physical_address(processor,
                                                             HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                             offset,
                                                             &physicalAddress);
    return physicalAddress;
}

static uint8_t hyperdos_pc_video_bios_read_guest_memory_byte(const hyperdos_pc_video_bios_interface* videoBiosInterface,
                                                             uint32_t                                physicalAddress)
{
    return hyperdos_bus_read_memory_byte_or_open_bus(&videoBiosInterface->pc->bus,
                                                     physicalAddress & HYPERDOS_X86_ADDRESS_MASK);
}

static uint16_t hyperdos_pc_video_bios_read_guest_memory_word(
        const hyperdos_pc_video_bios_interface* videoBiosInterface,
        uint32_t                                physicalAddress)
{
    uint16_t lowByte  = hyperdos_pc_video_bios_read_guest_memory_byte(videoBiosInterface, physicalAddress);
    uint16_t highByte = hyperdos_pc_video_bios_read_guest_memory_byte(videoBiosInterface, physicalAddress + 1u);

    return (uint16_t)(lowByte | (highByte << HYPERDOS_X86_BYTE_BIT_COUNT));
}

static uint16_t hyperdos_pc_video_bios_read_guest_stack_word(const hyperdos_pc_video_bios_interface* videoBiosInterface,
                                                             const hyperdos_x86_processor*           processor,
                                                             uint16_t                                stackByteOffset)
{
    uint32_t physicalAddress = 0u;
    uint16_t stackOffset =
            (uint16_t)(hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER) +
                       stackByteOffset);

    (void)hyperdos_x86_translate_logical_to_physical_address(processor,
                                                             HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                                             stackOffset,
                                                             &physicalAddress);
    return hyperdos_pc_video_bios_read_guest_memory_word(videoBiosInterface, physicalAddress);
}

static void hyperdos_pc_video_bios_write_guest_memory_byte(const hyperdos_pc_video_bios_interface* videoBiosInterface,
                                                           uint32_t                                physicalAddress,
                                                           uint8_t                                 value)
{
    hyperdos_bus_write_memory_byte_if_mapped(&videoBiosInterface->pc->bus,
                                             physicalAddress & HYPERDOS_X86_ADDRESS_MASK,
                                             value);
}

static void hyperdos_pc_video_bios_trace(const hyperdos_pc_video_bios_interface* videoBiosInterface,
                                         const char*                             format,
                                         ...)
{
    char    message[HYPERDOS_PC_VIDEO_BIOS_TRACE_TEXT_CAPACITY];
    va_list arguments;

    if (videoBiosInterface == NULL || videoBiosInterface->traceFunction == NULL)
    {
        return;
    }

    va_start(arguments, format);
    (void)vsnprintf(message, sizeof(message), format, arguments);
    va_end(arguments);
    videoBiosInterface->traceFunction(videoBiosInterface->userContext, message);
}

static uint8_t clamp_video_digital_to_analog_converter_component(uint8_t value)
{
    return (uint8_t)(value & HYPERDOS_PC_VIDEO_BIOS_DAC_COMPONENT_MASK);
}

static uint8_t get_video_palette_register_number(uint16_t base)
{
    return (uint8_t)(base & (HYPERDOS_PC_VIDEO_BIOS_PALETTE_REGISTER_COUNT - 1u));
}

static uint8_t get_video_digital_to_analog_converter_register_number(uint16_t base)
{
    return (uint8_t)(base & HYPERDOS_PC_VIDEO_BIOS_DAC_COLOR_COUNT_MASK);
}

static void set_video_palette_register(hyperdos_color_graphics_adapter* adapter, uint8_t registerNumber, uint8_t value)
{
    adapter->attributeControllerRegisters[registerNumber & (HYPERDOS_PC_VIDEO_BIOS_PALETTE_REGISTER_COUNT -
                                                            1u)] = (uint8_t)(value &
                                                                             HYPERDOS_PC_VIDEO_BIOS_DAC_COMPONENT_MASK);
}

static void set_video_border_color_register(hyperdos_color_graphics_adapter* adapter, uint8_t value)
{
    adapter->attributeControllerRegisters[HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_OVERSCAN_COLOR_REGISTER] =
            (uint8_t)(value & HYPERDOS_PC_VIDEO_BIOS_DAC_COMPONENT_MASK);
}

static void set_video_digital_to_analog_converter_register(hyperdos_color_graphics_adapter* adapter,
                                                           uint8_t                          registerNumber,
                                                           uint8_t                          red,
                                                           uint8_t                          green,
                                                           uint8_t                          blue)
{
    adapter->digitalToAnalogConverterPalette[registerNumber][0] = clamp_video_digital_to_analog_converter_component(
            red);
    adapter->digitalToAnalogConverterPalette[registerNumber][1] = clamp_video_digital_to_analog_converter_component(
            green);
    adapter->digitalToAnalogConverterPalette[registerNumber][2] = clamp_video_digital_to_analog_converter_component(
            blue);
}

static void set_low_byte(uint16_t* value, uint8_t lowByte)
{
    *value = (uint16_t)((*value & 0xFF00u) | lowByte);
}

static void set_high_byte(uint16_t* value, uint8_t highByte)
{
    *value = (uint16_t)((*value & 0x00FFu) | ((uint16_t)highByte << HYPERDOS_X86_BYTE_BIT_COUNT));
}

static void hyperdos_pc_video_bios_return_unsupported_service(
        hyperdos_x86_processor*                 processor,
        const hyperdos_pc_video_bios_interface* videoBiosInterface,
        uint8_t                                 serviceNumber,
        uint16_t                                accumulator,
        const char*                             eventName)
{
    uint16_t callerReturnOffset      = hyperdos_pc_video_bios_read_guest_stack_word(videoBiosInterface, processor, 0u);
    uint16_t callerReturnSegment     = hyperdos_pc_video_bios_read_guest_stack_word(videoBiosInterface, processor, 2u);
    uint16_t callerInstructionOffset = (uint16_t)(callerReturnOffset - HYPERDOS_X86_WORD_SIZE);

    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)((HYPERDOS_PC_VIDEO_BIOS_UNSUPPORTED_STATUS
                                                       << HYPERDOS_PC_VIDEO_BIOS_SERVICE_REGISTER_SHIFT) |
                                                      (accumulator & HYPERDOS_X86_LOW_BYTE_MASK)));

    hyperdos_pc_video_bios_trace(videoBiosInterface,
                                 "%s AH=%02X AL=%02X returned-status=%02X caller=%04X:%04X return=%04X:%04X",
                                 eventName,
                                 serviceNumber,
                                 accumulator & HYPERDOS_X86_LOW_BYTE_MASK,
                                 HYPERDOS_PC_VIDEO_BIOS_UNSUPPORTED_STATUS,
                                 callerReturnSegment,
                                 callerInstructionOffset,
                                 callerReturnSegment,
                                 callerReturnOffset);
    hyperdos_pc_video_bios_set_carry_flag(processor, 1);
}

hyperdos_x86_execution_result hyperdos_pc_video_bios_handle_interrupt(
        hyperdos_x86_processor*                 processor,
        const hyperdos_pc_video_bios_interface* videoBiosInterface,
        uint8_t                                 serviceNumber)
{
    uint16_t accumulator = hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
    uint16_t base        = hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE);
    uint16_t counter     = hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER);
    uint16_t data        = hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA);
    void*    userContext = videoBiosInterface->userContext;

    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_SET_MODE_SERVICE)
    {
        videoBiosInterface->setVideoMode(userContext, (uint8_t)(accumulator & HYPERDOS_X86_LOW_BYTE_MASK));
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_SET_CURSOR_SHAPE_SERVICE)
    {
        hyperdos_pc_bios_data_area_write_word(videoBiosInterface->pc,
                                              HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_CURSOR_SHAPE_OFFSET,
                                              counter);
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_SET_CURSOR_POSITION_SERVICE)
    {
        videoBiosInterface->setTextCursorPosition(userContext,
                                                  (uint8_t)(base >> HYPERDOS_PC_VIDEO_BIOS_SERVICE_REGISTER_SHIFT),
                                                  (uint16_t)(data >> HYPERDOS_PC_VIDEO_BIOS_SERVICE_REGISTER_SHIFT),
                                                  (uint16_t)(data & HYPERDOS_X86_LOW_BYTE_MASK));
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_READ_CURSOR_POSITION_SERVICE)
    {
        uint8_t  videoPage      = (uint8_t)(base >> HYPERDOS_PC_VIDEO_BIOS_SERVICE_REGISTER_SHIFT);
        uint16_t cursorPosition = 0u;

        if (videoPage <= HYPERDOS_PC_VIDEO_BIOS_MAXIMUM_TEXT_PAGE_NUMBER)
        {
            cursorPosition = videoBiosInterface->getTextCursorPosition(userContext, videoPage);
        }
        hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER, 0x0607u);
        hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA, cursorPosition);
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_SET_ACTIVE_PAGE_SERVICE)
    {
        videoBiosInterface->setActiveVideoPage(userContext, (uint8_t)(accumulator & HYPERDOS_X86_LOW_BYTE_MASK));
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_SCROLL_UP_SERVICE ||
        serviceNumber == HYPERDOS_PC_VIDEO_BIOS_SCROLL_DOWN_SERVICE)
    {
        videoBiosInterface->scrollTextRegion(userContext,
                                             (uint16_t)(counter >> HYPERDOS_PC_VIDEO_BIOS_SERVICE_REGISTER_SHIFT),
                                             (uint16_t)(counter & HYPERDOS_X86_LOW_BYTE_MASK),
                                             (uint16_t)(data >> HYPERDOS_PC_VIDEO_BIOS_SERVICE_REGISTER_SHIFT),
                                             (uint16_t)(data & HYPERDOS_X86_LOW_BYTE_MASK),
                                             (uint8_t)(accumulator & HYPERDOS_X86_LOW_BYTE_MASK),
                                             (uint8_t)(base >> HYPERDOS_PC_VIDEO_BIOS_SERVICE_REGISTER_SHIFT),
                                             serviceNumber == HYPERDOS_PC_VIDEO_BIOS_SCROLL_DOWN_SERVICE);
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_READ_CHARACTER_ATTRIBUTE_SERVICE)
    {
        uint8_t videoPage = (uint8_t)(base >> HYPERDOS_PC_VIDEO_BIOS_SERVICE_REGISTER_SHIFT);

        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                               videoBiosInterface->readTextCharacterAttributeAtCursor(userContext,
                                                                                                      videoPage));
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_WRITE_CHARACTER_ATTRIBUTE_SERVICE ||
        serviceNumber == HYPERDOS_PC_VIDEO_BIOS_WRITE_CHARACTER_SERVICE)
    {
        videoBiosInterface->writeCharacterRepeated(userContext,
                                                   (uint8_t)(base >> HYPERDOS_PC_VIDEO_BIOS_SERVICE_REGISTER_SHIFT),
                                                   (uint8_t)(accumulator & HYPERDOS_X86_LOW_BYTE_MASK),
                                                   (uint8_t)(base & HYPERDOS_X86_LOW_BYTE_MASK),
                                                   counter,
                                                   serviceNumber ==
                                                           HYPERDOS_PC_VIDEO_BIOS_WRITE_CHARACTER_ATTRIBUTE_SERVICE);
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_WRITE_PIXEL_SERVICE)
    {
        videoBiosInterface->writePixel(userContext,
                                       (uint8_t)(base >> HYPERDOS_PC_VIDEO_BIOS_SERVICE_REGISTER_SHIFT),
                                       counter,
                                       data,
                                       (uint8_t)(accumulator & HYPERDOS_X86_LOW_BYTE_MASK));
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_READ_PIXEL_SERVICE)
    {
        uint8_t color = videoBiosInterface->readPixel(userContext,
                                                      (uint8_t)(base >> HYPERDOS_PC_VIDEO_BIOS_SERVICE_REGISTER_SHIFT),
                                                      counter,
                                                      data);

        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                               (uint16_t)((accumulator & 0xFF00u) | color));
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_TELETYPE_SERVICE)
    {
        videoBiosInterface->writeTeletypeCharacter(userContext,
                                                   (uint8_t)(accumulator & HYPERDOS_X86_LOW_BYTE_MASK),
                                                   HYPERDOS_PC_VIDEO_BIOS_TEXT_ATTRIBUTE);
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_WRITE_STRING_SERVICE)
    {
        videoBiosInterface->writeString(processor,
                                        userContext,
                                        (uint8_t)(accumulator & HYPERDOS_X86_LOW_BYTE_MASK),
                                        (uint8_t)(base >> HYPERDOS_PC_VIDEO_BIOS_SERVICE_REGISTER_SHIFT),
                                        (uint8_t)(base & HYPERDOS_X86_LOW_BYTE_MASK),
                                        counter,
                                        (uint16_t)(data >> HYPERDOS_PC_VIDEO_BIOS_SERVICE_REGISTER_SHIFT),
                                        (uint16_t)(data & HYPERDOS_X86_LOW_BYTE_MASK));
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_GET_MODE_SERVICE)
    {
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                               (uint16_t)(((uint16_t)videoBiosInterface->getVideoColumnCount(
                                                                   userContext)
                                                           << HYPERDOS_X86_BYTE_BIT_COUNT) |
                                                          videoBiosInterface->getVideoMode(userContext)));
        hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE, 0u);
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_PALETTE_SERVICE)
    {
        hyperdos_color_graphics_adapter* adapter          = &videoBiosInterface->pc->colorGraphicsAdapter;
        uint8_t                          subserviceNumber = (uint8_t)(accumulator & HYPERDOS_X86_LOW_BYTE_MASK);
        uint8_t                          baseLowByte      = (uint8_t)(base & HYPERDOS_X86_LOW_BYTE_MASK);
        uint8_t                          baseHighByte     = (uint8_t)(base >> HYPERDOS_X86_BYTE_BIT_COUNT);
        uint8_t                          counterLowByte   = (uint8_t)(counter & HYPERDOS_X86_LOW_BYTE_MASK);
        uint8_t                          counterHighByte  = (uint8_t)(counter >> HYPERDOS_X86_BYTE_BIT_COUNT);
        uint8_t                          dataHighByte     = (uint8_t)(data >> HYPERDOS_X86_BYTE_BIT_COUNT);
        uint32_t tablePhysicalAddress = hyperdos_pc_video_bios_get_extra_segment_physical_address(processor, data);

        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_PALETTE_SET_SINGLE_REGISTER_SUBSERVICE)
        {
            set_video_palette_register(adapter, get_video_palette_register_number(base), baseHighByte);
            hyperdos_pc_video_bios_trace(videoBiosInterface,
                                         "int10 palette set register=%u value=%02X",
                                         (unsigned int)get_video_palette_register_number(base),
                                         baseHighByte);
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_PALETTE_SET_BORDER_COLOR_SUBSERVICE)
        {
            set_video_border_color_register(adapter, baseHighByte);
            hyperdos_pc_video_bios_trace(videoBiosInterface, "int10 palette set border=%02X", baseHighByte);
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_PALETTE_SET_ALL_REGISTERS_SUBSERVICE)
        {
            uint8_t registerNumber = 0u;

            for (registerNumber = 0u; registerNumber < HYPERDOS_PC_VIDEO_BIOS_PALETTE_REGISTER_COUNT; ++registerNumber)
            {
                set_video_palette_register(adapter,
                                           registerNumber,
                                           hyperdos_pc_video_bios_read_guest_memory_byte(videoBiosInterface,
                                                                                         tablePhysicalAddress +
                                                                                                 registerNumber));
            }
            set_video_border_color_register(adapter,
                                            hyperdos_pc_video_bios_read_guest_memory_byte(
                                                    videoBiosInterface,
                                                    tablePhysicalAddress +
                                                            HYPERDOS_PC_VIDEO_BIOS_PALETTE_REGISTER_COUNT));
            hyperdos_pc_video_bios_trace(videoBiosInterface,
                                         "int10 palette set all table=%05lX",
                                         (unsigned long)tablePhysicalAddress);
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_PALETTE_TOGGLE_INTENSITY_BLINKING_SUBSERVICE)
        {
            if (baseLowByte != 0u)
            {
                adapter->attributeControllerRegisters[HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_MODE_CONTROL_REGISTER] |=
                        HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_MODE_CONTROL_BLINK_BIT;
            }
            else
            {
                adapter->attributeControllerRegisters[HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_MODE_CONTROL_REGISTER] &=
                        (uint8_t)~HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_MODE_CONTROL_BLINK_BIT;
            }
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_PALETTE_READ_SINGLE_REGISTER_SUBSERVICE)
        {
            set_high_byte(&base, adapter->attributeControllerRegisters[get_video_palette_register_number(base)]);
            hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE, base);
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_PALETTE_READ_BORDER_COLOR_SUBSERVICE)
        {
            set_high_byte(&base,
                          adapter->attributeControllerRegisters
                                  [HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_OVERSCAN_COLOR_REGISTER]);
            hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE, base);
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_PALETTE_READ_ALL_REGISTERS_SUBSERVICE)
        {
            uint8_t registerNumber = 0u;

            for (registerNumber = 0u; registerNumber < HYPERDOS_PC_VIDEO_BIOS_PALETTE_REGISTER_COUNT; ++registerNumber)
            {
                hyperdos_pc_video_bios_write_guest_memory_byte(videoBiosInterface,
                                                               tablePhysicalAddress + registerNumber,
                                                               adapter->attributeControllerRegisters[registerNumber]);
            }
            hyperdos_pc_video_bios_write_guest_memory_byte(
                    videoBiosInterface,
                    tablePhysicalAddress + HYPERDOS_PC_VIDEO_BIOS_PALETTE_REGISTER_COUNT,
                    adapter->attributeControllerRegisters[HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_OVERSCAN_COLOR_REGISTER]);
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_PALETTE_SET_SINGLE_DAC_REGISTER_SUBSERVICE)
        {
            set_video_digital_to_analog_converter_register(adapter,
                                                           get_video_digital_to_analog_converter_register_number(base),
                                                           dataHighByte,
                                                           counterHighByte,
                                                           counterLowByte);
            hyperdos_pc_video_bios_trace(videoBiosInterface,
                                         "int10 palette set dac=%u red=%02X green=%02X blue=%02X",
                                         (unsigned int)get_video_digital_to_analog_converter_register_number(base),
                                         dataHighByte,
                                         counterHighByte,
                                         counterLowByte);
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_PALETTE_SET_DAC_REGISTER_BLOCK_SUBSERVICE)
        {
            uint16_t registerOffset = 0u;

            for (registerOffset = 0u; registerOffset < counter; ++registerOffset)
            {
                uint16_t colorRegisterNumber = (uint16_t)(base + registerOffset);

                if (colorRegisterNumber >= HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_CONVERTER_COLOR_COUNT)
                {
                    break;
                }
                set_video_digital_to_analog_converter_register(
                        adapter,
                        (uint8_t)colorRegisterNumber,
                        hyperdos_pc_video_bios_read_guest_memory_byte(videoBiosInterface,
                                                                      tablePhysicalAddress + registerOffset * 3u),
                        hyperdos_pc_video_bios_read_guest_memory_byte(videoBiosInterface,
                                                                      tablePhysicalAddress + registerOffset * 3u + 1u),
                        hyperdos_pc_video_bios_read_guest_memory_byte(videoBiosInterface,
                                                                      tablePhysicalAddress + registerOffset * 3u + 2u));
            }
            hyperdos_pc_video_bios_trace(videoBiosInterface,
                                         "int10 palette set dac block first=%u count=%u table=%05lX",
                                         (unsigned int)(base & HYPERDOS_PC_VIDEO_BIOS_DAC_COLOR_COUNT_MASK),
                                         (unsigned int)counter,
                                         (unsigned long)tablePhysicalAddress);
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_PALETTE_SET_COLOR_PAGE_STATE_SUBSERVICE)
        {
            if (baseLowByte == 0u)
            {
                if ((baseHighByte & 0x01u) != 0u)
                {
                    adapter->attributeControllerRegisters[HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_MODE_CONTROL_REGISTER] |=
                            HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_MODE_CONTROL_COLOR_SELECT_MODE_BIT;
                }
                else
                {
                    adapter->attributeControllerRegisters[HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_MODE_CONTROL_REGISTER] &=
                            (uint8_t)~HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_MODE_CONTROL_COLOR_SELECT_MODE_BIT;
                }
            }
            else if (baseLowByte == 1u)
            {
                adapter->attributeControllerRegisters
                        [HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_COLOR_SELECT_REGISTER] = (uint8_t)(baseHighByte & 0x0Fu);
            }
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_PALETTE_READ_SINGLE_DAC_REGISTER_SUBSERVICE)
        {
            const uint8_t* color = adapter->digitalToAnalogConverterPalette
                                           [get_video_digital_to_analog_converter_register_number(base)];

            set_high_byte(&data, color[0]);
            set_high_byte(&counter, color[1]);
            set_low_byte(&counter, color[2]);
            hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER, counter);
            hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA, data);
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_PALETTE_READ_DAC_REGISTER_BLOCK_SUBSERVICE)
        {
            uint16_t registerOffset = 0u;

            for (registerOffset = 0u; registerOffset < counter; ++registerOffset)
            {
                uint16_t       colorRegisterNumber = (uint16_t)(base + registerOffset);
                const uint8_t* color               = NULL;

                if (colorRegisterNumber >= HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_CONVERTER_COLOR_COUNT)
                {
                    break;
                }
                color = adapter->digitalToAnalogConverterPalette[colorRegisterNumber];
                hyperdos_pc_video_bios_write_guest_memory_byte(videoBiosInterface,
                                                               tablePhysicalAddress + registerOffset * 3u,
                                                               color[0]);
                hyperdos_pc_video_bios_write_guest_memory_byte(videoBiosInterface,
                                                               tablePhysicalAddress + registerOffset * 3u + 1u,
                                                               color[1]);
                hyperdos_pc_video_bios_write_guest_memory_byte(videoBiosInterface,
                                                               tablePhysicalAddress + registerOffset * 3u + 2u,
                                                               color[2]);
            }
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_PALETTE_SET_DAC_MASK_REGISTER_SUBSERVICE)
        {
            adapter->digitalToAnalogConverterMaskRegister = baseLowByte;
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_PALETTE_READ_DAC_MASK_REGISTER_SUBSERVICE)
        {
            set_low_byte(&base, adapter->digitalToAnalogConverterMaskRegister);
            hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE, base);
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_PALETTE_READ_COLOR_PAGE_STATE_SUBSERVICE)
        {
            uint8_t colorSelectMode = (adapter->attributeControllerRegisters
                                               [HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_MODE_CONTROL_REGISTER] &
                                       HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_MODE_CONTROL_COLOR_SELECT_MODE_BIT) != 0u
                                              ? 1u
                                              : 0u;
            uint8_t colorSelect     = adapter->attributeControllerRegisters
                                          [HYPERDOS_PC_VIDEO_BIOS_ATTRIBUTE_COLOR_SELECT_REGISTER];

            set_low_byte(&base, colorSelectMode);
            hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE, base);
            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
                                                   colorSelectMode != 0u ? (uint16_t)(colorSelect & 0x0Fu)
                                                                         : (uint16_t)((colorSelect >> 2u) & 0x03u));
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_PALETTE_SUM_COLORS_TO_GRAY_SUBSERVICE)
        {
            uint16_t registerOffset = 0u;

            for (registerOffset = 0u; registerOffset < counter; ++registerOffset)
            {
                uint16_t colorRegisterNumber = (uint16_t)(base + registerOffset);
                uint8_t* color               = NULL;
                uint8_t  grayValue           = 0u;

                if (colorRegisterNumber >= HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_CONVERTER_COLOR_COUNT)
                {
                    break;
                }
                color     = adapter->digitalToAnalogConverterPalette[colorRegisterNumber];
                grayValue = (uint8_t)((color[0] * 30u + color[1] * 59u + color[2] * 11u + 50u) / 100u);
                color[0]  = grayValue;
                color[1]  = grayValue;
                color[2]  = grayValue;
            }
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }

        hyperdos_pc_video_bios_trace(videoBiosInterface,
                                     "int10 palette unsupported subservice AL=%02X",
                                     subserviceNumber);
        hyperdos_pc_video_bios_set_carry_flag(processor, 1);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_FONT_SERVICE)
    {
        uint8_t subserviceNumber = (uint8_t)(accumulator & HYPERDOS_X86_LOW_BYTE_MASK);

        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_GET_FONT_INFORMATION_SUBSERVICE)
        {
            hyperdos_x86_set_segment_register(processor,
                                              HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                              HYPERDOS_PC_VIDEO_BIOS_FONT_SEGMENT);
            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_BASE_POINTER,
                                                   HYPERDOS_PC_VIDEO_BIOS_FONT_OFFSET);
            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
                                                   HYPERDOS_PC_VIDEO_BIOS_CHARACTER_HEIGHT);
            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                                   (uint16_t)((data & 0xFF00u) |
                                                              (HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT - 1u)));
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_ENHANCED_GRAPHICS_INFORMATION_SERVICE)
    {
        uint8_t subserviceNumber     = (uint8_t)(base & HYPERDOS_X86_LOW_BYTE_MASK);
        uint8_t alternateSelectValue = (uint8_t)(accumulator & HYPERDOS_X86_LOW_BYTE_MASK);

        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_GET_ENHANCED_GRAPHICS_INFORMATION_SUBSERVICE)
        {
            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_BASE,
                                                   (uint16_t)((base & 0xFF00u) | 0x0003u));
            hyperdos_x86_set_general_register_word(
                    processor,
                    HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
                    (uint16_t)((counter & 0xFF00u) | (hyperdos_pc_bios_data_area_read_byte(
                                                              videoBiosInterface->pc,
                                                              HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_VIDEO_SWITCHES_OFFSET) &
                                                      0x0Fu)));
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_VERTICAL_RESOLUTION_SUBSERVICE)
        {
            uint8_t videoModeSetControl = hyperdos_pc_bios_data_area_read_byte(
                    videoBiosInterface->pc,
                    HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_VIDEO_MODE_SET_CONTROL_OFFSET);
            uint8_t videoSwitches =
                    hyperdos_pc_bios_data_area_read_byte(videoBiosInterface->pc,
                                                         HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_VIDEO_SWITCHES_OFFSET);

            if (alternateSelectValue == HYPERDOS_PC_VIDEO_BIOS_VERTICAL_RESOLUTION_200_LINES)
            {
                videoModeSetControl = (uint8_t)((videoModeSetControl &
                                                 (uint8_t)~HYPERDOS_PC_VIDEO_BIOS_MODE_SET_CONTROL_400_LINE_BIT) |
                                                HYPERDOS_PC_VIDEO_BIOS_MODE_SET_CONTROL_200_LINE_BIT);
                videoSwitches       = (uint8_t)((videoSwitches & 0xF0u) | 0x08u);
            }
            else if (alternateSelectValue == HYPERDOS_PC_VIDEO_BIOS_VERTICAL_RESOLUTION_350_LINES)
            {
                videoModeSetControl = (uint8_t)(videoModeSetControl &
                                                (uint8_t)~(HYPERDOS_PC_VIDEO_BIOS_MODE_SET_CONTROL_200_LINE_BIT |
                                                           HYPERDOS_PC_VIDEO_BIOS_MODE_SET_CONTROL_400_LINE_BIT));
                videoSwitches       = (uint8_t)((videoSwitches & 0xF0u) | 0x09u);
            }
            else if (alternateSelectValue == HYPERDOS_PC_VIDEO_BIOS_VERTICAL_RESOLUTION_400_LINES)
            {
                videoModeSetControl = (uint8_t)((videoModeSetControl &
                                                 (uint8_t)~HYPERDOS_PC_VIDEO_BIOS_MODE_SET_CONTROL_200_LINE_BIT) |
                                                HYPERDOS_PC_VIDEO_BIOS_MODE_SET_CONTROL_400_LINE_BIT);
                videoSwitches       = (uint8_t)((videoSwitches & 0xF0u) | 0x09u);
            }

            hyperdos_pc_bios_data_area_write_byte(videoBiosInterface->pc,
                                                  HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_VIDEO_MODE_SET_CONTROL_OFFSET,
                                                  videoModeSetControl);
            hyperdos_pc_bios_data_area_write_byte(videoBiosInterface->pc,
                                                  HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_VIDEO_SWITCHES_OFFSET,
                                                  videoSwitches);
            hyperdos_x86_set_general_register_word(
                    processor,
                    HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                    (uint16_t)((accumulator & 0xFF00u) | HYPERDOS_PC_VIDEO_BIOS_ENHANCED_GRAPHICS_INFORMATION_SERVICE));
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_DEFAULT_PALETTE_LOADING_SUBSERVICE)
        {
            uint8_t videoControl =
                    hyperdos_pc_bios_data_area_read_byte(videoBiosInterface->pc,
                                                         HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_VIDEO_CONTROL_OFFSET);

            videoControl = (uint8_t)((videoControl &
                                      (uint8_t)~HYPERDOS_PC_VIDEO_BIOS_CONTROL_DEFAULT_PALETTE_LOADING_BIT) |
                                     ((alternateSelectValue & 0x01u) << 3u));
            hyperdos_pc_bios_data_area_write_byte(videoBiosInterface->pc,
                                                  HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_VIDEO_CONTROL_OFFSET,
                                                  videoControl);
            hyperdos_x86_set_general_register_word(
                    processor,
                    HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                    (uint16_t)((accumulator & 0xFF00u) | HYPERDOS_PC_VIDEO_BIOS_ENHANCED_GRAPHICS_INFORMATION_SERVICE));
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_ADDRESSING_SUBSERVICE)
        {
            hyperdos_x86_set_general_register_word(
                    processor,
                    HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                    (uint16_t)((accumulator & 0xFF00u) | HYPERDOS_PC_VIDEO_BIOS_ENHANCED_GRAPHICS_INFORMATION_SERVICE));
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_GRAY_SCALE_SUMMING_SUBSERVICE)
        {
            uint8_t videoModeSetControl = hyperdos_pc_bios_data_area_read_byte(
                    videoBiosInterface->pc,
                    HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_VIDEO_MODE_SET_CONTROL_OFFSET);
            uint8_t grayScaleSummingBit =
                    (uint8_t)(((alternateSelectValue << 1u) &
                               HYPERDOS_PC_VIDEO_BIOS_MODE_SET_CONTROL_GRAY_SCALE_SUMMING_DISABLED_BIT) ^
                              HYPERDOS_PC_VIDEO_BIOS_MODE_SET_CONTROL_GRAY_SCALE_SUMMING_DISABLED_BIT);

            videoModeSetControl =
                    (uint8_t)((videoModeSetControl &
                               (uint8_t)~HYPERDOS_PC_VIDEO_BIOS_MODE_SET_CONTROL_GRAY_SCALE_SUMMING_DISABLED_BIT) |
                              grayScaleSummingBit);
            hyperdos_pc_bios_data_area_write_byte(videoBiosInterface->pc,
                                                  HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_VIDEO_MODE_SET_CONTROL_OFFSET,
                                                  videoModeSetControl);
            hyperdos_x86_set_general_register_word(
                    processor,
                    HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                    (uint16_t)((accumulator & 0xFF00u) | HYPERDOS_PC_VIDEO_BIOS_ENHANCED_GRAPHICS_INFORMATION_SERVICE));
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_CURSOR_EMULATION_SUBSERVICE)
        {
            uint8_t videoControl =
                    hyperdos_pc_bios_data_area_read_byte(videoBiosInterface->pc,
                                                         HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_VIDEO_CONTROL_OFFSET);

            videoControl = (uint8_t)((videoControl & (uint8_t)~HYPERDOS_PC_VIDEO_BIOS_CONTROL_CURSOR_EMULATION_BIT) |
                                     (alternateSelectValue & HYPERDOS_PC_VIDEO_BIOS_CONTROL_CURSOR_EMULATION_BIT));
            hyperdos_pc_bios_data_area_write_byte(videoBiosInterface->pc,
                                                  HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_VIDEO_CONTROL_OFFSET,
                                                  videoControl);
            hyperdos_x86_set_general_register_word(
                    processor,
                    HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                    (uint16_t)((accumulator & 0xFF00u) | HYPERDOS_PC_VIDEO_BIOS_ENHANCED_GRAPHICS_INFORMATION_SERVICE));
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_DISPLAY_SWITCH_INTERFACE_SUBSERVICE ||
            subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_SCREEN_REFRESH_SUBSERVICE)
        {
            hyperdos_x86_set_general_register_word(
                    processor,
                    HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                    (uint16_t)((accumulator & 0xFF00u) | HYPERDOS_PC_VIDEO_BIOS_ENHANCED_GRAPHICS_INFORMATION_SERVICE));
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }

        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_DISPLAY_COMBINATION_CODE_SERVICE)
    {
        uint8_t subserviceNumber = (uint8_t)(accumulator & HYPERDOS_X86_LOW_BYTE_MASK);

        if (subserviceNumber == 0u)
        {
            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                   HYPERDOS_PC_VIDEO_BIOS_DISPLAY_COMBINATION_CODE_SERVICE);
            hyperdos_x86_set_general_register_word(
                    processor,
                    HYPERDOS_X86_GENERAL_REGISTER_BASE,
                    HYPERDOS_PC_VIDEO_BIOS_DISPLAY_COMBINATION_CODE_VIDEO_GRAPHICS_ARRAY_COLOR);
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == 1u)
        {
            hyperdos_pc_bios_data_area_write_byte(
                    videoBiosInterface->pc,
                    HYPERDOS_PC_VIDEO_BIOS_DATA_AREA_DISPLAY_COMBINATION_CODE_INDEX_OFFSET,
                    (uint8_t)(base & HYPERDOS_X86_LOW_BYTE_MASK));
            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                   HYPERDOS_PC_VIDEO_BIOS_DISPLAY_COMBINATION_CODE_SERVICE);
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_FUNCTIONALITY_STATE_SERVICE)
    {
        if (base == HYPERDOS_PC_VIDEO_BIOS_FUNCTIONALITY_STATE_IMPLEMENTATION_TYPE)
        {
            videoBiosInterface->writeFunctionalityStateTable(processor, userContext);
            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                   (uint16_t)((accumulator & 0xFF00u) |
                                                              HYPERDOS_PC_VIDEO_BIOS_FUNCTIONALITY_STATE_SERVICE));
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }

        hyperdos_pc_video_bios_trace(videoBiosInterface,
                                     "int10 functionality-state unsupported implementation BX=%04X",
                                     base);
        hyperdos_pc_video_bios_set_carry_flag(processor, 1);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_SAVE_RESTORE_STATE_SERVICE)
    {
        uint8_t  subserviceNumber = (uint8_t)(accumulator & HYPERDOS_X86_LOW_BYTE_MASK);
        uint16_t requestedState   = (uint16_t)(counter & HYPERDOS_PC_VIDEO_BIOS_SAVE_RESTORE_STATE_MASK);

        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_SAVE_RESTORE_GET_BUFFER_SIZE)
        {
            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                   (uint16_t)((accumulator & 0xFF00u) |
                                                              HYPERDOS_PC_VIDEO_BIOS_SAVE_RESTORE_STATE_SERVICE));
            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_BASE,
                                                   videoBiosInterface->getSaveRestoreStateBufferBlocks(requestedState));
            hyperdos_pc_video_bios_set_carry_flag(processor, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_SAVE_RESTORE_SAVE_STATE)
        {
            int savedState = videoBiosInterface->writeSaveRestoreState(processor, userContext, requestedState, base);

            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                   (uint16_t)((accumulator & 0xFF00u) |
                                                              HYPERDOS_PC_VIDEO_BIOS_SAVE_RESTORE_STATE_SERVICE));
            hyperdos_pc_video_bios_set_carry_flag(processor, savedState ? 0 : 1);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_SAVE_RESTORE_RESTORE_STATE)
        {
            int restoredState = videoBiosInterface->restoreSaveRestoreState(processor,
                                                                            userContext,
                                                                            requestedState,
                                                                            base);

            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                   (uint16_t)((accumulator & 0xFF00u) |
                                                              HYPERDOS_PC_VIDEO_BIOS_SAVE_RESTORE_STATE_SERVICE));
            hyperdos_pc_video_bios_set_carry_flag(processor, restoredState ? 0 : 1);
            return HYPERDOS_X86_EXECUTION_OK;
        }

        hyperdos_pc_video_bios_trace(videoBiosInterface,
                                     "int10 save-restore unsupported subservice AL=%02X",
                                     subserviceNumber);
        hyperdos_pc_video_bios_set_carry_flag(processor, 1);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_READ_REGISTER_SERVICE)
    {
        uint8_t registerValue = 0u;

        if (videoBiosInterface->readRegisterInterfaceRegister(userContext,
                                                              data,
                                                              (uint8_t)(base & HYPERDOS_X86_LOW_BYTE_MASK),
                                                              &registerValue))
        {
            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_BASE,
                                                   (uint16_t)((base & 0xFF00u) | registerValue));
        }
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_WRITE_REGISTER_SERVICE)
    {
        uint8_t registerValue = videoBiosInterface->registerGroupIsSingleRegister(data)
                                        ? (uint8_t)(base & HYPERDOS_X86_LOW_BYTE_MASK)
                                        : (uint8_t)(base >> HYPERDOS_X86_BYTE_BIT_COUNT);

        (void)videoBiosInterface->writeRegisterInterfaceRegister(userContext,
                                                                 data,
                                                                 (uint8_t)(base & HYPERDOS_X86_LOW_BYTE_MASK),
                                                                 registerValue);
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_BASE,
                                               (uint16_t)((base & 0xFF00u) | registerValue));
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_READ_REGISTER_RANGE_SERVICE)
    {
        uint8_t  firstRegisterNumber        = (uint8_t)(counter >> HYPERDOS_X86_BYTE_BIT_COUNT);
        uint8_t  registerCount              = (uint8_t)(counter & HYPERDOS_X86_LOW_BYTE_MASK);
        uint32_t destinationPhysicalAddress = hyperdos_pc_video_bios_get_extra_segment_physical_address(processor,
                                                                                                        base);
        uint8_t  registerOffset             = 0u;

        for (registerOffset = 0u; registerOffset < registerCount; ++registerOffset)
        {
            uint8_t registerValue = 0u;

            if (!videoBiosInterface->readRegisterInterfaceRegister(userContext,
                                                                   data,
                                                                   (uint8_t)(firstRegisterNumber + registerOffset),
                                                                   &registerValue))
            {
                break;
            }
            hyperdos_pc_video_bios_write_guest_memory_byte(videoBiosInterface,
                                                           destinationPhysicalAddress + registerOffset,
                                                           registerValue);
        }
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_WRITE_REGISTER_RANGE_SERVICE)
    {
        uint8_t  firstRegisterNumber   = (uint8_t)(counter >> HYPERDOS_X86_BYTE_BIT_COUNT);
        uint8_t  registerCount         = (uint8_t)(counter & HYPERDOS_X86_LOW_BYTE_MASK);
        uint32_t sourcePhysicalAddress = hyperdos_pc_video_bios_get_extra_segment_physical_address(processor, base);
        uint8_t  registerOffset        = 0u;

        for (registerOffset = 0u; registerOffset < registerCount; ++registerOffset)
        {
            if (!videoBiosInterface->writeRegisterInterfaceRegister(
                        userContext,
                        data,
                        (uint8_t)(firstRegisterNumber + registerOffset),
                        hyperdos_pc_video_bios_read_guest_memory_byte(videoBiosInterface,
                                                                      sourcePhysicalAddress + registerOffset)))
            {
                break;
            }
        }
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_READ_REGISTER_SET_SERVICE)
    {
        uint32_t tablePhysicalAddress = hyperdos_pc_video_bios_get_extra_segment_physical_address(processor, base);
        uint16_t registerSetIndex     = 0u;

        for (registerSetIndex = 0u; registerSetIndex < counter; ++registerSetIndex)
        {
            uint32_t entryPhysicalAddress = tablePhysicalAddress +
                                            (uint32_t)registerSetIndex *
                                                    HYPERDOS_PC_VIDEO_BIOS_REGISTER_SET_ENTRY_BYTE_COUNT;
            uint16_t registerGroup  = hyperdos_pc_video_bios_read_guest_memory_word(videoBiosInterface,
                                                                                   entryPhysicalAddress);
            uint8_t  registerNumber = hyperdos_pc_video_bios_read_guest_memory_byte(videoBiosInterface,
                                                                                   entryPhysicalAddress + 2u);
            uint8_t  registerValue  = 0u;

            if (videoBiosInterface->readRegisterInterfaceRegister(userContext,
                                                                  registerGroup,
                                                                  registerNumber,
                                                                  &registerValue))
            {
                hyperdos_pc_video_bios_write_guest_memory_byte(videoBiosInterface,
                                                               entryPhysicalAddress + 3u,
                                                               registerValue);
            }
        }
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_WRITE_REGISTER_SET_SERVICE)
    {
        uint32_t tablePhysicalAddress = hyperdos_pc_video_bios_get_extra_segment_physical_address(processor, base);
        uint16_t registerSetIndex     = 0u;

        for (registerSetIndex = 0u; registerSetIndex < counter; ++registerSetIndex)
        {
            uint32_t entryPhysicalAddress = tablePhysicalAddress +
                                            (uint32_t)registerSetIndex *
                                                    HYPERDOS_PC_VIDEO_BIOS_REGISTER_SET_ENTRY_BYTE_COUNT;
            uint16_t registerGroup  = hyperdos_pc_video_bios_read_guest_memory_word(videoBiosInterface,
                                                                                   entryPhysicalAddress);
            uint8_t  registerNumber = hyperdos_pc_video_bios_read_guest_memory_byte(videoBiosInterface,
                                                                                   entryPhysicalAddress + 2u);
            uint8_t  registerValue  = hyperdos_pc_video_bios_read_guest_memory_byte(videoBiosInterface,
                                                                                  entryPhysicalAddress + 3u);

            (void)videoBiosInterface->writeRegisterInterfaceRegister(userContext,
                                                                     registerGroup,
                                                                     registerNumber,
                                                                     registerValue);
        }
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_GET_VERSION_SERVICE)
    {
        hyperdos_x86_set_segment_register(processor,
                                          HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                          HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_VERSION_SEGMENT);
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_BASE,
                                               HYPERDOS_PC_VIDEO_BIOS_REGISTER_INTERFACE_VERSION_OFFSET);
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_ORIGINAL_EQUIPMENT_MANUFACTURER_EXTENSION_SERVICE)
    {
        hyperdos_pc_video_bios_return_unsupported_service(processor,
                                                          videoBiosInterface,
                                                          serviceNumber,
                                                          accumulator,
                                                          "int10 original-equipment-manufacturer extension "
                                                          "unsupported");
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_ADAPTER_EXTENSION_SERVICE)
    {
        hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, 0u);
        hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE, 0u);
        hyperdos_pc_video_bios_trace(videoBiosInterface, "int10 adapter extension absent AH=%02X", serviceNumber);
        hyperdos_pc_video_bios_set_carry_flag(processor, 1);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_KOREAN_EXTENSION_STATUS_SERVICE)
    {
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                               (uint16_t)(accumulator & 0xFF00u));
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_KOREAN_EXTENSION_CHARACTER_SHAPE_SERVICE)
    {
        uint8_t subserviceNumber = (uint8_t)(accumulator & HYPERDOS_X86_LOW_BYTE_MASK);

        if (subserviceNumber == HYPERDOS_PC_VIDEO_BIOS_KOREAN_EXTENSION_CHARACTER_SHAPE_SUBSERVICE)
        {
            videoBiosInterface->writeKoreanExtensionPlaceholderCharacterShape(processor, userContext, base);
        }
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_VIDEO_BIOS_KOREAN_EXTENSION_OUTPUT_SERVICE)
    {
        hyperdos_pc_video_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }

    hyperdos_pc_video_bios_return_unsupported_service(processor,
                                                      videoBiosInterface,
                                                      serviceNumber,
                                                      accumulator,
                                                      "int10 unhandled service");
    return HYPERDOS_X86_EXECUTION_OK;
}
