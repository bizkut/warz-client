//----------------------------------------------------------------------------------------------------------------------
/// \file UserData.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/UserData.h"
#include "XMD/Base.h"
#include "XMD/Model.h"

namespace XMD 
{
NMTL_POD_VECTOR_EXPORT(XUserData*,XMD_EXPORT,XM2);

//----------------------------------------------------------------------------------------------------------------------
XUserData::XUserData(XBase* ptr,const XString& data_id) 
{
  pdata = ptr;
  identifier = data_id;
  if(pdata)
    pdata->AddUserData(this);
}

//----------------------------------------------------------------------------------------------------------------------
XUserData::XUserData(const XUserData& ud)
{
  (void)ud;
}

//----------------------------------------------------------------------------------------------------------------------
XUserData::~XUserData() 
{
  if(pdata)
    pdata->RemUserData(this);
}

//----------------------------------------------------------------------------------------------------------------------
const XString& XUserData::GetIdentifier() const
{
  return identifier;
}
}
