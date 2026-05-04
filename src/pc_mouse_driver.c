#include "hyperdos/pc_mouse_driver.h"

#include <stddef.h>
#include <string.h>

#include "hyperdos/pc_bios.h"

enum
{
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_RESET_AND_STATUS                 = 0x0000u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SHOW_CURSOR                      = 0x0001u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_HIDE_CURSOR                      = 0x0002u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_GET_POSITION_AND_BUTTONS         = 0x0003u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_POSITION                     = 0x0004u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_GET_BUTTON_PRESS                 = 0x0005u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_GET_BUTTON_RELEASE               = 0x0006u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_HORIZONTAL_RANGE             = 0x0007u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_VERTICAL_RANGE               = 0x0008u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_GRAPHICS_CURSOR              = 0x0009u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_TEXT_CURSOR                  = 0x000Au,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_READ_MOTION_COUNTERS             = 0x000Bu,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_EVENT_HANDLER                = 0x000Cu,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_MICKEY_PIXEL_RATIO           = 0x000Fu,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_DEFINE_SCREEN_REGION             = 0x0010u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_DOUBLE_SPEED_THRESHOLD       = 0x0013u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_EXCHANGE_EVENT_HANDLER           = 0x0014u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_GET_STATE_STORAGE_SIZE           = 0x0015u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SAVE_STATE                       = 0x0016u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_RESTORE_STATE                    = 0x0017u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_SENSITIVITY                  = 0x001Au,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_GET_SENSITIVITY                  = 0x001Bu,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_INTERRUPT_RATE               = 0x001Cu,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_DISABLE_DRIVER                   = 0x001Fu,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_ENABLE_DRIVER                    = 0x0020u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SOFTWARE_RESET                   = 0x0021u,
    HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_GET_VERSION_TYPE_INTERRUPT       = 0x0024u,
    HYPERDOS_PC_MOUSE_DRIVER_INSTALLED_STATUS                          = 0xFFFFu,
    HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_HORIZONTAL_MINIMUM                = 0u,
    HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_HORIZONTAL_MAXIMUM                = 639u,
    HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_VERTICAL_MINIMUM                  = 0u,
    HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_VERTICAL_MAXIMUM                  = 199u,
    HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_HORIZONTAL_MICKEY_RATIO           = 8u,
    HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_VERTICAL_MICKEY_RATIO             = 16u,
    HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_TEXT_CURSOR_SCREEN_MASK           = 0xFFFFu,
    HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_TEXT_CURSOR_CURSOR_MASK           = 0x7700u,
    HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_DOUBLE_SPEED_THRESHOLD            = 64u,
    HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_SENSITIVITY                       = 50u,
    HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_INTERRUPT_RATE                    = 0u,
    HYPERDOS_PC_MOUSE_DRIVER_VERSION_WORD                              = 0x0800u,
    HYPERDOS_PC_MOUSE_DRIVER_TYPE_PS2                                  = 0x04u,
    HYPERDOS_PC_MOUSE_DRIVER_INTERRUPT_REQUEST_LINE                    = 12u,
    HYPERDOS_PC_MOUSE_DRIVER_TEXT_CELL_WIDTH                           = 8u,
    HYPERDOS_PC_MOUSE_DRIVER_TEXT_CELL_HEIGHT                          = 8u,
    HYPERDOS_PC_MOUSE_DRIVER_STATE_BYTE_COUNT                          = 96u,
    HYPERDOS_PC_MOUSE_DRIVER_STATE_VERSION                             = 2u,
    HYPERDOS_PC_MOUSE_DRIVER_EVENT_MOVEMENT                            = 0x0001u,
    HYPERDOS_PC_MOUSE_DRIVER_EVENT_LEFT_PRESS                          = 0x0002u,
    HYPERDOS_PC_MOUSE_DRIVER_EVENT_LEFT_RELEASE                        = 0x0004u,
    HYPERDOS_PC_MOUSE_DRIVER_EVENT_RIGHT_PRESS                         = 0x0008u,
    HYPERDOS_PC_MOUSE_DRIVER_EVENT_RIGHT_RELEASE                       = 0x0010u,
    HYPERDOS_PC_MOUSE_DRIVER_EVENT_MIDDLE_PRESS                        = 0x0020u,
    HYPERDOS_PC_MOUSE_DRIVER_EVENT_MIDDLE_RELEASE                      = 0x0040u,
    HYPERDOS_PC_MOUSE_DRIVER_BYTE_REGISTER_HIGH_PART                   = 0x04u,
    HYPERDOS_PC_MOUSE_DRIVER_BACKDOOR_DATA_POINTER_STACK_OFFSET        = 0x04u,
    HYPERDOS_PC_MOUSE_DRIVER_BACKDOOR_COUNTER_POINTER_STACK_OFFSET     = 0x06u,
    HYPERDOS_PC_MOUSE_DRIVER_BACKDOOR_BASE_POINTER_STACK_OFFSET        = 0x08u,
    HYPERDOS_PC_MOUSE_DRIVER_BACKDOOR_ACCUMULATOR_POINTER_STACK_OFFSET = 0x0Au
};

static uint8_t hyperdos_pc_mouse_driver_button_masks[HYPERDOS_PC_MOUSE_DRIVER_BUTTON_COUNT] =
        {HYPERDOS_PC_MOUSE_DRIVER_LEFT_BUTTON,
         HYPERDOS_PC_MOUSE_DRIVER_RIGHT_BUTTON,
         HYPERDOS_PC_MOUSE_DRIVER_MIDDLE_BUTTON};

static uint16_t hyperdos_pc_mouse_driver_button_press_events[HYPERDOS_PC_MOUSE_DRIVER_BUTTON_COUNT] =
        {HYPERDOS_PC_MOUSE_DRIVER_EVENT_LEFT_PRESS,
         HYPERDOS_PC_MOUSE_DRIVER_EVENT_RIGHT_PRESS,
         HYPERDOS_PC_MOUSE_DRIVER_EVENT_MIDDLE_PRESS};

static uint16_t hyperdos_pc_mouse_driver_button_release_events[HYPERDOS_PC_MOUSE_DRIVER_BUTTON_COUNT] =
        {HYPERDOS_PC_MOUSE_DRIVER_EVENT_LEFT_RELEASE,
         HYPERDOS_PC_MOUSE_DRIVER_EVENT_RIGHT_RELEASE,
         HYPERDOS_PC_MOUSE_DRIVER_EVENT_MIDDLE_RELEASE};

static uint8_t hyperdos_pc_mouse_driver_normalize_button_mask(uint8_t buttonMask)
{
    return buttonMask & (HYPERDOS_PC_MOUSE_DRIVER_LEFT_BUTTON | HYPERDOS_PC_MOUSE_DRIVER_RIGHT_BUTTON |
                         HYPERDOS_PC_MOUSE_DRIVER_MIDDLE_BUTTON);
}

static uint16_t hyperdos_pc_mouse_driver_get_range_center(uint16_t rangeMinimum, uint16_t rangeMaximum)
{
    return (uint16_t)(rangeMinimum + ((rangeMaximum - rangeMinimum + 1u) / 2u));
}

static uint16_t hyperdos_pc_mouse_driver_clamp_unsigned_position(uint16_t value,
                                                                 uint16_t rangeMinimum,
                                                                 uint16_t rangeMaximum)
{
    if (value < rangeMinimum)
    {
        return rangeMinimum;
    }
    if (value > rangeMaximum)
    {
        return rangeMaximum;
    }
    return value;
}

static uint16_t hyperdos_pc_mouse_driver_clamp_signed_position(int32_t  value,
                                                               uint16_t rangeMinimum,
                                                               uint16_t rangeMaximum)
{
    if (value < (int32_t)rangeMinimum)
    {
        return rangeMinimum;
    }
    if (value > (int32_t)rangeMaximum)
    {
        return rangeMaximum;
    }
    return (uint16_t)value;
}

