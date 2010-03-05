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
#include "Amp.h"

///////////////////////////////////////////////////////

DevicePluginHook(Amp, AmpID);

void Amp::Finalize()
{
  UnReference(m_Gain);
  UnReference(m_DC);
  
  Super::Finalize();
}

Amp *Amp::Initialize(Patch *Host)
{
  Super::Initialize(Host);

	m_Gain = FloatProperty::New(DefaultLinearFlags, 1.0f, -2.0f, 2.0f, 0.001f, 0.1f);
	m_DC = FloatProperty::New(DefaultLinearFlags, 0.0f, -2.0f, 2.0f, 0.001f, 0.1f);

	RegisterSharedProperty(m_Gain, StringHash("Gain"));
	RegisterSharedProperty(m_DC, StringHash("DC"));
	
  return this;
}

bool Amp::CreatePorts()
{
	InputPort::New(this/*, "Input"*/);
	OutputPort::New(this/*, "Output"*/);

	InputPort::New(this/*, "Gain CV"*/);
	InputPort::New(this/*, "DC Offset CV"*/);

	return true;
}

void Amp::Process(UnsignedType SampleCount)
{
	FloatType out, gain, dc;
	
	gain = m_Gain->Value.AsFloat;
	dc = m_DC->Value.AsFloat;

	for (UnsignedType n=0; n<SampleCount; n++)
	{
		out = GetInput(GetInputPort(0), n);
    out *= gain + GetInput(GetInputPort(1), n);
		out += (-dc) +  GetInput(GetInputPort(2), n);

		SetOutput(GetOutputPort(0), n, out);
	}		
}
