//----------------------------------------------------------------------------------------------------------------------
/// \file   NodeProperties.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a Wrap deformation
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Model.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief  defines a set of flags to determine attributes of a specific
///         property, i.e. is it animatable, is it read only etc.
//---------------------------------------------------------------------------------------------------------------------- 
class XPropertyFlags
{
public:
  /// \brief  property can be animated 
  static const XU32 kAnimatable=0x01;
  /// \brief  property is read only - i.e. it cannot be set since it's value is generated
  static const XU32 kReadOnly=0x02;
  /// \brief  property is an array that can be re-sized.
  static const XU32 kArrayIsResizable=0x04;
};
}
