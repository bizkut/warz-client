// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_DEFAULTERPHYSICSMGR_H
#define NM_DEFAULTERPHYSICSMGR_H
//----------------------------------------------------------------------------------------------------------------------
#include "defaultPhysicsMgr.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \class DefaultERPhysicsMgr
/// \brief Specialization of DefaultPhysicsMgr to add support for Euphoria assets
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class DefaultERPhysicsMgr : public DefaultPhysicsMgr
{
public:

  DefaultERPhysicsMgr(
    RuntimeTargetContext* context,
    DefaultAssetMgr* assetMgr,
    const NMP::CommandLineProcessor& commandLineArguments) :
    DefaultPhysicsMgr(context, assetMgr, commandLineArguments)
  {
  }

  ~DefaultERPhysicsMgr();

  /// \brief Initialize morpheme physics core.
  /// Register task queuing functions, attribute data types etc with Manager. Accumulate manager registry requirements etc.
  virtual void initializePhysicsCore(uint32_t numDispatchers, MR::Dispatcher** dispatchers) NM_OVERRIDE;

  /// \brief Manager registration of semantics etc post Manager::allocateRegistry().
  virtual void finaliseInitPhysicsCore() NM_OVERRIDE;

  virtual void updatePrePhysics(MCOMMS::InstanceID instanceID, float deltaTime, bool updatePhysicsRig = true) NM_OVERRIDE;

  virtual void updatePostPhysics(MCOMMS::InstanceID instanceID, float deltaTime, bool updatePhysicsRig = true) NM_OVERRIDE;

  virtual void updateInteractionProxy(MCOMMS::InstanceID instanceID, float deltaTime) NM_OVERRIDE;
}; // class DefaultERPhysicsMgr

#endif // #ifdef NM_DEFAULTERPHYSICSMGR_H

//----------------------------------------------------------------------------------------------------------------------
