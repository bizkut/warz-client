/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: GUAL - secondary contact: LASI
#ifndef GwNavGen_DynamicRasterColumn_H
#define GwNavGen_DynamicRasterColumn_H


#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavgeneration/common/boxoflists.h"
#include "gwnavgeneration/input/taggedtriangle3i.h"


namespace Kaim
{


class DynamicRasterPixel
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)
public:
	DynamicRasterPixel()
	{
		Set(0.0f, -1, KyUInt32MAXVAL, KyUInt32MAXVAL);
	}

	void Set(KyFloat32 altitude, KyInt32 pairing, KyUInt32 navTagIdx, KyUInt32 ownerIdx = KyUInt32MAXVAL)
	{
		m_altitude = altitude;
		m_pairing  = pairing;
		m_navTagIdx = navTagIdx;
		m_ownerIdx = ownerIdx;
	}

public:
	KyFloat32 m_altitude;
	KyInt32 m_pairing;
	KyUInt32 m_navTagIdx;
	KyUInt32 m_ownerIdx;

};

typedef BoxOfLists<DynamicRasterPixel, 4> DynamicRasterColumnPool;
typedef DynamicRasterColumnPool::Column DynamicRasterColumn;


}


#endif
