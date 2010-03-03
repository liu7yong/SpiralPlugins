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

#ifndef __RINGMOD_H__
#define __RINGMOD_H__

#include <SpiralCore/Property.h>
#include <SpiralCore/Port.h>
#include <SpiralCore/Device.h>
#include <SpiralCore/Patch.h>

using namespace Spiral;

//Initially commited by Dave, 
//md5 -s "Dave Griffiths::dave@pawfal.org::1027898295::RingMod"
//  => f9587ded2aed867d761fca167c1d3960 (legacy == 0xA)
#define RingModID f9587ded2aed867d761fca167c1d3960

class RingMod : Gumbo(Device)
{
  GumboClassDefinition(RingMod, Device,
    {
      mUniqueID = String::New(XSTRINGIFY(RingModID));
      mVersion = 1;
    },
    {
    },
  );
private:
	FloatProperty *m_Amount;

protected:
  virtual void Finalize();

public:
 	virtual RingMod* Initialize(Patch *Host);
  static inline RingMod *New(Patch *Host) { return Alloc()->Initialize(Host); }
  
	bool CreatePorts();

	virtual void Process(UnsignedType SampleCount);
};

#endif
