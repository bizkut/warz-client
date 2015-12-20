#pragma once


#include "..\..\External\pugiXML\src\pugixml.hpp"
#include "../UndoHistory/UndoHistory.h"
#include "../../../GameEngine/gameobjects/GameObj.h"

#define FNAME_LEVEL_EDITOR_SETTINGS		"%s/EditorSettings.ini"
#define FNAME_LEVEL_POSTPROCESS			"%s/Postprocess.ini"
#define FNAME_LEVEL_ATMOSPHERE			"%s\\Atmosphere.ini"
#define FNAME_LEVEL_CCCURVES			"%s/CCCurves.ini"

#define FNAME_CLOUD_TEMPLATE			"data/clouds/templates_clouds.ini"
#define FNAME_CLOUD_LIB					"data/clouds/clouds.cld"
#define LEVEL_SETTINGS_FILE				"%s/LevelSettings.xml"

class DecalGameObjectProxy;

typedef r3dgameVector(r3dSTLString) stringlist_t;

struct Editor_Level
{
  public:

	enum
	{
	  EDITMODE_SETTINGS = 0,
	  EDITMODE_TERRAIN,
	  EDITMODE_OBJECTS,
	  EDITMODE_MATERIALS,
	  EDITMODE_ENVIRONMENT,
	  EDITMODE_COLLECTION,
	  EDITMODE_DECORATORS,
	  EDITMODE_ROADS,
	  EDITMODE_GAMEPLAY,
	  EDITMODE_POSTFX,
	  EDITMODE_COLORGRADING,
	};


	int		MainToolIdx;
	int		TerrainEditMode;
	int		ObjectEditMode;

	// Terrain editing params (each brush has it's own settings)

	int		EnvironmentEditMode;

	r3dScreenBuffer* m_pPreviewBuffer;

	r3dColor	m_tPaintTerrainColor;

	class GameObject* m_DebugTexture;

	Editor_Level();

	~Editor_Level();

	void	Process(bool enable);
	void	ProcessSettings();
	void	ProcessTerrain();

	float	ProcessTerrain2_Settings( float SliderX, float SliderY );
	float	ProcessTerrain2_UpDown( float SliderX, float SliderY, int editMode, bool up );
	float	ProcessTerrain2_Level( float SliderX, float SliderY, int editMode );
	float	ProcessTerrain2_Smooth( float SliderX, float SliderY, int editMode );
	float	ProcessTerrain2_Noise( float SliderX, float SliderY, int editMode );
	float	ProcessTerrain2_Ramp( float SliderX, float SliderY, int editMode );
	float	ProcessTerrain2_Erosion( float SliderX, float SliderY, int editMode );
	float	ProcessTerrain2_Paint( float SliderX, float SliderY, int editMode );
	float	ProcessTerrain2_ColorPaint( float SliderX, float SliderY, int editMode );
	float	ProcessTerrain2_HMap( float SliderX, float SliderY );

	void	ProcessTerrain2();
	void	ProcessCreateTerrain2();

	bool	ProcessTerrain3_EditingAllowed( int leftButtonOnly );

	float	ProcessTerrain3_Settings( float SliderX, float SliderY );
	float	ProcessTerrain3_UpDown( float SliderX, float SliderY, int editMode, bool up );
	float	ProcessTerrain3_Level( float SliderX, float SliderY, int editMode );
	float	ProcessTerrain3_Smooth( float SliderX, float SliderY, int editMode );
	float	ProcessTerrain3_Noise( float SliderX, float SliderY, int editMode );
	float	ProcessTerrain3_Ramp( float SliderX, float SliderY, int editMode );
	float	ProcessTerrain3_Erosion( float SliderX, float SliderY, int editMode );
	float	ProcessTerrain3_Paint( float SliderX, float SliderY, int editMode );
	float	ProcessTerrain3_GlobalLayer( float SliderX, float SliderY, int editMode );
	float	ProcessTerrain3_ColorPaint( float SliderX, float SliderY, int editMode );
	float	ProcessTerrain3_HMap( float SliderX, float SliderY );
	float	ProcessTerrain3_ImportMask( float SliderX, float SliderY );
	float	ProcessTerrain3_Debug( float SliderX, float SliderY );

	void	ProcessTerrain3();