static int16_t hyperdos_pc_mouse_driver_add_saturated_int16(int16_t value, int16_t addend)
{
    int32_t result = (int32_t)value + addend;

    if (result > INT16_MAX)
    {
        return INT16_MAX;
    }
    if (result < INT16_MIN)
    {
        return INT16_MIN;
    }
    return (int16_t)result;
}

static uint16_t hyperdos_pc_mouse_driver_add_saturated_uint16(uint16_t value, uint16_t addend)
{
    uint32_t result = (uint32_t)value + addend;

    return result > UINT16_MAX ? UINT16_MAX : (uint16_t)result;
}

static int16_t hyperdos_pc_mouse_driver_convert_mickeys_to_pixels(int32_t* accumulator,
                                                                  int16_t  movement,
                                                                  uint16_t mickeysPerEightPixels)
{
    int32_t scaledMovement = 0;
    int32_t pixelMovement  = 0;

    if (accumulator == NULL)
    {
        return 0;
    }
    if (mickeysPerEightPixels == 0u)
    {
        mickeysPerEightPixels = 1u;
    }

    scaledMovement  = (int32_t)movement * HYPERDOS_PC_MOUSE_DRIVER_TEXT_CELL_WIDTH;
    *accumulator   += scaledMovement;
    pixelMovement   = *accumulator / (int32_t)mickeysPerEightPixels;
    *accumulator   -= pixelMovement * (int32_t)mickeysPerEightPixels;
    if (pixelMovement > INT16_MAX)
    {
        return INT16_MAX;
    }
    if (pixelMovement < INT16_MIN)
    {
        return INT16_MIN;
    }
    return (int16_t)pixelMovement;
}

static uint8_t hyperdos_pc_mouse_driver_get_button_mask_from_number(uint16_t buttonNumber)
{
    if (buttonNumber >= HYPERDOS_PC_MOUSE_DRIVER_BUTTON_COUNT)
    {
        return 0u;
    }
    return hyperdos_pc_mouse_driver_button_masks[buttonNumber];
}

static size_t hyperdos_pc_mouse_driver_get_button_index_from_mask(uint8_t buttonMask)
{
    size_t buttonIndex = 0u;

    for (buttonIndex = 0u; buttonIndex < HYPERDOS_PC_MOUSE_DRIVER_BUTTON_COUNT; ++buttonIndex)
    {
        if (hyperdos_pc_mouse_driver_button_masks[buttonIndex] == buttonMask)
        {
            return buttonIndex;
        }
    }
    return HYPERDOS_PC_MOUSE_DRIVER_BUTTON_COUNT;
}

static void hyperdos_pc_mouse_driver_clamp_current_position(hyperdos_pc_mouse_driver* mouseDriver)
{
    mouseDriver->horizontalPosition = hyperdos_pc_mouse_driver_clamp_unsigned_position(mouseDriver->horizontalPosition,
                                                                                       mouseDriver->horizontalMinimum,
                                                                                       mouseDriver->horizontalMaximum);
    mouseDriver->verticalPosition   = hyperdos_pc_mouse_driver_clamp_unsigned_position(mouseDriver->verticalPosition,
                                                                                     mouseDriver->verticalMinimum,
                                                                                     mouseDriver->verticalMaximum);
}

static void hyperdos_pc_mouse_driver_reset_geometry(hyperdos_pc_mouse_driver* mouseDriver)
{
    mouseDriver->horizontalMinimum  = HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_HORIZONTAL_MINIMUM;
    mouseDriver->horizontalMaximum  = HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_HORIZONTAL_MAXIMUM;
    mouseDriver->verticalMinimum    = HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_VERTICAL_MINIMUM;
    mouseDriver->verticalMaximum    = HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_VERTICAL_MAXIMUM;
    mouseDriver->horizontalPosition = hyperdos_pc_mouse_driver_get_range_center(mouseDriver->horizontalMinimum,
                                                                                mouseDriver->horizontalMaximum);
    mouseDriver->verticalPosition   = hyperdos_pc_mouse_driver_get_range_center(mouseDriver->verticalMinimum,
                                                                              mouseDriver->verticalMaximum);
}

void hyperdos_pc_mouse_driver_reset(hyperdos_pc_mouse_driver* mouseDriver)
{
    if (mouseDriver == NULL)
    {
        return;
    }

    memset(mouseDriver, 0, sizeof(*mouseDriver));
    mouseDriver->installed                         = 1u;
    mouseDriver->enabled                           = 1u;
    mouseDriver->buttonCount                       = HYPERDOS_PC_MOUSE_DRIVER_BUTTON_COUNT;
    mouseDriver->cursorDisplayCounter              = -1;
    mouseDriver->cursorDisplayCounterBeforeDisable = -1;
    mouseDriver->horizontalMickeysPerEightPixels   = HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_HORIZONTAL_MICKEY_RATIO;
    mouseDriver->verticalMickeysPerEightPixels     = HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_VERTICAL_MICKEY_RATIO;
    mouseDriver->textCursorScreenMask              = HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_TEXT_CURSOR_SCREEN_MASK;
    mouseDriver->textCursorCursorMask              = HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_TEXT_CURSOR_CURSOR_MASK;
    mouseDriver->doubleSpeedThreshold              = HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_DOUBLE_SPEED_THRESHOLD;
    mouseDriver->horizontalSensitivity             = HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_SENSITIVITY;
    mouseDriver->verticalSensitivity               = HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_SENSITIVITY;
    mouseDriver->interruptRate                     = HYPERDOS_PC_MOUSE_DRIVER_DEFAULT_INTERRUPT_RATE;
    hyperdos_pc_mouse_driver_reset_geometry(mouseDriver);
}

static void hyperdos_pc_mouse_driver_queue_event(hyperdos_pc_mouse_driver* mouseDriver,
                                                 uint16_t                  eventMask,
                                                 int16_t                   horizontalMovement,
                                                 int16_t                   verticalMovement)
{
    uint16_t matchedEventMask = 0u;

    if (mouseDriver == NULL || mouseDriver->eventHandlerMask == 0u ||
        (mouseDriver->eventHandlerOffset == 0u && mouseDriver->eventHandlerSegment == 0u))
    {
        return;
    }

    matchedEventMask = eventMask & mouseDriver->eventHandlerMask;
    if (matchedEventMask == 0u)
    {
        return;
    }

    mouseDriver->pendingEventMask |= matchedEventMask;
    mouseDriver->pendingEventHorizontalMovement =
            hyperdos_pc_mouse_driver_add_saturated_int16(mouseDriver->pendingEventHorizontalMovement,
                                                         horizontalMovement);
    mouseDriver->pendingEventVerticalMovement =
            hyperdos_pc_mouse_driver_add_saturated_int16(mouseDriver->pendingEventVerticalMovement, verticalMovement);
}

