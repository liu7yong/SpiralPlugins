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

DevicePluginHook(RingMod, RingModID);

///////////////////////////////////////////////////////

void RingMod::Finalize()
{
  UnReference(m_Amount);
  Super::Finalize();
}

RingMod *RingMod::Initialize(Patch *Host)
{
  Super::Initialize(Host);

  m_Amount = FloatProperty::New(DefaultLinearFlags,1.0f, 0, 3, 0.0001, 0.001);
  RegisterSharedProperty(m_Amount, StringHash("Amount")/*, "Amount"*/);

  return this;
}

bool RingMod::CreatePorts()
{	
	InputPort::New(this/*, "Input 1"*/);
	InputPort::New(this/*, "Input 2"*/);
	OutputPort::New(this/*, "Output"*/);

	return true;
}

void RingMod::Process(UnsignedType SampleCount)
{
	FloatType amount = m_Amount->Value.AsFloat;

	for (UnsignedType n=0; n<SampleCount; n++)
      SetOutput(GetOutputPort(0),n,GetInput(GetInputPort(0),n)*GetInput(GetInputPort(1),n)*amount);
}
