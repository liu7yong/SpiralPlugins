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
#include <math.h>
#include "AnotherFilter.h"
//#include "SpiralIcon.xpm"

#define PI 3.141592654

using namespace std;

static const int GRANULARITY = 10;

//Initially commited by Dave, Thu Jan 2 01:56:01 2003 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1043740561::AnotherFilter"
#define device_id 420c8ee427e7ae589bc74eb766e66323// legacy == 2B
#define device_version 1

DevicePluginHook(AnotherFilter, device_id, device_version)

/*const DeviceDescription AnotherFilter::mDescription = 
{
  UniqueID       : AnotherFilter::mUniqueID,
  AudioDriver    : false,
  HostPlugin     : false,

  Author         : "David Griffiths",
  Version        : 1,
  Label          : "AnotherFilter",
  Info           : "Another Filter",
  Category       : "Filters/FX",
  PluginInstance : DevicePluginHookName(device_id)
};*/

///////////////////////////////////////////////////////

const NumericPropertyValue zeroFloat = DefaultFloat(0.0f);
AnotherFilter::AnotherFilter(Patch *Host) :
	Device(Host),

	Cutoff(DefaultLinearFlags,0.0, 0.0, 1.0, 0.0001, 0.0001),
	Resonance(DefaultLinearFlags,0.0, 0.0, 1.0, 0.00001, 0.00001),

	/* Voice State Properties */
	m_VibraPosInd(NewStateProperty(zeroFloat)),
	m_VibraSpeedInd(NewStateProperty(zeroFloat)),
	m_ResonanceInd(NewStateProperty(zeroFloat)),
	m_CutoffInd(NewStateProperty(zeroFloat))
{
	RegisterSharedProperty(Cutoff, "Cutoff", "Cutoff");
	RegisterSharedProperty(Resonance, "Resonance", "Resonance");
}

bool AnotherFilter::CreatePorts()
{
	input[0] = new InputPort(this, "Input");
	output[0] = new OutputPort(this, "LowPass Output");

	/* These should be Control Ports, i.e., autocreated by the properties they are for */
	input[1] = new InputPort(this, "Cutoff CV");
	input[2] = new InputPort(this, "Emphasis CV");

	return true;
}

void AnotherFilter::Process(UnsignedType SampleCount)
{
	FloatType cut, res, vibrapos, vibraspeed, resonance, cutoff;
			
	cut = Cutoff.Value.AsFloat;		
	res = Resonance.Value.AsFloat;		

	vibrapos = StateValue(m_VibraPosInd)->AsFloat;
	vibraspeed = StateValue(m_VibraSpeedInd)->AsFloat;
	resonance = StateValue(m_ResonanceInd)->AsFloat;
	cutoff = StateValue(m_CutoffInd)->AsFloat;

	for (UnsignedType n=0; n<SampleCount; n++)
	{
		if (n%GRANULARITY==0)
		{
			FloatType w,q;

			w = 2.0*PI*((cut+GetInput(input[1], n))*(FloatType)(SignedType)10000+1.0)/(FloatType)(SignedType)SampleRate(); // Pole angle
			q = 1.0-w/(2.0*(((res+GetInput(input[2], n))*(FloatType)(SignedType)10+1.0)+0.5/(1.0+w))+w-2.0); // Pole magnitude
			resonance = q*q;
			cutoff = resonance+1.0-2.0*cos(w)*q;
		}

		/* Accelerate vibra by signal-vibra, multiplied by lowpasscutoff */
		vibraspeed += (GetInput(input[0], n)*0.3f - vibrapos) * cutoff;

		/* Add velocity to vibra's position */
		vibrapos += vibraspeed;

		/* Attenuate/amplify vibra's velocity by resonance */
		vibraspeed *= resonance;

		// needs clipping :(
		if (vibrapos>1.0f) vibrapos=1.0f;
		if (vibrapos<-1.0f) vibrapos=-1.0f;

		/* Store new value */
		SetOutput(output[0], n, vibrapos);
	}

	StateValue(m_VibraPosInd)->AsFloat = vibrapos;
	StateValue(m_VibraSpeedInd)->AsFloat = vibraspeed;
	StateValue(m_ResonanceInd)->AsFloat = resonance;
	StateValue(m_CutoffInd)->AsFloat = cutoff;
}
