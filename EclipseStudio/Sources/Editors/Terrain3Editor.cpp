#include "r3dPCH.h"
#include "r3d.h"

#if !defined(FINAL_BUILD) && !defined(WO_SERVER)

#include "TrueNature2/Terrain3.h"

#include "GameLevel.h"

#include "Terrain3Editor.h"

#include "r3dBackgroundTaskDispatcher.h"
#include "r3dDeviceQueue.h"

static int LayerToSplat( int layerIdx );

extern int   _terra_smoothBoxOff;

static int g_RecalcMegatilesDisplayed = 0;

Terrain3Editor::Terrain3Editor()
: m_UndoItem( NULL )
, m_PaintLayerIdx( -1 )
, m_HeightDirty( 0 )
, m_TempUnpackedMaskTexture( 0 )
, m_TempPackedMaskTexture( 0 )
, m_InUndoRecord( 0 )
{
	CHeightChanged3::Register();
	CLayerMaskPaint3::Register();
	CLayerColorPaint3::Register();
	CLayerMaskEraseAll3::Register();
}

//------------------------------------------------------------------------

Terrain3Editor::~Terrain3Editor()
{
	SAFE_RELEASE( m_TempUnpackedMaskTexture );
	SAFE_RELEASE( m_TempPackedMaskTexture );
}

//------------------------------------------------------------------------

void Terrain3Editor::Update()
{
	OptimizeMaskCache( m_UnpackedMaskTilesCache );
	OptimizeMaskCache( m_UnpackedHeightTilesCache );

	for( UnpackedMaskTiles::iterator	
			i = m_UnpackedMaskTilesCache.begin(), 
			e = m_UnpackedMaskTilesCache.end();

			i != e;

			++ i
		)
	{
		i->second.Freshness ++;
	}

	for( UnpackedHeightTiles::iterator	
			i = m_UnpackedHeightTilesCache.begin(), 
			e = m_UnpackedHeightTilesCache.end();

			i != e;

			++ i
		)
	{
		i->second.Freshness ++;
	}

	if(Terrain3)
	{
		if( !Terrain3->AreMegaTilesRecalculated() )
		{
			if( !g_RecalcMegatilesDisplayed )
			{
				if( MessageBoxA( r3dRenderer->HLibWin, "Terrain 3 mask mip chains have to be regenerated. Do this now?", "Question", MB_YESNO ) == IDYES )
				{
					RecalcAllMasksMipMaps();
				}
				g_RecalcMegatilesDisplayed = 1;
			}
		}
	}
}

//------------------------------------------------------------------------

void Terrain3Editor::SaveTerrain( const char * targetDir )
{
	if(!Terrain3)
		return;

	if( !Terrain3->AreMegaTilesRecalculated() )
	{
		if( MessageBoxA( r3dRenderer->HLibWin, "Terrain 3 mask mip chains have to be regenerated to save terrain. Do this now?", "Question", MB_YESNO ) == IDYES )
		{
			RecalcAllMasksMipMaps();
		}
	}

	if( Terrain3->AreMegaTilesRecalculated() )
	{
		Terrain3->Save( targetDir );
	}
	else
	{
		MessageBoxA( r3dRenderer->HLibWin, "Terrain 3 was not saved!", "Warning", MB_ICONEXCLAMATION );
	}
}

//------------------------------------------------------------------------

template< typename T >
void Terrain3Editor::OptimizeMaskCache( T& cache )
{
	int diffCount = cache.size() - MAX_MASK_CACHE_ENTRIES;

	if( diffCount > 0 )
	{
		typedef std::map< int, T::iterator > SortBasedOnFreshness;

		SortBasedOnFreshness sortBasedOnFreshness;

		for( T::iterator	i = cache.begin(), 
							e = cache.end();
											
							i != e;

							++ i
							)
		{
			sortBasedOnFreshness.insert( SortBasedOnFreshness::value_type( i->second.Freshness, i ) );
		}

		for( SortBasedOnFreshness::iterator i = sortBasedOnFreshness.begin(), 
											e = sortBasedOnFreshness.end();

											i != e && diffCount > 0 ;

											++ i )
		{
			cache.erase( i->second );

			diffCount --;
		}
	}
}

//------------------------------------------------------------------------

Terrain3Editor::UnpackedMaskTile* Terrain3Editor::GetUnpackedMaskTile( int X, int Z, int L, int MaskID )
{
	R3DPROFILE_FUNCTION( "Terrain3Editor::GetUnpackedMaskTile" );

	INT64 key = MakeCachedTileKey( X, Z, L, MaskID );

	UnpackedMaskTiles::iterator iter = m_UnpackedMaskTilesCache.find( key );

	if( iter != m_UnpackedMaskTilesCache.end() )
	{
		iter->second.Freshness = 0;
		return &iter->second;
	}

	UnpackedMaskTile tile;

	tile.X = X;
	tile.Z = Z;
	tile.L = L;
	tile.MaskId = MaskID;

	UnpackedMaskTiles::iterator inserted = m_UnpackedMaskTilesCache.insert( UnpackedMaskTiles::value_type( key, tile ) ).first;

	CreateTempTextures();

	Terrain3->UnpackMask( &inserted->second.Data, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture, X, Z, L, MaskID );

	return &inserted->second;
}

//------------------------------------------------------------------------

Terrain3Editor::UnpackedHeightTile* Terrain3Editor::GetUnpackedHeightTile( int X, int Z, int L )
{
	INT64 key = MakeCachedTileKey( X, Z, L, 0 );

	UnpackedHeightTiles::iterator iter = m_UnpackedHeightTilesCache.find( key );

	if( iter != m_UnpackedHeightTilesCache.end() )
	{
		iter->second.Freshness = 0;
		return &iter->second;
	}

	UnpackedHeightTile tile;

	tile.X = X;
	tile.Z = Z;
	tile.L = L;

	UnpackedHeightTiles::iterator inserted = m_UnpackedHeightTilesCache.insert( UnpackedHeightTiles::value_type( key, tile ) ).first;

	Terrain3->UnpackHeight( &inserted->second.Data, X, Z, L );

	inserted->second.FloatData.Resize( inserted->second.Data.Count() );

	const r3dTerrainDesc& desc = Terrain3->GetDesc();

	float heightRange = desc.MaxHeight - desc.MinHeight;

	for( int i = 0, e = (int)inserted->second.Data.Count(); i < e; i ++ )
	{
		inserted->second.FloatData[ i ] = inserted->second.Data[ i ] / 65535.0f * heightRange + desc.MinHeight;
	}

	return &inserted->second;
}

//------------------------------------------------------------------------

void Terrain3Editor::FinalizeHeightEditing()
{
#if 0
	Terrain3->ConvertHeightField( &m_ShortHeights, m_FloatHeights, NULL );
	Terrain3->UpdatePhysicsHeightField( m_ShortHeights );
#endif
}

//------------------------------------------------------------------------

void
Terrain3Editor::LoadColorsFromTerrain()
{
#if 0
	r3dTexture* colorTex = Terrain3->GetColorTexture();

	ID3DXBuffer* fileInMem( NULL );

	D3D_V( D3DXSaveTextureToFileInMemory( &fileInMem, D3DXIFF_DDS, colorTex->AsTex2D(), NULL ) );

	IDirect3DTexture9* r8g8b8a8;

	D3D_V( D3DXCreateTextureFromFileInMemoryEx( 
				r3dRenderer->pd3ddev, fileInMem->GetBufferPointer(), fileInMem->GetBufferSize(),
				colorTex->GetWidth(), colorTex->GetHeight(), 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, 
				D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &r8g8b8a8 ) );

	SAFE_RELEASE( fileInMem );

	if( m_ColorTex )
		r3dRenderer->DeleteTexture( m_ColorTex );

	m_ColorTex = r3dRenderer->AllocateTexture();

	r3dD3DTextureTunnel texTunnel;

	texTunnel.Set( r8g8b8a8 );

	m_ColorTex->Setup( colorTex->GetWidth(), colorTex->GetHeight(), 1, D3DFMT_A8R8G8B8, 1, &texTunnel, false );
#endif
}

//------------------------------------------------------------------------

int
Terrain3Editor::AreHeightsLoaded() const
{
	return (int)m_FloatHeights.Count();
}

//------------------------------------------------------------------------

void Terrain3Editor::StartHeightEditing()
{
	
}

//------------------------------------------------------------------------

static r3dTL::TArray< Terrain3Editor::UnpackedHeightTile* > g_TouchedHeightTiles;

//------------------------------------------------------------------------

float Terrain3Editor::GetPaddingExtraSpace()
{
	const r3dTerrain3::QualitySettings& curQS = Terrain3->GetCurrentQualitySettings();

	return ( curQS.HeightNormalAtlasTileDim + 2.0f * r3dTerrain3::HEIGHT_TILE_BORDER ) / curQS.HeightNormalAtlasTileDim;
}

//------------------------------------------------------------------------

typedef float (*DefaultGatherFunc)( r3dTerrain3::Floats* floats, int gatherXStart, int gatherZStart );

template< typename FuncObj >
void SetupBounds( FuncObj* func, int x0, int z0, int x1, int z1 )
{
	if( func )
	{
		func->SetupBounds( x0, z0, x1, z1 );
	}
}

template <> void SetupBounds( DefaultGatherFunc func, int x0, int z0, int x1, int z1 )
{

}


template< typename OutFunc, typename GatherFunc >
void Terrain3Editor::ApplyHeightBrushT( const r3dPoint3D &pnt, const float strength, const float radius, const float hardness, OutFunc outFunc, GatherFunc* gatherFunc, int gatherSize )
{
	R3DPROFILE_FUNCTION( "Terrain3Editor::ApplyHeightBrushT" );

	r3d_assert( Terrain3 );

	m_HeightDirty = 1;

	r3dTerrain3::Info info = Terrain3->GetInfo();
	const r3dTerrainDesc& desc = Terrain3->GetDesc();

	r3dTerrain3::QualitySettings curQS = Terrain3->GetCurrentQualitySettings();

	int tileCountX = desc.TileCountX / curQS.VertexTilesInMegaTexTileCount;
	int tileCountZ = desc.TileCountZ / curQS.VertexTilesInMegaTexTileCount;

	int heightResU = desc.CellCountX;
	int heightResV = desc.CellCountZ;

	float cellSize = desc.CellSize;

	float terrx = pnt.x / desc.XSize;
	float terrz = pnt.z / desc.ZSize;
	float terrd = radius / desc.XSize;

	int tileDim = curQS.HeightNormalAtlasTileDim;

	g_TouchedHeightTiles.Clear();

	Floats gathered;

	gathered.Resize( gatherSize * gatherSize );

	float paddingExtraSpace = GetPaddingExtraSpace();

	CHeightChanged3* undo( NULL );
	IUndoItem* genericUndo = GetUndoRecord();

	undo = static_cast< CHeightChanged3* >( genericUndo );

	int L = 0;
	{
		// and convert to cells
		int np = int( terrd * heightResU );
		int pcx = int( terrx * heightResU );
		int pcz = int( terrz * heightResV );

		int nc = int(radius / cellSize);	// number of cells in radius
		int nc1 = int((radius*hardness) / cellSize);	// number of cells in radius*hard

		float fcx = pnt.X / cellSize;
		float fcz = pnt.Z / cellSize;

		int cx = int( fcx );	// center cell x
		int cz = int( fcz );	// center cell z

		cx = r3dTL::Clamp( cx, 0, heightResU - 1 );
		cz = r3dTL::Clamp( cz, 0, heightResV - 1 );

		// affected area
		int z0 = cz-nc; if( z0 < 0 )			z0 = 0;
		int z1 = cz+nc; if( z1 >= heightResV )	z1 = heightResV - 1;

		int x0 = cx-nc; if( x0 < 0 )			x0 = 0;
		int x1 = cx+nc; if( x1 >= heightResU )	x1 = heightResU - 1;

		SetupBounds( gatherFunc, x0, z0, x1, z1 );

		int tx0 = x0 / tileDim;
		int tx1 = x1 / tileDim;
		int tz0 = z0 / tileDim;
		int tz1 = z1 / tileDim;

		float tileWorldDimX = desc.XSize / tileCountX;
		float tileWorldDimZ = desc.ZSize / tileCountZ;

		float heightRange = desc.MaxHeight - desc.MinHeight;

		UShortsPtrArr ushorts;

		for( int tz = tz0; tz <= tz1; tz ++ )
		{
			for( int tx = tx0; tx <= tx1; tx ++ )
			{
				// check if it is completely outside of the radius

				float dx0 = ( tx + 0 ) * tileWorldDimX - pnt.x;
				float dx1 = ( tx + 1 ) * tileWorldDimX - pnt.x;
				float dz0 = ( tz + 0 ) * tileWorldDimZ - pnt.z;
				float dz1 = ( tz + 1 ) * tileWorldDimZ - pnt.z;

				float dist0 = sqrtf( float( dx0*dx0 + dz0*dz0 ) );
				float dist1 = sqrtf( float( dx1*dx1 + dz0*dz0 ) );
				float dist2 = sqrtf( float( dx0*dx0 + dz1*dz1 ) );
				float dist3 = sqrtf( float( dx1*dx1 + dz1*dz1 ) );

				float minDist = R3D_MIN( R3D_MIN( dist0, dist1 ), R3D_MIN( dist2, dist3 ) );

				float dxr0 = dx0 - radius;
				float dxr1 = dx1 + radius;
				float dzr0 = dz0 - radius;
				float dzr1 = dz1 + radius;

				int isInside = dxr0 <= 0.f && dxr1 >= 0.f && dzr0 <= 0.f && dzr1 >= 0.f;

				if( minDist > radius && !isInside )
					continue;

				UnpackedHeightTile* uht = GetUnpackedHeightTile( tx, tz, L );

				if( gatherSize )
				{
					m_TempFloatHeights.Resize( curQS.HeightNormalAtlasTileDim * curQS.HeightNormalAtlasTileDim );

					for( int i = 0, e = uht->FloatData.Count(); i < e; i ++ )
					{
						m_TempFloatHeights[ i ] = uht->FloatData[ i ];
					}

					for( int i = 0, e = ADJ_COUNT; i < e; i ++ )
					{
						UnpackedHeightTile* uht_adj = NULL;

						int ttx = 0;
						int ttz = 0;

						switch( i )
						{
						case ADJ_00:
							ttx = tx - 1;	ttz = tz - 1;
							break;
						case ADJ_10:
							ttx = tx - 0;	ttz = tz - 1;
							break;
						case ADJ_20:
							ttx = tx + 1;	ttz = tz - 1;
							break;
						case ADJ_01:							
							ttx = tx - 1;	ttz = tz - 0;
							break;
						case ADJ_21:
							ttx = tx + 1;	ttz = tz - 0;
							break;
						case ADJ_02:
							ttx = tx - 1;	ttz = tz + 1;
							break;
						case ADJ_12:
							ttx = tx - 0;	ttz = tz + 1;
							break;
						case ADJ_22:
							ttx = tx + 1;	ttz = tz + 1;
							break;
						}

						if( ttx >= 0 && ttx < tileCountX 
							&&
							ttz >= 0 && ttz < tileCountZ
							)
						{
							uht_adj = GetUnpackedHeightTile( ttx, ttz, L );

							Floats& flts = m_TempFloatHeightsAdj[ i ];

							if( uht_adj )
							{
								flts.Resize( uht_adj->Data.Count() );

								for( int ii = 0, ee = uht_adj->Data.Count(); ii < ee; ii ++ )
								{
									flts[ ii ] = uht_adj->FloatData[ ii ];
								}
							}
							else
							{
								flts.Clear();
							}
						}
					}
				}

				g_TouchedHeightTiles.PushBack( uht );

				uht->Synced = 0;

				int px0 = tx * tileDim;
				int pz0 = tz * tileDim;

				int px1 = px0 + tileDim;
				int pz1 = pz0 + tileDim;

				float centerPx = px0 + tileDim / 2 + 0.5f;
				float centerPz = pz0 + tileDim / 2 + 0.5f;

				for( int z = pz0, lpz = 0; z < pz1; z++, lpz ++ )
				{
					for( int x = px0, lpx = 0; x < px1; x++, lpx ++ )
					{
						float llpx = ( x - centerPx ) * paddingExtraSpace + centerPx;
						float llpz = ( z - centerPz ) * paddingExtraSpace + centerPz;

						float dz = llpz-fcz;
						float dx = llpx-fcx;

						float coef = sqrtf(float(dx*dx+dz*dz)) / float(nc);
						if(coef > 1.0f) continue;

						if (coef <= hardness ) coef = 1.0f;
						else
							coef = 1.0f - (coef - hardness) / (1.0f-hardness);

						int nIndex = lpz * curQS.HeightNormalAtlasTileDim + lpx;

						float fOld = uht->FloatData[ nIndex ];

						coef *= strength;
						
						// do something with h - it will be modified directly

						float gatheredVal = 0.f;

						if( gatherSize )
						{
							int gatherZStart = lpz - gatherSize / 2;
							int gatherXStart = lpx - gatherSize / 2;

							for( int z = gatherZStart, e = z + gatherSize, gz = 0; z < e; z ++, gz ++ )
							{
								for( int x = gatherXStart, e = x + gatherSize, gx = 0; x < e; x ++, gx ++ )
								{
									int ax = x;
									int az = z;

									if( ax < 0 )
									{
										if( az < 0 )
										{
											Floats& adjFloats = m_TempFloatHeightsAdj[ ADJ_00 ];

											if( adjFloats.Count() )
											{
												gathered[ gx + gz * gatherSize ] = adjFloats[ ( curQS.HeightNormalAtlasTileDim + az ) * curQS.HeightNormalAtlasTileDim + curQS.HeightNormalAtlasTileDim + ax ];
											}
											else
											{
												gathered[ gx + gz * gatherSize ] = m_TempFloatHeights[ 0 ];
											}
										}
										else
										{
											if( az < curQS.HeightNormalAtlasTileDim )
											{
												Floats& adjFloats = m_TempFloatHeightsAdj[ ADJ_01 ];

												if( adjFloats.Count() )
												{
													gathered[ gx + gz * gatherSize ] = adjFloats[ az * curQS.HeightNormalAtlasTileDim + curQS.HeightNormalAtlasTileDim + ax ];
												}
												else
												{
													gathered[ gx + gz * gatherSize ] = m_TempFloatHeights[ az * curQS.HeightNormalAtlasTileDim ];
												}
											}
											else
											{
												Floats& adjFloats = m_TempFloatHeightsAdj[ ADJ_02 ];

												if( adjFloats.Count() )
												{
													gathered[ gx + gz * gatherSize ] = adjFloats[ ( az - curQS.HeightNormalAtlasTileDim ) * curQS.HeightNormalAtlasTileDim + curQS.HeightNormalAtlasTileDim + ax ];
												}
												else
												{
													gathered[ gx + gz * gatherSize ] = m_TempFloatHeights[ ( curQS.HeightNormalAtlasTileDim - 1 ) * curQS.HeightNormalAtlasTileDim ];
												}
											}
										}
									}
									else
									{
										if( ax < curQS.HeightNormalAtlasTileDim )
										{
											if( az < 0 )
											{
												Floats& adjFloats = m_TempFloatHeightsAdj[ ADJ_10 ];

												if( adjFloats.Count() )
												{
													gathered[ gx + gz * gatherSize ] = adjFloats[ ( az + curQS.HeightNormalAtlasTileDim ) * curQS.HeightNormalAtlasTileDim + ax ];
												}
												else
												{
													gathered[ gx + gz * gatherSize ] = m_TempFloatHeights[ 0 * curQS.HeightNormalAtlasTileDim + ax ];
												}
											}
											else
											{
												if( az < curQS.HeightNormalAtlasTileDim )
												{
													gathered[ gx + gz * gatherSize ] = m_TempFloatHeights[ az * curQS.HeightNormalAtlasTileDim + ax ];
												}
												else
												{
													Floats& adjFloats = m_TempFloatHeightsAdj[ ADJ_12 ];

													if( adjFloats.Count() )
													{
														gathered[ gx + gz * gatherSize ] = adjFloats[ ( az - curQS.HeightNormalAtlasTileDim ) * curQS.HeightNormalAtlasTileDim + ax ];
													}
													else
													{
														gathered[ gx + gz * gatherSize ] = m_TempFloatHeights[ ( curQS.HeightNormalAtlasTileDim - 1 ) * curQS.HeightNormalAtlasTileDim + ax ];
													}
												}
											}
										}
										else
										{
											// x >= curQS.HeightNormalAtlasTileDim

											if( az < 0 )
											{
												Floats& adjFloats = m_TempFloatHeightsAdj[ ADJ_20 ];

												if( adjFloats.Count() )
												{
													gathered[ gx + gz * gatherSize ] = adjFloats[ ( curQS.HeightNormalAtlasTileDim + az ) * curQS.HeightNormalAtlasTileDim + ax - curQS.HeightNormalAtlasTileDim ];
												}
												else
												{
													gathered[ gx + gz * gatherSize ] = m_TempFloatHeights[ 0 * curQS.HeightNormalAtlasTileDim + curQS.HeightNormalAtlasTileDim - 1 ];
												}
											}
											else
											{
												if( az < curQS.HeightNormalAtlasTileDim )
												{
													Floats& adjFloats = m_TempFloatHeightsAdj[ ADJ_21 ];

													if( adjFloats.Count() )
													{
														gathered[ gx + gz * gatherSize ] = adjFloats[ az * curQS.HeightNormalAtlasTileDim + ax - curQS.HeightNormalAtlasTileDim ];
													}
													else
													{
														gathered[ gx + gz * gatherSize ] = m_TempFloatHeights[ az * curQS.HeightNormalAtlasTileDim + curQS.HeightNormalAtlasTileDim - 1 ];
													}
												}
												else
												{
													Floats& adjFloats = m_TempFloatHeightsAdj[ ADJ_22 ];

													if( adjFloats.Count() )
													{
														gathered[ gx + gz * gatherSize ] = adjFloats[ ( az - curQS.HeightNormalAtlasTileDim ) * curQS.HeightNormalAtlasTileDim + ax - curQS.HeightNormalAtlasTileDim ];
													}
													else
													{
														gathered[ gx + gz * gatherSize ] = m_TempFloatHeights[ ( curQS.HeightNormalAtlasTileDim - 1 ) * curQS.HeightNormalAtlasTileDim + curQS.HeightNormalAtlasTileDim - 1 ];
													}
												}
											}
										}
									}
								}
							}

							gatheredVal = (*gatherFunc)( &gathered, gatherXStart, gatherZStart );
						}

						float h = outFunc( fOld, gatheredVal, coef, (int)llpx, (int)llpz );

						if( h < 0 )
							h = 0;

						CHeightChanged3::UndoHeight_t data;

						UINT16 newVal = UINT16 ( R3D_MAX( R3D_MIN( ( h - desc.MinHeight ) / heightRange * 65535.0f, 65535.0f ), 0.0f ) );

						if( undo )
						{
							data.X			= lpx + tx * curQS.HeightNormalAtlasTileDim;
							data.Z			= lpz + tz * curQS.HeightNormalAtlasTileDim;
							data.PrevHeight	= uht->Data[ nIndex ];
							data.CurrHeight = newVal;

							undo->AddData( data );
						}

						uht->FloatData[ nIndex ] = h;
						uht->Data[ nIndex ] = newVal;
					}
				}

				ushorts.Clear();

				ushorts.PushBack( &uht->Data );

				for( int i = 1, e = info.NumActiveMegaTexLayers, ttz = tz / 2, ttx = tx / 2; i < e; i ++, ttz /= 2, ttx /= 2 )
				{
					UnpackedHeightTile * uht = GetUnpackedHeightTile( ttx, ttz, i );
					uht->Synced = 0;
					ushorts.PushBack( &uht->Data );
				}

				RecalcTileHeightMipChain( tx, tz, ushorts );

				for( int ttx = tx, ttz = tz, i = 0; i < info.NumActiveMegaTexLayers; i ++, ttx /= 2, ttz /= 2 )
				{
#ifndef ARTIFICIAL_FINAL_BUILD
					Terrain3->UpdateHeight( ushorts[ i ], &m_TempNormalData, ttx, ttz, i );
					Terrain3->RefreshHeightNormalAtlasTile( ttx, ttz, i, r3dTerrain3::LOADTILE_HEIGHT );
#endif
				}
			}
		}
	}

	const r3dTerrain3::QualitySettings& qs = Terrain3->GetCurrentQualitySettings();

	for( int i = 0, e = (int)g_TouchedHeightTiles.Count(); i < e; i ++ )
	{
		UnpackedHeightTile* uht = g_TouchedHeightTiles[ i ];

		if( !uht->L )
		{
			Terrain3->RecalcTileField( uht->X, uht->Z, uht->L, uht->Data );
		}
	}

}

