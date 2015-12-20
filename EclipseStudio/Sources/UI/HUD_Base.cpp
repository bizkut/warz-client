#include "r3dPCH.h"
#include "r3d.h"

#include "../SF/CmdProcessor/CmdProcessor.h"
#include "../SF/CmdProcessor/CmdConsole.h"

#include "HUD_Base.h"


BaseHUD::BaseHUD () 
: bInited ( 0 )
{
}

BaseHUD::~BaseHUD() 
{
}

void BaseHUD::Init ()
{
	if ( !bInited )
	{
		bInited = 1;
		InitPure ();
	}
}

void BaseHUD::Destroy ()
{
	if ( bInited )
	{
		bInited = 0;
		DestroyPure ();
	}
}

void BaseHUD::SetCamera ( r3dCamera &Cam )
{
	SetCameraPure ( Cam );
}

void BaseHUD::HudSelected ()
{
	OnHudSelected ();
}

void BaseHUD::HudUnselected ()
{
	OnHudUnselected ();
}

/*virtual*/
r3dPoint3D
BaseHUD::GetCamOffset() const
{
	return r3dPoint3D( 0, 0, 0 );
}

//------------------------------------------------------------------------

/*virtual*/
void
BaseHUD::SetCamPos( const r3dPoint3D& pos )
{
	FPS_Position = pos - GetCamOffset();
}

//------------------------------------------------------------------------
#ifndef FINAL_BUILD

DECLARE_CMD( profile )
{
	if( r_show_profiler->GetInt() )
	{
		g_cursor_mode->SetInt( !g_cursor_mode->GetInt() );
	}
	else
	{
		r_show_profiler->SetInt( 1 );
		g_cursor_mode->SetInt( 1 );
	}
}

DECLARE_CMD( shadowstats )
{
	void DumpShadowDipsStats();
	DumpShadowDipsStats();
}

DECLARE_CMD( frozenshadowstats )
{
	void DumpFrozenShadowStats();
	DumpFrozenShadowStats();
}

DECLARE_CMD( skytexsize )
{
	void DumpAtmoTextureStats();
	DumpAtmoTextureStats();
}

DECLARE_CMD( resetterraphysics )
{
	void ResetTerraPhysics();
	ResetTerraPhysics();
}

DECLARE_CMD( terrastats )
{
	void PrintTerraStats();
	PrintTerraStats();
}

DECLARE_CMD( lightdepthstat )
{
	void DumpLightDepthStats();
	DumpLightDepthStats();
}

DECLARE_CMD( dumpmegatiles )
{
	void DumpTerrain3MegaTiles();
	DumpTerrain3MegaTiles();
}

DECLARE_CMD( auralpha )
{
	if ( ev.NumArgs() != 2 )
	{
		ConPrint( "auralpha {alpha}" );
		return;
	}

	void DebugPlayerAuraTrasnparency( float alpha );
	DebugPlayerAuraTrasnparency( ev.GetFloat( 1 ) );
}

DECLARE_CMD( freezeterra )
{
	void FreezeTerrain3();
	FreezeTerrain3();
}

DECLARE_CMD( fixterracam3 )
{
	void ToggleTerrain3FixedCam();
	ToggleTerrain3FixedCam();
}

DECLARE_CMD( refreshterratiles )
{
	void RefreshTerraTiles();
	RefreshTerraTiles();
	ConPrint( "Refreshed!" );
}

DECLARE_CMD( refreshmaskterratiles )
{
	void RefreshMaskTerraTiles();
	RefreshMaskTerraTiles();
	ConPrint( "Refreshed!" );
}

DECLARE_CMD( manufacture )
{
	void AI_PlayerManufacture();
	AI_PlayerManufacture();
}

DECLARE_CMD( thumbs )
{
	void ThumbStats();
	ThumbStats();
}

DECLARE_CMD( vmemcrash )
{
	void MemCrash( bool );
	MemCrash( true );
}


DECLARE_CMD( memcrash )
{
	void MemCrash( bool );
	MemCrash( false );
}

DECLARE_CMD( zspawn )
{
	void SpawnZombie();
	SpawnZombie();
}

DECLARE_CMD( dumpobjs )
{
	void DumpObjects();
	DumpObjects();
}

DECLARE_CMD( dumpheaps )
{
	void DumpHeaps();
	DumpHeaps();
}

DECLARE_CMD( dumptex )
{
	void DumpTextures();
	DumpTextures();
}

DECLARE_CMD( dumpobjtex )
{
	void DumpObjTextures();
	DumpObjTextures();
}


DECLARE_CMD( flushmesh )
{
	void r3dFlushReleasedMeshes() ;
	r3dFlushReleasedMeshes() ;
}

DECLARE_CMD( testfb )
{
	void TestFlashBang() ;
	TestFlashBang() ;
}

DECLARE_CMD( terrafetch )
{
	void SwitchTerraFetch() ;
	SwitchTerraFetch() ;

	void SwitchTerraFetchT2() ;
	SwitchTerraFetchT2() ;
}

DECLARE_CMD( rts )
{
	void DumpRTStats() ;
	DumpRTStats() ;
}

DECLARE_CMD( armory )
{
	void DumpArmoryStats() ;
	DumpArmoryStats() ;
}

DECLARE_CMD( playertex )
{
	void DumpPlayerTexes() ;
	DumpPlayerTexes() ;
}

DECLARE_CMD( vmem )
{
	void DumpVMemStats() ;
	DumpVMemStats() ;
}

DECLARE_CMD( phystats )
{
	void DumpPhysStats() ;
	DumpPhysStats() ;
}

