/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: LASI
#ifndef Navigation_CoordSystem_H
#define Navigation_CoordSystem_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/math/vec2f.h"
#include "gwnavruntime/math/box3f.h"
#include "gwnavruntime/navdata/navdatablobcategory.h"
#include "gwnavruntime/math/matrix3x3f.h"
#include "gwnavruntime/math/transform.h"
#include "gwnavruntime/math/triangle3f.h"


namespace Kaim
{

class BlobFieldsMapping;
class BlobAggregate;


/// Enumerates the coordinate types understood by the CoordSystem.
enum Coordinates
{
	COORDINATES_CLIENT  = 0, ///< Identifies the coordinate system used in the game engine.
	COORDINATES_NAVIGATION = 1 ///< Identifies the coordinate system used in Gameware Navigation.
};


/// Enumerates the possible axis orientations that can be assigned to each of
/// the Right, Front, and Up axes used within Gameware Navigation. Used in calls
/// to CoordSystem::Setup() and CoordSystem::SetAxes().
enum CoordSystemClientAxis
{
	CLIENT_X       = 0, ///< Represents the positive direction of the X axis.
	CLIENT_MINUS_X = 1, ///< Represents the negative direction of the X axis.
	CLIENT_Y       = 2, ///< Represents the positive direction of the Y axis.
	CLIENT_MINUS_Y = 3, ///< Represents the negative direction of the Y axis.
	CLIENT_Z       = 4, ///< Represents the positive direction of the Z axis.
	CLIENT_MINUS_Z = 5  ///< Represents the negative direction of the Z axis.
};


/// A structure that sets up the mapping between the coordinate system used in
/// the Gameware Navigation Engine and the game engine.
class CoordSystemConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	CoordSystemConfig()
	{
		m_oneMeterInClientUnits = 1.0f;
		m_clientAxisForNavigationX = (KyUInt32)CLIENT_X;
		m_clientAxisForNavigationY = (KyUInt32)CLIENT_Y;
		m_clientAxisForNavigationZ = (KyUInt32)CLIENT_Z;
	}

	/// Initialize the CoordSystemConfig to use a standard Z-up coordinate system
	/// in Gameware Navigation.
	void Init_Zup(KyFloat32 oneMeterInClientUnits)
	{
		Init(oneMeterInClientUnits, CLIENT_X, CLIENT_Y, CLIENT_Z);
	}

	/// Sets up an instance of this class with any required components and configuration parameters.
	/// \param oneMeterInClientUnits		Sets the value of #m_oneMeterInClientUnits.
	/// \param clientAxisForNavigationX		Sets the value of #m_clientAxisForNavigationX.
	/// \param clientAxisForNavigationY		Sets the value of #m_clientAxisForNavigationY.
	/// \param clientAxisForNavigationZ		Sets the value of #m_clientAxisForNavigationZ. 
	void Init(
		KyFloat32 oneMeterInClientUnits,
		CoordSystemClientAxis clientAxisForNavigationX,
		CoordSystemClientAxis clientAxisForNavigationY,
		CoordSystemClientAxis clientAxisForNavigationZ)
	{
		m_oneMeterInClientUnits = oneMeterInClientUnits;
		m_clientAxisForNavigationX = (KyUInt32)clientAxisForNavigationX;
		m_clientAxisForNavigationY = (KyUInt32)clientAxisForNavigationY;
		m_clientAxisForNavigationZ = (KyUInt32)clientAxisForNavigationZ;
	}

	/// For internal use. 
	void AddMapping(BlobFieldsMapping& mapping);

	/// Returns true if all data members in the specified CoordSystemConfig match all data members in this object. 
	bool operator==(const CoordSystemConfig& other) const
	{
		return m_oneMeterInClientUnits == other.m_oneMeterInClientUnits
			&& m_clientAxisForNavigationX == other.m_clientAxisForNavigationX
			&& m_clientAxisForNavigationY == other.m_clientAxisForNavigationY
			&& m_clientAxisForNavigationZ == other.m_clientAxisForNavigationZ;
	}

