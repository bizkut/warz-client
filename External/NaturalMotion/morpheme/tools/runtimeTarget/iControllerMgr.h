// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
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
#ifndef NM_LL_ICCM_H
#define NM_LL_ICCM_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/mcomms.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
  struct PosQuat;
}

namespace MR
{
  class Network;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class IControllerMgr
/// \brief Physical character interface (think: non-ragdoll character capsule)
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class IControllerMgr
{
public:

  /// Virtual destructor
  virtual  ~IControllerMgr() {};

  /// Create a controller associated with an external class
  virtual void createControllerRecord(
    MCOMMS::InstanceID  instanceID,
    MR::Network*        const network,
    const NMP::Vector3& characterStartPosition = NMP::Vector3::InitZero,
    const NMP::Quat&    characterStartRotation = NMP::Quat::kIdentity) = 0;

  virtual void updateControllerRecord(MCOMMS::InstanceID instanceID) = 0;

  /// Remove character controller
  virtual void destroyControllerRecord(MCOMMS::InstanceID instanceID) = 0;

  /// Sets the position record, but doesn't actually move the controller. Position is the feet.
  virtual void setPosition(MCOMMS::InstanceID instanceID, const NMP::Vector3& pos) = 0;

  /// Sets the orientation of the character controller
  virtual void setOrientation(MCOMMS::InstanceID instanceID, const NMP::Quat& ori) = 0;

  /// Gets the feet from the position record.
  virtual NMP::Vector3 getPosition(MCOMMS::InstanceID instanceID) const = 0;

  /// Gets the orientation of the character controller
  virtual NMP::Quat getOrientation(MCOMMS::InstanceID instanceID) const = 0;

  /// Gets the change in position of the controller
  virtual NMP::Vector3 getPositionDelta(MCOMMS::InstanceID instanceID) const = 0;

  /// Gets the change in orientation of the controller
  virtual NMP::Quat getOrientationDelta(MCOMMS::InstanceID instanceID) const = 0;

  /// Teleport the character to specified transform.
  virtual void teleport(MCOMMS::InstanceID instanceID, const NMP::PosQuat& rootTransform) = 0;
};

#endif
