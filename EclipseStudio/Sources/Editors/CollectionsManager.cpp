//=========================================================================
//	Module: CollectionsManager.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "CollectionsManager.h"
#include "../GameLevel.h"
#include "../XMLHelpers.h"
#include "../../../gameengine/TrueNature/ITerrain.h"
#include "../ObjectsCode/Nature/wind.h"

//////////////////////////////////////////////////////////////////////////

#ifndef WO_SERVER
void SetLoadingProgress( float progress );
#endif

CollectionsManager gCollectionsManager;
extern float gCollectionsCastShadowDistance;
extern int gCollectionsCastShadows;

#define R3D_ENABLE_TREE_WIND 1

//////////////////////////////////////////////////////////////////////////

namespace
{
	const uint32_t COLL_ELEMS_HEADER_V1 = 'CEV1';
	const char * const COLLECTION_ELEMENTS_SOLID_FILENAME = "elements.bin";

//////////////////////////////////////////////////////////////////////////

	char PATH_BUF[MAX_PATH] = {0};

//////////////////////////////////////////////////////////////////////////

	const char * GetLevelCollectionsXMLTypeDefFile(bool forSave)
	{
		sprintf(PATH_BUF, "%s\\collections\\collections.xml", forSave ? r3dGameLevel::GetSaveDir() : r3dGameLevel::GetHomeDir());
		return PATH_BUF;
	}

//////////////////////////////////////////////////////////////////////////

	const char * GetLevelCollectionElementsFilePath(bool forSave, const char *fileName)
	{
		sprintf(PATH_BUF, "%s\\collections\\%s", forSave ? r3dGameLevel::GetSaveDir() : r3dGameLevel::GetHomeDir(), fileName);
		return PATH_BUF;
	}

//////////////////////////////////////////////////////////////////////////

	const char * GetLevelCollectionElementsSolidFileName()
	{
		return "elements.bin";
	}

//////////////////////////////////////////////////////////////////////////

	const char * GetLevelCollectionQuadTreeFile(bool forSave)
	{
		sprintf(PATH_BUF, "%s\\collections\\quadtree.bin", forSave ? r3dGameLevel::GetSaveDir() : r3dGameLevel::GetHomeDir());
		return PATH_BUF;
	}

//////////////////////////////////////////////////////////////////////////
	
	const char * GetLevelCollectionSaveDir()
	{
		sprintf(PATH_BUF, "%s\\collections", r3dGameLevel::GetSaveDir());
		return PATH_BUF;
	}

//////////////////////////////////////////////////////////////////////////

	const char * GetElementsCellFileName(int xIdx, int zIdx)
	{
		static char localBuf[MAX_PATH] = {0};
		sprintf_s(localBuf, _countof(localBuf), "elements_cell_%i_%i.bin", xIdx, zIdx);
		return localBuf;
	}

//////////////////////////////////////////////////////////////////////////

	void MakeHalf4(float x, float y, float z, float w, short *outHalf4)
	{
		outHalf4[0] = r3dFloatToHalf(x);
		outHalf4[1] = r3dFloatToHalf(y);
		outHalf4[2] = r3dFloatToHalf(z);
		outHalf4[3] = r3dFloatToHalf(w);
	}

//////////////////////////////////////////////////////////////////////////

	void MakeFloat4(short x, short y, short z, short w, float *outFloat4)
	{
		outFloat4[0] = r3dHalfToFloat(x);
		outFloat4[1] = r3dHalfToFloat(y);
		outFloat4[2] = r3dHalfToFloat(z);
		outFloat4[3] = r3dHalfToFloat(w);
	}

//////////////////////////////////////////////////////////////////////////

	void InststancedRenderChangeCallback(int oldI, float oldF);
	const int g_iInstancesInBatch = 512;
	r3dVertexBuffer * g_VB_InstanceData = NULL;

	IDirect3DVertexDeclaration9 * g_pInstanceDeclaration = NULL;
	IDirect3DVertexDeclaration9 * g_pInstanceDeclaration_Anim = NULL;

#pragma pack(push,1)
	struct InstanceData_t 
	{
		r3dPoint3D vPos;
		//	This is half4
		short rotMatColumn0[4];
		//	This is half4
		short rotMatColumn1[4];
		//	This is half4
		short scale_rand_color[4];
		UINT8 animCoefs[ 4 ] ;

		float * ConvertToShaderConstants() const
		{
			//	This is for guys who forget to change this functions after changing vertex format above
			COMPILE_ASSERT(sizeof(InstanceData_t) == 40);

			static float buf[4 * 4] = {0};

			//	Pos
			buf[0] = vPos.x;
			buf[1] = vPos.y;
			buf[2] = vPos.z;
			buf[3] = 0;

			//	col0
			MakeFloat4(rotMatColumn0[0], rotMatColumn0[1], rotMatColumn0[2], rotMatColumn0[3], &buf[4]);
			//	col1
			MakeFloat4(rotMatColumn1[0], rotMatColumn1[1], rotMatColumn1[2], rotMatColumn1[3], &buf[8]);
			//	scale and rand color
			MakeFloat4(scale_rand_color[0], scale_rand_color[1], scale_rand_color[2], scale_rand_color[3], &buf[12]);

			return buf;
		}
	};
#pragma pack(pop)


//////////////////////////////////////////////////////////////////////////

	int				gInstanceBufferOffset;
	InstanceData_t	*InstanceDataBuffer;

//////////////////////////////////////////////////////////////////////////
#ifndef WO_SERVER
	void Init_Draw_Instances ()
	{
		R3D_ENSURE_MAIN_THREAD();

		r_trees_noninst_render->SetChangeCallback(&InststancedRenderChangeCallback);

		InstanceDataBuffer = (InstanceData_t*)malloc( sizeof( InstanceData_t ) * g_iInstancesInBatch );
		if(InstanceDataBuffer==NULL)
			r3dError("Out of memory!");

		if (r_trees_noninst_render->GetBool())
			return;

		// VB
		{
			r3d_assert ( g_VB_InstanceData == NULL );
			g_VB_InstanceData = gfx_new r3dVertexBuffer( g_iInstancesInBatch, sizeof ( InstanceData_t ), 0, true );
		}

		// VDecl
		{
			const D3DVERTEXELEMENT9 InstanceDataDECL[] =
			{
				{1, 0,  D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3},
				{1, 12, D3DDECLTYPE_FLOAT16_4,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4},
				{1, 20, D3DDECLTYPE_FLOAT16_4,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 5},
				{1, 28, D3DDECLTYPE_FLOAT16_4,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 6},
				{1, 36, D3DDECLTYPE_UBYTE4N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 7},
				D3DDECL_END()
			};

			const uint32_t MESH_DECL_ELEMS = R3D_MESH_VERTEX::VBDeclElemCount - 1;
			const uint32_t BENDING_MESH_DECL_ELEMS = R3D_BENDING_MESH_VERTEX::VBDeclElemCount - 1;
			const uint32_t INSTANCE_DECL_ELEMS = sizeof InstanceDataDECL / sizeof InstanceDataDECL[ 0 ];

			D3DVERTEXELEMENT9 *FullDecl = (D3DVERTEXELEMENT9*)_alloca( (MESH_DECL_ELEMS + INSTANCE_DECL_ELEMS) * sizeof FullDecl[ 0 ] );

			r3d_assert( FullDecl );

			uint32_t i = 0;
			for( uint32_t e = MESH_DECL_ELEMS; i < e; i ++ )
			{
				FullDecl[ i ] = R3D_MESH_VERTEX::VBDecl[ i ];
			}

			for( uint32_t j = 0,  e = INSTANCE_DECL_ELEMS ; j < e; i ++, j ++ )
			{
				FullDecl[ i ] = InstanceDataDECL[ j ];
			}

			r3dDeviceTunnel::CreateVertexDeclaration( FullDecl, &g_pInstanceDeclaration ) ;

			D3DVERTEXELEMENT9 *FullDecl_ANIM = (D3DVERTEXELEMENT9*)_alloca( (BENDING_MESH_DECL_ELEMS + INSTANCE_DECL_ELEMS) * sizeof FullDecl_ANIM[ 0 ] ) ;

			r3d_assert( FullDecl_ANIM ) ;

			i = 0;
			for( uint32_t e = BENDING_MESH_DECL_ELEMS; i < e; i ++ )
			{
				FullDecl_ANIM[ i ] = R3D_BENDING_MESH_VERTEX::VBDecl[ i ];
			}

			for( uint32_t j = 0,  e = INSTANCE_DECL_ELEMS ; j < e; i ++, j ++ )
			{
				FullDecl_ANIM[ i ] = InstanceDataDECL[ j ];
			}

			r3dDeviceTunnel::CreateVertexDeclaration( FullDecl_ANIM, &g_pInstanceDeclaration_Anim ) ;
		}
	}

//////////////////////////////////////////////////////////////////////////

