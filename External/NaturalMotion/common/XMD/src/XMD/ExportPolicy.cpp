//----------------------------------------------------------------------------------------------------------------------
/// \file ExportPolicy.cpp  (v2.0)
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/ExportPolicy.h"
#include <stdlib.h>

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XExportPolicy::XExportPolicy() 
{
  m_Flags[0]=WriteBones|
             WriteShapes|
             WriteMaterials|
             WriteTextures|
             WriteParticles|
             WriteDeformers|
             WriteAttributes|
             WriteAnimations |
             WriteAscii;
  m_Current=0;
}

//----------------------------------------------------------------------------------------------------------------------
XExportPolicy::~XExportPolicy() 
{
}

//----------------------------------------------------------------------------------------------------------------------
XExportPolicy::XExportPolicy(const XExportPolicy& dp)
{
  memcpy(m_Flags,dp.m_Flags,XMD_MAX_EXPORT_STACK*sizeof(XU32));
  m_Current = dp.m_Current;
}

//----------------------------------------------------------------------------------------------------------------------
void XExportPolicy::Enable(const XU32 flag)
{
  m_Flags[m_Current] |= flag;
}

//----------------------------------------------------------------------------------------------------------------------
void XExportPolicy::Disable(const XU32 flag)
{
  m_Flags[m_Current] &= (~flag);
}

//----------------------------------------------------------------------------------------------------------------------
void XExportPolicy::EnableAll() 
{
  m_Flags[m_Current] = 0xFFFFFFFF;
}

//----------------------------------------------------------------------------------------------------------------------
void XExportPolicy::DisableAll() 
{
  m_Flags[m_Current] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XExportPolicy::IsEnabled(const XU32 flag) const 
{
  return (m_Flags[m_Current] & flag) != 0;
}

//----------------------------------------------------------------------------------------------------------------------
void XExportPolicy::Push()
{
  ++m_Current;
  // if you assert here, you have Pushed more XExportPolicy's on the stack
  // than this class can handle
  XMD_ASSERT( m_Current < (XMD_MAX_EXPORT_STACK-1) );
  m_Flags[m_Current]=m_Flags[m_Current-1];
}

//----------------------------------------------------------------------------------------------------------------------
void XExportPolicy::Pop()
{
  // if you assert here you have popped too many times.
  XMD_ASSERT( m_Current );
  --m_Current;
}
}
