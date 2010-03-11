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
#include <cmath>
#include "ExtraNoise.h"

//md5 -s "Andrew Johnson; Jοhan Kiviniemi::ajgenius@ajgenius.us::1264696200::ExtraNoise"
//  => d1d47c96e200908aedda145e912f6300
DevicePluginHook(ExtraNoise, d1d47c96e200908aedda145e912f6300)

const NumericPropertyValue defZeroFloat = DefaultFloat(0.0f);

ExtraNoise *ExtraNoise::Initialize(Patch *Host)
{
  Super::Initialize(Host),

  /* Voice State Properties */
  mState = NewStateProperty(defZeroFloat);

  return this;
}

static const UnsignedType MagicACV = StringHash("Magic A CV", true);
static const UnsignedType MagicBCV = StringHash("Magic B CV", true);
static const UnsignedType Out = StringHash("Output", true);

bool ExtraNoise::CreatePorts()
{
  input[0] = GetInputPort(MagicACV);
  input[1] = GetInputPort(MagicBCV);
  
  output = GetOutputPort(Out);
  
  return true;
}

void ExtraNoise::CreateProperty(UnsignedType aPropertyID, Property *aProperty)
{
  /* Shared Property Instantiation */
  if (aPropertyID == StringHash("Magic A", true))
  {
    mMagicA = NumberProperty<FloatType>::New(aProperty);
    return;
  }

  if (aPropertyID == StringHash("Magic B", true))
  {
    mMagicB = NumberProperty<FloatType>::New(aProperty);
    return;
  }
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
	
	/* This has been broken several times - fixme */
	for (UnsignedType n=0; n<SampleCount; n++)
	{
		FloatType white = 2.0f*randf() - 1.0f;

		FloatType in0, in1;
		in0 = mMagicA->Value() + 6.0f + GetInput(input[0],n)*(12.0f - 6.0f);

		if (NUMBER_IS_INSANE(in0))
			in0 = 0.0f;

		in1 = mMagicB->Value() + GetInput(input[1],n);
		if (NUMBER_IS_INSANE(in1))
			in1 = 0.0f;

        FloatType leak = 1.0f-powf(fabsf(state), in0); 
		
		state = evil(leak*state, ((in1))*white); 

		SetOutput(output,n, state);
	}
}
