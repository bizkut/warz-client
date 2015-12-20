#pragma once

#include "../../ServerNetPackets/NetPacketsGameInfo.h"

// to save\load various user settings that are not compatible with vars (variable number of params for example)
class UserSettings
{
private:
	struct FilterGames
	{
		bool hideempty;
		bool hidefull;

		bool tracers2;
		bool nameplates2;
		bool crosshair2;
		bool enable_options;
		bool password;
		int	timeLimit;

		bool region_us;
		bool region_eu;
		bool region_ru;
		bool region_sa; // south america

		FilterGames() 
		{ 
			hideempty = false;
			hidefull = false;

			tracers2 = true;
			nameplates2 = true;
			crosshair2 = true;
			enable_options = false;
			password = false;
			timeLimit = 0;

			region_us = true;
			region_eu = false;
			region_ru = false;
			region_sa = false;
		}
	};
public:
	UserSettings();
	~UserSettings();

	FilterGames BrowseGames_Filter;
	typedef r3dgameList(GBGameInfo) TGameList;
	TGameList RecentGames;
	TGameList FavoriteGames;
	void loadGames(TGameList& list, const pugi::xml_node& xmlGames);
	void saveGames(const TGameList& list, pugi::xml_node& xmlGames);

	void loadSettings();
	void saveSettings();

	void addGameToRecent(const GBGameInfo& ginfo);
	void addGameToFavorite(const GBGameInfo& ginfo); // will auto from list if game already in list

	bool isInRecentGamesList(DWORD gameID);
	bool isInFavoriteGamesList(DWORD gameID);

private:
	bool createFullPath( char* dest, bool old );

	void loadXML(const char* file);
	void saveXML(const char* file);
};
extern UserSettings gUserSettings;