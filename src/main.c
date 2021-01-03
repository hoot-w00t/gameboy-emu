/*
main.c
Main function, command-line argument parsing

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
#include "gb_system.h"
#include "emulator.h"
#include "cartridge.h"
#include "emulator_utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static struct args {
    bool debug;
    bool no_audio;
    char *filename;
    bool filename_alloc;
    bool enable_bootrom;
} args;

void print_usage(const char *cmd)
{
    printf("Usage: %s [-d] [-l level] filename\n", cmd);
}

void print_help(const char *cmd)
{
    print_usage(cmd);
    printf("\nDescription:\n");
    printf("    filename        GameBoy ROM to emulate\n\n");
    printf("    -h              Show this help message\n");
    printf("    -l level        Set logging to level (default: warn)\n");
    printf("                    Options: crit, error, warn, info, debug, all\n");
    printf("    -b              Enable DMG bootrom\n");
    printf("    -d              Run in debugging mode\n");
    printf("    -n              Disable audio\n");
}

void parse_args(int ac, char **av)
{
    const char shortopts[] = "hl:bdn";
    int opt;

    // Default values
    args.debug = false;
    args.no_audio = false;
    args.filename = NULL;
    args.filename_alloc = false;
    args.enable_bootrom = false;

    // Optionnal arguments
    while ((opt = getopt(ac, av, shortopts)) >= 0) {
        switch (opt) {
            case 'h':
                print_help(av[0]);
                exit(EXIT_SUCCESS);

            case 'l':
                if (set_logger_level(optarg) < 0) {
                    fprintf(stderr, "Invalid log level\n");
                    exit(EXIT_FAILURE);
                }
                break;

            case 'b':
                args.enable_bootrom = true;
                break;

            case 'd':
                args.debug = true;
                break;

            case 'n':
                args.no_audio = true;
                break;

            default: exit(EXIT_FAILURE);
        }
    }

    // Positionnal arguments
    if (optind < ac)
        args.filename = av[optind];
}


int main(int ac, char **av)
{
    int emulation_ret = -1;
    gb_system_t *gb;

    parse_args(ac, av);
    initialize_sdl();
    if (!args.filename) {
        args.filename_alloc = true;
        if (!(args.filename = ask_for_file_drop())) {
            fprintf(stderr, "No ROM selected\n");
            return EXIT_FAILURE;
        }
    }

    if (!(gb = gb_system_create_load_rom(args.filename, args.enable_bootrom)))
        return EXIT_FAILURE;
    if (args.filename_alloc)
        free(args.filename);

    if (args.debug) {
        cartridge_dump(&gb->cartridge);
    }

    emulation_ret = emulate_gameboy(gb, !args.no_audio);
    gb_system_destroy(gb);
    return emulation_ret < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}