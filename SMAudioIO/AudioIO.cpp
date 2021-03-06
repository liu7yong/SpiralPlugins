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


static const int IN_FREQ  = 0;
static const int IN_PW    = 1;
static const int IN_SHLEN = 2;

static const int OUT_MAIN = 0;

int AudioIO::m_RefCount=0;
int AudioIO::m_NoExecuted=0;
int AudioIO::m_RealMode=3;

//SSM Was initially commited by Dave, Sun Jul 28 23:18:15 2002 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1027916292::Output"
//  =>  6e86a9417526779d302077af073d5111 (legacy == 0)
DevicePluginHook(AudioIO, 6e86a9417526779d302077af073d5111)

///////////////////////////////////////////////////////

void AudioIO::Finalize()
{
  m_RefCount--;
  
  if (m_RefCount==0)
  {
    cb_Blocking(m_Parent,false);
    OUTPUTCLIENT::PackUpAndGoHome();
    m_RealMode=0;
  }
  
  Super::Finalize();
}

AudioIO *AudioIO::Initialize(Patch *Host)
{
  Super::Initialize(Host);

  /*m_Volume = FloatProperty(, 0.5, 0, 1.0, 0.1, 0.01);
  m_Volume->command = true;
"Volume"
  m_Mode = SignedProperty(DefaultLinearFlags, 0, 0, 4, 1, 1);
  RegisterSharedProperty(m_Mode, "Mode", "Mode");
  m_Mode->command = true;*/

  m_Volume = 0.5f;
  m_Mode = 0;
  m_Mode = m_RealMode;

  /* FIXME - this is a horrible way to do things */
  /* and doesn't allow for multiple hosts to share an audio */
  if ((++m_RefCount) == 1)
  {
    OUTPUTCLIENT::host=Host;
    
    OUTPUTCLIENT::RunCallback = cb_Update;
    OUTPUTCLIENT::RunContext = m_Parent;
    
   // Reset();
  }
  
  return this;
}

static const UnsignedType LeftIn = StringHash("Left In", true);
static const UnsignedType RightIn = StringHash("Right In", true);
static const UnsignedType LeftOut = StringHash("Left Out", true);
static const UnsignedType RightOut = StringHash("Right Out", true);

bool AudioIO::CreatePorts()
{
  return true;
}

void AudioIO::Reset()
{
  if (!m_RefCount)
    return;

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
  if (!m_RefCount)
    return;
	if (m_RealMode == 4) 
		return;

	if (m_RealMode==2 || m_RealMode==3)
	{
      OUTPUTCLIENT::Get()->SendStereo(GetInputPort(LeftIn),GetInputPort(RightIn));
	}

	if (m_RealMode==1 || m_RealMode==3)
           OUTPUTCLIENT::Get()->GetStereo(GetOutputPort(LeftOut),GetOutputPort(RightOut));
}

void AudioIO::ProcessAudio()
{
  if (!m_RefCount)
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