	/// Returns true if at least one data member in the specified CoordSystemConfig does not match the corresponding
	/// data member in this object. 
	bool operator!=(const CoordSystemConfig& other) const { return !operator==(other); }

public:
	/// The number of world units in the game engine that add up to one meter in length. 
	KyFloat32 m_oneMeterInClientUnits;
	/// The axis in the game engine coordinate system that should be mapped to the X axis within the Gameware Navigation coordinate system. 
	KyUInt32 m_clientAxisForNavigationX;
	/// The axis in the game engine coordinate system that should be mapped to the Y axis within the Gameware Navigation coordinate system. 
	KyUInt32 m_clientAxisForNavigationY;
	/// The axis in the game engine coordinate system that should be mapped to the Z axis within the Gameware Navigation coordinate system. 
	KyUInt32 m_clientAxisForNavigationZ;
};


/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.
inline void SwapEndianness(Endianness::Target e, CoordSystemConfig& self)
{
	SwapEndianness(e, self.m_oneMeterInClientUnits);
	SwapEndianness(e, self.m_clientAxisForNavigationX);
	SwapEndianness(e, self.m_clientAxisForNavigationY);
	SwapEndianness(e, self.m_clientAxisForNavigationZ);
}


/// This class maintains the mapping between the system of coordinate axes used internally within the
/// game engine, and the system used within the Gameware Navigation engine at runtime. This mapping is used
/// to automatically convert coordinates and distances back and forth between the systems.
/// This class represents the mapping between global spaces by internally storing:
/// -	Which axis in the game engine relates to each of the X (Right), Y (Front) and Z (Up) axes within Gameware Navigation.
/// -	Whether the direction of each of those axes must be reversed (or negated).
/// -	The number of units in the game engine world that add up to one meter.
/// Internally, all Gameware Navigation components use a global space in which the X, Y and Z axes represent
/// the Right, Up and Front axes respectively, and all distances are expressed
/// and stored in meters using floating-point numbers.
/// When you open the Gameware Navigation Engine, an instance of the CoordSystem class is automatically created.
/// You can access this instance by calling Kaim::CoordSystem::Instance(). In your initialization
/// code, retrieve this instance and set it up with the correct mapping to the coordinate
/// system used in your game engine by calling Setup().
/// To convert values between different coordinate systems, you can use the class methods of this object
/// directly.
class CoordSystem
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	/// A type that represents the possible axis orientations that can be assigned to each of the
	/// Right, Front, and Up axes used within Gameware Navigation. Used in calls to
	/// CoordSystem::Setup() and CoordSystem::SetAxes().
	typedef CoordSystemClientAxis ClientAxis;

	static const ClientAxis CLIENT_X        = Kaim::CLIENT_X;       ///< Represents the positive direction of the X axis in the game engine.
	static const ClientAxis CLIENT_MINUS_X  = Kaim::CLIENT_MINUS_X; ///< Represents the negative direction of the X axis in the game engine.
	static const ClientAxis CLIENT_Y        = Kaim::CLIENT_Y;       ///< Represents the positive direction of the Y axis in the game engine.
	static const ClientAxis CLIENT_MINUS_Y  = Kaim::CLIENT_MINUS_Y; ///< Represents the negative direction of the Y axis in the game engine.
	static const ClientAxis CLIENT_Z        = Kaim::CLIENT_Z;       ///< Represents the positive direction of the Z axis in the game engine.
	static const ClientAxis CLIENT_MINUS_Z  = Kaim::CLIENT_MINUS_Z; ///< Represents the negative direction of the Z axis in the game engine.


