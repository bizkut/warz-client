#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "Gear.h"

int g_GearBalance ;

Gear::Gear(const GearConfig* conf) : m_pConfig(conf)
{
	g_GearBalance ++ ;

	m_pConfig->aquireMesh() ;
}

Gear::~Gear()
{
	g_GearBalance -- ;

	m_pConfig->releaseMesh() ;
}


