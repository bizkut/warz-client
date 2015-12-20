//----------------------------------------------------------------------------------------------------------------------
/// \file     FileTranslator.cpp
/// \note    (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/FileTranslator.h"
#include "XMD/Model.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XFileTranslator::XFileTranslator() 
{
}

//----------------------------------------------------------------------------------------------------------------------
XFileTranslator::~XFileTranslator() 
{
}

//----------------------------------------------------------------------------------------------------------------------
bool XFileTranslator::Import(const XChar* filename,XModel& model) 
{
  (void)filename; 
  (void)model;
  return false;
} 

//----------------------------------------------------------------------------------------------------------------------
bool XFileTranslator::ImportAnim(const XChar* filename,XModel& model) 
{
  (void)filename;
  (void)model; 
  return false; 
} 

//----------------------------------------------------------------------------------------------------------------------
bool XFileTranslator::Export(const XChar* filename,XModel& model) 
{
  (void)filename;
  (void)model; 
  return false; 
} 

//----------------------------------------------------------------------------------------------------------------------
bool XFileTranslator::HasExport() const 
{
  return false; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XFileTranslator::HasImport() const 
{ 
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFileTranslator::HasImportAnim() const 
{ 
  return false;
}
}
