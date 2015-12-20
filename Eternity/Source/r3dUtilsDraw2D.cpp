#include "r3dPCH.h"
#include "r3d.h"

void r3dDrawLine3DInternal(const r3dPoint3D& P1, const r3dPoint3D& P2, const r3dCamera &Cam, float Width, const r3dColor24& clr, r3dTexture *Tex)
{
	// make vector perpendicular to line and to camera vectors
	r3dVector V2 = (P2  - P1).Cross(Cam - P1);
	V2.Normalize();

	R3D_DEBUG_VERTEX V[3];

	// V[2].Pos.Assign(P1 - V2 * Width / 2);
	// V[1].Pos.Assign(P2 + V2 * Width / 2);
	// V[0].Pos.Assign(P1 + V2 * Width / 2);

	V[0].color = clr;
	V[1].color = clr;
	V[2].color = clr;

	V[0].Pos.Assign(P1 - V2 * Width / 2);
	V[1].Pos.Assign(P1 + V2 * Width / 2);
	V[2].Pos.Assign(P2 + V2 * Width / 2);

	V[0].tu   = 0; V[0].tv = 0;
	V[1].tu   = 1; V[1].tv = 0;
	V[2].tu   = 1; V[2].tv = 1;

	//r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_PUSH);
	R3D_DEBUG_VERTEX::ComputeNormals(V, 3);
	r3dRenderer->Fill3DPolygon(3, V);

	V[0].tu   = 1; V[0].tv = 1;
	V[1].tu   = 0; V[1].tv = 1;
	V[2].tu   = 0; V[2].tv = 0;

	V[0].Pos.Assign(P2 + V2 * Width / 2);
	V[1].Pos.Assign(P2 - V2 * Width / 2);
	V[2].Pos.Assign(P1 - V2 * Width / 2);

	R3D_DEBUG_VERTEX::ComputeNormals(V, 3);

	r3dRenderer->Fill3DPolygon(3, V);

	// r3dRenderer->Flush();
	//r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
}

void r3dDrawLine3D(const r3dPoint3D& P1, const r3dPoint3D& P2, const r3dCamera &Cam, float Width, const r3dColor24& clr, r3dTexture *Tex)
{
	r3dRenderer->BeginFill3DPolygon(6);
	r3dRenderer->SetTex(Tex);
	r3dDrawLine3DInternal(P1, P2, Cam, Width, clr, Tex);
	r3dRenderer->EndFill3DPolygon();
}

//////////////////////////////////////////////////////////////////////////

void r3dDrawLineList3D(const r3dDrawLineDesc *lines, int numLines, const r3dCamera &Cam, r3dTexture *Tex)
{
	if (!lines)
		return;

	int verticesInLine = 6;
	int maxLines = r3dRenderer->GetPolygon3DBufferCapacity() / verticesInLine;
	int batch = R3D_MIN(numLines, maxLines);
	int remainder = numLines - batch;
	int startLine = 0;

	while (batch > 0)
	{
		r3dRenderer->BeginFill3DPolygon(batch * verticesInLine);
		r3dRenderer->SetTex(Tex);
		for (int i = startLine; i < startLine + batch; ++i)
		{
			const r3dDrawLineDesc &l = lines[i];
			r3dDrawLine3DInternal(l.p1, l.p2, Cam, l.width, l.c, Tex);
		}
		r3dRenderer->EndFill3DPolygon();

		startLine += batch;
		batch = R3D_MIN(remainder, maxLines);
		remainder -= batch;
	}
}

//////////////////////////////////////////////////////////////////////////

void r3dDrawLine3DColors(const r3dPoint3D& P1, const r3dPoint3D& P2, r3dCamera &Cam, float Width, const r3dColor24& clr, const r3dColor24& clr2, r3dTexture *Tex)
{
  // make vector perpendicular to line and to camera vectors
  r3dVector V2 = (P2  - P1).Cross(Cam - P1);
  V2.Normalize();

 R3D_DEBUG_VERTEX V[3];

 V[0].color = clr;
 V[1].color = clr2;
 V[2].color = clr2;

 V[0].Pos.Assign(P1 - V2 * Width / 2);
 V[1].Pos.Assign(P1 + V2 * Width / 2);
 V[2].Pos.Assign(P2 + V2 * Width / 2);

 V[0].tu   = 0; V[0].tv = 0;
 V[1].tu   = 1; V[1].tv = 0;
 V[2].tu   = 1; V[2].tv = 1;

 //r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_PUSH);
 r3dRenderer->SetTex(Tex);
 r3dRenderer->Render3DPolygon(3, V);

 V[0].tu   = 1; V[0].tv = 1;
 V[1].tu   = 0; V[1].tv = 1;
 V[2].tu   = 0; V[2].tv = 0;

 V[0].color = clr2;
 V[1].color = clr;
 V[2].color = clr;

 V[0].Pos.Assign(P2 + V2 * Width / 2);
 V[1].Pos.Assign(P2 - V2 * Width / 2);
 V[2].Pos.Assign(P1 - V2 * Width / 2);

 R3D_DEBUG_VERTEX::ComputeNormals(V, 3);
 r3dRenderer->Render3DPolygon(3, V);

// r3dRenderer->Flush();
 //r3dRenderer->SetRenderingMode(R3D_BLEND_POP);

  return;
}

void r3dDrawCircle3D(const r3dPoint3D& P1, const float Radius, const r3dCamera &Cam, float Width, const r3dColor24& clr, r3dTexture *Tex, int Segments /*= 16*/, float* Heights /*= NULL*/)
{
  // add vertices
	float    g_dDegToRad     =   3.1415926f / 180.0f;

	r3dPoint3D VV, VV1;

	for( int x = 0; x<Segments; x++ )
    {
		float theta = float( x ) / float(Segments) * 360.0f * g_dDegToRad;
		float theta1 = float( x+1 ) / float(Segments) * 360.0f * g_dDegToRad;

		VV.Assign(	static_cast< float >( cosf( theta ) * Radius ),
							0,
							static_cast< float >( sinf( theta ) * Radius ) );

		VV1.Assign(	static_cast< float >( cosf( theta1 ) * Radius ),
							0,
							static_cast< float >(sinf( theta1 ) * Radius ) );

		VV1 += P1 ;
		VV += P1 ;

		if( Heights )
		{
			VV1.Y = Heights[ ( x + 1 ) % Segments ] ;
			VV.Y = Heights[ x ] ;
		}

		r3dDrawLine3D(VV,VV1, Cam, Width, clr, Tex);
	}

}



void r3dDrawUniformCircle3D(const r3dPoint3D& P1, const float Radius, r3dCamera &Cam, const r3dColor24& clr)
{
	// add vertices
	float    g_dDegToRad     =   3.1415926f / 180.0f;

	r3dPoint3D VV, VV1;

	r3dRenderer->BeginFill3DLine(2*16);

	int Segments = 16;
	for( int x = 0; x<Segments; x++ )
	{
		float theta = float( x ) / float(Segments) * 360.0f * g_dDegToRad;
		float theta1 = float( x+1 ) / float(Segments) * 360.0f * g_dDegToRad;

		VV.Assign(	static_cast< float >( cosf( theta ) * Radius ),
			0,
			static_cast< float >( sinf( theta ) * Radius ) );

		VV1.Assign(	static_cast< float >( cosf( theta1 ) * Radius ),
			0,
			static_cast< float >(sinf( theta1 ) * Radius ) );

		r3dFillUniformLine3D(P1+VV,P1+VV1, Cam, clr);
	}
	r3dRenderer->EndFill3DLine();

}













void r3dDrawLine3D1(const r3dPoint3D& P1, const r3dPoint3D& P2, r3dCamera &Cam, float Width1, float Width2, const r3dColor24& clr, r3dTexture *Tex)
{
  // make vector perpendicular to line and to camera vectors
  r3dVector V2 = (P2  - P1).Cross(Cam - P1);
  V2.Normalize();

 R3D_DEBUG_VERTEX V[3];

 V[0].color = clr;
 V[1].color = clr;
 V[2].color = clr;

 V[0].Pos.Assign(P1 - V2 * Width1 / 2);
 V[1].Pos.Assign(P1 + V2 * Width1 / 2);
 V[2].Pos.Assign(P2 + V2 * Width2 / 2);

 V[0].tu   = 0; V[0].tv = 0;
 V[1].tu   = 1; V[1].tv = 0;
 V[2].tu   = 1; V[2].tv = 1;

 //r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_PUSH);
 //r3dRenderer->SetTex(Tex);
 r3dRenderer->Render3DPolygon(3, V);

 V[0].tu   = 1; V[0].tv = 1;
 V[1].tu   = 0; V[1].tv = 1;
 V[2].tu   = 0; V[2].tv = 0;

 V[0].Pos.Assign(P2 + V2 * Width2 / 2);
 V[1].Pos.Assign(P2 - V2 * Width2 / 2);
 V[2].Pos.Assign(P1 - V2 * Width1 / 2);

 R3D_DEBUG_VERTEX::ComputeNormals(V, 3);
 r3dRenderer->Render3DPolygon(3, V);

// r3dRenderer->Flush();
 //r3dRenderer->SetRenderingMode(R3D_BLEND_POP);

  return;
}

void r3dDrawUniformLine3D(const r3dPoint3D& P1, const r3dPoint3D& P2, const r3dCamera &Cam, const r3dColor24& clr)
{
	R3D_DEBUG_VERTEX V[2];


	V[0].color = clr;
	V[1].color = clr;

	V[0].Pos = P1;
	V[1].Pos = P2;

	V[0].tu   = 0; V[0].tv = 0;
	V[1].tu   = 0; V[1].tv = 0;

//	V[0].Normal = V[1].Normal = r3dPoint3D(0.0f, 1.0f, 0.0f);

	r3dRenderer->Render3DLine(2, V);
}

