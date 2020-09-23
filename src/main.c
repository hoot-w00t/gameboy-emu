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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static struct args {
    bool debug;
    char *filename;
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
    printf("    -d              Run in debugging mode\n");
}

void parse_args(int ac, char **av)
{
    const char shortopts[] = "hl:d";
    int opt;

    // Default values
    args.debug = false;
    args.filename = NULL;

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

            case 'd':
                args.debug = true;
                break;

            default: exit(EXIT_FAILURE);
        }
    }

    // Positionnal arguments
    if (optind >= ac) {
        fprintf(stderr, "Missing argument: filename\n");
        exit(EXIT_FAILURE);
    }
    args.filename = av[optind];
}

int main(int ac, char **av)
{
    gb_system_t *gb;

    parse_args(ac, av);
    if (!(gb = gb_system_create_load_rom(args.filename)))
        return EXIT_FAILURE;

    gb_system_emulate(gb);
    gb_system_destroy(gb);
    return EXIT_SUCCESS;
}