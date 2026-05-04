#include "hyperdos/pc_disk_bios.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "hyperdos/pc_bios_data_area.h"

enum
{
    HYPERDOS_PC_DISK_BIOS_TRACE_TEXT_CAPACITY                            = 512u,
    HYPERDOS_PC_DISK_BIOS_SERVICE_REGISTER_SHIFT                         = 8u,
    HYPERDOS_PC_DISK_BIOS_RESET_SERVICE                                  = 0x00u,
    HYPERDOS_PC_DISK_BIOS_STATUS_SERVICE                                 = 0x01u,
    HYPERDOS_PC_DISK_BIOS_READ_SECTORS_SERVICE                           = 0x02u,
    HYPERDOS_PC_DISK_BIOS_WRITE_SECTORS_SERVICE                          = 0x03u,
    HYPERDOS_PC_DISK_BIOS_DRIVE_PARAMETERS_SERVICE                       = 0x08u,
    HYPERDOS_PC_DISK_BIOS_GET_TYPE_SERVICE                               = 0x15u,
    HYPERDOS_PC_DISK_BIOS_CHANGE_LINE_STATUS_SERVICE                     = 0x16u,
    HYPERDOS_PC_DISK_BIOS_EXTENSION_INSTALLATION_CHECK_SERVICE           = 0x41u,
    HYPERDOS_PC_DISK_BIOS_STATUS_SUCCESS                                 = 0x00u,
    HYPERDOS_PC_DISK_BIOS_STATUS_INVALID_FUNCTION                        = 0x01u,
    HYPERDOS_PC_DISK_BIOS_STATUS_WRITE_PROTECTED                         = 0x03u,
    HYPERDOS_PC_DISK_BIOS_STATUS_SECTOR_NOT_FOUND                        = 0x04u,
    HYPERDOS_PC_DISK_BIOS_STATUS_CHANGED                                 = 0x06u,
    HYPERDOS_PC_DISK_BIOS_TYPE_FLOPPY_WITHOUT_CHANGE_LINE                = 0x01u,
    HYPERDOS_PC_DISK_BIOS_TYPE_FLOPPY_WITH_CHANGE_LINE                   = 0x02u,
    HYPERDOS_PC_DISK_BIOS_TYPE_FIXED                                     = 0x03u,
    HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_RECALIBRATION_STATUS_OFFSET = 0x003Eu,
    HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_MOTOR_STATUS_OFFSET         = 0x003Fu,
    HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_MOTOR_TIMEOUT_OFFSET        = 0x0040u,
    HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_STATUS_OFFSET               = 0x0041u,
    HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_CONTROLLER_STATUS_OFFSET    = 0x0042u,
    HYPERDOS_PC_DISK_BIOS_DATA_AREA_HARD_DISK_STATUS_OFFSET              = 0x0074u,
    HYPERDOS_PC_DISK_BIOS_DATA_AREA_FIXED_DISK_COUNT_OFFSET              = 0x0075u,
    HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_CONFIGURATION_OFFSET        = 0x008Bu,
    HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_MEDIA_STATE_OFFSET          = 0x0090u,
    HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_CURRENT_CYLINDER_OFFSET     = 0x0094u,
    HYPERDOS_PC_DISK_BIOS_DISKETTE_RECALIBRATE_DRIVE_ZERO                = 0x01u,
    HYPERDOS_PC_DISK_BIOS_DISKETTE_DATA_RATE_500K                        = 0x00u,
    HYPERDOS_PC_DISK_BIOS_DISKETTE_DATA_RATE_250K                        = 0x80u,
    HYPERDOS_PC_DISK_BIOS_DISKETTE_MEDIA_STATE_UNKNOWN                   = 0x07u,
    HYPERDOS_PC_DISK_BIOS_DISKETTE_MEDIA_STATE_ESTABLISHED_BIT           = 0x10u,
    HYPERDOS_PC_DISK_BIOS_DISKETTE_MEDIA_STATE_DATA_RATE_250_KILOBITS    = 0x80u,
    HYPERDOS_PC_DISK_BIOS_DISKETTE_MEDIA_STATE_360K_ESTABLISHED =
            HYPERDOS_PC_DISK_BIOS_DISKETTE_MEDIA_STATE_DATA_RATE_250_KILOBITS |
            HYPERDOS_PC_DISK_BIOS_DISKETTE_MEDIA_STATE_ESTABLISHED_BIT | 0x03u,
    HYPERDOS_PC_DISK_BIOS_DISKETTE_MEDIA_STATE_12M_NOT_ESTABLISHED = 0x02u,
    HYPERDOS_PC_DISK_BIOS_DISKETTE_MEDIA_STATE_12M_ESTABLISHED =
            HYPERDOS_PC_DISK_BIOS_DISKETTE_MEDIA_STATE_ESTABLISHED_BIT | 0x05u,
    HYPERDOS_PC_DISK_BIOS_DISKETTE_CURRENT_CYLINDER_UNKNOWN = 0xFFu,
    HYPERDOS_PC_DISK_BIOS_DEFAULT_FLOPPY_CYLINDER_COUNT     = 80u,
    HYPERDOS_PC_DISK_BIOS_DEFAULT_FLOPPY_HEAD_COUNT         = 2u,
    HYPERDOS_PC_DISK_BIOS_DEFAULT_FLOPPY_SECTORS_PER_TRACK  = 18u,
    HYPERDOS_PC_DISK_BIOS_DEFAULT_FLOPPY_DRIVE_COUNT        = 1u,
    HYPERDOS_PC_DISK_BIOS_MAXIMUM_FLOPPY_DRIVE_COUNT        = 4u
};

static void hyperdos_pc_disk_bios_set_carry_flag(hyperdos_x86_processor* processor, int carry)
{
    hyperdos_x86_set_flag(processor, HYPERDOS_X86_FLAG_CARRY, carry);
}

static void hyperdos_pc_disk_bios_trace(const hyperdos_pc_disk_bios_interface* diskBiosInterface,
                                        const char*                            format,
                                        ...)
{
    char    message[HYPERDOS_PC_DISK_BIOS_TRACE_TEXT_CAPACITY];
    va_list arguments;

    if (diskBiosInterface == NULL || diskBiosInterface->traceFunction == NULL)
    {
        return;
    }
    va_start(arguments, format);
    vsnprintf(message, sizeof(message), format, arguments);
    va_end(arguments);
    diskBiosInterface->traceFunction(diskBiosInterface->userContext, message);
}

