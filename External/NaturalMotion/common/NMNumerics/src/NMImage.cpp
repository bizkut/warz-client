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
#include <string>
#include "NMNumerics/NMImage.h"

//----------------------------------------------------------------------------------------------------------------------
// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4996)
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// Image
//----------------------------------------------------------------------------------------------------------------------
Image::Image()
{
}

//----------------------------------------------------------------------------------------------------------------------
Image::Image(uint32_t numRows, uint32_t numColumns) : Matrix<float>(numRows, numColumns)
{
}

//----------------------------------------------------------------------------------------------------------------------
Image::Image(const Image& img) : Matrix<float>(img)
{
}

//----------------------------------------------------------------------------------------------------------------------
Image::Image(const Matrix<float>& img) : Matrix<float>(img)
{
}

//----------------------------------------------------------------------------------------------------------------------
Image::Image(const char* fileName)
{
  loadPGM(fileName);
}

//----------------------------------------------------------------------------------------------------------------------
Image::~Image()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool Image::loadPGM(const char* fileName)
{
  uint32_t imgType, imgWidth, imgHeight, imgMaxVal;
  size_t bytesRead, indx;
  NMP_ASSERT(fileName);

  //------------------------------
  FILE* file = fopen(fileName, "rb");
  if (file == NULL)
    return false;
  fseek(file, 0, SEEK_SET);

  //------------------------------
  // Read the PGM header
  if (!readHeader(file, imgType, imgWidth, imgHeight, imgMaxVal))
  {
    fclose(file);
    return false;
  }

  // Support binary PGM only
  uint32_t numSamples = imgWidth * imgHeight;
  if (imgType != 5 || numSamples == 0 || imgMaxVal > 255)
  {
    fclose(file);
    return false;
  }

  // Allocate the memory for the image data
  Matrix<float> data(imgHeight, imgWidth);
  swap(data);

  //------------------------------
  // Read the pixel data
  uint8_t* buffer = new uint8_t[numSamples];
  bytesRead = fread(buffer, 1, numSamples, file);
  if (bytesRead != numSamples)
  {
    delete[] buffer;
    fclose(file);
    return false;
  }

  indx = 0;
  for (uint32_t r = 0; r < imgHeight; ++r)
  {
    for (uint32_t c = 0; c < imgWidth; ++c, ++indx)
    {
      element(r, c) = (float)buffer[indx];
    }
  }

  delete[] buffer;

  //------------------------------
  fclose(file);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Image::savePGM(const char* fileName) const
{
  uint32_t imgWidth, imgHeight, numSamples;
  size_t bytesWritten, indx;
  float val;
  NMP_ASSERT(fileName);

  //------------------------------
  FILE* file = fopen(fileName, "wb");
  if (file == NULL)
    return false;

  //------------------------------
  // Write the PGM header
  imgWidth = numColumns();
  imgHeight = numRows();
  if (!writeHeader(file, 5, imgWidth, imgHeight, 255))
  {
    fclose(file);
    return false;
  }

  //------------------------------
  // Write the pixel data
  numSamples = imgWidth * imgHeight;
  uint8_t* buffer = new uint8_t[numSamples];

  indx = 0;
  for (uint32_t r = 0; r < imgHeight; ++r)
  {
    for (uint32_t c = 0; c < imgWidth; ++c, ++indx)
    {
      val = NMP::clampValue(element(r, c), 0.0f, 255.0f);
      buffer[indx] = (uint8_t)(val + 0.5f);
    }
  }

  bytesWritten = fwrite(buffer, 1, numSamples, file);
  delete[] buffer;
  fclose(file);

  return (bytesWritten == numSamples);
}

//----------------------------------------------------------------------------------------------------------------------
bool Image::readValue(FILE* file, uint32_t& result) const
{
  NMP_ASSERT(file);
  int ch;

  // Skip whitespace etc.
  do {
    ch = fgetc(file);
    if (feof(file) || ferror(file))
      return false;
  } while (isspace(ch));

  // Skip comment lines
  while (ch == '#')
  {
    // Find carriage return symbol
    do {
      ch = fgetc(file);
      if (feof(file) || ferror(file))
        return false;
    } while (ch != '\n');

    // Eat white space
    do {
      ch = fgetc(file);
      if (feof(file) || ferror(file))
        return false;
    } while (isspace(ch));
  }
  ungetc(ch, file); // Put character back

  // Read the value
  if (fscanf(file, "%u", &result) != 1)
    return false;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Image::readHeader(
  FILE*     file,
  uint32_t& imgType,
  uint32_t& imgWidth,
  uint32_t& imgHeight,
  uint32_t& imgMaxVal) const
{
  int ch;
  NMP_ASSERT(file);

  ch = fgetc(file);
  if (ch != 'P') return false;
  imgType = fgetc(file) - (uint32_t)'0';

  // Read the image width
  if (!readValue(file, imgWidth))
    return false;

  // Read the image height
  if (!readValue(file, imgHeight))
    return false;

  // Read the maximum value
  if (!readValue(file, imgMaxVal))
    return false;

  // Eat a single character (usually newline)
  ch = fgetc(file);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Image::writeHeader(
  FILE*    file,
  uint32_t imgType,
  uint32_t imgWidth,
  uint32_t imgHeight,
  uint32_t imgMaxVal) const
{
  NMP_ASSERT(file);
  fprintf(file, "P%u\n", imgType);
  fprintf(file, "%u %u\n", imgWidth, imgHeight);
  fprintf(file, "%u\n", imgMaxVal);
  return !ferror(file);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
