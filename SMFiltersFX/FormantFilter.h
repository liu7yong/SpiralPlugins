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

#ifndef __FormantFilter_H__
#define __FormantFilter_H__

#include <SpiralCore/Port.h>
#include <SpiralCore/Device.h>
#include <SpiralCore/Patch.h>

using namespace Spiral;

class FormantFilter : Gumbo(Device)
{
  GumboClassDefinition(FormantFilter, Device,
    {
      mVersion = 1;
    },
    {
    },
  );
private:
  InputPort *input[2];
  OutputPort *output;

  NumberProperty<FloatType> *m_Vowel;
      
  //FIXME - NOT VOICE SAFE
  double memory[5][10];

  void WipeMemory();
public:
  virtual FormantFilter *Initialize(Patch *Host);
  static inline FormantFilter *New(Patch *Host) { return Alloc()->Initialize(Host); }

  bool CreatePorts();

  virtual void Process(UnsignedType SampleCount);
  virtual void Reset();
};

#endif
