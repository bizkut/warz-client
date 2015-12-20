#include "r3dPCH.h"
#ifndef FINAL_BUILD
#include "r3d.h"

#include "GameLevel.h"

#include "GameObjects/ObjManag.h"

#include "VisibilityGrid.h"

extern class r3dITerrain* Terrain ;

VisibiltyGrid::VisibiltyGrid()
: m_Width ( 0 )
, m_Height ( 0 )
, m_Depth ( 0 )
, m_CellCountX ( 0 )
, m_CellCountY ( 0 )
, m_CellCountZ ( 0 )
, m_Start( 0, 0, 0 )
, m_DesiredCellHeight( 0.f )
, m_IsInited( 0 )
{

}

//------------------------------------------------------------------------

VisibiltyGrid::~VisibiltyGrid()
{

}

//------------------------------------------------------------------------

void
VisibiltyGrid::Init( int CellCountX, int CellCountY, int CellCountZ, float Width, float Height, float Depth, float DesiredCellHeight, r3dPoint3D Start, bool NeedBuildHeihgts )
{
	int totalCount = CellCountX * CellCountY * CellCountZ ;

	m_VisibilityCells.Resize( totalCount * totalCount ) ;
	m_VisibilityCells.ZeroAll();

	m_CellCountX	= CellCountX ;
	m_CellCountY	= CellCountY ;
	m_CellCountZ	= CellCountZ ;

	m_CellWidth		= Width / CellCountX ;
	m_CellHeight	= Height / CellCountY ;
	m_CellDepth		= Depth / CellCountZ ;

	m_Width			= Width ;
	m_Height		= Height ;
	m_Depth			= Depth ;

	m_Start			= Start ;

	// y is absolute...
	m_Start.y		= 0 ;

	m_IsInited		= 1 ;

	if( NeedBuildHeihgts )
	{
		BuildCellHeights( DesiredCellHeight );
	}
}

//------------------------------------------------------------------------

R3D_FORCEINLINE
int
VisibiltyGrid::GetCellPairIDX( int cx0, int cy0, int cz0, int cx1, int cy1, int cz1 ) const
{
	int idx0 = cx0 + cy0 * m_CellCountX * m_CellCountZ + cz0 * m_CellCountX ;
	int idx1 = cx1 + cy1 * m_CellCountX * m_CellCountZ + cz1 * m_CellCountX ;
	
	return idx0 + idx1 * m_CellCountX * m_CellCountY * m_CellCountZ ;
}

//------------------------------------------------------------------------

int
VisibiltyGrid::GetVisibilityAt( int cx0, int cy0, int cz0, int cx1, int cy1, int cz1 ) const
{
	return m_VisibilityCells[ GetCellPairIDX( cx0, cy0, cz0, cx1, cy1, cz1 ) ] ;
}

//------------------------------------------------------------------------
/*static*/

int
VisibiltyGrid::IsVisGridObj( GameObject* obj )
{
	return obj->isObjType( OBJTYPE_Mesh );
}

//------------------------------------------------------------------------

void
VisibiltyGrid::GetCellCoords( const r3dPoint3D& pos, int * oCX, int* oCY, int * oCZ ) const
{
	r3dPoint3D lpos = pos - m_Start ;

	int xx = (int) ( lpos.x / m_CellWidth ) ;
	int zz = (int) ( lpos.z / m_CellDepth ) ;

	xx = R3D_MIN( R3D_MAX( xx, 0 ), m_CellCountX - 1 ) ;
	zz = R3D_MIN( R3D_MAX( zz, 0 ), m_CellCountZ - 1 ) ;

	const HeightRange& hr = m_CellHeightRanges[ xx + zz * m_CellCountX ] ;

	float height = ( hr.Max - hr.Min ) / hr.CellCount ;

	int yy = R3D_MIN( R3D_MAX( int( ( lpos.y - hr.Min ) / height ), 0 ),  hr.CellCount - 1 ) ;

	*oCX = xx ;
	*oCY = yy ;
	*oCZ = zz ;
}

//------------------------------------------------------------------------

