//=========================================================================
//	Module: r3dEternityWebBrowser.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

#if ENABLE_WEB_BROWSER

namespace Berkelium
{
	class Context;
	class Window;
}

//////////////////////////////////////////////////////////////////////////

class MyBerkeliumDelegate;

/**	This class is manages all loading and off screen rendering of web pages. */
class EternityWebBrowser : public r3dIResource
{
public:
	friend bool r3dBrowserWinProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	friend class MyBerkeliumDelegate;
	friend struct URLHandlerSearcher;

	/**	Specific url handler type. */
	typedef void (*FnHandlerType)(const char *url);

	EternityWebBrowser();
	~EternityWebBrowser();

	/**	Initialize browser, set message handler. */
	void Init();

	/**	Deinitialize browser window. */
	void Destroy();

	/**	Restart browser. */
	void Restart();
	
	/**	Set web page size in pixels. */
	void SetSize(int width, int height);

	/**	Get browser window sizes. */
	void GetSize(int &width, int &height);

	/**	Load given URL. */
	void SetURL(const char *url);

	/**	Return url set by SetURL. */
	const char * GetURL() const { return lastURL.c_str(); }

	/**	Update internal state of web windows. */
	void Update();

	/**	Get texture pointer with browser content. */
	r3dTexture* GetWindow();

	/**	Set screen relative browser window position. This values used only for input coordinates transformation, there is no rendering influence. */
	void NotifyWindowPos(int screenX, int screenY, float scaleX, float scaleY);

	/**	Test browser drawing for testing purposes. */
	void DebugBrowserDraw();

	/**	Set specific url prefix handle. */
	void SetURLHandler(const char *urlPrefix, FnHandlerType handler);

	/**	Clear url handler. */
	void ClearURLHandler(const char *urlPrefix);

	virtual	void		D3DCreateResource()  OVERRIDE ;
	virtual	void		D3DReleaseResource() OVERRIDE ;

private:

	/**	Berkelium context, that is required to web windows creation. */
	Berkelium::Context *context;

	/**	Off screen web page instance. */
	Berkelium::Window *wnd;

	/**	
	* Delegate class that used for input communication between application and web page.
	* Also off screen surfaces copied into render target, reacting on appropriate callback.
	*/
	MyBerkeliumDelegate *dlg;

	/**	Texture that is used to draw web page content into. */
	r3dTexture* tex;
	int			texWidth ;
	int			texHeight ;

	/**	Web browser window offset. */
	int windowPosX;
	int windowPosY;
	// used for scaleform, as scaleform resizes texture inside of it, but berkelium still think that it is original size
	float windowScaleX;
	float windowScaleY;

	/**	Keyboard event. This are keys that are not character symbols. */
	void SendKeyEvent(unsigned int key, unsigned int info, bool down);

	/**	Send character symbols to the browser. */
	void SendCharEvent(uint32_t wcharCode, uint32_t info);

	/**	Send mouse wheel rotation information. */
	void SendMouseWheel(int delta);

	/**	Last navigated browser URL (URL set by SetURL function). */
	r3dSTLString lastURL;

	/**	Handler definition structure. */
	struct UrlHandler
	{
		/**	We store name hash to speedup searches. */
		uint32_t nameHash;

		/**	Handler function pointer. */
		FnHandlerType fn;

		UrlHandler(): nameHash(0), fn(0) {}
		UrlHandler(const char *name, FnHandlerType func)
		: nameHash(r3dHash::MakeHash(name))
		, fn(func)
		{}
	};

	/**	Array with handlers. */
	r3dTL::TArray<UrlHandler> urlHandlers;

	/**	Return url handler if one exists with this prefix, or null otherwise. */
	UrlHandler * FindURLHandler(const char *urlPrefix);
};
extern EternityWebBrowser* g_pBrowserManager;

//////////////////////////////////////////////////////////////////////////

#endif