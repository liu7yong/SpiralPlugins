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
#include "AnotherFilter.h"
//#include "SpiralIcon.xpm"

static const FloatType PI = 3.141592654f;

static const UnsignedType GRANULARITY = 10;

//Initially commited by Dave, Thu Jan 2 01:56:01 2003 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1043740561::AnotherFilter"
//  => 420c8ee427e7ae589bc74eb766e66323 (legacy == 2B)
DevicePluginHook(AnotherFilter, 420c8ee427e7ae589bc74eb766e66323)

///////////////////////////////////////////////////////

const NumericPropertyValue zeroFloat = DefaultFloat(0.0f);

AnotherFilter *AnotherFilter::Initialize(Patch *Host)
{
  Super::Initialize(Host),

  mCutoff = NumberProperty<FloatType>::New(Property::WriteOnly, 0.0f, 
                                          LinearConstraints<FloatType>::New(true, true, false, 0.0f, 1.0f, 0.0001f, 0.0001f));
  mResonance = NumberProperty<FloatType>::New(Property::WriteOnly, 0.0f, 
                                             LinearConstraints<FloatType>::New(true, true, false, 0.0f, 1.0f, 0.00001f, 0.00001f));
  
  RegisterSharedProperty(mCutoff, StringHash("Cutoff", true)/*"Cutoff", "Cutoff"*/);
  RegisterSharedProperty(mResonance, StringHash("Resonance", true)/*"Resonance", "Resonance"*/);
  
  /* Voice State Properties */
  m_VibraPosInd = NewStateProperty(zeroFloat);
  m_VibraSpeedInd = NewStateProperty(zeroFloat);
  m_ResonanceInd = NewStateProperty(zeroFloat);
  m_CutoffInd = NewStateProperty(zeroFloat);
  
  return this;
}

static const UnsignedType In = StringHash("Input", true);
static const UnsignedType LowPassOut = StringHash("Low Pass Output", true);

/* These should be Control Ports, i.e., autocreated by the properties they are for */
static const UnsignedType CutoffCV = StringHash("Cutoff CV", true);	
static const UnsignedType EmphasisCV = StringHash("Emphasis CV", true);	

bool AnotherFilter::CreatePorts()
{
  input[0] = GetInputPort(In);
  output[0] = GetOutputPort(LowPassOut);

  /* These should be Control Ports, i.e., autocreated by the properties they are for */
  input[1] = GetInputPort(CutoffCV);
  input[2] = GetInputPort(EmphasisCV);

  return true;
}

void AnotherFilter::Process(UnsignedType SampleCount)
{
  FloatType cut, res, vibrapos, vibraspeed, resonance, cutoff;
          
  cut = mCutoff->Value();		
  res = mResonance->Value();		

  vibrapos = StateValue(m_VibraPosInd)->AsFloat;
  vibraspeed = StateValue(m_VibraSpeedInd)->AsFloat;
  resonance = StateValue(m_ResonanceInd)->AsFloat;
  cutoff = StateValue(m_CutoffInd)->AsFloat;

  for (UnsignedType n=0; n<SampleCount; n++)
  {
    if (n%GRANULARITY==0)
    {
      FloatType w,q;

      w = 2.0f*PI*((cut+GetInput(input[1], n))*10000.0f+1.0f)/SampleRate(); // Pole angle
      q = 1.0f-w/(2.0f*(((res+GetInput(input[2], n))*10.0f+1.0f)+0.5f/(1.0f+w))+w-2.0f); // Pole magnitude
      resonance = q*q;
      cutoff = resonance+1.0f-2.0f*std::cos(w)*q;
    }

    /* Accelerate vibra by signal-vibra, multiplied by lowpasscutoff */
    vibraspeed += (GetInput(input[0], n)*0.3f - vibrapos) * cutoff;

    /* Add velocity to vibra's position */
    vibrapos += vibraspeed;

    /* Attenuate/amplify vibra's velocity by resonance */
    vibraspeed *= resonance;

    // needs clipping :(
    vibrapos = MIN(vibrapos, 1.0f);
    vibrapos = MAX(vibrapos, -1.0f);

    /* Store new value */
    SetOutput(output[0], n, vibrapos);
  }

  StateValue(m_VibraPosInd)->AsFloat = vibrapos;
  StateValue(m_VibraSpeedInd)->AsFloat = vibraspeed;
  StateValue(m_ResonanceInd)->AsFloat = resonance;
  StateValue(m_CutoffInd)->AsFloat = cutoff;
}
