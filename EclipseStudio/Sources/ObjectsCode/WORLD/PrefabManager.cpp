//=========================================================================
//	Module: PrefabManager.cpp
//	Copyright (C) 2013.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"
#include "PrefabManager.h"
#include "../../../GameEngine/gameobjects/ObjManag.h"
#include <Shlwapi.h>

//////////////////////////////////////////////////////////////////////////

PrefabManager g_PrefabManager;

//////////////////////////////////////////////////////////////////////////

#ifdef WO_SERVER
  // if on server, remove all unneeded stuff from prefabs
  #define FINAL_BUILD
#endif

namespace
{
	const char * const PREFABS_PATH = "Data\\Prefabs\\";
}

//////////////////////////////////////////////////////////////////////////

PrefabManager::PrefabManager()
{

}

//////////////////////////////////////////////////////////////////////////

PrefabManager::~PrefabManager()
{

}

//////////////////////////////////////////////////////////////////////////

bool PrefabManager::LoadPrefab(const r3dSTLString &name)
{
	r3dSTLString fullPrefabPath = PREFABS_PATH + name + ".xml";
	r3dFile* f = r3d_open(fullPrefabPath.c_str());
	if (!f)
	{
		r3dArtBug("PrefabManager: referenced prefab file was not found: %s\n", fullPrefabPath.c_str());
		return false;
	}

	r3dTL::TArray<char> fileBuffer(f->size + 1);

	fread(&fileBuffer[0], f->size, 1, f);
	fileBuffer[f->size] = 0;

	pugi::xml_document *prefabDoc = new pugi::xml_document;
	pugi::xml_parse_result parseResult = prefabDoc->load_buffer(&fileBuffer[0], f->size);
	fclose(f);

	if (!parseResult)
	{
		r3dArtBug("PrefabManager: Failed to parse %s, error: %s", fullPrefabPath.c_str(), parseResult.description());
		delete prefabDoc;
		return false;
	}

	pugi::xml_node n = prefabDoc->child("prefab");
	if (!n.empty())
	{
		prefabs[name] = prefabDoc;
		return true;
	}
	delete prefabDoc;
	return false;
}

//////////////////////////////////////////////////////////////////////////

