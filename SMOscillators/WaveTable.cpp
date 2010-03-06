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
#include <cmath>
#include "WaveTable.h"

static const unsigned int NUM_TABLES = 8;
static const unsigned int DEFAULT_TABLE_LEN = 1024;

static const int IN_FREQ  = 0;
static const int IN_PW    = 1;
static const int IN_SHLEN = 2;

//WaveTable Was initially commited by Dave, Sun Jul 28 23:18:17 2002 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1027916297::WaveTable"
//  => d7a6e545474f4fc1c5f6d2acedc28cd7 (legacy == 17)
DevicePluginHook(WaveTable, d7a6e545474f4fc1c5f6d2acedc28cd7)

void WaveTable::Class::WriteWaves()
{
  /* Food for thought - 
   How could this be pre-generated,
   even by template magic */
  
  for (UnsignedType n=0; n<NUM_TABLES; n++)
    mTable->Append(Sample::New(DEFAULT_TABLE_LEN));

  FloatType RadCycle = (FloatType)M_PI*2;
  FloatType Pos=0, v=0;
  FloatType HalfTab=DEFAULT_TABLE_LEN/2;
  
  for (UnsignedType n=0; n<DEFAULT_TABLE_LEN; n++)
  {
    if (n==0) Pos=0;
    else Pos=(n/(FloatType)DEFAULT_TABLE_LEN)*RadCycle;
    (*mTable)[SineWave]->Set(n,std::sin(Pos));		
    
    if (n<HalfTab) 
    {
      (*mTable)[SquareWave]->Set(n,1.0f);
      v=1-(n/HalfTab)*2.0f;
    }
    else 
    {
      (*mTable)[SquareWave]->Set(n,-1);				
      v=(((n-HalfTab)/HalfTab)*2.0f)-1.0f;
    }
    
    (*mTable)[ReverseSawWave]->Set(n,((n/(FloatType)DEFAULT_TABLE_LEN)*2.0f)-1.0f);
    
    (*mTable)[SawWave]->Set(n,1-(n/(FloatType)DEFAULT_TABLE_LEN)*2.0f);
    
    (*mTable)[TriangleWave]->Set(n,v*0.99f);		
    if (n<DEFAULT_TABLE_LEN/1.2) 
      (*mTable)[Pulse1Wave]->Set(n,1);
    else 
      (*mTable)[Pulse1Wave]->Set(n,-1);				
    
    if (n<DEFAULT_TABLE_LEN/1.5) 
      (*mTable)[Pulse2Wave]->Set(n,1);
    else 
      (*mTable)[Pulse2Wave]->Set(n,-1);				
  }
  
  Pos=0;
  for (UnsignedType n=0; n<DEFAULT_TABLE_LEN; n++)
  {
    if (n==0) Pos=0;
    else Pos=(n/(FloatType)DEFAULT_TABLE_LEN)*RadCycle;
    if (sin(Pos)==0) (*mTable)[InverseSineWave]->Set(n,0);
    else (*mTable)[InverseSineWave]->Set(n,(1.0f/std::sin(Pos))/10.0f);				
  }
}

const NumericPropertyValue WaveTypes[] = 
{
  /*{"Sine", */DefaultUnsigned(WaveTable::Class::SineWave)/*}*/,
	/*{"Square", */DefaultUnsigned(WaveTable::Class::SquareWave)/*}*/,
	/*{"Saw", */DefaultUnsigned(WaveTable::Class::SawWave)/*}*/,
	/*{"Reverse Saw", */DefaultUnsigned(WaveTable::Class::ReverseSawWave)/*}*/,
	/*{"Triangle", */DefaultUnsigned(WaveTable::Class::TriangleWave)/*}*/,
	/*{"Pulse 1", */DefaultUnsigned(WaveTable::Class::Pulse1Wave)/*}*/,
	/*{"Pulse 2", */DefaultUnsigned(WaveTable::Class::Pulse2Wave)/*}*/,
	/*{"Inverse Sine", */DefaultUnsigned(WaveTable::Class::InverseSineWave)/*}*/
};

const NumericPropertyValue defCyclePos = DefaultFloat(0.0f);
const NumericPropertyValue defNoteInd = DefaultSigned(0);

WaveTable *WaveTable::Initialize(Patch *Host)
{
  Super::Initialize(Host);

  /* Shared Properties */
  m_Type = SetProperty::New(Property::WriteOnly, 0, PropertySet::New(WaveTypes, sizeof(WaveTypes)/sizeof(WaveTypes[0])));
  m_Octave = SignedProperty::New(DefaultLinearFlags,0,-3,3,1,1);
  m_FineFreq = FloatProperty::New(DefaultLinearFlags,1, 0, 1.414f, 0.000001f, 0.0001f);
  m_ModAmount = FloatProperty::New(DefaultLinearFlags, 1.0f, 0, 2.0f, 0.001f, 0.01f);

  /* Voice State Properties */
  m_CyclePosInd = NewStateProperty(defCyclePos);
  m_NoteInd = NewStateProperty(defNoteInd);

  RegisterSharedProperty(m_Type, StringHash("WAVE TYPE")/*"Wave Type", "Wave Type"*/);
  RegisterSharedProperty(m_Octave, StringHash("OCTAVE")/*"Octave", "Octave"*/);
  RegisterSharedProperty(m_FineFreq, StringHash("FINEFREQ")/*"FineFreq", "FineFreq"*/);
  RegisterSharedProperty(m_ModAmount, StringHash("MODAMOUNT")/*"ModAmount", "ModAmount"*/);
  
  return this;
}

static const UnsignedType FrequencyCV = StringHash("Frequency CV", true);
static const UnsignedType Out = StringHash("Output", true);

bool WaveTable::CreatePorts()
{
  frequency = GetInputPort(FrequencyCV);
  output = GetOutputPort(Out);
	
  return true;
}

void WaveTable::Process(UnsignedType SampleCount)
{
  const Sample *table;
  
  UnsignedType octave, type, Note;
  FloatType Freq=0, fine, mod;
  FloatType Incr, CyclePos;

  type = m_Type->Value.AsUnsigned;
  octave = m_Octave->Value.AsSigned;
  fine = m_FineFreq->Value.AsFloat;
  mod = m_ModAmount->Value.AsFloat;

  table = ClassObject()->Table(type);

  CyclePos = StateValue(m_CyclePosInd)->AsFloat;
  Note = StateValue(m_NoteInd)->AsSigned;

  for (UnsignedType n=0; n<SampleCount; n++)
  {	
    output->SetSampleValue(n, 0, GetVoice());

    if (frequency->IsConnected()) 
    {
      Freq=0;

      if (!NUMBER_IS_HUGE(GetInput(frequency, n)))
        Freq=frequency->GetSamplePitch(n, GetVoice());

      Freq*=mod;
    }
    else 
    {	
        Freq=110;
    }

    Freq*=fine;
    if (octave>0) Freq*=1<<(octave);
    if (octave<0) Freq/=1<<(-octave);

    Incr = Freq*(DEFAULT_TABLE_LEN/(FloatType)SampleRate());
    CyclePos+=Incr;

    if (CyclePos > DEFAULT_TABLE_LEN)
      CyclePos = ((UnsignedType)floor(CyclePos) % DEFAULT_TABLE_LEN) + (CyclePos - std::floor(CyclePos));

    CyclePos = MAX(CyclePos, 0);

    SetOutput(output, n, (*table)[CyclePos]);
  }

  StateValue(m_CyclePosInd)->AsFloat = CyclePos;
  StateValue(m_NoteInd)->AsSigned = Note;
}
