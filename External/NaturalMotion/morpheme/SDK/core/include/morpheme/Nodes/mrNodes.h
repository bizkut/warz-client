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
#ifndef NM_NODES_H
#define NM_NODES_H
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup NodesLibraryTransitConditionsModule Nodes Library Transition Conditions
/// \ingroup TransitConditionsModule
///
/// Transition condition types included in the Nodes Library.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup NodesLibraryBlendTreeNodesModule Nodes Library Blend Tree Nodes.
/// \ingroup BlendTreeNodesModule
///
/// Blend tree Node types included in the Nodes Library.
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// Register the default node types.
extern void registerCoreQueuingFnsAndOutputCPTasks();

//----------------------------------------------------------------------------------------------------------------------
/// Register the morpheme core attribute semantics.
extern void registerCoreAttribSemantics(
  bool computeRegistryRequirements ///< true:  Determines required array sizes for storage of registered functions, data types etc.
                                   ///< false: Registers functions, data types etc.
  );

//----------------------------------------------------------------------------------------------------------------------
extern void registerCorePredictionModelTypes();

//----------------------------------------------------------------------------------------------------------------------
extern void registerCoreNodeInitDatas();

//----------------------------------------------------------------------------------------------------------------------
extern void registerCoreAttribDataTypes();

//----------------------------------------------------------------------------------------------------------------------
extern void registerCoreTransitConditions();

//----------------------------------------------------------------------------------------------------------------------
extern void registerCoreAssets();

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_NODES_H
//----------------------------------------------------------------------------------------------------------------------
