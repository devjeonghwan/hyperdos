#include "hyperdos/pc_keyboard_bios.h"

#include <stdio.h>
#include <string.h>

#include "hyperdos/pc_bios.h"
#include "hyperdos/pc_bios_data_area.h"

enum
{
    HYPERDOS_PC_KEYBOARD_BIOS_TRACE_TEXT_CAPACITY                  = 256u,
    HYPERDOS_PC_KEYBOARD_BIOS_CONTROLLER_STATUS_OUTPUT_BUFFER_FULL = 0x01u,
    HYPERDOS_PC_KEYBOARD_BIOS_UNSUPPORTED_STATUS                   = 0x86u,
};

static void hyperdos_pc_keyboard_bios_set_carry_flag(hyperdos_x86_processor* processor, int carry)
{
    hyperdos_x86_set_flag(processor, HYPERDOS_X86_FLAG_CARRY, carry);
}

static void hyperdos_pc_keyboard_bios_lock(const hyperdos_pc_keyboard_bios_interface* keyboardBiosInterface)
{
    if (keyboardBiosInterface != NULL && keyboardBiosInterface->lockKeyboard != NULL)
    {
        keyboardBiosInterface->lockKeyboard(keyboardBiosInterface->userContext);
    }
}

static void hyperdos_pc_keyboard_bios_unlock(const hyperdos_pc_keyboard_bios_interface* keyboardBiosInterface)
{
    if (keyboardBiosInterface != NULL && keyboardBiosInterface->unlockKeyboard != NULL)
    {
        keyboardBiosInterface->unlockKeyboard(keyboardBiosInterface->userContext);
    }
}

static uint16_t hyperdos_pc_keyboard_bios_advance_buffer_offset(uint16_t offset)
{
    offset = (uint16_t)(offset + 2u);
    if (offset >= HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_BUFFER_END)
    {
        offset = HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_BUFFER_START;
    }
    return offset;
}

static int hyperdos_pc_keyboard_bios_buffer_is_empty_locked(hyperdos_pc* pc)
{
    uint16_t headOffset = hyperdos_pc_bios_data_area_read_word(pc, HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_HEAD_OFFSET);
    uint16_t tailOffset = hyperdos_pc_bios_data_area_read_word(pc, HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_TAIL_OFFSET);

    return headOffset == tailOffset;
}

static int hyperdos_pc_keyboard_bios_buffer_push_word_locked(hyperdos_pc* pc, uint16_t keyWord)
{
    uint16_t headOffset     = hyperdos_pc_bios_data_area_read_word(pc, HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_HEAD_OFFSET);
    uint16_t tailOffset     = hyperdos_pc_bios_data_area_read_word(pc, HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_TAIL_OFFSET);
    uint16_t nextTailOffset = hyperdos_pc_keyboard_bios_advance_buffer_offset(tailOffset);

    if (nextTailOffset == headOffset)
    {
        return 0;
    }
    hyperdos_pc_bios_data_area_write_word(pc, tailOffset, keyWord);
    hyperdos_pc_bios_data_area_write_word(pc, HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_TAIL_OFFSET, nextTailOffset);
    return 1;
}

static int hyperdos_pc_keyboard_bios_buffer_peek_word_locked(hyperdos_pc* pc, uint16_t* keyWord)
{
    uint16_t headOffset = hyperdos_pc_bios_data_area_read_word(pc, HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_HEAD_OFFSET);
    uint16_t tailOffset = hyperdos_pc_bios_data_area_read_word(pc, HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_TAIL_OFFSET);

    if (headOffset == tailOffset)
    {
        return 0;
    }
    if (keyWord != NULL)
    {
        *keyWord = hyperdos_pc_bios_data_area_read_word(pc, headOffset);
    }
    return 1;
}

