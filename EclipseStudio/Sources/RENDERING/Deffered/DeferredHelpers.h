#ifndef DEFERREDHELPERS_H_INCLUDED
#define DEFERREDHELPERS_H_INCLUDED

void DrawDeferredBox3D ( float x, float y, float z, float w, float h, float d, const r3dColor24& clr, bool wireframe );
void DrawDeferredBox3D ( r3dBox3D BBox, const r3dColor24& clr, bool wireframe = true );

#endif