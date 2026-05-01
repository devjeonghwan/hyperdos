#ifndef HYPERDOS_PC_TEXT_H
#define HYPERDOS_PC_TEXT_H

#include <stdint.h>

uint16_t hyperdos_pc_text_code_page_437_unicode_character(uint8_t characterByte);

int hyperdos_pc_text_korean_code_page_949_is_lead_byte(uint8_t characterByte);

int hyperdos_pc_text_korean_code_page_949_is_trail_byte(uint8_t characterByte);

#endif
