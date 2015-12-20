//=========================================================================
//	Module: GenericQuadTree.h
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#pragma once

#include <cmath>

//////////////////////////////////////////////////////////////////////////

typedef int QuadTreeObjectID;

/**	Get AABB of object function pointer type. This is only data needed to make quad tree fully functional. */
typedef r3dBoundBox (*GetObjectAABB)(QuadTreeObjectID objId);

//////////////////////////////////////////////////////////////////////////

struct QuadTreeNode
{
	/**	x and z are 2D coordinates of cell center, y - bottom point of cell. */
	r3dPoint3D pivot;

	/**	Cell height. */
	float height;

	/**	Node depth. */
	int depth;

	/**	Children nodes. 0,0 - bottom left, 0,1 - bottom right, 1,0 - top left, 1,1 - top right. */
	int children[2][2];

	int parent;

	/**	Node objects array. */
	typedef r3dTL::TArray<QuadTreeObjectID> ObjectsArr;
	ObjectsArr objects;

	QuadTreeNode()
	: pivot(0, 0, 0)
	, height(0)
	, depth(-1)
	{
		children[0][0] = 
		children[1][0] = 
		children[0][1] = 
		children[1][1] = -1;

		parent = -1;
	}

	void SaveToMemory(char *buf) const;
	void LoadFromMemory(const char *buf);
	static uint32_t GetSerializableBlockSize();
};

//////////////////////////////////////////////////////////////////////////

class QuadTree
{
public:
	/**	Object list type, that os returned by query functions. */
	typedef r3dTL::TArray<QuadTreeObjectID> ObjectList;

	/**	Node visitor function. It is used for recursive node visiting gives as parameter node reference. */
	typedef void (*VisitNodeFunction)(const QuadTreeNode &node, void *userData);

private:
	/**	Pool of nodes. First node is root node. */
	r3dTL::TArray<QuadTreeNode> nodesPool;

	r3dTL::TArray<int> flattenedNodes0;
	r3dTL::TArray<int> flattenedNodes1;

	/**	Visibility info of nodes. */
	r3dTL::TArray<VisibilityInfoEnum> visibilityInfo;

	/**	Is visibility info not valid from previous frame. */
	bool visibilityInfoDirty;

	/**	Grid total size (in world units). I.e. size of root node. */
	float gridSize;

	/**	Get AABB function pointer. */
	GetObjectAABB objAABB_Getter;

	/** pointer to current quad tree which is processed with multithreading */
	static QuadTree* MTThis;

	/**	Get node index that should own given object. */
	int GetObjectNode(const r3dPoint3D &objCenter, int depth, int parentIdx, bool shouldAllocateNodes);

	/**	Allocate children for given leaf node. If node already have children, do nothing. */
	void AllocateChildren(int parentIdx);

	/**	Get node spacing for appropriate quad tree level. */
	R3D_FORCEINLINE float GetNodeSpacing(int level) const
	{
		return gridSize / float( 1 << level );
	}

	/**	Get node size for appropriate quad tree level. This value is not equal to node spacing because loose factor. */
	R3D_FORCEINLINE float GetNodeSize(int level) const
	{
		const float looseFactor = 2.0f;
		return GetNodeSpacing(level) * looseFactor;
	}

	/**	Get node bounding box. */
	R3D_FORCEINLINE r3dBoundBox QuadTree::GetNodeAABB(const QuadTreeNode &n) const
	{
		float size = GetNodeSize(n.depth);

		r3dBoundBox bb;
		bb.Org = n.pivot - r3dPoint3D(size / 2, 0, size / 2);
		bb.Size = r3dPoint3D(size, n.height, size);
		return bb;
	}


	/**	Recursive debug visualizer tree node routine. */
	void DebugVisualizeNode2D(int parentIdx, const r3dPoint3D &scaleOffset) const;

	/**	Recursive function for node visibility computation. */
	void ComputeVisibilityInternal(int nodeIdx);

	/**	Helper function for multithreading visibility computation. */
	static void ComputeVisibilityMT( void* Data, size_t ItemStart, size_t ItemCount, size_t ThreadIndex );

	/**	flatten children of nodes from the tree and into the array, while computing visibility
	    Flattened array is then used to further calculate visibility using multithreading*/
	void FlattenNodes();

	/**	Propagate visibility to child nodes. */
	void PropagateVisibility(int rootIdx, VisibilityInfoEnum visible);

	/**	Propagate node height up-tree. */
	void PropagateHeight(int currNodeIdx);

	/**	Internal recursive function for gathering objects into array. */
	void GatherObjects(int parentIdx, QuadTreeNode::ObjectsArr &objArr);

	/**	Calculate node depth by given object id. */
	int CalcLevelDepth(const r3dBoundBox &bb) const;

	/**	Recursive objects that in rectangle adder. */
	void GetObjectsInRectRecursive(const r3dBoundBox &bb, ObjectList &outObjects, int parentIdx) const;

	/**	Recursive objects that hit by ray adder. */
	void GetObjectsHitByRayRecursive(const r3dPoint3D &org, const r3dPoint3D &nrmDir, float len, ObjectList &outObjects, int parentIdx) const;

	/**	Recursive node visitor helper. */
	void RecursivelyVisitNodes(int parentIdx, bool doTailRecursion, VisitNodeFunction fn, void *userData) const;

public:

	/**	From */
	QuadTree(const r3dBoundBox &worldBB, GetObjectAABB getAABBFunc);
	~QuadTree();

	/**
	* Add one object and expand tree if needed.
	* @return true if addition was successful.
	*/
	bool AddObject(QuadTreeObjectID objId);

	/**	Remove object from quad tree. Note that tree structure is not recalculated automatically here. Call RebuildTree to make this operation. */
	void RemoveObject(QuadTreeObjectID objId);

	/**	Visualize quadtree as 2D picture. */
	void DebugVisualizeTree2D(const r3dPoint3D &scaleOffset = r3dPoint3D(2.0f, 400, 100)) const;

	/**	Visualize quadtree as 3D picture. */
	void DebugVisualizeTree3D(int parentIdx = 0) const;

	/**	Compute visibility for quadtree. Return visible object list */
	void ComputeVisibility(r3dTL::TArray<QuadTreeObjectID> &visibleObjects);

	/**	Save entire quadtree into file. Contained objects is not saved. */
	bool SaveToFile(const char *fileName);

	/**	Load entire quadtree from file. Note that */
	bool LoadFromFile(const char *fileName);

	/**	Rebuild tree. This operation is needed for removing unneeded empty cells which may appear after objects creation/destruction. */
	void RebuildTree();

	/**	Get all objects that reside in given bounding box. */
	void GetObjectsInRect(const r3dBoundBox &bb, ObjectList &outObjects) const;

	/**	Get all objects which AABB is hit by ray. */
	void GetObjectsHitByRay(const r3dPoint3D &org, const r3dPoint3D &dir, ObjectList &outObjects) const;

	/**	Get bounding box of top level node. */
	r3dBoundBox GetWorldAABB() const { return GetNodeAABB(nodesPool[0]); }

	/**	Visit all nodes recursively using user defined function. */
	void VisitAllQuadTree(VisitNodeFunction fn, void *userData, bool doTailRecursion) const;

	void OptimizeMemory();
};
