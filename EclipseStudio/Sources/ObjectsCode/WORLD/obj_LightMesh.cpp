//=========================================================================
//	Module: obj_LightMesh.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "Lamp.h"
#include "../../Editors/ObjectManipulator3d.h"
#include "obj_LightMesh.h"
#include "multiplayer/ClientGameLogic.h"
#include "XMLHelpers.h"

//////////////////////////////////////////////////////////////////////////

extern ObjectManipulator3d g_Manipulator3d;
void SaveLevelObject(pugi::xml_node &curNode, GameObject *obj);
GameObject * LoadLevelObject(pugi::xml_node &curNode);

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(obj_LightMesh, "obj_LightMesh", "Object");
AUTOREGISTER_CLASS(obj_LightMesh);

obj_LightMesh::obj_LightMesh()
: destroyMeshOnKillLight(false)
#ifndef FINAL_BUILD
, selectedLight(0)
#endif
, killedLightObjColor(r3dColor::black)
{
}

//////////////////////////////////////////////////////////////////////////

obj_LightMesh::~obj_LightMesh()
{
	DestroyLights();
}

//////////////////////////////////////////////////////////////////////////

void obj_LightMesh::DestroyLights()
{
	for (uint32_t i = 0; i < lightIDs.Count(); ++i)
	{
		DeleteLight(i);
	}
}

//////////////////////////////////////////////////////////////////////////

void obj_LightMesh::ReadSerializedData(pugi::xml_node& node)
{
	obj_Building::ReadSerializedData(node);

	DestroyLights();

	pugi::xml_node lightNode = node.child("object");

	if( lightNode.empty() )
	{
#ifndef FINAL_BUILD
		extern bool g_bEditMode ;
		if( g_bEditMode )
		{
			char buf[ 512 ] ;
			sprintf( buf, "Inconsistent light mesh object '%s'! ( mesh is here but light is not)", this->Name.c_str() );
			MessageBox( 0, buf, "Error", MB_OK );
		}
#endif
	}

	while (!lightNode.empty())
	{
		GameObject *o = 0;
		o = LoadLevelObject(lightNode);
		if (o)
		{
			gobjid_t l;
			l = o->ID;
			o->m_isSerializable = false;
			o->ownerID = ID;

			lightIDs.PushBack(l);
		}
		lightNode = lightNode.next_sibling("object");
	}

	pugi::xml_node customAttribs = node.child("custom_properties");
	if (!customAttribs.empty())
	{
		destroyMeshOnKillLight = customAttribs.attribute("destroy_builiding_with_light").as_bool();
		GetXMLVal("destroyed_light_obj_color", node, &killedLightObjColor);
	}
}

//////////////////////////////////////////////////////////////////////////

void obj_LightMesh::WriteSerializedData(pugi::xml_node& node)
{
#ifndef FINAL_BUILD
	obj_Building::WriteSerializedData(node);

	pugi::xml_node xmlCustomAttribs = node.append_child();
	xmlCustomAttribs.set_name("custom_properties");
	xmlCustomAttribs.append_attribute("destroy_builiding_with_light").set_value(destroyMeshOnKillLight);
	SetXMLVal("destroyed_light_obj_color", xmlCustomAttribs, &killedLightObjColor);

	for (uint32_t i = 0; i < lightIDs.Count(); ++i)
	{
		gobjid_t &lightID = lightIDs[i];
		GameObject *o = GameWorld().GetObject(lightID);

		obj_LightHelper* light = (obj_LightHelper*)o;
		if (light && !light->LT.IsOn())
			std::swap(killedLightObjColor, m_ObjectColor);

		if (o)
		{
			SaveLevelObject(node, o);
		}	

		if (light && !light->LT.IsOn())
			std::swap(killedLightObjColor, m_ObjectColor);
	}
#endif
}

//////////////////////////////////////////////////////////////////////////

