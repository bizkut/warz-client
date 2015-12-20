//----------------------------------------------------------------------------------------------------------------------
/// \file ImportPolicy.cpp  (v2.0)
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/ImportPolicy.h"
#include <stdlib.h>

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XImportPolicy::XImportPolicy() 
{
  m_Flags[0] = ImportBones | 
               ImportShapes | 
               ImportMaterials | 
               ImportTextures | 
               ImportParticles | 
               ImportDeformers | 
               ImportAttributes;
  m_Current=0;
}

//----------------------------------------------------------------------------------------------------------------------
XImportPolicy::~XImportPolicy() 
{
}

//----------------------------------------------------------------------------------------------------------------------
XImportPolicy::XImportPolicy(const XImportPolicy& dp)
{
  memcpy(m_Flags,dp.m_Flags,XMD_MAX_IMPORT_STACK*sizeof(XU32));
  m_Current = dp.m_Current;
}

//----------------------------------------------------------------------------------------------------------------------
void XImportPolicy::Enable(const XU32 flag)
{
  m_Flags[m_Current] |= flag;
}

//----------------------------------------------------------------------------------------------------------------------
void XImportPolicy::Disable(const XU32 flag)
{
  m_Flags[m_Current] &= (~flag);
}

//----------------------------------------------------------------------------------------------------------------------
void XImportPolicy::EnableAll() 
{
  m_Flags[m_Current] = 0xFFFFFFFF;
}

//----------------------------------------------------------------------------------------------------------------------
void XImportPolicy::DisableAll() 
{
  m_Flags[m_Current] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XImportPolicy::IsEnabled(const XU32 flag) const 
{
  return (m_Flags[m_Current] & flag) != 0;
}

//----------------------------------------------------------------------------------------------------------------------
void XImportPolicy::Push()
{
  ++m_Current;
  // if you assert here, you have Pushed more XImportPolicy's on the stack
  // than this class can handle
  XMD_ASSERT( m_Current < (XMD_MAX_IMPORT_STACK-1) );
  m_Flags[m_Current]=m_Flags[m_Current-1];
}

//----------------------------------------------------------------------------------------------------------------------
void XImportPolicy::Pop()
{
  // if you assert here you have popped too many times.
  XMD_ASSERT( m_Current );
  --m_Current;
}
}
