#ifndef LEVEL_COLLECTIONS_H
#define LEVEL_COLLECTIONS_H

#include "r3dPCH.h"
#include "r3d.h"
#include "r3dLight.h"
#include "r3dUtils.h"
#include "d3dfont.h"
#include "GameCommon.h"
//#include "Editors\Tool_AxisControl.h"
#include "LevelEditor.h"
#include "CollectionElement.h"
    
// grid
float terra_GetH( const r3dPoint3D &vPos );
int terra_IsWithinPreciseHeightmap( const r3dPoint3D &vPos );
int terra_IsWithinPreciseHeightmap( const r3dBoundBox &bb );
float GetMinTerrainHeight(const r3dBoundBox &bb);
 
int GetRandomActiveCollectionType();
r3dPoint3D GetRandomPointInRadius(float r, const r3dPoint3D &center);
r3dMaterial* Get_Material_By_Ray(const r3dPoint3D& vStart, const r3dPoint3D& vRay, float& dist);

// in GUI code
int imgui_Checkbox_Small(float x, float y,int wid, const char* name, int* edit_val, const DWORD flag, bool bUseDesctop = true );

void DrawCollectionBrush();

void Do_Collection_Editor_Init( float levelProgressStart, float levelProgressEnd );
void ShutdownCollections();

#endif // LEVEL_COLLECTIONS_H
