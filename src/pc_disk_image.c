#include "hyperdos/pc_disk_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hyperdos/x86_16_processor.h"

enum
{
    HYPERDOS_PC_DISK_IMAGE_FLOPPY_DRIVE_NUMBER                       = 0x00u,
    HYPERDOS_PC_DISK_IMAGE_HARD_DISK_DRIVE_NUMBER                    = 0x80u,
    HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_BYTE_COUNT                    = 512u,
    HYPERDOS_PC_DISK_IMAGE_BIOS_PARAMETER_BYTES_PER_SECTOR_OFFSET    = 0x0Bu,
    HYPERDOS_PC_DISK_IMAGE_BIOS_PARAMETER_TOTAL_SECTORS_OFFSET       = 0x13u,
    HYPERDOS_PC_DISK_IMAGE_BIOS_PARAMETER_SECTORS_PER_TRACK_OFFSET   = 0x18u,
    HYPERDOS_PC_DISK_IMAGE_BIOS_PARAMETER_HEAD_COUNT_OFFSET          = 0x1Au,
    HYPERDOS_PC_DISK_IMAGE_BIOS_DEFAULT_BYTES_PER_SECTOR             = 512u,
    HYPERDOS_PC_DISK_IMAGE_BIOS_DEFAULT_SECTORS_PER_TRACK            = 18u,
    HYPERDOS_PC_DISK_IMAGE_BIOS_DEFAULT_HEAD_COUNT                   = 2u,
    HYPERDOS_PC_DISK_IMAGE_BIOS_DEFAULT_CYLINDER_COUNT               = 80u,
    HYPERDOS_PC_DISK_IMAGE_HARD_DISK_BYTES_PER_SECTOR                = 512u,
    HYPERDOS_PC_DISK_IMAGE_HARD_DISK_SECTORS_PER_TRACK               = 63u,
    HYPERDOS_PC_DISK_IMAGE_HARD_DISK_HEAD_COUNT                      = 16u,
    HYPERDOS_PC_DISK_IMAGE_HARD_DISK_CYLINDER_COUNT                  = 64u,
    HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_SIGNATURE_OFFSET              = 510u,
    HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_SIGNATURE_FIRST_BYTE          = 0x55u,
    HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_SIGNATURE_SECOND_BYTE         = 0xAAu,
    HYPERDOS_PC_DISK_IMAGE_MASTER_BOOT_RECORD_PARTITION_TABLE_OFFSET = 446u
};

static int hyperdos_pc_disk_image_read_binary_file(const char* path, uint8_t** bytes, size_t* byteCount)
{
    FILE* file     = fopen(path, "rb");
    long  fileSize = 0;

    if (file == NULL)
    {
        return 0;
    }
    if (fseek(file, 0, SEEK_END) != 0)
    {
        fclose(file);
        return 0;
    }
    fileSize = ftell(file);
    if (fileSize <= 0)
    {
        fclose(file);
        return 0;
    }
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        fclose(file);
        return 0;
    }

    *bytes = (uint8_t*)malloc((size_t)fileSize);
    if (*bytes == NULL)
    {
        fclose(file);
        return 0;
    }
    if (fread(*bytes, 1u, (size_t)fileSize, file) != (size_t)fileSize)
    {
        free(*bytes);
        *bytes = NULL;
        fclose(file);
        return 0;
    }

    fclose(file);
    *byteCount = (size_t)fileSize;
    return 1;
}

static int hyperdos_pc_disk_image_write_binary_file(const char* path, const uint8_t* bytes, size_t byteCount)
{
    FILE* file = fopen(path, "wb");

    if (file == NULL)
    {
        return 0;
    }
    if (byteCount != 0u && fwrite(bytes, 1u, byteCount, file) != byteCount)
    {
        fclose(file);
        return 0;
    }
    fclose(file);
    return 1;
}

static void hyperdos_pc_disk_image_copy_string_to_buffer(char* destination, size_t destinationSize, const char* source)
{
    if (destinationSize == 0u)
    {
        return;
    }
    if (source == NULL)
    {
        destination[0] = '\0';
        return;
    }
    snprintf(destination, destinationSize, "%s", source);
}

static uint16_t hyperdos_pc_disk_image_read_little_endian_word(const uint8_t* bytes, size_t offset)
{
    return (uint16_t)(bytes[offset] | ((uint16_t)bytes[offset + 1u] << HYPERDOS_X86_16_BYTE_BIT_COUNT));
}

