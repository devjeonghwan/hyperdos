#define WIN32_LEAN_AND_MEAN

#include "pc_directory_disk.h"

#include <windows.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
    HYPERDOS_WIN32_DIRECTORY_DISK_PATH_CAPACITY               = HYPERDOS_PC_DISK_IMAGE_PATH_CAPACITY,
    HYPERDOS_WIN32_DIRECTORY_DISK_SHORT_NAME_BYTE_COUNT       = 11u,
    HYPERDOS_WIN32_DIRECTORY_DISK_DIRECTORY_ENTRY_SIZE        = 32u,
    HYPERDOS_WIN32_DIRECTORY_DISK_BYTES_PER_SECTOR            = 512u,
    HYPERDOS_WIN32_DIRECTORY_DISK_RESERVED_SECTOR_COUNT       = 1u,
    HYPERDOS_WIN32_DIRECTORY_DISK_FILE_ALLOCATION_TABLE_COUNT = 2u,
    HYPERDOS_WIN32_DIRECTORY_DISK_FLOPPY_ROOT_ENTRY_COUNT     = 224u,
    HYPERDOS_WIN32_DIRECTORY_DISK_FIXED_ROOT_ENTRY_COUNT      = 512u,
    HYPERDOS_WIN32_DIRECTORY_DISK_DOS_ATTRIBUTE_READ_ONLY     = 0x01u,
    HYPERDOS_WIN32_DIRECTORY_DISK_DOS_ATTRIBUTE_DIRECTORY     = 0x10u,
    HYPERDOS_WIN32_DIRECTORY_DISK_DOS_ATTRIBUTE_ARCHIVE       = 0x20u,
    HYPERDOS_WIN32_DIRECTORY_DISK_FAT12_END_OF_CHAIN          = 0x0FFFu,
    HYPERDOS_WIN32_DIRECTORY_DISK_FAT16_END_OF_CHAIN          = 0xFFFFu,
    HYPERDOS_WIN32_DIRECTORY_DISK_FIXED_HIDDEN_SECTORS        = 63u,
    HYPERDOS_WIN32_DIRECTORY_DISK_FIXED_SECTORS_PER_TRACK     = 63u,
    HYPERDOS_WIN32_DIRECTORY_DISK_FIXED_HEAD_COUNT            = 16u,
    HYPERDOS_WIN32_DIRECTORY_DISK_FIXED_SECTORS_PER_CLUSTER   = 4u,
    HYPERDOS_WIN32_DIRECTORY_DISK_FIXED_MINIMUM_TOTAL_SECTORS = 66528u,
    HYPERDOS_WIN32_DIRECTORY_DISK_PARTITION_TABLE_OFFSET      = 0x01BEu,
    HYPERDOS_WIN32_DIRECTORY_DISK_BOOT_SIGNATURE_OFFSET       = 0x01FEu
};

typedef enum hyperdos_win32_directory_disk_file_allocation_table_type
{
    HYPERDOS_WIN32_DIRECTORY_DISK_FILE_ALLOCATION_TABLE_TYPE_12 = 0,
    HYPERDOS_WIN32_DIRECTORY_DISK_FILE_ALLOCATION_TABLE_TYPE_16
} hyperdos_win32_directory_disk_file_allocation_table_type;

typedef struct hyperdos_win32_directory_disk_format
{
    uint32_t                                                 totalSectorCount;
    uint32_t                                                 hiddenSectorCount;
    uint16_t                                                 rootDirectoryEntryCount;
    uint16_t                                                 sectorsPerFileAllocationTable;
    uint16_t                                                 sectorsPerTrack;
    uint16_t                                                 headCount;
    uint8_t                                                  sectorsPerCluster;
    uint8_t                                                  mediaDescriptor;
    hyperdos_win32_directory_disk_file_allocation_table_type fileAllocationTableType;
} hyperdos_win32_directory_disk_format;

typedef struct hyperdos_win32_directory_disk_entry
{
    char     hostPath[HYPERDOS_WIN32_DIRECTORY_DISK_PATH_CAPACITY];
    uint8_t  shortName[HYPERDOS_WIN32_DIRECTORY_DISK_SHORT_NAME_BYTE_COUNT];
    size_t   parentEntryIndex;
    uint32_t fileSize;
    uint16_t modifiedDate;
    uint16_t modifiedTime;
    uint16_t firstCluster;
    uint16_t clusterCount;
    uint8_t  attribute;
} hyperdos_win32_directory_disk_entry;

typedef struct hyperdos_win32_directory_disk_entry_list
{
    hyperdos_win32_directory_disk_entry* entries;
    size_t                               entryCount;
    size_t                               entryCapacity;
} hyperdos_win32_directory_disk_entry_list;

static void hyperdos_win32_directory_disk_free_entry_list(hyperdos_win32_directory_disk_entry_list* entryList)
{
    if (entryList == NULL)
    {
        return;
    }
    free(entryList->entries);
    memset(entryList, 0, sizeof(*entryList));
}

static int hyperdos_win32_directory_disk_reserve_entries(hyperdos_win32_directory_disk_entry_list* entryList,
                                                         size_t                                    requiredEntryCount)
{
    size_t                               newEntryCapacity = 0u;
    hyperdos_win32_directory_disk_entry* newEntries       = NULL;

    if (entryList == NULL)
    {
        return 0;
    }
    if (requiredEntryCount <= entryList->entryCapacity)
    {
        return 1;
    }
    newEntryCapacity = entryList->entryCapacity == 0u ? 64u : entryList->entryCapacity;
    while (newEntryCapacity < requiredEntryCount)
    {
        newEntryCapacity *= 2u;
    }
    newEntries = (hyperdos_win32_directory_disk_entry*)realloc(entryList->entries,
                                                               newEntryCapacity * sizeof(*entryList->entries));
    if (newEntries == NULL)
    {
        return 0;
    }
    entryList->entries       = newEntries;
    entryList->entryCapacity = newEntryCapacity;
    return 1;
}

static int hyperdos_win32_directory_disk_append_entry(hyperdos_win32_directory_disk_entry_list*  entryList,
                                                      const hyperdos_win32_directory_disk_entry* sourceEntry)
{
    if (!hyperdos_win32_directory_disk_reserve_entries(entryList, entryList->entryCount + 1u))
    {
        return 0;
    }
    entryList->entries[entryList->entryCount] = *sourceEntry;
    ++entryList->entryCount;
    return 1;
}

static void hyperdos_win32_directory_disk_write_word(uint8_t* bytes, size_t byteOffset, uint16_t value)
{
    bytes[byteOffset]      = (uint8_t)(value & 0x00FFu);
    bytes[byteOffset + 1u] = (uint8_t)(value >> 8u);
}