void obj_LightMesh::SetPosition(const r3dPoint3D& pos)
{
	r3dPoint3D oldPos = GetPosition();
	obj_Building::SetPosition(pos);
	r3dPoint3D diff = GetPosition() - oldPos;

	for (uint32_t i = 0; i < lightIDs.Count(); ++i)
	{
		GameObject *o = GameWorld().GetObject(lightIDs[i]);

#ifndef FINAL_BUILD
		if(!g_Manipulator3d.IsSelected(o))
#endif
		{
			if (o)
			{
				r3dPoint3D lightPos = o->GetPosition();
				o->SetPosition(lightPos + diff);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void obj_LightMesh::SetObjFlags(int objFlags)
{
	GameObject::SetObjFlags(objFlags);

	if (objFlags | OBJFLAG_SkipCastRay)
	{
		for (uint32_t i = 0; i < lightIDs.Count(); ++i)
		{
			GameObject *o = GameWorld().GetObject(lightIDs[i]);
			if (o)
			{
				o->SetObjFlags(OBJFLAG_SkipCastRay);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void obj_LightMesh::DeleteLight(uint32_t idx)
{
	if (idx >= lightIDs.Count())
		return;

	gobjid_t lightID = lightIDs[idx];
	GameObject *o = GameWorld().GetObject(lightID);
	if (o)
	{
		GameWorld().DeleteObject(o);
	}
	lightIDs.Erase(idx);
}

//////////////////////////////////////////////////////////////////////////

#ifndef FINAL_BUILD
float obj_LightMesh::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float y = scry + parent::DrawPropertyEditor(scrx, scry, scrw, scrh, startClass, selected );

	y += 22.0f;
	if (imgui_Button(scrx, y, 360.0f, 22.0f, "Add light"))
	{
		CreateLight("Omni");
	}
	y += 22.0f;

	if( IsParentOrEqual( &ClassData, startClass ) && lightIDs.Count() > 0 )
	{
		if (imgui_Button(scrx, y, 360.0f, 22.0f, "Delete selected light"))
		{
			DeleteLight(selectedLight);
		}
		y += 22.0f;

		static stringlist_t lightsList;
		lightsList.clear();

		for (uint32_t i = 0; i < lightIDs.Count(); ++i)
		{
			char buf[0xf] = {0};
			lightsList.push_back(r3dSTLString("Light") + itoa(i, buf, 10));
		}

		float arrOffset = 0;
		float listHeight = 100.0f;
		imgui_DrawList(scrx, y, 360.0f, listHeight, lightsList, &arrOffset, &selectedLight);
		y += listHeight;

		GameObject *o = GameWorld().GetObject(lightIDs[selectedLight]);
		if (!o) return y;

		obj_LightHelper *lh = static_cast<obj_LightHelper*>(o);

		if (imgui_Button(scrx, y, 360, 20, "Select attached light"))
		{
			g_Manipulator3d.PickerResetPicked();
			g_Manipulator3d.PickerAddToPicked(o);
		}
		y += 22.0f;

		if (imgui_Button(scrx, y, 180, 20, "Omni", lh->LT.Type == R3D_OMNI_LIGHT))
		{
			lh->LT.Type = R3D_OMNI_LIGHT;
			lh->FileName = "Omni";
		}

		if (imgui_Button(scrx + 180, y, 180, 20, "Spot", lh->LT.Type == R3D_SPOT_LIGHT))
		{
			lh->LT.Type = R3D_SPOT_LIGHT;
			lh->FileName = "Spot";
		}

		y += 22.0f;

		if (selectedLight == 0)
		{
			if( ((obj_LightHelper*)o)->LT.IsOn() )
			{
				if (imgui_Button(scrx, y, 360, 20, "Destroy light"))
				{
					KillLight() ;
				}
			}
			else
			{
				if (imgui_Button(scrx, y, 360, 20, "Resurrect light"))
				{
					ResurrectLight() ;
				}
			}
			y += 22;
		}

		int v = destroyMeshOnKillLight ? 1 : 0;
		y += imgui_Checkbox(scrx, y, "Destroy object with light", &v, 1);
		destroyMeshOnKillLight = v != 0;

		return y - scry ;
	}
	else
	{
		return 0.f ;
	}
	
}
#endif

//////////////////////////////////////////////////////////////////////////

BOOL obj_LightMesh::OnCreate()
{
	parent::OnCreate();

#ifndef FINAL_BUILD
	extern bool g_bEditMode;
	if(!g_bEditMode)
#endif
	{
		SetNetworkID(gClientLogic().net_lastFreeId++);
		NetworkLocal = false;
	}

	if (lightIDs.Count() > 0 && GameWorld().GetObject(lightIDs[0]))
		return TRUE;

	GameObject *o = CreateLight("Omni");

	return !!o;
}

//////////////////////////////////////////////////////////////////////////

GameObject * obj_LightMesh::CreateLight(const char * fn)
{
	GameObject *o = srv_CreateGameObject("obj_LightHelper", fn, GetPosition());
	if (o)
	{
		gobjid_t l = o->ID;
		o->m_isSerializable = false;
		o->ownerID = ID;
		lightIDs.PushBack(l);
	}
	return o;
}

//------------------------------------------------------------------------

bool obj_LightMesh::isLightOn()
{
	if (lightIDs.Count() == 0)
		return false;

	GameObject *o = GameWorld().GetObject(lightIDs[0]);
	if (o)
	{
		obj_LightHelper* light = (obj_LightHelper*)o;
		return !!light->LT.IsOn();
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

void obj_LightMesh::KillLight()
{
	if (lightIDs.Count() == 0)
		return;

	GameObject *o = GameWorld().GetObject(lightIDs[0]);
	if (o)
	{
		obj_LightHelper* light = (obj_LightHelper*)o;
		light->LT.TurnOff();
		std::swap(m_ObjectColor, killedLightObjColor);
		
		if (destroyMeshOnKillLight)
		{
			this->DestroyBuilding();
		}
	}
}

//------------------------------------------------------------------------

void obj_LightMesh::ResurrectLight()
{
	if (lightIDs.Count() == 0)
		return;

	GameObject *o = GameWorld().GetObject(lightIDs[0]);
	if (o)
	{
		obj_LightHelper* light = (obj_LightHelper*)o;
		light->LT.TurnOn();
		std::swap(m_ObjectColor, killedLightObjColor);

		if (destroyMeshOnKillLight)
		{
			this->FixBuilding();
		}
	}

}

//////////////////////////////////////////////////////////////////////////

GameObject * obj_LightMesh::Clone()
{
	obj_LightMesh *clone = static_cast<obj_LightMesh*>(srv_CreateGameObject("obj_LightMesh", FileName.c_str(), GetPosition()));
	if (!clone)
		return 0;

	PostCloneParamsCopy(clone);

	for (uint32_t i = 0; i < lightIDs.Count(); ++i)
	{
		GameObject *o = GameWorld().GetObject(lightIDs[i]);
		if (o)
		{
			GameObject *newLight = o->Clone();
			if (newLight)
			{
				gobjid_t newLtID = newLight->ID;
				newLight->ownerID = clone->ID;
				newLight->m_isSerializable = false;
				clone->lightIDs.PushBack(newLtID);
			}
		}
	}
	
	return clone;
}

BOOL obj_LightMesh::OnNetReceive(DWORD EventID, const void* packetData, int packetSize)
{
	switch(EventID)
	{
	case PKT_S2C_LightMeshStatus:
		{
			KillLight() ;
		}
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------------

int	obj_LightMesh::IsStatic()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

gobjid_t obj_LightMesh::GetLightObjectID(uint32_t idx) const
{
	if (idx >= lightIDs.Count())
		return invalidGameObjectID;

	return lightIDs[idx];
}