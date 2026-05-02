#ifndef HYPERDOS_WIN32_PC_FILE_DISK_H
#define HYPERDOS_WIN32_PC_FILE_DISK_H

#include "hyperdos/pc_disk_image.h"

int hyperdos_win32_load_floppy_file_disk_image(hyperdos_pc_disk_image* diskImage, const char* path);

int hyperdos_win32_load_fixed_file_disk_image(hyperdos_pc_disk_image* diskImage, const char* path);

#endif
