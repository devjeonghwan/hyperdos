#include "pc_file_disk.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct hyperdos_win32_file_disk_context
{
    FILE*    file;
    uint64_t byteCount;
    uint16_t bytesPerSector;
    uint8_t  readOnly;
} hyperdos_win32_file_disk_context;

static int hyperdos_win32_file_disk_seek(FILE* file, uint64_t byteOffset)
{
    if (file == NULL || byteOffset > 0x7FFFFFFFFFFFFFFFull)
    {
        return 0;
    }
    return _fseeki64(file, (int64_t)byteOffset, SEEK_SET) == 0;
}

static int hyperdos_win32_file_disk_get_byte_count(FILE* file, uint64_t* byteCount)
{
    int64_t currentOffset = 0;
    int64_t endOffset     = 0;

    if (file == NULL || byteCount == NULL)
    {
        return 0;
    }
    currentOffset = _ftelli64(file);
    if (currentOffset < 0)
    {
        return 0;
    }
    if (_fseeki64(file, 0, SEEK_END) != 0)
    {
        return 0;
    }
    endOffset = _ftelli64(file);
    if (endOffset <= 0 || _fseeki64(file, currentOffset, SEEK_SET) != 0)
    {
        return 0;
    }
    *byteCount = (uint64_t)endOffset;
    return 1;
}

static hyperdos_pc_disk_transfer_result hyperdos_win32_file_disk_validate_transfer(
        const hyperdos_win32_file_disk_context* fileDiskContext,
        uint64_t                                logicalBlockAddress,
        uint16_t                                sectorCount,
        uint64_t*                               byteOffset,
        size_t*                                 byteCount)
{
    uint64_t transferByteOffset = 0u;
    uint64_t transferByteCount  = 0u;

    if (fileDiskContext == NULL || byteOffset == NULL || byteCount == NULL)
    {
        return HYPERDOS_PC_DISK_TRANSFER_HOST_ERROR;
    }
    if (logicalBlockAddress > UINT64_MAX / fileDiskContext->bytesPerSector)
    {
        return HYPERDOS_PC_DISK_TRANSFER_OUT_OF_RANGE;
    }
    transferByteOffset = logicalBlockAddress * fileDiskContext->bytesPerSector;
    transferByteCount  = (uint64_t)sectorCount * fileDiskContext->bytesPerSector;
    if (transferByteOffset > fileDiskContext->byteCount ||
        transferByteCount > fileDiskContext->byteCount - transferByteOffset)
    {
        return HYPERDOS_PC_DISK_TRANSFER_OUT_OF_RANGE;
    }
    if (transferByteCount > SIZE_MAX)
    {
        return HYPERDOS_PC_DISK_TRANSFER_HOST_ERROR;
    }
    *byteOffset = transferByteOffset;
    *byteCount  = (size_t)transferByteCount;
    return HYPERDOS_PC_DISK_TRANSFER_OK;
}

static hyperdos_pc_disk_transfer_result hyperdos_win32_file_disk_read_sectors(void*    userContext,
                                                                              uint64_t logicalBlockAddress,
                                                                              uint16_t sectorCount,
                                                                              uint8_t* destinationBytes)
{
    hyperdos_win32_file_disk_context* fileDiskContext = (hyperdos_win32_file_disk_context*)userContext;
    uint64_t                          byteOffset      = 0u;
    size_t                            byteCount       = 0u;
    hyperdos_pc_disk_transfer_result  transferResult  = hyperdos_win32_file_disk_validate_transfer(fileDiskContext,
                                                                                                 logicalBlockAddress,
                                                                                                 sectorCount,
                                                                                                 &byteOffset,
                                                                                                 &byteCount);

    if (destinationBytes == NULL)
    {
        return HYPERDOS_PC_DISK_TRANSFER_HOST_ERROR;
    }
    if (transferResult != HYPERDOS_PC_DISK_TRANSFER_OK)
    {
        return transferResult;
    }
    if (!hyperdos_win32_file_disk_seek(fileDiskContext->file, byteOffset))
    {
        return HYPERDOS_PC_DISK_TRANSFER_HOST_ERROR;
    }
    if (byteCount != 0u && fread(destinationBytes, 1u, byteCount, fileDiskContext->file) != byteCount)
    {
        return HYPERDOS_PC_DISK_TRANSFER_HOST_ERROR;
    }
    return HYPERDOS_PC_DISK_TRANSFER_OK;
}

