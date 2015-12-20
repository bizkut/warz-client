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
#ifndef NM_IPHYSICSMANAGER_H
#define NM_IPHYSICSMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/sceneObject.h"
#include "morpheme/mrDispatcher.h"
#include "sharedDefines/mPhysicsDebugInterface.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
  class OrderedStringTable;
}

namespace MCOMMS
{
class DataManagementInterface;
}

class IControllerMgr;

//----------------------------------------------------------------------------------------------------------------------
/// \class IPhysicsMgr
/// \brief Physics manager interface
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class IPhysicsMgr
{
public:

  /// virtual destructor
  virtual ~IPhysicsMgr() {}

  //--------------------------------------------------------------------------------------------------------------------
  // The following functions must be implemented by the application
  //--------------------------------------------------------------------------------------------------------------------

  /// Which physics engine does this manager wrap.
  virtual void getPhysicsEngineID(char* buffer, uint32_t bufferLength) const = 0;

  /// Removes any internally created objects
  virtual void clearScene() = 0;

  /// Resets the scene.
  virtual void resetScene() = 0;

  /// Initialize the scene.
  virtual void initializeScene() = 0;

  /// Get a reference to the character controller manager
  virtual IControllerMgr* getControllerManager() = 0;

  /// Update all physicalised scene objects
  virtual void updatePhysicalSceneObjects() = 0;

  /// \brief Initialize morpheme physics core.
  /// Register task queuing functions, attribute data types etc with Manager. Accumulate manager registry requirements etc.
  virtual void initializePhysicsCore(uint32_t numDispatchers, MR::Dispatcher** dispatchers) = 0;

  /// \brief Manager registration of semantics etc post Manager::allocateRegistry().
  virtual void finaliseInitPhysicsCore() = 0;

  // Update global physical environment settings from from the scene settings. Includes gravity.
  virtual void updateEnvironment() = 0;

  /// \brief Create a pin constraint between the specified body and the world.
  virtual bool createConstraint(uint64_t constraintGUID,
    MR::PhysicsObjectID physicsObjectID,
    const NMP::Vector3 &constraintPosition,
    bool lockOrientation,
    bool constrainAtCOM) = 0;

  /// \brief Destroy a previously created environmental constraint
  virtual bool destroyConstraint(uint64_t contraintGUID) = 0;

  /// \brief Move the location that an existing constraint is attaching a body to the world.
  virtual bool moveConstraint(uint64_t contraintGUID, const NMP::Vector3 &newGrabPosition) = 0;

  /// \brief Gets a physics object pointer (whatever the physics engine is) by its PhysicsObjectID.
  virtual void* getPhysicsObjectPointerFromPhysicsID(MR::PhysicsObjectID id) const = 0;

  /// \brief Gets a physics object ID from a physics object pointer (whatever the physics engine is).
  virtual MR::PhysicsObjectID getPhysicsObjectIDFromPhysicsObjectPointer(void* physicsObject) const = 0;

  enum ForceMode
  {
    kFORCE, // treat the force as a standard force
    kIMPULSE, // treat the force as an impulse
    kVELOCITY_CHANGE // the force is specified as a desired velocity change 
  };
  
  /// \brief apply a force to a given physics object
  virtual bool applyForce(uint32_t physicsEngineObjectID, 
    ForceMode mode,
    const NMP::Vector3& force,
    bool applyAtCOM,
    const NMP::Vector3& localSpacePosition,
    const NMP::Vector3& worldSpacePosition
    ) = 0; 

  /// \brief modify a physics object attribute (eg the object's transform)
  virtual bool setPhysicsObjectAttribute(uint32_t physicsEngineObjectID, const MCOMMS::Attribute* physicsObjAttribute) = 0;

  /// \brief modify a physics environment attribute (eg gravity)
  virtual bool setPhysicsEnvironmentAttribute(const MCOMMS::Attribute* physicsEnvAttribute) = 0;

  /// \brief called when the asset scale of the scene changes
  virtual void onAssetScaleChanged(float NMP_UNUSED(assetScale)) {};
  
  virtual bool validatePluginList(const NMP::OrderedStringTable& NMP_UNUSED(pluginList)) = 0;

}; // class IPhysicsMgr

#endif

//----------------------------------------------------------------------------------------------------------------------