static int hyperdos_pc_keyboard_bios_buffer_pop_word_locked(hyperdos_pc* pc, uint16_t* keyWord)
{
    uint16_t headOffset = hyperdos_pc_bios_data_area_read_word(pc, HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_HEAD_OFFSET);

    if (!hyperdos_pc_keyboard_bios_buffer_peek_word_locked(pc, keyWord))
    {
        return 0;
    }
    hyperdos_pc_bios_data_area_write_word(pc,
                                          HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_HEAD_OFFSET,
                                          hyperdos_pc_keyboard_bios_advance_buffer_offset(headOffset));
    return 1;
}

void hyperdos_pc_keyboard_bios_initialize(hyperdos_pc_keyboard_bios* keyboardBios)
{
    hyperdos_pc_keyboard_bios_reset(keyboardBios);
}

void hyperdos_pc_keyboard_bios_reset(hyperdos_pc_keyboard_bios* keyboardBios)
{
    if (keyboardBios == NULL)
    {
        return;
    }
    memset(keyboardBios, 0, sizeof(*keyboardBios));
}

void hyperdos_pc_keyboard_bios_initialize_data_area(hyperdos_pc_keyboard_bios* keyboardBios, hyperdos_pc* pc)
{
    if (keyboardBios == NULL || pc == NULL)
    {
        return;
    }
    hyperdos_pc_bios_data_area_write_word(pc,
                                          HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_HEAD_OFFSET,
                                          HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_BUFFER_START);
    hyperdos_pc_bios_data_area_write_word(pc,
                                          HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_TAIL_OFFSET,
                                          HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_BUFFER_START);
    hyperdos_pc_bios_data_area_write_byte(pc,
                                          HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_SHIFT_FLAGS_OFFSET,
                                          keyboardBios->shiftFlags);
    hyperdos_pc_bios_data_area_write_word(pc,
                                          HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_BUFFER_START_OFFSET,
                                          HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_BUFFER_START);
    hyperdos_pc_bios_data_area_write_word(pc,
                                          HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_BUFFER_END_OFFSET,
                                          HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_BUFFER_END);
}

int hyperdos_pc_keyboard_bios_push_key_word(hyperdos_pc_keyboard_bios*                 keyboardBios,
                                            const hyperdos_pc_keyboard_bios_interface* keyboardBiosInterface,
                                            hyperdos_pc*                               pc,
                                            uint16_t                                   keyWord)
{
    size_t nextWriteIndex = 0u;
    int    pushed         = 0;

    if (keyboardBios == NULL || pc == NULL || keyWord == 0u)
    {
        return 0;
    }
    hyperdos_pc_keyboard_bios_lock(keyboardBiosInterface);
    pushed = hyperdos_pc_keyboard_bios_buffer_push_word_locked(pc, keyWord);
    if (pushed)
    {
        nextWriteIndex = (keyboardBios->keyWordWriteIndex + 1u) % HYPERDOS_PC_KEYBOARD_BIOS_QUEUE_CAPACITY;
        if (nextWriteIndex != keyboardBios->keyWordReadIndex)
        {
            keyboardBios->keyWords[keyboardBios->keyWordWriteIndex] = keyWord;
            keyboardBios->keyWordWriteIndex                         = nextWriteIndex;
        }
    }
    hyperdos_pc_keyboard_bios_unlock(keyboardBiosInterface);
    return pushed;
}

int hyperdos_pc_keyboard_bios_pop_key_word(hyperdos_pc_keyboard_bios*                 keyboardBios,
                                           const hyperdos_pc_keyboard_bios_interface* keyboardBiosInterface,
                                           hyperdos_pc*                               pc,
                                           uint16_t*                                  keyWord)
{
    int popped = 0;

    if (keyboardBios == NULL || pc == NULL || keyWord == NULL)
    {
        return 0;
    }
    hyperdos_pc_keyboard_bios_lock(keyboardBiosInterface);
    popped = hyperdos_pc_keyboard_bios_buffer_pop_word_locked(pc, keyWord);
    if (popped && keyboardBios->keyWordReadIndex != keyboardBios->keyWordWriteIndex)
    {
        keyboardBios->keyWordReadIndex = (keyboardBios->keyWordReadIndex + 1u) %
                                         HYPERDOS_PC_KEYBOARD_BIOS_QUEUE_CAPACITY;
    }
    hyperdos_pc_keyboard_bios_unlock(keyboardBiosInterface);
    return popped;
}

