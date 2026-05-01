#include "hyperdos/pc_cmos.h"

#include <string.h>

enum
{
    HYPERDOS_PC_CMOS_OPEN_BUS_BYTE                                          = 0xFFu,
    HYPERDOS_PC_CMOS_REGISTER_MASK                                          = 0x3Fu,
    HYPERDOS_PC_CMOS_NON_MASKABLE_INTERRUPT_DISABLE_BIT                     = 0x80u,
    HYPERDOS_PC_CMOS_STATUS_REGISTER_A                                      = 0x0Au,
    HYPERDOS_PC_CMOS_STATUS_REGISTER_B                                      = 0x0Bu,
    HYPERDOS_PC_CMOS_STATUS_REGISTER_C                                      = 0x0Cu,
    HYPERDOS_PC_CMOS_STATUS_REGISTER_D                                      = 0x0Du,
    HYPERDOS_PC_CMOS_FLOPPY_TYPE_REGISTER                                   = 0x10u,
    HYPERDOS_PC_CMOS_FIXED_DISK_TYPE_REGISTER                               = 0x12u,
    HYPERDOS_PC_CMOS_EQUIPMENT_REGISTER                                     = 0x14u,
    HYPERDOS_PC_CMOS_BASE_MEMORY_LOW_REGISTER                               = 0x15u,
    HYPERDOS_PC_CMOS_BASE_MEMORY_HIGH_REGISTER                              = 0x16u,
    HYPERDOS_PC_CMOS_FIRST_FIXED_DISK_EXTENDED_TYPE_REGISTER                = 0x19u,
    HYPERDOS_PC_CMOS_SECOND_FIXED_DISK_EXTENDED_TYPE_REGISTER               = 0x1Au,
    HYPERDOS_PC_CMOS_FIRST_FIXED_DISK_PARAMETER_BASE_REGISTER               = 0x1Bu,
    HYPERDOS_PC_CMOS_SECOND_FIXED_DISK_PARAMETER_BASE_REGISTER              = 0x24u,
    HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_CYLINDER_LOW_OFFSET               = 0u,
    HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_CYLINDER_HIGH_OFFSET              = 1u,
    HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_HEAD_COUNT_OFFSET                 = 2u,
    HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_WRITE_PRECOMPENSATION_LOW_OFFSET  = 3u,
    HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_WRITE_PRECOMPENSATION_HIGH_OFFSET = 4u,
    HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_CONTROL_BYTE_OFFSET               = 5u,
    HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_LANDING_ZONE_LOW_OFFSET           = 6u,
    HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_LANDING_ZONE_HIGH_OFFSET          = 7u,
    HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_SECTORS_PER_TRACK_OFFSET          = 8u,
    HYPERDOS_PC_CMOS_STATUS_REGISTER_A_DEFAULT                              = 0x26u,
    HYPERDOS_PC_CMOS_STATUS_REGISTER_B_DEFAULT                              = 0x02u,
    HYPERDOS_PC_CMOS_STATUS_REGISTER_D_VALID_RAM_BIT                        = 0x80u,
    HYPERDOS_PC_CMOS_CONVENTIONAL_MEMORY_KILOBYTES                          = 640u,
    HYPERDOS_PC_CMOS_FIXED_DISK_TYPE_USER_DEFINED_NIBBLE                    = 0x0Fu,
    HYPERDOS_PC_CMOS_FIXED_DISK_CONTROL_DISABLE_RETRY_BIT                   = 0xC0u,
    HYPERDOS_PC_CMOS_FIXED_DISK_CONTROL_MORE_THAN_EIGHT_HEADS_BIT           = 0x08u,
    HYPERDOS_PC_CMOS_FIXED_DISK_WRITE_PRECOMPENSATION_UNUSED                = 0xFFFFu
};