static void hyperdos_pc_disk_bios_lock_disk_images(const hyperdos_pc_disk_bios_interface* diskBiosInterface)
{
    if (diskBiosInterface != NULL && diskBiosInterface->lockDiskImages != NULL)
    {
        diskBiosInterface->lockDiskImages(diskBiosInterface->userContext);
    }
}

static void hyperdos_pc_disk_bios_unlock_disk_images(const hyperdos_pc_disk_bios_interface* diskBiosInterface)
{
    if (diskBiosInterface != NULL && diskBiosInterface->unlockDiskImages != NULL)
    {
        diskBiosInterface->unlockDiskImages(diskBiosInterface->userContext);
    }
}

static int hyperdos_pc_disk_bios_floppy_disk_change_line_is_supported(
        const hyperdos_pc_disk_bios_interface* diskBiosInterface)
{
    return diskBiosInterface != NULL && diskBiosInterface->floppyDiskChangeLineSupported != 0u;
}

static uint8_t hyperdos_pc_disk_bios_get_floppy_drive_count(const hyperdos_pc_disk_bios_interface* diskBiosInterface)
{
    uint8_t floppyDriveCount = HYPERDOS_PC_DISK_BIOS_DEFAULT_FLOPPY_DRIVE_COUNT;

    if (diskBiosInterface != NULL && diskBiosInterface->floppyDriveCount != 0u)
    {
        floppyDriveCount = diskBiosInterface->floppyDriveCount;
    }
    if (floppyDriveCount > HYPERDOS_PC_DISK_BIOS_MAXIMUM_FLOPPY_DRIVE_COUNT)
    {
        floppyDriveCount = HYPERDOS_PC_DISK_BIOS_MAXIMUM_FLOPPY_DRIVE_COUNT;
    }
    return floppyDriveCount;
}

static int hyperdos_pc_disk_bios_floppy_drive_is_installed(const hyperdos_pc_disk_bios_interface* diskBiosInterface,
                                                           uint8_t                                driveNumber)
{
    return driveNumber < HYPERDOS_PC_DISK_BIOS_HARD_DISK_DRIVE_NUMBER &&
           driveNumber < hyperdos_pc_disk_bios_get_floppy_drive_count(diskBiosInterface);
}

static hyperdos_pc_disk_image* hyperdos_pc_disk_bios_get_disk_image(
        const hyperdos_pc_disk_bios_interface* diskBiosInterface,
        uint8_t                                driveNumber);

static uint8_t hyperdos_pc_disk_bios_get_fixed_disk_drive_count(
        const hyperdos_pc_disk_bios_interface* diskBiosInterface)
{
    uint16_t fixedDiskIndex      = 0u;
    uint8_t  fixedDiskDriveCount = 0u;

    if (diskBiosInterface == NULL)
    {
        return 0u;
    }
    for (fixedDiskIndex = 0u; fixedDiskIndex < diskBiosInterface->fixedDiskDriveCount && fixedDiskIndex < 0x80u;
         ++fixedDiskIndex)
    {
        hyperdos_pc_disk_image* diskImage = hyperdos_pc_disk_bios_get_disk_image(
                diskBiosInterface,
                (uint8_t)(HYPERDOS_PC_DISK_BIOS_HARD_DISK_DRIVE_NUMBER + fixedDiskIndex));

        if (diskImage != NULL && diskImage->isHardDisk)
        {
            fixedDiskDriveCount = (uint8_t)(fixedDiskIndex + 1u);
        }
    }
    return fixedDiskDriveCount;
}

static hyperdos_pc_disk_image* hyperdos_pc_disk_bios_get_disk_image(
        const hyperdos_pc_disk_bios_interface* diskBiosInterface,
        uint8_t                                driveNumber)
{
    if (diskBiosInterface == NULL || diskBiosInterface->getDiskImage == NULL)
    {
        return NULL;
    }
    return diskBiosInterface->getDiskImage(diskBiosInterface->userContext, driveNumber);
}

static uint8_t hyperdos_pc_disk_bios_get_established_diskette_media_state(const hyperdos_pc_disk_image* diskImage)
{
    if (diskImage == NULL)
    {
        return HYPERDOS_PC_DISK_BIOS_DISKETTE_MEDIA_STATE_UNKNOWN;
    }
    if (diskImage->headCount == 2u && diskImage->sectorsPerTrack == 9u && diskImage->cylinderCount <= 40u)
    {
        return HYPERDOS_PC_DISK_BIOS_DISKETTE_MEDIA_STATE_360K_ESTABLISHED;
    }
    if (diskImage->headCount == 2u && diskImage->sectorsPerTrack == 15u && diskImage->cylinderCount >= 80u)
    {
        return HYPERDOS_PC_DISK_BIOS_DISKETTE_MEDIA_STATE_12M_ESTABLISHED;
    }
    return HYPERDOS_PC_DISK_BIOS_DISKETTE_MEDIA_STATE_UNKNOWN;
}

static uint8_t hyperdos_pc_disk_bios_get_changed_diskette_media_state(const hyperdos_pc_disk_image* diskImage)
{
    if (diskImage != NULL && diskImage->headCount == 2u && diskImage->sectorsPerTrack == 15u &&
        diskImage->cylinderCount >= 80u)
    {
        return HYPERDOS_PC_DISK_BIOS_DISKETTE_MEDIA_STATE_12M_NOT_ESTABLISHED;
    }
    return HYPERDOS_PC_DISK_BIOS_DISKETTE_MEDIA_STATE_UNKNOWN;
}

static uint8_t hyperdos_pc_disk_bios_get_diskette_configuration_state(const hyperdos_pc_disk_image* diskImage)
{
    if (diskImage != NULL && diskImage->headCount == 2u && diskImage->sectorsPerTrack == 9u &&
        diskImage->cylinderCount <= 40u)
    {
        return HYPERDOS_PC_DISK_BIOS_DISKETTE_DATA_RATE_250K;
    }
    return HYPERDOS_PC_DISK_BIOS_DISKETTE_DATA_RATE_500K;
}

void hyperdos_pc_disk_bios_set_disk_operation_status(hyperdos_pc* pc, uint8_t driveNumber, uint8_t status)
{
    if (pc == NULL)
    {
        return;
    }
    hyperdos_pc_bios_data_area_write_byte(pc,
                                          driveNumber >= HYPERDOS_PC_DISK_BIOS_HARD_DISK_DRIVE_NUMBER
                                                  ? HYPERDOS_PC_DISK_BIOS_DATA_AREA_HARD_DISK_STATUS_OFFSET
                                                  : HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_STATUS_OFFSET,
                                          status);
}

