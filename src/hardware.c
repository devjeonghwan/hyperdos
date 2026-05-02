#include "hyperdos/hardware.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

enum
{
    HYPERDOS_OPEN_BUS_BYTE = 0xFFu
};

static int hyperdos_x86_unsigned_range_contains_32(uint32_t firstAddress, uint32_t lastAddress, uint32_t address)
{
    return address >= firstAddress && address <= lastAddress;
}

static int hyperdos_x86_unsigned_ranges_overlap_32(uint32_t firstStart,
                                                   uint32_t firstEnd,
                                                   uint32_t secondStart,
                                                   uint32_t secondEnd)
{
    return firstStart <= secondEnd && secondStart <= firstEnd;
}

static int hyperdos_x86_unsigned_range_contains_16(uint16_t firstPort, uint16_t lastPort, uint16_t port)
{
    return port >= firstPort && port <= lastPort;
}

void hyperdos_signal_line_initialize(hyperdos_signal_line* line, const char* name, hyperdos_signal_level initialLevel)
{
    if (line == NULL)
    {
        return;
    }
    line->name            = name;
    line->level           = initialLevel;
    line->transitionCount = 0;
}

void hyperdos_signal_line_drive(hyperdos_signal_line* line, hyperdos_signal_level level)
{
    if (line == NULL)
    {
        return;
    }
    if (line->level != level)
    {
        ++line->transitionCount;
        line->level = level;
    }
}

void hyperdos_bus_initialize(hyperdos_bus* bus)
{
    if (bus == NULL)
    {
        return;
    }
    memset(bus, 0, sizeof(*bus));
    hyperdos_signal_line_initialize(&bus->readyLine, "READY", HYPERDOS_SIGNAL_HIGH);
    hyperdos_signal_line_initialize(&bus->interruptRequestLine, "INTR", HYPERDOS_SIGNAL_LOW);
    hyperdos_signal_line_initialize(&bus->nonMaskableInterruptLine, "NMI", HYPERDOS_SIGNAL_LOW);
    hyperdos_signal_line_initialize(&bus->holdRequestLine, "HOLD", HYPERDOS_SIGNAL_LOW);
    hyperdos_signal_line_initialize(&bus->holdAcknowledgeLine, "HLDA", HYPERDOS_SIGNAL_LOW);
    hyperdos_signal_line_initialize(&bus->resetLine, "RESET", HYPERDOS_SIGNAL_LOW);
}

hyperdos_bus_access_result hyperdos_bus_map_memory(hyperdos_bus*                  bus,
                                                   uint32_t                       firstAddress,
                                                   uint32_t                       byteCount,
                                                   void*                          device,
                                                   hyperdos_bus_read_memory_byte  readByte,
                                                   hyperdos_bus_write_memory_byte writeByte)
{
    hyperdos_memory_mapping* mapping = NULL;

    if (bus == NULL || byteCount == 0u || device == NULL || readByte == NULL || writeByte == NULL)
    {
        return HYPERDOS_BUS_ACCESS_INVALID_ARGUMENT;
    }
    if (bus->memoryMappingCount >= HYPERDOS_BUS_MAXIMUM_MEMORY_MAPPINGS)
    {
        return HYPERDOS_BUS_ACCESS_TABLE_FULL;
    }

    mapping                              = &bus->memoryMappings[bus->memoryMappingCount];
    mapping->firstAddress                = firstAddress;
    mapping->lastAddress                 = firstAddress + byteCount - 1u;
    mapping->device                      = device;
    mapping->readByte                    = readByte;
    mapping->writeByte                   = writeByte;
    mapping->observerOldValueReadEnabled = 1u;
    ++bus->memoryMappingCount;
    return HYPERDOS_BUS_ACCESS_OK;
}