R3D_FORCEINLINE
void
VisibiltyGrid::SetupCell( r3dPoint3D ( &vertices ) [ 8 ], r3dBoundBox* box, const HeightRange* hr, int cx, int cy, int cz )
{
	float fullHeight = ( hr->Max - hr->Min ) ;
	float height = fullHeight / hr->CellCount ;

	float halfWidth		= m_CellWidth * 0.5f ;
	float halfHeight	= height * 0.5f ;
	float halfDepth		= m_CellDepth * 0.5f ;

	float x = cx * m_CellWidth + halfWidth ;
	float y = cy * height + halfHeight + hr->Min ;
	float z = cz * m_CellDepth + halfDepth ;
	
	vertices[ 0 ] = r3dPoint3D( x + halfWidth , y + halfHeight , z + halfDepth ) + m_Start ;
	vertices[ 1 ] = r3dPoint3D( x - halfWidth , y + halfHeight , z + halfDepth ) + m_Start ;
	vertices[ 2 ] = r3dPoint3D( x + halfWidth , y - halfHeight , z + halfDepth ) + m_Start ;
	vertices[ 3 ] = r3dPoint3D( x - halfWidth , y - halfHeight , z + halfDepth ) + m_Start ;
	vertices[ 4 ] = r3dPoint3D( x + halfWidth , y + halfHeight , z - halfDepth ) + m_Start ;
	vertices[ 5 ] = r3dPoint3D( x - halfWidth , y + halfHeight , z - halfDepth ) + m_Start ;
	vertices[ 6 ] = r3dPoint3D( x + halfWidth , y - halfHeight , z - halfDepth ) + m_Start ;
	vertices[ 7 ] = r3dPoint3D( x - halfWidth , y - halfHeight , z - halfDepth ) + m_Start ;

	box->Org = r3dPoint3D( x - halfWidth, y - halfHeight, z - halfDepth ) + m_Start ;
	box->Size = r3dPoint3D( m_CellWidth, fullHeight, m_CellDepth ) ;
}

//------------------------------------------------------------------------

void PushDebugBox(	r3dPoint3D p0, r3dPoint3D p1, r3dPoint3D p2, r3dPoint3D p3,
					r3dPoint3D p4, r3dPoint3D p5, r3dPoint3D p6, r3dPoint3D p7,
					r3dColor color ) ;

void
VisibiltyGrid::DebugCells() const
{
	enum Path
	{
		INVISIBLE,
		VISIBLE,
		SELECTED,
		PATH_COUNT
	};

	r3dColor colors [ 3 ] = { r3dColor::red, r3dColor::green, r3dColor::yellow } ;

	for( int path = INVISIBLE, e = r_debug_vis_grid_cell->GetInt() ? PATH_COUNT : INVISIBLE + 1 ; path < e ; path ++ )
	{
		for( int j = 0 ; j < m_CellCountZ ; j ++ )
		{
			float cellMiZ = ( j + 0 ) * m_CellDepth + m_Start.z ;
			float cellMaZ = ( j + 1 ) * m_CellDepth + m_Start.z ;

			for( int i = 0 ; i < m_CellCountX ; i ++ )
			{
				float cellMiX = ( i + 0 ) * m_CellWidth + m_Start.x ;
				float cellMaX = ( i + 1 ) * m_CellWidth + m_Start.x ;

				const HeightRange& hr = m_CellHeightRanges[ i + j * m_CellCountX ] ;

				float d = ( hr.Max - hr.Min ) / hr.CellCount ;

				for( int h = 0 ; h < hr.CellCount ; h ++ )
				{
					bool draw = false ;

					switch( path )
					{
					case INVISIBLE:
						if( !r_debug_vis_grid_cell->GetInt() )
						{
							draw = true ;
						}
						else
						{
							if( !GetVisibilityAt(	r_debug_vis_grid_cell_x->GetInt(), 
													r_debug_vis_grid_cell_y->GetInt(),
													r_debug_vis_grid_cell_z->GetInt(),
													i, h, j
													) 
								&&
								(	r_debug_vis_grid_cell_x->GetInt() != i 
									||
									r_debug_vis_grid_cell_y->GetInt() != h 
									||
									r_debug_vis_grid_cell_z->GetInt() != j )													
													)
							{
								draw = true ;
							}
						}
						break ;

					case VISIBLE:
						if( GetVisibilityAt(	r_debug_vis_grid_cell_x->GetInt(), 
												r_debug_vis_grid_cell_y->GetInt(),
												r_debug_vis_grid_cell_z->GetInt(),
												i, h, j )
								&&
								(	r_debug_vis_grid_cell_x->GetInt() != i 
									||
									r_debug_vis_grid_cell_y->GetInt() != h 
									||
									r_debug_vis_grid_cell_z->GetInt() != j )
							
							
							)
						{
							draw = true ;
						}
						break ;

					case SELECTED:
						if( r_debug_vis_grid_cell->GetInt() )
						{
							if( r_debug_vis_grid_cell_x->GetInt() == i 
								&& 
								r_debug_vis_grid_cell_y->GetInt() == h 
								&&
								r_debug_vis_grid_cell_z->GetInt() == j							
								)
							{
								draw = true ;
							}
						}
						break ;
					}

					if( !draw )
						continue ;

					float miH = hr.Min + ( h + 0 ) * d ;
					float maH = hr.Min + ( h + 1 ) * d ;

					PushDebugBox(	r3dPoint3D( cellMiX, miH, cellMiZ ),
									r3dPoint3D( cellMaX, miH, cellMiZ ),

									r3dPoint3D( cellMiX, maH, cellMiZ ),
									r3dPoint3D( cellMaX, maH, cellMiZ ),

									r3dPoint3D( cellMiX, miH, cellMaZ ),
									r3dPoint3D( cellMaX, miH, cellMaZ ),

									r3dPoint3D( cellMiX, maH, cellMaZ ),
									r3dPoint3D( cellMaX, maH, cellMaZ ),
									
									colors[ path ] ) ;
				}
			}
		}
	}
}

