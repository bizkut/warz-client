#include "r3dPCH.h"
#include "r3d.h"

#include "fmod/SoundSys.h"

#include "APIScaleformGfx.h"

#undef SetStreamSourceFreq

#include "GFx_Kernel.h"
#include "GFx_Renderer_D3D9.h"
#include "GFx_FontProvider_Win32.h"
#include "GFX/AS3/AS3_Global.h"

// See r3dRender.h
#define SetStreamSourceFreq DoSetStreamSourceFreq

#include "Render/ImageFiles/PNG_ImageFile.h"
#include "Render/ImageFiles/DDS_ImageFile.h"


#include "LangMngr.h"

#define NAMESPACE_NAME Scaleform
#define MEMORY_TYPE ALLOC_TYPE_SCALEFORM
#include "AutodeskMemoryAllocator.h"
#undef MEMORY_TYPE
#undef NAMESPACE_NAME

// libs
#pragma comment(lib, "libpng.lib")
#pragma comment(lib, "libjpeg.lib")
#pragma comment(lib, "pcre.lib")

#ifdef _DEBUG
#pragma comment(lib, "libgfx_DebugOpt.lib")
#pragma comment(lib, "libgfx_as3_DebugOpt.lib")
#pragma comment(lib, "libgfxexpat_DebugOpt.lib")
#elif defined(FINAL_BUILD)
#pragma comment(lib, "libgfx_Release.lib")
#pragma comment(lib, "libgfx_as3_Release.lib")
#pragma comment(lib, "libgfxexpat_Release.lib")
//#pragma comment(lib, "libgfx_Shipping.lib")
//#pragma comment(lib, "libgfx_as3_Shipping.lib")
//#pragma comment(lib, "libgfxexpat_Shipping.lib")
#else // RELEASE
#pragma comment(lib, "libgfx_Release.lib")
#pragma comment(lib, "libgfx_as3_Release.lib")
#pragma comment(lib, "libgfxexpat_Release.lib")
#endif

//////////////////////////////////////////////////////////////////////////

void r3dAddUITextureMemoryStats(int w, int h, int d, int mips, D3DFORMAT fmt)
{
	int mem = r3dGetTextureSizeInVideoMemory(w, h, d, mips, fmt);
	r3dRenderer->Stats.AddUITexMem(mem);
}

//////////////////////////////////////////////////////////////////////////

void r3dRemoveUITextureMemoryStats(int w, int h, int d, int mips, D3DFORMAT fmt)
{
	int mem = -r3dGetTextureSizeInVideoMemory(w, h, d, mips, fmt);
	r3dRenderer->Stats.AddUITexMem(mem);
}

//////////////////////////////////////////////////////////////////////////

void r3dAddUIBufferMemoryStats(int size)
{
	r3dRenderer->Stats.AddUIBufferMem(size);
}

//////////////////////////////////////////////////////////////////////////

char		_sGfx_DefaultImagePath[MAX_PATH] = "data\\menu";

//hack: extern var from eternity WinMain.cpp
typedef bool (*Win32MsgProc_fn)(UINT uMsg, WPARAM wParam, LPARAM lParam);

const char* detect_gfx_absolute_path(const char* purl)
{
	static const char ABSOLUTE_PATH_PREFIX = '$';

	//NOTE: '$' char will specify start of absolute path for files
	// because scaleform append original movie directory to all paths
	const char* fname = strchr(purl, ABSOLUTE_PATH_PREFIX);
	if(fname)
		fname = fname + 1;
	else
		fname = purl;

	return fname;
}

class r3dScaleformMemoryFile : public Scaleform::MemoryFile
{
public:
	const uint8_t* SavedFileData;

	r3dScaleformMemoryFile(const char* pfileName, const uint8_t *pBuffer, int buffSize)
		: Scaleform::MemoryFile(pfileName, pBuffer, buffSize)
	{
		//r3dOutToLog("r3dScaleformMemoryFile for %s\n", GetFilePath());
		SavedFileData = pBuffer;
	}

	virtual ~r3dScaleformMemoryFile()
	{
		//r3dOutToLog("~r3dScaleformMemoryFile for %s\n", GetFilePath());
		delete[] SavedFileData;
	}
};

// custom file opener
class r3dGFXFileOpener : public Scaleform::GFx::FileOpener
{
public:
	virtual Scaleform::File* OpenFile(const char* purl, int flags, int mode)
	{
		const char* abs_fname = detect_gfx_absolute_path(purl);
		r3dFile* f = r3d_open(abs_fname, "rb");
		if(f == NULL)
			return new Scaleform::SysFile(purl);

		//r3dOutToLog("GFX loaded file '%s'\n", purl);

		BYTE* data = new  BYTE[f->size+1];
		int   size = f->size;
		fread(data, 1, f->size, f);
		fclose(f);
		return new r3dScaleformMemoryFile(abs_fname, data, size);
	}
};

class r3dGFxLog : public Scaleform::GFx::Log
{
public:
	// We override this function in order to do custom logging.
	virtual void LogMessageVarg(Scaleform::LogMessageId messageType, const char* pfmt, va_list argList)
	{
#ifdef FINAL_BUILD
		return;
#endif

		char buf[1024];
		StringCbVPrintfA(buf, sizeof(buf), pfmt, argList);

		/*if ( d_gfxlog_filter->GetBool() )
		{
		const char * szStr[] = {
		"Error: Invoked method",
		"Error: Static method"
		};

		for ( int i = 0; i < sizeof( szStr) / sizeof( char* ); i++ )
		{
		if ( strstr( buf, szStr[ i ] ) )
		return;
		}
		}
*/
		if(strstr(buf, "CallMethod - 'apply' on invalid object") != NULL)
			return;
		if(strstr(buf, "broadcastMessage") != NULL)
			return;
		if(strstr(buf, "Invalid movie clip path")!=NULL)
			return;
		if(strstr(buf, "'getNextHighestDepth' on invalid object")!=NULL)
			return;
		if(strstr(buf, "'gotoAndPlay' on invalid object")!=NULL)
			return;
		if(strstr(buf, "'gotoAndStop' on invalid object")!=NULL)
			return;

		switch(messageType.GetMessageType())
		{
#ifndef FINAL_BUILD
case Scaleform::LogMessage_Text:
case Scaleform::LogMessage_Warning:
	r3dOutToLog("GFx Log: %s\n", buf);
	break;
case Scaleform::LogMessage_Report:
	r3dOutToLog("GFx Report: %s\n", buf);
	break;
#endif
case Scaleform::LogMessage_Error:
	r3dOutToLog("GFx Error: %s\n", buf);
	break;
case Scaleform::LogMessage_Assert:
	r3dOutToLog("GFx Assert: %s\n", buf);
#ifndef FINAL_BUILD
	r3d_assert(false);
#endif
	break;
default:
	break;
		}
	}
};

