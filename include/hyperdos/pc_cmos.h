#ifndef HYPERDOS_PC_CMOS_H
#define HYPERDOS_PC_CMOS_H

#include <stdint.h>

enum
{
    HYPERDOS_PC_CMOS_ADDRESS_PORT                 = 0x0070u,
    HYPERDOS_PC_CMOS_DATA_PORT                    = 0x0071u,
    HYPERDOS_PC_CMOS_PORT_COUNT                   = 2u,
    HYPERDOS_PC_CMOS_REGISTER_COUNT               = 0x40u,
    HYPERDOS_PC_CMOS_MAXIMUM_FLOPPY_DRIVE_COUNT   = 2u,
    HYPERDOS_PC_CMOS_MAXIMUM_FIXED_DISK_COUNT     = 2u,
    HYPERDOS_PC_CMOS_USER_DEFINED_FIXED_DISK_TYPE = 47u
};

typedef struct hyperdos_pc_cmos_fixed_disk_geometry
{
    uint16_t cylinderCount;
    uint8_t  headCount;
    uint8_t  sectorsPerTrack;
    uint8_t  installed;
} hyperdos_pc_cmos_fixed_disk_geometry;

typedef struct hyperdos_pc_cmos
{
    uint8_t registers[HYPERDOS_PC_CMOS_REGISTER_COUNT];
    uint8_t selectedRegister;
    uint8_t nonMaskableInterruptDisabled;
} hyperdos_pc_cmos;

void hyperdos_pc_cmos_initialize(hyperdos_pc_cmos* cmos);

void hyperdos_pc_cmos_set_equipment_flags(hyperdos_pc_cmos* cmos, uint16_t equipmentFlags);

void hyperdos_pc_cmos_set_base_memory_size_kilobytes(hyperdos_pc_cmos* cmos, uint16_t memorySizeKilobytes);

void hyperdos_pc_cmos_set_extended_memory_size_kilobytes(hyperdos_pc_cmos* cmos, uint16_t memorySizeKilobytes);

void hyperdos_pc_cmos_set_floppy_drive_type(hyperdos_pc_cmos* cmos, uint8_t driveNumber, uint8_t driveType);

void hyperdos_pc_cmos_set_fixed_disk_geometry(hyperdos_pc_cmos*                           cmos,
                                              uint8_t                                     fixedDiskIndex,
                                              const hyperdos_pc_cmos_fixed_disk_geometry* geometry);

uint8_t hyperdos_pc_cmos_read_input_output_byte(void* device, uint16_t port);

void hyperdos_pc_cmos_write_input_output_byte(void* device, uint16_t port, uint8_t value);

#endif
