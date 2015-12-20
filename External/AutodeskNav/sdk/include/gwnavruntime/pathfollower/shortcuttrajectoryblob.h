/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: NOBODY

#ifndef Navigation_ShortcutTrajectory_Blob_H
#define Navigation_ShortcutTrajectory_Blob_H

#include "gwnavruntime/world/runtimeblobcategory.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/math/vec3f.h"


namespace Kaim
{

class ShortcutTrajectoryBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, ShortcutTrajectoryBlob, 0)

public:
	ShortcutTrajectoryBlob() {}

	KyUInt32 m_visualDebugId;
	Vec3f m_target;
};

inline void SwapEndianness(Endianness::Target e, ShortcutTrajectoryBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_target);
}

class ShortcutTrajectoryBlobBuilder : public BaseBlobBuilder<ShortcutTrajectoryBlob>
{
	KY_CLASS_WITHOUT_COPY(ShortcutTrajectoryBlobBuilder)

public:
	ShortcutTrajectoryBlobBuilder(KyUInt32 visualDebugId, const Vec3f& target) : m_visualDebugId(visualDebugId), m_target(target) {}

	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_visualDebugId, m_visualDebugId);
		BLOB_SET(m_blob->m_target, m_target);
	}

private:
	const KyUInt32 m_visualDebugId;
	const Vec3f& m_target;
};

} // namespace Kaim

#endif
