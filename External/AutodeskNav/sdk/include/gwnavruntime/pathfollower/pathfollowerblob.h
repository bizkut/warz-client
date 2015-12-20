/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: LAPA
#ifndef Navigation_PathFollowerBlob_H
#define Navigation_PathFollowerBlob_H

#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/world/runtimeblobcategory.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/path/livepathblob.h"
#include "gwnavruntime/path/positiononlivepathblob.h"


namespace Kaim
{

class Bot;

class PathFollowerBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, PathFollowerBlob, 1)

public:
	KyUInt32 m_visualDebugId;
	PositionOnLivePathBlob m_progressOnPath;
	KyUInt8 m_progressOnPathStatus;
	KyUInt8 m_doValidateCheckPoint;
	KyUInt8 m_doComputeTrajectory;
	KyUInt8 m_avoidanceResult;
	KyUInt8 m_pathValidityStatus;
};

inline void SwapEndianness(Endianness::Target e, PathFollowerBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_progressOnPath);
	SwapEndianness(e, self.m_progressOnPathStatus);
	SwapEndianness(e, self.m_doValidateCheckPoint);
	SwapEndianness(e, self.m_doComputeTrajectory);
	SwapEndianness(e, self.m_avoidanceResult);
	SwapEndianness(e, self.m_pathValidityStatus);
}

class PathFollowerBlobBuilder: public BaseBlobBuilder<PathFollowerBlob>
{
	KY_CLASS_WITHOUT_COPY(PathFollowerBlobBuilder)

public:
	PathFollowerBlobBuilder(const Bot* bot): m_bot(bot) {}
	~PathFollowerBlobBuilder() {}

private:
	virtual void DoBuild();

	const Bot* m_bot;
};

} // namespace Kaim

#endif // Navigation_PathFollowerBlob_H
