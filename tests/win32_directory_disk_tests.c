#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "pc_directory_disk.h"

enum
{
    TEST_SECTOR_BYTE_COUNT                            = 512u,
    TEST_DIRECTORY_ENTRY_BYTE_COUNT                   = 32u,
    TEST_PARTITION_TABLE_OFFSET                       = 446u,
    TEST_PARTITION_ENTRY_START_SECTOR                 = 8u,
    TEST_BIOS_PARAMETER_BYTES_PER_SECTOR              = 11u,
    TEST_BIOS_PARAMETER_SECTORS_PER_CLUSTER           = 13u,
    TEST_BIOS_PARAMETER_RESERVED_SECTORS              = 14u,
    TEST_BIOS_PARAMETER_FILE_ALLOCATION_TABLE_COUNT   = 16u,
    TEST_BIOS_PARAMETER_ROOT_ENTRY_COUNT              = 17u,
    TEST_BIOS_PARAMETER_FILE_ALLOCATION_TABLE_SECTORS = 22u,
    TEST_BIOS_PARAMETER_HIDDEN_SECTORS                = 28u,
    TEST_FILE_ALLOCATION_TABLE_CLUSTER_2_OFFSET       = 4u
};

static uint16_t test_read_word(const uint8_t* bytes, size_t byteOffset)
{
    return (uint16_t)(bytes[byteOffset] | ((uint16_t)bytes[byteOffset + 1u] << 8u));
}

static uint32_t test_read_double_word(const uint8_t* bytes, size_t byteOffset)
{
    return (uint32_t)bytes[byteOffset] | ((uint32_t)bytes[byteOffset + 1u] << 8u) |
           ((uint32_t)bytes[byteOffset + 2u] << 16u) | ((uint32_t)bytes[byteOffset + 3u] << 24u);
}

static void test_write_word(uint8_t* bytes, size_t byteOffset, uint16_t value)
{
    bytes[byteOffset]      = (uint8_t)(value & 0x00FFu);
    bytes[byteOffset + 1u] = (uint8_t)(value >> 8u);
}

static void test_write_double_word(uint8_t* bytes, size_t byteOffset, uint32_t value)
{
    bytes[byteOffset]      = (uint8_t)(value & 0x000000FFu);
    bytes[byteOffset + 1u] = (uint8_t)((value >> 8u) & 0x000000FFu);
    bytes[byteOffset + 2u] = (uint8_t)((value >> 16u) & 0x000000FFu);
    bytes[byteOffset + 3u] = (uint8_t)(value >> 24u);
}

static void test_join_path(char* destination, size_t destinationSize, const char* parentPath, const char* childName)
{
    size_t parentLength = strlen(parentPath);
    int    needsSlash   = parentLength != 0u && parentPath[parentLength - 1u] != '\\';

    assert(parentLength + (needsSlash ? 1u : 0u) + strlen(childName) + 1u <= destinationSize);
    memcpy(destination, parentPath, parentLength);
    if (needsSlash)
    {
        destination[parentLength] = '\\';
        ++parentLength;
    }
    strcpy_s(destination + parentLength, destinationSize - parentLength, childName);
}

static void test_delete_directory_tree(const char* directoryPath)
{
    char             searchPath[MAX_PATH];
    WIN32_FIND_DATAA findData;
    HANDLE           findHandle = INVALID_HANDLE_VALUE;

    test_join_path(searchPath, sizeof(searchPath), directoryPath, "*");
    findHandle = FindFirstFileA(searchPath, &findData);
    if (findHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            char childPath[MAX_PATH];

            if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
            {
                continue;
            }
            test_join_path(childPath, sizeof(childPath), directoryPath, findData.cFileName);
            SetFileAttributesA(childPath, FILE_ATTRIBUTE_NORMAL);
            if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0u)
            {
                test_delete_directory_tree(childPath);
            }
            else
            {
                DeleteFileA(childPath);
            }
        }
        while (FindNextFileA(findHandle, &findData) != 0);
        FindClose(findHandle);
    }
    SetFileAttributesA(directoryPath, FILE_ATTRIBUTE_NORMAL);
    RemoveDirectoryA(directoryPath);
}

static void test_read_file_bytes(const char* path, uint8_t* bytes, size_t byteCount)
{
    FILE* hostFile = NULL;

    assert(fopen_s(&hostFile, path, "rb") == 0);
    assert(hostFile != NULL);
    assert(fread(bytes, 1u, byteCount, hostFile) == byteCount);
    assert(fgetc(hostFile) == EOF);
    assert(fclose(hostFile) == 0);
}

