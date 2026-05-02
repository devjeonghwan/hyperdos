#ifndef HYPERDOS_PC_SYSTEM_BIOS_H
#define HYPERDOS_PC_SYSTEM_BIOS_H

#include <stdint.h>

#include "hyperdos/pc_board.h"

enum
{
    HYPERDOS_PC_SYSTEM_BIOS_MODEL_IDENTIFIER_PC = 0xFFu,
    HYPERDOS_PC_SYSTEM_BIOS_MODEL_IDENTIFIER_XT = 0xFEu,
    HYPERDOS_PC_SYSTEM_BIOS_MODEL_IDENTIFIER_AT = 0xFCu
};

typedef struct hyperdos_pc_system_bios
{
    uint8_t  a20GateEnabled;
    uint8_t  modelIdentifier;
    uint8_t  waitEventActive;
    uint32_t waitEventFlagPhysicalAddress;
    uint64_t waitEventCompletionClockCount;
    uint16_t pointingDeviceHandlerOffset;
    uint16_t pointingDeviceHandlerSegment;
    uint8_t  pointingDeviceEnabled;
    uint8_t  pointingDeviceSampleRate;
    uint8_t  pointingDeviceResolution;
    uint8_t  pointingDeviceScalingTwoToOne;
    uint8_t  pointingDevicePacketBytes[3];
    uint8_t  pointingDevicePacketByteCount;
} hyperdos_pc_system_bios;

void hyperdos_pc_system_bios_initialize(hyperdos_pc_system_bios* systemBios);

void hyperdos_pc_system_bios_reset(hyperdos_pc_system_bios* systemBios);

void hyperdos_pc_system_bios_set_model_identifier(hyperdos_pc_system_bios* systemBios, uint8_t modelIdentifier);

uint8_t hyperdos_pc_system_bios_get_model_identifier(const hyperdos_pc_system_bios* systemBios);

int hyperdos_pc_system_bios_model_is_at_compatible(uint8_t modelIdentifier);

void hyperdos_pc_system_bios_install_configuration_table(hyperdos_pc* pc, const hyperdos_pc_system_bios* systemBios);

void hyperdos_pc_system_bios_install_identification(hyperdos_pc* pc, const hyperdos_pc_system_bios* systemBios);

void hyperdos_pc_system_bios_initialize_data_area(hyperdos_pc* pc);

void hyperdos_pc_system_bios_service_wait_event(hyperdos_pc_system_bios* systemBios, hyperdos_pc* pc);

void hyperdos_pc_system_bios_advance_timer_tick(hyperdos_pc* pc);

uint16_t hyperdos_pc_system_bios_get_equipment_flags(uint8_t coprocessorEnabled,
                                                     uint8_t floppyDriveCount,
                                                     uint8_t pointingDevicePresent);

uint16_t hyperdos_pc_system_bios_get_conventional_memory_size_kilobytes(void);

hyperdos_x86_execution_result hyperdos_pc_system_bios_handle_equipment_interrupt(hyperdos_x86_processor* processor,
                                                                                 uint8_t coprocessorEnabled,
                                                                                 uint8_t floppyDriveCount,
                                                                                 uint8_t pointingDevicePresent);

hyperdos_x86_execution_result hyperdos_pc_system_bios_handle_memory_size_interrupt(hyperdos_x86_processor* processor);

hyperdos_x86_execution_result hyperdos_pc_system_bios_handle_serial_interrupt(hyperdos_x86_processor* processor,
                                                                              uint8_t                 serviceNumber);

hyperdos_x86_execution_result hyperdos_pc_system_bios_handle_system_services_interrupt(
        hyperdos_x86_processor*  processor,
        hyperdos_pc*             pc,
        hyperdos_pc_system_bios* systemBios,
        uint8_t                  serviceNumber);

hyperdos_x86_execution_result hyperdos_pc_system_bios_handle_printer_interrupt(hyperdos_x86_processor* processor,
                                                                               uint8_t                 serviceNumber);

hyperdos_x86_execution_result hyperdos_pc_system_bios_handle_time_interrupt(hyperdos_x86_processor* processor,
                                                                            hyperdos_pc*            pc,
                                                                            uint8_t                 serviceNumber);

hyperdos_x86_execution_result hyperdos_pc_system_bios_handle_expanded_memory_manager_interrupt(
        hyperdos_x86_processor* processor,
        uint8_t                 serviceNumber);

#endif
