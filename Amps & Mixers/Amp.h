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

#ifndef __AMP_H__
#define __AMP_H__

#include <SpiralCore/Port.h>
#include <SpiralCore/Device.h>
#include <SpiralCore/Patch.h>

using namespace Spiral;

class Amp : Gumbo(Device)
{
  GumboClassDefinition(Amp, Device,
    {
      mVersion = 1;
    },
    {
    },
  );
  
private:
	NumberProperty<FloatType> *m_Gain, *m_DC;

protected:
  virtual void Finalize();
  
public:
 	virtual Amp *Initialize(Patch *Host);
  static inline Amp *New(Patch *Host) { return Alloc()->Initialize(Host); }

	virtual bool CreatePorts();

	virtual void Process(UnsignedType SampleCount);

};

#endif