void r3dFillUniformLine3D(const r3dPoint3D& P1, const r3dPoint3D& P2, const r3dCamera &Cam, const r3dColor24& clr)
{
	R3D_DEBUG_VERTEX V[2];


	V[0].color = clr;
	V[1].color = clr;

	V[0].Pos = P1;
	V[1].Pos = P2;

	V[0].tu   = 0; V[0].tv = 0;
	V[1].tu   = 0; V[1].tv = 0;

//	V[0].Normal = V[1].Normal = r3dPoint3D(0.0f, 1.0f, 0.0f);

	r3dRenderer->Fill3DLine(2, V);
}


void r3dDrawUniformSphere(r3dVector const & vPos, float fRadius, r3dCamera &Cam, r3dColor24 const & clr, uint32_t dwNumSphereRings, uint32_t dwNumSphereSegments, r3dVector vScale )
{
	float fDeltaRingAngle = ( D3DX_PI / dwNumSphereRings );
	float fDeltaSegAngle  = ( 2.0f * D3DX_PI / dwNumSphereSegments );

	// Generate the group of rings for the sphere
	for( uint32_t ring = 0; ring < dwNumSphereRings; ring++ )
	{
		float r0 = fRadius * sinf( (ring+0) * fDeltaRingAngle );
		float r1 = fRadius * sinf( (ring+1) * fDeltaRingAngle );
		float y0 = fRadius * cosf( (ring+0) * fDeltaRingAngle );
		float y1 = fRadius * cosf( (ring+1) * fDeltaRingAngle );

		// Generate the group of segments for the current ring
		for( uint32_t seg = 0; seg < (dwNumSphereSegments+1); seg++ )
		{
			float r0x1 =  r0 * cosf( ( seg + 1 ) * fDeltaSegAngle );
			float r0z1 =  r0 * sinf( ( seg + 1 ) * fDeltaSegAngle );

			float r1x0 =  r1 * cosf( seg * fDeltaSegAngle );
			float r1z0 =  r1 * sinf( seg * fDeltaSegAngle );

			float r1x1 =  r1 * cosf( ( seg + 1 ) * fDeltaSegAngle );
			float r1z1 =  r1 * sinf( ( seg + 1 ) * fDeltaSegAngle );

			r3dVector v0 = r3dVector(r1x0*vScale.x,y1*vScale.y,r1z0*vScale.z) + vPos;
			r3dVector v1 = r3dVector(r1x1*vScale.x,y1*vScale.y,r1z1*vScale.z) + vPos;
			r3dVector v2 = r3dVector(r0x1*vScale.x,y0*vScale.y,r0z1*vScale.z) + vPos;

			r3dDrawUniformLine3D ( v0, v1, Cam, clr );
			r3dDrawUniformLine3D ( v1, v2, Cam, clr );
		}
	}
}


/*
"                     "
"       /------/(4)   "
"      /      /|      "
"     /_____ / |      "
" (3)|      |         "
"    | /    | /       "
"    |/_____|/        "
"   (1)    (2)        "
*/
void r3dDrawUniformBoundBox( const r3dBoundBox &Box, const r3dCamera &Cam, const r3dColor24& clr )
{
	r3dPoint3D 	p1, p2;

// 1
  p1    = Box.Org;

  p2    = p1;
  p2.x += Box.Size.x;
  r3dDrawUniformLine3D(p1, p2, Cam, clr);
  p2    = p1;
  p2.z += Box.Size.z;
  r3dDrawUniformLine3D(p1, p2, Cam, clr);
  p2    = p1;
  p2.y += Box.Size.y;
  r3dDrawUniformLine3D(p1, p2, Cam, clr);

  p1.y += Box.Size.y;
  p2    = p1;
  p2.x += Box.Size.x;
  r3dDrawUniformLine3D(p1, p2, Cam, clr);
  p2    = p1;
  p2.z += Box.Size.z;
  r3dDrawUniformLine3D(p1, p2, Cam, clr);

// 2
  p1    = Box.Org;
  p1.x += Box.Size.x;

  p2    = p1;
  p2.z += Box.Size.z;
  r3dDrawUniformLine3D(p1, p2, Cam, clr);
  p2    = p1;
  p2.y += Box.Size.y;
  r3dDrawUniformLine3D(p1, p2, Cam, clr);

// 3
  p1    = Box.Org;
  p1.z += Box.Size.z;

  p2    = p1;
  p2.x += Box.Size.x;
  r3dDrawUniformLine3D(p1, p2, Cam, clr);
  p2    = p1;
  p2.y += Box.Size.y;
  r3dDrawUniformLine3D(p1, p2, Cam, clr);

// 4
  p1    = Box.Org + Box.Size;

  p2    = p1;
  p2.x -= Box.Size.x;
  r3dDrawUniformLine3D(p1, p2, Cam, clr);
  p2    = p1;
  p2.z -= Box.Size.z;
  r3dDrawUniformLine3D(p1, p2, Cam, clr);
  p2    = p1;
  p2.y -= Box.Size.y;
  r3dDrawUniformLine3D(p1, p2, Cam, clr);

  r3dRenderer->Flush();
  return;
}

void r3dDrawUniformBoundBox( const r3dBoundBox &box, const D3DXMATRIX& transform, const r3dCamera &Cam, const r3dColor24& clr )
{
	D3DXVECTOR3 poses[ 8 ];
	D3DXVECTOR4 transformed_poses[ 8 ];

	poses[ 0 ] = D3DXVECTOR3( box.Org.x,				box.Org.y,				box.Org.z );
	poses[ 1 ] = D3DXVECTOR3( box.Org.x + box.Size.x,	box.Org.y,				box.Org.z );
	poses[ 2 ] = D3DXVECTOR3( box.Org.x + box.Size.x,	box.Org.y + box.Size.y, box.Org.z );
	poses[ 3 ] = D3DXVECTOR3( box.Org.x,				box.Org.y + box.Size.y,	box.Org.z );

	poses[ 4 ] = D3DXVECTOR3( box.Org.x,				box.Org.y,				box.Org.z + box.Size.z );
	poses[ 5 ] = D3DXVECTOR3( box.Org.x + box.Size.x,	box.Org.y,				box.Org.z + box.Size.z );
	poses[ 6 ] = D3DXVECTOR3( box.Org.x + box.Size.x,	box.Org.y + box.Size.y, box.Org.z + box.Size.z );
	poses[ 7 ] = D3DXVECTOR3( box.Org.x,				box.Org.y + box.Size.y,	box.Org.z + box.Size.z );

	D3DXVec3TransformArray( transformed_poses, sizeof transformed_poses[ 0 ], poses, sizeof poses[ 0 ], &transform, 8 );

	r3dPoint3D p3ds[ 8 ];

	for( int i = 0, e = R3D_ARRAYSIZE(transformed_poses); i < e; i ++ )
	{
		D3DXVECTOR4& tpos = transformed_poses[ i ];
		p3ds[ i ] = r3dPoint3D( tpos.x, tpos.y, tpos.z );
	}

	r3dDrawUniformLine3D( p3ds[ 0 + 0 ], p3ds[ 1 + 0 ], Cam, clr );
	r3dDrawUniformLine3D( p3ds[ 1 + 0 ], p3ds[ 2 + 0 ], Cam, clr );
	r3dDrawUniformLine3D( p3ds[ 2 + 0 ], p3ds[ 3 + 0 ], Cam, clr );
	r3dDrawUniformLine3D( p3ds[ 3 + 0 ], p3ds[ 0 + 0 ], Cam, clr );

	r3dDrawUniformLine3D( p3ds[ 0 + 4 ], p3ds[ 1 + 4 ], Cam, clr );
	r3dDrawUniformLine3D( p3ds[ 1 + 4 ], p3ds[ 2 + 4 ], Cam, clr );
	r3dDrawUniformLine3D( p3ds[ 2 + 4 ], p3ds[ 3 + 4 ], Cam, clr );
	r3dDrawUniformLine3D( p3ds[ 3 + 4 ], p3ds[ 0 + 4 ], Cam, clr );

	r3dDrawUniformLine3D( p3ds[ 0 + 0 ], p3ds[ 0 + 4 ], Cam, clr );
	r3dDrawUniformLine3D( p3ds[ 1 + 0 ], p3ds[ 1 + 4 ], Cam, clr );
	r3dDrawUniformLine3D( p3ds[ 2 + 0 ], p3ds[ 2 + 4 ], Cam, clr );
	r3dDrawUniformLine3D( p3ds[ 3 + 0 ], p3ds[ 3 + 4 ], Cam, clr );

	r3dRenderer->Flush();
	return;
}

