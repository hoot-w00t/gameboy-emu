/*
xalloc.h
Memory allocation functions that handle failure

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

#include <stddef.h>

#ifndef _GB_XALLOC_H
#define _GB_XALLOC_H

void *xalloc(size_t size);
void *xzalloc(size_t size);
char *xstrdup(const char *s);

#endif