#include "hyperdos/pc_floppy_controller.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

enum
{
    HYPERDOS_PC_FLOPPY_CONTROLLER_TRACE_TEXT_CAPACITY              = 256u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_STATUS_READY_FOR_TRANSFER        = 0x80u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_STATUS_DATA_FROM_CONTROLLER      = 0x40u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_DIGITAL_INPUT_DISK_CHANGED       = 0x80u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_MASK                     = 0x1Fu,
    HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_SPECIFY                  = 0x03u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_SENSE_DRIVE_STATUS       = 0x04u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_RECALIBRATE              = 0x07u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_SENSE_INTERRUPT_STATUS   = 0x08u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_READ_IDENTIFIER          = 0x0Au,
    HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_SEEK                     = 0x0Fu,
    HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_VERSION                  = 0x10u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_CONFIGURE                = 0x13u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_STATUS_ZERO                      = 0x00u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_STATUS_INTERRUPT_CODE_SEEK_END   = 0x20u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_STATUS_THREE_READY               = 0x20u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_STATUS_THREE_TRACK_ZERO          = 0x10u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_VERSION_765A                     = 0x80u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_DIGITAL_OUTPUT_DRIVE_SELECT_MASK = 0x03u
};

static void hyperdos_pc_floppy_controller_trace(hyperdos_pc_floppy_controller* controller, const char* format, ...)
{
    char    message[HYPERDOS_PC_FLOPPY_CONTROLLER_TRACE_TEXT_CAPACITY];
    va_list arguments;

    if (controller == NULL || controller->traceFunction == NULL)
    {
        return;
    }

    va_start(arguments, format);
    (void)vsnprintf(message, sizeof(message), format, arguments);
    va_end(arguments);
    controller->traceFunction(controller->traceUserContext, message);
}

void hyperdos_pc_floppy_controller_initialize(hyperdos_pc_floppy_controller* controller)
{
    if (controller == NULL)
    {
        return;
    }

    memset(controller, 0, sizeof(*controller));
    controller->driveCount = 1u;
}

void hyperdos_pc_floppy_controller_set_trace_function(hyperdos_pc_floppy_controller*               controller,
                                                      hyperdos_pc_floppy_controller_trace_function traceFunction,
                                                      void*                                        traceUserContext)
{
    if (controller == NULL)
    {
        return;
    }

    controller->traceFunction    = traceFunction;
    controller->traceUserContext = traceUserContext;
}

void hyperdos_pc_floppy_controller_clear_results(hyperdos_pc_floppy_controller* controller)
{
    if (controller == NULL)
    {
        return;
    }

    controller->resultReadIndex = 0u;
    controller->resultCount     = 0u;
}

void hyperdos_pc_floppy_controller_reset(hyperdos_pc_floppy_controller* controller)
{
    hyperdos_pc_floppy_controller_trace_function traceFunction    = NULL;
    void*                                        traceUserContext = NULL;
    uint8_t                                      driveCount       = 0u;

    if (controller == NULL)
    {
        return;
    }

    traceFunction    = controller->traceFunction;
    traceUserContext = controller->traceUserContext;
    driveCount       = controller->driveCount;
    memset(controller, 0, sizeof(*controller));
    controller->traceFunction    = traceFunction;
    controller->traceUserContext = traceUserContext;
    controller->driveCount       = driveCount != 0u ? driveCount : 1u;
}

void hyperdos_pc_floppy_controller_set_drive_count(hyperdos_pc_floppy_controller* controller, uint8_t driveCount)
{
    if (controller == NULL)
    {
        return;
    }
    if (driveCount > HYPERDOS_PC_FLOPPY_CONTROLLER_MAXIMUM_DRIVE_COUNT)
    {
        driveCount = HYPERDOS_PC_FLOPPY_CONTROLLER_MAXIMUM_DRIVE_COUNT;
    }
    controller->driveCount = driveCount;
    if (controller->selectedDriveNumber >= controller->driveCount && controller->driveCount != 0u)
    {
        controller->selectedDriveNumber = 0u;
    }
}

static int hyperdos_pc_floppy_controller_drive_number_is_valid(const hyperdos_pc_floppy_controller* controller,
                                                               uint8_t                              driveNumber)
{
    uint8_t driveCount = 1u;

    if (controller == NULL)
    {
        return 0;
    }
    driveCount = controller->driveCount != 0u ? controller->driveCount : 1u;
    if (driveCount > HYPERDOS_PC_FLOPPY_CONTROLLER_MAXIMUM_DRIVE_COUNT)
    {
        driveCount = HYPERDOS_PC_FLOPPY_CONTROLLER_MAXIMUM_DRIVE_COUNT;
    }
    return driveNumber < driveCount;
}