public:
	/// For internal use: constructs a new instance of this class. 
	CoordSystem() { Setup(1.0f, CLIENT_X, CLIENT_Y, CLIENT_Z); }

	/// Sets the mapping between the coordinate system used in the Gameware Navigation Engine and the game engine.
	/// \param oneMeterInClientUnits       The number of world units in the game engine that add up to one meter in length.
	/// \param clientAxisForNavigationX    The axis in the game engine coordinate system that should be mapped to the X axis within the Gameware Navigation coordinate system.
	/// \param clientAxisForNavigationY    The axis in the game engine coordinate system that should be mapped to the Y axis within the Gameware Navigation coordinate system.
	/// \param clientAxisForNavigationZ    The axis in the game engine coordinate system that should be mapped to the Z axis within the Gameware Navigation coordinate system.
	void Setup(KyFloat32 oneMeterInClientUnits, ClientAxis clientAxisForNavigationX, ClientAxis clientAxisForNavigationY, ClientAxis clientAxisForNavigationZ);

	/// Sets the mapping between the coordinate system used in the Gameware Navigation Engine and the game engine using the
	/// values set in the specified CoordSystemConfig object. 
	void Setup(const CoordSystemConfig& config);

	/// Sets the number of world units in the game engine that add up to one meter in length. 
	void SetOneMeterInClientUnits(KyFloat32 oneMeter);

	/// Sets the axis in the game engine coordinate system that should be mapped to the X axis within the Gameware Navigation coordinate system. 
	void SetClientAxisForNavigationX(ClientAxis clientAxisForNavigationX);

	/// Sets the axis in the game engine coordinate system that should be mapped to the Y axis within the Gameware Navigation coordinate system. 
	void SetClientAxisForNavigationY(ClientAxis clientAxisForNavigationY);

	/// Sets the axis in the game engine coordinate system that should be mapped to the Z axis within the Gameware Navigation coordinate system. 
	void SetClientAxisForNavigationZ(ClientAxis clientAxisForNavigationZ);

	/// Retrieves the axis in the game engine coordinate system that is mapped to the X axis within the Gameware Navigation coordinate system. 
	ClientAxis GetClientAxisForNavigationX();

	/// Retrieves the axis in the game engine coordinate system that is mapped to the Y axis within the Gameware Navigation coordinate system. 
	ClientAxis GetClientAxisForNavigationY();

	/// Retrieves the axis in the game engine coordinate system that is mapped to the Z axis within the Gameware Navigation coordinate system. 
	ClientAxis GetClientAxisForNavigationZ();

