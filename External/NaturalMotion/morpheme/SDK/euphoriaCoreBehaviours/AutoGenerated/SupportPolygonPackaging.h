#pragma once

/*
 * Copyright (c) 2013 NaturalMotion Ltd. All rights reserved.
 *
 * Not to be copied, adapted, modified, used, distributed, sold,
 * licensed or commercially exploited in any manner without the
 * written consent of NaturalMotion.
 *
 * All non public elements of this software are the confidential
 * information of NaturalMotion and may not be disclosed to any
 * person nor used for any purpose not expressly approved by
 * NaturalMotion in writing.
 *
 */

//----------------------------------------------------------------------------------------------------------------------
//                                  This file is auto-generated
//----------------------------------------------------------------------------------------------------------------------

#ifndef NM_MDF_SUPPORTPOLYGON_PKG_H
#define NM_MDF_SUPPORTPOLYGON_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/SupportPolygon.module"

#include "SupportPolygonData.h"

#include "BodyFramePackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct SupportPolygonAPIBase
{
  SupportPolygonAPIBase(
    const SupportPolygonData* const _data, 
    const SupportPolygonInputs* const _in, 
    const SupportPolygonFeedbackInputs* const _feedIn, 
    const BodyFrameAPIBase* const _owner  ) :data(_data) ,in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const SupportPolygonData* const data;
  const SupportPolygonInputs* const in;
  const SupportPolygonFeedbackInputs* const feedIn;

  const BodyFrameAPIBase* const owner;

  SupportPolygonAPIBase(const SupportPolygonAPIBase& rhs);
  SupportPolygonAPIBase& operator = (const SupportPolygonAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct SupportPolygonUpdatePackage : public SupportPolygonAPIBase
{
  SupportPolygonUpdatePackage(
    SupportPolygonData* const _data, 
    const SupportPolygonInputs* const _in, 
    const SupportPolygonFeedbackInputs* const _feedIn, 
    SupportPolygonOutputs* const _out, 
    const BodyFrameAPIBase* const _owner  ) : SupportPolygonAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), out(_out) 
  {
  }

  SupportPolygonData* const data;

  SupportPolygonOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  SupportPolygonUpdatePackage(const SupportPolygonUpdatePackage& rhs);
  SupportPolygonUpdatePackage& operator = (const SupportPolygonUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct SupportPolygonFeedbackPackage : public SupportPolygonAPIBase
{
  SupportPolygonFeedbackPackage(
    SupportPolygonData* const _data, 
    const SupportPolygonFeedbackInputs* const _feedIn, 
    const SupportPolygonInputs* const _in, 
    const BodyFrameAPIBase* const _owner  ) : SupportPolygonAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data) 
  {
  }

  SupportPolygonData* const data;



  // No update api required for this module!


  SupportPolygonFeedbackPackage(const SupportPolygonFeedbackPackage& rhs);
  SupportPolygonFeedbackPackage& operator = (const SupportPolygonFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SUPPORTPOLYGON_PKG_H

