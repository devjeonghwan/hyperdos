#include "hyperdos/chipset.h"

#include <stdint.h>
#include <string.h>

enum
{
    HYPERDOS_INTEL_8284_PROCESSOR_DIVISOR            = 3u,
    HYPERDOS_INTEL_8284_PERIPHERAL_DIVISOR           = 6u,
    HYPERDOS_INTEL_8288_STATUS_INTERRUPT_ACKNOWLEDGE = 0x0u,
    HYPERDOS_INTEL_8288_STATUS_INPUT_OUTPUT_READ     = 0x2u,
    HYPERDOS_INTEL_8288_STATUS_INPUT_OUTPUT_WRITE    = 0x3u,
    HYPERDOS_INTEL_8288_STATUS_MEMORY_READ           = 0x5u,
    HYPERDOS_INTEL_8288_STATUS_MEMORY_WRITE          = 0x6u
};

void hyperdos_intel_8284_clock_generator_initialize(hyperdos_intel_8284_clock_generator* clockGenerator,
                                                    uint32_t                             crystalFrequencyHertz)
{
    if (clockGenerator == NULL)
    {
        return;
    }
    memset(clockGenerator, 0, sizeof(*clockGenerator));
    clockGenerator->crystalFrequencyHertz   = crystalFrequencyHertz;
    clockGenerator->processorFrequencyHertz = crystalFrequencyHertz / HYPERDOS_INTEL_8284_PROCESSOR_DIVISOR;
    hyperdos_signal_line_initialize(&clockGenerator->clockLine, "CLK", HYPERDOS_SIGNAL_LOW);
    hyperdos_signal_line_initialize(&clockGenerator->peripheralClockLine, "PCLK", HYPERDOS_SIGNAL_LOW);
    hyperdos_signal_line_initialize(&clockGenerator->readyLine, "READY", HYPERDOS_SIGNAL_HIGH);
    hyperdos_signal_line_initialize(&clockGenerator->resetLine, "RESET", HYPERDOS_SIGNAL_LOW);
}

void hyperdos_intel_8284_clock_generator_step(hyperdos_intel_8284_clock_generator* clockGenerator,
                                              hyperdos_bus*                        bus,
                                              uint64_t                             processorClockCount)
{
    if (clockGenerator == NULL)
    {
        return;
    }
    clockGenerator->generatedClockCount += processorClockCount;
    hyperdos_signal_line_drive(&clockGenerator->clockLine,
                               (clockGenerator->generatedClockCount & 1u) != 0u ? HYPERDOS_SIGNAL_HIGH
                                                                                : HYPERDOS_SIGNAL_LOW);
    hyperdos_signal_line_drive(&clockGenerator->peripheralClockLine,
                               ((clockGenerator->generatedClockCount / HYPERDOS_INTEL_8284_PERIPHERAL_DIVISOR) & 1u) !=
                                               0u
                                       ? HYPERDOS_SIGNAL_HIGH
                                       : HYPERDOS_SIGNAL_LOW);
    hyperdos_bus_tick(bus, processorClockCount);
}

void hyperdos_intel_8288_bus_controller_initialize(hyperdos_intel_8288_bus_controller* busController)
{
    if (busController == NULL)
    {
        return;
    }
    memset(busController, 0, sizeof(*busController));
    busController->currentCycleType = HYPERDOS_BUS_CYCLE_PASSIVE;
    hyperdos_signal_line_initialize(&busController->memoryReadCommandLine, "MRDC", HYPERDOS_SIGNAL_HIGH);
    hyperdos_signal_line_initialize(&busController->memoryWriteCommandLine, "MWTC", HYPERDOS_SIGNAL_HIGH);
    hyperdos_signal_line_initialize(&busController->inputOutputReadCommandLine, "IORC", HYPERDOS_SIGNAL_HIGH);
    hyperdos_signal_line_initialize(&busController->inputOutputWriteCommandLine, "IOWC", HYPERDOS_SIGNAL_HIGH);
    hyperdos_signal_line_initialize(&busController->interruptAcknowledgeLine, "INTA", HYPERDOS_SIGNAL_HIGH);
}