//------------------------------------------------------------------------

int
VisibiltyGrid::IsBBoxVisibleFrom( int cellX, int cellY, int cellZ, const r3dBoundBox& bbox )
{
	int negX, negY, negZ ;
	GetCellCoords( bbox.Org, &negX, &negY, &negZ ) ;

	int posX, posY, posZ ;
	GetCellCoords( bbox.Org + bbox.Size, &posX, &posY, &posZ ) ;

	for( int z = negZ ; z <= posZ ; z ++ )
	{
		for( int y = negY ; y <= posY ; y ++ )
		{
			for( int x = negX ; x <= posX ; x ++ )
			{
				if( GetVisibilityAt( cellX, cellY, cellZ, x, y, z ) )
					return 1 ;
			}
		}
	}

#ifndef FINAL_BUILD
	r3dRenderer->Stats.AddNumVisGridOcclusions( 1 ) ;
#endif

	return 0 ;
}

//------------------------------------------------------------------------

void
VisibiltyGrid::BuildCellHeights( float DesiredCellHeight )
{
	m_CellHeightRanges.Resize( m_CellCountX * m_CellCountZ ) ;

	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current; iter = GameWorld().GetNextOfAllObjects(iter) )
	{
		// force update world bbox
		iter.current->SetBBoxLocal( iter.current->GetBBoxLocal() ) ;
	}

	for( int j = 0 ; j < m_CellCountZ ; j ++ )
	{
		float cellMiZ = ( j + 0 ) * m_CellDepth + m_Start.z ;
		float cellMaZ = ( j + 1 ) * m_CellDepth + m_Start.z ;

		for( int i = 0 ; i < m_CellCountX ; i ++ )
		{
			float cellMiX = ( i + 0 ) * m_CellWidth + m_Start.x ;
			float cellMaX = ( i + 1 ) * m_CellWidth + m_Start.x ;

			float cellMiH = FLT_MAX, cellMaH = -FLT_MAX ;

			if( Terrain )
			{
				void terra_GetMinMaxTerraHeight( float miX, float miZ, float maX, float maZ, float* oMiH, float* oMaH ) ;
				terra_GetMinMaxTerraHeight( cellMiX, cellMiZ, cellMaX, cellMaZ, &cellMiH, &cellMaH ) ;
			}

			for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
			{
				GameObject * obj = iter.current;

				if( !IsVisGridObj( obj ) )
					continue ;

				const r3dBoundBox &box = obj->GetBBoxWorld() ;

				float boxMaxX = box.Org.X + box.Size.X;
				float boxMaxZ = box.Org.Z + box.Size.Z;

				if( boxMaxX < cellMiX )
					continue ;

				if( box.Org.x > cellMaX )
					continue ;

				if( boxMaxZ < cellMiZ )
					continue ;

				if( box.Org.z > cellMaZ )
					continue ;

				cellMiH = R3D_MIN( cellMiH, box.Org.Y ) ;
				cellMaH = R3D_MAX( cellMaH, box.Org.Y + box.Size.Y ) ;
			}

			if( cellMiH > cellMaH )
			{
				// set both to 0
				cellMiH = 0.f ;
				cellMaH = 0.f ;
			}

			HeightRange& hr = m_CellHeightRanges[ i + j * m_CellCountX ] ;
			hr.Min = cellMiH ;
			hr.Max = cellMaH ;

			hr.Max += 3.f ; // 3 meters for char height + jump..

			hr.CellCount = R3D_MIN( R3D_MAX( int( ( hr.Max - hr.Min ) / DesiredCellHeight ), 1 ), m_CellCountY ) ;
		}
	}
}