static uint16_t hyperdos_pc_mouse_driver_record_button_changes(hyperdos_pc_mouse_driver* mouseDriver,
                                                               uint8_t                   previousButtonMask,
                                                               uint8_t                   currentButtonMask)
{
    uint16_t eventMask   = 0u;
    size_t   buttonIndex = 0u;

    for (buttonIndex = 0u; buttonIndex < HYPERDOS_PC_MOUSE_DRIVER_BUTTON_COUNT; ++buttonIndex)
    {
        uint8_t buttonMask        = hyperdos_pc_mouse_driver_button_masks[buttonIndex];
        uint8_t previousButtonSet = (previousButtonMask & buttonMask) != 0u ? 1u : 0u;
        uint8_t currentButtonSet  = (currentButtonMask & buttonMask) != 0u ? 1u : 0u;

        if (previousButtonSet == currentButtonSet)
        {
            continue;
        }
        if (currentButtonSet != 0u)
        {
            mouseDriver->buttonHistory[buttonIndex].pressCount =
                    hyperdos_pc_mouse_driver_add_saturated_uint16(mouseDriver->buttonHistory[buttonIndex].pressCount,
                                                                  1u);
            mouseDriver->buttonHistory[buttonIndex].lastPressHorizontalPosition = mouseDriver->horizontalPosition;
            mouseDriver->buttonHistory[buttonIndex].lastPressVerticalPosition   = mouseDriver->verticalPosition;
            eventMask |= hyperdos_pc_mouse_driver_button_press_events[buttonIndex];
        }
        else
        {
            mouseDriver->buttonHistory[buttonIndex].releaseCount =
                    hyperdos_pc_mouse_driver_add_saturated_uint16(mouseDriver->buttonHistory[buttonIndex].releaseCount,
                                                                  1u);
            mouseDriver->buttonHistory[buttonIndex].lastReleaseHorizontalPosition = mouseDriver->horizontalPosition;
            mouseDriver->buttonHistory[buttonIndex].lastReleaseVerticalPosition   = mouseDriver->verticalPosition;
            eventMask |= hyperdos_pc_mouse_driver_button_release_events[buttonIndex];
        }
    }
    return eventMask;
}

void hyperdos_pc_mouse_driver_receive_relative_movement(hyperdos_pc_mouse_driver* mouseDriver,
                                                        int16_t                   horizontalMovement,
                                                        int16_t                   verticalMovement,
                                                        uint8_t                   buttonMask)
{
    uint8_t  normalizedButtonMask = 0u;
    uint8_t  previousButtonMask   = 0u;
    int16_t  horizontalPixels     = 0;
    int16_t  verticalPixels       = 0;
    uint16_t eventMask            = 0u;

    if (mouseDriver == NULL)
    {
        return;
    }

    normalizedButtonMask    = hyperdos_pc_mouse_driver_normalize_button_mask(buttonMask);
    previousButtonMask      = mouseDriver->buttonMask;
    mouseDriver->buttonMask = normalizedButtonMask;
    if (mouseDriver->installed == 0u || mouseDriver->enabled == 0u)
    {
        return;
    }

    mouseDriver->horizontalMotionCounter =
            hyperdos_pc_mouse_driver_add_saturated_int16(mouseDriver->horizontalMotionCounter, horizontalMovement);
    mouseDriver->verticalMotionCounter =
            hyperdos_pc_mouse_driver_add_saturated_int16(mouseDriver->verticalMotionCounter, verticalMovement);

    horizontalPixels = hyperdos_pc_mouse_driver_convert_mickeys_to_pixels(&mouseDriver->horizontalPixelAccumulator,
                                                                          horizontalMovement,
                                                                          mouseDriver->horizontalMickeysPerEightPixels);
    verticalPixels   = hyperdos_pc_mouse_driver_convert_mickeys_to_pixels(&mouseDriver->verticalPixelAccumulator,
                                                                        (int16_t)-verticalMovement,
                                                                        mouseDriver->verticalMickeysPerEightPixels);
    if (horizontalPixels != 0 || verticalPixels != 0)
    {
        mouseDriver->horizontalPosition = hyperdos_pc_mouse_driver_clamp_signed_position(
                (int32_t)mouseDriver->horizontalPosition + horizontalPixels,
                mouseDriver->horizontalMinimum,
                mouseDriver->horizontalMaximum);
        mouseDriver->verticalPosition =
                hyperdos_pc_mouse_driver_clamp_signed_position((int32_t)mouseDriver->verticalPosition + verticalPixels,
                                                               mouseDriver->verticalMinimum,
                                                               mouseDriver->verticalMaximum);
        eventMask |= HYPERDOS_PC_MOUSE_DRIVER_EVENT_MOVEMENT;
    }
    eventMask |= hyperdos_pc_mouse_driver_record_button_changes(mouseDriver, previousButtonMask, normalizedButtonMask);
    hyperdos_pc_mouse_driver_queue_event(mouseDriver, eventMask, horizontalMovement, verticalMovement);
}

static void hyperdos_pc_mouse_driver_set_range(uint16_t* rangeMinimum,
                                               uint16_t* rangeMaximum,
                                               uint16_t  requestedMinimum,
                                               uint16_t  requestedMaximum)
{
    if (requestedMinimum <= requestedMaximum)
    {
        *rangeMinimum = requestedMinimum;
        *rangeMaximum = requestedMaximum;
    }
    else
    {
        *rangeMinimum = requestedMaximum;
        *rangeMaximum = requestedMinimum;
    }
}

static void hyperdos_pc_mouse_driver_get_position_and_buttons(hyperdos_pc_mouse_driver* mouseDriver,
                                                              hyperdos_x86_processor*   processor)
{
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE, mouseDriver->buttonMask);
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
                                           mouseDriver->horizontalPosition);
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                           mouseDriver->verticalPosition);
}

static void hyperdos_pc_mouse_driver_get_button_history(hyperdos_pc_mouse_driver* mouseDriver,
                                                        hyperdos_x86_processor*   processor,
                                                        uint16_t                  buttonNumber,
                                                        int                       releaseHistory)
{
    uint8_t buttonMask  = hyperdos_pc_mouse_driver_get_button_mask_from_number(buttonNumber);
    size_t  buttonIndex = hyperdos_pc_mouse_driver_get_button_index_from_mask(buttonMask);

    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           mouseDriver->buttonMask);
    if (buttonIndex >= HYPERDOS_PC_MOUSE_DRIVER_BUTTON_COUNT)
    {
        hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE, 0u);
        hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER, 0u);
        hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA, 0u);
        return;
    }

    if (releaseHistory)
    {
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_BASE,
                                               mouseDriver->buttonHistory[buttonIndex].releaseCount);
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
                                               mouseDriver->buttonHistory[buttonIndex].lastReleaseHorizontalPosition);
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                               mouseDriver->buttonHistory[buttonIndex].lastReleaseVerticalPosition);
        mouseDriver->buttonHistory[buttonIndex].releaseCount = 0u;
    }
    else
    {
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_BASE,
                                               mouseDriver->buttonHistory[buttonIndex].pressCount);
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
                                               mouseDriver->buttonHistory[buttonIndex].lastPressHorizontalPosition);
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                               mouseDriver->buttonHistory[buttonIndex].lastPressVerticalPosition);
        mouseDriver->buttonHistory[buttonIndex].pressCount = 0u;
    }
}

static void hyperdos_pc_mouse_driver_store_uint16(uint8_t* stateBytes, size_t* stateByteOffset, uint16_t value)
{
    stateBytes[*stateByteOffset] = (uint8_t)(value & HYPERDOS_X86_LOW_BYTE_MASK);
    ++*stateByteOffset;
    stateBytes[*stateByteOffset] = (uint8_t)(value >> HYPERDOS_X86_BYTE_BIT_COUNT);
    ++*stateByteOffset;
}

static void hyperdos_pc_mouse_driver_store_int16(uint8_t* stateBytes, size_t* stateByteOffset, int16_t value)
{
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, stateByteOffset, (uint16_t)value);
}

static void hyperdos_pc_mouse_driver_store_int32(uint8_t* stateBytes, size_t* stateByteOffset, int32_t value)
{
    uint32_t unsignedValue = (uint32_t)value;

    stateBytes[*stateByteOffset] = (uint8_t)(unsignedValue & HYPERDOS_X86_LOW_BYTE_MASK);
    ++*stateByteOffset;
    stateBytes[*stateByteOffset] = (uint8_t)((unsignedValue >> HYPERDOS_X86_BYTE_BIT_COUNT) &
                                             HYPERDOS_X86_LOW_BYTE_MASK);
    ++*stateByteOffset;
    stateBytes[*stateByteOffset] = (uint8_t)((unsignedValue >> 16u) & HYPERDOS_X86_LOW_BYTE_MASK);
    ++*stateByteOffset;
    stateBytes[*stateByteOffset] = (uint8_t)((unsignedValue >> 24u) & HYPERDOS_X86_LOW_BYTE_MASK);
    ++*stateByteOffset;
}

