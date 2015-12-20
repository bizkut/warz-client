// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

// Specializes template implementations for the type ScalarType.
#ifndef ScalarType
  #error This file should not be compiled directly
#endif

// Check if the tolerances have been defined
#ifndef NMNORM_DWARF
  #error Lower bound tolorance undefined
#endif

#ifndef NMNORM_GIANT
  #error Upper bound tolorance undefined
#endif

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
template <>
ResultType pythag(ScalarType a, ScalarType b)
{
  const ResultType absa = fabs((ResultType)a);
  const ResultType absb = fabs((ResultType)b);

  if (absa > absb)
  {
    // |a| sqrt(1 + (|b|/|a|)^2)
    const ResultType u = absb / absa;
    return absa * sqrt(1 + u * u);
  } else {
    // |b| sqrt(1 + (|a|/|b|)^2)
    if (absb == 0) return 0;
    const ResultType u = absa / absb;
    return absb * sqrt(1 + u * u);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ResultType pythag(ScalarType a, ScalarType b, ScalarType c)
{
  ResultType absa = fabs((ResultType)a);
  ResultType absb = fabs((ResultType)b);
  ResultType absc = fabs((ResultType)c);

  if (absa > absb)
  {
    if (absa > absc)
    {
      ResultType u = absb / absa;
      ResultType v = absc / absa;
      return absa * sqrt(1 + u * u + v * v);
    }
    else
    {
      ResultType u = absa / absc;
      ResultType v = absb / absc;
      return absc * sqrt(1 + u * u + v * v);
    }
  }
  if (absb > absc)
  {
    ResultType u = absa / absb;
    ResultType v = absc / absb;
    return absb * sqrt(1 + u * u + v * v);
  }
  if (absc == 0) return 0;
  ResultType u = absa / absc;
  ResultType v = absb / absc;
  return absc * sqrt(1 + u * u + v * v);
}

//----------------------------------------------------------------------------------------------------------------------
// Function computes the Euclidean norm of the vector x.
// The Euclidean norm is computed by accumulating the sum of squares in three
// different sums. The sums of squares for the small and large components are
// scaled so that no overflows occur.
template <>
ResultType enorm(uint32_t n, const ScalarType* x)
{
  const ResultType rdwarf = NMNORM_DWARF;
  const ResultType rgiant = NMNORM_GIANT;

  ResultType u;
  ResultType sg, si, sd, xabs, xgmax, xdmax, agiant;
  uint32_t i;

  // Simple vector checks
  if (n == 0) return 0;
  if (n == 1) return fabs((ResultType)x[0]);
  if (n == 2) return pythag<ScalarType, ResultType>(x[0], x[1]);

  // Set up summation parameters
  sg = 0;
  si = 0;
  sd = 0;
  xgmax = 0;
  xdmax = 0;
  agiant = rgiant / (ResultType)n;

  for (i = 0; i < n; ++i)
  {
    xabs = fabs((ResultType)x[i]);

    // Sum for intermediate components
    if (xabs > rdwarf && xabs < agiant)
    {
      si += xabs * xabs;
      continue;
    }

    // Sum for small components
    if (xabs <= rdwarf)
    {
      // Check if value is less than the current maximum
      if (xabs <= xdmax) {
        if (xabs != 0) {
          u = xabs / xdmax;
          sd += u * u;
        }
      } else {
        u = xdmax / xabs;
        sd = 1 + sd * (u * u);
        xdmax = xabs;
      }
      continue;
    }

    // Sum for large components
    if (xabs <= xgmax)
    {
      u = xabs / xgmax;
      sg += u * u;
    } else {
      u = xgmax / xabs;
      sg = 1 + sg * (u * u);
      xgmax = xabs;
    }
  }

  // Calculation of the norm
  if (sg > 0) return xgmax * sqrt(sg + (si / xgmax / xgmax)); // Giant
  if (si == 0) return xdmax * sqrt(sd); // Small only

  // Intermediate components
  if (si >= xdmax)
  {
    return sqrt(si * (1 + xdmax / si * (xdmax * sd)));
  }
  return sqrt(xdmax * (si / xdmax + xdmax * sd));
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ResultType norm1(uint32_t n, const ScalarType* x)
{
  if (n == 0) return 0;

  ResultType val = fabs((ResultType)x[0]);
  for (uint32_t i = 1; i < n; ++i) val += fabs((ResultType)x[i]);

  return val;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ResultType inftynorm(uint32_t n, const ScalarType* x)
{
  if (n == 0) return 0;

  ResultType xmax, val;
  xmax = fabs((ResultType)x[0]);
  for (uint32_t i = 1; i < n; ++i)
  {
    val = fabs((ResultType)x[i]);
    if (val > xmax) xmax = val;
  }
  return xmax;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ResultType enorm(Vector<ScalarType>& v)
{
  return enorm<ScalarType, ResultType>(v.numElements(), &v[0]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ResultType norm1(Vector<ScalarType>& v)
{
  return norm1<ScalarType, ResultType>(v.numElements(), &v[0]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ResultType inftynorm(Vector<ScalarType>& v)
{
  return inftynorm<ScalarType, ResultType>(v.numElements(), &v[0]);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
