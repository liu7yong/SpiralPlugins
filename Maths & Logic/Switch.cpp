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

#include "Switch.h"

DevicePluginHook(Switch, SwitchID);

///////////////////////////////////////////////////////
void Switch::Finalize()
{
  UnReference(m_Mix);
  Super::Finalize();
}

Switch *Switch::Initialize(Patch *Host)
{
  Super::Initialize(Host);

	m_Mix = ToggleProperty::New(Property::WriteOnly,false);
	RegisterSharedProperty(m_Mix, StringHash("Mix")/*, "Mix"*/);

  return this;
}

bool Switch::CreatePorts()
{
	InputPort::New(this/*, "Input 1"*/);
	InputPort::New(this/*, "Input 2"*/);
	InputPort::New(this/*, "CV"*/);

	OutputPort::New(this/*, "Output"*/);

	return true;
}

void Switch::Process(UnsignedType SampleCount)
{	
	if (!m_Mix->Value.AsBoolean)
	{
		for (UnsignedType n=0; n<SampleCount; n++)
		{
			if (GetInput(GetInputPort(2),n)<=0)
			{
				SetOutput(GetOutputPort(0),n,GetInput(GetInputPort(0),n));
				continue;
			}

			SetOutput(GetOutputPort(0),n,GetInput(GetInputPort(1),n));
		}
	}
	else 
  	for (UnsignedType n=0; n<SampleCount; n++)
  		SetOutput(GetOutputPort(0),n,Linear(-1.0f,1.0f,GetInput(GetInputPort(2),n),GetInput(GetInputPort(0),n),GetInput(GetInputPort(1),n)));
}
