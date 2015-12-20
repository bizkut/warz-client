#include "r3dPCH.h"
#include "r3d.h"

#if !defined(FINAL_BUILD) && !defined(WO_SERVER)

#include "TrueNature2/Terrain2.h"
#include "TrueNature2/Terrain3.h"

#include "Terrain2Editor.h"

static int LayerToSplat( int layerIdx );

Terrain2Editor::Terrain2Editor()
: m_UndoItem( NULL )
, m_PaintLayerIdx( -1 )
, m_ColorTex( NULL )
, m_HeightDirty( 0 )
{
	CHeightChanged2::Register();
	CLayerMaskPaint2::Register();
	CLayerColorPaint2::Register();
	CTerrain2DestroyLayer::Register();
	CTerrain2InsertLayer::Register();
	CLayerMaskEraseAll2::Register();
}

//------------------------------------------------------------------------

Terrain2Editor::~Terrain2Editor()
{

}

//------------------------------------------------------------------------

void
Terrain2Editor::LoadHeightsFromTerrain()
{
	r3d_assert( Terrain2 );

	const r3dTerrainDesc& desc = Terrain2->GetDesc();

	m_FloatHeights.Resize( desc.CellCountX * desc.CellCountZ );
	m_ShortHeights.Resize( m_FloatHeights.Count() );

	Terrain2->SaveHeightField( &m_FloatHeights );

	Terrain2->ConvertHeightField( &m_ShortHeights, m_FloatHeights, NULL );
}

//------------------------------------------------------------------------

void
Terrain2Editor::SaveHeightsToTerrain()
{
	r3d_assert( Terrain2 );

	Terrain2->UpdateHeightRanges( m_FloatHeights );

	Terrain2->ConvertHeightField( &m_ShortHeights, m_FloatHeights, NULL );

	Terrain2->UpdateGraphicsHeightField( m_ShortHeights, NULL );

	Terrain2->UpdateNormals( m_FloatHeights, &m_TempVectors0, &m_TempVectors1, NULL );

	Terrain2->UpdatePhysicsHeightField( m_ShortHeights );

	Terrain2->RecalcTileField( m_ShortHeights, NULL );

	m_HeightDirty = 0;
}

//------------------------------------------------------------------------

void Terrain2Editor::FinalizeHeightEditing()
{
#if 0
	Terrain2->ConvertHeightField( &m_ShortHeights, m_FloatHeights, NULL );
	Terrain2->UpdatePhysicsHeightField( m_ShortHeights );
#endif
}

//------------------------------------------------------------------------