public:
	// ------------------ Client <-> Gameware Navigation: Distances ------------------

	/// Converts a distance measurement from the game engine coordinate system to the Gameware Navigation coordinate system.
	/// \param clientDist			The distance to convert, expressed in game engine units.
	/// \return The converted distance, expressed in meters. 
	KyFloat32 ClientToNavigation_Dist(KyFloat32 clientDist) const;  // Dist

	/// Converts a distance measurement from the Gameware Navigation coordinate system to the game engine coordinate system.
	/// \param navigationDist			The distance to convert, expressed in meters.
	/// \return The converted distance, expressed in game engine units. 
	KyFloat32 NavigationToClient_Dist(KyFloat32 navigationDist) const; // Dist


	// ------------------ Client <-> Gameware Navigation: Square Distances ------------------

	/// Converts a squared distance measurement from the game engine coordinate system to the Gameware Navigation coordinate system.
	/// \param clientSquareDist		The squared distance to convert, expressed in game engine units.
	/// \return The converted squared distance, expressed in meters. 
	KyFloat32 ClientToNavigation_SquareDist(KyFloat32 clientSquareDist) const;  // SquareDist

	/// Converts a squared distance measurement from the Gameware Navigation coordinate system to the game engine coordinate system.
	/// \param navigationSquareDist		The squared distance to convert, expressed in meters.
	/// \return The converted squared distance, expressed in game engine units. 
	KyFloat32 NavigationToClient_SquareDist(KyFloat32 navigationSquareDist) const; // SquareDist


	// ------------------ Client <-> Gameware Navigation: Position Vectors ------------------
	/// Convert positions or any other vectors which length have to be scaled when converted.

	/// Converts a position from the game engine coordinate system to the Gameware Navigation coordinate system.
	/// \param clientPos                The position to convert, expressed in the game engine coordinate system.
	/// \param[out] navigationPos       Used to store the converted position, expressed in the Gameware Navigation coordinate system. 
	void ClientToNavigation_Pos(const Vec3f& clientPos, Vec3f& navigationPos) const;

	/// Converts a position from the Gameware Navigation engine coordinate system to the game coordinate system.
	/// \param navigationPos            The position to convert, expressed in the Gameware Navigation coordinate system.
	/// \param[out] clientPos           Used to store the converted position, expressed in the game engine coordinate system. 
	void NavigationToClient_Pos(const Vec3f& navigationPos, Vec3f& clientPos) const;

	/// Converts a position from the game engine coordinate system to the Gameware Navigation coordinate system.
	/// \param clientPos                The position to convert, expressed in the game engine coordinate system.
	/// \return The converted position, expressed in the Gameware Navigation coordinate system. 
	Vec3f ClientToNavigation_Pos(const Vec3f& clientPos) const;

	/// Converts a position from the Gameware Navigation coordinate system to the game engine coordinate system.
	/// \param navigationPos            The position to convert, expressed in the Gameware Navigation coordinate system.
	/// \return The converted position, expressed in the game engine coordinate system. 
	Vec3f NavigationToClient_Pos(const Vec3f& navigationPos) const;

	// ------------------ Client <-> Gameware Navigation: Normal Vectors ------------------
	/// Convert normals or any other vectors which length has to be kept when converted.

	/// Converts a normalized vector from the game engine coordinate system to the Gameware Navigation coordinate system.
	/// \param clientNormal             The normal to convert, expressed in the game engine coordinate system.
	/// \param[out] navigationNormal    Used to store the converted normal, expressed in the Gameware Navigation coordinate system. 
	void ClientToNavigation_Normal(const Vec3f& clientNormal, Vec3f& navigationNormal) const;

	/// Converts a normalized vector from the Gameware Navigation coordinate system to the game engine coordinate system.
	/// \param navigationNormal         The normal to convert, expressed in the Gameware Navigation coordinate system.
	/// \param[out] clientNormal        Used to store the converted normal, expressed in the game engine coordinate system. 
	void NavigationToClient_Normal(const Vec3f& navigationNormal, Vec3f& clientNormal) const;

	/// Converts a normalized vector from the game engine coordinate system to the Gameware Navigation coordinate system.
	/// \param clientNormal             The normal to convert, expressed in the game engine coordinate system.
	/// \return The converted normal, expressed in the Gameware Navigation coordinate system. 
	Vec3f ClientToNavigation_Normal(const Vec3f& clientNormal) const;

	/// Converts a normalized vector from the Gameware Navigation coordinate system to the game engine coordinate system.
	/// \param navigationNormal         The normal to convert, expressed in the Gameware Navigation coordinate system.
	/// \return The converted normal, expressed in the game engine coordinate system. 
	Vec3f NavigationToClient_Normal(const Vec3f& navigationNormal) const;

	// ------------------ Client <-> Gameware Navigation: Angular Velocity Vectors ------------------
	/// Convert angular velocity vectors (special case of Normals).

	/// Converts an angular velocity vector from the game engine coordinate system to the Gameware Navigation coordinate system.
	/// \param clientAngularVelocity             The angular velocity vector to convert, expressed in the game engine coordinate system.
	/// \param[out] navigationAngularVelocity    Used to store the converted angular velocity vector, expressed in the Gameware Navigation coordinate system. 
	void ClientToNavigation_AngularVelocity(const Vec3f& clientAngularVelocity, Vec3f& navigationAngularVelocity) const;

	/// Converts an angular velocity vector from the Gameware Navigation coordinate system to the game engine coordinate system.
	/// \param navigationAngularVelocity         The angular velocity vector to convert, expressed in the Gameware Navigation coordinate system.
	/// \param[out] clientAngularVelocity        Used to store the converted angular velocity vector, expressed in the game engine coordinate system. 
	void NavigationToClient_AngularVelocity(const Vec3f& navigationAngularVelocity, Vec3f& clientAngularVelocity) const;

	/// Converts an angular velocity vector from the game engine coordinate system to the Gameware Navigation coordinate system.
	/// \param clientAngularVelocity             The angular velocity vector to convert, expressed in the game engine coordinate system.
	/// \return The converted angular velocity vector, expressed in the Gameware Navigation coordinate system. 
	Vec3f ClientToNavigation_AngularVelocity(const Vec3f& clientAngularVelocity) const;

	/// Converts an angular velocity vector from the Gameware Navigation coordinate system to the game engine coordinate system.
	/// \param navigationAngularVelocity         The angular velocity vector to convert, expressed in the Gameware Navigation coordinate system.
	/// \return The converted angular velocity vector, expressed in the game engine coordinate system. 
	Vec3f NavigationToClient_AngularVelocity(const Vec3f& navigationAngularVelocity) const;


	// ------------------ Client <-> Gameware Navigation: Extents Vectors ------------------
	/// Convert extents or any other vectors which have to be scaled and values kept positive when converted.

	/// Converts an extents vector from the game engine coordinate system to the Gameware Navigation coordinate system.
	/// \pre                                All coordinates of input vector must be positive or null.
	/// \param clientExtents                The extents vector to convert, expressed in the game engine coordinate system.
	/// \param[out] navigationExtents       Used to store the converted extents vector, expressed in the Gameware Navigation coordinate system.
	void ClientToNavigation_Extents(const Vec3f& clientExtents, Vec3f& navigationExtents) const;

	/// Converts an extents vector from the Gameware Navigation engine coordinate system to the game coordinate system.
	/// \pre                                All coordinates of input vector must be positive or null.
	/// \param navigationExtents            The extents vector to convert, expressed in the Gameware Navigation coordinate system.
	/// \param[out] clientExtents           Used to store the converted extents vector, expressed in the game engine coordinate system. 
	void NavigationToClient_Extents(const Vec3f& navigationExtents, Vec3f& clientExtents) const;

	/// Converts an extents vector from the game engine coordinate system to the Gameware Navigation coordinate system.
	/// \pre                                All coordinates of input vector must be positive or null.
	/// \param clientExtents                The extents vector to convert, expressed in the game engine coordinate system.
	/// \return The converted extents vector, expressed in the Gameware Navigation coordinate system. 
	Vec3f ClientToNavigation_Extents(const Vec3f& clientExtents) const;

	/// Converts an extents vector from the Gameware Navigation coordinate system to the game engine coordinate system.
	/// \pre                                All coordinates of input vector must be positive or null.
	/// \param navigationExtents            The extents vector to convert, expressed in the Gameware Navigation coordinate system.
	/// \return The converted extents vector, expressed in the game engine coordinate system. 
	Vec3f NavigationToClient_Extents(const Vec3f& navigationExtents) const;


	// ------------------ Client <-> Gameware Navigation: Triangle ------------------

	/// Converts a triangle from the game engine coordinate system to the Gameware Navigation coordinate system.
	/// \param clientTriangle           The converted triangle, expressed in the game engine coordinate system. 
	/// \param navigationTriangle       The triangle to convert, expressed in the Gameware Navigation coordinate system.
	void ClientToNavigation_Triangle(const Triangle3f& clientTriangle, Triangle3f& navigationTriangle) const;

	/// Converts a triangle from the Gameware Navigation coordinate system to the game engine coordinate system.
	/// \param navigationTriangle       The triangle to convert, expressed in the Gameware Navigation coordinate system.
	/// \param clientTriangle           The converted triangle, expressed in the game engine coordinate system. 
	void NavigationToClient_Triangle(const Triangle3f& navigationTriangle, Triangle3f& clientTriangle) const;


	// ------------------ Client <-> Gameware Navigation: Boxes ------------------

	/// Converts a bounding box from the game engine coordinate system to the Gameware Navigation coordinate system.
	/// \param clientMin				The minima of the bounding box to convert, expressed in the game engine coordinate system.
	/// \param clientMax				The maxima of the bounding box to convert, expressed in the game engine coordinate system.
	/// \param[out] navigationBox		Used to store the converted bounding box, expressed in the Gameware Navigation coordinate system. 
	void ClientToNavigation_Box(const Vec3f& clientMin, const Vec3f& clientMax, Box3f& navigationBox) const;

	/// Converts a bounding box from the game engine coordinate system to the Gameware Navigation coordinate system.
	/// \param clientBox				The bounding box to convert, expressed in the game engine coordinate system.
	/// \param[out] navigationBox		Used to store the converted bounding box, expressed in the Gameware Navigation coordinate system. 
	void ClientToNavigation_Box(const Box3f& clientBox, Box3f& navigationBox) const;

	/// Converts a bounding box from the Gameware Navigation coordinate system to the game engine coordinate system.
	/// \param navigationBox			The bounding box to convert, expressed in the Gameware Navigation coordinate system.
	/// \param[out] clientMin			The minima of the bounding box, expressed in the game engine coordinate system.
	/// \param[out] clientMax			The maxima of the bounding box, expressed in the game engine coordinate system. 
	void NavigationToClient_Box(const Box3f& navigationBox, Vec3f& clientMin, Vec3f& clientMax) const;

	/// Converts a bounding box from the Gameware Navigation coordinate system to the game engine coordinate system.
	/// \param navigationBox			The bounding box to convert, expressed in the Gameware Navigation coordinate system.
	/// \param[out] clientBox			Used to store the converted bounding box, expressed in the game engine coordinate system. 
	void NavigationToClient_Box(const Box3f& navigationBox, Box3f& clientBox) const;


	// ------------------ Client <-> Gameware Navigation: Matrixes & Transforms ------------------

	/// Converts a matrix from the game engine coordinate system to the Gameware Navigation coordinate system.
	/// \param clientMatrix				The matrix to convert, expressed in the game engine coordinate system.
	/// \param[out] navigationMatrix	Used to store the converted matrix, expressed in the Gameware Navigation coordinate system. 
	void ClientToNavigation_Matrix(const Matrix3x3f& clientMatrix, Matrix3x3f& navigationMatrix) const;

	/// Converts a matrix from the Gameware Navigation coordinate system to the game engine coordinate system.
	/// \param navigationMatrix			The matrix to convert, expressed in the Gameware Navigation coordinate system.
	/// \param[out] clientMatrix		Used to store the converted matrix, expressed in the game engine coordinate system. 
	void NavigationToClient_Matrix(const Matrix3x3f& navigationMatrix, Matrix3x3f& clientMatrix) const;


	/// Converts a Transform from the game engine coordinate system to the Gameware Navigation coordinate system.
	/// \param clientTranform			The Transform to convert, expressed in the game engine coordinate system.
	/// \param[out] navigationTranform	Used to store the converted Transform, expressed in the Gameware Navigation coordinate system. 
	void ClientToNavigation_Tranform(const Transform& clientTranform, Transform& navigationTranform) const;

	/// Converts a Transform from the Gameware Navigation coordinate system to the game engine coordinate system.
	/// \param navigationTranform		The Transform to convert, expressed in the Gameware Navigation coordinate system.
	/// \param[out] clientTranform		Used to store the converted Transform, expressed in the game engine coordinate system. 
	void NavigationToClient_Tranform(const Transform& navigationTranform, Transform& clientTranform) const;


	// ------------------ Other Member Functions ------------------

	/// Converts a position between two specified coordinate systems.
	/// \param coordinates_in			The coordinate system of the position to be converted.
	/// \param pos_in					The position to be converted.
	/// \param coordinates_out			The final coordinate system of the converted position.
	/// \param[out] pos_out				The converted position. 
	void Convert_Pos(Coordinates coordinates_in, const Vec3f& pos_in, Coordinates coordinates_out, Vec3f& pos_out) const;

	/// Converts a distance measurement between two specified coordinate systems.
	/// \param coordinates_in			The coordinate system of the measurement to be converted.
	/// \param dist_in					The distance measurement to be converted.
	/// \param coordinates_out			The final coordinate system of the converted measurement.
	/// \param[out] dist_out			The converted measurement. 
	void Convert_Dist(Coordinates coordinates_in, KyFloat32 dist_in  , Coordinates coordinates_out, KyFloat32& dist_out) const;

	/// Converts a bounding box between two specified coordinate systems.
	/// \param coordinates_in			The coordinate system of the bounding box to be converted.
	/// \param box_in					The bounding box to be converted.
	/// \param coordinates_out			The final coordinate system of the converted bounding box.
	/// \param[out] box_out				The converted bounding box. 
	void Convert_Box(Coordinates coordinates_in, const Box3f& box_in, Coordinates coordinates_out, Box3f& box_out) const;

	/// Indicates whether or not the two CoordSystem instances contain the same data members. 
	bool operator==(const CoordSystem& other) const;

	/// Indicates whether or not the two CoordSystem instances contain at least one different data member. 
	bool operator!=(const CoordSystem& other) const;

	/// Indicates whether or not CoordSystem instance is right-handed
	bool IsRightHanded() const;

