/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_HeightFieldBlob_H
#define Navigation_HeightFieldBlob_H

#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/collision/collisiondatablobcategory.h"

namespace Kaim
{

class HeightFieldBlob
{
	KY_ROOT_BLOB_CLASS(CollisionData, HeightFieldBlob, 0)
	KY_CLASS_WITHOUT_COPY(HeightFieldBlob)

public:
	HeightFieldBlob() {}

	Vec3f                m_origin;
	BlobArray<KyFloat32> m_altitudes;
	KyFloat32            m_tileSize;
	KyUInt32             m_xAltitudeCount;
	KyUInt32             m_yAltitudeCount;
};

inline void SwapEndianness(Endianness::Target e, HeightFieldBlob& self)
{
	SwapEndianness(e, self.m_origin);
	SwapEndianness(e, self.m_altitudes);
	SwapEndianness(e, self.m_tileSize);
	SwapEndianness(e, self.m_xAltitudeCount);
	SwapEndianness(e, self.m_yAltitudeCount);
}

class HeightFieldBlobBuilder : public BaseBlobBuilder<HeightFieldBlob>
{
public:
	HeightFieldBlobBuilder(const Vec3f& origin, KyFloat32 tileSize, KyUInt32 xCount, KyUInt32 yCount) :
	  m_origin(origin), m_tileSize(tileSize), m_xAltitudeCount(xCount), m_yAltitudeCount(yCount) {}

	  void DoBuild()
	  {
		  BLOB_SET(m_blob->m_origin, m_origin);
		  BLOB_ARRAY(m_blob->m_altitudes, m_xAltitudeCount * m_yAltitudeCount);
		  BLOB_SET(m_blob->m_tileSize, m_tileSize);
		  BLOB_SET(m_blob->m_xAltitudeCount, m_xAltitudeCount);
		  BLOB_SET(m_blob->m_yAltitudeCount, m_yAltitudeCount);
	  }

	  Vec3f      m_origin;
	  KyFloat32  m_tileSize;
	  KyUInt32   m_xAltitudeCount;
	  KyUInt32   m_yAltitudeCount;
};

}

#endif //Navigation_HeightFieldBlob_H
