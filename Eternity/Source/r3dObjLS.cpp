#include "r3dPCH.h"
#include "r3d.h"
#include "r3dBinMesh.h"

#include "r3dBackgroundTaskDispatcher.h"

#define DEFAULT_MAT_NAME ":default"

	int	_r3d_Mesh_LoadSecondUV = 1;

static int _r3dFinishObjectLoading(r3dMesh *obj)
{
	if(!obj->NumVertices || !obj->NumIndices) {
		return 0;
	}
	
	obj->RecalcBoundBox();

	r3d_assert(obj->VertexTangents);
	r3d_assert(obj->VertexTangentWs);
	r3d_assert(obj->VertexUVs);
	r3d_assert(obj->VertexNormals);
	r3d_assert(obj->VertexPositions);

	return 1;
}

/*virtual*/ r3dPoint3D* 
r3dMesh :: GetVertexPositions() const /*OVERRIDE*/
{
	return VertexPositions ;
}

/*virtual*/ uint32_t*
r3dMesh :: GetIndices() const /*OVERRIDE*/
{
	return Indices ;
}

void r3dMesh :: InitVertsList(int NumVerts)
{
	NumVertices     = NumVerts;

	r3d_assert(VertexPositions == NULL);
	r3d_assert(VertexUVs == NULL);
	r3d_assert(VertexNormals == NULL);
	r3d_assert(VertexTangents == NULL);
	r3d_assert(VertexTangentWs == NULL);

	VertexPositions = gfx_new r3dPoint3D[NumVertices];
	VertexUVs	= gfx_new r3dPoint2D[NumVertices];
	VertexNormals = gfx_new r3dVector[NumVertices];
	VertexTangents = gfx_new r3dVector[NumVertices];
	VertexTangentWs = gfx_new char[NumVertices];

	return;
}

void r3dMesh::InitIndexList(int numIndexes)
{
	NumIndices = numIndexes;
	Indices = gfx_new uint32_t[numIndexes];

	return;
}

bool getFileTimestamp(const char* fname, FILETIME& writeTime)
{
	HANDLE hFile = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if(hFile ==  INVALID_HANDLE_VALUE)
	{
		// we are reading from acrhive
		return false;
	}
	GetFileTime(hFile, 0, 0, &writeTime);
	CloseHandle(hFile);

	return true;
}

void setFileTimestamp(const char* fname, const FILETIME& time)
{
	HANDLE hFile = CreateFile(fname, GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_WRITE_ATTRIBUTES, 0);
	if(hFile ==  INVALID_HANDLE_VALUE)
	{
		r3dOutToLog("Error in setFileTimestamp! Error: %d", GetLastError()); // shouldn't be possible, as we are checking that file exist before calling this function
		return;
	}
	SetFileTime(hFile, &time, &time, &time);
	CloseHandle(hFile);
}

void ToSkinFileName( char (&skinFile)[256], const char* baseFile )
{
	r3dscpy(skinFile, baseFile);
	r3dscpy(&skinFile[strlen(skinFile)-3], "wgt");
}

struct MeshLoadParams : r3dTaskParams
{
	r3dMesh* Loadee;
	bool UseDefaultMaterial;
	bool UseThumbnails;
};

/*static*/
void r3dMesh::DoLoadMesh( r3dTaskParams* params )
{
	MeshLoadParams* mlp = static_cast< MeshLoadParams* >( params ) ;

	mlp->Loadee->DoLoad( mlp->UseDefaultMaterial, mlp->UseThumbnails ) ;
}

static r3dTaskParramsArray< MeshLoadParams > g_MeshLoadParamsArray ;

