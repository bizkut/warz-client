#pragma once

void GenerateEnvmap( r3dTexture* tex, const r3dString& texName, const r3dPoint3D& pos );
void GenerateEnvmap64F( r3dTexture* tex, const r3dString& texName, const r3dPoint3D& pos );

enum GenEnvmapSequence
{
	GENENVMAP_FACE0,
	GENENVMAP_FACE1,
	GENENVMAP_FACE2,
	GENENVMAP_FACE3,
	GENENVMAP_FACE4,
	GENENVMAP_FACE5,
	GENENVMAP_STITCH
};

enum GenEnvmapSubSequence
{
	GENENVMAPSUB_PREPARE,
	GENENVMAPSUB_TERRAIN,
	GENENVMAPSUB_OBJECTS,
	GENENVMAPSUB_FINALIZE,
	GENENVMAPSUB_COUNT
};

int GenerateEnvmapDynamic( r3dScreenBuffer* cube, const r3dPoint3D& pos, GenEnvmapSequence step, GenEnvmapSubSequence subStep, int* ioObjectPosition );