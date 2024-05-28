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
 * @file hamming.c
 * @brief Implementation of the hamming library.
 * 
 * @see hamming.h
 */


#include "hamming.h"


uint16_t pack_hamming11_5(uint8_t* raw){
    return(0);
}

uint16_t check_hamming11_5(uint16_t data){
    uint8_t i, lcount;
    uint8_t syndrome = 0x00;
    uint8_t p = 0x01;
    unsigned int cmask;
    
    //For each parity bit
    while(p<=0x08){
        lcount=0x00;
        switch(p){
            case 1: cmask = 0x8000; break;
            case 2: cmask = 0x4000; break;
            case 4: cmask = 0x1000; break;
            case 8: cmask = 0x0100; break;
        }
        while(cmask){
            i=0;
            while(i<p){
                if (data&cmask) lcount ^= 0x01;
                cmask >>=1 ;
                i++;
            }
  
            switch(p){
                case 8: cmask >>= 8; break; 
                case 4: cmask >>= 2;
                case 2: cmask >>= 1; 
                case 1: cmask >>= 1; 
            }
        }
        //Verify parity
        if (lcount){
            syndrome += p;
        }
        //Reset for next loop.
        p <<= 1;
    }
    
    cmask = 0x8000;
    lcount = 0x00;
    //Global Parity Bit
    while(cmask){
        if(data & cmask) lcount ^= 0x01;
        cmask >>= 1;
    }
    if(syndrome){
        //There is atleast one error.
        if (lcount){
            //Global Parity Failure - Good.
            return(data ^= (0x8000 >> (syndrome-1)));
        }
        else{
            //There are 2 errors - uncorrectable
            return(1);
        }
    }
    if (lcount){
        // Global Parity Failure, but individual parity bits are fine. This 
        // probably means the global parity got flipped. Hamming SED doesnt 
        // require global parity. Someone should check the theory.
        return(data ^= 0x0001);
    }
    return(data);
}

#if HAMMING_IMPL == HAMMING_IMPL_ITER
uint32_t pack_hamming26_6(uint8_t* data){
    // The first part is as good as its going to get. 
    // The parity generation can be improved.
    uint8_t lcount=0;
    uint8_t lcount1=0;
    uint8_t p=0x01;
    uint8_t imask, i;
    uint32_t result, buffer, cmask;

    // Insert the first byte
    result = (uint32_t)(data[0] & 0xFE) << 16;
    buffer = (uint32_t)(data[0] & 0x01) << 25;
    result |= buffer;

    // Insert the second byte
    buffer = (uint32_t)(data[1] & 0x7F) << 9;
    result |= buffer;
    buffer = (uint32_t)(data[1] & 0x80) << 19;
    result |= buffer;
    
    // Insert the third byte
    buffer = (uint32_t)(data[2]) << 1;
    result |= buffer;
    
    // Put in the authentication bits
    imask = 0x80;
    lcount = 0x00;
    while( imask ){
        if(data[1] & imask) lcount ^= 0x01;
        if(data[2] & imask) lcount ^= 0x01;
        if(data[0] & imask) lcount1 ^= 0x01;
        imask >>= 1;
    }

    if(lcount){
        result |= 0x08000000;
    }

    if(lcount1){
        result |= 0x20000000;
    }

    // Fill parity bits
    while(p <= 0x10){
        lcount=0x00;
        switch(p){
            case 1: cmask = 0x80000000; break;
            case 2: cmask = 0x40000000; break;
            case 4: cmask = 0x10000000; break;
            case 8: cmask = 0x01000000; break;
            case 16: cmask = 0x00010000; break;
        }
        while(cmask){
            i = 0;
            while(i < p){
                if(result & cmask) lcount ^= 0x01;
                cmask >>= 1;
                i ++;
            }
            
            switch(p){
                case 16: cmask >>= 16; break; 
                case 8: cmask >>= 8; break; 
                case 4: cmask >>= 2;
                case 2: cmask >>= 1; 
                case 1: cmask >>= 1; 
            }
        }
        // Verify parity
        if (lcount){
            switch(p){
                case 1: result |= 0x80000000; break;
                case 2: result |= 0x40000000; break;
                case 4: result |= 0x10000000; break;
                case 8: result |= 0x01000000; break;
                case 16: result |= 0x00010000; break;
            }
        }
        // Reset for next loop.
        p <<= 1;
    }
    
    // Set Global Parity
    cmask = 0x80000000;
    lcount = 0x00;
    while(cmask){
        if(result & cmask) lcount ^= 0x01;
        cmask >>= 1;
    }
    result |= lcount;
    return( result );
}
#endif