uint8_t hyperdos_pc_disk_bios_get_disk_operation_status(hyperdos_pc* pc, uint8_t driveNumber)
{
    if (pc == NULL)
    {
        return HYPERDOS_PC_DISK_BIOS_STATUS_INVALID_FUNCTION;
    }
    return hyperdos_pc_bios_data_area_read_byte(pc,
                                                driveNumber >= HYPERDOS_PC_DISK_BIOS_HARD_DISK_DRIVE_NUMBER
                                                        ? HYPERDOS_PC_DISK_BIOS_DATA_AREA_HARD_DISK_STATUS_OFFSET
                                                        : HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_STATUS_OFFSET);
}

void hyperdos_pc_disk_bios_set_fixed_disk_count(hyperdos_pc* pc, uint8_t fixedDiskCount)
{
    if (pc == NULL)
    {
        return;
    }
    hyperdos_pc_bios_data_area_write_byte(pc, HYPERDOS_PC_DISK_BIOS_DATA_AREA_FIXED_DISK_COUNT_OFFSET, fixedDiskCount);
}

static uint8_t hyperdos_pc_disk_bios_get_floppy_drive_recalibration_bit(uint8_t driveNumber)
{
    if (driveNumber >= HYPERDOS_PC_DISK_BIOS_MAXIMUM_FLOPPY_DRIVE_COUNT)
    {
        return 0u;
    }
    return (uint8_t)(HYPERDOS_PC_DISK_BIOS_DISKETTE_RECALIBRATE_DRIVE_ZERO << driveNumber);
}

void hyperdos_pc_disk_bios_mark_floppy_drive_media_state_changed(hyperdos_pc*                  pc,
                                                                 uint8_t                       driveNumber,
                                                                 const hyperdos_pc_disk_image* diskImage)
{
    uint8_t recalibrationStatus = 0u;

    if (pc == NULL)
    {
        return;
    }
    recalibrationStatus =
            hyperdos_pc_bios_data_area_read_byte(pc,
                                                 HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_RECALIBRATION_STATUS_OFFSET);

    hyperdos_pc_bios_data_area_write_byte(pc,
                                          HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_STATUS_OFFSET,
                                          HYPERDOS_PC_DISK_BIOS_STATUS_CHANGED);
    hyperdos_pc_bios_data_area_write_byte(pc,
                                          HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_RECALIBRATION_STATUS_OFFSET,
                                          (uint8_t)(recalibrationStatus |
                                                    hyperdos_pc_disk_bios_get_floppy_drive_recalibration_bit(
                                                            driveNumber)));
    hyperdos_pc_bios_data_area_write_byte(pc,
                                          HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_CONFIGURATION_OFFSET,
                                          hyperdos_pc_disk_bios_get_diskette_configuration_state(diskImage));
    hyperdos_pc_bios_data_area_write_byte(pc,
                                          (uint16_t)(HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_MEDIA_STATE_OFFSET +
                                                     driveNumber),
                                          hyperdos_pc_disk_bios_get_changed_diskette_media_state(diskImage));
    hyperdos_pc_bios_data_area_write_byte(pc,
                                          (uint16_t)(HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_CURRENT_CYLINDER_OFFSET +
                                                     driveNumber),
                                          HYPERDOS_PC_DISK_BIOS_DISKETTE_CURRENT_CYLINDER_UNKNOWN);
}

void hyperdos_pc_disk_bios_mark_floppy_media_state_changed(hyperdos_pc* pc, const hyperdos_pc_disk_image* diskImage)
{
    hyperdos_pc_disk_bios_mark_floppy_drive_media_state_changed(pc,
                                                                diskImage != NULL
                                                                        ? diskImage->driveNumber
                                                                        : HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER,
                                                                diskImage);
}

void hyperdos_pc_disk_bios_mark_floppy_drive_media_state_established(hyperdos_pc*                  pc,
                                                                     uint8_t                       driveNumber,
                                                                     const hyperdos_pc_disk_image* diskImage,
                                                                     uint8_t                       currentCylinder)
{
    uint8_t recalibrationStatus = 0u;

    if (pc == NULL)
    {
        return;
    }
    recalibrationStatus =
            hyperdos_pc_bios_data_area_read_byte(pc,
                                                 HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_RECALIBRATION_STATUS_OFFSET);

    hyperdos_pc_bios_data_area_write_byte(pc,
                                          HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_STATUS_OFFSET,
                                          HYPERDOS_PC_DISK_BIOS_STATUS_SUCCESS);
    hyperdos_pc_bios_data_area_write_byte(pc,
                                          HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_RECALIBRATION_STATUS_OFFSET,
                                          (uint8_t)(recalibrationStatus &
                                                    (uint8_t)~hyperdos_pc_disk_bios_get_floppy_drive_recalibration_bit(
                                                            driveNumber)));
    hyperdos_pc_bios_data_area_write_byte(pc,
                                          HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_CONFIGURATION_OFFSET,
                                          hyperdos_pc_disk_bios_get_diskette_configuration_state(diskImage));
    hyperdos_pc_bios_data_area_write_byte(pc,
                                          (uint16_t)(HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_MEDIA_STATE_OFFSET +
                                                     driveNumber),
                                          hyperdos_pc_disk_bios_get_established_diskette_media_state(diskImage));
    hyperdos_pc_bios_data_area_write_byte(pc,
                                          (uint16_t)(HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_CURRENT_CYLINDER_OFFSET +
                                                     driveNumber),
                                          currentCylinder);
}

void hyperdos_pc_disk_bios_mark_floppy_media_state_established(hyperdos_pc*                  pc,
                                                               const hyperdos_pc_disk_image* diskImage,
                                                               uint8_t                       currentCylinder)
{
    hyperdos_pc_disk_bios_mark_floppy_drive_media_state_established(pc,
                                                                    diskImage != NULL
                                                                            ? diskImage->driveNumber
                                                                            : HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER,
                                                                    diskImage,
                                                                    currentCylinder);
}

