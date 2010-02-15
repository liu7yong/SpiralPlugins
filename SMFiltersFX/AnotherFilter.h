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

#ifndef __AnotherFilter_H__
#define __AnotherFilter_H__

#include <SpiralCore/Port.h>
#include <SpiralCore/Device.h>
#include <SpiralCore/Patch.h>

using namespace Spiral;

// A State Variable Filter
class AnotherFilter : public Device
{
DeviceDescriptionTemplate(AnotherFilter)
private:
	// Shared Property Values
	FloatProperty *Cutoff, *Resonance;

	// Voice State Values (Index)
	UnsignedType m_VibraPosInd,m_VibraSpeedInd, m_ResonanceInd, m_CutoffInd;

	InputPort *input[3];
	OutputPort *output[1];

public:
 	AnotherFilter(Patch *Host);

	bool CreatePorts();
	
	virtual void Process(UnsignedType SampleCount);
};

#endif
