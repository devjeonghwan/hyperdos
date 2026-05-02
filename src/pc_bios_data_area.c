#include "hyperdos/pc_bios_data_area.h"

enum
{
    HYPERDOS_PC_BIOS_DATA_AREA_CURSOR_POSITION_OFFSET = 0x0050u,
};

void hyperdos_pc_bios_data_area_write_byte(hyperdos_pc* pc, uint16_t offset, uint8_t value)
{
    hyperdos_bus_write_memory_byte_if_mapped(&pc->bus, HYPERDOS_PC_BIOS_DATA_AREA_BASE_ADDRESS + offset, value);
}

void hyperdos_pc_bios_data_area_write_word(hyperdos_pc* pc, uint16_t offset, uint16_t value)
{
    hyperdos_pc_bios_data_area_write_byte(pc, offset, (uint8_t)(value & HYPERDOS_X86_LOW_BYTE_MASK));
    hyperdos_pc_bios_data_area_write_byte(pc, (uint16_t)(offset + 1u), (uint8_t)(value >> HYPERDOS_X86_BYTE_BIT_COUNT));
}

void hyperdos_pc_bios_data_area_write_double_word(hyperdos_pc* pc, uint16_t offset, uint32_t value)
{
    hyperdos_pc_bios_data_area_write_word(pc, offset, (uint16_t)(value & HYPERDOS_X86_WORD_MASK));
    hyperdos_pc_bios_data_area_write_word(pc,
                                          (uint16_t)(offset + HYPERDOS_X86_WORD_SIZE),
                                          (uint16_t)(value >> HYPERDOS_X86_WORD_BIT_COUNT));
}

uint8_t hyperdos_pc_bios_data_area_read_byte(hyperdos_pc* pc, uint16_t offset)
{
    return hyperdos_bus_read_memory_byte_or_open_bus(&pc->bus, HYPERDOS_PC_BIOS_DATA_AREA_BASE_ADDRESS + offset);
}

uint16_t hyperdos_pc_bios_data_area_read_word(hyperdos_pc* pc, uint16_t offset)
{
    uint16_t lowByte  = hyperdos_pc_bios_data_area_read_byte(pc, offset);
    uint16_t highByte = hyperdos_pc_bios_data_area_read_byte(pc, (uint16_t)(offset + 1u));

    return (uint16_t)(lowByte | (highByte << HYPERDOS_X86_BYTE_BIT_COUNT));
}

uint32_t hyperdos_pc_bios_data_area_read_double_word(hyperdos_pc* pc, uint16_t offset)
{
    uint32_t lowWord  = hyperdos_pc_bios_data_area_read_word(pc, offset);
    uint32_t highWord = hyperdos_pc_bios_data_area_read_word(pc, (uint16_t)(offset + HYPERDOS_X86_WORD_SIZE));

    return lowWord | (highWord << HYPERDOS_X86_WORD_BIT_COUNT);
}

uint16_t hyperdos_pc_bios_data_area_get_cursor_position_offset(uint8_t videoPage)
{
    return (uint16_t)(HYPERDOS_PC_BIOS_DATA_AREA_CURSOR_POSITION_OFFSET + videoPage * HYPERDOS_X86_WORD_SIZE);
}