	void Done_Draw_Instances ()
	{
		R3D_ENSURE_MAIN_THREAD();
		SAFE_DELETE( g_VB_InstanceData );

		if(InstanceDataBuffer)
		{
			free( InstanceDataBuffer );
			InstanceDataBuffer = 0;
		}
	}

//////////////////////////////////////////////////////////////////////////

	void InststancedRenderChangeCallback(int oldI, float oldF)
	{
		Done_Draw_Instances();
	}

//////////////////////////////////////////////////////////////////////////

	void DrawInstancedMesh( r3dMesh * pMesh, int iCount, float dsp )
	{
		void *data = g_VB_InstanceData->Lock( gInstanceBufferOffset, iCount );

		memcpy( data, InstanceDataBuffer, sizeof( InstanceData_t ) * iCount );

		g_VB_InstanceData->Unlock();

		// Set up the geometry data stream
		r3dRenderer->SetStreamSourceFreq( 0, (D3DSTREAMSOURCE_INDEXEDDATA | iCount ));
		g_VB_InstanceData->Set( 1, gInstanceBufferOffset );	

		// do not support precise vertices for instanced meshes
		r3d_assert( !(pMesh->VertexFlags & r3dMesh::vfPrecise) );

		if( dsp )
		{
			{
				float vsConst[ 4 ] = { +dsp, 0.0f, 0.f, 0 };
				// float3 vExtrudeScale            : register( c17 );
				D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 17, vsConst, 1 ) );
				pMesh->DrawMeshSimpleInstances( iCount );
			}

			{
				float vsConst[ 4 ] = { -dsp, 0.0f, 0.f, 0 };
				// float3 vExtrudeScale            : register( c17 );
				D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 17, vsConst, 1 ) );
				pMesh->DrawMeshSimpleInstances( iCount );
			}

			{
				float vsConst[ 4 ] = { 0.0f, +dsp, 0.f, 0 };
				// float3 vExtrudeScale            : register( c17 );
				D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 17, vsConst, 1 ) );
				pMesh->DrawMeshSimpleInstances( iCount );
			}

			{
				float vsConst[ 4 ] = { 0.0f, -dsp, 0.f, 0 };
				// float3 vExtrudeScale            : register( c17 );
				D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 17, vsConst, 1 ) );
				pMesh->DrawMeshSimpleInstances( iCount );
			}
		}
		else
		{
			pMesh->DrawMeshSimpleInstances( iCount );
		}

	}

//////////////////////////////////////////////////////////////////////////

	void DrawNoninstancedMesh(r3dMesh * pMesh, int iCount, float dsp )
	{
		for (int i = 0; i < iCount; ++i)
		{
			InstanceData_t *t = &InstanceDataBuffer[i];
			//	Set instance data as vertex shader constant
			r3dRenderer->pd3ddev->SetVertexShaderConstantF(9, t->ConvertToShaderConstants(), 4);


			if( dsp )
			{
				{
					float vsConst[ 4 ] = { +dsp, 0.0f, 0.f, 0 };
					// float3 vExtrudeScale            : register( c17 );
					D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 17, vsConst, 1 ) );
					pMesh->DrawMeshSimpleInstances(1);
				}

				{
					float vsConst[ 4 ] = { -dsp, 0.0f, 0.f, 0 };
					// float3 vExtrudeScale            : register( c17 );
					D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 17, vsConst, 1 ) );
					pMesh->DrawMeshSimpleInstances(1);
				}

				{
					float vsConst[ 4 ] = { 0.0f, +dsp, 0.f, 0 };
					// float3 vExtrudeScale            : register( c17 );
					D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 17, vsConst, 1 ) );
					pMesh->DrawMeshSimpleInstances(1);
				}

				{
					float vsConst[ 4 ] = { 0.0f, -dsp, 0.f, 0 };
					// float3 vExtrudeScale            : register( c17 );
					D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 17, vsConst, 1 ) );
					pMesh->DrawMeshSimpleInstances(1);
				}
			}
			else
			{
				pMesh->DrawMeshSimpleInstances(1);
			}
		}
	}

//////////////////////////////////////////////////////////////////////////

	void SetupAnimationConstants( CollectionType &parms, float t )
	{
		D3DXVECTOR4 vParms[ 5 ] ;

		float stiffness = R3D_MAX ( parms.Stiffness, 0.01f ) ;

		r3dPoint3D windDir = g_pWind->GetWindDir() ;

		windDir.Normalize() ;

		vParms[ 0 ].x = windDir.x * parms.AnimLayers[ 0 ].Scale ;
		vParms[ 0 ].y = windDir.z * parms.AnimLayers[ 0 ].Scale ;
		vParms[ 0 ].z = parms.BendPow ;
		vParms[ 0 ].w = float( parms.LeafMotionRandomness * M_PI ) ;

		for( int i = 1, e = CollectionType::MAX_ANIM_LAYERS ; i < e ; i ++ )
		{
			const CollectionType::AnimLayer& layer = parms.AnimLayers[ i ] ;

			D3DXVECTOR4& target = vParms[ i ] ;

			target.x = windDir.x * layer.Freq ;
			target.y = windDir.z * layer.Freq ;
			target.z = windDir.x * layer.Scale ;
			target.w = windDir.z * layer.Scale ;
		}

		vParms[ 3 ] = D3DXVECTOR4( 
			parms.AnimLayers[ 0 ].Speed * t,
			parms.AnimLayers[ 1 ].Speed * t,
			parms.AnimLayers[ 2 ].Speed * t,
			0 ) ;

		vParms[ 4 ] = g_pWind->GetTexcXfm() ;

		D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 4, (float*)vParms, sizeof vParms / sizeof vParms[ 0 ] ) );

		r3dRenderer->SetTex( g_pWind->GetWindTexture(), D3DVERTEXTEXTURESAMPLER0 ) ;

		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP ) ) ;
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP ) ) ;

		r3dSetFiltering( R3D_BILINEAR, D3DVERTEXTEXTURESAMPLER0 );
	}

