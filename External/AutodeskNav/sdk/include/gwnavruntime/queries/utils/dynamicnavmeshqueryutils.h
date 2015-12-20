/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: LASI - secondary contact: NOBODY
#ifndef Navigation_TagVolumeQueryUtils_H
#define Navigation_TagVolumeQueryUtils_H

#include "gwnavruntime/kernel/SF_Types.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/math/vec2i.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/dynamicnavmesh/dynamicnavmeshquerytypes.h"
#include "gwnavruntime/containers/kyarray.h"


namespace Kaim
{

class NavFloorBlob;
class DatabaseGenMetrics;
class ScopedDisplayList;
class MergedPolygonWithHoles;
class EarClippingTriangulatorInputPolygonWithHoles;
class DynamicTriangulation;
class TriangulatorOutput;
class VisualColor;
class WorkingMemory;

class DynamicNavMeshUtils
{
public:
	static Vec2i GetSnappedNavVertexPixelPos(const DatabaseGenMetrics& genMetrics, const Vec2i& cellOriginPixe, const Vec2f& pos2f);

	// RENDER
	static void PolylineToDisplayList(KyFloat32 integerPrecision, Vec2i* polyline, KyUInt32 polylinePointCount, ScopedDisplayList& displayList, const VisualColor& color, KyFloat32 zOffset);

	static KyResult ConvertTriangulatorInput(WorkingMemory* workingMemory, const MergedPolygonWithHoles& input, DynamicTriangulation& dynamicEarTriangulation, EarClippingTriangulatorInputPolygonWithHoles& convertedInput);

	static void RenderTriangulatorOutputToDisplayList(const TriangulatorOutput& output, ScopedDisplayList& displayList, const Kaim::VisualColor& color, KyFloat32 rasterPrecision, const Vec2i& offset);

	static void TriangulatorOuputFromDynamicTriangulation(
		const EarClippingTriangulatorInputPolygonWithHoles& earPolygon,
		const DynamicTriangulation& dynamicTriangulation,
		TriangulatorOutput& triangulatordynamicTriangulationOutput);
};

}

#endif //Navigation_TagVolumeQueryUtils_H