static uint16_t hyperdos_pc_mouse_driver_load_uint16(const uint8_t* stateBytes, size_t* stateByteOffset)
{
    uint16_t value = stateBytes[*stateByteOffset];

    ++*stateByteOffset;
    value |= (uint16_t)(stateBytes[*stateByteOffset] << HYPERDOS_X86_BYTE_BIT_COUNT);
    ++*stateByteOffset;
    return value;
}

static int16_t hyperdos_pc_mouse_driver_load_int16(const uint8_t* stateBytes, size_t* stateByteOffset)
{
    return (int16_t)hyperdos_pc_mouse_driver_load_uint16(stateBytes, stateByteOffset);
}

static int32_t hyperdos_pc_mouse_driver_load_int32(const uint8_t* stateBytes, size_t* stateByteOffset)
{
    uint32_t value = stateBytes[*stateByteOffset];

    ++*stateByteOffset;
    value |= (uint32_t)stateBytes[*stateByteOffset] << HYPERDOS_X86_BYTE_BIT_COUNT;
    ++*stateByteOffset;
    value |= (uint32_t)stateBytes[*stateByteOffset] << 16u;
    ++*stateByteOffset;
    value |= (uint32_t)stateBytes[*stateByteOffset] << 24u;
    ++*stateByteOffset;
    return (int32_t)value;
}

static int hyperdos_pc_mouse_driver_write_state_bytes(hyperdos_x86_processor* processor, const uint8_t* stateBytes)
{
    size_t   stateByteOffset   = 0u;
    uint16_t destinationOffset = hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA);

    for (stateByteOffset = 0u; stateByteOffset < HYPERDOS_PC_MOUSE_DRIVER_STATE_BYTE_COUNT; ++stateByteOffset)
    {
        if (hyperdos_x86_write_memory_byte(processor,
                                           HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                           destinationOffset,
                                           stateBytes[stateByteOffset]) != HYPERDOS_X86_EXECUTION_OK)
        {
            return 0;
        }
        ++destinationOffset;
    }
    return 1;
}

static int hyperdos_pc_mouse_driver_read_state_bytes(const hyperdos_x86_processor* processor, uint8_t* stateBytes)
{
    size_t   stateByteOffset = 0u;
    uint16_t sourceOffset    = hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA);

    for (stateByteOffset = 0u; stateByteOffset < HYPERDOS_PC_MOUSE_DRIVER_STATE_BYTE_COUNT; ++stateByteOffset)
    {
        if (hyperdos_x86_read_memory_byte(processor,
                                          HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                          sourceOffset,
                                          &stateBytes[stateByteOffset]) != HYPERDOS_X86_EXECUTION_OK)
        {
            return 0;
        }
        ++sourceOffset;
    }
    return 1;
}

static int hyperdos_pc_mouse_driver_save_state(hyperdos_pc_mouse_driver* mouseDriver, hyperdos_x86_processor* processor)
{
    uint8_t stateBytes[HYPERDOS_PC_MOUSE_DRIVER_STATE_BYTE_COUNT];
    size_t  stateByteOffset = 0u;
    size_t  buttonIndex     = 0u;

    memset(stateBytes, 0, sizeof(stateBytes));
    stateBytes[stateByteOffset++] = 'H';
    stateBytes[stateByteOffset++] = 'D';
    stateBytes[stateByteOffset++] = 'M';
    stateBytes[stateByteOffset++] = 'S';
    stateBytes[stateByteOffset++] = HYPERDOS_PC_MOUSE_DRIVER_STATE_VERSION;
    stateBytes[stateByteOffset++] = mouseDriver->enabled;
    stateBytes[stateByteOffset++] = mouseDriver->buttonMask;
    hyperdos_pc_mouse_driver_store_int16(stateBytes, &stateByteOffset, mouseDriver->cursorDisplayCounter);
    hyperdos_pc_mouse_driver_store_int16(stateBytes, &stateByteOffset, mouseDriver->cursorDisplayCounterBeforeDisable);
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, &stateByteOffset, mouseDriver->horizontalPosition);
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, &stateByteOffset, mouseDriver->verticalPosition);
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, &stateByteOffset, mouseDriver->horizontalMinimum);
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, &stateByteOffset, mouseDriver->horizontalMaximum);
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, &stateByteOffset, mouseDriver->verticalMinimum);
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, &stateByteOffset, mouseDriver->verticalMaximum);
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, &stateByteOffset, mouseDriver->horizontalMickeysPerEightPixels);
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, &stateByteOffset, mouseDriver->verticalMickeysPerEightPixels);
    hyperdos_pc_mouse_driver_store_int32(stateBytes, &stateByteOffset, mouseDriver->horizontalPixelAccumulator);
    hyperdos_pc_mouse_driver_store_int32(stateBytes, &stateByteOffset, mouseDriver->verticalPixelAccumulator);
    hyperdos_pc_mouse_driver_store_int16(stateBytes, &stateByteOffset, mouseDriver->horizontalMotionCounter);
    hyperdos_pc_mouse_driver_store_int16(stateBytes, &stateByteOffset, mouseDriver->verticalMotionCounter);
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, &stateByteOffset, mouseDriver->eventHandlerMask);
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, &stateByteOffset, mouseDriver->eventHandlerOffset);
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, &stateByteOffset, mouseDriver->eventHandlerSegment);
    stateBytes[stateByteOffset++] = mouseDriver->textCursorType;
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, &stateByteOffset, mouseDriver->textCursorScreenMask);
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, &stateByteOffset, mouseDriver->textCursorCursorMask);
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, &stateByteOffset, mouseDriver->doubleSpeedThreshold);
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, &stateByteOffset, mouseDriver->horizontalSensitivity);
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, &stateByteOffset, mouseDriver->verticalSensitivity);
    hyperdos_pc_mouse_driver_store_uint16(stateBytes, &stateByteOffset, mouseDriver->interruptRate);
    for (buttonIndex = 0u; buttonIndex < HYPERDOS_PC_MOUSE_DRIVER_BUTTON_COUNT; ++buttonIndex)
    {
        hyperdos_pc_mouse_driver_store_uint16(stateBytes,
                                              &stateByteOffset,
                                              mouseDriver->buttonHistory[buttonIndex].pressCount);
        hyperdos_pc_mouse_driver_store_uint16(stateBytes,
                                              &stateByteOffset,
                                              mouseDriver->buttonHistory[buttonIndex].releaseCount);
        hyperdos_pc_mouse_driver_store_uint16(stateBytes,
                                              &stateByteOffset,
                                              mouseDriver->buttonHistory[buttonIndex].lastPressHorizontalPosition);
        hyperdos_pc_mouse_driver_store_uint16(stateBytes,
                                              &stateByteOffset,
                                              mouseDriver->buttonHistory[buttonIndex].lastPressVerticalPosition);
        hyperdos_pc_mouse_driver_store_uint16(stateBytes,
                                              &stateByteOffset,
                                              mouseDriver->buttonHistory[buttonIndex].lastReleaseHorizontalPosition);
        hyperdos_pc_mouse_driver_store_uint16(stateBytes,
                                              &stateByteOffset,
                                              mouseDriver->buttonHistory[buttonIndex].lastReleaseVerticalPosition);
    }
    return hyperdos_pc_mouse_driver_write_state_bytes(processor, stateBytes);
}

