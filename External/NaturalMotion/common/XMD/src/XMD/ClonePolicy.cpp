//----------------------------------------------------------------------------------------------------------------------
/// \file     ClonePolicy.cpp  (v2.0)
/// \note    (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------


#include "XMD/ClonePolicy.h"
#include <stdlib.h>

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XClonePolicy::XClonePolicy() 
{
  m_Flags[0]= DuplicateGeometryDeformers|
              DuplicateAllChildren      |
              DuplicateAllInstances     |
              DuplicateInstanceShapes   |
              LeaveCurrentParent        | 
              DuplicateParticleShapes   |
              DuplicateDynamicsFields ;
  m_Current=0;
}

//----------------------------------------------------------------------------------------------------------------------
XClonePolicy::~XClonePolicy() 
{
}

//----------------------------------------------------------------------------------------------------------------------
XClonePolicy::XClonePolicy(const XClonePolicy& dp) 
{
  memcpy(m_Flags,dp.m_Flags,XMD_MAX_CLONE_STACK*sizeof(XU32));
  m_Current = dp.m_Current;
}

//----------------------------------------------------------------------------------------------------------------------
void XClonePolicy::Enable(const XU32 flag)
{
  m_Flags[m_Current] |= flag;
}

//----------------------------------------------------------------------------------------------------------------------
void XClonePolicy::Disable(const XU32 flag) 
{
  m_Flags[m_Current] &= (~flag);
}

//----------------------------------------------------------------------------------------------------------------------
void XClonePolicy::EnableAll()
{
  Enable(0xFFFFFFFF);
}

//----------------------------------------------------------------------------------------------------------------------
void XClonePolicy::DisableAll()
{
  m_Flags[m_Current] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XClonePolicy::IsEnabled(const XU32 flag) const 
{
  return (m_Flags[m_Current] & flag) != 0;
}

//----------------------------------------------------------------------------------------------------------------------
void XClonePolicy::Push()
{
  ++m_Current;

  // if you assert here, you have Pushed more ClonePolicies on the stack
  // than this class can handle
  XMD_ASSERT( m_Current < (XMD_MAX_CLONE_STACK-1) );
  m_Flags[m_Current]=m_Flags[m_Current-1];
}

//----------------------------------------------------------------------------------------------------------------------
void XClonePolicy::Pop() 
{
  // if you assert here you have popped too many times.
  XMD_ASSERT( m_Current );
  --m_Current;
}
}
