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
#ifndef MR_SIMPLE_ANIM_RUNTIMEID_TO_FILENAME_LOOKUP_H
#define MR_SIMPLE_ANIM_RUNTIMEID_TO_FILENAME_LOOKUP_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/AnimSource/mrAnimSource.h"
#include "morpheme/mrDefines.h"
#include "morpheme/mrManager.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

namespace UTILS
{

//----------------------------------------------------------------------------------------------------------------------
/// \class SimpleAnimRuntimeIDtoFilenameLookup
///
/// Generated against a specific NetworkDef in the AssetCompiler.
/// Stores a map between animation IDs (local to a NetworkDef) and other information such as
/// animation file name, format, CRC etc.
/// Animations used by a Network are assigned Network specific runtime IDs 0 to n.
/// These IDs are stored on AnimSourceNodes and when we wish to load our NetworkDefs animations we come via an
/// instance of this class to determine the file name to load etc.
//----------------------------------------------------------------------------------------------------------------------
class SimpleAnimRuntimeIDtoFilenameLookup
{
public:
  enum CustomAssetTypes
  {
    kAsset_SimpleAnimRuntimeIDtoFilenameLookup = MR::Manager::kAsset_NumAssetTypes,
    kAsset_NumAssetTypes
  };
  // \brief initialise an instance of this class into the supplied memory.
  //
  // Increments resource pointer.
  static SimpleAnimRuntimeIDtoFilenameLookup* init(
    NMP::Memory::Resource& resource,
    uint32_t               numAnims,
    uint32_t*              animCRC,
    char**                 fileNames,
    char**                 animFormats,
    char**                 sourceFilenames,
    char**                 sourceTakenames);

  // \brief Calculate the memory required to create an instance of this class from the supplied parameters
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t  numAnims,
    char**    fileNames,
    char**    animFormats,
    char**    sourceFilenames,
    char**    sourceTakenames);

  // \brief Get the memory requirements of this instance.
  NMP::Memory::Format getInstanceMemoryRequirements();

  // \brief Prepare a dislocated instance of this class for use.
  bool locate();

  // \brief Dislocate an instance of this class ready to move to a new memory location or storing as a binary asset.
  bool dislocate();

  uint32_t    getCRC(uint32_t id) const;
  const char* getFilename(uint32_t id) const;
  const char* getFormatType(uint32_t id) const;
  uint32_t    getNumAnims() const;

protected:

  NMP::OrderedStringTable* m_animIDFilenamesTable;
  NMP::OrderedStringTable* m_animIDAnimFormatTable;
  NMP::OrderedStringTable* m_animIDSourceFilenamesTable;
  NMP::OrderedStringTable* m_animIDSourceTakenamesTable;
  uint32_t*         m_animIDFileCRCTable;

};

} // namespace UTILS

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_SIMPLE_ANIM_RUNTIMEID_TO_FILENAME_LOOKUP_H
//----------------------------------------------------------------------------------------------------------------------
