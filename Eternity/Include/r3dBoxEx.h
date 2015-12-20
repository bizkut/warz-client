
class r3dBox3DEx : public r3dBox3D {

public:
	D3DXVECTOR3 m_vAxis[3];	// coordinate basis of bb (must be orthonormal)
	D3DXVECTOR3 m_vOrigin;	// position of bb's center point

#define HalfSize(bb,i)  ( ( ((i) == 0) ? (bb).Size.X : ( ((i) == 1) ? (bb).Size.Y : (bb).Size.Z) )* 0.5f)

public:
	r3dBox3DEx(r3dVector* vAxis, r3dVector* vOrigin, r3dPoint3D* pSize) {
		for (int i = 0; i < 3; i++) {
			m_vAxis[i] = D3DXVECTOR3(vAxis[i].x, vAxis[i].y, vAxis[i].z);
		}
		m_vOrigin = D3DXVECTOR3(vOrigin->x, vOrigin->y, vOrigin->z);
		Size = *pSize;
	}

	r3dBox3DEx(r3dVector* vAxis, r3dBox3D& box) {
		for (int i = 0; i < 3; i++) {
			m_vAxis[i] = D3DXVECTOR3(vAxis[i].x, vAxis[i].y, vAxis[i].z);
		}
		m_vOrigin = D3DXVECTOR3(box.Org.x, box.Org.y, box.Org.z);
		Size = 	box.Size;
	}

	r3dBox3DEx(r3dBox3D& box) {
		Create(box);
	}

	r3dBox3DEx() {

		Size = r3dPoint3D( 0, 0, 0 );
		m_vOrigin = D3DXVECTOR3(0, 0, 0);
		
		m_vAxis[0] = D3DXVECTOR3(1, 0, 0);
		m_vAxis[1] = D3DXVECTOR3(0, 1, 0);
		m_vAxis[2] = D3DXVECTOR3(0, 0, 1);
	}

	void Create(r3dBox3D& box) {
		
		m_vAxis[0] = D3DXVECTOR3(1, 0, 0);
		m_vAxis[1] = D3DXVECTOR3(0, 1, 0);
		m_vAxis[2] = D3DXVECTOR3(0, 0, 1);

		Size = 	box.Size;
		SetOrigin( box.Org );
		//Org = box.Org;
	}

	void SetOrigin(const r3dPoint3D& vOrg) {
		m_vOrigin = D3DXVECTOR3(vOrg.x + Size.X/2, vOrg.y + Size.Y/2, vOrg.z + Size.Z/2);
	}

	// Calculate bb support along given axis
	float FindSupport(const r3dBox3DEx& bb, const D3DXVECTOR3& vAxis)
	{
		float fSupport = 0;

		for (int i = 0; i<3; i++)
			fSupport += fabsf(D3DXVec3Dot(&bb.m_vAxis[i], &vAxis) * HalfSize(bb, i) );

		return fSupport;
	}
	
	// Tests two OBB for collision
	// (very straightforward implementation using Separating Axis Theorem)
	bool Collision(const r3dBox3DEx& bb2)
	{
		int i;

		// Separating axis theorem states that two polytopes
		// are disjoint iff there exists a separating
		// plane parallel to a face of either polytope or parallel to
		// an edge from each polytope. A consequence of this is that
		// two polytopes are disjoint iff there exists a separating
		// axis orthogonal to a face of either polytope or orthogonal
		// to an edge from each polytope. In case of OBB there
		// can be 15 candidates for a separating axis (among 6 edges
		// of the two polytopes and 9 their pairwise crossproducts)

		// Calculate distance between centers

		D3DXVECTOR3 vT = bb2.m_vOrigin - m_vOrigin;

		// Try each of the edges of bb1 and bb2

		for (i = 0; i<3; i++)
		{
			D3DXVECTOR3 vAxis1 = m_vAxis[i];
			float fAxis1Dist = fabsf(D3DXVec3Dot(&vT, &vAxis1));
			if (fAxis1Dist > HalfSize(*this, i) + FindSupport(bb2, vAxis1)) return false; // OBBs are disjoint
	
			D3DXVECTOR3 vAxis2 = bb2.m_vAxis[i];
			float fAxis2Dist = fabsf(D3DXVec3Dot(&vT, &vAxis2));
			if (fAxis2Dist > FindSupport(*this, vAxis2) + HalfSize(bb2, i)) return false; // OBBs are disjoint
		}

		// Try cross products
		// NOTE: this can be optimized pretty much

		for (i = 0; i<3; i++)
		{
			for (int j = 0; j<3; j++)
			{
				D3DXVECTOR3 vAxis;
				D3DXVec3Cross(&vAxis, &m_vAxis[i], &bb2.m_vAxis[j]);
				float fAxisDist = fabsf(D3DXVec3Dot(&vT, &vAxis));
				if (fAxisDist > FindSupport(*this, vAxis) + FindSupport(bb2, vAxis)) return false; // OBBs are disjoint
			}
		}
	
		return true; // no separating axis found, collision detected
	}

	void Rotate(const D3DXMATRIX *mXForm)
	{
		for (int i = 0; i<3; i++)
		{
			D3DXVec3TransformNormal(&m_vAxis[i], &m_vAxis[i], mXForm);
		}
	}
	
	void Rotate(r3dBox3DEx *pOut, const D3DXMATRIX *mXForm)
	{
		pOut->m_vOrigin = m_vOrigin;
		pOut->Size = Size;

		for (int i = 0; i<3; i++)
		{
			D3DXVec3TransformNormal(&pOut->m_vAxis[i], &m_vAxis[i], mXForm);
		}
	}
};

