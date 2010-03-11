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
#include "Counter.h"

//Initially commited by Dave, Sun Jan 19 01:25:51 2003 UTC 
//md5 -s "Dave Griffiths::dave@pawfal.org::1042939551::Counter"
//  =>  d56ffbfa40b244e2976ec36ce4d5b1a4 (legacy == 0x2D)
DevicePluginHook(Counter, d56ffbfa40b244e2976ec36ce4d5b1a4);

///////////////////////////////////////////////////////

void Counter::Finalize()
{
  UnReference(m_Count);

  Super::Finalize();
}

const NumericPropertyValue defTrigger = DefaultBoolean(false);
const NumericPropertyValue defCurrent = DefaultUnsigned(0);
const NumericPropertyValue defCurrentLevel = DefaultFloat(1.0f);

Counter *Counter::Initialize(Patch *Host)
{
  Super::Initialize(Host);
  
  m_Count = NumberProperty<UnsignedType>::New(Property::WriteOnly, 4, 
                                          LinearConstraints<UnsignedType>::New(true, true, false, 0, 10000, 1, 1));

#if 0
  RegisterSharedProperty(m_Count, StringHash("Count")/*, "Count"*/);
#endif
  
  m_TriggeredInd = NewStateProperty(defTrigger);
  m_CurrentInd = NewStateProperty(defCurrent);
  m_CurrentLevelInd = NewStateProperty(defCurrentLevel);
	
  return this;
}

static const UnsignedType In = StringHash("Input", true);

static const UnsignedType Out = StringHash("Output", true);

bool Counter::CreatePorts()
{	
  return true;
}

void Counter::Process(UnsignedType SampleCount)
{	
	UnsignedType count = m_Count->Value();

	bool Triggered = StateValue(m_TriggeredInd)->AsBoolean;
	UnsignedType Current = StateValue(m_CurrentInd)->AsUnsigned;
	FloatType CurrentLevel = StateValue(m_CurrentLevelInd)->AsFloat;
	
	for (UnsignedType n=0; n<SampleCount; n++)
	{
		if (GetInput(GetInputPort(In),n)>0)
		{
			if(!Triggered)
			{
				Triggered=true;
				Current++;
			}
		}
		else
		{
			if (Triggered)
			{
				Triggered=false;
				Current++;
			}
		}

		if (Current>=count)
		{
			CurrentLevel=-CurrentLevel;
			Current=0;
		}
		
		SetOutput(GetOutputPort(Out),n,CurrentLevel);
	}

	StateValue(m_TriggeredInd)->AsBoolean = Triggered;
	StateValue(m_CurrentInd)->AsUnsigned = Current;
	StateValue(m_CurrentLevelInd)->AsFloat = CurrentLevel;
}
