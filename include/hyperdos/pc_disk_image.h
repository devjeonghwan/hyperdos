#ifndef HYPERDOS_PC_DISK_IMAGE_H
#define HYPERDOS_PC_DISK_IMAGE_H

#include <stddef.h>
#include <stdint.h>

enum
{
    HYPERDOS_PC_DISK_IMAGE_PATH_CAPACITY       = 1024u,
    HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_SIZE    = 512u,
    HYPERDOS_PC_DISK_IMAGE_DEFAULT_SECTOR_SIZE = 512u
};

typedef enum hyperdos_pc_disk_transfer_result
{
    HYPERDOS_PC_DISK_TRANSFER_OK = 0,
    HYPERDOS_PC_DISK_TRANSFER_OUT_OF_RANGE,
    HYPERDOS_PC_DISK_TRANSFER_WRITE_PROTECTED,
    HYPERDOS_PC_DISK_TRANSFER_HOST_ERROR
} hyperdos_pc_disk_transfer_result;

typedef hyperdos_pc_disk_transfer_result (*hyperdos_pc_disk_read_sectors_function)(void*    userContext,
                                                                                   uint64_t logicalBlockAddress,
                                                                                   uint16_t sectorCount,
                                                                                   uint8_t* destinationBytes);

typedef hyperdos_pc_disk_transfer_result (*hyperdos_pc_disk_write_sectors_function)(void*          userContext,
                                                                                    uint64_t       logicalBlockAddress,
                                                                                    uint16_t       sectorCount,
                                                                                    const uint8_t* sourceBytes);

typedef hyperdos_pc_disk_transfer_result (*hyperdos_pc_disk_flush_function)(void* userContext);

typedef void (*hyperdos_pc_disk_destroy_function)(void* userContext);

typedef struct hyperdos_pc_disk_operations
{
    hyperdos_pc_disk_read_sectors_function  readSectors;
    hyperdos_pc_disk_write_sectors_function writeSectors;
    hyperdos_pc_disk_flush_function         flush;
    hyperdos_pc_disk_destroy_function       destroy;
} hyperdos_pc_disk_operations;

typedef struct hyperdos_pc_disk_image
{
    char                        path[HYPERDOS_PC_DISK_IMAGE_PATH_CAPACITY];
    size_t                      byteCount;
    uint64_t                    sectorCount;
    uint16_t                    bytesPerSector;
    uint16_t                    sectorsPerTrack;
    uint16_t                    headCount;
    uint16_t                    cylinderCount;
    uint8_t                     driveNumber;
    uint8_t                     inserted;
    uint8_t                     dirty;
    uint8_t                     mediaChanged;
    uint8_t                     mediaChangeReported;
    uint8_t                     isHardDisk;
    uint8_t                     readOnly;
    hyperdos_pc_disk_operations operations;
    void*                       operationUserContext;
} hyperdos_pc_disk_image;

int hyperdos_pc_disk_image_initialize_floppy(hyperdos_pc_disk_image*            diskImage,
                                             const char*                        path,
                                             uint64_t                           sectorCount,
                                             uint16_t                           bytesPerSector,
                                             uint8_t                            readOnly,
                                             const hyperdos_pc_disk_operations* operations,
                                             void*                              operationUserContext);

int hyperdos_pc_disk_image_initialize_hard_disk(hyperdos_pc_disk_image*            diskImage,
                                                const char*                        path,
                                                uint64_t                           sectorCount,
                                                uint16_t                           bytesPerSector,
                                                uint8_t                            readOnly,
                                                const hyperdos_pc_disk_operations* operations,
                                                void*                              operationUserContext);

int hyperdos_pc_disk_image_initialize_memory_floppy(hyperdos_pc_disk_image* diskImage,
                                                    const char*             path,
                                                    uint8_t*                bytes,
                                                    size_t                  byteCount,
                                                    uint8_t                 readOnly);

int hyperdos_pc_disk_image_initialize_memory_hard_disk(hyperdos_pc_disk_image* diskImage,
                                                       const char*             path,
                                                       uint8_t*                bytes,
                                                       size_t                  byteCount,
                                                       uint8_t                 readOnly);

void hyperdos_pc_disk_image_free(hyperdos_pc_disk_image* diskImage);

int hyperdos_pc_disk_image_flush(hyperdos_pc_disk_image* diskImage);

hyperdos_pc_disk_transfer_result hyperdos_pc_disk_image_read_sectors(const hyperdos_pc_disk_image* diskImage,
                                                                     uint64_t                      logicalBlockAddress,
                                                                     uint16_t                      sectorCount,
                                                                     uint8_t*                      destinationBytes);

hyperdos_pc_disk_transfer_result hyperdos_pc_disk_image_write_sectors(hyperdos_pc_disk_image* diskImage,
                                                                      uint64_t                logicalBlockAddress,
                                                                      uint16_t                sectorCount,
                                                                      const uint8_t*          sourceBytes);

#endif
