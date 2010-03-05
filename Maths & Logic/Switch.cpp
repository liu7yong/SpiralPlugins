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

static const UnsignedType In1 = StringHash("Input 1");
static const UnsignedType In2 = StringHash("Input 2");
static const UnsignedType CV = StringHash("CV");

static const UnsignedType Out = StringHash("Output");

bool Switch::CreatePorts()
{
  InputPort::New(this, In1);
  InputPort::New(this, In2);
  InputPort::New(this, CV);

  OutputPort::New(this, Out);

  return true;
}

void Switch::Process(UnsignedType SampleCount)
{	
	if (!m_Mix->Value.AsBoolean)
	{
		for (UnsignedType n=0; n<SampleCount; n++)
		{
			if (GetInput(GetInputPort(CV),n)<=0)
			{
				SetOutput(GetOutputPort(Out),n,GetInput(GetInputPort(In1),n));
				continue;
			}

			SetOutput(GetOutputPort(Out),n,GetInput(GetInputPort(In2),n));
		}
	}
	else 
  	for (UnsignedType n=0; n<SampleCount; n++)
  		SetOutput(GetOutputPort(Out),n,Linear(-1.0f,1.0f,GetInput(GetInputPort(CV),n),GetInput(GetInputPort(In1),n),GetInput(GetInputPort(In2),n)));
}