void r3dDrawSphereSolid(r3dVector const & vPos, float fRadius, const r3dCamera &Cam, r3dColor24 const & clr, uint32_t dwNumSphereRings, uint32_t dwNumSphereSegments, bool bIncludeOutline, r3dColor24 const & clr2 )
{
	float fDeltaRingAngle = ( D3DX_PI / dwNumSphereRings );
	float fDeltaSegAngle  = ( 2.0f * D3DX_PI / dwNumSphereSegments );

	float X = vPos.x;
	float Y = vPos.y;
	float Z = vPos.z;

	// Generate the group of rings for the sphere
	for( uint32_t ring = 0; ring < dwNumSphereRings; ring++ )
	{
		float r0 = fRadius * sinf( (ring+0) * fDeltaRingAngle );
		float r1 = fRadius * sinf( (ring+1) * fDeltaRingAngle );
		float y0 = fRadius * cosf( (ring+0) * fDeltaRingAngle );
		float y1 = fRadius * cosf( (ring+1) * fDeltaRingAngle );

		// Generate the group of segments for the current ring
		for( uint32_t seg = 0; seg < dwNumSphereSegments; seg++ )
		{
			float r0x0 =  r0 * cosf( seg * fDeltaSegAngle );
			float r0z0 =  r0 * sinf( seg * fDeltaSegAngle );

			float r0x1 =  r0 * cosf( ( seg + 1 ) * fDeltaSegAngle );
			float r0z1 =  r0 * sinf( ( seg + 1 ) * fDeltaSegAngle );

			float r1x0 =  r1 * cosf( seg * fDeltaSegAngle );
			float r1z0 =  r1 * sinf( seg * fDeltaSegAngle );

			float r1x1 =  r1 * cosf( ( seg + 1 ) * fDeltaSegAngle );
			float r1z1 =  r1 * sinf( ( seg + 1 ) * fDeltaSegAngle );

			r3dVector v0 = r3dVector(r1x0,y1,r1z0) + vPos;
			r3dVector v1 = r3dVector(r1x1,y1,r1z1) + vPos;
			r3dVector v2 = r3dVector(r0x1,y0,r0z1) + vPos;

			r3dVector vPt1 ( X + r0x0, Y + y0, Z + r0z0 );
			r3dVector vPt2 ( X + r0x1, Y + y0, Z + r0z1 );
			r3dVector vPt3 ( X + r1x0, Y + y1, Z + r1z0 );

			r3dVector vPt4 ( X + r0x1, Y + y0, Z + r0z1 );
			r3dVector vPt5 ( X + r1x1, Y + y1, Z + r1z1 );
			r3dVector vPt6 ( X + r1x0, Y + y1, Z + r1z0 );

			r3dDrawTriangle3D ( vPt1, vPt2, vPt3, Cam, clr );
			r3dDrawTriangle3D ( vPt4, vPt5, vPt6, Cam, clr );

			if( bIncludeOutline )
			{
				r3dDrawLine3D(vPt1, vPt2, Cam, 0.01f, clr2 );
				r3dDrawLine3D(vPt2, vPt3, Cam, 0.01f, clr2 );
				r3dDrawLine3D(vPt3, vPt1, Cam, 0.01f, clr2 );

				r3dDrawLine3D(vPt4, vPt5, Cam, 0.01f, clr2 );
				r3dDrawLine3D(vPt5, vPt6, Cam, 0.01f, clr2 );
				// No need to draw last line, because vPt3 == vPt6, vPt2 == vPt4
				// and a line has already been drawn from vPt2 to vPt3.
				//r3dDrawLine3D(vPt6, vPt4, Cam, 0.01f, clr2 );

			}
		}
	}
}

void r3dDrawBoundBox( const r3dBoundBox &Box, const r3dCamera &Cam, const r3dColor24& clr, float fLineWidth, bool flush )
{
	r3dPoint3D 	p1, p2;

	// 1
	p1    = Box.Org;

	p2    = p1;
	p2.x += Box.Size.x;
	r3dDrawLine3D(p1, p2, Cam, fLineWidth, clr);
	p2    = p1;
	p2.z += Box.Size.z;
	r3dDrawLine3D(p1, p2, Cam, fLineWidth, clr);
	p2    = p1;
	p2.y += Box.Size.y;
	r3dDrawLine3D(p1, p2, Cam, fLineWidth, clr);

	p1.y += Box.Size.y;
	p2    = p1;
	p2.x += Box.Size.x;
	r3dDrawLine3D(p1, p2, Cam, fLineWidth, clr);
	p2    = p1;
	p2.z += Box.Size.z;
	r3dDrawLine3D(p1, p2, Cam, fLineWidth, clr);

	// 2
	p1    = Box.Org;
	p1.x += Box.Size.x;

	p2    = p1;
	p2.z += Box.Size.z;
	r3dDrawLine3D(p1, p2, Cam, fLineWidth, clr);
	p2    = p1;
	p2.y += Box.Size.y;
	r3dDrawLine3D(p1, p2, Cam, fLineWidth, clr);

	// 3
	p1    = Box.Org;
	p1.z += Box.Size.z;

	p2    = p1;
	p2.x += Box.Size.x;
	r3dDrawLine3D(p1, p2, Cam, fLineWidth, clr);
	p2    = p1;
	p2.y += Box.Size.y;
	r3dDrawLine3D(p1, p2, Cam, fLineWidth, clr);

	// 4
	p1    = Box.Org + Box.Size;

	p2    = p1;
	p2.x -= Box.Size.x;
	r3dDrawLine3D(p1, p2, Cam, fLineWidth, clr);
	p2    = p1;
	p2.z -= Box.Size.z;
	r3dDrawLine3D(p1, p2, Cam, fLineWidth, clr);
	p2    = p1;
	p2.y -= Box.Size.y;
	r3dDrawLine3D(p1, p2, Cam, fLineWidth, clr);

	if( flush )
	{
		r3dRenderer->Flush();
	}
	return;
}

extern  void		r3dDrawUniformGridPlane( const r3dPoint3D& centrePos, const r3dCamera &Cam, float width, float depth, int cellsX, int cellsZ, const r3dColor24& colr )
{
	r3dPoint3D origin = centrePos;

	origin.x -= width * 0.5f;
	origin.z -= depth * 0.5f;

	for( int i = 0, e = cellsX; i <= e; i ++ )
	{
		r3dPoint3D start = origin;
		r3dPoint3D end = origin;

		float dw = width * i / cellsX;

		end.x += dw;
		start.x += dw;

		end.z += depth;

		r3dDrawUniformLine3D( start, end, Cam, colr );
	}

	for( int i = 0, e = cellsZ; i <= e; i ++ )
	{
		r3dPoint3D start	= origin;
		r3dPoint3D end		= origin;

		float dh = depth * i / cellsZ;

		end.z += dh;
		start.z += dh;

		end.x += width;

		r3dDrawUniformLine3D( start, end, Cam, colr );
	}
}

void r3dDrawGridPlane( const r3dPoint3D& centrePos, const r3dCamera &Cam, float width, float depth, int cellsX, int cellsZ, const r3dColor24& colr, float lineWidth  )
{
	r3dPoint3D origin = centrePos;

	origin.x -= width * 0.5f;
	origin.z -= depth * 0.5f;

	for( int i = 0, e = cellsX; i <= e; i ++ )
	{
		r3dPoint3D start = origin;
		r3dPoint3D end = origin;

		float dw = width * i / cellsX;

		end.x += dw;
		start.x += dw;

		end.z += depth;

		r3dDrawLine3D( start, end, Cam, lineWidth, colr );
	}

	for( int i = 0, e = cellsZ; i <= e; i ++ )
	{
		r3dPoint3D start	= origin;
		r3dPoint3D end		= origin;

		float dh = depth * i / cellsZ;

		end.z += dh;
		start.z += dh;

		end.x += width;

		r3dDrawLine3D( start, end, Cam, lineWidth, colr );
	}
}

//////////////////////////////////////////////////////////////////////////

void r3dDrawTriangle3DInternal(const r3dPoint3D& P1, const r3dPoint3D& P2, const r3dPoint3D& P3, const r3dCamera &Cam, const r3dColor24& clr, r3dTexture *Tex, float *TC, bool bDontDot)
{
	// make vector perpendicular to line and to camera vectors
	r3dVector norm = (P2  - P1).Cross(P3 - P1);
	norm.Normalize();

	R3D_DEBUG_VERTEX V[3];

	float LdotN = bDontDot ? 1.0f : norm.Dot(r3dPoint3D(0,1,0));
	V[0].color = clr * LdotN;
	V[1].color = clr * LdotN;
	V[2].color = clr * LdotN;

	//	V[0].Normal = norm;
	//	V[1].Normal = norm;
	//	V[2].Normal = norm;

	V[0].Pos = P1;
	V[1].Pos = P2;
	V[2].Pos = P3;

	V[0].tu   = 0; V[0].tv = 0;
	V[1].tu   = 0; V[1].tv = 1;
	V[2].tu   = 1; V[2].tv = 1;

	if ( TC )
	{
		V[0].tu   = TC[0]; V[0].tv = TC[1];
		V[1].tu   = TC[2]; V[1].tv = TC[3];
		V[2].tu   = TC[4]; V[2].tv = TC[5];
	}

	R3D_DEBUG_VERTEX::ComputeNormals(V, 3);
	r3dRenderer->Fill3DPolygon(3, V);
}

//////////////////////////////////////////////////////////////////////////

void r3dDrawTriangle3D(const r3dPoint3D& P1, const r3dPoint3D& P2, const r3dPoint3D& P3, const r3dCamera &Cam, const r3dColor24& clr, r3dTexture *Tex, float *TC, bool bDontDot )
{
	r3dRenderer->SetTex(Tex);
	r3dRenderer->BeginFill3DPolygon(3);
	r3dDrawTriangle3DInternal(P1, P2, P3, Cam, clr, Tex, TC, bDontDot);
	r3dRenderer->EndFill3DPolygon();
}

//////////////////////////////////////////////////////////////////////////

void r3dDrawTriangleList3D(const r3dDrawTriDesc *tris, int numTris, const r3dCamera &Cam, r3dTexture *Tex, bool bDontDot)
{
	if (!tris)
		return;

	int verticesInTri = 3;
	int maxTris = r3dRenderer->GetPolygon3DBufferCapacity() / verticesInTri;
	int batch = R3D_MIN(numTris, maxTris);
	int remainder = numTris - batch;
	int startTri = 0;

	while (batch > 0)
	{
		r3dRenderer->BeginFill3DPolygon(batch * verticesInTri);
		r3dRenderer->SetTex(Tex);
		for (int i = startTri; i < startTri + batch; ++i)
		{
			const r3dDrawTriDesc &t = tris[i];
			r3dDrawTriangle3DInternal(t.p1, t.p2, t.p3, Cam, t.c, Tex, 0, bDontDot);
		}
		r3dRenderer->EndFill3DPolygon();

		startTri += batch;
		batch = R3D_MIN(remainder, maxTris);
		remainder -= batch;
	}
}

//////////////////////////////////////////////////////////////////////////

