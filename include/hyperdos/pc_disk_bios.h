#ifndef HYPERDOS_PC_DISK_BIOS_H
#define HYPERDOS_PC_DISK_BIOS_H

#include <stdint.h>

#include "hyperdos/pc_board.h"
#include "hyperdos/pc_disk_image.h"
#include "hyperdos/pc_floppy_controller.h"

enum
{
    HYPERDOS_PC_DISK_BIOS_FLOPPY_DRIVE_NUMBER    = 0x00u,
    HYPERDOS_PC_DISK_BIOS_HARD_DISK_DRIVE_NUMBER = 0x80u
};

typedef void (*hyperdos_pc_disk_bios_trace_function)(void* userContext, const char* message);

typedef void (*hyperdos_pc_disk_bios_lock_function)(void* userContext);

typedef hyperdos_pc_disk_image* (*hyperdos_pc_disk_bios_get_disk_image_function)(void*   userContext,
                                                                                 uint8_t driveNumber);

typedef struct hyperdos_pc_disk_bios_interface
{
    void*                                         userContext;
    hyperdos_pc*                                  pc;
    hyperdos_pc_floppy_controller*                floppyController;
    uint8_t                                       floppyDriveCount;
    uint8_t                                       fixedDiskDriveCount;
    uint8_t                                       floppyDiskChangeLineSupported;
    hyperdos_pc_disk_bios_lock_function           lockDiskImages;
    hyperdos_pc_disk_bios_lock_function           unlockDiskImages;
    hyperdos_pc_disk_bios_get_disk_image_function getDiskImage;
    hyperdos_pc_disk_bios_trace_function          traceFunction;
} hyperdos_pc_disk_bios_interface;

void hyperdos_pc_disk_bios_initialize_data_area(hyperdos_pc*                  pc,
                                                const hyperdos_pc_disk_image* activeFloppyDisk,
                                                uint8_t                       fixedDiskCount);

void hyperdos_pc_disk_bios_set_fixed_disk_count(hyperdos_pc* pc, uint8_t fixedDiskCount);

void hyperdos_pc_disk_bios_mark_floppy_drive_media_state_changed(hyperdos_pc*                  pc,
                                                                 uint8_t                       driveNumber,
                                                                 const hyperdos_pc_disk_image* diskImage);

void hyperdos_pc_disk_bios_mark_floppy_media_state_changed(hyperdos_pc* pc, const hyperdos_pc_disk_image* diskImage);

void hyperdos_pc_disk_bios_mark_floppy_drive_media_state_established(hyperdos_pc*                  pc,
                                                                     uint8_t                       driveNumber,
                                                                     const hyperdos_pc_disk_image* diskImage,
                                                                     uint8_t                       currentCylinder);

void hyperdos_pc_disk_bios_mark_floppy_media_state_established(hyperdos_pc*                  pc,
                                                               const hyperdos_pc_disk_image* diskImage,
                                                               uint8_t                       currentCylinder);

void hyperdos_pc_disk_bios_set_disk_operation_status(hyperdos_pc* pc, uint8_t driveNumber, uint8_t status);

uint8_t hyperdos_pc_disk_bios_get_disk_operation_status(hyperdos_pc* pc, uint8_t driveNumber);

hyperdos_x86_execution_result hyperdos_pc_disk_bios_handle_interrupt(
        hyperdos_x86_processor*                processor,
        const hyperdos_pc_disk_bios_interface* diskBiosInterface);

#endif
