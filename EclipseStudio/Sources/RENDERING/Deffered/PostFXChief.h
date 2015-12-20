#pragma once

#include "../sf/RenderBuffer.h"

#include "PostFX.h"

class PostFXChief
{

	// types
public:

	enum RTType
	{
		RTT_PINGPONG_LAST,
		RTT_PINGPONG_NEXT,
		// to prevent patching them to output to final buffer
		RTT_PINGPONG_LAST_AS_TEMP,
		RTT_PINGPONG_NEXT_AS_TEMP,
		RTT_FULL0_64BIT,
		RTT_FULL1_64BIT,
		RTT_TEMP0_64BIT,
		RTT_HALVED0_32BIT,
		RTT_HALVED1_32BIT,
		RTT_ONEFOURTH0_64BIT,
		RTT_ONEFOURTH1_64BIT,

		RTT_ONE8_0_64BIT,
		RTT_ONE8_1_64BIT,
		RTT_ONE16_0_64BIT,
		RTT_ONE16_1_64BIT,
		RTT_ONE32_0_64BIT,
		RTT_ONE32_1_64BIT,
		RTT_ONE64_0_64BIT,
		RTT_ONE64_1_64BIT,

		RTT_DIFFUSE_32BIT,
		RTT_DEPTH_32BIT,
		RTT_AUX_32BIT,

		RTT_MLAA_LINES_H,
		RTT_MLAA_LINES_V,

		RTT_NORMALS_32BIT,

		RTT_FLASHBANG_MULTIFRAME_16BIT,
		RTT_DISTORTION_32BIT,

		RTT_UI_CHARACTER_32BIT,

		RTT_PREV_FRAME_DEPTH,
		RTT_PREV_FRAME_SSAO,

		RTT_AVG_SCENE_LUMA,

		RTT_SCENE_EXPOSURE0,
		RTT_SCENE_EXPOSURE1,

		RTT_SCENE_SSR,
		RTT_SCENE_PREVSSR,

		RTT_COUNT
	};

	enum ActionType
	{
		AT_DRAW_FX,
		AT_DRAW_LAST_FX,
		AT_CLEAR,
		AT_CLEAR_LAST,
		AT_SWAPBUFFERS,
		AT_GRABSCREEN,
		AT_COUNT
	};

	struct Action
	{
		ActionType Type;

		union
		{
			struct
			{
				RTType	Source;
				RTType	Dest;		
				PostFX*	FX;
				RECT	TargetViewport ;
			} DrawFX;

			struct
			{
				RTType	Dest;
				DWORD	Color;
			} Clear;

			struct
			{
				RTType				Source;
				r3dScreenBuffer*	Target;
			} GrabScren;
		};
	};

	typedef void ( PostFXChief::* ActionFunc )( const Action& );

	typedef r3dTL::TStaticArray< ActionFunc, AT_COUNT > ActionFuncTable;

	typedef r3dTL::TArray< Action > Actions;

	enum
	{
		FREE_TEX_STAGE_START = 4,
		NUM_SAMPLERS = 8,

		DEFAULT_COLOR_WRITE_MASK =	D3DCOLORWRITEENABLE_ALPHA	|
									D3DCOLORWRITEENABLE_BLUE	|
									D3DCOLORWRITEENABLE_GREEN	|
									D3DCOLORWRITEENABLE_RED
	};

	// construction/ destruction
public:
	PostFXChief();
	~PostFXChief();

	// manipulation/ access
public:
	void	Init();
	void	Close();

	void	AddFX( PostFX& fx, RTType dest = RTT_PINGPONG_NEXT, RTType src = RTT_PINGPONG_LAST, RECT* targetViewport = NULL );
	void	AddClear( DWORD color, RTType dest );
	void	AddSwapBuffers();
	void	AddGrabScreen( r3dScreenBuffer* target, RTType source );

	void	Execute( bool toBackBuffer, bool resetTargets );

	int			GetDefaultVSId() const;
	int			GetRestoreWVSId() const;

	r3dFilter	GetZeroTexStageFilter() const;

	void	BindBufferTexture( RTType type, int stage );

	void	SetDefaultTexAddressMode( int stage );

	template< int Stage >
	void	SetDefaultTexFiltering();

	template< int Stage >
	void	SetDefaultTexFilteringTillN();

	void	SetDefaultColorWriteMask();

	r3dScreenBuffer* GetBuffer( RTType type ) const;

	void	ResetBuffers();

	void	DEBUG_MakePostFXSnapshot();
	void	DEBUG_PrintPostFXes();

	// helpers
private:
	bool PatchActions();
	void CopyOutput();

	void DEBUG_SaveStates();
	void DEBUG_CheckStates();

	void PatchDefaultTextures( r3dScreenBuffer* dest );
	void DepatchDefaultTextures( r3dScreenBuffer* dest );

	// NOTE : doesn't sets 'dest' - only passes it to PFX as resolution reference
	void DoDrawFX( PostFX* PFX, r3dScreenBuffer* src, r3dScreenBuffer* dest, RECT viewportRect );
	void DoClear( const Action& act );
	void PrepareBackBufferRender();

	// actions
private:
	void DrawFX( const Action& act );
	void DrawLastFX( const Action& act );
	void Clear( const Action& act );
	void ClearLast( const Action& act );
	void SwapBuffers( const Action& act );
	void GrabScreen( const Action& act );

	// data
private:
	int								mDefaultVSID;
	int								mRestoreWVSID;

	r3dFilter						mZeroStageFilter;

	Actions							mPendingActions;

	ActionFuncTable					mActionFuncTable;

	r3dScreenBuffer*				mScreenBuffers[ RTT_COUNT ];

#ifdef _DEBUG

	DWORD							mDEBUG_AddressMode[ NUM_SAMPLERS ][ 3 ];
	DWORD							mDEBUG_Filtering[ NUM_SAMPLERS ][ 3 ];

	DWORD							mDEBUG_ColorWriteMask;
#endif

};
extern PostFXChief* g_pPostFXChief;

//------------------------------------------------------------------------

R3D_FORCEINLINE
r3dFilter
PostFXChief::GetZeroTexStageFilter() const
{
	return mZeroStageFilter ;
}