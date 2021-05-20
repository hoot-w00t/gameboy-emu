/*
xalloc.c
Memory allocation function wrappers

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Allocate SIZE bytes of memory.
void *xalloc(size_t size)
{
    void *ptr;

    if (!(ptr = malloc(size))) {
        fprintf(stderr, "%zu bytes memory allocation failed\n", size);
        abort();
    }

    return ptr;
}

// Allocate SIZE bytes of memory initialized with value 0
void *xzalloc(size_t size)
{
    void *ptr;

    if (!(ptr = calloc(size, 1))) {
        fprintf(stderr, "%zu bytes memory allocation failed\n", size);
        abort();
    }

    return ptr;
}

// Duplicate *s
char *xstrdup(const char *s)
{
    char *dup;

    if (!(dup = strdup(s))) {
        fprintf(stderr, "strdup(): %s (%zu bytes)\n", strerror(errno), strlen(s));
        abort();
    }

    return dup;
}