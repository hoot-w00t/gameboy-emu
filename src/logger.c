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
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static loglevel_t logger_level = LOG_WARN;
static const char *loglevel_names[] = {
    "All",
    "Debug",
    "Info",
    "Warn",
    "Error",
    "Crit",
    NULL
};

// Change the logging level
// Returns false on error
bool logger_set_level_name(const char *level_name)
{
    for (loglevel_t i = 0; loglevel_names[i]; ++i) {
        if (!strcasecmp(level_name, loglevel_names[i])) {
            logger_level = i;
            return true;
        }
    }
    return false;
}

// Log something
void logger(loglevel_t level, const char *format, ...)
{
    static char fmt_buf[384];

    if (level >= logger_level) {
        va_list ap;

        va_start(ap, format);
        vsnprintf(fmt_buf, sizeof(fmt_buf), format, ap);
        fprintf(stdout, "[%s] %s\n", loglevel_names[level], fmt_buf);
        fflush(stdout);
        va_end(ap);
    }
}