//------------------------------------------------------------------------

namespace
{
	static r3dPoint3D vFaceCamZDir[ r3dScreenBuffer::FACE_COUNT ] = 
	{
		r3dPoint3D( +1.f, +0.f, +0.f ),
		r3dPoint3D( -1.f, +0.f, +0.f ),
		r3dPoint3D( +0.f, +1.f, +0.f ),
		r3dPoint3D( +0.f, -1.f, +0.f ),
		r3dPoint3D( +0.f, +0.f, +1.f ),
		r3dPoint3D( +0.f, +0.f, -1.f )
	};

	static r3dPoint3D vFaceCamYDir[ r3dScreenBuffer::FACE_COUNT ] =
	{
		r3dPoint3D( +0.f, +1.f, +0.f ),
		r3dPoint3D( +0.f, +1.f, +0.f ),
		r3dPoint3D( +0.f, +0.f, -1.f ),
		r3dPoint3D( +0.f, +0.f, +1.f ),
		r3dPoint3D( +0.f, +1.f, +0.f ),
		r3dPoint3D( +0.f, +1.f, +0.f )
	};

	R3D_FORCEINLINE void SetupCamForFace( r3dCamera* Cam, r3dPoint3D pos, float Far, int f )
	{
		float fov = 90.f;

		*(r3dPoint3D*)Cam	= pos ;

		Cam->NearClip	= 0.05f ;
		Cam->FarClip	= Far ;

		Cam->vPointTo	= vFaceCamZDir[ f ];
		Cam->vUP		= vFaceCamYDir[ f ];

		// perspective projection matrix 
		Cam->FOV		= fov;

		Cam->ProjectionType	= r3dCamera::PROJTYPE_PRESPECTIVE;
		Cam->Width			= 0.f;
		Cam->Height			= 0.f;
		Cam->Aspect			= 1.0f;
	}
}

extern r3dCamera gCam ;

