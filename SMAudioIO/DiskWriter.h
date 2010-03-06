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

#ifndef __DiskWriter_H__
#define __DiskWriter_H__

#include <SpiralCore/RiffWav.h>
#include <SpiralCore/AudioDriver.h>
#include <SpiralCore/Patch.h>

using namespace Spiral;

class DiskWriter : Gumbo(AudioDriver)
{
  GumboClassDefinition(DiskWriter, AudioDriver,
    {
      mVersion = 2;
    },
    {
    },
  );
private:
  InputPort *left, *right, *recordcv;

  StringProperty *m_FileName;
  SetProperty *m_BitsPerSample;
  FloatProperty *m_TimeRecorded;
  ToggleProperty *m_Stereo, *m_Recording, *m_Open;

  int m_Version;
  WavFile *m_Wav;
  
  virtual void Finalize();
public:
  virtual DiskWriter *Initialize(Patch *Host);
  static inline DiskWriter *New(Patch *Host) { return Alloc()->Initialize(Host); }
  
  bool CreatePorts();
  
  virtual void Process(UnsignedType SampleCount);

  /* Audio Driver Specific Functions */
  virtual AudioProcessType	ProcessType() { return AudioDriver::MANUAL; }		
  virtual void			ProcessAudio() {}
};

#endif