//-------------------------------------------------------------------------
//	Save helpers
//-------------------------------------------------------------------------

	struct SaveVisitorData
	{
		typedef r3dTL::TArray<const CollectionElement *> ElementsArray;
		typedef r3dTL::TArray<ElementsArray> ElementsGrid;
		ElementsGrid elements;

		int numXCells;
		int numZCells;
		float worldXSize;
		float worldZSize;

		SaveVisitorData(int nX, int nZ, float wX, float wZ)
		: numXCells(nX)
		, numZCells(nZ)
		, worldXSize(wX)
		, worldZSize(wZ)
		{
			elements.Resize(nX * nZ);
		}

		void GetCellCoords(int &idxX, int &idxZ, float worldXPos, float worldZPos)
		{
			float cellXSize = worldXSize / numXCells;
			float cellZSize = worldZSize / numZCells;

			idxX = static_cast<int>(worldXPos / cellXSize);
			idxZ = static_cast<int>(worldZPos / cellZSize);
		}

		ElementsArray & GetGridCell(float worldXPos, float worldZPos)
		{
			int idxX = 0, idxZ = 0;
			GetCellCoords(idxX, idxZ, worldXPos, worldZPos);
			int idx = idxX * numXCells + idxZ;

			r3d_assert(idx < static_cast<int>(elements.Count()));
			return elements[idx];
		}
	};

//////////////////////////////////////////////////////////////////////////

	const CollectionElement * GetElementByIndex(const SaveVisitorData::ElementsArray &c, int idx)
	{
		return c[idx];
	}

	const CollectionElement * GetElementByIndex(const CollectionsManager::Elements &c, int idx)
	{
		return &c[idx];
	}

//////////////////////////////////////////////////////////////////////////

	template <typename Container>
	bool SaveContainerElements(const Container &els, const char * fileName)
	{
		if (els.Count() == 0)
			return false;

		const char *fname = GetLevelCollectionElementsFilePath(true, fileName);
		FILE* f = fopen_for_write(fname, "wb");
		if (f)
		{
			r3dTL::fwrite_be(COLL_ELEMS_HEADER_V1, f);

			const int oneElementSize = sizeof(CollectionElementSerializableData);
			const int bodySize = oneElementSize * els.Count();
			char *buf = gfx_new char[bodySize];
			char *ptr = buf;
			for (uint32_t i = 0; i < els.Count(); ++i)
			{
				const CollectionElement *ce = GetElementByIndex(els, i);
				if (!ce)
					continue;

				ce->SaveToMemory(ptr);
				ptr += oneElementSize;
			}

			uint32_t size = ptr - buf;
			fwrite(buf, size, 1, f);
			delete [] buf;
			fclose(f);
		}

		return true;
	}

	void SaveCollectionElementsByZonesVisitor(const QuadTreeNode &n, void *userData)
	{
		SaveVisitorData *svd = reinterpret_cast<SaveVisitorData*>(userData);
		for (uint32_t i = 0; i < n.objects.Count(); ++i)
		{
			int id = n.objects[i];
			CollectionElement *ce = gCollectionsManager.GetElement(id);
			const r3dPoint3D &pos = ce->pos;

			SaveVisitorData::ElementsArray &ea = svd->GetGridCell(pos.x, pos.z);
			
			ea.PushBack(ce);
		}
	}
#endif

} // unnamed namespace

//////////////////////////////////////////////////////////////////////////

#ifndef WO_SERVER
void DoneDrawCollections()
{
	Done_Draw_Instances();
}
#endif
//-------------------------------------------------------------------------
//	CollectionsManager class
//-------------------------------------------------------------------------

CollectionsManager::CollectionsManager()
: prevUpdateTime(0)
, numSaveXCells(0)
, numSaveZCells(0)
, animT( 0 )
{
#ifndef WO_SERVER
	quadTree = NULL;
	visibleObjects.Reserve( 8192 );
#endif
}

//////////////////////////////////////////////////////////////////////////

CollectionsManager::~CollectionsManager()
{
	Destroy();
}

//////////////////////////////////////////////////////////////////////////

void CollectionsManager::Init( float loadProgressStart, float loadProgressEnd )
{
	const float PROGRESS_MIDDLE = ( loadProgressStart + loadProgressEnd ) * 0.5f;

	Destroy();

	r3dBoundBox bbs;
	if (Terrain)
	{
		const r3dTerrainDesc& desc = Terrain->GetDesc();
		bbs.Org = r3dPoint3D(0, desc.MinHeight, 0);
		bbs.Size = r3dPoint3D(desc.WorldSize, desc.MaxHeight - desc.MinHeight, desc.WorldSize);
	}

#ifndef WO_SERVER
	quadTree = gfx_new QuadTree(bbs, &CollectionElement_GetAABB);
#endif
	//	Load types
	LoadTypesFromXML();

	//	Load elements
	LoadElements( loadProgressStart, PROGRESS_MIDDLE );

	//	Load quadtree.
#ifndef WO_SERVER
	if (!quadTree)
		return;

	quadTree->LoadFromFile(GetLevelCollectionQuadTreeFile(false));

	//	Populate objects

	float start = r3dGetTime();

	r3dOutToLog( "CollectionsManager::Init: populating %d objects... ", elements.Count() );

	for (uint32_t i = 0; i < elements.Count(); ++i)
	{
		SetLoadingProgress( PROGRESS_MIDDLE + ( loadProgressEnd - PROGRESS_MIDDLE ) * i / elements.Count() );
		quadTree->AddObject(i);
	}

	r3dOutToLog( "done in %.2f seconds\n", r3dGetTime() - start );

	quadTree->OptimizeMemory();
#endif
}

//////////////////////////////////////////////////////////////////////////

void CollectionsManager::Destroy()
{
#ifndef WO_SERVER
	delete quadTree;
	quadTree = 0;
#endif
	for (uint32_t i = 0; i < GetTypesCount(); ++i)
	{
		DeleteType(i);
	}
	elements.Clear();
	types.Clear();
#ifndef WO_SERVER
	visibleObjects.Clear();
#endif
}

//////////////////////////////////////////////////////////////////////////

CollectionType * CollectionsManager::GetType(int idx)
{
	if (idx == INVALID_COLLECTION_INDEX || idx >= static_cast<int>(types.Count()) || types.IsFree(idx))
		return 0;

	return &types[idx];
}

//////////////////////////////////////////////////////////////////////////

uint32_t CollectionsManager::GetTypesCount() const
{
	return types.Count();
}

//////////////////////////////////////////////////////////////////////////

CollectionElement * CollectionsManager::GetElement(int idx)
{
	if (idx == INVALID_COLLECTION_INDEX || idx >= static_cast<int>(elements.Count()) || elements.IsFree(idx))
		return 0;

	return &elements[idx];
}

//////////////////////////////////////////////////////////////////////////

uint32_t CollectionsManager::GetElementsCount() const
{
	return elements.Count();
}

//////////////////////////////////////////////////////////////////////////

int CollectionsManager::CreateNewType()
{
	return types.GetFreeIndex();
}

//////////////////////////////////////////////////////////////////////////

void CollectionsManager::DeleteType(int idx)
{
	if (idx == INVALID_COLLECTION_INDEX || idx >= static_cast<int>(types.Count()) || types.IsFree(idx))
		return;

	types.SetFree(idx);

	//	Delete all elements with this type
	for (uint32_t i = 0; i < elements.Count(); ++i)
	{
		CollectionElement &e = elements[i];
		if (e.typeIndex == idx)
			DeleteElement(i);
	}

	types[idx].FreeMeshes();
}

