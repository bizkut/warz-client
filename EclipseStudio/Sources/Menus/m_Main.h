#ifndef __wiimenu7_H
#define __wiimenu7_H

#include "ui/r3dMenu.h"

class Menu_Main : public MenuLayer
{
  public:
	START_MENU_ENUMS
		bEditor,
		bQuit
	};

  protected:	
	void		Draw();

  public:
	Menu_Main();
	~Menu_Main();
	
	int		DoModal();
};

#endif