int
VisibiltyGrid::Calculate()
{
#ifdef FINAL_BUILD
	return;
#else
	typedef r3dTL::TArray< IDirect3DQuery9* > Queries ;
	typedef r3dTL::TArray< float > FloatArray ;
	typedef r3dTL::TArray< int > IntArray ;

	IntArray queriesDone ;

	Queries queries ;	
	FloatArray pixelArray ;
	FloatArray prevPixelArray ;

	bool DoCaptureVisGrid = false ;

	if( r_capture_vis_grid->GetInt() )
	{
		DoCaptureVisGrid = true ;
		r_capture_vis_grid->SetInt( 0 ) ;
	}

	struct EnableDisableDistanceCull
	{
		EnableDisableDistanceCull()
		{
			oldValue = r_allow_distance_cull->GetInt();
			r_allow_distance_cull->SetInt( 0 );
		}

		~EnableDisableDistanceCull()
		{
			r_allow_distance_cull->SetInt( oldValue );
		}

		int oldValue;

	} enableDisableDistanceCull; (void)enableDisableDistanceCull;

	struct ModifyRestoreRender
	{
		ModifyRestoreRender()
		{
			prevOQ			= r_use_oq->GetInt();
			prevParticles	= r_draw_particles->GetInt();
			prevGrass		= r_grass_draw->GetInt();
			prevHideIcons	= r_hide_icons->GetInt();

			r_use_oq->SetInt( 0 ) ;
			r_draw_particles->SetInt( 0 ) ;
			r_grass_draw->SetInt( 0 ) ;
			r_hide_icons->SetInt( 0 ) ;

			r_depth_mode->SetInt( 1 ) ;

			rt = r3dScreenBuffer::CreateClass( "TempBufferz", 512, 512, D3DFMT_A8R8G8B8, r3dScreenBuffer::Z_OWN ) ;

			D3DVERTEXELEMENT9 VertexElements[] =
			{
				{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
				D3DDECL_END()
			};


			D3D_V( r3dRenderer->pd3ddev->CreateVertexDeclaration( VertexElements, &vdecl ) ) ;

			savedCam = gCam ;
		}

		~ModifyRestoreRender()
		{
			r_use_oq->SetInt( prevOQ ) ;
			r_draw_particles->SetInt( prevParticles ) ;
			r_grass_draw->SetInt( prevGrass ) ;
			r_hide_icons->SetInt( prevHideIcons ) ;

			r_depth_mode->SetInt( 0 ) ;

			SAFE_DELETE( rt ) ;

			SAFE_RELEASE( vdecl ) ;

			gCam = savedCam ;
		}

		int prevOQ ;
		int prevParticles ;
		int prevGrass ;
		int prevHideIcons ;

		r3dScreenBuffer* rt ;

		IDirect3DVertexDeclaration9* vdecl ;

		r3dCamera savedCam ;

	} modifyRestoreRender ;

	queries.Resize( m_CellCountX * m_CellCountY * m_CellCountZ ) ;
	queriesDone.Resize( queries.Count(), 0 ) ;

	m_VisibilityCells.ZeroAll();

	uint16_t indices[ 6 * 2 * 3 ] = 
	{
		0, 1, 2,
		1, 2, 3,

		4, 5, 6,
		5, 6, 7,

		0, 2, 4,
		2, 6, 4,

		1, 3, 7,
		1, 7, 5,

		2, 3, 6,
		3, 7, 6,
	};

	struct CreateAndDestroyQueries
	{
		CreateAndDestroyQueries( Queries* a_queries )
		{
			queries = a_queries ;

			for( uint32_t i = 0, e = queries->Count(); i < e; i ++ )
			{
				D3D_V( r3dRenderer->pd3ddev->CreateQuery( D3DQUERYTYPE_OCCLUSION, &(*queries)[ i ] ) );
			}			
		}

		~CreateAndDestroyQueries()
		{
			for( uint32_t i = 0, e = queries->Count(); i < e; i ++ )
			{
				SAFE_RELEASE( (*queries)[ i ] ) ;
			}
		}

		Queries * queries ;

	} createAndDestroyQueries( &queries ) ;

	r3dPoint3D vertices[ 8 ] ;
	r3dBoundBox box ;

	r3dCamera cam ;

	float Far = sqrtf( m_Width * m_Width + m_Height * m_Height + m_Depth * m_Depth ) ;

	prevPixelArray.Resize( queries.Count() ) ;

	r3dOutToLog( "Calculating visibility grid\n" );

	const float INFO_DELTA = 0.033f ;
	float lastInfoFrame = r3dGetTime() - INFO_DELTA - 0.01f ;

	int TotalFrames = 0 ;

	for( int z = 0 ; z < m_CellCountZ ; z ++ )
	{
		for( int x = 0 ; x < m_CellCountX ; x ++ )
		{
			const HeightRange& hra = m_CellHeightRanges[ x + z * m_CellCountX ] ;

			TotalFrames += hra.CellCount ;
		}
	}	

	int divFrameCoef = ( 1 + 2 * 2 * 2 + 4 * 4 * 4 ) ; 
	
	if( r_vgrid_calc_one_cell->GetInt() )
	{
		TotalFrames = divFrameCoef ;
	}
	else
	{
		TotalFrames *= divFrameCoef ;
	}

	int FramesDone = 0 ;

	int invisCount = 0 ;
	int visCount = 0 ;

	for( int z = 0 ; z < m_CellCountZ ; z ++ )
	{
		for( int x = 0 ; x < m_CellCountX ; x ++ )
		{
			const HeightRange& hra = m_CellHeightRanges[ x + z * m_CellCountX ] ;

			float cellHeight = ( hra.Max - hra.Min ) / hra.CellCount ;

			for( int y = 0 ; y < hra.CellCount ; y ++ )
			{
				if( 
					r_vgrid_calc_one_cell->GetInt() &&
					(	r_debug_vis_grid_cell_x->GetInt() != x ||
						r_debug_vis_grid_cell_y->GetInt() != y ||
						r_debug_vis_grid_cell_z->GetInt() != z )
					)
				{
					continue ;
				}


				int lastPixels = -1 ;

				for( int div = 1 ; div <= 4 ; div *= 2 )
				{
					int newPixels = 0 ;

					pixelArray.Resize( 0 ) ;
					pixelArray.Resize( queries.Count(), 0 ) ;

					for( int dz = 0; dz < div ; dz ++ )
					{
						for( int dy = 0; dy < div ; dy ++ )
						{
							for( int dx = 0; dx < div ; dx ++ )
							{
								FramesDone ++ ;

								r3dPoint3D camP(	x * m_CellWidth + dx * m_CellWidth / div + m_CellWidth / div / 2 ,
													y * cellHeight + dy * cellHeight / div + cellHeight / div / 2 + hra.Min,
													z * m_CellDepth + dz * m_CellDepth / div + m_CellDepth / div / 2
													) ;

								camP += m_Start ;

								if( DoCaptureVisGrid )
								{
									D3DPERF_SetMarker( 0, L"Capture" ) ;

									DoCaptureVisGrid = false ;
								}

								r3dRenderer->StartFrame() ;

								d3dc.Reset();

								modifyRestoreRender.rt->Activate() ;
								r3dRenderer->StartRender( 0 ) ;

								for( int face = 0 ; face < 6 ; face ++ )
								{
									SetupCamForFace( &cam, camP, Far, face );

									D3D_V( r3dRenderer->pd3ddev->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, r3dRenderer->GetClearZValue(), 0 ) );

									r3dRenderer->SetCamera( cam, false ) ;

									r3dRenderer->SetVertexShader( "VS_FWD_COLOR" ) ;
									r3dRenderer->SetPixelShader( "PS_FWD_COLOR" ) ;

									r3dRenderer->SetDefaultCullMode( D3DCULL_NONE ) ;
									r3dRenderer->RestoreCullMode();

									r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW ) ;

									void RenderDepthScene();
									RenderDepthScene();

									r3dRenderer->SetCullMode( D3DCULL_NONE );
									r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC ) ;

									D3DXMATRIX shaderMtx = r3dRenderer->ViewProjMatrix ;
									D3DXMatrixTranspose( &shaderMtx, &shaderMtx ) ;

									D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float*)&shaderMtx, 4 ) ) ;

									d3dc._SetDecl( modifyRestoreRender.vdecl ) ;

									r3dRenderer->SetVertexShader( "VS_FWD_COLOR" ) ;
									r3dRenderer->SetPixelShader( "PS_FWD_COLOR" ) ;

									memset( &queriesDone[ 0 ], 0, sizeof( queriesDone[ 0 ] ) * queries.Count() ) ;

									for( int zz = 0 ; zz < m_CellCountZ ; zz ++ )
									{
										for( int xx = 0 ; xx < m_CellCountX ; xx ++ )
										{
											const HeightRange& hr = m_CellHeightRanges[ xx + zz * m_CellCountX ] ;

											for( int yy = 0 ; yy < hr.CellCount ; yy ++ )
											{
												if( xx == x && yy == y && zz == z )
													continue ;

												int idx = GetCellPairIDX( x, y, z, xx, yy, zz ) ;
												if( m_VisibilityCells[ idx ] )
													continue ;

												SetupCell( vertices, &box, &hr, xx, yy, zz ) ;
												if( r3dRenderer->IsBoxInsideFrustum( box ) )
												{
													int id = xx + yy * m_CellCountX * m_CellCountZ + zz * m_CellCountX ;

													D3D_V( queries[ id ]->Issue( D3DISSUE_BEGIN ) ) ;

													queriesDone[ id ] = 1 ;

													r3dRenderer->DrawIndexedUP( D3DPT_TRIANGLELIST, 0, 8, 12, indices, D3DFMT_INDEX16, vertices, sizeof( vertices[ 0 ] ) ) ;

													D3D_V( queries[ id ]->Issue( D3DISSUE_END ) ) ;
												}
											}
										}
									}

									for( int zz = 0 ; zz < m_CellCountZ ; zz ++ )
									{
										for( int xx = 0 ; xx < m_CellCountX ; xx ++ )
										{
											const HeightRange& hr = m_CellHeightRanges[ xx + zz * m_CellCountX ] ;

											for( int yy = 0 ; yy < hr.CellCount ; yy ++ )
											{
												if( xx == x && yy == y && zz == z )
													continue ;

												int id = xx + yy * m_CellCountX * m_CellCountZ + zz * m_CellCountX ;

												if( queriesDone[ id ] )
												{
													DWORD pixels = 0 ;

													for( HRESULT hr = S_OK ; ( hr = queries[ id ]->GetData( &pixels, sizeof(DWORD), D3DGETDATA_FLUSH ) ) != S_OK ; )
													{
														// implement me
														r3d_assert( hr != D3DERR_DEVICELOST ) ;
													}

													if( pixels )
													{
														int idx0 = GetCellPairIDX( x, y, z, xx, yy, zz ) ;
														int idx1 = GetCellPairIDX( xx, yy, zz, x, y, z ) ;

														m_VisibilityCells[ idx0 ] = 1 ;
														m_VisibilityCells[ idx1 ] = 1 ;

														visCount ++ ;
													}
													else
													{
														invisCount ++ ;
													}

													pixelArray[ id ] += pixels ;
												}
											}
										}
									}

									for( int i = 0, e = pixelArray.Count(); i < e; i ++ )
									{
										pixelArray[ i ] /= div * div * div ;
									}

									float maxDelta = 0.f ;

									for( int i = 0, e = pixelArray.Count(); i < e; i ++ )
									{
										float delta = pixelArray[ i ] - prevPixelArray[ i ] ;

										maxDelta = R3D_MAX( delta, maxDelta ) ;
									}									
								}

								modifyRestoreRender.rt->Deactivate() ;

								if( r3dGetTime() - lastInfoFrame > INFO_DELTA )
								{
									lastInfoFrame = r3dGetTime() ;

									r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), r3dRenderer->GetClearZValue(), 0 );

									r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ ) ;

									extern	CD3DFont*	MenuFont_Editor;

									char buf[ 512 ] ;

									r3dRenderer->SetVertexShader();
									r3dRenderer->SetPixelShader();

									sprintf( buf, "%d of %d frames done", FramesDone, TotalFrames ) ;

									MenuFont_Editor->DrawScaledText( 128.f, 64.f, 2, 2, 1, r3dColor::white, buf, 0 );

									sprintf( buf, "%.4f invis/vis", visCount ? float( invisCount ) / visCount : 0.f ) ;

									MenuFont_Editor->DrawScaledText( 128.f, 96.f, 2, 2, 1, r3dColor::white, buf, 0 );

									r3dProcessWindowMessages();
								}

								void InputUpdate();
								InputUpdate();
								if( Keyboard->IsPressed( kbsEsc ) )
								{
									return 0 ;
								}

								r3dRenderer->EndFrame() ;
								r3dRenderer->EndRender( true ) ;
							}
						}
					}
				}
			}
		}
	}

	return 1 ;
