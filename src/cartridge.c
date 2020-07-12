/*
cartridge.c
Decode cartridge header information

Copyright (C) 2020 akrocynova

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "gb_defs.h"
#include "logger.h"
#include <string.h>

const byte_t nintendo_logo[48] = {
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
    0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
    0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
    0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
};

// Try to decode the cartridge header starting at *data and place those
// in *cartridge
void decode_cartridge_header(byte_t *data, gb_cartridge_hdr_t *cartridge)
{
    // Read Nintendo Logo bitmap
    for (byte_t i = 0; i < sizeof(cartridge->logo); ++i) {
        cartridge->logo[i] = data[GB_CR_LOGO_ADDR + i];
    }

    // Read game title
    memset(cartridge->title, 0, sizeof(cartridge->title));
    for (byte_t i = 0; i < 16; ++i) {
        cartridge->title[i] = data[GB_CR_TITLE_ADDR + i];
    }

    // Decode licensee code
    if (data[GB_CR_OLD_LICENSEE_CODE_ADDR] == 0x33) {
        // The cartridge is using the new licensee code
        cartridge->licensee_code[0] = data[GB_CR_LICENSEE_CODE_ADDR];
        cartridge->licensee_code[1] = data[GB_CR_LICENSEE_CODE_ADDR + 1];
        cartridge->old_licensee_code = false;
    } else {
        // The cartridge is using the old licensee code
        cartridge->licensee_code[0] = data[GB_CR_OLD_LICENSEE_CODE_ADDR];
        cartridge->old_licensee_code = true;
    }

    // Read cartridge MBC type
    cartridge->mbc_type = data[GB_CR_MBC_TYPE_ADDR];

    // Decode ROM size
    switch (data[GB_CR_ROM_SIZE_ADDR]) {
        case 0x00:
            cartridge->rom_banks = 1;
            break;
        case 0x01:
            cartridge->rom_banks = 4;
            break;
        case 0x02:
            cartridge->rom_banks = 8;
            break;
        case 0x03:
            cartridge->rom_banks = 16;
            break;
        case 0x04:
            cartridge->rom_banks = 32;
            break;
        case 0x05:
            cartridge->rom_banks = 64;
            break;
        case 0x06:
            cartridge->rom_banks = 128;
            break;
        case 0x07:
            cartridge->rom_banks = 256;
            break;
        case 0x52:
            cartridge->rom_banks = 72;
            break;
        case 0x53:
            cartridge->rom_banks = 80;
            break;
        case 0x54:
            cartridge->rom_banks = 96;
            break;
        default:
            logger(LOG_CRIT, "Invalid ROM Size in cartridge header: 0x%02X", data[GB_CR_ROM_SIZE_ADDR]);
    }

    // Decode RAM size
    switch (data[GB_CR_RAM_SIZE_ADDR]) {
        case 0x00:
            cartridge->ram_banks = 0;
            break;
        case 0x01:
            cartridge->ram_size = 2048;
            cartridge->ram_banks = 1;
            break;
        case 0x02:
            cartridge->ram_size = GB_RAM_BANK_SIZE;
            cartridge->ram_banks = 1;
            break;
        case 0x03:
            cartridge->ram_size = GB_RAM_BANK_SIZE;
            cartridge->ram_banks = 4;
            break;
        default:
            logger(LOG_CRIT, "Invalid RAM size in cartridge header: 0x%02X", data[GB_CR_RAM_SIZE_ADDR]);
    }

    // Read destination code
    cartridge->destination_code = data[GB_CR_DEST_CODE_ADDR];

    // Read ROM version number
    cartridge->rom_version = data[GB_CR_ROM_VERSION_ADDR];

    // Read header checksum
    cartridge->header_checksum = data[GB_CR_HEADER_CHECKSUM_ADDR];

    // Read global checksum
    cartridge->global_checksum = (data[GB_CR_GLOBAL_CHECKSUM_ADDR] << 8) | data[GB_CR_GLOBAL_CHECKSUM_ADDR + 1];
}

// Calculate the header checksum
byte_t compute_header_checksum(byte_t *data)
{
    byte_t x = 0;

    for (uint16_t i = 0x0134; i <= 0x014C; ++i) {
        x = x - data[i] - 1;
    }

    return x;
}

// Calculate the global checksum
uint16_t compute_global_checksum(byte_t *data)
{
    uint16_t x = 0;

    for (uint16_t i = CARTRIDGE_HEADER_LADDR; i < GB_CR_GLOBAL_CHECKSUM_ADDR; ++i) {
        x += data[i];
    }

    return x;
}

// Return true if Nintendo Logo bitmap is valid
bool valid_nintendo_logo(gb_cartridge_hdr_t *cartridge)
{
    for (byte_t i = 0; i < sizeof(nintendo_logo); ++i) {
        if (cartridge->logo[i] != nintendo_logo[i]) {
            return false;
        }
    }

    return true;
}

// Dump cartridge header to LOG_DEBUG
void dump_cartridge_header(gb_cartridge_hdr_t *cartridge)
{
    logger(LOG_DEBUG, "Cartridge Header");
    logger(LOG_DEBUG, "Nintendo Logo bitmap:");
    for (byte_t i = 0; i < sizeof(cartridge->logo); i += 16) {
        logger(
            LOG_DEBUG,
            "    %02X %02X %02X %02X %02X %02X %02X %02X   %02X %02X %02X %02X %02X %02X %02X %02X",
            cartridge->logo[i], cartridge->logo[i + 1], cartridge->logo[i + 2], cartridge->logo[i + 3],
            cartridge->logo[i + 4], cartridge->logo[i + 5], cartridge->logo[i + 6], cartridge->logo[i + 7],
            cartridge->logo[i + 8], cartridge->logo[i + 9], cartridge->logo[i + 10], cartridge->logo[i + 11],
            cartridge->logo[i + 12], cartridge->logo[i + 13], cartridge->logo[i + 14], cartridge->logo[i + 15]
        );
    }
    logger(LOG_DEBUG, "Title: %s", cartridge->title);
    if (cartridge->old_licensee_code) {
        logger(LOG_DEBUG, "Licensee code: 0x%02X", cartridge->licensee_code[0]);
    } else {
        logger(
            LOG_DEBUG,
            "Licensee code: %c%c",
            cartridge->licensee_code[0],
            cartridge->licensee_code[1]
        );
    }
    logger(LOG_DEBUG, "MBC Type: 0x%02X", cartridge->mbc_type);
    logger(LOG_DEBUG, "ROM banks: %u", cartridge->rom_banks);
    logger(
        LOG_DEBUG,
        "RAM: Bank Size: %u, Banks: %u",
        cartridge->ram_size,
        cartridge->ram_banks
    );
    logger(LOG_DEBUG, "ROM Version: 0x%02X", cartridge->rom_version);
    logger(LOG_DEBUG, "Destination code: 0x%02X", cartridge->destination_code);
    logger(LOG_DEBUG, "Header Checksum: 0x%02X", cartridge->header_checksum);
    logger(LOG_DEBUG, "Global Checksum: 0x%04X", cartridge->global_checksum);
}