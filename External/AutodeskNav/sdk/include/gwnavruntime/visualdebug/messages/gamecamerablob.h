/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_GameCameraBlob_H
#define Navigation_GameCameraBlob_H

#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/visualdebug/visualdebugblobcategory.h"

namespace Kaim
{

class GameCameraBlob
{
	KY_ROOT_BLOB_CLASS(VisualDebugMessage, GameCameraBlob, 0)

public:
	GameCameraBlob() : m_fovInDegrees(45.0f) {}

	Vec3f m_eye;              // camera position
	Vec3f m_center;           // camera look at position
	Vec3f m_up;               // camera up vector
	KyFloat32 m_fovInDegrees; // viewport field of view angle in degree
};

inline void SwapEndianness(Endianness::Target e, GameCameraBlob& self)
{
	SwapEndianness(e, self.m_eye);
	SwapEndianness(e, self.m_center);
	SwapEndianness(e, self.m_up);
	SwapEndianness(e, self.m_fovInDegrees);
}

class GameCameraBlobBuilder : public BaseBlobBuilder< GameCameraBlob >
{
	KY_CLASS_WITHOUT_COPY(GameCameraBlobBuilder)

public:
	GameCameraBlobBuilder(const Vec3f& eye, const Vec3f& center, const Vec3f& up, KyFloat32 fov)
		: m_eye(eye)
		, m_center(center)
		, m_up(up)
		, m_fovInDegrees(fov)
	{}

private:
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_eye, m_eye);
		BLOB_SET(m_blob->m_center, m_center);
		BLOB_SET(m_blob->m_up, m_up);
		BLOB_SET(m_blob->m_fovInDegrees, m_fovInDegrees);
	}

	Vec3f m_eye;              // camera position
	Vec3f m_center;           // camera look at position
	Vec3f m_up;               // camera up vector
	KyFloat32 m_fovInDegrees; // viewport field of view angle in degree
};

class GameCameraAttitudeBlob
{
	KY_ROOT_BLOB_CLASS(VisualDebugMessage, GameCameraAttitudeBlob, 0)

public:
	GameCameraAttitudeBlob() : m_fovInDegrees(45.0f) {}

	Vec3f m_pos;              // camera position
	Vec3f m_right;            // camera right vector
	Vec3f m_up;               // camera up vector
	Vec3f m_front;            // camera front vector
	KyFloat32 m_fovInDegrees; // viewport field of view angle in degree
};

inline void SwapEndianness(Endianness::Target e, GameCameraAttitudeBlob& self)
{
	SwapEndianness(e, self.m_pos);
	SwapEndianness(e, self.m_right);
	SwapEndianness(e, self.m_up);
	SwapEndianness(e, self.m_front);
	SwapEndianness(e, self.m_fovInDegrees);
}

class GameCameraAttitudeBlobBuilder : public BaseBlobBuilder< GameCameraAttitudeBlob >
{
	KY_CLASS_WITHOUT_COPY(GameCameraAttitudeBlobBuilder)

public:
	GameCameraAttitudeBlobBuilder(const Vec3f& pos, const Vec3f& right, const Vec3f& up, const Vec3f& front, KyFloat32 fov)
		: m_pos(pos)
		, m_right(right)
		, m_up(up)
		, m_front(front)
		, m_fovInDegrees(fov)
	{}

private:
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_pos, m_pos);
		BLOB_SET(m_blob->m_right, m_right);
		BLOB_SET(m_blob->m_up, m_up);
		BLOB_SET(m_blob->m_front, m_front);
		BLOB_SET(m_blob->m_fovInDegrees, m_fovInDegrees);
	}

	Vec3f m_pos;              // camera position
	Vec3f m_right;            // camera right vector
	Vec3f m_up;               // camera up vector
	Vec3f m_front;            // camera front vector
	KyFloat32 m_fovInDegrees; // viewport field of view angle in degree
};

} // namespace Kaim

#endif // Navigation_LogBlob_H