// "fscommand" callback, handles notification callbacks from ActionScript.
class r3dGFxFSCommandHandler : public Scaleform::GFx::FSCommandHandler
{
public:
	virtual	void Callback(Scaleform::GFx::Movie* pmovie, const char* command, const char* arg);
};

// "external interface" callback from Scaleform
class r3dGFxExternalInterface : public Scaleform::GFx::ExternalInterface
{
public:
	virtual	void Callback(Scaleform::GFx::Movie* pmovie, const char* methodName, const Scaleform::GFx::Value* args, unsigned argCount);
};

// user event handler
class r3dGFxUserEventHandler : public Scaleform::GFx::UserEventHandler
{
public:
	virtual	void HandleEvent(Scaleform::GFx::Movie* pmovie, const Scaleform::GFx::Event& event);
};

class TranslatorImpl : public Scaleform::GFx::Translator
{
	virtual unsigned GetCaps() const
	{
		// Kill ending new line that gets generated if HTML text field content is used to create a translation key.
		return Scaleform::GFx::Translator::Cap_StripTrailingNewLines;
	}

	virtual void Translate(Scaleform::GFx::Translator::TranslateInfo* ptranslateInfo)
	{
		const char* instanceName = ptranslateInfo->GetInstanceName();
		if(strcmp(instanceName, "ChatOutputText")==0) // hack noTranslate
			return;

		const wchar_t* keyName = ptranslateInfo->GetKey();
		// check if instance name has $ sign
		if (instanceName[0] == '$')
		{
			ptranslateInfo->SetResult(gLangMngr.getString(instanceName));
		}
		// also check if text value has $ sign, as in some cases it is easier to mark it that way, rather than change instance name in flash
		else if(wcschr(keyName, L'$'))
		{
			wchar_t tmpStrResult[2048] = {0};
			wchar_t tokenStr[2048] = {0};
			r3dscpy(tokenStr, keyName);
			wchar_t* token = wcstok(tokenStr, L" ");
			while(token != NULL)
			{
				if(token[0] == L'$')
				{
					wcscat(tmpStrResult, utf8ToWide(gLangMngr.getString(wideToUtf8(token))));
				}
				else
					wcscat(tmpStrResult, token);
				token = wcstok(NULL, L" ");
				if(token != NULL)
					wcscat(tmpStrResult, L" ");
			}
			ptranslateInfo->SetResult(tmpStrResult);
		}
		
		if (instanceName[0] == '%')
		{
			ptranslateInfo->SetResultHtml(gLangMngr.getString(instanceName));
		}
		// also check if text value has % sign, as in some cases it is easier to mark it that way, rather than change instance name in flash
		else if(keyName[0] == L'%')
		{
			ptranslateInfo->SetResultHtml(gLangMngr.getString(wideToUtf8(keyName)));
		}
	}
};

class r3dGFxTextClipboard : public Scaleform::GFx::TextClipboard
{ 
public: 
	void OnTextStore(const wchar_t* ptext, size_t len) 
	{ 
		// store the text in a system clipboard. 
		if (OpenClipboard(NULL)) 
		{ 
			// Empty the Clipboard. This also has the effect 
			// of allowing Windows to free the memory associated 
			// with any data that is in the Clipboard 
			EmptyClipboard(); 

			HGLOBAL hClipboardData; 
			hClipboardData = GlobalAlloc(GMEM_DDESHARE, (len+1)*sizeof(wchar_t)); 
			r3d_assert(sizeof(wchar_t) == 2); 

			// Calling GlobalLock returns a pointer to the 
			// data associated with the handle returned from 
			// GlobalAlloc 
			wchar_t * pchData; 
			pchData = (wchar_t*)GlobalLock(hClipboardData); 
			r3dscpy(pchData, ptext); 

			// Once done, unlock the memory. 
			// don't call GlobalFree because Windows will free the 
			// memory automatically when EmptyClipboard is next 
			// called. 
			GlobalUnlock(hClipboardData); 

			// Now, set the Clipboard data by specifying that 
			// Unicode text is being used and passing the handle to 
			// the global memory. 
			SetClipboardData(CF_UNICODETEXT, hClipboardData); 

			// Finally, close the Clipboard which has the effect of 
			// unlocking it so that other  apps can examine or modify its contents. 
			CloseClipboard(); 
		} 
	} 
}; 

class APIScaleformGfx : public r3dIResource
{
	friend void r3dScaleformReset();
public:
	Scaleform::GFx::Loader loader;
	Scaleform::Ptr<Scaleform::Render::D3D9::HAL>  RendererHAL;
	Scaleform::Ptr<Scaleform::Render::Renderer2D>  Renderer;

	LPDIRECT3DSTATEBLOCK9   pStateBlock;
	r3dScaleformMovie*		pCurMovie;		// movie currently being processed
	r3dScaleformMovie*		pKbdCaptureMovie;	// movie to receive keyboard input

protected:
	virtual	void		D3DCreateResource();
	virtual	void		D3DReleaseResource();

public:
	APIScaleformGfx( const r3dIntegrityGuardian& ig );
	virtual ~APIScaleformGfx();

	bool		Create();
	void		Destroy();
	void		SetFontLib();

	// ui sounds IDs
	int			sndMenuBtnClick;
	int			sndMenuBtnUse;
	int			sndMenuClose;
	int			sndMenuOpen;
	int			sndWeaponAttach;
	int			sndWeaponQuickChange;
};


APIScaleformGfx*	gAPIScaleformGfx = NULL;

void r3dScaleformReset()
{
	if(gAPIScaleformGfx)
	{
		gAPIScaleformGfx->D3DReleaseResource();
		gAPIScaleformGfx->D3DCreateResource();
	}
}

void r3dScaleformSetUserMatrix(float xScale, float yScale, float xOffset, float yOffset)
{
	/*if (gAPIScaleformGfx->RendererHAL)
	{
		Scaleform::Render::Matrix2x4<float> m(Scaleform::Render::Matrix2x4<float>::Identity);
		m.M[0][0] = xScale;
		m.M[1][1] = yScale;
		m.M[0][2] = xOffset;
		m.M[1][2] = yOffset;
		gAPIScaleformGfx->RendererHAL->SetUserMatrix(m);
	}*/
}

void r3dSetCurrentActiveMovie(r3dScaleformMovie* pMovie)
{
	if(!gAPIScaleformGfx) return;

	gAPIScaleformGfx->pCurMovie = pMovie;
}
void r3dScaleformGfxCreate()
{
	if(gAPIScaleformGfx) return;

#ifdef USE_R3D_MEMORY_ALLOCATOR
	static Scaleform::r3dAutodeskAlloc gScaleformAlloc;
	Scaleform::GFx::System::Init(&gScaleformAlloc);
#else
	Scaleform::GFx::System::Init();
#endif
 
	r3dIntegrityGuardian ig ;

	gAPIScaleformGfx = game_new APIScaleformGfx( ig );
	gAPIScaleformGfx->Create();
}

