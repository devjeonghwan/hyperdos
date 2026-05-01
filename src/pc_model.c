#include "hyperdos/pc_model.h"

#include "hyperdos/pc_system_bios.h"

uint8_t hyperdos_pc_model_get_system_bios_model_identifier(hyperdos_pc_model pcModel)
{
    if (pcModel == HYPERDOS_PC_MODEL_AT)
    {
        return HYPERDOS_PC_SYSTEM_BIOS_MODEL_IDENTIFIER_AT;
    }
    return HYPERDOS_PC_SYSTEM_BIOS_MODEL_IDENTIFIER_XT;
}