static void hyperdos_win32_directory_disk_write_double_word(uint8_t* bytes, size_t byteOffset, uint32_t value)
{
    bytes[byteOffset]      = (uint8_t)(value & 0x000000FFu);
    bytes[byteOffset + 1u] = (uint8_t)((value >> 8u) & 0x000000FFu);
    bytes[byteOffset + 2u] = (uint8_t)((value >> 16u) & 0x000000FFu);
    bytes[byteOffset + 3u] = (uint8_t)(value >> 24u);
}

static int hyperdos_win32_directory_disk_join_path(char*       destination,
                                                   size_t      destinationSize,
                                                   const char* parentPath,
                                                   const char* childName)
{
    size_t parentLength = 0u;
    size_t childLength  = 0u;
    int    needsSlash   = 0;

    if (destination == NULL || destinationSize == 0u || parentPath == NULL || childName == NULL)
    {
        return 0;
    }
    parentLength = strlen(parentPath);
    childLength  = strlen(childName);
    needsSlash   = parentLength != 0u && parentPath[parentLength - 1u] != '\\' && parentPath[parentLength - 1u] != '/';
    if (parentLength + (needsSlash ? 1u : 0u) + childLength + 1u > destinationSize)
    {
        return 0;
    }
    memcpy(destination, parentPath, parentLength);
    if (needsSlash)
    {
        destination[parentLength] = '\\';
        ++parentLength;
    }
    memcpy(destination + parentLength, childName, childLength + 1u);
    return 1;
}

static void hyperdos_win32_directory_disk_file_time_to_dos_time(const FILETIME* fileTime,
                                                                uint16_t*       dosDate,
                                                                uint16_t*       dosTime)
{
    FILETIME localFileTime;
    WORD     fileDate     = 0u;
    WORD     fileTimeWord = 0u;

    if (dosDate == NULL || dosTime == NULL)
    {
        return;
    }
    *dosDate = 0x0021u;
    *dosTime = 0u;
    if (fileTime == NULL)
    {
        return;
    }
    if (FileTimeToLocalFileTime(fileTime, &localFileTime) == 0)
    {
        localFileTime = *fileTime;
    }
    if (FileTimeToDosDateTime(&localFileTime, &fileDate, &fileTimeWord) != 0)
    {
        *dosDate = fileDate;
        *dosTime = fileTimeWord;
    }
}

