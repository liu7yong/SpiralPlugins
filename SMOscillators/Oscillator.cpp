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
#include "Oscillator.h"

//Initially commited by Dave, Sun Jul 28 23:18:15 2002 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1027898295::Oscillator"
//  =>  7f4e8dfa239d37c75b7ecce1a4048da9 (legacy == 0x4)
DevicePluginHook(Oscillator, 7f4e8dfa239d37c75b7ecce1a4048da9)

///////////////////////////////////////////////////////
enum {
  SquareWave=1,
  SawWave=2,
  NoiseWave=3
};

const UnsignedType WaveTypes[] = 
{
	/*{"Square", */SquareWave/*}*/,
	/*{"Saw", */SawWave/*}*/,
	/*{"Noise", */NoiseWave/*}*/
};

void Oscillator::Finalize()
{
  UnReference(m_Type);
  UnReference(m_Octave);
  UnReference(m_FineFreq);
  UnReference(m_PulseWidth);
  UnReference(m_ModAmount);
  UnReference(m_SHLen);

  Super::Finalize();
}

const NumericPropertyValue defCyclePos = DefaultSigned(0);
const NumericPropertyValue defNoisev = DefaultSigned(0);

Oscillator *Oscillator::Initialize(Patch *Host)
{
  Super::Initialize(Host);

  /* Shared Properties */
  m_Type = NumberProperty<UnsignedType>::New(Property::WriteOnly, SquareWave, 
                                              SetConstraints<UnsignedType>::New(WaveTypes, sizeof(WaveTypes)/sizeof(WaveTypes[0])));
  m_Octave = NumberProperty<UnsignedType>::New(Property::WriteOnly,0,
                                               LinearConstraints<UnsignedType>::New(true, true, false, 0,6,1,1));
  m_FineFreq = NumberProperty<FloatType>::New(Property::WriteOnly, 1, 
                                                 LinearConstraints<FloatType>::New(true, true, false, 0, 1.414f, 0.000001f, 0.0001f));
  m_PulseWidth = NumberProperty<FloatType>::New(Property::WriteOnly, 0.5f, 
                                                LinearConstraints<FloatType>::New(true, true, false, 0, 1, 0.01f, 0.1f));
  m_ModAmount = NumberProperty<FloatType>::New(Property::WriteOnly, 1.0f, 
                                               LinearConstraints<FloatType>::New(true, true, false, 0, 2.0f, 0.001f, 0.01f));
  m_SHLen = NumberProperty<FloatType>::New(Property::WriteOnly, 0.1f, 
                                           LinearConstraints<FloatType>::New(true, true, false, 0, 0.2f, 0.001f, 0.01f));

#if 0
  RegisterSharedProperty(m_Type, StringHash("Wave Type")/*, "Wave Type"*/);
  RegisterSharedProperty(m_Octave, StringHash("Octave")/*, "Octave"*/);
  RegisterSharedProperty(m_FineFreq, StringHash("Fine Tune")/*, "Fine Tune Frequency"*/);
  RegisterSharedProperty(m_PulseWidth, StringHash("Pulse Width")/*, "Pulse Width"*/);
  RegisterSharedProperty(m_ModAmount, StringHash("ModAmount")/*, "ModAmount"*/);
  RegisterSharedProperty(m_SHLen, StringHash("Sample & Hold Length")/*, "Sample & Hold Length"*/);
#endif
  /* Voice State Properties */
  m_CyclePosInd = NewStateProperty(defCyclePos);
  m_NoisevInd = NewStateProperty(defNoisev);

  return this;
}

static const UnsignedType FrequencyCV = StringHash("Frequency CV", true);
static const UnsignedType PulseWidthCV = StringHash("PulseWidth CV", true);
static const UnsignedType SampleHoldCV = StringHash("Sample & Hold Length CV", true);

static const UnsignedType Out = StringHash("Output", true);

bool Oscillator::CreatePorts() 
{
  return true;
}

