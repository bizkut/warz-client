/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Contacts: LAPA
#ifndef Navigation_BoxObstacleDisplay_H
#define Navigation_BoxObstacleDisplay_H

#include "gwnavruntime/visualsystem/visualcolor.h"

namespace Kaim
{

class ScopedDisplayList;
class BoxObstacleBlob;

class BoxObstacleDisplayListBuilder
{
public:
	BoxObstacleDisplayListBuilder()
	{
		m_boxColor = VisualColor::Green;
		m_obstacleLinearVelocityColor = VisualColor::LightGreen;
		m_obstacleAngularVelocityColor = VisualColor::LightCyan;
		m_obstacleAngularSpeedColor = VisualColor::Cyan;
		m_spatializedCylindersColor_Projected = VisualColor::Lime;
		m_spatializedCylindersColor_NotProjected = VisualColor::Red;
		m_spatializedCylindersVelocityColor = VisualColor::White;
		m_spatializationLinkColor = VisualColor::LightYellow;
		m_linearVelocityArrowWidth = 0.05f;
		m_angularVelocityArrowWidth = 0.05f;
		m_boxAlpha = 255;
	}

	void DisplayOutline(ScopedDisplayList* displayList, const BoxObstacleBlob* boxObstacle, VisualColor& obstacleColor, VisualColor& obstacleLineColor);
	void DisplayStatus(ScopedDisplayList* displayList, const BoxObstacleBlob* boxObstacle);
	void DisplayTransform(ScopedDisplayList* displayList, const BoxObstacleBlob* boxObstacleBlob);
	void DisplayBoxVelocities(ScopedDisplayList* displayList, const BoxObstacleBlob* boxObstacleBlob);
	
public:
	VisualColor m_boxColor;
	VisualColor m_obstacleLinearVelocityColor;
	VisualColor m_obstacleAngularVelocityColor;
	VisualColor m_obstacleAngularSpeedColor;
	VisualColor m_spatializedCylindersColor_Projected;
	VisualColor m_spatializedCylindersColor_NotProjected;
	VisualColor m_spatializedCylindersVelocityColor;
	VisualColor m_spatializationLinkColor;
	KyFloat32 m_linearVelocityArrowWidth;
	KyFloat32 m_angularVelocityArrowWidth;

	KyUInt8 m_boxAlpha;
	bool m_drawAsWireframe;

	bool m_forceBoxRender;
	bool m_renderTransform;
	bool m_renderBoxVelocities;
};

}

#endif

