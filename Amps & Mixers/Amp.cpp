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

//Initially commited by Dave, Sun Jul 28 23:18:15 2002 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1027898295::Amp"
//  =>  3d2307b7d767fd8d270c11e2f712a870 (legacy == 0x9)
DevicePluginHook(Amp, 3d2307b7d767fd8d270c11e2f712a870);

///////////////////////////////////////////////////////

void Amp::Finalize()
{
  UnReference(m_Gain);
  UnReference(m_DC);
  
  Super::Finalize();
}

Amp *Amp::Initialize(Patch *Host)
{
  Super::Initialize(Host);

  m_Gain = NumberProperty<FloatType>::New(Property::WriteOnly, 1.0f, 
                                            LinearConstraints<FloatType>::New(true, true, false, -2.0f, 2.0f, 0.001f, 0.1f));
  m_DC = NumberProperty<FloatType>::New(Property::WriteOnly, 0.0f, 
                                            LinearConstraints<FloatType>::New(true, true, false, -2.0f, 2.0f, 0.001f, 0.1f));

  RegisterSharedProperty(m_Gain, StringHash("Gain"));
  RegisterSharedProperty(m_DC, StringHash("DC"));
	
  return this;
}

static const UnsignedType In = StringHash("Input", true);
static const UnsignedType Out = StringHash("Output", true);

static const UnsignedType GainCV = StringHash("Gain CV", true);
static const UnsignedType DCOffsetCV = StringHash("DC Offset CV", true);

bool Amp::CreatePorts()
{
  return true;
}

void Amp::Process(UnsignedType SampleCount)
{
  FloatType out, gain, dc;

  gain = m_Gain->Value();
  dc = m_DC->Value();

  for (UnsignedType n=0; n<SampleCount; n++)
  {
    out = GetInput(GetInputPort(In), n);
    out *= gain + GetInput(GetInputPort(GainCV), n);
    out += (-dc) +  GetInput(GetInputPort(DCOffsetCV), n);

    SetOutput(GetOutputPort(Out), n, out);
  }		
}
