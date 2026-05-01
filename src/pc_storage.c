#include "hyperdos/pc_storage.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "hyperdos/pc_disk_bios.h"

enum
{
    HYPERDOS_PC_STORAGE_TRACE_TEXT_CAPACITY               = 512u,
    HYPERDOS_PC_STORAGE_FIXED_DISK_BIOS_DRIVE_NUMBER_BASE = 0x80u
};

static void hyperdos_pc_storage_trace(hyperdos_pc_storage_context* storageContext, const char* format, ...)
{
    char    message[HYPERDOS_PC_STORAGE_TRACE_TEXT_CAPACITY];
    va_list arguments;

    if (storageContext == NULL || storageContext->traceFunction == NULL)
    {
        return;
    }

    va_start(arguments, format);
    (void)vsnprintf(message, sizeof(message), format, arguments);
    va_end(arguments);
    storageContext->traceFunction(storageContext->traceUserContext, message);
}

static int hyperdos_pc_storage_context_is_valid(const hyperdos_pc_storage_context* storageContext)
{
    return storageContext != NULL && storageContext->pc != NULL && storageContext->floppyController != NULL &&
           storageContext->floppyDrives != NULL && storageContext->fixedDiskDrives != NULL;
}

static void hyperdos_pc_storage_move_disk_image(hyperdos_pc_disk_image* destination, hyperdos_pc_disk_image* source)
{
    *destination = *source;
    memset(source, 0, sizeof(*source));
}

static int hyperdos_pc_storage_floppy_drive_is_valid(const hyperdos_pc_storage_context* storageContext,
                                                     uint8_t                            driveNumber)
{
    return hyperdos_pc_storage_context_is_valid(storageContext) && driveNumber < storageContext->floppyDriveCount;
}

static int hyperdos_pc_storage_fixed_disk_drive_is_valid(const hyperdos_pc_storage_context* storageContext,
                                                         uint8_t                            driveIndex)
{
    return hyperdos_pc_storage_context_is_valid(storageContext) && driveIndex < storageContext->fixedDiskDriveCount;
}

static void hyperdos_pc_storage_update_fixed_disk_count(hyperdos_pc_storage_context* storageContext)
{
    if (storageContext == NULL)
    {
        return;
    }
    hyperdos_pc_disk_bios_set_fixed_disk_count(storageContext->pc,
                                               hyperdos_pc_storage_count_inserted_fixed_disks(storageContext));
}

static uint8_t hyperdos_pc_storage_get_floppy_cmos_drive_type(const hyperdos_pc_storage_floppy_drive* floppyDrive)
{
    const hyperdos_pc_disk_image* diskImage = NULL;

    if (floppyDrive == NULL || !floppyDrive->installed)
    {
        return 0u;
    }
    diskImage = floppyDrive->diskImage.inserted ? &floppyDrive->diskImage : NULL;
    if (diskImage == NULL)
    {
        return 4u;
    }
    if (diskImage->headCount == 2u && diskImage->sectorsPerTrack == 18u && diskImage->cylinderCount >= 80u)
    {
        return 4u;
    }
    if (diskImage->headCount == 2u && diskImage->sectorsPerTrack == 15u && diskImage->cylinderCount >= 80u)
    {
        return 2u;
    }
    if (diskImage->headCount == 2u && diskImage->sectorsPerTrack == 9u && diskImage->cylinderCount >= 80u)
    {
        return 3u;
    }
    if (diskImage->sectorsPerTrack <= 9u && diskImage->cylinderCount <= 40u)
    {
        return 1u;
    }
    return 4u;
}

static void hyperdos_pc_storage_update_cmos_floppy_drive_types(hyperdos_pc_storage_context* storageContext)
{
    uint8_t driveNumber = 0u;

    if (storageContext == NULL || storageContext->pc == NULL)
    {
        return;
    }
    for (driveNumber = 0u; driveNumber < HYPERDOS_PC_CMOS_MAXIMUM_FLOPPY_DRIVE_COUNT; ++driveNumber)
    {
        uint8_t driveType = 0u;

        if (driveNumber < storageContext->floppyDriveCount)
        {
            driveType = hyperdos_pc_storage_get_floppy_cmos_drive_type(&storageContext->floppyDrives[driveNumber]);
        }
        hyperdos_pc_cmos_set_floppy_drive_type(&storageContext->pc->realTimeClock, driveNumber, driveType);
    }
}