static void hyperdos_pc_cmos_write_word_to_registers(hyperdos_pc_cmos* cmos, uint8_t lowRegister, uint16_t value)
{
    if (cmos == NULL || lowRegister + 1u >= HYPERDOS_PC_CMOS_REGISTER_COUNT)
    {
        return;
    }
    cmos->registers[lowRegister]      = (uint8_t)(value & 0x00FFu);
    cmos->registers[lowRegister + 1u] = (uint8_t)(value >> 8u);
}

static uint8_t hyperdos_pc_cmos_get_fixed_disk_extended_type_register(uint8_t fixedDiskIndex)
{
    return fixedDiskIndex == 0u ? HYPERDOS_PC_CMOS_FIRST_FIXED_DISK_EXTENDED_TYPE_REGISTER
                                : HYPERDOS_PC_CMOS_SECOND_FIXED_DISK_EXTENDED_TYPE_REGISTER;
}

static uint8_t hyperdos_pc_cmos_get_fixed_disk_parameter_base_register(uint8_t fixedDiskIndex)
{
    return fixedDiskIndex == 0u ? HYPERDOS_PC_CMOS_FIRST_FIXED_DISK_PARAMETER_BASE_REGISTER
                                : HYPERDOS_PC_CMOS_SECOND_FIXED_DISK_PARAMETER_BASE_REGISTER;
}

static void hyperdos_pc_cmos_set_fixed_disk_type_nibble(hyperdos_pc_cmos* cmos,
                                                        uint8_t           fixedDiskIndex,
                                                        uint8_t           installed)
{
    uint8_t fixedDiskTypeRegister = 0u;

    if (cmos == NULL || fixedDiskIndex >= HYPERDOS_PC_CMOS_MAXIMUM_FIXED_DISK_COUNT)
    {
        return;
    }

    fixedDiskTypeRegister = cmos->registers[HYPERDOS_PC_CMOS_FIXED_DISK_TYPE_REGISTER];
    if (fixedDiskIndex == 0u)
    {
        fixedDiskTypeRegister = (uint8_t)((fixedDiskTypeRegister & 0x0Fu) | (installed ? 0xF0u : 0x00u));
    }
    else
    {
        fixedDiskTypeRegister = (uint8_t)((fixedDiskTypeRegister & 0xF0u) |
                                          (installed ? HYPERDOS_PC_CMOS_FIXED_DISK_TYPE_USER_DEFINED_NIBBLE : 0x00u));
    }
    cmos->registers[HYPERDOS_PC_CMOS_FIXED_DISK_TYPE_REGISTER] = fixedDiskTypeRegister;
}

static void hyperdos_pc_cmos_clear_fixed_disk_geometry(hyperdos_pc_cmos* cmos, uint8_t fixedDiskIndex)
{
    uint8_t baseRegister = 0u;

    if (cmos == NULL || fixedDiskIndex >= HYPERDOS_PC_CMOS_MAXIMUM_FIXED_DISK_COUNT)
    {
        return;
    }
    baseRegister = hyperdos_pc_cmos_get_fixed_disk_parameter_base_register(fixedDiskIndex);
    memset(&cmos->registers[baseRegister], 0, HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_SECTORS_PER_TRACK_OFFSET + 1u);
    cmos->registers[hyperdos_pc_cmos_get_fixed_disk_extended_type_register(fixedDiskIndex)] = 0u;
    hyperdos_pc_cmos_set_fixed_disk_type_nibble(cmos, fixedDiskIndex, 0u);
}

