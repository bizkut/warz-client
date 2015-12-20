#include "r3dPCH.h"
#include "SceneParameter.h"


//  Compute bounding box in the indicated coordinate.
//  pMat must not be a projection matrix.
void SBoundingBox::Transform(SBoundingBox& box, const D3DXMATRIX* pMat) const
{
	D3DXVECTOR3 vCenter;
	D3DXVECTOR3 vDiagonal;
	Centroid( &vCenter );
	HalfDiag( &vDiagonal );
	D3DXVECTOR3 vAxisX( vDiagonal.x, 0.0f, 0.0f );
	D3DXVECTOR3 vAxisY( 0.0f, vDiagonal.y, 0.0f );
	D3DXVECTOR3 vAxisZ( 0.0f, 0.0f, vDiagonal.z );

	// Transform the center position and the axis vectors.
	D3DXVec3TransformCoord( &vCenter, &vCenter, pMat );
	D3DXVec3TransformNormal( &vAxisX, &vAxisX, pMat );
	D3DXVec3TransformNormal( &vAxisY, &vAxisY, pMat );
	D3DXVec3TransformNormal( &vAxisZ, &vAxisZ, pMat );

	vDiagonal = D3DXVECTOR3(
		fabsf( vAxisX.x ) + fabsf( vAxisY.x ) + fabsf( vAxisZ.x ),
		fabsf( vAxisX.y ) + fabsf( vAxisY.y ) + fabsf( vAxisZ.y ),
		fabsf( vAxisX.z ) + fabsf( vAxisY.z ) + fabsf( vAxisZ.z ) );
	D3DXVec3Add( &box.vMax, &vCenter, &vDiagonal );
	D3DXVec3Subtract( &box.vMin, &vCenter, &vDiagonal );
}

//  Get scatterign parameters for pixel shader 
void SSceneParamter::GetShaderParam(SScatteringShaderParameters& param) const
{
	static const float PI = 3.14159265f;

	// vRayleigh : rgb : 3/(16*PI) * Br           w : -2*g
	D3DXVec3Scale( (D3DXVECTOR3*)&param.vRayleigh, &m_vRayleigh, 3.0f/(16.0f*PI) );
	param.vRayleigh.w = -2.0f * m_fG;

	// vMie : rgb : 1/(4*PI) * Bm * (1-g)^2  w : (1+g^2)
	float       fG = 1.0f -m_fG;
	D3DXVec3Scale( (D3DXVECTOR3*)&param.vMie, &m_vMie, fG*fG/(4.0f*PI) );
	param.vMie.w = 1.0f + m_fG * m_fG;

	D3DXVECTOR3 vSum;
	D3DXVec3Add( (D3DXVECTOR3*)&vSum, &m_vRayleigh, &m_vMie );

	// vESun : rgb : Esun/(Br+Bm)             w : R
	param.vESun.x = m_fLightScale * m_vLightColor.x/vSum.x;
	param.vESun.y = m_fLightScale * m_vLightColor.y/vSum.y;
	param.vESun.z = m_fLightScale * m_vLightColor.z/vSum.z;
	param.vESun.w = m_fEarthRadius;
	
	// vSum  : rgb : (Br+Bm)                  w : h(2R+h)
	// scale by inverse of farclip to apply constant scattering effect in case farclip is changed.
	D3DXVec3Scale( (D3DXVECTOR3*)&param.vSum, (D3DXVECTOR3*)&vSum, 1.0f/farPlane );
	param.vSum.w = m_fAtomosHeight * (2.0f*m_fEarthRadius + m_fAtomosHeight);

	// ambient term of scattering
	D3DXVec3Scale( (D3DXVECTOR3*)&param.vAmbient, &m_vAmbientLight, m_fAmbientScale );
	param.vAmbient.w = 1.0f/sqrtf( param.vSum.w );
}


void SSceneParamter::SetTime(float fTimeOfADay)
{
	static const float PI = 3.141592f;
	float fAngle = ( 45.0f ) * PI / 180.0f;
	D3DXVECTOR3 vRotAxis( 0.0f, sinf( fAngle ), cosf( fAngle ) );

	D3DXMATRIX matRot;
	D3DXMatrixRotationAxis( &matRot, &vRotAxis, fTimeOfADay * (1.0f*PI) );

	D3DXVECTOR3 v( -1.0f, 0.0f, 0.0f );
	D3DXVec3TransformNormal( &m_vLightDir, &v, &matRot );
}

bool SSceneParamter::isEqual(const SSceneParamter& sp) const
{
	return	m_fWindVel 		== sp.m_fWindVel		&&
			m_fUVScale 		== sp.m_fUVScale		&&
			m_fCoverage		== sp.m_fCoverage		&&
			useEngineColors == sp.useEngineColors	&&
			isActive		== sp.isActive			&&
			m_fG			== sp.m_fG				&&
			m_fLightScale	== sp.m_fLightScale		&&
			m_fAmbientScale	== sp.m_fAmbientScale	&&
			m_fEarthRadius	== sp.m_fEarthRadius	&&
			m_fAtomosHeight	== sp.m_fAtomosHeight	&&
			m_fCloudHeight	== sp.m_fCloudHeight	&&
			m_vFade_Density	== sp.m_vFade_Density	&&	
			m_vRayleigh		== sp.m_vRayleigh		&&
			m_vMie			== sp.m_vMie;
}