void r3dDrawBox3D(float x, float y, float z, float w, float h, float d, const r3dColor24& clr )
{
 const size_t VERTEX_COUNT = 36;
 R3D_DEBUG_VERTEX V[VERTEX_COUNT];
 for(int i=0, e = VERTEX_COUNT; i<e; i++)
 {

  V[i].tu    = V[i].tv  = 0;
  V[i].Pos   = r3dPoint3D( x, y, z );
  V[i].color = clr;
 }

 float hw = w * 0.5f;
 float hh = h * 0.5f;
 float hd = d * 0.5f;

 // face 0
 V[ 0].Pos   += r3dPoint3D( +hw, -hh, -hd );  // V[ 0].Normal   = r3dPoint3D( +0, +0, -1 );
 V[ 1].Pos   += r3dPoint3D( -hw, -hh, -hd );  // V[ 1].Normal   = r3dPoint3D( +0, +0, -1 );
 V[ 2].Pos   += r3dPoint3D( -hw, +hh, -hd );  // V[ 2].Normal   = r3dPoint3D( +0, +0, -1 );
 V[ 3].Pos   += r3dPoint3D( -hw, +hh, -hd );  // V[ 3].Normal   = r3dPoint3D( +0, +0, -1 );
 V[ 4].Pos   += r3dPoint3D( +hw, +hh, -hd );  // V[ 4].Normal   = r3dPoint3D( +0, +0, -1 );
 V[ 5].Pos   += r3dPoint3D( +hw, -hh, -hd );  // V[ 5].Normal   = r3dPoint3D( +0, +0, -1 );
 
 // face 1
 V[ 6].Pos   += r3dPoint3D( -hw, -hh, +hd );   // V[ 6].Normal   = r3dPoint3D( +0, +0, +1 );
 V[ 7].Pos   += r3dPoint3D( +hw, -hh, +hd );   // V[ 7].Normal   = r3dPoint3D( +0, +0, +1 );
 V[ 8].Pos   += r3dPoint3D( -hw, +hh, +hd );   // V[ 8].Normal   = r3dPoint3D( +0, +0, +1 );
 V[ 9].Pos   += r3dPoint3D( +hw, +hh, +hd );   // V[ 9].Normal   = r3dPoint3D( +0, +0, +1 );
 V[10].Pos   += r3dPoint3D( -hw, +hh, +hd );   // V[10].Normal   = r3dPoint3D( +0, +0, +1 );
 V[11].Pos   += r3dPoint3D( +hw, -hh, +hd );   // V[11].Normal   = r3dPoint3D( +0, +0, +1 );

 // face 2
 V[12].Pos   += r3dPoint3D( -hw, -hh, -hd );   // V[12].Normal   = r3dPoint3D( +0, -1, +0 );
 V[13].Pos   += r3dPoint3D( +hw, -hh, -hd );   // V[13].Normal   = r3dPoint3D( +0, -1, +0 );
 V[14].Pos   += r3dPoint3D( -hw, -hh, +hd );   // V[14].Normal   = r3dPoint3D( +0, -1, +0 );
 V[15].Pos   += r3dPoint3D( +hw, -hh, +hd );   // V[15].Normal   = r3dPoint3D( +0, -1, +0 );
 V[16].Pos   += r3dPoint3D( -hw, -hh, +hd );   // V[16].Normal   = r3dPoint3D( +0, -1, +0 );
 V[17].Pos   += r3dPoint3D( +hw, -hh, -hd );   // V[17].Normal   = r3dPoint3D( +0, -1, +0 );

 // face 3
 V[18].Pos   += r3dPoint3D( +hw, +hh, -hd );   // V[18].Normal   = r3dPoint3D( +0, +1, +0 );
 V[19].Pos   += r3dPoint3D( -hw, +hh, -hd );   // V[19].Normal   = r3dPoint3D( +0, +1, +0 );
 V[20].Pos   += r3dPoint3D( -hw, +hh, +hd );   // V[20].Normal   = r3dPoint3D( +0, +1, +0 );
 V[21].Pos   += r3dPoint3D( -hw, +hh, +hd );   // V[21].Normal   = r3dPoint3D( +0, +1, +0 );
 V[22].Pos   += r3dPoint3D( +hw, +hh, +hd );   // V[22].Normal   = r3dPoint3D( +0, +1, +0 );
 V[23].Pos   += r3dPoint3D( +hw, +hh, -hd );   // V[23].Normal   = r3dPoint3D( +0, +1, +0 );

 // face 4
 V[24].Pos   += r3dPoint3D( -hw, +hh, -hd );   // V[24].Normal   = r3dPoint3D( -1, +0, +0 );
 V[25].Pos   += r3dPoint3D( -hw, -hh, -hd );   // V[25].Normal   = r3dPoint3D( -1, +0, +0 );
 V[26].Pos   += r3dPoint3D( -hw, -hh, +hd );   // V[26].Normal   = r3dPoint3D( -1, +0, +0 );
 V[27].Pos   += r3dPoint3D( -hw, -hh, +hd );   // V[27].Normal   = r3dPoint3D( -1, +0, +0 );
 V[28].Pos   += r3dPoint3D( -hw, +hh, +hd );   // V[28].Normal   = r3dPoint3D( -1, +0, +0 );
 V[29].Pos   += r3dPoint3D( -hw, +hh, -hd );   // V[29].Normal   = r3dPoint3D( -1, +0, +0 );

 // face 5
 V[30].Pos   += r3dPoint3D( +hw, -hh, -hd );   // V[30].Normal   = r3dPoint3D( +1, +0, +0 );
 V[31].Pos   += r3dPoint3D( +hw, +hh, -hd );   // V[31].Normal   = r3dPoint3D( +1, +0, +0 );
 V[32].Pos   += r3dPoint3D( +hw, -hh, +hd );   // V[32].Normal   = r3dPoint3D( +1, +0, +0 );
 V[33].Pos   += r3dPoint3D( +hw, +hh, +hd );   // V[33].Normal   = r3dPoint3D( +1, +0, +0 );
 V[34].Pos   += r3dPoint3D( +hw, -hh, +hd );   // V[34].Normal   = r3dPoint3D( +1, +0, +0 );
 V[35].Pos   += r3dPoint3D( +hw, +hh, -hd );   // V[35].Normal   = r3dPoint3D( +1, +0, +0 );

 r3dRenderer->SetTex( NULL );
 r3dRenderer->Render3DTriangles( VERTEX_COUNT, V );
 r3dRenderer->Flush();

}

void r3dDrawBox2D(float x, float y, float w, float h, const r3dColor24& clr, r3dTexture *Tex, float *TC)
{
 R3D_SCREEN_VERTEX 	V[4];

 int		i;

 for(i=0; i<4; i++)
 {
  V[i].color    = clr.GetPacked();
  V[i].z   = r3dRenderer->GetNearPlaneZValue();    //Z;
  V[i].rwh = 1.0f; //Z;//1.0f / Z;
 }

 float DS = -0.5f;

 V[0].x   = x+DS;  	V[0].y   = y+DS;
 V[1].x   = x + w+DS;  	V[1].y   = y+DS;
 V[2].x   = x + w+DS;  	V[2].y   = y + h+DS;
 V[3].x   = x+DS;  	V[3].y   = y + h+DS;

  if (!TC)
  {
   V[0].tu = 0; 		V[0].tv = 0;
   V[1].tu = 1; 		V[1].tv = 0;
   V[2].tu = 1; 		V[2].tv = 1;
   V[3].tu = 0; 		V[3].tv = 1;
   V[0].tu2 = 0; 		V[0].tv2 = 0;
   V[1].tu2 = 1; 		V[1].tv2 = 0;
   V[2].tu2 = 1; 		V[2].tv2 = 1;
   V[3].tu2 = 0; 		V[3].tv2 = 1;
  }
  else
  {
   V[0].tu = TC[0]; 		V[0].tv = TC[1];
   V[1].tu = TC[2]; 		V[1].tv = TC[3];
   V[2].tu = TC[4]; 		V[2].tv = TC[5];
   V[3].tu = TC[6]; 		V[3].tv = TC[7];

   V[0].tu2 = TC[8]; 		V[0].tv2 = TC[9];
   V[1].tu2 = TC[10]; 		V[1].tv2 = TC[11];
   V[2].tu2 = TC[12]; 		V[2].tv2 = TC[13];
   V[3].tu2 = TC[14]; 		V[3].tv2 = TC[15];
  }

  r3dRenderer->SetTex(Tex);
  r3dRenderer->Render2DPolygon(4, V);
  r3dRenderer->Flush();
}

void 		r3dDrawBox2D         ( r3dPoint2D p0, r3dPoint2D p1, r3dPoint2D p2, r3dPoint2D p3, const r3dColor24& clr, r3dTexture *Tex/*=NULL*/, float *TC/*=NULL*/)
{
 R3D_SCREEN_VERTEX 	V[4];

 int		i;

 for(i=0; i<4; i++)
 {
  V[i].color    = clr.GetPacked();
  V[i].z   = r3dRenderer->GetNearPlaneZValue();    //Z;
  V[i].rwh = 1.0f; //Z;//1.0f / Z;
 }

 float DS = -0.5f;

 V[0].x   = p0.x;  	V[0].y   = p0.y;
 V[1].x   = p1.x;  	V[1].y   = p1.y;
 V[2].x   = p2.x;  	V[2].y   = p2.y;
 V[3].x   = p3.x;  	V[3].y   = p3.y;

  if (!TC)
  {
   V[0].tu = 0; 		V[0].tv = 0;
   V[1].tu = 1; 		V[1].tv = 0;
   V[2].tu = 1; 		V[2].tv = 1;
   V[3].tu = 0; 		V[3].tv = 1;
   V[0].tu2 = 0; 		V[0].tv2 = 0;
   V[1].tu2 = 1; 		V[1].tv2 = 0;
   V[2].tu2 = 1; 		V[2].tv2 = 1;
   V[3].tu2 = 0; 		V[3].tv2 = 1;
  }
  else
  {
   V[0].tu = TC[0]; 		V[0].tv = TC[1];
   V[1].tu = TC[2]; 		V[1].tv = TC[3];
   V[2].tu = TC[4]; 		V[2].tv = TC[5];
   V[3].tu = TC[6]; 		V[3].tv = TC[7];

   V[0].tu2 = TC[8]; 		V[0].tv2 = TC[9];
   V[1].tu2 = TC[10]; 		V[1].tv2 = TC[11];
   V[2].tu2 = TC[12]; 		V[2].tv2 = TC[13];
   V[3].tu2 = TC[14]; 		V[3].tv2 = TC[15];
  }

  r3dRenderer->SetTex(Tex);
  r3dRenderer->Render2DPolygon(4, V);
  r3dRenderer->Flush();
}

