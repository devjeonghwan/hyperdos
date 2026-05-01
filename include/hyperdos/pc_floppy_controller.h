#ifndef HYPERDOS_PC_FLOPPY_CONTROLLER_H
#define HYPERDOS_PC_FLOPPY_CONTROLLER_H

#include <stddef.h>
#include <stdint.h>

enum
{
    HYPERDOS_PC_FLOPPY_CONTROLLER_PORT                         = 0x03F0u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_PORT_COUNT                   = 8u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_DIGITAL_OUTPUT_REGISTER_PORT = 0x03F2u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_MAIN_STATUS_REGISTER_PORT    = 0x03F4u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_DATA_REGISTER_PORT           = 0x03F5u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_DIGITAL_INPUT_REGISTER_PORT  = 0x03F7u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_PARAMETER_CAPACITY           = 9u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_RESULT_CAPACITY              = 10u,
    HYPERDOS_PC_FLOPPY_CONTROLLER_MAXIMUM_DRIVE_COUNT          = 4u
};

typedef void (*hyperdos_pc_floppy_controller_trace_function)(void* userContext, const char* message);

typedef struct hyperdos_pc_floppy_controller
{
    uint8_t                                      digitalOutputRegister;
    uint8_t                                      commandRegister;
    uint8_t                                      parameterBytes[HYPERDOS_PC_FLOPPY_CONTROLLER_PARAMETER_CAPACITY];
    size_t                                       parameterCount;
    size_t                                       expectedParameterCount;
    uint8_t                                      resultBytes[HYPERDOS_PC_FLOPPY_CONTROLLER_RESULT_CAPACITY];
    size_t                                       resultReadIndex;
    size_t                                       resultCount;
    uint8_t                                      driveCount;
    uint8_t                                      selectedDriveNumber;
    uint8_t                                      presentCylinders[HYPERDOS_PC_FLOPPY_CONTROLLER_MAXIMUM_DRIVE_COUNT];
    uint8_t                                      diskChanged[HYPERDOS_PC_FLOPPY_CONTROLLER_MAXIMUM_DRIVE_COUNT];
    uint8_t                                      senseInterruptPending;
    uint8_t                                      senseInterruptDriveNumber;
    hyperdos_pc_floppy_controller_trace_function traceFunction;
    void*                                        traceUserContext;
} hyperdos_pc_floppy_controller;

void hyperdos_pc_floppy_controller_initialize(hyperdos_pc_floppy_controller* controller);

void hyperdos_pc_floppy_controller_set_trace_function(hyperdos_pc_floppy_controller*               controller,
                                                      hyperdos_pc_floppy_controller_trace_function traceFunction,
                                                      void*                                        traceUserContext);

void hyperdos_pc_floppy_controller_reset(hyperdos_pc_floppy_controller* controller);

void hyperdos_pc_floppy_controller_clear_results(hyperdos_pc_floppy_controller* controller);

void hyperdos_pc_floppy_controller_set_drive_count(hyperdos_pc_floppy_controller* controller, uint8_t driveCount);

void hyperdos_pc_floppy_controller_acknowledge_disk_change(hyperdos_pc_floppy_controller* controller);

void hyperdos_pc_floppy_controller_acknowledge_drive_disk_change(hyperdos_pc_floppy_controller* controller,
                                                                 uint8_t                        driveNumber);

void hyperdos_pc_floppy_controller_notify_disk_changed(hyperdos_pc_floppy_controller* controller);

void hyperdos_pc_floppy_controller_notify_drive_disk_changed(hyperdos_pc_floppy_controller* controller,
                                                             uint8_t                        driveNumber);

uint8_t hyperdos_pc_floppy_controller_drive_disk_changed(const hyperdos_pc_floppy_controller* controller,
                                                         uint8_t                              driveNumber);

uint8_t hyperdos_pc_floppy_controller_read_input_output_byte(void* device, uint16_t port);

void hyperdos_pc_floppy_controller_write_input_output_byte(void* device, uint16_t port, uint8_t value);

#endif
