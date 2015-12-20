#include "r3dPCH.h"
#include "r3d.h"

extern	char	__r3dBaseShaderPath[256];
extern	char	__r3dBaseShaderCachePath[256];

extern	r3dgfxMap(DWORD, r3dSTLString) _r3d_mShaderMap;

static char BIN_VER_SIG[] = "CVS100";

void WriteIncludesToFile( FILE* f1, const r3dTL::TArray< r3dString >& Includes );
void ReadIncludesFromFile( r3dFile* f1, r3dTL::TArray< r3dString >& oIncludes );

void r3dMakeCompiledShaderName(char* out, const char* shader, const char* defines)
{
  char temp[MAX_PATH];
  r3dscpy(temp, shader);
  for(char* fix = temp; *fix; fix++) {
    if(*fix == '\\' || *fix == '/')
      *fix = '_';
  }

  char drive[16], dir[ MAX_PATH], name[ MAX_PATH ], ext[ MAX_PATH ];

  _splitpath( temp, drive, dir, name, ext );

  r3d_assert( dir[ 0 ] == 0 );
  
  sprintf(out, "%s\\%s_%s.csr", __r3dBaseShaderCachePath, name, defines );
}

class r3dDXInclude : public ID3DXInclude
{
  private:
	char basePath[MAX_PATH];
	HRESULT __stdcall Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
	HRESULT __stdcall Close(LPCVOID pData);
  public:
	r3dDXInclude(const char* fname);

	r3dTL::TArray< r3dString > AccumIncludes;
};

r3dDXInclude::r3dDXInclude(const char* fname)
{
  // detect base directory from fname
  r3dscpy(basePath, fname);
  for(char* p = basePath; *p; p++)
    if(*p == '/') *p = '\\';
    
  char* p = strrchr(basePath, '\\');
  if(p) *p = 0;
  
  return;
}

HRESULT r3dDXInclude::Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
  *ppData = NULL;
  *pBytes = 0;

  //r3dOutToLog("r3dDXInclude::Open: %d %s\n", IncludeType, pFileName);
  char fname[MAX_PATH];
  sprintf(fname, "%s\\%s", basePath, pFileName);
  r3dFile* f = r3d_open(fname, "rb");
  if(!f) {
    r3dError("can't open shader include file %s (%s)\n", pFileName, fname);
    return E_FAIL;
  }

  char* buf = game_new char[f->size + 1];
  int size = fread(buf, 1, f->size, f);
  buf[size] = 0;
  fclose(f);

  AccumIncludes.PushBack( pFileName );
  
  *ppData = (void*)buf;
  *pBytes = size;
  return S_OK;
}

HRESULT r3dDXInclude::Close(LPCVOID pData)
{
  delete[] pData;
  return S_OK;
}

char LastCompilationError[ 16384 ];

int r3dCompileShader(
  const char* fname, 
  const D3DXMACRO* definesCpy, 
  const char* pFunctionName, 
  const char* pProfile,
  LPD3DXBUFFER* ppShader,
  r3dTL::TArray< r3dString > * pFilesIncluded
  )
{
  //r3dOutToLog("r3dCompileShader: %s\n", fname);

  DWORD flags = 0;
/*  
#ifdef _DEBUG
  flags |= D3DXSHADER_DEBUG|D3DXSHADER_SKIPOPTIMIZATION;
#endif
*/

  r3dFile* f = r3d_open(fname, "rb");
  if(!f) {
    r3dError("missing shader file %s\n", fname);
    return 0;
  }
  char* buf = game_new char[f->size + 1];
  int size = fread(buf, 1, f->size, f);
  buf[size] = 0;
  fclose(f);
  
  ID3DXBuffer* pError = NULL;
  HRESULT hr;
  r3dDXInclude include(fname);

  //hr = D3DXCompileShaderFromFile(fname, definesCpy, NULL, pFunctionName, pProfile, flags, ppShader, &pError, NULL);
  hr = D3DXCompileShader(buf, size, definesCpy, &include, pFunctionName, pProfile, flags, ppShader, &pError, NULL);
  delete[] buf;

  if( pFilesIncluded )
  {
	  pFilesIncluded->Swap( include.AccumIncludes );
  }

  if(FAILED(hr)) {
	  _snprintf( LastCompilationError, sizeof LastCompilationError - 1, "Shader compilation Error %s\n%s\n", fname, pError ? pError->GetBufferPointer() : "unknown error" );
    return 0;
  }
  
  return 1;
}