//------------------------------------------------------------------------

void Terrain3Editor::ApplyHeightBrush(const r3dPoint3D &pnt, const float strength, const float radius, const float hardness)
{
	struct 
	{
		float operator() ( float fOldH, float gathered, float coef, int x, int z )
		{
			return fOldH + coef;
		}
	} heightFunc;

	ApplyHeightBrushT( pnt, strength, radius, hardness, heightFunc, (DefaultGatherFunc)NULL, 0 );
}

//------------------------------------------------------------------------

void
Terrain3Editor::ApplyHeightLevel(const r3dPoint3D &pnt, const float H, const float strength, const float radius, const float hardness)
{
	struct 
	{
		float operator() ( float fOldH, float gathered, float coef, int x, int z )
		{
			return H*coef+(1-coef)*fOldH;
		}

		float H;
	} heightFunc = { H };

	ApplyHeightBrushT( pnt, strength, radius, hardness, heightFunc, (DefaultGatherFunc)NULL, 0 );
}

//------------------------------------------------------------------------

static float ApplyBoxFilter(float* heightMap, int x, int y, int Width, int Height, int stepOff)
{
	float avg = 0;	// average height of box
	int   pix = 0;	// number of pixels

	// sum all pixels in box
	for(int x1 = x-stepOff; x1 <= x+stepOff; x1++) {
		if(x1 < 0 || x1 >= Width) continue;
		for(int y1 = y-stepOff; y1 <= y+stepOff; y1++) {
			if(y1 < 0 || y1 >= Height) continue;

			avg += heightMap[x1 + y1*Width];
			pix++;
		}
	}

	avg /= pix;

	return avg;
}

void
Terrain3Editor::ApplyHeightSmooth(const r3dPoint3D &pnt, const float radius, const float hardness )
{
	struct 
	{
		float operator() ( float fOldH, float gathered, float coef, int x, int z )
		{
			float kk = k * coef;
			return gathered*kk+(1-kk)*fOldH;
		}

		float k;
	} heightFunc;

	extern float _terra_smoothSpeed;

	heightFunc.k = r3dGetFrameTime() * _terra_smoothSpeed * 8.0f;
	heightFunc.k = R3D_CLAMP( heightFunc.k, 0.0f, 1.0f );

	struct
	{
		float operator()( r3dTerrain3::Floats* floats, int x, int z )
		{
			float sum = 0.f;

			for( int i = 0, e = floats->Count(); i < e; i ++ )
			{
				sum += (*floats)[ i ];
			}

			return sum / floats->Count();
		}

		void SetupBounds( int x0, int z0, int x1, int z1 )
		{

		}

	} gatherFunc;

	ApplyHeightBrushT( pnt, 1.0f, radius, hardness, heightFunc, &gatherFunc, _terra_smoothBoxOff * 2 + 1 );
}

//------------------------------------------------------------------------

enum
{
	EROSION_GATHER_SIZE = 2
};

void
Terrain3Editor::ApplyHeightErosion(const r3dPoint3D &pnt, const float strength, const float radius, const float hardness)
{
	struct
	{
		float operator() ( float fOldH, float gathered, float coef, int x, int z )
		{
			float h1 = R3D_MAX( fOldH - gathered * coef, desc.MinHeight );
			if (h1 <0) 
				h1 = 0;

			return h1;
		}

		r3dTerrainDesc desc;

	} heightFunc;

	struct
	{
		float operator()( r3dTerrain3::Floats* floats, int xStart, int zStart )
		{
			float fOld = (*floats)[ EROSION_GATHER_SIZE / 2 * EROSION_GATHER_SIZE + EROSION_GATHER_SIZE / 2 ];

			int x = xStart + EROSION_GATHER_SIZE / 2;
			int z = zStart + EROSION_GATHER_SIZE / 2;

			int lx = EROSION_GATHER_SIZE / 2;
			int lz = EROSION_GATHER_SIZE / 2;

			int zB = lz + 1;
			int xB = lx + 1;

			int nIndexB = zB * EROSION_GATHER_SIZE + lx;
			int nIndexR = lz * EROSION_GATHER_SIZE + xB;

			float fOldR = (*floats)[ nIndexR ];
			float fOldB = (*floats)[ nIndexB ];

			float ddyH = fOldB - fOld;
			float ddxH = fOldR - fOld;

			float ddH = R3D_MIN( strength, R3D_MAX( abs( ddyH ), abs( ddxH ) ) );

			float u = (x - x0) / static_cast<float>(x1 - x0);
			float v = (z - z0) / static_cast<float>(z1 - z0);
			float c = gErosionPattern->SampleBilinear(u, v);

			return c * ddH;
		}

		void SetupBounds( int x0, int z0, int x1, int z1 )
		{
			this->x0 = x0;
			this->z0 = z0;

			this->x1 = x1;
			this->z1 = z1;
		}

		int x0;
		int z0;
		int x1;
		int z1;

		float strength;

	} gatherFunc;

	gatherFunc.strength = strength;

	heightFunc.desc = Terrain3->GetDesc();

	ApplyHeightBrushT( r3dPoint3D( pnt.X, 0.f, pnt.Z ), strength, radius, hardness, heightFunc, &gatherFunc, 3 );
}

//------------------------------------------------------------------------

void
Terrain3Editor::ApplyHeightNoise(const r3dPoint3D &pnt, const float radius, const float hardness)
{
	m_HeightDirty = 1;

	noiseParams.Apply( pnt, radius, hardness );
}

//------------------------------------------------------------------------

void
Terrain3Editor::ApplyHeightRamp( const r3dPoint3D& rampStart, const r3dPoint3D& rampEnd, const float rampWidthOuter, const float rampWidthInner )
{
	m_HeightDirty = 1;

	const r3dTerrainDesc& desc = Terrain3->GetDesc();

	r3dPoint3D rampStart2D = r3dPoint3D(rampStart.x, 0, rampStart.z);
	r3dPoint3D rampEnd2D   = r3dPoint3D(rampEnd.x,   0, rampEnd.z);
	float      dy          = (rampEnd.y - rampStart.y);

	// detect terrain box for update
	float bbx1, bbx2, bbz1, bbz2;
	bbx1 = R3D_MIN(rampStart.x, rampEnd.x) - rampWidthOuter;
	bbx2 = R3D_MAX(rampStart.x, rampEnd.x) + rampWidthOuter;
	bbz1 = R3D_MIN(rampStart.z, rampEnd.z) - rampWidthOuter;
	bbz2 = R3D_MAX(rampStart.z, rampEnd.z) + rampWidthOuter;

	float centreX = ( bbx1 + bbx2 ) * 0.5f;
	float centreZ = ( bbz1 + bbz2 ) * 0.5f;

	float radius = sqrtf( ( bbx1 - bbx2 ) * ( bbx1 - bbx2 ) + ( bbz2 - bbz1 ) * ( bbz2 - bbz1 ) ) * 0.5f;

	struct
	{
		float operator() ( float fOldH, float gathered, float coef, int x, int z )
		{
			r3dPoint3D p(x*desc.CellSize, 0, z*desc.CellSize);
			float k; // dist koef on line [0..1]
			float d = r3dDistancePointLine(p, rampStart2D, rampEnd2D, &k);
			if(k < 0 || k > 1.0f)  return fOldH;

			if(d > rampWidthOuter) return fOldH;

			float h1 = fOldH;
			float h2 = rampStart.y + (dy * k);

			float h;
			if(d < rampWidthInner) {
				h = h2;
			} else { 
				float lc = ((d - rampWidthInner) / (rampWidthOuter - rampWidthInner));
				h = h2 + (h1 - h2) * lc;
			}

			return h;
		}

		r3dPoint3D rampStart2D;
		r3dPoint3D rampEnd2D;
		r3dPoint3D rampStart;
		r3dPoint3D rampEnd;
		float dy;
		r3dTerrainDesc desc;
		float rampWidthOuter;
		float rampWidthInner;
	} rampFunc;

	rampFunc.rampStart2D = rampStart2D;
	rampFunc.rampEnd2D = rampEnd2D;
	rampFunc.dy = dy;
	rampFunc.desc = desc;
	rampFunc.rampWidthOuter = rampWidthOuter;
	rampFunc.rampStart = rampStart;
	rampFunc.rampEnd = rampEnd;
	rampFunc.rampWidthInner = rampWidthInner;

	ApplyHeightBrushT( r3dPoint3D( centreX, 0.f, centreZ ), 1.0f, radius, 1.0f, rampFunc, (DefaultGatherFunc)NULL, 0 );
}

//------------------------------------------------------------------------