static uint32_t hyperdos_pc_disk_image_read_little_endian_double_word(const uint8_t* bytes, size_t offset)
{
    return (uint32_t)bytes[offset] | ((uint32_t)bytes[offset + 1u] << 8u) | ((uint32_t)bytes[offset + 2u] << 16u) |
           ((uint32_t)bytes[offset + 3u] << 24u);
}

static int hyperdos_pc_disk_image_boot_sector_signature_is_present(const uint8_t* bytes, size_t byteOffset)
{
    return bytes[byteOffset + HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_SIGNATURE_OFFSET] ==
                   HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_SIGNATURE_FIRST_BYTE &&
           bytes[byteOffset + HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_SIGNATURE_OFFSET + 1u] ==
                   HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_SIGNATURE_SECOND_BYTE;
}

static void hyperdos_pc_disk_image_initialize_floppy_geometry(hyperdos_pc_disk_image* diskImage)
{
    uint16_t totalSectors = 0;

    diskImage->bytesPerSector  = HYPERDOS_PC_DISK_IMAGE_BIOS_DEFAULT_BYTES_PER_SECTOR;
    diskImage->sectorsPerTrack = HYPERDOS_PC_DISK_IMAGE_BIOS_DEFAULT_SECTORS_PER_TRACK;
    diskImage->headCount       = HYPERDOS_PC_DISK_IMAGE_BIOS_DEFAULT_HEAD_COUNT;
    diskImage->cylinderCount   = HYPERDOS_PC_DISK_IMAGE_BIOS_DEFAULT_CYLINDER_COUNT;

    if (diskImage->byteCount >= HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_BYTE_COUNT)
    {
        uint16_t bytesPerSector = hyperdos_pc_disk_image_read_little_endian_word(
                diskImage->bytes,
                HYPERDOS_PC_DISK_IMAGE_BIOS_PARAMETER_BYTES_PER_SECTOR_OFFSET);
        uint16_t sectorsPerTrack = hyperdos_pc_disk_image_read_little_endian_word(
                diskImage->bytes,
                HYPERDOS_PC_DISK_IMAGE_BIOS_PARAMETER_SECTORS_PER_TRACK_OFFSET);
        uint16_t headCount =
                hyperdos_pc_disk_image_read_little_endian_word(diskImage->bytes,
                                                               HYPERDOS_PC_DISK_IMAGE_BIOS_PARAMETER_HEAD_COUNT_OFFSET);
        totalSectors = hyperdos_pc_disk_image_read_little_endian_word(
                diskImage->bytes,
                HYPERDOS_PC_DISK_IMAGE_BIOS_PARAMETER_TOTAL_SECTORS_OFFSET);

        if (bytesPerSector != 0u)
        {
            diskImage->bytesPerSector = bytesPerSector;
        }
        if (sectorsPerTrack != 0u)
        {
            diskImage->sectorsPerTrack = sectorsPerTrack;
        }
        if (headCount != 0u)
        {
            diskImage->headCount = headCount;
        }
    }
    if (totalSectors == 0u)
    {
        totalSectors = (uint16_t)(diskImage->byteCount / diskImage->bytesPerSector);
    }
    if (diskImage->sectorsPerTrack != 0u && diskImage->headCount != 0u)
    {
        uint16_t cylinderCount = (uint16_t)(totalSectors / (diskImage->sectorsPerTrack * diskImage->headCount));
        if (cylinderCount != 0u)
        {
            diskImage->cylinderCount = cylinderCount;
        }
    }
}

static uint16_t hyperdos_pc_disk_image_decode_partition_cylinder(uint8_t sectorByte, uint8_t cylinderByte)
{
    return (uint16_t)(((uint16_t)cylinderByte) | (((uint16_t)sectorByte & 0xC0u) << 2u));
}

