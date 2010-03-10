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
#include <SpiralCore/NoteTable.h>
#include "Operator.h"

using namespace Spiral;

//Initially commited by Dave, Sun Jan 19 01:25:53 2003 UTC
//md5 -s "Dave Griffiths::dave@pawfal.org::1042939553::Operator"
//  =>  3bf687891e0216a806077e1ec1f4a4aa (legacy == 0x2C)
DevicePluginHook(Operator, 3bf687891e0216a806077e1ec1f4a4aa);

///////////////////////////////////////////////////////

void Operator::Finalize()
{
  UnReference(m_Operator);
  UnReference(m_Constant);

  Super::Finalize();
}

Operator *Operator::Initialize(Patch *Host)
{
  Super::Initialize(Host);

  m_Operator = NumberProperty<UnsignedType>::New(Property::WriteOnly,0,
                                                LinearConstraints<UnsignedType>::New(true, true, false, 0, 3, 1, 1));
  RegisterSharedProperty(m_Operator, StringHash("Operator"));

  m_Constant = NumberProperty<FloatType>::New(Property::WriteOnly,0,
                                               LinearConstraints<FloatType>::New(true, true, false, 0,1,0.01f,0.1f));
  RegisterSharedProperty(m_Constant, StringHash("Constant"));

  return this;
}

static const UnsignedType In1 = StringHash("Input 1", true);
static const UnsignedType In2 = StringHash("Input 2", true);

static const UnsignedType Out = StringHash("Output", true);

bool Operator::CreatePorts() 
{
  return true;
}

void Operator::Process(UnsignedType SampleCount)
{ 
  FloatType constant = m_Constant->Value();

  switch (m_Operator->Value()) 
  {
    default:
    case 0 : 
      if (InputExists(GetInputPort(In2)))
      {
        for (UnsignedType n=0; n<SampleCount; n++)
        {
          SetOutput(GetOutputPort(Out),n,GetInput(GetInputPort(In1),n)+GetInput(GetInputPort(In2),n));
        } 
      }
      else
      {
        for (UnsignedType n=0; n<SampleCount; n++)
        {
          SetOutput(GetOutputPort(Out),n,GetInput(GetInputPort(In1),n)+constant);
        } 
      }
    break;
      
    case 1 : 
      if (InputExists(GetInputPort(In2)))
      {
        for (UnsignedType n=0; n<SampleCount; n++)
        {
          SetOutput(GetOutputPort(Out),n,GetInput(GetInputPort(In1),n)-GetInput(GetInputPort(In2),n));
        } 
      }
      else
      {
        for (UnsignedType n=0; n<SampleCount; n++)
        {
          SetOutput(GetOutputPort(Out),n,GetInput(GetInputPort(In1),n)-constant);
        } 
      }
    break;
      
    case 2 : 
      if (InputExists(GetInputPort(In2)))
      {
        for (UnsignedType n=0; n<SampleCount; n++)
        {
          if (GetInput(GetInputPort(In2),n)==0) SetOutput(GetOutputPort(Out),n,0);
          else SetOutput(GetOutputPort(Out),n,GetInput(GetInputPort(In1),n)/GetInput(GetInputPort(In2),n));
        } 
      }
      else
      {
        for (UnsignedType n=0; n<SampleCount; n++)
        {
          if (constant==0) SetOutput(GetOutputPort(Out),n,0);
          else SetOutput(GetOutputPort(Out),n,GetInput(GetInputPort(In1),n)/constant);
        } 
      }
    break;
      
    case 3 :
      if (InputExists(GetInputPort(In2)))
      {
        for (UnsignedType n=0; n<SampleCount; n++)
        {
          SetOutput(GetOutputPort(Out),n,GetInput(GetInputPort(In1),n)*GetInput(GetInputPort(In2),n));
        }
      }
      else
      {
        for (UnsignedType n=0; n<SampleCount; n++)
        {
          SetOutput(GetOutputPort(Out),n,GetInput(GetInputPort(In1),n)*constant);
        }
      }
    break;
  }
}
