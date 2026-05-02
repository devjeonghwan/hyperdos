#ifndef HYPERDOS_X86_PROCESSOR_H
#define HYPERDOS_X86_PROCESSOR_H

#include <stddef.h>
#include <stdint.h>

#define HYPERDOS_X86_MEMORY_SIZE                     (1024u * 1024u)
#define HYPERDOS_X86_ADDRESS_MASK                    0xFFFFFu
#define HYPERDOS_X86_DOS_PROGRAM_OFFSET              0x0100u
#define HYPERDOS_X86_DEFAULT_DOS_SEGMENT             0x1000u
#define HYPERDOS_X86_DOS_INTERRUPT_TERMINATE_PROGRAM 0x20u
#define HYPERDOS_X86_DOS_INTERRUPT_FUNCTION_DISPATCH 0x21u
#define HYPERDOS_X86_DOS_SERVICE_PRINT_STRING        0x09u
#define HYPERDOS_X86_DOS_SERVICE_TERMINATE_PROCESS   0x4Cu
#define HYPERDOS_X86_DOS_PRINT_STRING_TERMINATOR     '$'

enum
{
    HYPERDOS_X86_BYTE_BIT_COUNT = 8u,
    HYPERDOS_X86_WORD_BIT_COUNT = 16u,
    HYPERDOS_X86_BYTE_SIZE      = 1u,
    HYPERDOS_X86_WORD_SIZE      = 2u,
    HYPERDOS_X86_SEGMENT_SHIFT  = 4u,
    HYPERDOS_X86_LOW_BYTE_MASK  = 0x00FFu,
    HYPERDOS_X86_BYTE_MASK      = HYPERDOS_X86_LOW_BYTE_MASK,
    HYPERDOS_X86_HIGH_BYTE_MASK = 0xFF00u,
    HYPERDOS_X86_WORD_MASK      = 0xFFFFu
};

typedef enum hyperdos_x86_execution_result
{
    HYPERDOS_X86_EXECUTION_OK = 0,
    HYPERDOS_X86_EXECUTION_HALTED,
    HYPERDOS_X86_EXECUTION_STEP_LIMIT_REACHED,
    HYPERDOS_X86_EXECUTION_INVALID_ARGUMENT,
    HYPERDOS_X86_EXECUTION_PROGRAM_TOO_LARGE,
    HYPERDOS_X86_EXECUTION_UNSUPPORTED_INSTRUCTION,
    HYPERDOS_X86_EXECUTION_INTERRUPT_NOT_HANDLED,
    HYPERDOS_X86_EXECUTION_DIVIDE_ERROR
} hyperdos_x86_execution_result;

typedef enum hyperdos_x86_general_register_index
{
    HYPERDOS_X86_GENERAL_REGISTER_ACCUMULATOR       = 0,
    HYPERDOS_X86_GENERAL_REGISTER_COUNTER           = 1,
    HYPERDOS_X86_GENERAL_REGISTER_DATA              = 2,
    HYPERDOS_X86_GENERAL_REGISTER_BASE              = 3,
    HYPERDOS_X86_GENERAL_REGISTER_STACK_POINTER     = 4,
    HYPERDOS_X86_GENERAL_REGISTER_BASE_POINTER      = 5,
    HYPERDOS_X86_GENERAL_REGISTER_SOURCE_INDEX      = 6,
    HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX = 7
} hyperdos_x86_general_register_index;

typedef enum hyperdos_x86_segment_register_index
{
    HYPERDOS_X86_SEGMENT_REGISTER_EXTRA = 0,
    HYPERDOS_X86_SEGMENT_REGISTER_CODE  = 1,
    HYPERDOS_X86_SEGMENT_REGISTER_STACK = 2,
    HYPERDOS_X86_SEGMENT_REGISTER_DATA  = 3
} hyperdos_x86_segment_register_index;

