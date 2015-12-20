/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavMeshUtils_H
#define Navigation_NavMeshUtils_H


#include "gwnavruntime/navmesh/navmeshtypes.h"


namespace Kaim
{
/// Retrieves the minimum and maximum values from the specified inputs.
/// \param a					The first value.
/// \param b					The second value.
/// \param[out] minValue		The lowest of the supplied values.
/// \param[out] maxValue		The largest of the supplied values.
KY_INLINE void GetMinMax(const KyFloat32 a, const KyFloat32 b, KyFloat32& minValue, KyFloat32& maxValue)
{
	minValue = Kaim::Min(a, b);
	maxValue = Kaim::Max(a, b);
}

/// Retrieves the minimum and maximum values from the specified inputs.
/// \param a					The first value.
/// \param b					The second value.
/// \param c					The third value.
/// \param[out] minValue		The lowest of the supplied values.
/// \param[out] maxValue		The largest of the supplied values. 
KY_INLINE void GetMinMax(const KyFloat32 a, const KyFloat32 b, const KyFloat32 c, KyFloat32& minValue, KyFloat32& maxValue)
{
	minValue = Kaim::Min(Kaim::Min(a, b), c);
	maxValue = Kaim::Max(Kaim::Max(a, b), c);
}

/// Retrieves the minimum and maximum values from the specified inputs.
/// \param a					The first value.
/// \param b					The second value.
/// \param c					The third value.
/// \param d					The fourth value.
/// \param[out] minValue		The lowest of the supplied values.
/// \param[out] maxValue		The largest of the supplied values.   
KY_INLINE void GetMinMax(const KyFloat32 a, const KyFloat32 b, const KyFloat32 c, const KyFloat32 d, KyFloat32& minValue, KyFloat32& maxValue)
{
	minValue = Kaim::Min(Kaim::Min(a, b), Kaim::Min(c, d));
	maxValue = Kaim::Max(Kaim::Max(a, b), Kaim::Max(c, d));
}

}

#endif

