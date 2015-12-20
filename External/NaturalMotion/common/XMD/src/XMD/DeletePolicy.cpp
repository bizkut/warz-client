//----------------------------------------------------------------------------------------------------------------------
/// \file DeletePolicy.cpp  (v2.0)
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/DeletePolicy.h"
#include <stdlib.h>

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XDeletePolicy::XDeletePolicy() 
{
  m_Flags[0]=0xFFFFFFFF;
  m_Current=0;
}

//----------------------------------------------------------------------------------------------------------------------
XDeletePolicy::~XDeletePolicy() 
{
}

//----------------------------------------------------------------------------------------------------------------------
XDeletePolicy::XDeletePolicy(const XDeletePolicy& dp) 
{
  memcpy(m_Flags,dp.m_Flags,XMD_MAX_DELETE_STACK*sizeof(XU32));
  m_Current = dp.m_Current;
}

//----------------------------------------------------------------------------------------------------------------------
void XDeletePolicy::Enable(const XU32 flag) 
{
  m_Flags[m_Current] |= flag;
}

//----------------------------------------------------------------------------------------------------------------------
void XDeletePolicy::Disable(const XU32 flag) 
{
  m_Flags[m_Current] &= (~flag);
}

//----------------------------------------------------------------------------------------------------------------------
void XDeletePolicy::EnableAll() 
{
  m_Flags[m_Current] = 0xFFFFFFFF;
}

//----------------------------------------------------------------------------------------------------------------------
void XDeletePolicy::DisableAll() 
{
  m_Flags[m_Current] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XDeletePolicy::IsEnabled(const XU32 flag) const 
{
  return (m_Flags[m_Current] & flag) != 0;
}

//----------------------------------------------------------------------------------------------------------------------
void XDeletePolicy::Push() 
{
  ++m_Current;
  // if you assert here, you have Pushed more XDeletePolicy's on the stack
  // than this class can handle
  XMD_ASSERT( m_Current < (XMD_MAX_DELETE_STACK-1) );
  m_Flags[m_Current]=m_Flags[m_Current-1];
}

//----------------------------------------------------------------------------------------------------------------------
void XDeletePolicy::Pop() 
{
  // if you assert here you have popped too many times.
  XMD_ASSERT( m_Current );
  --m_Current;
}
}
