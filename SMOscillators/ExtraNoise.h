/*  This program is free software; you can redistribute it and/or modify
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

#ifndef __ExtraNoise_H__
#define __ExtraNoise_H__

#include <SpiralCore/Misc.h>
#include <SpiralCore/Port.h>
#include <SpiralCore/Device.h>
#include <SpiralCore/Patch.h>

using namespace Spiral;

class ExtraNoise : Gumbo(Device)
{
  GumboClassDefinition(ExtraNoise, Device,
    {
      mVersion = 1;
    },
    {
    },
  );
private:
  /* Instance State */
  InputPort *input[2];
  OutputPort *output;
  
  // Voice Specific Properties (index)
  UnsignedType mState;
  
  NumberProperty<FloatType> *mMagicA, *mMagicB;
  
public:
  virtual ExtraNoise *Initialize(Patch *Host);
  static inline ExtraNoise *New(Patch *Host) { return Alloc()->Initialize(Host); }
  
  bool CreatePorts();
  void CreateProperty(UnsignedType aPropertyID, Property *aProperty);
  
  virtual void 		Process(UnsignedType SampleCount);
};

#endif