void r3dScaleformGfxDestroy()
{
	if(!gAPIScaleformGfx) return;

	gAPIScaleformGfx->Destroy();
	SAFE_DELETE(gAPIScaleformGfx);

	Scaleform::GFx::System::Destroy();
}

void r3dScaleformGfxSetImageDirectory(const char* dir)
{
	r3dscpy(_sGfx_DefaultImagePath, dir);
}

void r3dGFxFSCommandHandler::Callback(Scaleform::GFx::Movie* pmovie, const char* command, const char* arg)
{
	//r3dOutToLog("fsCallback: '%s' '%s'\n", command, arg);
	if(gAPIScaleformGfx->pCurMovie) 
		gAPIScaleformGfx->pCurMovie->OnCommandCallback(command, arg);

	return;
}

void r3dGFxExternalInterface::Callback(Scaleform::GFx::Movie* pmovie, const char* methodName, const Scaleform::GFx::Value* args, unsigned argCount)
{
	//r3dOutToLog("fsCallback: '%s' '%s'\n", methodName, arg);
	if(gAPIScaleformGfx->pCurMovie) 
		gAPIScaleformGfx->pCurMovie->OnCommandCallback(methodName, args, argCount);
}

void r3dGFxUserEventHandler::HandleEvent(Scaleform::GFx::Movie* pmovie, const Scaleform::GFx::Event& event)
{
	switch(event.Type)
	{
	case Scaleform::GFx::Event::DoShowMouse:
		Mouse->Show();
		break;
	case Scaleform::GFx::Event::DoHideMouse:
		Mouse->Hide();
		break;

	case Scaleform::GFx::Event::DoSetMouseCursor:
		{
			/*
			const GFxMouseCursorEvent& mcEvent = static_cast<const GFxMouseCursorEvent&>(event);
			switch(mcEvent.CursorShape)
			{
			case GFxMouseCursorEvent::ARROW:
			pApp->SetCursor(::LoadCursor(NULL, IDC_ARROW));
			break;
			case GFxMouseCursorEvent::HAND:
			pApp->SetCursor(::LoadCursor(NULL, IDC_HAND));
			break;
			case GFxMouseCursorEvent::IBEAM:
			pApp->SetCursor(::LoadCursor(NULL, IDC_IBEAM));
			break;
			}
			*/
			break;
		}
	}

	return;    
}



// Helper used to convert key codes and route them to GFxPlayer
static void sGfxOnKey(unsigned key, unsigned info, bool down)
{
	Scaleform::Key::Code c((Scaleform::Key::Code)key);

	if (key >= 'A' && key <= 'Z')
	{
		c = (Scaleform::Key::Code) ((key - 'A') + Scaleform::Key::A);
	}
	else if (key >= VK_F1 && key <= VK_F15)
	{
		c = (Scaleform::Key::Code) ((key - VK_F1) + Scaleform::Key::F1);
	}
	else if (key >= VK_NUMPAD0 && key <= VK_NUMPAD9)
	{
		c = (Scaleform::Key::Code) ((key - VK_NUMPAD0) + Scaleform::Key::KP_0);
	}
	else
	{
		// many keys don't correlate, so just use a look-up table.
		static struct //!AB added static modifier
		{
			int         vk;
			Scaleform::Key::Code    gs;
		} table[] =
		{
			{ VK_RETURN,    Scaleform::Key::Return },
			{ VK_ESCAPE,    Scaleform::Key::Escape },
			{ VK_LEFT,      Scaleform::Key::Left },
			{ VK_UP,        Scaleform::Key::Up },
			{ VK_RIGHT,     Scaleform::Key::Right },
			{ VK_DOWN,      Scaleform::Key::Down },

			// @@ TODO fill this out some more
			{ 0, Scaleform::Key::None}
		};

		for (int i = 0; table[i].vk != 0; i++)
		{
			if (key == (unsigned)table[i].vk)
			{
				c = table[i].gs;
				break;
			}
		}
	}

	if (c != Scaleform::Key::None)
	{
		Scaleform::KeyModifiers keyMods;
		keyMods.SetShiftPressed((::GetKeyState(VK_SHIFT) & 0x8000) ? 1: 0);
		keyMods.SetCtrlPressed((::GetKeyState(VK_CONTROL) & 0x8000) ? 1: 0);
		keyMods.SetAltPressed((::GetKeyState(VK_MENU) & 0x8000) ? 1: 0);
		keyMods.SetNumToggled((::GetKeyState(VK_NUMLOCK) & 0x1) ? 1: 0);
		keyMods.SetCapsToggled((::GetKeyState(VK_CAPITAL) & 0x1) ? 1: 0);
		keyMods.SetScrollToggled((::GetKeyState(VK_SCROLL) & 0x1) ? 1: 0);

		Scaleform::GFx::KeyEvent keyevent(down ? Scaleform::GFx::Event::KeyDown : Scaleform::GFx::Event::KeyUp, c, 0, 0, keyMods);
		gAPIScaleformGfx->pKbdCaptureMovie->GetMovie()->HandleEvent(keyevent);
	}
}

static void sGfxOnChar(uint32_t wcharCode, uint32_t info)
{
	Scaleform::GFx::CharEvent charevent(wcharCode);
	gAPIScaleformGfx->pKbdCaptureMovie->GetMovie()->HandleEvent(charevent);
}

static void sGfxOnMouseMove(int x, int y, int z)
{
	Scaleform::GFx::MouseEvent mevent(Scaleform::GFx::MouseEvent::MouseWheel, 0, (float)x, (float)y, (float)((z/WHEEL_DELTA)*3), 0);
	gAPIScaleformGfx->pKbdCaptureMovie->GetMovie()->HandleEvent(mevent);
}


bool r3dScaleformGfxWinProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(gAPIScaleformGfx->pKbdCaptureMovie == NULL)
		return false;

	switch(uMsg) 
	{
	case WM_KEYDOWN:
		sGfxOnKey((unsigned)wParam, (unsigned)lParam, true);   
		break;

	case WM_KEYUP:          
		sGfxOnKey((unsigned)wParam, (unsigned)lParam, false);   
		break;

	case WM_CHAR:
		sGfxOnChar((unsigned)wParam, (unsigned)lParam);
		break;

	case WM_IME_SETCONTEXT:
		return true;

	case WM_MOUSEWHEEL:
		{
			int x, y;
			Mouse->GetXY(x, y);
			int z = (int(int16_t(HIWORD(wParam)))*128)/WHEEL_DELTA; // some weird scaleform math
			sGfxOnMouseMove(x, y, z);
			break;
		}
	}

	return false;
}

