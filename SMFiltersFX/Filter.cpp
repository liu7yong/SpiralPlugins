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

static const FloatType MAX_RES = 10.0;
static const FloatType MIN_RES = 1.0;
static const FloatType MAX_CUTOFF = 3000.0;
static const FloatType MIN_CUTOFF = 500.0;
static const SignedType FILTERGRAN = 50;

//Initially commited by Dave, Sun Jul 28 23:18:15 2002 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1027916292::Filter"
#define device_id 90ad6dd029d6e71cb86008c489933f5c// legacy == B
#define device_version 1

DevicePluginHook(Filter, device_id, device_version)

/*const DeviceDescription Filter::mDescription = 
{
  UniqueID       : Filter::mUniqueID,
  AudioDriver    : false,
  HostPlugin     : false,

  Author         : "David Griffiths",
  Version        : 1,
  Label          : "Filter",
  Info           : "Filter",
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

Filter::Filter(Patch *Host) :
Device(Host),

iir(),

fc(DefaultLinearFlags, 100.0, zero, 100.0, 0.01,  one),
Q(DefaultLinearFlags, one, zero, 10.0, 0.01, 0.1),

m_LastFC(zero),
m_LastQ(one),

a0(zero), 
a1(zero), 
a2(zero), 
b0(zero), 
b1(zero), 
b2(zero), 
k(one)              // Set overall filter gain
{
	RegisterSharedProperty(fc, "Cutoff", "Cutoff");
	RegisterSharedProperty(Q, "Resonance", "Resonance");
}

bool Filter::Setup()
{
	bool result = Device::Setup();

	if (result)
	{
		SetupCoeffs();

		iir.length = FILTER_SECTIONS;         	
		iir.coef = new FloatType[4 * iir.length + 1]; 
	}

	return result;
}

void Filter::Shutdown()
{	
	Device::Shutdown();

	if (iir.coef)
		delete [] iir.coef;

	iir.coef = NULL;

	if (iir.history)
        	delete[] iir.history;

	iir.history = NULL;
}

bool Filter::CreatePorts()
{	
	input[0] = new InputPort(this, "Input");
	input[1] = new InputPort(this, "Cutoff CV");
	input[2] = new InputPort(this, "Emphasis CV");

	output = new OutputPort(this, "Output");

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

	_fc=fc.Value.AsFloat;
	_q=Q.Value.AsFloat;

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
		k=0.25;

		cv1 = GetInput(input[1],n);
		if (NUMBER_IS_INSANE(cv1))
			cv1 = zero;

		Cutoff = _fc + (cv1 * 1000.0);

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
	ProtoCoef[0].b1 = 0.765367; 
	ProtoCoef[0].b2 = one; 

	ProtoCoef[1].a0 = one; 
	ProtoCoef[1].a1 = zero; 
	ProtoCoef[1].a2 = zero; 
	ProtoCoef[1].b0 = one; 
	ProtoCoef[1].b1 = 1.847759; 
	ProtoCoef[1].b2 = one; 
} 	
