#pragma once

class r3dVertexBuffer ;
class r3dIndexBuffer ;

struct DeviceQueueItem
{
	enum eType
	{
		UNKNOWN,
		DEFAULT_POOL_RESOURCE_REGISTER,
		DEFAULT_POOL_RESOURCE_CREATE,
		DEFAULT_POOL_RESOURCE_UNREGISTER,
		DEFAULT_POOL_RESOURCE_DESTROY,
		CUSTOM
	} Type;

	volatile int * ReadyPtr ;


	struct DefPoolResData
	{
		r3dIResource* Resource ;
	};

	struct CustomData
	{
		void (*Func) (void*) ;
		void* Param ;
	};

	union
	{
		DefPoolResData		DefPoolRes ;
		CustomData			Custom ;
	};

	DeviceQueueItem();
};

void InitDeviceQueue() ;
void CloseDeviceQueue() ;

void BlockDeviceQueue() ;
void UnblockDeviceQueue() ;

void AddDeviceQueueItem( const DeviceQueueItem& item ) ;

void ProcessDeviceQueueItem( const DeviceQueueItem& item ) ;

void AddCustomDeviceQueueItem( void (*func)( void *), void* param ) ;

// this one waits for processing to end and the above one doesn't
void ProcessCustomDeviceQueueItem( void (*func)( void *), void* param ) ;

bool ProcessDeviceQueue( float chunkTimeStart, float maxDuration ) ;

void CreateQueuedResource( r3dIResource* res ) ;
void ReleaseQueuedResource( r3dIResource* res ) ;

void* AllocContextMem( size_t size ) ;
void FreeContextMem( void * ptr ) ;

#define R3D_DEIVCE_QUEUE_OBJ(type,name) type name##_STACK ; type& name = g_async_d3dqueue->GetInt() ? *(type*)AllocContextMem( sizeof (type) ) : name##_STACK ;
