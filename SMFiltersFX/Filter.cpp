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
#include "Filter.h"
//#include "SpiralIcon.xpm"

using namespace std;

static const FloatType MAX_RES = 10.0f;
static const FloatType MIN_RES = 1.0f;
static const FloatType MAX_CUTOFF = 3000.0f;
static const FloatType MIN_CUTOFF = 500.0f;
static const SignedType FILTERGRAN = 50;

DevicePluginHook(Filter, FilterID)

///////////////////////////////////////////////////////

/* Why on earth.... */
const FloatType zero = 0.0f; 
const FloatType one = 1.0f; 
const FloatType two = 2.0f; 
const FloatType three = 3.0f; 
const FloatType four = 4.0f; 
const FloatType five = 5.0f; 
const FloatType six = 6.0f; 

void Filter::Finalize()
{	
  if (iir.coef)
    delete [] iir.coef;
  
  iir.coef = NULL;
  
  if (iir.history)
    delete[] iir.history;
  
  iir.history = NULL;
  
  Super::Finalize();  
}

Filter *Filter::Initialize(Patch *Host)
{
  Super::Initialize(Host);

  iir = FILTER();

  fc = FloatProperty::New(DefaultLinearFlags, 100.0f, zero, 100.0f, 0.01f,  one);
  Q = FloatProperty::New(DefaultLinearFlags, one, zero, 10.0f, 0.01f, 0.1f);
  RegisterSharedProperty(fc, StringHash("CUTOFF") /*"Cutoff", "Cutoff"*/);
  RegisterSharedProperty(Q, StringHash("RESONANCE")/*"Resonance", "Resonance"*/);

  /* These should be internal state properties */
  m_LastFC = zero;
  m_LastQ = one;

  a0 = zero; 
  a1 = zero; 
  a2 = zero; 
  b0 = zero; 
  b1 = zero; 
  b2 = zero; 
  k = one;              // Set overall filter gain

  SetupCoeffs();
  
  iir.length = FILTER_SECTIONS;         	
  iir.coef = new FloatType[4 * iir.length + 1]; 
  
  return this;
}

static const UnsignedType In = StringHash("Input", true);

static const UnsignedType Out = StringHash("Low Pass Output", true);

/* These should be Control Ports, i.e., autocreated by the properties they are for */
static const UnsignedType CutoffCV = StringHash("Cutoff CV", true);	
static const UnsignedType EmphasisCV = StringHash("Emphasis CV", true);	

bool Filter::CreatePorts()
{	
  input[0] = GetInputPort(In);//InputPort::New(this, In);
  output = GetOutputPort(Out);//OutputPort::New(this, Out);

  /* These should be Control Ports, i.e., autocreated by the properties they are for */
  input[1] = GetInputPort(CutoffCV);//InputPort::New(this, CutoffCV);
  input[2] = GetInputPort(EmphasisCV);//InputPort::New(this, EmphasisCV);
  
  return true;
}

void Filter::Reset()
{
	Device::Reset();

	SetupCoeffs();
}

void Filter::Process(UnsignedType SampleCount)
{
	FloatType Cutoff, _fc, _q;
	FloatType Resonance;
	FloatType in, cv1, cv2;

	_fc=fc->Value.AsFloat;
	_q=Q->Value.AsFloat;

	if (_fc<zero)	return;

	if (! InputExists(input[0])) 
	{	
		GetOutputBuffer(output)->Zero();

		return;
	}
   
	for (UnsignedType n=0; n<SampleCount; n++)
	{
		in = GetInput(input[0],n);
		

		if (NUMBER_IS_INSANE(in))
			in = zero;

		coef = iir.coef + 1;     /* Skip k, or gain */ 
		k=0.25f;

		cv1 = GetInput(input[1],n);
		if (NUMBER_IS_INSANE(cv1))
			cv1 = zero;

		Cutoff = _fc + (cv1 * 1000.0f);

		cv2 = GetInput(input[2],n);
		if (NUMBER_IS_INSANE(cv2))
			cv2 = zero;

		Resonance = _q + cv2;  
 	
		Cutoff/=two;
 
		if (Resonance>MAX_RES) Resonance=MAX_RES; 
		if (Cutoff>MAX_CUTOFF) Cutoff=MAX_CUTOFF; 
		if (Resonance<MIN_RES) Resonance=MIN_RES; 
		if (Cutoff<MIN_CUTOFF) Cutoff=MIN_CUTOFF; 
 
		if (n%FILTERGRAN==0)
		{
			for (nInd = 0; nInd < iir.length; nInd++) 
			{
				a2 = ProtoCoef[nInd].a2; 
	
				a0 = ProtoCoef[nInd].a0; 
				a1 = ProtoCoef[nInd].a1; 
          
				b0 = ProtoCoef[nInd].b0; 
				b1 = ProtoCoef[nInd].b1 / Resonance;      
				b2 = ProtoCoef[nInd].b2; 	
				szxform(&a0, &a1, &a2, &b0, &b1, &b2, Cutoff*(Cutoff/1000.0f), (SignedType)SampleRate(), &k, coef); 
				coef += 4;                       

				iir.coef[0] = k; 
			
				m_LastQ=_q;
				m_LastFC=_fc;			
			}
		}		  
	 		 
	 	SetOutput(output,n,iir_filter(in/0.5f,&iir));
	}		
}

void Filter::SetupCoeffs()
{
	ProtoCoef[0].a0 = one; 
	ProtoCoef[0].a1 = zero; 
	ProtoCoef[0].a2 = zero; 
	ProtoCoef[0].b0 = one; 
	ProtoCoef[0].b1 = 0.765367f; 
	ProtoCoef[0].b2 = one; 

	ProtoCoef[1].a0 = one; 
	ProtoCoef[1].a1 = zero; 
	ProtoCoef[1].a2 = zero; 
	ProtoCoef[1].b0 = one; 
	ProtoCoef[1].b1 = 1.847759f; 
	ProtoCoef[1].b2 = one; 
} 	
