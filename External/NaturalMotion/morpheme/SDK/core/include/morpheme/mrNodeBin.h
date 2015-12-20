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
#ifndef MR_NODEBIN_H
#define MR_NODEBIN_H
//----------------------------------------------------------------------------------------------------------------------

#include "morpheme/mrAttribData.h"
#include "morpheme/mrTask.h"
#include "morpheme/mrManager.h"

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \struct MR::NodeBinEntry
/// \brief A linked list entry that holds a handle to a piece of AttribData.
/// \ingroup NetworkModule
///
/// In some cases several NodeBinEntrys can be pointing at 1 AttribData.
/// Each Node on a Network holds a list of these.
//----------------------------------------------------------------------------------------------------------------------
struct NodeBinEntry
{
  NodeBinEntry*         m_next;
  AttribDataHandle      m_attribDataHandle;
  AttribAddress         m_address;
  NMP::MemoryAllocator* m_allocatorUsed;  ///< This represents the allocator used to allocate this entry and the attrib
                                          ///<  data hanging off it.
  uint16_t              m_lifespan;       ///< How many frames this AttribData will persist for.
                                          ///<  + At beginning of Network update all lifespans in the Networks
                                          ///<  AttribDatalist are decremented. If zero then it is erased.

  AttribData* getAttribData() { return m_attribDataHandle.m_attribData; }

  template <class T> 
  T* getAttribData()
  {
    NMP_ASSERT(T::getDefaultType() == m_attribDataHandle.m_attribData->getType());
    return static_cast<T*>(m_attribDataHandle.m_attribData);
  }

  /// \brief Reduce the ref count to an AttribData and if zero delete the AttribData.
  void removeReference();
};

//----------------------------------------------------------------------------------------------------------------------
/// \struct MR::OutputCPPin
/// \brief Holds a handle to a single control param AttribData corresponding to a single output pin on a Node.
/// \ingroup NetworkModule
//----------------------------------------------------------------------------------------------------------------------
struct OutputCPPin
{
  AttribData* getAttribData() { return m_attribDataHandle.m_attribData; }

  NM_INLINE void deleteAttribData();

  template <class T> 
  T* getAttribData()
  {
    NMP_ASSERT(T::getDefaultType() == m_attribDataHandle.m_attribData->getType());
    return static_cast<T*>(m_attribDataHandle.m_attribData);
  }

  FrameCount       m_lastUpdateFrame;  ///< The last frame this control param output pin was updated.
                                       ///<  Avoids duplicate evaluation.
  AttribDataHandle m_attribDataHandle; ///< Included for parameter handling in SPU dispatcher.
};

//----------------------------------------------------------------------------------------------------------------------
/// \struct MR::NodeBinEntry
/// \brief Holds a linked list of attribute data entries for a specific node.
/// \ingroup NetworkModule
//----------------------------------------------------------------------------------------------------------------------
struct NodeBin
{
  /// \brief 
  NM_INLINE static NMP::Memory::Format getMemoryRequirements(NetworkDef* netDef);

  /// \brief 
  NM_INLINE static NodeBin* init(NMP::Memory::Resource& resource, NetworkDef* netDef);

  /// \brief Adds a NodeBinEntry to the list
  NM_INLINE void add(NodeBinEntry *newAD);

  /// \brief Delete all AttribData in this bin.
  NM_INLINE void deleteAllAttribData();

  /// \brief Delete all AttribData in this bin, excluding those semantic types listed.
  NM_INLINE void deleteAllAttribDataExcluding(uint32_t numExcluded, const AttribDataSemantic* excludeSemantics);

  /// \brief Delete all AttribData in this bin whose semantic type is included in the supplied list.
  NM_INLINE void deleteAllAttribDataSpecified(uint32_t numIncluded, const AttribDataSemantic* includedSemantics);

  ///  \brief 
  NM_INLINE bool updateAttribDataLifespans(FrameCount currentFrameNo);

  /// \brief 
  NM_INLINE NodeBinEntry* getEntry(
    AttribDataSemantic semantic,
    NodeID             targetNodeID,
    FrameCount         validFrame,
    AnimSetIndex       animSetIndex) const;

  /// \brief 
  NM_INLINE NodeBinEntry* getEntries() const;

  /// \brief 
  NM_INLINE OutputCPPin* getOutputCPPin(PinIndex sourceCPPinIndex) const;

