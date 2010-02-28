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

//Initially commited by Dave, Thu Jan 2 01:56:01 2003 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1043740561::AnotherFilter"
//  => 420c8ee427e7ae589bc74eb766e66323 (legacy == 2B)
#define AnotherFilterID 420c8ee427e7ae589bc74eb766e66323
class AnotherFilter : Gumbo(Device)
{
  GumboClassDefinition(AnotherFilter, Device,
    {
     mUniqueID = String::New(XSTRINGIFY(AnotherFilterID));
     mVersion = 1;
    },
    {
    },
  );
 private:
	// Shared Property Values
	FloatProperty *Cutoff, *Resonance;

	// Voice State Values (Index)
	UnsignedType m_VibraPosInd,m_VibraSpeedInd, m_ResonanceInd, m_CutoffInd;

	InputPort *input[3];
	OutputPort *output[1];

public:
 	virtual AnotherFilter *Initialize(Patch *Host);
    static inline AnotherFilter *New(Patch *Host) { return Alloc()->Initialize(Host); }

	bool CreatePorts();
	
	virtual void Process(UnsignedType SampleCount);
};

#endif
