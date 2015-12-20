//=========================================================================
//	Module: obj_Prefab.cpp
//	Copyright (C) 2013.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"
#include "obj_Prefab.h"
#include "PrefabManager.h"
#include "../../../GameEngine/gameobjects/ObjManag.h"
#include "../../Editors/ObjectManipulator3d.h"
#include "UI/UIimEdit.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(obj_Prefab, "obj_Prefab", "Object");
AUTOREGISTER_CLASS(obj_Prefab);

//////////////////////////////////////////////////////////////////////////

#ifdef WO_SERVER
  // if on server, remove all unneeded stuff from prefabs
  #define FINAL_BUILD
#endif

#ifndef FINAL_BUILD
extern ObjectManipulator3d g_Manipulator3d;
#endif

//////////////////////////////////////////////////////////////////////////

obj_Prefab::obj_Prefab()
: isSelected(false)
, isOpened(false)
, recalcChildPoses(false)
, drawDistance(0.0f)
{
	ObjTypeFlags |= OBJTYPE_Prefab;
}

//////////////////////////////////////////////////////////////////////////

obj_Prefab::~obj_Prefab()
{

}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::DeleteObjects()
{
	for (uint32_t i = 0; i < gameObjects.Count(); ++i)
	{
		GameObject *g = gameObjects[i];
		if (g)
		{
			g->ownerPrefab = 0;
			GameWorld().DeleteObject(g);
		}
	}
	gameObjects.Clear();
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);
	pugi::xml_node p = node.child("prefab");
	prefabName = p.attribute("name").value();
	drawDistance = p.attribute("draw_distance").as_float();

	pugi::xml_node xmlDesc = g_PrefabManager.GetPrefabDescription(prefabName);
	Instantiate(prefabName, xmlDesc);

	//	Update position
	SetPosition(GetPosition());
	SetRotationVector(GetRotationVector());
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);
	pugi::xml_node p = node.append_child();
	p.set_name("prefab");
	p.append_attribute("name") = prefabName.c_str();
	p.append_attribute("draw_distance") = drawDistance;
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::Instantiate(const r3dSTLString &name, const pugi::xml_node &description)
{
	DeleteObjects();

	prefabName = name;
	pugi::xml_node n = description;

	void LoadLevelObjectsGroups(pugi::xml_node &, GameObjects &);
	LoadLevelObjectsGroups(n, gameObjects);

	localPoses.Resize(gameObjects.Count());

	for (uint32_t i = 0; i < gameObjects.Count(); ++i)
	{
		GameObject *g = gameObjects[i];
		if (!g)
			continue;

		g->RegenerateHash();

		g->m_isSerializable = false;

		//	Special handling of particles, because particle system always return identity matrix
		//	by GetTransformMatrix()
		if (g->ObjTypeFlags & OBJTYPE_Particle)
		{
			D3DXMATRIX rot, pos;
			const r3dVector &r = g->GetRotationVector();
			const r3dVector &p = g->GetPosition();
			D3DXMatrixRotationYawPitchRoll(&rot, r.x, r.y, r.z);
			D3DXMatrixTranslation(&pos, p.x, p.y, p.z);
			D3DXMatrixMultiply(&localPoses[i], &pos, &rot);
		}
		else
		{
			localPoses[i] = g->GetTransformMatrix();
		}

		g->ownerPrefab = this;
	}

	//	Refresh position and rotation
	SetPosition(GetPosition());
	SetRotationVector(GetRotationVector());

	UpdateChildPoses(GetTransformMatrix());
	
// 	r3dBoundBox bb = CalcBoundingBox();
// 	r3dBoundBox bbLocal = bb;
// 	bbLocal.Org -= GetPosition();
// 	SetBBoxLocalAndWorld(bbLocal, bb);
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::CreateXMLDescription(pugi::xml_node &rootNode)
{
	::CreateXMLDescription(rootNode, gameObjects, this);
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::SetPosition(const r3dPoint3D& pos)
{
	GameObject::SetPosition(pos);
	
	if (isOpened)
		return;

	recalcChildPoses = true;
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::SetRotationVector(const r3dVector& Angles)
{
	GameObject::SetRotationVector(Angles);
	
	if (isOpened)
		return;

	recalcChildPoses = true;
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::SetPreviewMode(bool isPreview)
{
	m_isSerializable = !isPreview;

	for (uint32_t i = 0; i < gameObjects.Count(); ++i)
	{
		if (!gameObjects[i])
			continue;

		if (isPreview)
		{
			gameObjects[i]->SetObjFlags(OBJFLAG_SkipCastRay);
		}
		else
		{
			gameObjects[i]->ResetObjFlags(OBJFLAG_SkipCastRay);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::SelectPrefab(bool ignoreOpenState, bool forceSelectDirectParent)
{
#ifndef FINAL_BUILD
	if (isOpened && !ignoreOpenState)
	{
		g_Manipulator3d.PickerRemoveFromPicked(this);
		return;
	}

	if (!g_Manipulator3d.IsObjectPicked(this))
	{
		g_Manipulator3d.PickerAddToPicked(this, isOpened);
	}

	isSelected = true;

	for (uint32_t i = 0; i < gameObjects.Count(); ++i)
	{
		GameObject *g = gameObjects[i];
		if (!g)
			continue;

		g_Manipulator3d.PickerRemoveFromPicked(g);
	}

	//	Handle selection of nested prefabs
	if (ownerPrefab && !forceSelectDirectParent)
	{
		ownerPrefab->SelectPrefab(ignoreOpenState);
	}

#endif
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::DrawBoundsWhenOpened()
{
#ifndef FINAL_BUILD
	r3dBoundBox bb = CalcBoundingBox();
	r3dVector c[8];
	bb.GetCorners(c);

	PushDebugBox(c[0], c[3], c[1], c[2], c[4], c[7], c[5], c[6], r3dColor(0, 128, 255));
#endif
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::OnPickerDrop()
{
	isSelected = false;
}

//////////////////////////////////////////////////////////////////////////

r3dBoundBox obj_Prefab::CalcBoundingBox()
{
	//	Update bounding box
	r3dBoundBox bb;
	bb.InitForExpansion();

	for (uint32_t i = 0; i < gameObjects.Count(); ++i)
	{
		GameObject *g = gameObjects[i];
		if (!g)
			continue;

		r3dBoundBox objBB;
		if (g->ObjTypeFlags & OBJTYPE_Prefab)
		{
			obj_Prefab *p = static_cast<obj_Prefab*>(g);
			objBB = p->CalcBoundingBox();
		}
		else
		{
			objBB = g->GetBBoxWorld();
		}
		bb.ExpandTo(objBB);
	}
	return bb;
}


//////////////////////////////////////////////////////////////////////////

BOOL obj_Prefab::Update()
{
#ifndef FINAL_BUILD
	if (isSelected)
	{
		r3dBoundBox bb = CalcBoundingBox();
		SetBBoxLocalAndWorld(bb, bb);
	}

	if (isOpened)
	{
		DrawBoundsWhenOpened();
	}
#endif

	if (recalcChildPoses)
	{
		UpdateChildPoses(GetTransformMatrix());
		recalcChildPoses = false;
	}

	return GameObject::Update();
}

//////////////////////////////////////////////////////////////////////////

#ifndef FINAL_BUILD
void obj_Prefab::BreakSelf()
{
	g_Manipulator3d.PickerRemoveFromPicked(this);

	for (uint32_t i = 0; i < gameObjects.Count(); ++i)
	{
		GameObject *g = gameObjects[i];
		if (!g)
			continue;

		g->ownerPrefab = 0;
		g->m_isSerializable = true;
		g_Manipulator3d.PickerAddToPicked(g);
	}
	gameObjects.Clear();
	GameWorld().DeleteObject(this);
}

//////////////////////////////////////////////////////////////////////////

float obj_Prefab::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float StartY = scry;
	StartY += GameObject::DrawPropertyEditor(scrx, scry, scrw, scrh, startClass, selected);

	if (imgui_Button(scrx, StartY, 360.0f, 22.0f, "Break prefab"))
	{
		BreakSelf();
	}

	StartY += 23.0f;

	GameObjects gs;
	g_Manipulator3d.GetPickedObjects(&gs);
	
	//	Another object is selected along with prefab. Provide a merge option
	if (gs.Count() > 1)
	{
		if (imgui_Button(scrx, StartY, 360.0f, 22.0f, "Merge prefab"))
		{
			GameObjects newGameObjects = gameObjects;
			GameObjects toDel;
			for (uint32_t i = 0; i < gs.Count(); ++i)
			{
				GameObject *go = gs[i];
				if (go == this)
					continue;

				//	Check for recursive self inclusion
				if (go->isObjType(OBJTYPE_Prefab))
				{
					obj_Prefab *p = static_cast<obj_Prefab*>(go);
					if (p->prefabName == prefabName)
					{
						MessageBox(r3dRenderer->HLibWin, "Recursive inclusion of same prefab instances is not allowed!", "Error", MB_ICONERROR | MB_OK);
						return StartY;
					}
				}

				newGameObjects.PushBack(go);
				toDel.PushBack(go);
			}
			g_PrefabManager.CreatePrefab(newGameObjects, prefabName, this);
			g_Manipulator3d.PickerResetPicked();
			g_Manipulator3d.PickerAddToPicked(this);
			SelectPrefab();

			for (uint32_t i = 0; i < toDel.Count(); ++i)
			{
				GameWorld().DeleteObject(toDel[i]);
			}
		}
		StartY += 23.0f;
	}

	//	Open/close prefab functionality similar to 3ds groups
	if (imgui_Button(scrx, StartY, 360.0f, 22.0f, isOpened ? "Close prefab" : "Open Prefab"))
	{
		isOpened ? CloseAfterModification() : OpenForModification();		
	}
	StartY += 23.0f;

	StartY += imgui_Value_Slider(scrx, StartY, "Draw Distance", &drawDistance, 0.0f, 4096.0f, "%0.0f");
	if (drawDistance > 0)
	{
		for (uint32_t i = 0; i < gameObjects.Count(); ++i)
		{
			GameObject *g = gameObjects[i];
			g->DrawDistanceSq = drawDistance * drawDistance;
		}
	}


	return StartY - scry;
}
#endif

//////////////////////////////////////////////////////////////////////////

BOOL obj_Prefab::OnDestroy()
{
	DeleteObjects();
	return GameObject::OnDestroy();
}

//////////////////////////////////////////////////////////////////////////

GameObject * obj_Prefab::Clone()
{
	obj_Prefab * pNew = static_cast<obj_Prefab*>(srv_CreateGameObject("obj_Prefab", FileName.c_str(), GetPosition()));

	pNew->Instantiate(prefabName, g_PrefabManager.GetPrefabDescription(prefabName));
	pNew->isSelected = true;
	pNew->SetRotationVector(GetRotationVector());

	PostCloneParamsCopy(pNew);

	return pNew;
}

//////////////////////////////////////////////////////////////////////////

uint32_t obj_Prefab::GetNumObjects() const
{
	return gameObjects.Count();
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::GetGameObjects(GameObjects &outObjs) const
{
	outObjs = gameObjects;
}

//////////////////////////////////////////////////////////////////////////

const r3dVector & obj_Prefab::GetObjectWorldPos(uint32_t idx) const
{
	if (gameObjects.Count() <= idx || gameObjects[idx] == 0)
	{
		return GetPosition();
	}

	return gameObjects[idx]->GetPosition();
}

//////////////////////////////////////////////////////////////////////////

r3dVector obj_Prefab::GetObjectLocalPos(uint32_t idx) const
{
	r3dVector rv(0, 0, 0);
	if (gameObjects.Count() <= idx || gameObjects[idx] == 0)
	{
		return rv;
	}

	rv.Assign(localPoses[idx]._41, localPoses[idx]._42, localPoses[idx]._43);
	return rv;
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::SetObjFlags(int objFlags)
{
	GameObject::SetObjFlags(objFlags);

	for (uint32_t i = 0; i < gameObjects.Count(); ++i)
	{
		GameObject *g = gameObjects[i];
		if (!g)
			continue;

		g->SetObjFlags(objFlags);
	}
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::ResetObjFlags(int objFlags)
{
	GameObject::ResetObjFlags(objFlags);

	for (uint32_t i = 0; i < gameObjects.Count(); ++i)
	{
		GameObject *g = gameObjects[i];
		if (!g)
			continue;

		g->ResetObjFlags(objFlags);
	}
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::OpenForModification()
{
#ifndef FINAL_BUILD
	g_Manipulator3d.PickerRemoveFromPicked(this);
	isSelected = false;
	isOpened = true;
#endif
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::CloseAfterModification()
{
	isOpened = false;
	g_PrefabManager.CreatePrefab(gameObjects, prefabName, this);
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::NotifyObjectDestruction(GameObject *go)
{
	for (uint32_t i = 0; i < gameObjects.Count(); ++i)
	{
		GameObject *o = gameObjects[i];
		if (o == go)
		{
			gameObjects.Erase(i);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

bool obj_Prefab::IsOpenedForEdit() const
{
	return isOpened;
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::AddNewObject(GameObject *objToAdd)
{
	if (!isOpened)
		return;

	gameObjects.PushBack(objToAdd);
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::RemoveObject(GameObject *objToRemove)
{
	if (!isOpened)
		return;

	int idx = gameObjects.FindItemIndex(objToRemove);
	if (idx >= 0)
	{
		GameWorld().DeleteObject(objToRemove);
	}
}

//////////////////////////////////////////////////////////////////////////

void MatrixGetYawPitchRoll(const D3DXMATRIX &, float &, float &, float &);
void NormalizeYPR(r3dVector &);

void obj_Prefab::UpdateChildPoses(D3DXMATRIX localPos)
{
	for (uint32_t i = 0; i < gameObjects.Count(); ++i)
	{
		GameObject *o = gameObjects[i];
		if (o)
		{
			D3DXMATRIX objLocalPos = localPoses[i];
			D3DXMatrixMultiply(&objLocalPos, &objLocalPos, &localPos);

			bool isPrefab = (o->ObjTypeFlags & OBJTYPE_Prefab) != 0;
			if (isPrefab)
			{
				obj_Prefab *p = static_cast<obj_Prefab*>(o);
				p->UpdateChildPoses(objLocalPos);
			}

			o->SetTransformMatrix(objLocalPos);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

bool obj_Prefab::IsPartOfPrefab(GameObject *objToVerify) const
{
	for (uint32_t i = 0; i < gameObjects.Count(); ++i)
	{
		GameObject *g = gameObjects[i];
		if (!g)
			continue;

		if (g->isObjType(OBJTYPE_Prefab))
		{
			obj_Prefab *p = static_cast<obj_Prefab*>(g);
			bool rv = p->IsPartOfPrefab(objToVerify);
			if (rv)
				return rv;
		}
		else
		{
			if (g == objToVerify)
				return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

bool obj_Prefab::IsInternalObjectsValid() const
{
	for (uint32_t i = 0; i < gameObjects.Count(); ++i)
	{
		GameObject *g = gameObjects[i];
		if (!g)
		{
			return false;
		}
		else
		{
			r3dMesh *mesh = g->GetObjectMesh();
			if (mesh && (mesh->Flags & r3dMesh::obfMissingSource))
			{
				return false;
			}
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

void obj_Prefab::CleanInvalidObjects()
{
	struct MissingSourcePredicate
	{
		bool operator()(GameObject *g)
		{
			if (g)
			{
				r3dMesh *mesh = g->GetObjectMesh();
				return mesh && (mesh->Flags & r3dMesh::obfMissingSource);
			}
			return g == 0;
		}
	} pred;

	GameObject **b = &gameObjects[0];
	GameObject **e = b + gameObjects.Count();
	GameObject **firstInvalid = std::remove_if(b, e, pred);

	uint32_t newSize = std::distance(b, firstInvalid);
	GameObjects copy = gameObjects;
	copy.Resize(newSize);
	g_PrefabManager.CreatePrefab(copy, prefabName.c_str(), this);
}
