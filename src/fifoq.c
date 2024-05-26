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

#include "ds/fifoq.h"
#include <stddef.h>


void fifoq_init(fifoq_t * queue){
    queue->first = NULL;
    queue->last = NULL;
    return;
}

void fifoq_enqueue(fifoq_t * queue, _fifoq_item_stub_t * item){
    if (queue->first == NULL){
        queue->first = item;
    }
    if (queue->last){
        queue->last->next = item;
    }
    queue->last = item;
    return;
}

void fifoq_enqueue_priority(fifoq_t * queue, _fifoq_item_stub_t * item){
    if (queue->first){
        item->next = queue->first;
    }
    if (queue->last == NULL){
        queue->last = item;
    }
    queue->first = item;
    return;
}

void fifoq_cancel(fifoq_t * queue,  _fifoq_item_stub_t * item){
    _fifoq_item_stub_t * walker = queue->first;
    
    if (walker == NULL){
        return;
    }
    
    if (walker == item){
        queue->first = queue->first->next;
        if (queue->last == item) queue->last = NULL;
        return;
    }
    
    while (walker->next){
        if (walker->next == item){
            walker->next = walker->next->next;
            if (walker->next == NULL){
                queue->last = walker;
            }
            return;
        }
        walker = walker->next;
    }
    return;
}

_fifoq_item_stub_t * fifoq_pop_next(fifoq_t * queue){
    _fifoq_item_stub_t * ritem = queue->first;
    if (ritem == queue->last){
        queue->last = NULL;
    }
    if (ritem){
        queue->first = queue->first->next;
    }
    return ritem;
}