void Terrain3Editor::EndHeightEditing()
{
#ifndef ARTIFICIAL_FINAL_BUILD
	CreateTempTextures();

	const r3dTerrain3::QualitySettings& curQS = Terrain3->GetCurrentQualitySettings();

	for( UnpackedHeightTiles::iterator	i = m_UnpackedHeightTilesCache.begin(),
										e = m_UnpackedHeightTilesCache.end();
										i != e;
										++ i )
	{
		if( !i->second.Synced )
		{
			UnpackedHeightTile* uht = &i->second;

			int tileCountX, tileCountZ;

			Terrain3->GetHeightTileCountsForLod( uht->L, &tileCountX, &tileCountZ );

			UnpackedHeightTile* adj[ 3 ][ 3 ] = { 0 };

			adj[ 1 ][ 1 ] = uht;

			if( uht->X > 0 )
			{
				if( uht->Z > 0 )
					adj[ 0 ][ 0 ] = GetUnpackedHeightTile( uht->X - 1, uht->Z - 1, uht->L );

					adj[ 1 ][ 0 ] = GetUnpackedHeightTile( uht->X - 1, uht->Z, uht->L );

				if( uht->Z < tileCountZ - 1 )
					adj[ 2 ][ 0 ] = GetUnpackedHeightTile( uht->X - 1, uht->Z + 1, uht->L );
			}

			if( uht->Z > 0 )
				adj[ 0 ][ 1 ] = GetUnpackedHeightTile( uht->X, uht->Z - 1, uht->L );

			if( uht->Z < tileCountZ - 1 )
				adj[ 2 ][ 1 ] = GetUnpackedHeightTile( uht->X, uht->Z + 1, uht->L );

			if( uht->X < tileCountX - 1 )
			{
				if( uht->Z > 0 )
					adj[ 0 ][ 2 ] = GetUnpackedHeightTile( uht->X + 1, uht->Z - 1, uht->L );

					adj[ 1 ][ 2 ] = GetUnpackedHeightTile( uht->X + 1, uht->Z, uht->L );

				if( uht->Z < tileCountZ - 1 )
					adj[ 2 ][ 2 ] = GetUnpackedHeightTile( uht->X + 1, uht->Z + 1, uht->L );
			}

#if 0
			// left
			if( UnpackedHeightTile* adjt = adj[ 1 ][ 0 ] )
			{
				for( int i = 1, e = curQS.HeightNormalAtlasTileDim - 1; i < e; i ++ )
				{
					UINT16& val0 = uht->Data[ i * curQS.HeightNormalAtlasTileDim ];
					UINT16& val1 = adjt->Data[ i * curQS.HeightNormalAtlasTileDim + curQS.HeightNormalAtlasTileDim - 1 ];

					UINT16 avg = ( val0 + val1 ) / 2;
					
					val0 = avg;
					val1 = avg;
				}
			}

			// right
			if( UnpackedHeightTile* adjt = adj[ 1 ][ 2 ] )
			{
				for( int i = 1, e = curQS.HeightNormalAtlasTileDim - 1; i < e; i ++ )
				{
					UINT16& val0 = uht->Data[ i * curQS.HeightNormalAtlasTileDim + curQS.HeightNormalAtlasTileDim - 1 ];
					UINT16& val1 = adjt->Data[ i * curQS.HeightNormalAtlasTileDim ];

					UINT16 avg = ( val0 + val1 ) / 2;

					val0 = avg;
					val1 = avg;
				}
			}

			// top
			if( UnpackedHeightTile* adjt = adj[ 0 ][ 1 ] )
			{
				for( int i = 1, e = curQS.HeightNormalAtlasTileDim - 1; i < e; i ++ )
				{
					UINT16& val0 = uht->Data[ i ];
					UINT16& val1 = adjt->Data[ curQS.HeightNormalAtlasTileDim * ( curQS.HeightNormalAtlasTileDim - 1 ) + i ];

					UINT16 avg = ( val0 + val1 ) / 2;

					val0 = avg;
					val1 = avg;
				}
			}

			// bottom
			if( UnpackedHeightTile* adjt = adj[ 2 ][ 1 ] )
			{
				for( int i = 1, e = curQS.HeightNormalAtlasTileDim - 1; i < e; i ++ )
				{
					UINT16& val0 = uht->Data[ curQS.HeightNormalAtlasTileDim * ( curQS.HeightNormalAtlasTileDim - 1 ) + i ];
					UINT16& val1 = adjt->Data[ i ];

					UINT16 avg = ( val0 + val1 ) / 2;

					val0 = avg;
					val1 = avg;
				}
			}
#endif
			const UShorts* heights[ 9 ] = { 0 };

			for( int z = 0; z < 3; z ++ )
			{
				for( int x = 0; x < 3; x ++ )
				{
					if( UnpackedHeightTile* uuht = adj[ z ][ x ] )
					{
						Terrain3->UpdateHeight( &uuht->Data, &m_TempNormalData, uuht->X, uuht->Z, uuht->L );

						heights[ z * 3 + x ] = &uuht->Data;
					}
				}
			}

			Terrain3->RecalcNormalMap( heights, &m_TempFloatHeights, &m_TempVectors0, &m_TempVectors1, &m_TempUShorts, i->second.L );
			Terrain3->UpdateNormals( &m_TempUShorts, i->second.X, i->second.Z, i->second.L, m_TempUnpackedMaskTexture, m_TempPackedMaskTexture, &m_TempUShorts2 );

			Terrain3->MarkDirtyPhysChunk( i->second.X, i->second.Z );

			for( int z = 0; z < 3; z ++ )
			{
				for( int x = 0; x < 3; x ++ )
				{
					if( UnpackedHeightTile* uuht = adj[ z ][ x ] )
					{
						Terrain3->RefreshHeightNormalAtlasTile( uuht->X, uuht->Z, uuht->L, r3dTerrain3::LOADTILE_NORMAL | r3dTerrain3::LOADTILE_HEIGHT );
					}
				}
			}

			i->second.Synced = 1;
		}
	}
#endif
}

//------------------------------------------------------------------------

void Terrain3Editor::StartLayerBrush_G( int layerIdx )
{
	m_PaintLayerIdx = layerIdx;
}

//------------------------------------------------------------------------

void Terrain3Editor::ApplyRect( int xStart, int zStart, int xSize, int zSize, int L, const uint16_t* data )
{
	const r3dTerrain3::QualitySettings& curQS = Terrain3->GetCurrentQualitySettings();
	const r3dTerrainDesc& desc = Terrain3->GetDesc();

	int tileDim = curQS.MaskAtlasTileDim;

	int px0 = xStart;
	int pz0 = zStart;
	int px1 = xStart + xSize;
	int pz1 = zStart + zSize;

	int tx0 = px0 / tileDim;
	int tz0 = pz0 / tileDim;

	int tx1 = px1 / tileDim;
	int tz1 = pz1 / tileDim;

	int tileCountX = desc.TileCountX / curQS.VertexTilesInMegaTexTileCount;
	int tileCountZ = desc.TileCountZ / curQS.VertexTilesInMegaTexTileCount;

	tileCountX >>= L;
	tileCountZ >>= L;

	tx0 = R3D_MIN( R3D_MAX( tx0, 0 ), tileCountX - 1 );
	tz0 = R3D_MIN( R3D_MAX( tz0, 0 ), tileCountZ - 1 );

	tx1 = R3D_MIN( R3D_MAX( tx1, 0 ), tileCountX - 1 );
	tz1 = R3D_MIN( R3D_MAX( tz1, 0 ), tileCountZ - 1 );		

	int splatIdx = LayerToSplat( m_PaintLayerIdx );

	for( int tz = tz0; tz <= tz1; tz ++ )
	{
		for( int tx = tx0; tx <= tx1; tx ++ )
		{
			UnpackedMaskTile* maskTile = GetUnpackedMaskTile( tx, tz, L, splatIdx );

			maskTile->Synced = 0;

			for( int tpz = tz * curQS.MaskAtlasTileDim, lz = 0, e = (tz + 1) * curQS.MaskAtlasTileDim; tpz < e; tpz ++, lz ++ )
			{
				if( tpz < pz0 || tpz >= pz1 )
					continue;

				int undoZ = tpz - pz0;

				for( int tpx = tx * curQS.MaskAtlasTileDim, lx = 0, e = (tx + 1) * curQS.MaskAtlasTileDim; tpx < e; tpx ++, lx ++ )
				{
					if( tpx < px0 || tpx >= px1 )
						continue;

					int undoX = tpx - px0;

					maskTile->Data[ lx + lz * curQS.MaskAtlasTileDim ] = data[ undoZ * xSize + undoX ];
				}
			}
		}
	}
}

//------------------------------------------------------------------------

void Terrain3Editor::ApplyLayerBrush_G(const r3dTerrainPaintBoundControl& boundCtrl, const r3dPoint3D &pnt, int opType, int layerIdx, float val, float radius, const float hardness )
{
	int orgLayerIdx = layerIdx;

	r3d_assert( layerIdx >= 0 );
	r3d_assert( m_PaintLayerIdx == layerIdx );

	int SplatIdx = LayerToSplat( layerIdx );

	layerIdx -= 1;

	int Channel = layerIdx % r3dTerrain3::LAYERS_PER_MASK;

	r3dTerrain3::Info info = Terrain3->GetInfo();

	const r3dTerrainDesc& desc = Terrain3->GetDesc();

	const r3dTerrain3::QualitySettings& curQS = Terrain3->GetCurrentQualitySettings();

	float paddingExtraSpace = ( curQS.MaskAtlasTileDim + 2.0f * r3dTerrain3::MEGA_TEX_BORDER ) / curQS.MaskAtlasTileDim;

	int tileDim = curQS.MaskAtlasTileDim;
	float fFactor = 1.0f;

	int tileCountX = desc.TileCountX / curQS.VertexTilesInMegaTexTileCount;
	int tileCountZ = desc.TileCountZ / curQS.VertexTilesInMegaTexTileCount;

	int maskResU = desc.MaskResolutionU;
	int maskResV = desc.MaskResolutionV;

	r3d_assert( m_UndoItem );
	r3d_assert( m_UndoItem->GetActionID() == UA_TERRAIN3_MASK_PAINT );
	CLayerMaskPaint3 * pUndoItem = ( CLayerMaskPaint3 * ) m_UndoItem;

	for( int L = 0, e = info.NumActiveMegaTexLayers; L < e; 
			L ++, 
			tileCountX /= 2, 
			tileCountZ /= 2,
			maskResU /= 2,
			maskResV /= 2 )
	{
		// calc coord in [0..1] inside terrain

		float terrx = pnt.x / float(desc.CellSize * desc.CellCountX);
		float terry = pnt.z / float(desc.CellSize * desc.CellCountZ);
		float terrd = radius / float(desc.CellSize * desc.CellCountX);

		// and convert to cells

		int np = int( terrd * maskResU );
		int pcx = int( terrx * maskResU );
		int pcz = int( terry * maskResV );

		// affected area
		int pz0 = pcz-np; if( pz0 < 0)			pz0 = 0;
		int pz1 = pcz+np; if( pz1 > maskResV )	pz1 = maskResV;
		int px0 = pcx-np; if( px0 < 0)			px0 = 0;
		int px1 = pcx+np; if( px1 > maskResU )	px1 = maskResU;

		if( px0 > maskResU - 1	||
			pz0 > maskResV - 1	|| 
			px1 < 0	||  pz1 < 0 )
			return;

		int tx0 = px0 / tileDim;
		int tz0 = pz0 / tileDim;

		int tx1 = px1 / tileDim;
		int tz1 = pz1 / tileDim;

		float tileWorldDimX = desc.XSize / tileCountX;
		float tileWorldDimZ = desc.ZSize / tileCountZ;

		int lodTileCountX = info.MegaTileCountX >> L;
		int lodTileCountZ = info.MegaTileCountZ >> L;

		tx0 = R3D_MIN( R3D_MAX( tx0, 0 ), lodTileCountX - 1 );
		tz0 = R3D_MIN( R3D_MAX( tz0, 0 ), lodTileCountZ - 1 );

		tx1 = R3D_MIN( R3D_MAX( tx1, 0 ), lodTileCountX - 1 );
		tz1 = R3D_MIN( R3D_MAX( tz1, 0 ), lodTileCountZ - 1 );		

		for( int tz = tz0; tz <= tz1; tz ++ )
		{
			for( int tx = tx0; tx <= tx1; tx ++ )
			{
				// check if it is completely outside of the radius

				float dx0 = ( tx + 0 ) * tileWorldDimX - pnt.x;
				float dx1 = ( tx + 1 ) * tileWorldDimX - pnt.x;
				float dz0 = ( tz + 0 ) * tileWorldDimZ - pnt.z;
				float dz1 = ( tz + 1 ) * tileWorldDimZ - pnt.z;

				float dist0 = sqrtf( float( dx0*dx0 + dz0*dz0 ) );
				float dist1 = sqrtf( float( dx1*dx1 + dz0*dz0 ) );
				float dist2 = sqrtf( float( dx0*dx0 + dz1*dz1 ) );
				float dist3 = sqrtf( float( dx1*dx1 + dz1*dz1 ) );

				float minDist = R3D_MIN( R3D_MIN( dist0, dist1 ), R3D_MIN( dist2, dist3 ) );

				float dxr0 = dx0 - radius;
				float dxr1 = dx1 + radius;
				float dzr0 = dz0 - radius;
				float dzr1 = dz1 + radius;

				int isInside = dxr0 <= 0.f && dxr1 >= 0.f && dzr0 <= 0.f && dzr1 >= 0.f;

				if( minDist > radius && !isInside )
					continue;

				float fSoft = radius * ( 1.f - hardness );

				struct To16bit
				{
					R3D_FORCEINLINE UINT8 operator() ( UINT8 byte, int base )
					{
						int res = byte * base / 255;

						res = R3D_MAX( R3D_MIN( (int)res, base ), 0 );

						return UINT8 ( res );
					}

				} to16bit; (void)to16bit;

				UnpackedMaskTile* maskTile = GetUnpackedMaskTile( tx, tz, L, SplatIdx );

				maskTile->Synced = 0;

				int pxx0 = tx * tileDim;
				int pzz0 = tz * tileDim;

				int pxx1 = pxx0 + tileDim;
				int pzz1 = pzz0 + tileDim;

				float centerPx = pxx0 + tileDim / 2 + 0.5f;
				float centerPz = pzz0 + tileDim / 2 + 0.5f;

				int undoX0 = R3D_MAX( pxx0, px0 - 4 );
				int undoX1 = R3D_MIN( pxx1, px1 + 4 );

				int undoZ0 = R3D_MAX( pzz0, pz0 - 4 );
				int undoZ1 = R3D_MIN( pzz1, pz1 + 4 );

				CLayerMaskPaint3::PaintData_t undo;

				int nLine = undoX1 - undoX0;
				int zSize = ( undoZ1 - undoZ0 );
				int nSize = nLine * zSize;

				undo.rc.left = undoX0;
				undo.rc.right = undoX1;
				undo.rc.top = undoZ0;
				undo.rc.bottom = undoZ1;

				undo.L = L;
				undo.pData = game_new uint16_t[ nSize * 2 ];

				pUndoItem->AddData( undo );

				for( int pz = pzz0, lpz = 0; pz < pzz1; lpz ++, pz ++ )
				{
					for( int px = pxx0, lpx = 0; px < pxx1; lpx ++, px ++ )
					{
						float llpx = ( px - centerPx ) * paddingExtraSpace + centerPx;
						float llpz = ( pz - centerPz ) * paddingExtraSpace + centerPz;

						int undoIdx = -1;

						if( px >= undoX0 && px < undoX1 )
						{
							int dx = px - undoX0;
							int dz = pz - undoZ0;

							if( dx >= 0 && dx < nLine 
									&&
								dz >= 0 && dz < zSize
								)
							{
								undoIdx = dz * nLine + dx;
								uint16_t val = maskTile->Data[ lpx + lpz * tileDim ];
								undo.pData[ undoIdx ] = val;
								undo.pData[ undoIdx + nSize ] = val;
							}
						}

						float coef = 1.0f;
						if( np > 0 )
						{
							float dz = llpz - pcz;
							float dx = llpx - pcx;
							coef = sqrtf( float( dx*dx + dz*dz ) ) / float( np );
							if( coef > 1.0f )
							{
								continue;
							}
						}

						r3dPoint3D PP = 
							r3dPoint3D
							(
								( llpx / maskResU )*float( desc.CellSize * desc.CellCountX ), 
									0,
								( llpz / maskResV )*float( desc.CellSize * desc.CellCountX ) );

						PP.y = Terrain3->GetHeight( PP );

						if ( ! terra_IsValidPaintBounds( boundCtrl, PP, fSoft, &fFactor ) )
							continue;

						if( coef <= hardness )
							coef = 1.0f;
						else
							coef = 1.0f - (coef - hardness) / (1.0f-hardness);

						coef *= fFactor;

						struct Channeled16
						{
							UINT16 b : 5;
							UINT16 g : 6;
							UINT16 r : 5;
						} * rgb = ( Channeled16* )&maskTile->Data[ lpx + lpz * tileDim ];

						float pix;

						switch( Channel )
						{
						case 0:
							pix = rgb->r / 31.f * 255.0f;
							break;
						case 1:
							pix = rgb->g / 63.f * 255.0f;
							break;
						case 2:
							pix = rgb->b / 31.f * 255.0f;
							break;
						}

						if (opType) 
							pix = pix + val*coef;
						else
							pix = pix - val*coef;

						pix = R3D_CLAMP(pix, 0.0f, 255.0f);

						switch( Channel )
						{
						case 0:
							rgb->r = to16bit( (UINT8)pix, 31 );
							break;
						case 1:
							rgb->g = to16bit( (UINT8)pix, 63 );
							break;
						case 2:
							rgb->b = to16bit( (UINT8)pix, 31 );
							break;
						}

						if( undoIdx != -1 )
						{
							undo.pData[ undoIdx + nSize ] = *(uint16_t*)rgb;
						}
					}
				}

				// NOTE : we first must mark the tiles, and THEN RefreshMaskAtlasTile, because it uses bits
				// which are marked here
				if( !L )
				{
					int texHeight = desc.MaskResolutionV;
					int texWidth = desc.MaskResolutionU;

					int tileScale = desc.CellCountPerTile / r3dTerrain3::QualitySettings::MIN_VERTEX_TILE_DIM;

					int baseTilesX = desc.TileCountX * tileScale;
					int baseTilesZ = desc.TileCountZ * tileScale;

					int cx0 = ( tx + 0 ) * curQS.MaskAtlasTileDim * baseTilesX / desc.MaskResolutionU;
					int cx1 = ( tx + 1 ) * curQS.MaskAtlasTileDim * baseTilesX / desc.MaskResolutionU;

					int cz0 = ( tz + 0 ) * curQS.MaskAtlasTileDim * baseTilesZ / desc.MaskResolutionV;
					int cz1 = ( tz + 1 ) * curQS.MaskAtlasTileDim * baseTilesZ / desc.MaskResolutionV;

					float tileSize = desc.CellSize * r3dTerrain3::QualitySettings::MIN_VERTEX_TILE_DIM;

					for( int	z	= R3D_MAX( cz0, 0 ), 
								ez	= R3D_MIN( cz1, baseTilesZ - 1 );
								z < ez; z++ )
					{
						for( int	x	= R3D_MAX( cx0, 0 ),
									ex	= R3D_MIN( cx1, baseTilesX - 1 );
									x < ex; x++ )
						{
							float ax = x * tileSize;
							float az = z * tileSize;

							float dx0 = ax - pnt.x;
							float dz0 = az - pnt.z;

							if( sqrtf( dx0 * dx0 + dz0 * dz0 ) > radius + tileSize )
								continue;

							if( opType )
								Terrain3->MarkLayer( x, z, layerIdx + 1 );
							else
								Terrain3->RefreshAtlasTile( x / tileScale, z / tileScale );
						}
					}
				}

				Terrain3->RefreshMaskAtlasTile( tx, tz, L, r3dTerrain3::RM_NONE );

				D3DLOCKED_RECT lrect;
				D3DSURFACE_DESC sdesc;
				D3D_V( m_TempUnpackedMaskTexture->GetLevelDesc( 0, &sdesc ) );
				D3D_V( m_TempUnpackedMaskTexture->LockRect( 0, &lrect, NULL, 0 ) );
				r3d_assert( lrect.Pitch * sdesc.Height == maskTile->Data.Count() * sizeof maskTile->Data[ 0 ] );
				memcpy( lrect.pBits, &maskTile->Data[ 0 ], maskTile->Data.Count() * sizeof maskTile->Data[ 0 ] );
				D3D_V( m_TempUnpackedMaskTexture->UnlockRect( 0 ) );

#ifndef ARTIFICIAL_FINAL_BUILD
				Terrain3->AddReplacementTile( tx, tz, L, SplatIdx, m_TempUnpackedMaskTexture );
#endif
			}
		}
	}

	return;
}

