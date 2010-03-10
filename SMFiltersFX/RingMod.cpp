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
#include "RingMod.h"

//Initially commited by Dave, 
//md5 -s "Dave Griffiths::dave@pawfal.org::1027898295::RingMod"
//  => f9587ded2aed867d761fca167c1d3960 (legacy == 0xA)
DevicePluginHook(RingMod, f9587ded2aed867d761fca167c1d3960);

///////////////////////////////////////////////////////

void RingMod::Finalize()
{
  UnReference(m_Amount);
  Super::Finalize();
}

RingMod *RingMod::Initialize(Patch *Host)
{
  Super::Initialize(Host);

  m_Amount = NumberProperty<FloatType>::New(Property::WriteOnly, 1.0f, 
                                            LinearConstraints<FloatType>::New(true, true, false, 0, 3, 0.0001f, 0.001f));
  RegisterSharedProperty(m_Amount, StringHash("Amount", true)/*, "Amount"*/);

  return this;
}

static const UnsignedType In1 = StringHash("Input 1", true);
static const UnsignedType In2 = StringHash("Input 2", true);
static const UnsignedType Out = StringHash("Output", true);

bool RingMod::CreatePorts()
{	
  return true;
}

void RingMod::Process(UnsignedType SampleCount)
{
	FloatType amount = m_Amount->Value();

	for (UnsignedType n=0; n<SampleCount; n++)
      SetOutput(GetOutputPort(Out),n,GetInput(GetInputPort(In1),n)*GetInput(GetInputPort(In2),n)*amount);
}