r3dVertexShader::r3dVertexShader()
{
  m_pShader   = NULL; 
  FileName[0] = 0;
  Name[0]     = 0;
}

void r3dVertexShader :: Unload()
{
	if(m_pShader)
	{
		SetActive(0);

		R3D_ENSURE_MAIN_THREAD();

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

int r3dVertexShader :: LoadBinaryCache(const char* FName, const char* Path, const char* defines)
{
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

	r3dFile* f = r3d_open(FName2,"rb");
	if (!f) return 0;

	if( !sourceAvailable && !f )
	{
		r3dError( "r3dVertexShader :: LoadBinaryCache: both source and cached version a missing(%s)!\n", Path );
	}

	char CheckSig[ sizeof BIN_VER_SIG ];
	fread( CheckSig, sizeof CheckSig, 1, f );

	if( memcmp( CheckSig, BIN_VER_SIG, sizeof BIN_VER_SIG ) )
	{
		if( !sourceAvailable )
		{
			r3dError( "r3dVertexShader::LoadBinaryCache: source code is missing and cached version is unknown(%s)!\n", Path );
		}
		else
		{
			r3dOutToLog( "r3dVertexShader::LoadBinaryCache: unsupported binary version!\n" );
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

	char* shaderCode = gfx_new char[ codeSize+ 1];
	fread(shaderCode, codeSize, 1, f);
	fclose(f);

	Unload();

	R3D_ENSURE_MAIN_THREAD();

	// Create the vertex shader
	HRESULT hr;
	hr = r3dRenderer->pd3ddev->CreateVertexShader( (DWORD*)shaderCode, &m_pShader);

	r3dRenderer->CheckOutOfMemory( hr ) ;

	delete[] shaderCode;

	if( FAILED(hr) )
	{
		r3dOutToLog("Failed to create Vertex shader '%s' : ", FName);
		if(hr == D3DERR_INVALIDCALL)       r3dOutToLog ("Error: Invalid Call\n");
		if(hr == E_OUTOFMEMORY)            r3dOutToLog ("Error: Out of memory\n");
		if(hr == D3DERR_OUTOFVIDEOMEMORY)  r3dOutToLog ("Error: Out of Video Memory\n"); 

		m_pShader = NULL;
		return 0;
	}

	// r3dOutToLog("VERTEX SHADER INIT %d  -->%s\n", m_pShader, __VSFileName);

	return 1;
}

void  r3dVertexShader :: SaveBinaryCache(const char* FName,    ID3DXBuffer*	pCode, const char* defines, const r3dTL::TArray< r3dString >& Includes )
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

		fwrite( pCode->GetBufferPointer(), pCode->GetBufferSize(), 1, f1 );
		fclose(f1);
	}
	else
	{
		r3dOutToLog("ShaderCache: can't open %s for writing\n", FName2);
	}
}

int r3dVertexShader :: Load(const char* FName, int Type )
{
	r3dTL::TArray <D3DXMACRO> defines;
	return Load ( FName, Type, defines );
}

int r3dVertexShader :: Load(const char* FName, int Type, const r3dTL::TArray <D3DXMACRO> & defines )
{
	sprintf(FileName, "%s\\%s", __r3dBaseShaderPath, FName);

	char defines_string[256];
	memset(defines_string, 0, 256);
	for(unsigned int i=0; i<defines.Count(); ++i)
	{
		sprintf(&defines_string[strlen(defines_string)], "%s=%s,", defines[i].Name, defines[i].Definition);
	}

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
		definesCpy[definesCpy.Count() - 1].Definition	= "1";

		definesCpy.PushBack(D3DXMACRO());
		definesCpy[definesCpy.Count() - 1].Name			= "PIXEL_SHADER";
		definesCpy[definesCpy.Count() - 1].Definition	= "0";

		definesCpy.PushBack(D3DXMACRO());
		definesCpy[definesCpy.Count() - 1].Name = 0;
		definesCpy[definesCpy.Count() - 1].Definition = 0;

		r3dTL::TArray< r3dString > Includes;

		if( !r3dCompileShader( FileName, &definesCpy[0], "main", r3dRenderer->VertexShaderProfileName, &pCode, &Includes ) )
			return 0;

		Unload();

		R3D_ENSURE_MAIN_THREAD();

		// Create the vertex shader
		hr = r3dRenderer->pd3ddev->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(), &m_pShader);

		r3dRenderer->CheckOutOfMemory( hr ) ;

		if( FAILED(hr) )
		{
			r3dOutToLog("Failed to create Vertex shader '%s' : \n", FileName);
			if(hr == D3DERR_INVALIDCALL)       r3dOutToLog ("Error: Invalid Call\n");
			if(hr == E_OUTOFMEMORY)            r3dOutToLog ("Error: Out of memory\n");
			if(hr == D3DERR_OUTOFVIDEOMEMORY)  r3dOutToLog ("Error: Out of Video Memory\n"); 
			// mystic DDERR_CURRENTLYNOTAVAIL
			if(hr == 0x88760028)  r3dOutToLog ("DDERR_CURRENTLYNOTAVAIL\n");

			r3dOutToLog("error=%d\n", hr);

			m_pShader = NULL;
			pCode->Release();
			return 0;
		}

#ifdef _DEBUG
		_r3d_mShaderMap.insert(std::pair<DWORD, r3dSTLString>((DWORD)m_pShader, FileName));
		//r3dOutToLog("Shader[%x] created : %d, %s\n", m_pShader, _iNumShaders, __VSFileName);
#endif

		SaveBinaryCache( FName, pCode, defines_string, Includes );

		pCode->Release();
	}

	// save macros for reloading possibility ( TODO : make optional & disabled in final build )
	Macros.Resize( defines.Count() );

	for( uint32_t i = 0, e = defines.Count(); i < e ; i ++ )
	{
		ShaderMacro& macro = Macros[ i ];
		const D3DXMACRO& dxmacro = defines[ i ];

		macro.Name			= dxmacro.Name;
		macro.Definition	= dxmacro.Definition;
	}

	return 1;
}


