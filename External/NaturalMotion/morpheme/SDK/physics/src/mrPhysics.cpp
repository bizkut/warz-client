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
#include "physics/mrPhysics.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
void physicsOnAnimSetChanged(Network* net, AnimSetIndex animationSetIndex)
{
  PhysicsRig* physicsRig = getPhysicsRig(net);
  if (physicsRig)
  {    
    NMP_ASSERT_MSG((getActivePhysicsRigDef(net) == NULL) || (getActivePhysicsRigDef(net) == physicsRig->getPhysicsRigDef()),
                   "Attempting to switch to an anim set with a different physics rig def! This is not supported currently");

    MR::AnimToPhysicsMap* animToPhysicsMap = getAnimToPhysicsMap(net->getNetworkDef(), animationSetIndex);
    physicsRig->setRigAndAnimToPhysicsMap(net->getActiveRig(), animToPhysicsMap);
  }

  return;
}

//----------------------------------------------------------------------------------------------------------------------
void setPhysicsRig(Network* net, PhysicsRig* physicsRig)
{
  if (physicsRig)
  {
    physicsRig->setRigAndAnimToPhysicsMap(net->getActiveRig(), getAnimToPhysicsMap(net->getNetworkDef(), net->getActiveAnimSetIndex()));
  }
  NodeBinEntry* nodeBinEntry = net->getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_PHYSICS_RIG, NETWORK_NODE_ID);
  if (!nodeBinEntry)
  {
    AttribDataHandle handle = AttribDataPhysicsRig::create(net->getPersistentMemoryAllocator(), physicsRig);
    AttribAddress attribAddress(ATTRIB_SEMANTIC_PHYSICS_RIG, NETWORK_NODE_ID);
    net->addAttribData(attribAddress, handle, LIFESPAN_FOREVER);
  }
  else
  {
    AttribDataPhysicsRig* attribDataPhysicsRig = nodeBinEntry->getAttribData<AttribDataPhysicsRig>();
    // Make sure the physics rig references are clean.
    if (physicsRig && attribDataPhysicsRig->m_physicsRig != physicsRig)
    {
      while (physicsRig->isReferenced())
      {
        physicsRig->removeReference();
      }
    }
    attribDataPhysicsRig->m_physicsRig = physicsRig;
  }
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRig* getPhysicsRig(Network* net)
{
  PhysicsRig* result = NULL;
  NodeBinEntry* nodeBinEntry = net->getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_PHYSICS_RIG, NETWORK_NODE_ID, INVALID_NODE_ID, VALID_FOREVER, net->getActiveAnimSetIndex());
  if (nodeBinEntry)
  {
    AttribDataPhysicsRig* attribDataPhysicsRig = nodeBinEntry->getAttribData<AttribDataPhysicsRig>();
    result = attribDataPhysicsRig->m_physicsRig;
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRig* getPhysicsRig(Network* net, AnimSetIndex animSetIndex)
{
  PhysicsRig* result = NULL;
  NodeBinEntry* nodeBinEntry = net->getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_PHYSICS_RIG, NETWORK_NODE_ID, INVALID_NODE_ID, VALID_FOREVER, animSetIndex);
  if (nodeBinEntry)
  {
    AttribDataPhysicsRig* attribDataPhysicsRig = nodeBinEntry->getAttribData<AttribDataPhysicsRig>();
    result = attribDataPhysicsRig->m_physicsRig;
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRig* getPhysicsRig(const Network* net)
{
  return getPhysicsRig(const_cast<Network*>(net));
}

//----------------------------------------------------------------------------------------------------------------------
CollisionFilterMask* getCollisionFilterMask(NodeDef* nodeDef, Network* net)
{
  NodeID nodeID = nodeDef->getNodeID();
  NodeBinEntry* nodeBinEntry = net->getAttribDataNodeBinEntry(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
    nodeID,
    INVALID_NODE_ID,
    VALID_FRAME_ANY_FRAME);
  NMP_ASSERT(nodeBinEntry);

  return nodeBinEntry ? &(nodeBinEntry->getAttribData<AttribDataCollisionFilterMask>()->m_collisionFilterMask) : 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool setCollisionFilterMask(NodeDef* nodeDef, Network* net, const CollisionFilterMask& cfm)
{
  NodeID nodeID = nodeDef->getNodeID();
  NodeBinEntry* nodeBinEntry = net->getAttribDataNodeBinEntry(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
    nodeID,
    INVALID_NODE_ID,
    VALID_FRAME_ANY_FRAME);
  NMP_ASSERT(nodeBinEntry);

  if (nodeBinEntry)
  {
    nodeBinEntry->getAttribData<AttribDataCollisionFilterMask>()->m_collisionFilterMask = cfm;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigDef* getActivePhysicsRigDef(Network* net)
{
  return getPhysicsRigDef(net->getNetworkDef(), net->getActiveAnimSetIndex());
}

//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsRigDef* getPhysicsRigDef(NetworkDef* netDef, AnimSetIndex animSetIndex)
{
  NMP_ASSERT(animSetIndex < netDef->getNumAnimSets());

  AttribDataHandle* handle = netDef->getOptionalAttribDataHandle(ATTRIB_SEMANTIC_PHYSICS_RIG_DEF, NETWORK_NODE_ID, animSetIndex);
  if (handle)
  {
    AttribDataPhysicsRigDef* physRigDefAttrib = (AttribDataPhysicsRigDef*)handle->m_attribData;

    // The physics rig def is optional for a set.
    if (physRigDefAttrib)
    {
      return physRigDefAttrib->m_physicsRigDef;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
AnimToPhysicsMap* getAnimToPhysicsMap(NetworkDef* netDef, AnimSetIndex animSetIndex)
{
  NMP_ASSERT(animSetIndex < netDef->getNumAnimSets());

  AttribDataHandle* handle = netDef->getAttribDataHandle(ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP, NETWORK_NODE_ID, animSetIndex);
  NMP_ASSERT(handle);
  AttribDataAnimToPhysicsMap* animToPhysicsAttrib = (AttribDataAnimToPhysicsMap*)handle->m_attribData;


  if (!animToPhysicsAttrib)
    return 0;

  NMP_ASSERT(animToPhysicsAttrib->m_animToPhysicsMap);
  return animToPhysicsAttrib->m_animToPhysicsMap;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
