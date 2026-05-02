#ifndef HYPERDOS_DEVICES_H
#define HYPERDOS_DEVICES_H

#include <stddef.h>
#include <stdint.h>

#include "hyperdos/hardware.h"
#include "hyperdos/x86_processor.h"

#define HYPERDOS_CONVENTIONAL_MEMORY_SIZE                                         0xA0000u
#define HYPERDOS_COLOR_GRAPHICS_ADAPTER_MEMORY_ADDRESS                            0xB8000u
#define HYPERDOS_COLOR_GRAPHICS_ADAPTER_MEMORY_SIZE                               0x4000u
#define HYPERDOS_COLOR_GRAPHICS_ADAPTER_TEXT_MEMORY_ADDRESS                       0xB8000u
#define HYPERDOS_COLOR_GRAPHICS_ADAPTER_TEXT_MEMORY_SIZE                          0x4000u
#define HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_ADDRESS                              0xA0000u
#define HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_SIZE                                 0x10000u
#define HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_APERTURE_ADDRESS                     0xA0000u
#define HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_APERTURE_SIZE                        0x20000u
#define HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT                                 4u
#define HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_REGISTER_COUNT                    32u
#define HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_REGISTER_COUNT                    8u
#define HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_REGISTER_COUNT          16u
#define HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_CONVERTER_COLOR_COUNT     256u
#define HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_CONVERTER_COMPONENT_COUNT 3u
#define HYPERDOS_COLOR_GRAPHICS_ADAPTER_COLUMN_COUNT                              80u
#define HYPERDOS_COLOR_GRAPHICS_ADAPTER_ROW_COUNT                                 25u
#define HYPERDOS_COLOR_GRAPHICS_ADAPTER_CELL_BYTE_COUNT                           2u
#define HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_WIDTH_LOW                        320u
#define HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_WIDTH_HIGH                       640u
#define HYPERDOS_COLOR_GRAPHICS_ADAPTER_GRAPHICS_HEIGHT                           200u
#define HYPERDOS_VIDEO_GRAPHICS_ARRAY_MAXIMUM_DISPLAY_WIDTH                       640u
#define HYPERDOS_VIDEO_GRAPHICS_ARRAY_MAXIMUM_DISPLAY_HEIGHT                      480u
#define HYPERDOS_COLOR_GRAPHICS_ADAPTER_INPUT_OUTPUT_PORT                         0x03C0u
#define HYPERDOS_COLOR_GRAPHICS_ADAPTER_INPUT_OUTPUT_PORT_COUNT                   32u
#define HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_GRAPHICS                             0x02u
#define HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_HIGH_RESOLUTION                      0x10u
#define HYPERDOS_COLOR_GRAPHICS_ADAPTER_MODE_VIDEO_ENABLE                         0x08u
#define HYPERDOS_KEYBOARD_CONTROLLER_OUTPUT_QUEUE_CAPACITY                        64u
#define HYPERDOS_8087_CONTROL_WORD_DEFAULT                                        0x037Fu
#define HYPERDOS_8087_STATUS_WORD_DEFAULT                                         0x0000u
#define HYPERDOS_8087_TAG_WORD_DEFAULT                                            0xFFFFu
#define HYPERDOS_8087_REGISTER_COUNT                                              8u

typedef struct hyperdos_random_access_memory
{
    uint8_t* bytes;
    size_t   byteCount;
    uint32_t firstAddress;
} hyperdos_random_access_memory;

typedef struct hyperdos_read_only_memory
{
    const uint8_t* bytes;
    size_t         byteCount;
    uint32_t       firstAddress;
} hyperdos_read_only_memory;

typedef struct hyperdos_programmable_interrupt_controller
{
    uint8_t interruptRequestRegister;
    uint8_t inServiceRegister;
    uint8_t interruptMaskRegister;
    uint8_t vectorBase;
    uint8_t initializationCommandWordStep;
    uint8_t expectsInitializationCommandWordFour;
    uint8_t readInServiceRegister;
    uint8_t autoEndOfInterrupt;
} hyperdos_programmable_interrupt_controller;

typedef struct hyperdos_direct_memory_access_channel
{
    uint16_t baseAddress;
    uint16_t currentAddress;
    uint16_t baseCount;
    uint16_t currentCount;
    uint8_t  mode;
    uint8_t  masked;
} hyperdos_direct_memory_access_channel;