static void test_directory_backed_fixed_disk_writes_back_created_and_deleted_files(void)
{
    static const uint8_t   fileContents[] = {'w', 'r', 'i', 't', 't', 'e', 'n'};
    char                   temporaryPath[MAX_PATH];
    char                   directoryPath[MAX_PATH];
    char                   writtenFilePath[MAX_PATH];
    hyperdos_pc_disk_image diskImage;
    uint8_t                masterBootRecord[TEST_SECTOR_BYTE_COUNT];
    uint8_t                bootSector[TEST_SECTOR_BYTE_COUNT];
    uint8_t                fileAllocationTableSector[TEST_SECTOR_BYTE_COUNT];
    uint8_t                rootDirectorySector[TEST_SECTOR_BYTE_COUNT];
    uint8_t                fileSector[TEST_SECTOR_BYTE_COUNT];
    uint32_t               partitionStartSector            = 0u;
    uint16_t               bytesPerSector                  = 0u;
    uint8_t                sectorsPerCluster               = 0u;
    uint16_t               reservedSectorCount             = 0u;
    uint8_t                fileAllocationTableCount        = 0u;
    uint16_t               rootDirectoryEntryCount         = 0u;
    uint16_t               sectorsPerFileAllocationTable   = 0u;
    uint32_t               hiddenSectorCount               = 0u;
    uint32_t               fileAllocationTableSectorNumber = 0u;
    uint32_t               rootDirectorySectorNumber       = 0u;
    uint32_t               rootDirectorySectorCount        = 0u;
    uint32_t               firstDataSector                 = 0u;
    uint8_t                readBackBytes[sizeof(fileContents)];

    memset(&diskImage, 0, sizeof(diskImage));
    assert(GetTempPathA(sizeof(temporaryPath), temporaryPath) != 0u);
    assert(GetTempFileNameA(temporaryPath, "hdd", 0u, directoryPath) != 0u);
    assert(DeleteFileA(directoryPath) != 0);
    assert(CreateDirectoryA(directoryPath, NULL) != 0);
    test_join_path(writtenFilePath, sizeof(writtenFilePath), directoryPath, "WRITTEN.TXT");

    assert(hyperdos_win32_load_fixed_directory_disk_image(&diskImage, directoryPath));
    assert(diskImage.readOnly == 0u);
    assert(hyperdos_pc_disk_image_read_sectors(&diskImage, 0u, 1u, masterBootRecord) == HYPERDOS_PC_DISK_TRANSFER_OK);
    partitionStartSector = test_read_double_word(masterBootRecord,
                                                 TEST_PARTITION_TABLE_OFFSET + TEST_PARTITION_ENTRY_START_SECTOR);
    assert(hyperdos_pc_disk_image_read_sectors(&diskImage, partitionStartSector, 1u, bootSector) ==
           HYPERDOS_PC_DISK_TRANSFER_OK);

    bytesPerSector                = test_read_word(bootSector, TEST_BIOS_PARAMETER_BYTES_PER_SECTOR);
    sectorsPerCluster             = bootSector[TEST_BIOS_PARAMETER_SECTORS_PER_CLUSTER];
    reservedSectorCount           = test_read_word(bootSector, TEST_BIOS_PARAMETER_RESERVED_SECTORS);
    fileAllocationTableCount      = bootSector[TEST_BIOS_PARAMETER_FILE_ALLOCATION_TABLE_COUNT];
    rootDirectoryEntryCount       = test_read_word(bootSector, TEST_BIOS_PARAMETER_ROOT_ENTRY_COUNT);
    sectorsPerFileAllocationTable = test_read_word(bootSector, TEST_BIOS_PARAMETER_FILE_ALLOCATION_TABLE_SECTORS);
    hiddenSectorCount             = test_read_double_word(bootSector, TEST_BIOS_PARAMETER_HIDDEN_SECTORS);
    assert(bytesPerSector == TEST_SECTOR_BYTE_COUNT);
    assert(sectorsPerCluster != 0u);

    fileAllocationTableSectorNumber = hiddenSectorCount + reservedSectorCount;
    rootDirectorySectorNumber       = fileAllocationTableSectorNumber +
                                (uint32_t)fileAllocationTableCount * sectorsPerFileAllocationTable;
    rootDirectorySectorCount = ((uint32_t)rootDirectoryEntryCount * TEST_DIRECTORY_ENTRY_BYTE_COUNT +
                                TEST_SECTOR_BYTE_COUNT - 1u) /
                               TEST_SECTOR_BYTE_COUNT;
    firstDataSector = rootDirectorySectorNumber + rootDirectorySectorCount;

    memset(fileSector, 0, sizeof(fileSector));
    memcpy(fileSector, fileContents, sizeof(fileContents));
    assert(hyperdos_pc_disk_image_write_sectors(&diskImage, firstDataSector, 1u, fileSector) ==
           HYPERDOS_PC_DISK_TRANSFER_OK);

    assert(hyperdos_pc_disk_image_read_sectors(&diskImage,
                                               fileAllocationTableSectorNumber,
                                               1u,
                                               fileAllocationTableSector) == HYPERDOS_PC_DISK_TRANSFER_OK);
    test_write_word(fileAllocationTableSector, TEST_FILE_ALLOCATION_TABLE_CLUSTER_2_OFFSET, 0xFFFFu);
    assert(hyperdos_pc_disk_image_write_sectors(&diskImage,
                                                fileAllocationTableSectorNumber,
                                                1u,
                                                fileAllocationTableSector) == HYPERDOS_PC_DISK_TRANSFER_OK);
    assert(hyperdos_pc_disk_image_write_sectors(&diskImage,
                                                fileAllocationTableSectorNumber + sectorsPerFileAllocationTable,
                                                1u,
                                                fileAllocationTableSector) == HYPERDOS_PC_DISK_TRANSFER_OK);

    assert(hyperdos_pc_disk_image_read_sectors(&diskImage, rootDirectorySectorNumber, 1u, rootDirectorySector) ==
           HYPERDOS_PC_DISK_TRANSFER_OK);
    memset(rootDirectorySector, 0, TEST_DIRECTORY_ENTRY_BYTE_COUNT);
    memcpy(rootDirectorySector, "WRITTEN TXT", 11u);
    rootDirectorySector[11u] = 0x20u;
    test_write_word(rootDirectorySector, 22u, 0u);
    test_write_word(rootDirectorySector, 24u, 0x5A21u);
    test_write_word(rootDirectorySector, 26u, 2u);
    test_write_double_word(rootDirectorySector, 28u, (uint32_t)sizeof(fileContents));
    assert(hyperdos_pc_disk_image_write_sectors(&diskImage, rootDirectorySectorNumber, 1u, rootDirectorySector) ==
           HYPERDOS_PC_DISK_TRANSFER_OK);
    assert(hyperdos_pc_disk_image_flush(&diskImage));
    assert(GetFileAttributesA(writtenFilePath) != INVALID_FILE_ATTRIBUTES);
    test_read_file_bytes(writtenFilePath, readBackBytes, sizeof(readBackBytes));
    assert(memcmp(readBackBytes, fileContents, sizeof(fileContents)) == 0);

    rootDirectorySector[0] = 0xE5u;
    assert(hyperdos_pc_disk_image_write_sectors(&diskImage, rootDirectorySectorNumber, 1u, rootDirectorySector) ==
           HYPERDOS_PC_DISK_TRANSFER_OK);
    test_write_word(fileAllocationTableSector, TEST_FILE_ALLOCATION_TABLE_CLUSTER_2_OFFSET, 0u);
    assert(hyperdos_pc_disk_image_write_sectors(&diskImage,
                                                fileAllocationTableSectorNumber,
                                                1u,
                                                fileAllocationTableSector) == HYPERDOS_PC_DISK_TRANSFER_OK);
    assert(hyperdos_pc_disk_image_write_sectors(&diskImage,
                                                fileAllocationTableSectorNumber + sectorsPerFileAllocationTable,
                                                1u,
                                                fileAllocationTableSector) == HYPERDOS_PC_DISK_TRANSFER_OK);
    assert(hyperdos_pc_disk_image_flush(&diskImage));
    assert(GetFileAttributesA(writtenFilePath) == INVALID_FILE_ATTRIBUTES);

    hyperdos_pc_disk_image_free(&diskImage);
    test_delete_directory_tree(directoryPath);
}

int main(void)
{
    test_directory_backed_fixed_disk_writes_back_created_and_deleted_files();
    return 0;
}
