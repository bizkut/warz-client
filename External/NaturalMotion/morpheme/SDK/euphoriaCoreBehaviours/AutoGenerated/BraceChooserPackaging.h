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

#ifndef NM_MDF_BRACECHOOSER_PKG_H
#define NM_MDF_BRACECHOOSER_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/BraceChooser.module"

#include "BraceChooserData.h"

#include "BodySectionPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct BraceChooserAPIBase
{
  BraceChooserAPIBase(
    const BraceChooserFeedbackInputs* const _feedIn, 
    const BodySectionAPIBase* const _owner  ) :feedIn(_feedIn) ,owner(_owner)  {}

  const BraceChooserFeedbackInputs* const feedIn;

  const BodySectionAPIBase* const owner;

  BraceChooserAPIBase(const BraceChooserAPIBase& rhs);
  BraceChooserAPIBase& operator = (const BraceChooserAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct BraceChooserUpdatePackage : public BraceChooserAPIBase
{
  BraceChooserUpdatePackage(
    const BraceChooserFeedbackInputs* const _feedIn, 
    BraceChooserOutputs* const _out, 
    const BodySectionAPIBase* const _owner  ) : BraceChooserAPIBase(_feedIn ,_owner ), out(_out) 
  {
  }


  BraceChooserOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  BraceChooserUpdatePackage(const BraceChooserUpdatePackage& rhs);
  BraceChooserUpdatePackage& operator = (const BraceChooserUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct BraceChooserFeedbackPackage : public BraceChooserAPIBase
{
  BraceChooserFeedbackPackage(
    const BraceChooserFeedbackInputs* const _feedIn, 
    const BodySectionAPIBase* const _owner  ) : BraceChooserAPIBase(_feedIn ,_owner ) 
  {
  }




  // No update api required for this module!


  BraceChooserFeedbackPackage(const BraceChooserFeedbackPackage& rhs);
  BraceChooserFeedbackPackage& operator = (const BraceChooserFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BRACECHOOSER_PKG_H