  /// \brief 
  NM_INLINE FrameCount getLastFrameUpdate() const { return m_lastFrameUpdate; }

  /// \brief 
  NM_INLINE void setLastFrameUpdate(FrameCount frameIndex) { m_lastFrameUpdate = frameIndex; }

  /// \brief 
  NM_INLINE Task* findQueuedTaskID(TaskID taskID);

  /// \brief 
  NM_INLINE void addQueuedTask(Task* task);

  /// \brief 
  NM_INLINE void clearQueuedTasks();

  /// \brief 
  AnimSetIndex getOutputAnimSetIndex() const { return m_outputAnimSet; }

  /// \brief 
  void setOutputAnimSetIndex(AnimSetIndex animationSetIndex) { m_outputAnimSet = animationSetIndex; }

#if defined(MR_ATTRIB_DEBUG_BUFFERING)
  /// \brief 
  NM_INLINE uint32_t isDebugOutputSemanticSet(AttribDataSemantic semantic);

  /// \brief 
  NM_INLINE void setDebugOutputSemantic(AttribDataSemantic semantic, bool set);

  /// \brief 
  NM_INLINE void setDebugOutputAllSemantics(bool set);
#endif // MR_ATTRIB_DEBUG_BUFFERING

  FrameCount       m_lastFrameUpdate;      ///< The last frame that connections was updated on this node.
                                           ///<  Connections are updated if any tasks are queued for this node.
  NodeBinEntry*    m_attributes;           ///< Linked list of attribute data entries.

  uint32_t         m_frameLifespanUpdated; ///< The frame at which the lifespan of this nodes attributes was last updated.

  Task*            m_queuedTaskList;

  OutputCPPin*     m_outputCPPins;         ///< Stores output control param AttribData indexed by output PinIndex.

  AnimSetIndex m_outputAnimSet;

#if defined(MR_ATTRIB_DEBUG_BUFFERING)
  /// Identifies the semantics that this node has been asked to keep around so that they can be examined for debugging
  /// (usually for transmission to connect).
  /// If flagged, the lifespan that the attribute is generated with is modified to a minimum of 1 frame.
  NMP::BitArray*   m_debugOutputSemanticFlags;
#endif // MR_ATTRIB_DEBUG_BUFFERING
};

