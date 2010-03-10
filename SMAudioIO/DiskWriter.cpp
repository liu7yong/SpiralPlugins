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

#include "DiskWriter.h"

//Disk Writer was initially commited by Dave, Nov 11 23:27:35 2002 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1037078855::DiskWriter"
//  => 1f5efd4bb21f90cf99ed081a335d913a (legacy == 29)
DevicePluginHook(DiskWriter, 1f5efd4bb21f90cf99ed081a335d913a)

///////////////////////////////////////////////////////

const UnsignedType BitRates[] = { 16, 24, 32 };

void DiskWriter::Finalize()
{
  UnReference(m_Wav);
  Super::Finalize();
}

DiskWriter *DiskWriter::Initialize(Patch *Host)
{
  Super::Initialize(Host);
  
  m_Wav = WavFile::New();
  
//  m_FileName = StringProperty::New(Property::WriteOnly, "");
  m_Open = ToggleProperty::New(Property::WriteOnly | Property::ForceUpdate, false);
  m_Recording = ToggleProperty::New(Property::WriteOnly | Property::ForceUpdate, false);
  m_Stereo = ToggleProperty::New(Property::WriteOnly, true);

  m_BitsPerSample = NumberProperty<UnsignedType>::New(Property::WriteOnly, 0, 
                                                      SetConstraints<UnsignedType>::New(BitRates, sizeof(BitRates)/sizeof(BitRates[0])));

/* Just So The GUI Can Keep track of Current Time Recorded */
  m_TimeRecorded = NumberProperty<FloatType>::New(Property::ReadOnly, 0, NULL);

  RegisterSharedProperty(m_Open, StringHash("OPEN")/*"Open", "Open"*/);
  RegisterSharedProperty(m_Recording, StringHash("RECORDING")/*"Recording", "Recording"*/);
//  RegisterSharedProperty(m_FileName, StringHash("FILENAME") /*"Filename", "Filename"*/);
  RegisterSharedProperty(m_BitsPerSample, StringHash("BITS PER SAMPLE")/*"Bits Per Sample", "Bits Per Sample"*/);
  RegisterSharedProperty(m_Stereo, StringHash("STEREO")/*"Stereo", "Stereo"*/);
  RegisterSharedProperty(m_TimeRecorded, StringHash("TIME RECORDED")/*"Time Recorded", "Time Recorded"*/);

  return this;
}

static const UnsignedType LeftIn = StringHash("Left In", true);
static const UnsignedType RightIn = StringHash("Right In", true);
static const UnsignedType RecordCV= StringHash("Record CV", true);

bool DiskWriter::CreatePorts()
{
  left = GetInputPort(LeftIn);
  right = GetInputPort(RightIn);
  recordcv = GetInputPort(RecordCV);

  return true;
}

void DiskWriter::Process(UnsignedType SampleCount)
{
	if(m_Recording->Value() && m_Wav->IsOpen())
	{
		FloatType LeftBuffer[SampleCount], RightBuffer[SampleCount];

		for (UnsignedType n=0; n<SampleCount; n++)
		{
			// stereo channels - interleave
			LeftBuffer[n]=GetInput(left,n);
			RightBuffer[n]=GetInput(right,n);;
		}

		m_Wav->Save(LeftBuffer, RightBuffer, SampleCount);
		m_TimeRecorded->SetValue(m_Wav->GetSize()/m_Wav->GetSamplerate());
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
				if (!m_Wav->IsOpen())
				{
					if (m_Wav->GetSamplerate() != SampleRate()) 
					{
						m_Wav->SetSamplerate(SampleRate());
					}

					if (m_Wav->GetBitsPerSample() != m_BitsPerSample->Value.AsSigned) 
					{
						m_Wav->SetBitsPerSample(m_BitsPerSample->Value.AsSigned);
					}

					m_Wav->Open(ValueAsString(m_FileName),WavFile::WRITE, (ValueAsBoolean(m_Stereo))?(WavFile::STEREO):(WavFile::MONO));

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
				if (!m_Wav->IsOpen())
				{
					if (m_Wav->GetSamplerate() != GetHostInfo()->SAMPLERATE) 
					{
						m_Wav->SetSamplerate(GetHostInfo()->SAMPLERATE);
					}

					if (m_Wav->GetBitsPerSample() != m_BitsPerSample->Value.AsSigned) 
					{
						m_Wav->SetBitsPerSample(m_BitsPerSample->Value.AsSigned);
					}

					m_Wav->Open(ValueAsString(m_FileName),WavFile::WRITE, (ValueAsBoolean(m_Stereo))?(WavFile::STEREO):(WavFile::MONO));
				}

				return;
			}

			if (m_Wav->IsOpen())
			{
				m_Wav->Close();
			}
		}
	}
}*/
