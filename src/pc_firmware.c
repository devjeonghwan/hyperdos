#include "hyperdos/pc_firmware.h"

enum
{
    HYPERDOS_PC_FIRMWARE_INTERRUPT_VECTOR_BYTE_COUNT        = 4u,
    HYPERDOS_PC_FIRMWARE_OPERATION_CODE_INTERRUPT_IMMEDIATE = 0xCDu,
    HYPERDOS_PC_FIRMWARE_OPERATION_CODE_INTERRUPT_RETURN    = 0xCFu
};

static void hyperdos_pc_firmware_write_guest_memory_byte(hyperdos_pc* pc, uint32_t physicalAddress, uint8_t value)
{
    hyperdos_bus_write_memory_byte_if_mapped(&pc->bus, physicalAddress & HYPERDOS_X86_ADDRESS_MASK, value);
}

static void hyperdos_pc_firmware_write_guest_memory_word(hyperdos_pc* pc, uint32_t physicalAddress, uint16_t value)
{
    hyperdos_pc_firmware_write_guest_memory_byte(pc, physicalAddress, (uint8_t)(value & HYPERDOS_X86_LOW_BYTE_MASK));
    hyperdos_pc_firmware_write_guest_memory_byte(pc,
                                                 physicalAddress + 1u,
                                                 (uint8_t)(value >> HYPERDOS_X86_BYTE_BIT_COUNT));
}

void hyperdos_pc_firmware_write_byte(hyperdos_pc* pc, uint32_t physicalAddress, uint8_t value)
{
    pc->processorMemory[physicalAddress & HYPERDOS_X86_ADDRESS_MASK] = value;
}

void hyperdos_pc_firmware_install_interrupt_vector_stub(hyperdos_pc* pc,
                                                        uint8_t      interruptNumber,
                                                        uint8_t      monitorServiceInterruptNumber,
                                                        uint16_t     stubSegment,
                                                        uint16_t     stubOffset)
{
    uint32_t interruptVectorAddress = (uint32_t)interruptNumber * HYPERDOS_PC_FIRMWARE_INTERRUPT_VECTOR_BYTE_COUNT;
    uint32_t stubPhysicalAddress    = ((uint32_t)stubSegment << 4u) + stubOffset;

    hyperdos_pc_firmware_write_byte(pc, stubPhysicalAddress, HYPERDOS_PC_FIRMWARE_OPERATION_CODE_INTERRUPT_IMMEDIATE);
    hyperdos_pc_firmware_write_byte(pc, stubPhysicalAddress + 1u, monitorServiceInterruptNumber);
    hyperdos_pc_firmware_write_byte(pc,
                                    stubPhysicalAddress + HYPERDOS_X86_WORD_SIZE,
                                    HYPERDOS_PC_FIRMWARE_OPERATION_CODE_INTERRUPT_RETURN);
    hyperdos_pc_firmware_write_guest_memory_word(pc, interruptVectorAddress, stubOffset);
    hyperdos_pc_firmware_write_guest_memory_word(pc, interruptVectorAddress + HYPERDOS_X86_WORD_SIZE, stubSegment);
}
