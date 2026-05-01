#ifndef HYPERDOS_PC_VIDEO_SERVICES_H
#define HYPERDOS_PC_VIDEO_SERVICES_H

#include <stddef.h>
#include <stdint.h>

#include "hyperdos/pc_board.h"
#include "hyperdos/pc_video_bios.h"

typedef void (*hyperdos_pc_video_services_trace_function)(void* userContext, const char* message);

typedef struct hyperdos_pc_video_services
{
    hyperdos_pc*                              pc;
    void*                                     traceUserContext;
    hyperdos_pc_video_services_trace_function traceFunction;
    uint8_t                                   videoMode;
    uint8_t                                   activeVideoPage;
    uint16_t                                  videoColumnCount;
    uint16_t                                  cursorRow;
    uint16_t                                  cursorColumn;
} hyperdos_pc_video_services;

void hyperdos_pc_video_services_initialize(hyperdos_pc_video_services*               videoServices,
                                           hyperdos_pc*                              pc,
                                           hyperdos_pc_video_services_trace_function traceFunction,
                                           void*                                     traceUserContext);

void hyperdos_pc_video_services_initialize_static_functionality_table(hyperdos_pc_video_services* videoServices);

void hyperdos_pc_video_services_connect_bios_interface(hyperdos_pc_video_services*       videoServices,
                                                       hyperdos_pc_video_bios_interface* videoBiosInterface);

void hyperdos_pc_video_services_set_video_mode(hyperdos_pc_video_services* videoServices, uint8_t mode);

size_t hyperdos_pc_video_services_get_text_page_cell_memory_index(const hyperdos_pc_video_services* videoServices,
                                                                  uint8_t                           videoPage,
                                                                  uint16_t                          row,
                                                                  uint16_t                          column);

uint8_t hyperdos_pc_video_services_read_text_memory_byte(const hyperdos_pc_video_services* videoServices,
                                                         size_t                            memoryIndex);

int hyperdos_pc_video_services_get_video_graphics_array_mode_dimensions(uint8_t videoMode,
                                                                        int*    sourceWidth,
                                                                        int*    sourceHeight);

uint32_t hyperdos_pc_video_services_color_graphics_adapter_color_from_index(uint8_t colorIndex);

void hyperdos_pc_video_services_render_low_resolution_graphics_pixels(const hyperdos_color_graphics_adapter* adapter,
                                                                      uint32_t*                              pixels);

void hyperdos_pc_video_services_render_high_resolution_graphics_pixels(const hyperdos_color_graphics_adapter* adapter,
                                                                       uint32_t*                              pixels);

void hyperdos_pc_video_services_render_planar_video_graphics_array_pixels(
        const hyperdos_color_graphics_adapter* adapter,
        uint8_t                                videoMode,
        int                                    sourceWidth,
        int                                    sourceHeight,
        uint32_t*                              pixels);

void hyperdos_pc_video_services_render_linear_256_color_video_graphics_array_pixels(
        const hyperdos_color_graphics_adapter* adapter,
        int                                    sourceWidth,
        int                                    sourceHeight,
        uint32_t*                              pixels);

#endif
