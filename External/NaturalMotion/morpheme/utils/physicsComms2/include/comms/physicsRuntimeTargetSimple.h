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
#ifndef NM_PHYSICSRUNTIMETARGETSIMPLE_H
#define NM_PHYSICSRUNTIMETARGETSIMPLE_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/runtimeTargetInterface.h"
#include "sharedDefines/mPhysicsDebugInterface.h"
#include "Physics/mrPhysicsRig.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
class Network;
class NetworkDef;
}

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
/// \class SimplePhysicsDataManager
/// \brief Implements PhysicsDataManagementInterface.
/// \ingroup MorphemeComms
///
/// A simple implementation of the physics data management interface. Documentation for individual methods has been
/// omitted for brevity but is available on the method declaration of the interface this class is derived from.
///
/// This partial class declares two new pure virtual methods findNetworkByInstanceID() and findNetworkDefByGuid() which
/// provide the methods implemented with access to the client specific ID to network and GUID to network definition
/// translations.
//----------------------------------------------------------------------------------------------------------------------
class SimplePhysicsDataManager :
  public PhysicsDataManagementInterface
{
  // Construction/destruction
protected:

  SimplePhysicsDataManager();

public:

  virtual ~SimplePhysicsDataManager() {}

  /// \brief Helper method to convert from a instance ID to network instance.
  /// The mapping between the two is entirely defined by the client. It is also expected that the client already
  /// maintains a list of available network instances for general use.
  virtual MR::Network* findNetworkByInstanceID(InstanceID id) const = 0;

  /// \brief Helper method to convert from a network definition GUID to a network definition.
  /// It is expected that the client already maintains a list of available network definitions for general use.
  virtual MR::NetworkDef* findNetworkDefByGuid(const GUID& guid) const = 0;

  /// \brief Helper method to find a physics object id for a physcis rig part.
  /// It is expected that the client already maintains a list of available physics object IDs for general use.
  virtual MR::PhysicsObjectID findPhysicsObjectIDForPart(const MR::PhysicsRig::Part* part) const = 0;

  virtual MR::AnimSetIndex getPhysicsRigDefCount(const GUID& guid) NM_OVERRIDE;
  virtual bool serializeTxPhysicsRigDefData(const GUID& guid, MR::AnimSetIndex index, Connection* connection) NM_OVERRIDE;
  virtual bool serializeTxPhysicsRigPersistentData(InstanceID id, uint32_t index, Connection* connection) NM_OVERRIDE;
  virtual bool serializeTxPhysicsRigFrameData(InstanceID id, Connection* connection) NM_OVERRIDE;

  /// \brief This is an empty implementation, if the client physics engine supports dealing with different scale
  /// tolerances (dealing with very big assets or very small assets) then this function can be used to respond
  /// to changes in asset scales.
  virtual void onEnvironmentVariableChange(const Attribute* attribute) NM_OVERRIDE;

protected:
  /// \brief Calculates the collision set colour for a given part by assigning each collision set a different colour
  /// and then blending the colours of every set the part belongs to to produce the final output colour.
  /// The colour of each set is calculated by dividing the possible range of hues by the number of collision sets, 
  /// For example if there were 5 collision sets, collision set 3 would have a hue of (1.0f / 5) * 3.0f. Calculating
  /// the colour using hue saturation value ensures a better separation of colours for each set.
  bool calculatePartCollisionSetColour(MR::PhysicsRig* physicsRig, uint32_t partIndex, NMP::Colour* colour) const;

private:

  uint32_t    m_frameIndex;
  float       m_stepInterval;
  float       m_frameRate;
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_RUNTIMETARGETSIMPLE_H
//----------------------------------------------------------------------------------------------------------------------
