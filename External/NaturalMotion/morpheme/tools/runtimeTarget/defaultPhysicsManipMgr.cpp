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
#include "defaultPhysicsManipMgr.h"
#include "runtimeTargetLogger.h"
//----------------------------------------------------------------------------------------------------------------------
DefaultPhysicsManipMgr::DefaultPhysicsManipMgr(IPhysicsMgr* physicsManager) : m_physicsManager(physicsManager)
{
}

//----------------------------------------------------------------------------------------------------------------------
DefaultPhysicsManipMgr::~DefaultPhysicsManipMgr()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsManipMgr::canCreateAndEditConstraints() const
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsManipMgr::canApplyForces() const
{
  return true;
}

bool DefaultPhysicsManipMgr::canSetAttributes() const
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsManipMgr::createConstraint(uint64_t constraintGUID,
                      uint32_t physicsEngineObjectID,
                      const NMP::Vector3& NMP_UNUSED(localSpaceConstraintPosition),
                      const NMP::Vector3& worldSpaceConstraintPosition,
                      bool constrainOrientation,
                      bool constrainAtCentreOfMass,
                      const NMP::Vector3& NMP_UNUSED(sourceRayWSPositionHint),
                      const NMP::Vector3& NMP_UNUSED(sourceRayWSDirectionHint)
                      )
{
  NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "    Create Constraint for ObjectID '%i' at worldSpace %f,%f,%f\n",
                   physicsEngineObjectID, worldSpaceConstraintPosition.x, worldSpaceConstraintPosition.y, worldSpaceConstraintPosition.z);

  return m_physicsManager->createConstraint(constraintGUID, physicsEngineObjectID, worldSpaceConstraintPosition,
    constrainOrientation, constrainAtCentreOfMass);
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsManipMgr::moveConstraint(uint64_t constraintGUID, const NMP::Vector3& worldSpaceConstraintPosition)
{
  NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "    Move Constraint with ID '%lld' at worldSpace %f,%f,%f\n",
    constraintGUID, worldSpaceConstraintPosition.x, worldSpaceConstraintPosition.y, worldSpaceConstraintPosition.z);

  return m_physicsManager->moveConstraint(constraintGUID, worldSpaceConstraintPosition);
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsManipMgr::removeConstraint(uint64_t constraintGUID)
{
  NM_LOG_MESSAGE(RTT_LOGGER, RTT_MESSAGE_PRIORITY, "    Remove Constraint with ID '%lld'", constraintGUID);

  return m_physicsManager->destroyConstraint(constraintGUID);
}

//----------------------------------------------------------------------------------------------------------------------

bool DefaultPhysicsManipMgr::applyForce(uint32_t physicsEngineObjectID, 
                ForceMode mode,
                const NMP::Vector3& force,
                bool applyAtCOM,
                const NMP::Vector3& localSpacePosition,
                const NMP::Vector3& worldSpacePosition
                ) 
{
  IPhysicsMgr::ForceMode iphysicsMode = IPhysicsMgr::kFORCE; 
  switch(mode)
  {
  case PhysicsManipulationInterface::kFORCE :
    {
      iphysicsMode =  IPhysicsMgr::kFORCE; 
    }
    break;

  case PhysicsManipulationInterface::kIMPULSE :
    {
      iphysicsMode = IPhysicsMgr::kIMPULSE;
    }
    break;

  case PhysicsManipulationInterface::kVELOCITY_CHANGE : 
    {
      iphysicsMode = IPhysicsMgr::kVELOCITY_CHANGE;
    }
    break; 
  default:
    break;
  }
  return m_physicsManager->applyForce(physicsEngineObjectID, iphysicsMode, force, applyAtCOM, localSpacePosition, worldSpacePosition); 
}

//----------------------------------------------------------------------------------------------------------------------

bool DefaultPhysicsManipMgr::setPhysicsObjectAttribute(uint32_t physicsEngineObjectID,
                                      const MCOMMS::Attribute* physicsObjAttribute)
{
  return m_physicsManager->setPhysicsObjectAttribute(physicsEngineObjectID, physicsObjAttribute);
}
