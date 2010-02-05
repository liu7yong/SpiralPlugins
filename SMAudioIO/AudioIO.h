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

#include <SpiralCore/AudioDriver.h>
#include <SpiralCore/RiffWav.h>
#include <SpiralCore/Port.h>

using namespace Spiral;
#ifndef __AudioIO_H__
#define __AudioIO_H__

class AudioIO : public AudioDriver
{
DeviceDescriptionTemplate(AudioIO)
private:
  /* Instance State */
	InputPort *in[2];
	OutputPort *out[2];

	//NumericProperty m_Volume, *m_Mode;
	FloatType m_Volume;
	int m_Mode;

	static int m_RealMode;


	static int m_RefCount;
	static int m_NoExecuted;

	bool m_CheckedAlready;

public:
 	AudioIO(Patch *Host);
	virtual ~AudioIO();

	bool Setup();

	bool CreatePorts();

	/* General Plugin Function */
	virtual	void	Process(UnsignedType SampleCount);
	//virtual void	ExecuteCommands();

	virtual void	Shutdown();
	virtual void	Reset();
	
	/* Audio Driver Specific Functions */
	virtual AudioProcessType	ProcessType() { return AudioDriver::ALWAYS; }		
	virtual void			ProcessAudio();
};

#endif