int hyperdos_pc_keyboard_bios_peek_key_word(hyperdos_pc_keyboard_bios*                 keyboardBios,
                                            const hyperdos_pc_keyboard_bios_interface* keyboardBiosInterface,
                                            hyperdos_pc*                               pc,
                                            uint16_t*                                  keyWord)
{
    int found = 0;

    (void)keyboardBios;
    if (pc == NULL || keyWord == NULL)
    {
        return 0;
    }
    hyperdos_pc_keyboard_bios_lock(keyboardBiosInterface);
    if (!hyperdos_pc_keyboard_bios_buffer_is_empty_locked(pc))
    {
        found = hyperdos_pc_keyboard_bios_buffer_peek_word_locked(pc, keyWord);
    }
    hyperdos_pc_keyboard_bios_unlock(keyboardBiosInterface);
    return found;
}

int hyperdos_pc_keyboard_bios_scan_code_is_modifier_or_toggle(uint8_t scanCode)
{
    uint8_t baseScanCode = (uint8_t)(scanCode & (uint8_t)~HYPERDOS_PC_KEYBOARD_BIOS_RELEASE_MASK);

    return baseScanCode == 0x1Du || baseScanCode == 0x2Au || baseScanCode == 0x36u || baseScanCode == 0x38u ||
           baseScanCode == 0x3Au || baseScanCode == 0x45u || baseScanCode == 0x46u;
}

uint8_t hyperdos_pc_keyboard_bios_update_shift_flags_from_scan_code(uint8_t shiftFlags, uint8_t scanCode)
{
    uint8_t baseScanCode = (uint8_t)(scanCode & (uint8_t)~HYPERDOS_PC_KEYBOARD_BIOS_RELEASE_MASK);
    int     released     = (scanCode & HYPERDOS_PC_KEYBOARD_BIOS_RELEASE_MASK) != 0u;

    switch (baseScanCode)
    {
    case 0x2Au:
        return released ? (uint8_t)(shiftFlags & (uint8_t)~HYPERDOS_PC_KEYBOARD_BIOS_LEFT_SHIFT_FLAG)
                        : (uint8_t)(shiftFlags | HYPERDOS_PC_KEYBOARD_BIOS_LEFT_SHIFT_FLAG);
    case 0x36u:
        return released ? (uint8_t)(shiftFlags & (uint8_t)~HYPERDOS_PC_KEYBOARD_BIOS_RIGHT_SHIFT_FLAG)
                        : (uint8_t)(shiftFlags | HYPERDOS_PC_KEYBOARD_BIOS_RIGHT_SHIFT_FLAG);
    case 0x1Du:
        return released ? (uint8_t)(shiftFlags & (uint8_t)~HYPERDOS_PC_KEYBOARD_BIOS_CONTROL_FLAG)
                        : (uint8_t)(shiftFlags | HYPERDOS_PC_KEYBOARD_BIOS_CONTROL_FLAG);
    case 0x38u:
        return released ? (uint8_t)(shiftFlags & (uint8_t)~HYPERDOS_PC_KEYBOARD_BIOS_ALT_FLAG)
                        : (uint8_t)(shiftFlags | HYPERDOS_PC_KEYBOARD_BIOS_ALT_FLAG);
    case 0x3Au:
        return released ? shiftFlags : (uint8_t)(shiftFlags ^ HYPERDOS_PC_KEYBOARD_BIOS_CAPS_LOCK_FLAG);
    case 0x45u:
        return released ? shiftFlags : (uint8_t)(shiftFlags ^ HYPERDOS_PC_KEYBOARD_BIOS_NUM_LOCK_FLAG);
    case 0x46u:
        return released ? shiftFlags : (uint8_t)(shiftFlags ^ HYPERDOS_PC_KEYBOARD_BIOS_SCROLL_LOCK_FLAG);
    default:
        break;
    }
    return shiftFlags;
}

