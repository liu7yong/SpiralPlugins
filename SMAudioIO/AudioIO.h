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

//SSM Was initially commited by Dave, Sun Jul 28 23:18:15 2002 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1027916292::Output"
//  =>  6e86a9417526779d302077af073d5111 (legacy == 0)
#define AudioIOID 6e86a9417526779d302077af073d5111

class AudioIO : Gumbo(AudioDriver)
{
  GumboClassDefinition(AudioIO, AudioDriver,
    {
      mUniqueID = String::New(XSTRINGIFY(AudioIOID));
      mVersion = 1;
    },
    {
    },
  );
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

protected:
	virtual void Finalize();
public:
 	virtual AudioIO *Initialize(Patch *Host);

	static inline AudioIO *New(Patch *Host) { return Alloc()->Initialize(Host); }

	bool CreatePorts();

	/* General Plugin Function */
	virtual	void	Process(UnsignedType SampleCount);
	//virtual void	ExecuteCommands();

	virtual void	Reset();
	
	/* Audio Driver Specific Functions */
	virtual AudioProcessType	ProcessType() { return AudioDriver::ALWAYS; }		
	virtual void			ProcessAudio();
};

#endif