bool r3dMesh::Load(const char* fname, bool use_default_material /*= false*/, bool force_sync /*= true*/, bool use_thumbnails /*= false*/ )
{
	char szFixedName[MAX_PATH];	
	FixFileName(fname, szFixedName);
	FileName = szFixedName;

	MeshLoaded ( FileName.c_str () );

#if R3D_ALLOW_ASYNC_MESH_LOADING
	if( g_async_loading->GetInt() && R3D_IS_MAIN_THREAD() && !force_sync && g_pBackgroundTaskDispatcher )
	{
		r3dBackgroundTaskDispatcher::TaskDescriptor td;

		MeshLoadParams* params = g_MeshLoadParamsArray.Alloc();

		params->Loadee = this;
		params->UseDefaultMaterial = use_default_material;
		params->UseThumbnails = use_thumbnails;

		td.Fn				= DoLoadMesh;
		td.Params			= params;
		td.CompletionFlag	= NULL;
		td.Priority			= R3D_TASKPRIORITY_MESH;

		g_pBackgroundTaskDispatcher->AddTask( td );

		return TRUE;
	}
	else
#endif
	{
		return DoLoad( use_default_material, use_thumbnails ) ;
	}
}

void r3dMesh::LoadMaterials( bool use_thumbnails )
{

	for( int i = 0; i < NumMatChunks; i ++ )
	{
		if( MatChunks[ i ].MatName == DEFAULT_MAT_NAME )
		{
			MatChunks[ i ].Mat = r3dMaterialLibrary::GetDefaultMaterial();
		}
		else
		{
			MatChunks[ i ].Mat = r3dMaterialLibrary::RequestMaterialByMesh( MatChunks[ i ].MatName.c_str(), FileName.c_str(), Flags & obfPlayerMesh ? true : false, use_thumbnails );
		}
	}
}

void r3dMesh::TouchMaterials()
{
	for( int i = 0; i < NumMatChunks; i ++ )
	{
		if( r3dMaterial* mat = MatChunks[ i ].Mat )
			mat->TouchTextures();
	}
}

void r3dMesh::ReleaseMaterials()
{
	for( int i = 0; i < NumMatChunks; i ++ )
	{
		if( r3dMaterial* mat = MatChunks[ i ].Mat )
		{
			mat->ReleaseTextures();
		}
	}
}

#define SCB_EXT ".scb"

template< int N > 
static void ToBin( char (&outinFName)[ N ], const char* inFName )
{
	r3dscpy(outinFName, inFName);
	char* dot_start = strrchr(outinFName, '.');
	r3d_assert(dot_start);
	r3dscpy(dot_start, SCB_EXT);
}

/*static*/ bool r3dMesh::CanLoad( const char* fname )
{
	char bin_file[512];
	r3dscpy(bin_file, fname);

	ToBin( bin_file, fname ) ;

	bool bin_exist = r3d_access(bin_file, 0) == 0;
	bool txt_exist = r3d_access(fname, 0) == 0;

	if(!(txt_exist || bin_exist)) // if sco and scb both missing - then bail out
	{
		return false ;
	}

	return true ;
}

