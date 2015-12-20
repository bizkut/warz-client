#pragma once
// interface to the Scaleform GFX flash player


namespace Scaleform {
	namespace GFx {
		class MovieDef;
		class Movie;
		class Value;
	}
	namespace Render
	{
		namespace D3D9
		{
			class Texture;
		}
	}
}
/*#ifdef FINAL_BUILD
#define SF_BUILD_SHIPPING 1
#endif*/
#include "GFxConfig.h"
#include "GFx.h"

// Scaleform GFx movie
class r3dScaleformMovie
{
private:
	Scaleform::GFx::MovieDef*	pMovieDef;
	Scaleform::GFx::Movie*	pMovie;
	Scaleform::GFx::MovieDisplayHandle  hMovieDisplay;

	int		viewX, viewY, viewW, viewH;
	int		movieW, movieH;

	int		ConvertMouseCoords;

	float timeForNextUpdate;
	float timePrevUpdate;

	int byteSize;


public:
	// base for external interface callback
	class sGFxEICallback
	{
	public:
		virtual void Execute(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, uint32_t argCount) const = NULL;
	};

	// templated class for calling EI callbacks
	template <class CLASS>
	class TGFxEICallback : public sGFxEICallback
	{
	private:
		typedef void (CLASS::*fn_exec)(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, uint32_t argCount);
		CLASS*		This;
		fn_exec		pFunc;

	public:
		TGFxEICallback(CLASS* in_this, fn_exec in_func)
		{
			This  = in_this;
			pFunc = in_func;
		}

		virtual void Execute(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, uint32_t argCount) const 
		{
			r3d_assert(pFunc);
			(This->*pFunc)(pMovie, args, argCount);
		}
	};

	typedef void (*fn_gfxEventHandler1)(void* data, r3dScaleformMovie* pMovie, const char* arg);
	typedef void (*fn_gfxEventHandler2)(void* data, r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, uint32_t argCount);
	struct gfxEvent
	{
		r3dString	EventName;
		void*		data;
		fn_gfxEventHandler1 fnc1;
		fn_gfxEventHandler2 fnc2;
		sGFxEICallback* eiCallback;

		gfxEvent()
		{
			fnc1 = NULL;
			fnc2 = NULL;
			eiCallback = NULL;
		}
	};
	int		NumGfxEvents;
	gfxEvent	EventHandlers[256];

public:
	r3dScaleformMovie();
	virtual ~r3dScaleformMovie();

	bool		Load(const char* fname, bool set_keyboard_focus);
	void		Unload();

	r3dScaleformMovie*		SetKeyboardCapture();

	int			GetMovieWidth() const { return movieW; }
	int			GetMovieHeight() const { return movieH; }
	void		GetViewport(int* x, int* y, int* w, int* h) const;  
	void		SetViewportTemp(int x, int y, int w, int h, Scaleform::GFx::Movie::ScaleModeType scaleType, int NeedConvertMouseCoords );
	void		SetBackBufferViewport(Scaleform::GFx::Movie::ScaleModeType scaleType);
	void		SetCurentRTViewport(Scaleform::GFx::Movie::ScaleModeType scaleType);

	void		SetVariable(const char *Var, const char *Value);
	void		SetVariable(const char *Var, const wchar_t *Value);
	void		SetVariable(const char *Var, const float Value);
	void		SetVariable(const char *Var, const int Value);
	void		SetVariableArrayElement(const char *VarPath, unsigned int index, unsigned int count, const void* data);

	void		Invoke(const char *Var, const char *Value);
	void		Invoke(const char *Var, const wchar_t *Value);
	void		Invoke(const char *Var, float Value);
	void		Invoke(const char *Var, int Value) { Invoke(Var, (float)Value); }
	void		Invoke(const char* pmethodName, const Scaleform::GFx::Value* pargs, uint32_t numArgs);
	void		Invoke(const char* pmethodName, Scaleform::GFx::Value *presult, const Scaleform::GFx::Value* pargs, uint32_t numArgs);

	void		UpdateAndDraw(bool skipDraw=false);

	Scaleform::Render::D3D9::Texture*		BoundRTToImage(const char* resName, LPDIRECT3DTEXTURE9 pRenderTarget, int RTWidth, int RTHeight);
	void		UpdateTextureMatrices(const char* resName, int RTWidth, int RTHeight);

	BOOL		RegisterEventHandler(const char* EventString, void* data, fn_gfxEventHandler1 Fnc);
	BOOL		RegisterEventHandler(const char* EventString, void* data, fn_gfxEventHandler2 Fnc);
	BOOL		RegisterEventHandler(const char* EventString, sGFxEICallback* eiCallback);

	Scaleform::GFx::Movie*	GetMovie() { return pMovie; }
	virtual	void		OnCommandCallback(const char* command, const char* arg);
	virtual	void		OnCommandCallback(const char* methodName, const Scaleform::GFx::Value* args, uint32_t argCount);
};

extern	void		r3dScaleformGfxCreate();
extern	void		r3dScaleformGfxSetImageDirectory(const char* dir);
extern	void		r3dScaleformGfxDestroy();
extern	void		r3dScaleformGfxSetFontLib(const char* swf);
extern void			r3dScaleformReset();
extern void			r3dScaleformSetUserMatrix(float xScale = 1.0f, float yScale = 1.0f, float xOffset = 0, float yOffset = 0);
extern void			r3dSetCurrentActiveMovie(r3dScaleformMovie* pMovie);

extern void			r3dScaleformForceReTranslation();