void hyperdos_pc_cmos_initialize(hyperdos_pc_cmos* cmos)
{
    if (cmos == NULL)
    {
        return;
    }

    memset(cmos, 0, sizeof(*cmos));
    cmos->registers[HYPERDOS_PC_CMOS_STATUS_REGISTER_A] = HYPERDOS_PC_CMOS_STATUS_REGISTER_A_DEFAULT;
    cmos->registers[HYPERDOS_PC_CMOS_STATUS_REGISTER_B] = HYPERDOS_PC_CMOS_STATUS_REGISTER_B_DEFAULT;
    cmos->registers[HYPERDOS_PC_CMOS_STATUS_REGISTER_C] = 0u;
    cmos->registers[HYPERDOS_PC_CMOS_STATUS_REGISTER_D] = HYPERDOS_PC_CMOS_STATUS_REGISTER_D_VALID_RAM_BIT;
    hyperdos_pc_cmos_write_word_to_registers(cmos,
                                             HYPERDOS_PC_CMOS_BASE_MEMORY_LOW_REGISTER,
                                             HYPERDOS_PC_CMOS_CONVENTIONAL_MEMORY_KILOBYTES);
}

void hyperdos_pc_cmos_set_equipment_flags(hyperdos_pc_cmos* cmos, uint16_t equipmentFlags)
{
    if (cmos == NULL)
    {
        return;
    }
    cmos->registers[HYPERDOS_PC_CMOS_EQUIPMENT_REGISTER] = (uint8_t)(equipmentFlags & 0x00FFu);
}

void hyperdos_pc_cmos_set_floppy_drive_type(hyperdos_pc_cmos* cmos, uint8_t driveNumber, uint8_t driveType)
{
    uint8_t floppyTypeRegister = 0u;

    if (cmos == NULL || driveNumber >= HYPERDOS_PC_CMOS_MAXIMUM_FLOPPY_DRIVE_COUNT)
    {
        return;
    }

    floppyTypeRegister = cmos->registers[HYPERDOS_PC_CMOS_FLOPPY_TYPE_REGISTER];
    if (driveNumber == 0u)
    {
        floppyTypeRegister = (uint8_t)((floppyTypeRegister & 0x0Fu) | ((driveType & 0x0Fu) << 4u));
    }
    else
    {
        floppyTypeRegister = (uint8_t)((floppyTypeRegister & 0xF0u) | (driveType & 0x0Fu));
    }
    cmos->registers[HYPERDOS_PC_CMOS_FLOPPY_TYPE_REGISTER] = floppyTypeRegister;
}

void hyperdos_pc_cmos_set_fixed_disk_geometry(hyperdos_pc_cmos*                           cmos,
                                              uint8_t                                     fixedDiskIndex,
                                              const hyperdos_pc_cmos_fixed_disk_geometry* geometry)
{
    uint8_t baseRegister = 0u;
    uint8_t controlByte  = HYPERDOS_PC_CMOS_FIXED_DISK_CONTROL_DISABLE_RETRY_BIT;

    if (cmos == NULL || fixedDiskIndex >= HYPERDOS_PC_CMOS_MAXIMUM_FIXED_DISK_COUNT)
    {
        return;
    }
    if (geometry == NULL || !geometry->installed || geometry->cylinderCount == 0u || geometry->headCount == 0u ||
        geometry->sectorsPerTrack == 0u)
    {
        hyperdos_pc_cmos_clear_fixed_disk_geometry(cmos, fixedDiskIndex);
        return;
    }

    baseRegister = hyperdos_pc_cmos_get_fixed_disk_parameter_base_register(fixedDiskIndex);
    hyperdos_pc_cmos_set_fixed_disk_type_nibble(cmos, fixedDiskIndex, 1u);
    cmos->registers[hyperdos_pc_cmos_get_fixed_disk_extended_type_register(
            fixedDiskIndex)] = HYPERDOS_PC_CMOS_USER_DEFINED_FIXED_DISK_TYPE;
    cmos->registers[baseRegister +
                    HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_CYLINDER_LOW_OFFSET]  = (uint8_t)(geometry->cylinderCount &
                                                                                           0x00FFu);
    cmos->registers[baseRegister +
                    HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_CYLINDER_HIGH_OFFSET] = (uint8_t)(geometry->cylinderCount >>
                                                                                            8u);
    cmos->registers[baseRegister + HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_HEAD_COUNT_OFFSET] = geometry->headCount;
    cmos->registers[baseRegister + HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_WRITE_PRECOMPENSATION_LOW_OFFSET] =
            (uint8_t)(HYPERDOS_PC_CMOS_FIXED_DISK_WRITE_PRECOMPENSATION_UNUSED & 0x00FFu);
    cmos->registers[baseRegister + HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_WRITE_PRECOMPENSATION_HIGH_OFFSET] =
            (uint8_t)(HYPERDOS_PC_CMOS_FIXED_DISK_WRITE_PRECOMPENSATION_UNUSED >> 8u);
    if (geometry->headCount > 8u)
    {
        controlByte = (uint8_t)(controlByte | HYPERDOS_PC_CMOS_FIXED_DISK_CONTROL_MORE_THAN_EIGHT_HEADS_BIT);
    }
    cmos->registers[baseRegister + HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_CONTROL_BYTE_OFFSET] = controlByte;
    cmos->registers[baseRegister +
                    HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_LANDING_ZONE_LOW_OFFSET] = (uint8_t)(geometry->cylinderCount &
                                                                                               0x00FFu);
    cmos->registers
            [baseRegister +
             HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_LANDING_ZONE_HIGH_OFFSET] = (uint8_t)(geometry->cylinderCount >> 8u);
    cmos->registers[baseRegister +
                    HYPERDOS_PC_CMOS_FIXED_DISK_PARAMETER_SECTORS_PER_TRACK_OFFSET] = geometry->sectorsPerTrack;
}

