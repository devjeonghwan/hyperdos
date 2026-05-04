#ifndef HYPERDOS_CHIPSET_H
#define HYPERDOS_CHIPSET_H

#include <stdint.h>

#include "hyperdos/hardware.h"

typedef enum hyperdos_bus_cycle_type
{
    HYPERDOS_BUS_CYCLE_PASSIVE = 0,
    HYPERDOS_BUS_CYCLE_INTERRUPT_ACKNOWLEDGE,
    HYPERDOS_BUS_CYCLE_INPUT_OUTPUT_READ,
    HYPERDOS_BUS_CYCLE_INPUT_OUTPUT_WRITE,
    HYPERDOS_BUS_CYCLE_MEMORY_READ,
    HYPERDOS_BUS_CYCLE_MEMORY_WRITE
} hyperdos_bus_cycle_type;

typedef enum hyperdos_pc_chipset_profile
{
    HYPERDOS_PC_CHIPSET_PROFILE_DEFAULT = 0,
    HYPERDOS_PC_CHIPSET_PROFILE_PC,
    HYPERDOS_PC_CHIPSET_PROFILE_XT,
    HYPERDOS_PC_CHIPSET_PROFILE_AT_286,
    HYPERDOS_PC_CHIPSET_PROFILE_AT_386,
    HYPERDOS_PC_CHIPSET_PROFILE_AT_486
} hyperdos_pc_chipset_profile;

typedef enum hyperdos_programmable_interval_timer_model
{
    HYPERDOS_PROGRAMMABLE_INTERVAL_TIMER_MODEL_DEFAULT = 0,
    HYPERDOS_PROGRAMMABLE_INTERVAL_TIMER_MODEL_8253,
    HYPERDOS_PROGRAMMABLE_INTERVAL_TIMER_MODEL_8254
} hyperdos_programmable_interval_timer_model;

typedef enum hyperdos_universal_asynchronous_receiver_transmitter_model
{
    HYPERDOS_UNIVERSAL_ASYNCHRONOUS_RECEIVER_TRANSMITTER_MODEL_DEFAULT = 0,
    HYPERDOS_UNIVERSAL_ASYNCHRONOUS_RECEIVER_TRANSMITTER_MODEL_8250,
    HYPERDOS_UNIVERSAL_ASYNCHRONOUS_RECEIVER_TRANSMITTER_MODEL_16450,
    HYPERDOS_UNIVERSAL_ASYNCHRONOUS_RECEIVER_TRANSMITTER_MODEL_16550A
} hyperdos_universal_asynchronous_receiver_transmitter_model;

typedef struct hyperdos_intel_8284_clock_generator
{
    uint32_t             crystalFrequencyHertz;
    uint32_t             processorFrequencyHertz;
    uint64_t             generatedClockCount;
    hyperdos_signal_line clockLine;
    hyperdos_signal_line peripheralClockLine;
    hyperdos_signal_line readyLine;
    hyperdos_signal_line resetLine;
} hyperdos_intel_8284_clock_generator;

typedef struct hyperdos_intel_8288_bus_controller
{
    uint8_t                 statusLineZero;
    uint8_t                 statusLineOne;
    uint8_t                 statusLineTwo;
    hyperdos_bus_cycle_type currentCycleType;
    hyperdos_signal_line    memoryReadCommandLine;
    hyperdos_signal_line    memoryWriteCommandLine;
    hyperdos_signal_line    inputOutputReadCommandLine;
    hyperdos_signal_line    inputOutputWriteCommandLine;
    hyperdos_signal_line    interruptAcknowledgeLine;
} hyperdos_intel_8288_bus_controller;

typedef struct hyperdos_intel_8282_address_latch
{
    uint32_t             latchedAddress;
    hyperdos_signal_line latchEnableLine;
} hyperdos_intel_8282_address_latch;

typedef struct hyperdos_intel_8286_bus_transceiver
{
    uint8_t              enabled;
    uint8_t              transmitFromProcessor;
    hyperdos_signal_line outputEnableLine;
    hyperdos_signal_line directionLine;
} hyperdos_intel_8286_bus_transceiver;

void hyperdos_intel_8284_clock_generator_initialize(hyperdos_intel_8284_clock_generator* clockGenerator,
                                                    uint32_t                             crystalFrequencyHertz);

void hyperdos_intel_8284_clock_generator_set_processor_frequency_hertz(
        hyperdos_intel_8284_clock_generator* clockGenerator,
        uint32_t                             processorFrequencyHertz);

void hyperdos_intel_8284_clock_generator_step(hyperdos_intel_8284_clock_generator* clockGenerator,
                                              hyperdos_bus*                        bus,
                                              uint64_t                             processorClockCount);

void hyperdos_intel_8288_bus_controller_initialize(hyperdos_intel_8288_bus_controller* busController);

void hyperdos_intel_8288_bus_controller_decode_status(hyperdos_intel_8288_bus_controller* busController,
                                                      uint8_t                             statusLineTwo,
                                                      uint8_t                             statusLineOne,
                                                      uint8_t                             statusLineZero);

void hyperdos_intel_8282_address_latch_initialize(hyperdos_intel_8282_address_latch* addressLatch);

void hyperdos_intel_8282_address_latch_capture(hyperdos_intel_8282_address_latch* addressLatch,
                                               uint32_t                           multiplexedAddress);

void hyperdos_intel_8286_bus_transceiver_initialize(hyperdos_intel_8286_bus_transceiver* transceiver);

void hyperdos_intel_8286_bus_transceiver_set_direction(hyperdos_intel_8286_bus_transceiver* transceiver,
                                                       int                                  transmitFromProcessor);

#endif
