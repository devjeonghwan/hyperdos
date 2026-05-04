#ifndef HYPERDOS_HARDWARE_H
#define HYPERDOS_HARDWARE_H

#include <stddef.h>
#include <stdint.h>

#define HYPERDOS_BUS_MAXIMUM_MEMORY_MAPPINGS       64u
#define HYPERDOS_BUS_MAXIMUM_INPUT_OUTPUT_MAPPINGS 64u
#define HYPERDOS_BUS_MAXIMUM_CLOCK_DEVICES         64u

typedef enum hyperdos_bus_access_result
{
    HYPERDOS_BUS_ACCESS_OK = 0,
    HYPERDOS_BUS_ACCESS_INVALID_ARGUMENT,
    HYPERDOS_BUS_ACCESS_TABLE_FULL,
    HYPERDOS_BUS_ACCESS_UNMAPPED_ADDRESS
} hyperdos_bus_access_result;

typedef enum hyperdos_signal_level
{
    HYPERDOS_SIGNAL_LOW      = 0,
    HYPERDOS_SIGNAL_HIGH     = 1,
    HYPERDOS_SIGNAL_FLOATING = 2
} hyperdos_signal_level;

typedef uint8_t (*hyperdos_bus_read_memory_byte)(void* device, uint32_t physicalAddress);

typedef void (*hyperdos_bus_write_memory_byte)(void* device, uint32_t physicalAddress, uint8_t value);

typedef void (*hyperdos_bus_observe_memory_write)(void*    observerContext,
                                                  uint32_t physicalAddress,
                                                  uint8_t  oldValue,
                                                  uint8_t  newValue);

typedef uint32_t (*hyperdos_bus_translate_memory_address)(void* translatorContext, uint32_t physicalAddress);

typedef uint8_t (*hyperdos_bus_read_input_output_byte)(void* device, uint16_t port);

typedef void (*hyperdos_bus_write_input_output_byte)(void* device, uint16_t port, uint8_t value);

typedef void (*hyperdos_clock_tick_device)(void* device, uint64_t elapsedClockCount);

typedef struct hyperdos_signal_line
{
    const char*           name;
    hyperdos_signal_level level;
    uint64_t              transitionCount;
} hyperdos_signal_line;

typedef struct hyperdos_memory_mapping
{
    uint32_t                       firstAddress;
    uint32_t                       lastAddress;
    void*                          device;
    hyperdos_bus_read_memory_byte  readByte;
    hyperdos_bus_write_memory_byte writeByte;
    uint8_t                        observerOldValueReadEnabled;
} hyperdos_memory_mapping;

typedef struct hyperdos_input_output_mapping
{
    uint16_t                             firstPort;
    uint16_t                             lastPort;
    void*                                device;
    hyperdos_bus_read_input_output_byte  readByte;
    hyperdos_bus_write_input_output_byte writeByte;
} hyperdos_input_output_mapping;

typedef struct hyperdos_clock_device
{
    void*                      device;
    hyperdos_clock_tick_device tick;
} hyperdos_clock_device;

typedef struct hyperdos_bus
{
    hyperdos_memory_mapping               memoryMappings[HYPERDOS_BUS_MAXIMUM_MEMORY_MAPPINGS];
    size_t                                memoryMappingCount;
    hyperdos_input_output_mapping         inputOutputMappings[HYPERDOS_BUS_MAXIMUM_INPUT_OUTPUT_MAPPINGS];
    size_t                                inputOutputMappingCount;
    hyperdos_clock_device                 clockDevices[HYPERDOS_BUS_MAXIMUM_CLOCK_DEVICES];
    size_t                                clockDeviceCount;
    hyperdos_bus_observe_memory_write     memoryWriteObserver;
    void*                                 memoryWriteObserverContext;
    hyperdos_bus_translate_memory_address memoryAddressTranslator;
    void*                                 memoryAddressTranslatorContext;
    uint64_t                              clockCount;
    hyperdos_signal_line                  readyLine;
    hyperdos_signal_line                  interruptRequestLine;
    hyperdos_signal_line                  nonMaskableInterruptLine;
    hyperdos_signal_line                  holdRequestLine;
    hyperdos_signal_line                  holdAcknowledgeLine;
    hyperdos_signal_line                  resetLine;
} hyperdos_bus;

void hyperdos_signal_line_initialize(hyperdos_signal_line* line, const char* name, hyperdos_signal_level initialLevel);

void hyperdos_signal_line_drive(hyperdos_signal_line* line, hyperdos_signal_level level);

void hyperdos_bus_initialize(hyperdos_bus* bus);

hyperdos_bus_access_result hyperdos_bus_map_memory(hyperdos_bus*                  bus,
                                                   uint32_t                       firstAddress,
                                                   uint32_t                       byteCount,
                                                   void*                          device,
                                                   hyperdos_bus_read_memory_byte  readByte,
                                                   hyperdos_bus_write_memory_byte writeByte);

hyperdos_bus_access_result hyperdos_bus_map_input_output(hyperdos_bus*                        bus,
                                                         uint16_t                             firstPort,
                                                         uint16_t                             portCount,
                                                         void*                                device,
                                                         hyperdos_bus_read_input_output_byte  readByte,
                                                         hyperdos_bus_write_input_output_byte writeByte);

hyperdos_bus_access_result hyperdos_bus_attach_clock_device(hyperdos_bus*              bus,
                                                            void*                      device,
                                                            hyperdos_clock_tick_device tick);

void hyperdos_bus_set_memory_write_observer(hyperdos_bus*                     bus,
                                            hyperdos_bus_observe_memory_write observer,
                                            void*                             observerContext);

void hyperdos_bus_set_memory_address_translator(hyperdos_bus*                         bus,
                                                hyperdos_bus_translate_memory_address translator,
                                                void*                                 translatorContext);

uint32_t hyperdos_bus_translate_memory_address_for_device(hyperdos_bus* bus, uint32_t physicalAddress);

void hyperdos_bus_set_memory_mapping_observer_old_value_read_enabled(hyperdos_bus* bus,
                                                                     uint32_t      firstAddress,
                                                                     uint32_t      byteCount,
                                                                     int           enabled);

uint8_t hyperdos_bus_read_memory_byte_or_open_bus(hyperdos_bus* bus, uint32_t physicalAddress);

void hyperdos_bus_write_memory_byte_if_mapped(hyperdos_bus* bus, uint32_t physicalAddress, uint8_t value);

uint8_t hyperdos_bus_read_input_output_byte_or_open_bus(hyperdos_bus* bus, uint16_t port);

void hyperdos_bus_write_input_output_byte_if_mapped(hyperdos_bus* bus, uint16_t port, uint8_t value);

void hyperdos_bus_tick(hyperdos_bus* bus, uint64_t elapsedClockCount);

const char* hyperdos_bus_access_result_name(hyperdos_bus_access_result result);

#endif