static uint8_t hyperdos_win32_directory_disk_make_dos_attribute(const WIN32_FIND_DATAA* findData)
{
    uint8_t attribute = HYPERDOS_WIN32_DIRECTORY_DISK_DOS_ATTRIBUTE_ARCHIVE;

    if (findData == NULL)
    {
        return attribute;
    }
    if ((findData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0u)
    {
        attribute = HYPERDOS_WIN32_DIRECTORY_DISK_DOS_ATTRIBUTE_DIRECTORY;
    }
    if ((findData->dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0u)
    {
        attribute |= HYPERDOS_WIN32_DIRECTORY_DISK_DOS_ATTRIBUTE_READ_ONLY;
    }
    return attribute;
}

static int hyperdos_win32_directory_disk_character_is_valid_short_name_character(char character)
{
    static const char allowedCharacters[] = "$%'_-@~`!(){}^#&";

    if (character >= 'A' && character <= 'Z')
    {
        return 1;
    }
    if (character >= '0' && character <= '9')
    {
        return 1;
    }
    return strchr(allowedCharacters, character) != NULL;
}

static char hyperdos_win32_directory_disk_normalize_short_name_character(char character)
{
    if (character >= 'a' && character <= 'z')
    {
        character = (char)(character - 'a' + 'A');
    }
    if (hyperdos_win32_directory_disk_character_is_valid_short_name_character(character))
    {
        return character;
    }
    return '_';
}

static void hyperdos_win32_directory_disk_split_file_name(const char* fileName,
                                                          char*       baseName,
                                                          size_t      baseNameCapacity,
                                                          char*       extensionName,
                                                          size_t      extensionNameCapacity)
{
    const char* lastDot         = NULL;
    size_t      baseNameLength  = 0u;
    size_t      extensionLength = 0u;

    baseName[0]      = '\0';
    extensionName[0] = '\0';
    if (fileName == NULL)
    {
        return;
    }
    lastDot = strrchr(fileName, '.');
    if (lastDot != NULL && lastDot != fileName)
    {
        baseNameLength  = (size_t)(lastDot - fileName);
        extensionLength = strlen(lastDot + 1u);
    }
    else
    {
        baseNameLength = strlen(fileName);
    }
    if (baseNameLength >= baseNameCapacity)
    {
        baseNameLength = baseNameCapacity - 1u;
    }
    if (extensionLength >= extensionNameCapacity)
    {
        extensionLength = extensionNameCapacity - 1u;
    }
    memcpy(baseName, fileName, baseNameLength);
    baseName[baseNameLength] = '\0';
    if (lastDot != NULL && lastDot != fileName)
    {
        memcpy(extensionName, lastDot + 1u, extensionLength);
        extensionName[extensionLength] = '\0';
    }
}

static int hyperdos_win32_directory_disk_short_name_is_unique(const hyperdos_win32_directory_disk_entry_list* entryList,
                                                              size_t         parentEntryIndex,
                                                              const uint8_t* shortName)
{
    size_t entryIndex = 0u;

    for (entryIndex = 0u; entryIndex < entryList->entryCount; ++entryIndex)
    {
        const hyperdos_win32_directory_disk_entry* entry = &entryList->entries[entryIndex];

        if (entry->parentEntryIndex == parentEntryIndex &&
            memcmp(entry->shortName, shortName, HYPERDOS_WIN32_DIRECTORY_DISK_SHORT_NAME_BYTE_COUNT) == 0)
        {
            return 0;
        }
    }
    return 1;
}

static void hyperdos_win32_directory_disk_make_short_name_bytes(const char* fileName,
                                                                uint32_t    collisionNumber,
                                                                uint8_t*    shortName)
{
    char   baseName[260];
    char   extensionName[260];
    char   normalizedBaseName[9];
    char   normalizedExtensionName[4];
    size_t sourceIndex      = 0u;
    size_t destinationIndex = 0u;

    memset(shortName, ' ', HYPERDOS_WIN32_DIRECTORY_DISK_SHORT_NAME_BYTE_COUNT);
    memset(normalizedBaseName, 0, sizeof(normalizedBaseName));
    memset(normalizedExtensionName, 0, sizeof(normalizedExtensionName));
    hyperdos_win32_directory_disk_split_file_name(fileName,
                                                  baseName,
                                                  sizeof(baseName),
                                                  extensionName,
                                                  sizeof(extensionName));
    for (sourceIndex = 0u; baseName[sourceIndex] != '\0' && destinationIndex < sizeof(normalizedBaseName) - 1u;
         ++sourceIndex)
    {
        if (baseName[sourceIndex] == ' ' || baseName[sourceIndex] == '.')
        {
            continue;
        }
        normalizedBaseName[destinationIndex] = hyperdos_win32_directory_disk_normalize_short_name_character(
                baseName[sourceIndex]);
        ++destinationIndex;
    }
    if (destinationIndex == 0u)
    {
        normalizedBaseName[0] = '_';
        normalizedBaseName[1] = '\0';
    }
    destinationIndex = 0u;
    for (sourceIndex = 0u;
         extensionName[sourceIndex] != '\0' && destinationIndex < sizeof(normalizedExtensionName) - 1u;
         ++sourceIndex)
    {
        if (extensionName[sourceIndex] == ' ' || extensionName[sourceIndex] == '.')
        {
            continue;
        }
        normalizedExtensionName[destinationIndex] = hyperdos_win32_directory_disk_normalize_short_name_character(
                extensionName[sourceIndex]);
        ++destinationIndex;
    }

    if (collisionNumber != 0u)
    {
        char   collisionText[8];
        size_t collisionTextLength = 0u;
        size_t basePrefixLength    = 0u;

        (void)snprintf(collisionText, sizeof(collisionText), "~%lu", (unsigned long)collisionNumber);
        collisionTextLength = strlen(collisionText);
        basePrefixLength    = collisionTextLength < 8u ? 8u - collisionTextLength : 1u;
        if (basePrefixLength > strlen(normalizedBaseName))
        {
            basePrefixLength = strlen(normalizedBaseName);
        }
        memcpy(shortName, normalizedBaseName, basePrefixLength);
        memcpy(shortName + basePrefixLength, collisionText, collisionTextLength);
    }
    else
    {
        memcpy(shortName, normalizedBaseName, strlen(normalizedBaseName));
    }
    memcpy(shortName + 8u, normalizedExtensionName, strlen(normalizedExtensionName));
}

static void hyperdos_win32_directory_disk_make_unique_short_name(
        const hyperdos_win32_directory_disk_entry_list* entryList,
        size_t                                          parentEntryIndex,
        const char*                                     fileName,
        uint8_t*                                        shortName)
{
    uint32_t collisionNumber = 0u;

    do
    {
        hyperdos_win32_directory_disk_make_short_name_bytes(fileName, collisionNumber, shortName);
        if (hyperdos_win32_directory_disk_short_name_is_unique(entryList, parentEntryIndex, shortName))
        {
            return;
        }
        ++collisionNumber;
    }
    while (collisionNumber < 100000u);
}

static int hyperdos_win32_directory_disk_collect_directory(hyperdos_win32_directory_disk_entry_list* entryList,
                                                           const char*                               directoryPath,
                                                           size_t                                    parentEntryIndex)
{
    char             searchPath[HYPERDOS_WIN32_DIRECTORY_DISK_PATH_CAPACITY];
    WIN32_FIND_DATAA findData;
    HANDLE           findHandle = INVALID_HANDLE_VALUE;

    if (!hyperdos_win32_directory_disk_join_path(searchPath, sizeof(searchPath), directoryPath, "*"))
    {
        return 0;
    }
    findHandle = FindFirstFileA(searchPath, &findData);
    if (findHandle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    do
    {
        hyperdos_win32_directory_disk_entry entry;
        uint64_t                            fileSize           = 0u;
        size_t                              appendedEntryIndex = 0u;

        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
        {
            continue;
        }
        if ((findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0u)
        {
            continue;
        }

        memset(&entry, 0, sizeof(entry));
        if (!hyperdos_win32_directory_disk_join_path(entry.hostPath,
                                                     sizeof(entry.hostPath),
                                                     directoryPath,
                                                     findData.cFileName))
        {
            FindClose(findHandle);
            return 0;
        }
        entry.parentEntryIndex = parentEntryIndex;
        entry.attribute        = hyperdos_win32_directory_disk_make_dos_attribute(&findData);
        fileSize               = ((uint64_t)findData.nFileSizeHigh << 32u) | findData.nFileSizeLow;
        if (fileSize > 0xFFFFFFFFu)
        {
            FindClose(findHandle);
            return 0;
        }
        entry.fileSize = (uint32_t)fileSize;
        hyperdos_win32_directory_disk_file_time_to_dos_time(&findData.ftLastWriteTime,
                                                            &entry.modifiedDate,
                                                            &entry.modifiedTime);
        hyperdos_win32_directory_disk_make_unique_short_name(entryList,
                                                             parentEntryIndex,
                                                             findData.cFileName,
                                                             entry.shortName);
        appendedEntryIndex = entryList->entryCount;
        if (!hyperdos_win32_directory_disk_append_entry(entryList, &entry))
        {
            FindClose(findHandle);
            return 0;
        }
        if ((entry.attribute & HYPERDOS_WIN32_DIRECTORY_DISK_DOS_ATTRIBUTE_DIRECTORY) != 0u &&
            !hyperdos_win32_directory_disk_collect_directory(entryList, entry.hostPath, appendedEntryIndex))
        {
            FindClose(findHandle);
            return 0;
        }
    }
    while (FindNextFileA(findHandle, &findData) != 0);

    FindClose(findHandle);
    return GetLastError() == ERROR_NO_MORE_FILES;
}

static size_t hyperdos_win32_directory_disk_count_children(const hyperdos_win32_directory_disk_entry_list* entryList,
                                                           size_t parentEntryIndex)
{
    size_t entryIndex = 0u;
    size_t childCount = 0u;

    for (entryIndex = 0u; entryIndex < entryList->entryCount; ++entryIndex)
    {
        if (entryList->entries[entryIndex].parentEntryIndex == parentEntryIndex)
        {
            ++childCount;
        }
    }
    return childCount;
}

static uint16_t hyperdos_win32_directory_disk_round_up_clusters(uint32_t byteCount, uint32_t clusterByteCount)
{
    uint32_t clusterCount = 0u;

    if (byteCount == 0u)
    {
        return 0u;
    }
    clusterCount = (byteCount + clusterByteCount - 1u) / clusterByteCount;
    return clusterCount > 0xFFFFu ? 0u : (uint16_t)clusterCount;
}

static int hyperdos_win32_directory_disk_assign_clusters(hyperdos_win32_directory_disk_entry_list*   entryList,
                                                         const hyperdos_win32_directory_disk_format* format,
                                                         uint32_t                                    volumeStartSector,
                                                         uint16_t* requiredClusterCount)
{
    size_t   entryIndex             = 0u;
    uint32_t clusterByteCount       = HYPERDOS_WIN32_DIRECTORY_DISK_BYTES_PER_SECTOR * format->sectorsPerCluster;
    uint32_t nextCluster            = 2u;
    uint32_t rootChildCount         = (uint32_t)hyperdos_win32_directory_disk_count_children(entryList, SIZE_MAX);
    uint32_t rootDirectoryByteCount = (uint32_t)format->rootDirectoryEntryCount *
                                      HYPERDOS_WIN32_DIRECTORY_DISK_DIRECTORY_ENTRY_SIZE;
    (void)volumeStartSector;

    if (rootChildCount > format->rootDirectoryEntryCount)
    {
        return 0;
    }
    for (entryIndex = 0u; entryIndex < entryList->entryCount; ++entryIndex)
    {
        hyperdos_win32_directory_disk_entry* entry = &entryList->entries[entryIndex];

        if ((entry->attribute & HYPERDOS_WIN32_DIRECTORY_DISK_DOS_ATTRIBUTE_DIRECTORY) != 0u)
        {
            uint32_t directoryEntryCount = (uint32_t)hyperdos_win32_directory_disk_count_children(entryList,
                                                                                                  entryIndex) +
                                           2u;
            uint32_t directoryByteCount = directoryEntryCount * HYPERDOS_WIN32_DIRECTORY_DISK_DIRECTORY_ENTRY_SIZE;
            entry->clusterCount = hyperdos_win32_directory_disk_round_up_clusters(directoryByteCount, clusterByteCount);
        }
        else
        {
            entry->clusterCount = hyperdos_win32_directory_disk_round_up_clusters(entry->fileSize, clusterByteCount);
        }
        if (entry->clusterCount == 0u &&
            ((entry->attribute & HYPERDOS_WIN32_DIRECTORY_DISK_DOS_ATTRIBUTE_DIRECTORY) != 0u || entry->fileSize != 0u))
        {
            return 0;
        }
        entry->firstCluster  = entry->clusterCount != 0u ? (uint16_t)nextCluster : 0u;
        nextCluster         += entry->clusterCount;
        if (nextCluster > 0xFFF0u)
        {
            return 0;
        }
    }
    if (rootDirectoryByteCount / HYPERDOS_WIN32_DIRECTORY_DISK_DIRECTORY_ENTRY_SIZE <
        (uint32_t)hyperdos_win32_directory_disk_count_children(entryList, SIZE_MAX))
    {
        return 0;
    }
    *requiredClusterCount = (uint16_t)(nextCluster - 2u);
    return 1;
}

static uint32_t hyperdos_win32_directory_disk_get_root_directory_sector_count(uint16_t rootDirectoryEntryCount)
{
    return ((uint32_t)rootDirectoryEntryCount * HYPERDOS_WIN32_DIRECTORY_DISK_DIRECTORY_ENTRY_SIZE +
            HYPERDOS_WIN32_DIRECTORY_DISK_BYTES_PER_SECTOR - 1u) /
           HYPERDOS_WIN32_DIRECTORY_DISK_BYTES_PER_SECTOR;
}

static uint32_t hyperdos_win32_directory_disk_get_first_data_sector(const hyperdos_win32_directory_disk_format* format)
{
    return format->hiddenSectorCount + HYPERDOS_WIN32_DIRECTORY_DISK_RESERVED_SECTOR_COUNT +
           (uint32_t)HYPERDOS_WIN32_DIRECTORY_DISK_FILE_ALLOCATION_TABLE_COUNT * format->sectorsPerFileAllocationTable +
           hyperdos_win32_directory_disk_get_root_directory_sector_count(format->rootDirectoryEntryCount);
}

static uint32_t hyperdos_win32_directory_disk_get_data_cluster_count(const hyperdos_win32_directory_disk_format* format)
{
    uint32_t firstDataSector = hyperdos_win32_directory_disk_get_first_data_sector(format);

    if (format->totalSectorCount <= firstDataSector)
    {
        return 0u;
    }
    return (format->totalSectorCount - firstDataSector) / format->sectorsPerCluster;
}

static int hyperdos_win32_directory_disk_format_can_hold(const hyperdos_win32_directory_disk_format* format,
                                                         hyperdos_win32_directory_disk_entry_list*   entryList,
                                                         uint16_t* requiredClusterCount)
{
    uint32_t dataClusterCount = 0u;

    if (!hyperdos_win32_directory_disk_assign_clusters(entryList,
                                                       format,
                                                       format->hiddenSectorCount,
                                                       requiredClusterCount))
    {
        return 0;
    }
    dataClusterCount = hyperdos_win32_directory_disk_get_data_cluster_count(format);
    return *requiredClusterCount <= dataClusterCount;
}

static void hyperdos_win32_directory_disk_write_boot_sector(uint8_t* imageBytes,
                                                            uint32_t volumeStartSector,
                                                            const hyperdos_win32_directory_disk_format* format,
                                                            const char*                                 volumeLabel)
{
    uint8_t* bootSector = imageBytes + (size_t)volumeStartSector * HYPERDOS_WIN32_DIRECTORY_DISK_BYTES_PER_SECTOR;

    bootSector[0] = 0xEBu;
    bootSector[1] = 0x3Cu;
    bootSector[2] = 0x90u;
    memcpy(bootSector + 3u, "HYPERDOS", 8u);
    hyperdos_win32_directory_disk_write_word(bootSector, 11u, HYPERDOS_WIN32_DIRECTORY_DISK_BYTES_PER_SECTOR);
    bootSector[13] = format->sectorsPerCluster;
    hyperdos_win32_directory_disk_write_word(bootSector, 14u, HYPERDOS_WIN32_DIRECTORY_DISK_RESERVED_SECTOR_COUNT);
    bootSector[16] = HYPERDOS_WIN32_DIRECTORY_DISK_FILE_ALLOCATION_TABLE_COUNT;
    hyperdos_win32_directory_disk_write_word(bootSector, 17u, format->rootDirectoryEntryCount);
    if (format->totalSectorCount - format->hiddenSectorCount <= 0xFFFFu)
    {
        hyperdos_win32_directory_disk_write_word(bootSector,
                                                 19u,
                                                 (uint16_t)(format->totalSectorCount - format->hiddenSectorCount));
    }
    bootSector[21] = format->mediaDescriptor;
    hyperdos_win32_directory_disk_write_word(bootSector, 22u, format->sectorsPerFileAllocationTable);
    hyperdos_win32_directory_disk_write_word(bootSector, 24u, format->sectorsPerTrack);
    hyperdos_win32_directory_disk_write_word(bootSector, 26u, format->headCount);
    hyperdos_win32_directory_disk_write_double_word(bootSector, 28u, format->hiddenSectorCount);
    if (format->totalSectorCount - format->hiddenSectorCount > 0xFFFFu)
    {
        hyperdos_win32_directory_disk_write_double_word(bootSector,
                                                        32u,
                                                        format->totalSectorCount - format->hiddenSectorCount);
    }
    bootSector[36] = format->hiddenSectorCount == 0u ? 0u : 0x80u;
    bootSector[38] = 0x29u;
    hyperdos_win32_directory_disk_write_double_word(bootSector, 39u, 0x48594452u);
    memset(bootSector + 43u, ' ', 11u);
    if (volumeLabel != NULL)
    {
        size_t volumeLabelLength = strlen(volumeLabel);

        if (volumeLabelLength > 11u)
        {
            volumeLabelLength = 11u;
        }
        memcpy(bootSector + 43u, volumeLabel, volumeLabelLength);
    }
    memcpy(bootSector + 54u,
           format->fileAllocationTableType == HYPERDOS_WIN32_DIRECTORY_DISK_FILE_ALLOCATION_TABLE_TYPE_16 ? "FAT16   "
                                                                                                          : "FAT12   ",
           8u);
    memcpy(bootSector + 62u, "Non-system disk.\r\n", 18u);
    bootSector[HYPERDOS_WIN32_DIRECTORY_DISK_BOOT_SIGNATURE_OFFSET]      = 0x55u;
    bootSector[HYPERDOS_WIN32_DIRECTORY_DISK_BOOT_SIGNATURE_OFFSET + 1u] = 0xAAu;
}

static void hyperdos_win32_directory_disk_encode_chs(uint8_t* bytes,
                                                     size_t   byteOffset,
                                                     uint32_t logicalBlockAddress,
                                                     uint16_t headCount,
                                                     uint16_t sectorsPerTrack)
{
    uint32_t cylinder = logicalBlockAddress / ((uint32_t)headCount * sectorsPerTrack);
    uint32_t head     = (logicalBlockAddress / sectorsPerTrack) % headCount;
    uint32_t sector   = (logicalBlockAddress % sectorsPerTrack) + 1u;

    if (cylinder > 1023u)
    {
        cylinder = 1023u;
        head     = headCount - 1u;
        sector   = sectorsPerTrack;
    }
    bytes[byteOffset]      = (uint8_t)head;
    bytes[byteOffset + 1u] = (uint8_t)((sector & 0x3Fu) | ((cylinder >> 2u) & 0xC0u));
    bytes[byteOffset + 2u] = (uint8_t)(cylinder & 0xFFu);
}

static void hyperdos_win32_directory_disk_write_master_boot_record(uint8_t* imageBytes,
                                                                   const hyperdos_win32_directory_disk_format* format)
{
    uint32_t partitionStartSector = format->hiddenSectorCount;
    uint32_t partitionSectorCount = format->totalSectorCount - format->hiddenSectorCount;
    size_t   partitionEntryOffset = HYPERDOS_WIN32_DIRECTORY_DISK_PARTITION_TABLE_OFFSET;

    memcpy(imageBytes, "\x31\xC0\x8E\xD8\xBE\x15\x7C\xB4\x0E\xAC\x84\xC0\x74\x04\xCD\x10\xEB\xF7\xF4\xEB\xFD", 21u);
    memcpy(imageBytes + 21u, "Missing operating system", 24u);
    imageBytes[partitionEntryOffset] = 0x00u;
    hyperdos_win32_directory_disk_encode_chs(imageBytes,
                                             partitionEntryOffset + 1u,
                                             partitionStartSector,
                                             format->headCount,
                                             format->sectorsPerTrack);
    imageBytes[partitionEntryOffset + 4u] = 0x06u;
    hyperdos_win32_directory_disk_encode_chs(imageBytes,
                                             partitionEntryOffset + 5u,
                                             format->totalSectorCount - 1u,
                                             format->headCount,
                                             format->sectorsPerTrack);
    hyperdos_win32_directory_disk_write_double_word(imageBytes, partitionEntryOffset + 8u, partitionStartSector);
    hyperdos_win32_directory_disk_write_double_word(imageBytes, partitionEntryOffset + 12u, partitionSectorCount);
    imageBytes[HYPERDOS_WIN32_DIRECTORY_DISK_BOOT_SIGNATURE_OFFSET]      = 0x55u;
    imageBytes[HYPERDOS_WIN32_DIRECTORY_DISK_BOOT_SIGNATURE_OFFSET + 1u] = 0xAAu;
}

static void hyperdos_win32_directory_disk_set_file_allocation_table_entry(
        uint8_t*                                                 fileAllocationTable,
        uint16_t                                                 clusterNumber,
        uint16_t                                                 value,
        hyperdos_win32_directory_disk_file_allocation_table_type fileAllocationTableType)
{
    if (fileAllocationTableType == HYPERDOS_WIN32_DIRECTORY_DISK_FILE_ALLOCATION_TABLE_TYPE_16)
    {
        size_t byteOffset = (size_t)clusterNumber * 2u;

        hyperdos_win32_directory_disk_write_word(fileAllocationTable, byteOffset, value);
        return;
    }
    else
    {
        size_t byteOffset = clusterNumber + clusterNumber / 2u;

        if ((clusterNumber & 1u) != 0u)
        {
            fileAllocationTable[byteOffset]      = (uint8_t)((fileAllocationTable[byteOffset] & 0x0Fu) |
                                                        ((value & 0x000Fu) << 4u));
            fileAllocationTable[byteOffset + 1u] = (uint8_t)(value >> 4u);
        }
        else
        {
            fileAllocationTable[byteOffset]      = (uint8_t)(value & 0x00FFu);
            fileAllocationTable[byteOffset + 1u] = (uint8_t)((fileAllocationTable[byteOffset + 1u] & 0xF0u) |
                                                             ((value >> 8u) & 0x000Fu));
        }
    }
}

static int hyperdos_win32_directory_disk_write_file_allocation_tables(
        uint8_t*                                        imageBytes,
        const hyperdos_win32_directory_disk_format*     format,
        const hyperdos_win32_directory_disk_entry_list* entryList)
{
    uint32_t fileAllocationTableByteCount = (uint32_t)format->sectorsPerFileAllocationTable *
                                            HYPERDOS_WIN32_DIRECTORY_DISK_BYTES_PER_SECTOR;
    uint8_t* fileAllocationTable = (uint8_t*)calloc(1u, fileAllocationTableByteCount);
    size_t   entryIndex          = 0u;

    if (fileAllocationTable == NULL)
    {
        return 0;
    }
    if (format->fileAllocationTableType == HYPERDOS_WIN32_DIRECTORY_DISK_FILE_ALLOCATION_TABLE_TYPE_16)
    {
        hyperdos_win32_directory_disk_set_file_allocation_table_entry(fileAllocationTable,
                                                                      0u,
                                                                      (uint16_t)(0xFF00u | format->mediaDescriptor),
                                                                      format->fileAllocationTableType);
        hyperdos_win32_directory_disk_set_file_allocation_table_entry(fileAllocationTable,
                                                                      1u,
                                                                      HYPERDOS_WIN32_DIRECTORY_DISK_FAT16_END_OF_CHAIN,
                                                                      format->fileAllocationTableType);
    }
    else
    {
        hyperdos_win32_directory_disk_set_file_allocation_table_entry(fileAllocationTable,
                                                                      0u,
                                                                      (uint16_t)(0x0F00u | format->mediaDescriptor),
                                                                      format->fileAllocationTableType);
        hyperdos_win32_directory_disk_set_file_allocation_table_entry(fileAllocationTable,
                                                                      1u,
                                                                      HYPERDOS_WIN32_DIRECTORY_DISK_FAT12_END_OF_CHAIN,
                                                                      format->fileAllocationTableType);
    }
    for (entryIndex = 0u; entryIndex < entryList->entryCount; ++entryIndex)
    {
        const hyperdos_win32_directory_disk_entry* entry         = &entryList->entries[entryIndex];
        uint16_t                                   clusterOffset = 0u;

        for (clusterOffset = 0u; clusterOffset < entry->clusterCount; ++clusterOffset)
        {
            uint16_t clusterNumber = (uint16_t)(entry->firstCluster + clusterOffset);
            uint16_t nextClusterValue =
                    clusterOffset + 1u == entry->clusterCount
                            ? (format->fileAllocationTableType ==
                                               HYPERDOS_WIN32_DIRECTORY_DISK_FILE_ALLOCATION_TABLE_TYPE_16
                                       ? HYPERDOS_WIN32_DIRECTORY_DISK_FAT16_END_OF_CHAIN
                                       : HYPERDOS_WIN32_DIRECTORY_DISK_FAT12_END_OF_CHAIN)
                            : (uint16_t)(clusterNumber + 1u);

            hyperdos_win32_directory_disk_set_file_allocation_table_entry(fileAllocationTable,
                                                                          clusterNumber,
                                                                          nextClusterValue,
                                                                          format->fileAllocationTableType);
        }
    }
    for (entryIndex = 0u; entryIndex < HYPERDOS_WIN32_DIRECTORY_DISK_FILE_ALLOCATION_TABLE_COUNT; ++entryIndex)
    {
        uint32_t fileAllocationTableSector = format->hiddenSectorCount +
                                             HYPERDOS_WIN32_DIRECTORY_DISK_RESERVED_SECTOR_COUNT +
                                             (uint32_t)entryIndex * format->sectorsPerFileAllocationTable;
        memcpy(imageBytes + (size_t)fileAllocationTableSector * HYPERDOS_WIN32_DIRECTORY_DISK_BYTES_PER_SECTOR,
               fileAllocationTable,
               fileAllocationTableByteCount);
    }
    free(fileAllocationTable);
    return 1;
}

static uint32_t hyperdos_win32_directory_disk_get_cluster_byte_offset(
        const hyperdos_win32_directory_disk_format* format,
        uint16_t                                    clusterNumber)
{
    uint32_t firstDataSector = hyperdos_win32_directory_disk_get_first_data_sector(format);

    return (firstDataSector + (uint32_t)(clusterNumber - 2u) * format->sectorsPerCluster) *
           HYPERDOS_WIN32_DIRECTORY_DISK_BYTES_PER_SECTOR;
}

static void hyperdos_win32_directory_disk_write_directory_entry(uint8_t*       directoryBytes,
                                                                size_t         directoryEntryIndex,
                                                                const uint8_t* shortName,
                                                                uint8_t        attribute,
                                                                uint16_t       modifiedDate,
                                                                uint16_t       modifiedTime,
                                                                uint16_t       firstCluster,
                                                                uint32_t       fileSize)
{
    size_t directoryEntryOffset = directoryEntryIndex * HYPERDOS_WIN32_DIRECTORY_DISK_DIRECTORY_ENTRY_SIZE;

    memcpy(directoryBytes + directoryEntryOffset, shortName, HYPERDOS_WIN32_DIRECTORY_DISK_SHORT_NAME_BYTE_COUNT);
    directoryBytes[directoryEntryOffset + 11u] = attribute;
    hyperdos_win32_directory_disk_write_word(directoryBytes, directoryEntryOffset + 22u, modifiedTime);
    hyperdos_win32_directory_disk_write_word(directoryBytes, directoryEntryOffset + 24u, modifiedDate);
    hyperdos_win32_directory_disk_write_word(directoryBytes, directoryEntryOffset + 26u, firstCluster);
    hyperdos_win32_directory_disk_write_double_word(directoryBytes, directoryEntryOffset + 28u, fileSize);
}

static void hyperdos_win32_directory_disk_write_child_directory_entries(
        uint8_t*                                        directoryBytes,
        const hyperdos_win32_directory_disk_entry_list* entryList,
        size_t                                          parentEntryIndex,
        size_t                                          firstDirectoryEntryIndex)
{
    size_t entryIndex          = 0u;
    size_t directoryEntryIndex = firstDirectoryEntryIndex;

    for (entryIndex = 0u; entryIndex < entryList->entryCount; ++entryIndex)
    {
        const hyperdos_win32_directory_disk_entry* entry = &entryList->entries[entryIndex];

        if (entry->parentEntryIndex != parentEntryIndex)
        {
            continue;
        }
        hyperdos_win32_directory_disk_write_directory_entry(directoryBytes,
                                                            directoryEntryIndex,
                                                            entry->shortName,
                                                            entry->attribute,
                                                            entry->modifiedDate,
                                                            entry->modifiedTime,
                                                            entry->firstCluster,
                                                            entry->fileSize);
        ++directoryEntryIndex;
    }
}

static void hyperdos_win32_directory_disk_write_directories(uint8_t*                                        imageBytes,
                                                            const hyperdos_win32_directory_disk_format*     format,
                                                            const hyperdos_win32_directory_disk_entry_list* entryList)
{
    uint32_t rootDirectorySector = format->hiddenSectorCount + HYPERDOS_WIN32_DIRECTORY_DISK_RESERVED_SECTOR_COUNT +
                                   (uint32_t)HYPERDOS_WIN32_DIRECTORY_DISK_FILE_ALLOCATION_TABLE_COUNT *
                                           format->sectorsPerFileAllocationTable;
    uint8_t* rootDirectoryBytes = imageBytes +
                                  (size_t)rootDirectorySector * HYPERDOS_WIN32_DIRECTORY_DISK_BYTES_PER_SECTOR;
    size_t entryIndex = 0u;

    hyperdos_win32_directory_disk_write_child_directory_entries(rootDirectoryBytes, entryList, SIZE_MAX, 0u);
    for (entryIndex = 0u; entryIndex < entryList->entryCount; ++entryIndex)
    {
        const hyperdos_win32_directory_disk_entry* entry          = &entryList->entries[entryIndex];
        uint8_t*                                   directoryBytes = NULL;
        uint8_t  currentDirectoryShortName[HYPERDOS_WIN32_DIRECTORY_DISK_SHORT_NAME_BYTE_COUNT];
        uint8_t  parentDirectoryShortName[HYPERDOS_WIN32_DIRECTORY_DISK_SHORT_NAME_BYTE_COUNT];
        uint16_t parentCluster = 0u;

        if ((entry->attribute & HYPERDOS_WIN32_DIRECTORY_DISK_DOS_ATTRIBUTE_DIRECTORY) == 0u ||
            entry->clusterCount == 0u)
        {
            continue;
        }
        memset(currentDirectoryShortName, ' ', sizeof(currentDirectoryShortName));
        currentDirectoryShortName[0] = '.';
        memset(parentDirectoryShortName, ' ', sizeof(parentDirectoryShortName));
        parentDirectoryShortName[0] = '.';
        parentDirectoryShortName[1] = '.';
        if (entry->parentEntryIndex != SIZE_MAX)
        {
            parentCluster = entryList->entries[entry->parentEntryIndex].firstCluster;
        }
        directoryBytes = imageBytes +
                         hyperdos_win32_directory_disk_get_cluster_byte_offset(format, entry->firstCluster);
        hyperdos_win32_directory_disk_write_directory_entry(directoryBytes,
                                                            0u,
                                                            currentDirectoryShortName,
                                                            HYPERDOS_WIN32_DIRECTORY_DISK_DOS_ATTRIBUTE_DIRECTORY,
                                                            entry->modifiedDate,
                                                            entry->modifiedTime,
                                                            entry->firstCluster,
                                                            0u);
        hyperdos_win32_directory_disk_write_directory_entry(directoryBytes,
                                                            1u,
                                                            parentDirectoryShortName,
                                                            HYPERDOS_WIN32_DIRECTORY_DISK_DOS_ATTRIBUTE_DIRECTORY,
                                                            entry->modifiedDate,
                                                            entry->modifiedTime,
                                                            parentCluster,
                                                            0u);
        hyperdos_win32_directory_disk_write_child_directory_entries(directoryBytes, entryList, entryIndex, 2u);
    }
}

static int hyperdos_win32_directory_disk_write_file_data(uint8_t*                                        imageBytes,
                                                         const hyperdos_win32_directory_disk_format*     format,
                                                         const hyperdos_win32_directory_disk_entry_list* entryList)
{
    size_t entryIndex = 0u;

    for (entryIndex = 0u; entryIndex < entryList->entryCount; ++entryIndex)
    {
        const hyperdos_win32_directory_disk_entry* entry         = &entryList->entries[entryIndex];
        FILE*                                      hostFile      = NULL;
        uint16_t                                   clusterOffset = 0u;

        if ((entry->attribute & HYPERDOS_WIN32_DIRECTORY_DISK_DOS_ATTRIBUTE_DIRECTORY) != 0u || entry->fileSize == 0u)
        {
            continue;
        }
        hostFile = fopen(entry->hostPath, "rb");
        if (hostFile == NULL)
        {
            return 0;
        }
        for (clusterOffset = 0u; clusterOffset < entry->clusterCount; ++clusterOffset)
        {
            uint8_t* clusterBytes = imageBytes + hyperdos_win32_directory_disk_get_cluster_byte_offset(
                                                         format,
                                                         (uint16_t)(entry->firstCluster + clusterOffset));
            size_t remainingByteCount = (size_t)entry->fileSize -
                                        (size_t)clusterOffset * format->sectorsPerCluster *
                                                HYPERDOS_WIN32_DIRECTORY_DISK_BYTES_PER_SECTOR;
            size_t readByteCount = remainingByteCount < (size_t)format->sectorsPerCluster *
                                                                HYPERDOS_WIN32_DIRECTORY_DISK_BYTES_PER_SECTOR
                                           ? remainingByteCount
                                           : (size_t)format->sectorsPerCluster *
                                                     HYPERDOS_WIN32_DIRECTORY_DISK_BYTES_PER_SECTOR;

            if (readByteCount != 0u && fread(clusterBytes, 1u, readByteCount, hostFile) != readByteCount)
            {
                fclose(hostFile);
                return 0;
            }
        }
        fclose(hostFile);
    }
    return 1;
}

static int hyperdos_win32_directory_disk_build_image(hyperdos_pc_disk_image*                         diskImage,
                                                     const char*                                     directoryPath,
                                                     const hyperdos_win32_directory_disk_format*     format,
                                                     const hyperdos_win32_directory_disk_entry_list* entryList,
                                                     uint8_t                                         isHardDisk)
{
    uint8_t* imageBytes     = NULL;
    size_t   imageByteCount = (size_t)format->totalSectorCount * HYPERDOS_WIN32_DIRECTORY_DISK_BYTES_PER_SECTOR;

    imageBytes = (uint8_t*)calloc(1u, imageByteCount);
    if (imageBytes == NULL)
    {
        return 0;
    }
    if (format->hiddenSectorCount != 0u)
    {
        hyperdos_win32_directory_disk_write_master_boot_record(imageBytes, format);
    }
    hyperdos_win32_directory_disk_write_boot_sector(imageBytes,
                                                    format->hiddenSectorCount,
                                                    format,
                                                    isHardDisk ? "HOSTDIR" : "HOSTFLOPPY");
    if (!hyperdos_win32_directory_disk_write_file_allocation_tables(imageBytes, format, entryList))
    {
        free(imageBytes);
        return 0;
    }
    hyperdos_win32_directory_disk_write_directories(imageBytes, format, entryList);
    if (!hyperdos_win32_directory_disk_write_file_data(imageBytes, format, entryList))
    {
        free(imageBytes);
        return 0;
    }

    return isHardDisk ? hyperdos_pc_disk_image_initialize_memory_hard_disk(diskImage,
                                                                           directoryPath,
                                                                           imageBytes,
                                                                           imageByteCount,
                                                                           1u)
                      : hyperdos_pc_disk_image_initialize_memory_floppy(diskImage,
                                                                        directoryPath,
                                                                        imageBytes,
                                                                        imageByteCount,
                                                                        1u);
}

static uint16_t hyperdos_win32_directory_disk_calculate_fat16_sector_count(uint32_t totalSectorCount)
{
    uint16_t sectorsPerFileAllocationTable = 1u;

    for (;;)
    {
        uint32_t rootDirectorySectorCount = hyperdos_win32_directory_disk_get_root_directory_sector_count(
                HYPERDOS_WIN32_DIRECTORY_DISK_FIXED_ROOT_ENTRY_COUNT);
        uint32_t dataSectorCount = totalSectorCount - HYPERDOS_WIN32_DIRECTORY_DISK_FIXED_HIDDEN_SECTORS -
                                   HYPERDOS_WIN32_DIRECTORY_DISK_RESERVED_SECTOR_COUNT - rootDirectorySectorCount -
                                   (uint32_t)HYPERDOS_WIN32_DIRECTORY_DISK_FILE_ALLOCATION_TABLE_COUNT *
                                           sectorsPerFileAllocationTable;
        uint32_t clusterCount        = dataSectorCount / HYPERDOS_WIN32_DIRECTORY_DISK_FIXED_SECTORS_PER_CLUSTER;
        uint16_t requiredSectorCount = (uint16_t)(((clusterCount + 2u) * 2u +
                                                   HYPERDOS_WIN32_DIRECTORY_DISK_BYTES_PER_SECTOR - 1u) /
                                                  HYPERDOS_WIN32_DIRECTORY_DISK_BYTES_PER_SECTOR);

        if (requiredSectorCount == sectorsPerFileAllocationTable)
        {
            return sectorsPerFileAllocationTable;
        }
        sectorsPerFileAllocationTable = requiredSectorCount;
    }
}

static uint32_t hyperdos_win32_directory_disk_align_fixed_total_sectors(uint32_t totalSectorCount)
{
    uint32_t sectorsPerCylinder = HYPERDOS_WIN32_DIRECTORY_DISK_FIXED_SECTORS_PER_TRACK *
                                  HYPERDOS_WIN32_DIRECTORY_DISK_FIXED_HEAD_COUNT;

    return ((totalSectorCount + sectorsPerCylinder - 1u) / sectorsPerCylinder) * sectorsPerCylinder;
}

static int hyperdos_win32_directory_disk_load_directory_disk_image(hyperdos_pc_disk_image* diskImage,
                                                                   const char*             directoryPath,
                                                                   uint8_t                 isHardDisk)
{
    static const hyperdos_win32_directory_disk_format floppyFormats[] = {
        {2880u,  0u, 224u, 9u, 18u, 2u, 1u, 0xF0u, HYPERDOS_WIN32_DIRECTORY_DISK_FILE_ALLOCATION_TABLE_TYPE_12},
        {5760u,  0u, 240u, 9u, 36u, 2u, 2u, 0xF0u, HYPERDOS_WIN32_DIRECTORY_DISK_FILE_ALLOCATION_TABLE_TYPE_12},
        {11520u, 0u, 224u, 9u, 72u, 2u, 4u, 0xF0u, HYPERDOS_WIN32_DIRECTORY_DISK_FILE_ALLOCATION_TABLE_TYPE_12}
    };
    hyperdos_win32_directory_disk_entry_list entryList;
    hyperdos_win32_directory_disk_format     selectedFormat;
    uint16_t                                 requiredClusterCount = 0u;
    size_t                                   formatIndex          = 0u;

    memset(&entryList, 0, sizeof(entryList));
    memset(&selectedFormat, 0, sizeof(selectedFormat));
    if (diskImage == NULL || directoryPath == NULL || !hyperdos_win32_path_is_directory(directoryPath))
    {
        return 0;
    }
    if (!hyperdos_win32_directory_disk_collect_directory(&entryList, directoryPath, SIZE_MAX))
    {
        hyperdos_win32_directory_disk_free_entry_list(&entryList);
        return 0;
    }
    if (isHardDisk)
    {
        uint32_t totalSectorCount = HYPERDOS_WIN32_DIRECTORY_DISK_FIXED_MINIMUM_TOTAL_SECTORS;

        for (;;)
        {
            selectedFormat.totalSectorCount = hyperdos_win32_directory_disk_align_fixed_total_sectors(totalSectorCount);
            selectedFormat.hiddenSectorCount             = HYPERDOS_WIN32_DIRECTORY_DISK_FIXED_HIDDEN_SECTORS;
            selectedFormat.rootDirectoryEntryCount       = HYPERDOS_WIN32_DIRECTORY_DISK_FIXED_ROOT_ENTRY_COUNT;
            selectedFormat.sectorsPerTrack               = HYPERDOS_WIN32_DIRECTORY_DISK_FIXED_SECTORS_PER_TRACK;
            selectedFormat.headCount                     = HYPERDOS_WIN32_DIRECTORY_DISK_FIXED_HEAD_COUNT;
            selectedFormat.sectorsPerCluster             = HYPERDOS_WIN32_DIRECTORY_DISK_FIXED_SECTORS_PER_CLUSTER;
            selectedFormat.mediaDescriptor               = 0xF8u;
            selectedFormat.fileAllocationTableType       = HYPERDOS_WIN32_DIRECTORY_DISK_FILE_ALLOCATION_TABLE_TYPE_16;
            selectedFormat.sectorsPerFileAllocationTable = hyperdos_win32_directory_disk_calculate_fat16_sector_count(
                    selectedFormat.totalSectorCount);
            if (hyperdos_win32_directory_disk_format_can_hold(&selectedFormat, &entryList, &requiredClusterCount))
            {
                break;
            }
            if (totalSectorCount > 1024u * 1024u)
            {
                hyperdos_win32_directory_disk_free_entry_list(&entryList);
                return 0;
            }
            totalSectorCount *= 2u;
        }
    }
    else
    {
        for (formatIndex = 0u; formatIndex < sizeof(floppyFormats) / sizeof(floppyFormats[0]); ++formatIndex)
        {
            selectedFormat = floppyFormats[formatIndex];
            if (hyperdos_win32_directory_disk_format_can_hold(&selectedFormat, &entryList, &requiredClusterCount))
            {
                break;
            }
        }
        if (formatIndex == sizeof(floppyFormats) / sizeof(floppyFormats[0]))
        {
            hyperdos_win32_directory_disk_free_entry_list(&entryList);
            return 0;
        }
    }
    if (!hyperdos_win32_directory_disk_build_image(diskImage, directoryPath, &selectedFormat, &entryList, isHardDisk))
    {
        hyperdos_win32_directory_disk_free_entry_list(&entryList);
        return 0;
    }
    hyperdos_win32_directory_disk_free_entry_list(&entryList);
    return 1;
}

int hyperdos_win32_path_is_directory(const char* path)
{
    DWORD attributes = 0u;

    if (path == NULL || path[0] == '\0')
    {
        return 0;
    }
    attributes = GetFileAttributesA(path);
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0u;
}

int hyperdos_win32_load_floppy_directory_disk_image(hyperdos_pc_disk_image* diskImage, const char* directoryPath)
{
    return hyperdos_win32_directory_disk_load_directory_disk_image(diskImage, directoryPath, 0u);
}

int hyperdos_win32_load_fixed_directory_disk_image(hyperdos_pc_disk_image* diskImage, const char* directoryPath)
{
    return hyperdos_win32_directory_disk_load_directory_disk_image(diskImage, directoryPath, 1u);
}
