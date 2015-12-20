//=========================================================================
//	Module: GenericQuadTree.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "JobChief.h"

#include "GenericQuadTree.h"

extern r3dCamera gCam;

//-------------------------------------------------------------------------
//	Quad tree node
//-------------------------------------------------------------------------

void QuadTreeNode::SaveToMemory(char *buf) const
{
	uint32_t size = QuadTreeNode::GetSerializableBlockSize();
	memcpy_s(buf, size, this, size);
}

//////////////////////////////////////////////////////////////////////////

void QuadTreeNode::LoadFromMemory(const char *buf)
{
	uint32_t size = QuadTreeNode::GetSerializableBlockSize();
	memcpy_s(this, size, buf, size);
}

//////////////////////////////////////////////////////////////////////////

uint32_t QuadTreeNode::GetSerializableBlockSize()
{
	QuadTreeNode *n = reinterpret_cast<QuadTreeNode*>(0);
	return sizeof(n->pivot) + sizeof(n->height) + sizeof(n->depth) + sizeof(n->children);
}

//-------------------------------------------------------------------------
//	Quad tree
//-------------------------------------------------------------------------

QuadTree::QuadTree(const r3dBoundBox &worldBB, GetObjectAABB getAABBFunc)
: gridSize(std::max(worldBB.Size.x, worldBB.Size.z))
, objAABB_Getter(getAABBFunc)
, visibilityInfoDirty(true)
{
	r3d_assert(objAABB_Getter);

	//	Make first node
	nodesPool.Resize(1);
	QuadTreeNode &n = nodesPool[0];
	n.depth = 0;
	n.pivot.x = worldBB.Org.x + gridSize * 0.5f;
	n.pivot.y = worldBB.Org.y;
	n.pivot.z = worldBB.Org.z + gridSize * 0.5f;
}

//////////////////////////////////////////////////////////////////////////

QuadTree::~QuadTree()
{

}

//////////////////////////////////////////////////////////////////////////