static void hyperdos_pc_storage_update_cmos_fixed_disk_geometries(hyperdos_pc_storage_context* storageContext)
{
    uint8_t fixedDiskIndex = 0u;

    if (storageContext == NULL || storageContext->pc == NULL)
    {
        return;
    }
    for (fixedDiskIndex = 0u; fixedDiskIndex < HYPERDOS_PC_CMOS_MAXIMUM_FIXED_DISK_COUNT; ++fixedDiskIndex)
    {
        hyperdos_pc_cmos_fixed_disk_geometry geometry;

        memset(&geometry, 0, sizeof(geometry));
        if (fixedDiskIndex < storageContext->fixedDiskDriveCount &&
            storageContext->fixedDiskDrives[fixedDiskIndex].installed &&
            storageContext->fixedDiskDrives[fixedDiskIndex].diskImage.inserted)
        {
            const hyperdos_pc_disk_image* diskImage = &storageContext->fixedDiskDrives[fixedDiskIndex].diskImage;

            geometry.installed       = 1u;
            geometry.cylinderCount   = diskImage->cylinderCount;
            geometry.headCount       = (uint8_t)diskImage->headCount;
            geometry.sectorsPerTrack = (uint8_t)diskImage->sectorsPerTrack;
        }
        hyperdos_pc_cmos_set_fixed_disk_geometry(&storageContext->pc->realTimeClock, fixedDiskIndex, &geometry);
    }
}

static void hyperdos_pc_storage_notify_floppy_disk_changed(hyperdos_pc_storage_context* storageContext,
                                                           uint8_t                      driveNumber,
                                                           hyperdos_pc_disk_image*      diskImage)
{
    if (storageContext == NULL)
    {
        return;
    }

    hyperdos_pc_floppy_controller_notify_drive_disk_changed(storageContext->floppyController, driveNumber);
    hyperdos_pc_disk_bios_mark_floppy_drive_media_state_changed(storageContext->pc, driveNumber, diskImage);
}

void hyperdos_pc_storage_context_initialize(hyperdos_pc_storage_context*          storageContext,
                                            hyperdos_pc*                          pc,
                                            hyperdos_pc_floppy_controller*        floppyController,
                                            hyperdos_pc_storage_floppy_drive*     floppyDrives,
                                            size_t                                floppyDriveCount,
                                            hyperdos_pc_storage_fixed_disk_drive* fixedDiskDrives,
                                            size_t                                fixedDiskDriveCount,
                                            hyperdos_pc_storage_trace_function    traceFunction,
                                            void*                                 traceUserContext)
{
    if (storageContext == NULL)
    {
        return;
    }

    memset(storageContext, 0, sizeof(*storageContext));
    storageContext->pc                  = pc;
    storageContext->floppyController    = floppyController;
    storageContext->floppyDrives        = floppyDrives;
    storageContext->floppyDriveCount    = floppyDriveCount;
    storageContext->fixedDiskDrives     = fixedDiskDrives;
    storageContext->fixedDiskDriveCount = fixedDiskDriveCount;
    storageContext->traceFunction       = traceFunction;
    storageContext->traceUserContext    = traceUserContext;
}

void hyperdos_pc_storage_install_floppy_drive(hyperdos_pc_storage_context* storageContext, uint8_t driveNumber)
{
    if (!hyperdos_pc_storage_floppy_drive_is_valid(storageContext, driveNumber))
    {
        return;
    }
    storageContext->floppyDrives[driveNumber].installed = 1u;
    hyperdos_pc_storage_update_cmos_floppy_drive_types(storageContext);
}

void hyperdos_pc_storage_install_fixed_disk_drive(hyperdos_pc_storage_context* storageContext, uint8_t driveIndex)
{
    if (!hyperdos_pc_storage_fixed_disk_drive_is_valid(storageContext, driveIndex))
    {
        return;
    }
    storageContext->fixedDiskDrives[driveIndex].installed = 1u;
    hyperdos_pc_storage_update_fixed_disk_count(storageContext);
    hyperdos_pc_storage_update_cmos_fixed_disk_geometries(storageContext);
}

uint8_t hyperdos_pc_storage_count_installed_floppy_drives(const hyperdos_pc_storage_context* storageContext)
{
    size_t  driveIndex          = 0u;
    uint8_t installedDriveCount = 0u;

    if (!hyperdos_pc_storage_context_is_valid(storageContext))
    {
        return 0u;
    }
    for (driveIndex = 0u; driveIndex < storageContext->floppyDriveCount && installedDriveCount < 0xFFu; ++driveIndex)
    {
        if (storageContext->floppyDrives[driveIndex].installed)
        {
            ++installedDriveCount;
        }
    }
    return installedDriveCount;
}