uint8_t hyperdos_pc_cmos_read_input_output_byte(void* device, uint16_t port)
{
    hyperdos_pc_cmos* cmos = (hyperdos_pc_cmos*)device;

    if (cmos == NULL)
    {
        return HYPERDOS_PC_CMOS_OPEN_BUS_BYTE;
    }
    if (port == HYPERDOS_PC_CMOS_ADDRESS_PORT)
    {
        return (uint8_t)(cmos->selectedRegister |
                         (cmos->nonMaskableInterruptDisabled ? HYPERDOS_PC_CMOS_NON_MASKABLE_INTERRUPT_DISABLE_BIT
                                                             : 0u));
    }
    if (port == HYPERDOS_PC_CMOS_DATA_PORT)
    {
        uint8_t value = 0u;

        if (cmos->selectedRegister >= HYPERDOS_PC_CMOS_REGISTER_COUNT)
        {
            return HYPERDOS_PC_CMOS_OPEN_BUS_BYTE;
        }
        value = cmos->registers[cmos->selectedRegister];
        if (cmos->selectedRegister == HYPERDOS_PC_CMOS_STATUS_REGISTER_C)
        {
            cmos->registers[HYPERDOS_PC_CMOS_STATUS_REGISTER_C] = 0u;
        }
        return value;
    }
    return HYPERDOS_PC_CMOS_OPEN_BUS_BYTE;
}

void hyperdos_pc_cmos_write_input_output_byte(void* device, uint16_t port, uint8_t value)
{
    hyperdos_pc_cmos* cmos = (hyperdos_pc_cmos*)device;

    if (cmos == NULL)
    {
        return;
    }
    if (port == HYPERDOS_PC_CMOS_ADDRESS_PORT)
    {
        cmos->selectedRegister             = (uint8_t)(value & HYPERDOS_PC_CMOS_REGISTER_MASK);
        cmos->nonMaskableInterruptDisabled = (uint8_t)((value & HYPERDOS_PC_CMOS_NON_MASKABLE_INTERRUPT_DISABLE_BIT) !=
                                                       0u);
        return;
    }
    if (port == HYPERDOS_PC_CMOS_DATA_PORT && cmos->selectedRegister < HYPERDOS_PC_CMOS_REGISTER_COUNT)
    {
        if (cmos->selectedRegister == HYPERDOS_PC_CMOS_STATUS_REGISTER_C)
        {
            return;
        }
        cmos->registers[cmos->selectedRegister] = value;
    }
}
