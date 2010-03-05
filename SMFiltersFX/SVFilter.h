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

//Initially commited by Dave, Sun Jul 28 23:18:15 2002 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1027916292::SVFilter"
//  => ba7dbd3d0088109861334e1787616b2e (legacy == C)
#define SVFilterID ba7dbd3d0088109861334e1787616b2e

class SVFilter : Gumbo(Device)
{
  GumboClassDefinition(SVFilter, Device,
    {
      mUniqueID = String::New(XSTRINGIFY(SVFilterID));
      mVersion = 2;
    },
    {
    },
  );
private:
  /* Instance State */
  FloatProperty *Cutoff, *Resonance;

  //FIXME - NOT VOICE SAFE
  FloatType fc; // Cutoff
  FloatType q;  // Resonance

  // Calculation
  FloatType m_f, m_q, m_qnrm;

  // Outputs
  FloatType m_h, m_b, m_l, m_p, m_n;

  void WipeMemory();

public:
  virtual SVFilter *Initialize(Patch *Host);
  static inline SVFilter *New(Patch *Host) { return Alloc()->Initialize(Host); }

  bool CreatePorts();

  virtual void Process(UnsignedType SampleCount);
  virtual void Reset();
};

#endif
