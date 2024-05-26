/* 
   Copyright (c)
     (c) 2018 Chintalagiri Shashank, Quazar Technologies Pvt. Ltd.
   
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
 * @file sllist.c
 * @brief Implementation of the Sorted Linked List
 * 
 * @see sllist.h
 */

#ifndef DS_SLLIST_H
#define DS_SLLIST_H

#include<stdint.h>

#define SLLIST_TAG_t    int8_t


typedef struct _SLLIST_NODE_STUB_t{
    struct _SLLIST_NODE_STUB_t * next;
    const SLLIST_TAG_t tag;
}_sllist_node_stub_t;


void sllist_install(_sllist_node_stub_t ** root, _sllist_node_stub_t * node);
_sllist_node_stub_t * sllist_find(_sllist_node_stub_t ** root, uint8_t tag);

#endif
