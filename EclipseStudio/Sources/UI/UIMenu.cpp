#include "r3dPCH.h"

#include "UIMenu.h"


bool UIMenu::Load()
{
    if(!gfxMovie.Load(mname, true)) 
		return false;

	gfxMovie.SetBackBufferViewport(Scaleform::GFx::Movie::SM_ExactFit);

	_MenuCode = 1;

	return true;
}


bool UIMenu::Unload()
{
	gfxMovie.Unload();
	return true;
}

int UIMenu::Update()
{
	r3dProcessWindowMessages();

	r3dRenderer->StartRender(1);
	gfxMovie.UpdateAndDraw();
	r3dRenderer->Flush();  
	r3dRenderer->EndRender();
	return _MenuCode;
}