DECLARE_CMD( setpos )
{
	if ( ev.NumArgs() != 4 )
	{
		ConPrint( "setpos {x} {y} {z}" );
		return;
	}

	extern r3dCamera gCam;

	r3dPoint3D Position;

	Position.x = ev.GetFloat( 1 );
	Position.y = ev.GetFloat( 2 );
	Position.z = ev.GetFloat( 3 );

	if( Hud )
		Hud->SetCamPos( Position );
	else
		gCam = Position;
}

//------------------------------------------------------------------------

DECLARE_CMD( printpos )
{
	extern r3dCamera gCam;

	ConPrint( "Current Position: %.3f, %.3f, %.3f", gCam.x, gCam.y, gCam.z );
}

//------------------------------------------------------------------------

DECLARE_CMD( cursor )
{
	if ( ev.NumArgs() != 2 )
	{
		ConPrint( "cursor {0/1}" );
		return;
	}

	if( ev.GetInteger( 1 ) )
	{
		g_cursor_mode->SetInt( 1 );
		r3dMouse::Show(true);
	}
	else
	{
		g_cursor_mode->SetInt( 0 );
		r3dMouse::Show(false);
	}
}

DECLARE_CMD( showripples )
{
	if ( ev.NumArgs() != 2 )
	{
		ConPrint( "showripples {0/1}" );
		return;
	}

	void DebugShowRipples();
	void DebugTextureOff();

	if( ev.GetInteger( 1 ) )
	{
		DebugShowRipples();
	}
	else
	{
		DebugTextureOff();
	}
}

DECLARE_CMD( aura )
{
	void DebugCyclePlayerAura();
	DebugCyclePlayerAura();
}

DECLARE_CMD( die )
{
	void DebugPlayerDie();
	DebugPlayerDie();
}

DECLARE_CMD( ragdoll )
{
	void DebugPlayerRagdoll();
	DebugPlayerRagdoll();
}

DECLARE_CMD( export_physx_scene )
{
	void DebugExportPhysxScene();
	DebugExportPhysxScene();
}

//------------------------------------------------------------------------

void RegisterHUDCommands()
{
	REG_CCOMMAND( profile, 0, "Shows profiler and turns cursor mode on (for using mouse in game)" );
	REG_CCOMMAND( shadowstats, 0, "Prints shadow stats" );
	REG_CCOMMAND( frozenshadowstats, 0, "Prints stats of frozen shadow textures" );
	REG_CCOMMAND( skytexsize, 0, "Prints sky texture vid mem size" );
	REG_CCOMMAND( resetterraphysics, 0, "Resets terrain physics" );
	REG_CCOMMAND( terrastats, 0, "Print terrain stats" );
	REG_CCOMMAND( lightdepthstat, 0, "Output frozen light infromation" );
	REG_CCOMMAND( dumpmegatiles, 0, "Print all terrain 3 megatiles" );
	REG_CCOMMAND( auralpha, 0, "Set alpha on player aura" );
	REG_CCOMMAND( freezeterra, 0, "Freeze tile loading on terrain 3" );
	REG_CCOMMAND( fixterracam3, 0, "Toggle camera fixed mode for terrain3 (do not update with actual camera movement)" );
	REG_CCOMMAND( refreshmaskterratiles, 0, "Refresh all mask terrain 3 tiles" );
	REG_CCOMMAND( refreshterratiles, 0, "Refresh all terrain 2 tiles" );
	REG_CCOMMAND( manufacture, 0, "Player manufactures and drops items" );
	REG_CCOMMAND( thumbs, 0, "Show thumbnail stats" );
	REG_CCOMMAND( vmemcrash, 0, "Allocate video memory using d3d until the app crashes" );
	REG_CCOMMAND( memcrash, 0, "Allocate system memory using d3d until the app crashes" );
	REG_CCOMMAND( zspawn, 0, "Spawn zombie" );
	REG_CCOMMAND( dumpobjs, 0, "Dump objects stats" );
	REG_CCOMMAND( dumpheaps, 0, "Dump process heaps" );
	REG_CCOMMAND( dumptex, 0, "Dump all loaded textures" );
	REG_CCOMMAND( dumpobjtex, 0, "Dump loaded object textures" );
	REG_CCOMMAND( flushmesh, 0, "Flush released meshes");
	REG_CCOMMAND( testfb, 0, "Test flash bang");
	REG_CCOMMAND( terrafetch, 0, "Switch terrain vfetch flag" );
	REG_CCOMMAND( rts, 0, "Dump render target stats" );
	REG_CCOMMAND( armory, 0, "Dump armory stats usage" );
	REG_CCOMMAND( playertex, 0, "Dump player texture stats" );
	REG_CCOMMAND( vmem, 0, "Dump video memory stats" );
	REG_CCOMMAND( phystats, 0, "Dump physics stats" );
	REG_CCOMMAND( setpos, 0, "Teleport observer to given coordinates" );
	REG_CCOMMAND( printpos, 0, "Print current camera position" );
	REG_CCOMMAND( cursor, 0, "Turn mouse cursor on/off" );
	REG_CCOMMAND( showripples, 0, "Show ripples tex on/off" );
	REG_CCOMMAND( aura, 0, "Cycle player aura state"  );
	REG_CCOMMAND( die, 0, "Make character die!" );
	REG_CCOMMAND( ragdoll, 0, "Switch character to ragdoll" );
	REG_CCOMMAND( export_physx_scene, 0, "Export whole physx scene into collection file" );
}
#endif
