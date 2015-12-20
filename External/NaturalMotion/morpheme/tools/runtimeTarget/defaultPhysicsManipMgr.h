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
#ifndef NM_DEFAULTPHYSICSMANIPMGR_H
#define NM_DEFAULTPHYSICSMANIPMGR_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/runtimeTargetInterface.h"
#include "iPhysicsMgr.h"
//----------------------------------------------------------------------------------------------------------------------

class DefaultPhysicsManipMgr :
  public MCOMMS::PhysicsManipulationInterface
{
public:
  DefaultPhysicsManipMgr(IPhysicsMgr* physicsManager);
  virtual ~DefaultPhysicsManipMgr();
  bool canCreateAndEditConstraints() const NM_OVERRIDE;
  bool canApplyForces() const NM_OVERRIDE;
  bool canSetAttributes() const NM_OVERRIDE;

  bool createConstraint(uint64_t constraintGUID,
    uint32_t physicsEngineObjectID,
    const NMP::Vector3& localSpaceConstraintPosition,
    const NMP::Vector3& worldSpaceConstraintPosition,
    bool constrainOrientation,
    bool constrainAtCentreOfMass,
    const NMP::Vector3& sourceRayWSPositionHint,
    const NMP::Vector3& sourceRayWSDirectionHint
    ) NM_OVERRIDE;

  bool moveConstraint(uint64_t constraintGUID, const NMP::Vector3& worldSpaceConstraintPosition) NM_OVERRIDE;
  bool removeConstraint(uint64_t constraintGUID)  NM_OVERRIDE;

  bool applyForce(uint32_t physicsEngineObjectID, 
    ForceMode mode,
    const NMP::Vector3& force,
    bool applyAtCOM,
    const NMP::Vector3& localSpacePosition,
    const NMP::Vector3& worldSpacePosition
    ) NM_OVERRIDE;

  bool setPhysicsObjectAttribute(uint32_t physicsEngineObjectID, const MCOMMS::Attribute* physicsObjAttribute) NM_OVERRIDE;

protected:
  IPhysicsMgr* m_physicsManager; 
};


#endif // #ifdef NM_DEFAULTPHYSICSMANIPMGR_H

//----------------------------------------------------------------------------------------------------------------------