static int hyperdos_pc_disk_image_initialize_hard_disk_geometry_from_partition_table(hyperdos_pc_disk_image* diskImage)
{
    enum
    {
        PARTITION_TABLE_ENTRY_COUNT                     = 4u,
        PARTITION_TABLE_ENTRY_BYTE_COUNT                = 16u,
        PARTITION_TABLE_ENTRY_TYPE_OFFSET               = 4u,
        PARTITION_TABLE_ENTRY_START_HEAD_OFFSET         = 1u,
        PARTITION_TABLE_ENTRY_START_SECTOR_OFFSET       = 2u,
        PARTITION_TABLE_ENTRY_START_CYLINDER_OFFSET     = 3u,
        PARTITION_TABLE_ENTRY_END_HEAD_OFFSET           = 5u,
        PARTITION_TABLE_ENTRY_END_SECTOR_OFFSET         = 6u,
        PARTITION_TABLE_ENTRY_END_CYLINDER_OFFSET       = 7u,
        PARTITION_TABLE_ENTRY_TOTAL_SECTOR_COUNT_OFFSET = 12u,
        PARTITION_CHS_SECTOR_MASK                       = 0x3Fu
    };
    size_t   partitionEntryIndex     = 0u;
    uint16_t detectedHeadCount       = 0u;
    uint16_t detectedSectorsPerTrack = 0u;
    uint16_t detectedCylinderCount   = 0u;

    if (diskImage == NULL || diskImage->byteCount < HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_BYTE_COUNT ||
        !hyperdos_pc_disk_image_boot_sector_signature_is_present(diskImage->bytes, 0u))
    {
        return 0;
    }

    for (partitionEntryIndex = 0u; partitionEntryIndex < PARTITION_TABLE_ENTRY_COUNT; ++partitionEntryIndex)
    {
        size_t partitionEntryOffset = HYPERDOS_PC_DISK_IMAGE_MASTER_BOOT_RECORD_PARTITION_TABLE_OFFSET +
                                      partitionEntryIndex * PARTITION_TABLE_ENTRY_BYTE_COUNT;
        uint8_t  partitionType             = diskImage->bytes[partitionEntryOffset + PARTITION_TABLE_ENTRY_TYPE_OFFSET];
        uint32_t partitionTotalSectorCount = hyperdos_pc_disk_image_read_little_endian_double_word(
                diskImage->bytes,
                partitionEntryOffset + PARTITION_TABLE_ENTRY_TOTAL_SECTOR_COUNT_OFFSET);
        uint8_t  startHead     = diskImage->bytes[partitionEntryOffset + PARTITION_TABLE_ENTRY_START_HEAD_OFFSET];
        uint8_t  startSector   = (uint8_t)(diskImage->bytes[partitionEntryOffset +
                                                         PARTITION_TABLE_ENTRY_START_SECTOR_OFFSET] &
                                        PARTITION_CHS_SECTOR_MASK);
        uint16_t startCylinder = hyperdos_pc_disk_image_decode_partition_cylinder(
                diskImage->bytes[partitionEntryOffset + PARTITION_TABLE_ENTRY_START_SECTOR_OFFSET],
                diskImage->bytes[partitionEntryOffset + PARTITION_TABLE_ENTRY_START_CYLINDER_OFFSET]);
        uint8_t endHead   = diskImage->bytes[partitionEntryOffset + PARTITION_TABLE_ENTRY_END_HEAD_OFFSET];
        uint8_t endSector = (uint8_t)(diskImage->bytes[partitionEntryOffset + PARTITION_TABLE_ENTRY_END_SECTOR_OFFSET] &
                                      PARTITION_CHS_SECTOR_MASK);
        uint16_t endCylinder = hyperdos_pc_disk_image_decode_partition_cylinder(
                diskImage->bytes[partitionEntryOffset + PARTITION_TABLE_ENTRY_END_SECTOR_OFFSET],
                diskImage->bytes[partitionEntryOffset + PARTITION_TABLE_ENTRY_END_CYLINDER_OFFSET]);

        if (partitionType == 0u || partitionTotalSectorCount == 0u)
        {
            continue;
        }
        if ((uint16_t)(startHead + 1u) > detectedHeadCount)
        {
            detectedHeadCount = (uint16_t)(startHead + 1u);
        }
        if ((uint16_t)(endHead + 1u) > detectedHeadCount)
        {
            detectedHeadCount = (uint16_t)(endHead + 1u);
        }
        if (startSector > detectedSectorsPerTrack)
        {
            detectedSectorsPerTrack = startSector;
        }
        if (endSector > detectedSectorsPerTrack)
        {
            detectedSectorsPerTrack = endSector;
        }
        if ((uint16_t)(startCylinder + 1u) > detectedCylinderCount)
        {
            detectedCylinderCount = (uint16_t)(startCylinder + 1u);
        }
        if ((uint16_t)(endCylinder + 1u) > detectedCylinderCount)
        {
            detectedCylinderCount = (uint16_t)(endCylinder + 1u);
        }
    }

    if (detectedHeadCount == 0u || detectedSectorsPerTrack == 0u)
    {
        return 0;
    }
    if (detectedCylinderCount == 0u)
    {
        size_t totalSectors       = diskImage->byteCount / HYPERDOS_PC_DISK_IMAGE_HARD_DISK_BYTES_PER_SECTOR;
        size_t sectorsPerCylinder = detectedHeadCount * detectedSectorsPerTrack;

        detectedCylinderCount = (uint16_t)(totalSectors / sectorsPerCylinder);
    }
    else
    {
        size_t totalSectors       = diskImage->byteCount / HYPERDOS_PC_DISK_IMAGE_HARD_DISK_BYTES_PER_SECTOR;
        size_t sectorsPerCylinder = detectedHeadCount * detectedSectorsPerTrack;
        size_t imageCylinderCount = sectorsPerCylinder != 0u ? totalSectors / sectorsPerCylinder : 0u;

        if (imageCylinderCount > detectedCylinderCount)
        {
            detectedCylinderCount = imageCylinderCount > 0xFFFFu ? 0xFFFFu : (uint16_t)imageCylinderCount;
        }
    }
    if (detectedCylinderCount == 0u)
    {
        return 0;
    }

    diskImage->bytesPerSector  = HYPERDOS_PC_DISK_IMAGE_HARD_DISK_BYTES_PER_SECTOR;
    diskImage->sectorsPerTrack = detectedSectorsPerTrack;
    diskImage->headCount       = detectedHeadCount;
    diskImage->cylinderCount   = detectedCylinderCount;
    return 1;
}