#endif
}

//------------------------------------------------------------------------

static char FILE_TAG[] = "VGRID100" ;

static r3dString MakeFilePath()
{
	r3dString fullPath = r3dGameLevel::GetHomeDir() ;
	fullPath += "/Grid.vis" ;

	return fullPath ;
}

void
VisibiltyGrid::Save()
{
	r3dString fullPath = MakeFilePath();

	FILE* file = fopen( fullPath.c_str(), "wb" ) ;

	fwrite( FILE_TAG, sizeof FILE_TAG, 1, file ) ;

	fwrite( &m_Width, sizeof m_Width, 1, file ) ;
	fwrite( &m_Height, sizeof m_Height, 1, file ) ;
	fwrite( &m_Depth, sizeof m_Depth, 1, file ) ;

	fwrite( &m_Start, sizeof m_Start, 1, file ) ;

	fwrite( &m_CellCountX, sizeof m_CellCountX, 1, file ) ;
	fwrite( &m_CellCountY, sizeof m_CellCountY, 1, file ) ;
	fwrite( &m_CellCountZ, sizeof m_CellCountZ, 1, file ) ;

	fwrite( &m_DesiredCellHeight, sizeof m_DesiredCellHeight, 1, file ) ;

	r3d_assert( m_CellHeightRanges.Count() == m_CellCountX * m_CellCountZ ) ;

	fwrite( &m_CellHeightRanges[ 0 ], sizeof( HeightRange ) * m_CellHeightRanges.Count(), 1, file ) ;

	fwrite( m_VisibilityCells.GetDataPtr(), m_VisibilityCells.Count() / 8, 1, file ) ;

	fclose( file ) ;
}

