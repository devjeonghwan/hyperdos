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
    HYPERDOS_X86_BYTE_BIT_COUNT         = 8u,
    HYPERDOS_X86_WORD_BIT_COUNT         = 16u,
    HYPERDOS_X86_BYTE_SIZE              = 1u,
    HYPERDOS_X86_WORD_SIZE              = 2u,
    HYPERDOS_X86_CONTROL_REGISTER_COUNT = 4u,
    HYPERDOS_X86_SEGMENT_SHIFT          = 4u,
    HYPERDOS_X86_LOW_BYTE_MASK          = 0x00FFu,
    HYPERDOS_X86_BYTE_MASK              = HYPERDOS_X86_LOW_BYTE_MASK,
    HYPERDOS_X86_HIGH_BYTE_MASK         = 0xFF00u,
    HYPERDOS_X86_WORD_MASK              = 0xFFFFu
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
    HYPERDOS_X86_EXECUTION_DIVIDE_ERROR,
    HYPERDOS_X86_EXECUTION_PROCESSOR_SHUTDOWN
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

#define HYPERDOS_X86_CONTROL_REGISTER_ZERO_PROTECTION_ENABLE   UINT32_C(0x00000001)
#define HYPERDOS_X86_CONTROL_REGISTER_ZERO_MONITOR_COPROCESSOR UINT32_C(0x00000002)
#define HYPERDOS_X86_CONTROL_REGISTER_ZERO_EMULATE_COPROCESSOR UINT32_C(0x00000004)
#define HYPERDOS_X86_CONTROL_REGISTER_ZERO_TASK_SWITCHED       UINT32_C(0x00000008)
#define HYPERDOS_X86_CONTROL_REGISTER_ZERO_EXTENSION_TYPE      UINT32_C(0x00000010)
#define HYPERDOS_X86_CONTROL_REGISTER_ZERO_NUMERIC_ERROR       UINT32_C(0x00000020)
#define HYPERDOS_X86_CONTROL_REGISTER_ZERO_WRITE_PROTECT       UINT32_C(0x00010000)
#define HYPERDOS_X86_CONTROL_REGISTER_ZERO_ALIGNMENT_MASK      UINT32_C(0x00040000)
#define HYPERDOS_X86_CONTROL_REGISTER_ZERO_NOT_WRITE_THROUGH   UINT32_C(0x20000000)
#define HYPERDOS_X86_CONTROL_REGISTER_ZERO_CACHE_DISABLE       UINT32_C(0x40000000)
#define HYPERDOS_X86_CONTROL_REGISTER_ZERO_PAGING_ENABLE       UINT32_C(0x80000000)

typedef enum hyperdos_x86_processor_model
{
    HYPERDOS_X86_PROCESSOR_MODEL_8086     = 0,
    HYPERDOS_X86_PROCESSOR_MODEL_8088     = 1,
    HYPERDOS_X86_PROCESSOR_MODEL_80186    = 2,
    HYPERDOS_X86_PROCESSOR_MODEL_80188    = 3,
    HYPERDOS_X86_PROCESSOR_MODEL_80286    = 4,
    HYPERDOS_X86_PROCESSOR_MODEL_80386_DX = 5,
    HYPERDOS_X86_PROCESSOR_MODEL_80386_SX = 6,
    HYPERDOS_X86_PROCESSOR_MODEL_80486    = 7
} hyperdos_x86_processor_model;

typedef struct hyperdos_x86_segment_state
{
    uint16_t selector;
    uint32_t base;
    uint32_t limit;
    uint16_t attributes;
} hyperdos_x86_segment_state;

typedef struct hyperdos_x86_descriptor_table_state
{
    uint32_t base;
    uint16_t limit;
} hyperdos_x86_descriptor_table_state;

struct hyperdos_x86_processor;
struct hyperdos_bus;

typedef hyperdos_x86_execution_result (*hyperdos_x86_interrupt_handler)(struct hyperdos_x86_processor* processor,
                                                                        uint8_t                        interruptNumber,
                                                                        void*                          userContext);

typedef void (*hyperdos_x86_page_translation_cache_invalidation_handler)(struct hyperdos_x86_processor* processor,
                                                                         uint32_t                       linearAddress,
                                                                         uint8_t                        allEntries,
                                                                         void*                          userContext);