	void	ProcessNewTerrain( float SliderX, float SliderY );
	void	ProcessEnvironment();
	void	ProcessObjects();
	void	ProcessMaterials();
	void	ProcessGroups();
	void	ProcessPrefabs();
	void	ProcessDamageLib();
	void	ProcessUtils();
	void	ProcessCameraSpots();
	void	ProcessObjectDistances();
	void	ProcessMeshes();
	void	ProcessRoads();
	void	ProcessGamePlay ();

	void	ProcessPostFX();
	void	ProcessPost_Camera();
	void	ProcessPost_Glow();
	void	ProcessPost_Lighting();
	void	ProcessPost_NightVision();
	void	ProcessPost_Scopes();
	void	ProcessPost_Debug();

	void	ProcessColorGrading();
	float	ProcessColorCorrectionMenu( struct HUDFilterSettings &hfs, float SliderX, float SliderY );

	void	ProcessCollections();
	void	ProcessDecorators();

	void	ProcessAssets();
	void	ProcessParticleGun();

	void	ProcessNavigation();
	void	ProcessAutodeskNavigation(float SliderX, float SliderY);
	void	ProcessNavigationRegions(float SliderX, float SliderY);

	float	ProcessStaticSky( float SliderX, float SliderY );

	float	ProcessSkySun( float SliderX, float SliderY );
	float	ProcessLightSetup( float SliderX, float SliderY );

	void	ProcessKill();
	void	ProcessReveal();

	void	FlushStates();
	
	float	DrawPaintBounds		( float SliderX, float SliderY );

	void	PreInit				();
	void	Init				();
	void	Release				();

	void	SaveSettings		( const char* targetDir );
	int		LoadSettingsXML		();

	void	LoadSettings		();

	void	SaveLevel( const char* targetDir, bool showSign, bool autoSave );

	void	ToDamageLib( const char* MeshName );

private:
	int		ObjcategoryEditMode;
	char	DesiredDamageLibEntry[ MAX_PATH ];

};


class GameObject;

struct EntInfo_t
{
	uint32_t	objHashID; // safe pointer

	r3dPoint3D		vPos;
	r3dPoint3D		vScl;
	r3dPoint3D		vRot;
};




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoEntityChanged : public IUndoItem
{

private:

	static const UndoAction_e	ms_eActionID = UA_ENT_CHANGED;
	
	EntInfo_t			m_tDataOld;
	EntInfo_t			m_tDataNew;

	void				Set				( EntInfo_t &state );

public:

	void				Release			()	{ PURE_DELETE( this ); };
	UndoAction_e		GetActionID		()	{ return ms_eActionID; };

	void				Undo			() { Set( m_tDataOld ); }
	void				Redo			() { Set( m_tDataNew ); }

	void				Create			( EntInfo_t &state_old, EntInfo_t &state_new )
	{
		m_tDataOld = state_old;
		m_tDataNew = state_new;
	}

	UndoEntityChanged()
	{
		m_sTitle = "Entity change S/R/T";
	}

	static IUndoItem * CreateUndoItem	()
	{
		return game_new UndoEntityChanged;
	}


	static void Register()
	{
		UndoAction_t action;
		action.nActionID = ms_eActionID;
		action.pCreateUndoItem = CreateUndoItem;
		g_pUndoHistory->RegisterUndoAction( action );
	}
};

class UndoEntityGroupChanged : public IUndoItem
{

	static const UndoAction_e	ms_eActionID = UA_ENT_GROUP_CHANGED;

	EntInfo_t*			m_pDataOld;
	EntInfo_t*			m_pDataNew;
	int					m_numItems;

	void				Set				( EntInfo_t* pStates );

public:

	void				Release			()	{ SAFE_DELETE_ARRAY(m_pDataOld); SAFE_DELETE_ARRAY(m_pDataNew); PURE_DELETE( this ); };
	UndoAction_e		GetActionID		()	{ return ms_eActionID; };

	void				Undo			() { Set( m_pDataOld ); }
	void				Redo			() { Set( m_pDataNew ); }

	void				Create			(int numItems) 
	{ 
		r3d_assert(m_pDataNew==0 && m_pDataOld==0); 
		m_numItems=numItems; 
		m_pDataOld = game_new EntInfo_t[numItems]; 
		m_pDataNew = game_new EntInfo_t[numItems]; 
	}
	void				SetItem( int index, EntInfo_t &state_old, EntInfo_t &state_new )
	{
		m_pDataOld[index] = state_old;
		m_pDataNew[index] = state_new;
	}

