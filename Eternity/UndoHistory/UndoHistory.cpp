#include "r3dPCH.h"
#ifndef FINAL_BUILD

#include "r3d.h"
#include "../SF/Console/Config.h"

#include "UndoHistory.h"

UndoHistory *	g_pUndoHistory = NULL;

//--------------------------------------------------------------------------------------------------------
UndoHistory::UndoHistory()
: m_nCurrentIndex( INVALID_INDEX )
, m_ForcedTag(0)
{
}

//--------------------------------------------------------------------------------------------------------
UndoHistory::~UndoHistory()
{
	ClearUndoHistory();
}

//--------------------------------------------------------------------------------------------------------
void UndoHistory::RegisterUndoAction( const UndoAction_t &action )
{
	if ( ! action.pCreateUndoItem )
	{
		r3dOutToLog( "#CUndoHistory::RegisterUndoAction : Action id: %d create undo item function is not valid.\n", action.nActionID );
		return;
	}

	if ( GetUndoAction( action.nActionID ) )
	{
		r3dOutToLog( "#CUndoHistory::RegisterUndoAction : Action id: %d already exists\n", action.nActionID );
		return;
	}

	m_pUndoAction.PushBack( action );
}

//--------------------------------------------------------------------------------------------------------
UndoAction_t * UndoHistory::GetUndoAction( UndoAction_e actionID )
{
	uint32_t i;

	for ( i = 0; i < m_pUndoAction.Count(); i++ ) 
	{
		if ( m_pUndoAction[ i ].nActionID == actionID )
			return &m_pUndoAction[ i ];
	}

	return NULL;
}

//--------------------------------------------------------------------------------------------------------
IUndoItem * UndoHistory::CreateUndoItem( UndoAction_e actionID, IUndoItem * pParent )
{
	int i;

	UndoAction_t * pAction = GetUndoAction( actionID );
	if ( ! pAction )
	{
		r3dOutToLog( "#UndoHistory::CreateUndoItem : Action id: %d not registered\n", actionID );
		return NULL;
	}


	IUndoItem * pItem = pAction->pCreateUndoItem();
	if ( ! pItem )
	{
		r3dOutToLog( "#UndoHistory::CreateUndoItem : can't create undo action id: %d\n", actionID );
		return NULL;
	}

	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	>	Link to list
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	if ( pParent )
	{
		pParent->AddNext( pItem );
	}
	else
	{
		int size = m_pActionHistory.Count() - m_nCurrentIndex - 1;

		for ( i = m_nCurrentIndex; i < m_nCurrentIndex + size; i++ ) 
		{
			m_pActionHistory[ i + 1 ]->ReleaseList();
		}
		if ( size )
		{
			m_pActionHistory.Erase( m_nCurrentIndex + 1, size );
		}
		m_nCurrentIndex = m_pActionHistory.Count() - 1;
		
		if ( (int)m_pActionHistory.Count() >= e_undo_depth->GetInt() )
		{

			m_pActionHistory[ 0 ]->ReleaseList();
			m_pActionHistory.Erase( 0 );
			if ( m_nCurrentIndex >= 0 )
			{
				m_nCurrentIndex--;
			}
		}

		m_nCurrentIndex++;
		m_pActionHistory.PushBack( pItem );
	}

	if (m_ForcedTag != 0)
		pItem->SetTag(m_ForcedTag);

	return pItem;
}

//--------------------------------------------------------------------------------------------------------
void UndoHistory::Undo()
{
	if ( ! IsValidItem( m_nCurrentIndex ) )
		return;

	IUndoItem * pItem = m_pActionHistory[ m_nCurrentIndex ];
	pItem->UndoList();
	m_nCurrentIndex--;
}

//--------------------------------------------------------------------------------------------------------
void UndoHistory::Redo()
{
	if ( ! IsValidItem( m_nCurrentIndex + 1 ) )
		return;

	m_nCurrentIndex++;
	
	IUndoItem * pItem = m_pActionHistory[ m_nCurrentIndex ];
	pItem->RedoList();
}

//--------------------------------------------------------------------------------------------------------
const char * UndoHistory::GetTitleUndo()
{ 
	static char szText[ MAX_PATH ];
	static char szNull[] = "";
	
	if ( ! IsValidItem( m_nCurrentIndex ) ) 
		return szNull; 


	sprintf( szText, "%s (%d of %d)", m_pActionHistory[ m_nCurrentIndex ]->GetTitle().c_str(), m_nCurrentIndex + 1, m_pActionHistory.Count() );

	return szText;
}

//--------------------------------------------------------------------------------------------------------
const char * UndoHistory::GetUndoInfo()
{
	if ( ! IsValidItem( m_nCurrentIndex ) ) 
		return "";

	return m_pActionHistory[ m_nCurrentIndex ]->GetTitle().c_str();
}

//--------------------------------------------------------------------------------------------------------
const char * UndoHistory::GetRedoInfo()
{
	
	if ( ! IsValidItem( m_nCurrentIndex + 1  ) ) 
		return "";

	return m_pActionHistory[ m_nCurrentIndex + 1 ]->GetTitle().c_str();
}

//--------------------------------------------------------------------------------------------------------
bool UndoHistory::CanUndo()
{
	return IsValidItem( m_nCurrentIndex ) ? true : false;
}

//--------------------------------------------------------------------------------------------------------
bool UndoHistory::CanRedo()
{
	if ( ! IsValidItem( m_nCurrentIndex + 1 ) )
		return false;
	
	return true;
}

//--------------------------------------------------------------------------------------------------------
void UndoHistory::ClearUndoHistory()
{
	uint32_t i;

	for ( i = 0; i < m_pActionHistory.Count(); i++ ) 
	{
		m_pActionHistory[ i ]->ReleaseList();
	}
	m_pActionHistory.Clear();
	m_nCurrentIndex = INVALID_INDEX;
}

//--------------------------------------------------------------------------------------------------------
void UndoHistory::RemoveUndoActionsByTag(uint32_t tag)
{
	uint32_t i = 0;

	while (i < m_pActionHistory.Count())
	{
		IUndoItem *a = m_pActionHistory[ i ];
		if (a->GetTag() == tag)
		{
			a->ReleaseList();
			m_pActionHistory.Erase(i);
			if (m_nCurrentIndex >= static_cast<int>(i))
				--m_nCurrentIndex;
		}
		else
		{
			++i;
		}
		
	}
}

//--------------------------------------------------------------------------------------------------------

void UndoHistory::SetTagForAllNextActions(uint32_t t)
{
	m_ForcedTag = t;
}

#endif