typedef void (*hyperdos_x86_internal_cache_invalidation_handler)(struct hyperdos_x86_processor* processor,
                                                                 uint8_t                        writeBackModifiedLines,
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
    uint32_t                                                 generalRegisters[8];
    hyperdos_x86_segment_state                               segmentStates[4];
    hyperdos_x86_descriptor_table_state                      globalDescriptorTable;
    hyperdos_x86_descriptor_table_state                      localDescriptorTable;
    hyperdos_x86_descriptor_table_state                      interruptDescriptorTable;
    hyperdos_x86_segment_state                               taskRegister;
    uint32_t                                                 instructionPointer;
    uint32_t                                                 flags;
    uint16_t                                                 machineStatusWord;
    uint32_t                                                 controlRegisters[HYPERDOS_X86_CONTROL_REGISTER_COUNT];
    uint16_t                                                 relocationRegister;
    uint16_t                                                 localDescriptorTableSelector;
    uint16_t                                                 taskRegisterSelector;
    uint64_t                                                 executedInstructionCount;
    uint64_t                                                 externalBusCycleCount;
    uint64_t                                                 translationLookasideBufferInvalidationCount;
    uint64_t                                                 internalCacheInvalidationCount;
    uint32_t                                                 lastInvalidatedLinearAddress;
    uint8_t                                                  lastInvalidatedLinearAddressValid;
    uint8_t                                                  halted;
    uint8_t                                                  maskableInterruptInhibitCount;
    uint8_t                                                  pendingExceptionActive;
    uint8_t                                                  pendingExceptionType;
    uint8_t                                                  pendingExceptionHasErrorCode;
    uint16_t                                                 pendingExceptionErrorCode;
    uint8_t                                                  exceptionDeliveryActive;
    uint8_t                                                  exceptionDeliveryType;
    uint8_t                                                  processorShutdownActive;
    uint8_t                                                  lastOperationCode;
    uint16_t                                                 lastInstructionSegment;
    uint32_t                                                 lastInstructionOffset;
    uint8_t                                                  processorModel;
    uint8_t                                                  divideErrorReturnsToFaultingInstruction;
    struct hyperdos_bus*                                     bus;
    hyperdos_x86_page_translation_cache_invalidation_handler pageTranslationCacheInvalidationHandler;
    void*                                                    pageTranslationCacheInvalidationContext;
    hyperdos_x86_internal_cache_invalidation_handler         internalCacheInvalidationHandler;
    void*                                                    internalCacheInvalidationContext;
    hyperdos_x86_interrupt_handler                           interruptHandler;
    void*                                                    userContext;
    hyperdos_x86_coprocessor_wait_handler                    coprocessorWaitHandler;
    hyperdos_x86_coprocessor_escape_handler                  coprocessorEscapeHandler;
    void*                                                    coprocessorContext;
} hyperdos_x86_processor;

hyperdos_x86_execution_result hyperdos_x86_initialize_processor(hyperdos_x86_processor* processor);

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

uint16_t hyperdos_x86_get_relocation_register(const hyperdos_x86_processor* processor);

void hyperdos_x86_set_relocation_register(hyperdos_x86_processor* processor, uint16_t value);

void hyperdos_x86_attach_bus(hyperdos_x86_processor* processor, struct hyperdos_bus* bus);

uint8_t hyperdos_x86_get_external_data_bus_byte_count(const hyperdos_x86_processor* processor);

uint32_t hyperdos_x86_get_external_address_mask(const hyperdos_x86_processor* processor);

uint32_t hyperdos_x86_get_control_register(const hyperdos_x86_processor* processor, uint8_t registerIndex);

void hyperdos_x86_set_control_register(hyperdos_x86_processor* processor, uint8_t registerIndex, uint32_t value);

int hyperdos_x86_control_register_zero_cache_disable_enabled(const hyperdos_x86_processor* processor);

int hyperdos_x86_control_register_zero_not_write_through_enabled(const hyperdos_x86_processor* processor);

void hyperdos_x86_set_page_translation_cache_invalidation_handler(
        hyperdos_x86_processor*                                  processor,
        hyperdos_x86_page_translation_cache_invalidation_handler invalidationHandler,
        void*                                                    userContext);

void hyperdos_x86_set_internal_cache_invalidation_handler(
        hyperdos_x86_processor*                          processor,
        hyperdos_x86_internal_cache_invalidation_handler invalidationHandler,
        void*                                            userContext);

