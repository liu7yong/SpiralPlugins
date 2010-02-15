/*  SpiralSound
 *  Copyleft (C) 2001 David Griffiths <dave@pawfal.org>
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

#ifndef __Filter_H__
#define __Filter_H__

#include <SpiralCore/Port.h>
#include <SpiralCore/Device.h>
#include <SpiralCore/Patch.h>

#include "iir_filter.h"

using namespace Spiral;

class Filter : public Device
{
DeviceDescriptionTemplate(Filter)
private:
  /* Instance State */
	void SetupCoeffs();

	InputPort *input[3];
	OutputPort *output;

	//FIXME NOT VOICE SAFE YET

	// Voice specifics
	FILTER   iir; 
   	
	// Voice independant
	FloatType    *coef; 

	FloatProperty *fc;     // cutoff frequency  
	FloatProperty *Q;      // Resonance > 1.0 < 1000  

	//FIXME IS THIS VOICE SAFE??
	FloatType   m_LastFC;
	FloatType   m_LastQ;

	unsigned nInd; 
	FloatType   a0, a1, a2, b0, b1, b2; 
	FloatType   k;          // overall gain factor 
public:
 	Filter(Patch *Host);

	bool Setup();
	void Shutdown();

	bool CreatePorts();

	virtual void Process(UnsignedType SampleCount);
	virtual void Reset();
};

#endif
