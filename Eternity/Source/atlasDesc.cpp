#include "r3dPCH.h"
#include "r3d.h"
#include "atlasDesc.h"

#include "AtlasComposer/RectPlacement.h"

AtlasDesc::AtlasDesc()
:count(0), rects(0)
{

}

AtlasDesc::AtlasDesc(int cnt)
:count(0), rects(0)
{
	init(cnt);	
}

AtlasDesc::~AtlasDesc()
{
	clear();
}

void AtlasDesc::init(int cnt)
{
	clear();

	count = cnt;
	if(count > 0)
	{
		rects = game_new Rect[count];
	}	
}

void AtlasDesc::clear()
{
	if(rects)
		delete [] rects;

	rects = 0;
	count = 0;
}

void AtlasDesc::save(const char* fileName)
{
	FILE* f = fopen(fileName, "wb");
	if(f)
	{
		fwrite(&count, sizeof(count), 1, f);
		if(count > 0)
		{
			fwrite(rects, sizeof(Rect) * count, 1, f);
		}

		fclose(f);
	}	
}

void AtlasDesc::load(const char* fileName)
{
	clear();
	r3dFile* f = r3d_open(fileName, "rb");
	if(f)
	{
		fread(&count, sizeof(count), 1, f);
		init(count);
 		if(count > 0)
 		{
 			fread(rects, sizeof(Rect) * count, 1, f);
 		}	
		fclose(f);
	}	
}

const AtlasDesc::Rect& AtlasDesc::rect(int idx) const
{
	static const Rect defRect = {0.0f, 0.0f, 1.0f, 1.0f};

	if(count <= 0)
		return defRect;

	if(idx >= count)
		idx %= count;

	return rects[idx];
}

TextureRectangle::TextureRectangle()
: texture( NULL )
{

}

//------------------------------------------------------------------------

TextureRectangle::~TextureRectangle()
{
	SAFE_RELEASE( texture );
}

//------------------------------------------------------------------------