bool r3dMesh::DoLoad( bool use_default_material, bool use_thumbnails )
{
	r3dCSHolderWithDeviceQueue csholder( g_ResourceCritSection ) ; (void)csholder ;

	const char* fname = FileName.c_str() ;

	struct ArtBugComment
	{
		ArtBugComment( const char* fname )
		{
			r3dArtBugComment( ( r3dString( "Loading mesh " ) + fname ).c_str() ) ;
		}

		~ArtBugComment()
		{
			r3dArtBugComment( 0 ) ;
		}		
	} artBugComment( fname ) ;

	char bin_file[512];
	ToBin( bin_file, fname ) ;

	bool bin_exist = r3d_access(bin_file, 0) == 0;
	bool txt_exist = r3d_access(fname, 0) == 0;

	if(!(txt_exist || bin_exist)) // if sco and scb both missing - then bail out
	{
		// ptumik: show art bug, but do not crash editor
//#ifndef FINAL_BUILD
//		r3dError("Erorr: Missing mesh file: %s\n", fname);
//#endif
		r3dArtBug("r3dMesh::Load(): Can't load '%s' or '%s', file doesn't exist\n", fname, bin_file);
		return false;
	}
#ifndef FINAL_BUILD
	if(bin_exist && !txt_exist) // if bin exists but text sco is no longer there, show error and bail out
	{
		Flags |= obfMissingSource ;
		r3dArtBug("r3dMesh::Load(): Missing sco file for %s\n", fname);
	}
#endif

	bool res = false;

	bool save_bin = false;
	bool load_text = false;
	bool loading_archive = false;
	FILETIME creationTime;
	if(txt_exist && !getFileTimestamp(fname, creationTime))
		loading_archive = true;
	
	if(bin_exist)
	{
		FILETIME binCreationTime;
		if(!getFileTimestamp(bin_file, binCreationTime))
			loading_archive = true;
		
		if(!loading_archive && txt_exist &&
			(creationTime.dwLowDateTime != binCreationTime.dwLowDateTime ||
			creationTime.dwHighDateTime != binCreationTime.dwHighDateTime))
			load_text = true;
		else
		{
			r3dFile* f = r3d_open(bin_file, "rb");
			if(!LoadBin(f, use_default_material, use_thumbnails ))
				load_text = true;
			else
				res = true;
			fclose(f);
		}
	}
	else
		load_text = true;

	MatChunksNames = 0;
	if(load_text)
	{
		r3dFile* f = r3d_open(fname, "rt");
		int result = LoadAscii(f, use_default_material );
		if(result)
			res = true;
		else
			r3dArtBug("r3dMesh::Load(): Can't load '%s'\n", fname);
		fclose(f);

		if(result == 1) // save bin only if we didn't encounter any problems while loading text file
			save_bin = true;

		// to remain consistent
		if( !pWeights )
		{
			TryLoadWeights( fname );
		}
	}

	if(save_bin && !loading_archive)
	{
		this->OptimizeVCache() ;

		if(!SaveBin(bin_file) )
			r3dOutToLog("r3dMesh::Load(): Failed to save binary file mesh '%s'\n", bin_file);
		else
		{
			setFileTimestamp(bin_file, creationTime);
			// create ps3 version of mesh file
			// ptumik: disabled.
			/*char ps3_file[512];
			r3dscpy(ps3_file, fname);
			int len = strlen(fname);
			r3dscpy(&ps3_file[len-4], ".sc3");
			SaveBinPS3(ps3_file);
			setFileTimestamp(ps3_file, creationTime);*/
		}
	}
	if(MatChunksNames)
	{
		for(int i=0; i<NumMatChunks; ++i)
			delete [] MatChunksNames[i];
		delete [] MatChunksNames;
		MatChunksNames = 0;
	}

	// reset objects position to pivot. basically, in object's local coord system (0,0,0) = Pivot
	ResetXForm();

	FindAlphaTextures();

	return res;
}

