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

#ifndef __SWITCH_H__
#define __SWITCH_H__

#include <SpiralCore/Port.h>
#include <SpiralCore/Device.h>
#include <SpiralCore/Patch.h>

using namespace Spiral;

//Initially commited by Dave, Sun Jan 19 01:25:54 2003 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1042939554::Switch"
//  =>  18a0084d130df33f1ae53a08ccb2f558 (legacy == 0x2F)
#define SwitchID 18a0084d130df33f1ae53a08ccb2f558
class Switch : Gumbo(Device)
{
  GumboClassDefinition(Switch, Device,
    {
      mUniqueID = String::New(XSTRINGIFY(SwitchID));
      mVersion = 1;
    },
    {
    },
  );
  
private:
	ToggleProperty *m_Mix;

protected:
  virtual void Finalize();
  
public:
 	virtual Switch *Initialize(Patch *Host);
	static inline Switch *New(Patch *Host) { return Alloc()->Initialize(Host); }
  
	bool CreatePorts();

	virtual void Process(UnsignedType SampleCount);
};

#endif
