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

using namespace Spicy;
using namespace Spiral;

static const FloatType PI = 3.141592654f;
static const int GRANULARITY = 10;

//Initially commited by Dave, Sun Jul 28 23:18:15 2002 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1027916292::SVFilter"
//  => ba7dbd3d0088109861334e1787616b2e (legacy == C)
DevicePluginHook(SVFilter, ba7dbd3d0088109861334e1787616b2e)

///////////////////////////////////////////////////////

const FloatType zero = 0.0f; 
const FloatType one = 1.0f; 
const FloatType two = 2.0f; 
const FloatType three = 3.0f; 
const FloatType four = 4.0f; 
const FloatType five = 5.0f; 
const FloatType six = 6.0f; 

SVFilter *SVFilter::Initialize(Patch *Host)
{
  Super::Initialize(Host);
  
  mCutoff = NumberProperty<FloatType>::New(Property::WriteOnly, 0.0f, 
                                          LinearConstraints<FloatType>::New(true, true, false, 0.0f, 1.0f, 0.0001f, 0.001f));
  mResonance = NumberProperty<FloatType>::New(Property::WriteOnly, 0.0f, 
                                             LinearConstraints<FloatType>::New(true, true, false, 0.0f, 1.0f, 0.00001f, 0.0001f));
  
#if 0
  RegisterSharedProperty(mCutoff, StringHash("Cutoff", true)/*"Cutoff", "Cutoff"*/);
  RegisterSharedProperty(mResonance, StringHash("Resonance", true)/*"Resonance", "Resonance"*/);
#endif
  
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

static const UnsignedType In = StringHash("Input", true);

static const UnsignedType LowPassOut = StringHash("Low Pass Output", true);
static const UnsignedType BandPassOut = StringHash("Band Pass Output", true);
static const UnsignedType HighPassOut = StringHash("High Pass Output", true);
static const UnsignedType NotchOut = StringHash("Notch Output", true);
static const UnsignedType PeakingOut = StringHash("Peaking Output", true);

/* These should be Control Ports, i.e., autocreated by the properties they are for */
static const UnsignedType CutoffCV = StringHash("Cutoff CV", true);	
static const UnsignedType EmphasisCV = StringHash("Emphasis CV", true);	

bool SVFilter::CreatePorts()
{
  /*
  InputPort::New(this, In);	

  OutputPort::New(this, LowPassOut);
  OutputPort::New(this, BandPassOut);
  OutputPort::New(this, HighPassOut);
  OutputPort::New(this, NotchOut);
  OutputPort::New(this, PeakingOut);
*/
  /* These should be Control Ports, i.e., autocreated by the properties they are for */
//  InputPort::New(this, CutoffCV);	
//  InputPort::New(this, EmphasisCV);	

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

	cut = mCutoff->Value();
	res = mResonance->Value();

    for (UnsignedType n=0; n<SampleCount; n++)
	{
		if (n%GRANULARITY==0)
		{
			FloatType cv1, cv2;

			cv1 = GetInput(GetInputPort(CutoffCV),n);
			if (NUMBER_IS_INSANE(cv1))
				cv1 = zero;

			fc = 4000.0f*(cut+cv1);

			cv2 = GetInput(GetInputPort(EmphasisCV),n);
			if (NUMBER_IS_INSANE(cv2))
				cv2=zero;

	 		q  = one-res+cv2;
          m_f = two*std::sin(PI*fc/(FloatType)(SignedType)SampleRate());
		}

		in = GetInput(GetInputPort(In),n);

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
		
		SetOutput(GetOutputPort(LowPassOut),n,m_l);
		SetOutput(GetOutputPort(HighPassOut),n,m_b);
		SetOutput(GetOutputPort(BandPassOut),n,m_h);
		SetOutput(GetOutputPort(NotchOut),n,m_n);
		SetOutput(GetOutputPort(PeakingOut),n,m_p);
	}

}
