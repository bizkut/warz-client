/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_LodMaxBlob_H
#define Navigation_LodMaxBlob_H

#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/math/box3f.h"
#include "gwnavruntime/visualsystem/displaylist.h"
#include "gwnavruntime/visualsystem/idisplaylistbuilder.h"
#include "gwnavruntime/visualsystem/displaylistblob.h"
#include "gwnavruntime/world/runtimeblobcategory.h"


namespace Kaim
{


/// Send this blob to display a shape saying that the visual element with id m_visualDebugId is at LOD max 
/// Send visualDebugIdRemovalBlob with this same visualDebugId and blobType LodMaxBlob::GetBlobType() to stop
/// displaying this shape when the lod is back to default.
/// Store the position of the shape in m_shapePosition and send this blob each time the position of the visual debug element changes.
class LodMaxBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, LodMaxBlob, 0)

public:
	LodMaxBlob() : m_visualDebugId(KyUInt32MAXVAL) {}

	KyUInt32 m_visualDebugId;   // the id of the visualdebug element.
	Vec3f m_shapePosition;      // the 3D position of the shape.
};

inline void SwapEndianness(Kaim::Endianness::Target e, LodMaxBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_shapePosition);
}



class LodMaxBlobBuilder : public BaseBlobBuilder<LodMaxBlob>
{
public:
	LodMaxBlobBuilder(KyUInt32 visualDebugId, const Vec3f& shapePosition)
		: m_visualDebugId(visualDebugId), m_shapePosition(shapePosition) {}

	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_visualDebugId, m_visualDebugId);
		BLOB_SET(m_blob->m_shapePosition, m_shapePosition);
	}
private:
	KyUInt32 m_visualDebugId;
	Vec3f m_shapePosition;
};

class LodMaxDisplayListBuilder
{
public:
	LodMaxDisplayListBuilder() : m_radius(0.3f) {}
	
	void DisplayLodMax(ScopedDisplayList* displayList, const LodMaxBlob* lodMax)
	{
		if (lodMax == KY_NULL)
			return;

		KyFloat32 m_radius = 0.3f;
		KyFloat32 radius_div_3 = m_radius / 3.0f;

		KyFloat32 fadeFactor = 0.8f;
		const VisualColor color(VisualColor::Gold);
		const VisualColor fadeColor(
			KyUInt8(color.m_r * fadeFactor),
			KyUInt8(color.m_g * fadeFactor),
			KyUInt8(color.m_b * fadeFactor));
		VisualShapeColor shapeColor(color, fadeColor);

		// Horizontal box

		Vec3f horizontalBoxMin = lodMax->m_shapePosition;
		horizontalBoxMin.x -= m_radius;
		horizontalBoxMin.y -= radius_div_3;
		horizontalBoxMin.z -= radius_div_3;

		Vec3f horizontalBoxMax = lodMax->m_shapePosition;
		horizontalBoxMax.x += m_radius;
		horizontalBoxMax.y += radius_div_3;
		horizontalBoxMax.z += radius_div_3;

		displayList->PushBox(Box3f(horizontalBoxMin, horizontalBoxMax), shapeColor);


		// Vertical box

		Vec3f verticalBoxMin = lodMax->m_shapePosition;
		verticalBoxMin.x -= radius_div_3;
		verticalBoxMin.y -= radius_div_3;
		verticalBoxMin.z -= m_radius;

		Vec3f verticalBoxMax = lodMax->m_shapePosition;
		verticalBoxMax.x += radius_div_3;
		verticalBoxMax.y += radius_div_3;
		verticalBoxMax.z += m_radius;

		displayList->PushBox(Box3f(verticalBoxMin, verticalBoxMax), shapeColor);
	}

	KyFloat32 m_radius;
};

}

#endif // Navigation_VisualDebugIdRemovalBlob_H