void
Terrain2Editor::LoadColorsFromTerrain()
{
#if 0
	r3dTexture* colorTex = Terrain2->GetColorTexture();

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
Terrain2Editor::AreHeightsLoaded() const
{
	return (int)m_FloatHeights.Count();
}

//------------------------------------------------------------------------

void
Terrain2Editor::ApplyHeightBrush( const r3dPoint3D &pnt, const float strength, const float radius, const float hardness )
{ 
	R3DPROFILE_FUNCTION( "Terrain2Editor::ApplyHeightBrush" );

	r3d_assert( Terrain2 );

	m_HeightDirty = 1;

	const r3dTerrainDesc& desc = Terrain2->GetDesc();

	int nc = int(radius / desc.CellSize);	// number of cells in radius
	int nc1 = int((radius*hardness) / desc.CellSize);	// number of cells in radius*hard
	int cx = int(pnt.X / desc.CellSize);	// center cell x
	int cz = int(pnt.Z / desc.CellSize);	// center cell z

	cx = r3dTL::Clamp( cx, 0, desc.CellCountX - 1 );
	cz = r3dTL::Clamp( cz, 0, desc.CellCountZ - 1 );

	// affected area
	int z1 = cz-nc; if( z1 < 0 )				z1 = 0;
	int z2 = cz+nc; if( z2 > desc.CellCountZ )	z2 = desc.CellCountZ;

	int x1 = cx-nc; if( x1 < 0 )				x1 = 0;
	int x2 = cx+nc; if( x2 > desc.CellCountX )	x2 = desc.CellCountX;

	r3d_assert( m_UndoItem );
	r3d_assert( m_UndoItem->GetActionID() == UA_TERRAIN2_HEIGHT );
	CHeightChanged2 * pUndoItem = ( CHeightChanged2 * ) m_UndoItem;

	r3d_assert( m_FloatHeights.Count() );

	for(int z = z1; z < z2; z++)
	{
		for(int x = x1; x < x2; x++)
		{
			int dz = z-cz;
			int dx = x-cx;

			float coef = sqrtf(float(dx*dx+dz*dz)) / float(nc);
			if(coef > 1.0f) continue;

			if (coef <= hardness ) coef = 1.0f;
			else
				coef = 1.0f - (coef - hardness) / (1.0f-hardness);

			int nIndex = z*desc.CellCountX + x;

			float fOld = m_FloatHeights[ nIndex ];

			coef *= strength;

			// do something with h - it will be modified directly
			float h = fOld + coef;
			if (h <0) 
				h = 0;

			m_FloatHeights[ nIndex ] = h;

			CHeightChanged2::UndoHeight_t undo;
			undo.nIndex = nIndex;
			undo.fPrevHeight = fOld;
			undo.fCurrHeight = h;
			pUndoItem->AddData( undo );
		}
	}

	RECT rc;
	rc.left = x1;
	rc.bottom = z1;
	rc.right = x2;
	rc.top = z2;

	UpdateHeightRect( rc );
	
	return;
}

//------------------------------------------------------------------------

void
Terrain2Editor::ApplyHeightLevel(const r3dPoint3D &pnt, const float H, const float strength, const float radius, const float hardness)
{
	m_HeightDirty = 1;

	const r3dTerrainDesc& desc = Terrain2->GetDesc();

	int nc = int(radius / desc.CellSize);	// number of cells in radius
	int nc1 = int((radius*hardness) / desc.CellSize);	// number of cells in radius*hard
	int cx = int(pnt.X / desc.CellSize);	// center cell x
	int cz = int(pnt.Z / desc.CellSize);	// center cell z

	cx = r3dTL::Clamp( cx, 0, desc.CellCountX - 1 );
	cz = r3dTL::Clamp( cz, 0, desc.CellCountZ - 1 );

	// affected area
	int z1 = cz-nc; if(z1 < 0)					z1 = 0;
	int z2 = cz+nc; if(z2 > desc.CellCountZ)	z2 = desc.CellCountZ;
	int x1 = cx-nc; if(x1 < 0)					x1 = 0;
	int x2 = cx+nc; if(x2 > desc.CellCountX)	x2 = desc.CellCountX;

	r3d_assert( m_UndoItem );
	r3d_assert( m_UndoItem->GetActionID() == UA_TERRAIN2_HEIGHT );
	CHeightChanged2 * pUndoItem = ( CHeightChanged2 * ) m_UndoItem;

	for(int z = z1; z < z2; z++)
	{
		for(int x = x1; x < x2; x++)
		{
			int dz = z-cz;
			int dx = x-cx;

			float coef = sqrtf(float(dx*dx+dz*dz)) / float(nc);
			if( coef > 1.0f ) continue;

			if( coef <= hardness ) coef = 1.0f;
			else
				coef = 1.0f - (coef - hardness) / (1.0f-hardness);
			//r3dOutToLog("%d %d\n", x,z);

			int nIndex = z*int(desc.CellCountX) + x;

			float fOld = m_FloatHeights[ nIndex ];

			//  if (coef >Strength) coef = Strength;
			coef *= strength;

			// do something with h - it will be modified directly
			float h1 = H*coef+(1-coef)*fOld;

			if (h1 <0) 
				h1 = 0;

			m_FloatHeights[ nIndex ] = h1;

			CHeightChanged2::UndoHeight_t undo;
			undo.nIndex = nIndex;
			undo.fPrevHeight = fOld;
			undo.fCurrHeight = h1;
			pUndoItem->AddData( undo );
		}
	}

	RECT rc;
	rc.left = x1;
	rc.bottom = z1;
	rc.right = x2;
	rc.top = z2;

	UpdateHeightRect( rc );
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
Terrain2Editor::ApplyHeightSmooth(const r3dPoint3D &pnt, const float radius)
{
	m_HeightDirty = 1;

	const r3dTerrainDesc& desc = Terrain2->GetDesc();

	int nc = int( radius / desc.CellSize );	// number of cells in radius
	int cx = int( pnt.X / desc.CellSize );	// center cell x
	int cz = int( pnt.Z / desc.CellSize );	// center cell z

	cx = r3dTL::Clamp( cx, 0, desc.CellCountX - 1 );
	cz = r3dTL::Clamp( cz, 0, desc.CellCountZ - 1 );

	m_TempFloatHeights.Resize( desc.CellCountX * desc.CellCountZ );

	memcpy( &m_TempFloatHeights[ 0 ], &m_FloatHeights[ 0 ], desc.CellCountX * desc.CellCountZ * sizeof(float) );

	// affected area
	int z1 = cz-nc; if(z1 < 0)					z1 = 0;
	int z2 = cz+nc; if(z2 > desc.CellCountZ )	z2 = desc.CellCountZ;
	int x1 = cx-nc; if(x1 < 0)					x1 = 0;
	int x2 = cx+nc; if(x2 > desc.CellCountX )	x2 = desc.CellCountX;

	float koef = 1.0f;

	extern float _terra_smoothSpeed;
	extern int   _terra_smoothBoxOff;

	if( _terra_smoothSpeed )
	{
		koef = r3dGetFrameTime() / _terra_smoothSpeed;
		koef = R3D_CLAMP( koef, 0.0f, 1.0f );
	}

	r3d_assert( m_UndoItem );
	r3d_assert( m_UndoItem->GetActionID() == UA_TERRAIN2_HEIGHT );
	CHeightChanged2 * pUndoItem = ( CHeightChanged2 * ) m_UndoItem;

	for( int z = z1; z < z2; z++ )
	{
		for(int x = x1; x < x2; x++) 
		{
			int dz = z - cz;
			int dx = x - cx;

			float coef = sqrtf(float(dx*dx+dz*dz)) / float(nc);
			if(coef > 1.0f) continue;

			int nIndex = z * desc.CellCountX + x;

			float h1 = m_FloatHeights[ nIndex ];
			float h2 = ApplyBoxFilter( &m_TempFloatHeights[0], x, z, desc.CellCountX, desc.CellCountZ, _terra_smoothBoxOff );

			m_FloatHeights[ nIndex ] = h1 + (h2 - h1) * koef;

			CHeightChanged2::UndoHeight_t undo;
			undo.nIndex = nIndex;
			undo.fPrevHeight = h1;
			undo.fCurrHeight = m_FloatHeights[ nIndex ];
			pUndoItem->AddData( undo );
		}
	}

	RECT rc;
	rc.left = x1;
	rc.bottom = z1;
	rc.right = x2;
	rc.top = z2;

	UpdateHeightRect( rc );
}

//------------------------------------------------------------------------

void
Terrain2Editor::ApplyHeightErosion(const r3dPoint3D &pnt, const float strength, const float radius, const float hardness)
{
	m_HeightDirty = 1;

	const r3dTerrainDesc& desc = Terrain2->GetDesc();

	int nc = int(radius / desc.CellSize);	// number of cells in radius
	int nc1 = int((radius*hardness) / desc.CellSize);	// number of cells in radius*hard
	int cx = int(pnt.X / desc.CellSize);	// center cell x
	int cz = int(pnt.Z / desc.CellSize);	// center cell z

	cx = r3dTL::Clamp( cx, 0, desc.CellCountX - 1 );
	cz = r3dTL::Clamp( cz, 0, desc.CellCountZ - 1 );

	// affected area
	int z1 = cz-nc; if(z1 < 0)					z1 = 0;
	int z2 = cz+nc; if(z2 > desc.CellCountZ)	z2 = desc.CellCountZ;
	int x1 = cx-nc; if(x1 < 0)					x1 = 0;
	int x2 = cx+nc; if(x2 > desc.CellCountX)	x2 = desc.CellCountX;

	int centerIdx = cz * static_cast<int>(desc.CellCountX) + cx;
	float centerHeight = m_FloatHeights[centerIdx];

	r3d_assert( m_UndoItem );
	r3d_assert( m_UndoItem->GetActionID() == UA_TERRAIN2_HEIGHT );
	CHeightChanged2 * pUndoItem = ( CHeightChanged2 * ) m_UndoItem;

	float minValue = FLT_MAX;
	for(int z = z1; z < z2; z++)
	{
		for(int x = x1; x < x2; x++)
		{
			int nIndex = z*int(desc.CellCountX) + x;
			float f = m_FloatHeights[ nIndex ];
			minValue = R3D_MIN(minValue, f);
		}
	}

	for(int z = z1; z < z2; z++)
	{
		for(int x = x1; x < x2; x++)
		{
			int dz = z-cz;
			int dx = x-cx;

			float vLen = sqrtf(float(dx * dx + dz * dz));

			float coef = vLen / float(nc);
			if(coef > 1.0f) continue;

			if (coef <= hardness ) coef = 1.0f;
			else
				coef = 1.0f - (coef - hardness) / (1.0f-hardness);

			int nIndex = z*int(desc.CellCountX) + x;
			float fOld = m_FloatHeights[ nIndex ];

			int zB = r3dTL::Clamp(z + 1, 0, desc.CellCountZ - 1);
			int xB = r3dTL::Clamp(x + 1, 0, desc.CellCountX - 1);

			int nIndexB = zB * desc.CellCountX + x;
			int nIndexR = z * desc.CellCountX + xB;

			float fOldR = m_FloatHeights[ nIndexR ];
			float fOldB = m_FloatHeights[ nIndexB ];

			float ddyH = fOldB - fOld;
			float ddxH = fOldR - fOld;

			float ddH = R3D_MIN(strength, R3D_MAX(abs(ddyH), abs(ddxH)));

			float u = (x - x1) / static_cast<float>(x2 - x1);
			float v = (z - z1) / static_cast<float>(z2 - z1);
			float c = gErosionPattern->SampleBilinear(u, v);

			float h1 = R3D_MAX(fOld - coef * c * ddH, minValue);
			if (h1 <0) 
				h1 = 0;

			m_FloatHeights[ nIndex ] = h1;

			CHeightChanged2::UndoHeight_t undo;
			undo.nIndex = nIndex;
			undo.fPrevHeight = fOld;
			undo.fCurrHeight = h1;
			pUndoItem->AddData( undo );
		}
	}

	RECT rc;
	rc.left = x1;
	rc.bottom = z1;
	rc.right = x2;
	rc.top = z2;

	UpdateHeightRect( rc );
}

//------------------------------------------------------------------------

void
Terrain2Editor::ApplyHeightNoise(const r3dPoint3D &pnt, const float radius, const float hardness)
{
	m_HeightDirty = 1;

	noiseParams.Apply( pnt, radius, hardness );
}

//------------------------------------------------------------------------

void
Terrain2Editor::ApplyHeightRamp( const r3dPoint3D& rampStart, const r3dPoint3D& rampEnd, const float rampWidthOuter, const float rampWidthInner )
{
	m_HeightDirty = 1;

	const r3dTerrainDesc& desc = Terrain2->GetDesc();

	r3dPoint3D rampStart2D = r3dPoint3D(rampStart.x, 0, rampStart.z);
	r3dPoint3D rampEnd2D   = r3dPoint3D(rampEnd.x,   0, rampEnd.z);
	float      dy          = (rampEnd.y - rampStart.y);

	// detect terrain box for update
	float bbx1, bbx2, bby1, bby2;
	bbx1 = R3D_MIN(rampStart.x, rampEnd.x) - rampWidthOuter;
	bbx2 = R3D_MAX(rampStart.x, rampEnd.x) + rampWidthOuter;
	bby1 = R3D_MIN(rampStart.z, rampEnd.z) - rampWidthOuter;
	bby2 = R3D_MAX(rampStart.z, rampEnd.z) + rampWidthOuter;

	// terrain affected cells
	int z1 = r3dTL::Clamp( int( bby1/desc.CellSize ), 0, desc.CellCountZ );
	int z2 = r3dTL::Clamp( int( bby2/desc.CellSize ), 0, desc.CellCountZ );
	int x1 = r3dTL::Clamp( int( bbx1/desc.CellSize ), 0, desc.CellCountX );
	int x2 = r3dTL::Clamp( int( bbx2/desc.CellSize ), 0, desc.CellCountX );


	r3d_assert( m_UndoItem );
	r3d_assert( m_UndoItem->GetActionID() == UA_TERRAIN2_HEIGHT );
	CHeightChanged2 * pUndoItem = ( CHeightChanged2 * ) m_UndoItem;

	for(int z = z1; z < z2; z++) 
	{
		for(int x = x1; x < x2; x++) 
		{
			r3dPoint3D p(x*desc.CellSize, 0, z*desc.CellSize);
			float k; // dist koef on line [0..1]
			float d = r3dDistancePointLine(p, rampStart2D, rampEnd2D, &k);
			if(k < 0 || k > 1.0f)  continue;
			if(d > rampWidthOuter) continue;


			int nIndex = z*int(desc.CellCountX) + x;

			float h1 = m_FloatHeights[ nIndex ];
			float h2 = rampStart.y + (dy * k);

			float h;
			if(d < rampWidthInner) {
				h = h2;
			} else { 
				float lc = ((d - rampWidthInner) / (rampWidthOuter - rampWidthInner));
				h = h2 + (h1 - h2) * lc;
			}

			m_FloatHeights[ nIndex ] = h;

			CHeightChanged2::UndoHeight_t undo;
			undo.nIndex = nIndex;
			undo.fPrevHeight = h1;
			undo.fCurrHeight = h;
			pUndoItem->AddData( undo );
		}
	}

	RECT rc;
	rc.left = x1;
	rc.bottom = z1;
	rc.right = x2;
	rc.top = z2;

	UpdateHeightRect( rc );

}

//------------------------------------------------------------------------

void
Terrain2Editor::StartLayerBrush( int layerIdx )
{
	int splatIdx = LayerToSplat( layerIdx );
	Terrain2->SetReplacementMask( splatIdx, GetPaintMask( splatIdx ) );

	m_PaintLayerIdx = layerIdx;
}

//------------------------------------------------------------------------

void
Terrain2Editor::ApplyLayerBrush( const r3dTerrainPaintBoundControl& boundCtrl, const r3dPoint3D &pnt, int opType, int layerIdx, float val, const float radius, const float hardness )
{
	float fFactor;

	int orgLayerIdx = layerIdx;

	r3d_assert( layerIdx >= 0 );
	r3d_assert( m_PaintLayerIdx == layerIdx );

	int SplatIdx = LayerToSplat( layerIdx );

	layerIdx -= 1;

	int Channel = layerIdx % r3dTerrain2::LAYERS_PER_MASK;

	r3dTexture* tex = GetPaintMask( SplatIdx );
	if( tex->GetD3DFormat() != D3DFMT_R5G6B5 ) r3dError( "splat tex is not r5g6b5" );

	// calc coord in [0..1] inside terrain

	const r3dTerrainDesc& desc = Terrain2->GetDesc();

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

	r3d_assert( m_UndoItem );
	r3d_assert( m_UndoItem->GetActionID() == UA_TERRAIN2_MASK_PAINT );
	CLayerMaskPaint2 * pUndoItem = ( CLayerMaskPaint2 * ) m_UndoItem;

	CLayerMaskPaint2::PaintData_t undo;


	int nLine = R.right - R.left;
	int nSize = nLine * ( R.bottom - R.top );
	undo.rc = R;
	undo.LayerIdx = orgLayerIdx ;
	undo.pData = new uint16_t[ nSize * 2 ];


	for(int z = 0; z < z2 - z1; z++) 
	{
		uint16_t * pSrc = (uint16_t*)&colors[ z * tex->GetWidth() ];
		uint16_t * pDst = undo.pData + z * nLine;

		memcpy( pDst, pSrc, nLine * sizeof( uint16_t ) );
	}

	{
		int texHeight = (int)tex->GetHeight();
		int texWidth = (int)tex->GetWidth();

		int tileScale = desc.CellCountPerTile / r3dTerrain2::QualitySettings::MIN_VERTEX_TILE_DIM;

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
				if( opType )
					Terrain2->MarkLayer( x, z, layerIdx + 1 );
				else
					Terrain2->RefreshAtlasTile( x / tileScale, z / tileScale );
			}
		}
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

			PP.y = Terrain2->GetHeight( PP );

			if ( ! terra_IsValidPaintBounds( boundCtrl, PP, fSoft, &fFactor ) )
				continue;

			if(coef <= hardness) 
				coef = 1.0f;
			else
				coef = 1.0f - (coef - hardness) / (1.0f-hardness);

			coef *= fFactor;

			int Pitch = tex->GetWidth();
			Channeled16* rgb = &colors[(z-z1)*Pitch+(x-x1)];

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
		}
	}

	for(int z = 0; z < z2 - z1; z++) 
	{
		uint16_t * pSrc = (uint16_t*)&colors[ z * tex->GetWidth() ];
		uint16_t * pDst = undo.pData + z * nLine + nSize;

		memcpy( pDst, pSrc, nLine * sizeof( uint16_t ) );
	}

	pUndoItem->AddData( undo );
	pUndoItem->AddRectUpdate( undo.rc );

	tex->Unlock();

	return;
}

