//----------------------------------------------------------------------------------------------------------------------
/// \file   Upgrading.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file provides a a few utility functions to upgrade older 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Model.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMU  
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief  updates all deprecated XMD::XMesh nodes into the newer XMD::XPolygonMesh node type. 
/// \param  model - the model to update
/// \return true if OK
XMD_EXPORT bool XMUUpdateMeshData(XMD::XModel& model);

//----------------------------------------------------------------------------------------------------------------------
/// \brief  updates all deprecated XMD::XAnimCycle nodes into the newer XMD::XAnimationTake node type. 
/// \param  model - the model to update
/// \return true if OK
XMD_EXPORT bool XMUUpdateAnimationData(XMD::XModel& model,const XReal tolerance=XM2_FLOAT_ZERO);
}
