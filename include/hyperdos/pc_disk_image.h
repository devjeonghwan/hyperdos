#ifndef HYPERDOS_PC_DISK_IMAGE_H
#define HYPERDOS_PC_DISK_IMAGE_H

#include <stddef.h>
#include <stdint.h>

enum
{
    HYPERDOS_PC_DISK_IMAGE_PATH_CAPACITY = 1024u
};

typedef struct hyperdos_pc_disk_image
{
    char     path[HYPERDOS_PC_DISK_IMAGE_PATH_CAPACITY];
    uint8_t* bytes;
    size_t   byteCount;
    uint16_t bytesPerSector;
    uint16_t sectorsPerTrack;
    uint16_t headCount;
    uint16_t cylinderCount;
    uint8_t  driveNumber;
    uint8_t  inserted;
    uint8_t  dirty;
    uint8_t  mediaChanged;
    uint8_t  mediaChangeReported;
    uint8_t  isHardDisk;
} hyperdos_pc_disk_image;

void hyperdos_pc_disk_image_free(hyperdos_pc_disk_image* diskImage);

int hyperdos_pc_disk_image_flush(hyperdos_pc_disk_image* diskImage);

int hyperdos_pc_disk_image_load_floppy(hyperdos_pc_disk_image* diskImage, const char* path);

int hyperdos_pc_disk_image_load_hard_disk(hyperdos_pc_disk_image* diskImage, const char* path);

#endif
