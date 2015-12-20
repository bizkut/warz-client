//----------------------------------------------------------------------------------------------------------------------
/// \file CreatePolicy.cpp  (v2.0)
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/CreatePolicy.h"
#include <stdlib.h>

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XCreatePolicy::XCreatePolicy() 
{
  m_Flags[0]=NodeOnly;
  m_Current=0;
}

//----------------------------------------------------------------------------------------------------------------------
XCreatePolicy::~XCreatePolicy() 
{
}

//----------------------------------------------------------------------------------------------------------------------
XCreatePolicy::XCreatePolicy(const XCreatePolicy& dp) 
{
  memcpy(m_Flags,dp.m_Flags,XMD_MAX_CREATE_STACK*sizeof(XU32));
  m_Current = dp.m_Current;
}

//----------------------------------------------------------------------------------------------------------------------
void XCreatePolicy::Enable(const XU32 flag)
{
  m_Flags[m_Current] |= flag;
}

//----------------------------------------------------------------------------------------------------------------------
void XCreatePolicy::Disable(const XU32 flag) 
{
  m_Flags[m_Current] &= (~flag);
}

//----------------------------------------------------------------------------------------------------------------------
void XCreatePolicy::EnableAll()
{
  Enable(0xFFFFFFFF);
}

//----------------------------------------------------------------------------------------------------------------------
void XCreatePolicy::DisableAll()
{
  m_Flags[m_Current] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCreatePolicy::IsEnabled(const XU32 flag) const 
{
  return (m_Flags[m_Current] & flag) != 0;
}

//----------------------------------------------------------------------------------------------------------------------
void XCreatePolicy::Push()
{
  ++m_Current;

  // if you assert here, you have Pushed more CreatePolicies on the stack
  // than this class can handle
  XMD_ASSERT( m_Current < (XMD_MAX_CREATE_STACK-1) );
  m_Flags[m_Current]=m_Flags[m_Current-1];
}

//----------------------------------------------------------------------------------------------------------------------
void XCreatePolicy::Pop() 
{
  // if you assert here you have popped too many times.
  XMD_ASSERT( m_Current );
  --m_Current;
}
}