void r3dGfxOnDrawClipboardCallback(WPARAM wParam, LPARAM lParam)
{
	if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) 
		return; 
	if (!OpenClipboard(win::hWnd)) 
		return; 

	HGLOBAL hglb = GetClipboardData(CF_UNICODETEXT); 
	if (hglb != NULL) 
	{ 
		const wchar_t* wstr = (const wchar_t*)GlobalLock(hglb); 
		if (wstr != NULL) 
		{
			Scaleform::Ptr<Scaleform::GFx::TextClipboard> pclipBoard = gAPIScaleformGfx->loader.GetTextClipboard();
			if (pclipBoard)
				pclipBoard->SetText(wstr);
			GlobalUnlock(hglb); 
		} 
	} 
	CloseClipboard(); 
}

APIScaleformGfx::APIScaleformGfx( const r3dIntegrityGuardian& ig ) : 
r3dIResource( ig ),
pStateBlock(NULL),
pCurMovie(NULL),
pKbdCaptureMovie(NULL)
{
}

APIScaleformGfx::~APIScaleformGfx()
{
	r3d_assert(pStateBlock == NULL);
}

void APIScaleformGfx::D3DCreateResource()
{
	R3D_ENSURE_MAIN_THREAD();

	RendererHAL->RestoreAfterReset();

	// create state block
	D3D_V( r3dRenderer->pd3ddev->CreateStateBlock( D3DSBT_ALL, &pStateBlock ) );
}

void APIScaleformGfx::D3DReleaseResource()
{
	R3D_ENSURE_MAIN_THREAD();

	SAFE_RELEASE(pStateBlock);

	RendererHAL->PrepareForReset();
}

bool APIScaleformGfx::Create()
{
	r3d_assert(r3dRenderer);

	Scaleform::Ptr<r3dGFXFileOpener> fxFileOpener = *new  r3dGFXFileOpener();
	Scaleform::Ptr<r3dGFxFSCommandHandler> fxFSCommandHandler = *new  r3dGFxFSCommandHandler();
	Scaleform::Ptr<r3dGFxExternalInterface> fxEICommandHandler = *new  r3dGFxExternalInterface();
	Scaleform::Ptr<r3dGFxUserEventHandler> fxEventHandler = *new  r3dGFxUserEventHandler();
	//Scaleform::Ptr<r3dGFxImageCreator> fxImageCreator = *new r3dGFxImageCreator();
	Scaleform::Ptr<r3dGFxLog> fxLog = *new  r3dGFxLog();
	//Scaleform::Ptr<Scaleform::GFx::ASSupport> pAS2Support = *new  Scaleform::GFx::AS2Support();
	Scaleform::Ptr<Scaleform::GFx::ASSupport> pAS3Support = *new  Scaleform::GFx::AS3Support();
	Scaleform::Ptr<Scaleform::GFx::ImageFileHandlerRegistry> fxImageFileRegistry = *new  Scaleform::GFx::ImageFileHandlerRegistry();
	Scaleform::Ptr<r3dGFxTextClipboard> fxClipboard = *new r3dGFxTextClipboard();

	fxImageFileRegistry->AddHandler(&Scaleform::Render::JPEG::FileReader::Instance);
	fxImageFileRegistry->AddHandler(&Scaleform::Render::PNG::FileReader::Instance);
	fxImageFileRegistry->AddHandler(&Scaleform::Render::TGA::FileReader::Instance);
	fxImageFileRegistry->AddHandler(&Scaleform::Render::DDS::FileReader::Instance);
	loader.SetImageFileHandlerRegistry(fxImageFileRegistry);


	//loader.SetAS2Support(pAS2Support);
	loader.SetAS3Support(pAS3Support);
	loader.SetFileOpener(fxFileOpener); 
	loader.SetFSCommandHandler(fxFSCommandHandler);
	loader.SetExternalInterface(fxEICommandHandler);
	loader.SetUserEventHandler(fxEventHandler);
	//loader.SetImageCreator(fxImageCreator);
	loader.SetLog(fxLog);
	loader.SetTextClipboard(fxClipboard); 

	Scaleform::Ptr<TranslatorImpl> ptranslator = *new  TranslatorImpl();
	loader.SetTranslator(ptranslator);

	// for dynamic font caching, required for text effects to work
// 	GFxFontCacheManager::TextureConfig fontCacheConfig; 
// 	fontCacheConfig.TextureWidth   = 1024; 
// 	fontCacheConfig.TextureHeight  = 1024; 
// 	fontCacheConfig.MaxNumTextures = 6; // trying to increase, should help with a bug when stuck on a login page and UI doesn't show up
// 	fontCacheConfig.MaxSlotHeight  = 48; 
// 	fontCacheConfig.SlotPadding    = 2; 
// 	loader.GetFontCacheManager()->SetTextureConfig(fontCacheConfig); 
// 	loader.GetFontCacheManager()->EnableDynamicCache(true); 
// 	loader.GetFontCacheManager()->SetMaxRasterScale(1.2f);

	// load font library
	SetFontLib();

	// For D3D, it is good to override image creator to keep image data,
	// so that it can be restored in case of a lost device.
	Scaleform::Ptr<Scaleform::GFx::ImageCreator> fxImageCreator = *new Scaleform::GFx::ImageCreator();
	loader.SetImageCreator(fxImageCreator);

	// Create renderer.
	RendererHAL = new Scaleform::Render::D3D9::HAL();
	if(!(Renderer = new Scaleform::Render::Renderer2D(RendererHAL.GetPtr())))
	{
		r3dError("GFx: Failed to create Renderer2D\n");
		return false;
	}

	// create state block
	D3D_V( r3dRenderer->pd3ddev->CreateStateBlock( D3DSBT_ALL, &pStateBlock ) );

	Scaleform::Render::D3D9::HALInitParams initParams(r3dRenderer->pd3ddev, r3dRenderer->d3dpp, Scaleform::Render::D3D9::HALConfig_NoSceneCalls);
	RendererHAL->InitHAL(initParams);	

	//D3DCreateResource();

	// PT: fucks up loading of Cyrillic fonts (crash inside of scaleform)
	if(0)
	{
		Scaleform::Ptr<Scaleform::GFx::FontProviderWin32> fontProvider = *new Scaleform::GFx::FontProviderWin32(::GetDC(0));
		loader.SetFontProvider(fontProvider);
	}

	extern void RegisterMsgProc (Win32MsgProc_fn);
	RegisterMsgProc ( r3dScaleformGfxWinProc );

	/* ptumik: dont leave enabled, as RSUpdate will not remove this from code
#ifndef FINAL_BUILD
#ifdef SF_AMP_SERVER
	Scaleform::AmpServer::GetInstance().SetListeningPort(7534);
	Scaleform::AmpServer::GetInstance().SetConnectedApp("WarZ");
	Scaleform::AmpServer::GetInstance().OpenConnection();
#endif //SF_AMP_SERVER
#endif // FINAL_BUILD
	*/

	// load UI sounds
	sndMenuBtnClick = SoundSys.GetEventIDByPath("Sounds/MainMenu GUI/UI_menu_button_click");
	sndMenuBtnUse = SoundSys.GetEventIDByPath("Sounds/MainMenu GUI/UI_menu_button_use");
	sndMenuClose = SoundSys.GetEventIDByPath("Sounds/MainMenu GUI/UI_menu_close");
	sndMenuOpen = SoundSys.GetEventIDByPath("Sounds/MainMenu GUI/UI_menu_open");
	sndWeaponAttach = SoundSys.GetEventIDByPath("Sounds/MainMenu GUI/UI_weapon_attach");
	sndWeaponQuickChange = SoundSys.GetEventIDByPath("Sounds/MainMenu GUI/UI_weapon_quick_change");

	OnDrawClipboardCallback = r3dGfxOnDrawClipboardCallback;

	return true;    
}

