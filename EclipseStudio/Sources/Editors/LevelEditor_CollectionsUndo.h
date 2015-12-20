//=========================================================================
//	Module: LevelEditor_CollectionsUndo.h
//	Copyright (C) 2013.
//=========================================================================

#pragma once

#ifndef FINAL_BUILD

//////////////////////////////////////////////////////////////////////////

#include "../../../Eternity/UndoHistory/UndoHistory.h"
#include "CollectionElement.h"
#include "CollectionType.h"

//-------------------------------------------------------------------------
//	Undo/Redo actions
//-------------------------------------------------------------------------

class CollectionsBrushUndo: public IUndoItem
{
private:
	static const UndoAction_e ms_eActionID = UA_COLLECTION_PAINT;
	r3dTL::TArray<int> collectionElementIDs;
	r3dTL::TArray<CollectionElementSerializableData> collectionElements;

	void UndoAddElements();
	void UndoRemoveElements();

public:
	void Release();
	UndoAction_e GetActionID();

	void Undo();
	void Redo();

	void AddData(int collectionElementID);
	void AddData(CollectionElement *el);

	CollectionsBrushUndo();

	static IUndoItem * CreateUndoItem();
	static void Register();
};

//////////////////////////////////////////////////////////////////////////

class CollectionsTypeAddDelUndo: public IUndoItem
{
private:
	static const UndoAction_e ms_eActionID = UA_COLLECTION_TYPE_MANIPULATION;
	int collectionTypeIdx;
	r3dTL::TArray<CollectionElementSerializableData> storedElements;

	struct SavedCollectionTypeData
	{
		CollectionType ct;
		r3dString meshFilePath;
	};

	SavedCollectionTypeData collectionType;
	void UndoAddType();
	void UndoRemoveType();

public:
	void Release();
	UndoAction_e GetActionID();

	void Undo();
	void Redo();

	void SetAddTypeInfo(int collectionTypeID);
	void SetRemoveTypeInfo(int collectionTypeID);

	CollectionsTypeAddDelUndo();

	static IUndoItem * CreateUndoItem();
	static void Register();
};


#endif