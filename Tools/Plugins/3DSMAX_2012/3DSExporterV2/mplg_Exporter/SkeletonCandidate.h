#ifndef SKELETON__H
#define SKELETON__H

//----------------------------------------------------------------------------//
// Forward declarations                                                       //
//----------------------------------------------------------------------------//

class CBone;

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class CSkeleton
{
// member variables
public:
	std::vector<CBone *>	m_vBones;
	std::vector<int>	m_vRootBonesID;
	
	IBipMaster	*pBipMaster;
	IBipedExport	*pBipExport;
	
	DWORD		dwSkeletonID;

protected:
	BOOL		AddNode(INode *pNode, int parentId);

// constructors/destructor
public:
	CSkeleton();
	virtual ~CSkeleton();
	BOOL		CreateFromInterface();
	void		Clear();

	CBone*		CreateBone(INode* pNode, int parentId);
	int		FindBoneByNode(INode *pNode);
	CBone		*GetBone(int boneId);
	int		GetSelectedCount();
	Matrix3		GetBoneRTM(int boneId, TimeValue time);
	void		AdjustAnimationRange();

	std::vector<CBone *>	GetBoneVector()		{ return m_vBones;	 }
	std::vector<int>	GetListRootBoneId()	{ return m_vRootBonesID; }
};

extern	CSkeleton	theSkeleton;

#endif

//----------------------------------------------------------------------------//
