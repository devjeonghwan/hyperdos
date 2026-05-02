#include "hyperdos/pc_disk_image.h"

#include <stdlib.h>
#include <string.h>

enum
{
    HYPERDOS_PC_DISK_IMAGE_FLOPPY_DRIVE_NUMBER                       = 0x00u,
    HYPERDOS_PC_DISK_IMAGE_HARD_DISK_DRIVE_NUMBER                    = 0x80u,
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

typedef struct hyperdos_pc_memory_disk_context
{
    uint8_t* bytes;
    size_t   byteCount;
    uint16_t bytesPerSector;
    uint8_t  readOnly;
} hyperdos_pc_memory_disk_context;

static void hyperdos_pc_disk_image_copy_string_to_buffer(char* destination, size_t destinationSize, const char* source)
{
    size_t sourceLength = 0u;

    if (destination == NULL || destinationSize == 0u)
    {
        return;
    }
    if (source == NULL)
    {
        destination[0] = '\0';
        return;
    }
    sourceLength = strlen(source);
    if (sourceLength >= destinationSize)
    {
        sourceLength = destinationSize - 1u;
    }
    memcpy(destination, source, sourceLength);
    destination[sourceLength] = '\0';
}

static uint16_t hyperdos_pc_disk_image_read_little_endian_word(const uint8_t* bytes, size_t offset)
{
    return (uint16_t)(bytes[offset] | ((uint16_t)bytes[offset + 1u] << 8u));
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

static int hyperdos_pc_disk_image_read_first_sector(const hyperdos_pc_disk_image* diskImage, uint8_t* sectorBytes)
{
    if (diskImage == NULL || sectorBytes == NULL ||
        diskImage->bytesPerSector != HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_SIZE)
    {
        return 0;
    }
    return hyperdos_pc_disk_image_read_sectors(diskImage, 0u, 1u, sectorBytes) == HYPERDOS_PC_DISK_TRANSFER_OK;
}

static void hyperdos_pc_disk_image_initialize_floppy_geometry(hyperdos_pc_disk_image* diskImage)
{
    uint8_t  sectorBytes[HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_SIZE];
    uint16_t totalSectors = 0u;

    diskImage->bytesPerSector  = HYPERDOS_PC_DISK_IMAGE_BIOS_DEFAULT_BYTES_PER_SECTOR;
    diskImage->sectorsPerTrack = HYPERDOS_PC_DISK_IMAGE_BIOS_DEFAULT_SECTORS_PER_TRACK;
    diskImage->headCount       = HYPERDOS_PC_DISK_IMAGE_BIOS_DEFAULT_HEAD_COUNT;
    diskImage->cylinderCount   = HYPERDOS_PC_DISK_IMAGE_BIOS_DEFAULT_CYLINDER_COUNT;

    if (hyperdos_pc_disk_image_read_first_sector(diskImage, sectorBytes))
    {
        uint16_t bytesPerSector = hyperdos_pc_disk_image_read_little_endian_word(
                sectorBytes,
                HYPERDOS_PC_DISK_IMAGE_BIOS_PARAMETER_BYTES_PER_SECTOR_OFFSET);
        uint16_t sectorsPerTrack = hyperdos_pc_disk_image_read_little_endian_word(
                sectorBytes,
                HYPERDOS_PC_DISK_IMAGE_BIOS_PARAMETER_SECTORS_PER_TRACK_OFFSET);
        uint16_t headCount =
                hyperdos_pc_disk_image_read_little_endian_word(sectorBytes,
                                                               HYPERDOS_PC_DISK_IMAGE_BIOS_PARAMETER_HEAD_COUNT_OFFSET);
        totalSectors = hyperdos_pc_disk_image_read_little_endian_word(
                sectorBytes,
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
    if (totalSectors == 0u && diskImage->sectorCount <= 0xFFFFu)
    {
        totalSectors = (uint16_t)diskImage->sectorCount;
    }
    if (totalSectors != 0u && diskImage->sectorsPerTrack != 0u && diskImage->headCount != 0u)
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
    uint8_t  sectorBytes[HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_SIZE];
    size_t   partitionEntryIndex     = 0u;
    uint16_t detectedHeadCount       = 0u;
    uint16_t detectedSectorsPerTrack = 0u;
    uint16_t detectedCylinderCount   = 0u;

    if (!hyperdos_pc_disk_image_read_first_sector(diskImage, sectorBytes) ||
        !hyperdos_pc_disk_image_boot_sector_signature_is_present(sectorBytes, 0u))
    {
        return 0;
    }

    for (partitionEntryIndex = 0u; partitionEntryIndex < PARTITION_TABLE_ENTRY_COUNT; ++partitionEntryIndex)
    {
        size_t partitionEntryOffset = HYPERDOS_PC_DISK_IMAGE_MASTER_BOOT_RECORD_PARTITION_TABLE_OFFSET +
                                      partitionEntryIndex * PARTITION_TABLE_ENTRY_BYTE_COUNT;
        uint8_t  partitionType             = sectorBytes[partitionEntryOffset + PARTITION_TABLE_ENTRY_TYPE_OFFSET];
        uint32_t partitionTotalSectorCount = hyperdos_pc_disk_image_read_little_endian_double_word(
                sectorBytes,
                partitionEntryOffset + PARTITION_TABLE_ENTRY_TOTAL_SECTOR_COUNT_OFFSET);
        uint8_t  startHead   = sectorBytes[partitionEntryOffset + PARTITION_TABLE_ENTRY_START_HEAD_OFFSET];
        uint8_t  startSector = (uint8_t)(sectorBytes[partitionEntryOffset + PARTITION_TABLE_ENTRY_START_SECTOR_OFFSET] &
                                        PARTITION_CHS_SECTOR_MASK);
        uint16_t startCylinder = hyperdos_pc_disk_image_decode_partition_cylinder(
                sectorBytes[partitionEntryOffset + PARTITION_TABLE_ENTRY_START_SECTOR_OFFSET],
                sectorBytes[partitionEntryOffset + PARTITION_TABLE_ENTRY_START_CYLINDER_OFFSET]);
        uint8_t  endHead     = sectorBytes[partitionEntryOffset + PARTITION_TABLE_ENTRY_END_HEAD_OFFSET];
        uint8_t  endSector   = (uint8_t)(sectorBytes[partitionEntryOffset + PARTITION_TABLE_ENTRY_END_SECTOR_OFFSET] &
                                      PARTITION_CHS_SECTOR_MASK);
        uint16_t endCylinder = hyperdos_pc_disk_image_decode_partition_cylinder(
                sectorBytes[partitionEntryOffset + PARTITION_TABLE_ENTRY_END_SECTOR_OFFSET],
                sectorBytes[partitionEntryOffset + PARTITION_TABLE_ENTRY_END_CYLINDER_OFFSET]);

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
        uint64_t sectorsPerCylinder = (uint64_t)detectedHeadCount * detectedSectorsPerTrack;
        uint64_t imageCylinderCount = sectorsPerCylinder != 0u ? diskImage->sectorCount / sectorsPerCylinder : 0u;

        detectedCylinderCount = imageCylinderCount > 0xFFFFu ? 0xFFFFu : (uint16_t)imageCylinderCount;
    }
    else
    {
        uint64_t sectorsPerCylinder = (uint64_t)detectedHeadCount * detectedSectorsPerTrack;
        uint64_t imageCylinderCount = sectorsPerCylinder != 0u ? diskImage->sectorCount / sectorsPerCylinder : 0u;

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
    uint64_t sectorsPerCylinder = HYPERDOS_PC_DISK_IMAGE_HARD_DISK_SECTORS_PER_TRACK *
                                  HYPERDOS_PC_DISK_IMAGE_HARD_DISK_HEAD_COUNT;
    uint64_t cylinderCount = diskImage->sectorCount / sectorsPerCylinder;

    if (hyperdos_pc_disk_image_initialize_hard_disk_geometry_from_partition_table(diskImage))
    {
        return;
    }

    diskImage->bytesPerSector  = HYPERDOS_PC_DISK_IMAGE_HARD_DISK_BYTES_PER_SECTOR;
    diskImage->sectorsPerTrack = HYPERDOS_PC_DISK_IMAGE_HARD_DISK_SECTORS_PER_TRACK;
    diskImage->headCount       = HYPERDOS_PC_DISK_IMAGE_HARD_DISK_HEAD_COUNT;
    diskImage->cylinderCount   = cylinderCount == 0u ? HYPERDOS_PC_DISK_IMAGE_HARD_DISK_CYLINDER_COUNT
                                                     : (cylinderCount > 0xFFFFu ? 0xFFFFu : (uint16_t)cylinderCount);
}

static int hyperdos_pc_disk_image_master_boot_record_bootstrap_is_empty(const uint8_t* bytes)
{
    size_t byteOffset = 0u;

    for (byteOffset = 0u; byteOffset < HYPERDOS_PC_DISK_IMAGE_MASTER_BOOT_RECORD_PARTITION_TABLE_OFFSET; ++byteOffset)
    {
        if (bytes[byteOffset] != 0u)
        {
            return 0;
        }
    }
    return 1;
}

static int hyperdos_pc_disk_image_master_boot_record_bootstrap_is_hyperdos_x86_message_stub(const uint8_t* bytes)
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

static void hyperdos_pc_disk_image_install_master_boot_record_bootstrap(hyperdos_pc_disk_image* diskImage)
{
    uint8_t sectorBytes[HYPERDOS_PC_DISK_IMAGE_BOOT_SECTOR_SIZE];

    if (diskImage == NULL || diskImage->readOnly || !hyperdos_pc_disk_image_read_first_sector(diskImage, sectorBytes))
    {
        return;
    }
    if ((hyperdos_pc_disk_image_master_boot_record_bootstrap_is_empty(sectorBytes) ||
         hyperdos_pc_disk_image_master_boot_record_bootstrap_is_hyperdos_x86_message_stub(sectorBytes)) &&
        hyperdos_pc_disk_image_boot_sector_signature_is_present(sectorBytes, 0u))
    {
        hyperdos_pc_disk_image_write_master_boot_record_bootstrap(sectorBytes);
        (void)hyperdos_pc_disk_image_write_sectors(diskImage, 0u, 1u, sectorBytes);
        (void)hyperdos_pc_disk_image_flush(diskImage);
    }
}

static int hyperdos_pc_disk_image_initialize(hyperdos_pc_disk_image*            diskImage,
                                             const char*                        path,
                                             uint64_t                           sectorCount,
                                             uint16_t                           bytesPerSector,
                                             uint8_t                            isHardDisk,
                                             uint8_t                            readOnly,
                                             const hyperdos_pc_disk_operations* operations,
                                             void*                              operationUserContext)
{
    if (diskImage == NULL || operations == NULL || operations->readSectors == NULL || bytesPerSector == 0u ||
        sectorCount == 0u)
    {
        return 0;
    }
    memset(diskImage, 0, sizeof(*diskImage));
    hyperdos_pc_disk_image_copy_string_to_buffer(diskImage->path, sizeof(diskImage->path), path);
    diskImage->byteCount            = sectorCount > (uint64_t)(SIZE_MAX / bytesPerSector) ? SIZE_MAX
                                                                                          : (size_t)(sectorCount * bytesPerSector);
    diskImage->sectorCount          = sectorCount;
    diskImage->bytesPerSector       = bytesPerSector;
    diskImage->driveNumber          = isHardDisk ? HYPERDOS_PC_DISK_IMAGE_HARD_DISK_DRIVE_NUMBER
                                                 : HYPERDOS_PC_DISK_IMAGE_FLOPPY_DRIVE_NUMBER;
    diskImage->inserted             = 1u;
    diskImage->isHardDisk           = isHardDisk;
    diskImage->readOnly             = readOnly;
    diskImage->operations           = *operations;
    diskImage->operationUserContext = operationUserContext;
    if (isHardDisk)
    {
        hyperdos_pc_disk_image_initialize_hard_disk_geometry(diskImage);
        hyperdos_pc_disk_image_install_master_boot_record_bootstrap(diskImage);
    }
    else
    {
        hyperdos_pc_disk_image_initialize_floppy_geometry(diskImage);
    }
    return 1;
}

static hyperdos_pc_disk_transfer_result hyperdos_pc_memory_disk_read_sectors(void*    userContext,
                                                                             uint64_t logicalBlockAddress,
                                                                             uint16_t sectorCount,
                                                                             uint8_t* destinationBytes)
{
    hyperdos_pc_memory_disk_context* memoryDiskContext = (hyperdos_pc_memory_disk_context*)userContext;
    uint64_t                         byteOffset        = 0u;
    uint64_t                         byteCount         = 0u;

    if (memoryDiskContext == NULL || destinationBytes == NULL)
    {
        return HYPERDOS_PC_DISK_TRANSFER_HOST_ERROR;
    }
    if (logicalBlockAddress > UINT64_MAX / memoryDiskContext->bytesPerSector)
    {
        return HYPERDOS_PC_DISK_TRANSFER_OUT_OF_RANGE;
    }
    byteOffset = logicalBlockAddress * memoryDiskContext->bytesPerSector;
    byteCount  = (uint64_t)sectorCount * memoryDiskContext->bytesPerSector;
    if (byteOffset > memoryDiskContext->byteCount || byteCount > memoryDiskContext->byteCount - byteOffset)
    {
        return HYPERDOS_PC_DISK_TRANSFER_OUT_OF_RANGE;
    }
    memcpy(destinationBytes, memoryDiskContext->bytes + byteOffset, (size_t)byteCount);
    return HYPERDOS_PC_DISK_TRANSFER_OK;
}

static hyperdos_pc_disk_transfer_result hyperdos_pc_memory_disk_write_sectors(void*          userContext,
                                                                              uint64_t       logicalBlockAddress,
                                                                              uint16_t       sectorCount,
                                                                              const uint8_t* sourceBytes)
{
    hyperdos_pc_memory_disk_context* memoryDiskContext = (hyperdos_pc_memory_disk_context*)userContext;
    uint64_t                         byteOffset        = 0u;
    uint64_t                         byteCount         = 0u;

    if (memoryDiskContext == NULL || sourceBytes == NULL)
    {
        return HYPERDOS_PC_DISK_TRANSFER_HOST_ERROR;
    }
    if (memoryDiskContext->readOnly)
    {
        return HYPERDOS_PC_DISK_TRANSFER_WRITE_PROTECTED;
    }
    if (logicalBlockAddress > UINT64_MAX / memoryDiskContext->bytesPerSector)
    {
        return HYPERDOS_PC_DISK_TRANSFER_OUT_OF_RANGE;
    }
    byteOffset = logicalBlockAddress * memoryDiskContext->bytesPerSector;
    byteCount  = (uint64_t)sectorCount * memoryDiskContext->bytesPerSector;
    if (byteOffset > memoryDiskContext->byteCount || byteCount > memoryDiskContext->byteCount - byteOffset)
    {
        return HYPERDOS_PC_DISK_TRANSFER_OUT_OF_RANGE;
    }
    memcpy(memoryDiskContext->bytes + byteOffset, sourceBytes, (size_t)byteCount);
    return HYPERDOS_PC_DISK_TRANSFER_OK;
}

static hyperdos_pc_disk_transfer_result hyperdos_pc_memory_disk_flush(void* userContext)
{
    (void)userContext;
    return HYPERDOS_PC_DISK_TRANSFER_OK;
}

static void hyperdos_pc_memory_disk_destroy(void* userContext)
{
    hyperdos_pc_memory_disk_context* memoryDiskContext = (hyperdos_pc_memory_disk_context*)userContext;

    if (memoryDiskContext == NULL)
    {
        return;
    }
    free(memoryDiskContext->bytes);
    free(memoryDiskContext);
}

static int hyperdos_pc_disk_image_initialize_memory(hyperdos_pc_disk_image* diskImage,
                                                    const char*             path,
                                                    uint8_t*                bytes,
                                                    size_t                  byteCount,
                                                    uint8_t                 isHardDisk,
                                                    uint8_t                 readOnly)
{
    static const hyperdos_pc_disk_operations memoryDiskOperations = {hyperdos_pc_memory_disk_read_sectors,
                                                                     hyperdos_pc_memory_disk_write_sectors,
                                                                     hyperdos_pc_memory_disk_flush,
                                                                     hyperdos_pc_memory_disk_destroy};
    hyperdos_pc_memory_disk_context*         memoryDiskContext    = NULL;

    if (bytes == NULL || byteCount == 0u || byteCount % HYPERDOS_PC_DISK_IMAGE_DEFAULT_SECTOR_SIZE != 0u)
    {
        free(bytes);
        return 0;
    }
    memoryDiskContext = (hyperdos_pc_memory_disk_context*)calloc(1u, sizeof(*memoryDiskContext));
    if (memoryDiskContext == NULL)
    {
        free(bytes);
        return 0;
    }
    memoryDiskContext->bytes          = bytes;
    memoryDiskContext->byteCount      = byteCount;
    memoryDiskContext->bytesPerSector = HYPERDOS_PC_DISK_IMAGE_DEFAULT_SECTOR_SIZE;
    memoryDiskContext->readOnly       = readOnly;
    if (!hyperdos_pc_disk_image_initialize(diskImage,
                                           path,
                                           byteCount / HYPERDOS_PC_DISK_IMAGE_DEFAULT_SECTOR_SIZE,
                                           HYPERDOS_PC_DISK_IMAGE_DEFAULT_SECTOR_SIZE,
                                           isHardDisk,
                                           readOnly,
                                           &memoryDiskOperations,
                                           memoryDiskContext))
    {
        hyperdos_pc_memory_disk_destroy(memoryDiskContext);
        return 0;
    }
    return 1;
}

int hyperdos_pc_disk_image_initialize_floppy(hyperdos_pc_disk_image*            diskImage,
                                             const char*                        path,
                                             uint64_t                           sectorCount,
                                             uint16_t                           bytesPerSector,
                                             uint8_t                            readOnly,
                                             const hyperdos_pc_disk_operations* operations,
                                             void*                              operationUserContext)
{
    return hyperdos_pc_disk_image_initialize(diskImage,
                                             path,
                                             sectorCount,
                                             bytesPerSector,
                                             0u,
                                             readOnly,
                                             operations,
                                             operationUserContext);
}

int hyperdos_pc_disk_image_initialize_hard_disk(hyperdos_pc_disk_image*            diskImage,
                                                const char*                        path,
                                                uint64_t                           sectorCount,
                                                uint16_t                           bytesPerSector,
                                                uint8_t                            readOnly,
                                                const hyperdos_pc_disk_operations* operations,
                                                void*                              operationUserContext)
{
    return hyperdos_pc_disk_image_initialize(diskImage,
                                             path,
                                             sectorCount,
                                             bytesPerSector,
                                             1u,
                                             readOnly,
                                             operations,
                                             operationUserContext);
}

int hyperdos_pc_disk_image_initialize_memory_floppy(hyperdos_pc_disk_image* diskImage,
                                                    const char*             path,
                                                    uint8_t*                bytes,
                                                    size_t                  byteCount,
                                                    uint8_t                 readOnly)
{
    return hyperdos_pc_disk_image_initialize_memory(diskImage, path, bytes, byteCount, 0u, readOnly);
}

int hyperdos_pc_disk_image_initialize_memory_hard_disk(hyperdos_pc_disk_image* diskImage,
                                                       const char*             path,
                                                       uint8_t*                bytes,
                                                       size_t                  byteCount,
                                                       uint8_t                 readOnly)
{
    return hyperdos_pc_disk_image_initialize_memory(diskImage, path, bytes, byteCount, 1u, readOnly);
}

void hyperdos_pc_disk_image_free(hyperdos_pc_disk_image* diskImage)
{
    if (diskImage == NULL)
    {
        return;
    }
    if (diskImage->operations.destroy != NULL)
    {
        diskImage->operations.destroy(diskImage->operationUserContext);
    }
    memset(diskImage, 0, sizeof(*diskImage));
}

int hyperdos_pc_disk_image_flush(hyperdos_pc_disk_image* diskImage)
{
    if (diskImage == NULL || !diskImage->dirty)
    {
        return 1;
    }
    if (diskImage->operations.flush == NULL)
    {
        diskImage->dirty = 0u;
        return 1;
    }
    if (diskImage->operations.flush(diskImage->operationUserContext) != HYPERDOS_PC_DISK_TRANSFER_OK)
    {
        return 0;
    }
    diskImage->dirty = 0u;
    return 1;
}

hyperdos_pc_disk_transfer_result hyperdos_pc_disk_image_read_sectors(const hyperdos_pc_disk_image* diskImage,
                                                                     uint64_t                      logicalBlockAddress,
                                                                     uint16_t                      sectorCount,
                                                                     uint8_t*                      destinationBytes)
{
    if (diskImage == NULL || destinationBytes == NULL || diskImage->operations.readSectors == NULL || sectorCount == 0u)
    {
        return HYPERDOS_PC_DISK_TRANSFER_HOST_ERROR;
    }
    if ((uint64_t)sectorCount > diskImage->sectorCount || logicalBlockAddress > diskImage->sectorCount - sectorCount)
    {
        return HYPERDOS_PC_DISK_TRANSFER_OUT_OF_RANGE;
    }
    return diskImage->operations.readSectors(diskImage->operationUserContext,
                                             logicalBlockAddress,
                                             sectorCount,
                                             destinationBytes);
}

hyperdos_pc_disk_transfer_result hyperdos_pc_disk_image_write_sectors(hyperdos_pc_disk_image* diskImage,
                                                                      uint64_t                logicalBlockAddress,
                                                                      uint16_t                sectorCount,
                                                                      const uint8_t*          sourceBytes)
{
    hyperdos_pc_disk_transfer_result transferResult = HYPERDOS_PC_DISK_TRANSFER_OK;

    if (diskImage == NULL || sourceBytes == NULL || sectorCount == 0u)
    {
        return HYPERDOS_PC_DISK_TRANSFER_HOST_ERROR;
    }
    if (diskImage->readOnly)
    {
        return HYPERDOS_PC_DISK_TRANSFER_WRITE_PROTECTED;
    }
    if (diskImage->operations.writeSectors == NULL)
    {
        return HYPERDOS_PC_DISK_TRANSFER_HOST_ERROR;
    }
    if ((uint64_t)sectorCount > diskImage->sectorCount || logicalBlockAddress > diskImage->sectorCount - sectorCount)
    {
        return HYPERDOS_PC_DISK_TRANSFER_OUT_OF_RANGE;
    }
    transferResult = diskImage->operations.writeSectors(diskImage->operationUserContext,
                                                        logicalBlockAddress,
                                                        sectorCount,
                                                        sourceBytes);
    if (transferResult == HYPERDOS_PC_DISK_TRANSFER_OK)
    {
        diskImage->dirty = 1u;
    }
    return transferResult;
}