static int hyperdos_pc_mouse_driver_restore_state(hyperdos_pc_mouse_driver*     mouseDriver,
                                                  const hyperdos_x86_processor* processor)
{
    uint8_t stateBytes[HYPERDOS_PC_MOUSE_DRIVER_STATE_BYTE_COUNT];
    size_t  stateByteOffset = 0u;
    size_t  buttonIndex     = 0u;

    if (!hyperdos_pc_mouse_driver_read_state_bytes(processor, stateBytes))
    {
        return 0;
    }
    if (stateBytes[0] != 'H' || stateBytes[1] != 'D' || stateBytes[2] != 'M' || stateBytes[3] != 'S' ||
        stateBytes[4] != HYPERDOS_PC_MOUSE_DRIVER_STATE_VERSION)
    {
        return 0;
    }

    stateByteOffset                   = 5u;
    mouseDriver->installed            = 1u;
    mouseDriver->buttonCount          = HYPERDOS_PC_MOUSE_DRIVER_BUTTON_COUNT;
    mouseDriver->enabled              = stateBytes[stateByteOffset++] != 0u ? 1u : 0u;
    mouseDriver->buttonMask           = hyperdos_pc_mouse_driver_normalize_button_mask(stateBytes[stateByteOffset++]);
    mouseDriver->cursorDisplayCounter = hyperdos_pc_mouse_driver_load_int16(stateBytes, &stateByteOffset);
    mouseDriver->cursorDisplayCounterBeforeDisable = hyperdos_pc_mouse_driver_load_int16(stateBytes, &stateByteOffset);
    mouseDriver->horizontalPosition                = hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    mouseDriver->verticalPosition                  = hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    mouseDriver->horizontalMinimum                 = hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    mouseDriver->horizontalMaximum                 = hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    mouseDriver->verticalMinimum                   = hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    mouseDriver->verticalMaximum                   = hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    mouseDriver->horizontalMickeysPerEightPixels   = hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    mouseDriver->verticalMickeysPerEightPixels     = hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    mouseDriver->horizontalPixelAccumulator        = hyperdos_pc_mouse_driver_load_int32(stateBytes, &stateByteOffset);
    mouseDriver->verticalPixelAccumulator          = hyperdos_pc_mouse_driver_load_int32(stateBytes, &stateByteOffset);
    mouseDriver->horizontalMotionCounter           = hyperdos_pc_mouse_driver_load_int16(stateBytes, &stateByteOffset);
    mouseDriver->verticalMotionCounter             = hyperdos_pc_mouse_driver_load_int16(stateBytes, &stateByteOffset);
    mouseDriver->eventHandlerMask                  = hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    mouseDriver->eventHandlerOffset                = hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    mouseDriver->eventHandlerSegment               = hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    mouseDriver->textCursorType                    = stateBytes[stateByteOffset++];
    mouseDriver->textCursorScreenMask              = hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    mouseDriver->textCursorCursorMask              = hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    mouseDriver->doubleSpeedThreshold              = hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    mouseDriver->horizontalSensitivity             = hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    mouseDriver->verticalSensitivity               = hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    mouseDriver->interruptRate                     = hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    mouseDriver->pendingEventMask                  = 0u;
    mouseDriver->pendingEventHorizontalMovement    = 0;
    mouseDriver->pendingEventVerticalMovement      = 0;
    mouseDriver->eventHandlerActive                = 0u;
    mouseDriver->eventHandlerSavedProcessorStateActive = 0u;
    for (buttonIndex = 0u; buttonIndex < HYPERDOS_PC_MOUSE_DRIVER_BUTTON_COUNT; ++buttonIndex)
    {
        mouseDriver->buttonHistory[buttonIndex].pressCount   = hyperdos_pc_mouse_driver_load_uint16(stateBytes,
                                                                                                  &stateByteOffset);
        mouseDriver->buttonHistory[buttonIndex].releaseCount = hyperdos_pc_mouse_driver_load_uint16(stateBytes,
                                                                                                    &stateByteOffset);
        mouseDriver->buttonHistory[buttonIndex].lastPressHorizontalPosition =
                hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
        mouseDriver->buttonHistory[buttonIndex].lastPressVerticalPosition =
                hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
        mouseDriver->buttonHistory[buttonIndex].lastReleaseHorizontalPosition =
                hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
        mouseDriver->buttonHistory[buttonIndex].lastReleaseVerticalPosition =
                hyperdos_pc_mouse_driver_load_uint16(stateBytes, &stateByteOffset);
    }
    if (mouseDriver->horizontalMickeysPerEightPixels == 0u)
    {
        mouseDriver->horizontalMickeysPerEightPixels = 1u;
    }
    if (mouseDriver->verticalMickeysPerEightPixels == 0u)
    {
        mouseDriver->verticalMickeysPerEightPixels = 1u;
    }
    if (mouseDriver->horizontalMinimum > mouseDriver->horizontalMaximum)
    {
        hyperdos_pc_mouse_driver_set_range(&mouseDriver->horizontalMinimum,
                                           &mouseDriver->horizontalMaximum,
                                           mouseDriver->horizontalMinimum,
                                           mouseDriver->horizontalMaximum);
    }
    if (mouseDriver->verticalMinimum > mouseDriver->verticalMaximum)
    {
        hyperdos_pc_mouse_driver_set_range(&mouseDriver->verticalMinimum,
                                           &mouseDriver->verticalMaximum,
                                           mouseDriver->verticalMinimum,
                                           mouseDriver->verticalMaximum);
    }
    hyperdos_pc_mouse_driver_clamp_current_position(mouseDriver);
    return 1;
}

static uint16_t hyperdos_pc_mouse_driver_read_processor_memory_word(const hyperdos_x86_processor*       processor,
                                                                    hyperdos_x86_segment_register_index segmentRegister,
                                                                    uint16_t                            offset)
{
    uint8_t lowByte  = 0u;
    uint8_t highByte = 0u;

    if (hyperdos_x86_read_memory_byte(processor, segmentRegister, offset, &lowByte) != HYPERDOS_X86_EXECUTION_OK ||
        hyperdos_x86_read_memory_byte(processor, segmentRegister, (uint16_t)(offset + 1u), &highByte) !=
                HYPERDOS_X86_EXECUTION_OK)
    {
        return 0u;
    }
    return (uint16_t)(lowByte | ((uint16_t)highByte << HYPERDOS_X86_BYTE_BIT_COUNT));
}

static int hyperdos_pc_mouse_driver_write_processor_memory_word(hyperdos_x86_processor*             processor,
                                                                hyperdos_x86_segment_register_index segmentRegister,
                                                                uint16_t                            offset,
                                                                uint16_t                            value)
{
    if (hyperdos_x86_write_memory_byte(processor,
                                       segmentRegister,
                                       offset,
                                       (uint8_t)(value & HYPERDOS_X86_LOW_BYTE_MASK)) != HYPERDOS_X86_EXECUTION_OK)
    {
        return 0;
    }
    if (hyperdos_x86_write_memory_byte(processor,
                                       segmentRegister,
                                       (uint16_t)(offset + 1u),
                                       (uint8_t)(value >> HYPERDOS_X86_BYTE_BIT_COUNT)) != HYPERDOS_X86_EXECUTION_OK)
    {
        return 0;
    }
    return 1;
}