//------------------------------------------------------------------------

void Terrain3Editor::EndLayerBrush_G()
{
	for( UnpackedMaskTiles::iterator	i = m_UnpackedMaskTilesCache.begin(),
										e = m_UnpackedMaskTilesCache.end();
										i != e;
										++ i )
	{
		UnpackedMaskTile& umt = i->second;

		if( !umt.Synced )
		{
			Terrain3->UpdateMask( &umt.Data, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture, umt.X, umt.Z, umt.L, umt.MaskId );

			if( Terrain3->IsMegaTileUsed( umt.X, umt.Z, umt.L ) )
			{
				Terrain3->ReloadMegaTileInMainThread( umt.X, umt.Z, umt.L, 
														r3dTerrain3::LOADTILE_MASKS | 
														r3dTerrain3::LOADTILE_UPDATEATLAS |
														r3dTerrain3::LOADTILE_DISABLEFILTER );
			}
			else
				Terrain3->RefreshMaskAtlasTile( umt.X, umt.Z, umt.L, r3dTerrain3::RM_UPDATE );

			i->second.Synced = 1;
		}
	}

#ifndef ARTIFICIAL_FINAL_BUILD
	Terrain3->ClearEditorReplacementTileIndexes();
#endif
}

//------------------------------------------------------------------------

void Terrain3Editor::OptimizeLayerMasks()
{
	if( MessageBoxA( r3dRenderer->HLibWin, "This operation cannot be undone and Terrain 3 will be saved into the level. Continue?", "Question", MB_YESNO ) == IDYES )
	{
		CreateTempTextures();

		r3dFinishBackGroundTasks();

		r3dTerrainOptiStats stats = Terrain3->OptimizeLayerMasks( 0, &m_TempUShorts, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture );
		Terrain3->RefreshAtlasTiles();

		m_UnpackedMaskTilesCache.clear();

		char message[ 256 ];
		sprintf( message, 
						"Deleted %d masks ( %.2f MB )\nRemoved %d layer info entries", 
						stats.NumDeletedMasks, 
						( stats.NumDeletedMasks * Terrain3->GetMaskTileSizeInFile() ) / 1024.f / 1024.f, 
						stats.NumDeletedLayerInfos );

		MessageBoxA( r3dRenderer->HLibWin, message, "Optimization Results", MB_OK );
	}
}

//------------------------------------------------------------------------

void Terrain3Editor::StartEraseAllBrush()
{
	r3dError( "Terrain3Editor::StartEraseAllBrush: implement me!" );
}

//------------------------------------------------------------------------

void Terrain3Editor::ApplyEraseAllBrush(const r3dTerrainPaintBoundControl& boundCtrl, const r3dPoint3D &pnt, const float val, const float radius, const float hardness )
{
	r3dError( "Terrain3Editor::StartEraseAllBrush: implement me!" );

#if 0
	r3d_assert( m_UndoItem );
	r3d_assert( m_UndoItem->GetActionID() == UA_TERRAIN2_MASK_ERASEALL );
	CLayerMaskEraseAll3 * pUndoItem = ( CLayerMaskEraseAll3 * ) m_UndoItem;

	int maskCount = GetMaskCount();

	float fFactor;

	r3dTexture* tex = GetPaintMask( 0 );

	// calc coord in [0..1] inside terrain

	const r3dTerrainDesc& desc = Terrain3->GetDesc();

	float terrx = pnt.x / float(desc.CellSize * desc.CellCountX);
	float terry = pnt.z / float(desc.CellSize * desc.CellCountZ);
	float terrd = radius / float(desc.CellSize * desc.CellCountX);

	// and convert to pixels
	int nc = int(terrd * (float)tex->GetWidth());
	int cx = int(terrx * (float)tex->GetWidth());	// center cell x
	int cz = int(terry * (float)tex->GetHeight());	// center cell z

	// affected area
	int z1 = cz-nc; if(z1 < 0)					z1 = 0;
	int z2 = cz+nc; if(z2 > tex->GetHeight() )	z2 = tex->GetHeight();
	int x1 = cx-nc; if(x1 < 0)					x1 = 0;
	int x2 = cx+nc; if(x2 > tex->GetWidth() )	x2 = tex->GetWidth();

	if( x1 > tex->GetWidth() - 1	||
		z1 > tex->GetHeight() - 1	|| 
		x2 < 0	||  z2 < 0 )
		return;

	float fSoft = radius * ( 1.f - hardness );

	RECT R;
	R.left = x1;
	R.right = x2;
	R.top = z1;
	R.bottom = z2;

	int nLine = R.right - R.left;
	int nSize = nLine * ( R.bottom - R.top );

	CLayerMaskEraseAll3::PaintData_t undo;
	undo.rc = R;

	undo.masks.Resize( maskCount );

	for( int i = 0, e = maskCount; i < e; i ++ )
	{
		r3dTexture* tex = GetPaintMask( i );

		if( tex->GetD3DFormat() != D3DFMT_R5G6B5 ) r3dError( "splat tex is not r5g6b5" );

		struct To16bit
		{
			R3D_FORCEINLINE UINT8 operator() ( UINT8 byte, int base )
			{
				int res = byte * base / 255;

				res = R3D_MAX( R3D_MIN( (int)res, base ), 0 );

				return UINT8 ( res );
			}

		} to16bit;

		struct Channeled16
		{
			UINT16 b : 5;
			UINT16 g : 6;
			UINT16 r : 5;
		} * colors = (Channeled16*)tex->Lock( 1, &R );

		COMPILE_ASSERT( sizeof *colors == 2 );

		undo.masks[ i ].Resize( nSize * 2 );

		for(int z = 0; z < z2 - z1; z++) 
		{
			uint16_t * pSrc = (uint16_t*)&colors[ z * tex->GetWidth() ];
			uint16_t * pDst = &undo.masks[ i ][ z * nLine ] ;

			r3d_assert( z * nLine + nLine <= (int)undo.masks[ i ].Count() );

			memcpy( pDst, pSrc, nLine * sizeof( uint16_t ) );
		}

		for(int z = z1; z < z2; z++)
		{
			for(int x = x1; x < x2; x++) 
			{
				float coef = 1.0f;

				if(nc > 0) 
				{
					int dz = z-cz;
					int dx = x-cx;
					coef = sqrtf(float(dx*dx+dz*dz)) / float(nc);
					if(coef > 1.0f) 
					{
						continue;
					}
				}

				r3dPoint3D PP = 
						r3dPoint3D
								(
									(float(x)/tex->GetWidth())*float(desc.CellSize * desc.CellCountX), 
										0,
									(float(z)/tex->GetHeight())*float(desc.CellSize * desc.CellCountX) );

				PP.y = Terrain3->GetHeight( PP );

				if ( ! terra_IsValidPaintBounds( boundCtrl, PP, fSoft, &fFactor ) )
					continue;

				if(coef <= hardness) 
					coef = 1.0f;
				else
					coef = 1.0f - (coef - hardness) / (1.0f-hardness);

				coef *= fFactor;
				coef *= val;

				int Pitch = tex->GetWidth();
				Channeled16* rgb = &colors[(z-z1)*Pitch+(x-x1)];

				float fr = rgb->r / 31.f * 255.0f;
				float fg = rgb->g / 63.f * 255.0f;
				float fb = rgb->b / 31.f * 255.0f;

				fr -= coef;
				fg -= coef;
				fb -= coef;

				fr = R3D_CLAMP(fr, 0.0f, 255.0f);
				fg = R3D_CLAMP(fg, 0.0f, 255.0f);
				fb = R3D_CLAMP(fb, 0.0f, 255.0f);

				rgb->r = to16bit( (UINT8)fr, 31 );
				rgb->g = to16bit( (UINT8)fg, 63 );
				rgb->b = to16bit( (UINT8)fb, 31 );
			}
		}

		for(int z = 0; z < z2 - z1; z++) 
		{
			uint16_t * pSrc = (uint16_t*)&colors[ z * tex->GetWidth() ];
			uint16_t * pDst = &undo.masks[ i ][ z * nLine + nSize ];

			r3d_assert( z * nLine + nSize + nLine <= (int)undo.masks[ i ].Count() );

			memcpy( pDst, pSrc, nLine * sizeof( uint16_t ) );
		}

		tex->Unlock();
	}

	{
		int texHeight = (int)tex->GetHeight();
		int texWidth = (int)tex->GetWidth();

		int tileScale = desc.CellCountPerTile / r3dTerrain3::QualitySettings::MIN_VERTEX_TILE_DIM;

		int baseTilesX = desc.TileCountX * tileScale;
		int baseTilesZ = desc.TileCountZ * tileScale;

		int cz1 = z1 * baseTilesZ / texHeight;
		int cz2 = z2 * baseTilesZ / texHeight;

		int cx1 = x1 * baseTilesX / texWidth;
		int cx2 = x2 * baseTilesX / texWidth;

		for(	int		z	= R3D_MAX( ( cz1 - 1 ), 0 ), 
						ez	= R3D_MIN( ( cz2 + 1 ), baseTilesZ - 1 );
						z <= ez; z++ )
		{
			for( int	x	= R3D_MAX( ( cx1 - 1 ), 0 ),
						ex	= R3D_MIN( ( cx2 + 1 ), baseTilesX - 1 );
						x <= ex; x++ )
			{
				Terrain3->RefreshAtlasTile( x / tileScale, z / tileScale );
			}
		}
	}

	pUndoItem->AddData( undo );
	pUndoItem->AddRectUpdate( undo.rc );

	return;
#endif

}

//------------------------------------------------------------------------

void Terrain3Editor::EndEraseAllBrush()
{
#if 0
	for( int i = 0, e = GetMaskCount(); i < e; i ++ )
	{
		Terrain3->UpdateLayerMaskFromReplacementMask( i );

		r3dRenderer->DeleteTexture( m_PaintLayerMasks[ i ] );

		// this refreshes the paint mask with actual DXT compressed
		// artifacts so that you see what you get immediately and
		// paint without switching to undistorted data
		m_PaintLayerMasks[ i ] = NULL;
		GetPaintMask( i );
	}
#else
	r3dError( "Terrain3Editor::EndEraseAllBrush: implement me!" );
#endif
}

//------------------------------------------------------------------------

void
Terrain3Editor::StartColorBrush()
{
#if 0
	if( !m_ColorTex )
	{
		LoadColorsFromTerrain();
	}

	Terrain3->SetReplacementColorTexture( m_ColorTex );
#else
	r3dError( "Terrain3Editor::StartColorBrush: implement me!" );
#endif
}

//------------------------------------------------------------------------

void
Terrain3Editor::ApplyColorBrush( const r3dTerrainPaintBoundControl& boundCtrl, const r3dPoint3D &pnt, const r3dColor &dwColor, const float strength, const float radius, const float hardness )
{
#if 0
	r3dTexture* colorTex = Terrain3->GetColorTexture();
	const r3dTerrainDesc& desc = Terrain3->GetDesc();

	int width = colorTex->GetWidth();
	int height = colorTex->GetHeight();

	int nc = int(radius / desc.CellSize / desc.CellCountX * width );	// number of cells in radius
	int nc1 = int((radius*hardness) / desc.CellSize / desc.CellCountX * width );	// number of cells in radius*hard
	int cx = int(pnt.X / desc.CellSize / desc.CellCountX * width );	// center cell x
	int cz = int(pnt.Z / desc.CellSize / desc.CellCountZ * height );	// center cell z

	cx = r3dTL::Clamp( cx, 0, width - 1 );
	cz = r3dTL::Clamp( cz, 0, height - 1 );

	// affected area
	int z1 = cz-nc; if(z1 < 0)					z1 = 0;
	int z2 = cz+nc; if(z2 > height)				z2 = height;
	int x1 = cx-nc; if(x1 < 0)					x1 = 0;
	int x2 = cx+nc; if(x2 > width)				x2 = width;

	r3d_assert( m_UndoItem );
	r3d_assert( m_UndoItem->GetActionID() == UA_TERRAIN2_COLOR_PAINT );
	CLayerColorPaint3 * pUndoItem = static_cast< CLayerColorPaint3 * >( m_UndoItem );

	float fFactor = 1.0f;
	float fSoft = radius * ( 1.f - hardness );

	RECT rect;

	rect.left = x1;
	rect.right = x2;
	rect.top = z1;
	rect.bottom = z2;

	RECT tileRect;

	tileRect.left	= x1 * desc.CellCountX / width;
	tileRect.right	= R3D_MIN( x2 * desc.CellCountX / width, desc.CellCountX - 1 );
	tileRect.top	= z1 * desc.CellCountZ / height;
	tileRect.bottom	= R3D_MIN( z2 * desc.CellCountZ / height, desc.CellCountZ - 1 );

	Terrain3->RefreshAtlasTiles( tileRect );

	uint32_t * pColors = (uint32_t*) m_ColorTex->Lock( 1, &rect );

	int lockPitch = m_ColorTex->GetLockPitch() / sizeof( uint32_t );

	for(int z = z1; z < z2; z++) {
		for(int x = x1; x < x2; x++) {
			int dy = z-cz;
			int dx = x-cx;


			float coef = sqrtf(float(dx*dx+dy*dy)) / float(nc);
			if(coef > 1.0f) 
				continue;


			if (coef <= hardness)
				coef = 1.0f;
			else
				coef = 1.0f - (coef - hardness) / (1.0f-hardness);
			//r3dOutToLog("%d %d\n", x,z);

			r3dPoint3D PP = r3dPoint3D(
				(float(x)/colorTex->GetWidth())*float(desc.CellSize * desc.CellCountX), 0,
				(float(z)/colorTex->GetHeight())*float(desc.CellSize * desc.CellCountZ));
			PP.y = Terrain3->GetHeight( PP );

			if ( ! terra_IsValidPaintBounds( boundCtrl, PP, fSoft, &fFactor ) )
				continue;

			coef *= fFactor;

			int nIndex = ( z - z1 ) * lockPitch + ( x - x1 );
			int nAbsIndex = z * lockPitch + x;

			r3dColor clr_old;			
			clr_old.SetPacked( pColors[ nIndex ] | 0xff000000 );

			//  if (coef >Strength) coef = Strength;
			coef *= strength;

			// do something with h - it will be modified directly

			r3dColor clr;

			clr.R = BYTE( dwColor.R * coef + clr_old.R * (1-coef) );
			clr.G = BYTE( dwColor.G * coef + clr_old.G * (1-coef) );
			clr.B = BYTE( dwColor.B * coef + clr_old.B * (1-coef) );

			clr.R = r3dTL::Clamp( clr.R, 0, 255 );
			clr.G = r3dTL::Clamp( clr.G, 0, 255 );
			clr.B = r3dTL::Clamp( clr.B, 0, 255 );

			DWORD clrPacked = clr.GetPacked() | 0xff000000;

			pColors[ nIndex ] = clrPacked;

			CLayerColorPaint3::UndoColor_t undo;
			undo.nIndex = nAbsIndex;
			undo.dwPrevColor = clr_old.GetPacked();
			undo.dwCurrColor = clrPacked;
			pUndoItem->AddData( undo );
		}
	}

	RECT rc;
	rc.left = x1;
	rc.bottom = z1;
	rc.right = x2;
	rc.top = z2;

	m_ColorTex->Unlock();

	pUndoItem->AddRectUpdate( rc );

	return;
#else
	r3dError( "Terrain3Editor::ApplyColorBrush: implement me!" );
#endif
}