void hyperdos_pc_floppy_controller_acknowledge_disk_change(hyperdos_pc_floppy_controller* controller)
{
    hyperdos_pc_floppy_controller_acknowledge_drive_disk_change(controller, 0u);
}

void hyperdos_pc_floppy_controller_acknowledge_drive_disk_change(hyperdos_pc_floppy_controller* controller,
                                                                 uint8_t                        driveNumber)
{
    if (hyperdos_pc_floppy_controller_drive_number_is_valid(controller, driveNumber))
    {
        controller->diskChanged[driveNumber] = 0u;
    }
}

void hyperdos_pc_floppy_controller_notify_disk_changed(hyperdos_pc_floppy_controller* controller)
{
    hyperdos_pc_floppy_controller_notify_drive_disk_changed(controller, 0u);
}

void hyperdos_pc_floppy_controller_notify_drive_disk_changed(hyperdos_pc_floppy_controller* controller,
                                                             uint8_t                        driveNumber)
{
    if (hyperdos_pc_floppy_controller_drive_number_is_valid(controller, driveNumber))
    {
        controller->diskChanged[driveNumber]      = 1u;
        controller->presentCylinders[driveNumber] = 0u;
        controller->senseInterruptPending         = 1u;
        controller->senseInterruptDriveNumber     = driveNumber;
    }
}

uint8_t hyperdos_pc_floppy_controller_drive_disk_changed(const hyperdos_pc_floppy_controller* controller,
                                                         uint8_t                              driveNumber)
{
    if (!hyperdos_pc_floppy_controller_drive_number_is_valid(controller, driveNumber))
    {
        return 0u;
    }
    return controller->diskChanged[driveNumber];
}

static void hyperdos_pc_floppy_controller_push_result_byte(hyperdos_pc_floppy_controller* controller, uint8_t value)
{
    if (controller != NULL && controller->resultCount < HYPERDOS_PC_FLOPPY_CONTROLLER_RESULT_CAPACITY)
    {
        controller->resultBytes[controller->resultCount] = value;
        ++controller->resultCount;
    }
}

