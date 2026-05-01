#ifndef HYPERDOS_PC_BIOS_DATA_AREA_H
#define HYPERDOS_PC_BIOS_DATA_AREA_H

#include <stdint.h>

#include "hyperdos/pc_board.h"

enum
{
    HYPERDOS_PC_BIOS_DATA_AREA_BASE_ADDRESS                    = 0x00400u,
    HYPERDOS_PC_BIOS_DATA_AREA_EQUIPMENT_FLAGS_OFFSET          = 0x0010u,
    HYPERDOS_PC_BIOS_DATA_AREA_CONVENTIONAL_MEMORY_SIZE_OFFSET = 0x0013u,
    HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_SHIFT_FLAGS_OFFSET     = 0x0017u,
    HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_HEAD_OFFSET            = 0x001Au,
    HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_TAIL_OFFSET            = 0x001Cu,
    HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_BUFFER_START           = 0x001Eu,
    HYPERDOS_PC_BIOS_DATA_AREA_TIMER_TICK_COUNT_OFFSET         = 0x006Cu,
    HYPERDOS_PC_BIOS_DATA_AREA_TIMER_MIDNIGHT_FLAG_OFFSET      = 0x0070u,
    HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_BUFFER_START_OFFSET    = 0x0080u,
    HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_BUFFER_END_OFFSET      = 0x0082u,
    HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_BUFFER_END             = 0x003Eu
};

void hyperdos_pc_bios_data_area_write_byte(hyperdos_pc* pc, uint16_t offset, uint8_t value);

void hyperdos_pc_bios_data_area_write_word(hyperdos_pc* pc, uint16_t offset, uint16_t value);

void hyperdos_pc_bios_data_area_write_double_word(hyperdos_pc* pc, uint16_t offset, uint32_t value);

uint8_t hyperdos_pc_bios_data_area_read_byte(hyperdos_pc* pc, uint16_t offset);

uint16_t hyperdos_pc_bios_data_area_read_word(hyperdos_pc* pc, uint16_t offset);

uint32_t hyperdos_pc_bios_data_area_read_double_word(hyperdos_pc* pc, uint16_t offset);

uint16_t hyperdos_pc_bios_data_area_get_cursor_position_offset(uint8_t videoPage);

#endif