//////////////////////////////////////////////////////////////////////////

bool CollectionsManager::LoadTypesFromXML()
{
	Bytes xmlFileBuffer;

	pugi::xml_document xmlLevelFile;
	pugi::xml_node xmlRoot ;

	const char *fname = GetLevelCollectionsXMLTypeDefFile(false);

	r3dFile* f = r3d_open(fname, "rb");
	if (f)
	{
		r3dOutToLog("Loading '%s'\n", fname) ;

		if (!ParseXMLInMemory(f, &xmlFileBuffer, &xmlLevelFile))
		{
			fclose(f);
			return false;
		}
		xmlRoot = xmlLevelFile.root();

		fclose(f);
	}
	else
	{
		return false;
	}

	//	We should use differed deletion of incorrect types, to make all valid types have corresponding IDs.
	r3dTL::TArray<int> toDel;

	pugi::xml_node params = xmlRoot.child("params");
	GetXMLVal("save_split_x", params, &numSaveXCells);
	GetXMLVal("save_split_z", params, &numSaveZCells);

	pugi::xml_node collTypeDesc = xmlRoot.child("collection");
	while (collTypeDesc)
	{
		int newIdx = CreateNewType();
		CollectionType * newType = GetType(newIdx);
		r3dString meshName;
		GetXMLVal("mesh", collTypeDesc, &meshName);
		bool rv = newType->InitMeshes(meshName.c_str());

		pugi::xml_node meshNode = collTypeDesc.child("mesh");
		pugi::xml_node animLayerNode = meshNode.child("anim_layer");

		int i = 0;
		while (animLayerNode)
		{

			GetXMLVal("scale", animLayerNode, &newType->AnimLayers[i].Scale);
			GetXMLVal("freq", animLayerNode, &newType->AnimLayers[i].Freq);
			GetXMLVal("speed", animLayerNode, &newType->AnimLayers[i].Speed);

			animLayerNode = animLayerNode.next_sibling("anim_layer");
			++i;
		}

		GetXMLVal("stiffness", meshNode, &newType->Stiffness);
		GetXMLVal("dissipation", meshNode, &newType->Dissipation);
		GetXMLVal("bend_pow", meshNode, &newType->BendPow);
		GetXMLVal("mass", meshNode, &newType->Mass);
		GetXMLVal("leaf_motion_randomness", meshNode, &newType->LeafMotionRandomness);

		GetXMLVal("scale", collTypeDesc, &newType->scale);
		GetXMLVal("render_dist", collTypeDesc, &newType->renderDist);
		GetXMLVal("lod1_dist", collTypeDesc, &newType->lod1Dist);
		GetXMLVal("lod2_dist", collTypeDesc, &newType->lod2Dist);
		int tmpI = 0;
		GetXMLVal("physics_enable", collTypeDesc, &tmpI);
		newType->physicsEnable = !!tmpI;
		tmpI = 0;
		GetXMLVal("anim_enable", collTypeDesc, &tmpI);
		newType->hasAnimation = !!tmpI;
		tmpI = 0;
		GetXMLVal("terrain_aligned", collTypeDesc, &tmpI);
		newType->terrainAligned = !!tmpI;

		GetXMLVal( "groupinfo_active", collTypeDesc, &newType->groupInfo.Active );
		GetXMLVal( "groupinfo_scale", collTypeDesc, &newType->groupInfo.Scale );
		GetXMLVal( "groupinfo_rotate", collTypeDesc, &newType->groupInfo.Rotate );
		GetXMLVal( "groupinfo_spacing", collTypeDesc, &newType->groupInfo.Spacing );
		GetXMLVal( "groupinfo_expanded_properties", collTypeDesc, &newType->groupInfo.ExpandedProperties );

		collTypeDesc = collTypeDesc.next_sibling("collection");

		if (!rv)
		{
			r3dOutToLog("COLLECTIONS: Error init collection type with mesh: %s\n", meshName);
			toDel.PushBack(newIdx);
		}
	}

	//	Deferred delete
	for (uint32_t i = 0; i < toDel.Count(); ++i)
	{
		DeleteType(toDel[i]);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
#ifndef WO_SERVER
void CollectionsManager::SaveTypesToXML()
{
	pugi::xml_document doc;

	//	Write save grid split parameters
	pugi::xml_node params = doc.append_child();
	params.set_name("params");
	SetXMLVal("save_split_x", params, &numSaveXCells);
	SetXMLVal("save_split_z", params, &numSaveZCells);

	for (uint32_t i = 0; i < types.Count(); ++i)
	{
		CollectionType *newType = GetType(i);
		if (!newType)
			continue;

		pugi::xml_node collTypeDesc = doc.append_child();
		collTypeDesc.set_name("collection");

		r3d_assert(newType->meshLOD[0]);

		SetXMLVal("mesh", collTypeDesc, &newType->meshLOD[0]->FileName);
		//	Save mesh params
		pugi::xml_node meshNode = collTypeDesc.child("mesh");
		for (int i = 0; i < CollectionType::MAX_ANIM_LAYERS; ++i)
		{
			pugi::xml_node animLayerNode = meshNode.append_child();
			animLayerNode.set_name("anim_layer");

			SetXMLVal("scale", animLayerNode, &newType->AnimLayers[i].Scale);
			SetXMLVal("freq", animLayerNode, &newType->AnimLayers[i].Freq);
			SetXMLVal("speed", animLayerNode, &newType->AnimLayers[i].Speed);
		}

		SetXMLVal("stiffness", meshNode, &newType->Stiffness);
		SetXMLVal("dissipation", meshNode, &newType->Dissipation);
		SetXMLVal("bend_pow", meshNode, &newType->BendPow);
		SetXMLVal("mass", meshNode, &newType->Mass);
		SetXMLVal("leaf_motion_randomness", meshNode, &newType->LeafMotionRandomness);

		SetXMLVal("scale", collTypeDesc, &newType->scale);
		SetXMLVal("render_dist", collTypeDesc, &newType->renderDist);
		SetXMLVal("lod1_dist", collTypeDesc, &newType->lod1Dist);
		SetXMLVal("lod2_dist", collTypeDesc, &newType->lod2Dist);
		int tmpI = newType->physicsEnable ? 1 : 0;
		SetXMLVal("physics_enable", collTypeDesc, &tmpI);
		tmpI = newType->hasAnimation ? 1 : 0;
		SetXMLVal("anim_enable", collTypeDesc, &tmpI);
		tmpI = newType->terrainAligned ? 1 : 0;
		SetXMLVal("terrain_aligned", collTypeDesc, &tmpI);

		SetXMLVal( "groupinfo_active", collTypeDesc, &newType->groupInfo.Active );
		SetXMLVal( "groupinfo_scale", collTypeDesc, &newType->groupInfo.Scale );
		SetXMLVal( "groupinfo_rotate", collTypeDesc, &newType->groupInfo.Rotate );
		SetXMLVal( "groupinfo_spacing", collTypeDesc, &newType->groupInfo.Spacing );
		SetXMLVal( "groupinfo_expanded_properties", collTypeDesc, &newType->groupInfo.ExpandedProperties );		
	}
	doc.save_file(GetLevelCollectionsXMLTypeDefFile(true));
}

//////////////////////////////////////////////////////////////////////////

void CollectionsManager::Save()
{
	r3dSTLString s(GetLevelCollectionSaveDir());
	s += "\\*.*";

	r3d_remove_files_in_folder(s.c_str());
	r3d_create_path(s.c_str());

	RebuildElementTypeIndices();
	SaveTypesToXML();
	SaveElements();

	if (quadTree)
		quadTree->SaveToFile(GetLevelCollectionQuadTreeFile(true));
}
#endif
//////////////////////////////////////////////////////////////////////////

bool CollectionsManager::LoadElements(float loadProgressStart, float loadProgressEnd)
{
	if (numSaveXCells == 0 || numSaveZCells == 0)
	{
		const char *fname = GetLevelCollectionElementsFilePath(false, COLLECTION_ELEMENTS_SOLID_FILENAME);
		return LoadElementsFile(fname, loadProgressStart, loadProgressEnd);
	}

	//	Load separate cells
	int totalCells = numSaveZCells * numSaveXCells;
	float oneCellProgress = (loadProgressEnd - loadProgressStart) / totalCells;
	bool rv = true;
	for (int i = 0; i < numSaveXCells; ++i)
	{
		for (int j = 0; j < numSaveZCells; ++j)
		{
			const char * path = GetLevelCollectionElementsFilePath(false, GetElementsCellFileName(i, j));
			float curProgress = loadProgressStart + oneCellProgress * (i * numSaveXCells + j);
			rv &= LoadElementsFile(path, curProgress, curProgress + oneCellProgress);
		}
	}
	return rv;
}

//////////////////////////////////////////////////////////////////////////

bool CollectionsManager::LoadElementsFile(const char * fname, float loadProgressStart, float loadProgressEnd)
{
	r3dFile* f = r3d_open(fname, "rb");
	if (!f)
	{
		r3dOutToLog("COLLECTIONS: Could not open '%s'\n", fname);
		return false;
	}

	const int oneElementSize = sizeof(CollectionElementSerializableData);
	if (f->size < sizeof(COLL_ELEMS_HEADER_V1))
	{
		fclose(f);
		r3dOutToLog("COLLECTIONS: Collection elements file is corrupted: '%s'", fname);
		return false;
	}

	uint32_t hdr = 0;
	r3dTL::fread_be(hdr, f);
	if (hdr != COLL_ELEMS_HEADER_V1)
	{
		fclose(f);
		r3dOutToLog("COLLECTIONS: Unsupported collection elements file format: '%s'", fname);
		return false;
	}

	const uint32_t bodySize = f->size - sizeof(hdr);

	if (bodySize % oneElementSize != 0)
	{
		fclose(f);
		r3dOutToLog("COLLECTIONS: Collection elements file is corrupted: '%s'", fname);
		return false;
	}

	const uint32_t numElements = bodySize / oneElementSize;
	elements.values.Reserve(elements.Count() + numElements);
	elements.freeIndices.Clear();

	char *buf = gfx_new char[bodySize];
	fread(buf, bodySize, 1, f);
	char *ptr = buf;
	fclose(f);

	r3dOutToLog( "Loading collection elements from file: %s...", fname );
	float timeStart = r3dGetTime();

	for (uint32_t i = 0; i < numElements; ++i, ptr += oneElementSize)
	{
#ifndef WO_SERVER
		SetLoadingProgress( loadProgressStart + ( loadProgressEnd - loadProgressStart ) * i / numElements );
#endif

		CollectionElement ce;
		ce.LoadFromMemory(ptr);
		if (ce.typeIndex >= 0 && ce.typeIndex < static_cast<int>(GetTypesCount()) && !types.IsFree(ce.typeIndex))
		{
			ce.InitPhysicsData();
			elements.values.PushBack(ce);
		}
	}
	delete [] buf;

	r3dOutToLog( "done in %.2f seconds\n", r3dGetTime() - timeStart );

	return true;
}

//////////////////////////////////////////////////////////////////////////
#ifndef WO_SERVER
bool CollectionsManager::SaveElements() const
{
	if (!quadTree)
		return true;

	if (numSaveXCells == 0 || numSaveZCells == 0)
	{
		//	Save entire file
		return SaveContainerElements(elements, "elements.bin");
	}

	const r3dTerrainDesc &td = Terrain->GetDesc();
	SaveVisitorData vd(numSaveXCells, numSaveZCells, td.XSize, td.ZSize);

	quadTree->VisitAllQuadTree(&SaveCollectionElementsByZonesVisitor, &vd, true);

	for (uint32_t i = 0; i < vd.elements.Count(); ++i)
	{
		if (vd.elements.Count() == 0)
			continue;

		int xIdx = i % vd.numXCells;
		int zIdx = i / vd.numZCells;

		const char * path = GetElementsCellFileName(xIdx, zIdx);
		SaveContainerElements(vd.elements[i], path);
	}
	return true;
}

#endif
//////////////////////////////////////////////////////////////////////////

int CollectionsManager::CreateNewElement()
{
	return elements.GetFreeIndex();
}

//////////////////////////////////////////////////////////////////////////

void CollectionsManager::DeleteElement(int idx)
{
	r3d_assert(elements.Count() > static_cast<uint32_t>(idx));
	elements[idx].typeIndex = INVALID_COLLECTION_INDEX;

#ifndef WO_SERVER
	if (quadTree)
		quadTree->RemoveObject(idx);
#endif
	elements[idx].ClearPhysicsData();

	elements.SetFree(idx);
}

//////////////////////////////////////////////////////////////////////////
#ifndef WO_SERVER
void CollectionsManager::RebuildElementTypeIndices()
{
	if (types.values.Count() == 0)
		return;
	
	for (uint32_t i = 0; i < types.freeIndices.Count(); ++i)
	{
		uint32_t freeIdx = types.freeIndices[i];
		for (uint32_t j = 0; j < elements.Count(); ++j)
		{
			CollectionElement &ce = elements[j];
			if (ce.typeIndex > static_cast<int>(freeIdx))
				--ce.typeIndex;
		}
	}

	struct Partitioner
	{
		const Types &t;
		explicit Partitioner(const Types &tps)
		: t(tps)
		{}

		bool operator() (CollectionType &a)
		{
			int idx = std::distance<const CollectionType*>(&t.values.GetFirst(), &a);
			return !t.IsFree(idx);
		}
	};

	CollectionType *start = &types.values.GetFirst();
	CollectionType *end = &types.values.GetLast() + 1;

	std::stable_partition(start, end, Partitioner(types));
	
	uint32_t startDelIdx = types.values.Count() - types.freeIndices.Count();
	uint32_t numDel = types.freeIndices.Count();

	types.values.Erase(startDelIdx, numDel);
	types.freeIndices.Clear();
}

//////////////////////////////////////////////////////////////////////////

void CollectionsManager::ComputeVisibility(bool shadows, bool directionalSM)
{
	if (!g_trees->GetInt() || !quadTree)
		return ;

	R3DPROFILE_FUNCTION( "CM::ComputeVisibility" );

	r3d_assert( !directionalSM || ( directionalSM && shadows ) );

	visibleObjects.Clear();

	bool doDirShadowCheck = directionalSM && r_shadows->GetInt() && r_shadowcull->GetInt();

	quadTree->ComputeVisibility(visibleObjects);
	
	if( DoesShadowCullNeedRecalc() && shadows && directionalSM )
	{
		for (uint32_t i = 0, i_end = visibleObjects.Count(); i < i_end; ++i)
		{
			CollectionElement &e = elements[visibleObjects[i]];
			e.shadowExDataDirty = true;
		}
	}

	bool needRecalcDirShadows = !r_inst_precalc_shadowcull->GetInt();

	int lastSlice = gCurrentShadowSlice == r_active_shadow_slices->GetInt() - 1;

	uint32_t arrSize = visibleObjects.Count();
	//	Calculate lod index for objects, remove that too far, and sort items by type + lod
	for (uint32_t i = 0; i < arrSize;)
	{
		int elementIndex = visibleObjects[i];
		CollectionElement &e = elements[elementIndex];
		const CollectionType &t = types[e.typeIndex];
		r3dMesh* m = t.meshLOD[0];
		if (!m)
		{
			std::swap(visibleObjects[arrSize - 1], visibleObjects[i]);
			--arrSize;
			continue;
		}

		const float loddist1 = t.lod1Dist;
		const float loddist2 = t.lod2Dist;
		const float typeRenderDist	= t.renderDist;
		const float distx = gCam.x - e.pos.x;
		const float distz = gCam.z - e.pos.z;
		const float distSq = distx * distx + distz * distz;
		const float fRad = m->localBBox.Size.Length() * 0.5f ;

		if
		(
			(distSq > typeRenderDist * typeRenderDist) ||
			(shadows && ( e.pos - instanceViewRefPos ).LengthSq() >= gCollectionsCastShadowDistance * gCollectionsCastShadowDistance)
		)
		{
			std::swap(visibleObjects[arrSize - 1], visibleObjects[i]);
			--arrSize;
			continue;
		}

		e.curLod = 0;
		if (distSq > loddist2 * loddist2 && t.meshLOD[2])
		{
			e.curLod = 2;
		}
		else if (distSq > loddist1 * loddist1 && t.meshLOD[1])
		{
			e.curLod = 1;
		}

		if( doDirShadowCheck )
		{
			extern int CheckDirShadowVisibility( ShadowExtrusionData& dataPtr, bool updateExData, const r3dBoundBox& bbox, const D3DXMATRIX &objMtx, const D3DXMATRIX& lightMtx, float extrude, D3DXPLANE (&mainFrustumPlanes)[ 6 ], float* ResultingExtrude );

			D3DXMATRIX mtx ;
			D3DXMatrixTranslation(&mtx, e.pos.x, e.pos.y, e.pos.z);

			if( !lastSlice )
			{
				if( !CheckDirShadowVisibility( e.shadowExData, e.shadowExDataDirty || needRecalcDirShadows, m->localBBox, mtx, r3dRenderer->ViewMatrix, ShadowSunOffset * 2.f, r3dRenderer->FrustumPlanes, 0 ) )
				{
					std::swap(visibleObjects[arrSize - 1], visibleObjects[i]);
					--arrSize;
					continue;
				}
				e.shadowExDataDirty = false ;
			}

			if( doDirShadowCheck )
			{
				D3DXVECTOR4 tpos ;

				D3DXVec3Transform( &tpos, (D3DXVECTOR3*)&e.pos, &r3dRenderer->ViewMatrix );

				float maX = tpos.x + fRad ;
				float miX = tpos.x - fRad ;
				float maY = tpos.y + fRad ;
				float miY = tpos.y - fRad ;

				AppendShadowOptimizations( &gShadowMapOptimizationDataOpaque[ gCurrentShadowSlice ], miX, maX, miY, maY );
			}
		}
		e.wasVisible = true;
		++i;
	}

	//	Now sort element by type + lod, to make contiguous chains
	struct ElementSorter
	{
		Elements &els;
		ElementSorter(Elements &e): els(e) {}
		bool operator()(int l, int r)
		{
			const CollectionElement &le = els[l];
			const CollectionElement &re = els[r];
			if (le.typeIndex == re.typeIndex)
			{
				return le.curLod < re.curLod;
			}
			return le.typeIndex < re.typeIndex;
		}
	};

	visibleObjects.Erase(arrSize, visibleObjects.Count() - arrSize);

	if (visibleObjects.Count() == 0)
		return;

	int *start = &visibleObjects.GetFirst();
	int *end = &visibleObjects.GetLast();
	ElementSorter sorter(elements);
	std::sort(start, end, sorter);
}

//////////////////////////////////////////////////////////////////////////

bool CollectionsManager::UpdateElementQuadTreePlacement(int idx)
{
	if (quadTree)
	{
		quadTree->RemoveObject(idx);
		return quadTree->AddObject(idx);
	}
	return false;
}

//------------------------------------------------------------------------

void CollectionsManager::SaveVisibility()
{
	saved_visibleObjects = visibleObjects;
}

//------------------------------------------------------------------------

void CollectionsManager::RestoreVisibility()
{
	visibleObjects = saved_visibleObjects;
}

//////////////////////////////////////////////////////////////////////////

void CollectionsManager::DEBUG_Draw()
{
#ifndef FINAL_BUILD
	if (quadTree)
	{
		if (r_show_collection_grid->GetInt() & 1)
		{
 			static r3dTL::TArray<QuadTreeObjectID> dummy;
 			quadTree->ComputeVisibility(dummy);

			float w = quadTree->GetWorldAABB().Size.x / 2;
			float scale = r3dRenderer->ScreenH * 0.8f / w;

			static r3dPoint3D scaleOffset(scale, 400.0f, 100.0f);
			float time = r3dGetFrameTime();
			float scaleRate = 1.0f * time * scale;
			float offsetRate = 300.0f * time;
			if (Keyboard->IsPressed(kbsPgUp))
			{
				scaleOffset.x += scaleRate;
			}
			if (Keyboard->IsPressed(kbsPgDn))
			{
				scaleOffset.x -= scaleRate;
			}
			if (Keyboard->IsPressed(kbsLeft))
			{
				scaleOffset.y -= offsetRate;
			}
			if (Keyboard->IsPressed(kbsRight))
			{
				scaleOffset.y += offsetRate;
			}
			if (Keyboard->IsPressed(kbsUp))
			{
				scaleOffset.z += offsetRate;
			}
			if (Keyboard->IsPressed(kbsDown))
			{
				scaleOffset.z -= offsetRate;
			}
			quadTree->DebugVisualizeTree2D(scaleOffset);
		}
		if (r_show_collection_grid->GetInt() >> 1)
		{
			quadTree->DebugVisualizeTree3D();
		}
	}
#endif
}

//////////////////////////////////////////////////////////////////////////

void CollectionsManager::Render(CollectionsDrawModeEnum drawMode)
{
	const bool nonInstancedRender = r_trees_noninst_render->GetBool();
	//compute visibility for main camera view and for shadows only once

	bool bShadowMap = drawMode == R3D_IDME_SHADOW ;

	if ( InstanceDataBuffer == NULL)
		Init_Draw_Instances ();

	if ( !gCollectionsCastShadows && bShadowMap )
		return;

	{
		// float3 vExtrudeScale            : register( c17 );
		float vsCommon[ 8 ] = { 0, 0, 0, 0 };
		r3dRenderer->pd3ddev->SetVertexShaderConstantF( 17, vsCommon, 1 );
	}

	float highlighOffset = 0.0f;

	if( drawMode == R3D_IDME_HIGHLIGHT1 )
	{
		highlighOffset = 3.3f * r_highlight_thickness->GetFloat();
	}

	D3DPERF_BeginEvent(D3DCOLOR_XRGB(255,255,255), L"CM::Render" );

	if (!nonInstancedRender)
	{
		r3d_assert ( g_VB_InstanceData );
		r3d_assert ( g_pInstanceDeclaration );
	}

	if( !bShadowMap )
	{
		r3dRenderer->SetTex(__r3dShadeTexture[SHADETEXT_DEFAULT_NORMAL], 1);
		r3dRenderer->SetTex(__r3dShadeTexture[SHADETEXT_WHITE], 2);
		r3dRenderer->SetTex(__r3dShadeTexture[SHADETEXT_WHITE], 3);
		r3dRenderer->SetTex(__r3dShadeTexture[SHADETEXT_BLACK], 4);
	}

	struct SetRestoreShadowStates
	{
		SetRestoreShadowStates( bool a_bShadow )
		{
			bShadow = a_bShadow;

			if( bShadow )
			{
				if( r3dRenderer->DoubleDepthShadowPath )
				{
					r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
				}
			}
		}

		~SetRestoreShadowStates()
		{
			if( bShadow )
			{
				if( r3dRenderer->DoubleDepthShadowPath )
				{
					r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				}
			}
		}

		bool bShadow;
	} setRestoreShadowStates( bShadowMap ); (void)setRestoreShadowStates;

	r3dMaterial::SetRenderedObjectColor( r3dColor::white ) ;

	r3dMaterial::ResetMaterialFilter();

	if (nonInstancedRender)
		d3dc._SetDecl(R3D_MESH_VERTEX::getDecl());
	else
		d3dc._SetDecl ( g_pInstanceDeclaration );

	uint32_t numObjects = visibleObjects.Count();
	uint32_t i = 0;

	float highlightDistSq = r_rc_highlight_radius->GetFloat();
	highlightDistSq *= highlightDistSq;
	
	r3dPoint3D cam_xz = gCam;

	cam_xz.y = 0;

	int isHighLight = drawMode == R3D_IDME_HIGHLIGHT0 || drawMode ==  R3D_IDME_HIGHLIGHT1;

	while (i < numObjects)
	{
		D3DXMATRIX mViewProj ;
		D3DXMatrixTranspose(&mViewProj, &r3dRenderer->ViewProjMatrix);

		// 		D3DXMATRIX Identity;
		// 		D3DXMatrixIdentity(&Identity);

		r3dRenderer->pd3ddev->SetVertexShaderConstantF ( 0, (float*)&mViewProj, 4 );
		//r3dRenderer->pd3ddev->SetVertexShaderConstantF ( 4, (float*)&Identity, 4 );

		CollectionElement &ce = elements[visibleObjects[i]];

		CollectionType &ct = types[ce.typeIndex];

		r3dBoundBox tMeshBox; 
		tMeshBox.Org = r3dPoint3D(0,0,0);
		tMeshBox.Size = r3dPoint3D(1,1,1);

		r3dMesh * pMesh = ct.meshLOD[ce.curLod];

		int iCount = 0;

		InstanceData_t* pInstances( NULL );

		int iSpaceTillBufferEnd = 0;

		int animated = 0 ;

		if ( pMesh )
		{

			// otherwise animated remains 0
#if R3D_ENABLE_TREE_WIND
			animated = pMesh->VertexFlags & r3dMesh::vfBending && ct.hasAnimation
				&&
				// otherwise static shadows will interfere with trees
				r_active_shadow_slices->GetInt() > 1
				;
#endif

			if( pMesh->NumMatChunks > 0 )
			{
				if( isHighLight )
				{
					if( !pMesh->HasMaterialOfType( "Concrete_Resource" ) && !pMesh->HasMaterialOfType( "Wood_Resources" ) && !pMesh->HasMaterialOfType( "Metal_Resources" ) )
					{
						i ++;
						continue;
					}
				}

				r3dMatVSType vsType = R3D_MATVS_COUNT;

				if (nonInstancedRender)
					vsType = animated ? R3D_MATVS_ANIMATEDFOREST_NONINSTANCED : R3D_MATVS_FOREST_NONINSTANCED;
				else
					vsType = animated ? R3D_MATVS_ANIMATEDFOREST : R3D_MATVS_FOREST;

				if( bShadowMap )
					pMesh->MatChunks[0].Mat->StartShadows( vsType );
				else
					pMesh->MatChunks[0].Mat->Start( vsType, drawMode == R3D_IDME_DEPTH ? R3D_MATF_NO_PIXEL_SHADER : 0 ) ;
			}

			if( drawMode == R3D_IDME_HIGHLIGHT0 )
			{
				r3dSetColorAndAlphaPixelShader( r3dColor( 255, 255, 0, 64 ) );
			}

			if( drawMode == R3D_IDME_HIGHLIGHT1 )
			{
				r3dSetColorAndAlphaPixelShader( r3dColor( 255, 255, 0, 4 ) );
			}

			// do not support precise vertices for instanced meshes
			r3d_assert( ! ( pMesh->VertexFlags & r3dMesh::vfPrecise ) );

			pMesh->SetupTexcUnpackScaleVSConst();

			pMesh->DrawMeshStartInstances ();

			if (nonInstancedRender)
			{
				if( pMesh->VertexFlags & r3dMesh::vfBending )
				{
					d3dc._SetDecl( R3D_BENDING_MESH_VERTEX::getDecl() );
				}
				else
				{
					d3dc._SetDecl( R3D_MESH_VERTEX::getDecl() );
				}
			}
			else
			{
				if( pMesh->VertexFlags & r3dMesh::vfBending )
				{
					d3dc._SetDecl ( g_pInstanceDeclaration_Anim );
				}
				else
				{
					d3dc._SetDecl ( g_pInstanceDeclaration );
				}
			}


			if (ct.hasAnimation)
			{
				SetupAnimationConstants( ct, animT );
			}		

			if (!nonInstancedRender)
			{
				// Set up the instance data stream
				r3dRenderer->SetStreamSourceFreq(1, (D3DSTREAMSOURCE_INSTANCEDATA | 1));
			}

			iSpaceTillBufferEnd = g_iInstancesInBatch - gInstanceBufferOffset;

			pInstances = InstanceDataBuffer;
		}

		while (i < numObjects && elements[visibleObjects[i]].curLod == ce.curLod && elements[visibleObjects[i]].typeIndex == ce.typeIndex)
		{
			CollectionElement &newEl = elements[visibleObjects[i ++]];

			if( isHighLight )
			{
				r3dPoint3D p = newEl.pos;
				p.y = 0;

				if( ( p - cam_xz ).LengthSq() >= highlightDistSq  )
				{
					continue;
				}
			}

			const float maxscale = 0.75;

			const r3dPoint3D &vPos (newEl.pos);

			float scale = newEl.scale;

			if ( pMesh )
			{
				//	Skip objects if we have noninstanced render and no physics object attached
				if (!nonInstancedRender || newEl.physObj)
				{
					float randColor = bShadowMap ? 1.0f : ( float ( newEl.randomColor ) / 255.0f );

					MakeHalf4(newEl.rotMat._11, newEl.rotMat._12, newEl.rotMat._13, 0, pInstances->rotMatColumn0);
					MakeHalf4(newEl.rotMat._21, newEl.rotMat._22, newEl.rotMat._23, 0, pInstances->rotMatColumn1);

					r3dVector finalScale = pMesh->unpackScale * scale;
					MakeHalf4(finalScale.x, finalScale.y, finalScale.z, randColor, pInstances->scale_rand_color);

					pInstances->vPos = vPos;

					if( animated )
					{
						pInstances->animCoefs[ 0 ] = UINT8( newEl.bendVal * 255 ) ;
						pInstances->animCoefs[ 1 ] = UINT8( ( newEl.bendVal * 255 - int ( newEl.bendVal * 255 ) ) * 255 ) ;
						pInstances->animCoefs[ 2 ] = UINT8( newEl.windPower * 255 ) ;
					}
					else
					{
						pInstances->animCoefs[ 0 ] = 0 ;
						pInstances->animCoefs[ 1 ] = 0 ;
						pInstances->animCoefs[ 2 ] = 0 ;
					}

					pInstances->animCoefs[ 3 ] = 0 ;

					pInstances++;
					iCount++;

					if( iCount == iSpaceTillBufferEnd )
					{
						if (nonInstancedRender)
							DrawNoninstancedMesh( pMesh, iCount, highlighOffset );
						else
							DrawInstancedMesh( pMesh, iCount, highlighOffset );

						pInstances = InstanceDataBuffer;

						iCount = 0;
						gInstanceBufferOffset = 0;
					}
				}
			}
		}

		if ( pMesh )
		{
			if ( iCount > 0 )
			{
				if (nonInstancedRender)
					DrawNoninstancedMesh( pMesh, iCount, highlighOffset );
				else
					DrawInstancedMesh( pMesh, iCount, highlighOffset );

				gInstanceBufferOffset += iCount;
			}

			if( pMesh->NumMatChunks > 0 )
			{
				if( bShadowMap )
					pMesh->MatChunks[0].Mat->EndShadows();
				else
					pMesh->MatChunks[0].Mat->End();
			}

			pMesh->DrawMeshEndInstances ();
		}
	}

	if (!nonInstancedRender)
	{
		// disable instancing
		r3dRenderer->SetStreamSourceFreq(0, 1);
		r3dRenderer->SetStreamSourceFreq(1, 1);
	}

	r3dRenderer->SetVertexShader();

	if( drawMode != R3D_IDME_DEPTH )
	{
		r3dRenderer->SetPixelShader();
	}

	d3dc._SetDecl( R3D_MESH_VERTEX::getDecl() ); 

	D3DPERF_EndEvent();
}
#endif
//////////////////////////////////////////////////////////////////////////

void CollectionsManager::SetInstanceViewRefPos(const r3dPoint3D& pos)
{
	instanceViewRefPos = pos;
}

//////////////////////////////////////////////////////////////////////////
#ifndef WO_SERVER
void CollectionsManager::Update()
{
	animT = r3dGetTime();

	UpdateWind();
}
#endif
//////////////////////////////////////////////////////////////////////////

#ifndef WO_SERVER
void CollectionsManager::GetElementsInRadius(const r3dPoint2D &center, float r, r3dTL::TArray<int> &objIndices) const
{
	if (!quadTree)
		return;

	//	Not precise test

	r3dBoundBox worldBB = quadTree->GetWorldAABB();
	r3dBoundBox bb;
	bb.Org = r3dPoint3D(center.x, worldBB.Org.y, center.y) - r3dPoint3D(r, 0, r);
	bb.Size = r3dPoint3D(r * 2, worldBB.Size.y, r * 2);

	quadTree->GetObjectsInRect(bb, objIndices);
}
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef WO_SERVER
void CollectionsManager::UpdateWind()
{
#if R3D_ENABLE_TREE_WIND

	R3DPROFILE_FUNCTION( "CM::UpdateWind" ) ;

	float nt = r3dGetTime() ;
	float dt = nt - prevUpdateTime;
	prevUpdateTime = nt;

	if( dt > 0.1f )
		dt = 0.1f ;

	for (uint32_t i = 0; i < elements.Count(); ++i)
	{
		CollectionElement &ce = elements[i];
		if( ce.typeIndex >= 0 && ce.wasVisible )
		{
			CollectionType &ct = types[ce.typeIndex];

			float stiffness		= ct.Stiffness ;
			float mass			= ct.Mass;
			float dissipation	= ct.Dissipation ;

			float force = g_pWind->GetWindAt( ce.pos ) / 255.f ;

			ce.windPower = force ;

			force = R3D_MAX( force - 0.25f, 0.f ) ;

			float ss = ( ce.bendVal - 0.5f ) ;
			float sign = ( ss > 0 ? 1.f : -1.f ) ;

			ss *= ss ;
			ss *= ss ;

			float pullBack = stiffness * ss * sign ;

			force -= pullBack ; 

			ce.bendSpeed += force * dt / mass ;

			ce.bendSpeed = R3D_MAX( R3D_MIN( ce.bendSpeed, 0.75f ), -0.75f ) ;

			ce.bendVal += ce.bendSpeed * dt ;

			float DISSIPATE_BORDER = ct.BorderDissipation ;

			float DISSIPATE_BORDER_LEFT = DISSIPATE_BORDER ;
			float DISSIPATE_BORDER_RIGHT = 1.0f - DISSIPATE_BORDER ;

			if( ce.bendVal < DISSIPATE_BORDER_LEFT )
			{
				if( ce.bendSpeed < 0 )
				{
					ce.bendSpeed -= ce.bendSpeed * R3D_MIN( ( DISSIPATE_BORDER_LEFT - ce.bendVal ) * dt * ct.BorderDissipationStrength, 1.f ) ;
				}

				if( ce.bendVal < 0 )
				{
					ce.bendVal = 0.f ;
				}
			}

			if( ce.bendVal > DISSIPATE_BORDER_RIGHT )
			{
				if( ce.bendSpeed > 0 )
				{
					ce.bendSpeed -= ce.bendSpeed * R3D_MIN( ( ce.bendVal - DISSIPATE_BORDER_RIGHT ) * dt * ct.BorderDissipationStrength, 1.f ) ;
				}

				if( ce.bendVal > 1.0f )
				{
					ce.bendVal = 1.0f ;
				}
			}

			if( fabs( ce.bendSpeed ) > 0.025f )
			{
				float toadd = dt * dissipation ;

				float prevSpeed = ce.bendSpeed ;

				ce.bendSpeed -= ( ce.bendSpeed > 0 ? 1 : -1 ) * toadd ;

				if( ce.bendSpeed * prevSpeed < 0 )
				{
					ce.bendSpeed = 0 ;
				}
			}
		}
		ce.wasVisible = false ;
	}
#endif
}
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef WO_SERVER
void CollectionsManager::GetUsedMaterials(r3dgfxVector(r3dMaterial*)& materials)
{
	for (uint32_t i = 0; i < GetElementsCount(); ++i)
	{
		CollectionElement *ce = GetElement(i);
		if (!ce)
			continue;

		CollectionType *ct = GetType(ce->typeIndex);
		if (!ct)
			continue;
		
		for (uint32_t k = 0; k < _countof(ct->meshLOD); ++k)
		{
			r3dMesh *m = ct->meshLOD[k];
			if (!m)
				continue;

			for (int j = 0; j < m->NumMatChunks; ++j)
			{
				r3dTriBatch &tb = m->MatChunks[j];
				if (std::find(materials.begin(), materials.end(), tb.Mat) == materials.end())
				{
					materials.push_back(tb.Mat);
				}
			}
		}
	}
}
#endif
//////////////////////////////////////////////////////////////////////////

#ifndef WO_SERVER
void CollectionsManager::GetElementsAlongRay(const r3dPoint3D &org, const r3dPoint3D &dir, r3dTL::TArray<int> &objIndices) const
{
	if (quadTree)
		quadTree->GetObjectsHitByRay(org, dir, objIndices);
}
#endif