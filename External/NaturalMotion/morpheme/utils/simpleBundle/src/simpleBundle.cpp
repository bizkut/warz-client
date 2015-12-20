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
#include "simpleBundle/simpleBundle.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

namespace UTILS
{

//----------------------------------------------------------------------------------------------------------------------
// SimpleBundleHeader
//----------------------------------------------------------------------------------------------------------------------

// Message and error logging from the simple bundler.
NMP::PrioritiesLogger g_SBLogger;

//----------------------------------------------------------------------------------------------------------------------
// SimpleBundleWriter
//----------------------------------------------------------------------------------------------------------------------
SimpleBundleWriter::SimpleBundleWriter(FILE* stream)
{
  init(stream);
}

//----------------------------------------------------------------------------------------------------------------------
SimpleBundleWriter::SimpleBundleWriter() :
  m_stream(NULL)
{ }

//----------------------------------------------------------------------------------------------------------------------
void SimpleBundleWriter::init(FILE* stream)
{
  m_stream = stream;
  NMP_ASSERT((ptrdiff_t)ftell(m_stream) == 0); // Must be at head of file to be able to write
  //  assets to it aligned accurately.
}

//----------------------------------------------------------------------------------------------------------------------
uint8_t SimpleBundleWriter::asciiHexToValue(char symbol)
{
  if (symbol >= '0' && symbol <= '9')
    return symbol - '0';
  if (symbol >= 'a' && symbol <= 'f')
    return 10 + symbol - 'a';
  if (symbol >= 'A' && symbol <= 'F')
    return 10 + symbol - 'A';
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
uint8_t SimpleBundleWriter::convertAsciiHexPair(char c1, char c2)
{
  return 16 * asciiHexToValue(c1) + asciiHexToValue(c2);
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleBundleWriter::guidTextToBinary(const char* guidText, uint8_t* guidBinary)
{
  NMP_ASSERT(guidBinary != NULL);
  NMP_ASSERT(guidText != NULL);
  NMP_ASSERT(NMP_STRLEN(guidText) == NMP_STRLEN("00000000-0000-0000-0000-000000000000"));
  static uint32_t guidByteOffsetOrder[] = { 0, 2, 4, 6, /*-*/ 9, 11, /*-*/ 14, 16, /*-*/ 19, 21, /*-*/ 24, 26, 28, 30, 32, 34 };
  for (uint32_t i = 0; i < 16; ++i)
  {
    char c1 = guidText[ guidByteOffsetOrder[i] ];
    char c2 = guidText[ guidByteOffsetOrder[i] + 1 ];
    guidBinary[i] = convertAsciiHexPair(c1, c2);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleBundleWriter::padStreamToAlignment(size_t alignment)
{
  // Calculate how much padding is required to achieve the alignment required from the stream start.
  ptrdiff_t currentStreamPos = (ptrdiff_t)ftell(m_stream);
  ptrdiff_t paddedStreamPos = NMP::Memory::align(currentStreamPos, alignment);
  ptrdiff_t paddingRequired = paddedStreamPos - currentStreamPos;

  // Write padding to the stream.
  for (ptrdiff_t i = 0; i < paddingRequired; ++i)
  {
    if (fputc(0, m_stream) == EOF)
      return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleBundleWriter::writeAsset(
  MR::Manager::AssetType     assetType,
  uint32_t                   assetID,
  const uint8_t*             guid,
  const void*                asset,
  const NMP::Memory::Format& assetMemReqs)
{
  NMP_ASSERT(m_stream); // We must have been initialised.

  // Assumes stream position is always aligned to NMP_NATURAL_TYPE_ALIGNMENT bytes before writing a new asset.
  //  This is because the header itself only requires natural type alignment.  The asset that follows it might be
  //  aligned to a greater boundary.
  NMP_ASSERT(NMP_IS_ALIGNED((void*)(intptr_t)ftell(m_stream), NMP_NATURAL_TYPE_ALIGNMENT));

  // Construct a header object, endian-swapping it.
  SimpleBundleHeader header;
  header.m_assetType = assetType;
  header.m_assetID = assetID;
  header.m_platformFmt = MR::Manager::getTargetPlatformAssetFmt();
  header.m_binaryVersion = MR::Manager::getRuntimeBinaryVersion();
  guidTextToBinary((const char*) guid, header.m_guid);
  header.m_assetMemReqs = assetMemReqs;

  // Endian swap the header to the requirements of the target platform.
  // This means that the reader on the target platform (generally a console) will
  // not need to do any endian-swapping.  The asset compiler
  // will generate binary assets that are correct for the target.
  header.endianSwap();

  // Write out the header to the stream
  if (1 != fwrite(&header, sizeof(SimpleBundleHeader), 1, m_stream))
  {
    return false;
  }

  // Asset mem alignment requirement can not be greater than 16 as our base alignment is fixed at 16.
  NMP_ASSERT(assetMemReqs.alignment <= NMP_VECTOR_ALIGNMENT);

  // Pad to the asset's alignment requirements.
  if (!padStreamToAlignment(assetMemReqs.alignment))
  {
    return false;
  }

  // Write out the asset data to the stream. (Assumes its already prepared for streaming; endian swapping etc.)
  if (assetMemReqs.size != fwrite(asset, 1, assetMemReqs.size, m_stream))
  {
    return false;
  }

  // Pad the stream ready for writing of next asset.
  if (!padStreamToAlignment(NMP_NATURAL_TYPE_ALIGNMENT))
  {
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// SimpleBundleReader
//----------------------------------------------------------------------------------------------------------------------
SimpleBundleReader::SimpleBundleReader(void* buffer, size_t bufferSize)
{
  // We enforce 16 byte alignment of the buffer so that the alignment of assets within the buffer can be assured.
  // Asset alignment greater than 16 is not supported however.
  NMP_ASSERT(NMP_IS_ALIGNED(buffer, NMP_VECTOR_ALIGNMENT));
  bufferResource.ptr = buffer;
  bufferResource.format.alignment = NMP_VECTOR_ALIGNMENT;
  bufferResource.format.size = bufferSize;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleBundleReader::readNextAsset(
  MR::Manager::AssetType& assetType,
  RuntimeAssetID&         assetID,
  uint8_t*&               guid,
  void*&                  asset,
  NMP::Memory::Format&    assetMemReqs)
{
  NMP_ASSERT(bufferResource.ptr);
  NMP_ASSERT(NMP_IS_ALIGNED(bufferResource.ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  //  At this point, the m_buffer pointer is pointing to a 'header' object
  // or has reached the end of the file.  We bail out here if we have reached
  // the end of the file (the last asset).  We must take into account the
  // padding that the alignment settings in the allocAndLoad() function uses in
  // the default runtime
  if (bufferResource.format.size <= sizeof(SimpleBundleHeader))
  {
    NM_LOG_MESSAGE(g_SBLogger, SB_MESSAGE_PRIORITY, "Simple bundle - Normal reading completion\n");
    return false;
  }

  // Get a 'header' pointer fixed at m_buffer location
  SimpleBundleHeader* header = (SimpleBundleHeader*)bufferResource.ptr;

  // Endian swap the data. The asset compiler will produce binary assets of the correct endianness for the target
  // platform (as described in the notes for the simple bundle writer function). This means that the following
  // function will, in normal operation on a console, do nothing.
  // The exception would be if morpheme needed to read a binary asset that was created for a console.  This is where the following
  // is needed.
  header->endianSwap();
  NM_LOG_MESSAGE(g_SBLogger, SB_MESSAGE_PRIORITY, "  Reading asset from simple bundle. AssetID: %X\n", header->m_assetID);

  // Check that this asset is for the correct platform and is not from a previous version of the runtime.
  if (header->m_platformFmt != MR::Manager::getTargetPlatformAssetFmt())
  {
    NM_LOG_ERROR_MESSAGE(g_SBLogger, "Failed reading: Asset is for incorrect platform format. Check you are using the correct asset compiler.\n");
    NM_LOG_ERROR_MESSAGE(g_SBLogger, "Failed reading: This platform requires assets in %s format.\n", NM_PLATFORM_FORMAT_STRING);
    // MORPH-15912 There should be a better way of reporting this error than a forced break at this point.
    NM_BREAK();
    return false;
  }

  if (header->m_binaryVersion != MR::Manager::getRuntimeBinaryVersion())
  {
    NM_LOG_ERROR_MESSAGE(g_SBLogger, "Failed reading: Asset is for incorrect runtime version. Check you are using the correct asset compiler.\n");
    NM_LOG_ERROR_MESSAGE(g_SBLogger, "Failed reading: Asset type %u is version %i; runtime library is version %i.\n", header->m_assetType, header->m_binaryVersion, MR::Manager::getRuntimeBinaryVersion());
    // MORPH-15912 There should be a better way of reporting this error than a forced break at this point.
    NM_BREAK();
    return false;
  }

  NMP_ASSERT(header->m_assetMemReqs.alignment >= NMP_NATURAL_TYPE_ALIGNMENT);

  if (header->m_assetMemReqs.alignment > bufferResource.format.alignment)
  {
    NM_LOG_MESSAGE(
      g_SBLogger,
      SB_MESSAGE_PRIORITY,
      "  The alignment requirements of the asset (%i) can not be greater than the overall alignment of the source file buffer (%i)\n",
      header->m_assetMemReqs.alignment,
      bufferResource.format.alignment);
    return false;
  }

  // Move the buffer pointer on to a point immediately after the header
  // object and reduce the 'm_bufferLength' (which is really the amount of
  // buffer left to go before the end) by the header size.  The buffer pointer
  // should be pointing to the first byte of the asset itself.
  bufferResource.increment(sizeof(SimpleBundleHeader));
  bufferResource.align(header->m_assetMemReqs.alignment);

  // Bail out here if the remaining buffer area is less than what the header
  // says this asset should be (meaning: the file is too short and/or the header
  // is corrupt)
  NM_LOG_MESSAGE(
    g_SBLogger,
    SB_MESSAGE_PRIORITY,
    "    Remaining bytes in buffer = %i; Asset size = %i; Asset alignment = %i\n",
    bufferResource.format.size,
    header->m_assetMemReqs.size,
    header->m_assetMemReqs.alignment);

  if (bufferResource.format.size < header->m_assetMemReqs.size)
  {
    NM_LOG_ERROR_MESSAGE(g_SBLogger, "Failed reading completion: corrupt file?\n");
    return false;
  }

  // Record the header data for the current asset and advance m_buffer
  // to the next header position, ready for the next call to this function.
  assetType = (MR::Manager::AssetType) header->m_assetType;
  assetID = header->m_assetID;
  guid = &header->m_guid[0];
  asset = bufferResource.ptr;
  assetMemReqs = header->m_assetMemReqs;

  bufferResource.increment(header->m_assetMemReqs);
  bufferResource.align(NMP_NATURAL_TYPE_ALIGNMENT);

  return true;
}

} // namespace UTILS

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