static void hyperdos_pc_floppy_controller_finish_command(hyperdos_pc_floppy_controller* controller)
{
    uint8_t command        = 0u;
    uint8_t driveNumber    = 0u;
    size_t  parameterCount = 0u;

    if (controller == NULL)
    {
        return;
    }

    parameterCount = controller->parameterCount;
    command        = (uint8_t)(controller->commandRegister & HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_MASK);
    driveNumber    = parameterCount != 0u ? (uint8_t)(controller->parameterBytes[0] &
                                                   HYPERDOS_PC_FLOPPY_CONTROLLER_DIGITAL_OUTPUT_DRIVE_SELECT_MASK)
                                          : controller->selectedDriveNumber;
    hyperdos_pc_floppy_controller_clear_results(controller);
    controller->expectedParameterCount = 0u;
    controller->parameterCount         = 0u;

    if (command == HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_SENSE_INTERRUPT_STATUS)
    {
        driveNumber = controller->senseInterruptDriveNumber;
        hyperdos_pc_floppy_controller_trace(controller,
                                            "floppy-controller sense-interrupt drive=%u pending=%u cylinder=%u",
                                            driveNumber,
                                            controller->senseInterruptPending,
                                            hyperdos_pc_floppy_controller_drive_number_is_valid(controller, driveNumber)
                                                    ? controller->presentCylinders[driveNumber]
                                                    : 0u);
        hyperdos_pc_floppy_controller_push_result_byte(
                controller,
                (uint8_t)((controller->senseInterruptPending
                                   ? HYPERDOS_PC_FLOPPY_CONTROLLER_STATUS_INTERRUPT_CODE_SEEK_END
                                   : HYPERDOS_PC_FLOPPY_CONTROLLER_STATUS_ZERO) |
                          driveNumber));
        hyperdos_pc_floppy_controller_push_result_byte(controller,
                                                       hyperdos_pc_floppy_controller_drive_number_is_valid(controller,
                                                                                                           driveNumber)
                                                               ? controller->presentCylinders[driveNumber]
                                                               : 0u);
        controller->senseInterruptPending = 0u;
        return;
    }
    if (command == HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_SENSE_DRIVE_STATUS)
    {
        hyperdos_pc_floppy_controller_trace(controller,
                                            "floppy-controller sense-drive-status drive=%u cylinder=%u disk-changed=%u",
                                            driveNumber,
                                            hyperdos_pc_floppy_controller_drive_number_is_valid(controller, driveNumber)
                                                    ? controller->presentCylinders[driveNumber]
                                                    : 0u,
                                            hyperdos_pc_floppy_controller_drive_disk_changed(controller, driveNumber));
        hyperdos_pc_floppy_controller_push_result_byte(
                controller,
                (uint8_t)(HYPERDOS_PC_FLOPPY_CONTROLLER_STATUS_THREE_READY |
                          (hyperdos_pc_floppy_controller_drive_number_is_valid(controller, driveNumber) &&
                                           controller->presentCylinders[driveNumber] == 0u
                                   ? HYPERDOS_PC_FLOPPY_CONTROLLER_STATUS_THREE_TRACK_ZERO
                                   : 0u) |
                          driveNumber));
        return;
    }
    if (command == HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_RECALIBRATE)
    {
        hyperdos_pc_floppy_controller_trace(controller,
                                            "floppy-controller recalibrate drive=%u before disk-changed=%u",
                                            driveNumber,
                                            hyperdos_pc_floppy_controller_drive_disk_changed(controller, driveNumber));
        hyperdos_pc_floppy_controller_acknowledge_drive_disk_change(controller, driveNumber);
        if (hyperdos_pc_floppy_controller_drive_number_is_valid(controller, driveNumber))
        {
            controller->presentCylinders[driveNumber] = 0u;
        }
        controller->senseInterruptPending     = 1u;
        controller->senseInterruptDriveNumber = driveNumber;
        return;
    }
    if (command == HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_SEEK)
    {
        if (parameterCount >= 2u && hyperdos_pc_floppy_controller_drive_number_is_valid(controller, driveNumber))
        {
            controller->presentCylinders[driveNumber] = controller->parameterBytes[1];
        }
        hyperdos_pc_floppy_controller_trace(controller,
                                            "floppy-controller seek drive=%u cylinder=%u before disk-changed=%u",
                                            driveNumber,
                                            hyperdos_pc_floppy_controller_drive_number_is_valid(controller, driveNumber)
                                                    ? controller->presentCylinders[driveNumber]
                                                    : 0u,
                                            hyperdos_pc_floppy_controller_drive_disk_changed(controller, driveNumber));
        hyperdos_pc_floppy_controller_acknowledge_drive_disk_change(controller, driveNumber);
        controller->senseInterruptPending     = 1u;
        controller->senseInterruptDriveNumber = driveNumber;
        return;
    }
    if (command == HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_READ_IDENTIFIER)
    {
        hyperdos_pc_floppy_controller_trace(controller,
                                            "floppy-controller read-identifier drive=%u cylinder=%u disk-changed=%u",
                                            driveNumber,
                                            hyperdos_pc_floppy_controller_drive_number_is_valid(controller, driveNumber)
                                                    ? controller->presentCylinders[driveNumber]
                                                    : 0u,
                                            hyperdos_pc_floppy_controller_drive_disk_changed(controller, driveNumber));
        hyperdos_pc_floppy_controller_push_result_byte(controller, HYPERDOS_PC_FLOPPY_CONTROLLER_STATUS_ZERO);
        hyperdos_pc_floppy_controller_push_result_byte(controller, HYPERDOS_PC_FLOPPY_CONTROLLER_STATUS_ZERO);
        hyperdos_pc_floppy_controller_push_result_byte(controller, HYPERDOS_PC_FLOPPY_CONTROLLER_STATUS_ZERO);
        hyperdos_pc_floppy_controller_push_result_byte(controller,
                                                       hyperdos_pc_floppy_controller_drive_number_is_valid(controller,
                                                                                                           driveNumber)
                                                               ? controller->presentCylinders[driveNumber]
                                                               : 0u);
        hyperdos_pc_floppy_controller_push_result_byte(controller, 0u);
        hyperdos_pc_floppy_controller_push_result_byte(controller, 1u);
        hyperdos_pc_floppy_controller_push_result_byte(controller, 2u);
        return;
    }
    if (command == HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_VERSION)
    {
        hyperdos_pc_floppy_controller_trace(controller, "floppy-controller version");
        hyperdos_pc_floppy_controller_push_result_byte(controller, HYPERDOS_PC_FLOPPY_CONTROLLER_VERSION_765A);
    }
}

