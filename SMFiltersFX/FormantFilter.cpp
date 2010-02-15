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
#include <SpiralCore/Misc.h>

#include "FormantFilter.h"
//#include "SpiralIcon.xpm"

using namespace std;
using namespace Spicy;
using namespace Spiral;

/*
Public source code by alex@smartelectronix.com
Simple example of implementation of formant filter
Vowelnum can be 0,1,2,3,4 <=> A,E,I,O,U
Good for spectral rich input like saw or square
*/

//-------------------------------------------------------------VOWEL COEFFICIENTS
const double coeff[5][11]= {
{ 8.11044e-06,
8.943665402, -36.83889529, 92.01697887, -154.337906, 181.6233289,
-151.8651235,   89.09614114, -35.10298511, 8.388101016, -0.923313471  ///A
},
{4.36215e-06,
8.90438318, -36.55179099, 91.05750846, -152.422234, 179.1170248,  ///E
-149.6496211,87.78352223, -34.60687431, 8.282228154, -0.914150747
},
{ 3.33819e-06,
8.893102966, -36.49532826, 90.96543286, -152.4545478, 179.4835618,
-150.315433, 88.43409371, -34.98612086, 8.407803364, -0.932568035  ///I
},
{1.13572e-06,
8.994734087, -37.2084849, 93.22900521, -156.6929844, 184.596544,   ///O
-154.3755513, 90.49663749, -35.58964535, 8.478996281, -0.929252233
},
{4.09431e-07,
8.997322763, -37.20218544, 93.11385476, -156.2530937, 183.7080141,  ///U
-153.2631681, 89.59539726, -35.12454591, 8.338655623, -0.910251753
}
};
//---------------------------------------------------------------------------------

//Initially commited by Dave, Thu Jan 2 01:56:48 2003 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1041494208::FormantFilter"
#define device_id 78bd5c2e8967c372c264d5745359fdf5// legacy == 2A
#define device_version 1
DevicePluginHook(FormantFilter, device_id, device_version)

/*const DeviceDescription FormantFilter::mDescription = 
{
  UniqueID       : FormantFilter::mUniqueID,
  AudioDriver    : false,
  HostPlugin     : false,

  Author         : "David Griffiths",
  Version        : 1,
  Label          : "Formant Filter",
  Info           : "Formant Filter",
  Category       : "Filters/FX",
  PluginInstance : DevicePluginHookName(device_id)
};*/

///////////////////////////////////////////////////////
const FloatType zero = 0.0; 
const FloatType one = 1.0; 
const FloatType two = 2.0; 
const FloatType three = 3.0; 
const FloatType four = 4.0; 
const FloatType five = 5.0; 
const FloatType six = 6.0; 

FormantFilter::FormantFilter(Patch *Host) :
Device(Host),
output(NULL),
m_Vowel(new FloatProperty(DefaultLinearFlags,zero, zero, four, 0.0001, 0.001))
{
	RegisterSharedProperty(m_Vowel, StringHash("VOWEL")/*"Vowel", "Vowel"*/);
}

bool FormantFilter::CreatePorts()
{
	input[0] = new InputPort(this/*,  "Input"*/);
	input[1] = new InputPort(this/*, "Vowel CV"*/);
	output = new OutputPort(this/*, "Output"*/);

	return true;
}

void FormantFilter::Reset()
{
	Device::Reset();
	WipeMemory();
}

void FormantFilter::Process(UnsignedType SampleCount)
{
	FloatType res,o[5], in=zero, vow=m_Vowel->Value.AsFloat;
	
	//reset memory if disconnected, and skip out (prevents CPU spike)
	if (! InputExists(input[0])) 
	{	
		WipeMemory();
		GetOutputBuffer(output)->Zero();

		return;
	}

	for (UnsignedType n=0; n<SampleCount; n++)
	{		
		in = GetInput(input[0],n);
		
		if (NUMBER_IS_INSANE(in))
			in = zero;

		for (int v=0; v<5; v++)
		{
			res= (FloatType) (coeff[v][0]*(in*0.1f) +
					  coeff[v][1]*memory[v][0] +  
					  coeff[v][2]*memory[v][1] +
					  coeff[v][3]*memory[v][2] +
					  coeff[v][4]*memory[v][3] +
					  coeff[v][5]*memory[v][4] +
					  coeff[v][6]*memory[v][5] +
					  coeff[v][7]*memory[v][6] +
					  coeff[v][8]*memory[v][7] +
					  coeff[v][9]*memory[v][8] +
					  coeff[v][10]*memory[v][9] );

			memory[v][9]=memory[v][8];
			memory[v][8]=memory[v][7];
			memory[v][7]=memory[v][6];
			memory[v][6]=memory[v][5];
			memory[v][5]=memory[v][4];
			memory[v][4]=memory[v][3];
			memory[v][3]=memory[v][2];
			memory[v][2]=memory[v][1];
			memory[v][1]=memory[v][0];
		
			// work around denormal calculation CPU spikes where res --> 0
			if (NUMBER_IS_INSANE(res))
				res = zero;

			memory[v][0]=res;
			o[v]=res;
		}

		if (InputExists(input[1])) 
		{	
			in = fabs(GetInput(input[1],n));
			/* When old files can be imported with the knob reset to zero 
				if cv /was/ wired, this should be vow+= */
			vow=in*four;
		}
	
		// mix between vowel sounds
		if (vow==zero) 
		{
			SetOutput(output,n,o[0]);	 
			continue;
		}

		if (vow<one) 
		{
			SetOutput(output, n, Linear(zero,one,vow,o[1],o[0]));
			continue; 
		}	

		if (vow==one) 
		{
			SetOutput(output,n,o[1]);	 
			continue;
		}

		if (vow<two) 
		{
			SetOutput(output, n, Linear(zero,one,vow-one,o[2],o[1]));
			continue;
		}	

		if (vow==two) 
		{
			SetOutput(output,n,o[2]);	 
			continue;
		}

		if (vow<three) 
		{
			SetOutput(output, n, Linear(zero,one,vow-two,o[3],o[2]));
			continue;
		}	

		if (vow==three) 
		{
			SetOutput(output,n,o[3]);	 
			continue;
		}

		if (vow<four) 
		{
			SetOutput(output, n, Linear(zero,one,vow-three,o[4],o[3]));
			continue;
		}	

		SetOutput(output,n,o[4]);	 
	}
		
}
	
void FormantFilter::WipeMemory()
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			memory[i][j] = 0;
		}
	}
}
