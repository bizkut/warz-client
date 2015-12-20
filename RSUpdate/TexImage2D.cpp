#include "r3dPCH.h"
#include "r3d.h"

#include "TexImage2D.h"
#include "CkByteData.h"

bool GetResource(const char* name, const char* type, CkByteData& out_data)
{
  out_data.clear();

  HRSRC   src  = FindResource(NULL, name, type);
  if(src == NULL)
    return false;
  HGLOBAL res  = LoadResource(NULL, src);
  void*   data = LockResource(res);
  if(data == NULL)
    return false;
  DWORD   size = SizeofResource(NULL, src);
  
  out_data.borrowData((BYTE*)data, size);
  return true;
}


r3dTexImage2D::r3dTexImage2D(int resourceId, int w, int h)
{
  tex_ = NULL;
  u1_  = 0;
  v1_  = 0;
  
  CkByteData data;
  if(!GetResource(MAKEINTRESOURCE(resourceId), "IMAGE", data))
    return;
    
  IDirect3DTexture9* d3dtex;
  if(D3DXCreateTextureFromFileInMemory(
    r3dRenderer->pd3ddev, 
    data.getBytes(), 
    data.getSize(), 
    &d3dtex) != D3D_OK)
  {
    return;
  }
  
  tex_ = r3dRenderer->AllocateTexture();
  tex_->Create(4, 4, D3DFMT_X8R8G8B8, 1);
  tex_->SetNewD3DTexture(d3dtex);
  
  D3DSURFACE_DESC desc;
  d3dtex->GetLevelDesc(0, &desc);

  u1_   = (float)w / (float)desc.Width;
  v1_   = (float)h / (float)desc.Height;
  return;
}

void r3dTexImage2D::draw(float x, float y, float w, float h, const r3dColor& clr)
{
  if(tex_ == NULL)
    return;

  float TC[16] = {0};
  TC[0] = 0; 		TC[1] = 0;
  TC[2] = u1_;		TC[3] = 0;
  TC[4] = u1_;		TC[5] = v1_;
  TC[6] = 0; 		TC[7] = v1_;
  r3dDrawBox2D(x, y, w, h, clr, tex_, TC);
}