/*  SpiralSound
 *  Copyleft (C) 2002 Andy Preston <andy@clubunix.co.uk>
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

#ifndef __LFO_H__
#define __LFO_H__

#include <SpiralCore/Misc.h>
#include <SpiralCore/Port.h>
#include <SpiralCore/Device.h>
#include <SpiralCore/Patch.h>

using namespace Spiral;

class LFO : Gumbo(Device) 
{
  GumboClassDefinition(
    LFO, Device,
    {
      mVersion = 1;

      mTable = Array<Sample*>::New();
      WriteWaves();
    },

    {
      for (UnsignedType i=0; i < mTable->Count(); i++)
        UnReference((*mTable)[i]);
      
      UnReference(mTable);
    },

  private:
    Array<Sample *> *mTable;

    void WriteWaves();
                       
  public:
    enum WaveType
    {
      SineWave=0, 
      TriangleWave, 
      SquareWave, 
      SawWave
    };

    inline const Sample *Table(UnsignedType aType) const
    {
      return (*mTable)[aType];
    }
  ); 

private:
  /* Instance State */
  OutputPort *output[3];

  FloatType AdjustPos (FloatType pos);

  // Voice Specific Properties (index)
  UnsignedType m_CyclePosInd;

  // Common Shared Properties
  SetProperty *m_Type;
  FloatProperty *m_Freq;

  UnsignedType    m_TableLength;	

public:
  virtual LFO *Initialize(Patch *Host);
  static inline LFO *New(Patch *Host) { return Alloc()->Initialize(Host); }
  bool CreatePorts();

  virtual void Process(UnsignedType SampleCount);
  virtual void Reset();

  void WriteWaves();
  void NoteTrigger(int V,int s,int v);
};

#endif