#if HAMMING_IMPL == HAMMING_IMPL_MATRIX
uint32_t pack_hamming26_6(uint8_t * data){
    uint32_t result, cmask;
    uint32_t p1data, p2data, p4data, p8data, p16data;
    uint8_t lcount=0, lcount1=0;
    uint8_t imask;
    
    //Insert the first byte
    result=((uint32_t)(data[0] & 0xFE) << 16) | ((uint32_t)(data[0] & 0x01) << 25);
    //Insert the second byte
    result|=((uint32_t)(data[1] & 0x7F) << 9) | ((uint32_t)(data[1] & 0x80) << 19);
    //Insert the third byte
    result|=(uint32_t)(data[2]) << 1;
    
    //Put in the authentication bits
    imask = 0x80;
    lcount = 0x00;
    while(imask){
        if(data[1] & imask) lcount ^= 0x01;
        if(data[2] & imask) lcount ^= 0x01;
        if(data[0] & imask) lcount1 ^= 0x01;
        imask >>= 1;
    }

    if(lcount){
        result |= 0x08000000;
    }

    if(lcount1){
        result |= 0x20000000;
    }

    //Fill parity bits
    cmask = 0x80000000;
    lcount = 0x00;
    //Generate Parity Masks
    p1data=   result & P1MASK;
    p2data=   result & P2MASK;
    p4data=   result & P4MASK;
    p8data=   result & P8MASK;
    p16data=  result & P16MASK;
    //Calculate parity
    while(cmask > 1){
        if(result & cmask)  {
            lcount^=0x80;
        }
        if(p1data & cmask)  lcount^=0x01;
        if(p2data & cmask)  lcount^=0x02;
        if(p4data & cmask)  lcount^=0x04;
        if(p8data & cmask)  lcount^=0x08;
        if(p16data & cmask) lcount^=0x10;
        cmask >>= 1;
    }
    
    //Construct outgoing parity
    if(lcount & 0x01) {result |= 0x80000000; lcount ^= 0x80;}
    if(lcount & 0x02) {result |= 0x40000000; lcount ^= 0x80;}
    if(lcount & 0x04) {result |= 0x10000000; lcount ^= 0x80;}
    if(lcount & 0x08) {result |= 0x01000000; lcount ^= 0x80;}
    if(lcount & 0x10) {result |= 0x00010000; lcount ^= 0x80;}
    if(lcount & 0x80) result |= 0x00000001;
    return( result );
}
#endif

#if HAMMING_IMPL == HAMMING_IMPL_ITER
uint32_t check_hamming26_6(uint32_t data, uint8_t* result){
    //This function needs optimization. It should work as is, but it will be dicey.  
    uint8_t syndrome=0x00; 
    uint8_t p=0x01;
    uint8_t i, lcount; 
    unsigned long cmask;
    
    //For each parity bit
    while(p <= 0x10){
        lcount = 0x00;
        
        switch(p){
            case 1: cmask = 0x80000000; break;
            case 2: cmask = 0x40000000; break;
            case 4: cmask = 0x10000000; break;
            case 8: cmask = 0x01000000; break;
            case 16: cmask = 0x00010000; break;
        }
    
        while(cmask){
            i = 0;
            while(i < p){
                if(data & cmask) lcount ^= 0x01;
                cmask >>= 1;
                i++;
            }
            
            switch(p){
                case 16: cmask >>= 16; break; 
                case 8: cmask >>= 8; break; 
                case 4: cmask >>= 2;
                case 2: cmask >>= 1; 
                case 1: cmask >>= 1; 
            }
        }
        
        //Verify parity
        if (lcount){
            syndrome += p;
        }
        //Reset for next loop.
        p <<= 1;
    }
    
    cmask = 0x80000000;
    lcount = 0x00;
    
    //Global Parity Bit
    while(cmask){
        if(data & cmask) lcount ^= 0x01;
        cmask >>= 1;
    }
    if(syndrome){
        //There is atleast one error.
        if (lcount){
            //Global Parity Failure - Good.
            *result = 0x01;
            return(data ^= (0x80000000 >> (syndrome-1)));
        }
        else{
            //There are 2 errors - uncorrectable
            *result = 0x02;
            return(1);
        }
    }
    if (lcount){
        // Global Parity Failure, but individual parity bits are fine. This probably 
        // means global parity got flipped. Hamming SED doesnt require global parity. 
        // Someone should check the theory.
        *result = 0x04;
        return(data ^= 0x00000001);
    }
    *result = 0x00;
    return(data);
}
#endif

