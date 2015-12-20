#include	"r3dPCH.h"
#include	"r3d.h"

#include "../SF/RenderBuffer.h"

#include "r3dDeviceQueue.h"

#include "MeshGlobalBuffer.h"

MeshGlobalBuffer managedBuffer;

void MeshGlobalBuffer::Entry::Set()
{
	d3dc._SetStreamSource( 0, VB.Get(), vbOffset, vbStride );
	d3dc._SetIndices( IB.Get() );
}

void MeshGlobalBuffer::Entry::Init(int numVertices, int numIndices, int sizeofVertex, int sizeofIndex)
{
	MeshGlobalBuffer::instance->GetEntry(*this, numVertices, numIndices, sizeofVertex, sizeofIndex);
}

struct InitVBIBParams
{
	MeshGlobalBuffer::Entry* entry ;

	r3dVertexBuffer* vbuf ;
	r3dIndexBuffer* ibuf ;
};

void MeshGlobalBuffer::Entry::InitVBIB( void* params )
{
	InitVBIBParams* ivibParams = (InitVBIBParams*)params ;

	VB.Set( ivibParams->vbuf->GetResource() ) ;
	IB.Set( ivibParams->ibuf->GetResource() ) ;
}

void SetEntryIBVBMainThread( void * params )
{
	InitVBIBParams* ivibParams = (InitVBIBParams*)params ;
	ivibParams->entry->InitVBIB( params ) ;
}

void MeshGlobalBuffer::Entry::InitUnshared( r3dVertexBuffer* vbuf, r3dIndexBuffer* ibuf )
{
	InitVBIBParams ivibParms ;

	ivibParms.entry = this ;
	ivibParms.ibuf = ibuf ;
	ivibParms.vbuf = vbuf ;

	ProcessCustomDeviceQueueItem( SetEntryIBVBMainThread, &ivibParms ) ;

	VBId			= - 1 ;

	vbOffset		= 0 ;
	vbStride		= vbuf->GetItemSize() ;

	vCount			= vbuf->GetItemCount() ;
	startIndex		= 0 ;

	ibOffset		= 0 ;
	ibStride		= ibuf->GetItemSize() ;

	iCount			= ibuf->GetItemCount() ;
	minVertexIndex	= 0 ;
}

void MeshGlobalBuffer::Entry::Lock(void*& vertices, void*& indices)
{
	VB.Lock(vbOffset, vCount * vbStride, &vertices, 0);
	IB.Lock(ibOffset, iCount * ibStride, &indices, 0);
}

void MeshGlobalBuffer::Entry::Unlock()
{
	VB.Unlock();
	IB.Unlock();
}

MeshGlobalBuffer::Buffers::Buffers()
{
	vbCursor = 0;
	ibCursor = 0;
}

void MeshGlobalBuffer::Buffers::release()
{
	if(VB.Valid())
	{
		r3dRenderer->Stats.AddBufferMem (- (int)( vbSize ) ); 
	}

	if( IB.Valid() )
	{
		r3dRenderer->Stats.AddBufferMem (- (int)( ibSize ) ); 
	}

	VB.ReleaseAndReset();
	IB.ReleaseAndReset();
	vbCursor = 0;
	ibCursor = 0;
	
}

void SetD3DResourcePrivateData(LPDIRECT3DRESOURCE9 res, const char* FName);

void MeshGlobalBuffer::Buffers::allocIB()
{
	IB.ReleaseAndReset();

	r3dDeviceTunnel::CreateIndexBuffer( ibSize, 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &IB );

	ibCursor = 0;
	r3dRenderer->Stats.AddBufferMem ( +ibSize );

	r3dDeviceTunnel::SetD3DResourcePrivateData( &IB, "Global Mesh Index Buffer" );
}

void MeshGlobalBuffer::Buffers::allocVB()
{
	VB.ReleaseAndReset() ;

	r3dDeviceTunnel::CreateVertexBuffer(vbSize, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &VB );
	
	vbCursor = 0;
	r3dRenderer->Stats.AddBufferMem ( +vbSize );
	
	r3dDeviceTunnel::SetD3DResourcePrivateData( &VB, "Global Mesh Vertex Buffer");
}

MeshGlobalBuffer* MeshGlobalBuffer::instance = &managedBuffer;

void MeshGlobalBuffer::unloadManaged()
{
	managedBuffer.Clear();
}

void MeshGlobalBuffer::Clear()
{
	for (int i = 0; i < maxBuffersCount; ++i)
	{
		buffers[i].release();
	}

	currentVB = 0 ;
	currentIB = 0 ;
}

void MeshGlobalBuffer::GetEntry(Entry& entry, int numVertices, int numIndices, int sizeofVertex, int sizeofIndex)
{
	int vbMemSize = numVertices*sizeofVertex;
	int ibMemSize = numIndices*sizeofIndex;

	Buffers* currentVBBuf = &buffers[currentVB];
	if( !currentVBBuf->VB.Valid() )
		currentVBBuf->allocVB();

	int vbMemLeft = Buffers::vbSize - currentVBBuf->vbCursor;
	if(vbMemLeft < vbMemSize)
	{
		++currentVB;
		currentVBBuf = &buffers[currentVB];
		currentVBBuf->allocVB();
	}

	Buffers* currentIBBuf = &buffers[currentIB];
	if( !currentIBBuf->IB.Valid() )
		currentIBBuf->allocIB();

	int ibMemLeft = Buffers::ibSize - currentIBBuf->ibCursor;
	if(ibMemLeft < ibMemSize)
	{
		++currentIB;
		currentIBBuf = &buffers[currentIB];
		currentIBBuf->allocIB();
	}

	entry.IB = currentIBBuf->IB;
	entry.VB = currentVBBuf->VB;

	entry.VBId = currentVB ;

	entry.iCount = numIndices;	
	entry.vCount = numVertices;

	entry.ibStride = sizeofIndex;
	entry.vbStride = sizeofVertex;
	
	entry.ibOffset = currentIBBuf->ibCursor;
	entry.vbOffset = currentVBBuf->vbCursor;

	entry.startIndex = currentIBBuf->ibCursor / sizeofIndex;
	entry.minVertexIndex = 0;


	currentIBBuf->ibCursor += numIndices*sizeofIndex;
	currentVBBuf->vbCursor += numVertices*sizeofVertex; 
}

MeshGlobalBuffer::MeshGlobalBuffer()
: currentVB( 0 )
, currentIB( 0 )
{

}