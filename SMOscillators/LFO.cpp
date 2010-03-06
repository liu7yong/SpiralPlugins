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

#include <math.h>
#include "LFO.h"

//LFO Was initially commited by Dave, Sun Jul 28 23:18:18 2002 UTC
//md5 -s "Andy Preston::andy@clubunix.co.uk::1027916298::LFO"
//  =>  09f642cbfdbb5022d229b3a44c343ce0 (legacy == 7C)
DevicePluginHook(LFO, 09f642cbfdbb5022d229b3a44c343ce0)

static const UnsignedType NUM_TABLES = 6;
static const UnsignedType DEFAULT_TABLE_LEN = 1024;

void LFO::Class::WriteWaves()
{
  /* Food for thought - 
   How could this be pre-generated,
   even by template magic */
  
  for (UnsignedType n=0; n<NUM_TABLES; n++)
    mTable->Append(Sample::New(DEFAULT_TABLE_LEN));
  
  FloatType RadCycle = (FloatType)(M_PI/180)*360;
  FloatType Pos = 0;
  FloatType v = 0;
  UnsignedType HalfTab = Math::Divide<DEFAULT_TABLE_LEN, 2>::Result;
  UnsignedType QuatTab = Math::Divide<DEFAULT_TABLE_LEN, 4>::Result;
  UnsignedType ThreeQuatTab = Math::Subtract<DEFAULT_TABLE_LEN, Math::Divide<DEFAULT_TABLE_LEN, 4>::Result>::Result;
  SignedType Shift;
  
  for (UnsignedType n=0; n<DEFAULT_TABLE_LEN; n++) 
  {
    if (n==0) 
      Pos = 0; 
    else 
      Pos = (n/(FloatType)DEFAULT_TABLE_LEN) * RadCycle;
    
    (*mTable)[SineWave]->Set (n, std::sin (Pos));
    
    if (n < QuatTab) 
      Shift=n+ThreeQuatTab; 
    else 
      Shift=n-QuatTab;
    
    if (n<QuatTab || n>ThreeQuatTab) 
      v = (((Shift-HalfTab) / HalfTab) * 2) - 1;
    else 
      v = 1 - (Shift/HalfTab * 2);
    
    (*mTable)[TriangleWave]->Set (n, v);
    
    if (n<DEFAULT_TABLE_LEN/2) 
      (*mTable)[SquareWave]->Set (n, 1.0f);
    else 
      (*mTable)[SquareWave]->Set (n, -1);
    
    (*mTable)[SawWave]->Set (n, 1.0f - (n / (FloatType)DEFAULT_TABLE_LEN) * 2.0f);
  }
}

///////////////////////////////////////////////////////
const NumericPropertyValue WaveTypes[] = 
{
	/*{"Sine", */ DefaultUnsigned(LFO::Class::SineWave)/*}*/,
	/*{"Triangle", */ DefaultUnsigned(LFO::Class::TriangleWave)/*}*/,
	/*{"Square", */ DefaultUnsigned(LFO::Class::SquareWave)/*}*/,
	/*{"Saw", */ DefaultUnsigned(LFO::Class::SawWave)/*}*/
};

const NumericPropertyValue defCyclePos= DefaultFloat(0.0f);

LFO *LFO::Initialize(Patch *Host)
{
  Super::Initialize(Host);

  m_TableLength = DEFAULT_TABLE_LEN;

  /* Shared Properties */
  m_Type = SetProperty::New(Property::WriteOnly, 0, PropertySet::New(WaveTypes, sizeof(WaveTypes)/sizeof(WaveTypes[0])));
  m_Freq = FloatProperty::New(DefaultLinearFlags,0.1f, 0.0f, 1.0f, 0.001f, 0.1f);

  RegisterSharedProperty(m_Type, StringHash("WAVE TABLE")/*"Wave Type", "Wave Type"*/);
  RegisterSharedProperty(m_Freq, StringHash("FREQUENCY")/*"Frequency", "Frequency"*/);

  /* Voice State Properties */
  m_CyclePosInd = NewStateProperty(defCyclePos);
  
  return this;
}

static const UnsignedType Out = StringHash("Output", true);
static const UnsignedType CosOut = StringHash("Cosine Output", true);
static const UnsignedType InvOut = StringHash("Inverted Output", true);

bool LFO::CreatePorts() 
{
  output[0] = GetOutputPort(Out);
  output[1] = GetOutputPort(CosOut);
  output[2] = GetOutputPort(InvOut);

  return true;
}

FloatType LFO::AdjustPos (FloatType pos) 
{
	if (pos > DEFAULT_TABLE_LEN)
      pos = ((UnsignedType)floor(pos) % DEFAULT_TABLE_LEN) + (pos - std::floor(pos));

	pos = MAX(pos, 0);
    
	return pos;
}

void LFO::Reset()
{
	Device::Reset();
}

void LFO::Process(UnsignedType SampleCount) 
{
	UnsignedType type = m_Type->Value.AsUnsigned;
	FloatType freq =m_Freq->Value.AsFloat;
	FloatType Incr, CyclePos, Pos;

	CyclePos = StateValue(m_CyclePosInd)->AsFloat;

	for (UnsignedType n=0; n<SampleCount; n++) 
	{
		Incr = freq * (DEFAULT_TABLE_LEN / (FloatType)SampleRate());

		// Raw Output
		CyclePos = AdjustPos (CyclePos + Incr);
		SetOutput (output[0], n, (*ClassObject()->Table(type))[CyclePos]);

		// 'Cosine' Output
		Pos = AdjustPos (CyclePos + (m_TableLength * 0.25f));
		SetOutput (output[1], n, (*ClassObject()->Table(type))[Pos]);

		// Inverted Output
		Pos = AdjustPos (m_TableLength - CyclePos);
		SetOutput (output[2], n, (*ClassObject()->Table(type))[Pos]);
	}
}