int r3dMesh::LoadAscii( r3dFile *f, bool use_default_material )
{
	char inbuf[256], buf1[128];

	if(!f) 
		return 0;
	if(!f->IsValid()) 
		return 0;

	while(!feof(f)) {
		*inbuf = 0;
		if(fgets(inbuf, sizeof(inbuf), f) == NULL)
			break;
		if(strnicmp(inbuf,"[ObjectBegin]", 13) == 0)
			goto found;
	};

	return 0;

found:

	// Name=
	fgets(inbuf, sizeof(inbuf), f);
	sscanf(inbuf, "%s %s", buf1, Name);	
	if(stricmp("Name=", buf1))  {
		r3dArtBug("Invalid SCO\n");
		return 0;
	}

	int res = 1;

	r3dPoint3D CPoint;
	fgets(inbuf, sizeof(inbuf), f);
	sscanf(inbuf, "%s %f %f %f", buf1, &CPoint.X, &CPoint.Y, &CPoint.Z);

	// Verts=
	fgets(inbuf, sizeof(inbuf), f);
	sscanf(inbuf, "%s %d", buf1, &NumVertices);

	InitVertsList(NumVertices);
	vPivot        = CPoint;

	static bool showed_error = false;
	bool output_to_log_done = false;

	for(int i=0; i<NumVertices; i++)
	{
		fgets(inbuf, sizeof(inbuf), f);

		r3dPoint3D nrm;
		r3dPoint3D tangent;
		float      wtangent;
		int        clrr, clrg, clrb;
		int scanned = sscanf(inbuf, "%f %f %f %f %f %f %f %f %f %f %d %d %d", 
			&VertexPositions[i].x, 
			&VertexPositions[i].y, 
			&VertexPositions[i].z, 
			&nrm.x,
			&nrm.y,
			&nrm.z,
			&tangent.x,
			&tangent.y,
			&tangent.z,
			&wtangent,
			&clrr,
			&clrg,
			&clrb
			);

		// see if we have supplied normal
		if(scanned < 10)
		{
			res = 2; // make sure not to convert that file into binary, as there is something wrong with it
			if(!showed_error)
			{
				// DO NOT REMOVE THIS MESSAGE BOX!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// FUCK ARTISTS TO CORRECT MESHES, BUT DO NOT REMOVE IT!!!!
				MessageBox(0, "You are loading mesh of old format!\nPlease check log file (r3dlog.txt) for 'MESH_LOAD_BUG' and re export those meshes.\nThey might have visual artifacts!\nAnd that mesh will not be exported to PS3!\n\nThis message will appear only once.", "Error!", MB_OK);
				showed_error = true;
			}
			if(!output_to_log_done) // output to log only once per mesh
			{
				r3dArtBug("[MESH_LOAD_BUG] mesh file '%s' of old format. Please re-export it!!!\n", f->GetFileName());
				output_to_log_done = true;
			}
			tangent = r3dVector(1,0,0);
			wtangent = 1;
			if(scanned < 4)
				nrm = r3dVector(0,1,0);
		}
		
		if(scanned == 13)
		{
			if(VertexColors == NULL)
			{
				// precise & bending are not supported at the same time
				r3d_assert( !( VertexFlags & vfPrecise ) ) ;

				VertexColors = gfx_new r3dColor[NumVertices];
				VertexFlags  |= vfBending ;
			}
			
			VertexColors[i] = r3dColor(clrr, clrg, clrb);
		}

		VertexNormals[i] = nrm;
		VertexTangents[i] = tangent;
		VertexTangentWs[i] = char( wtangent > 0 ? 255 : 0 );
	}

	// Faces=
	fgets(inbuf, sizeof(inbuf), f);
	int numFaces = 0;
	sscanf(inbuf, "%s %d", buf1, &numFaces);

	InitIndexList(numFaces*3);

	char current_material[128];
	memset(current_material, 0, 128);
	NumMatChunks = 0;
	r3d_assert(MatChunksNames == 0);
	MatChunksNames = gfx_new char*[256];
	for(int i=0; i<numFaces; i++)
	{
		int	tmp1, VIdx[3];
		float	TX[3], TY[3];
		char	matname[128];

		fgets(inbuf, sizeof(inbuf), f);
		sscanf(inbuf, "%d %d %d %d %s %f %f %f %f %f %f",
			&tmp1,
			&VIdx[0], &VIdx[1], &VIdx[2],
			matname,
			&TX[0], &TY[0], &TX[1], &TY[1], &TX[2], &TY[2]
		);
		if(i==0)
		{
			r3dscpy(current_material, matname);
			MatChunksNames[NumMatChunks]  = game_new char[128];
			r3dscpy_s(MatChunksNames[NumMatChunks], 128, matname);
			MatChunks[NumMatChunks].StartIndex = 0;
			if( use_default_material )
			{
				MatChunks[NumMatChunks].Mat = r3dMaterialLibrary::GetDefaultMaterial() ;
			}
			else
			{
				MatChunks[NumMatChunks].Mat = r3dMaterialLibrary::RequestMaterialByMesh(matname, f->GetFileName(), Flags & obfPlayerMesh ? true : false, false );
			}
			extern int _r3d_MatLib_SkipAllMaterials;
			if(!_r3d_MatLib_SkipAllMaterials)
				r3d_assert(MatChunks[NumMatChunks].Mat);

			MatChunks[NumMatChunks].MatName = matname;
		}

		Indices[i*3+0] = VIdx[0];
		Indices[i*3+1] = VIdx[1];
		Indices[i*3+2] = VIdx[2];

		VertexUVs[VIdx[0]].x = TX[0];
		VertexUVs[VIdx[0]].y = TY[0];
		VertexUVs[VIdx[1]].x = TX[1];
		VertexUVs[VIdx[1]].y = TY[1];
		VertexUVs[VIdx[2]].x = TX[2];
		VertexUVs[VIdx[2]].y = TY[2];

		if(strcmp(current_material, matname)!=0)
		{
			// new material
			r3d_assert(i!=0);
			MatChunks[NumMatChunks].EndIndex = i*3;
			NumMatChunks++;
			r3d_assert(NumMatChunks < ConstNumMatChunks);

			MatChunks[NumMatChunks].StartIndex = i*3;

			if( use_default_material )
			{
				MatChunks[NumMatChunks].Mat = r3dMaterialLibrary::GetDefaultMaterial() ;
			}
			else
			{
				MatChunks[NumMatChunks].Mat = r3dMaterialLibrary::RequestMaterialByMesh(matname, f->GetFileName(), Flags & obfPlayerMesh ? true : false, false );
			}
			r3dscpy(current_material, matname);		
			MatChunksNames[NumMatChunks]  = game_new char[128];
			r3dscpy_s(MatChunksNames[NumMatChunks], 128, matname);
			MatChunks[NumMatChunks].MatName = matname;
		}
	}
	MatChunks[NumMatChunks].EndIndex = numFaces*3;
	++NumMatChunks;

	// read UV2 section if it exists
	fgets(inbuf, sizeof(inbuf), f);
	if(_r3d_Mesh_LoadSecondUV && strncmp(inbuf, "[UV2]", 5) == 0)
	{
		r3d_assert(VertexSecondUVs == NULL);
		VertexSecondUVs = gfx_new r3dPoint2D[NumVertices];
		VertexFlags |= vfHaveSecondUV;

		for(int i=0; i<NumVertices; i++)
		{
			fgets(inbuf, sizeof(inbuf), f);

			int scanned = sscanf(inbuf, "%f %f", &VertexSecondUVs[i].x, &VertexSecondUVs[i].y);
			if(scanned != 2)
			{
				MessageBox(0, "Bad UV2 mesh format", f->GetFileName(), MB_OK);
				break;
			}
		}
	}


	InterlockedExchange( &m_Loaded, 1 ) ;

	_r3dFinishObjectLoading(this);

	return res;
}

