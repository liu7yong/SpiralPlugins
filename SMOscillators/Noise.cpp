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
#include <cmath>
#include "Noise.h"

using namespace Spiral;

//Noise Was initially commited by Dave, Sun Jan 19 01:25:52 2003 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1042961152::Noise"
//  => 7cac72b3687628ec373ef10ae8a2ab4a (legacy == 32)
DevicePluginHook(Noise, 7cac72b3687628ec373ef10ae8a2ab4a)

///////////////////////////////////////////////////////
enum Type {WHITE=0, PINK, BROWN};

const NumericPropertyValue defZeroFloat = DefaultFloat(0.0f);

void Noise::Finalize()
{
  UnReference(m_Type);
  
  Super::Finalize();
}

Noise *Noise::Initialize(Patch *Host)
{
  Super::Initialize(Host);

  /* Voice State Properties */
  mB0 = NewStateProperty(defZeroFloat);
  mB1 = NewStateProperty(defZeroFloat);
  mB2 = NewStateProperty(defZeroFloat);
  mB3 = NewStateProperty(defZeroFloat);
  mB4 = NewStateProperty(defZeroFloat);
  mB5 = NewStateProperty(defZeroFloat);
  mB6 = NewStateProperty(defZeroFloat);

  return this;
}

static const UnsignedType Out = StringHash("Output", true);

bool Noise::CreatePorts()
{
  output = GetOutputPort(Out);

  return true;
}

void Noise::CreateProperty(UnsignedType aPropertyID, Property *aProperty)
{
  /* Shared Property Instantiation */
  if (aPropertyID == StringHash("Noise Type", true))
    m_Type = NumberProperty<UnsignedType>::New(aProperty);
}

void Noise::Process(UnsignedType SampleCount)
{
	switch (m_Type->Value())
	{
		case BROWN:
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
		default:
		{
			for (UnsignedType n=0; n<SampleCount; n++)
			{
				SetOutput(output,n, GenerateWhite());
			}
		}
    break;
	}
}
