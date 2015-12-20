//----------------------------------------------------------------------------------------------------------------------
/// \file NodeTypeRegister.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/XFn.h"
#include "XMD/Logger.h"
#include "XMD/NodeTypeRegister.h"
#include "XMD/FileTranslator.h"
#include <iostream>

namespace XMD 
{
XBasicLogger* XGlobal::gLogger = &gDefaultLogger;
XStdOutLogger XGlobal::gDefaultLogger;
XGlobal::InternalData* XGlobal::gInternal = 0;

//----------------------------------------------------------------------------------------------------------------------
XFileTranslatorRegister::~XFileTranslatorRegister() 
{
  delete m_Translator;
  m_Translator=0;
}

//----------------------------------------------------------------------------------------------------------------------
XFileTranslatorRegister::XFileTranslatorRegister() 
{
  m_Translator=0;
}

//----------------------------------------------------------------------------------------------------------------------
XBasicLogger* XGlobal::GetLogger()
{
  XMD_ASSERT(gLogger != 0);
  return gLogger;
}

//----------------------------------------------------------------------------------------------------------------------
void XGlobal::SetLogger(XBasicLogger* pLogger)
{
  if(pLogger)
    gLogger = pLogger;
  else
    gLogger = &gDefaultLogger;
}

//----------------------------------------------------------------------------------------------------------------------
XString XGlobal::GetXmdHeader()
{
  XString str = gInternal->ident;
  str += "#";
  str += gInternal->version;
  str += "|";
  return str;
}

//----------------------------------------------------------------------------------------------------------------------
void XGlobal::SetVersionIdentifier(const XChar* version)
{
  size_t len = strlen(version);
  if (len>4)
  {
    gInternal->ident = version;
    gInternal->ident.resize(4);
  }
  else
  if (len<4)
  {
    gInternal->ident="";
    for(size_t i=0;i!=(4-len);++i)
    {
      gInternal->ident += "0";
    }
    gInternal->ident += version;
  }
  else
    gInternal->ident = version;
}

//----------------------------------------------------------------------------------------------------------------------
void XGlobal::SetVersion(const XChar* version)
{
  size_t len = strlen(version);
  if (len>8)
  {
    gInternal->version = version;
    gInternal->version.resize(8);
  }
  else
  if (len<8)
  {
    gInternal->version="";
    for(size_t i=0;i!=(8-len);++i)
    {
      gInternal->version += "0";
    }
    gInternal->version += version;
  }
  else
    gInternal->version = version;
}

//----------------------------------------------------------------------------------------------------------------------
XGlobal::InternalData::InternalData() 
  : m_RegisteredTypes(), m_RegisteredTranslators()
{  
  version = "00000000";
  ident = "0000";
}

//----------------------------------------------------------------------------------------------------------------------
void XGlobal::InternalData::RegisterType(IXCreateBase* type)
{
  m_RegisteredTypes.push_back(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XGlobal::InternalData::RegisterTranslator(XFileTranslatorRegister* type)
{
  m_RegisteredTranslators.push_back(type);
}

//----------------------------------------------------------------------------------------------------------------------
IXCreateBase* XGlobal::InternalData::FindType(XFn::Type t)
{
  XM2::pod_vector<IXCreateBase*>::iterator it = m_RegisteredTypes.begin();
  for (; it != m_RegisteredTypes.end(); ++it) 
  {
    IXCreateBase* cb = *it;

    // a NULL type info has been found. This should not happen
    XMD_ASSERT(cb);
    if (cb->GetType() == t) 
    {
      return cb;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
IXCreateBase* XGlobal::InternalData::FindTypeFromApiStr(XString t)
{
  XM2::pod_vector<IXCreateBase*>::iterator it = m_RegisteredTypes.begin();
  for (; it != m_RegisteredTypes.end(); ++it) 
  {
    IXCreateBase* cb = *it;
    // a NULL type info has been found. This should not happen
    XMD_ASSERT(cb);
    if (cb->GetTypeString() == t) 
    {
      return cb;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
IXCreateBase* XGlobal::InternalData::FindTypeFromChunkName(XString cn)
{
  XM2::pod_vector<IXCreateBase*>::iterator it = m_RegisteredTypes.begin();
  for (; it != m_RegisteredTypes.end(); ++it) 
  {
    IXCreateBase* cb = *it;
    // a NULL type info has been found. This should not happen
    XMD_ASSERT(cb);
    if (cb->GetChunkName() == cn) 
    {
      return cb;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XGlobal::RegisterTypeInfo(IXCreateBase* type)
{
  if(!gInternal) return false;
  if( gInternal->FindType(type->GetType()) ||
      gInternal->FindTypeFromApiStr(type->GetTypeString()) )

  {
    XGlobal::GetLogger()->Logf(XBasicLogger::kError,
      "Type info clashes with previous entry ", type->GetTypeString().c_str());
    return false;
  }
  gInternal->RegisterType(type);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XGlobal::NodeTypeFromChunkName(const XChar* chunk_name)
{
  if(!gInternal) return XFn::Last;
  IXCreateBase* cb = gInternal->FindTypeFromChunkName(chunk_name);
  if (cb) 
  {
    return cb->GetType();
  }
  return XFn::Base;
}

//----------------------------------------------------------------------------------------------------------------------
XString XGlobal::ChunkNameFromNodeType(const XFn::Type type_id)
{
  if(!gInternal) return "invalid";
  IXCreateBase* cb = gInternal->FindType(type_id);
  if (cb) 
  {
    return cb->GetChunkName();
  }
  switch(type_id) 
  {
  case XFn::Info:
    return "INFO";
    break;
  default:
    break;
  }
  return XString();
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XGlobal::NodeTypeFromApiTypeStr(const XChar* type_name)
{
  if(!gInternal) return XFn::Last;
  IXCreateBase* cb = gInternal->FindTypeFromApiStr(type_name);
  if (cb) 
  {
    return cb->GetType();
  }
  return XFn::Base;
}

//----------------------------------------------------------------------------------------------------------------------
XString XGlobal::ApiTypeStrFromNodeType(const XFn::Type type_id)
{
  if(!gInternal) return "invalid";
  IXCreateBase* cb = gInternal->FindType(type_id);
  if (cb) 
  {
    return cb->GetTypeString();
  }
  return XString();
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XGlobal::CreateFromId(XFn::Type type,XModel* mod)
{
  if(!gInternal) return 0;
  IXCreateBase* cb = gInternal->FindType(type);
  if (cb) 
  {
    return cb->CreateNode(mod);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XGlobal::CreateFromChunkName(const XChar* type,XModel* mod)
{
  if(!gInternal) return 0;
  IXCreateBase* cb = gInternal->FindTypeFromChunkName(type);
  if (cb) 
  {
    return cb->CreateNode(mod);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XGlobal::CreateFromApiStr(const XChar* type,XModel* mod)
{
  if(!gInternal) return 0;
  IXCreateBase* cb = gInternal->FindTypeFromApiStr(type);
  if (cb) 
  {
    return cb->CreateNode(mod);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XGlobal::InternalData::~InternalData()
{
  XM2::pod_vector<IXCreateBase*>::iterator it = m_RegisteredTypes.begin();
  for (; it != m_RegisteredTypes.end(); ++it) 
  {
    IXCreateBase* cb = *it;
    delete cb;
  }
  m_RegisteredTypes.clear();


  XM2::pod_vector<XFileTranslatorRegister*>::iterator ii = m_RegisteredTranslators.begin();
  for (;ii != m_RegisteredTranslators.end();++ii) 
  {
    delete *ii;
  }
  m_RegisteredTranslators.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void XGlobal::Init(XBasicLogger* logger)
{
  delete gInternal;
  gInternal = new InternalData();
  SetLogger(logger);
}

//----------------------------------------------------------------------------------------------------------------------
void XGlobal::Cleanup()
{
  delete gInternal;
  gInternal=0;
}

//----------------------------------------------------------------------------------------------------------------------
XFileTranslator* XGlobal::InternalData::GetFileTranslatorForExt(const XChar* fname)
{
  XString EXT;
  XU32 l = (XU32)strlen(fname);
  EXT += (XChar)tolower(fname[l-3]);
  EXT += (XChar)tolower(fname[l-2]);
  EXT += (XChar)tolower(fname[l-1]);

  XM2::pod_vector<XFileTranslatorRegister*>::iterator it =  m_RegisteredTranslators.begin();
  for ( ; it !=  m_RegisteredTranslators.end(); ++it ) 
  {
    if ((*it)->m_Extension == EXT) 
    {
      return (*it)->m_Translator;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XGlobal::RegisterTranslator(XFileTranslator* translator,const XChar* ext)
{
  if(!gInternal) return false;
  XFileTranslatorRegister* ptr = new XFileTranslatorRegister;
  ptr->m_Extension=ext;
  ptr->m_Extension[0] = (XChar)tolower(ptr->m_Extension[0]);
  ptr->m_Extension[1] = (XChar)tolower(ptr->m_Extension[1]);
  ptr->m_Extension[2] = (XChar)tolower(ptr->m_Extension[2]);
  ptr->m_Translator = translator;
  gInternal->RegisterTranslator(ptr);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XGlobal::Import(const XChar* filename,XModel& model)
{
  if(!gInternal) return false;
  XFileTranslator* translator = gInternal->GetFileTranslatorForExt(filename);
  if (translator && translator->HasImport()) 
  {
    return translator->Import(filename,model);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XGlobal::ImportAnim(const XChar* filename,XModel& model)
{
  if(!gInternal) return false;
  XFileTranslator* translator = gInternal->GetFileTranslatorForExt(filename);
  if (translator && translator->HasImportAnim()) 
  {
    return translator->ImportAnim(filename,model);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XGlobal::Export(const XChar* filename,XModel& model)
{
  if(!gInternal) return false;
  XFileTranslator* translator = gInternal->GetFileTranslatorForExt(filename);
  if (translator && translator->HasExport()) 
  {
    return translator->Export(filename,model);
  }
  return false;
}
}