//----------------------------------------------------------------------------------------------------------------------
// NodeBin functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeBin::getMemoryRequirements(NetworkDef* netDef)
{
  uint32_t numNodes = netDef->getNumNodeDefs();
  NMP::Memory::Format result(numNodes * sizeof(NodeBin), NMP_NATURAL_TYPE_ALIGNMENT);
  for (NodeID i = 0; i < numNodes; i++)
  {
    NodeDef* nodeDef = netDef->getNodeDef(i);
    if (nodeDef) // Check for partially built network
    {
      result += NMP::Memory::Format(sizeof(OutputCPPin) * nodeDef->getNumOutputCPPins(), NMP_NATURAL_TYPE_ALIGNMENT);
    }
  }
#if defined(MR_ATTRIB_DEBUG_BUFFERING)
  uint32_t numSemantics = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  result += NMP::BitArray::getMemoryRequirements(numSemantics) * numNodes;
#endif // defined(MR_ATTRIB_DEBUG_BUFFERING)
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NodeBin* NodeBin::init(NMP::Memory::Resource& resource, NetworkDef* netDef)
{
  uint32_t numNodes = netDef->getNumNodeDefs();
  NMP::Memory::Format nodeBinMemReqs(numNodes * sizeof(NodeBin), NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(nodeBinMemReqs);
  NodeBin* result = (NodeBin*)resource.ptr;
  for (uint32_t i = 0; i < numNodes; i++)
  {
    result[i].m_attributes = NULL;
    result[i].m_lastFrameUpdate = NOT_FRAME_UPDATED;
    result[i].m_queuedTaskList = NULL;
    result[i].m_frameLifespanUpdated = 0;
    result[i].m_outputAnimSet = 0xFFFF;
  }
  resource.increment(nodeBinMemReqs);

  for (NodeID i = 0; i < numNodes; i++)
  {
    result[i].m_outputCPPins = NULL;
    NodeDef* nodeDef = netDef->getNodeDef(i);

    if (nodeDef && nodeDef->getNumOutputCPPins() > 0)  // May not be a nodeDef if this is a partially built network.
    {
      NMP::Memory::Format format(NMP::Memory::align(sizeof(OutputCPPin) * nodeDef->getNumOutputCPPins(),  NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
      resource.align(format);
      NMP_ASSERT(resource.ptr);
      result[i].m_outputCPPins = (OutputCPPin*) resource.ptr;
      resource.increment(format);

      for (uint32_t j = 0; j < nodeDef->getNumOutputCPPins(); ++j)
      {
        result[i].m_outputCPPins[j].m_lastUpdateFrame = 0;
        result[i].m_outputCPPins[j].m_attribDataHandle.set(NULL, NMP::Memory::Format(0));
      }
    }
  }

  for (uint32_t i = 0; i < numNodes; i++)
  {
#if defined(MR_ATTRIB_DEBUG_BUFFERING)
    uint32_t numSemantics = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
    result[i].m_debugOutputSemanticFlags = NMP::BitArray::init(resource, numSemantics);
#endif // defined(MR_ATTRIB_DEBUG_BUFFERING)
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBin::add(NodeBinEntry *newAD)
{
  newAD->m_next = m_attributes; // Insert at the beginning

  m_attributes = newAD;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBin::deleteAllAttribData()
{
  NodeBinEntry *entry = m_attributes;
  while (entry)
  {
    // We are deleting a reference to an AttribData so reduce the refCount.
    entry->removeReference();

    // Delete the tie itself.
    NodeBinEntry *temp = entry;
    entry = entry->m_next;
    temp->m_allocatorUsed->memFree(temp);
  }

  m_attributes = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void NodeBin::deleteAllAttribDataExcluding(uint32_t numExcluded, const AttribDataSemantic* excludeSemantics)
{
  NodeBinEntry** tie = &m_attributes;

  while (*tie)
  {
    // Check if this is an excluded semantic
    AttribDataSemantic semantic = (*tie)->m_address.m_semantic;
    bool isExcluded = false;
    for (uint32_t i = 0; i < numExcluded; ++i)
    {
      if (semantic == excludeSemantics[i])
      {
        isExcluded = true;
        break;
      }
    }

    if (!isExcluded)
    {
      // Delete this tie from the list.
      NodeBinEntry* nextTie = (*tie)->m_next;

      // We are deleting a reference to an AttribData so reduce the refCount and delete the AttribData if ref count
      // goes to zero.
      (*tie)->removeReference();

      // Delete the tie itself.
      (*tie)->m_allocatorUsed->memFree(*tie);
      (*tie) = nextTie;
    }
    else
    {
      // Skip this tie as it's not being removed
      tie = &((*tie)->m_next);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBin::deleteAllAttribDataSpecified(uint32_t numIncluded, const AttribDataSemantic* includedSemantics)
{
  NodeBinEntry** tie = &m_attributes;

  while (*tie)
  {
    // Check if this is an included semantic
    AttribDataSemantic semantic = (*tie)->m_address.m_semantic;
    bool isIncluded = false;
    for (uint32_t i = 0; i < numIncluded; ++i)
    {
      if (semantic == includedSemantics[i])
      {
        isIncluded = true;
        break;
      }
    }

    if (isIncluded)
    {
      // Delete this tie from the list.
      NodeBinEntry* nextTie = (*tie)->m_next;

      // We are deleting a reference to an AttribData so reduce the refCount and delete the AttribData if ref count
      // goes to zero.
      (*tie)->removeReference();

      // Delete the tie itself.
      (*tie)->m_allocatorUsed->memFree(*tie);
      (*tie) = nextTie;
    }
    else
    {
      // Skip this tie as it's not being removed
      tie = &((*tie)->m_next);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool NodeBin::updateAttribDataLifespans(FrameCount currentFrameNo)
{
  if (m_frameLifespanUpdated == currentFrameNo)
  {
    // We have already updated the lifespans for this node this frame so early out.
    // This is possible with multiply connected pass down connections.
    return false;
  }

  // Deal with our data.
  NodeBinEntry** tie = &(m_attributes);

  // Reset the nodes queued task list here
  m_queuedTaskList = NULL;

  while (*tie)
  {
    if ((*tie)->m_lifespan != LIFESPAN_FOREVER)
    {
      (*tie)->m_lifespan--;

      if ((*tie)->m_lifespan == 0)
      {
        // We are deleting a reference to an AttribData so reduce the refCount.
        (*tie)->removeReference();

        // Remove the tie and sort out the pointers
        NodeBinEntry* temp = *tie;
        *tie = (*tie)->m_next;
        temp->m_allocatorUsed->memFree(temp);

        continue;
      }
    }

    // We should only get here when we haven't removed something
    tie = &((*tie)->m_next);
  }

  // Set this nodes attrib lifespans as having already been updated.
  m_frameLifespanUpdated = currentFrameNo;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NodeBinEntry* NodeBin::getEntry(
  AttribDataSemantic semantic,
  NodeID             targetNodeID,
  FrameCount         validFrame,
  AnimSetIndex       animSetIndex) const
{
  NodeBinEntry* nodeAttribData = m_attributes;

  if (targetNodeID == INVALID_NODE_ID)
  {
    while (nodeAttribData)
    {
      if (
        (nodeAttribData->m_address.m_semantic == semantic) // It's the right type
        &&
        (
        (nodeAttribData->m_address.m_validFrame == validFrame) ||
        (nodeAttribData->m_address.m_validFrame == VALID_FOREVER) ||
        (validFrame == VALID_FRAME_ANY_FRAME)
        )
        &&
        (
        (nodeAttribData->m_address.m_animSetIndex == animSetIndex) ||
        (nodeAttribData->m_address.m_animSetIndex == ANIMATION_SET_ANY) ||
        (animSetIndex == ANIMATION_SET_ANY)
        ))
      {
        return nodeAttribData;
      }
      nodeAttribData = nodeAttribData->m_next;
    }
  }
  else
  {
    while (nodeAttribData)
    {
      if (
        (nodeAttribData->m_address.m_semantic == semantic) // It's the right type
        &&
        (
        (nodeAttribData->m_address.m_targetNodeID == targetNodeID) ||
        (nodeAttribData->m_address.m_targetNodeID == INVALID_NODE_ID))  // It's targeting the right node (or is for all nodes that ask for this owner's attribute of this type)
        &&
        (
        (nodeAttribData->m_address.m_validFrame == validFrame) ||
        (nodeAttribData->m_address.m_validFrame == VALID_FOREVER) ||
        (validFrame == VALID_FRAME_ANY_FRAME)
        )
        &&
        (
        (nodeAttribData->m_address.m_animSetIndex == animSetIndex) ||
        (nodeAttribData->m_address.m_animSetIndex == ANIMATION_SET_ANY) ||
        (animSetIndex == ANIMATION_SET_ANY)
        ))
      {
        return nodeAttribData;
      }
      nodeAttribData = nodeAttribData->m_next;
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeBinEntry* NodeBin::getEntries() const
{
  return m_attributes;
}

//----------------------------------------------------------------------------------------------------------------------
OutputCPPin* NodeBin::getOutputCPPin(PinIndex sourceCPPinIndex) const
{
  NMP_ASSERT(m_outputCPPins);
  return m_outputCPPins + sourceCPPinIndex;
}

//----------------------------------------------------------------------------------------------------------------------
Task* NodeBin::findQueuedTaskID(TaskID taskID)
{
  Task* indexTask = m_queuedTaskList;

  while (indexTask)
  {
    if (indexTask->m_taskid == taskID)
    {
      return indexTask;
    }
    indexTask = indexTask->m_next;
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBin::addQueuedTask(Task* task)
{
  task->m_next = m_queuedTaskList; // Insert at the beginning
  m_queuedTaskList = task;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBin::clearQueuedTasks()
{
  m_queuedTaskList = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_ATTRIB_DEBUG_BUFFERING)
uint32_t NodeBin::isDebugOutputSemanticSet(AttribDataSemantic semantic)
{
  NMP_ASSERT(m_debugOutputSemanticFlags);
  return m_debugOutputSemanticFlags->isBitSet(semantic);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBin::setDebugOutputSemantic(AttribDataSemantic semantic, bool set)
{
  NMP_ASSERT(m_debugOutputSemanticFlags);
  if (set)
    m_debugOutputSemanticFlags->setBit(semantic);
  else
    m_debugOutputSemanticFlags->clearBit(semantic);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBin::setDebugOutputAllSemantics(bool set)
{
  NMP_ASSERT(m_debugOutputSemanticFlags);
  if (set)
    m_debugOutputSemanticFlags->setAll();
  else
    m_debugOutputSemanticFlags->clearAll();
}
#endif // MR_ATTRIB_DEBUG_BUFFERING

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODEBIN_H
//----------------------------------------------------------------------------------------------------------------------
