/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: none
#ifndef KyRuntime_SpatializedCylinderDisplay_H
#define KyRuntime_SpatializedCylinderDisplay_H

#include "gwnavruntime/visualsystem/visualcolor.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/visualsystem/visualshapecolor.h"

namespace Kaim
{

class ScopedDisplayList;
class SpatializedCylinderBlob;
class SpatializedCylinderConfigBlob;
class SpatializationResultBlob;
class BotConfigBlob;
class FrontDirectionBlob;

class SpatializedCylinderDisplayVelocityConfig
{
public:
	Vec3f m_velocity;
	VisualShapeColor m_color;
	KyFloat32 m_altitudeRatio;
	KyFloat32 m_headWidth;	
};

class SpatializedCylinderDisplayListBuilder
{
public:
	static void DisplayBotVelocity(ScopedDisplayList* displayList, const BotConfigBlob* botConfigBlob, 
		const SpatializedCylinderBlob* spatializedCylinderBlob, 
		const SpatializedCylinderConfigBlob* spatializedCylinderConfigBlob, 
		const FrontDirectionBlob* frontDirectionBlob, 
		SpatializedCylinderDisplayVelocityConfig velocityConfig);

public:
	SpatializedCylinderDisplayListBuilder() { SetDefaults(); }

	void SetDefaults()
	{
		m_cylinderEdgeCount = 5;
		m_cylinderColor = Kaim::VisualColor::Magenta;
		m_cylinderLinesColor = Kaim::VisualColor::Magenta;

		m_velocityColor = Kaim::VisualColor::White;
	}

	void DisplayCylinder(Kaim::ScopedDisplayList* displayList, const Kaim::SpatializedCylinderBlob* spatializedCylinderBlob,
		const Kaim::SpatializedCylinderConfigBlob* cylinderConfigBlob, bool displayAsWireframe);

	void DisplayVelocity(Kaim::ScopedDisplayList* displayList, const Kaim::SpatializedCylinderBlob* spatializedCylinderBlob);
	void DisplayVelocity(ScopedDisplayList* displayList, const BotConfigBlob* botConfigBlob, 
		const SpatializedCylinderBlob* spatializedCylinderBlob, 
		const SpatializedCylinderConfigBlob* spatializedCylinderConfigBlob, 
		const FrontDirectionBlob* frontDirectionBlob, 
		bool displayWireframe);
	
	void DisplaySpatializationResult(Kaim::ScopedDisplayList* displayList,
		const Kaim::SpatializedCylinderBlob* spatializedCylinderBlob,
		const Kaim::SpatializedCylinderConfigBlob* cylinderConfigBlob,
		const Kaim::SpatializationResultBlob* spatializationResult,
		KyUInt32 databaseIndex);

public:
	KyUInt32 m_cylinderEdgeCount;
	Kaim::VisualColor m_cylinderColor;
	Kaim::VisualColor m_cylinderLinesColor;

	Kaim::VisualColor m_velocityColor;
};

} // namespace Kaim

#endif
