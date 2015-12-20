#pragma once

//  Bounding box for shadow
struct SBoundingBox 
{
	D3DXVECTOR3 vMin;
	D3DXVECTOR3 vMax;

	inline void Centroid(D3DXVECTOR3* p) const;
	inline void HalfDiag(D3DXVECTOR3* p) const;
	void Transform(SBoundingBox& box, const D3DXMATRIX* pMat) const;
};

// Return center position
void SBoundingBox::Centroid(D3DXVECTOR3* p) const
{
	D3DXVec3Add( p, &vMin, &vMax );
	D3DXVec3Scale( p, p, 0.5f );
}

// Return Half diagonal vector
void SBoundingBox::HalfDiag(D3DXVECTOR3* p) const
{
	D3DXVec3Subtract( p, &vMax, &vMin );
	D3DXVec3Scale( p, p, 0.5f );
}

//      Parameters for scattering in pixel shader 
// 
//   Scattering equation is 
//      L(s,theta) = L0 * Fex(s) + Lin(s,theta).
//   where,
//      Fex(s) = exp( -s * (Br+Bm) )
//      Lin(s,theta) = (Esun * ((Br(theta)+Bm(theta))/(Br+Bm)) + ambient)* (1.0f - exp( -s * (Br+Bm) ))
//      Br(theta) = 3/(16*PI) * Br * (1+cos^2(theta))
//      Bm(theta) = 1/(4*PI) * Bm * ((1-g)^2/(1+g^2-2*g*cos(theta))^(3/2))
// 
//   Distance light goes through the atomosphere in a certain ray is 
//      Distance(phi) = -R*sin(phi) + sqrt( (R*sin(phi))^2 + h * (2*R+h) )
//   where,
//      R   : Earth radius
//      h   : atomosphere height
//      phi : angle between a ray vector and a horizontal plane.

struct SScatteringShaderParameters 
{
	D3DXVECTOR4 vRayleigh;   // rgb : 3/(16*PI) * Br           w : -2*g
	D3DXVECTOR4 vMie;        // rgb : 1/(4*PI) * Bm * (1-g)^2  w : (1+g^2)
	D3DXVECTOR4 vESun;       // rgb : Esun/(Br+Bm)             w : R
	D3DXVECTOR4 vSum;        // rgb : (Br+Bm)                  w : h(2R+h)
	D3DXVECTOR4 vAmbient;    // rgb : ambient
};


//  scene parameters
struct SSceneParamter 
{
	// Camera
	float farPlane;
	D3DXVECTOR3 eyePt;
	D3DXMATRIX viewProj;

	float m_fWindVel;
	float m_fUVScale;

	float m_fCoverage;

	int useEngineColors;
	int isActive;

	char textureFileName[256];
	char textureCoverFileName[256];	

	//
	D3DXVECTOR4 m_vFade_Density;

	// light
	D3DXVECTOR3 m_vLightDir;    // sun light direction
	D3DXVECTOR3 m_vLightColor;  // sun light color
	D3DXVECTOR3 m_vAmbientLight;// ambient light

	// scattering
	D3DXVECTOR3 m_vRayleigh;     // rayleigh scattering
	D3DXVECTOR3 m_vMie;          // mie scattering
	float       m_fG;            // eccentricity of mie scattering 
	float       m_fLightScale;   // scaling parameter of light
	float       m_fAmbientScale; // scaline parameter of ambient term

	// distance for scattering
	float       m_fEarthRadius;  // radius of the earth
	float       m_fAtomosHeight; // height of the atomosphere
	float       m_fCloudHeight;  // height of cloud 

	void GetShaderParam(SScatteringShaderParameters& param) const;
	void SetTime(float fTime);

	inline void GetCloudDistance(D3DXVECTOR2& v) const;

	bool isEqual(const SSceneParamter& sp) const;
};


// Return parameters for computing distance to the cloud 
void SSceneParamter::GetCloudDistance(D3DXVECTOR2& v) const
{
	v.x = m_fEarthRadius;
	v.y = m_fCloudHeight * (2.0f*m_fEarthRadius + m_fCloudHeight);
}
