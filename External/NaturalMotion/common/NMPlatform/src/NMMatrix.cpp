// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include <math.h>
#include "NMPlatform/NMMatrix.h"
#include "NMPlatform/NMNorm.h"
#include "NMPlatform/NMFile.h"

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4996)
#endif

#define ScalarType float
#define NormType float
#include "NMMatrix_imp.cpp"
#undef NormType
#undef ScalarType

#define ScalarType double
#define NormType double
#include "NMMatrix_imp.cpp"
#undef NormType
#undef ScalarType

#define ScalarType int32_t
#define NormType double
#include "NMMatrix_imp.cpp"
#undef NormType
#undef ScalarType

namespace NMP
{
// Specialized template implementation stuff goes here

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<float>::fwritef(const char* filename) const
{
  FILE* file = fopen(filename, "wb");
  if (file)
  {
    char buffer[32];
    
    for (uint32_t i = 0; i < numRows(); i++)
    {
      for (uint32_t j = 0; j < numColumns(); j++)
      {
        NMP_SPRINTF(buffer, 32, "%.6f\t", element(i, j));
        fwrite(buffer, 1, strlen(buffer), file);
      }

      strcpy(buffer, "\n");
      fwrite(buffer, 1, strlen(buffer), file);
    }

    fclose(file);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<double>::fwritef(const char* filename) const
{
  NMFile file;
  file.create(filename);

  char buffer[32];

  for (uint32_t i = 0; i < numRows(); i++)
  {
    for (uint32_t j = 0; j < numColumns(); j++)
    {
      NMP_SPRINTF(buffer, 32, "%.6f\t", element(i, j));
      file.write(buffer, strlen(buffer));
    }

    strcpy(buffer, "\n");
    file.write(buffer, strlen(buffer));
  }

  file.close();
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