typedef enum hyperdos_x86_flag_mask
{
    HYPERDOS_X86_FLAG_CARRY            = 0x0001u,
    HYPERDOS_X86_FLAG_RESERVED         = 0xF002u,
    HYPERDOS_X86_FLAG_PARITY           = 0x0004u,
    HYPERDOS_X86_FLAG_AUXILIARY_CARRY  = 0x0010u,
    HYPERDOS_X86_FLAG_ZERO             = 0x0040u,
    HYPERDOS_X86_FLAG_SIGN             = 0x0080u,
    HYPERDOS_X86_FLAG_TRAP             = 0x0100u,
    HYPERDOS_X86_FLAG_INTERRUPT_ENABLE = 0x0200u,
    HYPERDOS_X86_FLAG_DIRECTION        = 0x0400u,
    HYPERDOS_X86_FLAG_OVERFLOW         = 0x0800u
} hyperdos_x86_flag_mask;

typedef enum hyperdos_x86_processor_model
{
    HYPERDOS_X86_PROCESSOR_MODEL_8086  = 0,
    HYPERDOS_X86_PROCESSOR_MODEL_8088  = 1,
    HYPERDOS_X86_PROCESSOR_MODEL_80186 = 2,
    HYPERDOS_X86_PROCESSOR_MODEL_80188 = 3
} hyperdos_x86_processor_model;

struct hyperdos_x86_processor;
struct hyperdos_bus;

typedef hyperdos_x86_execution_result (*hyperdos_x86_interrupt_handler)(struct hyperdos_x86_processor* processor,
                                                                        uint8_t                        interruptNumber,
                                                                        void*                          userContext);

typedef struct hyperdos_x86_coprocessor_instruction
{
    uint8_t                             operationCode;
    uint8_t                             registerMemoryByte;
    uint8_t                             isRegister;
    uint8_t                             registerIndex;
    uint8_t                             operationIndex;
    hyperdos_x86_segment_register_index segmentRegister;
    uint16_t                            offset;
} hyperdos_x86_coprocessor_instruction;

typedef hyperdos_x86_execution_result (*hyperdos_x86_coprocessor_wait_handler)(struct hyperdos_x86_processor* processor,
                                                                               void* userContext);

typedef hyperdos_x86_execution_result (*hyperdos_x86_coprocessor_escape_handler)(
        struct hyperdos_x86_processor*              processor,
        const hyperdos_x86_coprocessor_instruction* instruction,
        void*                                       userContext);

typedef struct hyperdos_x86_processor
{
    uint32_t                                generalRegisters[8];
    uint16_t                                segmentRegisters[4];
    uint32_t                                segmentBases[4];
    uint32_t                                instructionPointer;
    uint32_t                                flags;
    uint8_t*                                memory;
    size_t                                  memorySize;
    uint64_t                                executedInstructionCount;
    uint8_t                                 halted;
    uint8_t                                 maskableInterruptInhibitCount;
    uint8_t                                 lastOperationCode;
    uint16_t                                lastInstructionSegment;
    uint32_t                                lastInstructionOffset;
    uint8_t                                 processorModel;
    uint8_t                                 divideErrorReturnsToFaultingInstruction;
    uint8_t                                 escapeTrapEnabled;
    struct hyperdos_bus*                    bus;
    hyperdos_x86_interrupt_handler          interruptHandler;
    void*                                   userContext;
    hyperdos_x86_coprocessor_wait_handler   coprocessorWaitHandler;
    hyperdos_x86_coprocessor_escape_handler coprocessorEscapeHandler;
    void*                                   coprocessorContext;
} hyperdos_x86_processor;

hyperdos_x86_execution_result hyperdos_x86_initialize_processor(hyperdos_x86_processor* processor,
                                                                uint8_t*                memory,
                                                                size_t                  memorySize);

void hyperdos_x86_reset_processor(hyperdos_x86_processor* processor);

void hyperdos_x86_set_processor_model(hyperdos_x86_processor* processor, hyperdos_x86_processor_model model);

void hyperdos_x86_set_interrupt_handler(hyperdos_x86_processor*        processor,
                                        hyperdos_x86_interrupt_handler interruptHandler,
                                        void*                          userContext);

