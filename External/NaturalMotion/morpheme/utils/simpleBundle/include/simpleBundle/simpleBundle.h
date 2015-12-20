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
#ifndef MR_SIMPLEBUNDLE_H
#define MR_SIMPLEBUNDLE_H
//----------------------------------------------------------------------------------------------------------------------

// Suppress warning coming from xstring in PS3 SDK.
#ifdef NM_COMPILER_SNC
#pragma diag_push
#pragma diag_suppress=1669
#endif

#include <stdio.h>

#ifdef NM_COMPILER_SNC
#pragma diag_pop
#endif

#include "NMPlatform/NMPlatform.h"
#include "morpheme/mrManager.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

namespace UTILS
{

// Message and error logging from the simple bundler.
static const uint32_t SB_MESSAGE_PRIORITY = 130;
extern NMP::PrioritiesLogger g_SBLogger;

//----------------------------------------------------------------------------------------------------------------------
/// \struct SimpleBundleHeader
/// \brief Format of header of each block of data
/// \ingroup AssetCompilerModule
///
/// This is a very simple system for packaging up and labeling binary data.
/// A bundle file is simply a series of these header structs followed by the appropriate
/// amount of data.
//----------------------------------------------------------------------------------------------------------------------
struct SimpleBundleHeader
{
  uint32_t               m_binaryVersion; ///< The version of the runtime that this asset was created with.
  uint32_t               m_platformFmt;   ///< Platform format the asset was created for (correspond to MR::Manager::PlatformFormat).
  MR::Manager::AssetType m_assetType;     ///< Type of the asset
  RuntimeAssetID         m_assetID;       ///< Asset id assigned to this asset.
  uint8_t                m_guid[16];      ///< Copy of the Morpheme Connect guid.
  NMP::Memory::Format    m_assetMemReqs;  ///< The memory requirements of the asset data following this header structure.

  /// \brief Endian swaps the header when assets are being generated for platforms
  /// that have different endianness to the current platform.
  void endianSwap();
};

//----------------------------------------------------------------------------------------------------------------------
inline void SimpleBundleHeader::endianSwap()
{
  NMP::endianSwap(m_binaryVersion);
  NMP::endianSwap(m_platformFmt);
  NMP::endianSwap(m_assetType);
  NMP::endianSwap(m_assetID);
  NMP::endianSwap(m_assetMemReqs);
}

//----------------------------------------------------------------------------------------------------------------------
/// \class SimpleBundleWriter
/// \brief Class to use to create bundle files.
/// \ingroup AssetCompilerModule
///
/// Once a FILE * has been created, this class will write SimpleBundleHeader
/// objects and handle padding for every call to write.  The writer will
/// produce binaries of the correct endianness of the target platform
/// specified by the NM_TARGET_BIGENDIAN define.  This means that the
/// SimpleBundleReader function will generally not need to endian swap the data.
/// Asset alignment greater than 128 is not supported.
//----------------------------------------------------------------------------------------------------------------------
class SimpleBundleWriter
{
public:
  SimpleBundleWriter(FILE* stream);
  SimpleBundleWriter();

  void init(FILE* stream);

  /// Write an asset of given type, ID and guid, with data specified by buffer and bufferlength, to the bundlewriter's
  ///  output stream.
  bool writeAsset(
    MR::Manager::AssetType     assetType,
    uint32_t                   assetID,
    const uint8_t*             guid,
    const void*                asset,       ///< Asset to write.
    const NMP::Memory::Format& assetMemReqs ///< Memory requirements of the asset to write.
  );

protected:

  bool padStreamToAlignment(size_t alignment);

  /// Convert ascii-form guid to binary
  void guidTextToBinary(const char* guidText, uint8_t* guidBinary);

  /// Convert a pair of ascii hex characters to their byte value; c1 representing the most significant four bits
  uint8_t convertAsciiHexPair(char c1, char c2);

  /// Convert ascii representation of a hex digit to its actual value, returning 0 for unknowns
  uint8_t asciiHexToValue(char symbol);

  FILE*      m_stream;            ///< Output stream to write to.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class SimpleBundleReader
/// \brief Handles reading in of a bundle, and iterating through all objects inside.
/// \ingroup AssetCompilerModule
///
/// Runtime targets will not need to perform any endian swapping on the data,
/// as the SimpleBundleWriter will have generated binary assets of the correct
/// endianness.  The one exception where the SimpleBundleReader function does
/// have to perform endian swapping is where it is being used connect-side to
/// read in an asset created for a target platform.
/// Asset alignment greater than 128 is not supported.
//----------------------------------------------------------------------------------------------------------------------
class SimpleBundleReader
{
public:

  /// Construct and initialise.
  SimpleBundleReader(
    void*   buffer,     ///< Must be 128 byte aligned.
    size_t  bufferSize);

  /// Read an asset from the bundleReader's buffer, setting the type, ID, guid and object data/size of the asset.
  /// This advances the reading position of the internal buffer.
  bool readNextAsset(
    MR::Manager::AssetType& assetType,
    RuntimeAssetID&         assetID,
    uint8_t*&               guid,        ///< GUID of read asset.
    void*&                  asset,       ///< Read asset.
    NMP::Memory::Format&    assetMemReqs ///< Memory requirements of the read asset.
  );

protected:
  NMP::Memory::Resource bufferResource;
};

} // namespace UTILS

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_SIMPLEBUNDLE_H
//----------------------------------------------------------------------------------------------------------------------
