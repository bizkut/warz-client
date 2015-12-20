#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "GearConfig.h"

r3dMesh* GearConfig::getFirstPersonMesh() const
{
	if(m_FirstPersonModel == 0)
	{
#ifndef FINAL_BUILD
		extern bool g_bEditMode;
		if(g_bEditMode && !r3dFileExists(m_ModelPath_1st))
			return NULL;
#endif
		m_FirstPersonModel = r3dGOBAddMesh(m_ModelPath_1st, true, false, true, true );
		if(m_FirstPersonModel==NULL)
		{
			r3dError("ART: failed to load mesh '%s'\n", m_ModelPath_1st);
		}
		r3d_assert(m_FirstPersonModel);
	}

	return m_FirstPersonModel;
}

int GearConfig::getConfigMeshRefs() const
{
	return m_ModelRefCount ;
}

void GearConfig::aquireMesh() const
{
	if( !m_Model )
	{
		m_Model = r3dGOBAddMesh(m_ModelPath, true, false, true, true );

		if(m_Model==0)
		{
			r3dError("ART: failed to load mesh '%s'\n", m_ModelPath);
		}
		r3d_assert(m_Model);

		r3d_assert( !m_ModelRefCount ) ;

		m_ModelRefCount = 1 ;
	}
	else
	{
		r3d_assert( !r_allow_ingame_unloading->GetInt() || m_ModelRefCount ) ;
		m_ModelRefCount ++ ;
	}
}

void GearConfig::releaseMesh() const
{
	if( r_allow_ingame_unloading->GetInt() )
	{
		if( m_ModelRefCount == 1 )
		{
			r3d_assert( m_Model ) ;

			r3dGOBReleaseMesh( m_Model ) ;

			m_Model = 0 ;
		}
	}

	m_ModelRefCount -- ;
	r3d_assert( m_ModelRefCount >= 0 ) ;
}

bool GearConfig::loadBaseFromXml(pugi::xml_node& xmlGear)
{
	ModelItemConfig::loadBaseFromXml(xmlGear);

	{
		int len = strlen(m_ModelPath);
		m_ModelPath_1st = (char*)malloc(sizeof(char)*(len+32));
		r3dscpy(m_ModelPath_1st, m_ModelPath);
		r3dscpy(&m_ModelPath_1st[len-4], "_FPS.sco");
	}

	m_damagePerc = xmlGear.child("Armor").attribute("damagePerc").as_float()/100.0f; // from 30 -> 0.3
	m_damageMax = xmlGear.child("Armor").attribute("damageMax").as_float();
	m_bulkiness = xmlGear.child("Armor").attribute("bulkiness").as_float();
	m_inaccuracy = xmlGear.child("Armor").attribute("inaccuracy").as_float();
	m_stealth = xmlGear.child("Armor").attribute("stealth").as_float();

	m_RepairAmount = xmlGear.child("Dur").attribute("r1").as_float();
	m_PremRepairAmount = xmlGear.child("Dur").attribute("r2").as_float();
	m_RepairPriceGD = xmlGear.child("Dur").attribute("r3").as_int();

	return true;
}

bool BackpackConfig::loadBaseFromXml(pugi::xml_node& xmlItem)
{
	ModelItemConfig::loadBaseFromXml(xmlItem);

	m_maxSlots= xmlItem.child("Desc").attribute("maxSlots").as_int();
	m_maxWeight = xmlItem.child("Desc").attribute("maxWeight").as_float();

	return true;
}