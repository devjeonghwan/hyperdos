#include "hyperdos/pc_video_services.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "hyperdos/pc_bios_data_area.h"
#include "hyperdos/pc_firmware.h"


enum
{
    HYPERDOS_PC_VIDEO_SERVICES_INTERRUPT_VECTOR_BYTE_COUNT                                                 = 4u,
    HYPERDOS_PC_VIDEO_SERVICES_TEXT_ATTRIBUTE                                                              = 0x07u,
    HYPERDOS_PC_VIDEO_SERVICES_TRACE_TEXT_CAPACITY                                                         = 512u,
    HYPERDOS_X86_BIOS_DATA_AREA_ACTIVE_VIDEO_PAGE_OFFSET                                                   = 0x0062u,
    HYPERDOS_X86_BIOS_DATA_AREA_CHARACTER_HEIGHT_OFFSET                                                    = 0x0085u,
    HYPERDOS_X86_BIOS_DATA_AREA_CRT_CONTROLLER_PORT_OFFSET                                                 = 0x0063u,
    HYPERDOS_X86_BIOS_DATA_AREA_CURRENT_VIDEO_MODE_OFFSET                                                  = 0x0049u,
    HYPERDOS_X86_BIOS_DATA_AREA_CURSOR_SHAPE_OFFSET                                                        = 0x0060u,
    HYPERDOS_X86_BIOS_DATA_AREA_DISPLAY_COMBINATION_CODE_INDEX_OFFSET                                      = 0x008Au,
    HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_COLOR_SELECT_OFFSET                                                  = 0x0066u,
    HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_COLUMNS_OFFSET                                                       = 0x004Au,
    HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_CONTROL_OFFSET                                                       = 0x0087u,
    HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_MODE_CONTROL_OFFSET                                                  = 0x0065u,
    HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_MODE_SET_CONTROL_OFFSET                                              = 0x0089u,
    HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_PAGE_SIZE_OFFSET                                                     = 0x004Cu,
    HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_PAGE_START_OFFSET                                                    = 0x004Eu,
    HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_ROWS_MINUS_ONE_OFFSET                                                = 0x0084u,
    HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_SWITCHES_OFFSET                                                      = 0x0088u,
    HYPERDOS_X86_BIOS_GRAPHICS_CHARACTER_TABLE_INTERRUPT                                                   = 0x1Fu,
    HYPERDOS_X86_BIOS_PRINT_SCREEN_INTERRUPT                                                               = 0x05u,
    HYPERDOS_X86_BIOS_VIDEO_CHARACTER_GENERATOR_INTERRUPT                                                  = 0x43u,
    HYPERDOS_X86_BIOS_VIDEO_CHARACTER_HEIGHT                                                               = 16u,
    HYPERDOS_X86_BIOS_VIDEO_CONTROL_DEFAULT                                                                = 0x60u,
    HYPERDOS_X86_BIOS_VIDEO_CRT_CONTROLLER_PORT                                                            = 0x03D4u,
    HYPERDOS_X86_BIOS_VIDEO_DISPLAY_COMBINATION_CODE_NO_DISPLAY                                            = 0x00u,
    HYPERDOS_X86_BIOS_VIDEO_DISPLAY_COMBINATION_CODE_VIDEO_GRAPHICS_ARRAY_COLOR                            = 0x08u,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_ACTIVE_DISPLAY_CODE_OFFSET                                       = 0x25u,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_ACTIVE_PAGE_OFFSET                                               = 0x1Du,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_BUFFER_SIZE_OFFSET                                               = 0x07u,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_BUFFER_START_OFFSET                                              = 0x09u,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_CHARACTER_HEIGHT_OFFSET                                          = 0x23u,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_COLOR_COUNT_OFFSET                                               = 0x27u,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_COLOR_SELECT_OFFSET                                              = 0x21u,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_COLUMN_COUNT_OFFSET                                              = 0x05u,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_CRT_CONTROLLER_PORT_OFFSET                                       = 0x1Eu,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_CURSOR_END_LINE_OFFSET                                           = 0x1Bu,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_CURSOR_START_LINE_OFFSET                                         = 0x1Cu,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_CURSOR_TABLE_BYTE_COUNT                                          = 16u,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_CURSOR_TABLE_OFFSET                                              = 0x0Bu,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_FIRST_TEXT_CHARACTER_TABLE_OFFSET                                = 0x2Bu,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_FLAGS_OFFSET                                                     = 0x2Du,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_INACTIVE_DISPLAY_CODE_OFFSET                                     = 0x26u,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_MAXIMUM_PAGE_NUMBER_OFFSET                                       = 0x29u,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_MEMORY_SIZE_OFFSET                                               = 0x31u,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_MODE_CONTROL_OFFSET                                              = 0x20u,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_MODE_OFFSET                                                      = 0x04u,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_RASTER_SCAN_LINES_OFFSET                                         = 0x2Au,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_ROWS_MINUS_ONE_OFFSET                                            = 0x22u,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_SAVE_AREA_STATUS_OFFSET                                          = 0x32u,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_SECOND_TEXT_CHARACTER_TABLE_OFFSET                               = 0x2Cu,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_STATIC_TABLE_OFFSET                                              = 0x00u,
    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_TABLE_BYTE_COUNT                                                 = 64u,
    HYPERDOS_X86_BIOS_VIDEO_GRAPHICS_PAGE_SIZE                                                             = 0x4000u,
    HYPERDOS_X86_BIOS_VIDEO_KOREAN_EXTENSION_CHARACTER_SHAPE_ROW_COUNT                                     = 16u,
    HYPERDOS_X86_BIOS_VIDEO_MAXIMUM_TEXT_PAGE_NUMBER                                                       = 0x07u,
    HYPERDOS_X86_BIOS_VIDEO_MEMORY_SIZE_256_KILOBYTES                                                      = 0x03u,
    HYPERDOS_X86_BIOS_VIDEO_MODE_SET_CONTROL_200_LINE_BIT                                                  = 0x80u,
    HYPERDOS_X86_BIOS_VIDEO_PARAMETER_TABLE_INTERRUPT                                                      = 0x1Du,
    HYPERDOS_X86_BIOS_VIDEO_PIXEL_XOR_MASK                                                                 = 0x80u,
    HYPERDOS_X86_BIOS_VIDEO_RASTER_SCAN_LINES_200                                                          = 0x00u,
    HYPERDOS_X86_BIOS_VIDEO_RASTER_SCAN_LINES_350                                                          = 0x01u,
    HYPERDOS_X86_BIOS_VIDEO_RASTER_SCAN_LINES_400                                                          = 0x02u,
    HYPERDOS_X86_BIOS_VIDEO_RASTER_SCAN_LINES_480                                                          = 0x03u,
    HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_ATTRIBUTE_CONTROLLER_GROUP                                  = 0x18u,
    HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_CRT_CONTROLLER_GROUP                                        = 0x00u,
    HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_FEATURE_CONTROL_GROUP                                       = 0x28u,
    HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_GRAPHICS_CONTROLLER_GROUP                                   = 0x10u,
    HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_GRAPHICS_ONE_POSITION_GROUP                                 = 0x30u,
    HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_GRAPHICS_TWO_POSITION_GROUP                                 = 0x38u,
    HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_MISCELLANEOUS_OUTPUT_GROUP                                  = 0x20u,
    HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_SEQUENCER_GROUP                                             = 0x08u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_BYTE_COUNT                              = 0x3Au,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_CHARACTER_GENERATOR_VECTOR_OFFSET       = 0x36u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_DATA                                    = 0x0002u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_DISPLAY_COMBINATION_CODE_POINTER_OFFSET = 0x26u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_EQUIPMENT_OFFSET                        = 0x00u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_EXTENDED_VIDEO_DATA_BYTE_COUNT          = 0x07u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_EXTENDED_VIDEO_DATA_OFFSET              = 0x1Fu,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_GRAPHICS_CHARACTER_VECTOR_OFFSET        = 0x32u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_POINTER_OFFSET                          = 0x02u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_PRINT_SCREEN_VECTOR_OFFSET              = 0x2Au,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_VIDEO_DATA_BYTE_COUNT                   = 0x1Eu,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_VIDEO_DATA_OFFSET                       = 0x01u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_VIDEO_PARAMETER_VECTOR_OFFSET           = 0x2Eu,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BLOCK_BYTE_COUNT                                                  = 64u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_BYTE_COUNT                            = 0x304u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_COLOR_SELECT_OFFSET                   = 0x303u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_INDEX_OFFSET                          = 0x001u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_MASK_OFFSET                           = 0x002u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_PALETTE_OFFSET                        = 0x003u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_POINTER_OFFSET                        = 0x04u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_STATE                                 = 0x0004u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_STATE_OFFSET                          = 0x000u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_ATTRIBUTE_CONTROLLER_INDEX_OFFSET                        = 0x03u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_ATTRIBUTE_MODE_REGISTER_OFFSET                           = 0x33u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_ATTRIBUTE_PALETTE_REGISTER_OFFSET                        = 0x23u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_BYTE_COUNT                                               = 0x46u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_CATHODE_RAY_TUBE_CONTROLLER_INDEX_OFFSET                 = 0x01u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_CATHODE_RAY_TUBE_CONTROLLER_PORT_OFFSET                  = 0x40u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_CATHODE_RAY_TUBE_CONTROLLER_REGISTER_OFFSET              = 0x0Au,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_FEATURE_CONTROL_OFFSET                                   = 0x04u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_GRAPHICS_CONTROLLER_INDEX_OFFSET                         = 0x02u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_GRAPHICS_CONTROLLER_REGISTER_OFFSET                      = 0x37u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_GRAPHICS_LATCH_OFFSET                                    = 0x42u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_MISCELLANEOUS_OUTPUT_OFFSET                              = 0x09u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_POINTER_OFFSET                                           = 0x00u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_SEQUENCER_INDEX_OFFSET                                   = 0x00u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_SEQUENCER_REGISTER_OFFSET                                = 0x05u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_STATE                                                    = 0x0001u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HEADER_BYTE_COUNT                                                 = 0x20u,
    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_STATE_MASK                                                        = 0x0007u,
    HYPERDOS_X86_BIOS_VIDEO_STATE_BLINKING_ATTRIBUTE_ENABLED                                               = 0x20u,
    HYPERDOS_X86_BIOS_VIDEO_STATE_CURSOR_EMULATION_ENABLED                                                 = 0x10u,
    HYPERDOS_X86_BIOS_VIDEO_STATIC_ADDITIONAL_CAPABILITY_FLAGS                                             = 0x0Cu,
    HYPERDOS_X86_BIOS_VIDEO_STATIC_CAPABILITY_FLAGS                                                        = 0x7Cu,
    HYPERDOS_X86_BIOS_VIDEO_STATIC_CHARACTER_TABLE_COUNT                                                   = 0x04u,
    HYPERDOS_X86_BIOS_VIDEO_STATIC_FUNCTIONALITY_TABLE_BYTE_COUNT                                          = 16u,
    HYPERDOS_X86_BIOS_VIDEO_STATIC_FUNCTIONALITY_TABLE_OFFSET                                              = 0x0120u,
    HYPERDOS_X86_BIOS_VIDEO_STATIC_FUNCTIONALITY_TABLE_SEGMENT                                             = 0xF000u,
    HYPERDOS_X86_BIOS_VIDEO_STATIC_MAXIMUM_CHARACTER_SETS                                                  = 0x08u,
    HYPERDOS_X86_BIOS_VIDEO_STATIC_SUPPORTED_MODES_HIGH                                                    = 0x0Fu,
    HYPERDOS_X86_BIOS_VIDEO_STATIC_SUPPORTED_MODES_LOW                                                     = 0xFFu,
    HYPERDOS_X86_BIOS_VIDEO_STATIC_SUPPORTED_MODES_MIDDLE                                                  = 0xE0u,
    HYPERDOS_X86_BIOS_VIDEO_STATIC_SUPPORTED_SCAN_LINES                                                    = 0x07u,
    HYPERDOS_X86_BIOS_VIDEO_SWITCHES_DEFAULT                                                               = 0xF9u,
    HYPERDOS_X86_BIOS_VIDEO_TEXT_PAGE_SIZE                                                                 = 0x1000u,
    HYPERDOS_X86_BIOS_VIDEO_WRITE_STRING_ATTRIBUTE_BIT                                                     = 0x02u,
    HYPERDOS_X86_BIOS_VIDEO_WRITE_STRING_UPDATE_CURSOR_BIT                                                 = 0x01u
};


static void hyperdos_pc_video_services_trace(hyperdos_pc_video_services* videoServices, const char* format, ...);

static void write_guest_memory_byte(hyperdos_pc_video_services* videoServices, uint32_t physicalAddress, uint8_t value);

static void write_guest_memory_word(hyperdos_pc_video_services* videoServices,
                                    uint32_t                    physicalAddress,
                                    uint16_t                    value);

static void write_guest_memory_double_word(hyperdos_pc_video_services* videoServices,
                                           uint32_t                    physicalAddress,
                                           uint32_t                    value);

static uint8_t read_guest_memory_byte(const hyperdos_pc_video_services* videoServices, uint32_t physicalAddress);

static uint16_t read_guest_memory_word(const hyperdos_pc_video_services* videoServices, uint32_t physicalAddress);

static uint32_t read_guest_memory_double_word(const hyperdos_pc_video_services* videoServices,
                                              uint32_t                          physicalAddress);

static uint16_t get_text_cursor_position_for_page(hyperdos_pc_video_services* videoServices, uint8_t videoPage);

static void hyperdos_pc_video_services_trace(hyperdos_pc_video_services* videoServices, const char* format, ...)
{
    char    message[HYPERDOS_PC_VIDEO_SERVICES_TRACE_TEXT_CAPACITY];
    va_list arguments;

    if (videoServices == NULL || videoServices->traceFunction == NULL)
    {
        return;
    }

    va_start(arguments, format);
    (void)vsnprintf(message, sizeof(message), format, arguments);
    va_end(arguments);
    videoServices->traceFunction(videoServices->traceUserContext, message);
}

static uint8_t read_guest_memory_byte(const hyperdos_pc_video_services* videoServices, uint32_t physicalAddress)
{
    return hyperdos_bus_read_memory_byte_or_open_bus(&videoServices->pc->bus, physicalAddress);
}

static uint16_t read_guest_memory_word(const hyperdos_pc_video_services* videoServices, uint32_t physicalAddress)
{
    uint16_t lowByte  = read_guest_memory_byte(videoServices, physicalAddress);
    uint16_t highByte = read_guest_memory_byte(videoServices, physicalAddress + 1u);

    return (uint16_t)(lowByte | (highByte << HYPERDOS_X86_BYTE_BIT_COUNT));
}

