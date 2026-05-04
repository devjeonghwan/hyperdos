#ifndef HYPERDOS_PC_MOUSE_DRIVER_H
#define HYPERDOS_PC_MOUSE_DRIVER_H

#include <stdint.h>

#include "hyperdos/x86_processor.h"

enum
{
    HYPERDOS_PC_MOUSE_DRIVER_LEFT_BUTTON            = 0x01u,
    HYPERDOS_PC_MOUSE_DRIVER_RIGHT_BUTTON           = 0x02u,
    HYPERDOS_PC_MOUSE_DRIVER_MIDDLE_BUTTON          = 0x04u,
    HYPERDOS_PC_MOUSE_DRIVER_BUTTON_COUNT           = 3u,
    HYPERDOS_PC_MOUSE_DRIVER_GENERAL_REGISTER_COUNT = 8u
};

typedef struct hyperdos_pc_mouse_driver_button_history
{
    uint16_t pressCount;
    uint16_t releaseCount;
    uint16_t lastPressHorizontalPosition;
    uint16_t lastPressVerticalPosition;
    uint16_t lastReleaseHorizontalPosition;
    uint16_t lastReleaseVerticalPosition;
} hyperdos_pc_mouse_driver_button_history;

typedef struct hyperdos_pc_mouse_driver
{
    uint8_t  installed;
    uint8_t  enabled;
    uint8_t  buttonCount;
    uint8_t  buttonMask;
    int16_t  cursorDisplayCounter;
    int16_t  cursorDisplayCounterBeforeDisable;
    uint16_t horizontalPosition;
    uint16_t verticalPosition;
    uint16_t horizontalMinimum;
    uint16_t horizontalMaximum;
    uint16_t verticalMinimum;
    uint16_t verticalMaximum;
    uint16_t horizontalMickeysPerEightPixels;
    uint16_t verticalMickeysPerEightPixels;
    int32_t  horizontalPixelAccumulator;
    int32_t  verticalPixelAccumulator;
    int16_t  horizontalMotionCounter;
    int16_t  verticalMotionCounter;
    uint16_t eventHandlerMask;
    uint16_t eventHandlerOffset;
    uint16_t eventHandlerSegment;
    uint16_t pendingEventMask;
    int16_t  pendingEventHorizontalMovement;
    int16_t  pendingEventVerticalMovement;
    uint32_t savedEventHandlerGeneralRegisters[HYPERDOS_PC_MOUSE_DRIVER_GENERAL_REGISTER_COUNT];
    uint16_t savedEventHandlerExtraSegment;
    uint16_t savedEventHandlerDataSegment;
    uint16_t savedEventHandlerFlags;
    uint8_t  eventHandlerActive;
    uint8_t  eventHandlerSavedProcessorStateActive;
    uint8_t  textCursorType;
    uint16_t textCursorScreenMask;
    uint16_t textCursorCursorMask;
    uint16_t doubleSpeedThreshold;
    uint16_t horizontalSensitivity;
    uint16_t verticalSensitivity;
    uint16_t interruptRate;
    hyperdos_pc_mouse_driver_button_history buttonHistory[HYPERDOS_PC_MOUSE_DRIVER_BUTTON_COUNT];
} hyperdos_pc_mouse_driver;

void hyperdos_pc_mouse_driver_reset(hyperdos_pc_mouse_driver* mouseDriver);

void hyperdos_pc_mouse_driver_receive_relative_movement(hyperdos_pc_mouse_driver* mouseDriver,
                                                        int16_t                   horizontalMovement,
                                                        int16_t                   verticalMovement,
                                                        uint8_t                   buttonMask);

hyperdos_x86_execution_result hyperdos_pc_mouse_driver_handle_interrupt(hyperdos_pc_mouse_driver* mouseDriver,
                                                                        hyperdos_x86_processor*   processor);

hyperdos_x86_execution_result hyperdos_pc_mouse_driver_handle_backdoor_interrupt(hyperdos_pc_mouse_driver* mouseDriver,
                                                                                 hyperdos_x86_processor*   processor);

hyperdos_x86_execution_result hyperdos_pc_mouse_driver_dispatch_pending_event_handler(
        hyperdos_pc_mouse_driver* mouseDriver,
        hyperdos_x86_processor*   processor);

void hyperdos_pc_mouse_driver_complete_event_handler(hyperdos_pc_mouse_driver* mouseDriver,
                                                     hyperdos_x86_processor*   processor);

int hyperdos_pc_mouse_driver_cursor_is_visible(const hyperdos_pc_mouse_driver* mouseDriver);

int hyperdos_pc_mouse_driver_get_text_cursor_cell(const hyperdos_pc_mouse_driver* mouseDriver,
                                                  uint16_t                        columnCount,
                                                  uint16_t                        rowCount,
                                                  uint16_t*                       cursorColumn,
                                                  uint16_t*                       cursorRow);

#endif