#if HAMMING_IMPL == HAMMING_IMPL_MATRIX
uint32_t check_hamming26_6(uint32_t data, uint8_t* result){
    
    uint8_t syndrome=0x00; 
    uint8_t lcount, parity; 
    uint32_t cmask, p1data, p2data, p4data, p8data, p16data;

    cmask = 0x80000000;
    lcount = 0x00;
    //Generate Parity Masks
    p1data = data & P1MASK;
    p2data = data & P2MASK;
    p4data = data & P4MASK;
    p8data = data & P8MASK;
    p16data = data & P16MASK;
    
    //Calculate parity
    while (cmask > 0x01){
        if (data & cmask)    lcount ^= 0x80;
        if (p1data & cmask)  lcount ^= 0x01;
        if (p2data & cmask)  lcount ^= 0x02;
        if (p4data & cmask)  lcount ^= 0x04;
        if (p8data & cmask)  lcount ^= 0x08;
        if (p16data & cmask) lcount ^= 0x10;
        cmask >>= 1;
    }
    
    //Construct incoming parity
    parity = 0x00;
    if (data & 0x80000000) parity |= 0x01;
    if (data & 0x40000000) parity |= 0x02;
    if (data & 0x10000000) parity |= 0x04;
    if (data & 0x01000000) parity |= 0x08;
    if (data & 0x00010000) parity |= 0x10;
    if (data & 0x00000001) parity |= 0x80;

    syndrome = lcount ^ parity;

    if (syndrome & 0x7F){
        //There is atleast one error.
        if (syndrome & 0x80){
            //Global Parity Failure - Good.
            *result = 0x01;
            return(data ^= (0x80000000 >> ((syndrome & 0x7F)-1)));
        }
        else{
            //There are 2 errors - uncorrectable
            *result = 0x02;
            return(1);
        }
    }
    if (syndrome & 0x80){
        // Global Parity Failure, but individual parity bits are fine. This probably 
        // means global parity got flipped. Hamming SED doesnt require global parity. 
        // Someone should check the theory.
        *result = 0x04;
        return(data ^= 0x00000001);
    }
    *result = 0x00;
    return(data);
}
#endif


uint8_t unpack_hamming26_6(uint32_t code, uint8_t * target){
    // This is as good as it is going to get.
    uint8_t imask, lcount, lcount1;
    
    target[2] = (code & 0x0000001FE) >> 1;
    target[1] = ((code & 0x0000FE00) >> 9) | ((code & 0x04000000) >> 19);
    target[0] = ((code & 0x00FE0000) >> 16) | ((code & 0x02000000) >> 25);
    
    // Check auth bits
    imask = 0x80;
    lcount = 0x00;
    lcount1 = 0x00;
    
    while(imask){
        if(target[1] & imask) lcount ^= 0x01;
        if(target[2] & imask) lcount ^= 0x01;
        if(target[0] & imask) lcount1 ^= 0x01;
        imask >>= 1;
    }
    
    if(lcount){
        if(code & 0x08000000) {
            imask &= 0x80;
        }
        else {
            imask |= 0x80;
        }
    }
    else{
        if(code & 0x08000000) {
            imask |= 0x80;
        }
        else {
            imask &= 0x80;
        }
    }
    if(lcount1){
        if(code & 0x20000000) {
            imask &= 0x40;
        }
        else {
            imask |= 0x40;
        }
    }
    else{
        if(code & 0x20000000) {
            imask |= 0x40;
        }
        else {
            imask &= 0x40;
        }
    }
    return imask;
}
