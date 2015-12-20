//=========================================================================
//	Module: r3dEternityWebBrowser.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#if ENABLE_WEB_BROWSER
//#define BERKELIM_EXTENDED_LOG 1

#include <berkelium\Berkelium.hpp>
#include <berkelium\Window.hpp>
#include <berkelium\Context.hpp>
#include <berkelium\WindowDelegate.hpp>

#include "r3dEternityWebBrowser.h"

#pragma comment(lib, "../external/berkelium/lib/berkelium.lib")

//////////////////////////////////////////////////////////////////////////

EternityWebBrowser* g_pBrowserManager = 0;
using namespace Berkelium;

//hack: extern var from eternity WinMain.cpp
typedef bool (*Win32MsgProc_fn)(UINT uMsg, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////////////

class MyBerkeliumDelegate: public Berkelium::WindowDelegate
{
public:
	r3dTexture *tex;
	const size_t pixelSize;

	MyBerkeliumDelegate()
	: tex(0)
	, pixelSize(4)
	{}

	void ScrollSubrect(void *data, const Berkelium::Rect& scroll_rect, int dx, int dy)
	{
		if (!data || !tex)
			return;

		byte *byteData = reinterpret_cast<byte*>(data);

		Berkelium::Rect dstRect = scroll_rect.translate(dx, dy);
		
		//	Y scroll
		if (dy != 0)
		{
			int lineOffset = dy < 0 ? 1 : -1;

			int srcLine = dy < 0 ? scroll_rect.top() : scroll_rect.bottom() - 1;
			int dstLine = dy < 0 ? dstRect.top() : dstRect.bottom() - 1;

			int i = 0;
			while (i < scroll_rect.height())
			{
				if (srcLine >= scroll_rect.top() && dstLine >= scroll_rect.top() && srcLine < scroll_rect.bottom() && dstLine < scroll_rect.bottom())
				{
					byte *srcMem = byteData + srcLine * tex->GetLockPitch() + scroll_rect.left() * pixelSize;
					byte *dstMem = byteData + dstLine * tex->GetLockPitch() + scroll_rect.left() * pixelSize;
					size_t bytesToCopy = scroll_rect.width() * pixelSize;

					#ifndef FINAL_BUILD
						//	Paranoid memory overrun check
						r3d_assert(dstMem + bytesToCopy <= byteData + tex->GetHeight() * tex->GetLockPitch());
					#endif

					memcpy_s(dstMem, bytesToCopy, srcMem, bytesToCopy);
				}

				++i;
				srcLine += lineOffset;
				dstLine += lineOffset;
			}
		}

		//	X Scroll
		if (dx != 0)
		{
			Berkelium::Rect r = scroll_rect.intersect(dstRect);

			size_t tmpBufSize = r.width() * pixelSize;
			byte *tmpBuf = reinterpret_cast<byte*>(_alloca(tmpBufSize));

			int srcOffset = r3dTL::Clamp(scroll_rect.left() - dx, 0, scroll_rect.right());

			for (int i = r.top(); i < r.bottom(); ++i)
			{
				byte *srcData = byteData + i * tex->GetLockPitch() + srcOffset * pixelSize;
				byte *dstData = byteData + i * tex->GetLockPitch() + r.left() * pixelSize;
				memcpy_s(tmpBuf, tmpBufSize, srcData, tmpBufSize);

				#ifndef FINAL_BUILD
					//	Paranoid memory overrun check
					r3d_assert(dstData + tmpBufSize <= byteData + tex->GetHeight() * tex->GetLockPitch());
				#endif

				memcpy_s(dstData, tmpBufSize, tmpBuf, tmpBufSize);
			}
		}
	}

	virtual void onPaint
	(
		Berkelium::Window* wini,
		const unsigned char *bitmap_in,
		const Berkelium::Rect &bitmap_rect,
		size_t num_copy_rects,
		const Berkelium::Rect* copy_rects,
		int dx,
		int dy,
		const Berkelium::Rect& scroll_rect
	)
	{
		if (!tex || (tex->GetFlags() & r3dTexture::fCreated) == 0 || !d_show_browser->GetBool()) return;

		void * dstBuf = tex->Lock(true);

		if (dstBuf)
		{
			ScrollSubrect(dstBuf, scroll_rect, dx, dy);

			for (size_t i = 0; i < num_copy_rects; ++i)
			{
				//	Calc bitmap_rect relative coordinates
				int height = copy_rects[i].height();
				int width = copy_rects[i].width();
				int left = copy_rects[i].left();
				int top = copy_rects[i].top();

				r3d_assert(bitmap_rect.left() <= left);
				r3d_assert(bitmap_rect.top() <= top);
				r3d_assert(bitmap_rect.width() >= width);
				r3d_assert(bitmap_rect.height() >= height);

				unsigned char *dstPtr = reinterpret_cast<byte*>(dstBuf) + left * pixelSize + top * tex->GetLockPitch();
				const unsigned char *srcPtr = bitmap_in + (left - bitmap_rect.left()) * pixelSize + (top - bitmap_rect.top()) * bitmap_rect.width() * pixelSize;
				
				for (int j = 0; j < height; ++j)
				{
					memcpy(dstPtr, srcPtr, width * pixelSize);
					dstPtr += tex->GetLockPitch();
					srcPtr += bitmap_rect.width() * pixelSize;
				}
			}
		}

		tex->Unlock();
	}
	virtual void onAddressBarChanged(Window *win, URLString newURL)
	{
#ifdef BERKELIM_EXTENDED_LOG
		r3dOutToLog("*** onAddressBarChanged %s\n", newURL.data());
#endif
    }
    virtual void onStartLoading(Window *win, URLString newURL)
	{
#ifdef BERKELIM_EXTENDED_LOG
		r3dOutToLog("*** onStartLoading %s\n", newURL.data());
#endif
		Berkelium::WideString ws = Berkelium::WideString::point_to(L"javascript:void(document.body.style.background = \"#2e3e4a\");");
		win->executeJavascript(ws);
    }
    virtual void onLoad(Window *win)
	{
#ifdef BERKELIM_EXTENDED_LOG
		r3dOutToLog("*** onLoad \n");
#endif
    }
    virtual void onCrashedWorker(Window *win)
	{
#ifdef BERKELIM_EXTENDED_LOG
		r3dOutToLog("*** onCrashedWorker \n");
#endif
    }
    virtual void onCrashedPlugin(Window *win, WideString pluginName)
	{
#ifdef BERKELIM_EXTENDED_LOG
		r3dOutToLog("*** onCrashedPlugin \n");
#endif
    }
    virtual void onProvisionalLoadError(Window *win, URLString url,
                                        int errorCode, bool isMainFrame)
	{
#ifdef BERKELIM_EXTENDED_LOG
		r3dOutToLog("*** onProvisionalLoadError %s\n", url.data());
#endif
    }
    virtual void onConsoleMessage(Window *win, WideString message,
                                  WideString sourceId, int line_no)
	{
    }

	virtual void onScriptAlert(Window *win, WideString message,
                              WideString defaultValue, URLString url,
                              int flags, bool &success, WideString &value)
	{
    }

	virtual void onNavigationRequested(Window *win, URLString newURL,
                                       URLString referrer, bool isNewWindow,
                                       bool &cancelDefaultAction)
	{
		if (isNewWindow) cancelDefaultAction = true;

		//	Parse prefix of new URL, and call appropriate handler
		const char *start = newURL.data();
		const char *end = newURL.data() + newURL.length();
		const char *colon = std::find(start, end, ':');

		if (colon != end)
		{
			char prefix[128] = {0};
			size_t colonOffset = colon - start;
			memcpy_s(prefix, (_countof(prefix) - 1) * sizeof(prefix[0]), start, colonOffset);
			EternityWebBrowser::UrlHandler *h = g_pBrowserManager->FindURLHandler(prefix);
			if (h && h->fn && newURL.length() > colonOffset + 3)
			{
				//	Send rest of the URL string to the handler
				h->fn(newURL.data() + colonOffset + 3);
			}
		}


#ifdef BERKELIM_EXTENDED_LOG
		r3dOutToLog("*** onNavigationRequested %s\n", newURL.data());
#endif
    }

    virtual void onLoadingStateChanged(Window *win, bool isLoading)
	{
#ifdef BERKELIM_EXTENDED_LOG
		r3dOutToLog("*** onLoadingStateChanged %s\n", isLoading ? "started" : "stopped");
#endif
    }

	virtual void onTitleChanged(Window *win, WideString title)
	{
    }
    
	virtual void onTooltipChanged(Window *win, WideString text)
	{
    }

	virtual void onCrashed(Window *win)
	{
		//	Try to restart berkelium
		g_pBrowserManager->Restart();

#ifdef BERKELIM_EXTENDED_LOG
		r3dOutToLog("*** onCrashed \n");
#endif
    }

    virtual void onUnresponsive(Window *win)
	{
#ifdef BERKELIM_EXTENDED_LOG
		r3dOutToLog("*** onUnresponsive \n");
#endif
    }

    virtual void onResponsive(Window *win)
	{
#ifdef BERKELIM_EXTENDED_LOG
		r3dOutToLog("*** onResponsive \n");
#endif
    }
    
	virtual void onCreatedWindow(Window *win, Window *newWindow,
                                 const Rect &initialRect)
	{
    }

	virtual void onWidgetCreated(Window *win, Widget *newWidget, int zIndex)
	{
    }
    
	virtual void onWidgetResize(Window *win, Widget *wid, int newWidth, int newHeight)
	{
    }

	virtual void onWidgetMove(Window *win, Widget *wid, int newX, int newY)
	{
    }

	virtual void onShowContextMenu(Window *win,
                                   const ContextMenuEventArgs& args)
	{
    }

    virtual void onJavascriptCallback(Window *win, void* replyMsg, URLString url, WideString funcName, Script::Variant *args, size_t numArgs)
	{
    }

    /** Display a file chooser dialog, if necessary. The value to be returned should go ______.
     * \param win  Window instance that fired this event.
     * \param mode  Type of file chooser expected. See FileChooserType.
     * \param title  Title for dialog. "Open" or "Save" should be used if empty.
     * \param defaultFile  Default file to select in dialog.
     */
    virtual void onRunFileChooser(Window *win, int mode, WideString title, FileString defaultFile) 
	{
    }

    virtual void onExternalHost(
        Berkelium::Window *win,
        Berkelium::WideString message,
        Berkelium::URLString origin,
        Berkelium::URLString target)
    {
    }

};

//////////////////////////////////////////////////////////////////////////

void UrlNavigateChangeCallback(int oldI, float oldF)
{
	g_pBrowserManager->SetURL(d_url_navigate->GetString());
}

//////////////////////////////////////////////////////////////////////////

bool r3dBrowserWinProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!g_pBrowserManager || !d_show_browser->GetBool())
		return false;

	switch(uMsg) 
	{
	case WM_KEYDOWN: 
		g_pBrowserManager->SendKeyEvent((unsigned int)wParam, (unsigned int)lParam, true);   
		break;

	case WM_KEYUP:          
		g_pBrowserManager->SendKeyEvent((unsigned int)wParam, (unsigned int)lParam, false);   
		break;

	case WM_CHAR:
		{
			unsigned int wcharCode = (unsigned int)wParam;
			g_pBrowserManager->SendCharEvent(wcharCode, (unsigned)lParam);
			break;
		}
	case WM_MOUSEWHEEL:
		{
			short dy = HIWORD(wParam);
			g_pBrowserManager->SendMouseWheel(dy);
		}
		break;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

void TestFN(const char *bb)
{
	if (bb)
	{
		r3dOutToLog(bb);
	}
}

//////////////////////////////////////////////////////////////////////////


EternityWebBrowser::EternityWebBrowser()
: r3dIResource( r3dIntegrityGuardian() )
, context(0)
, wnd(0)
, dlg(game_new MyBerkeliumDelegate)
, tex(r3dRenderer->AllocateTexture())
, windowPosX(0)
, windowPosY(0)
, windowScaleX(1)
, windowScaleY(1)
, texWidth( 0 )
, texHeight( 0 )
{
	Init();
	SetURLHandler("http", &TestFN);
}

//////////////////////////////////////////////////////////////////////////

EternityWebBrowser::~EternityWebBrowser()
{
	Destroy();
	delete dlg;
	r3dRenderer->DeleteTexture(tex);
}

//////////////////////////////////////////////////////////////////////////

void EternityWebBrowser::Init()
{
	r3d_assert(!wnd);
	r3d_assert(!context);

	if (!Berkelium::init(Berkelium::FileString::empty()))
	{
		r3d_assert(!"Couldn't initialize Berkelium.");
	}

	context = Berkelium::Context::create();
	wnd = Berkelium::Window::create(context);
	r3d_assert(wnd);
	dlg->tex = tex;
	wnd->setDelegate(dlg);

	d_url_navigate->SetChangeCallback(&UrlNavigateChangeCallback);

	void RegisterMsgProc (Win32MsgProc_fn);
	RegisterMsgProc(&r3dBrowserWinProc);
}

//////////////////////////////////////////////////////////////////////////

void EternityWebBrowser::Destroy()
{
	void UnregisterMsgProc (Win32MsgProc_fn);
	UnregisterMsgProc (&r3dBrowserWinProc);

	if (wnd)
	{
		wnd->destroy();
		wnd = 0;
	}
	if (context)
	{
		context->destroy();
		context = 0;
	}
	Berkelium::destroy();
}

//////////////////////////////////////////////////////////////////////////

void EternityWebBrowser::SetSize(int width, int height)
{
	if (width == 0 || height == 0)
		return;

	wnd->resize(width, height);

	if ( texWidth != width || texHeight != height)
	{
		D3DReleaseResource() ;

		texWidth = width ;
		texHeight = height ;

		D3DCreateResource() ;
	}

	wnd->focus();
}

//////////////////////////////////////////////////////////////////////////

void EternityWebBrowser::SetURL(const char *url)
{
	if (!wnd || !d_show_browser->GetBool() || !url)
		return;

	lastURL = url;
	wnd->navigateTo(url, strlen(url));
}

//////////////////////////////////////////////////////////////////////////

void EternityWebBrowser::Update()
{
	if (!wnd || !(tex->GetFlags() & r3dTexture::fCreated) || !d_show_browser->GetBool()) return;
	R3DPROFILE_FUNCTION("EternityWebBrowser::Update");

	//	Send input events to the web browser
	int x = 0, y = 0;
	Mouse->GetXY(x, y);
	x -= windowPosX;
	y -= windowPosY;
	x = int(ceilf(float(x)*windowScaleX));
	y = int(ceilf(float(y)*windowScaleY));

	wnd->mouseMoved(x, y);

	//	Check for pressed mouse buttons
	for (int i = 0; i < static_cast<int>(r3dMouse::mTotalButtons); ++i)
	{
		if (Mouse->WasPressed(i))
		{
			wnd->mouseButton(i, true);
		}
		if (Mouse->WasReleased(i))
		{
			wnd->mouseButton(i, false);
		}
	}

	Berkelium::update();
}

//////////////////////////////////////////////////////////////////////////

r3dTexture * EternityWebBrowser::GetWindow()
{
	return tex;
}

//////////////////////////////////////////////////////////////////////////

void EternityWebBrowser::NotifyWindowPos(int screenX, int screenY, float scaleX, float scaleY)
{
	windowPosY = screenY;
	windowPosX = screenX;
	windowScaleX = scaleX;
	windowScaleY = scaleY;
}

//////////////////////////////////////////////////////////////////////////

void EternityWebBrowser::DebugBrowserDraw()
{
	if (d_show_browser->GetBool())
	{
		if (!(tex->GetFlags() & r3dTexture::fCreated))
		{
			SetSize(1024, 512);
		}
		r3dDrawBox2D(100.0f, 100.0f, 1024.0f, 512.0f, r3dColor::white, tex);
		NotifyWindowPos(100, 100, 1.0f, 1.0f);
	}
}

//////////////////////////////////////////////////////////////////////////

void EternityWebBrowser::SendKeyEvent(unsigned int key, unsigned int info, bool down)
{
	if (!wnd || !d_show_browser->GetBool()) return;

	int keyMods = 0;
	if (::GetKeyState(VK_SHIFT) & 0x8000)
	{
		keyMods |= Berkelium::SHIFT_MOD;
	}
	if (::GetKeyState(VK_CONTROL) & 0x8000)
	{
		keyMods |= Berkelium::CONTROL_MOD;
	}
	if (::GetKeyState(VK_MENU) & 0x8000)
	{
		keyMods |= Berkelium::ALT_MOD;
	}

	int scanCode = HIWORD(info) & 0xff;
	wnd->keyEvent(down, keyMods, key, scanCode);
}

//////////////////////////////////////////////////////////////////////////

void EternityWebBrowser::SendCharEvent(uint32_t wcharCode, uint32_t info)
{
	if (!wnd || !d_show_browser->GetBool()) return;

	char ansiKey = static_cast<char>(wcharCode);
	wchar_t unicodeKey = 0;
	MultiByteToWideChar(CP_ACP, 0, &ansiKey, 1, &unicodeKey, 1);
	wnd->textEvent(&unicodeKey, 1);
}

//////////////////////////////////////////////////////////////////////////

void EternityWebBrowser::SendMouseWheel(int delta)
{
	if (!wnd || !d_show_browser->GetBool()) return;

	wnd->mouseWheel(0, delta);
}

//////////////////////////////////////////////////////////////////////////

void EternityWebBrowser::GetSize(int &width, int &height)
{
	if (!wnd || !(tex->GetFlags() & r3dTexture::fCreated))
	{
		width = 0;
		height = 0;
	}
	else
	{
		width = tex->GetWidth();
		height = tex->GetHeight();
	}
}

//////////////////////////////////////////////////////////////////////////

void EternityWebBrowser::Restart()
{
	Destroy();
	Init();

	int w = 0, h = 0;
	GetSize(w, h);
	SetSize(w, h);

	SetURL(GetURL());
}

//////////////////////////////////////////////////////////////////////////

void EternityWebBrowser::SetURLHandler(const char *urlPrefix, FnHandlerType handler)
{
	//	Try to find existing entry.
	UrlHandler *h = FindURLHandler(urlPrefix);

	if (h)
	{
		h->fn = handler;
	}
	else
	{
		UrlHandler newHandler(urlPrefix, handler);
		urlHandlers.PushBack(newHandler);
	}
}

//////////////////////////////////////////////////////////////////////////

void EternityWebBrowser::ClearURLHandler(const char *urlPrefix)
{
	UrlHandler *h = FindURLHandler(urlPrefix);
	if (h)
	{
		uint32_t idx = h - &urlHandlers.GetFirst();
		urlHandlers.Erase(idx);
	}
}

//////////////////////////////////////////////////////////////////////////
/*virtual*/

void
EternityWebBrowser::D3DCreateResource() /*OVERRIDE*/
{
	if( texWidth && texHeight )
	{
		tex->Create(texWidth, texHeight, D3DFMT_A8R8G8B8, 1, D3DPOOL_DEFAULT);

		//	Clear texture
		void *data = tex->Lock(true);
		::ZeroMemory(data, tex->GetLockPitch() * tex->GetHeight());
		tex->Unlock();
	}
}

//////////////////////////////////////////////////////////////////////////
/*virtual*/

void EternityWebBrowser::D3DReleaseResource() /*OVERRIDE*/
{
	if( tex->GetFlags() & r3dTexture::fCreated )
	{
		tex->Destroy() ;
	}
}


//////////////////////////////////////////////////////////////////////////

EternityWebBrowser::UrlHandler * EternityWebBrowser::FindURLHandler(const char *urlPrefix)
{
	/**	Custom url handler searcher. */
	struct URLHandlerSearcher
	{
		uint32_t hash;

		URLHandlerSearcher(): hash(0) {};
		explicit URLHandlerSearcher(const char *name): hash(r3dHash::MakeHash(name)) {}

		bool operator()(const EternityWebBrowser::UrlHandler &a)
		{
			return a.nameHash == hash;
		}
	};

	if (urlHandlers.Count() == 0)
		return 0;

	URLHandlerSearcher uhs(urlPrefix);
	UrlHandler *start = &urlHandlers.GetFirst();
	UrlHandler *end = &urlHandlers.GetFirst() + urlHandlers.Count();
	UrlHandler *h = std::find_if(start, end, uhs);

	if (h != end)
		return h;
	return 0;
}

//////////////////////////////////////////////////////////////////////////

#endif // ENABLE_WEB_BROWSER