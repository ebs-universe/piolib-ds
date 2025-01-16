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
 * @file hashmap.c
 * @brief Implementation of the Hash Table data structure.
 * 
 * @see hashmap.h
 */


#include "hashmap.h"

static HASHMAP_SIZE_t hash_function(uint16_t key, HASHMAP_SIZE_t size) {
    return key % size;
}

void hashmap_init(hashmap_t * map, hashmap_entry_t * table, size_t capacity) {
    map->table = table;
    map->size = capacity;
    map->count = 0;
}

void hashmap_clear(hashmap_t * map) {
    map->count = 0;
    memset(map->table, 0, map->size * sizeof(hashmap_entry_t));
}

bool hashmap_insert(hashmap_t * map, uint16_t key, void * ptr, size_t len){
    if (map->count >= map->size) return false;

    size_t idx = hash_function(key, map->size);
    for (size_t i = 0; i < map->size; i++) {
        size_t probe_idx = (idx + i) % map->size; // Linear probing
        hashmap_entry_t *entry = &map->table[probe_idx];

        if (!entry->in_use || entry->key == key) {
            entry->key = key;
            entry->ptr = ptr;
            if (!entry->in_use) {
                entry->in_use = true;
                map->count++;
            }
            return true;
        }
    }

    return false;
}

hashmap_entry_t * hashmap_get(hashmap_t * map, uint16_t key){
    size_t idx = hash_function(key, map->size);
    for (size_t i = 0; i < map->size; i++) {
        size_t probe_idx = (idx + i) % map->size;
        hashmap_entry_t * entry = &map->table[probe_idx];

        if (entry->in_use && entry->key == key) {
            return entry;
        }
        if (!entry->in_use) 
        {
            // Key not found
            return NULL; 
        }
    }
    // Key not found
    return NULL; 
}

bool hashmap_remove(hashmap_t * map, uint16_t key){
    size_t idx = hash_function(key, map->size);
    for (size_t i = 0; i < map->size; i++) {
        size_t probe_idx = (idx + i) % map->size;
        hashmap_entry_t *entry = &map->table[probe_idx];

        if (entry->in_use && entry->key == key) {
            entry->in_use = false; 
            entry->ptr = NULL;
            entry->len = 0;
            map->count--;
            return true;
        }
        if (!entry->in_use) {
            return false; 
        }
    }

    return false;
}
