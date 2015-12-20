#pragma once

#include "../Include/Tsg_stl/TString.h"
#include "../Include/Tsg_stl/TArray.h"

enum UndoAction_e
{
	UA_ENT_CHANGED,
	UA_ENT_GROUP_CHANGED,
	UA_ENT_ADDDEL,
	UA_TERRAIN_HEIGHT,
	UA_TERRAIN_MASK_PAINT,
	UA_TERRAIN_COLOR_PAINT,
	UA_ALLOW_ROAD_SPLINE_EDIT,
	UA_DECAL_ADD,
	UA_DECAL_DEL,
	UA_FLOAT_CHANGE,
	UA_INT_CHANGE,
	UA_COLOR_CHANGE,
	UA_MATERIAL_TEXTURE_CHANGE,
	UA_STRING_CHANGE,
	UA_TIME_GRADIENT_CHANGE,
	UA_VECTOR3_CHANGE,
	UA_PARTICLE_EMITTER_ADDDEL,
	UA_WATER_GRID_CHANGE,
	UA_BOOL_CHANGE,
	UA_TERRAIN2_HEIGHT,
	UA_TERRAIN2_MASK_PAINT,
	UA_TERRAIN2_MASK_ERASEALL,
	UA_TERRAIN2_COLOR_PAINT,
	UA_TERRAIN2_DESTROY_LAYER,
	UA_TERRAIN2_INSERT_LAYER,
	UA_TERRAIN3_HEIGHT,
	UA_TERRAIN3_MASK_PAINT,
	UA_TERRAIN3_MASK_ERASEALL,
	UA_TERRAIN3_COLOR_PAINT,
	UA_WATERPLANE_SETTINGS_CHANGE,
	UA_COLLECTION_PAINT,
	UA_COLLECTION_TYPE_MANIPULATION,
	UA_GRASS_PAINT,
	UA_PREFAB_CREATE,
	UA_REPLACE_OBJECTS,
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class IUndoItem
{
	r3dTL::TArray< IUndoItem * >	m_dItems;
	
protected:
	FixedString	m_sTitle;
	uint32_t m_Tag;

public:
	IUndoItem()
	: m_Tag(0)
	{}

	void					AddNext			( IUndoItem * pNext )
	{
		m_dItems.PushBack( pNext );
	}

	void					ReleaseList		()
	{
		for ( int i = 0; i < (int)m_dItems.Count(); i++ )
		{
			m_dItems[ i ]->ReleaseList();
		}
		Release();
	}

	//--------------------------------------------------------------------------------------------------------
	void			UndoList			()
	{
		for ( int i = (int)m_dItems.Count() - 1; i >= 0; i-- )
		{
			m_dItems[ i ]->UndoList();
		}

		Undo();
	}

	//--------------------------------------------------------------------------------------------------------
	void			RedoList			()
	{
		Redo();

		for ( int i = 0; i < (int)m_dItems.Count(); i++ )
		{
			m_dItems[ i ]->RedoList();
		}
	}

	virtual void			Release			() = 0;
	virtual	UndoAction_e	GetActionID		() = 0;

	virtual void			Undo			() = 0;
	virtual void			Redo			() = 0;

	virtual const FixedString&	GetTitle		()							{ return m_sTitle; }
	void					SetTitle		( const char * szTitle )	{ m_sTitle = szTitle; }
	uint32_t				GetTag			() const  { return m_Tag; }
	void					SetTag			(uint32_t t) { m_Tag = t; }
};

typedef IUndoItem * ( * TCreateUndoFunc )();


struct UndoAction_t
{
	UndoAction_e			nActionID;
	TCreateUndoFunc		pCreateUndoItem;
};


class UndoHistory
{
	r3dTL::TArray< UndoAction_t >	m_pUndoAction;
	r3dTL::TArray< IUndoItem * >	m_pActionHistory;
	uint32_t						m_ForcedTag;
								

	int							m_nCurrentIndex;

	bool						IsValidItem			( int index )	{ return index >= 0 && index < (int)m_pActionHistory.Count(); };
								
	UndoAction_t *				GetUndoAction		( UndoAction_e actionID );

public:

	void						RegisterUndoAction	( const UndoAction_t &action );
								
								
	IUndoItem * 				CreateUndoItem		( UndoAction_e actionID, IUndoItem * pParent = NULL );

	IUndoItem *					GetUndoObject		()
	{

		if ( ! IsValidItem( m_nCurrentIndex ) ) 
			return NULL; 

		return m_pActionHistory[ m_nCurrentIndex ];
	}
								
								
	const char *				GetTitleUndo		();
								
	void						Undo				();
	void						Redo				();
								
	bool						CanUndo				();
	bool						CanRedo				();
								
	const char *				GetUndoInfo			();
	const char *				GetRedoInfo			();

	void						ClearUndoHistory	();
	void						RemoveUndoActionsByTag(uint32_t tag);
	void						SetTagForAllNextActions(uint32_t tag);

								UndoHistory		();
								~UndoHistory		();
};


extern UndoHistory *	g_pUndoHistory;
