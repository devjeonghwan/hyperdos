#ifndef HYPERDOS_WIN32_PC_DIRECTORY_DISK_H
#define HYPERDOS_WIN32_PC_DIRECTORY_DISK_H

#include "hyperdos/pc_disk_image.h"

int hyperdos_win32_path_is_directory(const char* path);

int hyperdos_win32_load_floppy_directory_disk_image(hyperdos_pc_disk_image* diskImage, const char* directoryPath);

int hyperdos_win32_load_fixed_directory_disk_image(hyperdos_pc_disk_image* diskImage, const char* directoryPath);

#endif