int ComposeAtlas( const char* basePath, const char* atlasName )
{
	char pathWithWildCards[ 512 ];

	strcpy( pathWithWildCards, basePath );
	strcat( pathWithWildCards, atlasName );
	strcat( pathWithWildCards, "/*.dds" );

	TextureAtlas atlas;

	WIN32_FIND_DATA ffblk;

	HANDLE h = FindFirstFile( pathWithWildCards, &ffblk );
	if(h == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	stringlist_t files; // local copy of dir files

	char szName[ 512 ];

	do
	{
		if( ffblk.cFileName[0] != '.' ) 
		{
			FixFileName( ffblk.cFileName, szName );
			files.push_back( szName );
		}
	} while(FindNextFile(h, &ffblk) != 0);

	FindClose(h);

	char texPath[ 512 ];
	strcpy( texPath, basePath );
	strcat( texPath, atlasName );
	strcat( texPath, "/" );

	for( int i = 0, e = (int)files.size(); i < e; i ++ )
	{
		const r3dSTLString& file = files[ i ];

		TextureRectangle rect;

		D3DXIMAGE_INFO iinfo;

		D3DXGetImageInfoFromFile( ( texPath + file ).c_str(), &iinfo );

		rect.rect = CRectPlacement::TRect(0, 0, iinfo.Width, iinfo.Height );
		rect.fileName = file;
		atlas.rectangles.push_back( rect );
	}

	CRectPlacement rectPlacement;

	enum
	{
		ATLAS_DIMS = 4096
	};

	for( TextureAtlas::TextureList::iterator i = atlas.rectangles.begin(), e = atlas.rectangles.end(); i != e; ++ i )
	{
		TextureRectangle& trect = *i;

		int res = rectPlacement.AddAtEmptySpotAutoGrow( &trect.rect, ATLAS_DIMS, ATLAS_DIMS );

		if( !res )
		{
			MessageBoxA( r3dRenderer->HLibWin, "Atlas is too small!", "Error", MB_ICONERROR );
			return 0;
		}
	}

	atlas.result.rect.w = rectPlacement.GetW();
	atlas.result.rect.h = rectPlacement.GetH();	

	AtlasDesc atlasDesc;

	atlasDesc.init( (int)atlas.rectangles.size() );

	TextureAtlas::TextureList::iterator sti = atlas.rectangles.begin();

	//------------------------------------------------------------------------
	{
		TextureRectangle& rr = atlas.result;

		if( r3dRenderer->pd3ddev->CreateTexture( atlas.result.rect.w, atlas.result.rect.h, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &rr.texture, NULL ) != S_OK )
			return 0;

		int fileIdx = 0;

		for( TextureAtlas::TextureList::iterator	i = atlas.rectangles.begin(),
													e = atlas.rectangles.end(); i != e; ++ i, fileIdx ++ )
		{
			TextureRectangle& sourceRect = *i;

			IDirect3DTexture9* tempTex( NULL );

			const r3dSTLString& file = files[ fileIdx ];

			TextureRectangle rect;

			if( D3DXCreateTextureFromFileEx( r3dRenderer->pd3ddev, ( texPath + file ).c_str(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &tempTex ) != S_OK )
				return 0;

			D3DLOCKED_RECT lrect;
			RECT lockArea;

			lockArea.left = sourceRect.rect.x;
			lockArea.right = sourceRect.rect.x + sourceRect.rect.w;

			lockArea.top = sourceRect.rect.y;
			lockArea.bottom = sourceRect.rect.y + sourceRect.rect.h;

			D3D_V( rr.texture->LockRect( 0, &lrect, &lockArea, 0 ) );

			D3DLOCKED_RECT ttlrect;
			D3D_V( tempTex->LockRect( 0, &ttlrect, NULL, 0 ) );

			for( int y = 0, e = (int)sourceRect.rect.h; y < e; y ++ )
			{
				memcpy(	(char*)lrect.pBits + y * lrect.Pitch,
						(char*)ttlrect.pBits + y * ttlrect.Pitch, sourceRect.rect.w * sizeof( DWORD ) );						
			}

			D3D_V( tempTex->UnlockRect( 0 ) );
			D3D_V( rr.texture->UnlockRect( 0 ) );
			
			SAFE_RELEASE( tempTex );
		}
	}
	//------------------------------------------------------------------------

	for( int i = 0, e = atlasDesc.count; i < e; i ++, ++ sti )
	{
		r3d_assert( sti != atlas.rectangles.end() );

		AtlasDesc::Rect& rect = atlasDesc.rects[ i ];

		const TextureRectangle& trect = *sti;

		const CRectPlacement::TRect& srect = trect.rect;

		rect.minX = float( srect.x + 0.f ) / atlas.result.rect.w;
		rect.minY = float( srect.y + 0.f ) / atlas.result.rect.h;
		rect.maxX = float( srect.x + srect.w ) / atlas.result.rect.w;
		rect.maxY = float( srect.y + srect.h ) / atlas.result.rect.h;

#if 0
		rect.minY = 1.0f - rect.minY;
		rect.maxY = 1.0f - rect.maxY;
#endif
	}

	//------------------------------------------------------------------------

	char destAtlasFileName[ 512 ];
	strcpy( destAtlasFileName, basePath );
	strcat( destAtlasFileName, atlasName );
	strcat( destAtlasFileName, ".dds.atlas" );

	atlasDesc.save( destAtlasFileName );

	char destAtlasTexFileName[ 512 ];
	strcpy( destAtlasTexFileName, basePath );
	strcat( destAtlasTexFileName, atlasName );
	strcat( destAtlasTexFileName, ".dds" );

	if( D3DXSaveTextureToFile( destAtlasTexFileName, D3DXIFF_DDS, atlas.result.texture, NULL ) != S_OK )
		return 0;

	return 1;
}