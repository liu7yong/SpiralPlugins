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

#include "AudioIO.h"
#include <SpiralCore/Patch.h>

#define OUTPUTCLIENT ESDClient
#include "ESDClient.h"

//#include "SpiralIcon.xpm"

using namespace std;

static const int IN_FREQ  = 0;
static const int IN_PW    = 1;
static const int IN_SHLEN = 2;

static const int OUT_MAIN = 0;

int AudioIO::m_RefCount=0;
int AudioIO::m_NoExecuted=0;
int AudioIO::m_RealMode=3;

//SSM Was initially commited by Dave, Sun Jul 28 23:18:15 2002 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1027916292::Output"
#define device_id 6e86a9417526779d302077af073d5111// legacy == 0
#define device_version 1
DevicePluginHook(AudioIO, device_id, device_version)

/*const DeviceDescription AudioIO::mDescription = 
{
  UniqueID       : AudioIO::mUniqueID,
  AudioDriver    : true,
  HostPlugin     : false,

  Author         : "Dave Griffiths",
  Version        : device_version,
  Label          : "Audio",
  Info           : "Audio Input/Output",
  Category       : "Input/Output",
  PluginInstance : DevicePluginHookName(device_id)
};*/

///////////////////////////////////////////////////////

AudioIO::AudioIO(Patch *Host) :
AudioDriver(Host)
{
	/*m_Volume = FloatProperty(, 0.5, 0, 1.0, 0.1, 0.01);
	m_Volume->command = true;
 "Volume"
	m_Mode = SignedProperty(DefaultLinearFlags, 0, 0, 4, 1, 1);
	RegisterSharedProperty(m_Mode, "Mode", "Mode");
	m_Mode->command = true;*/

	m_Volume = 0.5;
	m_Mode = 0;
}

AudioIO::~AudioIO()
{
	if (IsLive())
		Shutdown();

	if (m_RefCount==0)
	{
		cb_Blocking(m_Parent,false);
		OUTPUTCLIENT::PackUpAndGoHome();
		m_RealMode=0;
	}
}

bool AudioIO::CreatePorts()
{
	in[0] = new InputPort(this, /*"Left In",*/ Port::IS_MONOPHONIC | Port::CAN_FEEDBACK);
	in[1] = new InputPort(this, /*"Right In",*/ Port::IS_MONOPHONIC | Port::CAN_FEEDBACK);

	out[0] = new OutputPort(this, /*"Left Out",*/ Port::IS_MONOPHONIC);
	out[1] = new OutputPort(this, /*"Right Out",*/ Port::IS_MONOPHONIC);

	return true;
}

bool AudioIO::Setup()
{
	/* This has to be done BEFORE passing up the stack */
	if (++m_RefCount == 1)
	{
		OUTPUTCLIENT::host=Host();

		OUTPUTCLIENT::RunCallback = cb_Update;
		OUTPUTCLIENT::RunContext = m_Parent;
	}

	m_Mode = m_RealMode;

	bool result = AudioDriver::Setup();
	
	return result;
}

void AudioIO::Shutdown()
{
	AudioDriver::Shutdown();

	m_RefCount--;
	if (m_RefCount==0)
	{
		OUTPUTCLIENT::Get()->Kill();
		m_RealMode=0;
		cb_Blocking(m_Parent,false);
	}
}

void AudioIO::Reset()
{
	if (!IsLive())	return;

	OUTPUTCLIENT::Get()->Kill();
	cb_Blocking(m_Parent,false);

	OUTPUTCLIENT::Get()->AllocateBuffer();

	switch (m_RealMode)
	{
		case 1 :
 			OUTPUTCLIENT::Get()->OpenRead();
			cb_Blocking(m_Parent,false);
		break;
 		case 2 :
    			OUTPUTCLIENT::Get()->OpenWrite();
			cb_Blocking(m_Parent,true);
		break;
		case 3 :
			OUTPUTCLIENT::Get()->OpenReadWrite();
			cb_Blocking(m_Parent,true);
		break;
		
		default:{}
	}
}

void AudioIO::Process(UnsignedType SampleCount)
{
	if (!IsLive() || (m_RealMode == 4)) 
		return;

	if (m_RealMode==2 || m_RealMode==3)
	{
			OUTPUTCLIENT::Get()->SendStereo(in[0],in[1]);
	}

	if (m_RealMode==1 || m_RealMode==3)
           OUTPUTCLIENT::Get()->GetStereo(out[0],out[1]);
}

/*void AudioIO::ExecuteCommands()
{
	if (!IsLive()) 
		return;

	if (m_AudioCH->IsCommandWaiting())
	{
		UnsignedType command = m_AudioCH->GetCommand();

		if (command == m_Volume->id)
		{
			OUTPUTCLIENT::Get()->SetVolume(m_Volume.Value.AsFloat);

			return;
		}

		if (command == m_Mode->id)
		{
			SignedType mode = m_Mode.Value.AsSigned;

			switch (mode)
			{
				case 1:
					if (OUTPUTCLIENT::Get()->OpenRead())
					{
						m_RealMode=1;
					}
				break;

				case 2 :
					if (OUTPUTCLIENT::Get()->OpenWrite())
					{
						m_RealMode=2;
						cb_Blocking(m_Parent,true);
					}
				break;

				case 3 :
					if (OUTPUTCLIENT::Get()->OpenReadWrite())
					{
						m_RealMode=3;
						cb_Blocking(m_Parent,true);
					}
				break;

				case 4 :
					m_RealMode=4;
					cb_Blocking(m_Parent,false);
					OUTPUTCLIENT::Get()->Close();
				break;
			}
		}
        }
}*/

void AudioIO::ProcessAudio()
{
	if (!IsLive()) 
		return;

        if (m_RealMode == 0)
        {
                m_RealMode=2;
		Reset();
        }

	if ((SampleCount() != OUTPUTCLIENT::Get()->SampleCount())
		|| (SampleRate() != OUTPUTCLIENT::Get()->SampleRate()))
	{
		ChangeBufferAndSampleRate(OUTPUTCLIENT::Get()->SampleCount(), OUTPUTCLIENT::Get()->SampleRate(), m_Parent);
	}

	// Only Play() once per set of plugins
	m_NoExecuted--;
	if (m_NoExecuted<=0)
	{
		if (m_RealMode==1 || m_RealMode==3) OUTPUTCLIENT::Get()->Read();
		if (m_RealMode==2 || m_RealMode==3) OUTPUTCLIENT::Get()->Play();
		m_NoExecuted=m_RefCount;
	}
}