hyperdos_bus_access_result hyperdos_bus_map_input_output(hyperdos_bus*                        bus,
                                                         uint16_t                             firstPort,
                                                         uint16_t                             portCount,
                                                         void*                                device,
                                                         hyperdos_bus_read_input_output_byte  readByte,
                                                         hyperdos_bus_write_input_output_byte writeByte)
{
    hyperdos_input_output_mapping* mapping = NULL;

    if (bus == NULL || portCount == 0u || device == NULL || readByte == NULL || writeByte == NULL)
    {
        return HYPERDOS_BUS_ACCESS_INVALID_ARGUMENT;
    }
    if (bus->inputOutputMappingCount >= HYPERDOS_BUS_MAXIMUM_INPUT_OUTPUT_MAPPINGS)
    {
        return HYPERDOS_BUS_ACCESS_TABLE_FULL;
    }

    mapping            = &bus->inputOutputMappings[bus->inputOutputMappingCount];
    mapping->firstPort = firstPort;
    mapping->lastPort  = (uint16_t)(firstPort + portCount - 1u);
    mapping->device    = device;
    mapping->readByte  = readByte;
    mapping->writeByte = writeByte;
    ++bus->inputOutputMappingCount;
    return HYPERDOS_BUS_ACCESS_OK;
}

hyperdos_bus_access_result hyperdos_bus_attach_clock_device(hyperdos_bus*              bus,
                                                            void*                      device,
                                                            hyperdos_clock_tick_device tick)
{
    hyperdos_clock_device* clockDevice = NULL;

    if (bus == NULL || device == NULL || tick == NULL)
    {
        return HYPERDOS_BUS_ACCESS_INVALID_ARGUMENT;
    }
    if (bus->clockDeviceCount >= HYPERDOS_BUS_MAXIMUM_CLOCK_DEVICES)
    {
        return HYPERDOS_BUS_ACCESS_TABLE_FULL;
    }

    clockDevice         = &bus->clockDevices[bus->clockDeviceCount];
    clockDevice->device = device;
    clockDevice->tick   = tick;
    ++bus->clockDeviceCount;
    return HYPERDOS_BUS_ACCESS_OK;
}

void hyperdos_bus_set_memory_write_observer(hyperdos_bus*                     bus,
                                            hyperdos_bus_observe_memory_write observer,
                                            void*                             observerContext)
{
    if (bus == NULL)
    {
        return;
    }
    bus->memoryWriteObserver        = observer;
    bus->memoryWriteObserverContext = observerContext;
}

void hyperdos_bus_set_memory_mapping_observer_old_value_read_enabled(hyperdos_bus* bus,
                                                                     uint32_t      firstAddress,
                                                                     uint32_t      byteCount,
                                                                     int           enabled)
{
    size_t   mappingIndex = 0u;
    uint32_t lastAddress  = byteCount == 0u ? firstAddress : firstAddress + byteCount - 1u;

    if (bus == NULL || byteCount == 0u)
    {
        return;
    }
    for (mappingIndex = 0u; mappingIndex < bus->memoryMappingCount; ++mappingIndex)
    {
        hyperdos_memory_mapping* mapping = &bus->memoryMappings[mappingIndex];
        if (hyperdos_x86_unsigned_ranges_overlap_32(mapping->firstAddress,
                                                    mapping->lastAddress,
                                                    firstAddress,
                                                    lastAddress))
        {
            mapping->observerOldValueReadEnabled = enabled != 0 ? 1u : 0u;
        }
    }
}

uint8_t hyperdos_bus_read_memory_byte_or_open_bus(hyperdos_bus* bus, uint32_t physicalAddress)
{
    size_t mappingIndex = 0;

    if (bus == NULL)
    {
        return HYPERDOS_OPEN_BUS_BYTE;
    }

    for (mappingIndex = 0; mappingIndex < bus->memoryMappingCount; ++mappingIndex)
    {
        hyperdos_memory_mapping* mapping = &bus->memoryMappings[mappingIndex];
        if (hyperdos_x86_unsigned_range_contains_32(mapping->firstAddress, mapping->lastAddress, physicalAddress))
        {
            return mapping->readByte(mapping->device, physicalAddress);
        }
    }
    return HYPERDOS_OPEN_BUS_BYTE;
}

