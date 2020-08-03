/*
nop.c
NOP opcode

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

#include "gb/defs.h"
#include "gb/opcodes.h"
#include "gb/memory.h"
#include "gb/registers.h"
#include "logger.h"

// Opcode table
const gb_opcode_t gb_opcode_table[] = {
    {
        .mnemonic     = "NOP",
        .opcode       = 0x00,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "No OPeration",
        .handler      = &gb_opcode_nop
    },
    {
        .mnemonic     = "LD BC,d16",
        .opcode       = 0x01,
        .length       = 3,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load 16 bit value into register BC",
        .handler      = &gb_opcode_ld_rr_d16
    },
    {
        .mnemonic     = "STOP",
        .opcode       = 0x10,
        .length       = 2,
        .cycles_true  = 0,
        .cycles_false = 0,
        .comment      = "Low Power Standby Mode",
        .handler      = &gb_opcode_stop
    },
    {
        .mnemonic     = "LD DE,d16",
        .opcode       = 0x11,
        .length       = 3,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load 16 bit value into register DE",
        .handler      = &gb_opcode_ld_rr_d16
    },
    {
        .mnemonic     = "JR r8",
        .opcode       = 0x18,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Relative jump",
        .handler      = &gb_opcode_jr
    },
    {
        .mnemonic     = "JR NZ,r8",
        .opcode       = 0x20,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 8,
        .comment      = "Relative jump if Zero Flag is clear",
        .handler      = &gb_opcode_jr
    },
    {
        .mnemonic     = "LD HL,d16",
        .opcode       = 0x21,
        .length       = 3,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load 16 bit value into register HL",
        .handler      = &gb_opcode_ld_rr_d16
    },
    {
        .mnemonic     = "JR Z,r8",
        .opcode       = 0x28,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 8,
        .comment      = "Relative jump if Zero Flag is set",
        .handler      = &gb_opcode_jr
    },
    {
        .mnemonic     = "JR NC,r8",
        .opcode       = 0x30,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 8,
        .comment      = "Relative jump if Carry Flag is clear",
        .handler      = &gb_opcode_jr
    },
    {
        .mnemonic     = "LD SP,d16",
        .opcode       = 0x31,
        .length       = 3,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load 16 bit value into SP",
        .handler      = &gb_opcode_ld_rr_d16
    },
    {
        .mnemonic     = "JR C,r8",
        .opcode       = 0x38,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 8,
        .comment      = "Relative jump if Carry Flag is set",
        .handler      = &gb_opcode_jr
    },
    {
        .mnemonic     = "XOR B",
        .opcode       = 0xA8,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Exclusive OR between registers A and B",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "XOR C",
        .opcode       = 0xA9,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Exclusive OR between registers A and C",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "XOR D",
        .opcode       = 0xAA,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Exclusive OR between registers A and D",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "XOR E",
        .opcode       = 0xAB,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Exclusive OR between registers A and E",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "XOR H",
        .opcode       = 0xAC,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Exclusive OR between registers A and H",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "XOR L",
        .opcode       = 0xAD,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Exclusive OR between registers A and L",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "XOR (HL)",
        .opcode       = 0xAE,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Exclusive OR between register A and value at address stored in HL",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "XOR A",
        .opcode       = 0xAF,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Exclusive OR between registers A and A",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "RET NZ",
        .opcode       = 0xC0,
        .length       = 1,
        .cycles_true  = 20,
        .cycles_false = 8,
        .comment      = "Return if Zero Flag is clear",
        .handler      = &gb_opcode_ret
    },
    {
        .mnemonic     = "JP NZ,a16",
        .opcode       = 0xC2,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 12,
        .comment      = "Jump to 16 bit address if Zero Flag is clear",
        .handler      = &gb_opcode_jp
    },
    {
        .mnemonic     = "JP a16",
        .opcode       = 0xC3,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Jump to 16 bit address",
        .handler      = &gb_opcode_jp
    },
    {
        .mnemonic     = "CALL NZ,a16",
        .opcode       = 0xC4,
        .length       = 3,
        .cycles_true  = 24,
        .cycles_false = 12,
        .comment      = "Call 16 bit address if Zero Flag is clear",
        .handler      = &gb_opcode_call
    },
    {
        .mnemonic     = "RET Z",
        .opcode       = 0xC8,
        .length       = 1,
        .cycles_true  = 20,
        .cycles_false = 8,
        .comment      = "Return if Zero Flag is set",
        .handler      = &gb_opcode_ret
    },
    {
        .mnemonic     = "RET",
        .opcode       = 0xC9,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Return",
        .handler      = &gb_opcode_ret
    },
    {
        .mnemonic     = "JP Z,a16",
        .opcode       = 0xCA,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 12,
        .comment      = "Jump to 16 bit address if Zero Flag is set",
        .handler      = &gb_opcode_jp
    },
    {
        .mnemonic     = "CALL Z,a16",
        .opcode       = 0xCC,
        .length       = 3,
        .cycles_true  = 24,
        .cycles_false = 12,
        .comment      = "Call 16 bit address if Zero Flag is set",
        .handler      = &gb_opcode_call
    },
    {
        .mnemonic     = "CALL a16",
        .opcode       = 0xCD,
        .length       = 3,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Call 16 bit address",
        .handler      = &gb_opcode_call
    },
    {
        .mnemonic     = "RET NC",
        .opcode       = 0xD0,
        .length       = 1,
        .cycles_true  = 20,
        .cycles_false = 8,
        .comment      = "Return if Carry Flag is clear",
        .handler      = &gb_opcode_ret
    },
    {
        .mnemonic     = "JP NC,a16",
        .opcode       = 0xD2,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 12,
        .comment      = "Jump to 16 bit address if Carry Flag is clear",
        .handler      = &gb_opcode_jp
    },
    {
        .mnemonic     = "CALL NC,a16",
        .opcode       = 0xD4,
        .length       = 3,
        .cycles_true  = 24,
        .cycles_false = 12,
        .comment      = "Call 16 bit address if Carry Flag is clear",
        .handler      = &gb_opcode_call
    },
    {
        .mnemonic     = "RET C",
        .opcode       = 0xD8,
        .length       = 1,
        .cycles_true  = 20,
        .cycles_false = 8,
        .comment      = "Return if Carry Flag is set",
        .handler      = &gb_opcode_ret
    },
    {
        .mnemonic     = "RETI",
        .opcode       = 0xD9,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Return and enable interrupts",
        .handler      = &gb_opcode_ret
    },
    {
        .mnemonic     = "JP C,a16",
        .opcode       = 0xDA,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 12,
        .comment      = "Jump to 16 bit address if Carry Flag is set",
        .handler      = &gb_opcode_jp
    },
    {
        .mnemonic     = "CALL C,a16",
        .opcode       = 0xDC,
        .length       = 3,
        .cycles_true  = 24,
        .cycles_false = 12,
        .comment      = "Call 16 bit address if Carry Flag is set",
        .handler      = &gb_opcode_call
    },
    {
        .mnemonic     = "JP (HL)",
        .opcode       = 0xE9,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Jump to address stored in HL register",
        .handler      = &gb_opcode_jp
    },
    {
        .mnemonic     = "XOR d8",
        .opcode       = 0xEE,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Exclusive OR between register A and d8",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "DI",
        .opcode       = 0xF3,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Disable Interrupts",
        .handler      = &gb_opcode_di
    },
    {
        .mnemonic     = "LD HL,SP+r8",
        .opcode       = 0xF8,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load 16 bit register HL into SP",
        .handler      = &gb_opcode_ld_hl_sp_r8
    },
    {
        .mnemonic     = "LD SP,HL",
        .opcode       = 0xF9,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load 16 bit register HL into SP",
        .handler      = &gb_opcode_ld_sp_hl
    },
    {
        .mnemonic     = "EI",
        .opcode       = 0xFB,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Enable Interrupts",
        .handler      = &gb_opcode_ei
    },
    {NULL, 0, 0, 0, 0, NULL, NULL}
};

// Push address to stack
void gb_cpu_push_stack(const uint16_t address, gb_system_t *gb)
{
    gb_write_byte(gb->sp, (address >> 8) & 0xff, false, gb);
    gb_write_byte(gb->sp - 1, address & 0xff, false, gb);

    gb->sp -= 2;
}

// Pop address from stack
uint16_t gb_cpu_pop_stack(gb_system_t *gb)
{
    uint16_t address = gb_read_uint16(gb->sp + 1, gb);

    gb->sp += 2;
    return address;
}

// Call address
void gb_cpu_call(const uint16_t address, gb_system_t *gb)
{
    gb_cpu_push_stack(gb->pc, gb);

    gb->pc = address;
}

// Return
void gb_cpu_ret(gb_system_t *gb)
{
    gb->pc = gb_cpu_pop_stack(gb);
}

// Emulate a GameBoy CPU cycle
int gb_cpu_cycle(gb_system_t *gb)
{
    if (gb->idle_cycles > 0) {
        gb->idle_cycles -= 1;
        return 0;
    }

    byte_t opcode_value = gb_read_byte(gb->pc, gb);
    const gb_opcode_t *opcode = NULL;
    int handler_ret;

    for (uint16_t i = 0; gb_opcode_table[i].handler; ++i) {
        if (gb_opcode_table[i].opcode == opcode_value) {
            opcode = &gb_opcode_table[i];
            break;
        }
    }

    if (opcode) {
        logger(
            LOG_DEBUG,
            "PC: 0x%04X   Opcode: 0x%02X: %s",
            gb->pc,
            opcode_value,
            opcode->mnemonic
        );

        handler_ret = (*opcode->handler)(opcode, gb);
    } else {
        handler_ret = OPCODE_ILLEGAL;
    }

    if (handler_ret == OPCODE_ILLEGAL) {
        logger(
            LOG_CRIT,
            "PC: 0x%04X   Opcode: 0x%02X: Illegal opcode",
            gb->pc,
            opcode_value
        );
        return -1;
    }

    if (handler_ret & OPCODE_ACTION) {
        gb->idle_cycles += opcode->cycles_true - 1;
    } else if (handler_ret & OPCODE_NOACTION) {
        gb->idle_cycles += opcode->cycles_false - 1;
    }

    if (!(handler_ret & OPCODE_NOPC)) {
        gb->pc += opcode->length;
    }

    if (handler_ret & OPCODE_EXIT) {
        return -2;
    }

    return 0;
}