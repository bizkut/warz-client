#include "r3dPCH.h"
#include "r3d.h"

extern	char	__r3dBaseShaderPath[256];
extern	char	__r3dBaseShaderCachePath[256];

extern	r3dgfxMap(DWORD, r3dSTLString) _r3d_mShaderMap;
extern	void r3dMakeCompiledShaderName(char* out, const char* shader, const char* defines);
extern  int r3dCompileShader(
  const char* FileName, 
  const D3DXMACRO* definesCpy, 
  const char* pFunctionName, 
  const char* pProfile,
  LPD3DXBUFFER* ppShader,
  r3dTL::TArray< r3dString > * pIncludes
  );

static char BIN_VER_SIG[] = "CPS100";

void WriteIncludesToFile( FILE* f1, const r3dTL::TArray< r3dString >& Includes );
void ReadIncludesFromFile( r3dFile* f1, r3dTL::TArray< r3dString >& oIncludes );

#define USE_PROJECT_NAME 0

r3dPixelShader::r3dPixelShader()
{
  m_pShader   = NULL; 
  FileName[0] = 0;
  Name[0]     = 0;
}

void r3dPixelShader :: Unload()
{
	R3D_ENSURE_MAIN_THREAD();

	if(m_pShader)
	{
		SetActive(0);

#ifdef _DEBUG

		r3dgfxMap(DWORD, r3dSTLString)::iterator pIter = _r3d_mShaderMap.find((DWORD)m_pShader);

		int refcnt = m_pShader->Release();
		if(refcnt) r3dOutToLog("WARNING: shader '%s' have refcnt %d when unloading\n", (pIter != _r3d_mShaderMap.end()) ? pIter->second.c_str() : "<unknown>", refcnt);

		if(pIter != _r3d_mShaderMap.end()) {
			_r3d_mShaderMap.erase(pIter);
		}

		//r3dOutToLog("Shader[%x] removing\n", m_pShader);
		m_pShader = NULL;
#else
		SAFE_RELEASE(m_pShader);
#endif
	}
}


int   r3dPixelShader :: LoadBinaryCache( const char* FName, const char* Path, const char* defines )
{
	R3D_ENSURE_MAIN_THREAD();

	char FName2[512];
	r3dMakeCompiledShaderName(FName2, FName, defines);
	if(r3d_access(FName2, 0) != 0) return 0;

	bool sourceAvailable = r3d_access( Path, 0 ) == 0;

	// is source code available?
	if( sourceAvailable )
	{
		// source code is newer, have to recompile
		if( r3d_fstamp( Path ) > r3d_fstamp( FName2 ) )
			return 0;
	}

	r3dFile* f = r3d_open( FName2, "rb" );

	if( !sourceAvailable && !f )
	{
		r3dError( "r3dPixelShader :: LoadBinaryCache: both source and cached version a missing(%s)!\n", Path );
	}

	if (!f) return 0;

	char CheckSig[ sizeof BIN_VER_SIG ];
	fread( CheckSig, sizeof CheckSig, 1, f );

	if( memcmp( CheckSig, BIN_VER_SIG, sizeof BIN_VER_SIG ) )
	{
		if( !sourceAvailable )
		{
			r3dError( "r3dPixelShader::LoadBinaryCache: source code is missing and cached version is unknown(%s)!\n", Path );
		}
		else
		{
			r3dOutToLog( "r3dPixelShader::LoadBinaryCache: unsupported binary version!\n" );
		}

		fclose( f );
		return 0;
	}

	r3dTL::TArray< r3dString > Includes;

	ReadIncludesFromFile( f, Includes );

	if( sourceAvailable )
	{
		for( uint32_t i = 0, e = Includes.Count(); i < e; i ++ )
		{
			char IncludeName[ 512 ];
			char drive[ 16 ], dir[ 512 ], name[ 512 ], ext[ 512 ];

			_splitpath( FName, drive, dir, name, ext );

			sprintf( IncludeName, "%s\\%s\\%s", __r3dBaseShaderPath, dir, Includes[ i ].c_str() );

			if( r3d_fstamp( IncludeName ) > r3d_fstamp( FName2 ) )
			{
				fclose( f );
				return 0;
			}
		}
	}

	size_t codeSize = f->size - ftell( f );

	char* shaderCode = gfx_new char[ codeSize + 1 ];
	fread( shaderCode, codeSize, 1, f );
	fclose( f );

	Unload();

	// Create the pixel shader
	HRESULT hr;
	hr = r3dRenderer->pd3ddev->CreatePixelShader( (DWORD*)shaderCode, &m_pShader);

	r3dRenderer->CheckOutOfMemory( hr ) ;

#ifndef FINAL_BUILD
	if( r_warn_shaders->GetInt() )
	{
		ID3DXBuffer *buffer( NULL ) ;
		D3D_V( D3DXDisassembleShader( (DWORD*)shaderCode, FALSE, NULL, &buffer ) ) ;

		char* cbuff = (char*)buffer->GetBufferPointer() ;

		if( char* substr = strstr( cbuff, "// approximately" ) )
		{
			int icount = 0 ;
			sscanf( substr, "// approximately %d", &icount ) ;

			if( icount > 512 )
			{
				r3dArtBug( "Pixel Shader '%s' is %d instructions long!\n", ( r3dString( FName ) + ":" + this->Name ).c_str(), icount ) ;
			}
		}
		else
		{
			r3dArtBug( "Successfully disassembled the shader but couldn't find instruction count!\n" ) ;
		}

		SAFE_RELEASE( buffer ) ;
	}
#endif

	delete[] shaderCode;

	if( FAILED(hr) )
	{
		r3dOutToLog("Failed to create Pixel shader '%s' : \n", FName);
		if(hr == D3DERR_INVALIDCALL)       r3dOutToLog ("Invalid Call\n");
		if(hr == E_OUTOFMEMORY)            r3dOutToLog ("Out of memory\n");
		if(hr == D3DERR_OUTOFVIDEOMEMORY)  r3dOutToLog ("Out of Video Memory\n"); 
		// mystic DDERR_CURRENTLYNOTAVAIL
		if(hr == 0x88760028)  r3dOutToLog ("DDERR_CURRENTLYNOTAVAIL\n"); 
		r3dOutToLog("error=%d\n", hr);

		m_pShader = NULL;
		return 0;
	}

	//r3dOutToLog("PIXEL SHADER INIT %d\n%s<--\n\n", m_pShader, __pCodeCode);

	return 1;
}