static size_t hyperdos_pc_floppy_controller_get_parameter_count(uint8_t commandRegister)
{
    uint8_t command = (uint8_t)(commandRegister & HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_MASK);

    if (command == HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_SPECIFY)
    {
        return 2u;
    }
    if (command == HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_SENSE_DRIVE_STATUS ||
        command == HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_RECALIBRATE ||
        command == HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_READ_IDENTIFIER)
    {
        return 1u;
    }
    if (command == HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_SEEK)
    {
        return 2u;
    }
    if (command == HYPERDOS_PC_FLOPPY_CONTROLLER_COMMAND_CONFIGURE)
    {
        return 3u;
    }
    return 0u;
}

uint8_t hyperdos_pc_floppy_controller_read_input_output_byte(void* device, uint16_t port)
{
    hyperdos_pc_floppy_controller* controller = (hyperdos_pc_floppy_controller*)device;

    if (controller == NULL)
    {
        return 0xFFu;
    }
    if (port == HYPERDOS_PC_FLOPPY_CONTROLLER_DIGITAL_OUTPUT_REGISTER_PORT)
    {
        return controller->digitalOutputRegister;
    }
    if (port == HYPERDOS_PC_FLOPPY_CONTROLLER_MAIN_STATUS_REGISTER_PORT)
    {
        return (uint8_t)(HYPERDOS_PC_FLOPPY_CONTROLLER_STATUS_READY_FOR_TRANSFER |
                         (controller->resultReadIndex < controller->resultCount
                                  ? HYPERDOS_PC_FLOPPY_CONTROLLER_STATUS_DATA_FROM_CONTROLLER
                                  : 0u));
    }
    if (port == HYPERDOS_PC_FLOPPY_CONTROLLER_DATA_REGISTER_PORT)
    {
        uint8_t value = 0u;

        if (controller->resultReadIndex < controller->resultCount)
        {
            value = controller->resultBytes[controller->resultReadIndex];
            ++controller->resultReadIndex;
            if (controller->resultReadIndex >= controller->resultCount)
            {
                hyperdos_pc_floppy_controller_clear_results(controller);
            }
        }
        return value;
    }
    if (port == HYPERDOS_PC_FLOPPY_CONTROLLER_DIGITAL_INPUT_REGISTER_PORT)
    {
        hyperdos_pc_floppy_controller_trace(
                controller,
                "floppy-controller read-digital-input drive=%u disk-changed=%u",
                controller->selectedDriveNumber,
                hyperdos_pc_floppy_controller_drive_disk_changed(controller, controller->selectedDriveNumber));
        return hyperdos_pc_floppy_controller_drive_disk_changed(controller, controller->selectedDriveNumber)
                       ? HYPERDOS_PC_FLOPPY_CONTROLLER_DIGITAL_INPUT_DISK_CHANGED
                       : 0u;
    }
    return 0xFFu;
}

void hyperdos_pc_floppy_controller_write_input_output_byte(void* device, uint16_t port, uint8_t value)
{
    hyperdos_pc_floppy_controller* controller = (hyperdos_pc_floppy_controller*)device;

    if (controller == NULL)
    {
        return;
    }
    if (port == HYPERDOS_PC_FLOPPY_CONTROLLER_DIGITAL_OUTPUT_REGISTER_PORT)
    {
        controller->digitalOutputRegister = value;
        controller->selectedDriveNumber   = (uint8_t)(value &
                                                    HYPERDOS_PC_FLOPPY_CONTROLLER_DIGITAL_OUTPUT_DRIVE_SELECT_MASK);
        return;
    }
    if (port == HYPERDOS_PC_FLOPPY_CONTROLLER_DATA_REGISTER_PORT)
    {
        if (controller->expectedParameterCount != 0u)
        {
            if (controller->parameterCount < HYPERDOS_PC_FLOPPY_CONTROLLER_PARAMETER_CAPACITY)
            {
                controller->parameterBytes[controller->parameterCount] = value;
                ++controller->parameterCount;
            }
            if (controller->parameterCount >= controller->expectedParameterCount)
            {
                hyperdos_pc_floppy_controller_finish_command(controller);
            }
            return;
        }

        controller->commandRegister        = value;
        controller->expectedParameterCount = hyperdos_pc_floppy_controller_get_parameter_count(value);
        controller->parameterCount         = 0u;
        hyperdos_pc_floppy_controller_trace(controller,
                                            "floppy-controller command value=%02X expected-parameters=%zu",
                                            value,
                                            controller->expectedParameterCount);
        if (controller->expectedParameterCount == 0u)
        {
            hyperdos_pc_floppy_controller_finish_command(controller);
        }
    }
}
