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
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMHash.h"
#include "morpheme/mrManager.h"
#include "morpheme/Prediction/mrNetworkPredictionDef.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
const PredictionNodeModels* NetworkPredictionDef::findNodeEntry(NodeID nodeID) const
{
  for (uint32_t i = 0; i < m_numNodeModels; ++i)
  {
    if(nodeID == m_nodeModelsArray[i].getNodeID())
    {
      return &(m_nodeModelsArray[i]);
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkPredictionDef::findAllPredictionModelsForAnimSet(
  NodeID               nodeID,
  AnimSetIndex         animSetIndex,
  const PredictionModelDef** foundModels,
  uint32_t             NMP_USED_FOR_ASSERTS(foundModelsArraySize))
{
  const PredictionNodeModels* nodeEntry = findNodeEntry(nodeID);
  NMP_ASSERT(nodeEntry);
  uint32_t numModelAnimSetGroups = nodeEntry->getNumPredictionModelAnimSetGroups();

  uint32_t numFound = 0;
  for(uint32_t i = 0; i < numModelAnimSetGroups; ++i)
  {
    const PredictionModelAnimSetGroup* groupEntry = nodeEntry->getPredictionModelAnimSetGroup(i);
    NMP_ASSERT(groupEntry);

    const PredictionModelDef* model = groupEntry->getPredictionModelForAnimSet(animSetIndex);
    if (model)
    {
      NMP_ASSERT(numFound < foundModelsArraySize);
      foundModels[numFound] = model;
      ++numFound;
    }
  }
  return numFound;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkPredictionDef::locate()
{
  // Table of prediction models for lookup by associated NodeID.
  NMP::endianSwap(m_numNodeModels);
  REFIX_SWAP_PTR(PredictionNodeModels, m_nodeModelsArray);
  for(uint32_t i = 0; i < m_numNodeModels; ++i)
  {
    m_nodeModelsArray[i].locate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkPredictionDef::dislocate()
{
  // Table of prediction models for lookup by associated NodeID.
  for(uint32_t i = 0; i < m_numNodeModels; ++i)
  {
    m_nodeModelsArray[i].dislocate();
  }
  UNFIX_SWAP_PTR(PredictionNodeModels, m_nodeModelsArray);
  NMP::endianSwap(m_numNodeModels);
}


//----------------------------------------------------------------------------------------------------------------------
// NetworkPredictionDef::NameSearchEntry functions.
//----------------------------------------------------------------------------------------------------------------------
void PredictionModelAnimSetGroup::locate()
{
  MR::Manager& manager = MR::Manager::getInstance();

  REFIX_SWAP_PTR(char, m_name);
  NMP::endianSwap(m_hash);
  NMP::endianSwap(m_numAnimSets);
  NMP::endianSwap(m_numUsedAnimSets);

  // Unique prediction models with used anim set data
  REFIX_SWAP_PTR(PredictionModelDef*, m_predictionModels);
  for(uint32_t i = 0; i < m_numUsedAnimSets; ++i)
  {
    REFIX_SWAP_PTR(PredictionModelDef, m_predictionModels[i]);
    if(m_predictionModels[i])
    {
      // Find the correct locate function in manager.
      PredictionType type = m_predictionModels[i]->getType();
      NMP::endianSwap(type); // Need to endian swap the type as the model hasn't been located yet.

      // Locate the model.
      MR::PredictionModelLocateFn locateFn = manager.getPredictionModelLocateFn(type);
      NMP_ASSERT(locateFn);
      locateFn(m_predictionModels[i]);
    }
  }

  // Array of pointers to prediction models for each anim set entry
  REFIX_SWAP_PTR(PredictionModelDef*, m_animSetPredictionModelMap);
  for(uint32_t i = 0; i < m_numAnimSets; ++i)
  {
    REFIX_SWAP_PTR(PredictionModelDef, m_animSetPredictionModelMap[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PredictionModelAnimSetGroup::dislocate()
{
  MR::Manager& manager = MR::Manager::getInstance();

  // Array of pointers to prediction models for each anim set entry
  for(uint32_t i = 0; i < m_numAnimSets; ++i)
  {
    UNFIX_SWAP_PTR(PredictionModelDef, m_animSetPredictionModelMap[i]);
  }
  UNFIX_SWAP_PTR(PredictionModelDef*, m_animSetPredictionModelMap);

  // Unique prediction models with used anim set data
  for(uint32_t i = 0; i < m_numUsedAnimSets; ++i)
  {
    if(m_predictionModels[i])
    {
      // Find the correct dislocate function in manager.
      PredictionType type = m_predictionModels[i]->getType();
      
      // Dislocate the model.
      MR::PredictionModelDislocateFn dislocateFn = manager.getPredictionModelDislocateFn(type);
      NMP_ASSERT(dislocateFn);
      dislocateFn(m_predictionModels[i]);
    }
    UNFIX_SWAP_PTR(PredictionModelDef, m_predictionModels[i]);
  }
  UNFIX_SWAP_PTR(PredictionModelDef*, m_predictionModels);

  NMP::endianSwap(m_numUsedAnimSets);
  NMP::endianSwap(m_numAnimSets);
  NMP::endianSwap(m_hash);
  UNFIX_SWAP_PTR(char, m_name);
}

//----------------------------------------------------------------------------------------------------------------------
// PredictionNodeModels
//----------------------------------------------------------------------------------------------------------------------
void PredictionNodeModels::locate()
{
  NMP::endianSwap(m_nodeID);

  // Table of prediction models for lookup by hashed name.
  NMP::endianSwap(m_numModelAnimSetGroups);
  REFIX_SWAP_PTR(PredictionModelAnimSetGroup, m_modelAnimSetGroups);
  for(uint32_t i = 0; i < m_numModelAnimSetGroups; ++i)
  {
    m_modelAnimSetGroups[i].locate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PredictionNodeModels::dislocate()
{
  // Table of prediction models for lookup by hashed name.
  for(uint32_t i = 0; i < m_numModelAnimSetGroups; ++i)
  {
    m_modelAnimSetGroups[i].dislocate();
  }
  UNFIX_SWAP_PTR(PredictionModelAnimSetGroup, m_modelAnimSetGroups);
  NMP::endianSwap(m_numModelAnimSetGroups);

  NMP::endianSwap(m_nodeID);
}

//----------------------------------------------------------------------------------------------------------------------
// PredictionModelDef functions.
//----------------------------------------------------------------------------------------------------------------------
void PredictionModelDef::locate()
{
  NMP::endianSwap(m_type);
  REFIX_SWAP_PTR(char, m_name);
}

//----------------------------------------------------------------------------------------------------------------------
void PredictionModelDef::dislocate()
{
  NMP::endianSwap(m_type);
  UNFIX_SWAP_PTR(char, m_name);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
bool locateNetworkPredictionDef(uint32_t NMP_USED_FOR_ASSERTS(assetType), void* assetMemory)
{
  NMP_ASSERT(assetType == MR::Manager::kAsset_NetworkPredictionDef);
  MR::NetworkPredictionDef* networkPredictionDef = (MR::NetworkPredictionDef*)assetMemory;
  networkPredictionDef->locate();
  return true;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
