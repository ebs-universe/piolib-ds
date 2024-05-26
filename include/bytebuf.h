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
 * @file bytebuf.h
 * @brief Interface and usage of bytebuf buffers.
 *
 * This library provides a ::bytebuf structure and associated functions
 * to implement a circular byte buffer. The design of the buffer is intended 
 * to support continuous byte-wide datastreams. A simple locking mechanism
 * for writes is included.
 * 
 * The typical application of such a buffer is when one or more data sources 
 * (producers) are writing to a single sink (consumer). In the general case,
 * the buffer is instantiated at and by the consumer, while the producers 
 * user their respective tokens to write to the buffer. 
 * 
 * While errors are reported, for optimal use implementations should ensure
 * that the buffers are large enough for the consumers to handle all of the 
 * produced data before an overflow occurs. 
 * 
 * A bytebuf buffer can be at-most 255 bytes long. For longer buffers, some 
 * datatypes need to be changed from uint8_t to uint16_t. This will have a
 * performance penalty on uCs with an 8-bit bus.
 * 
 * @see bytebuf.c
 */

#ifndef BYTEBUF_H
#define BYTEBUF_H

#include<stdint.h>

/**
 * @name Generic Write Tokens
 */
/**@{*/ 

#define BYTEBUF_LOCK_OPEN       0
#define BYTEBUF_TOKEN_PRINT     1
#define BYTEBUF_TOKEN_SCHAR     2

/**@}*/ 


/**
 * @brief Primary bytebuf struct typedef
 * 
 * The core of the bytebuf implementation, this type contains bytebuf 
 * metadata and control information. This struct should be created by 
 * instantiating a variable at the consumer of this type, and calling 
 * bytebuf_vInit(). 
 * 
 * This struct does not contain the actual data but has a pointer to 
 * a byte array which should be separately created and contains the 
 * actual data. 
 */
typedef struct BYTEBUF{
    uint8_t * _bufp;              /**< @brief Pointer to the start of the underlying buffer. */
    uint8_t * _inp;               /**< @brief Pointer to the current write location. */
    uint8_t * _outp;              /**< @brief Pointer to the current read location. */
    volatile uint8_t _population; /**< @brief Number of bytes presently in the buffer. */
    uint8_t _size;                /**< @brief Size of buffer. */
    uint8_t _lock;                /**< @brief Token which holds the write lock at present. */
    uint8_t _locklen;             /**< @brief Number of additional bytes for which the lock is valid. */
} bytebuf;

/**
  * Initialize a bytebuf structure using  an externally supplied (and allocated) 
  * memory array. 
  * @param bufferp Pointer to the array; should be in the heap and not in a task stack.
  * @param size Size of the array. Beware : we have no good way to check this later.
  * @param bytebufp Pointer to the bytebuf structure to be initialized. 
  */
void bytebuf_vInit(uint8_t * bufferp, uint8_t size, bytebuf * bytebufp);

/**
  * Flush a buffer and reset it to its pristine state.
  * @warning Any locks held on the buffer will disappear and holder is not informed.
  * @warning Any data present in the data will be lost. 
  * @param bytebufp Pointer to the bytebuf structure.
  */
void bytebuf_vFlush(bytebuf * bytebufp);


/**
  * Return the number of bytes present in the buffer to be read.
  * @param bytebufp Pointer to the bytebuf structure.
  * @return The number of bytes left in the buffer to be read.
  */
static inline uint8_t bytebuf_cPopulation(bytebuf * bytebufp);

static inline uint8_t bytebuf_cPopulation(bytebuf * bytebufp){
    return (bytebufp->_population);
}


/**
  * Return the number of bytes that can be written into the buffer right now.
  * You still need to acquire a lock if you want to write into it. 
  * @see bytebuf_cPushReqLock()
  * @param bytebufp Pointer to the bytebuf structure.
  * @return The number of empty bytes left in the buffer.
  */
static inline uint8_t bytebuf_cGetFree(bytebuf * bytebufp);
    
static inline uint8_t bytebuf_cGetFree(bytebuf * bytebufp){
    return (bytebufp->_size - bytebufp->_population);
}


/**
  * \brief Return the number of bytes that can be written contiguously.
  * You still need to acquire a lock if you want to write into it. 
  * 
  * @see bytebuf_cPushChunk()
  * @param bytebufp Pointer to the bytebuf structure.
  * @param past_rollover Pointer to uint8_t in which to store rollover flag.
  * @return The number of bytes that can be written contiguously.
  */
static inline uint8_t bytebuf_cPushChunkLen(bytebuf * bytebufp, uint8_t * past_rollover);

static inline uint8_t bytebuf_cPushChunkLen(bytebuf * bytebufp, uint8_t * past_rollover){
    uint8_t free = bytebufp->_size - bytebufp->_population;
    uint8_t lend = bytebufp -> _bufp + bytebufp -> _size - bytebufp -> _inp + 1;
    if (free < lend){
        *past_rollover = 1;
        return free;
    }
    else{
        *past_rollover = 0;
        return lend;
    }
    return 0;
}


/**
  * \brief Return the number of bytes that can be read contiguously.
  * 
  * @see bytebuf_cPopChunk()
  * @param bytebufp Pointer to the bytebuf structure.
  * @param at_rollover Pointer to uint8_t in which to store rollover flag.
  * @return The number of bytes that can be read contiguously.
  */