void r3dDrawBox2DWireframe(float x, float y, float w, float h, const r3dColor24& clr)
{
	r3dDrawLine2D ( x + 0, y + 0, x + w, y + 0, 2, clr );
	r3dDrawLine2D ( x + w, y + 0, x + w, y + h, 2, clr );
	r3dDrawLine2D ( x + w, y + h, x + 0, y + h, 2, clr );
	r3dDrawLine2D ( x + 0, y + h, x + 0, y + 0, 2, clr );

	r3dRenderer->Flush();
}

void r3dDrawBoxFS ( float w, float h, const r3dColor24& clr, r3dTexture *Tex /*=NULL*/, float fOffset/*=0.0f*/ )
{
	R3D_SCREEN_VERTEX 	V[4];

	int		i;

	for(i=0; i<4; i++)
	{
		V[i].color    = clr.GetPacked();
		V[i].z   = r3dRenderer->GetNearPlaneZValue();    //Z;
		V[i].rwh = 1.0f; //Z;//1.0f / Z;
	}

	float DS = fOffset;

	V[0].x   = 0.0f+DS; 	V[0].y   = 0.0f+DS;
	V[1].x   = w+DS;  		V[1].y   = 0.0f+DS;
	V[2].x   = w+DS;  		V[2].y   = h+DS;
	V[3].x   = 0.0f+DS; 	V[3].y   = h+DS;

	float sx = 0.5f / w;
	float sy = 0.5f / h;

	V[0].tu = 0 + sx; 	V[0].tv = 0 + sy;
	V[1].tu = 1 + sx; 	V[1].tv = 0 + sy;
	V[2].tu = 1 + sx; 	V[2].tv = 1 + sy;
	V[3].tu = 0 + sx; 	V[3].tv = 1 + sy;

	V[0].tu2 = 0; 		V[0].tv2 = 0;
	V[1].tu2 = 1; 		V[1].tv2 = 0;
	V[2].tu2 = 1; 		V[2].tv2 = 1;
	V[3].tu2 = 0; 		V[3].tv2 = 1;

	r3dRenderer->SetTex(Tex);
	r3dRenderer->Render2DPolygon(4, V);
	r3dRenderer->Flush();
}

void r3dDrawBox2DNoTex(float x, float y, float w, float h, const r3dColor24& clr, float *TC)
{
 R3D_SCREEN_VERTEX 	V[4];

 int		i;

 for(i=0; i<4; i++)
 {
  V[i].color    = clr.GetPacked();
  V[i].z   = r3dRenderer->GetNearPlaneZValue();    //Z;
  V[i].rwh = 1.0f; //Z;//1.0f / Z;
 }

 float DS = -0.5f;

 V[0].x   = x+DS;  	V[0].y   = y+DS;
 V[1].x   = x + w+DS;  	V[1].y   = y+DS;
 V[2].x   = x + w+DS;  	V[2].y   = y + h+DS;
 V[3].x   = x+DS;  	V[3].y   = y + h+DS;

  if (!TC)
  {
   V[0].tu = 0; 		V[0].tv = 0;
   V[1].tu = 1; 		V[1].tv = 0;
   V[2].tu = 1; 		V[2].tv = 1;
   V[3].tu = 0; 		V[3].tv = 1;
   V[0].tu2 = 0; 		V[0].tv2 = 0;
   V[1].tu2 = 1; 		V[1].tv2 = 0;
   V[2].tu2 = 1; 		V[2].tv2 = 1;
   V[3].tu2 = 0; 		V[3].tv2 = 1;
  }
  else
  {
   V[0].tu = TC[0]; 		V[0].tv = TC[1];
   V[1].tu = TC[2]; 		V[1].tv = TC[3];
   V[2].tu = TC[4]; 		V[2].tv = TC[5];
   V[3].tu = TC[6]; 		V[3].tv = TC[7];

   V[0].tu2 = TC[7]; 		V[0].tv2 = TC[8];
   V[1].tu2 = TC[9]; 		V[1].tv2 = TC[10];
   V[2].tu2 = TC[11]; 		V[2].tv2 = TC[12];
   V[3].tu2 = TC[13]; 		V[3].tv2 = TC[14];
  }

  r3dRenderer->Render2DPolygon(4, V);
  r3dRenderer->Flush();
}



void r3dDrawBox2DRotate(float center_x, float center_y, float w, float h, float fRot, const r3dColor24& clr, r3dTexture *Tex, float *TC)
{
  R3D_SCREEN_VERTEX 	V[4];


  for(int i=0; i<4; i++)
  {
    V[i].color  = clr.GetPacked();
    V[i].z      = r3dRenderer->GetNearPlaneZValue();    //Z;
    V[i].rwh    = 1.0f; //1.0f / Z;
  }

  float DS = -0.5f;
  V[0].x = -w/2.0f + DS; V[0].y = -h/2.0f + DS;
  V[1].x = +w/2.0f + DS; V[1].y = -h/2.0f + DS;
  V[2].x = +w/2.0f + DS; V[2].y = +h/2.0f + DS;
  V[3].x = -w/2.0f + DS; V[3].y = +h/2.0f + DS;
  
  float sin_y = sinf(R3D_DEG2RAD(fRot));
  float cos_y = cosf(R3D_DEG2RAD(fRot));
  for(int i=0; i<4; i++)
  {
    float Zt = V[i].y * cos_y - V[i].x * sin_y;
    float Xt = V[i].y * sin_y + V[i].x * cos_y;
    V[i].y = Zt + center_y;
    V[i].x = Xt + center_x;
  }
  
  if(!TC) 
  {
    V[0].tu  = 0;	V[0].tv  = 0;
    V[1].tu  = 1;	V[1].tv  = 0;
    V[2].tu  = 1;	V[2].tv  = 1;
    V[3].tu  = 0;	V[3].tv  = 1;
    V[0].tu2 = 0;	V[0].tv2 = 0;
    V[1].tu2 = 1;	V[1].tv2 = 0;
    V[2].tu2 = 1;	V[2].tv2 = 1;
    V[3].tu2 = 0;	V[3].tv2 = 1;
  }
  else
  {
    V[0].tu = TC[0]; 	V[0].tv = TC[1];
    V[1].tu = TC[2]; 	V[1].tv = TC[3];
    V[2].tu = TC[4]; 	V[2].tv = TC[5];
    V[3].tu = TC[6]; 	V[3].tv = TC[7];

    V[0].tu2 = TC[7]; 	V[0].tv2 = TC[8];
    V[1].tu2 = TC[9]; 	V[1].tv2 = TC[10];
    V[2].tu2 = TC[11]; 	V[2].tv2 = TC[12];
    V[3].tu2 = TC[13]; 	V[3].tv2 = TC[14];
  }

  r3dRenderer->SetTex(Tex);
  r3dRenderer->Render2DPolygon(4, V);
  r3dRenderer->Flush();
  
  return;
}


void r3dDrawBox2DZ(float x, float y, float w, float h, float z, const r3dColor24& clr, r3dTexture *Tex, float *TC)
{
  //NOTE: fix later = need to get this from the global camera
  const D3DXMATRIX& m = r3dRenderer->InvViewMatrix ;

  r3dPoint3D vCamPos(m._41, m._42, m._43);
  r3dPoint3D vCamForw(m._31, m._32, m._33);

  // create screen-aligned plane at speficied depth
  r3dPoint3D vDepth = vCamPos + vCamForw * z;
  D3DXPLANE pl;
  D3DXVECTOR3 vP(vDepth.x, vDepth.y, vDepth.z);
  D3DXVECTOR3 vN(-vCamForw.x, -vCamForw.y, -vCamForw.z);
  D3DXPlaneFromPointNormal(&pl, &vP, &vN);

  // find intersection of screen corners with depth plane  
  r3dPoint3D v1, v2, v3, v4;
  r3dPoint3D v1t, v2t, v3t, v4t;
  r3dPoint3D v1c, v2c, v3c, v4c;
  r3dScreenTo3D(x,   y,   &v1); v1t = vCamPos + v1 * 999999.0f;
  r3dScreenTo3D(x+w, y,   &v2); v2t = vCamPos + v2 * 999999.0f;
  r3dScreenTo3D(x+w, y+h, &v3); v3t = vCamPos + v3 * 999999.0f;
  r3dScreenTo3D(x,   y+h, &v4); v4t = vCamPos + v4 * 999999.0f;
  D3DXPlaneIntersectLine(v1c.d3dx(), &pl, vCamPos.d3dx(), v1t.d3dx());
  D3DXPlaneIntersectLine(v2c.d3dx(), &pl, vCamPos.d3dx(), v2t.d3dx());
  D3DXPlaneIntersectLine(v3c.d3dx(), &pl, vCamPos.d3dx(), v3t.d3dx());
  D3DXPlaneIntersectLine(v4c.d3dx(), &pl, vCamPos.d3dx(), v4t.d3dx());
 
  // render da poly
  R3D_DEBUG_VERTEX	V[4]; 

  for(int i=0; i<4; i++) {
    V[i].color   = clr;
  }

  V[0].Pos = v1c;
  V[1].Pos = v2c;
  V[2].Pos = v3c;
  V[3].Pos = v4c;

  if(!TC)
  {
    V[0].tu = 0; 		V[0].tv = 0;
    V[1].tu = 1; 		V[1].tv = 0;
    V[2].tu = 1; 		V[2].tv = 1;
    V[3].tu = 0; 		V[3].tv = 1;
  }
  else
  {
    V[0].tu = TC[0]; 		V[0].tv = TC[1];
    V[1].tu = TC[2]; 		V[1].tv = TC[3];
    V[2].tu = TC[4]; 		V[2].tv = TC[5];
    V[3].tu = TC[6]; 		V[3].tv = TC[7]; 
  }

//  r3dRenderer->SetTex(Tex);
  r3dRenderer->Render3DPolygon(4, V);
  r3dRenderer->Flush();
  
  return;
}



