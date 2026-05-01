#ifndef HYPERDOS_PC_MODEL_H
#define HYPERDOS_PC_MODEL_H

#include <stdint.h>

typedef enum hyperdos_pc_model
{
    HYPERDOS_PC_MODEL_XT = 0,
    HYPERDOS_PC_MODEL_AT = 1
} hyperdos_pc_model;

uint8_t hyperdos_pc_model_get_system_bios_model_identifier(hyperdos_pc_model pcModel);

#endif