public:
	// Client axis <=> Gameware Navigation axis
	// navigation_x = m_navigationX_sign * clientPos[m_navigationX_idx];
	// clientPos[m_navigationX_idx] = m_navigationX_sign * navigation_x;
	KyInt32 m_navigationX_idx; ///< Identifies the index within each Vec3f that contains game engine coordinates that is mapped to the X axis in the Gameware Navigation coordinate system. 
	KyFloat32 m_navigationX_sign; ///< Indicates whether coordinates on the Gameware Navigation X axis need to be negated when converting between the Gameware Navigation and game engine coordinate systems. 

	KyInt32 m_navigationY_idx; ///< Identifies the index within each Vec3f that contains game engine coordinates that is mapped to the Y axis in the Gameware Navigation coordinate system. 
	KyFloat32 m_navigationY_sign; ///< Indicates whether coordinates on the Gameware Navigation Y axis need to be negated when converting between the Gameware Navigation and game engine coordinate systems. 

	KyInt32 m_navigationZ_idx; ///< Identifies the index within each Vec3f that contains game engine coordinates that is mapped to the Z axis in the Gameware Navigation coordinate system. 
	KyFloat32 m_navigationZ_sign; ///< Indicates whether coordinates on the Gameware Navigation Z axis need to be negated when converting between the Gameware Navigation and game engine coordinate systems. 

	// client dist <=> navigation dist
	KyFloat32 m_oneMeter; ///< The number of world units in the game engine that add up to one meter in length. 
	KyFloat32 m_inv_oneMeter; ///< One divided by the #m_oneMeter value. 

};

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.
inline void SwapEndianness(Endianness::Target e, CoordSystem& self)
{
	SwapEndianness(e, self.m_navigationX_idx);
	SwapEndianness(e, self.m_navigationX_sign);

	SwapEndianness(e, self.m_navigationY_idx);
	SwapEndianness(e, self.m_navigationY_sign);

	SwapEndianness(e, self.m_navigationZ_idx);
	SwapEndianness(e, self.m_navigationZ_sign);

	SwapEndianness(e, self.m_oneMeter);
	SwapEndianness(e, self.m_inv_oneMeter);
}


}

#include "gwnavruntime/basesystem/coordsystem.inl"



#endif

