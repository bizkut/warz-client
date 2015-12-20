/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: nobody
#ifndef Navigation_RadiusProfileBlob_H
#define Navigation_RadiusProfileBlob_H

#include "gwnavruntime/pathfollower/radiusprofile.h"
#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/blob/baseblobbuilder.h"


namespace Kaim
{

class RadiusProfile;
class RadiusProfileArray;


//////////////////////////////////////////////////////////////////////////
//
// RadiusProfileArrayBlob & builder
//
//////////////////////////////////////////////////////////////////////////

class RadiusProfileBlob
{
public:
	BlobArray<char> m_name;
	BlobArray<KyFloat32> m_radiusArray;
};

inline void SwapEndianness(Endianness::Target e, RadiusProfileBlob& self)
{
	SwapEndianness(e, self.m_name);
	SwapEndianness(e, self.m_radiusArray);
}


class RadiusProfileBlobBuilder : public BaseBlobBuilder<RadiusProfileBlob>
{
	KY_CLASS_WITHOUT_COPY(RadiusProfileBlobBuilder)

public:
	RadiusProfileBlobBuilder(const RadiusProfile* radiusProfile)
		: m_radiusProfile(radiusProfile)
	{}

	virtual void DoBuild();

private:
	const RadiusProfile* m_radiusProfile;
};



//////////////////////////////////////////////////////////////////////////
//
// RadiusProfileArrayBlob & builder
//
//////////////////////////////////////////////////////////////////////////

class RadiusProfileArrayBlob
{
public:
	BlobArray<RadiusProfileBlob> m_radiusProfileArray;
};

inline void SwapEndianness(Endianness::Target e, RadiusProfileArrayBlob& self)
{
	SwapEndianness(e, self.m_radiusProfileArray);
}


class RadiusProfileArrayBlobBuilder : public BaseBlobBuilder<RadiusProfileArrayBlob>
{
	KY_CLASS_WITHOUT_COPY(RadiusProfileArrayBlobBuilder)

public:
	RadiusProfileArrayBlobBuilder(const RadiusProfileArray* radiusProfileArray)
		: m_radiusProfileArray(radiusProfileArray)
	{}

	virtual void DoBuild();

private:
	const RadiusProfileArray* m_radiusProfileArray;
};

} // namespace Kaim

#endif // Navigation_RadiusProfileBlob_H
