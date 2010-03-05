// for lrintf()
#define	_ISOC9X_SOURCE	1
#define _ISOC99_SOURCE	1
#include  <math.h>
#include <unistd.h>
#include "ESDClient.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

ESDClient* ESDClient::m_Singleton = NULL;
Patch *ESDClient::host=NULL;
void (*ESDClient::RunCallback)(void*, bool m) = NULL;
void * ESDClient::RunContext = NULL;

ESDClient::ESDClient() :
m_Amp(0.5f),
m_Channels(2),
m_ReadBufferNum(0),
m_WriteBufferNum(0),
m_IsDead(false)

{
	pcm.connected = false;

	m_Buffer[0]=NULL;
	m_Buffer[1]=NULL;
	m_InBuffer[0]=NULL;
	m_InBuffer[1]=NULL;
}

ESDClient::~ESDClient()
{
	Close();
	DeallocateBuffer();
}

void ESDClient::AllocateBuffer()
{
	if (m_Buffer[0]==NULL)
	{
		if (!pcm.connected)
			pcm.frames = (unsigned int)host->SampleCount();

		// initialise for stereo
		m_Buffer[0] = new short[pcm.frames*2];

		m_Buffer[1] = new short[pcm.frames*2];
		m_InBuffer[0] = new short[pcm.frames*2];
		m_InBuffer[1] = new short[pcm.frames*2];
	}
}

void ESDClient::DeallocateBuffer()
{
	if (m_Buffer[0]!=NULL)
	{
		delete[] m_Buffer[0];
		delete[] m_Buffer[1];
		delete[] m_InBuffer[0];
		delete[] m_InBuffer[1];

		m_Buffer[0]=NULL;
		m_Buffer[1]=NULL;
		m_InBuffer[0]=NULL;
		m_InBuffer[1]=NULL;
	}
}

void ESDClient::SendStereo(InputPort *ldata,InputPort *rdata) 
{
	if (m_Channels!=2)	return;
	
	int on=0;
	float32 t;
	UnsignedType count;

	const Sample *l, *r;
	
    l = ((ldata) && ldata->IsConnected())?ldata->GetSampleBuffer(0):(Sample::Null());
    r = ((rdata) && rdata->IsConnected())?rdata->GetSampleBuffer(0):(Sample::Null());

	count = (!l->IsNull())?l->GetLength():((!r->IsNull())?r->GetLength():0);
	count = MIN(pcm.frames, count);

	for (UnsignedType n=0; n<count; n++)
	{
		if (m_IsDead)	return;

		// stereo channels - interleave	
		if (!l->IsNull()) 
		{
			t = CLAMP((float32)(ldata->GetSampleValue(n, 0)*m_Amp), -1, 1);

			m_Buffer[m_WriteBufferNum][on]+=lrintf(t*SHRT_MAX);
		}
		on++;
		
        if (!r->IsNull()) 
		{
			t = CLAMP((float32)(rdata->GetSampleValue(n, 0)*m_Amp), -1, 1);

			m_Buffer[m_WriteBufferNum][on]+=lrintf(t*SHRT_MAX);
		}		
		on++;
	}
}

void ESDClient::Play()
{
	int BufferToSend=!m_WriteBufferNum;
	
	if (pcm.connected)
	{
      write (pcm.handle, m_Buffer[BufferToSend], pcm.frames*sizeof(short)*2);
	}

	memset(m_Buffer[BufferToSend],0,pcm.frames*sizeof(short)*2);
	m_WriteBufferNum=BufferToSend;
}

void ESDClient::GetStereo(OutputPort *ldata,OutputPort *rdata) 
{
	if (m_Channels!=2)	return;
	
	int on=0;
	UnsignedType count;

	count = ldata->GetSampleBuffer(0)->GetLength();

	count = MIN(pcm.frames, count);
	for (UnsignedType n=0; n<count; n++)
	{
		if (m_IsDead)	return;

		// stereo channels - interleave	
		if (ldata) ldata->SetSampleValue(n, m_Amp*(FloatType)(SignedType)m_InBuffer[m_ReadBufferNum][on], 0);
		on++;
		if (rdata) rdata->SetSampleValue(n, m_Amp*(FloatType)(SignedType)m_InBuffer[m_ReadBufferNum][on], 0);
		on++;
	}
}

void ESDClient::Read()
{
	int BufferToRead=!m_ReadBufferNum;

	if (pcm.connected);

	m_ReadBufferNum=BufferToRead;
}

bool ESDClient::Close()
{
	if (pcm.connected)
	{
		
		esd_close (pcm.control);
		close (pcm.handle);
		pcm.connected = false;
	}

	return !pcm.connected;
}

bool ESDClient::OpenWrite()
{ 	
	return OpenReadWrite();
}

bool ESDClient::OpenRead()
{ 	
	return OpenReadWrite();
}

bool ESDClient::OpenReadWrite()
{
	Close();

	pcm.rate = (unsigned int)host->SampleRate();
	pcm.frames = (unsigned int)host->SampleRate();

	DeallocateBuffer();
	AllocateBuffer();
	printf("OPENING ESD STREAM\n");
	pcm.control = esd_open_sound ("localhost");
	pcm.handle = esd_play_stream (ESD_STREAM | ESD_PLAY | ESD_BITS16 | ESD_STEREO, 
					pcm.rate, "localhost", "SpiralSynthModular");

	pcm.connected = true;

	return pcm.connected;
}