void r3dDrawBox2D1(float x, float y, float w, float h, const r3dColor24& clr, r3dTexture *Tex, float *TC)
{
 R3D_SCREEN_VERTEX 	V[4];
 float  	Z = r3dRenderer->NearClip;
 int		i;

 for(i=0; i<4; i++)
 {
  V[i].color    = clr.GetPacked();
  V[i].z  = Z;
  V[i].rwh = Z;//1.0f / Z;
 }

 float DS = -0.5f;

 V[0].x   = x+DS;  	V[0].y   = y+DS;
 V[1].x   = x + w+DS;  	V[1].y   = y+DS;
 V[2].x   = x + w+DS;  	V[2].y   = y + h+DS;
 V[3].x   = x+DS;  	V[3].y   = y + h+DS;

  if (!TC)
  {
   V[0].tu = 0; 		V[0].tv = 0;
   V[1].tu = 1; 		V[1].tv = 0;
   V[2].tu = 1; 		V[2].tv = 1;
   V[3].tu = 0; 		V[3].tv = 1;
   V[0].tu2 = 0; 		V[0].tv2 = 0;
   V[1].tu2 = 1; 		V[1].tv2 = 0;
   V[2].tu2 = 1; 		V[2].tv2 = 1;
   V[3].tu2 = 0; 		V[3].tv2 = 1;
  }
  else
  {
   V[0].tu = TC[0]; 		V[0].tv = TC[1];
   V[1].tu = TC[2]; 		V[1].tv = TC[3];
   V[2].tu = TC[4]; 		V[2].tv = TC[5];
   V[3].tu = TC[6]; 		V[3].tv = TC[7];
  }
  
  r3dRenderer->SetTex(Tex,1);
  r3dRenderer->Render2DPolygon(4, V);
  r3dRenderer->Flush();
}




void r3dDrawBox2DGradientV(float x, float y, float w, float h, const r3dColor24& TopClr, const r3dColor24& BottomClr, r3dTexture *Tex, float *TC)
{
 R3D_SCREEN_VERTEX 	V[4];
 float  	Z = r3dRenderer->NearClip;
 int		i;

 for(i=0; i<4; i++)
 {
  V[i].z  = Z;
  V[i].rwh = Z;//1.0f / Z;
 }

 V[0].color    = TopClr.GetPacked();
 V[1].color    = TopClr.GetPacked();
 V[2].color    = BottomClr.GetPacked();
 V[3].color    = BottomClr.GetPacked();

 float DS = -0.5f;

 V[0].x   = x+DS;  	V[0].y   = y+DS;
 V[1].x   = x + w+DS;  	V[1].y   = y+DS;
 V[2].x   = x + w+DS;  	V[2].y   = y + h+DS;
 V[3].x   = x+DS;  	V[3].y   = y + h+DS;

  if (!TC)
  {
   V[0].tu = 0; 		V[0].tv = 0;
   V[1].tu = 1; 		V[1].tv = 0;
   V[2].tu = 1; 		V[2].tv = 1;
   V[3].tu = 0; 		V[3].tv = 1;
  }
  else
  {
   V[0].tu = TC[0]; 		V[0].tv = TC[1];
   V[1].tu = TC[2]; 		V[1].tv = TC[3];
   V[2].tu = TC[4]; 		V[2].tv = TC[5];
   V[3].tu = TC[6]; 		V[3].tv = TC[7];
  }
  
  r3dRenderer->SetTex(Tex);
  r3dRenderer->Render2DPolygon(4, V);
  r3dRenderer->Flush();
}


void r3dDrawBox2DGradientH(float x, float y, float w, float h, const r3dColor24& TopClr, const r3dColor24& BottomClr, r3dTexture *Tex, float *TC)
{
 R3D_SCREEN_VERTEX 	V[4];
 float  	Z = r3dRenderer->NearClip;
 int		i;

 for(i=0; i<4; i++)
 {
  V[i].z  = Z;
  V[i].rwh = Z;//1.0f / Z;
 }

 V[0].color    = TopClr.GetPacked();
 V[3].color    = TopClr.GetPacked();
 V[1].color    = BottomClr.GetPacked();
 V[2].color    = BottomClr.GetPacked();

 float DS = -0.5f;

 V[0].x   = x+DS;  	V[0].y   = y+DS;
 V[1].x   = x + w+DS;  	V[1].y   = y+DS;
 V[2].x   = x + w+DS;  	V[2].y   = y + h+DS;
 V[3].x   = x+DS;  	V[3].y   = y + h+DS;

  if (!TC)
  {
   V[0].tu = 0; 		V[0].tv = 0;
   V[1].tu = 1; 		V[1].tv = 0;
   V[2].tu = 1; 		V[2].tv = 1;
   V[3].tu = 0; 		V[3].tv = 1;
  }
  else
  {
   V[0].tu = TC[0]; 		V[0].tv = TC[1];
   V[1].tu = TC[2]; 		V[1].tv = TC[3];
   V[2].tu = TC[4]; 		V[2].tv = TC[5];
   V[3].tu = TC[6]; 		V[3].tv = TC[7];
  }
  
  r3dRenderer->SetTex(Tex);
  r3dRenderer->Render2DPolygon(4, V);
  r3dRenderer->Flush();
}


void r3dDrawBox2DGradient(float x, float y, float w, float h, const r3dColor24& C1, const r3dColor24& C2, const r3dColor24& C3, const r3dColor24& C4, r3dTexture *Tex, float *TC)
{
 R3D_SCREEN_VERTEX 	V[4];
 float  	Z = r3dRenderer->NearClip;
 int		i;

 for(i=0; i<4; i++)
 {
  V[i].z  = Z;
  V[i].rwh = Z;//1.0f / Z;
 }

 V[0].color    = C1.GetPacked();
 V[1].color    = C2.GetPacked();
 V[2].color    = C3.GetPacked();
 V[3].color    = C4.GetPacked();

 float DS = -0.5f;

 V[0].x   = x+DS;  	V[0].y   = y+DS;
 V[1].x   = x + w+DS;  	V[1].y   = y+DS;
 V[2].x   = x + w+DS;  	V[2].y   = y + h+DS;
 V[3].x   = x+DS;  	V[3].y   = y + h+DS;

  if (!TC)
  {
   V[0].tu = 0; 		V[0].tv = 0;
   V[1].tu = 1; 		V[1].tv = 0;
   V[2].tu = 1; 		V[2].tv = 1;
   V[3].tu = 0; 		V[3].tv = 1;
  }
  else
  {
   V[0].tu = TC[0]; 		V[0].tv = TC[1];
   V[1].tu = TC[2]; 		V[1].tv = TC[3];
   V[2].tu = TC[4]; 		V[2].tv = TC[5];
   V[3].tu = TC[6]; 		V[3].tv = TC[7];
  }
  
  r3dRenderer->SetTex(Tex);
  r3dRenderer->Render2DPolygon(4, V);
  r3dRenderer->Flush();
}




void r3dDrawBox2DShadow(float x, float y, float w, float h, float sWidth, float bWidth, const r3dColor24& clr)
{
 r3dRenderer->SetRenderingMode ( R3D_BLEND_SUB );

 r3dDrawBox2DGradient(x-sWidth, y-sWidth, sWidth, sWidth, r3dColor24(0,0,0),r3dColor24(0,0,0),r3dColor24(255,255,255),r3dColor24(0,0,0));
 r3dDrawBox2DGradient(x+w, y-sWidth, sWidth, sWidth, r3dColor24(0,0,0),r3dColor24(0,0,0), r3dColor24(0,0,0),r3dColor24(255,255,255));
 r3dDrawBox2DGradient(x+w, y+h, sWidth, sWidth, r3dColor24(255,255,255),r3dColor24(0,0,0), r3dColor24(0,0,0),r3dColor24(0,0,0));
 r3dDrawBox2DGradient(x-sWidth, y+h, sWidth, sWidth, r3dColor24(0,0,0),r3dColor24(255,255,255), r3dColor24(0,0,0),r3dColor24(0,0,0));
 
 r3dDrawBox2DGradientV(x, y-sWidth, w,sWidth, r3dColor24(0,0,0),r3dColor24(255,255,255));
 r3dDrawBox2DGradientV(x, y+h, w,sWidth, r3dColor24(255,255,255), r3dColor24(0,0,0));
 r3dDrawBox2DGradientH(x-sWidth, y, sWidth,h, r3dColor24(0,0,0),r3dColor24(255,255,255)); 
 r3dDrawBox2DGradientH(x+w, y, sWidth,h, r3dColor24(255,255,255), r3dColor24(0,0,0)); 

 r3dRenderer->SetRenderingMode ( R3D_BLEND_ALPHA );
 r3dDrawBox2D(x,y,w,h,clr);

 r3dRenderer->Flush();
 r3dRenderer->SetRenderingMode ( R3D_BLEND_COPY_ZCW );

 if (bWidth)
 {
  r3dDrawBox2D(x,y,w,bWidth, r3dColor24(255,255,255));
  r3dDrawBox2D(x,y+h-bWidth,w,bWidth, r3dColor24(255,255,255));
  r3dDrawBox2D(x,y,bWidth,h, r3dColor24(255,255,255));
  r3dDrawBox2D(x+w-bWidth,y,bWidth,h, r3dColor24(255,255,255));
 }
 
}

void r3dDrawTextInBox(float x, float y, float w, float h, CD3DFont* pFont, CHAR* pText, r3dColor24 clr, int Align, int Margin)
{
SIZE ts;
int dx=0, dy;
	pFont->GetTextExtent(pText, &ts);
	switch(Align) {
	case UTIL_TEXT_ALIGN_LEFT:
		dx = Margin;
		break;
	case UTIL_TEXT_ALIGN_CENTER:
		dx = int((w-ts.cx)/2);
		break;
	case UTIL_TEXT_ALIGN_RIGHT:
		dx = int(w - ts.cx - Margin);
		break;
	}
	dy = int((h-ts.cy)/2);
	pFont->PrintF(x+dx,y+dy, clr, pText);
}

