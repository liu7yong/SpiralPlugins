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

#ifndef __Oscillator_H__
#define __Oscillator_H__

#include <SpiralCore/Port.h>
#include <SpiralCore/Device.h>
#include <SpiralCore/Patch.h>

using namespace Spiral;

//Initially commited by Dave, Sun Jul 28 23:18:15 2002 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1027898295::Oscillator"
//  =>  7f4e8dfa239d37c75b7ecce1a4048da9 (legacy == 0x4)
#define OscillatorID 7f4e8dfa239d37c75b7ecce1a4048da9
  
class Oscillator : Gumbo(Device)
{
  GumboClassDefinition(Oscillator, Device,
    {
      mUniqueID = String::New(XSTRINGIFY(OscillatorID));
      mVersion = 1;
    },
    {
    },
  );
  
private:
  // Voice Specific Properties (index)
  UnsignedType m_CyclePosInd, m_NoisevInd;

  // Common Shared Properties
  SetProperty *m_Type;
  UnsignedProperty *m_Octave;

  FloatProperty *m_FineFreq;
  FloatProperty *m_PulseWidth;
  FloatProperty *m_SHLen;
  FloatProperty *m_ModAmount;
protected:
  virtual void Finalize();
public:
  virtual Oscillator *Initialize(Patch *Host);
  static inline Oscillator *New(Patch *Host) { return Alloc()->Initialize(Host); }

  bool CreatePorts();

  virtual void Process(UnsignedType SampleCount);
};

#endif