void hyperdos_x86_attach_coprocessor(hyperdos_x86_processor*                 processor,
                                     hyperdos_x86_coprocessor_wait_handler   waitHandler,
                                     hyperdos_x86_coprocessor_escape_handler escapeHandler,
                                     void*                                   userContext);

void hyperdos_x86_set_escape_trap_enabled(hyperdos_x86_processor* processor, int enabled);

void hyperdos_x86_attach_bus(hyperdos_x86_processor* processor, struct hyperdos_bus* bus);

hyperdos_x86_execution_result hyperdos_x86_load_dos_program(hyperdos_x86_processor* processor,
                                                            const uint8_t*          programBytes,
                                                            size_t                  programSize,
                                                            uint16_t                programSegment,
                                                            const char*             commandTail,
                                                            size_t                  commandTailLength);

hyperdos_x86_execution_result hyperdos_x86_execute(hyperdos_x86_processor* processor, uint64_t instructionLimit);

int hyperdos_x86_processor_accepts_maskable_interrupt(const hyperdos_x86_processor* processor);

hyperdos_x86_execution_result hyperdos_x86_request_maskable_interrupt(hyperdos_x86_processor* processor,
                                                                      uint8_t                 interruptNumber);

void hyperdos_x86_stop_processor(hyperdos_x86_processor* processor);

uint32_t hyperdos_x86_get_physical_address(const hyperdos_x86_processor*       processor,
                                           hyperdos_x86_segment_register_index segmentRegister,
                                           uint16_t                            offset);

hyperdos_x86_execution_result hyperdos_x86_read_memory_byte(const hyperdos_x86_processor*       processor,
                                                            hyperdos_x86_segment_register_index segmentRegister,
                                                            uint16_t                            offset,
                                                            uint8_t*                            value);

hyperdos_x86_execution_result hyperdos_x86_write_memory_byte(hyperdos_x86_processor*             processor,
                                                             hyperdos_x86_segment_register_index segmentRegister,
                                                             uint16_t                            offset,
                                                             uint8_t                             value);

uint8_t hyperdos_x86_read_input_output_byte(hyperdos_x86_processor* processor, uint16_t port);

void hyperdos_x86_write_input_output_byte(hyperdos_x86_processor* processor, uint16_t port, uint8_t value);

uint32_t hyperdos_x86_get_general_register(const hyperdos_x86_processor*       processor,
                                           hyperdos_x86_general_register_index registerIndex);

void hyperdos_x86_set_general_register(hyperdos_x86_processor*             processor,
                                       hyperdos_x86_general_register_index registerIndex,
                                       uint32_t                            value);

static inline uint16_t hyperdos_x86_get_general_register_word(const hyperdos_x86_processor*       processor,
                                                              hyperdos_x86_general_register_index registerIndex)
{
    if (processor == NULL || registerIndex > HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX)
    {
        return 0;
    }
    return (uint16_t)(processor->generalRegisters[registerIndex] & HYPERDOS_X86_WORD_MASK);
}

static inline void hyperdos_x86_set_general_register_word(hyperdos_x86_processor*             processor,
                                                          hyperdos_x86_general_register_index registerIndex,
                                                          uint16_t                            value)
{
    if (processor == NULL || registerIndex > HYPERDOS_X86_GENERAL_REGISTER_DESTINATION_INDEX)
    {
        return;
    }
    processor->generalRegisters[registerIndex] = (processor->generalRegisters[registerIndex] & 0xFFFF0000u) | value;
}

uint16_t hyperdos_x86_get_segment_register(const hyperdos_x86_processor*       processor,
                                           hyperdos_x86_segment_register_index segmentRegister);

void hyperdos_x86_set_segment_register(hyperdos_x86_processor*             processor,
                                       hyperdos_x86_segment_register_index segmentRegister,
                                       uint16_t                            value);

const char* hyperdos_x86_execution_result_name(hyperdos_x86_execution_result result);

#endif
