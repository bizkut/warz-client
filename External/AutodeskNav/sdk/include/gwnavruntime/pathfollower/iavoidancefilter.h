/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: MUBI - secondary contact: LAPA
#ifndef Navigation_IAvoidanceFilter_H
#define Navigation_IAvoidanceFilter_H

#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/kernel/SF_RefCount.h"

namespace Kaim
{

class SpatializedCylinder;
class Bot;

/// Enumerated the possible results of a call toIAvoidanceFilter::Filter.
enum AvoidanceFilterResult
{
	AvoidanceFilter_Ignore = 0, ///< Ignore the collider for Avoidance
	AvoidanceFilter_Avoid       ///< Take the collider into account for Avoidance
};


/// Interface class for potential collider filtering classes.
class IAvoidanceFilter: public RefCountBase<IAvoidanceFilter, MemStat_PathFollowing>
{
	KY_CLASS_WITHOUT_COPY(IAvoidanceFilter)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	IAvoidanceFilter() {}
	virtual ~IAvoidanceFilter() {}

	virtual AvoidanceFilterResult Filter(const Bot& referenceBot, const SpatializedCylinder& obstacle) = 0;
};

} // namespace Kaim

#endif // Navigation_IAvoidanceFilter_H