void Oscillator::CreateProperty(UnsignedType aPropertyID, Property *aProperty)
{
  if (aPropertyID == StringHash("Wave Type", true))
  {
    m_Type =  NumberProperty<UnsignedType>::New(aProperty);
    return;
  }

  if (aPropertyID == StringHash("Octave", true))
  {
    m_Octave =  NumberProperty<UnsignedType>::New(aProperty);
    return;
  }

  if (aPropertyID == StringHash("Fine Tune", true))
  {
    m_FineFreq =  NumberProperty<FloatType>::New(aProperty);
    return;
  }

  if (aPropertyID == StringHash("Pulse Width", true))
  {
    m_PulseWidth =  NumberProperty<FloatType>::New(aProperty);
    return;
  }

  if (aPropertyID == StringHash("Mod Amount", true))
  {
    m_ModAmount =  NumberProperty<FloatType>::New(aProperty);
    return;
  }

  if (aPropertyID == StringHash("Sample & Hold Length", true))
  {
    m_ModAmount =  NumberProperty<FloatType>::New(aProperty);
    return;
  }
}

void Oscillator::Process(UnsignedType SampleCount)
{
	FloatType Freq=0, fine, mod, pulsewidth, shlen;
	FloatType CycleLen=0;
	SignedType CyclePos, Noisev;
	SignedType samplelen, PW, octave;

	octave = m_Octave->Value();
	fine = m_FineFreq->Value();
	mod = m_ModAmount->Value();
	pulsewidth = m_PulseWidth->Value();
	shlen = m_SHLen->Value();

	CyclePos = StateValue(m_CyclePosInd)->AsSigned;
	Noisev = StateValue(m_NoisevInd)->AsSigned;

	switch (m_Type->Value())
	{
	case SquareWave:
		for (UnsignedType n=0; n<SampleCount; n++)
		{
			if (InputExists(GetInputPort(FrequencyCV))) 
			  Freq=GetInputPitch(GetInputPort(FrequencyCV),n);
			else 
			  Freq=110;
			  
			Freq*=fine;
			
			if (octave>0) Freq*=1<<(octave);
			if (octave<0) Freq/=1<<(-octave);
			
			CycleLen = SampleRate()/Freq;
			PW = (int)((pulsewidth+GetInput(GetInputPort(PulseWidthCV),n)*mod) * CycleLen);

			// calculate square wave pattern
			CyclePos++;
			if (CyclePos>CycleLen) CyclePos=0;

			if (CyclePos<PW) SetOutput(GetOutputPort(Out),n,1);
			else SetOutput(GetOutputPort(Out),n,-1);
		}
		break;

	case SawWave:
		for (UnsignedType n=0; n<SampleCount; n++)
		{
			if (InputExists(GetInputPort(FrequencyCV))) Freq=GetInputPitch(GetInputPort(FrequencyCV),n);
			else Freq=110;
			Freq*=fine;
			if (octave>0) Freq*=1<<(octave);
			if (octave<0) Freq/=1<<(-octave);
			CycleLen = SampleRate()/Freq;
			PW = (int)((pulsewidth+GetInput(GetInputPort(PulseWidthCV),n)*mod) * CycleLen);

			// normalise position between cycle
			CyclePos++;
			if (CyclePos>CycleLen) CyclePos=0;

			if (CyclePos<PW)
			{
				// before pw -1->1
				SetOutput(GetOutputPort(Out),n,Linear(0,PW,CyclePos,-1,1));
			}
			else
			{
				// after pw 1->-1
				SetOutput(GetOutputPort(Out),n,Linear(PW,CycleLen,CyclePos,1,-1));
			}
		}
		break;

	case NoiseWave:
		for (UnsignedType n=0; n<SampleCount; n++)
		{
			CyclePos++;

			//modulate the sample & hold length
			samplelen = (int)((shlen+GetInput(GetInputPort(SampleHoldCV),n)*mod)*SampleRate());

			// do sample & hold on the noise
			if (CyclePos>samplelen)
			{
				Noisev=(short)((rand()%SHRT_MAX*2)-SHRT_MAX);
				CyclePos=0;
			}
			SetOutput(GetOutputPort(Out),n,Noisev/(FloatType)SHRT_MAX);
		}
		break;
	}

	StateValue(m_CyclePosInd)->AsSigned = CyclePos;
	StateValue(m_NoisevInd)->AsSigned = Noisev;
}
