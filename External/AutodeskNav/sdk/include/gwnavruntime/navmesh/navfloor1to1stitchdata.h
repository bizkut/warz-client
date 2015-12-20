/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavFloor1To1StitchData_H
#define Navigation_NavFloor1To1StitchData_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/base/types.h"

namespace Kaim
{

class Stitch1To1EdgeLink;
class NavFloor1To1StitchDataBlob;

class NavFloor1To1StitchData
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
	KY_CLASS_WITHOUT_COPY(NavFloor1To1StitchData)
public:
	NavFloor1To1StitchData() : m_floor1To1StitchDataBlob(KY_NULL), m_stitch1To1EdgeLinkBuffer(KY_NULL), m_numberOfLink(0), m_numberOfUnconnectedLink(0) {}

	const NavFloor1To1StitchDataBlob* m_floor1To1StitchDataBlob;
	Stitch1To1EdgeLink* m_stitch1To1EdgeLinkBuffer;
	KyUInt16 m_numberOfLink;
	KyUInt16 m_numberOfUnconnectedLink;
};



}

#endif //Navigation_NavFloor_H