void PrefabManager::SavePrefabs()
{
	r3dSTLString pattern = r3dSTLString(PREFABS_PATH) + "*.xml";
	r3d_remove_files_in_folder(pattern.c_str());

	for (Prefabs::const_iterator it = prefabs.begin(); it != prefabs.end(); ++it)
	{
		r3dSTLString fullPath = PREFABS_PATH + it->first + ".xml";
		it->second->save_file(fullPath.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////

void PrefabManager::CreatePrefab(const GameObjects &gs, const r3dSTLString &name, const obj_Prefab *rootObj)
{
	Prefabs::iterator it = prefabs.find(name);
	bool renew = it != prefabs.end();
	if (renew)
	{
		delete it->second;
		prefabs.erase(it);
	}

	pugi::xml_document *d = new pugi::xml_document;
	pugi::xml_node p = d->append_child();
	p.set_name("prefab");
	prefabs[name] = d;

	CreateXMLDescription(p, gs, rootObj);

	if (renew)
		RefreshPrefabObjects(name, p);
}

//////////////////////////////////////////////////////////////////////////

void PrefabManager::DeletePrefab(const r3dSTLString &name)
{
	Prefabs::iterator it = prefabs.find(name);
	if (it == prefabs.end())
		return;

	GameObject* obj = GameWorld().GetFirstObject();
	while (obj)
	{
		GameObject *nextObj = GameWorld().GetNextObject(obj);
		if (obj->isObjType(OBJTYPE_Prefab))
		{
			obj_Prefab *p = static_cast<obj_Prefab*>(obj);
			if (p->prefabName == name)
				GameWorld().DeleteObject(obj);
		}
		obj = nextObj;
	}

	delete it->second;
	prefabs.erase(it);
}

//////////////////////////////////////////////////////////////////////////

void PrefabManager::FillNameList(stringlist_t &outNames)
{
	for (Prefabs::const_iterator it = prefabs.begin(); it != prefabs.end(); ++it)
	{
		outNames.push_back(it->first);
	}
}

//////////////////////////////////////////////////////////////////////////

pugi::xml_node PrefabManager::GetPrefabDescription(const r3dSTLString &name)
{
	Prefabs::const_iterator it = prefabs.find(name);
	pugi::xml_node emptyNode;
	if (it == prefabs.end())
	{
		if (!LoadPrefab(name))
		{
			return emptyNode;
		}
		it = prefabs.find(name);
	}
	if (it != prefabs.end())
	{
		pugi::xml_document *d = it->second;
		pugi::xml_node n = d->child("prefab");
		return n;
	}
	return emptyNode;
}

//////////////////////////////////////////////////////////////////////////

void PrefabManager::RefreshPrefabObjects(const r3dSTLString &name, const pugi::xml_node &n)
{
	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		GameObject* obj = iter.current;
		if (obj->isObjType(OBJTYPE_Prefab))
		{
			obj_Prefab *p = static_cast<obj_Prefab*>(obj);
			if (p->prefabName == name)
			{
				const r3dVector prevPos = p->GetPosition();
				p->Instantiate(name, n);
				//	Update position
				p->Update();
				const r3dVector nextPos = p->GetObjectWorldPos(0);

				r3dVector diff = prevPos - nextPos;
				p->SetPosition(prevPos);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

bool PrefabManager::LoadAllPrefabs()
{
	// Enumerate all files in prefabs folder
	bool rv = true;

#ifndef FINAL_BUILD
	DeleteAllPrefabs();
	WIN32_FIND_DATA fd;
	r3dSTLString s = r3dSTLString(PREFABS_PATH) + "*.xml";
	HANDLE h = FindFirstFile(s.c_str(), &fd);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			char* fn = PathFindFileName(fd.cFileName);
			char* ext = PathFindExtension(fn);
			ext[0] = 0;
			rv &= LoadPrefab(fn);
		}
		while(FindNextFile(h, &fd) != 0);
		FindClose(h);
	}

#endif
	return rv;
}

//////////////////////////////////////////////////////////////////////////

void PrefabManager::DeleteAllPrefabs()
{
	for (Prefabs::iterator it = prefabs.begin(); it != prefabs.end(); ++it)
	{
		delete it->second;
	}
	prefabs.clear();
}

//////////////////////////////////////////////////////////////////////////

obj_Prefab * PrefabManager::InstantiatePrefab(const r3dSTLString &name, const r3dVector &pos)
{
	obj_Prefab * p = static_cast<obj_Prefab*>(srv_CreateGameObject("obj_Prefab", name.c_str(), pos));
	p->Instantiate(name, GetPrefabDescription(name));
	return p;
}

//////////////////////////////////////////////////////////////////////////

void PrefabManager::ValidatePrefabs()
{
#ifndef FINAL_BUILD
	using namespace pugi;
	const char * const objName = "object";
	const char * const prefabName = "prefab";

	for (Prefabs::iterator it = prefabs.begin(), it_end = prefabs.end(); it != it_end; ++it)
	{
		xml_document *d = it->second;

		xml_node prefab = d->child(prefabName);
		xml_node obj = prefab.child(objName);
		for (; !obj.empty(); obj = obj.next_sibling(objName))
		{
			xml_node prefabNode = obj.child(prefabName);
			if (!prefabNode.empty())
			{
				const char * prefabName = prefabNode.attribute("name").value();
				if (prefabName)
				{
					r3dSTLString fullPrefabPath = r3dSTLString(PREFABS_PATH) + prefabName + ".xml";
					int a = r3d_access(fullPrefabPath.c_str(), 0);
					if (a != 0)
					{
						char buf[256] = {0};
						sprintf_s
						(
							buf,
							_countof(buf),
							"Prefab '%s' contains reference to inexistent prefab named '%s'. Would you like to remove it from '%s' definition?",
							it->first.c_str(),
							strlen(prefabName) > 0 ? prefabName : "[empty_name]",
							it->first.c_str()
						);

						if (MessageBoxA(r3dRenderer->HLibWin, buf, "WARNING", MB_YESNO | MB_ICONWARNING) == IDYES)
						{
							prefab.remove_child(obj);
						}
					}
				}
			}
		}
	}

	//	Validate existing prefab objects
	typedef r3dgameMap(r3dSTLString, bool) IncorrectPrefabsMap;
	IncorrectPrefabsMap incorrectPrefabsMap;
	typedef r3dgameVector(GameObject*) ToRemove;
	ToRemove toRemove;
	
	for (ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current; iter = GameWorld().GetNextOfAllObjects(iter))
	{
		GameObject *o = iter.current;
		if (o->ObjTypeFlags & OBJTYPE_Prefab)
		{
			obj_Prefab *p = static_cast<obj_Prefab*>(o);
			
			IncorrectPrefabsMap::iterator itr = incorrectPrefabsMap.find(p->prefabName);
			if (itr == incorrectPrefabsMap.end())
			{
				r3dSTLString fullPrefabPath = r3dSTLString(PREFABS_PATH) + p->prefabName + ".xml";
				int a = r3d_access(fullPrefabPath.c_str(), 0);
				if (a != 0)
				{
					char buf[256] = {0};
					sprintf_s
					(
						buf,
						_countof(buf),
						"Prefab '%s' referenced on level, but its definition file '%s' is missing! Would you like to remove all instances of this prefab from scene?",
						p->prefabName.c_str(),
						fullPrefabPath.c_str()
					);

					bool removeInstances = MessageBoxA(r3dRenderer->HLibWin, buf, "WARNING", MB_YESNO | MB_ICONWARNING) == IDYES;
					IncorrectPrefabsMap::_Pairib rv = incorrectPrefabsMap.insert(IncorrectPrefabsMap::value_type(p->prefabName, removeInstances));
					itr = rv.first;
				}
			}
			
			if (itr != incorrectPrefabsMap.end() && itr->second)
			{
				toRemove.push_back(o);
			}
		}
	}

	for (ToRemove::iterator it = toRemove.begin(), it_end = toRemove.end(); it != it_end; ++it)
	{
		GameWorld().DeleteObject(*it);
	}
#endif
}

//////////////////////////////////////////////////////////////////////////

void PrefabManager::BreakAllPrefabs()
{
#ifndef FINAL_BUILD
	for (ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current; iter = GameWorld().GetNextOfAllObjects(iter))
	{
		GameObject *o = iter.current;
		if (o->ObjTypeFlags & OBJTYPE_Prefab)
		{
			obj_Prefab *p = static_cast<obj_Prefab*>(o);
			p->BreakSelf();
		}
	}
#endif
}

//-------------------------------------------------------------------------
//	Standalone helper functions
//-------------------------------------------------------------------------

void CreateXMLDescription(pugi::xml_node &rootNode, const GameObjects &gameObjects, const obj_Prefab *rootObj)
{
#ifndef FINAL_BUILD
	D3DXMATRIX rootTransform;
	if (rootObj)
	{
		rootTransform = rootObj->GetTransformMatrix();
	}
	else
	{
		r3dVector *origPositions = new r3dVector[gameObjects.Count()];
		r3dVector center(0, 0, 0);
		for (uint32_t i = 0; i < gameObjects.Count(); ++i)
		{
			center += gameObjects[i]->GetPosition();;
		}
		center /= static_cast<float>(gameObjects.Count());
		D3DXMatrixTranslation(&rootTransform, center.x, center.y, center.z);
		delete [] origPositions;
	}

	D3DXMatrixInverse(&rootTransform, 0, &rootTransform);

	for (uint32_t i = 0; i < gameObjects.Count(); ++i)
	{
		GameObject *go = gameObjects[i];
		if (go)
		{
			D3DXMATRIX origTrans = go->GetTransformMatrix();
			D3DXMATRIX newPos;
			D3DXMatrixMultiply(&newPos, &origTrans, &rootTransform);

			go->SetTransformMatrix(newPos);
			void SaveLevelObject(pugi::xml_node &, GameObject *);
			SaveLevelObject(rootNode, go);
			go->SetTransformMatrix(origTrans);
		}
	}
#endif
}