//------------------------------------------------------------------------

void Terrain2Editor::EndLayerBrush()
{
	r3d_assert( m_PaintLayerIdx > 0 );

	int splatIdx = LayerToSplat( m_PaintLayerIdx );

	Terrain2->UpdateLayerMaskFromReplacementMask( splatIdx );

	r3dRenderer->DeleteTexture( m_PaintLayerMasks[ splatIdx ] );

	// this refreshes the paint mask with actual DXT compressed
	// artifacts so that you see what you get immediately and
	// paint without switching to undistorted data
	m_PaintLayerMasks[ splatIdx ] = NULL;
	GetPaintMask( splatIdx );
}

//------------------------------------------------------------------------

void Terrain2Editor::StartEraseAllBrush()
{
	for( int i = 0, e = GetMaskCount(); i < e; i ++ )
	{
		Terrain2->SetReplacementMask( i, GetPaintMask( i ) );
	}
}

//------------------------------------------------------------------------

void Terrain2Editor::ApplyEraseAllBrush(const r3dTerrainPaintBoundControl& boundCtrl, const r3dPoint3D &pnt, const float val, const float radius, const float hardness )
{
	r3d_assert( m_UndoItem );
	r3d_assert( m_UndoItem->GetActionID() == UA_TERRAIN2_MASK_ERASEALL );
	CLayerMaskEraseAll2 * pUndoItem = ( CLayerMaskEraseAll2 * ) m_UndoItem;

	int maskCount = GetMaskCount();

	float fFactor;

	r3dTexture* tex = GetPaintMask( 0 );

	// calc coord in [0..1] inside terrain

	const r3dTerrainDesc& desc = Terrain2->GetDesc();

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

	CLayerMaskEraseAll2::PaintData_t undo;
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

				PP.y = Terrain2->GetHeight( PP );

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

		int tileScale = desc.CellCountPerTile / r3dTerrain2::QualitySettings::MIN_VERTEX_TILE_DIM;

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
				Terrain2->RefreshAtlasTile( x / tileScale, z / tileScale );
			}
		}
	}

	pUndoItem->AddData( undo );
	pUndoItem->AddRectUpdate( undo.rc );

	return;

}

