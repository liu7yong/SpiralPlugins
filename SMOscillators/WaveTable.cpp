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
#include <math.h>
#include "WaveTable.h"

using namespace std;

static const unsigned int NUM_TABLES = 8;
static const unsigned int DEFAULT_TABLE_LEN = 1024;

static const int IN_FREQ  = 0;
static const int IN_PW    = 1;
static const int IN_SHLEN = 2;

//WaveTable Was initially commited by Dave, Sun Jul 28 23:18:17 2002 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1027916297::WaveTable"
#define device_id d7a6e545474f4fc1c5f6d2acedc28cd7// legacy == 17
#define device_version 1

DevicePluginHook(WaveTable, device_id, device_version)

/*const DeviceDescription WaveTable::mDescription = 
{
  UniqueID       : WaveTable::mUniqueID,
  AudioDriver    : false,
  HostPlugin     : false,

  Author         : "David Griffiths",
  Version        : 1,
  Label          : "WaveTable",
  Info           : "Wave Table",
  Category       : "Oscillators",
  PluginInstance : DevicePluginHookName(device_id)
};*/

enum {SINE=0,SQUARE,SAW,REVSAW,TRIANGLE,PULSE1,PULSE2,INVSINE};
const NumericPropertyValue WaveTypes[] = 
{
	/*{"Sine", */DefaultUnsigned(SINE)/*}*/,
	/*{"Square", */DefaultUnsigned(SQUARE)/*}*/,
	/*{"Saw", */DefaultUnsigned(SAW)/*}*/,
	/*{"Reverse Saw", */DefaultUnsigned(REVSAW)/*}*/,
	/*{"Triangle", */DefaultUnsigned(TRIANGLE)/*}*/,
	/*{"Pulse 1", */DefaultUnsigned(PULSE1)/*}*/,
	/*{"Pulse 2", */DefaultUnsigned(PULSE2)/*}*/,
	/*{"Inverse Sine", */DefaultUnsigned(INVSINE)/*}*/
};

const NumericPropertyValue defCyclePos = DefaultFloat(0.0f);
const NumericPropertyValue defNoteInd = DefaultSigned(0);

WaveTable::WaveTable(Patch *Host) :
  Device(Host),
  m_TableLength(DEFAULT_TABLE_LEN),

	/* Shared Properties */
	m_Type(Property::WriteOnly, 0, PropertySet(WaveTypes, sizeof(WaveTypes)/sizeof(WaveTypes[0]))),
	m_Octave(DefaultLinearFlags,0,-3,3,1,1),
	m_FineFreq(DefaultLinearFlags,1, 0, 1.414, 0.000001, 0.0001),
	m_ModAmount(DefaultLinearFlags, 1.0f, 0, 2.0f, 0.001, 0.01),

	/* Voice State Properties */
	m_CyclePosInd(NewStateProperty(defCyclePos)),
	m_NoteInd(NewStateProperty(defNoteInd))
{
	RegisterSharedProperty(m_Type, "Wave Type", "Wave Type");
	RegisterSharedProperty(m_Octave, "Octave", "Octave");
	RegisterSharedProperty(m_FineFreq, "FineFreq", "FineFreq");
	RegisterSharedProperty(m_ModAmount, "ModAmount", "ModAmount");
}

bool WaveTable::CreatePorts()
{	
	for (UnsignedType n=0; n<NUM_TABLES; n++)
	{
		m_Table[n].Allocate(m_TableLength);
	}
	
	WriteWaves();
		
	frequency = new InputPort(this, "Frequency CV");
	output = new OutputPort(this, "Output");
	
	return true;
}

void WaveTable::WriteWaves()
{
	FloatType RadCycle = (M_PI/180)*360;
	FloatType Pos=0, v=0;
	FloatType HalfTab=m_TableLength/2;

	for (UnsignedType n=0; n<m_TableLength; n++)
	{
		if (n==0) Pos=0;
		else Pos=(n/(FloatType)m_TableLength)*RadCycle;
		m_Table[SINE].Set(n,sin(Pos));		

		if (n<HalfTab) 
		{
			m_Table[SQUARE].Set(n,1.0f);
			v=1-(n/HalfTab)*2.0f;
		}
		else 
		{
			m_Table[SQUARE].Set(n,-1);				
			v=(((n-HalfTab)/HalfTab)*2.0f)-1.0f;
		}
		
		m_Table[REVSAW].Set(n,((n/(FloatType)m_TableLength)*2.0f)-1.0f);

		m_Table[SAW].Set(n,1-(n/(FloatType)m_TableLength)*2.0f);
		
		m_Table[TRIANGLE].Set(n,v*0.99);		
		if (n<m_TableLength/1.2) 
			m_Table[PULSE1].Set(n,1);
		else 
			m_Table[PULSE1].Set(n,-1);				

		if (n<m_TableLength/1.5) 
			m_Table[PULSE2].Set(n,1);
		else 
			m_Table[PULSE2].Set(n,-1);				
	}

	Pos=0;
	for (UnsignedType n=0; n<m_TableLength; n++)
	{
		if (n==0) Pos=0;
		else Pos=(n/(FloatType)m_TableLength)*RadCycle;
		if (sin(Pos)==0) m_Table[INVSINE].Set(n,0);
		else m_Table[INVSINE].Set(n,(1.0f/sin(Pos))/10.0f);				
	}
}

void WaveTable::Reset()
{
	Device::Reset();

	for (UnsignedType n=0; n<NUM_TABLES; n++)
	{
		m_Table[n].Clear();
		m_Table[n].Allocate(m_TableLength);
	}
	
	WriteWaves();
}

void WaveTable::Process(UnsignedType SampleCount)
{
	int octave, type, Note;
 	FloatType Freq=0, fine, mod;
	FloatType Incr, CyclePos;
	
	type = m_Type.Value.AsUnsigned;
	octave = m_Octave.Value.AsSigned;
	fine = m_FineFreq.Value.AsFloat;
	mod = m_ModAmount.Value.AsFloat;

	CyclePos = StateValue(m_CyclePosInd)->AsFloat;
	Note = StateValue(m_NoteInd)->AsSigned;

	for (UnsignedType n=0; n<SampleCount; n++)
	{	
		output->SetSampleValue(n, 0, GetVoice());
	
		if (frequency->IsConnected()) 
		{
			Freq=0;

			if (!NUMBER_IS_HUGE(GetInput(frequency, n)))
				Freq=frequency->GetSamplePitch(n, GetVoice());

			Freq*=mod;
		}
		else 
		{	
			Freq=110;
		}

		Freq*=fine;
		if (octave>0) Freq*=1<<(octave);
		if (octave<0) Freq/=1<<(-octave);
		
		Incr = Freq*(m_TableLength/(FloatType)SampleRate());
		CyclePos+=Incr;
		
		if (CyclePos > m_TableLength)
			CyclePos = ((UnsignedType)floor(CyclePos) % m_TableLength) + (CyclePos - floor(CyclePos));

		CyclePos = MAX(CyclePos, 0);
		
		SetOutput(output, n, m_Table[type][CyclePos]);
	}

	StateValue(m_CyclePosInd)->AsFloat = CyclePos;
	StateValue(m_NoteInd)->AsSigned = Note;
}