typedef struct hyperdos_direct_memory_access_controller
{
    hyperdos_direct_memory_access_channel channels[4];
    uint8_t                               command;
    uint8_t                               status;
    uint8_t                               request;
    uint8_t                               temporary;
    uint8_t                               firstLastFlipFlop;
} hyperdos_direct_memory_access_controller;

typedef struct hyperdos_x86_interval_timer_channel
{
    uint16_t reloadValue;
    uint16_t currentValue;
    uint16_t latchedValue;
    uint32_t reloadCounter;
    uint32_t currentCounter;
    uint8_t  accessMode;
    uint8_t  operatingMode;
    uint8_t  writeLatchLowByte;
    uint8_t  readLatchLowByte;
    uint8_t  countLatched;
    uint8_t  latchedReadLowByte;
    uint8_t  outputLevel;
    uint8_t  outputTransitionPending;
} hyperdos_x86_interval_timer_channel;

typedef struct hyperdos_programmable_interval_timer
{
    hyperdos_x86_interval_timer_channel channels[3];
} hyperdos_programmable_interval_timer;

typedef struct hyperdos_programmable_peripheral_interface
{
    uint8_t portA;
    uint8_t portB;
    uint8_t portC;
    uint8_t control;
} hyperdos_programmable_peripheral_interface;

typedef struct hyperdos_intel_8042_keyboard_controller
{
    uint8_t outputQueue[HYPERDOS_KEYBOARD_CONTROLLER_OUTPUT_QUEUE_CAPACITY];
    uint8_t outputQueueAuxiliaryDevice[HYPERDOS_KEYBOARD_CONTROLLER_OUTPUT_QUEUE_CAPACITY];
    size_t  outputQueueReadIndex;
    size_t  outputQueueWriteIndex;
    size_t  outputQueueCount;
    uint8_t commandByte;
    uint8_t statusRegister;
    uint8_t inputPort;
    uint8_t outputPort;
    uint8_t latchedOutputByte;
    uint8_t latchedOutputByteAvailable;
    uint8_t pendingControllerCommand;
    uint8_t pendingKeyboardCommand;
    uint8_t keyboardScanCodeSet;
    uint8_t keyboardScanningEnabled;
    uint8_t pendingAuxiliaryDeviceCommand;
    uint8_t auxiliaryDeviceSampleRate;
    uint8_t auxiliaryDeviceResolution;
    uint8_t auxiliaryDeviceScalingTwoToOne;
    uint8_t auxiliaryDeviceReportingEnabled;
    uint8_t auxiliaryDeviceButtonMask;
    uint8_t auxiliaryDeviceDisabled;
    uint8_t keyboardInterruptRequestPending;
    uint8_t auxiliaryDeviceInterruptRequestPending;
} hyperdos_intel_8042_keyboard_controller;

typedef struct hyperdos_universal_asynchronous_receiver_transmitter
{
    uint8_t receiverBuffer;
    uint8_t transmitterHolding;
    uint8_t interruptEnable;
    uint8_t interruptIdentification;
    uint8_t lineControl;
    uint8_t modemControl;
    uint8_t lineStatus;
    uint8_t modemStatus;
    uint8_t divisorLowByte;
    uint8_t divisorHighByte;
} hyperdos_universal_asynchronous_receiver_transmitter;