//------------------------------------------------------------------------

void Terrain2Editor::EndEraseAllBrush()
{
	for( int i = 0, e = GetMaskCount(); i < e; i ++ )
	{
		Terrain2->UpdateLayerMaskFromReplacementMask( i );

		r3dRenderer->DeleteTexture( m_PaintLayerMasks[ i ] );

		// this refreshes the paint mask with actual DXT compressed
		// artifacts so that you see what you get immediately and
		// paint without switching to undistorted data
		m_PaintLayerMasks[ i ] = NULL;
		GetPaintMask( i );
	}
}

//------------------------------------------------------------------------

void
Terrain2Editor::StartColorBrush()
{
	if( !m_ColorTex )
	{
		LoadColorsFromTerrain();
	}

	Terrain2->SetReplacementColorTexture( m_ColorTex );
}

//------------------------------------------------------------------------

void
Terrain2Editor::ApplyColorBrush( const r3dTerrainPaintBoundControl& boundCtrl, const r3dPoint3D &pnt, const r3dColor &dwColor, const float strength, const float radius, const float hardness )
{
	r3dTexture* colorTex = Terrain2->GetColorTexture();
	const r3dTerrainDesc& desc = Terrain2->GetDesc();

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
	CLayerColorPaint2 * pUndoItem = static_cast< CLayerColorPaint2 * >( m_UndoItem );

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

	Terrain2->RefreshAtlasTiles( tileRect );

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
			PP.y = Terrain2->GetHeight( PP );

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

			CLayerColorPaint2::UndoColor_t undo;
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
}

