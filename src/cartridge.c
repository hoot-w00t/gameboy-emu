/*
cartridge.c
Decode cartridge header and verify checksums

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

#include "logger.h"
#include "cartridge.h"
#include <stdio.h>
#include <string.h>

const byte_t nintendo_logo[48] = {
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
    0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
    0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
    0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
};

struct licensee_publishers {
    char *code;
    char *publisher;
};
const struct licensee_publishers licensee_publishers[] = {
    { "00", "none" },
    { "01", "Nintendo" },
    { "08", "Capcom" },
    { "13", "Electronic" },
    { "18", "Hudson" },
    { "19", "b-ai" },
    { "20", "kss" },
    { "22", "pow" },
    { "24", "PCM" },
    { "25", "san-x" },
    { "28", "Kemco" },
    { "29", "seta" },
    { "30", "Viacom" },
    { "31", "Nintendo" },
    { "32", "Bandai" },
    { "33", "Ocean/Acclaim" },
    { "34", "Konami" },
    { "35", "Hector" },
    { "37", "Taito" },
    { "38", "Hudson" },
    { "39", "Banpresto" },
    { "41", "Ubi" },
    { "42", "Atlus" },
    { "44", "Malibu" },
    { "46", "angel" },
    { "47", "Bullet-Proof" },
    { "49", "irem" },
    { "50", "Absolute" },
    { "51", "Acclaim" },
    { "52", "Activision" },
    { "53", "American" },
    { "54", "Konami" },
    { "55", "Hi" },
    { "56", "LJN" },
    { "57", "Matchbox" },
    { "58", "Mattel" },
    { "59", "Milton" },
    { "60", "Titus" },
    { "61", "Virgin" },
    { "64", "LucasArts" },
    { "67", "Ocean" },
    { "69", "Electronic" },
    { "70", "Infogrames" },
    { "71", "Interplay" },
    { "72", "Broderbund" },
    { "73", "sculptured" },
    { "75", "sci" },
    { "78", "THQ" },
    { "79", "Accolade" },
    { "80", "misawa" },
    { "83", "lozc" },
    { "86", "Tokuma" },
    { "87", "Tsukuda" },
    { "91", "Chunsoft" },
    { "92", "Video" },
    { "93", "Ocean/Acclaim" },
    { "95", "Varie" },
    { "96", "Yonezawa/s'pal" },
    { "97", "Kaneko" },
    { "99", "Pack" },
    { "A4", "Konami" },
    { NULL, NULL }
};

// Return the cartridge publisher (only for the new licensee code)
char *cartridge_publisher(cartridge_hdr_t *cr)
{
    for (int i = 0; licensee_publishers[i].code; ++i) {
        if (cr->licensee_code[0] == licensee_publishers[i].code[0] &&
            cr->licensee_code[1] == licensee_publishers[i].code[1]) {

            return licensee_publishers[i].publisher;
        }
    }
    return "unknown";
}

// Return the cartridge MBC type details
char *cartridge_mbc_type(cartridge_hdr_t *cr)
{
    switch (cr->mbc_type) {
        case 0x00: return "Rom Only";
        case 0x01: return "MBC1";
        case 0x02: return "MBC1 (+RAM)";
        case 0x03: return "MBC1 (+RAM +Battery)";
        case 0x05: return "MBC2";
        case 0x06: return "MBC2 (+Battery)";
        case 0x08: return "Rom (+RAM)";
        case 0x09: return "Rom (+RAM +Battery)";
        case 0x0B: return "MMM01";
        case 0x0C: return "MMM01 (+RAM)";
        case 0x0D: return "MMM01 (+RAM +Battery)";
        case 0x0F: return "MBC3 (+Timer +Battery)";
        case 0x10: return "MBC3 (+Timer +RAM +Battery)";
        case 0x11: return "MBC3";
        case 0x12: return "MBC3 (+RAM)";
        case 0x13: return "MBC3 (+RAM +Battery)";
        case 0x19: return "MBC5";
        case 0x1A: return "MBC5 (+RAM)";
        case 0x1B: return "MBC5 (+RAM +Battery)";
        case 0x1C: return "MBC5 (+Rumble)";
        case 0x1D: return "MBC5 (+Rumble +RAM)";
        case 0x1E: return "MBC5 (+Rumble +RAM +Battery)";
        case 0x20: return "MBC6";
        case 0x22: return "MBC7 (+Sensor +Rumble +RAM +Battery)";
        case 0xFC: return "Pocket Camera";
        case 0xFD: return "BANDAI TAMA5";
        case 0xFE: return "HuC3";
        case 0xFF: return "HuC1 (+RAM, +Battery)";
        default: return "Unknown";
    }
}

// Decode the cartridge header starting at *data into *cr
// Returns false if there was an error
bool cartridge_decode_hdr(byte_t *data, cartridge_hdr_t *cr)
{
    // Color GameBoy is not supported
    if (data[CR_CGB_FLAG_ADDR] == 0x80) {
        logger(LOG_WARN, "Color GameBoy functions are not supported");
    } else if (data[CR_CGB_FLAG_ADDR] == 0xC0) {
        logger(LOG_ERROR, "Color GameBoy is not supported");
        return false;
    }

    // Read Nintendo Logo bitmap
    for (byte_t i = 0; i < sizeof(cr->logo); ++i) {
        cr->logo[i] = data[CR_LOGO_ADDR + i];
    }

    // Read game title
    memset(cr->title, 0, sizeof(cr->title));
    for (byte_t i = 0; i < 16; ++i) {
        cr->title[i] = data[CR_TITLE_ADDR + i];
    }

    // Decode licensee code
    if (data[CR_OLD_LICENSEE_CODE_ADDR] == 0x33) {
        // The cartridge is using the new licensee code
        cr->licensee_code[0] = data[CR_LICENSEE_CODE_ADDR];
        cr->licensee_code[1] = data[CR_LICENSEE_CODE_ADDR + 1];
        cr->old_licensee_code = false;
    } else {
        // The cartridge is using the old licensee code
        cr->licensee_code[0] = data[CR_OLD_LICENSEE_CODE_ADDR];
        cr->old_licensee_code = true;
    }

    // Read cartridge MBC type
    cr->mbc_type = data[CR_MBC_TYPE_ADDR];

    // Decode ROM size
    switch (data[CR_ROM_SIZE_ADDR]) {
        case 0x00: cr->rom_banks = 2;   break;
        case 0x01: cr->rom_banks = 4;   break;
        case 0x02: cr->rom_banks = 8;   break;
        case 0x03: cr->rom_banks = 16;  break;
        case 0x04: cr->rom_banks = 32;  break;
        case 0x05: cr->rom_banks = 64;  break;
        case 0x06: cr->rom_banks = 128; break;
        case 0x07: cr->rom_banks = 256; break;
        case 0x08: cr->rom_banks = 512; break;
        case 0x52: cr->rom_banks = 72;  break;
        case 0x53: cr->rom_banks = 80;  break;
        case 0x54: cr->rom_banks = 96;  break;
        default:
            logger(LOG_ERROR,
                   "Invalid ROM Size: $%02X",
                   data[CR_ROM_SIZE_ADDR]);
            return false;
    }

    // Decode RAM size
    switch (data[CR_RAM_SIZE_ADDR]) {
        case 0x00:
            cr->ram_banks = 0;
            break;
        case 0x01:
            cr->ram_size = 2048;
            cr->ram_banks = 1;
            break;
        case 0x02:
            cr->ram_size = RAM_BANK_SIZE;
            cr->ram_banks = 1;
            break;
        case 0x03:
            cr->ram_size = RAM_BANK_SIZE;
            cr->ram_banks = 4;
            break;
        case 0x04:
            cr->ram_size = RAM_BANK_SIZE;
            cr->ram_banks = 16;
            break;
        case 0x05:
            cr->ram_size = RAM_BANK_SIZE;
            cr->ram_banks = 8;
            break;
        default:
            logger(LOG_CRIT,
                   "Invalid RAM size: $%02X",
                   data[CR_RAM_SIZE_ADDR]);
            return false;
    }

    // Read destination code
    cr->destination_code = data[CR_DEST_CODE_ADDR];

    // Read ROM version number
    cr->rom_version = data[CR_ROM_VERSION_ADDR];

    // Read header checksum
    cr->header_checksum = data[CR_HEADER_CHECKSUM_ADDR];

    // Read global checksum
    cr->global_checksum = (data[CR_GLOBAL_CHECKSUM_ADDR] << 8) |
                           data[CR_GLOBAL_CHECKSUM_ADDR + 1];

    return true;
}

// Calculate the header checksum
byte_t compute_header_checksum(byte_t *data)
{
    byte_t x = 0;

    for (uint16_t i = 0x134; i <= 0x14C; ++i) {
        x = x - data[i] - 1;
    }

    return x;
}

// Return true if Nintendo Logo bitmap is valid
bool cartridge_check_logo(cartridge_hdr_t *cr)
{
    for (byte_t i = 0; i < sizeof(nintendo_logo); ++i) {
        if (cr->logo[i] != nintendo_logo[i]) {
            return false;
        }
    }

    return true;
}

// Dump cartridge header to stdout
void cartridge_dump(cartridge_hdr_t *cr)
{
    printf("Cartridge Header\n");
    printf("Nintendo Logo bitmap (%s):\n",
           cartridge_check_logo(cr) ? "valid" : "invalid");
    for (byte_t i = 0; i < sizeof(cr->logo); i += 16) {
        printf("    %02X %02X %02X %02X %02X %02X %02X %02X   %02X %02X %02X %02X %02X %02X %02X %02X\n",
               cr->logo[i], cr->logo[i + 1], cr->logo[i + 2], cr->logo[i + 3],
               cr->logo[i + 4], cr->logo[i + 5], cr->logo[i + 6], cr->logo[i + 7],
               cr->logo[i + 8], cr->logo[i + 9], cr->logo[i + 10], cr->logo[i + 11],
               cr->logo[i + 12], cr->logo[i + 13], cr->logo[i + 14], cr->logo[i + 15]);
    }

    printf("Title: %s\n", cr->title);

    if (cr->old_licensee_code) {
        printf("Licensee code (old): $%02X\n", cr->licensee_code[0]);
    } else {
        printf("Licensee code: %c%c\n",
               cr->licensee_code[0],
               cr->licensee_code[1]);
        printf("Publisher: %s\n", cartridge_publisher(cr));
    }

    printf("MBC Type: %s ($%02X)\n", cartridge_mbc_type(cr), cr->mbc_type);
    printf("ROM banks: %u\n", cr->rom_banks);
    printf("RAM: Bank Size: %u, Banks: %u\n",
           cr->ram_size,
           cr->ram_banks);

    printf("ROM Version: $%02X\n", cr->rom_version);
    printf("Destination code: $%02X (%s)\n",
           cr->destination_code,
           cr->destination_code ? "Non-Japanese" : "Japanese");

    printf("Header Checksum: $%02X\n", cr->header_checksum);
    printf("Global Checksum: $%04X\n", cr->global_checksum);
}