void r3dVertexShader :: SetActive(int Act)
{
	if (!m_pShader ) return;

	//  r3dRenderer->Flush();

	if(Act)
	{
		if( HRESULT hr = d3dc._SetVertexShader(m_pShader) != S_OK )
		{
			r3dError( "Failed to set %s vertex shader, err=%d", this->Name, hr ) ;
		}
	}
	else
	{
		D3D_V( d3dc._SetVertexShader(0) );
	}
}


void WriteIncludesToFile( FILE* f1, const r3dTL::TArray< r3dString >& Includes )
{
	uint16_t numIncludes = (uint16_t)Includes.Count();
	fwrite( &numIncludes, sizeof numIncludes, 1, f1 );

	for( uint32_t i = 0, e = numIncludes; i < e; i ++ )
	{
		const r3dString& s = Includes[ i ];
		uint16_t slen = (uint16_t) s.Length();

		fwrite( &slen, sizeof slen, 1, f1 );

		fwrite( s.c_str(), slen, 1, f1 );
	}
}

void ReadIncludesFromFile( r3dFile* f1, r3dTL::TArray< r3dString >& oIncludes )
{
	uint16_t count;

	fread( &count, sizeof count, 1, f1 );

	oIncludes.Reserve( count );

	for( uint32_t i = 0, e = count; i < count; i ++ )
	{
		uint16_t slen;

		fread( &slen, sizeof slen, 1, f1 );

		char* str = (char*)alloca( slen + 1 );
		if(str)
		{
			fread( str, slen, 1, f1 );
			str[ slen ] = 0;
			oIncludes.PushBack( str );
		}
		else
			r3d_assert(str && "Out of memory");
	}
}