void r3dScaleformForceReTranslation()
{
	if(gAPIScaleformGfx)
	{
		Scaleform::Ptr<TranslatorImpl> ptranslator = *new  TranslatorImpl();
		gAPIScaleformGfx->loader.SetTranslator(ptranslator);
	}
}

void APIScaleformGfx::SetFontLib()
{
	//TODO: later on, we can handle here loading different fonts for different languages if required

	// create font map and map fonts
	Scaleform::Ptr<Scaleform::GFx::FontMap> fontMap = *new  Scaleform::GFx::FontMap;
	loader.SetFontMap(fontMap);

	fontMap->MapFont("$InfFontPrimary", "Bebas Neue Cyrillic", Scaleform::GFx::FontMap::MFF_Normal);
	fontMap->MapFont("$InfFontSecondary", "Driver Gothic Pro", Scaleform::GFx::FontMap::MFF_Normal);
	fontMap->MapFont("$InfFontChat", "Tahoma", Scaleform::GFx::FontMap::MFF_Normal);

	Scaleform::Ptr<Scaleform::GFx::FontLib> fontLib = *new  Scaleform::GFx::FontLib;
	loader.SetFontLib(fontLib);

	// load file that holds actual font data
	Scaleform::Ptr<Scaleform::GFx::MovieDef> fontMovie = *loader.CreateMovie("data/menu/font_en.swf");
	fontLib->AddFontsFrom(fontMovie, true); // pin it, so that we don't need to keep track of that pointer

	// display all available fonts.
	/*Scaleform::StringHash<Scaleform::String> fontnames;
	fontLib->LoadFontNames(fontnames);

	r3dOutToLog("Loaded fonts:\n");
	for(Scaleform::StringHash<Scaleform::String>::Iterator it = fontnames.Begin(); it != fontnames.End(); ++it) 
	{
		r3dOutToLog(" '%s' = '%s'\n", (*it).First.ToCStr(), (*it).Second.ToCStr());
	}*/
}

void APIScaleformGfx::Destroy()
{

	R3D_ENSURE_MAIN_THREAD();
#ifndef FINAL_BUILD
#ifdef SF_AMP_SERVER
	Scaleform::AmpServer::GetInstance().CloseConnection();
#endif //SF_AMP_SERVER
#endif // FINAL_BUILD
	
	SAFE_RELEASE(pStateBlock);
	//D3DReleaseResource();

	OnDrawClipboardCallback = NULL;

	// release it here, because of static ptr
	RendererHAL->ShutdownHAL();
	RendererHAL = NULL;

	void UnregisterMsgProc (Win32MsgProc_fn);
	UnregisterMsgProc ( r3dScaleformGfxWinProc );

	return;
}

r3dScaleformMovie::r3dScaleformMovie() : 
pMovieDef(NULL),
pMovie(NULL),
movieH(0),
movieW(0)
{
	byteSize = 0;
	NumGfxEvents = 0;
	ConvertMouseCoords = 0;
}

r3dScaleformMovie::~r3dScaleformMovie()
{
	Unload();

	return;
}

void r3dScaleformMovie::Unload()
{
	R3D_ENSURE_MAIN_THREAD();

	if(!pMovie)
		return;

	SAFE_RELEASE(pMovieDef);
	SAFE_RELEASE(pMovie);

	if(gAPIScaleformGfx && gAPIScaleformGfx->pKbdCaptureMovie == this) {
		gAPIScaleformGfx->pKbdCaptureMovie = NULL;
	}

	for(int i=0; i<NumGfxEvents; i++)
	{
		SAFE_DELETE(EventHandlers[i].eiCallback) //-V599
	}
	NumGfxEvents = 0;

	r3dRenderer->Stats.AddUITexMem( -byteSize );
	byteSize = 0;

	return;
}

bool r3dScaleformMovie::Load(const char* fname, bool set_keyboard_focus)
{
	R3D_ENSURE_MAIN_THREAD();

	R3DPROFILE_FUNCTION("r3dScaleformMovie::Load");
	r3d_assert(gAPIScaleformGfx);
	r3d_assert(gAPIScaleformGfx->pStateBlock);

	Unload();

#ifndef FINAL_BUILD
	r3dOutToLog("Loading SWF %s\n", fname);
#endif

#ifndef FINAL_BUILD
	struct ReVisit : Scaleform::GFx::MovieDef::ResourceVisitor
	{
		ReVisit()
		{
			memcount = 0;
		}

		virtual void    Visit(	Scaleform::GFx::MovieDef* pmovieDef, Scaleform::GFx::Resource* presource,
								Scaleform::GFx::ResourceId rid, const char* pexportName ) OVERRIDE
		{
			if( presource->GetResourceType() == Scaleform::GFx::Resource::RT_Image )
			{
				Scaleform::GFx::ImageResource* imgRes = static_cast< Scaleform::GFx::ImageResource* >( presource );

				Scaleform::Render::ImageBase* img = imgRes->GetImage();

				int byteSize = Scaleform::Render::ImageData::GetMipLevelsSize( img->GetFormat(), img->GetSize(), img->GetMipmapCount() );

				memcount += byteSize;
			}
		}

		int memcount;

	} revisit ;
#endif

	if(!(pMovieDef = gAPIScaleformGfx->loader.CreateMovie(fname, Scaleform::GFx::Loader::LoadAll))) 
	{
		r3dOutToLog("r3dScaleformMovie::Load() '%s' load failed\n", fname);
		return false;
	}

#ifndef FINAL_BUILD
	pMovieDef->VisitResources( &revisit );
	byteSize = revisit.memcount;
	r3dRenderer->Stats.AddUITexMem( byteSize );
#endif

	if(!(pMovie = pMovieDef->CreateInstance(true))) 
	{
		r3dOutToLog("r3dScaleformMovie::Load() '%s' CreateInstance() failed\n", fname);		

		SAFE_RELEASE(pMovieDef);
		return false;
	}

	movieW = (int)pMovieDef->GetWidth();
	movieH = (int)pMovieDef->GetHeight();
	SetViewportTemp(0, 0, movieW, movieH, Scaleform::GFx::Movie::SM_NoScale, 0);

	// Background Alpha = 0 for transparent background,
	// and playback view spanning the entire window.
	pMovie->SetBackgroundAlpha(0.0f);

	hMovieDisplay = pMovie->GetDisplayHandle();

#ifndef FINAL_BUILD
	r3dOutToLog("r3dScaleformMovie::Load() '%s' ok\n", fname);
#endif

	timeForNextUpdate = r3dGetTime();
	timePrevUpdate = r3dGetTime();

	// and set keyboard capture to it, just in case
	if(set_keyboard_focus)
		SetKeyboardCapture();

	return true;
}