void hyperdos_bus_write_memory_byte_if_mapped(hyperdos_bus* bus, uint32_t physicalAddress, uint8_t value)
{
    size_t mappingIndex = 0;

    if (bus == NULL)
    {
        return;
    }

    for (mappingIndex = 0; mappingIndex < bus->memoryMappingCount; ++mappingIndex)
    {
        hyperdos_memory_mapping* mapping = &bus->memoryMappings[mappingIndex];
        if (hyperdos_x86_unsigned_range_contains_32(mapping->firstAddress, mapping->lastAddress, physicalAddress))
        {
            uint8_t oldValue = HYPERDOS_OPEN_BUS_BYTE;
            if (bus->memoryWriteObserver != NULL && mapping->observerOldValueReadEnabled != 0u)
            {
                oldValue = mapping->readByte(mapping->device, physicalAddress);
            }
            mapping->writeByte(mapping->device, physicalAddress, value);
            if (bus->memoryWriteObserver != NULL)
            {
                bus->memoryWriteObserver(bus->memoryWriteObserverContext, physicalAddress, oldValue, value);
            }
            return;
        }
    }
}

uint8_t hyperdos_bus_read_input_output_byte_or_open_bus(hyperdos_bus* bus, uint16_t port)
{
    size_t mappingIndex = 0;

    if (bus == NULL)
    {
        return HYPERDOS_OPEN_BUS_BYTE;
    }

    for (mappingIndex = 0; mappingIndex < bus->inputOutputMappingCount; ++mappingIndex)
    {
        hyperdos_input_output_mapping* mapping = &bus->inputOutputMappings[mappingIndex];
        if (hyperdos_x86_unsigned_range_contains_16(mapping->firstPort, mapping->lastPort, port))
        {
            return mapping->readByte(mapping->device, port);
        }
    }
    return HYPERDOS_OPEN_BUS_BYTE;
}

void hyperdos_bus_write_input_output_byte_if_mapped(hyperdos_bus* bus, uint16_t port, uint8_t value)
{
    size_t mappingIndex = 0;

    if (bus == NULL)
    {
        return;
    }

    for (mappingIndex = 0; mappingIndex < bus->inputOutputMappingCount; ++mappingIndex)
    {
        hyperdos_input_output_mapping* mapping = &bus->inputOutputMappings[mappingIndex];
        if (hyperdos_x86_unsigned_range_contains_16(mapping->firstPort, mapping->lastPort, port))
        {
            mapping->writeByte(mapping->device, port, value);
            return;
        }
    }
}

void hyperdos_bus_tick(hyperdos_bus* bus, uint64_t elapsedClockCount)
{
    size_t clockDeviceIndex = 0;

    if (bus == NULL)
    {
        return;
    }

    bus->clockCount += elapsedClockCount;
    for (clockDeviceIndex = 0; clockDeviceIndex < bus->clockDeviceCount; ++clockDeviceIndex)
    {
        hyperdos_clock_device* clockDevice = &bus->clockDevices[clockDeviceIndex];
        clockDevice->tick(clockDevice->device, elapsedClockCount);
    }
}

const char* hyperdos_bus_access_result_name(hyperdos_bus_access_result result)
{
    switch (result)
    {
    case HYPERDOS_BUS_ACCESS_OK:
        return "ok";
    case HYPERDOS_BUS_ACCESS_INVALID_ARGUMENT:
        return "invalid argument";
    case HYPERDOS_BUS_ACCESS_TABLE_FULL:
        return "table full";
    case HYPERDOS_BUS_ACCESS_UNMAPPED_ADDRESS:
        return "unmapped address";
    }
    return "unknown";
}