//------------------------------------------------------------------------

void
Terrain2Editor::EndColorBrush()
{
	Terrain2->UpdateColorTexture();
}

//------------------------------------------------------------------------

int
Terrain2Editor::ImportHeight( const char* path, float scale, float offset )
{
	m_HeightDirty = 1;

	const r3dTerrainDesc& desc = Terrain2->GetDesc();

	IDirect3DTexture9* r32f_tex;
	if( D3DXCreateTextureFromFileEx( r3dRenderer->pd3ddev, path, desc.CellCountX, desc.CellCountZ, 1, 0, D3DFMT_R32F, D3DPOOL_SYSTEMMEM, D3DX_FILTER_TRIANGLE, D3DX_FILTER_NONE, 0, NULL, NULL, &r32f_tex ) != S_OK )
	{
		return 0;
	}

	if( !m_FloatHeights.Count( ) )
	{
		LoadHeightsFromTerrain();
	}

	D3DLOCKED_RECT lrect;
	D3D_V( r32f_tex->LockRect( 0, &lrect, NULL, D3DLOCK_READONLY  ) );

	for( int i = 0, e = desc.CellCountX * desc.CellCountZ; i < e; i ++ )
	{
		m_FloatHeights[ i ] = ((float*)lrect.pBits)[ i ] * scale + offset;
	}

	D3D_V( r32f_tex->UnlockRect( 0 ) );

	SAFE_RELEASE( r32f_tex );

	SaveHeightsToTerrain();

	return 1;
}

//------------------------------------------------------------------------

int
Terrain2Editor::ExportHeight( const char* path )
{
	r3dTexture* r3dtex = Terrain2->GetHeightTexture();

	r3d_assert( r3dtex->GetD3DFormat() == D3DFMT_L16 );

	IDirect3DTexture9* tex = r3dtex->AsTex2D();
	IDirect3DTexture9* r32ftex;
	D3D_V( r3dRenderer->pd3ddev->CreateTexture( r3dtex->GetWidth(), r3dtex->GetHeight(), 1, 0, D3DFMT_R32F, D3DPOOL_SYSTEMMEM, &r32ftex, NULL ) );

	D3DLOCKED_RECT src_lrect;
	D3DLOCKED_RECT targ_lrect;

	D3D_V( tex->LockRect( 0, &src_lrect, NULL, D3DLOCK_READONLY ) );

	D3D_V( r32ftex->LockRect( 0, &targ_lrect, NULL, 0 ) );

	for( int y = 0, e = r3dtex->GetHeight(); y < e; y ++ )
	{
		for( int x = 0, e = r3dtex->GetWidth(); x < e; x ++ )
		{
			((float*)((char*)targ_lrect.pBits + targ_lrect.Pitch * y ))[ x ] 
									= 
										((UINT16*)((char*)src_lrect.pBits + src_lrect.Pitch * y ))[ x ] / 65535.f;
		}
	}

	D3D_V( r32ftex->UnlockRect( 0 ) );

	D3D_V( tex->UnlockRect( 0 ) );

	D3D_V( D3DXSaveTextureToFile( path, D3DXIFF_DDS, r32ftex, NULL ) );

	SAFE_RELEASE( r32ftex );

	return 1;
}

//------------------------------------------------------------------------

void
Terrain2Editor::UpdateHeightRect( const RECT& rc )
{
	CHeightChanged2 * pUndoItem = ( CHeightChanged2 * ) m_UndoItem;

	pUndoItem->AddRectUpdate( rc );

	UpdateTerrainHeightRect( rc );

}

//------------------------------------------------------------------------

r3dTexture*
Terrain2Editor::GetPaintMask( int idx )
{
	if( idx >= (int)m_PaintLayerMasks.Count() )
		m_PaintLayerMasks.Resize( idx + 1 );

	r3dTexture*& mask = m_PaintLayerMasks[ idx ];

	if( !mask )
	{
		mask = r3dRenderer->AllocateTexture();
		
		r3dTexture* sourceMask = Terrain2->GetLayerMask( idx );

		ID3DXBuffer* fileInMem ( NULL );

		D3D_V( D3DXSaveTextureToFileInMemory( &fileInMem, D3DXIFF_DDS, sourceMask->GetD3DTexture(), NULL ) );

		IDirect3DTexture9* result;

		D3D_V(
			D3DXCreateTextureFromFileInMemoryEx(	r3dRenderer->pd3ddev, 
													fileInMem->GetBufferPointer(), 
													fileInMem->GetBufferSize(),
													sourceMask->GetWidth(),
													sourceMask->GetHeight(), 
													1, 0, D3DFMT_R5G6B5, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL,
													&result
													)
				);

		r3dD3DTextureTunnel tunnel;
		tunnel.Set( result );

		mask->Setup( sourceMask->GetWidth(), sourceMask->GetHeight(), 1, D3DFMT_R5G6B5, 1, &tunnel, false );

		SAFE_RELEASE( fileInMem );
	}

	return mask;
}

//------------------------------------------------------------------------

void
Terrain2Editor::UpdateTerrainHeightRect( const RECT& rc )
{
	float start = r3dGetTime();

	m_HeightDirty = 1;

	RECT texRect = rc;
	R3D_SWAP( texRect.top, texRect.bottom );

	RECT shortRect = texRect;

	if( Terrain2->UpdateHeightRanges( m_FloatHeights ) )
	{
		const r3dTerrainDesc& desc = Terrain2->GetDesc();

		shortRect.left		= 0;
		shortRect.right		= desc.CellCountX;
		shortRect.top		= 0;
		shortRect.bottom	= desc.CellCountZ;
	}
	Terrain2->ConvertHeightField( &m_ShortHeights, m_FloatHeights, &shortRect );

	Terrain2->UpdateGraphicsHeightField( m_ShortHeights, &shortRect );

	Terrain2->UpdateNormals( m_FloatHeights, &m_TempVectors0, &m_TempVectors1, &texRect );

	Terrain2->RecalcTileField( m_ShortHeights, &shortRect );
	r3dOutToLog( "Terrain2Editor::UpdateTerrainHeightRect: %.4f\n", r3dGetTime() - start  );
}

//------------------------------------------------------------------------

