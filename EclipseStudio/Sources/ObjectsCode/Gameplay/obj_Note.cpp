#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_Note.h"

#include "ObjectsCode/weapons/WeaponArmory.h"

IMPLEMENT_CLASS(obj_Note, "obj_Note", "Object");
AUTOREGISTER_CLASS(obj_Note);

obj_Note::obj_Note()
{
	m_GotData = false;
}

obj_Note::~obj_Note()
{
}

BOOL obj_Note::Load(const char *fname)
{
	const char* cpMeshName = "Data\\ObjectsDepot\\Weapons\\Item_MsgBottle.sco";
	if(!parent::Load(cpMeshName)) 
		return FALSE;

	return TRUE;
}

BOOL obj_Note::OnCreate()
{
	m_ActionUI_Title = gLangMngr.getString("Note");
	m_ActionUI_Msg = gLangMngr.getString("HoldEToReadNote");

	m_spawnPos = GetPosition();

	if(!DisablePhysX)
	{
		ReadPhysicsConfig();
		PhysicsConfig.group = PHYSCOLL_TINY_GEOMETRY; // skip collision with players
		PhysicsConfig.requireNoBounceMaterial = true;
		PhysicsConfig.isFastMoving = true;
	}

	return parent::OnCreate();
}

BOOL obj_Note::OnDestroy()
{
	return parent::OnDestroy();
}

BOOL obj_Note::Update()
{
	return parent::Update();
}
