// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
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
#ifndef MR_NETWORK_PREDICTION
#define MR_NETWORK_PREDICTION
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMHash.h"
#include "sharedDefines/mSharedDefines.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup NetworkPrediction API provides for fast predetermination of the behaviour of certain characteristics
///             of components of the associated NetworkDef prior to instancing or running a Network.
///           This data can vary in complexity from a very simple set of precomputed values to complex interpolable
///             prediction tables.
/// \ingroup CoreModule
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

// Uniquely identifies each prediction model class.
typedef uint32_t PredictionType;

//----------------------------------------------------------------------------------------------------------------------
/// Base class.
//----------------------------------------------------------------------------------------------------------------------
class PredictionModelDef
{
public:
  PredictionType getType() const { return m_type; }
  const char* getName() const { return m_name; }

  void setType(PredictionType type) { m_type = type; }
  void setName(const char* name) { m_name = name; }

  void locate();
  void dislocate();

protected:
  PredictionType m_type;  ///< The type of this prediction model instance.
  const char*    m_name;  ///< Unique name identifier of this prediction model instance.
};

//----------------------------------------------------------------------------------------------------------------------
/// All the animation set specific instances of a uniquely named prediction model.
//----------------------------------------------------------------------------------------------------------------------
class PredictionModelAnimSetGroup
{
public:
  void locate();
  void dislocate();

  /// \brief Get the model for the specified anim set index. Does a type check and casts to the template type.
  ///
  /// Can be NULL if there is no model for the specified Anim Set.
  template <class _PredictionModelType> 
  NM_INLINE const _PredictionModelType* getPredictionModelForAnimSet(AnimSetIndex animSetIndex) const;

  /// \brief Get the model for the specified anim set index. Returns a pointer to the base class.
  ///
  /// Can be NULL if there is no model for the specified Anim Set.
  NM_INLINE const PredictionModelDef* getPredictionModelForAnimSet(AnimSetIndex animSetIndex) const;

  NM_INLINE const char* getName() const { return m_name; }
  NM_INLINE int32_t getNameHash() const { return m_hash; }
  NM_INLINE AnimSetIndex getNumAnimSets() const { return m_numAnimSets; }
  NM_INLINE AnimSetIndex getNumUsedAnimSets() const { return m_numUsedAnimSets; }

protected:
  char*                 m_name;                   ///< Unique name of this prediction model.
  int32_t               m_hash;                   ///< Hash of the name (must be unique).
  AnimSetIndex          m_numAnimSets;            ///< The size of the anim set to prediction models map
  AnimSetIndex          m_numUsedAnimSets;        ///< The number of unique prediction models with used anim set data
  PredictionModelDef**  m_predictionModels;       ///< Array of unique prediction models with used anim set data
  PredictionModelDef**  m_animSetPredictionModelMap;  ///< Array of pointers to prediction models for each anim set entry
};

//----------------------------------------------------------------------------------------------------------------------
class PredictionNodeModels
{
public:
  void locate();
  void dislocate();

  NM_INLINE NodeID getNodeID() const { return m_nodeID; }
  NM_INLINE uint32_t getNumPredictionModelAnimSetGroups() const { return m_numModelAnimSetGroups; }
  NM_INLINE const PredictionModelAnimSetGroup* getPredictionModelAnimSetGroup(uint32_t i) const;

protected:
  NodeID                       m_nodeID;
  uint32_t                     m_numModelAnimSetGroups; ///< The number of (anim set grouped) prediction models associated with a node.
  PredictionModelAnimSetGroup* m_modelAnimSetGroups;    ///< Table of, anim set grouped, prediction models for lookup by hashed name.
};

//----------------------------------------------------------------------------------------------------------------------
/// Storage location for all prediction models.
/// Suggested use: On game or level start find the models to be used and cache pointers to them for fast access.
//----------------------------------------------------------------------------------------------------------------------
class NetworkPredictionDef
{
public:
  //---------------------------------------------------------------------
  /// \name   NetworkDef Initialisation
  /// \brief  Functions to manage the memory for the NetworkDef.
  //---------------------------------------------------------------------
  //@{
  void locate();
  void dislocate();
  //@}
  
  /// \brief Locating a prediction model via the Node it is attached to and its unique name identifier.
  ///  Note: 1 Node can have multiple associated prediction models (of the same or differing types).
  ///        A model must have 1 associated node.
  template <class _PredictionModelType> 
  NM_INLINE const _PredictionModelType* findNamedPredictionModelForAnimSet(
    NodeID       nodeID,
    const char*  modelName,
    AnimSetIndex animSetIndex) const;