hyperdos_x86_execution_result hyperdos_pc_mouse_driver_handle_interrupt(hyperdos_pc_mouse_driver* mouseDriver,
                                                                        hyperdos_x86_processor*   processor)
{
    uint16_t functionNumber = 0u;

    if (mouseDriver == NULL || processor == NULL)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }

    functionNumber = hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
    switch (functionNumber)
    {
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_RESET_AND_STATUS:
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SOFTWARE_RESET:
        hyperdos_pc_mouse_driver_reset(mouseDriver);
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                               HYPERDOS_PC_MOUSE_DRIVER_INSTALLED_STATUS);
        hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE, mouseDriver->buttonCount);
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SHOW_CURSOR:
        if (mouseDriver->cursorDisplayCounter < INT16_MAX)
        {
            ++mouseDriver->cursorDisplayCounter;
        }
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_HIDE_CURSOR:
        if (mouseDriver->cursorDisplayCounter > INT16_MIN)
        {
            --mouseDriver->cursorDisplayCounter;
        }
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_GET_POSITION_AND_BUTTONS:
        hyperdos_pc_mouse_driver_get_position_and_buttons(mouseDriver, processor);
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_POSITION:
        mouseDriver->horizontalPosition = hyperdos_pc_mouse_driver_clamp_unsigned_position(
                hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER),
                mouseDriver->horizontalMinimum,
                mouseDriver->horizontalMaximum);
        mouseDriver->verticalPosition = hyperdos_pc_mouse_driver_clamp_unsigned_position(
                hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA),
                mouseDriver->verticalMinimum,
                mouseDriver->verticalMaximum);
        mouseDriver->horizontalPixelAccumulator = 0;
        mouseDriver->verticalPixelAccumulator   = 0;
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_GET_BUTTON_PRESS:
        hyperdos_pc_mouse_driver_get_button_history(
                mouseDriver,
                processor,
                hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE),
                0);
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_GET_BUTTON_RELEASE:
        hyperdos_pc_mouse_driver_get_button_history(
                mouseDriver,
                processor,
                hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE),
                1);
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_HORIZONTAL_RANGE:
        hyperdos_pc_mouse_driver_set_range(
                &mouseDriver->horizontalMinimum,
                &mouseDriver->horizontalMaximum,
                hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER),
                hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA));
        hyperdos_pc_mouse_driver_clamp_current_position(mouseDriver);
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_VERTICAL_RANGE:
        hyperdos_pc_mouse_driver_set_range(
                &mouseDriver->verticalMinimum,
                &mouseDriver->verticalMaximum,
                hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER),
                hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA));
        hyperdos_pc_mouse_driver_clamp_current_position(mouseDriver);
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_GRAPHICS_CURSOR:
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_TEXT_CURSOR:
        mouseDriver->textCursorType =
                (uint8_t)(hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE) &
                          HYPERDOS_X86_LOW_BYTE_MASK);
        mouseDriver->textCursorScreenMask =
                hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER);
        mouseDriver->textCursorCursorMask = hyperdos_x86_get_general_register_word(processor,
                                                                                   HYPERDOS_X86_GENERAL_REGISTER_DATA);
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_READ_MOTION_COUNTERS:
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
                                               (uint16_t)mouseDriver->horizontalMotionCounter);
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                               (uint16_t)mouseDriver->verticalMotionCounter);
        mouseDriver->horizontalMotionCounter = 0;
        mouseDriver->verticalMotionCounter   = 0;
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_EVENT_HANDLER:
        mouseDriver->eventHandlerMask               = hyperdos_x86_get_general_register_word(processor,
                                                                               HYPERDOS_X86_GENERAL_REGISTER_COUNTER);
        mouseDriver->eventHandlerOffset             = hyperdos_x86_get_general_register_word(processor,
                                                                                 HYPERDOS_X86_GENERAL_REGISTER_DATA);
        mouseDriver->eventHandlerSegment            = hyperdos_x86_get_segment_register(processor,
                                                                             HYPERDOS_X86_SEGMENT_REGISTER_EXTRA);
        mouseDriver->pendingEventMask               = 0u;
        mouseDriver->pendingEventHorizontalMovement = 0;
        mouseDriver->pendingEventVerticalMovement   = 0;
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_MICKEY_PIXEL_RATIO:
        mouseDriver->horizontalMickeysPerEightPixels =
                hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER);
        mouseDriver->verticalMickeysPerEightPixels =
                hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA);
        if (mouseDriver->horizontalMickeysPerEightPixels == 0u)
        {
            mouseDriver->horizontalMickeysPerEightPixels = 1u;
        }
        if (mouseDriver->verticalMickeysPerEightPixels == 0u)
        {
            mouseDriver->verticalMickeysPerEightPixels = 1u;
        }
        mouseDriver->horizontalPixelAccumulator = 0;
        mouseDriver->verticalPixelAccumulator   = 0;
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_DEFINE_SCREEN_REGION:
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_DOUBLE_SPEED_THRESHOLD:
        mouseDriver->doubleSpeedThreshold = hyperdos_x86_get_general_register_word(processor,
                                                                                   HYPERDOS_X86_GENERAL_REGISTER_DATA);
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_EXCHANGE_EVENT_HANDLER:
    {
        uint16_t previousEventHandlerMask    = mouseDriver->eventHandlerMask;
        uint16_t previousEventHandlerOffset  = mouseDriver->eventHandlerOffset;
        uint16_t previousEventHandlerSegment = mouseDriver->eventHandlerSegment;

        mouseDriver->eventHandlerMask               = hyperdos_x86_get_general_register_word(processor,
                                                                               HYPERDOS_X86_GENERAL_REGISTER_COUNTER);
        mouseDriver->eventHandlerOffset             = hyperdos_x86_get_general_register_word(processor,
                                                                                 HYPERDOS_X86_GENERAL_REGISTER_DATA);
        mouseDriver->eventHandlerSegment            = hyperdos_x86_get_segment_register(processor,
                                                                             HYPERDOS_X86_SEGMENT_REGISTER_EXTRA);
        mouseDriver->pendingEventMask               = 0u;
        mouseDriver->pendingEventHorizontalMovement = 0;
        mouseDriver->pendingEventVerticalMovement   = 0;
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
                                               previousEventHandlerMask);
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                               previousEventHandlerOffset);
        hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, previousEventHandlerSegment);
        return HYPERDOS_X86_EXECUTION_OK;
    }
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_GET_STATE_STORAGE_SIZE:
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_BASE,
                                               HYPERDOS_PC_MOUSE_DRIVER_STATE_BYTE_COUNT);
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SAVE_STATE:
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                               hyperdos_pc_mouse_driver_save_state(mouseDriver, processor) ? 1u : 0u);
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_RESTORE_STATE:
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                               hyperdos_pc_mouse_driver_restore_state(mouseDriver, processor) ? 1u
                                                                                                              : 0u);
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_SENSITIVITY:
        mouseDriver->horizontalSensitivity = hyperdos_x86_get_general_register_word(processor,
                                                                                    HYPERDOS_X86_GENERAL_REGISTER_BASE);
        mouseDriver->verticalSensitivity =
                hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER);
        mouseDriver->doubleSpeedThreshold = hyperdos_x86_get_general_register_word(processor,
                                                                                   HYPERDOS_X86_GENERAL_REGISTER_DATA);
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_GET_SENSITIVITY:
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_BASE,
                                               mouseDriver->horizontalSensitivity);
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
                                               mouseDriver->verticalSensitivity);
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                               mouseDriver->doubleSpeedThreshold);
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_INTERRUPT_RATE:
        mouseDriver->interruptRate = hyperdos_x86_get_general_register_word(processor,
                                                                            HYPERDOS_X86_GENERAL_REGISTER_COUNTER);
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_DISABLE_DRIVER:
        if (mouseDriver->enabled != 0u)
        {
            mouseDriver->cursorDisplayCounterBeforeDisable = mouseDriver->cursorDisplayCounter;
            if (mouseDriver->cursorDisplayCounter >= 0)
            {
                mouseDriver->cursorDisplayCounter = -1;
            }
        }
        mouseDriver->enabled = 0u;
        hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE, 0u);
        hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, 0u);
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_ENABLE_DRIVER:
        mouseDriver->enabled              = 1u;
        mouseDriver->cursorDisplayCounter = mouseDriver->cursorDisplayCounterBeforeDisable;
        return HYPERDOS_X86_EXECUTION_OK;
    case HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_GET_VERSION_TYPE_INTERRUPT:
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_BASE,
                                               HYPERDOS_PC_MOUSE_DRIVER_VERSION_WORD);
        hyperdos_x86_set_general_register_byte(processor,
                                               (uint8_t)((uint8_t)HYPERDOS_X86_GENERAL_REGISTER_COUNTER |
                                                         (uint8_t)HYPERDOS_PC_MOUSE_DRIVER_BYTE_REGISTER_HIGH_PART),
                                               HYPERDOS_PC_MOUSE_DRIVER_TYPE_PS2);
        hyperdos_x86_set_general_register_byte(processor,
                                               (uint8_t)HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
                                               HYPERDOS_PC_MOUSE_DRIVER_INTERRUPT_REQUEST_LINE);
        return HYPERDOS_X86_EXECUTION_OK;
    default:
        hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, 0u);
        return HYPERDOS_X86_EXECUTION_OK;
    }
}

