#include "r3dPCH.h"
#include "r3d.h"


static const size_t BOX_VERTEX_COUNT		= 36;
static const size_t WIRE_BOX_VERTEX_COUNT	= 24;
static R3D_WORLD_VERTEX g_tBoxVertexes [ BOX_VERTEX_COUNT > WIRE_BOX_VERTEX_COUNT ? BOX_VERTEX_COUNT : WIRE_BOX_VERTEX_COUNT ];

extern r3dCamera gCam;

//------------------------------------------------------------------------

void DrawDeferredBox3D ( float x, float y, float z, float w, float h, float d, const r3dColor24& clr, bool wireframe )
{
	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW);

	d3dc._SetDecl(R3D_WORLD_VERTEX::getDecl());
	
	struct RestoreShaders
	{
		RestoreShaders()
		{
			psId = r3dRenderer->GetCurrentPixelShaderIdx();
			vsId = r3dRenderer->GetCurrentVertexShaderIdx();
		}

		~RestoreShaders()
		{
			r3dRenderer->SetPixelShader( psId );
			r3dRenderer->SetVertexShader( vsId );
		}

		int vsId;
		int psId;
	} restoreShaders; (void)restoreShaders;

	r3dRenderer->SetVertexShader("VS_DRAW3D_DEFERRED");
	r3dRenderer->SetPixelShader("PS_DRAW3D_DEFERRED");

	// box color
	D3DXVECTOR4 vconst = D3DXVECTOR4 ( clr.R, clr.G, clr.B, clr.A );
	vconst /= 255.0f;

	// float4 vColor : register( c64 );
	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 64, (float*)&vconst, 1 ) );

	D3DXMATRIX world;
	D3DXMatrixIdentity( &world );

	r3dMeshSetShaderConsts( world, NULL );

	for( int i=0, e = sizeof g_tBoxVertexes / sizeof g_tBoxVertexes[0]; i < e; i++ )
	{
		g_tBoxVertexes[i].tu    = g_tBoxVertexes[i].tv  = 0;
		g_tBoxVertexes[i].vU    = g_tBoxVertexes[i].vV  = r3dVector( 0, 0, 0 );
		g_tBoxVertexes[i].Pos   = r3dPoint3D( x, y, z );
	}

	float hw = w * 0.5f;
	float hh = h * 0.5f;
	float hd = d * 0.5f;

	if( wireframe )
	{
		for( size_t ii = 0, e = WIRE_BOX_VERTEX_COUNT ; ii < e; ii ++ )
		{
			g_tBoxVertexes[ ii ].Normal = -gCam.vPointTo;
		}

		size_t i = 0;

		// face 0
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( -hw, -hh, -hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( +hw, -hh, -hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( +hw, -hh, -hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( +hw, +hh, -hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( +hw, +hh, -hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( -hw, +hh, -hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( -hw, +hh, -hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( -hw, -hh, -hd );

		// face 1
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( -hw, -hh, +hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( +hw, -hh, +hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( +hw, -hh, +hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( +hw, +hh, +hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( +hw, +hh, +hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( -hw, +hh, +hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( -hw, +hh, +hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( -hw, -hh, +hd );

		// face 2, 3, 4, 5
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( -hw, -hh, -hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( -hw, -hh, +hd );

		g_tBoxVertexes[i++].Pos   += r3dPoint3D( +hw, -hh, -hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( +hw, -hh, +hd );

		g_tBoxVertexes[i++].Pos   += r3dPoint3D( +hw, +hh, -hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( +hw, +hh, +hd );

		g_tBoxVertexes[i++].Pos   += r3dPoint3D( -hw, +hh, -hd );
		g_tBoxVertexes[i++].Pos   += r3dPoint3D( -hw, +hh, +hd );

		assert( i == WIRE_BOX_VERTEX_COUNT );

		r3dRenderer->DrawUP( D3DPT_LINELIST, WIRE_BOX_VERTEX_COUNT / 2, g_tBoxVertexes, sizeof(g_tBoxVertexes[0]) );
	}
	else
	{
		size_t i = 0;

		// face 0
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, -hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +0, -1 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, -hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +0, -1 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, +hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +0, -1 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, +hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +0, -1 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, +hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +0, -1 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, -hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +0, -1 );

		// face 1
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, -hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +0, +1 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, -hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +0, +1 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, +hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +0, +1 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, +hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +0, +1 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, +hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +0, +1 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, -hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +0, +1 );

		// face 2
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, -hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, -1, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, -hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, -1, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, -hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, -1, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, -hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, -1, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, -hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, -1, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, -hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, -1, +0 );

		// face 3
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, +hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +1, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, +hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +1, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, +hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +1, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, +hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +1, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, +hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +1, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, +hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +0, +1, +0 );

		// face 4
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, +hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( -1, +0, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, -hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( -1, +0, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, -hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( -1, +0, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, -hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( -1, +0, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, +hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( -1, +0, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( -hw, +hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( -1, +0, +0 );

		// face 5
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, -hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +1, +0, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, +hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +1, +0, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, -hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +1, +0, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, +hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +1, +0, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, -hh, +hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +1, +0, +0 );
		g_tBoxVertexes[i].Pos   += r3dPoint3D( +hw, +hh, -hd );   g_tBoxVertexes[i++].Normal   = r3dPoint3D( +1, +0, +0 );

		assert( i == BOX_VERTEX_COUNT );

		r3dRenderer->DrawUP(D3DPT_TRIANGLELIST, BOX_VERTEX_COUNT / 3, g_tBoxVertexes, sizeof(g_tBoxVertexes[0]));
	}
}

//------------------------------------------------------------------------

void DrawDeferredBox3D ( r3dBox3D BBox, const r3dColor24& clr, bool wireframe )
{	
	r3dPoint3D vOrg = BBox.Org + BBox.Size * 0.5f;
	DrawDeferredBox3D( vOrg.x, vOrg.y, vOrg.z, BBox.Size.x, BBox.Size.y, BBox.Size.z, clr, wireframe ); 
}