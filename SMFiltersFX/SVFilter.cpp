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

#include "SVFilter.h"
//#include "SpiralIcon.xpm"

using namespace Spicy;
using namespace Spiral;

static const FloatType PI = 3.141592654;
static const int GRANULARITY = 10;

DevicePluginHook(SVFilter, SVFilterID)

///////////////////////////////////////////////////////

const FloatType zero = 0.0; 
const FloatType one = 1.0; 
const FloatType two = 2.0; 
const FloatType three = 3.0; 
const FloatType four = 4.0; 
const FloatType five = 5.0; 
const FloatType six = 6.0; 

SVFilter *SVFilter::Initialize(Patch *Host)
{
  Super::Initialize(Host);
  
  Cutoff = FloatProperty::New(DefaultLinearFlags, zero, zero, one, 0.0001, 0.001);
  Resonance = FloatProperty::New(DefaultLinearFlags, zero, zero, one, 0.00001, 0.0001);
  
  RegisterSharedProperty(Cutoff, StringHash("CUTOFF")/*"Cutoff", "Cutoff"*/);
  RegisterSharedProperty(Resonance, StringHash("RESONANCE")/*"Resonance", "Resonance"*/);
  
  /* These should be internal state properties */
  fc = 1000.0f;
  q = zero;
  m_f = zero;
  m_q = zero;
  m_qnrm = zero;
  m_h = zero;
  m_b = zero; 
  m_l = zero;
  m_p = zero;
  m_n = zero;

  return this;
}

bool SVFilter::CreatePorts()
{
  InputPort::New(this/*, "Input"*/);	

  OutputPort::New(this/*,  "LowPass output"*/);
  OutputPort::New(this/*,  "BandPass output"*/);
  OutputPort::New(this/*,  "HighPass output"*/);
  OutputPort::New(this/*,  "Notch output"*/);
  OutputPort::New(this/*,  "Peaking output"*/);

  /* These should be Control Ports, i.e., autocreated by the properties they are for */
  InputPort::New(this/*, "Cutoff CV"*/);	
  InputPort::New(this/*, "Emphasis CV"*/);	

  return true;
}

void SVFilter::Reset()
{
	Device::Reset();
	m_h=zero;
	m_b=zero;
	m_l=zero;
	m_p=zero;
	m_n=zero;
}

void SVFilter::Process(UnsignedType SampleCount)
{
	FloatType in, cut, res;

	cut = Cutoff->Value.AsFloat;
	res = Resonance->Value.AsFloat;

    for (UnsignedType n=0; n<SampleCount; n++)
	{
		if (n%GRANULARITY==0)
		{
			FloatType cv1, cv2;

			cv1 = GetInput(GetInputPort(1),n);
			if (NUMBER_IS_INSANE(cv1))
				cv1 = zero;

			fc = 4000.0f*(cut+cv1);

			cv2 = GetInput(GetInputPort(2),n);
			if (NUMBER_IS_INSANE(cv2))
				cv2=zero;

	 		q  = one-res+cv2;
			m_f = two*sin(PI*fc/(FloatType)(SignedType)SampleRate());
		}

		in = GetInput(GetInputPort(0),n);

		if (NUMBER_IS_INSANE(in))
			in=zero;

		if (in == zero)
		{
			m_h=zero;
			m_b=zero;
			m_l=zero;
			m_p=zero;
			m_n=zero;
		}
		else 
		{				
			FloatType scale=0.5f;

			m_l = m_l + m_f*m_b;
			m_h = scale*in - m_l - q*m_b;
			m_b = m_b + m_f*m_h;
			m_n = m_l + m_h;
			m_p = m_l - m_h;
		}
		
		SetOutput(GetOutputPort(0),n,m_l);
		SetOutput(GetOutputPort(1),n,m_b);
		SetOutput(GetOutputPort(2),n,m_h);
		SetOutput(GetOutputPort(3),n,m_n);
		SetOutput(GetOutputPort(4),n,m_p);
	}

}