void  r3dPixelShader :: SaveBinaryCache( const char* FName, ID3DXBuffer* pCode, const char* defines, const r3dTL::TArray< r3dString >& Includes )
{
	// return if we don't have cache dir
	if(r3d_access( R3D_BASE_SHADER_CACHE_PATH, 0 ) != 0)
		return;

	if( r3d_access( __r3dBaseShaderCachePath, 0 ) )
		mkdir( __r3dBaseShaderCachePath );

	if( r3d_access( __r3dBaseShaderCachePath, 0 ) )
		return;

	char FName2[512];
	r3dMakeCompiledShaderName(FName2, FName, defines);
	FILE* f1 = fopen(FName2, "wb");
	if (f1)
	{
		fwrite ( BIN_VER_SIG, sizeof BIN_VER_SIG, 1, f1 );
		WriteIncludesToFile( f1, Includes );

		fwrite (pCode->GetBufferPointer(), pCode->GetBufferSize(),1,f1);
		fclose(f1);
	}
	else
	{
		r3dOutToLog("ShaderCache: can't open %s for writing\n", FName2);
	}

}

int r3dPixelShader :: Load(const char* FName, int Type)
{
	r3dTL::TArray <D3DXMACRO> defines;
	return Load ( FName, Type, defines );
}

int r3dPixelShader :: Load( const char* FName, int Type, const r3dTL::TArray <D3DXMACRO> & defines )
{
	R3D_ENSURE_MAIN_THREAD();

	sprintf(FileName, "%s\\%s", __r3dBaseShaderPath, FName);

	char defines_string[256+128];
	memset(defines_string, 0, sizeof defines_string);
	for(unsigned int i=0; i<defines.Count(); ++i)
	{
		sprintf(&defines_string[strlen(defines_string)], "%s=%s,", defines[i].Name, defines[i].Definition);
	}

#if USE_PROJECT_NAME

#ifndef RENDER_PROJECT_NAME
	#ifdef PROJECT_MECHS
		#define RENDER_PROJECT_NAME "MECHS"
	#else
		#define RENDER_PROJECT_NAME "WARZ"
	#endif
#endif

		sprintf(&defines_string[strlen(defines_string)], "%s=1", RENDER_PROJECT_NAME); 
#endif 

#ifndef FINAL_BUILD
	if( r3d_access( FileName, 0 ) )
	{
		char msg[ 512 ];
		sprintf( msg, "Requested shader file '%s' doesn't exist!", FName );
		MessageBox( r3dRenderer->HLibWin, msg, "WARNING", MB_ICONEXCLAMATION );
	}
#endif

	if ( !LoadBinaryCache( FName, FileName, defines_string ) )
	{
		ID3DXBuffer*	pCode;
		HRESULT hr;

		r3dTL::TArray <D3DXMACRO> definesCpy( defines );

		definesCpy.PushBack(D3DXMACRO());
		definesCpy[definesCpy.Count() - 1].Name			= "VERTEX_SHADER";
		definesCpy[definesCpy.Count() - 1].Definition	= "0";

		definesCpy.PushBack(D3DXMACRO());
		definesCpy[definesCpy.Count() - 1].Name			= "PIXEL_SHADER";
		definesCpy[definesCpy.Count() - 1].Definition	= "1";

#if USE_PROJECT_NAME
		definesCpy.PushBack(D3DXMACRO());
		definesCpy[definesCpy.Count() - 1].Name = RENDER_PROJECT_NAME;
		definesCpy[definesCpy.Count() - 1].Definition = "1";
#endif

		definesCpy.PushBack(D3DXMACRO());
		definesCpy[definesCpy.Count() - 1].Name = 0;
		definesCpy[definesCpy.Count() - 1].Definition = 0;


		r3dTL::TArray < r3dString > Includes;

		if( !r3dCompileShader( FileName, &definesCpy[0], "main", r3dRenderer->PixelShaderProfileName, &pCode, &Includes ))
			return 0;

		Unload();

		// Create the pixel shader
		hr = r3dRenderer->pd3ddev->CreatePixelShader( (DWORD*)pCode->GetBufferPointer(), &m_pShader);

		r3dRenderer->CheckOutOfMemory( hr ) ;

		if( FAILED(hr) )
		{
			r3dOutToLog("Failed to create Pixel shader '%s' : ", FileName);
			if(hr == D3DERR_INVALIDCALL)       r3dOutToLog ("Invalid Call\n");
			if(hr == E_OUTOFMEMORY)            r3dOutToLog ("Out of memory\n");
			if(hr == D3DERR_OUTOFVIDEOMEMORY)  r3dOutToLog ("Out of Video Memory\n"); 
			else r3dOutToLog("error: %d\n", hr);

			m_pShader = NULL;
			pCode->Release();
			return 0;
		}

#ifdef _DEBUG
		_r3d_mShaderMap.insert(std::pair<DWORD, r3dSTLString>((DWORD)m_pShader, FileName));
		//r3dOutToLog("Shader[%x] created : %d, %s\n", m_dwShader, _iNumShaders, FileName);
#endif

		SaveBinaryCache( FName, pCode, defines_string, Includes );
		
		pCode->Release();
	}

	// save macros for reloading possibility ( TODO : make optional & disabled in final build )
	Macros.Resize( defines.Count());

	for( uint32_t i = 0, e = defines.Count(); i < e ; i ++ )
	{
		ShaderMacro& macro = Macros[ i ];
		const D3DXMACRO& dxmacro = defines[ i ];

		macro.Name			= dxmacro.Name;
		macro.Definition	= dxmacro.Definition;
	}

#if USE_PROJECT_NAME
	Macros.PushBack(ShaderMacro());
	Macros[Macros.Count() - 1].Name = RENDER_PROJECT_NAME;
	Macros[Macros.Count() - 1].Definition = "1";
#endif

	return 1;

}




void r3dPixelShader :: SetActive(int Act)
{
	if(!m_pShader) return;

	r3dRenderer->Flush();

	if(Act)
	{
		if( HRESULT hr = d3dc._SetPixelShader(m_pShader) != S_OK )
		{
			r3dError( "Failed to set %s pixel shader, err=%d\n", this->Name, hr ) ;
		}
	}
	else
	{
		D3D_V( d3dc._SetPixelShader(0) );
	}
}


