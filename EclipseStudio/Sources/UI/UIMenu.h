#ifndef uimenu_h
#define uimenu_h

#include "r3d.h"
#include "APIScaleformGfx.h"

class UIMenu
{
public:
	UIMenu(const char * movieName) : mname(movieName), _MenuCode(0) {}
	virtual ~UIMenu() {}

	int	_MenuCode;

    r3dScaleformMovie* GetMovie() { return &gfxMovie; }

	virtual bool 	Load();
	virtual bool 	Initialize() = 0;
	virtual bool 	Unload();

	virtual int 	Update();

protected:
    r3dScaleformMovie gfxMovie;
	const char *mname;
	
};

#endif