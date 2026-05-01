#ifndef HYPERDOS_X86_16_PROCESSOR_H
#define HYPERDOS_X86_16_PROCESSOR_H

#include <stddef.h>
#include <stdint.h>

#define HYPERDOS_X86_16_MEMORY_SIZE                     (1024u * 1024u)
#define HYPERDOS_X86_16_ADDRESS_MASK                    0xFFFFFu
#define HYPERDOS_X86_16_DOS_PROGRAM_OFFSET              0x0100u
#define HYPERDOS_X86_16_DEFAULT_DOS_SEGMENT             0x1000u
#define HYPERDOS_X86_16_DOS_INTERRUPT_TERMINATE_PROGRAM 0x20u
#define HYPERDOS_X86_16_DOS_INTERRUPT_FUNCTION_DISPATCH 0x21u
#define HYPERDOS_X86_16_DOS_SERVICE_PRINT_STRING        0x09u
#define HYPERDOS_X86_16_DOS_SERVICE_TERMINATE_PROCESS   0x4Cu
#define HYPERDOS_X86_16_DOS_PRINT_STRING_TERMINATOR     '$'

enum
{
    HYPERDOS_X86_16_BYTE_BIT_COUNT = 8u,
    HYPERDOS_X86_16_WORD_BIT_COUNT = 16u,
    HYPERDOS_X86_16_BYTE_SIZE      = 1u,
    HYPERDOS_X86_16_WORD_SIZE      = 2u,
    HYPERDOS_X86_16_SEGMENT_SHIFT  = 4u,
    HYPERDOS_X86_16_LOW_BYTE_MASK  = 0x00FFu,
    HYPERDOS_X86_16_BYTE_MASK      = HYPERDOS_X86_16_LOW_BYTE_MASK,
    HYPERDOS_X86_16_HIGH_BYTE_MASK = 0xFF00u,
    HYPERDOS_X86_16_WORD_MASK      = 0xFFFFu
};

typedef enum hyperdos_x86_16_execution_result
{
    HYPERDOS_X86_16_EXECUTION_OK = 0,
    HYPERDOS_X86_16_EXECUTION_HALTED,
    HYPERDOS_X86_16_EXECUTION_STEP_LIMIT_REACHED,
    HYPERDOS_X86_16_EXECUTION_INVALID_ARGUMENT,
    HYPERDOS_X86_16_EXECUTION_PROGRAM_TOO_LARGE,
    HYPERDOS_X86_16_EXECUTION_UNSUPPORTED_INSTRUCTION,
    HYPERDOS_X86_16_EXECUTION_INTERRUPT_NOT_HANDLED,
    HYPERDOS_X86_16_EXECUTION_DIVIDE_ERROR
} hyperdos_x86_16_execution_result;

typedef enum hyperdos_x86_16_general_register_index
{
    HYPERDOS_X86_16_GENERAL_REGISTER_ACCUMULATOR       = 0,
    HYPERDOS_X86_16_GENERAL_REGISTER_COUNTER           = 1,
    HYPERDOS_X86_16_GENERAL_REGISTER_DATA              = 2,
    HYPERDOS_X86_16_GENERAL_REGISTER_BASE              = 3,
    HYPERDOS_X86_16_GENERAL_REGISTER_STACK_POINTER     = 4,
    HYPERDOS_X86_16_GENERAL_REGISTER_BASE_POINTER      = 5,
    HYPERDOS_X86_16_GENERAL_REGISTER_SOURCE_INDEX      = 6,
    HYPERDOS_X86_16_GENERAL_REGISTER_DESTINATION_INDEX = 7
} hyperdos_x86_16_general_register_index;

typedef enum hyperdos_x86_16_segment_register_index
{
    HYPERDOS_X86_16_SEGMENT_REGISTER_EXTRA = 0,
    HYPERDOS_X86_16_SEGMENT_REGISTER_CODE  = 1,
    HYPERDOS_X86_16_SEGMENT_REGISTER_STACK = 2,
    HYPERDOS_X86_16_SEGMENT_REGISTER_DATA  = 3
} hyperdos_x86_16_segment_register_index;

typedef enum hyperdos_x86_16_flag_mask
{
    HYPERDOS_X86_16_FLAG_CARRY            = 0x0001u,
    HYPERDOS_X86_16_FLAG_RESERVED         = 0xF002u,
    HYPERDOS_X86_16_FLAG_PARITY           = 0x0004u,
    HYPERDOS_X86_16_FLAG_AUXILIARY_CARRY  = 0x0010u,
    HYPERDOS_X86_16_FLAG_ZERO             = 0x0040u,
    HYPERDOS_X86_16_FLAG_SIGN             = 0x0080u,
    HYPERDOS_X86_16_FLAG_TRAP             = 0x0100u,
    HYPERDOS_X86_16_FLAG_INTERRUPT_ENABLE = 0x0200u,
    HYPERDOS_X86_16_FLAG_DIRECTION        = 0x0400u,
    HYPERDOS_X86_16_FLAG_OVERFLOW         = 0x0800u
} hyperdos_x86_16_flag_mask;

typedef enum hyperdos_x86_16_processor_model
{
    HYPERDOS_X86_16_PROCESSOR_MODEL_8086  = 0,
    HYPERDOS_X86_16_PROCESSOR_MODEL_80186 = 1
} hyperdos_x86_16_processor_model;

struct hyperdos_x86_16_processor;
struct hyperdos_bus;

typedef hyperdos_x86_16_execution_result (*hyperdos_x86_16_interrupt_handler)(
        struct hyperdos_x86_16_processor* processor,
        uint8_t                           interruptNumber,
        void*                             userContext);

