#pragma once

#include "menu/ct_menu.h"

class Menu_Launcher : public MenuLayer2
{
  protected:
	START_MENU_ENUMS
	  bClose,
	  
	  bBtnGroup1_Start,
	    bScene,
	    bTerrain,
	    bObjects,
	    bWater,
	    bProcedural,
	    bMegatexture,
	    bEnvironment,
	    bLights,
	    bRendering,
	  bBtnGroup1_End,


	  bBtnGroup2_Start,
	    bNewScene,
	    bLoadScene,
	    bSaveScene,
	    bHaveTerrain,
	    bHaveWater,
	  bBtnGroup2_End,

	  bBtnGroup2_Start,
	    bHeightmap,
	    bHeightScale,
	    bCellsize,
	    bRenderQuality,
	  bBtnGroup2_End,


	  
	};
	void 		DrawButtons(int Start, int Num, char *Names[]);

	void		ProcessIMControls();

  protected:
	void		Init();
	void		Destroy();

	void		Activate();
	void		Deactivate();
	int		Update();
	void		Draw();

  public:
	Menu_Launcher();
	~Menu_Launcher();
	
	int		DoModal() { assert(0); return 1; }
};