void hyperdos_pc_disk_bios_initialize_data_area(hyperdos_pc*                  pc,
                                                const hyperdos_pc_disk_image* activeFloppyDisk,
                                                uint8_t                       fixedDiskCount)
{
    size_t  controllerStatusIndex = 0;
    uint8_t driveNumber           = 0u;

    if (pc == NULL)
    {
        return;
    }
    hyperdos_pc_disk_bios_set_fixed_disk_count(pc, fixedDiskCount);
    hyperdos_pc_bios_data_area_write_byte(pc,
                                          HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_RECALIBRATION_STATUS_OFFSET,
                                          activeFloppyDisk != NULL
                                                  ? HYPERDOS_PC_DISK_BIOS_DISKETTE_RECALIBRATE_DRIVE_ZERO
                                                  : 0u);
    hyperdos_pc_bios_data_area_write_byte(pc, HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_MOTOR_STATUS_OFFSET, 0u);
    hyperdos_pc_bios_data_area_write_byte(pc, HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_MOTOR_TIMEOUT_OFFSET, 0u);
    hyperdos_pc_bios_data_area_write_byte(pc,
                                          HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_STATUS_OFFSET,
                                          HYPERDOS_PC_DISK_BIOS_STATUS_SUCCESS);
    for (controllerStatusIndex = 0; controllerStatusIndex < 7u; ++controllerStatusIndex)
    {
        hyperdos_pc_bios_data_area_write_byte(
                pc,
                (uint16_t)(HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_CONTROLLER_STATUS_OFFSET + controllerStatusIndex),
                0u);
    }
    hyperdos_pc_bios_data_area_write_byte(pc,
                                          HYPERDOS_PC_DISK_BIOS_DATA_AREA_HARD_DISK_STATUS_OFFSET,
                                          HYPERDOS_PC_DISK_BIOS_STATUS_SUCCESS);
    hyperdos_pc_bios_data_area_write_byte(pc,
                                          HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_CONFIGURATION_OFFSET,
                                          hyperdos_pc_disk_bios_get_diskette_configuration_state(activeFloppyDisk));
    for (driveNumber = 0u; driveNumber < HYPERDOS_PC_DISK_BIOS_MAXIMUM_FLOPPY_DRIVE_COUNT; ++driveNumber)
    {
        hyperdos_pc_bios_data_area_write_byte(pc,
                                              (uint16_t)(HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_MEDIA_STATE_OFFSET +
                                                         driveNumber),
                                              driveNumber == HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER
                                                      ? hyperdos_pc_disk_bios_get_established_diskette_media_state(
                                                                activeFloppyDisk)
                                                      : HYPERDOS_PC_DISK_BIOS_DISKETTE_MEDIA_STATE_UNKNOWN);
        hyperdos_pc_bios_data_area_write_byte(
                pc,
                (uint16_t)(HYPERDOS_PC_DISK_BIOS_DATA_AREA_DISKETTE_CURRENT_CYLINDER_OFFSET + driveNumber),
                HYPERDOS_PC_DISK_BIOS_DISKETTE_CURRENT_CYLINDER_UNKNOWN);
    }
}

static void hyperdos_pc_disk_bios_set_status_in_accumulator(hyperdos_x86_processor* processor, uint8_t status)
{
    uint16_t accumulator = hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)(((uint16_t)status
                                                       << HYPERDOS_PC_DISK_BIOS_SERVICE_REGISTER_SHIFT) |
                                                      (accumulator & HYPERDOS_X86_LOW_BYTE_MASK)));
}

static void hyperdos_pc_disk_bios_set_drive_parameters(hyperdos_x86_processor* processor,
                                                       uint16_t                accumulator,
                                                       uint16_t                cylinderCount,
                                                       uint16_t                headCount,
                                                       uint16_t                sectorsPerTrack,
                                                       uint8_t                 driveCount)
{
    if (cylinderCount == 0u)
    {
        cylinderCount = 1u;
    }
    if (headCount == 0u)
    {
        headCount = 1u;
    }
    if (sectorsPerTrack == 0u)
    {
        sectorsPerTrack = 1u;
    }
    if (cylinderCount > 1024u)
    {
        cylinderCount = 1024u;
    }
    if (headCount > 256u)
    {
        headCount = 256u;
    }
    if (sectorsPerTrack > 63u)
    {
        sectorsPerTrack = 63u;
    }
    uint16_t maximumCylinder = (uint16_t)(cylinderCount - 1u);
    uint16_t maximumHead     = (uint16_t)(headCount - 1u);

    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)(accumulator & HYPERDOS_X86_LOW_BYTE_MASK));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
                                           (uint16_t)(((maximumCylinder & 0x00FFu)
                                                       << HYPERDOS_PC_DISK_BIOS_SERVICE_REGISTER_SHIFT) |
                                                      ((maximumCylinder >> 2u) & 0x00C0u) |
                                                      (sectorsPerTrack & 0x003Fu)));
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                           (uint16_t)((maximumHead << HYPERDOS_PC_DISK_BIOS_SERVICE_REGISTER_SHIFT) |
                                                      driveCount));
}

static hyperdos_x86_execution_result hyperdos_pc_disk_bios_fail_transfer(hyperdos_x86_processor* processor,
                                                                         hyperdos_pc*            pc,
                                                                         uint8_t                 driveNumber,
                                                                         uint8_t                 status)
{
    hyperdos_pc_disk_bios_set_disk_operation_status(pc, driveNumber, status);
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)(status << HYPERDOS_PC_DISK_BIOS_SERVICE_REGISTER_SHIFT));
    hyperdos_pc_disk_bios_set_carry_flag(processor, 1);
    return HYPERDOS_X86_EXECUTION_OK;
}

static uint8_t hyperdos_pc_disk_bios_get_transfer_status(hyperdos_pc_disk_transfer_result transferResult)
{
    if (transferResult == HYPERDOS_PC_DISK_TRANSFER_WRITE_PROTECTED)
    {
        return HYPERDOS_PC_DISK_BIOS_STATUS_WRITE_PROTECTED;
    }
    return HYPERDOS_PC_DISK_BIOS_STATUS_SECTOR_NOT_FOUND;
}