//------------------------------------------------------------------------

int
VisibiltyGrid::Load()
{
	struct FileWrap
	{
		FileWrap()
		{
			r3dString fullPath = MakeFilePath();

			file = r3d_open( fullPath.c_str(), "rb" ) ;
		}

		~FileWrap()
		{
			fclose( file ) ;
		}

		r3dFile* file ;
	} fw ;

	if( !fw.file )
		return 0 ;

	char checkTag[ sizeof FILE_TAG ] ;
	fread( checkTag, sizeof checkTag, 1, fw.file ) ;

	if( memcmp( checkTag, FILE_TAG, sizeof FILE_TAG ) )
		return 0 ;

	float width, height, depth, desiredCellHeight ;
	int cellCountX, cellCountY, cellCountZ ;

	r3dPoint3D start ;

	fread( &width, sizeof width, 1, fw.file ) ;
	fread( &height, sizeof height, 1, fw.file ) ;
	fread( &depth, sizeof depth, 1, fw.file ) ;

	fread( &start, sizeof m_Start, 1, fw.file ) ;

	fread( &cellCountX, sizeof cellCountX, 1, fw.file ) ;
	fread( &cellCountY, sizeof cellCountY, 1, fw.file ) ;
	fread( &cellCountZ, sizeof cellCountZ, 1, fw.file ) ;

	fread( &desiredCellHeight, sizeof desiredCellHeight, 1, fw.file ) ;

	m_CellHeightRanges.Resize( cellCountX * cellCountZ ) ;
	
	fread( &m_CellHeightRanges[ 0 ], sizeof( HeightRange ) * m_CellHeightRanges.Count(), 1, fw.file ) ;

	Init( cellCountX, cellCountY, cellCountZ, width, height, depth, desiredCellHeight, start, false ) ;

	fread( m_VisibilityCells.GetDataPtr(), m_VisibilityCells.Count() / 8, 1, fw.file ) ;

	return 1 ;
}

int	VisibiltyGrid::GetCellCountX ( ) const
{
	return m_CellCountX ;
}

//------------------------------------------------------------------------

int	VisibiltyGrid::GetCellCountY ( ) const
{
	return m_CellCountY ;
}

//------------------------------------------------------------------------

int	VisibiltyGrid::GetCellCountZ ( ) const
{
	return m_CellCountZ ;
}

//------------------------------------------------------------------------

int VisibiltyGrid::IsInited( ) const
{
	return m_IsInited ;
}


VisibiltyGrid * g_pVisibilityGrid ;

#endif