#include "r3dPCH.h"
#include "r3d.h"

#include "pipeline.h"


r3dRenderPipeline::r3dRenderPipeline()
{
}

r3dRenderPipeline::~r3dRenderPipeline()
{
}

void r3dRenderPipeline::Init()
{
}

void r3dRenderPipeline::Close()
{
}

void r3dRenderPipeline::PreRender()
{
}

void r3dRenderPipeline::Render()
{
}

void r3dRenderPipeline::AppendPostFXes()
{

}

void r3dRenderPipeline::PostProcess()
{
}

//------------------------------------------------------------------------
/*virtual*/ void r3dRenderPipeline::RenderStage_Prepare()
{

}

//------------------------------------------------------------------------
/*virtual*/ void r3dRenderPipeline::RenderStage_Terrain()
{

}

//------------------------------------------------------------------------
/*virtual*/ void r3dRenderPipeline::RenderStage_Deferred( int startRenderable, int renderableCount )
{

}

//------------------------------------------------------------------------
/*virtual*/ void r3dRenderPipeline::RenderStage_Finilize( const D3DVIEWPORT9* viewport )
{

}

//------------------------------------------------------------------------
/*virtual*/ int r3dRenderPipeline::GetStagedRenderObjectCount()
{
	return 0;
}

r3dRenderPipeline* CurRenderPipeline;