typedef struct hyperdos_color_graphics_adapter
{
    uint8_t memory[HYPERDOS_COLOR_GRAPHICS_ADAPTER_MEMORY_SIZE];
    uint8_t graphicsPlanes[HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT][HYPERDOS_VIDEO_GRAPHICS_ARRAY_MEMORY_SIZE];
    uint8_t graphicsLatch[HYPERDOS_VIDEO_GRAPHICS_ARRAY_PLANE_COUNT];
    uint8_t indexRegister;
    uint8_t registers[32];
    uint8_t modeControl;
    uint8_t colorSelect;
    uint8_t statusRegister;
    uint8_t attributeControllerIndexRegister;
    uint8_t attributeControllerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_ATTRIBUTE_REGISTER_COUNT];
    uint8_t attributeControllerAddressFlipFlop;
    uint8_t attributeControllerPaletteAddressSource;
    uint8_t sequencerIndexRegister;
    uint8_t sequencerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_SEQUENCER_REGISTER_COUNT];
    uint8_t graphicsControllerIndexRegister;
    uint8_t graphicsControllerRegisters[HYPERDOS_VIDEO_GRAPHICS_ARRAY_GRAPHICS_CONTROLLER_REGISTER_COUNT];
    uint8_t miscellaneousOutputRegister;
    uint8_t featureControlRegister;
    uint8_t digitalToAnalogConverterMaskRegister;
    uint8_t digitalToAnalogConverterReadIndexRegister;
    uint8_t digitalToAnalogConverterWriteIndexRegister;
    uint8_t digitalToAnalogConverterReadComponentIndex;
    uint8_t digitalToAnalogConverterWriteComponentIndex;
    uint8_t digitalToAnalogConverterState;
    uint8_t digitalToAnalogConverterPalette[HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_CONVERTER_COLOR_COUNT]
                                           [HYPERDOS_VIDEO_GRAPHICS_ARRAY_DIGITAL_TO_ANALOG_CONVERTER_COMPONENT_COUNT];
} hyperdos_color_graphics_adapter;

typedef struct hyperdos_8087
{
    long double registers[HYPERDOS_8087_REGISTER_COUNT];
    uint8_t     registerTags[HYPERDOS_8087_REGISTER_COUNT];
    uint16_t    controlWord;
    uint16_t    statusWord;
    uint16_t    tagWord;
    uint16_t    lastInstructionOffset;
    uint16_t    lastInstructionSegment;
    uint16_t    lastOperandOffset;
    uint16_t    lastOperandSegment;
    uint8_t     stackTop;
} hyperdos_8087;

void hyperdos_random_access_memory_initialize(hyperdos_random_access_memory* memory,
                                              uint8_t*                       bytes,
                                              size_t                         byteCount,
                                              uint32_t                       firstAddress);

void hyperdos_read_only_memory_initialize(hyperdos_read_only_memory* memory,
                                          const uint8_t*             bytes,
                                          size_t                     byteCount,
                                          uint32_t                   firstAddress);

uint8_t hyperdos_random_access_memory_read_byte(void* device, uint32_t physicalAddress);

void hyperdos_random_access_memory_write_byte(void* device, uint32_t physicalAddress, uint8_t value);

uint8_t hyperdos_read_only_memory_read_byte(void* device, uint32_t physicalAddress);

void hyperdos_read_only_memory_write_byte(void* device, uint32_t physicalAddress, uint8_t value);

void hyperdos_programmable_interrupt_controller_initialize(hyperdos_programmable_interrupt_controller* controller,
                                                           uint8_t                                     vectorBase);

uint8_t hyperdos_programmable_interrupt_controller_read_byte(void* device, uint16_t port);

void hyperdos_programmable_interrupt_controller_write_byte(void* device, uint16_t port, uint8_t value);

void hyperdos_programmable_interrupt_controller_raise_request(hyperdos_programmable_interrupt_controller* controller,
                                                              uint8_t                                     requestLine);

int hyperdos_programmable_interrupt_controller_acknowledge(hyperdos_programmable_interrupt_controller* controller,
                                                           uint8_t*                                    interruptNumber);

int hyperdos_programmable_interrupt_controller_acknowledge_request(
        hyperdos_programmable_interrupt_controller* controller,
        uint8_t*                                    requestLine,
        uint8_t*                                    interruptNumber);

int hyperdos_programmable_interrupt_controller_has_pending_unmasked_request(
        const hyperdos_programmable_interrupt_controller* controller);

void hyperdos_direct_memory_access_controller_initialize(hyperdos_direct_memory_access_controller* controller);

uint8_t hyperdos_direct_memory_access_controller_read_byte(void* device, uint16_t port);

void hyperdos_direct_memory_access_controller_write_byte(void* device, uint16_t port, uint8_t value);

void hyperdos_programmable_interval_timer_initialize(hyperdos_programmable_interval_timer* timer);

uint8_t hyperdos_programmable_interval_timer_read_byte(void* device, uint16_t port);

void hyperdos_programmable_interval_timer_write_byte(void* device, uint16_t port, uint8_t value);

void hyperdos_programmable_interval_timer_tick(void* device, uint64_t elapsedClockCount);

int hyperdos_programmable_interval_timer_get_and_clear_output_transition(hyperdos_programmable_interval_timer* timer,
                                                                         uint8_t channelIndex);