uint8_t hyperdos_pc_storage_count_inserted_fixed_disks(const hyperdos_pc_storage_context* storageContext)
{
    size_t  driveIndex        = 0u;
    uint8_t insertedDiskCount = 0u;

    if (!hyperdos_pc_storage_context_is_valid(storageContext))
    {
        return 0u;
    }
    for (driveIndex = 0u; driveIndex < storageContext->fixedDiskDriveCount && insertedDiskCount < 0xFFu; ++driveIndex)
    {
        if (storageContext->fixedDiskDrives[driveIndex].installed &&
            storageContext->fixedDiskDrives[driveIndex].diskImage.inserted)
        {
            ++insertedDiskCount;
        }
    }
    return insertedDiskCount;
}

hyperdos_pc_disk_image* hyperdos_pc_storage_get_floppy_disk_for_drive_number(
        const hyperdos_pc_storage_context* storageContext,
        uint8_t                            driveNumber)
{
    hyperdos_pc_storage_floppy_drive* floppyDrive = NULL;

    if (!hyperdos_pc_storage_floppy_drive_is_valid(storageContext, driveNumber))
    {
        return NULL;
    }
    floppyDrive = &storageContext->floppyDrives[driveNumber];
    if (!floppyDrive->installed || !floppyDrive->diskImage.inserted)
    {
        return NULL;
    }
    return &floppyDrive->diskImage;
}

hyperdos_pc_disk_image* hyperdos_pc_storage_get_fixed_disk_for_drive_index(
        const hyperdos_pc_storage_context* storageContext,
        uint8_t                            driveIndex)
{
    hyperdos_pc_storage_fixed_disk_drive* fixedDiskDrive = NULL;

    if (!hyperdos_pc_storage_fixed_disk_drive_is_valid(storageContext, driveIndex))
    {
        return NULL;
    }
    fixedDiskDrive = &storageContext->fixedDiskDrives[driveIndex];
    if (!fixedDiskDrive->installed || !fixedDiskDrive->diskImage.inserted)
    {
        return NULL;
    }
    return &fixedDiskDrive->diskImage;
}

hyperdos_pc_disk_image* hyperdos_pc_storage_get_disk_for_bios_drive_number(
        const hyperdos_pc_storage_context* storageContext,
        uint8_t                            biosDriveNumber)
{
    if (biosDriveNumber < HYPERDOS_PC_STORAGE_FIXED_DISK_BIOS_DRIVE_NUMBER_BASE)
    {
        return hyperdos_pc_storage_get_floppy_disk_for_drive_number(storageContext, biosDriveNumber);
    }
    return hyperdos_pc_storage_get_fixed_disk_for_drive_index(
            storageContext,
            (uint8_t)(biosDriveNumber - HYPERDOS_PC_STORAGE_FIXED_DISK_BIOS_DRIVE_NUMBER_BASE));
}

int hyperdos_pc_storage_flush_all_disk_images(hyperdos_pc_storage_context* storageContext)
{
    size_t driveIndex = 0u;
    int    flushed    = 1;

    if (!hyperdos_pc_storage_context_is_valid(storageContext))
    {
        return 0;
    }

    for (driveIndex = 0u; driveIndex < storageContext->floppyDriveCount; ++driveIndex)
    {
        if (!hyperdos_pc_disk_image_flush(&storageContext->floppyDrives[driveIndex].diskImage))
        {
            flushed = 0;
        }
    }
    for (driveIndex = 0u; driveIndex < storageContext->fixedDiskDriveCount; ++driveIndex)
    {
        if (!hyperdos_pc_disk_image_flush(&storageContext->fixedDiskDrives[driveIndex].diskImage))
        {
            flushed = 0;
        }
    }
    return flushed;
}

void hyperdos_pc_storage_free_all_disk_images(hyperdos_pc_storage_context* storageContext)
{
    size_t driveIndex = 0u;

    if (!hyperdos_pc_storage_context_is_valid(storageContext))
    {
        return;
    }

    for (driveIndex = 0u; driveIndex < storageContext->floppyDriveCount; ++driveIndex)
    {
        hyperdos_pc_disk_image_free(&storageContext->floppyDrives[driveIndex].diskImage);
    }
    for (driveIndex = 0u; driveIndex < storageContext->fixedDiskDriveCount; ++driveIndex)
    {
        hyperdos_pc_disk_image_free(&storageContext->fixedDiskDrives[driveIndex].diskImage);
    }
}