//------------------------------------------------------------------------

void
Terrain3Editor::EndColorBrush()
{
#if 0
	Terrain3->UpdateColorTexture();
#else
	r3dError( "Terrain3Editor::EndColorBrush: implement me!" );
#endif
}

//------------------------------------------------------------------------

int
Terrain3Editor::ImportHeight( const char* path, float scaleY, float offsetY, int cellOffsetX, int cellOffsetZ )
{
	r3dFinishBackGroundTasks();

	StartHeightEditing();

	m_HeightDirty = 1;

	const r3dTerrainDesc& tdesc = Terrain3->GetDesc();
	const r3dTerrain3::QualitySettings& qs = Terrain3->GetCurrentQualitySettings();

	const r3dTerrain3::Info& info = Terrain3->GetInfo();

	IDirect3DTexture9* r32f_tex( NULL );
	if( D3DXCreateTextureFromFileEx( r3dRenderer->pd3ddev, path, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 0, D3DFMT_R32F, D3DPOOL_SYSTEMMEM, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &r32f_tex ) != S_OK )
	{
		return 0;
	}

	D3DLOCKED_RECT lrect;
	D3DSURFACE_DESC sdesc;

	D3D_V( r32f_tex->GetLevelDesc( 0, &sdesc ) );

	D3D_V( r32f_tex->LockRect( 0, &lrect, NULL, D3DLOCK_READONLY ) );

	r3d_assert( lrect.Pitch == sizeof(float) * sdesc.Width );

	int cellsPerTyle = tdesc.CellCountPerTile * qs.VertexTilesInMegaTexTileCount;

	r3d_assert( cellsPerTyle == qs.HeightNormalAtlasTileDim );

	int tsx = cellOffsetX / cellsPerTyle;
	int tsz = cellOffsetZ / cellsPerTyle;

	int tex = ( cellOffsetX + sdesc.Width ) / cellsPerTyle;
	int tez = ( cellOffsetZ + sdesc.Height ) / cellsPerTyle;

	tex = R3D_MIN( tex, tdesc.CellCountX / cellsPerTyle - 1 );
	tez = R3D_MIN( tez, tdesc.CellCountZ / cellsPerTyle - 1 );

	float paddingExtraSpace = GetPaddingExtraSpace();

	float invRange = 65535.0f / ( tdesc.MaxHeight - tdesc.MinHeight );

	UShortsPtrArr heights;

	for( int tz = tsz; tz <= tez; tz ++ )
	{
		for( int tx = tsx; tx <= tex; tx ++ )
		{
			heights.Clear();

			UnpackedHeightTile* uht = GetUnpackedHeightTile( tx, tz, 0 );

			heights.PushBack( &uht->Data );

			for( int i = 1, e = info.NumActiveMegaTexLayers, ttx = tx / 2, ttz = tz / 2; i < e; i ++, ttx /= 2, ttz /= 2 )
			{
				UnpackedHeightTile* parentUHT = GetUnpackedHeightTile( ttx, ttz, i );
				heights.PushBack( &parentUHT->Data );
				parentUHT->Synced = 0;
			}

			int centerCellX = tx * cellsPerTyle + cellsPerTyle / 2;
			int centerCellZ = tz * cellsPerTyle + cellsPerTyle / 2;

			for( int cz = tz * cellsPerTyle, cez = (tz + 1) * cellsPerTyle, lcz = 0; cz < cez; cz ++, lcz ++ )
			{
				for( int cx = tx * cellsPerTyle, cex = (tx + 1) * cellsPerTyle, lcx = 0; cx < cex; cx ++, lcx ++ )
				{
					int teX = int( ( cx - centerCellX ) * paddingExtraSpace ) + centerCellX - cellOffsetX;
					int teZ = int( ( cz - centerCellZ ) * paddingExtraSpace ) + centerCellZ - cellOffsetZ;

					int boundsX = cx - cellOffsetX;
					int boundsZ = cz - cellOffsetZ;

					if( boundsX < 0 || boundsZ < 0 || boundsX >= (int)sdesc.Width || boundsZ >= (int)sdesc.Height )
						continue;

					teX = R3D_MIN( R3D_MAX( teX, 0 ), (int)sdesc.Width - 1 );
					teZ = R3D_MIN( R3D_MAX( teZ, 0 ), (int)sdesc.Height - 1 );

					float height = ((float*)lrect.pBits)[ teX + ( sdesc.Height - teZ - 1 ) * sdesc.Width ] * scaleY + offsetY;

					UINT16 uval = UINT16( R3D_MIN( R3D_MAX( int(( height - tdesc.MinHeight ) * invRange), 0 ), 65535 ) );

					int idx = lcx + lcz * qs.HeightNormalAtlasTileDim;
					uht->Data[ idx ] = uval;
					uht->FloatData[ idx ] = height;
					uht->Synced = 0;
				}
			}

			RecalcTileHeightMipChain( tx, tz, heights );
		}
	}

	D3D_V( r32f_tex->UnlockRect( 0 ) );

	SAFE_RELEASE( r32f_tex );

	EndHeightEditing();

	return 1;
}

//------------------------------------------------------------------------

int
Terrain3Editor::ExportHeight( const char* path, int cellOffsetX, int cellOffsetZ, int sizeX, int sizeZ )
{
	const r3dTerrainDesc& tdesc = Terrain3->GetDesc();
	const r3dTerrain3::QualitySettings& qs = Terrain3->GetCurrentQualitySettings();

	cellOffsetX = R3D_MIN( R3D_MAX( cellOffsetX, 0 ), tdesc.CellCountX - 1 );
	cellOffsetZ = R3D_MIN( R3D_MAX( cellOffsetZ, 0 ), tdesc.CellCountZ - 1 );

	int endX = cellOffsetX + sizeX;
	int endZ = cellOffsetZ + sizeZ;

	endX = R3D_MIN( R3D_MAX( endX, 0 ), tdesc.CellCountX );
	endZ = R3D_MIN( R3D_MAX( endZ, 0 ), tdesc.CellCountZ );

	sizeX = endX - cellOffsetX;
	sizeZ = endZ - cellOffsetZ;

	IDirect3DTexture9* texture( NULL );

	D3D_V( r3dRenderer->pd3ddev->CreateTexture( sizeX, sizeZ, 1, 0, D3DFMT_R32F, D3DPOOL_SYSTEMMEM, &texture, NULL ) );

	D3DLOCKED_RECT lrect;

	D3D_V( texture->LockRect( 0, &lrect, NULL, 0 ) );

	int cellsPerTyle = tdesc.CellCountPerTile * qs.VertexTilesInMegaTexTileCount;

	r3d_assert( cellsPerTyle == qs.HeightNormalAtlasTileDim );

	int tsx = cellOffsetX / cellsPerTyle;
	int tsz = cellOffsetZ / cellsPerTyle;

	int tex = ( cellOffsetX + sizeX ) / cellsPerTyle;
	int tez = ( cellOffsetZ + sizeZ ) / cellsPerTyle;

	tex = R3D_MIN( tex, tdesc.CellCountX / cellsPerTyle - 1 );
	tez = R3D_MIN( tez, tdesc.CellCountZ / cellsPerTyle - 1 );

	float paddingExtra = GetPaddingExtraSpace();

	int halfCellsPerTyle = cellsPerTyle / 2;

	for( int tz = tsz; tz <= tez; tz ++ )
	{
		for( int tx = tsx; tx <= tex; tx ++ )
		{
			UnpackedHeightTile* uht = GetUnpackedHeightTile( tx, tz, 0 );

			for( int z = tz * cellsPerTyle, e = (tz + 1) * cellsPerTyle, lz = 0; z < e; z ++, lz ++ )
			{
				for( int x = tx * cellsPerTyle, e = (tx + 1) * cellsPerTyle, lx = 0; x < e; x ++, lx ++ )
				{
					int llx = int( ( lx - halfCellsPerTyle ) / paddingExtra ) + halfCellsPerTyle;
					int llz = int( ( lz - halfCellsPerTyle ) / paddingExtra ) + halfCellsPerTyle;

					float height = ( uht->Data[ llx + llz * cellsPerTyle ] + 0.125f ) / 65535.0f;

					int tx = x - cellOffsetX;
					int tz = z - cellOffsetZ;

					if( tx >= 0 && tx < sizeX && tz >= 0 && tz < sizeZ )
					{
						((float*)((char*)lrect.pBits + ( sizeZ - tz - 1 ) * lrect.Pitch))[ tx ] = height;
					}
				}
			}
		}
	}

	D3D_V( texture->UnlockRect( 0 ) );

	int success = D3DXSaveTextureToFile( path, D3DXIFF_DDS, texture, NULL ) == S_OK;

	SAFE_RELEASE( texture );
	
	return success;
}

//------------------------------------------------------------------------

static void ReportTileBordersRecalc( int i, int e )
{
	char buf[ 512 ];
	sprintf( buf, "Stitching %d of %d", i, e );
	Terrain3->ReportProgress( buf );
}

void Terrain3Editor::RecalcTileCoordBorders( const TileCoords& tcoords )
{
	CreateTempTextures();

	Terrain3->RecalcTileCoordBorders( tcoords, m_TempUShorts, m_TempUShorts2, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture, ReportTileBordersRecalc );
}

//------------------------------------------------------------------------

void Terrain3Editor::RecalcAllMasksMipMaps()
{
	CreateTempTextures();

	r3dRenderer->EndFrame();
	r3dRenderer->EndRender( true );

	FinishTasksWithReporting();

	r3dTerrain3::Info info = Terrain3->GetInfo();

	int maskCount = Terrain3->GetMaskCount();

	float lastInfoFrame = r3dGetTime();
	const float INFO_DELTA = 0.125f;

	int mdim = Terrain3->GetCurrentQualitySettings().MaskAtlasTileDim;
	m_TempUShorts.Clear();
	m_TempUShorts.Resize( mdim * mdim, 0 );

	for( int L = 1, e = Terrain3->GetInfo().NumActiveMegaTexLayers; L < e; L ++ )
	{
		for( int tz = 0, e = info.MegaTileCountZ >> L; tz < e; tz ++ )
		{
			for( int tx = 0, e = info.MegaTileCountX >> L; tx < e; tx ++ )
			{
				for( int m = 0, e = maskCount; m < e; m ++ )
				{
					if( r3dGetTime() - lastInfoFrame > INFO_DELTA )
					{
						lastInfoFrame = r3dGetTime();
						char buf[ 512 ];
						sprintf( buf, "Clearing (%d,%d,%d)", tx, tz, L );
						Terrain3->ReportProgress( buf );
					}

					if( Terrain3->HasMask( tx, tz, L, m ) )
					{
						Terrain3->UpdateMask( &m_TempUShorts, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture, tx, tz, L, m );
					}
				}
			}
		}
	}

	for( int L = 1, e = Terrain3->GetInfo().NumActiveMegaTexLayers; L < e; L ++ )
	{
		for( int tz = 0, e = info.MegaTileCountZ >> L; tz < e; tz ++ )
		{
			for( int tx = 0, e = info.MegaTileCountX >> L; tx < e; tx ++ )
			{
				for( int m = 0, e = maskCount; m < e; m ++ )
				{
					if( r3dGetTime() - lastInfoFrame > INFO_DELTA )
					{
						lastInfoFrame = r3dGetTime();
						char buf[ 512 ];
						sprintf( buf, "Calculating (%d,%d,%d)", tx, tz, L );
						Terrain3->ReportProgress( buf );
					}

					if( Terrain3->HasMask( tx * 2 + 0, tz * 2 + 0, L - 1, m ) ||
						Terrain3->HasMask( tx * 2 + 1, tz * 2 + 0, L - 1, m ) ||
						Terrain3->HasMask( tx * 2 + 0, tz * 2 + 1, L - 1, m ) ||
						Terrain3->HasMask( tx * 2 + 1, tz * 2 + 1, L - 1, m )
						)
					{
						UpdateLayerMaskMipFromLevelBelow( tx, tz, L, m );
					}
				}
			}
		}

		TileCoords tileCoords;

		for( int tz = 0, e = info.MegaTileCountZ >> L; tz < e; tz ++ )
		{
			for( int tx = 0, e = info.MegaTileCountX >> L; tx < e; tx ++ )
			{	
				for( int m = 0; m < maskCount; m ++ )
				{
					if( Terrain3->HasMask( tx, tz, L, m ) )
					{
						TileCoord tc;

						tc.X = tx;
						tc.Z = tz;
						tc.L = L;
						tc.M = m;

						tileCoords.PushBack( tc );
					}
				}
			}
		}

		RecalcTileCoordBorders( tileCoords );
	}

	//------------------------------------------------------------------------

	Terrain3->RefreshAllMaskAtlasTiles();

	Terrain3->MarkMegaTilesRecalculated();

	FinishTasksWithReporting();

	r3dRenderer->StartFrame();
	d3dc.Reset();
	r3dRenderer->StartRender( 0 );
}

//------------------------------------------------------------------------

