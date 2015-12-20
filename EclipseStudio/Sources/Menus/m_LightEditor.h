#pragma once

#include "menu/ct_menu.h"
#include "menu/Tool_AxisControl.h"

class Menu_LightEditor : public MenuLayer2
{
  protected:
	START_MENU_ENUMS
	  bClose,
	  
	  bBtnGroup1_Start,
	    bButton1,
	  bBtnGroup1_End,
		bButton2,
	    bButton3,
	    bButton4,
	  
	};
	void 		DrawButtons(int Start, int Num, char *Names[]);

	float 				Light_R;
	float 				Light_G;
	float 				Light_B;
	float 				Light_Radius;
	Tool_AxisControl	Tool;
	
	void		ProcessIMControls();

  protected:
	void		Init();
	void		Destroy();

	void		Activate();
	void		Deactivate();
	int		Update();
	void		Draw();

  public:
	Menu_LightEditor();
	~Menu_LightEditor();
	
	int		DoModal() { game_assert(0); return 1; }
};
