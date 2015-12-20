#ifndef MAX_MESH_H
#define MAX_MESH_H

class CMaxMaterial;
class CMaxMesh
{
// member variables
public:
	INode		*m_pNode;
	TimeValue	m_time;

	TriObject	*m_pTriObject;
	Mesh		*m_pIMesh;
	BOOL		m_bDelete;

	std::vector<Mtl *> m_vectorMtl;

	Modifier*	m_pPhysiqueModifier;
	Modifier*	m_pSkinModifier;
	bool		IsHaveSkin() const {
	  return (m_pPhysiqueModifier != NULL) || (m_pSkinModifier != NULL);
	}

	Matrix3		m_ObjTM;
	Matrix3		m_NodeTM;
	
	int		m_iFaceOrder[3];

// constructors/destructor
public:
	CMaxMesh();
	virtual ~CMaxMesh();

	void		Create(INode *pNode, TriObject *pTriObject, BOOL bDelete, TimeValue time);
	void		  FillMaterials(Mtl *pMtl);

	int		GetFaceMaterialId(int faceId);
	CMaxMaterial*	GetFaceMaxMaterial(int faceId);

	Mtl*		GetSubmeshMtl(int submeshId);
	CVertex*	GetVertex(int faceId, int faceVertexId);
	void		FillVertexWeight(CVertex *pVertex, int vertexId);

	Point3		GetVertexNormal(int faceId, int vertexId, int cornerId);
	Matrix3		Transpose(Matrix3& matrix);
};

#endif

//----------------------------------------------------------------------------//
