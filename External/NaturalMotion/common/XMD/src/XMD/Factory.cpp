//----------------------------------------------------------------------------------------------------------------------
/// \file Factory.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Model.h"
#include "XMD/Factory.h"
#include "XMD/NodeTypeRegister.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XFn::Type XFactory::HeaderToType(const XString& chunk_header) 
{
  DUMPER(XFactory__HeaderToType);
  return XGlobal::NodeTypeFromChunkName(chunk_header.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
XString XFactory::TypeToHeader(const XFn::Type type) 
{
  DUMPER(XFactory__TypeToHeader);
  return XGlobal::ChunkNameFromNodeType(type);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XFactory::CreateNode(XModel* pmod,const XFn::Type type) 
{
  DUMPER(XFactory__CreateNode);
  return XGlobal::CreateFromId(type,pmod);
}
}
