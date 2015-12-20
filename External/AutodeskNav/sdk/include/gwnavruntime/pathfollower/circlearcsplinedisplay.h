/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY

#ifndef Navigation_CircleArcSplineDisplay_H
#define Navigation_CircleArcSplineDisplay_H


#include "gwnavruntime/visualsystem/displaylistblob.h"


namespace Kaim
{

class ScopedDisplayList;
class CircleArcBlob;
class CircleArcSplineBlob;
class PositionOnCircleArcSplineBlob;

class CircleArcDisplayConfig
{
public:
	CircleArcDisplayConfig() { SetDefaults(); }
	void SetDefaults();

	VisualShapeColor m_pointColor;
	VisualColor m_segmentColor;
	VisualColor m_arcColor;
	KyFloat32 m_pointRenderSize;

	bool m_displayArcSector;
	bool m_displayArcRadius;

	// Both parameters setup the arc precision rendering: at display list feed time,
	// we use the most precise between:
	//  - m_arcRenderPrecisionMeter and 
	//  - m_arcRenderPrecisionRadian * arc radius
	KyFloat32 m_arcRenderPrecisionMeter;
	KyFloat32 m_arcRenderPrecisionRadian;
};

class CircleArcSplineDisplayConfig
{
public:
	CircleArcDisplayConfig m_circleArcDisplayConfig;
};

class PositionOnCircleArcSplineDisplayConfig
{
public:
	PositionOnCircleArcSplineDisplayConfig() { SetDefaults(); }
	void SetDefaults();

	VisualShapeColor m_pointColor;
	VisualColor m_tangentColor;
	KyFloat32 m_pointRenderSize;
};


class CircleArcSplineDisplayListBuilder
{
public:
	static void DisplayCircleArc(ScopedDisplayList* displayList, const CircleArcBlob* circleArc, const CircleArcDisplayConfig& displayConfig);
	static void DisplayCircleArcSpline(ScopedDisplayList* displayList, const CircleArcSplineBlob* spline, const CircleArcSplineDisplayConfig& displayConfig);
	static void DisplayPositionOnCircleArcSpline(ScopedDisplayList* displayList, const PositionOnCircleArcSplineBlob* positionOnSpline, const PositionOnCircleArcSplineDisplayConfig& displayConfig);
};

} // namespace Kaim

#endif
