#ifndef	__R3D_CONST_H
#define	__R3D_CONST_H

//
// Math Constants
// 
#define R3D_E            2.71828182845904523536f
#define R3D_LOG2E        1.44269504088896340736f
#define R3D_LOG10E       0.434294481903251827651f
#define R3D_LN2          0.693147180559945309417f
#define R3D_LN10         2.30258509299404568402f
#define R3D_PI           3.1415926f
#define R3D_PI_2         1.57079632679489661923f
#define R3D_PI_4         0.785398163397448309616f
#define R3D_1_PI         0.318309886183790671538f
#define R3D_2_PI         0.636619772367581343076f
#define R3D_1_SQRTPI     0.564189583547756286948f
#define R3D_2_SQRTPI     1.12837916709551257390f
#define R3D_SQRT2        1.41421356237309504880f
#define R3D_SQRT_2       0.707106781186547524401f
#define R3D_180_Over_PI  (180.0f / R3D_PI)

#define R3D_SQ(a)        ((a)*(a))
#define R3D_DEG(a)       (a*10)
#define R3D_DEG2RAD(x)   (((x) * R3D_PI) / 180.0f)
#define R3D_RAD2DEG(x)   (((x) * 180.0f) / R3D_PI)
#define R3D_ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))

//
// Useful defines in function declarations
//
#define INARG
#define OUTARG
#define INOUTARG


//
// Library defines
//
#define R3D_MAX_MATERIAL_NAME 	128
#define	R3D_MAX_OBJECT_NAME	128
#define R3D_MAX_FILE_NAME	256

#endif	// __R3D_CONST_H