void
Terrain2Editor::BeginUndoRecord( const char * title, UndoAction_e eAction )
{
	m_UndoItem = g_pUndoHistory->CreateUndoItem( eAction );
	assert( m_UndoItem );
	m_UndoItem->SetTitle( title );
}

//------------------------------------------------------------------------

void
Terrain2Editor::EndUndoRecord()
{
	m_UndoItem = NULL;
}

//------------------------------------------------------------------------

int Terrain2Editor::IsHeightDirty() const
{
	return m_HeightDirty;
}

//------------------------------------------------------------------------

int Terrain2Editor::GetMaskCount() const
{
	int layerCount = Terrain->GetDesc().LayerCount - 1;
	return layerCount / r3dTerrain2::LAYERS_PER_MASK + ( layerCount % r3dTerrain2::LAYERS_PER_MASK ? 1 : 0 );
}

//------------------------------------------------------------------------

Terrain2Editor* g_pTerrain2Editor;

//------------------------------------------------------------------------

Terrain2Editor::NoiseParams::NoiseParams()
{
	maxHeight         = 100.0f;
	minHeight         = 0.0;
	heightApplySpeed  = 1.0f;
	heightRelative    = 0;

	noiseData         = NULL;
	Height            = 0;
	Width             = 0;
}

//------------------------------------------------------------------------

Terrain2Editor::NoiseParams::~NoiseParams()
{
	
}

//------------------------------------------------------------------------

void
Terrain2Editor::NoiseParams::ResetCache()
{
	noise.seed_ = u_random(0x7FFF);

	const r3dTerrainDesc& desc = Terrain->GetDesc();

	if( desc.CellCountZ != Height || desc.CellCountX != Width || !noiseData )
	{
		SAFE_DELETE_ARRAY(noiseData);
		Height = desc.CellCountZ;
		Width  = desc.CellCountX;

		noiseData = new cell_s[Height * Width];
	}

	for( int j = 0, e = Height; j < e; j ++ )
	{
		for( int i = 0, e = Width; i < e; i ++ )
		{
			noiseData[ i + j * e ].org = Terrain->GetHeight( i, j );
			noiseData[ i + j * e ].trg = -99999;
			noiseData[ i + j * e ].mc  = 0;
		}
	}
}

//------------------------------------------------------------------------

float
Terrain2Editor::NoiseParams::GetNoise( int x, int y )
{
	r3d_assert(noiseData);

	float h = noiseData[y*Width+x].trg;
	if(h <= -99999) 
	{
		h = noise.PerlinNoise_2D( (float)x, (float)y );
		h += 1.0f / 2.0f; // convert to [0..1];
		h = minHeight + h * (maxHeight - minHeight);

		if(heightRelative) {
			h += noiseData[y*Width+x].org;
		}
		noiseData[y*Width+x].trg = h;
	}

	return h;
}

//------------------------------------------------------------------------

void
Terrain2Editor::NoiseParams::Apply( const r3dPoint3D& pos, float radius, float hardiness )
{
	if(noiseData == NULL) {
		ResetCache();
	}

	const r3dTerrainDesc& desc = Terrain->GetDesc();

	float CellSize = desc.CellSize;

	int nc = int( radius / CellSize );	// number of cells in radius
	int cx = int( pos.X / CellSize );	// center cell x
	int cz = int( pos.Z / CellSize );	// center cell y

	cx = r3dTL::Clamp( cx, 0, Width - 1 );
	cz = r3dTL::Clamp( cz, 0, Height - 1 );

	// affected area
	int z1 = cz-nc; if(z1 < 0)			z1 = 0;
	int z2 = cz+nc; if(z2 > Height)		z2 = Height;
	int x1 = cx-nc; if(x1 < 0)			x1 = 0;
	int x2 = cx+nc; if(x2 > Width)		x2 = Width;

	r3d_assert( g_pTerrain2Editor->m_UndoItem );
	r3d_assert( g_pTerrain2Editor->m_UndoItem->GetActionID() == UA_TERRAIN2_HEIGHT );
	CHeightChanged2 * pUndoItem = ( CHeightChanged2 * ) g_pTerrain2Editor->m_UndoItem;

	for(int y = z1; y < z2; y++)
	{
		for(int x = x1; x < x2; x++)
		{
			int dy = y-cz;
			int dx = x-cx;

			float coef = sqrtf(float(dx*dx+dy*dy)) / float(nc);
			if(coef > 1.0f) continue;

			// check for target morph coef
			float tmc  = 1.0f;	 // target morph coef
			if(coef > hardiness) {
				float k = (coef - hardiness) / (1.0f - hardiness);
				tmc = 1.0f - k;
			}

			float h2 = GetNoise(x, y);

			int nIndex = y*int(Width) + x;

			cell_s& cell = noiseData[nIndex];
			if(cell.mc < tmc) {
				cell.mc += r3dGetFrameTime() / heightApplySpeed;
				cell.mc = R3D_CLAMP(cell.mc, 0.0f, tmc);
			}

			float h = cell.org + (cell.trg - cell.org) * cell.mc;

			float fOld = g_pTerrain2Editor->m_FloatHeights[nIndex];
			g_pTerrain2Editor->m_FloatHeights[nIndex] = h;


			CHeightChanged2::UndoHeight_t undo;
			undo.nIndex = nIndex;
			undo.fPrevHeight = fOld;
			undo.fCurrHeight = h;
			pUndoItem->AddData( undo );
		}
	}

	RECT rc;
	rc.left = x1;
	rc.bottom = z1;
	rc.right = x2;
	rc.top = z2;

	g_pTerrain2Editor->UpdateHeightRect( rc );

	return;
}


//------------------------------------------------------------------------

void CHeightChanged2::Release()
{
	PURE_DELETE( this ); 
}

//------------------------------------------------------------------------

UndoAction_e CHeightChanged2::GetActionID()
{
	return ms_eActionID;
}

//------------------------------------------------------------------------

void
CHeightChanged2::Undo()
{
	const r3dTerrainDesc& desc = Terrain2->GetDesc();

	for ( int i = m_pData.Count() - 1; i >= 0; i-- ) 
	{
		const UndoHeight_t &data = m_pData[ i ];
		r3d_assert( data.nIndex >= 0 && data.nIndex < desc.CellCountX * desc.CellCountZ );

		g_pTerrain2Editor->m_FloatHeights[ data.nIndex ] = data.fPrevHeight;
	}

	g_pTerrain2Editor->UpdateTerrainHeightRect( m_rc );
	g_pTerrain2Editor->SaveHeightsToTerrain();
}