static inline uint8_t bytebuf_cPopChunkLen(bytebuf * bytebufp, uint8_t * at_rollover);

static inline uint8_t bytebuf_cPopChunkLen(bytebuf * bytebufp, uint8_t * at_rollover){
    uint8_t pop = bytebufp -> _population;
    uint8_t lend = bytebufp -> _bufp + bytebufp -> _size - bytebufp -> _outp + 1;
    if (pop < lend){
        *at_rollover = 0;
        return pop;
    }
    else{
        *at_rollover = 1;
        return lend;
    }
    return 0;
}


/** 
  * Request a write lock on a bytebuf for a specific length.
  * Actual writing is done using other functions. 
  * @see bytebuf_cPush()
  * @see bytebuf_cPushLen()
  * @param bytebufp Pointer to the bytebuf structure.
  * @param length Number of bytes you want to write.
  * @param token An identifier against which to issue the lock.
  * @return 0 for lock denied, 1 for lock issued.
  */
uint8_t bytebuf_cPushReqLock(bytebuf * bytebufp, uint8_t length, uint8_t token);

/** 
  * Request a write lock on a bytebuf for an unspecified length.
  * Actual writing is done using other functions. 
  * @see bytebuf_cPush()
  * @see bytebuf_cPushLen()
  * @param bytebufp Pointer to the bytebuf structure.
  * @param token An identifier against which to issue the lock.
  * @return 0 for lock denied, number of characters free for granted.
  */
uint8_t bytebuf_cPushReqBlindLock(bytebuf * bytebufp, uint8_t token);

/** 
  * Relinquish a write lock on a bytebuf.
  * Actual writing is done using other functions. 
  * @see bytebuf_cPushReqLock()
  * @param bytebufp Pointer to the bytebuf structure.
  * @param token An identifier against which the lock was issued.
  * @return 0 for incorrect token, 1 for lock relinquished, 2 for already open.
  */
uint8_t bytebuf_cPushRelinquishLock(bytebuf * bytebufp, uint8_t token);


/**
  * Write a single byte into the bytebuf, clobbering the oldest
  * value in the buffer if there isn't room. Locking isn't used.
  *
  * @param bytebufp Pointer to the bytebuf structure.
  * @param byte Byte to be written in.
  * @return 0 for failed write, 1 for successful write.
  */
uint8_t bytebuf_cWriteByte(bytebuf * bytebufp, uint8_t byte);


/**
  * Push a single byte into the bytebuf.
  * You should already hold the write lock to do this.
  * @see bytebuf_cPushReqLock()
  * @param bytebufp Pointer to the bytebuf structure.
  * @param byte Byte to be written in.
  * @param token Identifier against which you own the lock.
  * @return 0 for failed write, 1 for successful write.
  */
uint8_t bytebuf_cPushByte(bytebuf * bytebufp, uint8_t byte, uint8_t token);

/**
  * Push a length of data into a bytebuf. 
  * You should already hold the write lock to do this.
  * @see bytebuf_cPushReqLock()
  * @param bytebufp Pointer to the bytebuf structure.
  * @param sp Pointer to the source buffer.
  * @param len Number of bytes to push in.
  * @param token Identifier against which you own the lock.
  * @return 0 for failed write, 1 for successful write.
  */
uint8_t bytebuf_cPushLen(bytebuf * bytebufp, void* sp, uint8_t len, uint8_t token);


/**
  * Pop a byte from the bytebuf. 
  * You should make sure that there actually is a byte to be read first.
  * @see cbytebuf_population()
  * @param bytebufp Pointer to the bytebuf structure.
  * @return The byte that got popped, or 0 by default if there wasn't one. 
  * @warning return value of 0 does not necessarily mean an error occured.
  */
uint8_t bytebuf_cPopByte(bytebuf * bytebufp);


/**
  * Pop a length of data from a bytebuf using the platform's memcpy 
  * function. 
  * 
  * You should ensure that that number of bytes actually exists in the 
  * buffer to be read.
  * @see bytebuf_cPopulation()
  * @param bytebufp Pointer to the bytebuf structure.
  * @param dp Pointer to the destination buffer.
  * @param len Number of bytes to pop.
  * @return 0 for failed read, 1 for successful read.
  */
uint8_t bytebuf_cPopLen(bytebuf * bytebufp, void* dp, uint8_t len);


/**
  * Copy a length of data from a bytebuf using the platform's memcpy 
  * function. 
  * 
  * You should ensure that that number of bytes actually exists in the 
  * buffer to be read.
  * @see bytebuf_cPopulation()
  * @param bytebufp Pointer to the bytebuf structure.
  * @param dp Pointer to the destination buffer.
  * @param len Number of bytes to copy.
  * @return 0 for failed read, 1 for successful copy.
  */
uint8_t bytebuf_cCopyLen(bytebuf * bytebufp, void* dp, uint8_t len);

/**
  * \brief Mark the next `n` outbytes as handled. 
  * This function is intended to be used with DMA or other functions 
  * which read the array in the background. After the read is complete, 
  * the buffer should be informed via this function that the read data 
  * can be discarded from the buffer.
  * ,
  * @see bytebuf_cPopChunkLen()
  * @param bytebufp Pointer to the bytebuf structure.
  * @param len The number of bytes that should be popped.
  * @return 0 if insufficient bytes to pop, the number of bytes popped 
  *         otherwise.
  */
uint8_t bytebuf_cPopChunk(bytebuf * bytebufp, uint8_t len);


#endif
