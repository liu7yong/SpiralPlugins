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

//Initially commited by Dave, Sun Jul 28 23:18:15 2002 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1027916292::MoogFilter"
#define device_id d668f07998c4e09b149ae1570d2143cc// legacy == D
#define device_version 1

DevicePluginHook(MoogFilter, device_id, device_version)

/*const DeviceDescription MoogFilter::mDescription = 
{
  UniqueID       : MoogFilter::mUniqueID,
  AudioDriver    : false,
  HostPlugin     : false,

  Author         : "David Griffiths",
  Version        : 1,
  Label          : "Moog Filter",
  Info           : "Moog Filter",
  Category       : "Filters/FX",
  PluginInstance : DevicePluginHookName(device_id)
};*/

///////////////////////////////////////////////////////

const FloatType zero = 0.0; 
const FloatType one = 1.0; 
const FloatType two = 2.0; 
const FloatType three = 3.0; 
const FloatType four = 4.0; 
const FloatType five = 5.0; 
const FloatType six = 6.0; 

MoogFilter::MoogFilter(Patch *Host) :
	Device(Host),
	Cutoff(new FloatProperty(DefaultLinearFlags,0.5f, zero, one, 0.0001, 0.001)),
	Resonance(new FloatProperty(DefaultLinearFlags,0.0f, zero, one, 0.00001, 0.0001)),

	fc(1000.0f),
	f(zero),
	p(zero),
	q(zero),
	b0(0.1f),
	b1(zero),
	b2(zero),
	b3(zero),
	b4(zero),
	t1(zero),
	t2(zero)
{
	RegisterSharedProperty(Cutoff, StringHash("CUTOFF")/*"Cutoff", "Cutoff"*/);
	RegisterSharedProperty(Resonance, StringHash("RESONANCE")/*"Resonance", "Resonance"*/);
}

bool MoogFilter::CreatePorts()
{	
	input[0] = new InputPort(this/*, "Input"*/);	
	input[1] = new InputPort(this/*, "Cutoff CV"*/);	
	input[2] = new InputPort(this/*, "Emphasis CV"*/);	

	output[0] = new OutputPort(this/*, "LowPass output"*/);
	output[1] = new OutputPort(this/*, "BandPass output"*/);
	output[2] = new OutputPort(this/*, "HighPass output"*/);

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

			fc = MAX((cut+in1)*0.25, zero);
			
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