//------------------------------------------------------------------------

void
CHeightChanged2::Redo()
{
	const r3dTerrainDesc& desc = Terrain2->GetDesc();

	for ( int i = 0; i < (int)m_pData.Count(); i++ ) 
	{
		const UndoHeight_t &data = m_pData[ i ];
		r3d_assert( data.nIndex >= 0 && data.nIndex < desc.CellCountX * desc.CellCountZ );
		g_pTerrain2Editor->m_FloatHeights[ data.nIndex ] = data.fCurrHeight;
	}

	g_pTerrain2Editor->UpdateTerrainHeightRect( m_rc );
	g_pTerrain2Editor->SaveHeightsToTerrain();
}

//------------------------------------------------------------------------

void
CHeightChanged2::AddData( const UndoHeight_t & data )
{
	m_pData.PushBack( data );
}

//------------------------------------------------------------------------

void
CHeightChanged2::AddRectUpdate	( const RECT &rc )				
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

CHeightChanged2::CHeightChanged2()
{
	memset( &m_rc, 0, sizeof m_rc );
}

//------------------------------------------------------------------------
/*static*/

IUndoItem *
CHeightChanged2::CreateUndoItem	()
{
	return new CHeightChanged2;
}

//------------------------------------------------------------------------
/*static*/
void
CHeightChanged2::Register()
{
	UndoAction_t action;
	action.nActionID = ms_eActionID;
	action.pCreateUndoItem = CreateUndoItem;

	g_pUndoHistory->RegisterUndoAction( action );
}

//------------------------------------------------------------------------

void
CLayerMaskPaint2::Release()
{ 
	for ( int i = 0; i < (int)m_pData.Count(); i++ )
	{
		SAFE_DELETE_ARRAY( m_pData[ i ].pData );
	}
	PURE_DELETE( this ); 
};

//------------------------------------------------------------------------

UndoAction_e
CLayerMaskPaint2::GetActionID()
{
	return ms_eActionID;
}

//------------------------------------------------------------------------

void
CLayerMaskPaint2::UndoRedo( bool redo )
{
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

		r3d_assert( data.LayerIdx > 0 );

		// don't undo pointless stuff
		g_pTerrain2Editor->StartLayerBrush( data.LayerIdx ); 

		int splatIdx = LayerToSplat( data.LayerIdx );

		r3dTexture* tex = g_pTerrain2Editor->m_PaintLayerMasks[ splatIdx ];
		uint16_t* colors = (uint16_t*)tex->Lock( 1, &data.rc ); // fixme:

		int nLine = data.rc.right - data.rc.left;
		int nSize = redo ? nLine * ( data.rc.bottom - data.rc.top ) : 0;

		for( int y = 0; y < data.rc.bottom - data.rc.top; y++ ) 
		{
			uint16_t * pDst = (uint16_t*)&colors[ y * tex->GetWidth() ];
			uint16_t * pSrc = data.pData + y * nLine + nSize;
			memcpy( pDst, pSrc, nLine * sizeof( uint16_t ) );
		}

		tex->Unlock();

		g_pTerrain2Editor->EndLayerBrush();

		Terrain2->RefreshAtlasTiles( data.rc );
	}
}

//------------------------------------------------------------------------
void
CLayerMaskPaint2::Undo()
{
	UndoRedo( false );
}

//------------------------------------------------------------------------

void
CLayerMaskPaint2::Redo()
{
	UndoRedo( true );
}

//------------------------------------------------------------------------

void
CLayerMaskPaint2::AddData( const PaintData_t & data )
{
	m_pData.PushBack( data );
}

//------------------------------------------------------------------------

void
CLayerMaskPaint2::AddRectUpdate	( const RECT &rc )				
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

CLayerMaskPaint2::PaintData_t::PaintData_t()
: pData( 0 )
, LayerIdx( -1 )
{

}

//------------------------------------------------------------------------

CLayerMaskPaint2::CLayerMaskPaint2()
{
	m_sTitle = "Paint mask";

	m_rc.top = m_rc.left = INT_MAX;
	m_rc.bottom = m_rc.right = -INT_MAX;
}

//------------------------------------------------------------------------
/*static*/

IUndoItem* CLayerMaskPaint2::CreateUndoItem	()
{
	return new CLayerMaskPaint2;
};

//------------------------------------------------------------------------
/*static*/

void
CLayerMaskPaint2::Register()
{
	UndoAction_t action;
	action.nActionID = ms_eActionID;
	action.pCreateUndoItem = CreateUndoItem;
	g_pUndoHistory->RegisterUndoAction( action );
}

//------------------------------------------------------------------------

void
CLayerColorPaint2::Release()
{
	PURE_DELETE( this );
}

//------------------------------------------------------------------------

UndoAction_e
CLayerColorPaint2::GetActionID()
{
	return ms_eActionID;
}

//------------------------------------------------------------------------

void CLayerColorPaint2::UndoRedo	( bool redo )
{
	g_pTerrain2Editor->StartColorBrush();

	uint32_t* pColor = (uint32_t*)g_pTerrain2Editor->m_ColorTex->Lock( 1, NULL );

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

	g_pTerrain2Editor->m_ColorTex->Unlock();

	g_pTerrain2Editor->EndColorBrush();

	Terrain2->RefreshAtlasTiles();	
}

//------------------------------------------------------------------------

void
CLayerColorPaint2::Undo()
{
	UndoRedo( false );
}

//------------------------------------------------------------------------

void
CLayerColorPaint2::Redo()
{
	UndoRedo( true );
}

//------------------------------------------------------------------------

void
CLayerColorPaint2::AddData( const UndoColor_t & data )
{
	r3d_assert( data.nIndex < m_CellCount );

	m_pData.PushBack( data );
}

//------------------------------------------------------------------------

void CLayerColorPaint2::AddRectUpdate( const RECT &rc )
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

CLayerMaskEraseAll2::PaintData_t::PaintData_t()
{

}

//------------------------------------------------------------------------

void CLayerMaskEraseAll2::Release()
{
	PURE_DELETE( this );
}

//------------------------------------------------------------------------

UndoAction_e CLayerMaskEraseAll2::GetActionID()
{
	return ms_eActionID;
}

