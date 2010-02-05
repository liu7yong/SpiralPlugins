/*  SpiralSound
 *  Copyleft (C) 2002 Andy Preston <andy@clubunix.co.uk>
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
#include "LFO.h"

using namespace std;

static const int NUM_TABLES = 6;
static const int DEFAULT_TABLE_LEN = 1024;

//LFO Was initially commited by Dave, Sun Jul 28 23:18:18 2002 UTC
//md5 -s "Andy Preston::andy@clubunix.co.uk::1027916298::LFO"
#define device_id 09f642cbfdbb5022d229b3a44c343ce0// legacy == 7C
#define device_version 1

DevicePluginHook(LFO, device_id, device_version)

/*const DeviceDescription LFO::mDescription = 
{
  UniqueID       : LFO::mUniqueID,
  AudioDriver    : false,
  HostPlugin     : false,

  Author         : "Andy Preston",
  Version        : 1,
  Label          : "LFO",
  Info           : "LFO",
  Category       : "Oscillators",
  PluginInstance : DevicePluginHookName(device_id)
};*/

///////////////////////////////////////////////////////
enum Type {SINE=0, TRIANGLE, SQUARE, SAW};
const NumericPropertyValue WaveTypes[] = 
{
	/*{"Sine", */ DefaultUnsigned(SINE)/*}*/,
	/*{"Triangle", */ DefaultUnsigned(TRIANGLE)/*}*/,
	/*{"Square", */ DefaultUnsigned(SQUARE)/*}*/,
	/*{"Saw", */ DefaultUnsigned(SAW)/*}*/
};

const NumericPropertyValue defCyclePos= DefaultFloat(0.0f);

LFO::LFO(Patch *Host) :
	Device(Host),
	m_TableLength(DEFAULT_TABLE_LEN),

	/* Shared Properties */
	m_Type(Property::WriteOnly, 0, PropertySet(WaveTypes, sizeof(WaveTypes)/sizeof(WaveTypes[0]))),
	m_Freq(DefaultLinearFlags,0.1, 0.0, 1.0, 0.001, 0.1),

	/* Voice State Properties */
	m_CyclePosInd(NewStateProperty(defCyclePos))
{
	RegisterSharedProperty(m_Type, "Wave Type", "Wave Type");
	RegisterSharedProperty(m_Freq, "Frequency", "Frequency");
}

bool LFO::CreatePorts() 
{
	for (UnsignedType n=0; n<NUM_TABLES; n++)
	{
		m_Table[n].Allocate(m_TableLength);
	}
	
	WriteWaves();
		
	output[0] = new OutputPort(this, "Output");
	output[1] = new OutputPort(this, "'Cosine' Output");
	output[2] = new OutputPort(this, "Inverted Output");

	return true;
}

void LFO::WriteWaves() 
{
	FloatType RadCycle = (M_PI/180)*360;
	FloatType Pos = 0;
	FloatType v = 0;
	FloatType HalfTab = m_TableLength / 2;
	int QuatTab = m_TableLength / 4;
	int ThreeQuatTab = m_TableLength - QuatTab;
	int Shift;

	for (int n=0; n<m_TableLength; n++) 
	{
	    if (n==0) 
			Pos = 0; 
		else 
			Pos = (n/(FloatType)m_TableLength) * RadCycle;
			
	    m_Table[SINE].Set (n, sin (Pos));

	    if (n < QuatTab) 
			Shift=n+ThreeQuatTab; 
		else 
			Shift=n-QuatTab;
			
	    if (n<QuatTab || n>ThreeQuatTab) 
			v = (((Shift-HalfTab) / HalfTab) * 2) - 1;
	    else 
			v = 1 - (Shift/HalfTab * 2);
			
		m_Table[TRIANGLE].Set (n, v);

	    if (n<m_TableLength/2) 
			m_Table[SQUARE].Set (n, 1.0f);
	    else 
			m_Table[SQUARE].Set (n, -1);

	    m_Table[SAW].Set (n, 1.0f - (n / (FloatType)m_TableLength) * 2.0f);
	}
}

FloatType LFO::AdjustPos (FloatType pos) 
{
	if (pos > m_TableLength)
		pos = ((UnsignedType)floor(pos) % m_TableLength) + (pos - floor(pos));

	pos = MAX(pos, 0);
    
	return pos;
}

void LFO::Reset()
{
	Device::Reset();

	for (UnsignedType n=0; n<NUM_TABLES; n++)
	{
		m_Table[n].Clear();
		m_Table[n].Allocate(m_TableLength);
	}
	
	WriteWaves();
}

void LFO::Process(UnsignedType SampleCount) 
{
	UnsignedType type = m_Type.Value.AsUnsigned;
	FloatType freq =m_Freq.Value.AsFloat;
	FloatType Incr, CyclePos, Pos;

	CyclePos = StateValue(m_CyclePosInd)->AsFloat;

	for (UnsignedType n=0; n<SampleCount; n++) 
	{
		Incr = freq * (m_TableLength / (FloatType)SampleRate());

		// Raw Output
		CyclePos = AdjustPos (CyclePos + Incr);
		SetOutput (output[0], n, m_Table[type][CyclePos]);

		// 'Cosine' Output
		Pos = AdjustPos (CyclePos + (m_TableLength * 0.25));
		SetOutput (output[1], n, m_Table[type][Pos]);

		// Inverted Output
		Pos = AdjustPos (m_TableLength - CyclePos);
		SetOutput (output[2], n, m_Table[type][Pos]);
	}
}

