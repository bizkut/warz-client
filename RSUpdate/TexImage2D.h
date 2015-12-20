#pragma once

class r3dTexImage2D
{
  public:
	float		u1_, v1_;	// coordinates of end of image insite big texture
	r3dTexture*	tex_;

  public:
	r3dTexImage2D(int resourceId, int w, int h);
	void		draw(float x, float y, float w, float h, const r3dColor& clr);
};