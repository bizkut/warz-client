#ifndef __SCENEBOX_H__
#define __SCENEBOX_H__

#include "r3dTreeNode.h"

#define SCENEBOX_MAXOBJECTS 25

struct TraverseTreeCallArguments
{
	r3dCamera Cam;
	struct draw_s* result;
	int* numObjects;
	float minObjectSize;
	float defDrawDistanceSq;
	int shadowPass;
};

class GameObject;
class SceneBox : public r3dTreeNode<SceneBox>
{
public:
	SceneBox();
	SceneBox(const r3dPoint3D& centerPos, const r3dVector& halfSize);
	virtual ~SceneBox();

	void Update();

	void Add( GameObject* obj, bool setVisible );
	void Remove(GameObject* obj);

	void Move(GameObject* obj);

	void PrepareForRender();

	R3D_FORCEINLINE void AppendNodeObjects( const r3dCamera& Cam, struct draw_s* result, int& numObjects, int isFullyInside, float defDrawDistanceSq, float minObjectSize, int shadowPass );

	void TraverseTree( int shadowPass, const r3dBoundBox& box, GameObject** objects, int& numObjects );
	static void TraverseTreeMT( void* Data, size_t ItemStart, size_t ItemCount, size_t ThreadIndex );
	void TraverseTree( int shadowPass, const r3dCamera& Cam, struct draw_s* result, int& numObjects, float minObjectSize, int isFullyInside = 0 );
	void DoTraverseTree( TraverseTreeCallArguments* args, int isFullyInside );

	void IsVisibleFrom();

	void TraverseAndPrepareForOcclusionQueries( const r3dCamera& Cam, int frameId );
	void DoOcclusionQueries( const r3dCamera& Cam );
	void TraverseDebug(const r3dCamera& Cam, int isFullyInside = 0);

	void MarkDirty();
	void onResetDevice();

	bool SetupBox();

	void AppendDebugBoxes();

	r3dPoint3D GetCenterPos() const { return m_CenterPos; }
	r3dVector  GetHalfSize() const { return m_HalfSize; }

	//temp, test
	int queryIndex;
	bool visible;
	int lastVisited;
	int lastRendered;

	float distance;
	
	int counter;

	int needQuery;

	int deleteCounter;

	int getLevel() const { return m_Level; }

	bool operator<(const SceneBox& r) const;

//private:
public: // test!!!
	void InternalAdd(GameObject* obj, int id, bool setVisible );
	void UpdateHeuristicScreenSpaceVisibilty();
	float CalculateScreenSpaceArea();

	r3dPoint3D m_CenterPos;
	r3dVector m_HalfSize;

	GameObject* m_Objects[SCENEBOX_MAXOBJECTS];
	uint32_t m_ObjectCount;

	bool m_bDirty;
	bool m_bEmpty;

	int m_ID;
	int m_Level;
};

struct SceneTraversalStats
{
	int NumTraversedNodes;
	int MaxTraversedNodes;

	SceneTraversalStats();
};

void UpdateSceneTraversalStats();
SceneTraversalStats GetSceneTraversalStats();

void InitOcclusionQuerySystem();
void CloseOcclusionQuerySystem();

void AdvanceQueryBalancer();
void SceneBoxOnUpdate();

#endif //__SCENEBOX_H__