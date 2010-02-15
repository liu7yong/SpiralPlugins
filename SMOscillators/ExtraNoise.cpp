/*  This program is free software; you can redistribute it and/or modify
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
#include <limits.h>
#include <math.h>
#include "ExtraNoise.h"

//md5 -s "Andrew Johnson; Jοhan Kiviniemi::ajgenius@ajgenius.us::1264696200::ExtraNoise"
#define device_id d1d47c96e200908aedda145e912f6300
#define device_version 1

DevicePluginHook(ExtraNoise, device_id, device_version)

/*const DeviceDescription ExtraNoise::mDescription = 
{
	UniqueID       : ExtraNoise::mUniqueID,
	AudioDriver    : false,
	HostPlugin     : false,
	
	Author         : "Andrew Johnson; Jοhan Kiviniemi",
	Version        : 1,
	Label          : "Extra Noise",
	Info           : "Configurable Noise Generator",
	Category       : "Oscillators",
	PluginInstance : DevicePluginHookName(device_id)
};*/

const NumericPropertyValue defZeroFloat = DefaultFloat(0.0f);

ExtraNoise::ExtraNoise(Patch *Host) :
Device(Host),

/* Voice State Properties */
mState(NewStateProperty(defZeroFloat)),

mMagicA(new FloatProperty(DefaultLogarithmicFlags,8.0f, 6.0f, 12.0f, 0.00001, 0.001)),
mMagicB(new FloatProperty(DefaultLinearFlags,0.25f, 0.0f, 1.0f, 0.00001, 0.001))
{
	RegisterSharedProperty(mMagicA, StringHash("MAGIC A")/*"Magic A", "Magic A"*/);
	RegisterSharedProperty(mMagicB, StringHash("MAGIC B")/*"Magic B", "Magic B"*/);
}

bool ExtraNoise::CreatePorts()
{
	input[0] = new InputPort(this/*,  "Magic A"*/);
	input[1] = new InputPort(this/*, "Magic B"*/);

	output = new OutputPort(this/*, "Output"*/);
	
	return true;
}

static inline FloatType randf (void) { return (FloatType)rand() / (FloatType)RAND_MAX; }

static inline float
evil(const float a, const float b)
{
	union { float f; uint32_t i; } u, ub;
	u.f = a + b;
	
	// Only keep the exponent, which is n+127 where n<0 if |a+b|<1.0.
	u.i &= 0xff<<23;
	// Normalize the exponent. The sign will match in both representations.
	u.i -= 127<<23;
	// Only keep the sign. Minus if |a+b|<1.0.
	u.i &= 1<<31;
	
	// Flip ub’s sign based on u, negating ub if |a+b|<1.0.
	ub.f = b;
	ub.i ^= u.i;
	
	return a - ub.f;
}

void ExtraNoise::Process(UnsignedType SampleCount)
{
	#define state StateValue(mState)->AsFloat
	
	for (UnsignedType n=0; n<SampleCount; n++)
	{
		FloatType white = 2.0f*randf() - 1.0f;

		FloatType in0, in1;
		in0 = mMagicA->Min.AsFloat + GetInput(input[0],n)*(mMagicA->Max.AsFloat - mMagicA->Min.AsFloat);

		if (NUMBER_IS_INSANE(in0))
			in0 = 0.0f;

		in1 = mMagicB->Min.AsFloat + GetInput(input[1],n)*(mMagicB->Max.AsFloat - mMagicB->Min.AsFloat);
		if (NUMBER_IS_INSANE(in1))
			in1 = 0.0f;

		FloatType leak = 1.0f-powf(fabsf(state), (mMagicA->Value.AsFloat)); 
		
		state = evil(leak*state, ((in1))*white); 

		SetOutput(output,n, state);
	}
}
