//----------------------------------------------------------------------------------------------------------------------
/// \file BaseProperties.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/BaseProperties.h"
#include "XMD/CompoundProperties.h"
#include "XMD/NodeProperties.h"

namespace XMD
{
XBaseProperties gXBaseProperties; 

//----------------------------------------------------------------------------------------------------------------------
XBaseProperties* XBase::GetProperties() const 
{
  return &gXBaseProperties;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::SetProperty(XBase*,const XU32,const XQuaternion&) 
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::SetProperty(XBase*,const XU32,const XVector3&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::SetProperty(XBase*,const XU32,const bool&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::SetProperty(XBase*,const XU32,const XReal&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::SetProperty(XBase*,const XU32,const XU32&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::SetProperty(XBase*,const XU32,const XS32&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::SetProperty(XBase*,const XU32,const XColour&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::SetProperty(XBase*,const XU32,const XMatrix&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::SetProperty(XBase*,const XU32,const XString&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::SetProperty(XBase*,const XU32,const XBase*&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::SetProperty(XBase*,const XU32,const XRealArray&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::SetProperty(XBase*,const XU32,const XU32Array&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::SetProperty(XBase*,const XU32,const XVector3Array&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::SetProperty(XBase*,const XU32,const XList&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::GetProperty(XBase*,const XU32,XQuaternion&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::GetProperty(XBase*,const XU32,XVector3&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::GetProperty(XBase*,const XU32,bool&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::GetProperty(XBase*,const XU32,XU32&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::GetProperty(XBase*,const XU32,XS32&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::GetProperty(XBase*,const XU32,XReal&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::GetProperty(XBase*,const XU32,XColour&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::GetProperty(XBase*,const XU32,XMatrix&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::GetProperty(XBase*,const XU32,XString&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::GetProperty(XBase*,const XU32,XBase*&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::GetProperty(XBase*,const XU32,XRealArray&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::GetProperty(XBase*,const XU32,XU32Array&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::GetProperty(XBase*,const XU32,XVector3Array&)
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBaseProperties::GetProperty(XBase*,const XU32,XList&) 
{ 
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
XString XBaseProperties::GetPropertyName(const XU32) 
{
  return ""; 
}

//----------------------------------------------------------------------------------------------------------------------
XPropertyType::Type XBaseProperties::GetPropertyType(const XU32) 
{ 
  return XPropertyType::kInvalid; 
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XBaseProperties::GetPropertyID(const XString&) 
{ 
  return 0; 
}

}
