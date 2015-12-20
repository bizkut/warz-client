/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_PointOfInterestDisplay_H
#define Navigation_PointOfInterestDisplay_H

#include "gwnavruntime/visualsystem/visualcolor.h"
#include "gwnavruntime/spatialization/spatializedpointdisplay.h"

namespace Kaim
{

class PointOfInterestBlob;

class PointOfInterestDisplayListBuilder
{
public:
	PointOfInterestDisplayListBuilder()
	{
		m_flagColor_Undefined = VisualColor::Red;
		m_flagColor_Custom = VisualColor::Orange;
		m_mastHeight = 2.5f;
		m_flagHeight = 0.5f;
		m_circleRadius = 0.2f;
	}

	void DisplayPointOfInterest(ScopedDisplayList* displayList, const PointOfInterestBlob* pointOfInterest);
	const VisualColor& GetColorFromPoiType(KyUInt32 pointOfInterestType);

public:
	VisualColor m_flagColor_Undefined;
	VisualColor m_flagColor_Custom;
	KyFloat32 m_mastHeight;
	KyFloat32 m_flagHeight;
	KyFloat32 m_circleRadius;
};

} // namespace Kaim

#endif // Navigation_PointOfInterestDisplay_H
