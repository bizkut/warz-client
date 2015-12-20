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

#ifndef NM_MDF_BODYSECTION_PKG_H
#define NM_MDF_BODYSECTION_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/BodySection.module"

#include "BodySectionData.h"
#include "RotateCoreData.h"
#include "PositionCoreData.h"
#include "BraceChooserData.h"

#include "MyNetworkPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

struct RotateCoreUpdatePackage;
struct RotateCoreFeedbackPackage;
class RotateCore_Con;
struct PositionCoreUpdatePackage;
struct PositionCoreFeedbackPackage;
class PositionCore_Con;
struct BraceChooserUpdatePackage;
struct BraceChooserFeedbackPackage;
class BraceChooser_Con;

//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct BodySectionAPIBase
{
  BodySectionAPIBase(
    const BodySectionInputs* const _in, 
    const BodySectionFeedbackInputs* const _feedIn, 
    const MyNetworkAPIBase* const _owner  ) :in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const BodySectionInputs* const in;
  const BodySectionFeedbackInputs* const feedIn;

  const MyNetworkAPIBase* const owner;

  BodySectionAPIBase(const BodySectionAPIBase& rhs);
  BodySectionAPIBase& operator = (const BodySectionAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct BodySectionUpdatePackage : public BodySectionAPIBase
{
  BodySectionUpdatePackage(
    const BodySectionInputs* const _in, 
    const BodySectionFeedbackInputs* const _feedIn, 
    BodySectionOutputs* const _out, 
    const MyNetworkAPIBase* const _owner  ) : BodySectionAPIBase(_in ,_feedIn ,_owner ), out(_out) 
  {
  }


  BodySectionOutputs* const out;


  // No update api required for this module!


  BodySectionUpdatePackage(const BodySectionUpdatePackage& rhs);
  BodySectionUpdatePackage& operator = (const BodySectionUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct BodySectionFeedbackPackage : public BodySectionAPIBase
{
  BodySectionFeedbackPackage(
    const BodySectionFeedbackInputs* const _feedIn, 
    const BodySectionInputs* const _in, 
    BodySectionFeedbackOutputs* const _feedOut, 
    const MyNetworkAPIBase* const _owner  ) : BodySectionAPIBase(_in ,_feedIn ,_owner ), feedOut(_feedOut) 
  {
  }


  BodySectionFeedbackOutputs* const feedOut;


  // No update api required for this module!


  BodySectionFeedbackPackage(const BodySectionFeedbackPackage& rhs);
  BodySectionFeedbackPackage& operator = (const BodySectionFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BODYSECTION_PKG_H

