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
#ifndef NM_IMAGE_H
#define NM_IMAGE_H

//----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include "NMPlatform/NMMatrix.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Class that represents a greyscale image
///
class Image : public Matrix<float>
{
public:
  //---------------------------------------------------------------------
  Image();
  Image(uint32_t numRows, uint32_t numColumns);
  Image(const Image& img);
  Image(const Matrix<float>& img);
  Image(const char* fileName);
  ~Image();

  uint32_t getWidth() const { return numColumns(); }
  uint32_t getHeight() const { return numRows(); }

  //---------------------------------------------------------------------
  bool loadPGM(const char* fileName);
  bool savePGM(const char* fileName) const;

protected:
  bool readValue(
    FILE*     file,
    uint32_t& result) const;

  bool readHeader(
    FILE*     file,
    uint32_t& imgType,
    uint32_t& imgWidth,
    uint32_t& imgHeight,
    uint32_t& imgMaxVal) const;

  bool writeHeader(
    FILE*    file,
    uint32_t imgType,
    uint32_t imgWidth,
    uint32_t imgHeight,
    uint32_t imgMaxVal) const;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_IMAGE_H
