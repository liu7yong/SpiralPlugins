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
#include <SpiralCore/NoteTable.h>
#include "Trig.h"

static const FloatType RAD = (FloatType)(2.0f*M_PI);

using namespace Spiral;

//Initially commited by Dave, Tue Feb 4 19:57:26 2003 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1044401569::Trig"
//  =>  ede0519adcefedfb270bf0613c5fc210 (legacy == 0x35)
DevicePluginHook(Trig, ede0519adcefedfb270bf0613c5fc210);

///////////////////////////////////////////////////////

void Trig::Finalize()
{
  UnReference(m_Operator);
  Super::Finalize();
}

Trig* Trig::Initialize(Patch *Host)
{
  Super::Initialize(Host);

  m_Operator = SignedProperty::New(DefaultLinearFlags,1,0,3, 1, 1);
  RegisterSharedProperty(m_Operator, StringHash("Operator") /*"Operator", "Operator"*/);
  
  return this;
}

static const UnsignedType In = StringHash("Input", true);
static const UnsignedType Out = StringHash("Output", true);

bool Trig::CreatePorts() 
{

  return true;
}

void Trig::Process(UnsignedType SampleCount)
{ 
  SignedType op = m_Operator->Value.AsSigned;

  switch (op) 
  {
    case 1 : 
      for (UnsignedType n=0; n<SampleCount; n++)
      {
		SetOutput(GetOutputPort(Out),n,std::sin(GetInput(GetInputPort(In),n)*RAD));
      } 
      break;
      
    case 2 : 
      for (UnsignedType n=0; n<SampleCount; n++)
      {
		SetOutput(GetOutputPort(Out),n,std::cos(GetInput(GetInputPort(In),n)*RAD));
      } 
      break;
      
    case 3 :
      for (UnsignedType n=0; n<SampleCount; n++)
      {
		SetOutput(GetOutputPort(Out),n,std::tan(GetInput(GetInputPort(In),n)*RAD));
      }
      break;
      
    default: 
      break;
  }
}