//-----------------------------------------------------------------------
// this version us also used on PS3! Update PS3 too if you change it
static const uint32_t R3DMESH_BINARY_VERSION = 0xFADC0038;
uint32_t R3DMESH_BINARY_VERSION_get() { return R3DMESH_BINARY_VERSION; }

// return false to notify to load text version of mesh
// return true - success

bool r3dMesh::LoadBin(r3dFile *f, bool use_default_material, bool use_thumbnails )
{
	if(!f->IsValid()) 
		return false;

	uint32_t version;
	fread(&version, sizeof(uint32_t), 1, f);
	if( version != R3DMESH_BINARY_VERSION )
		return false;

	DWORD flags = 0;
	fread(&flags, sizeof flags, 1, f);

	int len = 0;
	fread(&len, sizeof(len), 1, f);
	memset(Name, 0, sizeof(Name));
	fread(Name, len, 1, f);
	
	fread(&vPivot, sizeof(r3dPoint3D), 1, f);

	fread(&NumVertices, sizeof(NumVertices), 1, f);
	InitVertsList(NumVertices);

	fread(VertexPositions, sizeof(r3dPoint3D)*NumVertices, 1, f);
	fread(VertexUVs, sizeof(r3dPoint2D)*NumVertices, 1, f);
	fread(VertexNormals, sizeof(r3dVector)*NumVertices, 1, f);
	fread(VertexTangents, sizeof(r3dVector)*NumVertices, 1, f);

	fread( VertexTangentWs, sizeof VertexTangentWs[0] * NumVertices, 1, f );

	fread(&NumIndices, sizeof(NumIndices), 1, f);
	InitIndexList(NumIndices);
	fread(Indices, sizeof(uint32_t)*NumIndices, 1, f);

	fread(&NumMatChunks, sizeof(NumMatChunks), 1, f);
	for(int i=0; i<NumMatChunks; ++i)
	{
		fread(&MatChunks[i].StartIndex, sizeof(int), 1, f);
		fread(&MatChunks[i].EndIndex, sizeof(int), 1, f);
		int len = 0;
		fread(&len, sizeof(int), 1, f);
		char* mat_name = game_new char[len+1];
		memset(mat_name, 0, len+1);
		fread(mat_name, len, 1, f);
		if( use_default_material )
		{
			MatChunks[i].MatName = DEFAULT_MAT_NAME;
		}
		else
		{
			MatChunks[i].MatName = mat_name;
		}
		
		MatChunks[i].Mat = 0;

		delete [] mat_name;
	}

	LoadMaterials( use_thumbnails );

	if( flags & 1 )
	{
		AllocateWeights();

		LoadWeights_BinaryV1(f, true);
	}
	
	if( flags & 2 )
	{
		r3d_assert(VertexColors == NULL);
		VertexColors = gfx_new r3dColor[NumVertices];
		VertexFlags |= vfBending ;

		r3d_assert( !( VertexFlags & vfPrecise ) ) ;
		fread(VertexColors, sizeof(VertexColors[0])*NumVertices, 1, f);
	}

	if( (flags & 4) && _r3d_Mesh_LoadSecondUV )
	{
		r3d_assert(VertexSecondUVs == NULL);
		VertexSecondUVs = gfx_new r3dPoint2D[NumVertices];
		VertexFlags |= vfHaveSecondUV;

		r3d_assert( !( VertexFlags & vfPrecise ) ) ;
		fread(VertexSecondUVs, sizeof(VertexSecondUVs[0])*NumVertices, 1, f);
	}

	InterlockedExchange( &m_Loaded, 1 ) ;

	_r3dFinishObjectLoading(this);

	return true;
}