hyperdos_pc_storage_result hyperdos_pc_storage_insert_floppy_disk_image(hyperdos_pc_storage_context* storageContext,
                                                                        uint8_t                      driveNumber,
                                                                        hyperdos_pc_disk_image*      replacementDisk)
{
    hyperdos_pc_storage_floppy_drive* floppyDrive = NULL;

    if (!hyperdos_pc_storage_floppy_drive_is_valid(storageContext, driveNumber) || replacementDisk == NULL ||
        !replacementDisk->inserted)
    {
        return HYPERDOS_PC_STORAGE_INVALID_ARGUMENT;
    }
    floppyDrive = &storageContext->floppyDrives[driveNumber];
    if (!floppyDrive->installed)
    {
        return HYPERDOS_PC_STORAGE_DRIVE_NOT_INSTALLED;
    }
    if (!hyperdos_pc_disk_image_flush(&floppyDrive->diskImage))
    {
        return HYPERDOS_PC_STORAGE_FLUSH_FAILED;
    }

    replacementDisk->driveNumber         = driveNumber;
    replacementDisk->mediaChanged        = 1u;
    replacementDisk->mediaChangeReported = 0u;
    hyperdos_pc_storage_trace(storageContext,
                              "floppy insert drive=%u path=\"%s\" bytes=%zu sectors-per-track=%u heads=%u "
                              "cylinders=%u",
                              driveNumber,
                              replacementDisk->path,
                              replacementDisk->byteCount,
                              replacementDisk->sectorsPerTrack,
                              replacementDisk->headCount,
                              replacementDisk->cylinderCount);

    hyperdos_pc_disk_image_free(&floppyDrive->diskImage);
    hyperdos_pc_storage_move_disk_image(&floppyDrive->diskImage, replacementDisk);
    hyperdos_pc_storage_update_cmos_floppy_drive_types(storageContext);
    hyperdos_pc_storage_notify_floppy_disk_changed(storageContext, driveNumber, &floppyDrive->diskImage);
    return HYPERDOS_PC_STORAGE_OK;
}

hyperdos_pc_storage_result hyperdos_pc_storage_eject_floppy_disk(hyperdos_pc_storage_context* storageContext,
                                                                 uint8_t                      driveNumber)
{
    hyperdos_pc_storage_floppy_drive* floppyDrive = NULL;

    if (!hyperdos_pc_storage_floppy_drive_is_valid(storageContext, driveNumber))
    {
        return HYPERDOS_PC_STORAGE_INVALID_ARGUMENT;
    }
    floppyDrive = &storageContext->floppyDrives[driveNumber];
    if (!floppyDrive->installed)
    {
        return HYPERDOS_PC_STORAGE_DRIVE_NOT_INSTALLED;
    }
    if (!floppyDrive->diskImage.inserted)
    {
        return HYPERDOS_PC_STORAGE_NO_MEDIA;
    }
    if (!hyperdos_pc_disk_image_flush(&floppyDrive->diskImage))
    {
        return HYPERDOS_PC_STORAGE_FLUSH_FAILED;
    }
    hyperdos_pc_disk_image_free(&floppyDrive->diskImage);
    hyperdos_pc_storage_update_cmos_floppy_drive_types(storageContext);
    hyperdos_pc_storage_notify_floppy_disk_changed(storageContext, driveNumber, NULL);
    return HYPERDOS_PC_STORAGE_OK;
}

hyperdos_pc_storage_result hyperdos_pc_storage_attach_fixed_disk_image(hyperdos_pc_storage_context* storageContext,
                                                                       uint8_t                      fixedDiskIndex,
                                                                       hyperdos_pc_disk_image*      replacementDisk)
{
    hyperdos_pc_storage_fixed_disk_drive* fixedDiskDrive = NULL;

    if (!hyperdos_pc_storage_fixed_disk_drive_is_valid(storageContext, fixedDiskIndex) || replacementDisk == NULL ||
        !replacementDisk->inserted)
    {
        return HYPERDOS_PC_STORAGE_INVALID_ARGUMENT;
    }
    fixedDiskDrive = &storageContext->fixedDiskDrives[fixedDiskIndex];
    if (!hyperdos_pc_disk_image_flush(&fixedDiskDrive->diskImage))
    {
        return HYPERDOS_PC_STORAGE_FLUSH_FAILED;
    }

    replacementDisk->driveNumber = (uint8_t)(HYPERDOS_PC_STORAGE_FIXED_DISK_BIOS_DRIVE_NUMBER_BASE + fixedDiskIndex);
    hyperdos_pc_disk_image_free(&fixedDiskDrive->diskImage);
    hyperdos_pc_storage_move_disk_image(&fixedDiskDrive->diskImage, replacementDisk);
    fixedDiskDrive->installed = 1u;
    hyperdos_pc_storage_update_fixed_disk_count(storageContext);
    hyperdos_pc_storage_update_cmos_fixed_disk_geometries(storageContext);
    return HYPERDOS_PC_STORAGE_OK;
}
