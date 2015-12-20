/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_PolygonBuilder_H
#define Navigation_PolygonBuilder_H

#include "gwnavruntime/kernel/SF_Alg.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/dynamicnavmesh/dynamicnavmeshquerytypes.h"
#include "gwnavruntime/containers/bitfield.h"
#include "gwnavruntime/math/geometryfunctions.h"
#include "gwnavruntime/querysystem/workingmemcontainers/workingmemarray.h"

namespace Kaim
{

class ScopedDisplayList;
class WorkingMemory;
class DynamicNavMeshQuery;

class PolygonBuilder
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public: 
	PolygonBuilder(DynamicNavMeshQuery* query): m_query(query) {}

	KyResult Init(WorkingMemory* workingMemory);
	enum BuildStatus { BuildDone, BuildInPRocess };
	KyResult Build(WorkingMemory* workingMemory, KyArray<MergedPolygonWithHoles>& result, BuildStatus& buildStatus);

public: 
	void RenderInput(WorkingMemory* workingMemory, KyFloat32 rasterPrecision, const Vec2i& cellOffset, ScopedDisplayList& displayList);
	void RenderOutput(WorkingMemory* workingMemory, const KyArray<MergedPolygonWithHoles>& output, KyFloat32 rasterPrecision, const Vec2i& cellOffset, ScopedDisplayList& displayList);

private: 
	KyResult ExtractRawPolygons(WorkingMemory* workingMemory, BuildStatus& buildStatus);
	KyResult ComputePolygonWinding(WorkingMemory* workingMemory);
	KyResult BindHoles(WorkingMemory* workingMemory);
	KyResult BuildOutput(WorkingMemory* workingMemory, KyArray<MergedPolygonWithHoles>& result);

private: 
	DynamicNavMeshQuery* m_query;
};

} // namespace Kaim

#endif // Navigation_PolygonBuilder_H
