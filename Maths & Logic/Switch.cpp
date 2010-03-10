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

//Initially commited by Dave, Sun Jan 19 01:25:54 2003 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1042939554::Switch"
//  =>  18a0084d130df33f1ae53a08ccb2f558 (legacy == 0x2F)
DevicePluginHook(Switch, 18a0084d130df33f1ae53a08ccb2f558);

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

static const UnsignedType In1 = StringHash("Input 1", true);
static const UnsignedType In2 = StringHash("Input 2", true);
static const UnsignedType CV = StringHash("CV", true);

static const UnsignedType Out = StringHash("Output", true);

bool Switch::CreatePorts()
{
  return true;
}

void Switch::Process(UnsignedType SampleCount)
{	
	if (!m_Mix->Value())
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