static uint16_t hyperdos_pc_keyboard_bios_make_key_word_from_set_one_scan_code(uint8_t scanCode, uint8_t shiftFlags)
{
    int     shiftPressed    = (shiftFlags &
                        (HYPERDOS_PC_KEYBOARD_BIOS_LEFT_SHIFT_FLAG | HYPERDOS_PC_KEYBOARD_BIOS_RIGHT_SHIFT_FLAG)) != 0u;
    int     controlPressed  = (shiftFlags & HYPERDOS_PC_KEYBOARD_BIOS_CONTROL_FLAG) != 0u;
    int     altPressed      = (shiftFlags & HYPERDOS_PC_KEYBOARD_BIOS_ALT_FLAG) != 0u;
    int     capsLockEnabled = (shiftFlags & HYPERDOS_PC_KEYBOARD_BIOS_CAPS_LOCK_FLAG) != 0u;
    uint8_t character       = 0u;

    if ((scanCode & HYPERDOS_PC_KEYBOARD_BIOS_RELEASE_MASK) != 0u)
    {
        return 0u;
    }

    switch (scanCode)
    {
    case 0x01u:
        character = 0x1Bu;
        break;
    case 0x0Eu:
        character = '\b';
        break;
    case 0x0Fu:
        character = '\t';
        break;
    case 0x1Cu:
        character = '\r';
        break;
    case 0x39u:
        character = ' ';
        break;
    default:
        break;
    }
    if (character != 0u)
    {
        return (uint16_t)(((uint16_t)scanCode << HYPERDOS_X86_BYTE_BIT_COUNT) | character);
    }

    if (scanCode >= 0x3Bu && scanCode <= 0x44u)
    {
        uint8_t functionKeyIndex = (uint8_t)(scanCode - 0x3Bu);
        uint8_t returnedScanCode = scanCode;

        if (altPressed)
        {
            returnedScanCode = (uint8_t)(0x68u + functionKeyIndex);
        }
        else if (controlPressed)
        {
            returnedScanCode = (uint8_t)(0x5Eu + functionKeyIndex);
        }
        else if (shiftPressed)
        {
            returnedScanCode = (uint8_t)(0x54u + functionKeyIndex);
        }
        return (uint16_t)((uint16_t)returnedScanCode << HYPERDOS_X86_BYTE_BIT_COUNT);
    }
    if (scanCode == 0x57u || scanCode == 0x58u || (scanCode >= 0x47u && scanCode <= 0x53u))
    {
        return (uint16_t)((uint16_t)scanCode << HYPERDOS_X86_BYTE_BIT_COUNT);
    }

    switch (scanCode)
    {
    case 0x10u:
        character = 'q';
        break;
    case 0x11u:
        character = 'w';
        break;
    case 0x12u:
        character = 'e';
        break;
    case 0x13u:
        character = 'r';
        break;
    case 0x14u:
        character = 't';
        break;
    case 0x15u:
        character = 'y';
        break;
    case 0x16u:
        character = 'u';
        break;
    case 0x17u:
        character = 'i';
        break;
    case 0x18u:
        character = 'o';
        break;
    case 0x19u:
        character = 'p';
        break;
    case 0x1Eu:
        character = 'a';
        break;
    case 0x1Fu:
        character = 's';
        break;
    case 0x20u:
        character = 'd';
        break;
    case 0x21u:
        character = 'f';
        break;
    case 0x22u:
        character = 'g';
        break;
    case 0x23u:
        character = 'h';
        break;
    case 0x24u:
        character = 'j';
        break;
    case 0x25u:
        character = 'k';
        break;
    case 0x26u:
        character = 'l';
        break;
    case 0x2Cu:
        character = 'z';
        break;
    case 0x2Du:
        character = 'x';
        break;
    case 0x2Eu:
        character = 'c';
        break;
    case 0x2Fu:
        character = 'v';
        break;
    case 0x30u:
        character = 'b';
        break;
    case 0x31u:
        character = 'n';
        break;
    case 0x32u:
        character = 'm';
        break;
    default:
        break;
    }
    if (character >= 'a' && character <= 'z')
    {
        if (controlPressed)
        {
            character = (uint8_t)(character - 'a' + 1u);
        }
        else if (shiftPressed ^ capsLockEnabled)
        {
            character = (uint8_t)(character - ('a' - 'A'));
        }
        return (uint16_t)(((uint16_t)scanCode << HYPERDOS_X86_BYTE_BIT_COUNT) | character);
    }

    switch (scanCode)
    {
    case 0x02u:
        character = shiftPressed ? '!' : '1';
        break;
    case 0x03u:
        character = shiftPressed ? '@' : '2';
        break;
    case 0x04u:
        character = shiftPressed ? '#' : '3';
        break;
    case 0x05u:
        character = shiftPressed ? '$' : '4';
        break;
    case 0x06u:
        character = shiftPressed ? '%' : '5';
        break;
    case 0x07u:
        character = shiftPressed ? '^' : '6';
        break;
    case 0x08u:
        character = shiftPressed ? '&' : '7';
        break;
    case 0x09u:
        character = shiftPressed ? '*' : '8';
        break;
    case 0x0Au:
        character = shiftPressed ? '(' : '9';
        break;
    case 0x0Bu:
        character = shiftPressed ? ')' : '0';
        break;
    case 0x0Cu:
        character = shiftPressed ? '_' : '-';
        break;
    case 0x0Du:
        character = shiftPressed ? '+' : '=';
        break;
    case 0x1Au:
        character = shiftPressed ? '{' : '[';
        break;
    case 0x1Bu:
        character = shiftPressed ? '}' : ']';
        break;
    case 0x27u:
        character = shiftPressed ? ':' : ';';
        break;
    case 0x28u:
        character = shiftPressed ? '"' : '\'';
        break;
    case 0x29u:
        character = shiftPressed ? '~' : '`';
        break;
    case 0x2Bu:
        character = shiftPressed ? '|' : '\\';
        break;
    case 0x33u:
        character = shiftPressed ? '<' : ',';
        break;
    case 0x34u:
        character = shiftPressed ? '>' : '.';
        break;
    case 0x35u:
        character = shiftPressed ? '?' : '/';
        break;
    default:
        return 0u;
    }
    return (uint16_t)(((uint16_t)scanCode << HYPERDOS_X86_BYTE_BIT_COUNT) | character);
}