//------------------------------------------------------------------------

void CLayerMaskEraseAll2::UndoRedo( bool redo )
{
	if( !m_pData.Count() )
		return;

	int first = 1;

	int splatCount = m_pData[ 0 ].masks.Count();

	for( int splatIdx = 0, e = splatCount; splatIdx < e; splatIdx ++, first = 0 )
	{
		g_pTerrain2Editor->StartLayerBrush( splatIdx * r3dTerrain2::LAYERS_PER_MASK + 1 );

		r3dTexture* tex = g_pTerrain2Editor->m_PaintLayerMasks[ splatIdx ];

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
				Terrain2->RefreshAtlasTiles( data.rc );
			}
		}

		g_pTerrain2Editor->EndLayerBrush();
	}
}

//------------------------------------------------------------------------

void CLayerMaskEraseAll2::Undo()
{
	UndoRedo( false );
}

//------------------------------------------------------------------------

void CLayerMaskEraseAll2::Redo()
{
	UndoRedo( true );
}

//------------------------------------------------------------------------

void CLayerMaskEraseAll2::AddData( const PaintData_t & data )
{
	m_pData.PushBack( data );
}

//------------------------------------------------------------------------

void CLayerMaskEraseAll2::AddRectUpdate( const RECT &rc )
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

CLayerMaskEraseAll2::CLayerMaskEraseAll2()
{
	m_sTitle = "Erase all";

	m_rc.top = m_rc.left = INT_MAX;
	m_rc.bottom = m_rc.right = -INT_MAX;	
}

//------------------------------------------------------------------------

IUndoItem* CLayerMaskEraseAll2::CreateUndoItem()
{
	return new CLayerMaskEraseAll2;
}

//------------------------------------------------------------------------

void CLayerMaskEraseAll2::Register()
{
	UndoAction_t action;
	action.nActionID = ms_eActionID;
	action.pCreateUndoItem = CreateUndoItem;

	g_pUndoHistory->RegisterUndoAction( action );
}

//------------------------------------------------------------------------

CLayerColorPaint2::CLayerColorPaint2()
{
	m_sTitle = "Color paint";

	m_pData.Reserve( 8 );

	m_rc.bottom = m_rc.left = INT_MAX;
	m_rc.top = m_rc.right = -INT_MAX;

	r3dTexture* colTex = Terrain2->GetColorTexture();

	m_CellCount = colTex->GetWidth() * colTex->GetHeight();

}

//------------------------------------------------------------------------
/*static*/

IUndoItem *
CLayerColorPaint2::CreateUndoItem	()
{
	return new CLayerColorPaint2;
};

//------------------------------------------------------------------------
/*static*/

void
CLayerColorPaint2::Register()
{
	UndoAction_t action;
	action.nActionID = ms_eActionID;
	action.pCreateUndoItem = CreateUndoItem;
#ifndef FINAL_BUILD
	g_pUndoHistory->RegisterUndoAction( action );
#endif
}

//------------------------------------------------------------------------

CTerrain2DestroyLayer::CTerrain2DestroyLayer()
: m_TerrainLayerIdx( -1 )
{
	m_sTitle = "Remove Terrain Layer";
}

//------------------------------------------------------------------------

void
CTerrain2DestroyLayer::Release()
{
	PURE_DELETE( this );
}

//------------------------------------------------------------------------

UndoAction_e
CTerrain2DestroyLayer::GetActionID()
{
	return ms_eActionID;
}

//------------------------------------------------------------------------

void
CTerrain2DestroyLayer::Undo()
{
	Terrain2->InsertLayer( m_TerrainLayerIdx, &m_TerrainLayerData, &m_TerrainLayer );
}

//------------------------------------------------------------------------

void
CTerrain2DestroyLayer::Redo()
{
	Terrain2->DestroyLayer( m_TerrainLayerIdx );
}

//------------------------------------------------------------------------

void
CTerrain2DestroyLayer::SetData( int layerIdx )
{
	m_TerrainLayerIdx = layerIdx;

	Terrain2->GetLayerChannel( &m_TerrainLayerData, m_TerrainLayerIdx );
	m_TerrainLayer = Terrain2->GetLayer( m_TerrainLayerIdx + 1 );
}

//------------------------------------------------------------------------
/*static*/

IUndoItem*
CTerrain2DestroyLayer::CreateUndoItem()
{
	return new CTerrain2DestroyLayer();
}

//------------------------------------------------------------------------
/*static*/

void
CTerrain2DestroyLayer::Register()
{
	UndoAction_t action;
	action.nActionID = ms_eActionID;
	action.pCreateUndoItem = CreateUndoItem;

	g_pUndoHistory->RegisterUndoAction( action );
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

CTerrain2InsertLayer::CTerrain2InsertLayer()
: m_TerrainLayerIdx( -1 )
{
	m_sTitle = "Insert Terrain Layer";
}

//------------------------------------------------------------------------

void
CTerrain2InsertLayer::Release()
{
	PURE_DELETE( this );
}

//------------------------------------------------------------------------

UndoAction_e
CTerrain2InsertLayer::GetActionID()
{
	return ms_eActionID;
}

//------------------------------------------------------------------------

void
CTerrain2InsertLayer::Undo()
{
	Terrain2->DestroyLayer( m_TerrainLayerIdx );
}

//------------------------------------------------------------------------

void
CTerrain2InsertLayer::Redo()
{
	Terrain2->InsertLayer( m_TerrainLayerIdx, NULL, NULL );
}

//------------------------------------------------------------------------

void
CTerrain2InsertLayer::SetData( int layerIdx )
{
	m_TerrainLayerIdx = layerIdx;
}

//------------------------------------------------------------------------
/*static*/

IUndoItem*
CTerrain2InsertLayer::CreateUndoItem()
{
	return new CTerrain2InsertLayer();
}

//------------------------------------------------------------------------
/*static*/

void
CTerrain2InsertLayer::Register()
{
	UndoAction_t action;
	action.nActionID = ms_eActionID;
	action.pCreateUndoItem = CreateUndoItem;

	g_pUndoHistory->RegisterUndoAction( action );
}

//------------------------------------------------------------------------
static int LayerToSplat( int layerIdx )
{
	return ( layerIdx - 1 ) / r3dTerrain2::LAYERS_PER_MASK;
}

//------------------------------------------------------------------------


#endif