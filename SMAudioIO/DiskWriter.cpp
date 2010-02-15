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

// is this still relevant
// for lrintf()
#define	_ISOC9X_SOURCE	1
#define _ISOC99_SOURCE	1

#include "DiskWriter.h"
//#include "SpiralIcon.xpm"

//Disk Writer was initially commited by Dave, Nov 11 23:27:35 2002 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1037078855::DiskWriter"
#define device_id 1f5efd4bb21f90cf99ed081a335d913a// legacy == "29"
#define device_version 2

DevicePluginHook(DiskWriter, device_id, device_version)

/*const DeviceDescription DiskWriter::mDescription = 
{
	UniqueID       : DiskWriter::mUniqueID,
	AudioDriver    : true,
	HostPlugin     : false,

	Author         : "Dave Griffiths",
	Version        : 2,
	Label          : "DiskWriter",
	Info           : "Disk Writer",
	Category       : "Input/Output",
	PluginInstance : DevicePluginHookName(device_id)
};*/

///////////////////////////////////////////////////////

const NumericPropertyValue BitRates[] = 
{
	DefaultUnsigned(16),
	DefaultUnsigned(24),
	DefaultUnsigned(32)
};

DiskWriter::DiskWriter(Patch *Host):
AudioDriver(Host),
m_FileName(new StringProperty(Property::WriteOnly, "")),
m_Open(new ToggleProperty(Property::WriteOnly | Property::ForceUpdate, false)),
m_Recording(new ToggleProperty(Property::WriteOnly | Property::ForceUpdate, false)),
m_BitsPerSample(new SetProperty(Property::WriteOnly, 0, new PropertySet(BitRates, sizeof(BitRates)/sizeof(BitRates[0])))),
m_Stereo(new ToggleProperty(Property::WriteOnly, true)),

/* Just So The GUI Can Keep track of Current Time Recorded */
m_TimeRecorded(new FloatProperty(Property::ReadOnly, 0, 0, 0, 0, 0))
{
	RegisterSharedProperty(m_Open, StringHash("OPEN")/*"Open", "Open"*/);
	RegisterSharedProperty(m_Recording, StringHash("RECORDING")/*"Recording", "Recording"*/);
	RegisterSharedProperty(m_FileName, StringHash("FILENAME") /*"Filename", "Filename"*/);
	RegisterSharedProperty(m_BitsPerSample, StringHash("BITS PER SAMPLE")/*"Bits Per Sample", "Bits Per Sample"*/);
	RegisterSharedProperty(m_Stereo, StringHash("STEREO")/*"Stereo", "Stereo"*/);
	RegisterSharedProperty(m_TimeRecorded, StringHash("TIME RECORDED")/*"Time Recorded", "Time Recorded"*/);
}

bool DiskWriter::CreatePorts()
{
	left = new InputPort(this, /*"Left Out",*/ Port::IS_MONOPHONIC);
	right = new InputPort(this, /*"Right Out",*/ Port::IS_MONOPHONIC);
	recordcv = new InputPort(this, /*"Record Controller",*/ Port::IS_MONOPHONIC);

	return true;
}

void DiskWriter::Process(UnsignedType SampleCount)
{
	if(m_Recording->Value.AsBoolean && m_Wav.IsOpen())
	{
		FloatType LeftBuffer[SampleCount], RightBuffer[SampleCount];

		for (UnsignedType n=0; n<SampleCount; n++)
		{
			// stereo channels - interleave
			LeftBuffer[n]=GetInput(left,n);
			RightBuffer[n]=GetInput(right,n);;
		}

		m_Wav.Save(LeftBuffer, RightBuffer, SampleCount);
		m_TimeRecorded->Value.AsFloat = (m_Wav.GetSize()/m_Wav.GetSamplerate());
	}
}

/*void DiskWriter::ExecuteCommands()
{
	if (m_AudioCH->IsCommandWaiting())
	{
		UnsignedType command = m_AudioCH->GetCommand();

		if (command == m_Recording->id)
		{
			if (ValueAsBoolean(m_Recording))
			{
				if (!m_Wav.IsOpen())
				{
					if (m_Wav.GetSamplerate() != SampleRate()) 
					{
						m_Wav.SetSamplerate(SampleRate());
					}

					if (m_Wav.GetBitsPerSample() != m_BitsPerSample->Value.AsSigned) 
					{
						m_Wav.SetBitsPerSample(m_BitsPerSample->Value.AsSigned);
					}

					m_Wav.Open(ValueAsString(m_FileName),WavFile::WRITE, (ValueAsBoolean(m_Stereo))?(WavFile::STEREO):(WavFile::MONO));

					ValueSetBoolean(m_Open, true);

				}
				
				return;
			}

			return;
		}

		if (command == m_Open->id)
		{
			if (ValueAsBoolean(m_Open))
			{
				if (!m_Wav.IsOpen())
				{
					if (m_Wav.GetSamplerate() != GetHostInfo()->SAMPLERATE) 
					{
						m_Wav.SetSamplerate(GetHostInfo()->SAMPLERATE);
					}

					if (m_Wav.GetBitsPerSample() != m_BitsPerSample->Value.AsSigned) 
					{
						m_Wav.SetBitsPerSample(m_BitsPerSample->Value.AsSigned);
					}

					m_Wav.Open(ValueAsString(m_FileName),WavFile::WRITE, (ValueAsBoolean(m_Stereo))?(WavFile::STEREO):(WavFile::MONO));
				}

				return;
			}

			if (m_Wav.IsOpen())
			{
				m_Wav.Close();
			}
		}
	}
}*/