static int hyperdos_pc_keyboard_bios_scan_code_is_keyboard_response(uint8_t scanCode)
{
    return scanCode == 0x00u || scanCode == 0xEEu || scanCode == 0xFAu || scanCode == 0xFEu || scanCode == 0xFFu;
}

void hyperdos_pc_keyboard_bios_process_scan_code(hyperdos_pc_keyboard_bios*                 keyboardBios,
                                                 const hyperdos_pc_keyboard_bios_interface* keyboardBiosInterface,
                                                 hyperdos_pc*                               pc,
                                                 uint8_t                                    scanCode)
{
    uint16_t keyWord = 0u;

    if (keyboardBios == NULL || pc == NULL || hyperdos_pc_keyboard_bios_scan_code_is_keyboard_response(scanCode))
    {
        return;
    }
    if (scanCode == HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_SCAN_CODE_PREFIX ||
        scanCode == HYPERDOS_PC_KEYBOARD_BIOS_PAUSE_SCAN_CODE_PREFIX)
    {
        keyboardBios->extendedScanCodePrefix = scanCode;
        return;
    }
    if (keyboardBios->extendedScanCodePrefix != 0u)
    {
        keyboardBios->extendedScanCodePrefix = 0u;
    }
    if (hyperdos_pc_keyboard_bios_scan_code_is_modifier_or_toggle(scanCode))
    {
        uint8_t storedShiftFlags = 0u;

        hyperdos_pc_keyboard_bios_lock(keyboardBiosInterface);
        storedShiftFlags         = hyperdos_pc_bios_data_area_read_byte(pc,
                                                                HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_SHIFT_FLAGS_OFFSET);
        keyboardBios->shiftFlags = hyperdos_pc_keyboard_bios_update_shift_flags_from_scan_code(keyboardBios->shiftFlags,
                                                                                               scanCode);
        hyperdos_pc_bios_data_area_write_byte(pc,
                                              HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_SHIFT_FLAGS_OFFSET,
                                              (uint8_t)(keyboardBios->shiftFlags |
                                                        (storedShiftFlags & HYPERDOS_PC_KEYBOARD_BIOS_INSERT_FLAG)));
        hyperdos_pc_keyboard_bios_unlock(keyboardBiosInterface);
        return;
    }
    if ((scanCode & HYPERDOS_PC_KEYBOARD_BIOS_RELEASE_MASK) != 0u)
    {
        return;
    }

    keyWord = hyperdos_pc_keyboard_bios_make_key_word_from_set_one_scan_code(scanCode, keyboardBios->shiftFlags);
    if (keyWord != 0u)
    {
        (void)hyperdos_pc_keyboard_bios_push_key_word(keyboardBios, keyboardBiosInterface, pc, keyWord);
    }
}

