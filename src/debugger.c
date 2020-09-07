/*
debugger.c
Debugging interface for the emulator

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

#include "gb/cpu.h"
#include "gb/memory.h"
#include "gb/cartridge.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

struct breakpoint {
    bool pc_break;
    uint16_t pc;
    struct breakpoint *next;
};

static struct params {
    int last_ret;
    bool emulate_cycles;
    struct breakpoint *bps;
} params;

static inline bool empty_line(const char *line)
{
    while (*line) {
        if (*line != ' ' && *line != '\t')
            return false;
    }
    return true;
}

void emulate_n_cycles(gb_system_t *gb, uint32_t n)
{
    while (n > 0 && (params.last_ret = gb_cpu_cycle(gb, params.emulate_cycles)) >= 0) {
        --n;
    }
}

void emulate_until_breakpoint(gb_system_t *gb)
{
    struct breakpoint *bp;
    bool run = true;

    while (run && (params.last_ret = gb_cpu_cycle(gb, params.emulate_cycles)) >= 0) {
        for (bp = params.bps; bp; bp = bp->next) {
            if (bp->pc_break && bp->pc == gb->pc) {
                run = false;
                printf("Breakpoint: PC at 0x%04X\n", bp->pc);
            }
        }
    }
}

void bp_add_pc(uint16_t pc)
{
    struct breakpoint *bp = malloc(sizeof(struct breakpoint));

    if (!bp) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    memset(bp, 0, sizeof(struct breakpoint));
    bp->pc = pc;
    bp->pc_break = true;

    if (params.bps) {
        struct breakpoint *last = params.bps;

        while (last->next) {
            if (last->pc_break && last->pc == pc) {
                free(bp);
                fprintf(stderr, "A breakpoint for PC=0x%04X already exists\n", pc);
                return;
            }
            last = last->next;
        }
        if (last->pc_break && last->pc == pc) {
            free(bp);
            fprintf(stderr, "A breakpoint for PC=0x%04X already exists\n", pc);
            return;
        }

        last->next = bp;
    } else {
        params.bps = bp;
    }
    printf("Added breakpoint for PC=0x%04X\n", pc);
}

void bp_remove_pc(uint16_t pc)
{
    struct breakpoint *prev = NULL;
    struct breakpoint *bp;

    for (bp = params.bps; bp; bp = bp->next) {
        if (bp->pc_break && bp->pc == pc) {
            if (prev) {
                prev->next = bp->next;
                free(bp);
            } else {
                params.bps = bp->next;
                free(bp);
            }
            printf("Removed breakpoint for PC=0x%04X\n", pc);
            return;
        }

        prev = bp;
    }

    fprintf(stderr, "No breakpoint for PC=0x%04X\n", pc);
}

void bp_freeall(void)
{
    struct breakpoint *bp = params.bps;
    struct breakpoint *next;

    while (bp) {
        next = bp->next;
        free(bp);
        bp = next;
    }

    params.bps = NULL;
}

void debug_gb_system(gb_system_t *gb)
{
    const char tokdelim[] = " ";
    char prompt[128];
    char *line = NULL, *tok;

    // Default values
    params.last_ret = 0;
    params.emulate_cycles = false;
    params.bps = NULL;

    stifle_history(50);
    while (1) {
        // Get user input
        snprintf(
            prompt,
            sizeof(prompt),
            "Cycle %lu - PC 0x%04X $ ",
            gb->cycle_nb,
            gb->pc);

        if (line) {
            free(line);
            line = NULL;
        }
        if (!(line = readline(prompt))) {
            printf("\n");
            break;
        }

        // Skip empty lines
        if (empty_line(line))
            continue;

        add_history(line);
        tok = strtok(line, tokdelim);

        if (!strcmp(tok, "s") || !strcmp(tok, "step")) {
            // step
            // Emulate one or n CPU cycles
            if ((tok = strtok(NULL, tokdelim))) {
                emulate_n_cycles(gb, (uint32_t) atoi(tok));
            } else {
                emulate_n_cycles(gb, 1);
            }

        } else if (!strcmp(tok, "sd") || !strcmp(tok, "stepd")) {
            // stepd (step dump)
            // Emulate one CPU cycle and dump CPU
            emulate_n_cycles(gb, 1);
            dump_cpu(gb);

        } else if (!strcmp(tok, "bp") || !strcmp(tok, "breakpoint")) {
            // Add/remove breakpoints
            if (!(tok = strtok(NULL, tokdelim))) {
                printf("Breakpoints:\n");
                for (struct breakpoint *bp = params.bps; bp; bp = bp->next) {
                    if (bp->pc_break) {
                        printf("PC=0x%04X\n", bp->pc);
                    } else {
                        printf("Unexpected empty breakpoint\n");
                    }
                }
                continue;
            }

            if (!strcmp(tok, "add")) {
                bp_add_pc((uint16_t) strtol(strtok(NULL, tokdelim), NULL, 16));
            } else if (!strcmp(tok, "remove")) {
                bp_remove_pc((uint16_t) strtol(strtok(NULL, tokdelim), NULL, 16));
            }

        } else if (!strcmp(tok, "run")) {
            emulate_until_breakpoint(gb);

        } else if (!strcmp(tok, "cpu")) {
            // cpu
            // Dump CPU
            dump_cpu(gb);

        } else if (!strcmp(tok, "mem")) {
            // mem
            // Dump n bytes of memory (512 by default) starting at address (PC by default)
            uint16_t address = gb->pc;
            uint16_t bytes = 512;

            if ((tok = strtok(NULL, tokdelim)))
                address = (uint16_t) strtol(tok, NULL, 16);
            if ((tok = strtok(NULL, tokdelim)))
                bytes = (uint16_t) atoi(tok);

            dump_memory(gb, address, bytes, true);

        } else if (!strcmp(tok, "emulate-cycles")) {
            // emulate-cycles
            // Toggle real cycle emulation ON/OFF
            params.emulate_cycles = !params.emulate_cycles;
            printf("Real cycle emulation: %s\n",
                params.emulate_cycles ? "enabled" : "disabled");

        } else if (!strcmp(tok, "quit") || !strcmp(tok, "q") || !strcmp(tok, "exit")) {
            break;

        } else {
            printf("Unknown command: %s\n", tok);
        }
    }

    free(line);
    bp_freeall();
    printf("Exitting debugger\n");
}