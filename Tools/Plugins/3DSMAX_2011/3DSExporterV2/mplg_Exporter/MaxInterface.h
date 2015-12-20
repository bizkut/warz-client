#ifndef MAX_INTERFACE_H_E7E05A8C733F
#define MAX_INTERFACE_H_E7E05A8C733F

// this is the class for all biped controllers except the root and the footsteps
#define BIPSLAVE_CONTROL_CLASS_ID	Class_ID(0x9154,0)
// this is the class for the center of mass, biped root controller ("Bip01")
#define BIPBODY_CONTROL_CLASS_ID	Class_ID(0x9156,0) 
// MultiRes modifier
#define MULTIRES_CLASS_ID		Class_ID(0x6a9e4c6b, 0x494744dd)
#define MULTIRES_PBID_GENERATE		10
#define MULTIRES_PBID_VERTEXCOUNT	1
#define MULTIRES_PBID_VERTEXPERCENT	2
// Toon Texture ClassID
#define	TOON_TEXTURE_CLASS_ID		Class_ID(0x1a8169a, 0x4d3960a5)


//----------------------------------------------------------------------------//
// Forward declarations                                                       //
//----------------------------------------------------------------------------//
class CMaxMesh;

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//
class CMaxInterface
{
// constructors/destructor
public:
	CMaxInterface();
	virtual ~CMaxInterface();

	BOOL		GetCancel();
	CWnd*		GetMainWnd();
	TimeValue	GetMaxTime();

	CMaxMesh*	GetMesh(INode *pNode, TimeValue time);

	Matrix3		GetNodeRTM(INode *pNode, INode *pParentNode, TimeValue time);
	Matrix3		GetNodeTM(INode *pNode, TimeValue time);		// return node matrix (scaling removed)
	Matrix3		GetObjectTM(INode *pNode, TimeValue time);		// return rotation matrix for object (scaling removed!)

	std::string	m_progressText;
	void		SetProgressInfo(int percentage);
	void		StartProgressInfo(const char * strText);
	void		StopProgressInfo();

	Modifier	*FindModifier(INode *pNode, Class_ID &ClassId);

	BOOL		IsBipedBone(INode *pNode);
	BOOL		IsBone(INode *pNode);
	BOOL		IsDummy(INode *pNode);
	BOOL		IsMesh(INode *pNode);
	enum NodeType
	{
	  TYPE_OTHER = 0,
	  TYPE_BONE,
	  TYPE_DUMMY
	};
	NodeType	GetNodeType(INode *pNode);
};

extern	BOOL	_MMRES_Present(INode *pNode);
extern	BOOL	_MMRES_Attach(INode *pNode);
extern	BOOL	_MMRES_SetLOD(INode *pNode, float perc);
extern	BOOL	_MMRES_Remove(INode *pNode);

#endif	//MAX_INTERFACE_H_E7E05A8C733F
