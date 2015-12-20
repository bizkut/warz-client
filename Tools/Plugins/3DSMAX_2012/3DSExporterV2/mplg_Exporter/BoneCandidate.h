#ifndef BONE__H_3DEF8F55BBDF
#define BONE__H_3DEF8F55BBDF

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//
class CBone
{
// member variables
public:
	INode		*m_pNode;

	int		m_id;
	int		m_parentId;
	std::list<int>	m_listChildId;
	BOOL		m_bSelected;

// constructors/destructor
public:
	CBone();
	virtual ~CBone();
	void		Create(int id, int parentId, INode *pNode);

	void		AddChildId(int childId);
	BOOL		HasChild(); // does it have at least one child
	int		GetId();
	float		GetLength();
	std::list<int>	GetListChildId();
	INode*		GetNode();
 	
	const char*	GetName();

	int		GetParentId();
	int		GetChildId();
	BOOL		IsSelected();
	void		SetSelected(BOOL bSelected);
};

#endif	//BONE__H_3DEF8F55BBDF