//-----------------------------------------------------------------------
bool r3dMesh::SaveBin(const char* fname)
{
	FILE* f = fopen(fname, "wb");
	if(!f) 
		return false;

	fwrite(&R3DMESH_BINARY_VERSION, sizeof(uint32_t), 1, f);

	DWORD flags = 0;
	if(pWeights)
		flags |= 1;
	if(VertexColors)
		flags |= 2;
	if(VertexSecondUVs)
		flags |= 4;
	fwrite(&flags, sizeof(DWORD), 1, f);

	int len = strlen(Name);
	fwrite(&len, sizeof(len), 1, f);
	fwrite(Name, len, 1, f);

	fwrite(&vPivot, sizeof(r3dPoint3D), 1, f);

	fwrite(&NumVertices, sizeof(NumVertices), 1, f);

	r3d_assert(VertexPositions);
	r3d_assert(VertexUVs);
	r3d_assert(VertexNormals);
	r3d_assert(VertexTangents);
	r3d_assert(VertexTangentWs);

	fwrite(VertexPositions, sizeof(r3dPoint3D)*NumVertices, 1, f);
	fwrite(VertexUVs, sizeof(r3dPoint2D)*NumVertices, 1, f);
	fwrite(VertexNormals, sizeof(r3dVector)*NumVertices, 1, f);
	fwrite(VertexTangents, sizeof(r3dVector)*NumVertices, 1, f);
	fwrite(VertexTangentWs, sizeof(VertexTangentWs[0]) * NumVertices, 1, f);

	fwrite(&NumIndices, sizeof(NumIndices), 1, f);
	fwrite(Indices, sizeof(uint32_t)*NumIndices, 1, f);

	fwrite(&NumMatChunks, sizeof(NumMatChunks), 1, f);
	for(int i=0; i<NumMatChunks; ++i)
	{
		fwrite(&MatChunks[i].StartIndex, sizeof(int), 1, f);
		fwrite(&MatChunks[i].EndIndex, sizeof(int), 1, f);
		int len = strlen(MatChunksNames[i]);//strlen(MatChunks[i].Mat->Name);
		fwrite(&len, sizeof(int), 1, f);
		//fwrite(MatChunks[i].Mat->Name, len, 1, f);
		fwrite(MatChunksNames[i], len, 1, f);
	}

	if( pWeights )
	{
		SaveWeights_BinaryV1(f);
	}
	
	if( VertexColors )
	{
		fwrite(VertexColors, sizeof(VertexColors[0])*NumVertices, 1, f);
	}
	
	if( VertexSecondUVs )
	{
		fwrite(VertexSecondUVs, sizeof(VertexSecondUVs[0])*NumVertices, 1, f);
	}

	fclose(f);
	return true;
}

