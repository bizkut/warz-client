#ifndef __HEROWARS__M_GUI_H
#define __HEROWARS__M_GUI_H

class Menu_ItemShop2;
class Menu_CharInfo;

extern r3dPoint3D Level_Center;
extern r3dPoint3D Level_Size;
extern r3dPoint3D Level_Org;

#include "menu/ct_menu.h"

#define GUIMODE_MAIN	0
#define GUIMODE_STATS	3
#define GUIMODE_PAUSE	4
#define GUIMODE_CONSOLE	5

class GameObject;

class CFieldOfView
{
  public:
	float*		fowData;
	int		cellsX_;
	int		cellsY_;

	float		worldSize;
	float		fowDecay;

  public:
	CFieldOfView();
	~CFieldOfView();
	
	void		Init(int w, int h);
	void		Destroy();
	
	void		Update(float timePassed);
	void		MarkCell(const r3dPoint3D& pos, float radius);
	float		Get(const r3dPoint3D& pos);

	float		Get(int x, int y) {
	  assert(x >= 0 && x < cellsX_);
	  assert(y >= 0 && y < cellsY_);
	  return fowData[y * cellsX_ + x];
	}
};

class CGUIMap
{
  public:
	float		map_x, map_y, map_w, map_h;
	float		worldSize;
	r3dTexture*	pingTex;
	
	float		pingStop;
	r3dPoint3D	pingLoc;
	
	CFieldOfView	fowArray;
	
	r3dTexture*	fowTex;
	float		nextFowUpdate;
	void		UpdateFow();

  public:
	CGUIMap();
	~CGUIMap();

	void		Init(float x, float y, float w, float h, float ws, r3dTexture* tex);
	void		Destroy();
	void		SetWorldSize(float ws) {
	  worldSize = ws;
	  fowArray.worldSize = ws;
	}
	void		Update();
	void		Draw();
	
	void		ToMap(const r3dPoint3D& pos, float* x, float* y) {
		  r3dPoint3D pos1 = pos - Level_Org;

          *x = map_x + (pos1.X / worldSize) * map_w;
          *y = map_y + (map_h - (pos1.Z / worldSize) * map_h);
	}
	void		FromMap(float x, float y, r3dPoint3D* pos) {

	  pos->x = (x-map_x) * (worldSize / map_w) + Level_Org.X;
	  pos->y = 0;
	  pos->z = (worldSize - (y-map_y) * (worldSize / map_h)) + Level_Org.Z;
	}

	void		Ping(const r3dPoint3D& pos);
};
extern CGUIMap		guiMap;


class Menu_GUI : public MenuLayer
{
  public:
	START_MENU_ENUMS
	  bMainHUDArea,

	  bMiniMap,
	  bPingMiniMap,

	  bInvSlot1, bInvSlot2, bInvSlot3, bInvSlot4, bInvSlot5, bInvSlot6,

	  bSpellUpgrade1, bSpellUpgrade2, bSpellUpgrade3,   bSpellUpgrade4,

	  bSpellCast1, bSpellCast2, bSpellCast3, bSpellCast4,
    
	  bTargetInvPane,
	  bTargetInvSlot1, bTargetInvSlot2, bTargetInvSlot3, bTargetInvSlot4, bTargetInvSlot5, bTargetInvSlot6,

	  bPetPane,
    
	  bSummonCast,
	  bSummonBar1,
	  bSummonBar2,

	  bChar1Pict,
	  bChar1HPBar,
	  bChar1MPBar,
	  bChar1EXPBar,
	  bChar1Stats,
	  bChar1Buffs,
    
	  // note: 2nd copy of vars for 2nd target display
	  bChar2Pict,
	  bChar2HPBar,
	  bChar2MPBar,
	  bChar2EXPBar,
	  bChar2Stats,
	  bChar2Buffs,

	  bGoldArea,
    
	  bUI_Immortal,
	  bUI_Options,
	  bUI_GameStatus,

	  bStats_Close,

	  bOptions_Options,
	  bOptions_Resume,
	  bOptions_Quit,

    /*  These enums are for zones used in localized UI tooltips.    
        Not buttons, used only for testing mouse location against a tooltip zone.   
        KB 10.5.07  */
	ltt_TimeOfDay,
    };

  public:
	r3dTexture*	imgHUD1;
	r3dTexture*	imgHUD2;

	r3dTexture*	imgDayNight;
	r3dTexture*	imgPingMarker;
	UIElements*	ui_elm;
	
	Menu_ItemShop2*	menuShop;
	Menu_CharInfo*  menuCharInfo;

	int		CurMode;

	bool		mPinging;
	bool		mMapMoveBlocked;
	
	int		invDragSlot;
	
	float		invPane2Visible;	// target inventory pane visibility koef
	float		petPaneVisible;		// pet pane visibility

	void		DrawGameStats();
	void		DrawPause();

	void		ActivateShop();
	void		ActivateCharacterInfo(int TabID);

	void		DrawUnitInfo(const GameObject* obj, bool bTarget);
	void		DrawHeroInfo(const GameObject* obj);

	void		ProcessMain();
	void		ProcessGameStats();
	void		ProcessPause();
	void		ProcessConsole();

  public:
	Menu_GUI();

	void		Init();
	void		Destroy();

	void		Activate();
	void		Deactivate();

	int		Process();
	void		Draw();
	int		DoModal() { r3dError("Menu_GUI::DoModal"); return 0; }

	void		PingMiniMap(const r3dPoint3D& PingLoc);
};

#endif	// __HEROWARS__M_GUI_H
