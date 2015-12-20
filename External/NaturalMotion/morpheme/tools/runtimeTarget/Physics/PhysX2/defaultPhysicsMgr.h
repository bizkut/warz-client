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
#include "mrPhysicsScenePhysX2.h"
#include "Physics/mrPhysicsRig.h"
#include "../../iPhysicsMgr.h"
#include "NMPlatform/NMCommandLineProcessor.h"
#include <vector>
#include "NMPlatform/NMHashMap.h"

#include "sharedDefines/mPhysicsDebugInterface.h"
//----------------------------------------------------------------------------------------------------------------------

class DefaultAssetMgr;
class DefaultControllerMgr;
class DefaultSceneObjectMgr;

//----------------------------------------------------------------------------------------------------------------------
/// \class DefaultPhysicsMgr
/// \brief Implements the IPhysicsMgr interface
/// \ingroup LiveLinkRuntimeModule
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

  /// \brief Apply a force to the character
  virtual bool applyForce(uint32_t physicsEngineObjectID, 
    ForceMode mode,
    const NMP::Vector3& force,
    bool applyAtCOM,
    const NMP::Vector3& localSpacePosition,
    const NMP::Vector3& worldSpacePosition
    ) NM_OVERRIDE;
  
  virtual bool validatePluginList(const NMP::OrderedStringTable& pluginList) NM_OVERRIDE;

    /// \brief Set the attribute of the physics object with given ID
  bool setPhysicsObjectAttribute(uint32_t physicsEngineObjectID, const MCOMMS::Attribute* physicsObjAttribute) NM_OVERRIDE;
  
  /// \brief modify a physics environment attribute (eg gravity)
  bool setPhysicsEnvironmentAttribute(const MCOMMS::Attribute* physicsEnvAttribute) NM_OVERRIDE;

  /// \brief Gets a physics object pointer (whatever the physics engine is) by its PhysicsObjectID.
  void* getPhysicsObjectPointerFromPhysicsID(MR::PhysicsObjectID id) const NM_OVERRIDE;

  /// \brief Gets a physics object ID from a physics object pointer (whatever the physics engine is).
  MR::PhysicsObjectID getPhysicsObjectIDFromPhysicsObjectPointer(void* physicsObject) const NM_OVERRIDE;

  //----------------------------------------------------------------------------------------------------------------------
// Functions NOT inherited from iPhysicsMgr
//----------------------------------------------------------------------------------------------------------------------

  /// \brief Create triangle mesh data (physX).
  PhysXTriangleMesh* createTriangleMeshData(const PhysXTriangleMeshDesc& meshDesc);

  /// \brief Initialise triangle mesh shape description (physX).
  bool initializeTriangleMeshShapeDesc(
    const PhysXTriangleMeshDesc& meshDesc,
    PhysXTriangleMeshShapeDesc&  shapeDesc);

  /// \brief Create triangle mesh data describing a convex mesh(physX).
  PhysXConvexMesh* createConvexMeshData(const PhysXConvexMeshDesc& meshDesc);

  /// \brief Initialise a convex triangle mesh shape description (physX).
  bool initializeConvexShapeDesc(
    const PhysXConvexMeshDesc& meshDesc,
    PhysXConvexShapeDesc&      shapeDesc);

  /// \brief Create a new physics body.
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
    float                 density,
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

  /// \brief Destroy a physics body, given its scene object ID.
  void destroyPhysicsBody(MCOMMS::SceneObjectID objectID);

  /// \brief Run a step of physics simulation.
  void simulate(float deltaTime);

  /// \brief Get the actual timestep used.
  float getActualTimestep(float deltaTime) const;

  /// \brief Create a physics rigs for the specified network
  void createPhysicsRig(MR::Network* network, NMP::Vector3* initialPosition);

  /// \brief Destroy the physics rigs that the specified instance ID is using (if it is using one).
  void destroyPhysicsRig(MR::Network* network);

  /// \brief Update the entire physics system (calls simulate, along with all update functions for controllers).
  void update(float deltaTime);

  /// \brief Update all controllers.
  void updateControllers(float deltaTime);

  /// \brief Update network connections.
  void updateConnections(
    MCOMMS::InstanceID instanceID,
    float              deltaTime);

  /// \brief Update pre controller.
  void updatePreController(
    MCOMMS::InstanceID instanceID,
    float              deltaTime);

  /// \brief Update pre physics.
  virtual void updatePrePhysics(
    MCOMMS::InstanceID instanceID,
    float              deltaTime);

  /// \brief Update post physics.
  virtual void updatePostPhysicsInit(
    MCOMMS::InstanceID instanceID,
    float              deltaTime);
  /// \brief Update post physics.
  virtual void updatePostPhysics(
    MCOMMS::InstanceID instanceID,
    float              deltaTime);

  /// \brief Set root of a network and its controller (includes gravity effect).
  void setRoot(
    MCOMMS::InstanceID  instanceID,
    const NMP::Vector3& pos);

  /// \brief Assign physical bodies pre connections.
  void assignPhysicalBodiesPreConnections();

  /// \brief Assign physical bodies post connections.
  void assignPhysicalBodiesPostConnections();

  /// \brief Update all scene objects.
  void updateSceneObjects(float deltaTime);

  /// \brief Get a vector* of all physX actors in the scene.
  std::vector<PhysXActor*>& getSceneActors() { return m_sceneActors; }

  /// \brief Get a vector* of all physX materials in the scene.
  std::vector<NxMaterial*>& getMaterials() { return m_materials; }

  /// \brief Get a vector* of all physX joints in the scene.
  std::vector<NxJoint*>& getJoints() { return m_joints; }

  /// \brief Get the physics scene.
  MR::PhysicsScenePhysX2* getPhysicsScene() { return m_physicsScene; }

  /// \brief Gets a physics actor by its PhysicsObjectID, actors returned by this can be either
  /// scene objects or parts of the physics rig.
  PhysXActor*         getActorByPhysicsID(MR::PhysicsObjectID id) const;
  /// \brief Gets a PhysicsObjectID for a given physics actor, actors can be either scene objects or
  /// parts of the physics rig.
  MR::PhysicsObjectID getPhysicsIDForActor(const PhysXActor* actor) const;
  /// \brief Gets a PhysicsObjectID for a given physics physics rig part.
  MR::PhysicsObjectID getPhysicsIDForPart(const MR::PhysicsRig::Part* part) const;