static hyperdos_x86_execution_result hyperdos_pc_disk_bios_handle_transfer(
        hyperdos_x86_processor*                processor,
        const hyperdos_pc_disk_bios_interface* diskBiosInterface,
        int                                    isWrite)
{
    hyperdos_pc* pc      = diskBiosInterface->pc;
    uint16_t accumulator = hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
    uint16_t counter     = hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER);
    uint16_t data        = hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA);
    uint8_t  sectorCount = (uint8_t)(accumulator & HYPERDOS_X86_LOW_BYTE_MASK);
    uint16_t cylinder    = (uint16_t)((counter >> HYPERDOS_PC_DISK_BIOS_SERVICE_REGISTER_SHIFT) |
                                   ((counter & 0x00C0u) << 2u));
    uint16_t sector      = (uint16_t)(counter & 0x003Fu);
    uint16_t head        = (uint16_t)(data >> HYPERDOS_PC_DISK_BIOS_SERVICE_REGISTER_SHIFT);
    uint8_t  driveNumber = (uint8_t)(data & HYPERDOS_X86_LOW_BYTE_MASK);
    uint16_t transferOffset  = hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE);
    uint32_t transferAddress = 0u;
    hyperdos_pc_disk_image*          diskImage           = NULL;
    uint64_t                         logicalBlockAddress = 0u;
    size_t                           transferByteCount   = 0u;
    size_t                           byteIndex           = 0;
    uint8_t*                         transferBytes       = NULL;
    hyperdos_pc_disk_transfer_result transferResult      = HYPERDOS_PC_DISK_TRANSFER_OK;

    if (hyperdos_x86_translate_logical_to_physical_address(processor,
                                                           HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                                           transferOffset,
                                                           &transferAddress) != HYPERDOS_X86_EXECUTION_OK)
    {
        hyperdos_pc_disk_bios_set_status_in_accumulator(processor, HYPERDOS_PC_DISK_BIOS_STATUS_INVALID_FUNCTION);
        hyperdos_pc_disk_bios_set_carry_flag(processor, 1);
        return HYPERDOS_X86_EXECUTION_OK;
    }

    hyperdos_pc_disk_bios_lock_disk_images(diskBiosInterface);
    diskImage = hyperdos_pc_disk_bios_get_disk_image(diskBiosInterface, driveNumber);
    if (diskImage == NULL)
    {
        hyperdos_pc_disk_bios_trace(diskBiosInterface,
                                    "int13 transfer invalid write=%d drive=%02X cylinder=%u head=%u sector=%u "
                                    "count=%u",
                                    isWrite,
                                    driveNumber,
                                    cylinder,
                                    head,
                                    sector,
                                    sectorCount);
        hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);
        return hyperdos_pc_disk_bios_fail_transfer(processor,
                                                   pc,
                                                   driveNumber,
                                                   HYPERDOS_PC_DISK_BIOS_STATUS_SECTOR_NOT_FOUND);
    }
    if (driveNumber < HYPERDOS_PC_DISK_BIOS_HARD_DISK_DRIVE_NUMBER && !diskImage->isHardDisk &&
        (diskImage->mediaChanged ||
         (diskBiosInterface->floppyController != NULL &&
          hyperdos_pc_floppy_controller_drive_disk_changed(diskBiosInterface->floppyController, driveNumber))))
    {
        hyperdos_pc_disk_bios_trace(
                diskBiosInterface,
                "int13 transfer reports disk-changed write=%d drive=%02X cylinder=%u head=%u sector=%u count=%u "
                "path=\"%s\" media-changed=%u reported=%u controller-changed=%u",
                isWrite,
                driveNumber,
                cylinder,
                head,
                sector,
                sectorCount,
                diskImage->path,
                diskImage->mediaChanged,
                diskImage->mediaChangeReported,
                diskBiosInterface->floppyController != NULL
                        ? hyperdos_pc_floppy_controller_drive_disk_changed(diskBiosInterface->floppyController,
                                                                           driveNumber)
                        : 0u);
        hyperdos_pc_disk_bios_mark_floppy_drive_media_state_changed(pc, driveNumber, diskImage);
        diskImage->mediaChangeReported = 1u;
        diskImage->mediaChanged        = 0u;
        if (diskBiosInterface->floppyController != NULL)
        {
            hyperdos_pc_floppy_controller_acknowledge_drive_disk_change(diskBiosInterface->floppyController,
                                                                        driveNumber);
        }
        hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                               (uint16_t)(HYPERDOS_PC_DISK_BIOS_STATUS_CHANGED
                                                          << HYPERDOS_PC_DISK_BIOS_SERVICE_REGISTER_SHIFT));
        hyperdos_pc_disk_bios_set_carry_flag(processor, 1);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (sectorCount == 0u || sector == 0u || sector > diskImage->sectorsPerTrack || head >= diskImage->headCount)
    {
        hyperdos_pc_disk_bios_trace(diskBiosInterface,
                                    "int13 transfer invalid write=%d drive=%02X cylinder=%u head=%u sector=%u "
                                    "count=%u",
                                    isWrite,
                                    driveNumber,
                                    cylinder,
                                    head,
                                    sector,
                                    sectorCount);
        hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);
        return hyperdos_pc_disk_bios_fail_transfer(processor,
                                                   pc,
                                                   driveNumber,
                                                   HYPERDOS_PC_DISK_BIOS_STATUS_SECTOR_NOT_FOUND);
    }

    logicalBlockAddress = ((uint64_t)cylinder * diskImage->headCount + head) * diskImage->sectorsPerTrack +
                          (sector - 1u);
    transferByteCount = (size_t)sectorCount * diskImage->bytesPerSector;
    if ((uint64_t)sectorCount > diskImage->sectorCount || logicalBlockAddress > diskImage->sectorCount - sectorCount)
    {
        hyperdos_pc_disk_bios_trace(diskBiosInterface,
                                    "int13 transfer out-of-range write=%d drive=%02X lba=%llu sectors=%u "
                                    "image-sectors=%llu",
                                    isWrite,
                                    driveNumber,
                                    (unsigned long long)logicalBlockAddress,
                                    sectorCount,
                                    (unsigned long long)diskImage->sectorCount);
        hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);
        return hyperdos_pc_disk_bios_fail_transfer(processor,
                                                   pc,
                                                   driveNumber,
                                                   HYPERDOS_PC_DISK_BIOS_STATUS_SECTOR_NOT_FOUND);
    }
    if (isWrite && diskImage->readOnly)
    {
        hyperdos_pc_disk_bios_trace(diskBiosInterface,
                                    "int13 transfer write-protected drive=%02X cylinder=%u head=%u sector=%u count=%u "
                                    "path=\"%s\"",
                                    driveNumber,
                                    cylinder,
                                    head,
                                    sector,
                                    sectorCount,
                                    diskImage->path);
        hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);
        return hyperdos_pc_disk_bios_fail_transfer(processor,
                                                   pc,
                                                   driveNumber,
                                                   HYPERDOS_PC_DISK_BIOS_STATUS_WRITE_PROTECTED);
    }
    transferBytes = (uint8_t*)malloc(transferByteCount);
    if (transferBytes == NULL)
    {
        hyperdos_pc_disk_bios_trace(diskBiosInterface,
                                    "int13 transfer allocation-failed write=%d drive=%02X bytes=%zu",
                                    isWrite,
                                    driveNumber,
                                    transferByteCount);
        hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);
        return hyperdos_pc_disk_bios_fail_transfer(processor,
                                                   pc,
                                                   driveNumber,
                                                   HYPERDOS_PC_DISK_BIOS_STATUS_SECTOR_NOT_FOUND);
    }
    if (isWrite)
    {
        for (byteIndex = 0; byteIndex < transferByteCount; ++byteIndex)
        {
            transferBytes[byteIndex] = hyperdos_bus_read_memory_byte_or_open_bus(&pc->bus,
                                                                                 transferAddress + (uint32_t)byteIndex);
        }
        transferResult = hyperdos_pc_disk_image_write_sectors(diskImage,
                                                              logicalBlockAddress,
                                                              sectorCount,
                                                              transferBytes);
    }
    else
    {
        transferResult = hyperdos_pc_disk_image_read_sectors(diskImage,
                                                             logicalBlockAddress,
                                                             sectorCount,
                                                             transferBytes);
        if (transferResult == HYPERDOS_PC_DISK_TRANSFER_OK)
        {
            for (byteIndex = 0; byteIndex < transferByteCount; ++byteIndex)
            {
                hyperdos_bus_write_memory_byte_if_mapped(&pc->bus,
                                                         transferAddress + (uint32_t)byteIndex,
                                                         transferBytes[byteIndex]);
            }
        }
    }
    free(transferBytes);
    if (transferResult != HYPERDOS_PC_DISK_TRANSFER_OK)
    {
        uint8_t status = hyperdos_pc_disk_bios_get_transfer_status(transferResult);

        hyperdos_pc_disk_bios_trace(diskBiosInterface,
                                    "int13 transfer failed write=%d drive=%02X lba=%llu count=%u status=%02X "
                                    "path=\"%s\"",
                                    isWrite,
                                    driveNumber,
                                    (unsigned long long)logicalBlockAddress,
                                    sectorCount,
                                    status,
                                    diskImage->path);
        hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);
        return hyperdos_pc_disk_bios_fail_transfer(processor, pc, driveNumber, status);
    }
    if (driveNumber < HYPERDOS_PC_DISK_BIOS_HARD_DISK_DRIVE_NUMBER && !diskImage->isHardDisk)
    {
        diskImage->mediaChanged = 0u;
        hyperdos_pc_disk_bios_mark_floppy_drive_media_state_established(pc, driveNumber, diskImage, (uint8_t)cylinder);
        hyperdos_pc_disk_bios_trace(
                diskBiosInterface,
                "int13 transfer success write=%d drive=%02X cylinder=%u head=%u sector=%u "
                "count=%u lba=%llu path=\"%s\" media-changed=%u reported=%u controller-changed=%u",
                isWrite,
                driveNumber,
                cylinder,
                head,
                sector,
                sectorCount,
                (unsigned long long)logicalBlockAddress,
                diskImage->path,
                diskImage->mediaChanged,
                diskImage->mediaChangeReported,
                diskBiosInterface->floppyController != NULL
                        ? hyperdos_pc_floppy_controller_drive_disk_changed(diskBiosInterface->floppyController,
                                                                           driveNumber)
                        : 0u);
    }
    else
    {
        hyperdos_pc_disk_bios_set_disk_operation_status(pc, driveNumber, HYPERDOS_PC_DISK_BIOS_STATUS_SUCCESS);
        hyperdos_pc_disk_bios_trace(diskBiosInterface,
                                    "int13 transfer success write=%d drive=%02X cylinder=%u head=%u sector=%u "
                                    "count=%u lba=%llu path=\"%s\"",
                                    isWrite,
                                    driveNumber,
                                    cylinder,
                                    head,
                                    sector,
                                    sectorCount,
                                    (unsigned long long)logicalBlockAddress,
                                    diskImage->path);
    }
    hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);

    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, sectorCount);
    hyperdos_pc_disk_bios_set_carry_flag(processor, 0);
    return HYPERDOS_X86_EXECUTION_OK;
}

