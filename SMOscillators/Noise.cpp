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
#include <limits.h>
#include <math.h>
#include "Noise.h"

using namespace Spiral;

DevicePluginHook(Noise, NoiseID)

///////////////////////////////////////////////////////
enum Type {WHITE=0, PINK, BROWN};
const NumericPropertyValue NoiseTypes[] = 
{
	/*{"White Noise",*/ DefaultUnsigned(WHITE)/*}*/,
	/*{"Pink Noise", */ DefaultUnsigned(PINK)/*}*/,
	/*{"Pink Noise", */ DefaultUnsigned(BROWN)/*}*/
};

const NumericPropertyValue defZeroFloat = DefaultFloat(0.0f);

void Noise::Finalize()
{
  UnReference(m_Type);
  
  Super::Finalize();
}

Noise *Noise::Initialize(Patch *Host)
{
  Super::Initialize(Host);

  /* Shared Properties */
  m_Type = SetProperty::New(Property::WriteOnly, 0, PropertySet::New(NoiseTypes, sizeof(NoiseTypes)/sizeof(NoiseTypes[0])));

  /* Voice State Properties */
  mB0 = NewStateProperty(defZeroFloat);
  mB1 = NewStateProperty(defZeroFloat);
  mB2 = NewStateProperty(defZeroFloat);
  mB3 = NewStateProperty(defZeroFloat);
  mB4 = NewStateProperty(defZeroFloat);
  mB5 = NewStateProperty(defZeroFloat);
  mB6 = NewStateProperty(defZeroFloat);

  RegisterSharedProperty(m_Type, StringHash("NOISE TYPE")/*, "Noise Type", "Noise Type"*/);
  
  return this;
}

bool Noise::CreatePorts()
{
  output = OutputPort::New(this/*, "Output"*/);

  return true;
}

void Noise::Process(UnsignedType SampleCount)
{
	switch (BROWN)//(m_Type->Value.AsUnsigned)
	{
		case BROWN:
		default:
		{
			for (UnsignedType n=0; n<SampleCount; n++)
			{
				SetOutput(output,n,GenerateBrown());
			}
		}
		break;
			
		case PINK:
		{
			for (UnsignedType n=0; n<SampleCount; n++)
			{
				SetOutput(output,n,GeneratePink());
			}
		}
		break;

		case WHITE:
		{
			for (UnsignedType n=0; n<SampleCount; n++)
			{
				SetOutput(output,n, GenerateWhite());
			}
		}
        break;
	}
}
