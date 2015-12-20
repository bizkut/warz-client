#ifndef VERTEX__H_B67E6CF11194
#define VERTEX__H_B67E6CF11194

class CMaxMaterial;

class CVertex
{
// misc
public:
	static float	POS_EPSILON;       // epsilon for position compare
	static float	NRM_ANGLE_EPSILON; // epsilon for normal compare

	Point3		m_position;
	Point3		m_normal;
	Point3		m_tangent;
	float		m_w_tangent;	// .w component of tangent (handiness - mirror_flag)
	Point3		m_bitangent;
	Point3		m_color;

	// index of the vertex
	int		m_iVertexId;

	typedef struct
	{
	  int		boneId;
	  float		weight;
	} Influence;
	std::vector<Influence> m_vectorInfluence;

	typedef struct
	{
	  float		u, v;
	} TexCoord;
	TexCoord	m_tu[4];
	int		m_iMapChannels;

// constructors/destructor
public:
	CVertex();
	virtual ~CVertex();

	// apply_epsilon here is false when doing arrangement sort (position/normal/etc), true when doing actual vertices comparing
	int		SortOperand(const CVertex& rhs, bool apply_epsilon) const;
	
	void		AddInfluence(INode *pNode, float weight);
	void		AdjustBoneInfluences(int maxBoneCount, float weightThreshold);
	void		SetTextureCoordinate(int ch, float u, float v);

protected:
	static BOOL	CompareInfluenceWeight(const Influence& influence1, const Influence& influence2);
};

#endif	//B67E6CF11194