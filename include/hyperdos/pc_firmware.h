#ifndef HYPERDOS_PC_FIRMWARE_H
#define HYPERDOS_PC_FIRMWARE_H

#include <stdint.h>

#include "hyperdos/pc_board.h"

void hyperdos_pc_firmware_write_byte(hyperdos_pc* pc, uint32_t physicalAddress, uint8_t value);

void hyperdos_pc_firmware_install_interrupt_vector_stub(hyperdos_pc* pc,
                                                        uint8_t      interruptNumber,
                                                        uint8_t      monitorServiceInterruptNumber,
                                                        uint16_t     stubSegment,
                                                        uint16_t     stubOffset);

#endif