  /// \brief Locating all the prediction models for for a specific NodeID and anim set.
  /// \return Num prediction models found.
  uint32_t findAllPredictionModelsForAnimSet(
    NodeID               nodeID,
    AnimSetIndex         animSetIndex,
    const PredictionModelDef** foundModels,       ///< Array filled with the found models themselves.
    uint32_t             foundModelsArraySize     ///< Size of foundModels.
    );
  
  /// \brief Find a NodeID identified lookup table entry.
  const PredictionNodeModels* findNodeEntry(NodeID nodeID) const;

  /// \brief How many nodes are there with prediction models.
  uint32_t getNumNodesWithModels() const { return m_numNodeModels; }

  /// \brief Get the node entry from the node table at the specified table index.
  NM_INLINE const PredictionNodeModels* getNodeEntry(uint32_t nodesTableIndex) const;

protected:
  /// Table of, anim set grouped, prediction models for lookup by associated NodeID.
  uint32_t                m_numNodeModels;
  PredictionNodeModels*   m_nodeModelsArray;
};

//----------------------------------------------------------------------------------------------------------------------
// NetworkPredictionDef inline functions.
//----------------------------------------------------------------------------------------------------------------------
template <class _PredictionModelType> 
NM_INLINE const _PredictionModelType* NetworkPredictionDef::findNamedPredictionModelForAnimSet(
  NodeID       nodeID,
  const char*  modelName,
  AnimSetIndex animSetIndex) const
{
  const PredictionNodeModels* nodeEntry = findNodeEntry(nodeID);
  NMP_ASSERT_MSG(nodeEntry, "Cannot find any prediction models on node %i", nodeID);

  uint32_t numModelAnimSetGroups = nodeEntry->getNumPredictionModelAnimSetGroups();
  
  // Hash the name.
  int32_t key = NMP::hashStringCRC32(modelName);

  // Locate the correct hashed name in table.
  for (uint32_t i = 0; i < numModelAnimSetGroups; ++i)
  {
    const PredictionModelAnimSetGroup* nameEntry = nodeEntry->getPredictionModelAnimSetGroup(i);
    NMP_ASSERT_MSG(nameEntry, "Cannot find requested prediction model on node %i", nodeID);

    if(key == nameEntry->getNameHash())
    {
      return nameEntry->getPredictionModelForAnimSet<_PredictionModelType>(animSetIndex);
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const PredictionNodeModels* NetworkPredictionDef::getNodeEntry(uint32_t nodesTableIndex) const
{
  NMP_ASSERT(nodesTableIndex < m_numNodeModels);
  return &(m_nodeModelsArray[nodesTableIndex]);
}

//----------------------------------------------------------------------------------------------------------------------
// PredictionModelAnimSetGroup inline functions.
//----------------------------------------------------------------------------------------------------------------------
template <class _PredictionModelType> 
NM_INLINE const _PredictionModelType* PredictionModelAnimSetGroup::getPredictionModelForAnimSet(AnimSetIndex animSetIndex) const
{
  NMP_ASSERT(animSetIndex < m_numAnimSets);
  
  // Check that we are casting to the correct PredictionType.
  NMP_ASSERT(m_animSetPredictionModelMap[animSetIndex] == NULL ? true : _PredictionModelType::getClassPredictionType() == m_animSetPredictionModelMap[animSetIndex]->getType());
  return static_cast<const _PredictionModelType*>(m_animSetPredictionModelMap[animSetIndex]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const PredictionModelDef* PredictionModelAnimSetGroup::getPredictionModelForAnimSet(AnimSetIndex animSetIndex) const
{
  NMP_ASSERT(animSetIndex < m_numAnimSets);
  return m_animSetPredictionModelMap[animSetIndex];
}

//----------------------------------------------------------------------------------------------------------------------
// PredictionNodeModels inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const PredictionModelAnimSetGroup* PredictionNodeModels::getPredictionModelAnimSetGroup(uint32_t i) const
{
  NMP_ASSERT(i < m_numModelAnimSetGroups);
  return &m_modelAnimSetGroups[i];
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
bool locateNetworkPredictionDef(uint32_t assetDesc, void* assetMemory);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NETWORK_PREDICTION
//----------------------------------------------------------------------------------------------------------------------
