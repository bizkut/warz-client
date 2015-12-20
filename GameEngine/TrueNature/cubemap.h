#pragma once

#define SKY_DOME_RADIUS 30000.f
// some people fit box in that radius
#define SKY_FAR_PLANE ( SKY_DOME_RADIUS * 1.733f )

struct CubemapTransform
{
	enum FACE
	{
		CUBE_POS_X = D3DCUBEMAP_FACE_POSITIVE_X,
		CUBE_NEG_X = D3DCUBEMAP_FACE_NEGATIVE_X,
		CUBE_POS_Y = D3DCUBEMAP_FACE_POSITIVE_Y,
		CUBE_NEG_Y = D3DCUBEMAP_FACE_NEGATIVE_Y,
		CUBE_POS_Z = D3DCUBEMAP_FACE_POSITIVE_Z,
		CUBE_NEG_Z = D3DCUBEMAP_FACE_NEGATIVE_Z,
	};

	struct Matrices
	{
		D3DXMATRIX projection;
		D3DXMATRIX view[6];
	};

	template<FACE faceIndex>
	static void getLookAtAndUpVectors(D3DXVECTOR3& lookAt, D3DXVECTOR3& up);

	template<> static void getLookAtAndUpVectors<CUBE_POS_X>(D3DXVECTOR3& lookAt, D3DXVECTOR3& up) { lookAt = D3DXVECTOR3( 1.0f, 0.0f, 0.0f); up = D3DXVECTOR3(0.0f, 1.0f, 0.0f); }
	template<> static void getLookAtAndUpVectors<CUBE_NEG_X>(D3DXVECTOR3& lookAt, D3DXVECTOR3& up) { lookAt = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); up = D3DXVECTOR3(0.0f, 1.0f, 0.0f); }
	template<> static void getLookAtAndUpVectors<CUBE_POS_Y>(D3DXVECTOR3& lookAt, D3DXVECTOR3& up) { lookAt = D3DXVECTOR3( 0.0f, 1.0f, 0.0f); up = D3DXVECTOR3(0.0f, 0.0f,-1.0f); }
	template<> static void getLookAtAndUpVectors<CUBE_NEG_Y>(D3DXVECTOR3& lookAt, D3DXVECTOR3& up) { lookAt = D3DXVECTOR3( 0.0f,-1.0f, 0.0f); up = D3DXVECTOR3(0.0f, 0.0f, 1.0f); }
	template<> static void getLookAtAndUpVectors<CUBE_POS_Z>(D3DXVECTOR3& lookAt, D3DXVECTOR3& up) { lookAt = D3DXVECTOR3( 0.0f, 0.0f, 1.0f); up = D3DXVECTOR3(0.0f, 1.0f, 0.0f); }
	template<> static void getLookAtAndUpVectors<CUBE_NEG_Z>(D3DXVECTOR3& lookAt, D3DXVECTOR3& up) { lookAt = D3DXVECTOR3( 0.0f, 0.0f,-1.0f); up = D3DXVECTOR3(0.0f, 1.0f, 0.0f); }

	template<FACE faceIndex>
	static void getViewMatrix(const D3DXVECTOR3& pos, D3DXMATRIX& view)
	{
		D3DXVECTOR3 lookAt, up;
		getLookAtAndUpVectors<faceIndex>(lookAt, up);

		D3DXVECTOR3 dir = pos + lookAt;
		D3DXMatrixLookAtLH( &view, (D3DXVECTOR3*)&pos, (D3DXVECTOR3*)&dir, (D3DXVECTOR3*)&up );
	}

	static void getProjMatrix(D3DXMATRIX& projection)
	{
		float n = 1.0f;
		float f = SKY_FAR_PLANE;

		r3dRenderer->BuildMatrixPerspectiveFovLH(&projection, R3D_PI_2, 1.0f, n, f);
	}

	static void getMatrices(const D3DXVECTOR3& pos, Matrices& mtx)
	{
		getProjMatrix(mtx.projection);
		getViewMatrix<CUBE_POS_X>(pos, mtx.view[CUBE_POS_X]);
		getViewMatrix<CUBE_NEG_X>(pos, mtx.view[CUBE_NEG_X]);
		getViewMatrix<CUBE_POS_Y>(pos, mtx.view[CUBE_POS_Y]);
		getViewMatrix<CUBE_NEG_Y>(pos, mtx.view[CUBE_NEG_Y]);
		getViewMatrix<CUBE_POS_Z>(pos, mtx.view[CUBE_POS_Z]);
		getViewMatrix<CUBE_NEG_Z>(pos, mtx.view[CUBE_NEG_Z]);
	}
};