void r3dDrawCircle2D(const r3dPoint2D& pos, float radius, const r3dColor& clr1, const r3dColor& clr2)
{
  const int iters = 20;
  
  if(radius < 0.001f)
    return;
  
  r3dPoint2D fv[iters+1];

  for(int i=0; i<iters; i++) {
    fv[i].x = pos.x + sinf(R3D_PI*2 * float(i) / float(iters)) * radius;
    fv[i].y = pos.y + cosf(R3D_PI*2 * float(i) / float(iters)) * radius;
  }

  R3D_SCREEN_VERTEX V[3];
  V[0].tu  = 0; V[0].tv = 0;
  V[1].tu  = 1; V[1].tv = 0;
  V[2].tu  = 1; V[2].tv = 1;

  V[0].x = pos.x;
  V[0].y = pos.y;
  V[0].z = r3dRenderer->GetNearPlaneZValue();
  V[1].z = r3dRenderer->GetNearPlaneZValue();
  V[2].z = r3dRenderer->GetNearPlaneZValue();

  V[0].rwh = 1.0f;
  V[1].rwh = 1.0f;
  V[2].rwh = 1.0f;

  V[0].color = clr1.GetPacked();
  V[1].color = clr2.GetPacked();
  V[2].color = clr2.GetPacked();

  for(int i=1; i<iters; i++) {
    V[2].x  = fv[i-1].x;
	V[2].y  = fv[i-1].y;

	V[1].x  = fv[i].x;
	V[1].y  = fv[i].y;
    r3dRenderer->Render2DPolygon(3, V);
  }

  V[2].x = fv[iters-1].x;
  V[2].y = fv[iters-1].y;
  V[1].x = fv[0].x;
  V[1].y = fv[0].y;


  r3dRenderer->Render2DPolygon(3, V);
  
  r3dRenderer->Flush();
}


void r3dDrawCylinder ( const r3dCylinder & cylinder, const r3dCamera &Cam, r3dColor tColor, DWORD dwNumSegments )
{
	r3d_assert ( dwNumSegments > 0 && dwNumSegments < 256 );

	r3dDrawUniformLine3D(cylinder.vBase, cylinder.vBase + cylinder.vAltitude, Cam, r3dColor::yellow );
	
	D3DXMATRIX mTrans;
	D3DXMatrixTranslation ( &mTrans, cylinder.vBase.x, cylinder.vBase.y, cylinder.vBase.z );

	D3DXMATRIX mRot;
	D3DXMatrixIdentity ( &mRot );

	r3dVector vNormal;
	D3DXVec3Normalize ( vNormal.d3dx(), cylinder.vAltitude.d3dx() );

	r3dVector v3 = vNormal;
	r3dVector v2 = r3dVector (vNormal.z,0, -vNormal.x );
	r3dVector v1;

	if ( vNormal.z * vNormal.z + vNormal.x * vNormal.x < FLT_EPSILON )
	{
		v2.x = 1.0f;
		v2.y = 0.0f;
		v2.z = 0.0f;
	}

	D3DXVec3Cross ( v1.d3dx(), v2.d3dx(), v3.d3dx() );

	D3DXVec3Normalize ( v1.d3dx(), v1.d3dx() );
	D3DXVec3Normalize ( v2.d3dx(), v2.d3dx() );
	D3DXVec3Normalize ( v3.d3dx(), v3.d3dx() );

	mRot ( 0, 0 ) = v1.x;
	mRot ( 0, 1 ) = v1.y;
	mRot ( 0, 2 ) = v1.z;

	mRot ( 1, 0 ) = v2.x;
	mRot ( 1, 1 ) = v2.y;
	mRot ( 1, 2 ) = v2.z;

	mRot ( 2, 0 ) = v3.x;
	mRot ( 2, 1 ) = v3.y;
	mRot ( 2, 2 ) = v3.z;

	mRot *= mTrans;

	D3DXMATRIX mOldTransform;
#ifndef FINAL_BUILD
	r3dRenderer->pd3ddev->GetTransform ( D3DTS_WORLD, &mOldTransform );
	r3dRenderer->pd3ddev->SetTransform ( D3DTS_WORLD, &mRot );
#endif

	float fHeight = cylinder.vAltitude.Length();

	FLOAT fDeltaSegAngle  = ( 2.0f * D3DX_PI / dwNumSegments );

	// Generate the group of segments for the current ring
	for( DWORD seg = 0; seg < (dwNumSegments+1); seg++ )
	{
		FLOAT x0 =  cylinder.fRadius * cosf( seg * fDeltaSegAngle );
		FLOAT y0 =  cylinder.fRadius * sinf( seg * fDeltaSegAngle );

		FLOAT x1 =  cylinder.fRadius * cosf( ( seg + 1 ) * fDeltaSegAngle );
		FLOAT y1 =  cylinder.fRadius * sinf( ( seg + 1 ) * fDeltaSegAngle );

		r3dDrawUniformLine3D(r3dVector(x0, y0, 0), r3dVector(x1, y1, 0), Cam, tColor );
		r3dDrawUniformLine3D(r3dVector(x0, y0, fHeight), r3dVector(x1, y1, fHeight), Cam, tColor );
		r3dDrawUniformLine3D(r3dVector(x1, y1, 0), r3dVector(x1, y1, fHeight), Cam, tColor );
	}

	r3dRenderer->Flush ();

	r3dRenderer->pd3ddev->SetTransform ( D3DTS_WORLD, &mOldTransform );
}

void r3dDrawCylinderSolid( const r3dCylinder & cylinder, const r3dCamera &Cam, r3dColor tColor, DWORD dwNumSegments/* = 16 */, bool bIncludeOutline/* = false */, r3dColor24 const & clr2/* = r3dColor24::white */ )
{
	r3d_assert ( dwNumSegments > 0 && dwNumSegments < 256 );

	r3dDrawUniformLine3D(cylinder.vBase, cylinder.vBase + cylinder.vAltitude, Cam, r3dColor::yellow );
	
	D3DXMATRIX mTrans;
	D3DXMatrixTranslation ( &mTrans, cylinder.vBase.x, cylinder.vBase.y, cylinder.vBase.z );

	D3DXMATRIX mRot;
	D3DXMatrixIdentity ( &mRot );

	r3dVector vNormal;
	D3DXVec3Normalize ( vNormal.d3dx(), cylinder.vAltitude.d3dx() );

	r3dVector v3 = vNormal;
	r3dVector v2 = r3dVector (vNormal.z,0, -vNormal.x );
	r3dVector v1;

	if ( vNormal.z * vNormal.z + vNormal.x * vNormal.x < FLT_EPSILON )
	{
		v2.x = 1.0f;
		v2.y = 0.0f;
		v2.z = 0.0f;
	}

	D3DXVec3Cross ( v1.d3dx(), v2.d3dx(), v3.d3dx() );

	D3DXVec3Normalize ( v1.d3dx(), v1.d3dx() );
	D3DXVec3Normalize ( v2.d3dx(), v2.d3dx() );
	D3DXVec3Normalize ( v3.d3dx(), v3.d3dx() );

	mRot ( 0, 0 ) = v1.x;
	mRot ( 0, 1 ) = v1.y;
	mRot ( 0, 2 ) = v1.z;

	mRot ( 1, 0 ) = v2.x;
	mRot ( 1, 1 ) = v2.y;
	mRot ( 1, 2 ) = v2.z;

	mRot ( 2, 0 ) = v3.x;
	mRot ( 2, 1 ) = v3.y;
	mRot ( 2, 2 ) = v3.z;

	mRot *= mTrans;

	D3DXMATRIX mOldTransform;
#ifndef FINAL_BUILD
	r3dRenderer->pd3ddev->GetTransform ( D3DTS_WORLD, &mOldTransform );
	r3dRenderer->pd3ddev->SetTransform ( D3DTS_WORLD, &mRot );
#endif

	float fHeight = cylinder.vAltitude.Length();

	FLOAT fDeltaSegAngle  = ( 2.0f * D3DX_PI / dwNumSegments );

	// Generate the group of segments for the current ring
	for( DWORD seg = 0; seg < (dwNumSegments+1); seg++ )
	{
		FLOAT x0 =  cylinder.fRadius * cosf( seg * fDeltaSegAngle );
		FLOAT y0 =  cylinder.fRadius * sinf( seg * fDeltaSegAngle );

		FLOAT x1 =  cylinder.fRadius * cosf( ( seg + 1 ) * fDeltaSegAngle );
		FLOAT y1 =  cylinder.fRadius * sinf( ( seg + 1 ) * fDeltaSegAngle );

		r3dVector vp0(x0, y0, 0);
		r3dVector vp1(x1, y1, 0);
		r3dVector vp2(x0, y0, fHeight);
		r3dVector vp3(x1, y1, fHeight);
		r3dVector vp4(0, 0, 0);
		r3dVector vp5(0, 0, fHeight);

		// Side
		r3dDrawTriangle3D ( vp0, vp1, vp2, Cam, tColor );
		r3dDrawTriangle3D ( vp2, vp1, vp3, Cam, tColor );
		// Top
		r3dDrawTriangle3D ( vp5, vp2, vp3, Cam, tColor );
		// Bottom
		r3dDrawTriangle3D ( vp4, vp1, vp0, Cam, tColor );

		if( bIncludeOutline )
		{
			// Side
			r3dDrawUniformLine3D(vp0, vp1, Cam, clr2 );
			r3dDrawUniformLine3D(vp2, vp3, Cam, clr2 );
			r3dDrawUniformLine3D(vp1, vp3, Cam, clr2 );

			// Top
			r3dDrawUniformLine3D(vp5, vp2, Cam, clr2 );
			r3dDrawUniformLine3D(vp2, vp3, Cam, clr2 );
			r3dDrawUniformLine3D(vp3, vp5, Cam, clr2 );

			// Bottom
			r3dDrawUniformLine3D(vp4, vp1, Cam, clr2 );
			r3dDrawUniformLine3D(vp1, vp0, Cam, clr2 );
			r3dDrawUniformLine3D(vp0, vp4, Cam, clr2 );
		}
	}

	r3dRenderer->Flush ();

	r3dRenderer->pd3ddev->SetTransform ( D3DTS_WORLD, &mOldTransform );
}