int hyperdos_pc_keyboard_bios_service_hardware_byte(hyperdos_pc_keyboard_bios*                 keyboardBios,
                                                    const hyperdos_pc_keyboard_bios_interface* keyboardBiosInterface,
                                                    hyperdos_pc*                               pc,
                                                    uint8_t                                    preloadedScanCode,
                                                    int                                        hasPreloadedScanCode,
                                                    hyperdos_pc_keyboard_bios_trace_function   traceFunction,
                                                    void*                                      traceUserContext)
{
    int     defaultHardwareInterruptVector = 0;
    uint8_t statusRegister                 = 0u;
    uint8_t scanCode                       = 0u;

    if (keyboardBios == NULL || pc == NULL)
    {
        return 0;
    }

    defaultHardwareInterruptVector = hyperdos_pc_bios_keyboard_hardware_interrupt_vector_is_default(pc);
    if (hasPreloadedScanCode)
    {
        scanCode = preloadedScanCode;
    }
    else
    {
        statusRegister =
                hyperdos_intel_8042_keyboard_controller_read_byte(&pc->keyboardController,
                                                                  HYPERDOS_PC_KEYBOARD_CONTROLLER_COMMAND_PORT);

        if ((statusRegister & HYPERDOS_PC_KEYBOARD_BIOS_CONTROLLER_STATUS_OUTPUT_BUFFER_FULL) == 0u)
        {
            return 0;
        }
        hyperdos_intel_8042_keyboard_controller_clear_interrupt_request(&pc->keyboardController);
        scanCode = hyperdos_intel_8042_keyboard_controller_read_byte(&pc->keyboardController,
                                                                     HYPERDOS_PC_KEYBOARD_CONTROLLER_DATA_PORT);
    }

    if (traceFunction != NULL)
    {
        char traceMessage[HYPERDOS_PC_KEYBOARD_BIOS_TRACE_TEXT_CAPACITY];

        (void)snprintf(traceMessage,
                       sizeof(traceMessage),
                       "bios int09 scan-code=%02X default-vector=%u preloaded=%u",
                       scanCode,
                       defaultHardwareInterruptVector,
                       hasPreloadedScanCode != 0);
        traceFunction(traceUserContext, traceMessage);
    }
    hyperdos_pc_keyboard_bios_process_scan_code(keyboardBios, keyboardBiosInterface, pc, scanCode);
    return 1;
}