static hyperdos_pc_disk_transfer_result hyperdos_win32_file_disk_write_sectors(void*          userContext,
                                                                               uint64_t       logicalBlockAddress,
                                                                               uint16_t       sectorCount,
                                                                               const uint8_t* sourceBytes)
{
    hyperdos_win32_file_disk_context* fileDiskContext = (hyperdos_win32_file_disk_context*)userContext;
    uint64_t                          byteOffset      = 0u;
    size_t                            byteCount       = 0u;
    hyperdos_pc_disk_transfer_result  transferResult  = hyperdos_win32_file_disk_validate_transfer(fileDiskContext,
                                                                                                 logicalBlockAddress,
                                                                                                 sectorCount,
                                                                                                 &byteOffset,
                                                                                                 &byteCount);

    if (sourceBytes == NULL)
    {
        return HYPERDOS_PC_DISK_TRANSFER_HOST_ERROR;
    }
    if (fileDiskContext != NULL && fileDiskContext->readOnly)
    {
        return HYPERDOS_PC_DISK_TRANSFER_WRITE_PROTECTED;
    }
    if (transferResult != HYPERDOS_PC_DISK_TRANSFER_OK)
    {
        return transferResult;
    }
    if (!hyperdos_win32_file_disk_seek(fileDiskContext->file, byteOffset))
    {
        return HYPERDOS_PC_DISK_TRANSFER_HOST_ERROR;
    }
    if (byteCount != 0u && fwrite(sourceBytes, 1u, byteCount, fileDiskContext->file) != byteCount)
    {
        return HYPERDOS_PC_DISK_TRANSFER_HOST_ERROR;
    }
    return HYPERDOS_PC_DISK_TRANSFER_OK;
}

static hyperdos_pc_disk_transfer_result hyperdos_win32_file_disk_flush(void* userContext)
{
    hyperdos_win32_file_disk_context* fileDiskContext = (hyperdos_win32_file_disk_context*)userContext;

    if (fileDiskContext == NULL || fileDiskContext->file == NULL)
    {
        return HYPERDOS_PC_DISK_TRANSFER_HOST_ERROR;
    }
    return fflush(fileDiskContext->file) == 0 ? HYPERDOS_PC_DISK_TRANSFER_OK : HYPERDOS_PC_DISK_TRANSFER_HOST_ERROR;
}

static void hyperdos_win32_file_disk_destroy(void* userContext)
{
    hyperdos_win32_file_disk_context* fileDiskContext = (hyperdos_win32_file_disk_context*)userContext;

    if (fileDiskContext == NULL)
    {
        return;
    }
    if (fileDiskContext->file != NULL)
    {
        fclose(fileDiskContext->file);
    }
    free(fileDiskContext);
}

static int hyperdos_win32_load_file_disk_image(hyperdos_pc_disk_image* diskImage, const char* path, uint8_t isHardDisk)
{
    static const hyperdos_pc_disk_operations fileDiskOperations = {hyperdos_win32_file_disk_read_sectors,
                                                                   hyperdos_win32_file_disk_write_sectors,
                                                                   hyperdos_win32_file_disk_flush,
                                                                   hyperdos_win32_file_disk_destroy};
    hyperdos_win32_file_disk_context*        fileDiskContext    = NULL;
    FILE*                                    file               = NULL;
    uint64_t                                 byteCount          = 0u;
    uint8_t                                  readOnly           = 0u;

    if (diskImage == NULL || path == NULL || path[0] == '\0')
    {
        return 0;
    }
    file = fopen(path, "rb+");
    if (file == NULL)
    {
        file     = fopen(path, "rb");
        readOnly = 1u;
    }
    if (file == NULL || !hyperdos_win32_file_disk_get_byte_count(file, &byteCount) ||
        byteCount % HYPERDOS_PC_DISK_IMAGE_DEFAULT_SECTOR_SIZE != 0u)
    {
        if (file != NULL)
        {
            fclose(file);
        }
        return 0;
    }
    fileDiskContext = (hyperdos_win32_file_disk_context*)calloc(1u, sizeof(*fileDiskContext));
    if (fileDiskContext == NULL)
    {
        fclose(file);
        return 0;
    }
    fileDiskContext->file           = file;
    fileDiskContext->byteCount      = byteCount;
    fileDiskContext->bytesPerSector = HYPERDOS_PC_DISK_IMAGE_DEFAULT_SECTOR_SIZE;
    fileDiskContext->readOnly       = readOnly;
    if (isHardDisk)
    {
        if (!hyperdos_pc_disk_image_initialize_hard_disk(diskImage,
                                                         path,
                                                         byteCount / HYPERDOS_PC_DISK_IMAGE_DEFAULT_SECTOR_SIZE,
                                                         HYPERDOS_PC_DISK_IMAGE_DEFAULT_SECTOR_SIZE,
                                                         readOnly,
                                                         &fileDiskOperations,
                                                         fileDiskContext))
        {
            hyperdos_win32_file_disk_destroy(fileDiskContext);
            return 0;
        }
    }
    else if (!hyperdos_pc_disk_image_initialize_floppy(diskImage,
                                                       path,
                                                       byteCount / HYPERDOS_PC_DISK_IMAGE_DEFAULT_SECTOR_SIZE,
                                                       HYPERDOS_PC_DISK_IMAGE_DEFAULT_SECTOR_SIZE,
                                                       readOnly,
                                                       &fileDiskOperations,
                                                       fileDiskContext))
    {
        hyperdos_win32_file_disk_destroy(fileDiskContext);
        return 0;
    }
    return 1;
}

int hyperdos_win32_load_floppy_file_disk_image(hyperdos_pc_disk_image* diskImage, const char* path)
{
    return hyperdos_win32_load_file_disk_image(diskImage, path, 0u);
}

int hyperdos_win32_load_fixed_file_disk_image(hyperdos_pc_disk_image* diskImage, const char* path)
{
    return hyperdos_win32_load_file_disk_image(diskImage, path, 1u);
}
