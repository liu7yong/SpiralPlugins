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

#ifndef __MoogFilter_H__
#define __MoogFilter_H__

#include <SpiralCore/Property.h>
#include <SpiralCore/Port.h>
#include <SpiralCore/Device.h>
#include <SpiralCore/Patch.h>

using namespace Spiral;

//Initially commited by Dave, Sun Jul 28 23:18:15 2002 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1027916292::MoogFilter"
//  => d668f07998c4e09b149ae1570d2143cc (legacy == D)
#define MoogFilterID d668f07998c4e09b149ae1570d2143cc

class MoogFilter : public Device
{
  GumboClassDefinition(MoogFilter, Device,
    {
      mUniqueID = String::New(XSTRINGIFY(MoogFilterID));
      mVersion = 1;
    },
    {
    },
  );
private:
  /* Instance State */
  void WipeState();

  InputPort *input[3];
  OutputPort *output[3];

  FloatProperty *Cutoff, *Resonance;

  //FIXME - NOT VOICE SAFE
  FloatType fc;
  FloatType f,p,q;
  FloatType b0,b1,b2,b3,b4;
  FloatType t1,t2;
public:
  virtual MoogFilter *Initialize(Patch *Host);
  static inline MoogFilter *New(Patch *Host) { return Alloc()->Initialize(Host); }

  bool CreatePorts();

  virtual void Process(UnsignedType SampleCount);
  virtual void Reset();
};

#endif
