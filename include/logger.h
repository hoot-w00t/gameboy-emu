/*
logger.h
Header for logger.c

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

#ifndef _logger_attr
#define _logger_attr __attribute__((format(printf, 2, 3)))
#endif

#ifndef _LOGGER_H
#define _LOGGER_H

#include <stdbool.h>

typedef enum loglevel {
    LOG_ALL = 0,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_CRIT
} loglevel_t;

bool logger_set_level_name(const char *level_name);

_logger_attr
void logger(loglevel_t level, const char *format, ...);

#endif