typedef struct hyperdos_x86_16_coprocessor_instruction
{
    uint8_t                                operationCode;
    uint8_t                                registerMemoryByte;
    uint8_t                                isRegister;
    uint8_t                                registerIndex;
    uint8_t                                operationIndex;
    hyperdos_x86_16_segment_register_index segmentRegister;
    uint16_t                               offset;
} hyperdos_x86_16_coprocessor_instruction;

typedef hyperdos_x86_16_execution_result (
        *hyperdos_x86_16_coprocessor_wait_handler)(struct hyperdos_x86_16_processor* processor, void* userContext);

typedef hyperdos_x86_16_execution_result (*hyperdos_x86_16_coprocessor_escape_handler)(
        struct hyperdos_x86_16_processor*              processor,
        const hyperdos_x86_16_coprocessor_instruction* instruction,
        void*                                          userContext);

typedef struct hyperdos_x86_16_processor
{
    uint16_t                                   generalRegisters[8];
    uint16_t                                   segmentRegisters[4];
    uint32_t                                   segmentBases[4];
    uint16_t                                   instructionPointer;
    uint16_t                                   flags;
    uint8_t*                                   memory;
    size_t                                     memorySize;
    uint64_t                                   executedInstructionCount;
    uint8_t                                    halted;
    uint8_t                                    maskableInterruptInhibitCount;
    uint8_t                                    lastOperationCode;
    uint16_t                                   lastInstructionSegment;
    uint16_t                                   lastInstructionOffset;
    uint8_t                                    processorModel;
    uint8_t                                    divideErrorReturnsToFaultingInstruction;
    struct hyperdos_bus*                       bus;
    hyperdos_x86_16_interrupt_handler          interruptHandler;
    void*                                      userContext;
    hyperdos_x86_16_coprocessor_wait_handler   coprocessorWaitHandler;
    hyperdos_x86_16_coprocessor_escape_handler coprocessorEscapeHandler;
    void*                                      coprocessorContext;
} hyperdos_x86_16_processor;

hyperdos_x86_16_execution_result hyperdos_x86_16_initialize_processor(hyperdos_x86_16_processor* processor,
                                                                      uint8_t*                   memory,
                                                                      size_t                     memorySize);

void hyperdos_x86_16_reset_processor(hyperdos_x86_16_processor* processor);

void hyperdos_x86_16_set_processor_model(hyperdos_x86_16_processor* processor, hyperdos_x86_16_processor_model model);

void hyperdos_x86_16_set_interrupt_handler(hyperdos_x86_16_processor*        processor,
                                           hyperdos_x86_16_interrupt_handler interruptHandler,
                                           void*                             userContext);

void hyperdos_x86_16_attach_coprocessor(hyperdos_x86_16_processor*                 processor,
                                        hyperdos_x86_16_coprocessor_wait_handler   waitHandler,
                                        hyperdos_x86_16_coprocessor_escape_handler escapeHandler,
                                        void*                                      userContext);

void hyperdos_x86_16_attach_bus(hyperdos_x86_16_processor* processor, struct hyperdos_bus* bus);

hyperdos_x86_16_execution_result hyperdos_x86_16_load_dos_program(hyperdos_x86_16_processor* processor,
                                                                  const uint8_t*             programBytes,
                                                                  size_t                     programSize,
                                                                  uint16_t                   programSegment,
                                                                  const char*                commandTail,
                                                                  size_t                     commandTailLength);

hyperdos_x86_16_execution_result hyperdos_x86_16_execute(hyperdos_x86_16_processor* processor,
                                                         uint64_t                   instructionLimit);

int hyperdos_x86_16_processor_accepts_maskable_interrupt(const hyperdos_x86_16_processor* processor);

hyperdos_x86_16_execution_result hyperdos_x86_16_request_maskable_interrupt(hyperdos_x86_16_processor* processor,
                                                                            uint8_t                    interruptNumber);

void hyperdos_x86_16_stop_processor(hyperdos_x86_16_processor* processor);

uint32_t hyperdos_x86_16_get_physical_address(const hyperdos_x86_16_processor*       processor,
                                              hyperdos_x86_16_segment_register_index segmentRegister,
                                              uint16_t                               offset);

hyperdos_x86_16_execution_result hyperdos_x86_16_read_memory_byte(
        const hyperdos_x86_16_processor*       processor,
        hyperdos_x86_16_segment_register_index segmentRegister,
        uint16_t                               offset,
        uint8_t*                               value);

hyperdos_x86_16_execution_result hyperdos_x86_16_write_memory_byte(
        hyperdos_x86_16_processor*             processor,
        hyperdos_x86_16_segment_register_index segmentRegister,
        uint16_t                               offset,
        uint8_t                                value);

uint8_t hyperdos_x86_16_read_input_output_byte(hyperdos_x86_16_processor* processor, uint16_t port);

void hyperdos_x86_16_write_input_output_byte(hyperdos_x86_16_processor* processor, uint16_t port, uint8_t value);

uint16_t hyperdos_x86_16_get_general_register(const hyperdos_x86_16_processor*       processor,
                                              hyperdos_x86_16_general_register_index registerIndex);

void hyperdos_x86_16_set_general_register(hyperdos_x86_16_processor*             processor,
                                          hyperdos_x86_16_general_register_index registerIndex,
                                          uint16_t                               value);

uint16_t hyperdos_x86_16_get_segment_register(const hyperdos_x86_16_processor*       processor,
                                              hyperdos_x86_16_segment_register_index segmentRegister);

void hyperdos_x86_16_set_segment_register(hyperdos_x86_16_processor*             processor,
                                          hyperdos_x86_16_segment_register_index segmentRegister,
                                          uint16_t                               value);

const char* hyperdos_x86_16_execution_result_name(hyperdos_x86_16_execution_result result);

#endif