protected:

  DefaultAssetMgr*            const m_assetMgr;
  RuntimeTargetContext*       const m_context;

  std::vector<NxJoint*>       m_joints;
  std::vector<NxMaterial*>    m_materials;
  std::vector<PhysXActor*>    m_sceneActors;

  float                       m_maxTimeStep;                  ///< max duration of each internal sub-step.
  uint32_t                    m_physicsFreq;                  ///< frequency for sub stepping physics update
  uint32_t                    m_maxNumSubSteps;               ///< max number of internal sub-steps.
  uint32_t                    m_frameIndex;                   ///< number of frames.
  MR::PhysicsScenePhysX2*     m_physicsScene;                 ///< PhysicsScene object associated with the Mgr.
  DefaultControllerMgr*       m_characterControllerManager;   ///< CCM associated with the Mgr.

  typedef NMP::hash_map<MR::PhysicsObjectID, PhysXActor*> PhysicsIDToActorMap;
  typedef NMP::hash_map<PhysXActor*, MR::PhysicsObjectID> ActorToPhysicsIDMap;

  typedef struct
  {
    NMP::Vector3 m_grabOffsetFromCOM; // The vector from COM to the grab point on the body.
    PhysXActor *m_constrainedActor;
    NxJoint *m_jointConstraint;
    PhysXActor *m_dummyActor; // This is a kinematic object used to anchor the joint.
    bool m_isCOMConstraint;
  }
  Constraint;

  typedef NMP::hash_map<uint64_t, Constraint*> GUIDToConstraintMap;

  MR::PhysicsObjectID         m_nextPhysicsObjectID;          ///< physics object id to use for next physics object created.
  PhysicsIDToActorMap         m_physicsIDActorMap;            ///< map of physics object ids to PhysXActors to allow 
                                                              ///  commands from morpheme:connect to interact with
                                                              ///  physics actors.
  ActorToPhysicsIDMap         m_actorPhysicsIDMap;            ///< reverse of the map above for quick lookup when removing
                                                              ///< physics actors.
  GUIDToConstraintMap         m_constraintMap;                ///< A map of GUIDs (from connect) to constraints that we
                                                              ///< have created.

  /// \brief Assigns an id to an actor so they can be addressed by commands from morpheme:connect.
  MR::PhysicsObjectID assignPhysicsIDToActor(PhysXActor* actor);
  /// \brief Clears any currently assigned physics ids.
  void                clearAllPhysicsIDs();

  Constraint* getConstraint(uint64_t guid) const;

public:
  /// \brief Frees up an id when an actor is destroyed.
  bool unassignPhysicsID(PhysXActor* actor);

}; // class DefaultPhysicsMgr

#endif // #ifdef NM_DEFAULTPHYSICSMGR_H

//----------------------------------------------------------------------------------------------------------------------