static void hyperdos_pc_disk_image_initialize_hard_disk_geometry(hyperdos_pc_disk_image* diskImage)
{
    size_t sectorsPerCylinder = HYPERDOS_PC_DISK_IMAGE_HARD_DISK_SECTORS_PER_TRACK *
                                HYPERDOS_PC_DISK_IMAGE_HARD_DISK_HEAD_COUNT;
    size_t totalSectors  = diskImage->byteCount / HYPERDOS_PC_DISK_IMAGE_HARD_DISK_BYTES_PER_SECTOR;
    size_t cylinderCount = totalSectors / sectorsPerCylinder;

    if (hyperdos_pc_disk_image_initialize_hard_disk_geometry_from_partition_table(diskImage))
    {
        return;
    }

    diskImage->bytesPerSector  = HYPERDOS_PC_DISK_IMAGE_HARD_DISK_BYTES_PER_SECTOR;
    diskImage->sectorsPerTrack = HYPERDOS_PC_DISK_IMAGE_HARD_DISK_SECTORS_PER_TRACK;
    diskImage->headCount       = HYPERDOS_PC_DISK_IMAGE_HARD_DISK_HEAD_COUNT;
    diskImage->cylinderCount   = cylinderCount == 0u ? HYPERDOS_PC_DISK_IMAGE_HARD_DISK_CYLINDER_COUNT
                                                     : (uint16_t)cylinderCount;
}

static int hyperdos_pc_disk_image_master_boot_record_bootstrap_is_empty(const uint8_t* bytes)
{
    size_t byteOffset = 0;

    for (byteOffset = 0u; byteOffset < HYPERDOS_PC_DISK_IMAGE_MASTER_BOOT_RECORD_PARTITION_TABLE_OFFSET; ++byteOffset)
    {
        if (bytes[byteOffset] != 0u)
        {
            return 0;
        }
    }
    return 1;
}

static int hyperdos_pc_disk_image_master_boot_record_bootstrap_is_hyperdos_x86_16_message_stub(const uint8_t* bytes)
{
    static const uint8_t messageStubBytes[] = {0x31u, 0xC0u, 0x8Eu, 0xD8u, 0xBEu, 0x15u, 0x7Cu, 0xB4u, 0x0Eu, 0xACu,
                                               0x84u, 0xC0u, 0x74u, 0x04u, 0xCDu, 0x10u, 0xEBu, 0xF7u, 0xF4u, 0xEBu,
                                               0xFDu, 'M',   'i',   's',   's',   'i',   'n',   'g',   ' ',   'o',
                                               'p',   'e',   'r',   'a',   't',   'i',   'n',   'g',   ' ',   's',
                                               'y',   's',   't',   'e',   'm',   0u};

    return memcmp(bytes, messageStubBytes, sizeof(messageStubBytes)) == 0;
}

