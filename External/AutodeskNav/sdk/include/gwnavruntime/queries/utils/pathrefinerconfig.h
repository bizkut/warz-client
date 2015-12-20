/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_PathRefinerConfig_H
#define Navigation_PathRefinerConfig_H

#include "gwnavruntime/base/types.h"


namespace Kaim
{


class PathRefinerConfig
{
public:
	PathRefinerConfig() { SetDefault(); }
	void SetDefault();

	/// The ideal distance, in meter, the Path refiner should try to keep between
	/// the Path and the NavMesh borders.
	/// This value will be capped to be at least just a bit more than Database
	/// PathMargin, that is the absolute minimal allowed distance between the
	/// Path and NavMesh borders.
	/// \defaultvalue 0.1 m
	KyFloat32 m_idealDistanceToBorders;

	/// The minimal length, in meter, of a new edge added to cut a turn. This
	/// does not prevent having finally edges shorter than this value, since an
	/// edge can be cut afterwards to adjust an adjacent turn.
	/// \defaultvalue 0.1 m
	KyFloat32 m_minimalNewEdgeLength;

	/// The angle, in degrees, between 2 adjacent edges over which no other
	/// simplification than direct cut will be attempted.
	/// Note that the angle is taken between edges and is in fact the complementary
	/// to 180 of the direction variation on that turn.
	/// Theoretical range is [0, 180], but you should better stay near 170 degrees.
	/// \defaultvalue 170 degrees
	KyFloat32 m_thresholdAngleDeg;


public: // internal
	// This method is called at queries early stages to compute only once the internal
	// values, such as m_thresholdAngleCos, that are costly to compute from the actual
	// parameters expressed in common measures.
	void ComputeInternals();

	KyFloat32 m_thresholdAngleCos; // Computed in ComputeInternals
};

}


#endif //Navigation_PathRefinerConfig_H

