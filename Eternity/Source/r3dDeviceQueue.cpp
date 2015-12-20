#include "r3dPCH.h"
#include "r3d.h"

#include "../sf/RenderBuffer.h"

#include "r3dDeviceQueue.h"

typedef r3dTL::TArray< DeviceQueueItem > DeviceQueueItems ;

#if R3D_PROFILE_LOADING
float gTotalTimeWaited = 0.f ;
r3dgfxMap( int, float ) waitMap  ;
#endif

static DeviceQueueItems gDeviceQueueItems ;
static CRITICAL_SECTION gDeviceQueueCS ;

// TODO : make it check things & make free work
class ContextRingBuffer
{
public:
	enum
	{
		CONTEXT_RING_BUFFER_SIZE = 2048 * 1024
	};

	ContextRingBuffer();
	~ContextRingBuffer();

	void Init();
	void Close();

	void* Alloc( size_t size );
	void Free( void* ptr );

private:
	char* mData ;
	size_t mPtr ;

} static gCRB;

DeviceQueueItem::DeviceQueueItem()
{
	memset( this, 0, sizeof *this );
}

void InitDeviceQueue()
{
	InitializeCriticalSection( &gDeviceQueueCS );
	gCRB.Init();
}

void CloseDeviceQueue()
{
	r3d_assert( !gDeviceQueueItems.Count() );
	DeleteCriticalSection( &gDeviceQueueCS ) ;
	gCRB.Close();
}

void BlockDeviceQueue()
{
	EnterCriticalSection( &gDeviceQueueCS ) ;
}

void UnblockDeviceQueue()
{
	LeaveCriticalSection( &gDeviceQueueCS ) ;
}

static void ProcessItem( const DeviceQueueItem& item )
{
	switch( item.Type )
	{
	case DeviceQueueItem::UNKNOWN:
		// someone forgot to fill!
		r3d_assert( false ) ;
		break ;


	case DeviceQueueItem::DEFAULT_POOL_RESOURCE_REGISTER:
		r3dRenderer->RegisterResource( item.DefPoolRes.Resource );
		break ;

	case DeviceQueueItem::DEFAULT_POOL_RESOURCE_CREATE:
		item.DefPoolRes.Resource->D3DCreateResource();
		break ;

	case DeviceQueueItem::CUSTOM:
		item.Custom.Func( item.Custom.Param );
		break ;
	}

	if( item.ReadyPtr )
	{
		*item.ReadyPtr = 1 ;
	}
}


//------------------------------------------------------------------------

void AddDeviceQueueItem( const DeviceQueueItem& item )
{
	if( R3D_IS_MAIN_THREAD() )
	{
		ProcessItem( item );
	}
	else
	{
		r3dCSHolder block( gDeviceQueueCS ) ;

		if( item.ReadyPtr )
			*item.ReadyPtr = 0 ;

		gDeviceQueueItems.PushBack( item ) ;
	}
}

void ProcessDeviceQueueItem( const DeviceQueueItem& item )
{
	DeviceQueueItem dqi = item ;

	volatile int Ready ;

	if( !dqi.ReadyPtr )
		dqi.ReadyPtr = &Ready ;

	AddDeviceQueueItem( dqi ) ;

#if R3D_PROFILE_LOADING
	float t = r3dGetTime();
#endif

	for( ; !*dqi.ReadyPtr; );

#if R3D_PROFILE_LOADING
	gTotalTimeWaited += r3dGetTime() - t ;
#endif
}

void AddCustomDeviceQueueItem( void (*func)( void *), void* param )
{
	DeviceQueueItem dqi ;

	dqi.Type = DeviceQueueItem::CUSTOM ;
	dqi.Custom.Func = func ;
	dqi.Custom.Param = param ;

	AddDeviceQueueItem( dqi ) ;
}

void ProcessCustomDeviceQueueItem( void (*func)( void *), void* param )
{
	DeviceQueueItem dqi ;

	dqi.Type = DeviceQueueItem::CUSTOM ;
	dqi.Custom.Func = func ;
	dqi.Custom.Param = param ;

	volatile int Ready ;

	dqi.ReadyPtr = &Ready ;

	AddDeviceQueueItem( dqi ) ;

#if R3D_PROFILE_LOADING
	float t = r3dGetTime();
#endif

	for( ; !Ready ; )
		YieldProcessor();
		

#if R3D_PROFILE_LOADING
	float dt = r3dGetTime() - t ;
	gTotalTimeWaited += dt ;

	waitMap[ (int)Func ] += dt ;
#endif
}

bool ProcessDeviceQueue( float chunkTimeStart, float maxDuration )
{
	R3DPROFILE_FUNCTION("ProcessDeviceQueue");
	r3dCSHolder block( gDeviceQueueCS ) ;

	size_t i = 0, e = gDeviceQueueItems.Count()  ;
	for( ; i < e ; i ++ )
	{
		if( r3dGetTime() - chunkTimeStart > maxDuration )
			break ;

		ProcessItem( gDeviceQueueItems[ i ] );
	}

#ifndef FINAL_BUILD
	if( g_async_loading->GetInt() )
	{
		if( r3dGetTime() - chunkTimeStart > maxDuration )
		{
			r3dOutToLog( "Device queue too long: %.2f (%d items)\n", r3dGetTime() - chunkTimeStart, e ) ;
		}
	}
#endif

	bool active = gDeviceQueueItems.Count() ? true : false ;

	gDeviceQueueItems.Erase( 0, i ) ;

	return active ;
}

void CreateQueuedResource( r3dIResource* res ) 
{
	if( R3D_IS_MAIN_THREAD() )
	{
		res->D3DCreateResource();
	}
	else
	{
		DeviceQueueItem dqi ;

		volatile int Ready ;

		dqi.Type = DeviceQueueItem::DEFAULT_POOL_RESOURCE_CREATE ;
		dqi.DefPoolRes.Resource = res ;
		dqi.ReadyPtr = &Ready ;

		AddDeviceQueueItem( dqi ) ;

#if R3D_PROFILE_LOADING
		float t = r3dGetTime();
#endif

		for( ; !Ready; )
			YieldProcessor();

#if R3D_PROFILE_LOADING
		gTotalTimeWaited += r3dGetTime() - t ;
#endif
	}
}

void ReleaseQueuedResource( r3dIResource* res )
{
	R3D_ENSURE_MAIN_THREAD();

	res->D3DReleaseResource();
}

//------------------------------------------------------------------------

ContextRingBuffer::ContextRingBuffer()
: mData( 0 )
, mPtr( 0 )
{
}

ContextRingBuffer::~ContextRingBuffer()
{

}

//------------------------------------------------------------------------

void ContextRingBuffer::Init()
{
	if( g_async_d3dqueue->GetInt() )
	{
		mData = (char*)malloc( CONTEXT_RING_BUFFER_SIZE ) ;
	}
}

//------------------------------------------------------------------------

void ContextRingBuffer::Close()
{
	free( mData ) ;
}

//------------------------------------------------------------------------

void* ContextRingBuffer::Alloc( size_t size )
{
	r3d_assert( mPtr ) ;

	if( mPtr + size > CONTEXT_RING_BUFFER_SIZE )
		mPtr = 0 ;

	char* res = mData + mPtr ;

	mPtr += size ;

	return res ;
}

void ContextRingBuffer::Free( void* ptr )
{
	// TODO 
}


//------------------------------------------------------------------------

void* AllocContextMem( size_t size )
{
	return gCRB.Alloc( size ) ;
}

void FreeContextMem( void * ptr )
{
	gCRB.Free( ptr ) ;
}
