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

DevicePluginHook(Operator, OperatorID);

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

  m_Operator = SignedProperty::New(DefaultLinearFlags,0,0,3,1,1);
  RegisterSharedProperty(m_Operator, StringHash("Operator"));

  m_Constant = FloatProperty::New(DefaultLinearFlags,0,0,1,0.01f,0.1f);
  RegisterSharedProperty(m_Constant, StringHash("Constant"));

  return this;
}

bool Operator::CreatePorts() 
{
  InputPort::New(this/*, "Input 1"*/);
  InputPort::New(this/*, "Input 2"*/);
  OutputPort::New(this/*, "Output"*/);

  return true;
}

void Operator::Process(UnsignedType SampleCount)
{ 
  FloatType constant = m_Constant->Value.AsFloat;

  switch (m_Operator->Value.AsSigned) 
  {
    default:
    case 0 : 
      if (InputExists(GetInputPort(1)))
      {
        for (UnsignedType n=0; n<SampleCount; n++)
        {
          SetOutput(GetOutputPort(0),n,GetInput(GetInputPort(0),n)+GetInput(GetInputPort(1),n));
        } 
      }
      else
      {
        for (UnsignedType n=0; n<SampleCount; n++)
        {
          SetOutput(GetOutputPort(0),n,GetInput(GetInputPort(0),n)+constant);
        } 
      }
    break;
      
    case 1 : 
      if (InputExists(GetInputPort(1)))
      {
        for (UnsignedType n=0; n<SampleCount; n++)
        {
          SetOutput(GetOutputPort(0),n,GetInput(GetInputPort(0),n)-GetInput(GetInputPort(1),n));
        } 
      }
      else
      {
        for (UnsignedType n=0; n<SampleCount; n++)
        {
          SetOutput(GetOutputPort(0),n,GetInput(GetInputPort(0),n)-constant);
        } 
      }
    break;
      
    case 2 : 
      if (InputExists(GetInputPort(1)))
      {
        for (UnsignedType n=0; n<SampleCount; n++)
        {
          if (GetInput(GetInputPort(1),n)==0) SetOutput(GetOutputPort(0),n,0);
          else SetOutput(GetOutputPort(0),n,GetInput(GetInputPort(0),n)/GetInput(GetInputPort(1),n));
        } 
      }
      else
      {
        for (UnsignedType n=0; n<SampleCount; n++)
        {
          if (constant==0) SetOutput(GetOutputPort(0),n,0);
          else SetOutput(GetOutputPort(0),n,GetInput(GetInputPort(0),n)/constant);
        } 
      }
    break;
      
    case 3 :
      if (InputExists(GetInputPort(1)))
      {
        for (UnsignedType n=0; n<SampleCount; n++)
        {
          SetOutput(GetOutputPort(0),n,GetInput(GetInputPort(0),n)*GetInput(GetInputPort(1),n));
        }
      }
      else
      {
        for (UnsignedType n=0; n<SampleCount; n++)
        {
          SetOutput(GetOutputPort(0),n,GetInput(GetInputPort(0),n)*constant);
        }
      }
    break;
  }
}