static uint32_t read_guest_memory_double_word(const hyperdos_pc_video_services* videoServices, uint32_t physicalAddress)
{
    uint32_t lowWord  = read_guest_memory_word(videoServices, physicalAddress);
    uint32_t highWord = read_guest_memory_word(videoServices, physicalAddress + HYPERDOS_X86_WORD_SIZE);

    return lowWord | (highWord << 16u);
}

static void write_guest_memory_byte(hyperdos_pc_video_services* videoServices, uint32_t physicalAddress, uint8_t value)
{
    hyperdos_bus_write_memory_byte_if_mapped(&videoServices->pc->bus, physicalAddress, value);
}

static void write_guest_memory_word(hyperdos_pc_video_services* videoServices, uint32_t physicalAddress, uint16_t value)
{
    write_guest_memory_byte(videoServices, physicalAddress, (uint8_t)(value & HYPERDOS_X86_LOW_BYTE_MASK));
    write_guest_memory_byte(videoServices, physicalAddress + 1u, (uint8_t)(value >> HYPERDOS_X86_BYTE_BIT_COUNT));
}

static void write_guest_memory_double_word(hyperdos_pc_video_services* videoServices,
                                           uint32_t                    physicalAddress,
                                           uint32_t                    value)
{
    write_guest_memory_word(videoServices, physicalAddress, (uint16_t)(value & 0xFFFFu));
    write_guest_memory_word(videoServices, physicalAddress + HYPERDOS_X86_WORD_SIZE, (uint16_t)(value >> 16u));
}

void hyperdos_pc_video_services_initialize(hyperdos_pc_video_services*               videoServices,
                                           hyperdos_pc*                              pc,
                                           hyperdos_pc_video_services_trace_function traceFunction,
                                           void*                                     traceUserContext)
{
    if (videoServices == NULL)
    {
        return;
    }

    memset(videoServices, 0, sizeof(*videoServices));
    videoServices->pc               = pc;
    videoServices->traceFunction    = traceFunction;
    videoServices->traceUserContext = traceUserContext;
    videoServices->videoMode        = 0x03u;
    videoServices->videoColumnCount = HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT;
}

uint8_t hyperdos_pc_video_services_read_text_memory_byte(const hyperdos_pc_video_services* videoServices,
                                                         size_t                            memoryIndex)
{
    return videoServices->pc->colorGraphicsAdapter.memory[memoryIndex];
}

static void write_text_memory_byte(hyperdos_pc_video_services* videoServices, size_t memoryIndex, uint8_t value)
{
    videoServices->pc->colorGraphicsAdapter.memory[memoryIndex] = value;
}

static void update_bios_cursor_position(hyperdos_pc_video_services* videoServices)
{
    hyperdos_pc_bios_data_area_write_word(videoServices->pc,
                                          hyperdos_pc_bios_data_area_get_cursor_position_offset(
                                                  videoServices->activeVideoPage),
                                          (uint16_t)((videoServices->cursorRow << HYPERDOS_X86_BYTE_BIT_COUNT) |
                                                     videoServices->cursorColumn));
}

static uint16_t get_current_bios_video_page_size(const hyperdos_pc_video_services* videoServices)
{
    const hyperdos_color_graphics_adapter* adapter = &videoServices->pc->colorGraphicsAdapter;

    if (videoServices->videoMode == 0x13u)
    {
        return 0xFA00u;
    }
    if (videoServices->videoMode == 0x11u || videoServices->videoMode == 0x12u)
    {
        return 0x9600u;
    }
    if (videoServices->videoMode == 0x0Fu || videoServices->videoMode == 0x10u)
    {
        return 0x6D60u;
    }
    if (videoServices->videoMode == 0x0Du)
    {
        return 0x1F40u;
    }
    if (videoServices->videoMode == 0x0Eu)
    {
        return 0x3E80u;
    }
    if ((adapter->modeControl & HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_GRAPHICS) != 0u)
    {
        return HYPERDOS_X86_BIOS_VIDEO_GRAPHICS_PAGE_SIZE;
    }
    return HYPERDOS_X86_BIOS_VIDEO_TEXT_PAGE_SIZE;
}

static void update_bios_video_state(hyperdos_pc_video_services* videoServices)
{
    hyperdos_color_graphics_adapter* adapter        = &videoServices->pc->colorGraphicsAdapter;
    uint16_t                         videoPageStart = (uint16_t)(videoServices->activeVideoPage *
                                         get_current_bios_video_page_size(videoServices));

    hyperdos_pc_bios_data_area_write_byte(videoServices->pc,
                                          HYPERDOS_X86_BIOS_DATA_AREA_CURRENT_VIDEO_MODE_OFFSET,
                                          videoServices->videoMode);
    hyperdos_pc_bios_data_area_write_word(videoServices->pc,
                                          HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_COLUMNS_OFFSET,
                                          videoServices->videoColumnCount);
    hyperdos_pc_bios_data_area_write_word(videoServices->pc,
                                          HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_PAGE_SIZE_OFFSET,
                                          get_current_bios_video_page_size(videoServices));
    hyperdos_pc_bios_data_area_write_word(videoServices->pc,
                                          HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_PAGE_START_OFFSET,
                                          videoPageStart);
    hyperdos_pc_bios_data_area_write_word(videoServices->pc, HYPERDOS_X86_BIOS_DATA_AREA_CURSOR_SHAPE_OFFSET, 0x0607u);
    hyperdos_pc_bios_data_area_write_byte(videoServices->pc,
                                          HYPERDOS_X86_BIOS_DATA_AREA_ACTIVE_VIDEO_PAGE_OFFSET,
                                          videoServices->activeVideoPage);
    hyperdos_pc_bios_data_area_write_word(videoServices->pc,
                                          HYPERDOS_X86_BIOS_DATA_AREA_CRT_CONTROLLER_PORT_OFFSET,
                                          HYPERDOS_X86_BIOS_VIDEO_CRT_CONTROLLER_PORT);
    hyperdos_pc_bios_data_area_write_byte(videoServices->pc,
                                          HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_MODE_CONTROL_OFFSET,
                                          adapter->modeControl);
    hyperdos_pc_bios_data_area_write_byte(videoServices->pc,
                                          HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_COLOR_SELECT_OFFSET,
                                          adapter->colorSelect);
    hyperdos_pc_bios_data_area_write_byte(videoServices->pc,
                                          HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_ROWS_MINUS_ONE_OFFSET,
                                          HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT - 1u);
    hyperdos_pc_bios_data_area_write_word(videoServices->pc,
                                          HYPERDOS_X86_BIOS_DATA_AREA_CHARACTER_HEIGHT_OFFSET,
                                          HYPERDOS_X86_BIOS_VIDEO_CHARACTER_HEIGHT);
    hyperdos_pc_bios_data_area_write_byte(videoServices->pc,
                                          HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_CONTROL_OFFSET,
                                          HYPERDOS_X86_BIOS_VIDEO_CONTROL_DEFAULT);
    hyperdos_pc_bios_data_area_write_byte(videoServices->pc,
                                          HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_SWITCHES_OFFSET,
                                          HYPERDOS_X86_BIOS_VIDEO_SWITCHES_DEFAULT);
    hyperdos_pc_bios_data_area_write_byte(
            videoServices->pc,
            HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_MODE_SET_CONTROL_OFFSET,
            (uint8_t)(hyperdos_pc_bios_data_area_read_byte(videoServices->pc,
                                                           HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_MODE_SET_CONTROL_OFFSET) &
                      (uint8_t)~HYPERDOS_X86_BIOS_VIDEO_MODE_SET_CONTROL_200_LINE_BIT));
    hyperdos_pc_bios_data_area_write_byte(videoServices->pc,
                                          HYPERDOS_X86_BIOS_DATA_AREA_DISPLAY_COMBINATION_CODE_INDEX_OFFSET,
                                          HYPERDOS_X86_BIOS_VIDEO_DISPLAY_COMBINATION_CODE_VIDEO_GRAPHICS_ARRAY_COLOR);
    update_bios_cursor_position(videoServices);
}

static void clear_text_screen(hyperdos_pc_video_services* videoServices, uint8_t attribute)
{
    size_t cellIndex = 0;

    for (cellIndex = 0;
         cellIndex < HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT * HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT;
         ++cellIndex)
    {
        size_t memoryIndex = cellIndex * HYPERDOS_COLOR_GRAPHICS_ADAPTER_CELL_BYTE_COUNT;
        write_text_memory_byte(videoServices, memoryIndex, ' ');
        write_text_memory_byte(videoServices, memoryIndex + 1u, attribute);
    }
    videoServices->cursorRow    = 0u;
    videoServices->cursorColumn = 0u;
    update_bios_cursor_position(videoServices);
}

static void clear_graphics_memory(hyperdos_pc_video_services* videoServices)
{
    memset(videoServices->pc->colorGraphicsAdapter.memory, 0, sizeof(videoServices->pc->colorGraphicsAdapter.memory));
    memset(videoServices->pc->colorGraphicsAdapter.graphicsPlanes,
           0,
           sizeof(videoServices->pc->colorGraphicsAdapter.graphicsPlanes));
}

static void scroll_text_region(hyperdos_pc_video_services* videoServices,
                               uint16_t                    firstRow,
                               uint16_t                    firstColumn,
                               uint16_t                    lastRow,
                               uint16_t                    lastColumn,
                               uint8_t                     lineCount,
                               uint8_t                     attribute,
                               int                         scrollDown)
{
    uint16_t row    = 0;
    uint16_t column = 0;

    if (firstRow >= HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT)
    {
        firstRow = HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT - 1u;
    }
    if (lastRow >= HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT)
    {
        lastRow = HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT - 1u;
    }
    if (firstColumn >= HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT)
    {
        firstColumn = HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT - 1u;
    }
    if (lastColumn >= HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT)
    {
        lastColumn = HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT - 1u;
    }
    if (firstRow > lastRow || firstColumn > lastColumn)
    {
        return;
    }
    if (lineCount == 0u || lineCount > (uint8_t)(lastRow - firstRow + 1u))
    {
        lineCount = (uint8_t)(lastRow - firstRow + 1u);
    }

    if (scrollDown)
    {
        for (row = lastRow; row >= firstRow; --row)
        {
            for (column = firstColumn; column <= lastColumn; ++column)
            {
                size_t destinationIndex =
                        hyperdos_pc_video_services_get_text_page_cell_memory_index(videoServices,
                                                                                   videoServices->activeVideoPage,
                                                                                   row,
                                                                                   column);
                if (row >= firstRow + lineCount)
                {
                    size_t sourceIndex =
                            hyperdos_pc_video_services_get_text_page_cell_memory_index(videoServices,
                                                                                       videoServices->activeVideoPage,
                                                                                       (uint16_t)(row - lineCount),
                                                                                       column);
                    write_text_memory_byte(videoServices,
                                           destinationIndex,
                                           hyperdos_pc_video_services_read_text_memory_byte(videoServices,
                                                                                            sourceIndex));
                    write_text_memory_byte(videoServices,
                                           destinationIndex + 1u,
                                           hyperdos_pc_video_services_read_text_memory_byte(videoServices,
                                                                                            sourceIndex + 1u));
                }
                else
                {
                    write_text_memory_byte(videoServices, destinationIndex, ' ');
                    write_text_memory_byte(videoServices, destinationIndex + 1u, attribute);
                }
            }
            if (row == 0u)
            {
                break;
            }
        }
        return;
    }

    for (row = firstRow; row <= lastRow; ++row)
    {
        for (column = firstColumn; column <= lastColumn; ++column)
        {
            size_t destinationIndex =
                    hyperdos_pc_video_services_get_text_page_cell_memory_index(videoServices,
                                                                               videoServices->activeVideoPage,
                                                                               row,
                                                                               column);
            if (row + lineCount <= lastRow)
            {
                size_t sourceIndex =
                        hyperdos_pc_video_services_get_text_page_cell_memory_index(videoServices,
                                                                                   videoServices->activeVideoPage,
                                                                                   (uint16_t)(row + lineCount),
                                                                                   column);
                write_text_memory_byte(videoServices,
                                       destinationIndex,
                                       hyperdos_pc_video_services_read_text_memory_byte(videoServices, sourceIndex));
                write_text_memory_byte(videoServices,
                                       destinationIndex + 1u,
                                       hyperdos_pc_video_services_read_text_memory_byte(videoServices,
                                                                                        sourceIndex + 1u));
            }
            else
            {
                write_text_memory_byte(videoServices, destinationIndex, ' ');
                write_text_memory_byte(videoServices, destinationIndex + 1u, attribute);
            }
        }
    }
}

static void set_active_video_page(hyperdos_pc_video_services* videoServices, uint8_t videoPage)
{
    uint16_t cursorPosition = 0u;

    if (videoPage > HYPERDOS_X86_BIOS_VIDEO_MAXIMUM_TEXT_PAGE_NUMBER)
    {
        return;
    }

    videoServices->activeVideoPage = videoPage;
    hyperdos_pc_bios_data_area_write_byte(videoServices->pc,
                                          HYPERDOS_X86_BIOS_DATA_AREA_ACTIVE_VIDEO_PAGE_OFFSET,
                                          videoPage);
    hyperdos_pc_bios_data_area_write_word(videoServices->pc,
                                          HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_PAGE_START_OFFSET,
                                          (uint16_t)(videoPage * get_current_bios_video_page_size(videoServices)));

    cursorPosition              = hyperdos_pc_bios_data_area_read_word(videoServices->pc,
                                                          hyperdos_pc_bios_data_area_get_cursor_position_offset(
                                                                  videoPage));
    videoServices->cursorRow    = (uint16_t)(cursorPosition >> HYPERDOS_X86_BYTE_BIT_COUNT);
    videoServices->cursorColumn = (uint16_t)(cursorPosition & HYPERDOS_X86_LOW_BYTE_MASK);
}

static void set_text_cursor_from_row_column_for_page(hyperdos_pc_video_services* videoServices,
                                                     uint8_t                     videoPage,
                                                     uint16_t                    row,
                                                     uint16_t                    column)
{
    if (row >= HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT)
    {
        row = HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT - 1u;
    }
    if (column >= videoServices->videoColumnCount)
    {
        column = (uint16_t)(videoServices->videoColumnCount - 1u);
    }
    if (videoPage <= HYPERDOS_X86_BIOS_VIDEO_MAXIMUM_TEXT_PAGE_NUMBER)
    {
        hyperdos_pc_bios_data_area_write_word(videoServices->pc,
                                              hyperdos_pc_bios_data_area_get_cursor_position_offset(videoPage),
                                              (uint16_t)((row << HYPERDOS_X86_BYTE_BIT_COUNT) | column));
    }
    if (videoPage == videoServices->activeVideoPage)
    {
        videoServices->cursorRow    = row;
        videoServices->cursorColumn = column;
    }
}