void hyperdos_programmable_peripheral_interface_initialize(hyperdos_programmable_peripheral_interface* interface);

uint8_t hyperdos_programmable_peripheral_interface_read_byte(void* device, uint16_t port);

void hyperdos_programmable_peripheral_interface_write_byte(void* device, uint16_t port, uint8_t value);

void hyperdos_intel_8042_keyboard_controller_initialize(hyperdos_intel_8042_keyboard_controller* controller);

uint8_t hyperdos_intel_8042_keyboard_controller_read_byte(void* device, uint16_t port);

void hyperdos_intel_8042_keyboard_controller_write_byte(void* device, uint16_t port, uint8_t value);

int hyperdos_intel_8042_keyboard_controller_receive_scan_code(hyperdos_intel_8042_keyboard_controller* controller,
                                                              uint8_t                                  scanCode);

int hyperdos_intel_8042_keyboard_controller_can_receive_scan_code(
        const hyperdos_intel_8042_keyboard_controller* controller);

int hyperdos_intel_8042_keyboard_controller_output_queue_is_full(
        const hyperdos_intel_8042_keyboard_controller* controller);

int hyperdos_intel_8042_keyboard_controller_has_interrupt_request(
        const hyperdos_intel_8042_keyboard_controller* controller);

int hyperdos_intel_8042_keyboard_controller_has_auxiliary_device_interrupt_request(
        const hyperdos_intel_8042_keyboard_controller* controller);

void hyperdos_intel_8042_keyboard_controller_clear_interrupt_request(
        hyperdos_intel_8042_keyboard_controller* controller);

void hyperdos_intel_8042_keyboard_controller_clear_auxiliary_device_interrupt_request(
        hyperdos_intel_8042_keyboard_controller* controller);

int hyperdos_intel_8042_keyboard_controller_receive_auxiliary_mouse_packet(
        hyperdos_intel_8042_keyboard_controller* controller,
        int16_t                                  horizontalMovement,
        int16_t                                  verticalMovement,
        uint8_t                                  buttonMask);

void hyperdos_intel_8042_keyboard_controller_set_auxiliary_mouse_reporting_enabled(
        hyperdos_intel_8042_keyboard_controller* controller,
        uint8_t                                  enabled);

void hyperdos_universal_asynchronous_receiver_transmitter_initialize(
        hyperdos_universal_asynchronous_receiver_transmitter* transmitter);

uint8_t hyperdos_universal_asynchronous_receiver_transmitter_read_byte(void* device, uint16_t port);

void hyperdos_universal_asynchronous_receiver_transmitter_write_byte(void* device, uint16_t port, uint8_t value);

void hyperdos_color_graphics_adapter_initialize(hyperdos_color_graphics_adapter* adapter);

uint8_t hyperdos_color_graphics_adapter_read_memory_byte(void* device, uint32_t physicalAddress);

void hyperdos_color_graphics_adapter_write_memory_byte(void* device, uint32_t physicalAddress, uint8_t value);

void hyperdos_color_graphics_adapter_set_bios_video_mode(hyperdos_color_graphics_adapter* adapter, uint8_t videoMode);

int hyperdos_color_graphics_adapter_graphics_mode_is_enabled(const hyperdos_color_graphics_adapter* adapter);

uint16_t hyperdos_color_graphics_adapter_get_display_start_address(const hyperdos_color_graphics_adapter* adapter);

uint16_t hyperdos_color_graphics_adapter_get_display_stride_byte_count(const hyperdos_color_graphics_adapter* adapter);

uint8_t hyperdos_color_graphics_adapter_read_input_output_byte(void* device, uint16_t port);

void hyperdos_color_graphics_adapter_write_input_output_byte(void* device, uint16_t port, uint8_t value);

const uint8_t* hyperdos_color_graphics_adapter_get_text_memory(const hyperdos_color_graphics_adapter* adapter);

void hyperdos_8087_initialize(hyperdos_8087* coprocessor);

hyperdos_x86_execution_result hyperdos_8087_wait(hyperdos_x86_processor* processor, void* userContext);

hyperdos_x86_execution_result hyperdos_8087_escape(hyperdos_x86_processor*                     processor,
                                                   const hyperdos_x86_coprocessor_instruction* instruction,
                                                   void*                                       userContext);

#endif