r3dScaleformMovie* r3dScaleformMovie::SetKeyboardCapture()
{
	r3dScaleformMovie* prevCapture = gAPIScaleformGfx->pKbdCaptureMovie;
	gAPIScaleformGfx->pKbdCaptureMovie = this;

	return prevCapture;
}

void r3dScaleformMovie::GetViewport(int* x, int* y, int* w, int* h) const
{
	if(x) *x = viewX;
	if(y) *y = viewY;
	if(w) *w = viewW;
	if(h) *h = viewH;

	return;
}

void r3dScaleformMovie::SetViewportTemp(int x, int y, int w, int h, Scaleform::GFx::Movie::ScaleModeType scaleType, int NeedConvertMouseCoords)
{
	ConvertMouseCoords = NeedConvertMouseCoords;
	viewX = x;
	viewY = y;
	viewW = w;
	viewH = h;
	pMovie->SetViewport(viewW, viewH, viewX, viewY, viewW, viewH, 0);
	pMovie->SetViewScaleMode(scaleType);
}

void r3dScaleformMovie::SetBackBufferViewport(Scaleform::GFx::Movie::ScaleModeType scaleType)
{
	float X, Y, W, H;

	r3dRenderer->GetBackBufferViewport( &X, &Y, &W, &H );

	SetViewportTemp( (int)X, (int)Y, (int)W, (int)H, scaleType, 0 );
}

void r3dScaleformMovie::SetCurentRTViewport(Scaleform::GFx::Movie::ScaleModeType scaleType)
{
	SetViewportTemp( 0, 0, (int)r3dRenderer->ScreenW, (int)r3dRenderer->ScreenH, scaleType, 1 );
}

#if 0
typedef r3dgameVector(r3dSTLString) stringlist_t;
stringlist_t currentMovies ;

typedef r3dgameVector( float ) floats ;
floats movieDurations ;
#endif

void UpdateImageTextureMatrix(const Scaleform::Render::ImageSize &origSize, const Scaleform::Render::ImageSize &newSize, Scaleform::Render::Image *pti)
{
	if (!pti)
		return;

	Scaleform::Render::Size<float> scaleParameters(((float)origSize.Width) / newSize.Width, ((float)origSize.Height) / newSize.Height);
	Scaleform::Render::Matrix2F textureMatrix = Scaleform::Render::Matrix2F::Scaling(scaleParameters.Width, scaleParameters.Height);
	Scaleform::Render::Matrix2F origMat;
	pti->GetMatrix(&origMat);
	origMat = origMat * textureMatrix;
	pti->SetMatrix(origMat);
}

Scaleform::Render::D3D9::Texture* r3dScaleformMovie::BoundRTToImage(const char* resName, LPDIRECT3DTEXTURE9 pRenderTarget, int RTWidth, int RTHeight)
{
	Scaleform::GFx::Resource*      pres = pMovieDef->GetResource(resName);
	Scaleform::GFx::ImageResource* pimageRes = 0;
	if (pres && pres->GetResourceType() == Scaleform::GFx::Resource::RT_Image)
		pimageRes = (Scaleform::GFx::ImageResource*)pres;


	Scaleform::Render::D3D9::Texture* pHWTexture = NULL;

	if (pimageRes)
	{
		Scaleform::Render::ImageBase* pimageOrig = pimageRes->GetImage();
		if (pimageOrig)
		{
			Scaleform::Render::D3D9::TextureManager* pmanager = (Scaleform::Render::D3D9::TextureManager*)gAPIScaleformGfx->RendererHAL->GetTextureManager();
			pHWTexture = (Scaleform::Render::D3D9::Texture*)pmanager->CreateTexture( pRenderTarget, Scaleform::Render::ImageSize(RTWidth, RTHeight));

			Scaleform::Ptr<Scaleform::Render::TextureImage> pti = * new Scaleform::Render::TextureImage(Scaleform::Render::Image_R8G8B8, pHWTexture->GetSize(), 0, pHWTexture);

			Scaleform::Render::Matrix2F m;
			pimageOrig->GetAsImage()->GetMatrix(&m);
			pti->SetMatrix(m);

			UpdateImageTextureMatrix(pimageOrig->GetSize(), Scaleform::Render::ImageSize(RTWidth, RTHeight), pti);
			pimageRes->SetImage(pti);
		}
	}

	return pHWTexture;
}

void r3dScaleformMovie::UpdateTextureMatrices(const char* resName, int RTWidth, int RTHeight)
{
	Scaleform::GFx::Resource*      pres = pMovieDef->GetResource(resName);
	Scaleform::GFx::ImageResource* pimageRes = 0;
	if (pres && pres->GetResourceType() == Scaleform::GFx::Resource::RT_Image)
		pimageRes = (Scaleform::GFx::ImageResource*)pres;

	Scaleform::Render::ImageBase* pimageOrig = pimageRes->GetImage();
	if (!pimageOrig)
		return;

	Scaleform::Render::Image *pti = pimageRes->GetImage()->GetAsImage();

	// If the texture is different size than the image it replaces, set the image matrix to their relative scales.
	Scaleform::Render::ImageSize originalSize = pimageOrig->GetSize();
	Scaleform::Render::ImageSize newSize(RTWidth, RTHeight);

	UpdateImageTextureMatrix(originalSize, newSize, pti);
}

float g_ScaleFormUpdateAndDraw ;
int g_ScaleFormUpdateAndDrawCount ;

