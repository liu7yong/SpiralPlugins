/*  SpiralSynth
 *  Copyleft (C) 2000 David Griffiths <dave@pawfal.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/ 

/* 
Resonant low pass filter source code. 
This code was originally written by baltrax@hotmail.com 

See <http://www.harmony-central.com/Computer/Programming/resonant-lp-filter.c>
for the full version with explanatory comments, and the maths!
*/ 


#ifndef __IIR_FILTER_H__
#define __IIR_FILTER_H__

#include <stdlib.h>
#include <stdio.h>

#include <SpiralCore/Misc.h>

using namespace Spicy;
using namespace Spiral;

/* FILTER INFORMATION STRUCTURE FOR FILTER ROUTINES */ 

class FILTER{ 
	public:
	FILTER() : length(0), history(NULL), coef(NULL){}
    UnsignedType length;       /* size of filter */ 
    FloatType *history;            /* pointer to history in filter */ 
    FloatType *coef;               /* pointer to coefficients of filter */ 
}; 

#define FILTER_SECTIONS   2   /* 2 filter sections for 24 db/oct filter */ 

typedef struct { 
        FloatType a0, a1, a2;       /* numerator coefficients */ 
        FloatType b0, b1, b2;       /* denominator coefficients */ 
} BIQUAD; 

extern BIQUAD ProtoCoef[FILTER_SECTIONS];     /* Filter prototype coefficients, 
	                                                 1 for each filter section */
													 
FloatType iir_filter(FloatType input,FILTER *iir);
 
void szxform( 
    FloatType *a0, FloatType *a1, FloatType *a2,     /* numerator coefficients */ 
    FloatType *b0, FloatType *b1, FloatType *b2,   /* denominator coefficients */ 
    FloatType fc,           /* Filter cutoff frequency */ 
    FloatType fs,           /* sampling rate */ 
    FloatType *k,           /* overall gain factor */ 
    FloatType *coef);         /* pointer to 4 iir coefficients */ 

void prewarp(FloatType *a0, FloatType *a1, FloatType *a2, FloatType fc, FloatType fs); 

void bilinear( 
    FloatType a0, FloatType a1, FloatType a2,    /* numerator coefficients */ 
    FloatType b0, FloatType b1, FloatType b2,    /* denominator coefficients */ 
    FloatType *k,                                   /* overall gain factor */ 
    FloatType fs,                                   /* sampling rate */ 
    FloatType *coef); 
	
#endif
