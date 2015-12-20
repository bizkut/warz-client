/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_LivePathBlob_H
#define Navigation_LivePathBlob_H

#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/world/runtimeblobcategory.h"
#include "gwnavruntime/path/positiononpathblob.h"
#include "gwnavruntime/queries/blobs/pathfinderqueryblob.h"
#include "gwnavruntime/world/worldelement.h"


namespace Kaim
{

class LivePath;


//////////////////////////////////////////////////////////////////////////
// LivePathDetailsBlob
class LivePathDetailsBlob
{
public:
	LivePathDetailsBlob() : m_newPathSource(PathSource_NoPath) {}

	KyUInt32 m_newPathSource;
	PositionOnPathBlob m_lowerBound;
	PositionOnPathBlob m_upperBound;
	BlobArray<PositionOnPathBlob> m_pathEventPositionOnPaths;
	BlobArray<KyUInt8> m_pathEventCheckPointStatus;
};

inline void SwapEndianness(Endianness::Target e, LivePathDetailsBlob& self)
{
	SwapEndianness(e, self.m_newPathSource);
	SwapEndianness(e, self.m_lowerBound);
	SwapEndianness(e, self.m_upperBound);
	SwapEndianness(e, self.m_pathEventPositionOnPaths);
	SwapEndianness(e, self.m_pathEventCheckPointStatus);
}

class LivePathDetailsBlobBuilder: public BaseBlobBuilder<LivePathDetailsBlob>
{
	KY_CLASS_WITHOUT_COPY(LivePathDetailsBlobBuilder)

public:
	LivePathDetailsBlobBuilder(const LivePath* livePath): m_livePath(livePath) {}
	~LivePathDetailsBlobBuilder() {}

private:
	virtual void DoBuild();

	const LivePath* m_livePath;
};


//////////////////////////////////////////////////////////////////////////
// LivePathBlob
class LivePathBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, LivePathBlob, 0)

public:
	LivePathBlob() : m_pathType(PathSource_NoPath), m_pathValidityStatus(PathIsValid) {}

	KyUInt32 m_visualDebugId;
	KyFloat32 m_botHeight;
	Vec3f m_botPosition;
	PathFinderQueryBlob m_pathFinderInfo;
	BlobRef<LivePathDetailsBlob> m_details;
	KyUInt32 m_pathType;
	KyUInt32 m_pathValidityStatus;
};

inline void SwapEndianness(Endianness::Target e, LivePathBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_botHeight);
	SwapEndianness(e, self.m_botPosition);
	SwapEndianness(e, self.m_pathFinderInfo);
	SwapEndianness(e, self.m_details);
	SwapEndianness(e, self.m_pathType);
	SwapEndianness(e, self.m_pathValidityStatus);
}

class LivePathBlobBuilder: public BaseBlobBuilder<LivePathBlob>
{
	KY_CLASS_WITHOUT_COPY(LivePathBlobBuilder)

public:
	LivePathBlobBuilder(const LivePath* livePath, const Vec3f& botPos, KyFloat32 height)
		: m_livePath(livePath)
		, m_botPos(botPos)
		, m_botHeight(height)
		, m_visualDebugId(0)
	{}

	~LivePathBlobBuilder() {}

private:
	virtual void DoBuild();

public:
	const LivePath* m_livePath;
	const Vec3f m_botPos;
	KyFloat32 m_botHeight;
	KyUInt32 m_visualDebugId;
};

} // namespace Kaim

#endif // Navigation_LivePathBlob_H