void r3dScaleformMovie::UpdateAndDraw(bool skipDraw)
{
	R3DPROFILE_FUNCTION("r3dScaleformMovie::UpdateAndDraw");
	if(!pMovie)
		return;

#ifndef FINAL_BUILD
	float updateStart = r3dGetTime() ;
#endif

	gAPIScaleformGfx->pCurMovie = this;

	int mx, my, mb;
	Mouse->GetXY(mx, my);
	mb = Mouse->GetRawMouseData();

	// translate to local coords
	if( ConvertMouseCoords )
	{
		float VX, VY, VW, VH ;
		r3dRenderer->GetBackBufferViewport( &VX, &VY, &VW, &VH );

		mx = int( (mx - VX) / VW * viewW + 0.5f );
		my = int( (my - VY) / VH * viewH + 0.5f );
	}

	pMovie->NotifyMouseState(float(mx - viewX), float(my - viewY), mb);

	// Advance time
	// pt: we need to call Advance exactly at movie's frame rate, otherwise flash onInterval isn't working properly, or sometimes not working at all.
	float curTime = r3dGetTime();
	if(timeForNextUpdate < curTime)
	{
		float delta = curTime-timePrevUpdate;
		if(delta > (1.0f/pMovie->GetFrameRate()))
			delta = 1.0f/pMovie->GetFrameRate();
		timePrevUpdate = curTime;
		float timeTillNextTicks = pMovie->Advance(delta, 2, !skipDraw);
		timeForNextUpdate = curTime + timeTillNextTicks;
		if (timeForNextUpdate < curTime) // wrap-around check.
			timeForNextUpdate = curTime;
	}
	else
		pMovie->Advance(0.0f, 0, !skipDraw);

	if(!skipDraw)
	{
		if(gAPIScaleformGfx->pStateBlock)
			gAPIScaleformGfx->pStateBlock->Capture();

		r3dRenderer->Flush();

		LPDIRECT3DSURFACE9 pRT = NULL;
		LPDIRECT3DSURFACE9 pSS = NULL;
		r3dRenderer->GetRT(0, &pRT);
		r3dRenderer->GetDSS(&pSS);

		int refCountRT = pRT->AddRef(); refCountRT = pRT->Release();
		int refCountSS = pSS->AddRef(); refCountSS = pSS->Release();

		static Scaleform::Render::RenderTarget* gfxRT = new Scaleform::Render::RenderTarget(NULL, Scaleform::Render::RBuffer_User, Scaleform::Render::ImageSize((uint32_t)r3dRenderer->ScreenW, (uint32_t)r3dRenderer->ScreenH));
		static Scaleform::Render::DepthStencilBuffer* gfxDSB = new Scaleform::Render::DepthStencilBuffer(NULL, Scaleform::Render::ImageSize((uint32_t)r3dRenderer->ScreenW, (uint32_t)r3dRenderer->ScreenH));

		Scaleform::Render::D3D9::RenderTargetData::UpdateData(gfxRT, pRT, gfxDSB, pSS);

		Scaleform::Render::RenderTarget* defRT = gAPIScaleformGfx->RendererHAL->GetDefaultRenderTarget();

		uint32_t numPasses = (r3dRenderer->GetPresentEye() == R3D_STEREO_EYE_MONO ? 1 : 2);
		for (UINT i = 0; i < numPasses; ++i)
		{
			gAPIScaleformGfx->RendererHAL->SetRenderTarget(gfxRT, 1);
			if (numPasses > 1)
				r3dRenderer->SetEye(i == 0 ? R3D_STEREO_EYE_LEFT : R3D_STEREO_EYE_RIGHT);

			{
				gAPIScaleformGfx->Renderer->BeginFrame();
				if(hMovieDisplay.NextCapture(gAPIScaleformGfx->Renderer->GetContextNotify()))
				{
					gAPIScaleformGfx->Renderer->Display(hMovieDisplay);
				}
				gAPIScaleformGfx->Renderer->EndFrame();
			}

			gAPIScaleformGfx->RendererHAL->SetRenderTarget(defRT, 1);
		}

		if (numPasses > 1)
			r3dRenderer->SetEye(R3D_STEREO_EYE_MONO);

		Scaleform::Render::D3D9::RenderTargetData::UpdateData(gfxRT, NULL, gfxDSB, NULL);

		//SAFE_DELETE(gfxRT);
		//SAFE_DELETE(gfxDSB);

		int refCountRT1 = pRT->AddRef(); refCountRT1 = pRT->Release();
		int refCountSS1 = pSS->AddRef(); refCountSS1 = pSS->Release();

		SAFE_RELEASE(pRT);
		SAFE_RELEASE(pSS);

		if(gAPIScaleformGfx->pStateBlock)
			gAPIScaleformGfx->pStateBlock->Apply();
	}

	gAPIScaleformGfx->pCurMovie = NULL;

	//pMovie->ForceCollectGarbage();

#ifndef FINAL_BUILD
#ifdef SF_AMP_SERVER
	SF_AMP_CODE(Scaleform::AmpServer::GetInstance().AdvanceFrame());
#endif //SF_AMP_SERVER
#endif // FINAL_BUILD

#ifndef FINAL_BUILD
	float delta = r3dGetTime() - updateStart;

	g_ScaleFormUpdateAndDraw += delta;

	g_ScaleFormUpdateAndDrawCount ++;

	ScaleFormSample& sample = g_ScaleFormSamples[ pMovieDef->GetFileURL() ];

	sample.count ++;
	sample.time += delta;
#endif

	return;
}

BOOL r3dScaleformMovie::RegisterEventHandler(const char* EventString, void* data, fn_gfxEventHandler1 Fnc)
{
	if (NumGfxEvents > 254) return FALSE;

	EventHandlers[NumGfxEvents].EventName = EventString;
	EventHandlers[NumGfxEvents].data      = data;
	EventHandlers[NumGfxEvents].fnc1      = Fnc;
	NumGfxEvents ++;

	return TRUE;
}

BOOL r3dScaleformMovie::RegisterEventHandler(const char* EventString, void* data, fn_gfxEventHandler2 Fnc)
{
	if (NumGfxEvents > 254) return FALSE;

	EventHandlers[NumGfxEvents].EventName = EventString;
	EventHandlers[NumGfxEvents].data      = data;
	EventHandlers[NumGfxEvents].fnc2      = Fnc;
	NumGfxEvents ++;

	return TRUE;
}

BOOL r3dScaleformMovie::RegisterEventHandler(const char* EventString, sGFxEICallback* eiCallback)
{
	if (NumGfxEvents > 254) return FALSE;

	EventHandlers[NumGfxEvents].EventName = EventString;
	EventHandlers[NumGfxEvents].data      = NULL;
	EventHandlers[NumGfxEvents].eiCallback= eiCallback;
	NumGfxEvents ++;

	return TRUE;
}


void r3dScaleformMovie::OnCommandCallback(const char* command, const char* arg)
{
	for(int i = 0; i < NumGfxEvents; i++)
	{
		if(EventHandlers[i].EventName == command)
		{
#ifdef _DEBUG
			r3dOutToLog("fsCallback[%p]: '%s' -> '%s'\n", this, command, arg);
#endif
			r3d_assert(EventHandlers[i].fnc1);
			EventHandlers[i].fnc1(EventHandlers[i].data, this, arg);
			return;
		}
	}   

	r3dOutToLog("fsCallback[%p]: '%s' -> '%s' (unhandled)\n", this, command, arg);
}

