/* 
   Copyright (c)
     (c) 2015-2016 Chintalagiri Shashank, Quazar Technologies Pvt. Ltd.
   
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
 * @file avltree.c
 * @brief Implementation of the AVL tree data structure.
 * 
 * @see avltree.h
 */

#ifndef DS_AVLTREE_H
#define DS_AVLTREE_H

#include<stdint.h>
#include<stddef.h>

#define AVLT_HEIGHT_t   int8_t

typedef struct AVLT_NODE_t{
    uint16_t key;
    struct AVLT_NODE_t * left;
    struct AVLT_NODE_t * right;
    uint8_t height;
    void * content;
}avlt_node_t;


typedef struct AVLT_t {
    avlt_node_t *root;
}avlt_t;


void avlt_init_node(avlt_node_t * node, uint16_t key, void * content);
void avlt_insert_node(avlt_t * parent, avlt_node_t * node);
avlt_node_t * avlt_find_node(avlt_t * tree, uint16_t key);

#endif