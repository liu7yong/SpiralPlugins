/*
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

#ifndef __ESDClient_H__
#define __ESDClient_H__

#include "esd.h"

#include <SpiralCore/Port.h>
#include <SpiralCore/Device.h>
#include <SpiralCore/Patch.h>

#include <SpiralCore/RiffWav.h>

using namespace Spiral;

class ESDClient
{
public:
	static ESDClient *Get()       { if(!m_Singleton) m_Singleton=new ESDClient; return m_Singleton; }
	static void PackUpAndGoHome() { if(m_Singleton)  { delete m_Singleton; m_Singleton=NULL; } }
	~ESDClient();

	void    AllocateBuffer();
	void    DeallocateBuffer();
	void    SendStereo(InputPort *ldata, InputPort *rdata);
	void    GetStereo(OutputPort *ldata,OutputPort *rdata);
	void    SetVolume(FloatType s) {m_Amp=s;}
	void    SetNumChannels(int s) {m_Channels=s;}
	FloatType   GetVolume() {return m_Amp;}
	void    Play();
	void    Read();

	short  *GetBuffer() {return m_Buffer[m_WriteBufferNum];}

	bool 	OpenReadWrite();
	bool    OpenWrite();
	bool    OpenRead();
	bool    Close();
	void    Kill() { m_IsDead = true; PackUpAndGoHome(); }

	inline UnsignedType SampleRate() { return pcm.rate; }
	inline UnsignedType SampleCount() { return pcm.frames; }

	static Patch *host;

	static void(*RunCallback)(void*, bool m);
	static void *RunContext;
private:
	struct
	{
		int control;
		int handle;

		bool connected;

		UnsignedType frames;
		UnsignedType rate;
	} pcm;

	static ESDClient* m_Singleton;

 	ESDClient();

	short  *m_Buffer[2];
	short  *m_InBuffer[2];
	int     m_BufSizeBytes;

	FloatType   m_Amp;
	int     m_Channels;

	int    	m_ReadBufferNum;
	int     m_WriteBufferNum;
	bool	m_IsDead;
};

#endif