void r3dScaleformMovie::OnCommandCallback(const char* methodName, const Scaleform::GFx::Value* args, unsigned argCount)
{
	if(strstr(methodName, "eventSoundPlay"))
	{
		r3d_assert(argCount==1);
		const char* soundName = args[0].GetString();
		if(strcmp(soundName, "menu_click")==0)
			SoundSys.PlayAndForget(gAPIScaleformGfx->sndMenuBtnClick, r3dPoint3D(0,0,0));
		else if(strcmp(soundName, "menu_use")==0)
			SoundSys.PlayAndForget(gAPIScaleformGfx->sndMenuBtnUse, r3dPoint3D(0,0,0));
		else if(strcmp(soundName, "menu_close")==0)
			SoundSys.PlayAndForget(gAPIScaleformGfx->sndMenuClose, r3dPoint3D(0,0,0));
		else if(strcmp(soundName, "menu_open")==0)
			SoundSys.PlayAndForget(gAPIScaleformGfx->sndMenuOpen, r3dPoint3D(0,0,0));
		else if(strcmp(soundName, "weapon_attach")==0)
			SoundSys.PlayAndForget(gAPIScaleformGfx->sndWeaponAttach, r3dPoint3D(0,0,0));
		else if(strcmp(soundName, "weapon_quick_change")==0)
			SoundSys.PlayAndForget(gAPIScaleformGfx->sndWeaponQuickChange, r3dPoint3D(0,0,0));

		return;
	}

	for(int i = 0; i < NumGfxEvents; i++)
	{
		if(EventHandlers[i].EventName == methodName)
		{
#ifdef _DEBUG
			r3dOutToLog("eventCallback[%p]: '%s'\n", this, methodName);
#endif
			if(EventHandlers[i].eiCallback)
			{
				EventHandlers[i].eiCallback->Execute(this, args, argCount);
			}
			else
			{
				r3d_assert(EventHandlers[i].fnc2);
				EventHandlers[i].fnc2(EventHandlers[i].data, this, args, argCount);
			}
			return;
		}
	}   

	r3dOutToLog("eventCallback[%p]: '%s' (unhandled)\n", this, methodName);
}

void r3dScaleformMovie::SetVariable(const char *Var, const char *Value)
{
	pMovie->SetVariable(Var, Value);
}

void r3dScaleformMovie::SetVariable(const char *Var, const wchar_t *Value)
{
	pMovie->SetVariable(Var, Value);
}

void r3dScaleformMovie::SetVariable(const char *Var, const float Value)
{
	Scaleform::GFx::Value arg;
	arg.SetNumber((double)Value);

	pMovie->SetVariable(Var, arg);
}

void r3dScaleformMovie::SetVariable(const char *Var, const int Value)
{
	Scaleform::GFx::Value arg;
	arg.SetNumber((double)Value);

	pMovie->SetVariable(Var, arg);
}



void r3dScaleformMovie::SetVariableArrayElement(const char* VarPath, unsigned int index, unsigned int count, const void* data)
{
	const char* temp = static_cast <const char*>(data);

	Scaleform::GFx::Value myVal;

	myVal.SetString(temp);

	pMovie->SetVariableArray(VarPath, index, &myVal, count);
}

#ifdef SF_AMP_SERVER
#define R3D_AMP_SCOPE_TIMER_ID(functionName)   Scaleform::AmpFunctionTimer _amp_timer_##functionId(Scaleform::AmpServer::GetInstance().GetDisplayStats(), (functionName))
#else
#define R3D_AMP_SCOPE_TIMER_ID(functionName)
#endif

#ifndef FINAL_BUILD

float g_ScaleFormCompositeInvoke ;
int g_ScaleFormInvokeCount ;

#define R3D_MARK_INVOKE_START float invokeStart_ = r3dGetTime() ; g_ScaleFormInvokeCount ++ ;
#define R3D_MARK_INVOKE_END g_ScaleFormCompositeInvoke += r3dGetTime() - invokeStart_ ;

#else

#define R3D_MARK_INVOKE_START
#define R3D_MARK_INVOKE_END

#endif

void r3dScaleformMovie::Invoke(const char *Var, const char *Value)
{
	if(!pMovie) return;

	R3D_MARK_INVOKE_START

	R3DPROFILE_FUNCTION( "Scaleform Invoke" ) ;

	Scaleform::GFx::Value args[1], result;
	args[0].SetString(Value);

	R3DPROFILE_START( Var ) ;
	R3D_AMP_SCOPE_TIMER_ID(Var);
	pMovie->Invoke(Var, &result, args, 1);
	R3DPROFILE_END( Var ) ;
	R3D_MARK_INVOKE_END
}

void r3dScaleformMovie::Invoke(const char *Var, const wchar_t *Value)
{
	if(!pMovie) return;
	R3D_MARK_INVOKE_START

	R3DPROFILE_FUNCTION( "Scaleform Invoke" ) ;

	Scaleform::GFx::Value args[1], result;
	args[0].SetStringW(Value);

	R3DPROFILE_START( Var ) ;
	R3D_AMP_SCOPE_TIMER_ID(Var);
	pMovie->Invoke(Var, &result, args, 1);
	R3DPROFILE_END( Var ) ;
	R3D_MARK_INVOKE_END
}

void r3dScaleformMovie::Invoke(const char *Var, float value)
{
	if(!pMovie) return;

	R3DPROFILE_FUNCTION( "Scaleform Invoke" ) ;
	R3D_MARK_INVOKE_START

	Scaleform::GFx::Value args[1], result;
	args[0].SetNumber((double)value);

	R3DPROFILE_START( Var ) ;
	R3D_AMP_SCOPE_TIMER_ID(Var);
	pMovie->Invoke(Var, &result, args, 1);
	R3DPROFILE_END( Var ) ;
	R3D_MARK_INVOKE_END
}

void r3dScaleformMovie::Invoke(const char* pmethodName, const Scaleform::GFx::Value* pargs, uint32_t numArgs) 
{
	if(pMovie)
	{
		R3DPROFILE_FUNCTION( "Scaleform Invoke" ) ;
		R3D_MARK_INVOKE_START

		R3DPROFILE_START( pmethodName ) ;
		R3D_AMP_SCOPE_TIMER_ID(pmethodName);
		pMovie->Invoke(pmethodName, NULL, pargs, numArgs);
		R3DPROFILE_END( pmethodName ) ;
		R3D_MARK_INVOKE_END
	}
}

void r3dScaleformMovie::Invoke(const char* pmethodName, Scaleform::GFx::Value *presult, const Scaleform::GFx::Value* pargs, uint32_t numArgs) 
{
	if(pMovie)
	{
		R3DPROFILE_FUNCTION( "Scaleform Invoke" ) ;
		R3D_MARK_INVOKE_START

		R3DPROFILE_START( pmethodName ) ;
		R3D_AMP_SCOPE_TIMER_ID(pmethodName);
		pMovie->Invoke(pmethodName, presult, pargs, numArgs);
		R3DPROFILE_END( pmethodName ) ;
		R3D_MARK_INVOKE_END
	}
}