hyperdos_x86_execution_result hyperdos_pc_mouse_driver_handle_backdoor_interrupt(hyperdos_pc_mouse_driver* mouseDriver,
                                                                                 hyperdos_x86_processor*   processor)
{
    uint16_t                      stackPointer        = 0u;
    uint16_t                      accumulatorPointer  = 0u;
    uint16_t                      basePointer         = 0u;
    uint16_t                      counterPointer      = 0u;
    uint16_t                      dataPointer         = 0u;
    uint16_t                      functionNumber      = 0u;
    uint16_t                      originalDataSegment = 0u;
    hyperdos_x86_execution_result result              = HYPERDOS_X86_EXECUTION_OK;

    if (mouseDriver == NULL || processor == NULL)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }

    stackPointer       = hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    accumulatorPointer = hyperdos_pc_mouse_driver_read_processor_memory_word(
            processor,
            HYPERDOS_X86_SEGMENT_REGISTER_STACK,
            (uint16_t)(stackPointer + HYPERDOS_PC_MOUSE_DRIVER_BACKDOOR_ACCUMULATOR_POINTER_STACK_OFFSET));
    basePointer = hyperdos_pc_mouse_driver_read_processor_memory_word(
            processor,
            HYPERDOS_X86_SEGMENT_REGISTER_STACK,
            (uint16_t)(stackPointer + HYPERDOS_PC_MOUSE_DRIVER_BACKDOOR_BASE_POINTER_STACK_OFFSET));
    counterPointer = hyperdos_pc_mouse_driver_read_processor_memory_word(
            processor,
            HYPERDOS_X86_SEGMENT_REGISTER_STACK,
            (uint16_t)(stackPointer + HYPERDOS_PC_MOUSE_DRIVER_BACKDOOR_COUNTER_POINTER_STACK_OFFSET));
    dataPointer = hyperdos_pc_mouse_driver_read_processor_memory_word(
            processor,
            HYPERDOS_X86_SEGMENT_REGISTER_STACK,
            (uint16_t)(stackPointer + HYPERDOS_PC_MOUSE_DRIVER_BACKDOOR_DATA_POINTER_STACK_OFFSET));

    functionNumber      = hyperdos_pc_mouse_driver_read_processor_memory_word(processor,
                                                                         HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                                                         accumulatorPointer);
    originalDataSegment = hyperdos_x86_get_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_DATA);
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, functionNumber);
    hyperdos_x86_set_general_register_word(
            processor,
            HYPERDOS_X86_GENERAL_REGISTER_BASE,
            hyperdos_pc_mouse_driver_read_processor_memory_word(processor,
                                                                HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                                                basePointer));
    hyperdos_x86_set_general_register_word(
            processor,
            HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
            hyperdos_pc_mouse_driver_read_processor_memory_word(processor,
                                                                HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                                                counterPointer));
    hyperdos_x86_set_general_register_word(
            processor,
            HYPERDOS_X86_GENERAL_REGISTER_DATA,
            hyperdos_pc_mouse_driver_read_processor_memory_word(processor,
                                                                HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                                                dataPointer));

    if (functionNumber == HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_GRAPHICS_CURSOR ||
        functionNumber == HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SAVE_STATE ||
        functionNumber == HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_RESTORE_STATE)
    {
        hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA, originalDataSegment);
    }
    if (functionNumber == HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_SET_EVENT_HANDLER ||
        functionNumber == HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_EXCHANGE_EVENT_HANDLER)
    {
        uint16_t requestedEventHandlerSegment =
                hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE);

        hyperdos_x86_set_segment_register(processor,
                                          HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                          requestedEventHandlerSegment != 0u ? requestedEventHandlerSegment
                                                                             : originalDataSegment);
    }
    if (functionNumber == HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_DEFINE_SCREEN_REGION)
    {
        uint16_t screenRegionPointer = hyperdos_x86_get_general_register_word(processor,
                                                                              HYPERDOS_X86_GENERAL_REGISTER_DATA);

        hyperdos_x86_set_general_register_word(
                processor,
                HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
                hyperdos_pc_mouse_driver_read_processor_memory_word(processor,
                                                                    HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                                                    screenRegionPointer));
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                               hyperdos_pc_mouse_driver_read_processor_memory_word(
                                                       processor,
                                                       HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                                       (uint16_t)(screenRegionPointer + HYPERDOS_X86_WORD_SIZE)));
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX,
                                               hyperdos_pc_mouse_driver_read_processor_memory_word(
                                                       processor,
                                                       HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                                       (uint16_t)(screenRegionPointer + HYPERDOS_X86_WORD_SIZE * 2u)));
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX,
                                               hyperdos_pc_mouse_driver_read_processor_memory_word(
                                                       processor,
                                                       HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                                       (uint16_t)(screenRegionPointer + HYPERDOS_X86_WORD_SIZE * 3u)));
    }

    result = hyperdos_pc_mouse_driver_handle_interrupt(mouseDriver, processor);
    if (result != HYPERDOS_X86_EXECUTION_OK)
    {
        return result;
    }

    (void)hyperdos_pc_mouse_driver_write_processor_memory_word(
            processor,
            HYPERDOS_X86_SEGMENT_REGISTER_DATA,
            accumulatorPointer,
            hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR));
    (void)hyperdos_pc_mouse_driver_write_processor_memory_word(
            processor,
            HYPERDOS_X86_SEGMENT_REGISTER_DATA,
            basePointer,
            hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE));
    (void)hyperdos_pc_mouse_driver_write_processor_memory_word(
            processor,
            HYPERDOS_X86_SEGMENT_REGISTER_DATA,
            counterPointer,
            hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_COUNTER));
    (void)hyperdos_pc_mouse_driver_write_processor_memory_word(
            processor,
            HYPERDOS_X86_SEGMENT_REGISTER_DATA,
            dataPointer,
            hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_DATA));
    if (functionNumber == HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_DISABLE_DRIVER)
    {
        (void)hyperdos_pc_mouse_driver_write_processor_memory_word(
                processor,
                HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                basePointer,
                hyperdos_x86_get_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA));
    }
    if (functionNumber == HYPERDOS_PC_MOUSE_DRIVER_FUNCTION_EXCHANGE_EVENT_HANDLER)
    {
        (void)hyperdos_pc_mouse_driver_write_processor_memory_word(
                processor,
                HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                counterPointer,
                hyperdos_x86_get_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_EXTRA));
    }
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, functionNumber);
    return HYPERDOS_X86_EXECUTION_OK;
}

static int hyperdos_pc_mouse_driver_push_processor_word(hyperdos_x86_processor* processor, uint16_t value)
{
    uint16_t stackPointer =
            (uint16_t)(hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER) -
                       HYPERDOS_X86_WORD_SIZE);

    if (hyperdos_x86_write_memory_byte(processor,
                                       HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                       stackPointer,
                                       (uint8_t)(value & HYPERDOS_X86_LOW_BYTE_MASK)) != HYPERDOS_X86_EXECUTION_OK)
    {
        return 0;
    }
    if (hyperdos_x86_write_memory_byte(processor,
                                       HYPERDOS_X86_SEGMENT_REGISTER_STACK,
                                       (uint16_t)(stackPointer + 1u),
                                       (uint8_t)(value >> HYPERDOS_X86_BYTE_BIT_COUNT)) != HYPERDOS_X86_EXECUTION_OK)
    {
        return 0;
    }
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER, stackPointer);
    return 1;
}

