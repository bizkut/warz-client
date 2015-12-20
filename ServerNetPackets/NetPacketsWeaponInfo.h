#pragma once

#pragma pack(push)
#pragma pack(1)

// MAKE SURE to increase this after chanding following structs
#define GBWEAPINFO_VERSION 0x00000002

struct GBWeaponInfo
{
	float m_AmmoMass;
	float m_AmmoSpeed;
	float m_AmmoDamage;
	float m_AmmoDecay;
	float m_AmmoArea;
	float m_AmmoDelay;
	float m_AmmoTimeout;

	float m_reloadTime;
	float m_reloadActiveTick;
	float m_fireDelay;
	float m_spread; 
	float m_recoil;

	float m_DurabilityUse;
	float m_RepairAmount;
	float m_PremRepairAmount;
	int   m_RepairPriceGD;
};

struct GBGearInfo
{
	float m_Weight;
	float m_damagePerc;
	float m_damageMax;
	float m_bulkiness;
	float m_inaccuracy;
	float m_stealth;

	float m_RepairAmount;
	float m_PremRepairAmount;
	int   m_RepairPriceGD;
};

struct GBAttmInfo
{
	float m_DurabilityUse;
	float m_RepairAmount;
	float m_PremRepairAmount;
	int   m_RepairPriceGD;
};

#pragma pack(pop)
