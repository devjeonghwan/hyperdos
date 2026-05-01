#ifndef HYPERDOS_PC_STORAGE_H
#define HYPERDOS_PC_STORAGE_H

#include <stddef.h>
#include <stdint.h>

#include "hyperdos/pc_board.h"
#include "hyperdos/pc_disk_image.h"
#include "hyperdos/pc_floppy_controller.h"

typedef enum hyperdos_pc_storage_result
{
    HYPERDOS_PC_STORAGE_OK = 0,
    HYPERDOS_PC_STORAGE_INVALID_ARGUMENT,
    HYPERDOS_PC_STORAGE_DRIVE_NOT_INSTALLED,
    HYPERDOS_PC_STORAGE_NO_MEDIA,
    HYPERDOS_PC_STORAGE_FLUSH_FAILED
} hyperdos_pc_storage_result;

typedef void (*hyperdos_pc_storage_trace_function)(void* userContext, const char* message);

typedef struct hyperdos_pc_storage_floppy_drive
{
    uint8_t                installed;
    hyperdos_pc_disk_image diskImage;
} hyperdos_pc_storage_floppy_drive;

typedef struct hyperdos_pc_storage_fixed_disk_drive
{
    uint8_t                installed;
    hyperdos_pc_disk_image diskImage;
} hyperdos_pc_storage_fixed_disk_drive;

typedef struct hyperdos_pc_storage_context
{
    hyperdos_pc*                          pc;
    hyperdos_pc_floppy_controller*        floppyController;
    hyperdos_pc_storage_floppy_drive*     floppyDrives;
    size_t                                floppyDriveCount;
    hyperdos_pc_storage_fixed_disk_drive* fixedDiskDrives;
    size_t                                fixedDiskDriveCount;
    hyperdos_pc_storage_trace_function    traceFunction;
    void*                                 traceUserContext;
} hyperdos_pc_storage_context;

void hyperdos_pc_storage_context_initialize(hyperdos_pc_storage_context*          storageContext,
                                            hyperdos_pc*                          pc,
                                            hyperdos_pc_floppy_controller*        floppyController,
                                            hyperdos_pc_storage_floppy_drive*     floppyDrives,
                                            size_t                                floppyDriveCount,
                                            hyperdos_pc_storage_fixed_disk_drive* fixedDiskDrives,
                                            size_t                                fixedDiskDriveCount,
                                            hyperdos_pc_storage_trace_function    traceFunction,
                                            void*                                 traceUserContext);

void hyperdos_pc_storage_install_floppy_drive(hyperdos_pc_storage_context* storageContext, uint8_t driveNumber);

void hyperdos_pc_storage_install_fixed_disk_drive(hyperdos_pc_storage_context* storageContext, uint8_t driveIndex);

uint8_t hyperdos_pc_storage_count_installed_floppy_drives(const hyperdos_pc_storage_context* storageContext);

uint8_t hyperdos_pc_storage_count_inserted_fixed_disks(const hyperdos_pc_storage_context* storageContext);

hyperdos_pc_disk_image* hyperdos_pc_storage_get_floppy_disk_for_drive_number(
        const hyperdos_pc_storage_context* storageContext,
        uint8_t                            driveNumber);

hyperdos_pc_disk_image* hyperdos_pc_storage_get_fixed_disk_for_drive_index(
        const hyperdos_pc_storage_context* storageContext,
        uint8_t                            driveIndex);

hyperdos_pc_disk_image* hyperdos_pc_storage_get_disk_for_bios_drive_number(
        const hyperdos_pc_storage_context* storageContext,
        uint8_t                            biosDriveNumber);

int hyperdos_pc_storage_flush_all_disk_images(hyperdos_pc_storage_context* storageContext);

void hyperdos_pc_storage_free_all_disk_images(hyperdos_pc_storage_context* storageContext);

hyperdos_pc_storage_result hyperdos_pc_storage_insert_floppy_disk_image(hyperdos_pc_storage_context* storageContext,
                                                                        uint8_t                      driveNumber,
                                                                        hyperdos_pc_disk_image*      replacementDisk);

hyperdos_pc_storage_result hyperdos_pc_storage_eject_floppy_disk(hyperdos_pc_storage_context* storageContext,
                                                                 uint8_t                      driveNumber);

hyperdos_pc_storage_result hyperdos_pc_storage_attach_fixed_disk_image(hyperdos_pc_storage_context* storageContext,
                                                                       uint8_t                      fixedDiskIndex,
                                                                       hyperdos_pc_disk_image*      replacementDisk);

#endif
