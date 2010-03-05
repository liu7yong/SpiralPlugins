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
#include <SpiralCore/Misc.h>

#include "MoogFilter.h"
//#include "SpiralIcon.xpm"

#define PI 3.141592654

using namespace std;
using namespace Spicy;
using namespace Spiral;

static const int GRANULARITY = 10;

DevicePluginHook(MoogFilter, MoogFilterID)

///////////////////////////////////////////////////////
// ...............????????????
const FloatType zero = 0.0f; 
const FloatType one = 1.0f; 
const FloatType two = 2.0f; 
const FloatType three = 3.0f; 
const FloatType four = 4.0f; 
const FloatType five = 5.0f; 
const FloatType six = 6.0f; 

MoogFilter *MoogFilter::Initialize(Patch *Host)
{
  Super::Initialize(Host),
  
  Cutoff = FloatProperty::New(DefaultLinearFlags,0.5f, zero, one, 0.0001f, 0.001f);
  Resonance = FloatProperty::New(DefaultLinearFlags,0.0f, zero, one, 0.00001f, 0.0001f);
  RegisterSharedProperty(Cutoff, StringHash("CUTOFF")/*"Cutoff", "Cutoff"*/);
  RegisterSharedProperty(Resonance, StringHash("RESONANCE")/*"Resonance", "Resonance"*/);

  /* These should be internal state properties */
  fc = 1000.0f;
  f = zero;
  p = zero;
  q = zero;
  b0 = 0.1f;
  b1 = zero;
  b2 = zero;
  b3 = zero;
  b4 = zero;
  t1 = zero;
  t2 = zero;

  return this;
}

bool MoogFilter::CreatePorts()
{	
  input[0] = InputPort::New(this/*, "Input"*/);	

  output[0] = OutputPort::New(this/*, "LowPass output"*/);
  output[1] = OutputPort::New(this/*, "BandPass output"*/);
  output[2] = OutputPort::New(this/*, "HighPass output"*/);

  /* These should be Control Ports, i.e., autocreated by the properties they are for */
  input[1] = InputPort::New(this/*, "Cutoff CV"*/);	
  input[2] = InputPort::New(this/*, "Emphasis CV"*/);	

  return true;
}

void MoogFilter::Reset()
{
	Device::Reset();

	WipeState();
}

void MoogFilter::Process(UnsignedType SampleCount)
{
	FloatType in, Q, res, cut;
	
	// Early out?	
	// We don't want to early out if the resonance is set to
	// self oscillation, as this turns into a generator
	// in it's own right...
	/*if (Resonance<0.5 && !GetInput(2))
	{
		if (!m_Input[0] || m_Input[0]->IsEmpty())
		{
			cerr<<"moog early out"<<endl;
		
			m_Output[0]->Zero();
			m_Output[1]->Zero();	
			m_Output[2]->Zero();				
			return;
		}
	}*/
	
	res=Resonance->Value.AsFloat;
	cut=Cutoff->Value.AsFloat;

	for (UnsignedType n=0; n<SampleCount; n++)
	{
		if (n%GRANULARITY==0)
		{
			FloatType in1;
			in1 = GetInput(input[1],n);

			if (NUMBER_IS_INSANE(in1))
				in1 = zero;

			fc = MAX((cut+in1)*0.25f, zero);
			
			q = one - fc;
			p = fc + 0.8f * fc * q;
			f = p + p - one;

			FloatType in2;
			in2 = GetInput(input[2],n);

			if (NUMBER_IS_INSANE(in2))
				in2 = zero;

			Q = ((res+in2)*six)-three;
			q = Q + (one + 0.5f * q * (one - q + 5.6f * q * q));
		}

		in = GetInput(input[0],n);

		if (NUMBER_IS_INSANE(in))
			in = zero;

		if (in == zero)
		{
			WipeState();
			SetOutput(output[0],n,zero);	 
			SetOutput(output[1],n,zero);
			SetOutput(output[2],n,zero);	
		} else {				
			in -= q * b4;
			
			in = CLAMP(in, -one, one);
									
			t1 = b1; b1 = (in + b0) * p - b1 * f;
			t2 = b2; b2 = (b1 + t1) * p - b2 * f;
			t1 = b3; b3 = (b2 + t2) * p - b3 * f;		
     			     b4 = (b3 + t1) * p - b4 * f;	
			b4 = b4 - b4 * b4 * b4 * 0.166667f;
			
			b0 = in;	
			 
			SetOutput(output[0],n,b4);	 
			SetOutput(output[1],n,(in-b4));
			SetOutput(output[2],n,three * (b3 - b4));	
		}
	}			
}

void MoogFilter::WipeState()
{
	b0 = 0.0f;
	b1 = 0.0f;
	b2 = 0.0f;
	b3 = 0.0f;
	b4 = 0.0f;
	t1 = 0.0f;
	t2 = 0.0f;
}
