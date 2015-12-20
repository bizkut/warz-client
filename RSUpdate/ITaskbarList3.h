#pragma once

// copy from Win7 SDK ShObjIdl.h 

#ifndef __ITaskbarList3_INTERFACE_DEFINED__
#define __ITaskbarList3_INTERFACE_DEFINED__

typedef /* [v1_enum] */ 
enum THUMBBUTTONFLAGS
    {	THBF_ENABLED	= 0,
	THBF_DISABLED	= 0x1,
	THBF_DISMISSONCLICK	= 0x2,
	THBF_NOBACKGROUND	= 0x4,
	THBF_HIDDEN	= 0x8,
	THBF_NONINTERACTIVE	= 0x10
    } 	THUMBBUTTONFLAGS;

typedef /* [v1_enum] */ 
enum THUMBBUTTONMASK
    {	THB_BITMAP	= 0x1,
	THB_ICON	= 0x2,
	THB_TOOLTIP	= 0x4,
	THB_FLAGS	= 0x8
    } 	THUMBBUTTONMASK;

#include <pshpack8.h>
typedef struct THUMBBUTTON
    {
    THUMBBUTTONMASK dwMask;
    UINT iId;
    UINT iBitmap;
    HICON hIcon;
    WCHAR szTip[ 260 ];
    THUMBBUTTONFLAGS dwFlags;
    } 	THUMBBUTTON;

typedef struct THUMBBUTTON *LPTHUMBBUTTON;

/* interface ITaskbarList3 */
/* [object][uuid] */ 

typedef /* [v1_enum] */ 
enum TBPFLAG
    {	TBPF_NOPROGRESS	= 0,
	TBPF_INDETERMINATE	= 0x1,
	TBPF_NORMAL	= 0x2,
	TBPF_ERROR	= 0x4,
	TBPF_PAUSED	= 0x8
    } 	TBPFLAG;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ea1afb91-9e28-4b86-90e9-9e9f8a5eefaf")
    ITaskbarList3 : public ITaskbarList2
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetProgressValue( 
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ ULONGLONG ullCompleted,
            /* [in] */ ULONGLONG ullTotal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetProgressState( 
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ TBPFLAG tbpFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RegisterTab( 
            /* [in] */ __RPC__in HWND hwndTab,
            /* [in] */ __RPC__in HWND hwndMDI) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnregisterTab( 
            /* [in] */ __RPC__in HWND hwndTab) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTabOrder( 
            /* [in] */ __RPC__in HWND hwndTab,
            /* [in] */ __RPC__in HWND hwndInsertBefore) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTabActive( 
            /* [in] */ __RPC__in HWND hwndTab,
            /* [in] */ __RPC__in HWND hwndMDI,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ThumbBarAddButtons( 
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ UINT cButtons,
            /* [size_is][in] */ __RPC__in_ecount_full(cButtons) LPTHUMBBUTTON pButton) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ThumbBarUpdateButtons( 
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ UINT cButtons,
            /* [size_is][in] */ __RPC__in_ecount_full(cButtons) LPTHUMBBUTTON pButton) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ThumbBarSetImageList( 
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ __RPC__in_opt HIMAGELIST himl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetOverlayIcon( 
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ __RPC__in HICON hIcon,
            /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszDescription) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetThumbnailTooltip( 
            /* [in] */ __RPC__in HWND hwnd,
            /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszTip) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetThumbnailClip( 
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ __RPC__in RECT *prcClip) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITaskbarList3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in ITaskbarList3 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in ITaskbarList3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *HrInit )( 
            __RPC__in ITaskbarList3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddTab )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *DeleteTab )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ActivateTab )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *SetActiveAlt )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *MarkFullscreenWindow )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ BOOL fFullscreen);
        
        HRESULT ( STDMETHODCALLTYPE *SetProgressValue )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ ULONGLONG ullCompleted,
            /* [in] */ ULONGLONG ullTotal);
        
        HRESULT ( STDMETHODCALLTYPE *SetProgressState )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ TBPFLAG tbpFlags);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterTab )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in HWND hwndTab,
            /* [in] */ __RPC__in HWND hwndMDI);
        
        HRESULT ( STDMETHODCALLTYPE *UnregisterTab )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in HWND hwndTab);
        
        HRESULT ( STDMETHODCALLTYPE *SetTabOrder )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in HWND hwndTab,
            /* [in] */ __RPC__in HWND hwndInsertBefore);
        
        HRESULT ( STDMETHODCALLTYPE *SetTabActive )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in HWND hwndTab,
            /* [in] */ __RPC__in HWND hwndMDI,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *ThumbBarAddButtons )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ UINT cButtons,
            /* [size_is][in] */ __RPC__in_ecount_full(cButtons) LPTHUMBBUTTON pButton);
        
        HRESULT ( STDMETHODCALLTYPE *ThumbBarUpdateButtons )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ UINT cButtons,
            /* [size_is][in] */ __RPC__in_ecount_full(cButtons) LPTHUMBBUTTON pButton);
        
        HRESULT ( STDMETHODCALLTYPE *ThumbBarSetImageList )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ __RPC__in_opt HIMAGELIST himl);
        
        HRESULT ( STDMETHODCALLTYPE *SetOverlayIcon )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ __RPC__in HICON hIcon,
            /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszDescription);
        
        HRESULT ( STDMETHODCALLTYPE *SetThumbnailTooltip )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in HWND hwnd,
            /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszTip);
        
        HRESULT ( STDMETHODCALLTYPE *SetThumbnailClip )( 
            __RPC__in ITaskbarList3 * This,
            /* [in] */ __RPC__in HWND hwnd,
            /* [in] */ __RPC__in RECT *prcClip);
        
        END_INTERFACE
    } ITaskbarList3Vtbl;

    interface ITaskbarList3
    {
        CONST_VTBL struct ITaskbarList3Vtbl *lpVtbl;
    };

#endif
#endif
