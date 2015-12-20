//=========================================================================
//	Module: LevelEditor_CollectionsUndo.cpp
//	Copyright (C) 2013.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#ifndef FINAL_BUILD

#include "LevelEditor_CollectionsUndo.h"
#include "CollectionsManager.h"

//////////////////////////////////////////////////////////////////////////

namespace
{
	CollectionsBrushUndo *gCollectionBrushUndoItem = 0;
}

//-------------------------------------------------------------------------
//	Undo/Redo actions
//-------------------------------------------------------------------------

CollectionsBrushUndo::CollectionsBrushUndo()
{
	SetTitle("Collections brush");
}

//////////////////////////////////////////////////////////////////////////

IUndoItem * CollectionsBrushUndo::CreateUndoItem()
{
	return game_new CollectionsBrushUndo;
}

//////////////////////////////////////////////////////////////////////////

void CollectionsBrushUndo::Release()
{
	delete this;
}

//////////////////////////////////////////////////////////////////////////

UndoAction_e CollectionsBrushUndo::GetActionID()
{
	return ms_eActionID;
}

//////////////////////////////////////////////////////////////////////////

void CollectionsBrushUndo::UndoAddElements()
{
	for (uint32_t i = 0; i < collectionElementIDs.Count(); ++i)
	{
		int idx = collectionElementIDs[i];
		CollectionElement* el = gCollectionsManager.GetElement(idx);
		AddData(el);

		gCollectionsManager.DeleteElement(idx);
	}
	collectionElementIDs.Clear();
}

//////////////////////////////////////////////////////////////////////////

void CollectionsBrushUndo::UndoRemoveElements()
{
	for (uint32_t i = 0; i < collectionElements.Count(); ++i)
	{
		int newIdx = gCollectionsManager.CreateNewElement();
		CollectionElement *el = gCollectionsManager.GetElement(newIdx);
		*static_cast<CollectionElementSerializableData*>(el) = collectionElements[i];

		el->InitPhysicsData();

		collectionElementIDs.PushBack(newIdx);
		gCollectionsManager.UpdateElementQuadTreePlacement(newIdx);
	}
	collectionElements.Clear();
}

//////////////////////////////////////////////////////////////////////////

void CollectionsBrushUndo::Undo()
{
	if (collectionElementIDs.Count() > 0)
	{
		UndoAddElements();
	}
	else if (collectionElements.Count() > 0)
	{
		UndoRemoveElements();
	}
}

//////////////////////////////////////////////////////////////////////////

void CollectionsBrushUndo::Redo()
{
	Undo();
}

//////////////////////////////////////////////////////////////////////////

void CollectionsBrushUndo::AddData(int collectionElementID)
{
	collectionElementIDs.PushBack(collectionElementID);
}

//////////////////////////////////////////////////////////////////////////

void CollectionsBrushUndo::AddData(CollectionElement *el)
{
	CollectionElementSerializableData savedEl = *el;
	collectionElements.PushBack(savedEl);
}

//////////////////////////////////////////////////////////////////////////

void CollectionsBrushUndo::Register()
{
	UndoAction_t action;
	action.nActionID = ms_eActionID;
	action.pCreateUndoItem = CreateUndoItem;

	g_pUndoHistory->RegisterUndoAction(action);
}

//-------------------------------------------------------------------------
//	CollectionsTypeAddDelUndo
//-------------------------------------------------------------------------

CollectionsTypeAddDelUndo::CollectionsTypeAddDelUndo()
: collectionTypeIdx(INVALID_COLLECTION_INDEX)
{
	SetTitle("Collection type add/del");
}

//////////////////////////////////////////////////////////////////////////

IUndoItem * CollectionsTypeAddDelUndo::CreateUndoItem()
{
	return game_new CollectionsTypeAddDelUndo;
}

//////////////////////////////////////////////////////////////////////////

void CollectionsTypeAddDelUndo::Release()
{
	delete this;
}

//////////////////////////////////////////////////////////////////////////

UndoAction_e CollectionsTypeAddDelUndo::GetActionID()
{
	return ms_eActionID;
}

//////////////////////////////////////////////////////////////////////////

void CollectionsTypeAddDelUndo::UndoAddType()
{
	CollectionType *ct = gCollectionsManager.GetType(collectionTypeIdx);
	SetRemoveTypeInfo(collectionTypeIdx);

	gCollectionsManager.DeleteType(collectionTypeIdx);
	collectionTypeIdx = INVALID_COLLECTION_INDEX;
}

//////////////////////////////////////////////////////////////////////////

void CollectionsTypeAddDelUndo::UndoRemoveType()
{
	int idx = gCollectionsManager.CreateNewType();
	CollectionType *t = gCollectionsManager.GetType(idx);

	*t = collectionType.ct;
	t->InitMeshes(collectionType.meshFilePath.c_str());
	collectionTypeIdx = idx;

	//	Restore all elements
	for (uint32_t i = 0; i < storedElements.Count(); ++i)
	{
		CollectionElementSerializableData &ce = storedElements[i];
		ce.typeIndex = idx;

		int newIdx = gCollectionsManager.CreateNewElement();
		CollectionElement *el = gCollectionsManager.GetElement(newIdx);
		*static_cast<CollectionElementSerializableData*>(el) = ce;

		el->InitPhysicsData();

		gCollectionsManager.UpdateElementQuadTreePlacement(newIdx);
	}
	storedElements.Clear();
}

//////////////////////////////////////////////////////////////////////////

void CollectionsTypeAddDelUndo::Undo()
{
	collectionTypeIdx == INVALID_COLLECTION_INDEX ? UndoRemoveType() : UndoAddType();
}

//////////////////////////////////////////////////////////////////////////

void CollectionsTypeAddDelUndo::Redo()
{
	Undo();
}

//////////////////////////////////////////////////////////////////////////

void CollectionsTypeAddDelUndo::SetAddTypeInfo(int collectionTypeID)
{
	collectionTypeIdx = collectionTypeID;
}

//////////////////////////////////////////////////////////////////////////

void CollectionsTypeAddDelUndo::SetRemoveTypeInfo(int collectionTypeID)
{
	CollectionType *el = gCollectionsManager.GetType(collectionTypeID);
	if (!el)
		return;

	//	Memorize all elements of this type
	for (uint32_t i = 0; i < gCollectionsManager.GetElementsCount(); ++i)
	{
		CollectionElement *ce = gCollectionsManager.GetElement(i);
		if (ce && ce->typeIndex == collectionTypeID)
		{
			storedElements.PushBack(*ce);
		}
	}

	collectionType.ct = *el;
	r3dMesh *m = collectionType.ct.meshLOD[0];
	if (m)
	{
		collectionType.meshFilePath = m->FileName;
	}
	
	ZeroMemory(collectionType.ct.meshLOD, sizeof(collectionType.ct.meshLOD[0]) * _countof(collectionType.ct.meshLOD));
}

//////////////////////////////////////////////////////////////////////////

void CollectionsTypeAddDelUndo::Register()
{
	UndoAction_t action;
	action.nActionID = ms_eActionID;
	action.pCreateUndoItem = CreateUndoItem;

	g_pUndoHistory->RegisterUndoAction(action);
}

#endif