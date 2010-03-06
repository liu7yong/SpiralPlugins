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

#include "Mixer.h"

//Initially commited by Dave, Sun Jul 28 23:18:15 2002 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1027898295::Mixer"
//  =>  752ea58ee28b9765618d0b306bcd79b6 (legacy == 0x7)
DevicePluginHook(Mixer, 752ea58ee28b9765618d0b306bcd79b6);

///////////////////////////////////////////////////////

void Mixer::Finalize()
{
  m_NumChannels->Value.AsUnsigned = 0;

  EnsureChannelProperties();

  UnReference(m_Volume);
  UnReference(m_VolumePeak);

  UnReference(m_Peak);
  UnReference(m_NumChannels);

  Super::Finalize();
}

Mixer *Mixer::Initialize(Patch *Host)
{
  m_NumChannels = UnsignedProperty::New(DefaultLinearFlags | Property::ForceUpdate, 4, 2, 256, 1, 1);
  m_Peak = ToggleProperty::New(Property::ReadOnly, false);
  m_Volume = ArrayProperty<FloatProperty>::New();
  m_VolumePeak = ArrayProperty<ToggleProperty>::New();

  Super::Initialize(Host);

  RegisterSharedProperty(m_NumChannels, StringHash("Channel Count")/*, "Channels"*/);
  RegisterSharedProperty(m_Peak, StringHash("Peak")/*, "Peak"*/);
  RegisterSharedProperty(m_Volume, StringHash("Channel Volume"));
  RegisterSharedProperty(m_VolumePeak, StringHash("Channel Peak"));
  
  return this;
}

static const UnsignedType Out = StringHash("Output", true);

bool Mixer::CreatePorts()
{
  /* FIXME - right now these ports can get created on Synth Thread */
  /* This is unavoidable for now, - list nodes, Port self insert */
  /* However, as long as the dsp code doesn't access the lists */
  /* directly, it should be possible to handle adding ports */
  /* on control thread, safely. The map would simply have to grow */
  /* Which would mean allocating new map & nodes on control thread */
  /* and lazy swapping, as a dependency for this - so queue of */
  /* commands that require thread reply before continuing. */
	while (InputPorts()->Count() > m_NumChannels->Value.AsUnsigned) 
	{
		UnReference(InputPorts()->Last());
	}

	while (InputPorts()->Count() < m_NumChannels->Value.AsUnsigned) 
	{
	  /* TODO - Given that meta information is going to be variable */
	  /* There needs to be a way to both map to unique localizeable information */
	  /* And have the description in the info.json file */
    String *tmp = FormatedString("Input %d", InputPorts()->Count());
		InputPort::New(this,tmp->Hash());
    UnReference(tmp);
	}

	EnsureChannelProperties();

	return true;
}

#if 0
/* Rework these for new design */
/* Need to rethink property loading etc */
/* Using json for file format makes things flexible */
bool Mixer::LoadProperties(map<string,Property*> &properties)
{
	bool result=true;

	/* See if this is a versioned set of properties */
	if (properties.count("VERSION"))
	{
		UnsignedProperty *version = dynamic_cast<UnsignedProperty*>(properties["VERSION"]);

		/* We need to force the Channel Count To be Processed First */
		if (properties.count("CHANNELS"))
		{
			map<string,Property*> force;
			force["CHANNELS"] = properties["CHANNELS"];

			result = Device::LoadProperties(force);
		}

		if (result)
		{
			/* Force it No Matter What Version (for now) */
			properties.erase("VERSION");

			result = Device::LoadProperties(properties);

			properties["VERSION"]=version;
		}
	}
	else
	{
		result = Device::LoadProperties(properties);
	}

	return result;
}

bool Mixer::UpdateProperty(string name)
{
	if (name == "CHANNELS")
	{
		CreatePorts();
	}

	return true;
}
#endif

void Mixer::Process(UnsignedType SampleCount) 
{
	// Mix the inputs
	for (UnsignedType n=0; n<SampleCount; n++) 
	{
		FloatType in, out = 0.0f;

		for (UnsignedType c=0; c<InputPorts()->Count(); c++) 
		{
			in = GetInput(((InputPort*)(*InputPorts())[c]), n) * ((*m_Volume)[c])->Value.AsFloat;
			out += in;

			/* This is going to potentially be wrong for voices - one voice might be peaked, another not */
			/* But that doesn't matter when dealing with an all for one slider */
			/* More specifics will need a VoiceMixer */
			((*m_VolumePeak)[c])->Value.AsBoolean = (in > 1.0);
		}

		SetOutput(GetOutputPort(Out), n, out);

		/* Again, This is going to potentially be wrong for voices - one voice might be peaked, another not */
		/* More specifics will need a VoiceMixer */
		m_Peak->Value.AsBoolean = (out > 1.0);
	}
}

void Mixer::EnsureChannelProperties()
{
	UnsignedType num = m_NumChannels->Value.AsUnsigned;

	/* Remove Channel Properties */
	while(m_Volume->Count() > num) 
	{
    UnReference(m_Volume->Values->RemoveLast());
    UnReference(m_VolumePeak->Values->RemoveLast());
	}

	/* Add Channel Properties */
	while (m_Volume->Count() < num) 
	{
		m_Volume->Append(FloatProperty::New(DefaultLinearFlags, 1.0f, 0.0f, 2.0f, 0.001f, 0.01f));
		m_VolumePeak->Append(ToggleProperty::New(Property::ReadOnly, false));
	}
}