static void hyperdos_pc_mouse_driver_save_event_handler_processor_state(hyperdos_pc_mouse_driver*     mouseDriver,
                                                                        const hyperdos_x86_processor* processor)
{
    hyperdos_x86_general_register_index generalRegisterIndex = HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR;

    for (generalRegisterIndex = HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR;
         generalRegisterIndex <= HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX;
         ++generalRegisterIndex)
    {
        mouseDriver->savedEventHandlerGeneralRegisters
                [generalRegisterIndex] = hyperdos_x86_get_general_register(processor, generalRegisterIndex);
    }
    mouseDriver->savedEventHandlerExtraSegment         = hyperdos_x86_get_segment_register(processor,
                                                                                   HYPERDOS_X86_SEGMENT_REGISTER_EXTRA);
    mouseDriver->savedEventHandlerDataSegment          = hyperdos_x86_get_segment_register(processor,
                                                                                  HYPERDOS_X86_SEGMENT_REGISTER_DATA);
    mouseDriver->savedEventHandlerFlags                = hyperdos_x86_get_flags_word(processor);
    mouseDriver->eventHandlerSavedProcessorStateActive = 1u;
}

static void hyperdos_pc_mouse_driver_restore_event_handler_processor_state(hyperdos_pc_mouse_driver* mouseDriver,
                                                                           hyperdos_x86_processor*   processor)
{
    hyperdos_x86_general_register_index generalRegisterIndex = HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR;

    if (mouseDriver->eventHandlerSavedProcessorStateActive == 0u || processor == NULL)
    {
        mouseDriver->eventHandlerSavedProcessorStateActive = 0u;
        return;
    }

    for (generalRegisterIndex = HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR;
         generalRegisterIndex <= HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX;
         ++generalRegisterIndex)
    {
        if (generalRegisterIndex == HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER)
        {
            continue;
        }
        hyperdos_x86_set_general_register(processor,
                                          generalRegisterIndex,
                                          mouseDriver->savedEventHandlerGeneralRegisters[generalRegisterIndex]);
    }
    hyperdos_x86_set_segment_register(processor,
                                      HYPERDOS_X86_SEGMENT_REGISTER_EXTRA,
                                      mouseDriver->savedEventHandlerExtraSegment);
    hyperdos_x86_set_segment_register(processor,
                                      HYPERDOS_X86_SEGMENT_REGISTER_DATA,
                                      mouseDriver->savedEventHandlerDataSegment);
    hyperdos_x86_set_flags_word(processor, mouseDriver->savedEventHandlerFlags);
    mouseDriver->eventHandlerSavedProcessorStateActive = 0u;
}

hyperdos_x86_execution_result hyperdos_pc_mouse_driver_dispatch_pending_event_handler(
        hyperdos_pc_mouse_driver* mouseDriver,
        hyperdos_x86_processor*   processor)
{
    uint16_t originalStackPointer = 0u;
    uint16_t returnOffset         = 0u;
    uint16_t returnSegment        = 0u;

    if (mouseDriver == NULL || processor == NULL)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }
    if (mouseDriver->pendingEventMask == 0u || mouseDriver->eventHandlerActive != 0u ||
        (mouseDriver->eventHandlerOffset == 0u && mouseDriver->eventHandlerSegment == 0u))
    {
        return HYPERDOS_X86_EXECUTION_OK;
    }

    returnOffset         = hyperdos_x86_get_instruction_pointer_word(processor);
    returnSegment        = hyperdos_x86_get_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE);
    originalStackPointer = hyperdos_x86_get_general_register_word(processor,
                                                                  HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER);
    hyperdos_pc_mouse_driver_save_event_handler_processor_state(mouseDriver, processor);
    if (!hyperdos_pc_mouse_driver_push_processor_word(processor, returnSegment) ||
        !hyperdos_pc_mouse_driver_push_processor_word(processor, returnOffset) ||
        !hyperdos_pc_mouse_driver_push_processor_word(processor,
                                                      HYPERDOS_PC_BIOS_MOUSE_CALLBACK_CLEANUP_STUB_SEGMENT) ||
        !hyperdos_pc_mouse_driver_push_processor_word(processor, HYPERDOS_PC_BIOS_MOUSE_CALLBACK_CLEANUP_STUB_OFFSET))
    {
        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER,
                                               originalStackPointer);
        mouseDriver->eventHandlerSavedProcessorStateActive = 0u;
        return HYPERDOS_X86_EXECUTION_OK;
    }

    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                           mouseDriver->pendingEventMask);
    hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_BASE, mouseDriver->buttonMask);
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_COUNTER,
                                           mouseDriver->horizontalPosition);
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_DATA,
                                           mouseDriver->verticalPosition);
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX,
                                           (uint16_t)mouseDriver->pendingEventHorizontalMovement);
    hyperdos_x86_set_general_register_word(processor,
                                           HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX,
                                           (uint16_t)mouseDriver->pendingEventVerticalMovement);
    hyperdos_x86_set_segment_register(processor, HYPERDOS_X86_SEGMENT_REGISTER_CODE, mouseDriver->eventHandlerSegment);
    hyperdos_x86_set_instruction_pointer_word(processor, mouseDriver->eventHandlerOffset);
    mouseDriver->eventHandlerActive             = 1u;
    mouseDriver->pendingEventMask               = 0u;
    mouseDriver->pendingEventHorizontalMovement = 0;
    mouseDriver->pendingEventVerticalMovement   = 0;
    return HYPERDOS_X86_EXECUTION_OK;
}

void hyperdos_pc_mouse_driver_complete_event_handler(hyperdos_pc_mouse_driver* mouseDriver,
                                                     hyperdos_x86_processor*   processor)
{
    if (mouseDriver == NULL)
    {
        return;
    }
    hyperdos_pc_mouse_driver_restore_event_handler_processor_state(mouseDriver, processor);
    mouseDriver->eventHandlerActive = 0u;
}

int hyperdos_pc_mouse_driver_cursor_is_visible(const hyperdos_pc_mouse_driver* mouseDriver)
{
    return mouseDriver != NULL && mouseDriver->installed != 0u && mouseDriver->enabled != 0u &&
           mouseDriver->cursorDisplayCounter >= 0;
}

int hyperdos_pc_mouse_driver_get_text_cursor_cell(const hyperdos_pc_mouse_driver* mouseDriver,
                                                  uint16_t                        columnCount,
                                                  uint16_t                        rowCount,
                                                  uint16_t*                       cursorColumn,
                                                  uint16_t*                       cursorRow)
{
    uint16_t column = 0u;
    uint16_t row    = 0u;

    if (!hyperdos_pc_mouse_driver_cursor_is_visible(mouseDriver) || columnCount == 0u || rowCount == 0u ||
        cursorColumn == NULL || cursorRow == NULL)
    {
        return 0;
    }

    column = (uint16_t)(mouseDriver->horizontalPosition / HYPERDOS_PC_MOUSE_DRIVER_TEXT_CELL_WIDTH);
    row    = (uint16_t)(mouseDriver->verticalPosition / HYPERDOS_PC_MOUSE_DRIVER_TEXT_CELL_HEIGHT);
    if (column >= columnCount)
    {
        column = (uint16_t)(columnCount - 1u);
    }
    if (row >= rowCount)
    {
        row = (uint16_t)(rowCount - 1u);
    }
    *cursorColumn = column;
    *cursorRow    = row;
    return 1;
}