	UndoEntityGroupChanged()
	{
		m_sTitle = "Group change S/R/T";
		m_pDataOld = 0;
		m_pDataNew = 0;
		m_numItems = 0;
	}

	static IUndoItem * CreateUndoItem	()
	{
		return game_new UndoEntityGroupChanged;
	}


	static void Register()
	{
		UndoAction_t action;
		action.nActionID = ms_eActionID;
		action.pCreateUndoItem = CreateUndoItem;
		g_pUndoHistory->RegisterUndoAction( action );
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class xml_buffer: public pugi::xml_writer
{
public:
	virtual void write(const void* data, size_t size)
	{
		uint32_t nOldSize = dData.Count();
		dData.Resize( nOldSize + (size2int_t)size );
		memcpy( &dData[ nOldSize ], data, size );
	}

	const char * GetData() const
	{
		return (const char *)&dData[0];
	}

	int GetSize() const
	{
		return dData.Count();
	}

private:
	r3dTL::TArray<uint8_t> dData;	
};

struct EntAddDel_t
{
	GameObject * pEnt;

	int EditorID;
	gobjid_t OwnerID;

	bool bDelete;	

	xml_buffer  data;

};


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class UndoEntityAddDel : public IUndoItem
{

private:

	static const UndoAction_e	ms_eActionID = UA_ENT_ADDDEL;
	
	r3dTL::TArray < EntAddDel_t > m_dData;

public:

	void				Release			()	{ PURE_DELETE( this ); };
	UndoAction_e		GetActionID		()	{ return ms_eActionID; };
	const FixedString&	GetTitle		();

	void				Undo			();
	void				Redo			();

	void				ExpandState		( EntAddDel_t &state );

	void				AddItem			( EntAddDel_t &state );
	void				Create			( EntAddDel_t &state );
	void				Create			();

	UndoEntityAddDel()
	{
	}

	static IUndoItem * CreateUndoItem	()
	{
		return game_new UndoEntityAddDel;
	}


	static void Register()
	{
		UndoAction_t action;
		action.nActionID = ms_eActionID;
		action.pCreateUndoItem = CreateUndoItem;
		g_pUndoHistory->RegisterUndoAction( action );
	}
};

//------------------------------------------------------------------------

class UndoReplaceObjects : public IUndoItem
{

public:

	static const UndoAction_e	ms_eActionID = UA_REPLACE_OBJECTS;

	struct UndoEntry
	{
		GameObject* Object;

		int EditorID;
		gobjid_t OwnerID;

		r3dString SrcType;
		r3dString TargType;

		xml_buffer xmlBuffer;
	};

private:
	r3dTL::TArray < UndoEntry > m_dData;

public:

	void				Release			()	{ PURE_DELETE( this ); };
	UndoAction_e		GetActionID		()	{ return ms_eActionID; };

	void				UndoRedo( bool redo );

	void				Undo			();
	void				Redo			();

	void				AddItem			( const UndoEntry &entry );

	UndoReplaceObjects()
	{
	}

	static IUndoItem * CreateUndoItem	()
	{
		return game_new UndoReplaceObjects;
	}


	static void Register()
	{
		UndoAction_t action;
		action.nActionID = ms_eActionID;
		action.pCreateUndoItem = CreateUndoItem;
		g_pUndoHistory->RegisterUndoAction( action );
	}
};

//------------------------------------------------------------------------


struct CatSkeletonConfig
{
	r3dSkeleton* DefaultSkeleton;

	CatSkeletonConfig();
};

struct  CategoryStruct
{
	int			Type;
	float		Offset;
	int			NumObjects;
	stringlist_t	ObjectsClasses;
	stringlist_t	ObjectsNames;
	stringlist_t	ObjectsSubfolders;

	CatSkeletonConfig	SkelConfig;
};

void InitObjCategories();
void CloseCategories();

r3dSkeleton* GetDefaultSkeleton( const char* MeshName );

void ReadXMLEditorSettingsStartPosition( r3dPoint3D* oPos );

void PushDebugBox(	r3dPoint3D p0, r3dPoint3D p1, r3dPoint3D p2, r3dPoint3D p3,
					r3dPoint3D p4, r3dPoint3D p5, r3dPoint3D p6, r3dPoint3D p7,
					r3dColor color );

void ResetDebugBoxes();

extern Editor_Level	LevelEditor;