static void hyperdos_pc_disk_image_write_master_boot_record_bootstrap(uint8_t* bytes)
{
    static const uint8_t bootstrapBytes[] = {0xFAu, 0x31u, 0xC0u, 0x8Eu, 0xD0u, 0xBCu, 0x00u, 0x7Cu, 0xFBu, 0x8Eu,
                                             0xD8u, 0x8Eu, 0xC0u, 0xBEu, 0x00u, 0x7Cu, 0xBFu, 0x00u, 0x06u, 0xB9u,
                                             0x00u, 0x01u, 0xFCu, 0xF3u, 0xA5u, 0xEAu, 0x1Eu, 0x06u, 0x00u, 0x00u,
                                             0xBEu, 0xBEu, 0x07u, 0xB9u, 0x04u, 0x00u, 0x80u, 0x3Cu, 0x80u, 0x74u,
                                             0x07u, 0x83u, 0xC6u, 0x10u, 0xE2u, 0xF6u, 0xEBu, 0x1Du, 0xBBu, 0x00u,
                                             0x7Cu, 0xB8u, 0x01u, 0x02u, 0x8Au, 0x74u, 0x01u, 0x8Bu, 0x4Cu, 0x02u,
                                             0xCDu, 0x13u, 0x72u, 0x0Du, 0x81u, 0x3Eu, 0xFEu, 0x7Du, 0x55u, 0xAAu,
                                             0x75u, 0x05u, 0xEAu, 0x00u, 0x7Cu, 0x00u, 0x00u, 0xBEu, 0x5Eu, 0x06u,
                                             0xB4u, 0x0Eu, 0xACu, 0x84u, 0xC0u, 0x74u, 0x04u, 0xCDu, 0x10u, 0xEBu,
                                             0xF7u, 0xF4u, 0xEBu, 0xFDu, 'M',   'i',   's',   's',   'i',   'n',
                                             'g',   ' ',   'o',   'p',   'e',   'r',   'a',   't',   'i',   'n',
                                             'g',   ' ',   's',   'y',   's',   't',   'e',   'm',   0u};

    memcpy(bytes, bootstrapBytes, sizeof(bootstrapBytes));
}

void hyperdos_pc_disk_image_free(hyperdos_pc_disk_image* diskImage)
{
    if (diskImage == NULL)
    {
        return;
    }
    free(diskImage->bytes);
    memset(diskImage, 0, sizeof(*diskImage));
}

int hyperdos_pc_disk_image_flush(hyperdos_pc_disk_image* diskImage)
{
    if (diskImage == NULL || !diskImage->dirty || diskImage->path[0] == '\0')
    {
        return 1;
    }
    if (!hyperdos_pc_disk_image_write_binary_file(diskImage->path, diskImage->bytes, diskImage->byteCount))
    {
        return 0;
    }
    diskImage->dirty = 0u;
    return 1;
}

int hyperdos_pc_disk_image_load_floppy(hyperdos_pc_disk_image* diskImage, const char* path)
{
    memset(diskImage, 0, sizeof(*diskImage));
    if (!hyperdos_pc_disk_image_read_binary_file(path, &diskImage->bytes, &diskImage->byteCount))
    {
        return 0;
    }
    hyperdos_pc_disk_image_copy_string_to_buffer(diskImage->path, sizeof(diskImage->path), path);
    diskImage->driveNumber = HYPERDOS_PC_DISK_IMAGE_FLOPPY_DRIVE_NUMBER;
    diskImage->inserted    = 1u;
    diskImage->isHardDisk  = 0u;
    hyperdos_pc_disk_image_initialize_floppy_geometry(diskImage);
    return 1;
}

static void hyperdos_pc_disk_image_initialize_loaded_hard_disk(hyperdos_pc_disk_image* diskImage, const char* path)
{
    hyperdos_pc_disk_image_copy_string_to_buffer(diskImage->path, sizeof(diskImage->path), path);
    diskImage->driveNumber = HYPERDOS_PC_DISK_IMAGE_HARD_DISK_DRIVE_NUMBER;
    diskImage->inserted    = 1u;
    diskImage->isHardDisk  = 1u;
    if (diskImage->byteCount >= HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_BYTE_COUNT &&
        (hyperdos_pc_disk_image_master_boot_record_bootstrap_is_empty(diskImage->bytes) ||
         hyperdos_pc_disk_image_master_boot_record_bootstrap_is_hyperdos_x86_16_message_stub(diskImage->bytes)) &&
        hyperdos_pc_disk_image_boot_sector_signature_is_present(diskImage->bytes, 0u))
    {
        hyperdos_pc_disk_image_write_master_boot_record_bootstrap(diskImage->bytes);
        diskImage->dirty = 1u;
        if (hyperdos_pc_disk_image_write_binary_file(diskImage->path, diskImage->bytes, diskImage->byteCount))
        {
            diskImage->dirty = 0u;
        }
    }
    hyperdos_pc_disk_image_initialize_hard_disk_geometry(diskImage);
}

int hyperdos_pc_disk_image_load_hard_disk(hyperdos_pc_disk_image* diskImage, const char* path)
{
    memset(diskImage, 0, sizeof(*diskImage));
    if (!hyperdos_pc_disk_image_read_binary_file(path, &diskImage->bytes, &diskImage->byteCount))
    {
        return 0;
    }
    hyperdos_pc_disk_image_initialize_loaded_hard_disk(diskImage, path);
    return 1;
}
