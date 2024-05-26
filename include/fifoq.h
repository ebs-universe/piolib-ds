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
 * @file fifoq.c
 * @brief Implementation of a generic FIFO queue.
 * 
 * @see fifoq.h
 */

#ifndef DS_FIFOQ_H
#define DS_FIFOQ_H

#include<stdint.h>


typedef struct _FIFOQ_ITEM_STUB_t{
    struct _FIFOQ_ITEM_STUB_t * next;
}_fifoq_item_stub_t;


typedef struct FIFOQ_t{
    struct _FIFOQ_ITEM_STUB_t * first;
    struct _FIFOQ_ITEM_STUB_t * last;
}fifoq_t;


void fifoq_init(fifoq_t * queue);
void fifoq_item_init(_fifoq_item_stub_t item);
void fifoq_enqueue(fifoq_t * queue, _fifoq_item_stub_t * item);
void fifoq_enqueue_priority(fifoq_t * queue, _fifoq_item_stub_t * item);
void fifoq_cancel(fifoq_t * queue, _fifoq_item_stub_t * item);
_fifoq_item_stub_t * fifoq_pop_next(fifoq_t * queue);

#endif