void r3dDrawConeSolid ( const r3dCone & tCone, r3dCamera &Cam, r3dColor tColor, float fLen, int iSamples )
{
	// form a basis in plance perpendicular to direction
	r3dVector v1, v2;

	v1 = ( fabs ( tCone.vDir.z ) < 0.999f )
		? r3dVector ( tCone.vDir.y, -tCone.vDir.x, 0 )
		: r3dVector ( 1, 0, 0 );
	D3DXVec3Normalize( v1.d3dx(), v1.d3dx() );
	D3DXVec3Cross ( v2.d3dx(), v1.d3dx(), tCone.vDir.d3dx() );
	D3DXVec3Normalize( v2.d3dx(), v2.d3dx() );

	r3d_assert ( "[DrawConeSolid]" && v1.Dot( tCone.vDir ) < 0.1f );
	r3d_assert ( "[DrawConeSolid]" && v2.Dot ( tCone.vDir ) < 0.1f );
	r3d_assert ( "[DrawConeSolid]" && v1.Dot( v2 ) < 0.1f );

	// calc radius in that plane
	float r = tanf ( tCone.fFOV / 2.0f );
	v1 *= r;
	v2 *= r;

	// do some angles
	float a = 0.0f;
	float da = 2*R3D_PI / float(iSamples);

	for (int i = 0; i < iSamples; i++, a += da)
	{
		r3dVector vp1 = tCone.vCenter;
		r3dVector vp2 = tCone.vCenter + fLen* ( tCone.vDir + v1*cosf(a) + v2*sinf (a) );
		r3dVector vp3 = tCone.vCenter + fLen* ( tCone.vDir + v1*cosf(a+da) + v2*sinf(a+da ) );
		r3dDrawTriangle3D ( vp1, vp2, vp3, Cam, tColor );
	}

	r3dRenderer->Flush ();
}

void r3dDrawCone2D(const r3dPoint3D& pos, float radius, float angle, const r3dPoint3D& dir, const r3dColor& clr1, const r3dColor& clr2)
{
  const int iters = 10;
  
  if(radius < 0.001f)
    return;
  
  r3dPoint3D fv[iters+1];
  
  float startangle = atan2(dir.x, dir.z);
  float rmin = -R3D_DEG2RAD(angle) + startangle;
  float rmax = R3D_DEG2RAD(angle)  + startangle;

  for(int i=0; i<iters; i++) {
    float k = float(i) / float(iters);
    float a = rmin + (rmax - rmin) * k;
    fv[i].y = pos.y;
    fv[i].x = pos.x + sinf(a) * radius;
    fv[i].z = pos.z + cosf(a) * radius;
  }

  R3D_DEBUG_VERTEX V[3];
  V[0].tu  = 0; V[0].tv = 0;
  V[1].tu  = 1; V[1].tv = 0;
  V[2].tu  = 1; V[2].tv = 1;

  V[0].Pos  = pos;
  V[0].color = clr1;
  V[1].color = clr2;
  V[2].color = clr2;

  for(int i=1; i<iters; i++) {
    V[1].Pos  = fv[i-1];
    V[2].Pos  = fv[i];
    r3dRenderer->Render3DPolygon(3, V);
  }

  V[1].Pos  = fv[iters-1];
  V[2].Pos  = fv[0];
  R3D_DEBUG_VERTEX::ComputeNormals(V, 3);
  r3dRenderer->Render3DPolygon(3, V);
  
  r3dRenderer->Flush();
}

//////////////////////////////////////////////////////////////////////////

void r3dDrawTriangleList(R3D_SCREEN_VERTEX *vertices, int num)
{
	r3dRenderer->Render2DPolygon(num, vertices);
	r3dRenderer->Flush();
}

//------------------------------------------------------------------------


void r3dDrawOrientedBoundBox( const r3dBoundBox &Box, const r3dPoint3D& rotation, const r3dCamera &Cam, const r3dColor24& clr, float fLineWidth /*= 0.1f */ )
{
	r3dPoint3D points[8];
	r3dGenerateOrientedBoundBoxPoints( points, 8, Box, rotation );

	r3dDrawLine3D( points[0], points[1], Cam, fLineWidth, clr);
	r3dDrawLine3D( points[1], points[2], Cam, fLineWidth, clr);
	r3dDrawLine3D( points[2], points[3], Cam, fLineWidth, clr);
	r3dDrawLine3D( points[3], points[0], Cam, fLineWidth, clr);
	
	r3dDrawLine3D( points[4], points[5], Cam, fLineWidth, clr);
	r3dDrawLine3D( points[5], points[6], Cam, fLineWidth, clr);
	r3dDrawLine3D( points[6], points[7], Cam, fLineWidth, clr);
	r3dDrawLine3D( points[7], points[4], Cam, fLineWidth, clr);

	r3dDrawLine3D( points[0], points[4], Cam, fLineWidth, clr);
	r3dDrawLine3D( points[1], points[5], Cam, fLineWidth, clr);
	r3dDrawLine3D( points[2], points[6], Cam, fLineWidth, clr);
	r3dDrawLine3D( points[3], points[7], Cam, fLineWidth, clr);
	
	r3dRenderer->Flush();
	return;
}
void r3dDrawOrientedBoundBoxSolid( const r3dBoundBox &Box, const r3dPoint3D& rotation, const r3dCamera &Cam, const r3dColor24& clr, float fLineWidth /*= 0.1f */, bool bIncludeOutline/* = false */, r3dColor24 const & clr2/* = r3dColor24::white */, CD3DFont* font/* = NULL */ )
{
	r3dPoint3D points[8];
	r3dGenerateOrientedBoundBoxPoints( points, 8, Box, rotation );

	r3dDrawTriangle3D( points[1], points[0], points[3], Cam, clr, NULL, NULL, true );
	r3dDrawTriangle3D( points[2], points[1], points[3], Cam, clr, NULL, NULL, true );
	r3dDrawTriangle3D( points[3], points[0], points[4], Cam, clr, NULL, NULL, true );
	r3dDrawTriangle3D( points[4], points[7], points[3], Cam, clr, NULL, NULL, true );
	r3dDrawTriangle3D( points[3], points[7], points[6], Cam, clr, NULL, NULL, true );
	r3dDrawTriangle3D( points[6], points[2], points[3], Cam, clr, NULL, NULL, true );
	r3dDrawTriangle3D( points[1], points[2], points[6], Cam, clr, NULL, NULL, true );
	r3dDrawTriangle3D( points[6], points[5], points[1], Cam, clr, NULL, NULL, true );
	r3dDrawTriangle3D( points[1], points[5], points[4], Cam, clr, NULL, NULL, true );
	r3dDrawTriangle3D( points[1], points[4], points[0], Cam, clr, NULL, NULL, true );
	r3dDrawTriangle3D( points[4], points[5], points[6], Cam, clr, NULL, NULL, true );
	r3dDrawTriangle3D( points[6], points[7], points[4], Cam, clr, NULL, NULL, true );

	if( bIncludeOutline )
	{
		r3dDrawLine3D( points[0], points[1], Cam, fLineWidth, clr2);
		r3dDrawLine3D( points[1], points[2], Cam, fLineWidth, clr2);
		r3dDrawLine3D( points[2], points[3], Cam, fLineWidth, clr2);
		r3dDrawLine3D( points[3], points[0], Cam, fLineWidth, clr2);
		
		r3dDrawLine3D( points[4], points[5], Cam, fLineWidth, clr2);
		r3dDrawLine3D( points[5], points[6], Cam, fLineWidth, clr2);
		r3dDrawLine3D( points[6], points[7], Cam, fLineWidth, clr2);
		r3dDrawLine3D( points[7], points[4], Cam, fLineWidth, clr2);

		r3dDrawLine3D( points[0], points[4], Cam, fLineWidth, clr2);
		r3dDrawLine3D( points[1], points[5], Cam, fLineWidth, clr2);
		r3dDrawLine3D( points[2], points[6], Cam, fLineWidth, clr2);
		r3dDrawLine3D( points[3], points[7], Cam, fLineWidth, clr2);
	}
	
	if( font )
	{
		for(uint32_t i = 0; i < 8; ++i)
		{
			r3dPoint3D labelPos;
			if (r3dProjectToScreen(points[i], &labelPos))
			{
				font->PrintF(labelPos.x, labelPos.y, r3dColor(255, 255, 255), "%d", i);
			}
		}
	}

	r3dRenderer->Flush();
	return;
}


void r3dDrawOrientedPlaneSolid( const r3dBoundBox &Box, const r3dPoint3D& rotation, const r3dCamera &Cam, const r3dColor24& clr, float fLineWidth /*= 0.1f */, bool bIncludeOutline/* = false */, r3dColor24 const & clr2/* = r3dColor24::white */, CD3DFont* font/* = NULL */ )
{
	r3dPoint3D points[4];
	r3dGenerateOrientedPlanePoints( points, 4, Box, rotation );

	r3dDrawTriangle3D( points[2], points[1], points[0], Cam, clr, NULL, NULL, true );
	r3dDrawTriangle3D( points[1], points[2], points[3], Cam, clr, NULL, NULL, true );

	r3dDrawTriangle3D( points[0], points[1], points[2], Cam, clr, NULL, NULL, true );
	r3dDrawTriangle3D( points[3], points[2], points[1], Cam, clr, NULL, NULL, true );

	if( bIncludeOutline )
	{
		r3dDrawLine3D( points[0], points[1], Cam, fLineWidth, clr2);
		r3dDrawLine3D( points[1], points[2], Cam, fLineWidth, clr2);
		r3dDrawLine3D( points[2], points[0], Cam, fLineWidth, clr2);		

		r3dDrawLine3D( points[2], points[3], Cam, fLineWidth, clr2);
		r3dDrawLine3D( points[3], points[1], Cam, fLineWidth, clr2);
	}

	if( font )
	{
		for(uint32_t i = 0; i < 4; ++i)
		{
			r3dPoint3D labelPos;
			if (r3dProjectToScreen(points[i], &labelPos))
			{
				font->PrintF(labelPos.x, labelPos.y, r3dColor(255, 255, 255), "%d", i);
			}
		}
	}

	r3dRenderer->Flush();
	return;
}
