/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: MAMU
#ifndef Navigation_SpatializedPointDisplay_H
#define Navigation_SpatializedPointDisplay_H

#include "gwnavruntime/visualsystem/visualcolor.h"
#include "gwnavruntime/math/vec3f.h"

namespace Kaim
{

class ScopedDisplayList;
class PointDbSpatializationBlob;
class SpatializationResultBlob;

class SpatializedPointDisplayListBuilder
{
public:
	SpatializedPointDisplayListBuilder() { SetDefaults(); }

	void SetDefaults()
	{
		m_linkColor = VisualColor::LightYellow;
		m_spatializedPointColor_Projected = VisualColor::Lime;
		m_spatializedPointColor_NotProjected = VisualColor::Red;
		m_spatializationDetailsColor_Projected = VisualColor::Cyan;
		m_spatializationDetailsColor_NotProjected = VisualColor::Red;
		m_trianglePointColor = VisualColor::LightGreen;
		m_spatializedPointWidth = 0.1f;
		m_rangePointsWidth = 0.1f;
	}

	void DisplaySpatializationError(ScopedDisplayList* displayList,
		const Vec3f& spatializedPoint, const SpatializationResultBlob* spatializationResult, KyUInt32 databaseIndex, KyFloat32 textPositionZOffset = 3.0f);

	/// If databasIndex == KyUInt32MAXVAL, display all db spatializations, else only display spatialization for the specified databaseIndex.
	void DisplaySpatializationResult(ScopedDisplayList* displayList,
		const Vec3f& spatializedPoint, KyFloat32 height, const SpatializationResultBlob* spatializationResult, KyUInt32 databaseIndex);

	void DisplayPointDbSpatialization(ScopedDisplayList* displayList,
		const Vec3f& spatializedPoint, KyFloat32 height, const PointDbSpatializationBlob& pointDbSpatializationBlob);

	void DisplayTriangleProjection(ScopedDisplayList* displayList,
		const Vec3f& spatializedPoint, const PointDbSpatializationBlob& pointDbSpatializationBlob);

	void DisplayProjectionDetails(ScopedDisplayList* displayList,
		const Vec3f& spatializedPoint, KyFloat32 height, const PointDbSpatializationBlob& pointDbSpatializationBlob,
		const VisualColor& lineColor);

public:
	VisualColor m_linkColor;
	VisualColor m_spatializedPointColor_Projected;
	VisualColor m_spatializedPointColor_NotProjected;
	VisualColor m_spatializationDetailsColor_Projected;
	VisualColor m_spatializationDetailsColor_NotProjected;
	VisualColor m_trianglePointColor;
	KyFloat32 m_spatializedPointWidth;
	KyFloat32 m_rangePointsWidth;
};

} // namespace Kaim

#endif // Navigation_SpatializedPointDisplay_H