void Terrain3Editor::RebuildLayerInfo()
{
	float lastInfoFrame = r3dGetTime();
	const float INFO_DELTA = 0.125f;

	r3dTerrain3::Info info = Terrain3->GetInfo();

	int maskCount = Terrain3->GetMaskCount();
	int layerCount = Terrain3->GetDesc().LayerCount;

	Terrain3->ClearMegaLayerInfos();

	for( int L = 0, e = Terrain3->GetInfo().NumActiveMegaTexLayers; L < e; L ++ )
	{
		for( int tz = 0, e = info.MegaTileCountZ >> L; tz < e; tz ++ )
		{
			for( int tx = 0, e = info.MegaTileCountX >> L; tx < e; tx ++ )
			{
				for( int m = 0, e = maskCount; m < e; m ++ )
				{
					if( r3dGetTime() - lastInfoFrame > INFO_DELTA )
					{
						lastInfoFrame = r3dGetTime();
						char buf[ 512 ];
						sprintf( buf, "Rebuilding layer info (%d,%d,%d)", tx, tz, L );
						Terrain3->ReportProgress( buf );
					}

					if( Terrain3->HasMask( tx, tz, L, m ) )
					{
						Terrain3->UnpackMask( &m_TempUShorts, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture, tx, tz, L, m );

						union
						{
							struct
							{
								UINT16 b : 5;
								UINT16 g : 6;
								UINT16 r : 5;
							};

							UINT16 sample;
						};

						int r_present = 0;
						int g_present = 0;
						int b_present = 0;

						for( int i = 0, e = (int)m_TempUShorts.Count(); i < e; i ++ )
						{
							sample = m_TempUShorts[ i ];

							if( r ) r_present = 1;
							if( g ) g_present = 1;
							if( b ) b_present = 1;

							if( r_present & g_present & b_present )
								break;
						}

						int lidx0 = m * r3dTerrain3::LAYERS_PER_MASK + 0;
						int lidx1 = m * r3dTerrain3::LAYERS_PER_MASK + 1;
						int lidx2 = m * r3dTerrain3::LAYERS_PER_MASK + 2;

						if( r_present && lidx0 < layerCount ) Terrain3->AddLayerToMegaInfo( tx, tz, L, lidx0 );
						if( g_present && lidx1 < layerCount ) Terrain3->AddLayerToMegaInfo( tx, tz, L, lidx1 );
						if( b_present && lidx2 < layerCount ) Terrain3->AddLayerToMegaInfo( tx, tz, L, lidx2 );
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------------

void Terrain3Editor::RecalcAllHeightMipMaps()
{
#ifndef ARTIFICIAL_FINAL_BUILD
	CreateTempTextures();

	r3dRenderer->EndFrame();
	r3dRenderer->EndRender( true );

	FinishTasksWithReporting();

	r3dTerrain3::Info info = Terrain3->GetInfo();

	int tcx = info.MegaTileCountX / 2;
	int tcz = info.MegaTileCountZ / 2;

	int heightDim = Terrain3->GetCurrentQualitySettings().HeightNormalAtlasTileDim;
	int halfHeightDim = heightDim / 2;

	float lastInfoFrame = r3dGetTime();

	const float INFO_DELTA = 0.125f;

	for( int L = 1, e = (int)Terrain3->GetInfo().NumActiveMegaTexLayers; L < e; L ++, tcx /= 2, tcz /= 2 )
	{
		for( int tz = 0; tz < tcz; tz ++ )
		{
			for( int tx = 0; tx < tcx; tx ++ )
			{
				if( r3dGetTime() - lastInfoFrame > INFO_DELTA )
				{
					lastInfoFrame = r3dGetTime();
					char buf[ 512 ];
					sprintf( buf, "Rebuilding height mips (%d,%d,%d)", tx, tz, L );
					Terrain3->ReportProgress( buf );
				}

				Terrain3->UnpackHeight( &m_TempUShorts, tx, tz, L );

				for( int ttz = tz * 2, lz = 0,e = tz * 2 + 2; ttz < e; ttz ++, lz += halfHeightDim )
				{
					for( int ttx = tx * 2, lx = 0, e = tx * 2 + 2; ttx < e; ttx ++, lx += halfHeightDim )
					{
						Terrain3->UnpackHeight( &m_TempUShorts2, ttx, ttz, L - 1 );

						for( int z = 0, e = heightDim / 2; z < e; z ++ )
						{
							for( int x = 0, e = heightDim / 2; x < e; x ++ )
							{
								int avg = 0;

								avg += m_TempUShorts2[ x * 2 + 0 + ( z * 2 + 0 ) * heightDim ];
								avg += m_TempUShorts2[ x * 2 + 1 + ( z * 2 + 0 ) * heightDim ];
								avg += m_TempUShorts2[ x * 2 + 0 + ( z * 2 + 1 ) * heightDim ];
								avg += m_TempUShorts2[ x * 2 + 1 + ( z * 2 + 1 ) * heightDim ];

								avg /= 4;

								m_TempUShorts[ x + lx + ( z + lz ) * heightDim ] = avg;
							}
						}
					}
				}

				//------------------------------------------------------------------------
				Terrain3->UpdateHeight( &m_TempUShorts, &m_TempNormalData, tx, tz, L );

				const UShorts* heights[ 9 ] = { 0 };

				for( int z = 0; z < 3; z ++ )
				{
					for( int x = 0; x < 3; x ++ )
					{
						if( x == 1 && z == 1 )
							continue;

						int finalTX = tx + x - 1;
						int finalTZ = tz + z - 1;

						if( finalTX < 0 || finalTZ < 0 
							||
							finalTX >= tcx || finalTZ >= tcz )
							continue;

						UnpackedHeightTile* uht = GetUnpackedHeightTile( finalTX, finalTZ, L );

						heights[ z * 3 + x ] = &uht->Data;
					}
				}

				heights[ 4 ] = &m_TempUShorts;

				Terrain3->RecalcNormalMap( heights, &m_TempFloatHeights, &m_TempVectors0, &m_TempVectors1, &m_TempUShorts, L );
				Terrain3->UpdateNormals( &m_TempUShorts, tx, tz, L, m_TempUnpackedMaskTexture, m_TempPackedMaskTexture, &m_TempUShorts2 );
				//------------------------------------------------------------------------
			}
		}
	}

	Terrain3->RefreshAllMaskAtlasTiles();

	FinishTasksWithReporting();

	r3dRenderer->StartFrame();
	d3dc.Reset();
	r3dRenderer->StartRender( 0 );
#endif
}

//------------------------------------------------------------------------

void Terrain3Editor::UpdateLayerMaskMipFromTemps( int tx, int tz, int maskId, TileCoords* oTileCoords )
{
	const r3dTerrain3::QualitySettings& qs = Terrain3->GetCurrentQualitySettings();

	int orgDim = qs.MaskAtlasTileDim;
	int dim = orgDim / 2;

	int xStart = 0;
	int zStart = 0;

	enum
	{
		HALF_MEGA_TEX_BORDER = r3dTerrain3::MEGA_TEX_BORDER / 2
	};

	for( int L = 1, e = Terrain3->GetInfo().NumActiveMegaTexLayers, ttx = tx / 2, ttz = tz / 2; L < e; L ++, dim /= 2, ttz /= 2, ttx /= 2 )
	{
		Terrain3->UnpackMask( &m_TempUShorts2, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture, ttx, ttz, L, maskId );

		int relToPrevX = ( tx - ( tx >> L << L ) ) >> ( L - 1 );
		int relToPrevZ = ( tz - ( tz >> L << L ) ) >> ( L - 1 );

		int xTargetStartOrg = ( tx - ( tx >> L << L ) ) * dim;
		int zTargetStartOrg = ( tz - ( tz >> L << L ) ) * dim;

		if( oTileCoords )
		{
			if( !xTargetStartOrg || 
				!zTargetStartOrg ||
				xTargetStartOrg + dim == qs.MaskAtlasTileDim ||
				zTargetStartOrg + dim == qs.MaskAtlasTileDim )
			{
				TileCoord tc;

				tc.X = tx;
				tc.Z = tz;
				tc.L = L;
				tc.M = maskId;

				oTileCoords->PushBack( tc );
			}
		}

		int xTargetStart = xTargetStartOrg;
		int zTargetStart = zTargetStartOrg;

		if( relToPrevX )
			xTargetStart -= HALF_MEGA_TEX_BORDER;
		else
			xTargetStart += HALF_MEGA_TEX_BORDER;

		if( relToPrevZ )
			zTargetStart -= HALF_MEGA_TEX_BORDER;
		else
			zTargetStart += HALF_MEGA_TEX_BORDER;

		int xTargetEnd = xTargetStart + dim;
		int zTargetEnd = zTargetStart + dim;

		if( xStart >= r3dTerrain3::MEGA_TEX_BORDER )
		{
			xStart -= r3dTerrain3::MEGA_TEX_BORDER;
			xTargetStart -= HALF_MEGA_TEX_BORDER;
		}

		if( zStart >= r3dTerrain3::MEGA_TEX_BORDER )
		{
			zStart -= r3dTerrain3::MEGA_TEX_BORDER;
			zTargetStart -= HALF_MEGA_TEX_BORDER;
		}

		if( xStart + dim * 2 < orgDim - r3dTerrain3::MEGA_TEX_BORDER )
		{
			xTargetEnd += HALF_MEGA_TEX_BORDER;
		}

		if( zStart + dim * 2 < orgDim - r3dTerrain3::MEGA_TEX_BORDER )
		{
			zTargetEnd += HALF_MEGA_TEX_BORDER;
		}

		for( int z = zTargetStart, lz = 0; z < zTargetEnd; z ++, lz ++ )
		{
			for( int x = xTargetStart, lx = 0; x < xTargetEnd; x ++, lx ++ )
			{
				int srcIdxes[ 4 ];

				srcIdxes[ 0 ] = ( zStart + lz * 2 + 0 ) * qs.MaskAtlasTileDim + xStart + lx * 2 + 0;
				srcIdxes[ 1 ] = ( zStart + lz * 2 + 1 ) * qs.MaskAtlasTileDim + xStart + lx * 2 + 0;
				srcIdxes[ 2 ] = ( zStart + lz * 2 + 0 ) * qs.MaskAtlasTileDim + xStart + lx * 2 + 1;
				srcIdxes[ 3 ] = ( zStart + lz * 2 + 1 ) * qs.MaskAtlasTileDim + xStart + lx * 2 + 1;

				int rsum = 0;
				int gsum = 0;
				int bsum = 0;

				union
				{
					struct
					{
						UINT16 r : 5;
						UINT16 g : 6;
						UINT16 b : 5;
					};

					UINT16 sample;
				};

				for( int i = 0; i < 4; i ++ )
				{
					sample = m_TempUShorts[ srcIdxes[ i ] ];

					rsum += r;
					gsum += g;
					bsum += b;
				}

				rsum /= 4;
				gsum /= 4;
				bsum /= 4;

				r = rsum;
				g = gsum;
				b = bsum;

				m_TempUShorts2[ z * qs.MaskAtlasTileDim + x ] = sample;
			}
		}

		xStart = xTargetStartOrg;
		zStart = zTargetStartOrg;

		Terrain3->UpdateMask( &m_TempUShorts2, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture, ttx, ttz, L, maskId );

		m_TempUShorts.Swap( m_TempUShorts2 );
	}
}

//------------------------------------------------------------------------

void Terrain3Editor::UpdateLayerMaskMipFromLevelBelow( int tx, int tz, int L, int maskId )
{
	Terrain3->UpdateLayerMaskMipFromLevelBelow( tx, tz, L, maskId, m_TempUShorts, m_TempUShorts2, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture );
}

//------------------------------------------------------------------------

int Terrain3Editor::ImportLayer( const char* path, int layerIndex, int cellOffsetX, int cellOffsetZ )
{
	r3dRenderer->EndFrame();
	r3dRenderer->EndRender( true );

	FinishTasksWithReporting();

	CreateTempTextures();

	const r3dTerrain3::QualitySettings& qs = Terrain3->GetCurrentQualitySettings();
	const r3dTerrainDesc& tdesc = Terrain3->GetDesc();

	int maskId = layerIndex / r3dTerrain3::LAYERS_PER_MASK;
	int channelId = layerIndex % r3dTerrain3::LAYERS_PER_MASK;

	D3DXIMAGE_INFO dxii;

	D3DXGetImageInfoFromFile( path, &dxii );

	int cellsPerTyle = qs.MaskAtlasTileDim;

	r3d_assert( cellsPerTyle == qs.HeightNormalAtlasTileDim );

	int tsx = cellOffsetX / cellsPerTyle;
	int tsz = cellOffsetZ / cellsPerTyle;

	int tex = ( cellOffsetX + dxii.Width ) / cellsPerTyle;
	int tez = ( cellOffsetZ + dxii.Height ) / cellsPerTyle;

	tex = R3D_MIN( tex, tdesc.CellCountX / cellsPerTyle - 1 );
	tez = R3D_MIN( tez, tdesc.CellCountZ / cellsPerTyle - 1 );

	const int MASKTILEDIM = qs.MaskAtlasTileDim;

	if( dxii.Width % MASKTILEDIM || dxii.Height % MASKTILEDIM )
	{
		char msg[ 256 ];
		sprintf( msg, "Imported texture dimensions must be multiples of %d!", MASKTILEDIM );
		MessageBox( r3dRenderer->HLibWin, msg, "Error", MB_OK );
		return 0;
	}

	// need to check algorithm if this changes!
	COMPILE_ASSERT( r3dTerrain3::MEGA_TEX_BORDER == 4 );

	r3d_assert( !( cellOffsetX % MASKTILEDIM ) && !( cellOffsetZ % MASKTILEDIM ) );

	int tileCountX = dxii.Width / MASKTILEDIM;
	int tileCountZ = dxii.Height / MASKTILEDIM;

	int shrinkedCellSize = ( MASKTILEDIM - 2 * r3dTerrain3::MEGA_TEX_BORDER );

	int scaledWidth = tileCountX * shrinkedCellSize;
	int scaledHeight = tileCountZ * shrinkedCellSize;

	IDirect3DTexture9* texture( NULL );
	if( D3DXCreateTextureFromFileEx( r3dRenderer->pd3ddev, path, scaledWidth, scaledHeight, 1, 0, D3DFMT_L8, D3DPOOL_SYSTEMMEM, D3DX_FILTER_TRIANGLE, D3DX_FILTER_NONE, 0, NULL, NULL, &texture ) != S_OK )
		return 0;

	D3DLOCKED_RECT lrect;
	D3D_V( texture->LockRect( 0, &lrect, NULL, D3DLOCK_READONLY ) );

	D3DSURFACE_DESC sdesc;
	D3D_V( texture->GetLevelDesc( 0, &sdesc ) );

	r3d_assert( sdesc.Width == scaledWidth && sdesc.Height == scaledHeight );

	r3d_assert( lrect.Pitch == sizeof( char ) * scaledWidth );

	float lastInfoFrame = r3dGetTime();
	const float INFO_DELTA = 0.125f;

	TileCoords tileCoords;

	for( int tz = tsz; tz <= tez; tz ++ )	
	{
		for( int tx = tsx; tx <= tex; tx ++ )
		{
			if( r3dGetTime() - lastInfoFrame > INFO_DELTA )
			{
				lastInfoFrame = r3dGetTime();
				char buf[ 512 ];
				sprintf( buf, "Importing (%d,%d)", tx, tz );
				Terrain3->ReportProgress( buf );
			}

			Terrain3->UnpackMask( &m_TempUShorts, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture, tx, tz, 0, maskId );

			int rawPsx = ( tx + 0 ) * shrinkedCellSize;
			int rawPsz = ( tz + 0 ) * shrinkedCellSize;

			int halfDim = qs.MaskAtlasTileDim / 2;

			for( int destZ = 0, e = qs.MaskAtlasTileDim; destZ < e; destZ ++ )
			{
				int srcZ = destZ + rawPsz - r3dTerrain3::MEGA_TEX_BORDER;

				int finZ = srcZ - cellOffsetZ;

				if( finZ < 0 || finZ >= (int)scaledHeight )
					continue;

				for( int destX = 0, e = qs.MaskAtlasTileDim; destX < e; destX ++ )
				{
					int srcX = destX + rawPsx - r3dTerrain3::MEGA_TEX_BORDER;

					int finX = srcX - cellOffsetX;

					if( finX < 0 || finX >= (int)scaledWidth )
						continue;

					int destIdx = destX + destZ * qs.MaskAtlasTileDim;

					UINT16& destSample = m_TempUShorts[ destIdx ];

					UINT8 finalSample = *( (char*)lrect.pBits + ( sdesc.Height - finZ - 1 )* lrect.Pitch + finX );

					int channeledSample = 0;

					switch( channelId )
					{
					case 0:
						channeledSample = finalSample * 31 / 255;
						destSample &= ~( 0x1f << 11 );
						destSample |= channeledSample << 11;
						break;
					case 1:
						channeledSample = finalSample * 63 / 255;
						destSample &= ~( 0x3f << 5 );
						destSample |= channeledSample << 5;
						break;
					case 2:
						channeledSample = finalSample * 31 / 255;
						destSample &= ~0x1f;
						destSample |= channeledSample << 0;
						break;
					}
				}
			}

			Terrain3->UpdateMask( &m_TempUShorts, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture, tx, tz, 0, maskId );
			Terrain3->InsertLayerInfo( tx, tz, layerIndex + 1 );

			UpdateLayerMaskMipFromTemps( tx, tz, maskId, &tileCoords );
		}
	}

	RecalcTileCoordBorders( tileCoords );

	for( int tz = tsz; tz <= tez; tz ++ )	
	{
		for( int tx = tsx; tx <= tex; tx ++ )
		{
			for( int i = 0, e = Terrain3->GetInfo().NumActiveMegaTexLayers, ttx = tx, ttz = tz; i < e; i ++, ttx >>= 1, ttz >>= 1 )
			{
				Terrain3->RefreshMaskAtlasTile( ttx, ttz, i, r3dTerrain3::RM_RELOAD );
			}
		}
	}

	D3D_V( texture->UnlockRect( 0 ) );

	//------------------------------------------------------------------------

	FinishTasksWithReporting();

	r3dRenderer->StartFrame();
	d3dc.Reset();
	r3dRenderer->StartRender( 0 );

	return 1;
}

//------------------------------------------------------------------------

int Terrain3Editor::ImportLayer_RawTiles( const char* path, int layerIndex )
{
	r3dRenderer->EndFrame();
	r3dRenderer->EndRender( true );

	FinishTasksWithReporting();

	CreateTempTextures();

	const r3dTerrain3::QualitySettings& qs = Terrain3->GetCurrentQualitySettings();
	const r3dTerrainDesc& tdesc = Terrain3->GetDesc();

	int maskId = layerIndex / r3dTerrain3::LAYERS_PER_MASK;
	int channelId = layerIndex % r3dTerrain3::LAYERS_PER_MASK;

	int cellsPerTyle = qs.MaskAtlasTileDim;

	r3d_assert( cellsPerTyle == qs.HeightNormalAtlasTileDim );

	const int MASKTILEDIM = qs.MaskAtlasTileDim;

	r3dFile* fin = r3d_open( path, "rb" );

	struct CloseOnExit
	{
		~CloseOnExit()
		{
			fclose( file );
		}

		r3dFile* file;
	} closeOnExit; (void)closeOnExit;

	closeOnExit.file = fin;

	float lastInfoFrame = r3dGetTime();
	const float INFO_DELTA = 0.125f;

	TileCoords tileCoords;

	INT32 tileCount;
	int n = fread( &tileCount, sizeof tileCount, 1, fin );

	if( n != 1 )
		return 0;

	tileCoords.Reserve( tileCount * 2 );

	r3dTerrain3::Info info = Terrain3->GetInfo();

	int tileCountX = info.MegaTileCountX;
	int tileCountZ = info.MegaTileCountZ;

	for( int L = 0, e = info.NumActiveMegaTexLayers; L < e; L ++ )
	{
		for( int tz = 0; tz < tileCountZ; tz ++ )
		{
			for( int tx = 0; tx < tileCountX; tx ++ )
			{
				if( Terrain3->HasMask( tx, tz, L, maskId ) )
				{
					if( r3dGetTime() - lastInfoFrame > INFO_DELTA )
					{
						lastInfoFrame = r3dGetTime();
						char buf[ 512 ];
						sprintf( buf, "Cleaning (%d,%d,%d,%d)", tx, tz, L, maskId );
						Terrain3->ReportProgress( buf );
					}

					Terrain3->UnpackMask( &m_TempUShorts, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture, tx, tz, L, maskId );

					for( int i = 0, e = m_TempUShorts.Count(); i < e; i ++ )
					{
						UINT16& destSample = m_TempUShorts[ i ];

						switch( channelId )
						{
						case 0:
							destSample &= ~( 0x1f << 11 );
							break;
						case 1:
							destSample &= ~( 0x3f << 5 );
							break;
						case 2:
							destSample &= ~0x1f;
							break;
						}
					}

					Terrain3->UpdateMask( &m_TempUShorts, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture, tx, tz, L, maskId );
				}
			}
		}
	}

	for( int tile = 0; tile < tileCount; tile ++ )	
	{
		if( r3dGetTime() - lastInfoFrame > INFO_DELTA )
		{
			lastInfoFrame = r3dGetTime();
			char buf[ 512 ];
			sprintf( buf, "Importing %d of %d", tile, tileCount );
			Terrain3->ReportProgress( buf );
		}

		INT32 tx( 0 ), tz( 0 );

		int n = 0;
		n = fread( &tx, sizeof tx, 1, fin ); if( n != 1 ) return 0;

		n = fread( &tz, sizeof tz, 1, fin ); if( n != 1 ) return 0;

		for( int i = 0, e = info.NumActiveMegaTexLayers; i < e; i ++ )
		{
			TileCoord tc;

			tc.X = tx >> i;
			tc.Z = tz >> i;
			tc.L = i;
			tc.M = maskId;

			tileCoords.PushBack( tc );
		}

		Terrain3->UnpackMask( &m_TempUShorts, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture, tx, tz, 0, maskId );

		m_TempBytes.Resize( m_TempUShorts.Count() );

		n = fread( &m_TempBytes[ 0 ], m_TempBytes.Count() * sizeof m_TempBytes[ 0 ], 1, fin );
		r3d_assert( n == 1 );

		for( int destZ = 0, e = qs.MaskAtlasTileDim; destZ < e; destZ ++ )
		{
			for( int destX = 0, e = qs.MaskAtlasTileDim; destX < e; destX ++ )
			{
				int destIdx = destX + destZ * qs.MaskAtlasTileDim;

				UINT16& destSample = m_TempUShorts[ destIdx ];

				UINT8 sourceSample = m_TempBytes[ destIdx ];

				// NOTE: channels 0 and 2 were offset by 1 bit in export function, hence they are corrected
				// by 1 bit here
				switch( channelId )
				{
				case 0:
					destSample &= ~( 0x1f << 11 );
					destSample |= sourceSample >> 1 << 11;
					break;
				case 1:
					destSample &= ~( 0x3f << 5 );
					destSample |= sourceSample << 5;
					break;
				case 2:
					destSample &= ~0x1f;
					destSample |= sourceSample >> 1;
					break;
				}
			}
		}

		Terrain3->UpdateMask( &m_TempUShorts, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture, tx, tz, 0, maskId );
		Terrain3->InsertLayerInfo( tx, tz, layerIndex + 1 );

		UpdateLayerMaskMipFromTemps( tx, tz, maskId, &tileCoords );
	}

	RecalcTileCoordBorders( tileCoords );

	//------------------------------------------------------------------------

	FinishTasksWithReporting();

	r3dRenderer->StartFrame();
	d3dc.Reset();
	r3dRenderer->StartRender( 0 );

	return 1;
}

//------------------------------------------------------------------------

int Terrain3Editor::ExportLayer( const char* path, int layerIndex, int cellOffsetX, int cellOffsetZ, int texSizeX, int texSizeZ )
{
	CreateTempTextures();

	int success = 0;

	int maskId = layerIndex / r3dTerrain3::LAYERS_PER_MASK;
	int channelId = layerIndex % r3dTerrain3::LAYERS_PER_MASK;

	const r3dTerrain3::QualitySettings& qs = Terrain3->GetCurrentQualitySettings();
	const r3dTerrainDesc& tdesc = Terrain3->GetDesc();

	IDirect3DTexture9* texture( NULL );

	if( r3dRenderer->pd3ddev->CreateTexture( texSizeX, texSizeZ, 1, 0, D3DFMT_L8, D3DPOOL_SYSTEMMEM, &texture, NULL ) != S_OK )
	{
		return 0;
	}

	int cellsPerTile = qs.MaskAtlasTileDim;
	int halfCellsPerTile = cellsPerTile / 2;

	int tsx = cellOffsetX / cellsPerTile;
	int tsz = cellOffsetZ / cellsPerTile;

	int tex = ( cellOffsetX + texSizeX ) / cellsPerTile;
	int tez = ( cellOffsetZ + texSizeZ ) / cellsPerTile;

	tex = R3D_MIN( tex, tdesc.CellCountX / cellsPerTile - 1 );
	tez = R3D_MIN( tez, tdesc.CellCountZ / cellsPerTile - 1 );

	D3DLOCKED_RECT lrect;
	D3D_V( texture->LockRect( 0, &lrect, NULL, D3DLOCK_READONLY ) );

	memset( lrect.pBits, 0, lrect.Pitch * texSizeZ );

	float padding = Terrain3->GetMaskPaddingCoef();

	for( int tz = tsz; tz <= tez; tz ++ )	
	{
		for( int tx = tsx; tx <= tex; tx ++ )
		{
			if( !Terrain3->HasMask( tx, tz, 0, maskId ) )
				continue;

			Terrain3->UnpackMask( &m_TempUShorts, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture, tx, tz, 0, maskId );

			for( int z = 0, e = cellsPerTile; z < e; z ++ )
			{
				for( int x = 0, e = cellsPerTile; x < e; x ++ )
				{
					int tax = x + tx * cellsPerTile - cellOffsetX;
					int taz = z + tz * cellsPerTile - cellOffsetZ;

					if( tax < 0 || taz < 0 || tax >= texSizeX || taz >= texSizeZ )
						continue;

					int lx = int( ( x - halfCellsPerTile ) / padding + halfCellsPerTile );
					int lz = int( ( z - halfCellsPerTile ) / padding + halfCellsPerTile );

					r3d_assert( lx >= 0 && lz >= 0 && lx < cellsPerTile && lz < cellsPerTile );

					UINT16 sample16 = m_TempUShorts[ lx + lz * cellsPerTile ];
					UINT8 sample = 0;

					switch( channelId )
					{
					case 0:
						sample = ( sample16 >> 11 & 0x1f ) * 255 / 31;
						break;
					case 1:
						sample = ( sample16 >> 5 & 0x3f ) * 255 / 63;
						break;
					case 2:
						sample = ( sample16 >> 0 & 0x1f ) * 255 / 31;
						break;
					}

					*((UINT8*)lrect.pBits + ( texSizeZ - taz - 1 )* lrect.Pitch + tax ) = sample;
				}
			}
		}
	}

	D3D_V( texture->UnlockRect( 0 ) );

	success = D3DXSaveTextureToFile( path, D3DXIFF_DDS, texture, NULL ) == S_OK;
	SAFE_RELEASE( texture );

	return success;
}

//------------------------------------------------------------------------

int Terrain3Editor::ExportLayer_RawTiles( const char* path, int layerIndex )
{
	CreateTempTextures();

	int maskId = layerIndex / r3dTerrain3::LAYERS_PER_MASK;
	int channelId = layerIndex % r3dTerrain3::LAYERS_PER_MASK;

	const r3dTerrain3::QualitySettings& qs = Terrain3->GetCurrentQualitySettings();
	const r3dTerrainDesc& tdesc = Terrain3->GetDesc();

	FILE* fout = fopen( path, "wb" );

	int cellsPerTile = qs.MaskAtlasTileDim;
	int halfCellsPerTile = cellsPerTile / 2;

	r3dTerrain3::Info info = Terrain3->GetInfo();

	INT32 tileCount = 0;

	for( int tz = 0; tz < info.MegaTileCountZ; tz ++ )	
	{
		for( int tx = 0; tx < info.MegaTileCountX; tx ++ )
		{
			if( Terrain3->HasMask( tx, tz, 0, maskId ) )
				tileCount ++;
		}
	}

	fwrite( &tileCount, sizeof tileCount, 1, fout );

	for( int tz = 0; tz < info.MegaTileCountZ; tz ++ )	
	{
		for( int tx = 0; tx < info.MegaTileCountX; tx ++ )
		{
			if( !Terrain3->HasMask( tx, tz, 0, maskId ) )
				continue;

			Terrain3->UnpackMask( &m_TempUShorts, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture, tx, tz, 0, maskId );

			INT32 tx32 = tx;
			INT32 tz32 = tz;

			fwrite( &tx32, sizeof tx32, 1, fout );
			fwrite( &tz32, sizeof tz32, 1, fout );

			m_TempBytes.Clear();
			m_TempBytes.Resize( m_TempUShorts.Count(), 0 );

			for( int i = 0, e = cellsPerTile * cellsPerTile; i < e; i ++ )
			{
				int sample = m_TempUShorts[ i ];
				UINT8 byte = 0;

				switch( channelId )
				{
				case 0:
					byte = UINT8( sample >> 11 & 0x1f) << 1;
					break;
				case 1:
					byte = UINT8( sample >> 5 & 0x3f );
					break;
				case 2:
					byte = UINT8( sample >> 0 & 0x1f ) << 1;
					break;
				}

				m_TempBytes[ i ] = byte;
			}

			fwrite( &m_TempBytes[ 0 ], m_TempBytes.Count() * sizeof m_TempBytes[ 0 ], 1, fout );
		}
	}

	fclose( fout );

	return 1;
}

//------------------------------------------------------------------------

int Terrain3Editor::SwapLayers( int layerIndex0, int layerIndex1 )
{
	char tempFilePath0[ 1024 ] = { 0 };
	char tempFilePath1[ 1024 ] = { 0 };

	GetTempPath( sizeof tempFilePath0 - 1, tempFilePath0 );
	strcat( tempFilePath0, "\\layer0.dat" );

	GetTempPath( sizeof tempFilePath1 - 1, tempFilePath1 );
	strcat( tempFilePath1, "\\layer1.dat" );

	int res = 0x7ffffff;

	if( res ) res &= ExportLayer_RawTiles( tempFilePath0, layerIndex0 );
	if( res ) res &= ExportLayer_RawTiles( tempFilePath1, layerIndex1 );

	if( res )
	{
		Terrain3->ClearLayerInfo( layerIndex0 );
		Terrain3->ClearLayerInfo( layerIndex1 );
	}

	if( res ) res &= ImportLayer_RawTiles( tempFilePath1, layerIndex0 );
	if( res ) res &= ImportLayer_RawTiles( tempFilePath0, layerIndex1 );

	if( res ) Terrain3->RefreshAllMaskAtlasTiles();
	if( res ) Terrain3->SwapLayers( layerIndex0, layerIndex1 );

	return res;
}

//------------------------------------------------------------------------

void
Terrain3Editor::RecalcTileHeightMipChain( int tileX, int tileZ, UShortsPtrArr& datas )
{
	R3DPROFILE_FUNCTION( "r3dTerrain3::RecalcTileHeightMipChain" );

	const r3dTerrain3::QualitySettings& curQS = Terrain3->GetCurrentQualitySettings();

	int fullDim = curQS.HeightNormalAtlasTileDim;

	int prevSX = 0;
	int prevSZ = 0;

	for( int i = 1, e = (int)datas.Count(), dim = fullDim / 2; 
			i < e; 
			i ++, dim /= 2 )
	{
		int inParentX = tileX % ( 1 << i );
		int inParentZ = tileZ % ( 1 << i );

		UShorts& src = *datas[ i - 1 ];
		UShorts& dest = *datas[ i ];

		int sx = inParentX * dim;
		int sz = inParentZ * dim;

		for( int z = sz, e = z + dim; z < e; z ++ )
		{
			for( int x = sx, e = x + dim; x < e; x ++ )
			{
				int cx = ( x - sx ) * 2 + prevSX;
				int cz = ( z - sz ) * 2 + prevSZ;

				UINT32 sum = src[ cx + cz * fullDim ];

				sum += src[ ( cx + 1 ) + ( cz + 0 ) * fullDim ];
				sum += src[ ( cx + 0 ) + ( cz + 1 ) * fullDim ];
				sum += src[ ( cx + 1 ) + ( cz + 1 ) * fullDim ];

				sum /= 4;

				dest[ x + z * fullDim ] = sum;
			}
		}

		prevSX = sx;
		prevSZ = sz;
	}
}

//------------------------------------------------------------------------

void
Terrain3Editor::UpdateHeightRect( const RECT& rc )
{
	CHeightChanged3 * pUndoItem = ( CHeightChanged3 * ) m_UndoItem;

	pUndoItem->AddRectUpdate( rc );
}

//------------------------------------------------------------------------

void Terrain3Editor::RecalcNormalEdges()
{
#ifndef ARTIFICIAL_FINAL_BUILD
	r3dTerrain3::Info info = Terrain3->GetInfo();

	for( int L = 0, e = info.NumActiveMegaTexLayers - 1; L < e; L ++ )
	{
		int tileSizeX = info.MegaTileCountX >> L;
		int tileSizeZ = info.MegaTileCountZ >> L;

		for( int tz = 0, te = tileSizeZ; tz < te; tz ++ )
		{
			for( int tx = 0, te = tileSizeX; tx < te; tx ++ )
			{
				Terrain3->UpdateTileNormalEdges( tx, tz, L, &m_TempUShorts, &m_TempUShorts2, &m_TempUShorts3 );
				Terrain3->RefreshHeightNormalAtlasTile( tx, tz, L, r3dTerrain3::LOADTILE_NORMAL );
			}
		}
	}
#endif
}

//------------------------------------------------------------------------

void
Terrain3Editor::BeginUndoRecord( const char * title, UndoAction_e eAction )
{
	m_InUndoRecord = 1;

	if( eAction == UA_TERRAIN3_MASK_PAINT || eAction == UA_TERRAIN3_HEIGHT )
	{
		m_UndoItem = g_pUndoHistory->CreateUndoItem( eAction );
		assert( m_UndoItem );
		m_UndoItem->SetTitle( title );
	}
}

//------------------------------------------------------------------------

void
Terrain3Editor::EndUndoRecord()
{
	m_InUndoRecord = 0;
	m_UndoItem = NULL;
}

//------------------------------------------------------------------------

int Terrain3Editor::IsHeightDirty() const
{
	return m_HeightDirty;
}

//------------------------------------------------------------------------

int Terrain3Editor::GetMaskCount() const
{
	int layerCount = Terrain->GetDesc().LayerCount - 1;
	return layerCount / r3dTerrain3::LAYERS_PER_MASK + ( layerCount % r3dTerrain3::LAYERS_PER_MASK ? 1 : 0 );
}

//------------------------------------------------------------------------

INT64 Terrain3Editor::MakeCachedTileKey( int X, int Z, int L, int MaskID )
{
	INT64 key = 0;

	INT64 bits = 0;

	key |= INT64( X ) << bits; bits += 16;
	key |= INT64( Z ) << bits; bits += 16;
	key |= INT64( L ) << bits; bits += 8;
	key |= INT64( MaskID ) << bits; bits += 8;

	return key;
}

//------------------------------------------------------------------------

void Terrain3Editor::CreateTempTextures()
{
	if( !m_TempUnpackedMaskTexture )
	{
		const r3dTerrain3::QualitySettings& qs = Terrain3->GetCurrentQualitySettings();
		D3D_V( r3dRenderer->pd3ddev->CreateTexture( qs.MaskAtlasTileDim, qs.MaskAtlasTileDim, 1, 0, D3DFMT_R5G6B5, D3DPOOL_SYSTEMMEM, &m_TempUnpackedMaskTexture, NULL ) );
	}

	if( !m_TempPackedMaskTexture )
	{
		const r3dTerrain3::QualitySettings& qs = Terrain3->GetCurrentQualitySettings();

		D3DFORMAT fmt = Terrain3->GetInfo().MaskFormat;

		D3D_V( r3dRenderer->pd3ddev->CreateTexture( qs.MaskAtlasTileDim, qs.MaskAtlasTileDim, 1, 0, fmt, D3DPOOL_SYSTEMMEM, &m_TempPackedMaskTexture, NULL ) );
	}	
}

//------------------------------------------------------------------------

void Terrain3Editor::DeleteLayer( int lidx )
{
	CreateTempTextures();
	Terrain3->DeleteLayer( lidx, m_TempPackedMaskTexture, m_TempUnpackedMaskTexture );
}

//------------------------------------------------------------------------

void Terrain3Editor::FinishTasksWithReporting()
{
	if( g_pBackgroundTaskDispatcher )
	{
		float lastInfoFrame = r3dGetTime();
		const float INFO_DELTA = 0.125f;

		for( ; g_pBackgroundTaskDispatcher->GetTaskCount(); )
		{
			// loading tasks may issude d3d commands into main thread
			ProcessDeviceQueue( r3dGetTime(), INFO_DELTA * 0.5f );

			if( r3dGetTime() - lastInfoFrame > INFO_DELTA )
			{
				lastInfoFrame = r3dGetTime();
				char buf[ 512 ];
				sprintf( buf, "Finishing tasks (%d)", g_pBackgroundTaskDispatcher->GetTaskCount() );
				Terrain3->ReportProgress( buf );
			}
		}
	}
}

//------------------------------------------------------------------------

Terrain3Editor* g_pTerrain3Editor;

//------------------------------------------------------------------------

Terrain3Editor::UnpackedMaskTile::UnpackedMaskTile()
: X( 0 )
, Z( 0 )
, L( 0 )
, MaskId( -1 )
, Freshness( 0 )
, Synced( 1 )
{

}

//------------------------------------------------------------------------

Terrain3Editor::UnpackedHeightTile::UnpackedHeightTile()
: X( 0 )
, Z( 0 )
, L( 0 )
, Freshness( 0 )
, Synced( 1 )
{

}

//------------------------------------------------------------------------

Terrain3Editor::NoiseParams::NoiseParams()
{
	maxHeight         = 100.0f;
	minHeight         = 0.0;
	heightApplySpeed  = 1.0f;
	heightRelative    = 0;
}

//------------------------------------------------------------------------

Terrain3Editor::NoiseParams::~NoiseParams()
{
	
}

//------------------------------------------------------------------------

float
Terrain3Editor::NoiseParams::GetNoise( int x, int y )
{
	float h = noise.PerlinNoise_2D( (float)x, (float)y );

	h += 1.0f / 2.0f; // convert to [0..1];

	h = minHeight + h * (maxHeight - minHeight);

	return h;
}

//------------------------------------------------------------------------

void
Terrain3Editor::NoiseParams::Apply( const r3dPoint3D& pos, float radius, float hardiness )
{
	struct
	{
		float operator() ( float fOldH, float gathered, float coef, int x, int z )
		{
			float h2 = parent->GetNoise( x, z );

			float c = R3D_MIN( R3D_MAX( coef, 0.f ), 1.f );

			float h;
			
			if( parent->heightRelative )
				h = fOldH + ( h2 - fOldH + Terrain3->GetHeight( x, z ) ) * coef;
			else
				h = fOldH + ( h2 - fOldH ) * coef;

			return h;
		}

		NoiseParams* parent;
	} heightFunc = { this };

	g_pTerrain3Editor->ApplyHeightBrushT( r3dPoint3D( pos.X, 0.f, pos.Z ), 1.0f, radius, hardiness, heightFunc, (DefaultGatherFunc)NULL, 0 );

	return;
}

//------------------------------------------------------------------------

void CHeightChanged3::Release()
{
	PURE_DELETE( this ); 
}

//------------------------------------------------------------------------

UndoAction_e CHeightChanged3::GetActionID()
{
	return ms_eActionID;
}

//------------------------------------------------------------------------

void CHeightChanged3::UndoRedo( bool redo )
{
	typedef r3dTL::TArray< UINT16 > UShorts;
	typedef r3dTL::TArray< UShorts* > UShortsPtrArr;

	UShortsPtrArr ushorts;

	typedef std::set< Terrain3Editor::UnpackedHeightTile* > UnpackedHeightTileSet;
	UnpackedHeightTileSet uhts;

	const r3dTerrain3::Info& info = Terrain3->GetInfo();
	const r3dTerrain3::QualitySettings& qs = Terrain3->GetCurrentQualitySettings();

	Terrain3Editor::UnpackedHeightTile* uht = NULL;

	int curTileX = -1;
	int curTileZ = -1;

	const r3dTerrainDesc& tdesc = Terrain3->GetDesc();

	r3dFinishBackGroundTasks();
	g_pTerrain3Editor->StartHeightEditing();
	{
		int tileCountX = info.MegaTileCountX;
		int tileCountZ = info.MegaTileCountZ;

		int tileResX = tileCountX * qs.HeightNormalAtlasTileDim;
		int tileResZ = tileCountZ * qs.HeightNormalAtlasTileDim;

		float heightCoef = ( tdesc.MaxHeight - tdesc.MinHeight ) / 65535.f;

		for( int i =	redo ?		0 : DataArr.Count() - 1; 
						redo ?		i < (int)DataArr.Count() : i >= 0;
						redo ? 		i ++ : i-- )
		{
			const UndoHeight_t &data = DataArr[ i ];
			r3d_assert( data.X >= 0 && data.X < tileResX
							&&
						data.Z >= 0 && data.Z < tileResZ );

			int tileX = data.X / qs.HeightNormalAtlasTileDim;
			int tileZ = data.Z / qs.HeightNormalAtlasTileDim;

			if( tileX != curTileX || tileZ != curTileZ )
			{
				uht = g_pTerrain3Editor->GetUnpackedHeightTile( tileX, tileZ, 0 );
				uhts.insert( uht );

				uht->Synced = 0;

				curTileX = tileX;
				curTileZ = tileZ;
			}

			r3d_assert( uht );

			int lx = data.X % qs.HeightNormalAtlasTileDim;
			int lz = data.Z % qs.HeightNormalAtlasTileDim;

			int idx = lx + lz * qs.HeightNormalAtlasTileDim;

			uht->Data[ idx ] = redo ? data.CurrHeight : data.PrevHeight;
			uht->FloatData[ idx ] = uht->Data[ idx ] * heightCoef + tdesc.MinHeight;
		}
	}


	for( UnpackedHeightTileSet::iterator i = uhts.begin(), e = uhts.end();
										i != e;
										++ i )
	{
		Terrain3Editor::UnpackedHeightTile* uht = *i;

		ushorts.Clear();
		ushorts.PushBack( &uht->Data );

		for( int i = 1, e = info.NumActiveMegaTexLayers, ttz = uht->Z / 2, ttx = uht->X / 2; i < e; i ++, ttz /= 2, ttx /= 2 )
		{
			Terrain3Editor::UnpackedHeightTile* uht = g_pTerrain3Editor->GetUnpackedHeightTile( ttx, ttz, i );
			uht->Synced = 0;
			ushorts.PushBack( &uht->Data );
		}

		g_pTerrain3Editor->RecalcTileHeightMipChain( uht->X, uht->Z, ushorts );
	}
	g_pTerrain3Editor->EndHeightEditing();
	r3dFinishBackGroundTasks();
}

//------------------------------------------------------------------------

void
CHeightChanged3::Undo()
{
	UndoRedo( false );
}

//------------------------------------------------------------------------

void
CHeightChanged3::Redo()
{
	UndoRedo( true );
}

//------------------------------------------------------------------------

void
CHeightChanged3::AddData( const UndoHeight_t & data )
{
	DataArr.PushBack( data );
}

//------------------------------------------------------------------------

void
CHeightChanged3::AddRectUpdate	( const RECT &rc )				
{
	if ( m_rc.bottom > rc.bottom )
		m_rc.bottom = rc.bottom;
	if ( m_rc.left > rc.left ) 
		m_rc.left = rc.left;
	if ( m_rc.top < rc.top ) 
		m_rc.top = rc.top;
	if ( m_rc.right < rc.right ) 
		m_rc.right = rc.right;
}

//------------------------------------------------------------------------

CHeightChanged3::CHeightChanged3()
{
	memset( &m_rc, 0, sizeof m_rc );
}

//------------------------------------------------------------------------
/*static*/

IUndoItem *
CHeightChanged3::CreateUndoItem	()
{
	return game_new CHeightChanged3;
}

//------------------------------------------------------------------------
/*static*/
void
CHeightChanged3::Register()
{
	UndoAction_t action;
	action.nActionID = ms_eActionID;
	action.pCreateUndoItem = CreateUndoItem;

	g_pUndoHistory->RegisterUndoAction( action );
}

//------------------------------------------------------------------------

void
CLayerMaskPaint3::Release()
{ 
	for ( int i = 0; i < (int)m_pData.Count(); i++ )
	{
		SAFE_DELETE_ARRAY( m_pData[ i ].pData );
	}
	PURE_DELETE( this ); 
};

//------------------------------------------------------------------------

UndoAction_e
CLayerMaskPaint3::GetActionID()
{
	return ms_eActionID;
}

//------------------------------------------------------------------------

void
CLayerMaskPaint3::UndoRedo( bool redo )
{
	// don't undo pointless stuff
	g_pTerrain3Editor->StartLayerBrush_G( LayerIdx ); 

	r3d_assert( LayerIdx > 0 );

	for( int i = 		
				redo ?
					0
					:
					m_pData.Count() - 1; 

				redo ?
					i < (int)m_pData.Count()
					:
					i >= 0; 

				redo ?
					i ++
					:
					i -- 		
				) 
	{
		const PaintData_t &data = m_pData[ i ];

		r3d_assert( data.rc.right > data.rc.left );
		r3d_assert( data.rc.bottom > data.rc.top );

		int xSize = data.rc.right - data.rc.left;
		int zSize = data.rc.bottom - data.rc.top;

		g_pTerrain3Editor->ApplyRect( data.rc.left, data.rc.top, xSize, zSize, data.L, redo ? data.pData + xSize * zSize : data.pData );
	}

	g_pTerrain3Editor->EndLayerBrush_G();
}

//------------------------------------------------------------------------
void
CLayerMaskPaint3::Undo()
{
	UndoRedo( false );
}

//------------------------------------------------------------------------

void
CLayerMaskPaint3::Redo()
{
	UndoRedo( true );
}

//------------------------------------------------------------------------

void
CLayerMaskPaint3::AddData( const PaintData_t & data )
{
	m_pData.PushBack( data );
}

//------------------------------------------------------------------------

void
CLayerMaskPaint3::AddRectUpdate	( const RECT &rc )				
{
	if ( m_rc.bottom < rc.bottom ) 
		m_rc.bottom = rc.bottom;
	if ( m_rc.left > rc.left ) 
		m_rc.left = rc.left;
	if ( m_rc.top > rc.top ) 
		m_rc.top = rc.top;
	if ( m_rc.right < rc.right ) 
		m_rc.right = rc.right;
}

//------------------------------------------------------------------------

CLayerMaskPaint3::PaintData_t::PaintData_t()
: pData( 0 )
, L ( 0 )
{

}

//------------------------------------------------------------------------

CLayerMaskPaint3::PaintData_t::~PaintData_t()
{

}

//------------------------------------------------------------------------

CLayerMaskPaint3::CLayerMaskPaint3()
{
	LayerIdx = -1;

	m_sTitle = "Paint mask";

	m_rc.top = m_rc.left = INT_MAX;
	m_rc.bottom = m_rc.right = -INT_MAX;
}

//------------------------------------------------------------------------
/*static*/

IUndoItem* CLayerMaskPaint3::CreateUndoItem	()
{
	return game_new CLayerMaskPaint3;
};

//------------------------------------------------------------------------
/*static*/

void
CLayerMaskPaint3::Register()
{
	UndoAction_t action;
	action.nActionID = ms_eActionID;
	action.pCreateUndoItem = CreateUndoItem;
	g_pUndoHistory->RegisterUndoAction( action );
}

//------------------------------------------------------------------------

void
CLayerColorPaint3::Release()
{
	PURE_DELETE( this );
}

//------------------------------------------------------------------------

UndoAction_e
CLayerColorPaint3::GetActionID()
{
	return ms_eActionID;
}

//------------------------------------------------------------------------

void CLayerColorPaint3::UndoRedo	( bool redo )
{
#if 0
	g_pTerrain3Editor->StartColorBrush();

	uint32_t* pColor = (uint32_t*)g_pTerrain3Editor->m_ColorTex->Lock( 1, NULL );

	for( int i = 		
	redo ?
		0
		:
		m_pData.Count() - 1; 

	redo ?
		i < (int)m_pData.Count()
		:
		i >= 0; 

	redo ?
		i ++
		:
		i -- 		
		) 
	{
		const UndoColor_t &data = m_pData[ i ];
		r3d_assert( data.nIndex >= 0 && data.nIndex < m_CellCount );
		pColor[ data.nIndex ] = redo ? data.dwCurrColor : data.dwPrevColor;
	}

	g_pTerrain3Editor->m_ColorTex->Unlock();

	g_pTerrain3Editor->EndColorBrush();

	Terrain3->RefreshAtlasTiles();	
#else
	r3dError("CLayerColorPaint3::UndoRedo: implement me!");
#endif
}

//------------------------------------------------------------------------

void
CLayerColorPaint3::Undo()
{
	UndoRedo( false );
}

//------------------------------------------------------------------------

void
CLayerColorPaint3::Redo()
{
	UndoRedo( true );
}

//------------------------------------------------------------------------

void
CLayerColorPaint3::AddData( const UndoColor_t & data )
{
	r3d_assert( data.nIndex < m_CellCount );

	m_pData.PushBack( data );
}

//------------------------------------------------------------------------

void CLayerColorPaint3::AddRectUpdate( const RECT &rc )
{
	if ( m_rc.bottom > rc.bottom ) 
		m_rc.bottom = rc.bottom;
	if ( m_rc.left > rc.left ) 
		m_rc.left = rc.left;
	if ( m_rc.top < rc.top ) 
		m_rc.top = rc.top;
	if ( m_rc.right < rc.right ) 
		m_rc.right = rc.right;
}

//------------------------------------------------------------------------

CLayerMaskEraseAll3::PaintData_t::PaintData_t()
{

}

//------------------------------------------------------------------------

void CLayerMaskEraseAll3::Release()
{
	PURE_DELETE( this );
}

//------------------------------------------------------------------------

UndoAction_e CLayerMaskEraseAll3::GetActionID()
{
	return ms_eActionID;
}

//------------------------------------------------------------------------

void CLayerMaskEraseAll3::UndoRedo( bool redo )
{
	r3dError( "CLayerMaskEraseAll3::UndoRedo: implement me!" );
#if 0
	if( !m_pData.Count() )
		return;

	int first = 1;

	int splatCount = m_pData[ 0 ].masks.Count();

	for( int splatIdx = 0, e = splatCount; splatIdx < e; splatIdx ++, first = 0 )
	{
		g_pTerrain3Editor->StartLayerBrush( splatIdx * r3dTerrain3::LAYERS_PER_MASK + 1 );

		r3dTexture* tex = g_pTerrain3Editor->m_PaintLayerMasks[ splatIdx ];

		for( int i = 		
			redo ?
				0
				:
				m_pData.Count() - 1; 

			redo ?
				i < (int)m_pData.Count()
				:
				i >= 0; 

			redo ?
				i ++
				:
				i -- 		
			) 
		{
			const PaintData_t &data = m_pData[ i ];

			uint16_t* colors = (uint16_t*)tex->Lock( 1, &data.rc ); // fixme:

			int nLine = data.rc.right - data.rc.left;
			int nSize = redo ? nLine * ( data.rc.bottom - data.rc.top ) : 0;

			for( int y = 0; y < data.rc.bottom - data.rc.top; y++ ) 
			{
				uint16_t * pDst = (uint16_t*)&colors[ y * tex->GetWidth() ];
				const uint16_t * pSrc = &data.masks[ splatIdx ][ y * nLine + nSize ];

				r3d_assert( y * nLine + nSize + nLine <= (int)data.masks[ splatIdx ].Count() );

				memcpy( pDst, pSrc, nLine * sizeof( uint16_t ) );
			}

			tex->Unlock();

			if( first )
			{
				Terrain3->RefreshAtlasTiles( data.rc );
			}
		}

		g_pTerrain3Editor->EndLayerBrush();
	}
#endif
}

//------------------------------------------------------------------------

void CLayerMaskEraseAll3::Undo()
{
	UndoRedo( false );
}

//------------------------------------------------------------------------

void CLayerMaskEraseAll3::Redo()
{
	UndoRedo( true );
}

//------------------------------------------------------------------------

void CLayerMaskEraseAll3::AddData( const PaintData_t & data )
{
	m_pData.PushBack( data );
}

//------------------------------------------------------------------------

void CLayerMaskEraseAll3::AddRectUpdate( const RECT &rc )
{
	if ( m_rc.bottom > rc.bottom )
		m_rc.bottom = rc.bottom;
	if ( m_rc.left > rc.left ) 
		m_rc.left = rc.left;
	if ( m_rc.top < rc.top ) 
		m_rc.top = rc.top;
	if ( m_rc.right < rc.right ) 
		m_rc.right = rc.right;
}

//------------------------------------------------------------------------

CLayerMaskEraseAll3::CLayerMaskEraseAll3()
{
	m_sTitle = "Erase all";

	m_rc.top = m_rc.left = INT_MAX;
	m_rc.bottom = m_rc.right = -INT_MAX;	
}

//------------------------------------------------------------------------

IUndoItem* CLayerMaskEraseAll3::CreateUndoItem()
{
	return game_new CLayerMaskEraseAll3;
}

//------------------------------------------------------------------------

void CLayerMaskEraseAll3::Register()
{
	UndoAction_t action;
	action.nActionID = ms_eActionID;
	action.pCreateUndoItem = CreateUndoItem;

	g_pUndoHistory->RegisterUndoAction( action );
}

//------------------------------------------------------------------------

CLayerColorPaint3::CLayerColorPaint3()
{
#if 0
	m_sTitle = "Color paint";

	m_pData.Reserve( 8 );

	m_rc.bottom = m_rc.left = INT_MAX;
	m_rc.top = m_rc.right = -INT_MAX;

	r3dTexture* colTex = Terrain3->GetColorTexture();

	m_CellCount = colTex->GetWidth() * colTex->GetHeight();
#else
	r3dError( "CLayerColorPaint3::CLayerColorPaint3: implement me!" );
#endif

}

//------------------------------------------------------------------------
/*static*/

IUndoItem *
CLayerColorPaint3::CreateUndoItem	()
{
	return game_new CLayerColorPaint3;
};

//------------------------------------------------------------------------
/*static*/

void
CLayerColorPaint3::Register()
{
	UndoAction_t action;
	action.nActionID = ms_eActionID;
	action.pCreateUndoItem = CreateUndoItem;
#ifndef FINAL_BUILD
	g_pUndoHistory->RegisterUndoAction( action );
#endif
}

//------------------------------------------------------------------------

static int LayerToSplat( int layerIdx )
{
	return ( layerIdx - 1 ) / r3dTerrain3::LAYERS_PER_MASK;
}

//------------------------------------------------------------------------


#endif