using namespace r3dTL;
bool r3dMesh::SaveBinPS3(const char* fname)
{
	FILE* f = fopen(fname, "wb");
	if(!f) 
		return false;

	fwrite_be(R3DMESH_BINARY_VERSION, f);

	int len = strlen(Name);
	fwrite_be(len, f);
	fwrite_be(Name, len, f);

	fwrite_be(vPivot, f);

	fwrite_be(NumVertices, f);

	r3d_assert(VertexPositions);
	r3d_assert(VertexUVs);
	r3d_assert(VertexNormals);
	r3d_assert(VertexTangents);
	r3d_assert(VertexTangentWs);

	fwrite_be(VertexPositions, NumVertices, f);
	fwrite_be(VertexUVs, NumVertices, f);
	fwrite_be(VertexNormals, NumVertices, f);
	fwrite_be(VertexTangents, NumVertices, f);
	fwrite_be(VertexTangentWs, sizeof(VertexTangentWs[ 0 ]) * NumVertices, f);

	fwrite_be(NumIndices, f);
	fwrite_be(Indices, NumIndices, f);

	fwrite_be(NumMatChunks, f);
	for(int i=0; i<NumMatChunks; ++i)
	{
		fwrite_be(MatChunks[i].StartIndex, f);
		fwrite_be(MatChunks[i].EndIndex, f);
		int len = strlen(MatChunksNames[i]);
		fwrite_be(len, f);
		fwrite_be(MatChunksNames[i], len, f);
	}

	fclose(f);
	return true;
}

//------------------------------------------------------------------------

void
r3dMesh::FindAlphaTextures()
{
	r3d_assert( m_Loaded ) ;

	HasAlphaTextures = false;
	extern int _r3d_MatLib_SkipAllMaterials;
	if(_r3d_MatLib_SkipAllMaterials)
		return;

	for (int i=0;i<NumMatChunks;i++)
	{
		if( r3dMaterial* Mat = MatChunks[i].Mat )
		{
			if( Mat->Flags & R3D_MAT_HASALPHA )
			{
				HasAlphaTextures = true;
				break;
			}
		}
	}	

}

//------------------------------------------------------------------------

/*static*/
void
r3dMesh::Init()
{
	g_MeshLoadParamsArray.Init( 64 ) ;
}

//------------------------------------------------------------------------

/*static*/
void
r3dMesh::Close()
{

}