static void set_text_cursor_from_row_column(hyperdos_pc_video_services* videoServices, uint16_t row, uint16_t column)
{
    set_text_cursor_from_row_column_for_page(videoServices, videoServices->activeVideoPage, row, column);
}

static void advance_text_cursor(hyperdos_pc_video_services* videoServices)
{
    ++videoServices->cursorColumn;
    if (videoServices->cursorColumn >= videoServices->videoColumnCount)
    {
        videoServices->cursorColumn = 0u;
        ++videoServices->cursorRow;
    }
    if (videoServices->cursorRow >= HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT)
    {
        scroll_text_region(videoServices,
                           0u,
                           0u,
                           HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT - 1u,
                           (uint16_t)(videoServices->videoColumnCount - 1u),
                           1u,
                           HYPERDOS_PC_VIDEO_SERVICES_TEXT_ATTRIBUTE,
                           0);
        videoServices->cursorRow = HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT - 1u;
    }
    update_bios_cursor_position(videoServices);
}

static void write_text_character_at_cursor(hyperdos_pc_video_services* videoServices,
                                           uint8_t                     character,
                                           uint8_t                     attribute)
{
    size_t memoryIndex = hyperdos_pc_video_services_get_text_page_cell_memory_index(videoServices,
                                                                                    videoServices->activeVideoPage,
                                                                                    videoServices->cursorRow,
                                                                                    videoServices->cursorColumn);

    write_text_memory_byte(videoServices, memoryIndex, character);
    write_text_memory_byte(videoServices, memoryIndex + 1u, attribute);
}

static void write_teletype_character(hyperdos_pc_video_services* videoServices, uint8_t character, uint8_t attribute)
{
    if (character == '\r')
    {
        videoServices->cursorColumn = 0u;
        update_bios_cursor_position(videoServices);
        return;
    }
    if (character == '\n')
    {
        ++videoServices->cursorRow;
        if (videoServices->cursorRow >= HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT)
        {
            scroll_text_region(videoServices,
                               0u,
                               0u,
                               HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT - 1u,
                               (uint16_t)(videoServices->videoColumnCount - 1u),
                               1u,
                               attribute,
                               0);
            videoServices->cursorRow = HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT - 1u;
        }
        update_bios_cursor_position(videoServices);
        return;
    }
    if (character == '\b')
    {
        if (videoServices->cursorColumn > 0u)
        {
            --videoServices->cursorColumn;
        }
        update_bios_cursor_position(videoServices);
        return;
    }

    write_text_character_at_cursor(videoServices, character, attribute);
    advance_text_cursor(videoServices);
}

static void write_character_repeated(hyperdos_pc_video_services* videoServices,
                                     uint8_t                     videoPage,
                                     uint8_t                     character,
                                     uint8_t                     attribute,
                                     uint16_t                    count,
                                     int                         updateAttribute)
{
    uint16_t characterIndex = 0;
    uint16_t cursorPosition = get_text_cursor_position_for_page(videoServices, videoPage);
    uint16_t row            = (uint16_t)(cursorPosition >> HYPERDOS_X86_BYTE_BIT_COUNT);
    uint16_t column         = (uint16_t)(cursorPosition & HYPERDOS_X86_LOW_BYTE_MASK);

    for (characterIndex = 0; characterIndex < count; ++characterIndex)
    {
        size_t memoryIndex = hyperdos_pc_video_services_get_text_page_cell_memory_index(videoServices,
                                                                                        videoPage,
                                                                                        row,
                                                                                        column);

        write_text_memory_byte(videoServices, memoryIndex, character);
        if (updateAttribute)
        {
            write_text_memory_byte(videoServices, memoryIndex + 1u, attribute);
        }
        ++column;
        if (column >= videoServices->videoColumnCount)
        {
            column = 0u;
            ++row;
            if (row >= HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT)
            {
                break;
            }
        }
    }
}

void hyperdos_pc_video_services_set_video_mode(hyperdos_pc_video_services* videoServices, uint8_t mode)
{
    hyperdos_color_graphics_adapter* adapter            = &videoServices->pc->colorGraphicsAdapter;
    uint8_t                          modeNumber         = (uint8_t)(mode & 0x7Fu);
    int                              clearDisplayMemory = (mode & 0x80u) == 0u;

    videoServices->videoMode       = modeNumber;
    videoServices->activeVideoPage = 0u;
    videoServices->cursorRow       = 0u;
    videoServices->cursorColumn    = 0u;
    hyperdos_color_graphics_adapter_set_bios_video_mode(adapter, modeNumber);
    hyperdos_pc_video_services_trace(videoServices,
                                     "int10 set video mode requested=%02X mode=%02X clear=%u",
                                     mode,
                                     modeNumber,
                                     clearDisplayMemory ? 1u : 0u);
    switch (modeNumber)
    {
    case 0x00u:
    case 0x01u:
        videoServices->videoColumnCount = 40u;
        if (clearDisplayMemory)
        {
            clear_text_screen(videoServices, HYPERDOS_PC_VIDEO_SERVICES_TEXT_ATTRIBUTE);
        }
        break;
    case 0x04u:
    case 0x05u:
        videoServices->videoColumnCount = 40u;
        if (clearDisplayMemory)
        {
            clear_graphics_memory(videoServices);
        }
        break;
    case 0x06u:
        videoServices->videoColumnCount = 80u;
        if (clearDisplayMemory)
        {
            clear_graphics_memory(videoServices);
        }
        break;
    case 0x0Du:
        videoServices->videoColumnCount = 40u;
        if (clearDisplayMemory)
        {
            clear_graphics_memory(videoServices);
        }
        break;
    case 0x0Eu:
    case 0x0Fu:
    case 0x10u:
    case 0x11u:
    case 0x12u:
        videoServices->videoColumnCount = 80u;
        if (clearDisplayMemory)
        {
            clear_graphics_memory(videoServices);
        }
        break;
    case 0x13u:
        videoServices->videoColumnCount = 40u;
        if (clearDisplayMemory)
        {
            clear_graphics_memory(videoServices);
        }
        break;
    case 0x02u:
    case 0x03u:
        videoServices->videoColumnCount = 80u;
        if (clearDisplayMemory)
        {
            clear_text_screen(videoServices, HYPERDOS_PC_VIDEO_SERVICES_TEXT_ATTRIBUTE);
        }
        break;
    default:
        videoServices->videoMode = 0x03u;
        hyperdos_color_graphics_adapter_set_bios_video_mode(adapter, videoServices->videoMode);
        videoServices->videoColumnCount = 80u;
        if (clearDisplayMemory)
        {
            clear_text_screen(videoServices, HYPERDOS_PC_VIDEO_SERVICES_TEXT_ATTRIBUTE);
        }
        break;
    }
    adapter->colorSelect = 0u;
    update_bios_video_state(videoServices);
}

void hyperdos_pc_video_services_initialize_static_functionality_table(hyperdos_pc_video_services* videoServices)
{
    static const uint8_t tableBytes[HYPERDOS_X86_BIOS_VIDEO_STATIC_FUNCTIONALITY_TABLE_BYTE_COUNT] =
            {HYPERDOS_X86_BIOS_VIDEO_STATIC_SUPPORTED_MODES_LOW,
             HYPERDOS_X86_BIOS_VIDEO_STATIC_SUPPORTED_MODES_MIDDLE,
             HYPERDOS_X86_BIOS_VIDEO_STATIC_SUPPORTED_MODES_HIGH,
             0u,
             0u,
             0u,
             0u,
             HYPERDOS_X86_BIOS_VIDEO_STATIC_SUPPORTED_SCAN_LINES,
             HYPERDOS_X86_BIOS_VIDEO_STATIC_MAXIMUM_CHARACTER_SETS,
             HYPERDOS_X86_BIOS_VIDEO_STATIC_CHARACTER_TABLE_COUNT,
             HYPERDOS_X86_BIOS_VIDEO_STATIC_CAPABILITY_FLAGS,
             HYPERDOS_X86_BIOS_VIDEO_STATIC_ADDITIONAL_CAPABILITY_FLAGS,
             0u,
             0u,
             0u,
             0u};
    size_t   byteIndex            = 0u;
    uint32_t tablePhysicalAddress = ((uint32_t)HYPERDOS_X86_BIOS_VIDEO_STATIC_FUNCTIONALITY_TABLE_SEGMENT << 4u) +
                                    HYPERDOS_X86_BIOS_VIDEO_STATIC_FUNCTIONALITY_TABLE_OFFSET;

    for (byteIndex = 0u; byteIndex < HYPERDOS_X86_BIOS_VIDEO_STATIC_FUNCTIONALITY_TABLE_BYTE_COUNT; ++byteIndex)
    {
        hyperdos_pc_firmware_write_byte(videoServices->pc,
                                        tablePhysicalAddress + (uint32_t)byteIndex,
                                        tableBytes[byteIndex]);
    }
}

static uint16_t get_video_state_color_count(const hyperdos_pc_video_services* videoServices)
{
    if (videoServices->videoMode == 0x06u || videoServices->videoMode == 0x0Fu || videoServices->videoMode == 0x11u)
    {
        return 2u;
    }
    if (videoServices->videoMode == 0x04u || videoServices->videoMode == 0x05u)
    {
        return 4u;
    }
    if (videoServices->videoMode == 0x13u)
    {
        return 256u;
    }
    return 16u;
}

static uint8_t get_video_state_raster_scan_line_code(const hyperdos_pc_video_services* videoServices)
{
    const hyperdos_color_graphics_adapter* adapter = &videoServices->pc->colorGraphicsAdapter;

    if ((adapter->modeControl & HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_GRAPHICS) != 0u)
    {
        if (videoServices->videoMode == 0x0Fu || videoServices->videoMode == 0x10u)
        {
            return HYPERDOS_X86_BIOS_VIDEO_RASTER_SCAN_LINES_350;
        }
        if (videoServices->videoMode == 0x11u || videoServices->videoMode == 0x12u)
        {
            return HYPERDOS_X86_BIOS_VIDEO_RASTER_SCAN_LINES_480;
        }
        return HYPERDOS_X86_BIOS_VIDEO_RASTER_SCAN_LINES_200;
    }
    return HYPERDOS_X86_BIOS_VIDEO_RASTER_SCAN_LINES_400;
}

static uint16_t get_video_state_buffer_size(const hyperdos_pc_video_services* videoServices)
{
    return get_current_bios_video_page_size(videoServices);
}

static uint16_t get_video_save_restore_state_byte_count(uint16_t requestedState)
{
    uint16_t stateMask      = (uint16_t)(requestedState & HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_STATE_MASK);
    uint16_t stateByteCount = HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HEADER_BYTE_COUNT;

    if (stateMask == 0u)
    {
        return 0u;
    }
    if ((stateMask & HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_STATE) != 0u)
    {
        stateByteCount = (uint16_t)(stateByteCount + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_BYTE_COUNT);
    }
    if ((stateMask & HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_DATA) != 0u)
    {
        stateByteCount = (uint16_t)(stateByteCount +
                                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_BYTE_COUNT);
    }
    if ((stateMask & HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_STATE) != 0u)
    {
        stateByteCount = (uint16_t)(stateByteCount +
                                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_BYTE_COUNT);
    }
    return stateByteCount;
}

static uint16_t get_video_save_restore_state_buffer_blocks(uint16_t requestedState)
{
    uint16_t stateByteCount = get_video_save_restore_state_byte_count(requestedState);

    if (stateByteCount == 0u)
    {
        return 0u;
    }
    return (uint16_t)((stateByteCount + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BLOCK_BYTE_COUNT - 1u) /
                      HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BLOCK_BYTE_COUNT);
}

static void write_guest_memory_bytes_from_physical_range(hyperdos_pc_video_services* videoServices,
                                                         uint32_t                    destinationPhysicalAddress,
                                                         uint32_t                    sourcePhysicalAddress,
                                                         uint16_t                    byteCount)
{
    uint16_t byteIndex = 0u;

    for (byteIndex = 0u; byteIndex < byteCount; ++byteIndex)
    {
        write_guest_memory_byte(videoServices,
                                destinationPhysicalAddress + byteIndex,
                                read_guest_memory_byte(videoServices, sourcePhysicalAddress + byteIndex));
    }
}

static void restore_guest_memory_bytes_to_physical_range(hyperdos_pc_video_services* videoServices,
                                                         uint32_t                    destinationPhysicalAddress,
                                                         uint32_t                    sourcePhysicalAddress,
                                                         uint16_t                    byteCount)
{
    uint16_t byteIndex = 0u;

    for (byteIndex = 0u; byteIndex < byteCount; ++byteIndex)
    {
        write_guest_memory_byte(videoServices,
                                destinationPhysicalAddress + byteIndex,
                                read_guest_memory_byte(videoServices, sourcePhysicalAddress + byteIndex));
    }
}

