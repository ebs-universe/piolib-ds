/* 
   Copyright (c)
     (c) 2016      Chintalagiri Shashank, Firefly Aerospace Pvt.Ltd.
     (c) 2009-2011 Chintalagiri Shashank, Jugnu, IIT Kanpur
    
   This file is part of
   Embedded bootstraps : hamming library
   
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
 * @file hamming.h
 * @brief Interface and usage of the hamming library.
 * 
 * @see hamming.c
 */

#ifndef HAMMING_H
#define HAMMING_H

#include <stdint.h>
#include "config.h"

#define P1MASK  0b00101010101010101010101010101010
#define P2MASK  0b00100110011001100110011001100110
#define P4MASK  0b00001110000111100001111000011110
#define P8MASK  0b00000000111111100000000111111110
#define P16MASK 0b00000000000000001111111111111110


#ifdef __cplusplus
extern "C"
{
#endif

#ifndef SWIG
/**
 * @brief Install the hamming library descriptor to the application.
 * 
 * The hamming application descriptor contains the library version number and 
 * is installed to the application descriptors with the tag specified in UCDM's 
 * descriptor header as DESCRIPTOR_TAG_LIBVERSION. 
 * 
 * This does not effect the functionality of the hamming library in any way.
 */
void hamming_install_descriptor(void);

#endif

/**
  * @brief Checks a the provided hamming encoded data for validity as a 
  * hamming(11, 5) unit, and corrects recoverable errors. 
  * 
  * In case the errors are not correctable, returns 1.
  * 
  * @param code The hamming(11, 5) encoded data in a uint16_t
  * @return Verified / corrected data, or 0x0001 if not recoverable.
  * 
  */
uint16_t check_hamming11_5(uint16_t code);

/**
  * @brief Packs a single byte into a hamming(11, 5) encoded uint16_t. 
  * 
  * The three additional data bits are left unused, making this implementation 
  * very inefficient in it's packing ratio.
  * 
  * @param raw Pointer to the data to be packed
  * @return The hamming encoded data in a uint16_t
  * 
  */
uint16_t pack_hamming11_5(uint8_t* raw);


/**
  * @brief Checks a the provided hamming encoded data for validity as a 
  * hamming(26, 6) unit, corrects recoverable errors. 
  * 
  * Single bit errors are corrected. Even bit errors are detected. Odd number 
  * of bit errors are iffy - the auth check at unpacking is the last line of 
  * defense. 
  * 
  * result is updated to hold the result of the hamming check. 
  *     - 0x01 : One error detected and corrected
  *     - 0x02 : Even number of errors - uncorrectable
  *     - 0x04 : Global Parity Failure. Not sure what this means
  *     - 0x00 : The data was fine
  * 
  * @param code The hamming(26,6) encoded data in a uint32_t
  * @param result Pointer to storage to hold the result of the verification
  * @return Verified / corrected data, or 0x0001 if not recoverable.
  * 
  */
uint32_t check_hamming26_6(uint32_t code, uint8_t* result);

/**
  * @brief Packs three bytes into a hamming(26, 6) encoded uint32_t. 
  * 
  * The two additional data bits are used as authentication bits for an additional 
  * verification stage, though the degree of protection provided by this is unknown. 
  * 
  * @param raw Pointer to the data to be packed. Should be an array of uint8_t with 
  *            atleast 3 bytes of valid data.
  * @return The hamming encoded data in a uint16_t
  * 
  */
uint32_t pack_hamming26_6(uint8_t* raw);

/**
  * @brief Unpacks three bytes of data from a hamming(26, 6) encoded uint32_t. 
  * The two additional data bits are used as authentication bits for an additional 
  * verification stage, though the degree of protection provided by this is unknown. 
  * 
  * @param code The hamming(26, 6) encoded data to unpack.
  * @param target Pointer to the storage array to unpack to. Three (uint8_t) bytes will 
  *               be filled in.
  * @return The unpack status, 
  *            0x00 : Auth passed. 
  *            0x80 : Auth 1 failed.
  *            0x40 : Auth 2 failed.  
  */
uint8_t unpack_hamming26_6(uint32_t code, uint8_t* target);

#ifdef __cplusplus
}
#endif

#endif
