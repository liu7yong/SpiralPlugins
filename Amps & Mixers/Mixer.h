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

#ifndef __MIXER_H__
#define __MIXER_H__

#include <SpiralCore/Port.h>
#include <SpiralCore/Device.h>
#include <SpiralCore/Patch.h>

using namespace Spiral;

class Mixer : Gumbo(Device)
{
  GumboClassDefinition(Mixer, Device,
    {
      mVersion = 2;
    },
    {
    },
  );
  
private:
	void EnsureChannelProperties();

	UnsignedProperty *m_NumChannels;
	ToggleProperty *m_Peak;

	ArrayProperty<FloatProperty> *m_Volume;
	ArrayProperty<ToggleProperty> *m_VolumePeak;

protected:
  virtual void Finalize();
  
public:
	virtual Mixer *Initialize(Patch *Host);
  static inline Mixer *New(Patch *Host) { return Alloc()->Initialize(Host); }
  
	//bool LoadProperties(map<string,Property*> &properties);
	//bool UpdateProperty(string name);

	bool CreatePorts();

	virtual void Process(UnsignedType SampleCount);
};

#endif
