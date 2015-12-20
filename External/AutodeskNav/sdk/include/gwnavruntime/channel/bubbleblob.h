/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_BubbleBlob_H
#define Navigation_BubbleBlob_H


#include "gwnavruntime/world/runtimeblobcategory.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/math/vec3f.h"
#include "bubble.h"


namespace Kaim
{

class BubbleBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, BubbleBlob, 0)

public:
	BubbleBlob() {}

public:
	Vec3f m_referencePosition;
	Vec3f m_center;
	KyFloat32 m_radius;
	KyUInt32 m_rotationDirection;
};

inline void SwapEndianness(Endianness::Target e, BubbleBlob& self)
{
	SwapEndianness(e, self.m_referencePosition);
	SwapEndianness(e, self.m_center);
	SwapEndianness(e, self.m_radius);
	SwapEndianness(e, self.m_rotationDirection);
}


class BubbleBlobBuilder : public BaseBlobBuilder<BubbleBlob>
{
	KY_CLASS_WITHOUT_COPY(BubbleBlobBuilder)

public:
	BubbleBlobBuilder(const Bubble* bubble)
		: m_bubble(bubble)
	{}

	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_referencePosition, m_bubble->GetReferencePosition());
		BLOB_SET(m_blob->m_center, m_bubble->GetCenter());
		BLOB_SET(m_blob->m_radius, m_bubble->GetRadius());
		BLOB_SET(m_blob->m_rotationDirection, KyUInt32(m_bubble->GetRotationDirection()));
	}

private:
	const Bubble* m_bubble;
};

} // namespace Kaim

#endif // Navigation_Bubble_H
