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
#ifndef NM_DEFAULTPHYSICSMGR_H
#define NM_DEFAULTPHYSICSMGR_H
//----------------------------------------------------------------------------------------------------------------------
#include "defaultControllerMgr.h"
#include "../iPhysicsMgr.h"
#include "morpheme/mrRig.h"

#include "NMPlatform/NMCommandLineProcessor.h"

#include "sharedDefines/mPhysicsDebugInterface.h"
//----------------------------------------------------------------------------------------------------------------------

class DefaultAssetMgr;
class DefaultControllerMgr;
class DefaultSceneObjectMgr;

//----------------------------------------------------------------------------------------------------------------------
/// \class DefaultPhysicsMgr
/// \brief Implements the IPhysicsMgr interface
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class DefaultPhysicsMgr : public IPhysicsMgr
{
public:

  DefaultPhysicsMgr(
    RuntimeTargetContext* context,
    DefaultAssetMgr* assetMgr,
    const NMP::CommandLineProcessor& commandLineArguments);

  virtual ~DefaultPhysicsMgr();

  /// \brief This custom assignment operator is required as the compiler cannot generate the default assignment operator
  ///  due to const members.
  DefaultPhysicsMgr & operator= (const DefaultPhysicsMgr & other) {*this = other; return *this;}

  DefaultAssetMgr* getAssetMgr() const { return m_assetMgr; }

//----------------------------------------------------------------------------------------------------------------------
// Functions inherited from iPhysicsMgr
//----------------------------------------------------------------------------------------------------------------------

  /// \brief Which physics engine does this manager wrap.
  virtual void getPhysicsEngineID(char* buffer, uint32_t bufferLength) const NM_OVERRIDE;

  /// \brief Clear the physics scene.
  virtual void clearScene() NM_OVERRIDE;

  /// \brief Reset the physics scene.
  virtual void resetScene() NM_OVERRIDE;

  /// \brief Reset the physics scene.
  virtual void initializeScene() NM_OVERRIDE;

  /// \brief Get the controller manager object
  virtual IControllerMgr* getControllerManager() NM_OVERRIDE;

  /// \brief Update all physical scene objects.
  virtual void updatePhysicalSceneObjects() NM_OVERRIDE;

  /// \brief Initialize morpheme physics core.
  /// Register task queuing functions, attribute data types etc with Manager. Accumulate manager registry requirements etc.
  virtual void initializePhysicsCore(uint32_t numDispatchers, MR::Dispatcher** dispatchers) NM_OVERRIDE;

  /// \brief Manager registration of semantics etc post Manager::allocateRegistry().
  virtual void finaliseInitPhysicsCore() NM_OVERRIDE;

  // Update global physical environment settings from from the scene settings. Includes gravity.
  virtual void updateEnvironment() NM_OVERRIDE;

  /// \brief Create a pin constraint between the specified body and the world.
  virtual bool createConstraint(uint64_t constraintGUID,
    MR::PhysicsObjectID physicsObjectID,
    const NMP::Vector3 &constraintPosition,
    bool lockOrientation,
    bool constrainAtCOM) NM_OVERRIDE;

  /// \brief Destroy a previously created environmental constraint
  virtual bool destroyConstraint(uint64_t contraintGUID) NM_OVERRIDE;

  /// \brief Move the location that an existing constraint is attaching a body to the world.
  virtual bool moveConstraint(uint64_t contraintGUID, const NMP::Vector3 &newGrabPosition) NM_OVERRIDE;

  /// \brief apply a force.
  virtual bool applyForce(uint32_t physicsEngineObjectID, 
    ForceMode mode,
    const NMP::Vector3& force,
    bool applyAtCOM,
    const NMP::Vector3& localSpacePosition,
    const NMP::Vector3& worldSpacePosition
    ) NM_OVERRIDE;

  /// \brief Set the attribute of the physics object with given ID
  virtual bool setPhysicsObjectAttribute(uint32_t physicsEngineObjectID, const MCOMMS::Attribute* physicsObjAttribute) NM_OVERRIDE;
  
  /// \brief modify a physics environment attribute (eg gravity)
  virtual bool setPhysicsEnvironmentAttribute(const MCOMMS::Attribute* physicsEnvAttribute) NM_OVERRIDE;


  virtual bool validatePluginList(const NMP::OrderedStringTable& pluginList) NM_OVERRIDE;

  /// \brief Gets a physics object pointer (whatever the physics engine is) by its PhysicsObjectID.
  void* getPhysicsObjectPointerFromPhysicsID(MR::PhysicsObjectID id) const NM_OVERRIDE;

  /// \brief Gets a physics object ID from a physics object pointer (whatever the physics engine is).
  MR::PhysicsObjectID getPhysicsObjectIDFromPhysicsObjectPointer(void* physicsObject) const NM_OVERRIDE;

//----------------------------------------------------------------------------------------------------------------------
// Functions NOT inherited from iPhysicsMgr
//----------------------------------------------------------------------------------------------------------------------

  /// Create a new physics body
  MR::PhysicsObjectID createNewPhysicsBody(
    float                 dynamicFriction,
    float                 staticFriction,
    float                 restitution,
    bool                  isDynamic,
    MCOMMS::Attribute::PhysicsShapeType shapeType,
    float                 depth,
    float                 height,
    float                 length,
    float                 radius,
    float                 skinWidth,
    bool                  hasIndices,
    bool                  hasVertices,
    size_t                numPoints,
    size_t                numIndices,
    NMP::Vector3*         points,
    int32_t*              indices,
    MCOMMS::Attribute::VerticesWindingType windingType,
    NMP::Matrix34&        transform,
    float                 mass,
    bool                  isConstrained,
    MCOMMS::SceneObjectID objectID,
    NMP::Matrix34&        constraintGlobalTransform,
    float                 constraintDamping,
    float                 constraintStiffness,
    NMP::Matrix34&        constraintLocalTransform,
    uint32_t              positionSolverIterationCount,
    uint32_t              velocitySolverIterationCount,
    float                 sleepThreshold,
    float                 maxAngularVelocity,
    float                 linearDamping,
    float                 angularDamping);

  /// destroy a physics body, given its scene object ID
  void destroyPhysicsBody(MCOMMS::SceneObjectID objectID);

  /// update the entire physics system (calls simulate, along with all update functions for controllers)
  void update(float delta);

  /// update all controllers
  void updateControllers(float delta);

  /// update network connectivity
  void updateConnectivity(
    MCOMMS::InstanceID instanceID,
    float              dt);

  /// update pre controller
  void updatePreController(
    MCOMMS::InstanceID instanceID,
    float              delta);

  /// update pre physics
  void updatePrePhysics(
    MCOMMS::InstanceID instanceID,
    float              delta);

  /// update post physics
  void updatePostPhysics(
    MCOMMS::InstanceID instanceID,
    float              delta);

  /// set root of a network and its controller (includes gravity effect)
  void setRoot(
    MCOMMS::InstanceID  instanceID,
    const NMP::Vector3& pos);

  /// update all scene objects
  void updateSceneObjects(float delta);

protected:

  DefaultAssetMgr*            const m_assetMgr;
  RuntimeTargetContext*       const m_context;

  // number of frames
  uint32_t                    m_frameIndex;

  // CCM associated with the Mgr
  DefaultControllerMgr*       m_characterControllerManager;

}; // class DefaultPhysicsMgr

//----------------------------------------------------------------------------------------------------------------------
#endif // #ifdef NM_DEFAULTPHYSICSMGR_H
//----------------------------------------------------------------------------------------------------------------------