static void write_video_save_restore_hardware_state(hyperdos_pc_video_services* videoServices,
                                                    uint32_t                    stateDataPhysicalAddress)
{
    hyperdos_color_graphics_adapter* adapter = &videoServices->pc->colorGraphicsAdapter;
    uint16_t                         cathodeRayTubeControllerPort =
            hyperdos_pc_bios_data_area_read_word(videoServices->pc,
                                                 HYPERDOS_X86_BIOS_DATA_AREA_CRT_CONTROLLER_PORT_OFFSET);
    uint16_t registerNumber = 0u;
    uint16_t planeIndex     = 0u;

    if (cathodeRayTubeControllerPort == 0u)
    {
        cathodeRayTubeControllerPort = HYPERDOS_X86_BIOS_VIDEO_CRT_CONTROLLER_PORT;
    }

    write_guest_memory_byte(videoServices,
                            stateDataPhysicalAddress +
                                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_SEQUENCER_INDEX_OFFSET,
                            adapter->sequencerIndexRegister);
    write_guest_memory_byte(
            videoServices,
            stateDataPhysicalAddress +
                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_CATHODE_RAY_TUBE_CONTROLLER_INDEX_OFFSET,
            adapter->indexRegister);
    write_guest_memory_byte(videoServices,
                            stateDataPhysicalAddress +
                                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_GRAPHICS_CONTROLLER_INDEX_OFFSET,
                            adapter->graphicsControllerIndexRegister);
    write_guest_memory_byte(videoServices,
                            stateDataPhysicalAddress +
                                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_ATTRIBUTE_CONTROLLER_INDEX_OFFSET,
                            (uint8_t)(adapter->attributeControllerIndexRegister |
                                      adapter->attributeControllerPaletteAddressSource));
    write_guest_memory_byte(videoServices,
                            stateDataPhysicalAddress +
                                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_FEATURE_CONTROL_OFFSET,
                            adapter->featureControlRegister);

    for (registerNumber = 1u; registerNumber < 5u; ++registerNumber)
    {
        write_guest_memory_byte(videoServices,
                                stateDataPhysicalAddress +
                                        HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_SEQUENCER_REGISTER_OFFSET +
                                        (registerNumber - 1u),
                                adapter->sequencerRegisters[registerNumber]);
    }
    write_guest_memory_byte(videoServices,
                            stateDataPhysicalAddress +
                                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_MISCELLANEOUS_OUTPUT_OFFSET,
                            adapter->miscellaneousOutputRegister);
    for (registerNumber = 0u; registerNumber < 0x19u; ++registerNumber)
    {
        write_guest_memory_byte(
                videoServices,
                stateDataPhysicalAddress +
                        HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_CATHODE_RAY_TUBE_CONTROLLER_REGISTER_OFFSET +
                        registerNumber,
                adapter->registers[registerNumber]);
    }
    for (registerNumber = 0u; registerNumber < 0x10u; ++registerNumber)
    {
        write_guest_memory_byte(
                videoServices,
                stateDataPhysicalAddress +
                        HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_ATTRIBUTE_PALETTE_REGISTER_OFFSET +
                        registerNumber,
                adapter->attributeControllerRegisters[registerNumber]);
    }
    for (registerNumber = 0u; registerNumber < 4u; ++registerNumber)
    {
        write_guest_memory_byte(videoServices,
                                stateDataPhysicalAddress +
                                        HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_ATTRIBUTE_MODE_REGISTER_OFFSET +
                                        registerNumber,
                                adapter->attributeControllerRegisters[0x10u + registerNumber]);
    }
    for (registerNumber = 0u; registerNumber < 9u; ++registerNumber)
    {
        write_guest_memory_byte(
                videoServices,
                stateDataPhysicalAddress +
                        HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_GRAPHICS_CONTROLLER_REGISTER_OFFSET +
                        registerNumber,
                adapter->graphicsControllerRegisters[registerNumber]);
    }
    write_guest_memory_word(
            videoServices,
            stateDataPhysicalAddress +
                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_CATHODE_RAY_TUBE_CONTROLLER_PORT_OFFSET,
            cathodeRayTubeControllerPort);
    for (planeIndex = 0u; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
    {
        write_guest_memory_byte(videoServices,
                                stateDataPhysicalAddress +
                                        HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_GRAPHICS_LATCH_OFFSET +
                                        planeIndex,
                                adapter->graphicsLatch[planeIndex]);
    }
}

static void restore_video_save_restore_hardware_state(hyperdos_pc_video_services* videoServices,
                                                      uint32_t                    stateDataPhysicalAddress)
{
    hyperdos_color_graphics_adapter* adapter                      = &videoServices->pc->colorGraphicsAdapter;
    uint16_t                         registerNumber               = 0u;
    uint16_t                         planeIndex                   = 0u;
    uint8_t                          attributeControllerAddress   = 0u;
    uint16_t                         cathodeRayTubeControllerPort = read_guest_memory_word(
            videoServices,
            stateDataPhysicalAddress +
                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_CATHODE_RAY_TUBE_CONTROLLER_PORT_OFFSET);

    adapter->sequencerIndexRegister = read_guest_memory_byte(
            videoServices,
            stateDataPhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_SEQUENCER_INDEX_OFFSET);
    adapter->indexRegister = read_guest_memory_byte(
            videoServices,
            stateDataPhysicalAddress +
                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_CATHODE_RAY_TUBE_CONTROLLER_INDEX_OFFSET);
    adapter->graphicsControllerIndexRegister = read_guest_memory_byte(
            videoServices,
            stateDataPhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_GRAPHICS_CONTROLLER_INDEX_OFFSET);
    attributeControllerAddress = read_guest_memory_byte(
            videoServices,
            stateDataPhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_ATTRIBUTE_CONTROLLER_INDEX_OFFSET);
    adapter->attributeControllerIndexRegister        = (uint8_t)(attributeControllerAddress & 0x1Fu);
    adapter->attributeControllerPaletteAddressSource = (uint8_t)(attributeControllerAddress & 0x20u);
    adapter->featureControlRegister                  = read_guest_memory_byte(
            videoServices,
            stateDataPhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_FEATURE_CONTROL_OFFSET);

    for (registerNumber = 1u; registerNumber < 5u; ++registerNumber)
    {
        adapter->sequencerRegisters[registerNumber] = read_guest_memory_byte(
                videoServices,
                stateDataPhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_SEQUENCER_REGISTER_OFFSET +
                        (registerNumber - 1u));
    }
    adapter->miscellaneousOutputRegister = read_guest_memory_byte(
            videoServices,
            stateDataPhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_MISCELLANEOUS_OUTPUT_OFFSET);
    for (registerNumber = 0u; registerNumber < 0x19u; ++registerNumber)
    {
        adapter->registers[registerNumber] = read_guest_memory_byte(
                videoServices,
                stateDataPhysicalAddress +
                        HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_CATHODE_RAY_TUBE_CONTROLLER_REGISTER_OFFSET +
                        registerNumber);
    }
    for (registerNumber = 0u; registerNumber < 0x10u; ++registerNumber)
    {
        adapter->attributeControllerRegisters[registerNumber] = read_guest_memory_byte(
                videoServices,
                stateDataPhysicalAddress +
                        HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_ATTRIBUTE_PALETTE_REGISTER_OFFSET +
                        registerNumber);
    }
    for (registerNumber = 0u; registerNumber < 4u; ++registerNumber)
    {
        adapter->attributeControllerRegisters[0x10u + registerNumber] = read_guest_memory_byte(
                videoServices,
                stateDataPhysicalAddress +
                        HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_ATTRIBUTE_MODE_REGISTER_OFFSET + registerNumber);
    }
    for (registerNumber = 0u; registerNumber < 9u; ++registerNumber)
    {
        adapter->graphicsControllerRegisters[registerNumber] = read_guest_memory_byte(
                videoServices,
                stateDataPhysicalAddress +
                        HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_GRAPHICS_CONTROLLER_REGISTER_OFFSET +
                        registerNumber);
    }
    if (cathodeRayTubeControllerPort != 0u)
    {
        hyperdos_pc_bios_data_area_write_word(videoServices->pc,
                                              HYPERDOS_X86_BIOS_DATA_AREA_CRT_CONTROLLER_PORT_OFFSET,
                                              cathodeRayTubeControllerPort);
    }
    for (planeIndex = 0u; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
    {
        adapter->graphicsLatch[planeIndex] = read_guest_memory_byte(
                videoServices,
                stateDataPhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_GRAPHICS_LATCH_OFFSET +
                        planeIndex);
    }
}

static uint32_t get_interrupt_vector_physical_address(uint8_t interruptVectorNumber)
{
    return (uint32_t)interruptVectorNumber * HYPERDOS_PC_VIDEO_SERVICES_INTERRUPT_VECTOR_BYTE_COUNT;
}

static void write_video_save_restore_basic_input_output_system_data(hyperdos_pc_video_services* videoServices,
                                                                    uint32_t stateDataPhysicalAddress)
{
    uint32_t videoDataAreaPhysicalAddress = HYPERDOS_PC_BIOS_DATA_AREA_BASE_ADDRESS +
                                            HYPERDOS_X86_BIOS_DATA_AREA_CURRENT_VIDEO_MODE_OFFSET;
    uint32_t extendedVideoDataAreaPhysicalAddress = HYPERDOS_PC_BIOS_DATA_AREA_BASE_ADDRESS +
                                                    HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_ROWS_MINUS_ONE_OFFSET;
    uint32_t displayCombinationCodePointerPhysicalAddress =
            HYPERDOS_PC_BIOS_DATA_AREA_BASE_ADDRESS + HYPERDOS_X86_BIOS_DATA_AREA_DISPLAY_COMBINATION_CODE_INDEX_OFFSET;

    write_guest_memory_byte(
            videoServices,
            stateDataPhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_EQUIPMENT_OFFSET,
            (uint8_t)(read_guest_memory_byte(videoServices,
                                             HYPERDOS_PC_BIOS_DATA_AREA_BASE_ADDRESS +
                                                     HYPERDOS_PC_BIOS_DATA_AREA_EQUIPMENT_FLAGS_OFFSET) &
                      0x30u));
    write_guest_memory_bytes_from_physical_range(
            videoServices,
            stateDataPhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_VIDEO_DATA_OFFSET,
            videoDataAreaPhysicalAddress,
            HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_VIDEO_DATA_BYTE_COUNT);
    write_guest_memory_bytes_from_physical_range(
            videoServices,
            stateDataPhysicalAddress +
                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_EXTENDED_VIDEO_DATA_OFFSET,
            extendedVideoDataAreaPhysicalAddress,
            HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_EXTENDED_VIDEO_DATA_BYTE_COUNT);
    write_guest_memory_double_word(
            videoServices,
            stateDataPhysicalAddress +
                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_DISPLAY_COMBINATION_CODE_POINTER_OFFSET,
            read_guest_memory_double_word(videoServices, displayCombinationCodePointerPhysicalAddress));
    write_guest_memory_double_word(
            videoServices,
            stateDataPhysicalAddress +
                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_PRINT_SCREEN_VECTOR_OFFSET,
            read_guest_memory_double_word(videoServices,
                                          get_interrupt_vector_physical_address(
                                                  HYPERDOS_X86_BIOS_PRINT_SCREEN_INTERRUPT)));
    write_guest_memory_double_word(
            videoServices,
            stateDataPhysicalAddress +
                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_VIDEO_PARAMETER_VECTOR_OFFSET,
            read_guest_memory_double_word(videoServices,
                                          get_interrupt_vector_physical_address(
                                                  HYPERDOS_X86_BIOS_VIDEO_PARAMETER_TABLE_INTERRUPT)));
    write_guest_memory_double_word(
            videoServices,
            stateDataPhysicalAddress +
                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_GRAPHICS_CHARACTER_VECTOR_OFFSET,
            read_guest_memory_double_word(videoServices,
                                          get_interrupt_vector_physical_address(
                                                  HYPERDOS_X86_BIOS_GRAPHICS_CHARACTER_TABLE_INTERRUPT)));
    write_guest_memory_double_word(
            videoServices,
            stateDataPhysicalAddress +
                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_CHARACTER_GENERATOR_VECTOR_OFFSET,
            read_guest_memory_double_word(videoServices,
                                          get_interrupt_vector_physical_address(
                                                  HYPERDOS_X86_BIOS_VIDEO_CHARACTER_GENERATOR_INTERRUPT)));
}

static void restore_video_save_restore_basic_input_output_system_data(hyperdos_pc_video_services* videoServices,
                                                                      uint32_t stateDataPhysicalAddress)
{
    hyperdos_color_graphics_adapter* adapter                      = &videoServices->pc->colorGraphicsAdapter;
    uint32_t                         videoDataAreaPhysicalAddress = HYPERDOS_PC_BIOS_DATA_AREA_BASE_ADDRESS +
                                            HYPERDOS_X86_BIOS_DATA_AREA_CURRENT_VIDEO_MODE_OFFSET;
    uint32_t extendedVideoDataAreaPhysicalAddress = HYPERDOS_PC_BIOS_DATA_AREA_BASE_ADDRESS +
                                                    HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_ROWS_MINUS_ONE_OFFSET;
    uint32_t displayCombinationCodePointerPhysicalAddress =
            HYPERDOS_PC_BIOS_DATA_AREA_BASE_ADDRESS + HYPERDOS_X86_BIOS_DATA_AREA_DISPLAY_COMBINATION_CODE_INDEX_OFFSET;
    uint8_t  equipmentLowByte = read_guest_memory_byte(videoServices,
                                                      HYPERDOS_PC_BIOS_DATA_AREA_BASE_ADDRESS +
                                                              HYPERDOS_PC_BIOS_DATA_AREA_EQUIPMENT_FLAGS_OFFSET);
    uint16_t cursorPosition   = 0u;

    equipmentLowByte =
            (uint8_t)((equipmentLowByte & 0xCFu) |
                      read_guest_memory_byte(
                              videoServices,
                              stateDataPhysicalAddress +
                                      HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_EQUIPMENT_OFFSET));
    write_guest_memory_byte(videoServices,
                            HYPERDOS_PC_BIOS_DATA_AREA_BASE_ADDRESS + HYPERDOS_PC_BIOS_DATA_AREA_EQUIPMENT_FLAGS_OFFSET,
                            equipmentLowByte);
    restore_guest_memory_bytes_to_physical_range(
            videoServices,
            videoDataAreaPhysicalAddress,
            stateDataPhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_VIDEO_DATA_OFFSET,
            HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_VIDEO_DATA_BYTE_COUNT);
    restore_guest_memory_bytes_to_physical_range(
            videoServices,
            extendedVideoDataAreaPhysicalAddress,
            stateDataPhysicalAddress +
                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_EXTENDED_VIDEO_DATA_OFFSET,
            HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_EXTENDED_VIDEO_DATA_BYTE_COUNT);
    write_guest_memory_double_word(
            videoServices,
            displayCombinationCodePointerPhysicalAddress,
            read_guest_memory_double_word(
                    videoServices,
                    stateDataPhysicalAddress +
                            HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_DISPLAY_COMBINATION_CODE_POINTER_OFFSET));
    write_guest_memory_double_word(
            videoServices,
            get_interrupt_vector_physical_address(HYPERDOS_X86_BIOS_PRINT_SCREEN_INTERRUPT),
            read_guest_memory_double_word(
                    videoServices,
                    stateDataPhysicalAddress +
                            HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_PRINT_SCREEN_VECTOR_OFFSET));
    write_guest_memory_double_word(
            videoServices,
            get_interrupt_vector_physical_address(HYPERDOS_X86_BIOS_VIDEO_PARAMETER_TABLE_INTERRUPT),
            read_guest_memory_double_word(
                    videoServices,
                    stateDataPhysicalAddress +
                            HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_VIDEO_PARAMETER_VECTOR_OFFSET));
    write_guest_memory_double_word(
            videoServices,
            get_interrupt_vector_physical_address(HYPERDOS_X86_BIOS_GRAPHICS_CHARACTER_TABLE_INTERRUPT),
            read_guest_memory_double_word(
                    videoServices,
                    stateDataPhysicalAddress +
                            HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_GRAPHICS_CHARACTER_VECTOR_OFFSET));
    write_guest_memory_double_word(
            videoServices,
            get_interrupt_vector_physical_address(HYPERDOS_X86_BIOS_VIDEO_CHARACTER_GENERATOR_INTERRUPT),
            read_guest_memory_double_word(
                    videoServices,
                    stateDataPhysicalAddress +
                            HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_CHARACTER_GENERATOR_VECTOR_OFFSET));

    videoServices->videoMode =
            hyperdos_pc_bios_data_area_read_byte(videoServices->pc,
                                                 HYPERDOS_X86_BIOS_DATA_AREA_CURRENT_VIDEO_MODE_OFFSET);
    videoServices->videoColumnCount =
            hyperdos_pc_bios_data_area_read_word(videoServices->pc, HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_COLUMNS_OFFSET);
    if (videoServices->videoColumnCount == 0u ||
        videoServices->videoColumnCount > HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT)
    {
        videoServices->videoColumnCount = HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT;
    }
    videoServices->activeVideoPage =
            hyperdos_pc_bios_data_area_read_byte(videoServices->pc,
                                                 HYPERDOS_X86_BIOS_DATA_AREA_ACTIVE_VIDEO_PAGE_OFFSET);
    if (videoServices->activeVideoPage > HYPERDOS_X86_BIOS_VIDEO_MAXIMUM_TEXT_PAGE_NUMBER)
    {
        videoServices->activeVideoPage = 0u;
    }
    cursorPosition              = hyperdos_pc_bios_data_area_read_word(videoServices->pc,
                                                          hyperdos_pc_bios_data_area_get_cursor_position_offset(
                                                                  videoServices->activeVideoPage));
    videoServices->cursorRow    = (uint16_t)(cursorPosition >> HYPERDOS_X86_BYTE_BIT_COUNT);
    videoServices->cursorColumn = (uint16_t)(cursorPosition & HYPERDOS_X86_LOW_BYTE_MASK);
    if (videoServices->cursorRow >= HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT)
    {
        videoServices->cursorRow = HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT - 1u;
    }
    if (videoServices->cursorColumn >= videoServices->videoColumnCount)
    {
        videoServices->cursorColumn = (uint16_t)(videoServices->videoColumnCount - 1u);
    }
    adapter->modeControl = hyperdos_pc_bios_data_area_read_byte(videoServices->pc,
                                                                HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_MODE_CONTROL_OFFSET);
    adapter->colorSelect = hyperdos_pc_bios_data_area_read_byte(videoServices->pc,
                                                                HYPERDOS_X86_BIOS_DATA_AREA_VIDEO_COLOR_SELECT_OFFSET);
}

static void write_video_save_restore_digital_to_analog_converter_state(hyperdos_pc_video_services* videoServices,
                                                                       uint32_t stateDataPhysicalAddress)
{
    const hyperdos_color_graphics_adapter* adapter = &videoServices->pc->colorGraphicsAdapter;
    uint8_t  digitalToAnalogConverterState         = adapter->digitalToAnalogConverterState == 0u ? 0u : 1u;
    uint8_t  digitalToAnalogConverterIndex         = digitalToAnalogConverterState == 0u
                                                             ? adapter->digitalToAnalogConverterWriteIndexRegister
                                                             : adapter->digitalToAnalogConverterReadIndexRegister;
    uint16_t colorIndex                            = 0u;
    uint16_t componentIndex                        = 0u;

    write_guest_memory_byte(videoServices,
                            stateDataPhysicalAddress +
                                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_STATE_OFFSET,
                            digitalToAnalogConverterState);
    write_guest_memory_byte(videoServices,
                            stateDataPhysicalAddress +
                                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_INDEX_OFFSET,
                            digitalToAnalogConverterIndex);
    write_guest_memory_byte(videoServices,
                            stateDataPhysicalAddress +
                                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_MASK_OFFSET,
                            adapter->digitalToAnalogConverterMaskRegister);

    for (colorIndex = 0u; colorIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_CONVERTER_COLOR_COUNT;
         ++colorIndex)
    {
        for (componentIndex = 0u;
             componentIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_CONVERTER_COMPONENT_COUNT;
             ++componentIndex)
        {
            write_guest_memory_byte(
                    videoServices,
                    stateDataPhysicalAddress +
                            HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_PALETTE_OFFSET +
                            colorIndex * HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_CONVERTER_COMPONENT_COUNT +
                            componentIndex,
                    adapter->digitalToAnalogConverterPalette[colorIndex][componentIndex]);
        }
    }
    write_guest_memory_byte(
            videoServices,
            stateDataPhysicalAddress +
                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_COLOR_SELECT_OFFSET,
            adapter->attributeControllerRegisters[0x14u]);
}

static void restore_video_save_restore_digital_to_analog_converter_state(hyperdos_pc_video_services* videoServices,
                                                                         uint32_t stateDataPhysicalAddress)
{
    hyperdos_color_graphics_adapter* adapter                       = &videoServices->pc->colorGraphicsAdapter;
    uint8_t                          digitalToAnalogConverterState = read_guest_memory_byte(
            videoServices,
            stateDataPhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_STATE_OFFSET);
    uint8_t digitalToAnalogConverterIndex = read_guest_memory_byte(
            videoServices,
            stateDataPhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_INDEX_OFFSET);
    uint16_t colorIndex     = 0u;
    uint16_t componentIndex = 0u;

    adapter->digitalToAnalogConverterMaskRegister = read_guest_memory_byte(
            videoServices,
            stateDataPhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_MASK_OFFSET);
    for (colorIndex = 0u; colorIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_CONVERTER_COLOR_COUNT;
         ++colorIndex)
    {
        for (componentIndex = 0u;
             componentIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_CONVERTER_COMPONENT_COUNT;
             ++componentIndex)
        {
            adapter->digitalToAnalogConverterPalette[colorIndex][componentIndex] = read_guest_memory_byte(
                    videoServices,
                    stateDataPhysicalAddress +
                            HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_PALETTE_OFFSET +
                            colorIndex * HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_CONVERTER_COMPONENT_COUNT +
                            componentIndex);
        }
    }
    adapter->attributeControllerRegisters[0x14u] = read_guest_memory_byte(
            videoServices,
            stateDataPhysicalAddress +
                    HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_COLOR_SELECT_OFFSET);
    adapter->digitalToAnalogConverterReadComponentIndex  = 0u;
    adapter->digitalToAnalogConverterWriteComponentIndex = 0u;
    if (digitalToAnalogConverterState == 0u)
    {
        adapter->digitalToAnalogConverterState              = 0u;
        adapter->digitalToAnalogConverterWriteIndexRegister = digitalToAnalogConverterIndex;
    }
    else
    {
        adapter->digitalToAnalogConverterState             = 3u;
        adapter->digitalToAnalogConverterReadIndexRegister = digitalToAnalogConverterIndex;
    }
}

static int write_video_save_restore_state(hyperdos_x86_processor*     processor,
                                          hyperdos_pc_video_services* videoServices,
                                          uint16_t                    requestedState,
                                          uint16_t                    bufferSegmentOffset)
{
    uint16_t stateMask              = (uint16_t)(requestedState & HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_STATE_MASK);
    uint32_t bufferPhysicalAddress  = 0u;
    uint16_t nextStateSegmentOffset = (uint16_t)(bufferSegmentOffset +
                                                 HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HEADER_BYTE_COUNT);
    uint16_t headerByteIndex        = 0u;

    if (stateMask == 0u ||
        hyperdos_x86_translate_logical_to_physical_address(processor,
                                                           HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                           bufferSegmentOffset,
                                                           &bufferPhysicalAddress) != HYPERDOS_X86_EXECUTION_OK)
    {
        return 0;
    }
    for (headerByteIndex = 0u; headerByteIndex < HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HEADER_BYTE_COUNT;
         ++headerByteIndex)
    {
        write_guest_memory_byte(videoServices, bufferPhysicalAddress + headerByteIndex, 0u);
    }

    if ((stateMask & HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_STATE) != 0u)
    {
        uint32_t stateDataPhysicalAddress = 0u;

        (void)hyperdos_x86_translate_logical_to_physical_address(processor,
                                                                 HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                                 nextStateSegmentOffset,
                                                                 &stateDataPhysicalAddress);

        write_guest_memory_word(videoServices,
                                bufferPhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_POINTER_OFFSET,
                                nextStateSegmentOffset);
        write_video_save_restore_hardware_state(videoServices, stateDataPhysicalAddress);
        nextStateSegmentOffset = (uint16_t)(nextStateSegmentOffset +
                                            HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_BYTE_COUNT);
    }
    if ((stateMask & HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_DATA) != 0u)
    {
        uint32_t stateDataPhysicalAddress = 0u;

        (void)hyperdos_x86_translate_logical_to_physical_address(processor,
                                                                 HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                                 nextStateSegmentOffset,
                                                                 &stateDataPhysicalAddress);

        write_guest_memory_word(videoServices,
                                bufferPhysicalAddress +
                                        HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_POINTER_OFFSET,
                                nextStateSegmentOffset);
        write_video_save_restore_basic_input_output_system_data(videoServices, stateDataPhysicalAddress);
        nextStateSegmentOffset = (uint16_t)(nextStateSegmentOffset +
                                            HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_BYTE_COUNT);
    }
    if ((stateMask & HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_STATE) != 0u)
    {
        uint32_t stateDataPhysicalAddress = 0u;

        (void)hyperdos_x86_translate_logical_to_physical_address(processor,
                                                                 HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                                 nextStateSegmentOffset,
                                                                 &stateDataPhysicalAddress);

        write_guest_memory_word(videoServices,
                                bufferPhysicalAddress +
                                        HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_POINTER_OFFSET,
                                nextStateSegmentOffset);
        write_video_save_restore_digital_to_analog_converter_state(videoServices, stateDataPhysicalAddress);
    }

    hyperdos_pc_video_services_trace(videoServices,
                                     "int10 video save state mask=%04X buffer=%05lX blocks=%u",
                                     stateMask,
                                     (unsigned long)bufferPhysicalAddress,
                                     get_video_save_restore_state_buffer_blocks(stateMask));
    return 1;
}

static int restore_video_save_restore_state(hyperdos_x86_processor*     processor,
                                            hyperdos_pc_video_services* videoServices,
                                            uint16_t                    requestedState,
                                            uint16_t                    bufferSegmentOffset)
{
    uint16_t stateMask             = (uint16_t)(requestedState & HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_STATE_MASK);
    uint32_t bufferPhysicalAddress = 0u;

    if (stateMask == 0u ||
        hyperdos_x86_translate_logical_to_physical_address(processor,
                                                           HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                           bufferSegmentOffset,
                                                           &bufferPhysicalAddress) != HYPERDOS_X86_EXECUTION_OK)
    {
        return 0;
    }
    if ((stateMask & HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_STATE) != 0u)
    {
        uint16_t stateDataSegmentOffset = read_guest_memory_word(
                videoServices,
                bufferPhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_HARDWARE_POINTER_OFFSET);

        if (stateDataSegmentOffset != 0u)
        {
            uint32_t stateDataPhysicalAddress = 0u;

            (void)hyperdos_x86_translate_logical_to_physical_address(processor,
                                                                     HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                                     stateDataSegmentOffset,
                                                                     &stateDataPhysicalAddress);
            restore_video_save_restore_hardware_state(videoServices, stateDataPhysicalAddress);
        }
    }
    if ((stateMask & HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_DATA) != 0u)
    {
        uint16_t stateDataSegmentOffset = read_guest_memory_word(
                videoServices,
                bufferPhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_BASIC_INPUT_OUTPUT_SYSTEM_POINTER_OFFSET);

        if (stateDataSegmentOffset != 0u)
        {
            uint32_t stateDataPhysicalAddress = 0u;

            (void)hyperdos_x86_translate_logical_to_physical_address(processor,
                                                                     HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                                     stateDataSegmentOffset,
                                                                     &stateDataPhysicalAddress);
            restore_video_save_restore_basic_input_output_system_data(videoServices, stateDataPhysicalAddress);
        }
    }
    if ((stateMask & HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_STATE) != 0u)
    {
        uint16_t stateDataSegmentOffset = read_guest_memory_word(
                videoServices,
                bufferPhysicalAddress +
                        HYPERDOS_X86_BIOS_VIDEO_SAVE_RESTORE_DIGITAL_TO_ANALOG_CONVERTER_POINTER_OFFSET);

        if (stateDataSegmentOffset != 0u)
        {
            uint32_t stateDataPhysicalAddress = 0u;

            (void)hyperdos_x86_translate_logical_to_physical_address(processor,
                                                                     HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                                     stateDataSegmentOffset,
                                                                     &stateDataPhysicalAddress);
            restore_video_save_restore_digital_to_analog_converter_state(videoServices, stateDataPhysicalAddress);
        }
    }

    hyperdos_pc_video_services_trace(videoServices,
                                     "int10 video restore state mask=%04X buffer=%05lX",
                                     stateMask,
                                     (unsigned long)bufferPhysicalAddress);
    return 1;
}

static void write_video_functionality_state_table(hyperdos_x86_processor*     processor,
                                                  hyperdos_pc_video_services* videoServices)
{
    size_t                           byteIndex            = 0u;
    size_t                           cursorPageIndex      = 0u;
    hyperdos_color_graphics_adapter* adapter              = &videoServices->pc->colorGraphicsAdapter;
    uint32_t                         tablePhysicalAddress = 0u;
    uint32_t staticFunctionalityTablePointer = ((uint32_t)HYPERDOS_X86_BIOS_VIDEO_STATIC_FUNCTIONALITY_TABLE_SEGMENT
                                                << 16u) |
                                               HYPERDOS_X86_BIOS_VIDEO_STATIC_FUNCTIONALITY_TABLE_OFFSET;

    (void)hyperdos_x86_translate_logical_to_physical_address(
            processor,
            HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
            hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX),
            &tablePhysicalAddress);

    for (byteIndex = 0u; byteIndex < HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_TABLE_BYTE_COUNT; ++byteIndex)
    {
        write_guest_memory_byte(videoServices, tablePhysicalAddress + (uint32_t)byteIndex, 0u);
    }

    write_guest_memory_double_word(videoServices,
                                   tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_STATIC_TABLE_OFFSET,
                                   staticFunctionalityTablePointer);
    write_guest_memory_byte(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_MODE_OFFSET,
                            videoServices->videoMode);
    write_guest_memory_word(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_COLUMN_COUNT_OFFSET,
                            videoServices->videoColumnCount);
    write_guest_memory_word(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_BUFFER_SIZE_OFFSET,
                            get_video_state_buffer_size(videoServices));
    write_guest_memory_word(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_BUFFER_START_OFFSET,
                            0u);
    for (cursorPageIndex = 0u;
         cursorPageIndex < HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_CURSOR_TABLE_BYTE_COUNT / HYPERDOS_X86_WORD_SIZE;
         ++cursorPageIndex)
    {
        uint16_t cursorPosition = 0u;

        if (cursorPageIndex == videoServices->activeVideoPage)
        {
            cursorPosition = (uint16_t)((videoServices->cursorRow << HYPERDOS_X86_BYTE_BIT_COUNT) |
                                        videoServices->cursorColumn);
        }
        else
        {
            cursorPosition = hyperdos_pc_bios_data_area_read_word(videoServices->pc,
                                                                  hyperdos_pc_bios_data_area_get_cursor_position_offset(
                                                                          (uint8_t)cursorPageIndex));
        }
        write_guest_memory_word(videoServices,
                                tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_CURSOR_TABLE_OFFSET +
                                        (uint32_t)(cursorPageIndex * HYPERDOS_X86_WORD_SIZE),
                                cursorPosition);
    }
    write_guest_memory_byte(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_CURSOR_END_LINE_OFFSET,
                            0x07u);
    write_guest_memory_byte(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_CURSOR_START_LINE_OFFSET,
                            0x06u);
    write_guest_memory_byte(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_ACTIVE_PAGE_OFFSET,
                            videoServices->activeVideoPage);
    write_guest_memory_word(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_CRT_CONTROLLER_PORT_OFFSET,
                            HYPERDOS_X86_BIOS_VIDEO_CRT_CONTROLLER_PORT);
    write_guest_memory_byte(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_MODE_CONTROL_OFFSET,
                            adapter->modeControl);
    write_guest_memory_byte(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_COLOR_SELECT_OFFSET,
                            adapter->colorSelect);
    write_guest_memory_byte(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_ROWS_MINUS_ONE_OFFSET,
                            HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT - 1u);
    write_guest_memory_word(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_CHARACTER_HEIGHT_OFFSET,
                            HYPERDOS_X86_BIOS_VIDEO_CHARACTER_HEIGHT);
    write_guest_memory_byte(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_ACTIVE_DISPLAY_CODE_OFFSET,
                            HYPERDOS_X86_BIOS_VIDEO_DISPLAY_COMBINATION_CODE_VIDEO_GRAPHICS_ARRAY_COLOR);
    write_guest_memory_byte(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_INACTIVE_DISPLAY_CODE_OFFSET,
                            HYPERDOS_X86_BIOS_VIDEO_DISPLAY_COMBINATION_CODE_NO_DISPLAY);
    write_guest_memory_word(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_COLOR_COUNT_OFFSET,
                            get_video_state_color_count(videoServices));
    write_guest_memory_byte(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_MAXIMUM_PAGE_NUMBER_OFFSET,
                            HYPERDOS_X86_BIOS_VIDEO_MAXIMUM_TEXT_PAGE_NUMBER);
    write_guest_memory_byte(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_RASTER_SCAN_LINES_OFFSET,
                            get_video_state_raster_scan_line_code(videoServices));
    write_guest_memory_byte(videoServices,
                            tablePhysicalAddress +
                                    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_FIRST_TEXT_CHARACTER_TABLE_OFFSET,
                            0u);
    write_guest_memory_byte(videoServices,
                            tablePhysicalAddress +
                                    HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_SECOND_TEXT_CHARACTER_TABLE_OFFSET,
                            0u);
    write_guest_memory_byte(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_FLAGS_OFFSET,
                            HYPERDOS_X86_BIOS_VIDEO_STATE_CURSOR_EMULATION_ENABLED |
                                    HYPERDOS_X86_BIOS_VIDEO_STATE_BLINKING_ATTRIBUTE_ENABLED);
    write_guest_memory_byte(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_MEMORY_SIZE_OFFSET,
                            HYPERDOS_X86_BIOS_VIDEO_MEMORY_SIZE_256_KILOBYTES);
    write_guest_memory_byte(videoServices,
                            tablePhysicalAddress + HYPERDOS_X86_BIOS_VIDEO_DYNAMIC_STATE_SAVE_AREA_STATUS_OFFSET,
                            0u);

    hyperdos_pc_video_services_trace(videoServices,
                                     "int10 functionality-state table=%05lX mode=%02X columns=%u buffer=%u colors=%u",
                                     (unsigned long)tablePhysicalAddress,
                                     videoServices->videoMode,
                                     videoServices->videoColumnCount,
                                     get_video_state_buffer_size(videoServices),
                                     get_video_state_color_count(videoServices));
}

static void write_korean_extension_placeholder_character_shape(hyperdos_x86_processor*     processor,
                                                               hyperdos_pc_video_services* videoServices,
                                                               uint16_t                    characterShapeOffset)
{
    uint16_t rowIndex                      = 0u;
    uint32_t characterShapePhysicalAddress = 0u;

    (void)hyperdos_x86_translate_logical_to_physical_address(processor,
                                                             HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                             characterShapeOffset,
                                                             &characterShapePhysicalAddress);

    for (rowIndex = 0u; rowIndex < HYPERDOS_X86_BIOS_VIDEO_KOREAN_EXTENSION_CHARACTER_SHAPE_ROW_COUNT; ++rowIndex)
    {
        uint16_t rowBits = (rowIndex == 0u ||
                            rowIndex == HYPERDOS_X86_BIOS_VIDEO_KOREAN_EXTENSION_CHARACTER_SHAPE_ROW_COUNT - 1u)
                                   ? 0x7FFEu
                                   : 0x4002u;
        write_guest_memory_byte(videoServices,
                                characterShapePhysicalAddress + rowIndex * 2u,
                                (uint8_t)(rowBits >> HYPERDOS_X86_BYTE_BIT_COUNT));
        write_guest_memory_byte(videoServices,
                                characterShapePhysicalAddress + rowIndex * 2u + 1u,
                                (uint8_t)(rowBits & HYPERDOS_X86_LOW_BYTE_MASK));
    }
}

static int video_register_interface_group_is_single_register(uint16_t registerGroup)
{
    return registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_MISCELLANEOUS_OUTPUT_GROUP ||
           registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_FEATURE_CONTROL_GROUP ||
           registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_GRAPHICS_ONE_POSITION_GROUP ||
           registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_GRAPHICS_TWO_POSITION_GROUP;
}

static int read_video_register_interface_register(const hyperdos_pc_video_services* videoServices,
                                                  uint16_t                          registerGroup,
                                                  uint8_t                           registerNumber,
                                                  uint8_t*                          value)
{
    const hyperdos_color_graphics_adapter* adapter = &videoServices->pc->colorGraphicsAdapter;

    if (value == NULL)
    {
        return 0;
    }
    if (registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_CRT_CONTROLLER_GROUP && registerNumber < 0x19u)
    {
        *value = adapter->registers[registerNumber];
        return 1;
    }
    if (registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_SEQUENCER_GROUP && registerNumber < 0x05u)
    {
        *value = adapter->sequencerRegisters[registerNumber];
        return 1;
    }
    if (registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_GRAPHICS_CONTROLLER_GROUP && registerNumber < 0x09u)
    {
        *value = adapter->graphicsControllerRegisters[registerNumber];
        return 1;
    }
    if (registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_ATTRIBUTE_CONTROLLER_GROUP &&
        registerNumber < 0x14u)
    {
        *value = adapter->attributeControllerRegisters[registerNumber];
        return 1;
    }
    if (registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_MISCELLANEOUS_OUTPUT_GROUP ||
        registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_GRAPHICS_ONE_POSITION_GROUP)
    {
        *value = adapter->miscellaneousOutputRegister;
        return 1;
    }
    if (registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_FEATURE_CONTROL_GROUP ||
        registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_GRAPHICS_TWO_POSITION_GROUP)
    {
        *value = adapter->featureControlRegister;
        return 1;
    }
    return 0;
}

static int write_video_register_interface_register(hyperdos_pc_video_services* videoServices,
                                                   uint16_t                    registerGroup,
                                                   uint8_t                     registerNumber,
                                                   uint8_t                     value)
{
    hyperdos_color_graphics_adapter* adapter = &videoServices->pc->colorGraphicsAdapter;

    if (registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_CRT_CONTROLLER_GROUP && registerNumber < 0x19u)
    {
        adapter->registers[registerNumber] = value;
        return 1;
    }
    if (registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_SEQUENCER_GROUP && registerNumber < 0x05u)
    {
        adapter->sequencerRegisters[registerNumber] = value;
        return 1;
    }
    if (registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_GRAPHICS_CONTROLLER_GROUP && registerNumber < 0x09u)
    {
        adapter->graphicsControllerRegisters[registerNumber] = value;
        return 1;
    }
    if (registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_ATTRIBUTE_CONTROLLER_GROUP &&
        registerNumber < 0x14u)
    {
        adapter->attributeControllerRegisters[registerNumber] = value;
        return 1;
    }
    if (registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_MISCELLANEOUS_OUTPUT_GROUP ||
        registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_GRAPHICS_ONE_POSITION_GROUP)
    {
        adapter->miscellaneousOutputRegister = value;
        return 1;
    }
    if (registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_FEATURE_CONTROL_GROUP ||
        registerGroup == HYPERDOS_X86_BIOS_VIDEO_REGISTER_INTERFACE_GRAPHICS_TWO_POSITION_GROUP)
    {
        adapter->featureControlRegister = value;
        return 1;
    }
    return 0;
}

static uint32_t get_extra_segment_physical_address(const hyperdos_x86_processor* processor, uint16_t offset)
{
    uint32_t physicalAddress = 0u;

    (void)hyperdos_x86_translate_logical_to_physical_address(processor,
                                                             HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                             offset,
                                                             &physicalAddress);
    return physicalAddress;
}

static uint16_t get_text_page_memory_offset(const hyperdos_pc_video_services* videoServices, uint8_t videoPage)
{
    return (uint16_t)((uint16_t)videoPage * get_current_bios_video_page_size(videoServices));
}

size_t hyperdos_pc_video_services_get_text_page_cell_memory_index(const hyperdos_pc_video_services* videoServices,
                                                                  uint8_t                           videoPage,
                                                                  uint16_t                          row,
                                                                  uint16_t                          column)
{
    uint16_t pageOffset  = get_text_page_memory_offset(videoServices, videoPage);
    size_t   memoryIndex = (size_t)pageOffset + ((size_t)row * HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT + column) *
                                                      HYPERDOS_COLOR_GRAPHICS_ADAPTER_CELL_BYTE_COUNT;

    return memoryIndex & (HYPERDOS_COLOR_GRAPHICS_ADAPTER_MEMORY_SIZE - 1u);
}

static uint16_t get_text_cursor_position_for_page(hyperdos_pc_video_services* videoServices, uint8_t videoPage)
{
    if (videoPage <= HYPERDOS_X86_BIOS_VIDEO_MAXIMUM_TEXT_PAGE_NUMBER)
    {
        return hyperdos_pc_bios_data_area_read_word(videoServices->pc,
                                                    hyperdos_pc_bios_data_area_get_cursor_position_offset(videoPage));
    }
    return (uint16_t)((videoServices->cursorRow << HYPERDOS_X86_BYTE_BIT_COUNT) | videoServices->cursorColumn);
}

static int get_bios_video_pixel_dimensions(uint8_t videoMode, int* pixelWidth, int* pixelHeight)
{
    if (hyperdos_pc_video_services_get_video_graphics_array_mode_dimensions(videoMode, pixelWidth, pixelHeight))
    {
        return 1;
    }
    if (videoMode == 0x04u || videoMode == 0x05u)
    {
        *pixelWidth  = HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_WIDTH_LOW;
        *pixelHeight = HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_HEIGHT;
        return 1;
    }
    if (videoMode == 0x06u)
    {
        *pixelWidth  = HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_WIDTH_HIGH;
        *pixelHeight = HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_HEIGHT;
        return 1;
    }
    return 0;
}

static uint32_t get_color_graphics_adapter_pixel_byte_offset(uint16_t column, uint16_t row, uint8_t highResolution)
{
    uint32_t rowMemoryOffset = (row & 1u) != 0u ? 0x2000u : 0u;

    rowMemoryOffset += ((uint32_t)row >> 1u) * 80u;
    return rowMemoryOffset + (uint32_t)(column / (highResolution ? 8u : 4u));
}

static uint8_t read_bios_video_pixel(hyperdos_pc_video_services* videoServices,
                                     uint8_t                     videoPage,
                                     uint16_t                    column,
                                     uint16_t                    row)
{
    hyperdos_color_graphics_adapter* adapter     = &videoServices->pc->colorGraphicsAdapter;
    int                              pixelWidth  = 0;
    int                              pixelHeight = 0;
    uint32_t                         pageOffset = (uint32_t)videoPage * get_current_bios_video_page_size(videoServices);

    if (!get_bios_video_pixel_dimensions(videoServices->videoMode, &pixelWidth, &pixelHeight) ||
        column >= (uint16_t)pixelWidth || row >= (uint16_t)pixelHeight)
    {
        return 0u;
    }

    if (videoServices->videoMode == 0x04u || videoServices->videoMode == 0x05u)
    {
        uint32_t byteOffset   = pageOffset + get_color_graphics_adapter_pixel_byte_offset(column, row, 0);
        uint8_t  packedPixels = adapter->memory[byteOffset & (HYPERDOS_COLOR_GRAPHICS_ADAPTER_MEMORY_SIZE - 1u)];
        uint8_t  bitShift     = (uint8_t)((3u - (column & 0x03u)) * 2u);

        return (uint8_t)((packedPixels >> bitShift) & 0x03u);
    }
    if (videoServices->videoMode == 0x06u)
    {
        uint32_t byteOffset   = pageOffset + get_color_graphics_adapter_pixel_byte_offset(column, row, 1);
        uint8_t  packedPixels = adapter->memory[byteOffset & (HYPERDOS_COLOR_GRAPHICS_ADAPTER_MEMORY_SIZE - 1u)];
        uint8_t  bitMask      = (uint8_t)(0x80u >> (column & 0x07u));

        return (uint8_t)((packedPixels & bitMask) != 0u ? 1u : 0u);
    }
    if (videoServices->videoMode == 0x13u)
    {
        uint32_t pixelOffset  = pageOffset + (uint32_t)row * 320u + column;
        size_t   planeIndex   = pixelOffset & 0x03u;
        uint32_t planeAddress = (pixelOffset >> 2u) & (HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_SIZE - 1u);

        return adapter->graphicsPlanes[planeIndex][planeAddress];
    }
    {
        uint32_t byteOffset = pageOffset +
                              (uint32_t)row * hyperdos_color_graphics_adapter_get_display_stride_byte_count(adapter) +
                              (uint32_t)(column >> 3u);
        uint32_t planeAddress = byteOffset & (HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_SIZE - 1u);
        uint8_t  bitMask      = (uint8_t)(0x80u >> (column & 0x07u));
        uint8_t  color        = 0u;
        size_t   planeIndex   = 0u;

        for (planeIndex = 0u; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
        {
            if ((adapter->graphicsPlanes[planeIndex][planeAddress] & bitMask) != 0u)
            {
                color |= (uint8_t)(1u << planeIndex);
            }
        }
        if (videoServices->videoMode == 0x0Fu || videoServices->videoMode == 0x11u)
        {
            color &= 0x01u;
        }
        return color;
    }
}

static void write_bios_video_pixel(hyperdos_pc_video_services* videoServices,
                                   uint8_t                     videoPage,
                                   uint16_t                    column,
                                   uint16_t                    row,
                                   uint8_t                     color)
{
    hyperdos_color_graphics_adapter* adapter     = &videoServices->pc->colorGraphicsAdapter;
    int                              pixelWidth  = 0;
    int                              pixelHeight = 0;
    uint32_t                         pageOffset = (uint32_t)videoPage * get_current_bios_video_page_size(videoServices);
    uint8_t                          xorPixel   = (uint8_t)((color & HYPERDOS_X86_BIOS_VIDEO_PIXEL_XOR_MASK) != 0u);

    if (!get_bios_video_pixel_dimensions(videoServices->videoMode, &pixelWidth, &pixelHeight) ||
        column >= (uint16_t)pixelWidth || row >= (uint16_t)pixelHeight)
    {
        return;
    }

    if (videoServices->videoMode == 0x04u || videoServices->videoMode == 0x05u)
    {
        uint32_t byteOffset   = pageOffset + get_color_graphics_adapter_pixel_byte_offset(column, row, 0);
        uint8_t  bitShift     = (uint8_t)((3u - (column & 0x03u)) * 2u);
        uint8_t  pixelMask    = (uint8_t)(0x03u << bitShift);
        uint8_t  pixelColor   = (uint8_t)((color & 0x03u) << bitShift);
        uint8_t* packedPixels = &adapter->memory[byteOffset & (HYPERDOS_COLOR_GRAPHICS_ADAPTER_MEMORY_SIZE - 1u)];

        if (xorPixel)
        {
            *packedPixels = (uint8_t)(*packedPixels ^ pixelColor);
        }
        else
        {
            *packedPixels = (uint8_t)((*packedPixels & (uint8_t)~pixelMask) | pixelColor);
        }
        return;
    }
    if (videoServices->videoMode == 0x06u)
    {
        uint32_t byteOffset   = pageOffset + get_color_graphics_adapter_pixel_byte_offset(column, row, 1);
        uint8_t  bitMask      = (uint8_t)(0x80u >> (column & 0x07u));
        uint8_t* packedPixels = &adapter->memory[byteOffset & (HYPERDOS_COLOR_GRAPHICS_ADAPTER_MEMORY_SIZE - 1u)];

        if (xorPixel)
        {
            *packedPixels = (uint8_t)(*packedPixels ^ ((color & 0x01u) != 0u ? bitMask : 0u));
        }
        else if ((color & 0x01u) != 0u)
        {
            *packedPixels = (uint8_t)(*packedPixels | bitMask);
        }
        else
        {
            *packedPixels = (uint8_t)(*packedPixels & (uint8_t)~bitMask);
        }
        return;
    }
    if (videoServices->videoMode == 0x13u)
    {
        uint32_t pixelOffset  = pageOffset + (uint32_t)row * 320u + column;
        size_t   planeIndex   = pixelOffset & 0x03u;
        uint32_t planeAddress = (pixelOffset >> 2u) & (HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_SIZE - 1u);

        adapter->graphicsPlanes[planeIndex][planeAddress] = color;
        return;
    }
    {
        uint32_t byteOffset = pageOffset +
                              (uint32_t)row * hyperdos_color_graphics_adapter_get_display_stride_byte_count(adapter) +
                              (uint32_t)(column >> 3u);
        uint32_t planeAddress = byteOffset & (HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_SIZE - 1u);
        uint8_t  bitMask      = (uint8_t)(0x80u >> (column & 0x07u));
        size_t   planeIndex   = 0u;

        for (planeIndex = 0u; planeIndex < HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT; ++planeIndex)
        {
            uint8_t* planeByte = &adapter->graphicsPlanes[planeIndex][planeAddress];
            uint8_t  planeBit  = (uint8_t)((color >> planeIndex) & 0x01u);

            if ((videoServices->videoMode == 0x0Fu || videoServices->videoMode == 0x11u) && planeIndex != 0u)
            {
                planeBit = 0u;
            }
            if (xorPixel)
            {
                *planeByte = (uint8_t)(*planeByte ^ (planeBit ? bitMask : 0u));
            }
            else if (planeBit)
            {
                *planeByte = (uint8_t)(*planeByte | bitMask);
            }
            else
            {
                *planeByte = (uint8_t)(*planeByte & (uint8_t)~bitMask);
            }
        }
    }
}

static void write_bios_video_string(hyperdos_x86_processor*     processor,
                                    hyperdos_pc_video_services* videoServices,
                                    uint8_t                     writeMode,
                                    uint8_t                     videoPage,
                                    uint8_t                     attribute,
                                    uint16_t                    characterCount,
                                    uint16_t                    row,
                                    uint16_t                    column)
{
    uint16_t characterIndex           = 0u;
    uint16_t currentRow               = row;
    uint16_t currentColumn            = column;
    uint16_t sourceOffset             = hyperdos_x86_get_general_register_word(processor,
                                                                   HYPERDOS_X86_GENERAL_REGISTER_BASE_POINTER);
    int      stringContainsAttributes = (writeMode & HYPERDOS_X86_BIOS_VIDEO_WRITE_STRING_ATTRIBUTE_BIT) != 0u;

    for (characterIndex = 0u; characterIndex < characterCount; ++characterIndex)
    {
        uint8_t character          = read_guest_memory_byte(videoServices,
                                                   get_extra_segment_physical_address(processor, sourceOffset));
        uint8_t characterAttribute = attribute;
        size_t  memoryIndex        = 0u;

        ++sourceOffset;
        if (stringContainsAttributes)
        {
            characterAttribute = read_guest_memory_byte(videoServices,
                                                        get_extra_segment_physical_address(processor, sourceOffset));
            ++sourceOffset;
        }

        if (currentRow >= HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT)
        {
            break;
        }
        if (currentColumn < videoServices->videoColumnCount)
        {
            memoryIndex = hyperdos_pc_video_services_get_text_page_cell_memory_index(videoServices,
                                                                                     videoPage,
                                                                                     currentRow,
                                                                                     currentColumn);
            write_text_memory_byte(videoServices, memoryIndex, character);
            write_text_memory_byte(videoServices, memoryIndex + 1u, characterAttribute);
        }
        ++currentColumn;
        if (currentColumn >= videoServices->videoColumnCount)
        {
            currentColumn = 0u;
            ++currentRow;
        }
    }

    if ((writeMode & HYPERDOS_X86_BIOS_VIDEO_WRITE_STRING_UPDATE_CURSOR_BIT) != 0u)
    {
        if (currentRow >= HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT)
        {
            currentRow    = HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT - 1u;
            currentColumn = (uint16_t)(videoServices->videoColumnCount - 1u);
        }
        set_text_cursor_from_row_column_for_page(videoServices, videoPage, currentRow, currentColumn);
    }
}

static uint8_t get_video_bios_mode(void* userContext)
{
    hyperdos_pc_video_services* videoServices = (hyperdos_pc_video_services*)userContext;

    return videoServices->videoMode;
}

static uint16_t get_video_bios_column_count(void* userContext)
{
    hyperdos_pc_video_services* videoServices = (hyperdos_pc_video_services*)userContext;

    return videoServices->videoColumnCount;
}

static void set_video_bios_mode(void* userContext, uint8_t mode)
{
    hyperdos_pc_video_services_set_video_mode((hyperdos_pc_video_services*)userContext, mode);
}

static void set_video_bios_active_page(void* userContext, uint8_t videoPage)
{
    set_active_video_page((hyperdos_pc_video_services*)userContext, videoPage);
}

static void set_video_bios_text_cursor_position(void* userContext, uint8_t videoPage, uint16_t row, uint16_t column)
{
    set_text_cursor_from_row_column_for_page((hyperdos_pc_video_services*)userContext, videoPage, row, column);
}

static uint16_t get_video_bios_text_cursor_position(void* userContext, uint8_t videoPage)
{
    return get_text_cursor_position_for_page((hyperdos_pc_video_services*)userContext, videoPage);
}

static uint16_t read_video_bios_text_character_attribute_at_cursor(void* userContext, uint8_t videoPage)
{
    hyperdos_pc_video_services* videoServices  = (hyperdos_pc_video_services*)userContext;
    uint16_t                    cursorPosition = 0u;
    uint16_t                    row            = 0u;
    uint16_t                    column         = 0u;
    size_t                      memoryIndex    = 0u;

    if (videoPage <= HYPERDOS_X86_BIOS_VIDEO_MAXIMUM_TEXT_PAGE_NUMBER)
    {
        cursorPosition = hyperdos_pc_bios_data_area_read_word(videoServices->pc,
                                                              hyperdos_pc_bios_data_area_get_cursor_position_offset(
                                                                      videoPage));
    }
    row    = (uint16_t)(cursorPosition >> HYPERDOS_X86_BYTE_BIT_COUNT);
    column = (uint16_t)(cursorPosition & HYPERDOS_X86_LOW_BYTE_MASK);
    if (row >= HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT)
    {
        row = HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT - 1u;
    }
    if (column >= HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT)
    {
        column = HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT - 1u;
    }
    memoryIndex = hyperdos_pc_video_services_get_text_page_cell_memory_index(videoServices, videoPage, row, column);
    return (uint16_t)(hyperdos_pc_video_services_read_text_memory_byte(videoServices, memoryIndex) |
                      ((uint16_t)hyperdos_pc_video_services_read_text_memory_byte(videoServices, memoryIndex + 1u)
                       << HYPERDOS_X86_BYTE_BIT_COUNT));
}

static void scroll_video_bios_text_region(void*    userContext,
                                          uint16_t firstRow,
                                          uint16_t firstColumn,
                                          uint16_t lastRow,
                                          uint16_t lastColumn,
                                          uint8_t  lineCount,
                                          uint8_t  attribute,
                                          int      scrollDown)
{
    scroll_text_region((hyperdos_pc_video_services*)userContext,
                       firstRow,
                       firstColumn,
                       lastRow,
                       lastColumn,
                       lineCount,
                       attribute,
                       scrollDown);
}

static void write_video_bios_character_repeated(void*    userContext,
                                                uint8_t  videoPage,
                                                uint8_t  character,
                                                uint8_t  attribute,
                                                uint16_t count,
                                                int      updateAttribute)
{
    write_character_repeated((hyperdos_pc_video_services*)userContext,
                             videoPage,
                             character,
                             attribute,
                             count,
                             updateAttribute);
}

static void write_video_bios_pixel(void* userContext, uint8_t videoPage, uint16_t column, uint16_t row, uint8_t color)
{
    write_bios_video_pixel((hyperdos_pc_video_services*)userContext, videoPage, column, row, color);
}

static uint8_t read_video_bios_pixel(void* userContext, uint8_t videoPage, uint16_t column, uint16_t row)
{
    return read_bios_video_pixel((hyperdos_pc_video_services*)userContext, videoPage, column, row);
}

static void write_video_bios_teletype_character(void* userContext, uint8_t character, uint8_t attribute)
{
    write_teletype_character((hyperdos_pc_video_services*)userContext, character, attribute);
}

static void write_video_bios_string(hyperdos_x86_processor* processor,
                                    void*                   userContext,
                                    uint8_t                 writeMode,
                                    uint8_t                 videoPage,
                                    uint8_t                 attribute,
                                    uint16_t                characterCount,
                                    uint16_t                row,
                                    uint16_t                column)
{
    write_bios_video_string(processor,
                            (hyperdos_pc_video_services*)userContext,
                            writeMode,
                            videoPage,
                            attribute,
                            characterCount,
                            row,
                            column);
}

static void write_video_bios_functionality_state_table(hyperdos_x86_processor* processor, void* userContext)
{
    write_video_functionality_state_table(processor, (hyperdos_pc_video_services*)userContext);
}

static int write_video_bios_save_restore_state(hyperdos_x86_processor* processor,
                                               void*                   userContext,
                                               uint16_t                requestedState,
                                               uint16_t                bufferOffset)
{
    return write_video_save_restore_state(processor,
                                          (hyperdos_pc_video_services*)userContext,
                                          requestedState,
                                          bufferOffset);
}

static int restore_video_bios_save_restore_state(hyperdos_x86_processor* processor,
                                                 void*                   userContext,
                                                 uint16_t                requestedState,
                                                 uint16_t                bufferOffset)
{
    return restore_video_save_restore_state(processor,
                                            (hyperdos_pc_video_services*)userContext,
                                            requestedState,
                                            bufferOffset);
}

static int read_video_bios_register_interface_register(void*    userContext,
                                                       uint16_t registerGroup,
                                                       uint8_t  registerNumber,
                                                       uint8_t* registerValue)
{
    return read_video_register_interface_register((hyperdos_pc_video_services*)userContext,
                                                  registerGroup,
                                                  registerNumber,
                                                  registerValue);
}

static int write_video_bios_register_interface_register(void*    userContext,
                                                        uint16_t registerGroup,
                                                        uint8_t  registerNumber,
                                                        uint8_t  registerValue)
{
    return write_video_register_interface_register((hyperdos_pc_video_services*)userContext,
                                                   registerGroup,
                                                   registerNumber,
                                                   registerValue);
}

static void write_video_bios_korean_extension_placeholder_character_shape(hyperdos_x86_processor* processor,
                                                                          void*                   userContext,
                                                                          uint16_t                bufferOffset)
{
    write_korean_extension_placeholder_character_shape(processor,
                                                       (hyperdos_pc_video_services*)userContext,
                                                       bufferOffset);
}

static void trace_video_bios_event(void* userContext, const char* message)
{
    hyperdos_pc_video_services_trace((hyperdos_pc_video_services*)userContext, "%s", message);
}

static void initialize_video_bios_static_functionality_table(void* userContext)
{
    hyperdos_pc_video_services_initialize_static_functionality_table((hyperdos_pc_video_services*)userContext);
}

void hyperdos_pc_video_services_connect_bios_interface(hyperdos_pc_video_services*       videoServices,
                                                       hyperdos_pc_video_bios_interface* videoBiosInterface)
{
    if (videoBiosInterface == NULL)
    {
        return;
    }

    memset(videoBiosInterface, 0, sizeof(*videoBiosInterface));
    if (videoServices == NULL)
    {
        return;
    }

    videoBiosInterface->userContext                        = videoServices;
    videoBiosInterface->pc                                 = videoServices->pc;
    videoBiosInterface->getVideoMode                       = get_video_bios_mode;
    videoBiosInterface->getVideoColumnCount                = get_video_bios_column_count;
    videoBiosInterface->setVideoMode                       = set_video_bios_mode;
    videoBiosInterface->setActiveVideoPage                 = set_video_bios_active_page;
    videoBiosInterface->setTextCursorPosition              = set_video_bios_text_cursor_position;
    videoBiosInterface->getTextCursorPosition              = get_video_bios_text_cursor_position;
    videoBiosInterface->readTextCharacterAttributeAtCursor = read_video_bios_text_character_attribute_at_cursor;
    videoBiosInterface->initializeStaticFunctionalityTable = initialize_video_bios_static_functionality_table;
    videoBiosInterface->scrollTextRegion                   = scroll_video_bios_text_region;
    videoBiosInterface->writeCharacterRepeated             = write_video_bios_character_repeated;
    videoBiosInterface->writePixel                         = write_video_bios_pixel;
    videoBiosInterface->readPixel                          = read_video_bios_pixel;
    videoBiosInterface->writeTeletypeCharacter             = write_video_bios_teletype_character;
    videoBiosInterface->writeString                        = write_video_bios_string;
    videoBiosInterface->writeFunctionalityStateTable       = write_video_bios_functionality_state_table;
    videoBiosInterface->getSaveRestoreStateBufferBlocks    = get_video_save_restore_state_buffer_blocks;
    videoBiosInterface->writeSaveRestoreState              = write_video_bios_save_restore_state;
    videoBiosInterface->restoreSaveRestoreState            = restore_video_bios_save_restore_state;
    videoBiosInterface->registerGroupIsSingleRegister      = video_register_interface_group_is_single_register;
    videoBiosInterface->readRegisterInterfaceRegister      = read_video_bios_register_interface_register;
    videoBiosInterface->writeRegisterInterfaceRegister     = write_video_bios_register_interface_register;
    videoBiosInterface->writeKoreanExtensionPlaceholderCharacterShape =
            write_video_bios_korean_extension_placeholder_character_shape;
    videoBiosInterface->traceFunction = trace_video_bios_event;
}

static uint32_t make_packed_color(uint8_t red, uint8_t green, uint8_t blue)
{
    return (uint32_t)red | ((uint32_t)green << 8u) | ((uint32_t)blue << 16u);
}

uint32_t hyperdos_pc_video_services_color_graphics_adapter_color_from_index(uint8_t colorIndex)
{
    static const uint32_t colors[16] = {0x00000000u,
                                        0x00AA0000u,
                                        0x0000AA00u,
                                        0x00AAAA00u,
                                        0x000000AAu,
                                        0x00AA00AAu,
                                        0x000055AAu,
                                        0x00AAAAAAu,
                                        0x00555555u,
                                        0x00FF5555u,
                                        0x0055FF55u,
                                        0x00FFFF55u,
                                        0x005555FFu,
                                        0x00FF55FFu,
                                        0x0055FFFFu,
                                        0x00FFFFFFu};

    return colors[colorIndex & 0x0Fu];
}

static void build_graphics_palette(const hyperdos_color_graphics_adapter* adapter, uint32_t palette[4])
{
    static const uint8_t lowPalette[4]  = {0u, 2u, 4u, 6u};
    static const uint8_t highPalette[4] = {0u, 3u, 5u, 7u};
    const uint8_t*       paletteIndexes = (adapter->colorSelect & 0x20u) != 0u ? highPalette : lowPalette;
    uint8_t              intensity      = (adapter->colorSelect & 0x10u) != 0u ? 8u : 0u;
    size_t               paletteIndex   = 0u;

    palette[0] = hyperdos_pc_video_services_color_graphics_adapter_color_from_index(
            (uint8_t)(adapter->colorSelect & 0x0Fu));
    for (paletteIndex = 1u; paletteIndex < 4u; ++paletteIndex)
    {
        palette[paletteIndex] = hyperdos_pc_video_services_color_graphics_adapter_color_from_index(
                (uint8_t)(paletteIndexes[paletteIndex] | intensity));
    }
}

void hyperdos_pc_video_services_render_low_resolution_graphics_pixels(const hyperdos_color_graphics_adapter* adapter,
                                                                      uint32_t*                              pixels)
{
    uint32_t palette[4];
    uint32_t row        = 0u;
    uint32_t byteColumn = 0u;

    if (adapter == NULL || pixels == NULL)
    {
        return;
    }

    build_graphics_palette(adapter, palette);
    for (row = 0u; row < HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_HEIGHT; ++row)
    {
        uint32_t rowMemoryOffset  = (row & 1u) != 0u ? 0x2000u : 0u;
        rowMemoryOffset          += (row >> 1u) * 80u;
        for (byteColumn = 0u; byteColumn < 80u; ++byteColumn)
        {
            uint8_t  packedPixels = adapter->memory[rowMemoryOffset + byteColumn];
            uint32_t pixelColumn  = byteColumn * 4u;

            pixels[row * HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_WIDTH_LOW +
                   pixelColumn] = palette[(packedPixels >> 6u) & 0x03u];
            pixels[row * HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_WIDTH_LOW + pixelColumn +
                   1u]          = palette[(packedPixels >> 4u) & 0x03u];
            pixels[row * HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_WIDTH_LOW + pixelColumn +
                   2u]          = palette[(packedPixels >> 2u) & 0x03u];
            pixels[row * HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_WIDTH_LOW + pixelColumn + 3u] = palette[packedPixels &
                                                                                                          0x03u];
        }
    }
}

void hyperdos_pc_video_services_render_high_resolution_graphics_pixels(const hyperdos_color_graphics_adapter* adapter,
                                                                       uint32_t*                              pixels)
{
    uint32_t foregroundColor = 0u;
    uint32_t backgroundColor = make_packed_color(0u, 0u, 0u);
    uint32_t row             = 0u;
    uint32_t byteColumn      = 0u;

    if (adapter == NULL || pixels == NULL)
    {
        return;
    }

    foregroundColor = hyperdos_pc_video_services_color_graphics_adapter_color_from_index(
            (uint8_t)(adapter->colorSelect & 0x0Fu));
    for (row = 0u; row < HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_HEIGHT; ++row)
    {
        uint32_t rowMemoryOffset  = (row & 1u) != 0u ? 0x2000u : 0u;
        rowMemoryOffset          += (row >> 1u) * 80u;
        for (byteColumn = 0u; byteColumn < 80u; ++byteColumn)
        {
            uint8_t  packedPixels = adapter->memory[rowMemoryOffset + byteColumn];
            uint32_t pixelColumn  = byteColumn * 8u;
            uint32_t bitIndex     = 0u;

            for (bitIndex = 0u; bitIndex < 8u; ++bitIndex)
            {
                uint8_t mask     = (uint8_t)(0x80u >> bitIndex);
                pixels[row * HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_WIDTH_HIGH + pixelColumn +
                       bitIndex] = (packedPixels & mask) != 0u ? foregroundColor : backgroundColor;
            }
        }
    }
}

static uint8_t expand_digital_to_analog_component(uint8_t component)
{
    component = (uint8_t)(component & 0x3Fu);
    return (uint8_t)((component << 2u) | (component >> 4u));
}

static uint32_t color_from_digital_to_analog_converter_index(const hyperdos_color_graphics_adapter* adapter,
                                                             uint8_t                                colorIndex)
{
    uint8_t        maskedColorIndex = (uint8_t)(colorIndex & adapter->digitalToAnalogConverterMaskRegister);
    const uint8_t* color            = adapter->digitalToAnalogConverterPalette[maskedColorIndex];

    return make_packed_color(expand_digital_to_analog_component(color[0]),
                             expand_digital_to_analog_component(color[1]),
                             expand_digital_to_analog_component(color[2]));
}

static uint32_t color_from_attribute_palette(const hyperdos_color_graphics_adapter* adapter, uint8_t colorIndex)
{
    uint8_t colorPlaneEnable = adapter->attributeControllerRegisters[0x12u] & 0x0Fu;
    uint8_t modeControl      = adapter->attributeControllerRegisters[0x10u];
    uint8_t colorSelect      = adapter->attributeControllerRegisters[0x14u] & 0x0Fu;
    uint8_t paletteIndex = (uint8_t)(adapter->attributeControllerRegisters[(colorIndex & colorPlaneEnable) & 0x0Fu] &
                                     0x3Fu);

    if ((modeControl & 0x80u) != 0u)
    {
        paletteIndex = (uint8_t)((paletteIndex & 0x0Fu) | ((colorSelect & 0x03u) << 4u));
    }
    paletteIndex = (uint8_t)(paletteIndex | ((colorSelect & 0x0Cu) << 4u));
    return color_from_digital_to_analog_converter_index(adapter, paletteIndex);
}

int hyperdos_pc_video_services_get_video_graphics_array_mode_dimensions(uint8_t videoMode,
                                                                        int*    sourceWidth,
                                                                        int*    sourceHeight)
{
    if (sourceWidth == NULL || sourceHeight == NULL)
    {
        return 0;
    }

    switch (videoMode)
    {
    case 0x0Du:
    case 0x13u:
        *sourceWidth  = 320;
        *sourceHeight = 200;
        return 1;
    case 0x0Eu:
        *sourceWidth  = 640;
        *sourceHeight = 200;
        return 1;
    case 0x0Fu:
    case 0x10u:
        *sourceWidth  = 640;
        *sourceHeight = 350;
        return 1;
    case 0x11u:
    case 0x12u:
        *sourceWidth  = 640;
        *sourceHeight = 480;
        return 1;
    }
    return 0;
}

void hyperdos_pc_video_services_render_planar_video_graphics_array_pixels(
        const hyperdos_color_graphics_adapter* adapter,
        uint8_t                                videoMode,
        int                                    sourceWidth,
        int                                    sourceHeight,
        uint32_t*                              pixels)
{
    uint32_t defaultBytesPerScanLine = (uint32_t)((sourceWidth + 7) / 8);
    uint32_t displayStartAddress     = 0u;
    uint32_t bytesPerScanLine        = 0u;
    uint32_t row                     = 0u;

    if (adapter == NULL || pixels == NULL || sourceWidth <= 0 || sourceHeight <= 0)
    {
        return;
    }

    displayStartAddress = hyperdos_color_graphics_adapter_get_display_start_address(adapter);
    bytesPerScanLine    = hyperdos_color_graphics_adapter_get_display_stride_byte_count(adapter);
    if (bytesPerScanLine == 0u)
    {
        bytesPerScanLine = defaultBytesPerScanLine;
    }
    for (row = 0u; row < (uint32_t)sourceHeight; ++row)
    {
        uint32_t byteColumn = 0u;
        for (byteColumn = 0u; byteColumn < defaultBytesPerScanLine; ++byteColumn)
        {
            uint32_t planeAddress = (displayStartAddress + row * bytesPerScanLine + byteColumn) &
                                    (HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_SIZE - 1u);
            uint8_t  planeZeroByte  = adapter->graphicsPlanes[0][planeAddress];
            uint8_t  planeOneByte   = adapter->graphicsPlanes[1][planeAddress];
            uint8_t  planeTwoByte   = adapter->graphicsPlanes[2][planeAddress];
            uint8_t  planeThreeByte = adapter->graphicsPlanes[3][planeAddress];
            uint32_t bitIndex       = 0u;

            for (bitIndex = 0u; bitIndex < 8u; ++bitIndex)
            {
                uint8_t  bitMask     = (uint8_t)(0x80u >> bitIndex);
                uint8_t  colorIndex  = (uint8_t)(((planeZeroByte & bitMask) != 0u ? 0x01u : 0u) |
                                               ((planeOneByte & bitMask) != 0u ? 0x02u : 0u) |
                                               ((planeTwoByte & bitMask) != 0u ? 0x04u : 0u) |
                                               ((planeThreeByte & bitMask) != 0u ? 0x08u : 0u));
                uint32_t pixelColumn = byteColumn * 8u + bitIndex;

                if (pixelColumn >= (uint32_t)sourceWidth)
                {
                    continue;
                }
                if (videoMode == 0x0Fu || videoMode == 0x11u)
                {
                    colorIndex                                        = (uint8_t)(colorIndex & 0x01u);
                    pixels[row * (uint32_t)sourceWidth + pixelColumn] = colorIndex != 0u
                                                                                ? make_packed_color(255u, 255u, 255u)
                                                                                : make_packed_color(0u, 0u, 0u);
                }
                else
                {
                    pixels[row * (uint32_t)sourceWidth + pixelColumn] = color_from_attribute_palette(adapter,
                                                                                                     colorIndex);
                }
            }
        }
    }
}

void hyperdos_pc_video_services_render_linear_256_color_video_graphics_array_pixels(
        const hyperdos_color_graphics_adapter* adapter,
        int                                    sourceWidth,
        int                                    sourceHeight,
        uint32_t*                              pixels)
{
    uint32_t displayStartPixel   = 0u;
    uint32_t displayStridePixels = 0u;
    uint32_t row                 = 0u;

    if (adapter == NULL || pixels == NULL || sourceWidth <= 0 || sourceHeight <= 0)
    {
        return;
    }

    displayStartPixel = (uint32_t)hyperdos_color_graphics_adapter_get_display_start_address(adapter) *
                        HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT;
    displayStridePixels = (uint32_t)hyperdos_color_graphics_adapter_get_display_stride_byte_count(adapter) *
                          HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT;
    if (displayStridePixels == 0u)
    {
        displayStridePixels = (uint32_t)sourceWidth;
    }
    for (row = 0u; row < (uint32_t)sourceHeight; ++row)
    {
        uint32_t column = 0u;
        for (column = 0u; column < (uint32_t)sourceWidth; ++column)
        {
            uint32_t targetPixelOffset = row * (uint32_t)sourceWidth + column;
            uint32_t sourcePixelOffset = displayStartPixel + row * displayStridePixels + column;
            size_t   planeIndex        = sourcePixelOffset & 0x03u;
            uint32_t planeAddress      = (sourcePixelOffset >> 2u) & (HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_SIZE - 1u);
            uint8_t  colorIndex        = adapter->graphicsPlanes[planeIndex][planeAddress];

            pixels[targetPixelOffset] = color_from_digital_to_analog_converter_index(adapter, colorIndex);
        }
    }
}
