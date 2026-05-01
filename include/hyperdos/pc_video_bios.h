#ifndef HYPERDOS_PC_VIDEO_BIOS_H
#define HYPERDOS_PC_VIDEO_BIOS_H

#include <stdint.h>

#include "hyperdos/pc_board.h"

typedef void (*hyperdos_pc_video_bios_trace_function)(void* userContext, const char* message);

typedef struct hyperdos_pc_video_bios_interface
{
    void*        userContext;
    hyperdos_pc* pc;
    uint8_t (*getVideoMode)(void* userContext);
    uint16_t (*getVideoColumnCount)(void* userContext);
    void (*setVideoMode)(void* userContext, uint8_t mode);
    void (*setActiveVideoPage)(void* userContext, uint8_t videoPage);
    void (*setTextCursorPosition)(void* userContext, uint8_t videoPage, uint16_t row, uint16_t column);
    uint16_t (*getTextCursorPosition)(void* userContext, uint8_t videoPage);
    uint16_t (*readTextCharacterAttributeAtCursor)(void* userContext, uint8_t videoPage);
    void (*initializeStaticFunctionalityTable)(void* userContext);
    void (*scrollTextRegion)(void*    userContext,
                             uint16_t firstRow,
                             uint16_t firstColumn,
                             uint16_t lastRow,
                             uint16_t lastColumn,
                             uint8_t  lineCount,
                             uint8_t  attribute,
                             int      scrollDown);
    void (*writeCharacterRepeated)(void*    userContext,
                                   uint8_t  videoPage,
                                   uint8_t  character,
                                   uint8_t  attribute,
                                   uint16_t count,
                                   int      updateAttribute);
    void (*writePixel)(void* userContext, uint8_t videoPage, uint16_t column, uint16_t row, uint8_t color);
    uint8_t (*readPixel)(void* userContext, uint8_t videoPage, uint16_t column, uint16_t row);
    void (*writeTeletypeCharacter)(void* userContext, uint8_t character, uint8_t attribute);
    void (*writeString)(hyperdos_x86_16_processor* processor,
                        void*                      userContext,
                        uint8_t                    writeMode,
                        uint8_t                    videoPage,
                        uint8_t                    attribute,
                        uint16_t                   characterCount,
                        uint16_t                   row,
                        uint16_t                   column);
    void (*writeFunctionalityStateTable)(hyperdos_x86_16_processor* processor, void* userContext);
    uint16_t (*getSaveRestoreStateBufferBlocks)(uint16_t requestedState);
    int (*writeSaveRestoreState)(hyperdos_x86_16_processor* processor,
                                 void*                      userContext,
                                 uint16_t                   requestedState,
                                 uint16_t                   bufferOffset);
    int (*restoreSaveRestoreState)(hyperdos_x86_16_processor* processor,
                                   void*                      userContext,
                                   uint16_t                   requestedState,
                                   uint16_t                   bufferOffset);
    int (*registerGroupIsSingleRegister)(uint16_t registerGroup);
    int (*readRegisterInterfaceRegister)(void*    userContext,
                                         uint16_t registerGroup,
                                         uint8_t  registerNumber,
                                         uint8_t* registerValue);
    int (*writeRegisterInterfaceRegister)(void*    userContext,
                                          uint16_t registerGroup,
                                          uint8_t  registerNumber,
                                          uint8_t  registerValue);
    void (*writeKoreanExtensionPlaceholderCharacterShape)(hyperdos_x86_16_processor* processor,
                                                          void*                      userContext,
                                                          uint16_t                   bufferOffset);
    hyperdos_pc_video_bios_trace_function traceFunction;
} hyperdos_pc_video_bios_interface;

hyperdos_x86_16_execution_result hyperdos_pc_video_bios_handle_interrupt(
        hyperdos_x86_16_processor*              processor,
        const hyperdos_pc_video_bios_interface* videoBiosInterface,
        uint8_t                                 serviceNumber);

#endif
