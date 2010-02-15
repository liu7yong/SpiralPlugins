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

#ifndef __SVFILTER_H__
#define __SVFILTER_H__

#include <SpiralCore/Port.h>
#include <SpiralCore/Device.h>
#include <SpiralCore/Patch.h>

using namespace Spiral;

static const int NUM_CHANNELS = 4;

// A State Variable Filter
class SVFilter : public Device
{
DeviceDescriptionTemplate(SVFilter)
private:
  /* Instance State */
	FloatProperty *Cutoff, *Resonance;
	
	FloatType fc; // Cutoff
	FloatType q;  // Resonance
	
	// Calculation
	FloatType m_f, m_q, m_qnrm;
	 
	// Outputs
	FloatType m_h, m_b, m_l, m_p, m_n;

	void WipeMemory();
public:
 	SVFilter(Patch *Host);
	
	bool CreatePorts();

	virtual void Process(UnsignedType SampleCount);
	virtual void Reset();
};

#endif