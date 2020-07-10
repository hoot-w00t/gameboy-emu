/*
logger.c
Logging functions

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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static int log_level = LOG_WARN;
const char *logger_levels[] = {
    "ALL",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "CRIT",
    NULL
};

// set logging level
// return the log level or -1 on error
int set_logger_level(const char *level)
{
    char uppercase[16];

    memset(uppercase, 0, sizeof(uppercase));
    strncpy(uppercase, level, sizeof(uppercase) - 1);
    for (int i = 0; uppercase[i]; ++i) {
        uppercase[i] = toupper(uppercase[i]);
    }

    for (int i = 0; logger_levels[i]; ++i) {
        if (!strcmp(uppercase, logger_levels[i])) {
            log_level = i;
            return i;
        }
    }

    return -1;
}

// log something
void logger(const int level, const char *format, ...)
{
    if (level >= log_level) {
        va_list ap;
        va_start(ap, format);

        fprintf(stdout, "[%s] ", logger_levels[level]);
        vfprintf(stdout, format, ap);
        fprintf(stdout, "\n");

        va_end(ap);
    }
}