void hyperdos_x86_invalidate_page_translation_cache_entry(hyperdos_x86_processor* processor, uint32_t linearAddress);

void hyperdos_x86_invalidate_all_page_translation_cache_entries(hyperdos_x86_processor* processor);

void hyperdos_x86_invalidate_internal_cache(hyperdos_x86_processor* processor);

void hyperdos_x86_write_back_and_invalidate_internal_cache(hyperdos_x86_processor* processor);

hyperdos_x86_execution_result hyperdos_x86_load_dos_program(hyperdos_x86_processor* processor,
                                                            const uint8_t*          programBytes,
                                                            size_t                  programSize,
                                                            uint16_t                programSegment,
                                                            const char*             commandTail,
                                                            size_t                  commandTailLength);

hyperdos_x86_execution_result hyperdos_x86_execute(hyperdos_x86_processor* processor, uint64_t instructionLimit);

uint64_t hyperdos_x86_get_external_bus_cycle_count(const hyperdos_x86_processor* processor);

void hyperdos_x86_reset_external_bus_cycle_count(hyperdos_x86_processor* processor);

int hyperdos_x86_processor_accepts_maskable_interrupt(const hyperdos_x86_processor* processor);

hyperdos_x86_execution_result hyperdos_x86_request_maskable_interrupt(hyperdos_x86_processor* processor,
                                                                      uint8_t                 interruptNumber);

void hyperdos_x86_stop_processor(hyperdos_x86_processor* processor);

uint32_t hyperdos_x86_get_physical_address(const hyperdos_x86_processor*       processor,
                                           hyperdos_x86_segment_register_index segmentRegister,
                                           uint16_t                            offset);

hyperdos_x86_execution_result hyperdos_x86_translate_logical_to_linear_address(
        const hyperdos_x86_processor*       processor,
        hyperdos_x86_segment_register_index segmentRegister,
        uint16_t                            offset,
        uint32_t*                           linearAddress);

hyperdos_x86_execution_result hyperdos_x86_translate_linear_to_physical_address(const hyperdos_x86_processor* processor,
                                                                                uint32_t  linearAddress,
                                                                                uint32_t* physicalAddress);

hyperdos_x86_execution_result hyperdos_x86_translate_logical_to_physical_address(
        const hyperdos_x86_processor*       processor,
        hyperdos_x86_segment_register_index segmentRegister,
        uint16_t                            offset,
        uint32_t*                           physicalAddress);

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

uint8_t hyperdos_x86_get_general_register_byte(const hyperdos_x86_processor* processor, uint8_t byteRegisterIndex);

void hyperdos_x86_set_general_register_byte(hyperdos_x86_processor* processor,
                                            uint8_t                 byteRegisterIndex,
                                            uint8_t                 value);

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

uint32_t hyperdos_x86_get_segment_base(const hyperdos_x86_processor*       processor,
                                       hyperdos_x86_segment_register_index segmentRegister);

uint32_t hyperdos_x86_get_segment_limit(const hyperdos_x86_processor*       processor,
                                        hyperdos_x86_segment_register_index segmentRegister);

uint16_t hyperdos_x86_get_segment_attributes(const hyperdos_x86_processor*       processor,
                                             hyperdos_x86_segment_register_index segmentRegister);

uint16_t hyperdos_x86_get_instruction_pointer_word(const hyperdos_x86_processor* processor);

void hyperdos_x86_set_instruction_pointer_word(hyperdos_x86_processor* processor, uint16_t value);

uint32_t hyperdos_x86_get_instruction_pointer_dword(const hyperdos_x86_processor* processor);

void hyperdos_x86_set_instruction_pointer_dword(hyperdos_x86_processor* processor, uint32_t value);

uint16_t hyperdos_x86_get_flags_word(const hyperdos_x86_processor* processor);

void hyperdos_x86_set_flags_word(hyperdos_x86_processor* processor, uint16_t value);

int hyperdos_x86_get_flag(const hyperdos_x86_processor* processor, hyperdos_x86_flag_mask flagMask);

void hyperdos_x86_set_flag(hyperdos_x86_processor* processor, hyperdos_x86_flag_mask flagMask, int enabled);

void hyperdos_x86_update_flags_word(hyperdos_x86_processor* processor, uint16_t flagMask, uint16_t flagValues);

const char* hyperdos_x86_execution_result_name(hyperdos_x86_execution_result result);

#endif
