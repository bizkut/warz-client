#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"

#include "m_AppSelect.h"
#include "..\UI\UIMenu.h"

int	AppSelectMode = 100;


Menu_AppSelect::Menu_AppSelect()
{
}

Menu_AppSelect::~Menu_AppSelect()
{
}



void Menu_AppSelect::Draw()
{

	return;
}


extern bool g_bExit;

void ClearFullScreen_Menu()
{
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE );
	r3dRenderer->SetViewport( 0.f, 0.f, (float)r3dRenderer->d3dpp.BackBufferWidth, (float)r3dRenderer->d3dpp.BackBufferHeight );
	D3D_V( r3dRenderer->pd3ddev->Clear( 0, NULL, D3DCLEAR_TARGET, 0, r3dRenderer->GetClearZValue(), 0 ) );
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE );
}

int Menu_AppSelect::DoModal()
{
	// TEST SWF BEGIN
#if 0
	class CTestMenu : public UIMenu
	{
	public:
		CTestMenu(const char * movieName) : UIMenu(movieName) {}
		virtual ~CTestMenu() {};

		virtual bool Initialize() {return true;}
		virtual int Update() 
		{
			r3dProcessWindowMessages();

			r3dMouse::Show();
			r3dStartFrame();

			r3dRenderer->StartRender(1);
			r3dRenderer->StartFrame();

			r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);
			ClearFullScreen_Menu();

			gfxMovie.Update();
			gfxMovie.Draw();

			r3dRenderer->Flush();  
			r3dRenderer->EndFrame();
			r3dRenderer->EndRender( true );

			r3dEndFrame();

			return 0;
		};
	};

	CTestMenu* menu = new (ALLOC_TYPE_LOGIC) CTestMenu("data/menu/HUD_OutOfBattleZone.swf");
	menu->Load();
	int res = 0;
	if(menu->Initialize())
	{
		while(res == 0) {
			res = menu->Update();
		}
	}
	menu->Unload();
	SAFE_DELETE(menu);
	// TEST SWF END
#endif


	AppSelectMode = 100;

	Desktop().SetViewSize( r3dRenderer->ScreenW, r3dRenderer->ScreenH );

	while(1)
	{
		if(g_bExit)
			return 0;
		r3dStartFrame();

		mUpdate();

		imgui_Update();

		int ret = 1;

		mDrawStart();

		ClearFullScreen_Menu();

		r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);
		r3dSetFiltering( R3D_POINT );
		r3dRenderer->SetMipMapBias(-6.0f,-1);

		switch (AppSelectMode)
		{
		case 100:
			{
				const static char *BNames1[] = {"Update DB", "Game (Public Server)", "Game (DEV Server)" };

				for (int i=0;i<R3D_ARRAYSIZE(BNames1);i++)
					if (imgui_Button(r3dRenderer->ScreenW/2-(210*R3D_ARRAYSIZE(BNames1))/2+210*i, r3dRenderer->ScreenH/2 - 30,200, 30,BNames1[i], 0)) 
						released_id = bUpdateDB+i;

				const static char* BNames[] = {"Level Editor", "Particle Editor", "Physics Editor", "Character Editor" };

				for (int i=0;i<R3D_ARRAYSIZE(BNames);i++)
					if (imgui_Button(r3dRenderer->ScreenW/2-(210*R3D_ARRAYSIZE(BNames))/2+210*i, r3dRenderer->ScreenH/2 + 30,200, 30,BNames[i], 0)) 
						released_id = bStartLevelEditor+i;
			}
			break;
		}

		r3dRenderer->pd3ddev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_NZ);

		mDrawEnd();
		r3dEndFrame();

		switch(released_id)
		{
			case -1:
				break;
			default:
				return released_id;
		};

	}

	return 0;
}