bool QuadTree::AddObject(QuadTreeObjectID objId)
{
	r3dBoundBox bb = objAABB_Getter(objId);
	r3dPoint3D center = bb.Center();
	
	r3dBoundBox worldBB(GetWorldAABB());

	int treeNodeDepth = CalcLevelDepth(bb);

	int nodeIdx = GetObjectNode(center, treeNodeDepth, 0, true);
	if (nodeIdx == -1)
		return false;

	QuadTreeNode &p = nodesPool[nodeIdx];

	p.objects.PushBack(objId);
	float minY = std::min(p.pivot.y, bb.Org.y);
	float maxY = std::max(p.pivot.y + p.height, bb.Org.y + bb.Size.y);

	p.height = maxY - minY;
	p.pivot.y = minY;

	PropagateHeight(nodeIdx);
	return true;
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::RemoveObject(QuadTreeObjectID objId)
{
	for (uint32_t i = 0; i < nodesPool.Count(); ++i)
	{
		QuadTreeNode &p = nodesPool[i];
		if (p.objects.Count() > 0)
		{
			QuadTreeObjectID *start = &p.objects.GetFirst();
			QuadTreeObjectID *end = start + p.objects.Count();

			QuadTreeObjectID *found = std::find(start, end, objId);
			if (found < end)
			{
				uint32_t foundIdx = std::distance(start, found);
				std::swap(p.objects[foundIdx], p.objects.GetLast());
				p.objects.PopBack();
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::AllocateChildren(int parentIdx)
{
	QuadTreeNode &p = nodesPool[parentIdx];
	//	Have children already
	if (p.children[0][0] >= 0)
		return;

	int newNodeIdx = nodesPool.Count();

	if( nodesPool.Count() + 4 > nodesPool.GetSpace() )
	{
		nodesPool.Reserve( nodesPool.GetSpace() * 2 );
	}

	nodesPool.Resize(nodesPool.Count() + 4);

	//	Get parent again, because pointer can change after array resize
	QuadTreeNode &parent = nodesPool[parentIdx];

	int newDepth = parent.depth + 1;
	float spacing = GetNodeSpacing(newDepth);

	int k = 0;
	for (int i = newNodeIdx; i < static_cast<int>(nodesPool.Count()); ++i, ++k)
	{
		int row = k / 2;
		int col = k % 2;

		r3d_assert(row < 2);

		QuadTreeNode &newNode = nodesPool[i];

		newNode.depth = newDepth;

		r3dPoint3D pt;
		pt.x = parent.pivot.x + spacing * (col - 0.5f);
		pt.z = parent.pivot.z + spacing * (row - 0.5f);
		pt.y = parent.pivot.y;

		newNode.pivot = pt;

		parent.children[row][col] = i;

		newNode.parent = parentIdx;
	}
}

//////////////////////////////////////////////////////////////////////////

int QuadTree::GetObjectNode(const r3dPoint3D &objCenter, int depth, int parentIdx, bool shouldAllocateNodes)
{
	r3d_assert(parentIdx >= 0);
	QuadTreeNode &p = nodesPool[parentIdx];
	//	Requested level was reached
	if (depth <= 0)
	{
		return parentIdx;
	}

	if (!shouldAllocateNodes && p.children[0][0] == -1)
	{
		return -1;
	}

	AllocateChildren(parentIdx);

	//	Yes, get it again, because of possible vector reallocation
	QuadTreeNode &parent = nodesPool[parentIdx];

	//	Search for nearest child node
	float s = GetNodeSpacing(parent.depth + 1);

	int col = static_cast<int>((objCenter.x - parent.pivot.x + s) / s);
	int row = static_cast<int>((objCenter.z - parent.pivot.z + s) / s);

	//	Object can be outside valid grid area
	if (row < 0 || row >= 2 || col < 0 || col >= 2)
		return -1;

// 	r3d_assert(row >= 0 && row < 2);
// 	r3d_assert(col >= 0 && col < 2);

	int newParentIdx = parent.children[row][col];

	return GetObjectNode(objCenter, depth - 1, newParentIdx, shouldAllocateNodes);
}

//////////////////////////////////////////////////////////////////////////

int QuadTree::CalcLevelDepth(const r3dBoundBox &bb) const
{
	//	Get bounding sphere radius
	float bbr = (bb.Size * 0.5f).Length();

	uint32_t l = static_cast<uint32_t>(gridSize / bbr);
	l = r3dLog2(l);
	int treeNodeDepth = static_cast<int>(l);

	return treeNodeDepth;
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::DebugVisualizeTree2D(const r3dPoint3D &scaleOffset) const
{
	const QuadTreeNode &n = nodesPool[0];
	float spacing = GetNodeSpacing(n.depth) / 2;
	r3dPoint2D p0 = r3dPoint2D(n.pivot.x - spacing, n.pivot.z - spacing) * scaleOffset.x + r3dPoint2D(scaleOffset.y, scaleOffset.z);
	r3dPoint2D p1 = r3dPoint2D(n.pivot.x + spacing, n.pivot.z - spacing) * scaleOffset.x + r3dPoint2D(scaleOffset.y, scaleOffset.z);
	r3dPoint2D p2 = r3dPoint2D(n.pivot.x + spacing, n.pivot.z + spacing) * scaleOffset.x + r3dPoint2D(scaleOffset.y, scaleOffset.z);
	r3dPoint2D p3 = r3dPoint2D(n.pivot.x - spacing, n.pivot.z + spacing) * scaleOffset.x + r3dPoint2D(scaleOffset.y, scaleOffset.z);
	p0.y = r3dRenderer->ScreenH - p0.y;
	p1.y = r3dRenderer->ScreenH - p1.y;
	p2.y = r3dRenderer->ScreenH - p2.y;
	p3.y = r3dRenderer->ScreenH - p3.y;
	r3dDrawBox2D(p1, p0, p3, p2, r3dColor::blue);

	DebugVisualizeNode2D(0, scaleOffset);

	//	Draw frustum planes
	p0 = r3dPoint2D(r3dRenderer->FrustumCorners[0].x, r3dRenderer->FrustumCorners[0].z) * scaleOffset.x + r3dPoint2D(scaleOffset.y, scaleOffset.z);
	p1 = r3dPoint2D(r3dRenderer->FrustumCorners[1].x, r3dRenderer->FrustumCorners[1].z) * scaleOffset.x + r3dPoint2D(scaleOffset.y, scaleOffset.z);
	p2 = r3dPoint2D(r3dRenderer->FrustumCorners[5].x, r3dRenderer->FrustumCorners[5].z) * scaleOffset.x + r3dPoint2D(scaleOffset.y, scaleOffset.z);
	p3 = r3dPoint2D(r3dRenderer->FrustumCorners[6].x, r3dRenderer->FrustumCorners[6].z) * scaleOffset.x + r3dPoint2D(scaleOffset.y, scaleOffset.z);
	p0.y = r3dRenderer->ScreenH - p0.y;
	p1.y = r3dRenderer->ScreenH - p1.y;
	p2.y = r3dRenderer->ScreenH - p2.y;
	p3.y = r3dRenderer->ScreenH - p3.y;
	r3dDrawLine2D(p0.x, p0.y, p1.x, p1.y, 2.0f, r3dColor::green);
	r3dDrawLine2D(p0.x, p0.y, p2.x, p2.y, 2.0f, r3dColor::green);
	r3dDrawLine2D(p1.x, p1.y, p3.x, p3.y, 2.0f, r3dColor::green);

	r3dRenderer->Flush();
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::DebugVisualizeTree3D(int parentIdx) const
{
	if (parentIdx < 0)
		return;

	const QuadTreeNode &n = nodesPool[parentIdx];
	float s = GetNodeSpacing(n.depth);

	r3dBoundBox bb;
	bb.Org = n.pivot;
	bb.Org.y += n.height / 2;
	bb.Size = r3dPoint3D(s, n.height, s);

	r3dDrawOrientedBoundBox(bb, r3dPoint3D(0, 0, 0), gCam, r3dColor::white);

	DebugVisualizeTree3D(n.children[0][0]);
	DebugVisualizeTree3D(n.children[0][1]);
	DebugVisualizeTree3D(n.children[1][0]);
	DebugVisualizeTree3D(n.children[1][1]);
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::DebugVisualizeNode2D(int parentIdx, const r3dPoint3D &scaleOffset) const
{
	const QuadTreeNode &n = nodesPool[parentIdx];

	r3dColor cl = r3dColor::white;
	const r3dColor colors[] = { r3dColor::grey, r3dColor::white, r3dColor::red };

	//	If visibility properly computed (at least memory allocated)?
	if (visibilityInfo.Count() == nodesPool.Count())
	{
		int vis = visibilityInfo[parentIdx];
		r3d_assert(vis < 3 && vis >= 0);
		cl = colors[vis];
	}

	float spacing = GetNodeSpacing(n.depth) / 2;

	r3dPoint2D p0 = r3dPoint2D(n.pivot.x - spacing, n.pivot.z - spacing) * scaleOffset.x + r3dPoint2D(scaleOffset.y, scaleOffset.z);
	r3dPoint2D p1 = r3dPoint2D(n.pivot.x - spacing, n.pivot.z) * scaleOffset.x + r3dPoint2D(scaleOffset.y, scaleOffset.z);
	r3dPoint2D p2 = r3dPoint2D(n.pivot.x, n.pivot.z - spacing) * scaleOffset.x + r3dPoint2D(scaleOffset.y, scaleOffset.z);
	r3dPoint2D p3 = r3dPoint2D(n.pivot.x, n.pivot.z) * scaleOffset.x + r3dPoint2D(scaleOffset.y, scaleOffset.z);
	float s = spacing * scaleOffset.x;

	p0.y = r3dRenderer->ScreenH - p0.y;
	p1.y = r3dRenderer->ScreenH - p1.y;
	p2.y = r3dRenderer->ScreenH - p2.y;
	p3.y = r3dRenderer->ScreenH - p3.y;

	//	Have children?
	if (n.children[0][0] != -1)
	{
		cl = colors[visibilityInfo[n.children[0][0]]];
		r3dDrawBox2DWireframe(p0.x, p0.y, s, -s, cl);
		cl = colors[visibilityInfo[n.children[0][1]]];
		r3dDrawBox2DWireframe(p1.x, p1.y, s, -s, cl);
		cl = colors[visibilityInfo[n.children[1][0]]];
		r3dDrawBox2DWireframe(p2.x, p2.y, s, -s, cl);
		cl = colors[visibilityInfo[n.children[1][1]]];
		r3dDrawBox2DWireframe(p3.x, p3.y, s, -s, cl);

		DebugVisualizeNode2D(n.children[0][0], scaleOffset);
		DebugVisualizeNode2D(n.children[0][1], scaleOffset);
		DebugVisualizeNode2D(n.children[1][0], scaleOffset);
		DebugVisualizeNode2D(n.children[1][1], scaleOffset);
	}

	//	Visualize object radii
/*
	for (uint32_t i = 0; i < n.objects.Count(); ++i)
	{
		ObjType *o = n.objects[i];
		r3dBoundBox bb = GetObjectBoundingBox(*o);
		float r = bb.Size.Length() / 2;

		r3dPoint3D c(bb.Center());
		r3dPoint2D center = r3dPoint2D(c.x, c.z) * scaleOffset.x + r3dPoint2D(scaleOffset.y, scaleOffset.z);
		center.y = r3dRenderer->ScreenH - center.y;

		r *= scaleOffset.x;

		r3dDrawCircle2D(center, r, r3dColor::green, r3dColor::green);
	}
*/
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::ComputeVisibility(r3dTL::TArray<QuadTreeObjectID> &visibleObjects)
{
	R3DPROFILE_FUNCTION( "QuadTree::ComputeVisibility" );

	if (visibilityInfo.Count() != nodesPool.Count())
		visibilityInfoDirty = true;

	visibilityInfo.Resize(nodesPool.Count());

	if( !r_multithreading->GetInt() || g_pJobChief->GetThreadCount() <= 1 )
	{
		ComputeVisibilityInternal(0);
	}
	else
	{
		int desiredFlattenedCount = g_pJobChief->GetThreadCount() * 4;

		flattenedNodes0.Reserve( desiredFlattenedCount * 4 );
		flattenedNodes1.Reserve( desiredFlattenedCount * 4 );

		flattenedNodes0.Clear();
		flattenedNodes0.PushBack( 0 );

		for( ; ; )
		{
			FlattenNodes();

			if( (int)flattenedNodes1.Count() >= desiredFlattenedCount )
				break;

			if( !flattenedNodes1.Count() )
				break;

			if( flattenedNodes1.Count() < flattenedNodes0.Count() )
				break;

			flattenedNodes0.Swap( flattenedNodes1 );
		}

		MTThis = this;

		if( flattenedNodes1.Count() )
		{
			g_pJobChief->Exec( ComputeVisibilityMT, &flattenedNodes1[ 0 ], flattenedNodes1.Count() );
		}
	}

	visibleObjects.Clear();

	for (uint32_t i = 0; i < nodesPool.Count(); ++i)
	{
		QuadTreeNode &n = nodesPool[i];
		if (visibilityInfo[i] != VI_OUTSIDE && n.objects.Count() > 0)
		{
			for (uint32_t k = 0; k < n.objects.Count(); ++k)
			{
				visibleObjects.PushBack(n.objects[k]);
			}
		}
	}

	//	Visibility info is back to normal
	visibilityInfoDirty = false;
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::ComputeVisibilityInternal(int nodeIdx)
{
	r3d_assert(nodeIdx >= 0);

	QuadTreeNode &n = nodesPool[nodeIdx];

	r3dBoundBox bb = GetNodeAABB(n);
	VisibilityInfoEnum visible = r3dRenderer->IsBoxInsideFrustum(bb);

	//	If fully invisible or fully visible, all children have same property too
	if (visible != VI_INTERSECTING)
	{
		PropagateVisibility(nodeIdx, visible);
	}
	//	Else do more checks
	else
	{
		visibilityInfo[nodeIdx] = visible;

		if (n.children[0][0] != -1)
		{
			ComputeVisibilityInternal(n.children[0][0]);
			ComputeVisibilityInternal(n.children[0][1]);
			ComputeVisibilityInternal(n.children[1][0]);
			ComputeVisibilityInternal(n.children[1][1]);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::ComputeVisibilityMT( void* Data, size_t ItemStart, size_t ItemCount, size_t ThreadIndex )
{
	(void)ThreadIndex;

	for( size_t i = ItemStart; i < ItemStart + ItemCount; i ++ )
	{
		MTThis->ComputeVisibilityInternal( ((int*)Data)[ i ] );
	}
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::FlattenNodes()
{
	flattenedNodes1.Clear();

	for( int i = 0, e = (int)flattenedNodes0.Count(); i < e; i ++ )
	{
		int nodeIndex = flattenedNodes0[ i ];
		QuadTreeNode& n = nodesPool[ nodeIndex ];

		r3dBoundBox bb = GetNodeAABB( n );
		VisibilityInfoEnum visible = r3dRenderer->IsBoxInsideFrustum(bb);

		visibilityInfo[ nodeIndex ] = visible;

		if (n.children[0][0] != -1)
		{
			flattenedNodes1.PushBack( n.children[ 0 ][ 0 ] );
			flattenedNodes1.PushBack( n.children[ 0 ][ 1 ] );
			flattenedNodes1.PushBack( n.children[ 1 ][ 0 ] );
			flattenedNodes1.PushBack( n.children[ 1 ][ 1 ] );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::PropagateVisibility(int rootIdx, VisibilityInfoEnum visible)
{
	QuadTreeNode &n = nodesPool[rootIdx];

	//	Do early exit if visibility was not changed
 	VisibilityInfoEnum prevVisInfo = visibilityInfo[rootIdx];
   	if (prevVisInfo == visible && !visibilityInfoDirty)
   		return;

	visibilityInfo[rootIdx] = visible;

	if (n.children[0][0] != -1)
	{
		PropagateVisibility(n.children[0][0], visible);
		PropagateVisibility(n.children[0][1], visible);
		PropagateVisibility(n.children[1][0], visible);
		PropagateVisibility(n.children[1][1], visible);
	}
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::PropagateHeight(int currNodeIdx)
{
	QuadTreeNode &n = nodesPool[currNodeIdx];

	float h = n.height;
	float y = n.pivot.y;
	//	Search for node parent

	if( n.parent < 0 )
		return;

	QuadTreeNode &parent = nodesPool[ n.parent ];

	//	Update height
	float minY = std::min(parent.pivot.y, y);
	float maxY = std::max(parent.pivot.y + parent.height, y + h);

	parent.height = maxY - minY;
	parent.pivot.y = minY;

	//	Propagate height uptree
	PropagateHeight( n.parent );
	
}

//////////////////////////////////////////////////////////////////////////

bool QuadTree::LoadFromFile(const char *fileName)
{
	r3dFile *f = r3d_open(fileName);
	if (!f)
	{
		return false;
	}

	const uint32_t oneCellSize = QuadTreeNode::GetSerializableBlockSize();
	const uint32_t numCells = f->size / oneCellSize;
	r3d_assert(f->size % oneCellSize == 0);
	nodesPool.Resize(numCells);

	char *buf = new char[f->size];
	fread(buf, f->size, 1, f);
	fclose(f);
	char *ptr = buf;

	for (uint32_t i = 0; i < nodesPool.Count(); ++i, ptr += oneCellSize)
	{
		nodesPool[i].LoadFromMemory(ptr);
	}

	delete [] buf;
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool QuadTree::SaveToFile(const char *fileName)
{
	RebuildTree();
	
	//	Allocate necessary memory block to save all objects
	FILE *f = fopen_for_write(fileName, "wb");

	const uint32_t oneCellSize = QuadTreeNode::GetSerializableBlockSize();
	const uint32_t numCells = nodesPool.Count();
	const uint32_t totalSize = numCells * oneCellSize;

	char *buf = game_new char[totalSize];
	char *ptr = buf;

	for (uint32_t i = 0; i < nodesPool.Count(); ++i, ptr += oneCellSize)
	{
		const QuadTreeNode &n = nodesPool[i];
		n.SaveToMemory(ptr);
	}

	fwrite(buf, totalSize, 1, f);
	delete [] buf;
	fclose(f);

	return true;
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::RebuildTree()
{
	QuadTreeNode::ObjectsArr oa;
	GatherObjects(0, oa);

	//	Clear all nodes except first
	nodesPool.Erase(1, nodesPool.Count() - 1);
	QuadTreeNode &n = nodesPool[0];

	n.children[0][0] =
	n.children[0][1] =
	n.children[1][0] =
	n.children[1][1] = -1;

	//	Repopulate objects
	for (uint32_t i = 0; i < oa.Count(); ++i)
	{
		AddObject(oa[i]);
	}

	OptimizeMemory();
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::GatherObjects(int parentIdx, QuadTreeNode::ObjectsArr &objArr)
{
	if (parentIdx != -1)
	{
		QuadTreeNode &n = nodesPool[parentIdx];
		for (uint32_t i = 0; i < n.objects.Count(); ++i)
		{
			objArr.PushBack(n.objects[i]);
		}

		GatherObjects(n.children[0][0], objArr);
		GatherObjects(n.children[0][1], objArr);
		GatherObjects(n.children[1][0], objArr);
		GatherObjects(n.children[1][1], objArr);
	}
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::GetObjectsInRect(const r3dBoundBox &bb, r3dTL::TArray<QuadTreeObjectID> &outObjects) const
{
	outObjects.Clear();
	GetObjectsInRectRecursive(bb, outObjects, 0);
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::GetObjectsInRectRecursive(const r3dBoundBox &bb, r3dTL::TArray<QuadTreeObjectID> &outObjects, int parentIdx) const
{
	if (parentIdx == -1)
		return;

	const QuadTreeNode &n = nodesPool[parentIdx];
	r3dBoundBox nodeBox = GetNodeAABB(n);

	if (!nodeBox.Intersect(bb))
		return;

	//	Check all children
	GetObjectsInRectRecursive(bb, outObjects, n.children[0][0]);
	GetObjectsInRectRecursive(bb, outObjects, n.children[0][1]);
	GetObjectsInRectRecursive(bb, outObjects, n.children[1][0]);
	GetObjectsInRectRecursive(bb, outObjects, n.children[1][1]);

	//	Check objects AABBs for intersections
	for (uint32_t i = 0; i < n.objects.Count(); ++i)
	{
		QuadTreeObjectID objId = n.objects[i];
		r3dBoundBox objBox = objAABB_Getter(objId);
		if (objBox.Intersect(bb))
			outObjects.PushBack(objId);
	}
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::GetObjectsHitByRay(const r3dPoint3D &org, const r3dPoint3D &dir, ObjectList &outObjects) const
{
	outObjects.Clear();
	float len = dir.Length();
	r3dPoint3D nrmDir = dir / len;
	GetObjectsHitByRayRecursive(org, nrmDir, len, outObjects, 0);
}

//------------------------------------------------------------------------

void QuadTree::OptimizeMemory()
{
	int freeCount = nodesPool.GetSpace() - nodesPool.Count();
	if( freeCount * sizeof( QuadTreeNode ) > 1024 * 1024 )
	{
		r3dOutToLog( "Optimizing QuadTree node pool: freeing %d objects (%.2f MB)\n", freeCount, freeCount * sizeof(QuadTreeNode) / 1024.f / 1024.f );
		nodesPool.AdjustReserve( nodesPool.Count() );
	}
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::GetObjectsHitByRayRecursive(const r3dPoint3D &org, const r3dPoint3D &nrmDir, float len, ObjectList &outObjects, int parentIdx) const
{
	if (parentIdx == -1)
		return;

	const QuadTreeNode &n = nodesPool[parentIdx];
	r3dBoundBox nodeBox = GetNodeAABB(n);

	float dist = 0;
	if (!nodeBox.ContainsRay(org, nrmDir, len, &dist))
		return;

	//	Check all children
	GetObjectsHitByRayRecursive(org, nrmDir, len, outObjects, n.children[0][0]);
	GetObjectsHitByRayRecursive(org, nrmDir, len, outObjects, n.children[0][1]);
	GetObjectsHitByRayRecursive(org, nrmDir, len, outObjects, n.children[1][0]);
	GetObjectsHitByRayRecursive(org, nrmDir, len, outObjects, n.children[1][1]);

	//	Check objects AABBs for intersections
	for (uint32_t i = 0; i < n.objects.Count(); ++i)
	{
		QuadTreeObjectID objId = n.objects[i];
		r3dBoundBox objBox = objAABB_Getter(objId);
		if (objBox.ContainsRay(org, nrmDir, len, &dist))
			outObjects.PushBack(objId);
	}
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::VisitAllQuadTree(VisitNodeFunction fn, void *userData, bool doTailRecursion) const
{
	if (!fn)
		return;

	RecursivelyVisitNodes(0, doTailRecursion, fn, userData);
}

//////////////////////////////////////////////////////////////////////////

void QuadTree::RecursivelyVisitNodes(int parentIdx, bool doTailRecursion, VisitNodeFunction fn, void *userData) const
{
	if (parentIdx != -1)
	{
		const QuadTreeNode &n = nodesPool[parentIdx];
		if (!doTailRecursion)
			fn(n, userData);

		RecursivelyVisitNodes(n.children[0][0], doTailRecursion, fn, userData);
		RecursivelyVisitNodes(n.children[0][1], doTailRecursion, fn, userData);
		RecursivelyVisitNodes(n.children[1][0], doTailRecursion, fn, userData);
		RecursivelyVisitNodes(n.children[1][1], doTailRecursion, fn, userData);

		if (doTailRecursion)
			fn(n, userData);
	}
}

//////////////////////////////////////////////////////////////////////////

QuadTree* QuadTree::MTThis;