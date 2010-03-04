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

#ifndef __Noise_H__
#define __Noise_H__

#include <SpiralCore/Misc.h>
#include <SpiralCore/Port.h>
#include <SpiralCore/Device.h>
#include <SpiralCore/Patch.h>

using namespace Spiral;

//Noise Was initially commited by Dave, Sun Jan 19 01:25:52 2003 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1042961152::Noise"
//  => 7cac72b3687628ec373ef10ae8a2ab4a (legacy == 32)
#define NoiseID 7cac72b3687628ec373ef10ae8a2ab4a

class Noise : Gumbo(Device)
{
  GumboClassDefinition(Noise, Device,
    {
      mUniqueID = String::New(XSTRINGIFY(NoiseID));
      mVersion = 1;
    },
    {
    },
  );
private:
  /* Instance State */
  OutputPort *output;

  // Voice Specific Properties (index)
  UnsignedType mB0,mB1,mB2,mB3,mB4,mB5,mB6;

  // Common Shared Properties
  SetProperty *m_Type;

  #define b(n) StateValue(CONCAT(mB, n))->AsFloat
  
  inline FloatType GenerateWhite()
  {
    return 1.0f-((rand()%INT_MAX)/(FloatType)INT_MAX)*2.0;
  }
  
  inline FloatType GeneratePink()
  {
    FloatType White = GenerateWhite()*0.2f, Pink;
    
    b(0) = 0.99886f * b(0) + White * 0.0555179f;
    b(1) = 0.99332f * b(1) + White * 0.0750759f;
    b(2) = 0.96900f * b(2) + White * 0.1538520f;
    b(3) = 0.86650f * b(3) + White * 0.3104856f;
    b(4) = 0.55000f * b(4) + White * 0.5329522f;
    b(5) = -0.7616f * b(5) - White * 0.0168980f;
    
    Pink = b(0) + b(1) + b(2) + b(3) + b(4) + b(5) + b(6) + White * 0.5362f;
    
    b(6) = White * 0.115926f;
    
    return Pink;
  }

  static inline FloatType randf (void) { return (FloatType)rand() / (FloatType)RAND_MAX; }

  /*static inline float
  reference (const float a, const float b)
  {
      float aplusb = a + b;
      if ((aplusb <= -1.0f) || (aplusb >= 1.0f))
          return a-b;
      return aplusb;
  }*/
  inline FloatType GenerateBrown()
  {
    FloatType white = 0.1f*(2.0f*randf() - 1.0f);

    union { FloatType f; UnsignedType i; } u, ub;
    u.f = b(0) + white;
    
    // Only keep the exponent, which is n+127 where n<0 if |a+b|<1.0.
    u.i &= 0xff<<23;
    // Normalize the exponent. The sign will match in both representations.
    u.i -= 127<<23;
    // Only keep the sign. Minus if |a+b|<1.0.
    u.i &= 1<<31;
    
    // Flip ub’s sign based on u, negating ub if |a+b|<1.0.
    ub.f = white;
    ub.i ^= u.i;
    
    b(0) -= ub.f;
    
    //b(0) = reference(b(0), white);
    
    return b(0);
  }

protected:
  virtual void Finalize();

public:
  virtual Noise *Initialize(Patch *Host);
  static inline Noise *New(Patch *Host) { return Alloc()->Initialize(Host); }

  bool CreatePorts();

  virtual void 	Process(UnsignedType SampleCount);
};

#endif