hyperdos_x86_execution_result hyperdos_pc_keyboard_bios_handle_interrupt(
        hyperdos_x86_processor*                    processor,
        hyperdos_pc_keyboard_bios*                 keyboardBios,
        const hyperdos_pc_keyboard_bios_interface* keyboardBiosInterface,
        hyperdos_pc*                               pc,
        uint8_t                                    serviceNumber)
{
    uint16_t keyWord       = 0u;
    int      isReadService = serviceNumber == HYPERDOS_PC_KEYBOARD_BIOS_READ_SERVICE ||
                        serviceNumber == HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_READ_SERVICE;
    int isStatusService = serviceNumber == HYPERDOS_PC_KEYBOARD_BIOS_STATUS_SERVICE ||
                          serviceNumber == HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_STATUS_SERVICE;
    int isShiftStatusService = serviceNumber == HYPERDOS_PC_KEYBOARD_BIOS_SHIFT_STATUS_SERVICE ||
                               serviceNumber == HYPERDOS_PC_KEYBOARD_BIOS_EXTENDED_SHIFT_STATUS_SERVICE;

    if (processor == NULL || keyboardBios == NULL || pc == NULL)
    {
        return HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT;
    }

    if (keyboardBiosInterface != NULL && keyboardBiosInterface->refreshInputState != NULL)
    {
        keyboardBiosInterface->refreshInputState(keyboardBiosInterface->userContext);
    }

    if (isReadService)
    {
        if (hyperdos_pc_keyboard_bios_pop_key_word(keyboardBios, keyboardBiosInterface, pc, &keyWord))
        {
            hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, keyWord);
            hyperdos_x86_set_flag(processor, HYPERDOS_X86_FLAG_ZERO, 0);
            return HYPERDOS_X86_EXECUTION_OK;
        }

        hyperdos_x86_set_instruction_pointer_word(processor, (uint16_t)processor->lastInstructionOffset);
        hyperdos_x86_set_flag(processor, HYPERDOS_X86_FLAG_INTERRUPT_ENABLE, 1);
        return HYPERDOS_X86_EXECUTION_STEP_LIMIT_REACHED;
    }

    if (isStatusService)
    {
        if (hyperdos_pc_keyboard_bios_peek_key_word(keyboardBios, keyboardBiosInterface, pc, &keyWord))
        {
            hyperdos_x86_set_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR, keyWord);
            hyperdos_x86_set_flag(processor, HYPERDOS_X86_FLAG_ZERO, 0);
        }
        else
        {
            hyperdos_x86_set_flag(processor, HYPERDOS_X86_FLAG_ZERO, 1);
        }
        hyperdos_x86_set_flag(processor, HYPERDOS_X86_FLAG_INTERRUPT_ENABLE, 1);
        return HYPERDOS_X86_EXECUTION_OK;
    }

    if (isShiftStatusService)
    {
        uint16_t accumulator = hyperdos_x86_get_general_register_word(processor,
                                                                      HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR);
        uint8_t  shiftFlags =
                hyperdos_pc_bios_data_area_read_byte(pc, HYPERDOS_PC_BIOS_DATA_AREA_KEYBOARD_SHIFT_FLAGS_OFFSET);

        hyperdos_x86_set_general_register_word(processor,
                                               HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
                                               (uint16_t)((accumulator & (uint16_t)~HYPERDOS_X86_LOW_BYTE_MASK) |
                                                          shiftFlags));
        hyperdos_x86_set_flags_word(processor, hyperdos_x86_get_flags_word(processor));
        return HYPERDOS_X86_EXECUTION_OK;
    }

    hyperdos_x86_set_general_register_word(
            processor,
            HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR,
            (uint16_t)((HYPERDOS_PC_KEYBOARD_BIOS_UNSUPPORTED_STATUS
                        << HYPERDOS_PC_KEYBOARD_BIOS_SERVICE_REGISTER_SHIFT) |
                       (hyperdos_x86_get_general_register_word(processor, HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR) &
                        HYPERDOS_X86_LOW_BYTE_MASK)));
    hyperdos_pc_keyboard_bios_set_carry_flag(processor, 1);
    return HYPERDOS_X86_EXECUTION_OK;
}