void hyperdos_intel_8288_bus_controller_decode_status(hyperdos_intel_8288_bus_controller* busController,
                                                      uint8_t                             statusLineTwo,
                                                      uint8_t                             statusLineOne,
                                                      uint8_t                             statusLineZero)
{
    uint8_t statusCode = 0;

    if (busController == NULL)
    {
        return;
    }

    busController->statusLineZero   = statusLineZero;
    busController->statusLineOne    = statusLineOne;
    busController->statusLineTwo    = statusLineTwo;
    statusCode                      = (uint8_t)((statusLineTwo << 2u) | (statusLineOne << 1u) | statusLineZero);
    busController->currentCycleType = HYPERDOS_BUS_CYCLE_PASSIVE;
    hyperdos_signal_line_drive(&busController->memoryReadCommandLine, HYPERDOS_SIGNAL_HIGH);
    hyperdos_signal_line_drive(&busController->memoryWriteCommandLine, HYPERDOS_SIGNAL_HIGH);
    hyperdos_signal_line_drive(&busController->inputOutputReadCommandLine, HYPERDOS_SIGNAL_HIGH);
    hyperdos_signal_line_drive(&busController->inputOutputWriteCommandLine, HYPERDOS_SIGNAL_HIGH);
    hyperdos_signal_line_drive(&busController->interruptAcknowledgeLine, HYPERDOS_SIGNAL_HIGH);

    switch (statusCode)
    {
    case HYPERDOS_INTEL_8288_STATUS_INTERRUPT_ACKNOWLEDGE:
        busController->currentCycleType = HYPERDOS_BUS_CYCLE_INTERRUPT_ACKNOWLEDGE;
        hyperdos_signal_line_drive(&busController->interruptAcknowledgeLine, HYPERDOS_SIGNAL_LOW);
        break;
    case HYPERDOS_INTEL_8288_STATUS_INPUT_OUTPUT_READ:
        busController->currentCycleType = HYPERDOS_BUS_CYCLE_INPUT_OUTPUT_READ;
        hyperdos_signal_line_drive(&busController->inputOutputReadCommandLine, HYPERDOS_SIGNAL_LOW);
        break;
    case HYPERDOS_INTEL_8288_STATUS_INPUT_OUTPUT_WRITE:
        busController->currentCycleType = HYPERDOS_BUS_CYCLE_INPUT_OUTPUT_WRITE;
        hyperdos_signal_line_drive(&busController->inputOutputWriteCommandLine, HYPERDOS_SIGNAL_LOW);
        break;
    case HYPERDOS_INTEL_8288_STATUS_MEMORY_READ:
        busController->currentCycleType = HYPERDOS_BUS_CYCLE_MEMORY_READ;
        hyperdos_signal_line_drive(&busController->memoryReadCommandLine, HYPERDOS_SIGNAL_LOW);
        break;
    case HYPERDOS_INTEL_8288_STATUS_MEMORY_WRITE:
        busController->currentCycleType = HYPERDOS_BUS_CYCLE_MEMORY_WRITE;
        hyperdos_signal_line_drive(&busController->memoryWriteCommandLine, HYPERDOS_SIGNAL_LOW);
        break;
    default:
        break;
    }
}

void hyperdos_intel_8282_address_latch_initialize(hyperdos_intel_8282_address_latch* addressLatch)
{
    if (addressLatch == NULL)
    {
        return;
    }
    memset(addressLatch, 0, sizeof(*addressLatch));
    hyperdos_signal_line_initialize(&addressLatch->latchEnableLine, "ALE", HYPERDOS_SIGNAL_LOW);
}

void hyperdos_intel_8282_address_latch_capture(hyperdos_intel_8282_address_latch* addressLatch,
                                               uint32_t                           multiplexedAddress)
{
    if (addressLatch == NULL)
    {
        return;
    }
    addressLatch->latchedAddress = multiplexedAddress;
    hyperdos_signal_line_drive(&addressLatch->latchEnableLine, HYPERDOS_SIGNAL_HIGH);
    hyperdos_signal_line_drive(&addressLatch->latchEnableLine, HYPERDOS_SIGNAL_LOW);
}

void hyperdos_intel_8286_bus_transceiver_initialize(hyperdos_intel_8286_bus_transceiver* transceiver)
{
    if (transceiver == NULL)
    {
        return;
    }
    memset(transceiver, 0, sizeof(*transceiver));
    hyperdos_signal_line_initialize(&transceiver->outputEnableLine, "DEN", HYPERDOS_SIGNAL_HIGH);
    hyperdos_signal_line_initialize(&transceiver->directionLine, "DT/R", HYPERDOS_SIGNAL_LOW);
}

void hyperdos_intel_8286_bus_transceiver_set_direction(hyperdos_intel_8286_bus_transceiver* transceiver,
                                                       int                                  transmitFromProcessor)
{
    if (transceiver == NULL)
    {
        return;
    }
    transceiver->enabled               = 1u;
    transceiver->transmitFromProcessor = transmitFromProcessor != 0;
    hyperdos_signal_line_drive(&transceiver->outputEnableLine, HYPERDOS_SIGNAL_LOW);
    hyperdos_signal_line_drive(&transceiver->directionLine,
                               transmitFromProcessor != 0 ? HYPERDOS_SIGNAL_HIGH : HYPERDOS_SIGNAL_LOW);
}