static void hyperdos_pc_disk_bios_return_invalid_function(hyperdos_x86_processor* processor,
                                                          hyperdos_pc*            pc,
                                                          uint8_t                 driveNumber,
                                                          uint16_t                accumulator)
{
    hyperdos_pc_disk_bios_set_disk_operation_status(pc, driveNumber, HYPERDOS_PC_DISK_BIOS_STATUS_INVALID_FUNCTION);
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           (uint16_t)((HYPERDOS_PC_DISK_BIOS_STATUS_INVALID_FUNCTION
                                                       << HYPERDOS_PC_DISK_BIOS_SERVICE_REGISTER_SHIFT) |
                                                      (accumulator & HYPERDOS_X86_LOW_BYTE_MASK)));
    hyperdos_pc_disk_bios_set_carry_flag(processor, 1);
}

hyperdos_x86_execution_result hyperdos_pc_disk_bios_handle_interrupt(
        hyperdos_x86_processor*                processor,
        const hyperdos_pc_disk_bios_interface* diskBiosInterface)
{
    hyperdos_pc* pc            = NULL;
    uint16_t     accumulator   = 0u;
    uint16_t     data          = 0u;
    uint8_t      serviceNumber = 0u;
    uint8_t      driveNumber   = 0u;

    if (processor == NULL || diskBiosInterface == NULL || diskBiosInterface->pc == NULL)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }
    pc            = diskBiosInterface->pc;
    accumulator   = hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
    data          = hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA);
    serviceNumber = (uint8_t)(accumulator >> HYPERDOS_PC_DISK_BIOS_SERVICE_REGISTER_SHIFT);
    driveNumber   = (uint8_t)(data & HYPERDOS_X86_LOW_BYTE_MASK);

    if (serviceNumber == HYPERDOS_PC_DISK_BIOS_RESET_SERVICE)
    {
        hyperdos_pc_disk_image* diskImage = NULL;

        hyperdos_pc_disk_bios_lock_disk_images(diskBiosInterface);
        diskImage = hyperdos_pc_disk_bios_get_disk_image(diskBiosInterface, driveNumber);
        hyperdos_pc_disk_bios_trace(
                diskBiosInterface,
                "int13 reset drive=%02X media-changed=%u reported=%u controller-changed=%u",
                driveNumber,
                diskImage != NULL ? diskImage->mediaChanged : 0u,
                diskImage != NULL ? diskImage->mediaChangeReported : 0u,
                diskBiosInterface->floppyController != NULL
                        ? hyperdos_pc_floppy_controller_drive_disk_changed(diskBiosInterface->floppyController,
                                                                           driveNumber)
                        : 0u);
        hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);
        hyperdos_pc_disk_bios_set_disk_operation_status(pc, driveNumber, HYPERDOS_PC_DISK_BIOS_STATUS_SUCCESS);
        hyperdos_x86_set_general_register_word(
                processor,
                HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                (uint16_t)(hyperdos_x86_get_general_register_word(processor,
                                                                  HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) &
                           HYPERDOS_X86_LOW_BYTE_MASK));
        hyperdos_pc_disk_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_DISK_BIOS_STATUS_SERVICE)
    {
        uint8_t status = hyperdos_pc_disk_bios_get_disk_operation_status(pc, driveNumber);

        hyperdos_pc_disk_bios_trace(diskBiosInterface, "int13 status drive=%02X status=%02X", driveNumber, status);
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                               (uint16_t)(((uint16_t)status
                                                           << HYPERDOS_PC_DISK_BIOS_SERVICE_REGISTER_SHIFT) |
                                                          status));
        hyperdos_pc_disk_bios_set_carry_flag(processor, status != HYPERDOS_PC_DISK_BIOS_STATUS_SUCCESS);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_DISK_BIOS_READ_SECTORS_SERVICE)
    {
        return hyperdos_pc_disk_bios_handle_transfer(processor, diskBiosInterface, 0);
    }
    if (serviceNumber == HYPERDOS_PC_DISK_BIOS_WRITE_SECTORS_SERVICE)
    {
        return hyperdos_pc_disk_bios_handle_transfer(processor, diskBiosInterface, 1);
    }
    if (serviceNumber == HYPERDOS_PC_DISK_BIOS_DRIVE_PARAMETERS_SERVICE)
    {
        hyperdos_pc_disk_image* diskImage = NULL;

        hyperdos_pc_disk_bios_lock_disk_images(diskBiosInterface);
        diskImage = hyperdos_pc_disk_bios_get_disk_image(diskBiosInterface, driveNumber);
        if (diskImage == NULL)
        {
            if (hyperdos_pc_disk_bios_floppy_drive_is_installed(diskBiosInterface, driveNumber))
            {
                uint8_t floppyDriveCount = hyperdos_pc_disk_bios_get_floppy_drive_count(diskBiosInterface);

                hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);
                hyperdos_pc_disk_bios_set_drive_parameters(processor,
                                                           accumulator,
                                                           HYPERDOS_PC_DISK_BIOS_DEFAULT_FLOPPY_CYLINDER_COUNT,
                                                           HYPERDOS_PC_DISK_BIOS_DEFAULT_FLOPPY_HEAD_COUNT,
                                                           HYPERDOS_PC_DISK_BIOS_DEFAULT_FLOPPY_SECTORS_PER_TRACK,
                                                           floppyDriveCount);
                hyperdos_pc_disk_bios_set_disk_operation_status(pc, driveNumber, HYPERDOS_PC_DISK_BIOS_STATUS_SUCCESS);
                hyperdos_pc_disk_bios_set_carry_flag(processor, 0);
                return HYPERDOS_X86_EXECUTION_OK;
            }
            hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);
            hyperdos_pc_disk_bios_return_invalid_function(processor, pc, driveNumber, accumulator);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        hyperdos_pc_disk_bios_set_drive_parameters(processor,
                                                   accumulator,
                                                   (uint16_t)diskImage->cylinderCount,
                                                   (uint16_t)diskImage->headCount,
                                                   (uint16_t)diskImage->sectorsPerTrack,
                                                   diskImage->isHardDisk
                                                           ? hyperdos_pc_disk_bios_get_fixed_disk_drive_count(
                                                                     diskBiosInterface)
                                                           : hyperdos_pc_disk_bios_get_floppy_drive_count(
                                                                     diskBiosInterface));
        hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);
        hyperdos_pc_disk_bios_set_disk_operation_status(pc, driveNumber, HYPERDOS_PC_DISK_BIOS_STATUS_SUCCESS);
        hyperdos_pc_disk_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_DISK_BIOS_GET_TYPE_SERVICE)
    {
        hyperdos_pc_disk_image* diskImage = NULL;

        hyperdos_pc_disk_bios_lock_disk_images(diskBiosInterface);
        diskImage = hyperdos_pc_disk_bios_get_disk_image(diskBiosInterface, driveNumber);
        if (diskImage == NULL)
        {
            if (hyperdos_pc_disk_bios_floppy_drive_is_installed(diskBiosInterface, driveNumber))
            {
                uint8_t floppyType = hyperdos_pc_disk_bios_floppy_disk_change_line_is_supported(diskBiosInterface)
                                             ? HYPERDOS_PC_DISK_BIOS_TYPE_FLOPPY_WITH_CHANGE_LINE
                                             : HYPERDOS_PC_DISK_BIOS_TYPE_FLOPPY_WITHOUT_CHANGE_LINE;

                hyperdos_pc_disk_bios_trace(diskBiosInterface,
                                            "int13 type drive=%02X floppy type=%u no-media",
                                            driveNumber,
                                            floppyType);
                hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);
                hyperdos_x86_set_general_register_word(processor,
                                                       HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                       (uint16_t)((floppyType
                                                                   << HYPERDOS_PC_DISK_BIOS_SERVICE_REGISTER_SHIFT) |
                                                                  (accumulator & HYPERDOS_X86_LOW_BYTE_MASK)));
                hyperdos_pc_disk_bios_set_disk_operation_status(pc, driveNumber, HYPERDOS_PC_DISK_BIOS_STATUS_SUCCESS);
                hyperdos_pc_disk_bios_set_carry_flag(processor, 0);
                return HYPERDOS_X86_EXECUTION_OK;
            }
            hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);
            hyperdos_pc_disk_bios_return_invalid_function(processor, pc, driveNumber, accumulator);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        if (diskImage->isHardDisk)
        {
            uint32_t totalSectorCount = diskImage->sectorCount > 0xFFFFFFFFu ? 0xFFFFFFFFu
                                                                             : (uint32_t)diskImage->sectorCount;
            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                   (uint16_t)((HYPERDOS_PC_DISK_BIOS_TYPE_FIXED
                                                               << HYPERDOS_PC_DISK_BIOS_SERVICE_REGISTER_SHIFT) |
                                                              (accumulator & HYPERDOS_X86_LOW_BYTE_MASK)));
            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
                                                   (uint16_t)(totalSectorCount >> HYPERDOS_X86_WORD_BIT_COUNT));
            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                                   (uint16_t)totalSectorCount);
        }
        else
        {
            uint8_t floppyType = hyperdos_pc_disk_bios_floppy_disk_change_line_is_supported(diskBiosInterface)
                                         ? HYPERDOS_PC_DISK_BIOS_TYPE_FLOPPY_WITH_CHANGE_LINE
                                         : HYPERDOS_PC_DISK_BIOS_TYPE_FLOPPY_WITHOUT_CHANGE_LINE;
            hyperdos_pc_disk_bios_trace(diskBiosInterface,
                                        "int13 type drive=%02X floppy type=%u path=\"%s\"",
                                        driveNumber,
                                        floppyType,
                                        diskImage->path);
            hyperdos_x86_set_general_register_word(processor,
                                                   HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                   (uint16_t)((floppyType
                                                               << HYPERDOS_PC_DISK_BIOS_SERVICE_REGISTER_SHIFT) |
                                                              (accumulator & HYPERDOS_X86_LOW_BYTE_MASK)));
        }
        hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);
        hyperdos_pc_disk_bios_set_disk_operation_status(pc, driveNumber, HYPERDOS_PC_DISK_BIOS_STATUS_SUCCESS);
        hyperdos_pc_disk_bios_set_carry_flag(processor, 0);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_DISK_BIOS_CHANGE_LINE_STATUS_SERVICE)
    {
        hyperdos_pc_disk_image* diskImage = NULL;
        uint8_t                 changed   = 0u;

        hyperdos_pc_disk_bios_lock_disk_images(diskBiosInterface);
        diskImage = hyperdos_pc_disk_bios_get_disk_image(diskBiosInterface, driveNumber);
        if (diskImage == NULL || diskImage->isHardDisk)
        {
            if (diskImage == NULL && hyperdos_pc_disk_bios_floppy_drive_is_installed(diskBiosInterface, driveNumber))
            {
                hyperdos_pc_disk_bios_trace(diskBiosInterface,
                                            "int13 change-line drive=%02X changed=1 no-media",
                                            driveNumber);
                hyperdos_pc_disk_bios_mark_floppy_drive_media_state_changed(pc, driveNumber, NULL);
                hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);
                hyperdos_x86_set_general_register_word(processor,
                                                       HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                                       (uint16_t)((HYPERDOS_PC_DISK_BIOS_STATUS_CHANGED
                                                                   << HYPERDOS_PC_DISK_BIOS_SERVICE_REGISTER_SHIFT) |
                                                                  (accumulator & HYPERDOS_X86_LOW_BYTE_MASK)));
                hyperdos_pc_disk_bios_set_carry_flag(processor, 1);
                return HYPERDOS_X86_EXECUTION_OK;
            }
            hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);
            hyperdos_pc_disk_bios_return_invalid_function(processor, pc, driveNumber, accumulator);
            return HYPERDOS_X86_EXECUTION_OK;
        }
        changed = (uint8_t)(diskImage->mediaChanged ||
                            (diskBiosInterface->floppyController != NULL &&
                             hyperdos_pc_floppy_controller_drive_disk_changed(diskBiosInterface->floppyController,
                                                                              driveNumber)));
        hyperdos_pc_disk_bios_trace(
                diskBiosInterface,
                "int13 change-line drive=%02X changed=%u media-changed=%u reported=%u "
                "controller-changed=%u path=\"%s\"",
                driveNumber,
                changed,
                diskImage->mediaChanged,
                diskImage->mediaChangeReported,
                diskBiosInterface->floppyController != NULL
                        ? hyperdos_pc_floppy_controller_drive_disk_changed(diskBiosInterface->floppyController,
                                                                           driveNumber)
                        : 0u,
                diskImage->path);
        if (changed)
        {
            hyperdos_pc_disk_bios_mark_floppy_drive_media_state_changed(pc, driveNumber, diskImage);
            diskImage->mediaChangeReported = 1u;
            diskImage->mediaChanged        = 0u;
            if (diskBiosInterface->floppyController != NULL)
            {
                hyperdos_pc_floppy_controller_acknowledge_drive_disk_change(diskBiosInterface->floppyController,
                                                                            driveNumber);
            }
        }
        else
        {
            hyperdos_pc_disk_bios_set_disk_operation_status(pc, driveNumber, HYPERDOS_PC_DISK_BIOS_STATUS_SUCCESS);
        }
        hyperdos_pc_disk_bios_unlock_disk_images(diskBiosInterface);
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                               (uint16_t)(((changed ? HYPERDOS_PC_DISK_BIOS_STATUS_CHANGED
                                                                    : HYPERDOS_PC_DISK_BIOS_STATUS_SUCCESS)
                                                           << HYPERDOS_PC_DISK_BIOS_SERVICE_REGISTER_SHIFT) |
                                                          (accumulator & HYPERDOS_X86_LOW_BYTE_MASK)));
        hyperdos_pc_disk_bios_set_carry_flag(processor, changed);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    if (serviceNumber == HYPERDOS_PC_DISK_BIOS_EXTENSION_INSTALLATION_CHECK_SERVICE)
    {
        hyperdos_pc_disk_bios_set_status_in_accumulator(processor, HYPERDOS_PC_DISK_BIOS_STATUS_INVALID_FUNCTION);
        hyperdos_pc_disk_bios_set_carry_flag(processor, 1);
        return HYPERDOS_X86_EXECUTION_OK;
    }

    hyperdos_pc_disk_bios_set_status_in_accumulator(processor, HYPERDOS_PC_DISK_BIOS_STATUS_INVALID_FUNCTION);
    hyperdos_pc_disk_bios_set_carry_flag(processor, 1);
    return HYPERDOS_X86_EXECUTION_OK;
}
