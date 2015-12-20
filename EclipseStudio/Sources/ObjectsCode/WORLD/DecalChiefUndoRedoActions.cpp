//=========================================================================
//	Module: DecalChiefUndoRedoActions.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "DecalChiefUndoRedoActions.h"
#include "DecalChief.h"
#include "../../../../GameEngine/gameobjects/GameObj.h"
#include "../../Editors/ObjectManipulator3d.h"

#ifndef FINAL_BUILD

//////////////////////////////////////////////////////////////////////////

extern ObjectManipulator3d g_Manipulator3d;

//////////////////////////////////////////////////////////////////////////

namespace
{
	void RemoveDecal(const DecalParams &decalParms)
	{
		if (decalParms.TypeID == -1)
			return;

		UINT n = g_pDecalChief->GetStaticDecalCount(decalParms.TypeID);

		if (n > 0)
		{
			r3dTL::TArray<GameObject*> o;
			g_Manipulator3d.GetPickedObjects(&o);
			for (uint32_t i = 0; i < o.Count(); ++i)
			{
				GameObject *obj = o[i];
				if (obj && obj->isObjType(OBJTYPE_DecalProxy))
				{
					g_Manipulator3d.PickerRemoveFromPicked(obj);
				}
			}
			g_pDecalChief->RemoveStaticDecal(decalParms.TypeID, n - 1);
		}
	}

//////////////////////////////////////////////////////////////////////////

	void AddDecal(const DecalParams &decalParms)
	{
		g_pDecalChief->Add(decalParms, false);
	}
} // unnamed namespace

//-------------------------------------------------------------------------
//	DecalUndoAdd
//-------------------------------------------------------------------------

DecalUndoAdd::DecalUndoAdd()
{
	m_sTitle = "Add static decal";
}

//////////////////////////////////////////////////////////////////////////

void DecalUndoAdd::Undo()
{
	RemoveDecal(decalParms);
}

//////////////////////////////////////////////////////////////////////////

void DecalUndoAdd::Redo()
{
	AddDecal(decalParms);
}

//-------------------------------------------------------------------------
//	DecalUndoDel
//-------------------------------------------------------------------------

DecalUndoDel::DecalUndoDel()
{
	m_sTitle = "Delete decal";
}

//////////////////////////////////////////////////////////////////////////

void DecalUndoDel::Undo()
{
	AddDecal(decalParms);
}

//////////////////////////////////////////////////////////////////////////

void DecalUndoDel::Redo()
{
	RemoveDecal(decalParms);
}

//////////////////////////////////////////////////////////////////////////

#endif // FINAL_BUILD