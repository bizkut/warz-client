//=========================================================================
//	Module: AI_PlayerSenses.h
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

class obj_Zombie;
class obj_Player;

class PlayerLifeProps
{
	friend void ProcessZombieMod();

	//	Visibility block
	float standStillVisiblity;
	float walkVisibility;
	float runVisibility;
	float sprintVisibility;
	float swimIdleVisibility;
	float swimSlowVisibility;
	float swimVisibility;
	float swimFastVisibility;

	//	Player noise values
	float walkNoise;
	float runNoise;
	float sprintNoise;
	float fireNoise;
	float swimSlowNoise;
	float swimNoise;
	float swimFastNoise;

	//	Smell values
	float smell;

	obj_Player *owner;

	float GetPlayerRawSmell() const;
	float GetPlayerRawNoise() const;
	float GetPlayerRawVisibility() const;

	template <bool W>
	void SerializeAISettingsXML(pugi::xml_node &rootNode);

public:
	bool weaponFired;
	explicit PlayerLifeProps(obj_Player *o);
	bool DetectByZombie(const obj_Zombie &z, bool &hardLock);

	float getPlayerVisibility(); // return from 0 to 1
	float getPlayerHearRadius(); // return from 0 to 1

	/**	Visualize debug information in editor. */
#ifndef FINAL_BUILD
	void DebugVisualizeRanges();
//	void DebugDrawZombieSenses();
	bool SaveAISettingsXML();
	bool LoadAISettingsXML();
#endif;
};

//////////////////////////////////////////////////////////////////////////

template <bool W>
void PlayerLifeProps::SerializeAISettingsXML(pugi::xml_node &rootNode)
{
	pugi::xml_node ai = SerializeXMLNode<W>(rootNode, "player_ai");
	if (ai)
	{
		SerializeXMLVal<W>("stand_visibility", ai, &standStillVisiblity);
		SerializeXMLVal<W>("walk_visibility", ai, &walkVisibility);
		SerializeXMLVal<W>("run_visibility", ai, &runVisibility);
		SerializeXMLVal<W>("sprint_visibility", ai, &sprintVisibility);

		SerializeXMLVal<W>("walk_noise", ai, &walkNoise);
		SerializeXMLVal<W>("run_noise", ai, &runNoise);
		SerializeXMLVal<W>("sprint_noise", ai, &sprintNoise);
		SerializeXMLVal<W>("shoot_noise", ai, &fireNoise);

		SerializeXMLVal<W>("smell", ai, &smell);
	}
}

//////////////////////////////////////////////////////////////////////////

