/* 
   Copyright (c)
     (c) 2025 Chintalagiri Shashank
   
   This file is part of
   Embedded bootstraps : data structures library
   
   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. 
*/

/**
 * @file hashmap.h
 * @brief Interface and usage of Hash Tables.
 *
 * @see hashmap.c
 */

#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifndef HASHMAP_SIZE_t
#define HASHMAP_SIZE_t uint8_t
#endif

typedef struct {
    uint16_t key;        // Key (can be uint8_t or uint16_t)
    bool in_use;         // Marker to differentiate empty slots
    size_t len;          // Length of the data
    void * ptr;        // Pointer to the associated data
} hashmap_entry_t;

typedef struct {
    hashmap_entry_t * table;   // Array of hash entries
    HASHMAP_SIZE_t size;       // Total capacity of the table
    HASHMAP_SIZE_t count;      // Number of elements in the table
} hashmap_t;

void hashmap_init(hashmap_t * map, hashmap_entry_t * table, size_t capacity);
void hashmap_clear(hashmap_t * map);
bool hashmap_insert(hashmap_t * map, uint16_t key, void *value, size_t len);
hashmap_entry_t * hashmap_get(hashmap_t * map, uint16_t key);
bool hashmap_remove(hashmap_t * map, uint16_t key);

#endif
