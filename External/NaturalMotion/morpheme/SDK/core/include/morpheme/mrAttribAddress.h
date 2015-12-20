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
#ifndef MR_ATTRIB_ADDRESS
#define MR_ATTRIB_ADDRESS

#include "NMPlatform/NMMemory.h"
#include "morpheme/mrDefines.h"

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
const FrameCount VALID_FOREVER = 0xFFFFFFFF;         ///< If an AttribData is valid forever, it's valid frame will be
                                                     ///<  VALID_FOREVER.
const FrameCount VALID_FRAME_ANY_FRAME = 0xFFFFFFFD; ///< Used when searching for AttribData to denote that any frame's
                                                     ///<  data is an acceptable result.

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribAddress
/// \brief Every piece of attribute data that exists, whether instance or def data, has an AttribAddress which uniquely
///        identifies it and allow it to be located by search functions.
/// \see MR::AttribDataSemantic
/// \see MR::NodeID
/// \see MR::Network::getAttribDataNodeBinEntry
/// \see MR::Network::getAttribData
/// \see MR::Network::getAttribDataNodeBinEntryRecurseFilterNodes
/// \see MR::Network::getAttribDataNodeBinEntryRecurseFilterNodes
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribAddress
{
public:
  AttribAddress()
  {
    m_semantic = ATTRIB_SEMANTIC_NA;
    m_owningNodeID = INVALID_NODE_ID;
    m_targetNodeID = INVALID_NODE_ID;
    m_validFrame = VALID_FOREVER;
    m_animSetIndex = 0;
  }

  AttribAddress(
    AttribDataSemantic semantic,
    NodeID             owningNodeID,
    NodeID             targetNodeID = INVALID_NODE_ID,
    FrameCount         validFrame = VALID_FOREVER,
    AnimSetIndex       animSetIndex = ANIMATION_SET_ANY)
  {
    init(semantic, owningNodeID, targetNodeID, validFrame, animSetIndex);
  };

  NM_INLINE void init(
    AttribDataSemantic semantic,
    NodeID             owningNodeID,
    NodeID             targetNodeID = INVALID_NODE_ID,
    FrameCount         validFrame = VALID_FOREVER,
    AnimSetIndex       animSetIndex = ANIMATION_SET_ANY)
  {
    m_semantic = semantic;
    m_owningNodeID = owningNodeID;
    m_targetNodeID = targetNodeID;
    m_validFrame = validFrame;
    m_animSetIndex = animSetIndex;
  };

  NM_INLINE void endianSwap()
  {
    NMP::endianSwap(m_semantic);
    NMP::endianSwap(m_owningNodeID);
    NMP::endianSwap(m_targetNodeID);
    NMP::endianSwap(m_validFrame);
    NMP::endianSwap(m_animSetIndex);
  }

  NM_INLINE const char* getAttribSemanticName() const { return getAttribSemanticName(m_semantic); }

  static const char* getAttribSemanticName(AttribDataSemantic smt);

public:
  NodeID             m_owningNodeID;  ///< The node that this attribute belongs to.
  NodeID             m_targetNodeID;  ///< The node that this attribute is updated for use by.
                                      ///<  For example 1 of several child nodes on a BlendN.
  AttribDataSemantic m_semantic;      ///< Semantic of attribute this is an address of.
  AnimSetIndex       m_animSetIndex;  ///< Allows addressing of animation set specific attribute data.
                                      ///<  Bits of AttribData with different AnimSetIndexes will be created when the networks ActiveAnimSet changes.
  FrameCount         m_validFrame;    ///< The frame index for which this data is valid. Usually the frame it was created or VALID_FOREVER.
                                      ///<  Used when searching for a piece of AttribData from the Network AttribDatas,
                                      ///<  or Task ParamList.
};

//----------------------------------------------------------------------------------------------------------------------
/// \struct MR::CPConnection
/// \brief Describes a control parameter output pin and is stored by other nodes to identify an input.
///
/// 
/// This is how output control parameter data is addressed.
/// Is the output pin of the node that we are connected to. We will be using this nodes output as our input.
//----------------------------------------------------------------------------------------------------------------------
struct CPConnection
{
  NM_INLINE void set(NodeID sourceNodeID, PinIndex sourcePinIndex)
  {
    m_sourceNodeID = sourceNodeID;
    m_sourcePinIndex = sourcePinIndex;
  }

  NM_INLINE void endianSwap()
  {
    NMP::endianSwap(m_sourceNodeID);
    NMP::endianSwap(m_sourcePinIndex);
  }

  NodeID   m_sourceNodeID;   ///< The ID of the node we are connected to.
  PinIndex m_sourcePinIndex; ///< The index of the output pin that we are connected to on the source node.
};

}

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ATTRIB_ADDRESS
//----------------------------------------------------------------------------------------------------------------------
