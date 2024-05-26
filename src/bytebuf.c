/* 
   Copyright (c)
     (c) 2013, 2015-2016 Chintalagiri Shashank, Quazar Technologies Pvt.Ltd.
     (c) 2009-2011 Chintalagiri Shashank, Jugnu, IIT Kanpur
   
   This file is part of
   Embedded bootstraps : bytebuf library
   
   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. 
*/

/**
 * @file bytebuf.c
 * @brief Implementation of the bytebuf interface
 * 
 * See bytebuf.h for usage documentation. Implementation 
 * documentation should be added to this file as some point. 
 * 
 * @see bytebuf.h
 */

#include "bytebuf.h"
#include <string.h>


void bytebuf_vInit( uint8_t * bufferp, uint8_t size, bytebuf * bytebufp )
{
    bytebufp -> _bufp = bufferp;
    bytebufp -> _size = size;
    bytebuf_vFlush( bytebufp );
    return;
}


void bytebuf_vFlush( bytebuf * bytebufp )
{
    bytebufp -> _inp = bytebufp -> _bufp;
    bytebufp -> _outp = bytebufp -> _bufp;
    bytebufp -> _population = 0;
    bytebufp -> _lock = BYTEBUF_LOCK_OPEN;
    bytebufp -> _locklen = 0;
    return;
}


uint8_t bytebuf_cPushReqLock( bytebuf * bytebufp, uint8_t length, uint8_t token )
{
    if ( bytebufp -> _lock == BYTEBUF_LOCK_OPEN )
    {
        if ( bytebuf_cGetFree( bytebufp ) >= length )
        {
            bytebufp -> _lock = token;
            bytebufp -> _locklen = length;
            return 1;
        }
    }
    return 0; 
}


uint8_t bytebuf_cPushReqBlindLock( bytebuf * bytebufp, uint8_t token )
{
    uint8_t avail;
    if ( bytebufp -> _lock == BYTEBUF_LOCK_OPEN )
    {
        avail = bytebuf_cGetFree( bytebufp );
        if ( avail > 0 )
        {
            bytebufp -> _lock = token;
            bytebufp -> _locklen = avail;
            return avail;
        }
    }
    return 0; 
}


uint8_t bytebuf_cPushRelinquishLock(bytebuf * bytebufp, uint8_t token){
    if ( bytebufp -> _lock == token){
        bytebufp -> _lock = BYTEBUF_LOCK_OPEN;
        bytebufp -> _locklen = 0;
        return 1;
    }
    else if ( bytebufp -> _lock == BYTEBUF_LOCK_OPEN){
        return 2;
    }
    return 0;
}


uint8_t bytebuf_cWriteByte( bytebuf * bytebufp, uint8_t byte )
{
    if (!bytebuf_cGetFree(bytebufp)){
        if(bytebufp->_outp == bytebufp->_bufp + bytebufp->_size){
            bytebufp->_outp = bytebufp->_bufp;
        }
        else{
            bytebufp->_outp++;
        }
    }
    else{
        bytebufp->_population++;
    }
    
    *( bytebufp->_inp ) = byte;
    
    if ( bytebufp->_inp == bytebufp->_bufp + bytebufp->_size ){
        bytebufp->_inp = bytebufp->_bufp;
    }
    else{
        bytebufp->_inp++;
    }
    return 1;
}


uint8_t bytebuf_cPushByte( bytebuf * bytebufp, uint8_t byte, uint8_t token )
{
    if ( bytebufp->_lock == token )
    {
        *( bytebufp->_inp ) = byte;
        if ( bytebufp->_inp == bytebufp->_bufp + bytebufp->_size ){
            bytebufp->_inp = bytebufp->_bufp;
        }
        else{
            bytebufp->_inp++;
        }
        
        if (token != BYTEBUF_LOCK_OPEN){
            bytebufp->_locklen--;
            if ( !(bytebufp->_locklen) )
            {
                bytebufp->_lock = BYTEBUF_LOCK_OPEN;
            }
        }
        bytebufp->_population++;
        return 1;
    }
    return 0;
}


uint8_t bytebuf_cPushLen( bytebuf * bytebufp, void* sp, uint8_t len, uint8_t token )
{
    uint8_t clen = 0;
    uint8_t past_rollover;
    if ( bytebufp->_lock == token && ( (bytebufp->_locklen) >= len ) ){
        clen = bytebuf_cPushChunkLen(bytebufp, &past_rollover);
        if (len < clen || past_rollover){
            memcpy((void *)(bytebufp->_inp), sp, len);
            bytebufp->_inp += len;
        }
        else{
            memcpy((void *)(bytebufp->_inp), sp, clen);
            memcpy((void *)(bytebufp->_bufp), (void *)((uint8_t *)sp + clen), (len - clen));
            bytebufp->_inp = bytebufp->_bufp + (len - clen);
        }
        bytebufp->_locklen -= len;
        if (!(bytebufp->_locklen)){
            bytebufp->_lock = BYTEBUF_LOCK_OPEN;
        }
        bytebufp->_population += len;
        return len;
    }
    return 0;
}


uint8_t bytebuf_cPopByte( bytebuf * bytebufp )
{
    if (bytebufp -> _population){
        uint8_t rval = *(bytebufp -> _outp);
        if(bytebufp->_outp == bytebufp->_bufp + bytebufp->_size){
            bytebufp->_outp = bytebufp->_bufp;
        }
        else{
            bytebufp->_outp++;
        }
        bytebufp->_population--;
        return rval;
    }
    return 0;
}


uint8_t bytebuf_cPopLen(bytebuf * bytebufp, void* dp, uint8_t len){
    uint8_t clen = 0;
    uint8_t at_rollover;
    if (bytebufp->_population >= len){
        clen = bytebuf_cPopChunkLen(bytebufp, &at_rollover);
        if (len <= clen){
            memcpy(dp, (void *)(bytebufp->_outp), len);
            bytebufp->_outp += len;
        }
        else{
            memcpy(dp, (void *)(bytebufp->_outp), clen);
            memcpy((void *)((uint8_t*)dp + clen), (void *)(bytebufp->_bufp), (len - clen));
            bytebufp->_outp = bytebufp->_bufp + (len - clen);
        }
        bytebufp->_population -= len;
        return len;    
    }
    return 0;
}


uint8_t bytebuf_cCopyLen(bytebuf * bytebufp, void* dp, uint8_t len){
    uint8_t clen = 0;
    uint8_t at_rollover;
    if (bytebufp->_population >= len){
        clen = bytebuf_cPopChunkLen(bytebufp, &at_rollover);
        if (len <= clen){
            memcpy(dp, (void *)(bytebufp->_outp), len);
        }
        else{
            memcpy(dp, (void *)(bytebufp->_outp), clen);
            memcpy((void *)((uint8_t*)dp + clen), (void *)(bytebufp->_bufp), (len - clen));
        }
        return len;    
    }
    return 0;
}


uint8_t bytebuf_cPopChunk(bytebuf * bytebufp, uint8_t len){
    uint8_t pop = bytebufp->_population;
    uint8_t clen;
    uint8_t at_rollover;
    if (pop < len){
        return 0;
    }
    clen = bytebuf_cPopChunkLen(bytebufp, &at_rollover);
    if (at_rollover){
        bytebufp->_outp = bytebufp->_bufp + (len - clen);
    }
    else{
        bytebufp->_outp += len;
    }
    bytebufp->_population -= len;
    return len;
}
