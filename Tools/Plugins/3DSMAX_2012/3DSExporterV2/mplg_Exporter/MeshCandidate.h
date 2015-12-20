#ifndef MESH__H_43C83FE2DDA
#define	MESH__H_43C83FE2DDA

#include "VertexCandidate.h"
#include "MaxMaterial.h"

class CSubmesh;
class CMaxMesh;
class CSkeleton;


class CMesh
{
// member variables
public:
	CMaxMesh	*m_pMesh;

	class SubMaterial
	{
	  public:
	  CMaxMaterial	*pMyMtl;
	  int		iFaceCount;
	  std::vector<CVertex*>	vVertices;
	  std::vector<int>	vIndices;	// numfaces * 3
	  
	  // filled after ::Finalize() - show position of that submaterial within mesh array
	  int		iVertexStart, iVertexEnd;
	  int		iIndexStart, iIndexEnd;
	};
	std::vector<SubMaterial*> m_SubMaterials;

	std::vector<CVertex*>	m_vVertices;	// pointer to created vertices. numFaces * 3
	std::vector<int>	m_vIndices;	// numfaces * 3

	int		m_iVertexCount;
	int		m_iFaceCount;

	int		m_exportWeights;

// constructors/destructor
 public:
	CMesh();
	virtual ~CMesh();

	int		CreateFromSingleMesh(INode* pNode, int maxBoneCount, float weightThreshold, TimeValue time)
	{
	  Clear();
	  AddMesh(pNode, time);
	  Finalize(maxBoneCount, weightThreshold);
	  return 1;
	}

	void		Clear();
	int		AddMesh(INode *pNode, TimeValue time);
	int		Finalize(int maxBoneCount, float weightThreshold);
	void		 Finalize_SubMat(SubMaterial* sbm);
	void		  SubMat_CalcTangentSpace(SubMaterial* sbm);
	void		  SubMat_RemoveDups(SubMaterial* sbm);
	void		  SubMat_Optimize(SubMaterial* sbm);
	void		  SubMat_AddToMesh(const SubMaterial* sbm);
};

#endif	//MESH__H_43C83FE2DDA
