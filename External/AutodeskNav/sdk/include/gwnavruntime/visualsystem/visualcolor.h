/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: MAMU
#ifndef Navigation_VisualColor_H
#define Navigation_VisualColor_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/base/endianness.h"

namespace Kaim
{

/// Represents a single RGBA color.
class VisualColor
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualSystem)

public:
	KY_INLINE static VisualColor Inactive() { return VisualColor::TransparentBlack; }

	/// Creates a new VisualColor instance set to opaque black.
	VisualColor() : m_r(0), m_g(0), m_b(0), m_a(255) {}

	/// Creates a new VisualColor instance initialized with provided (r, g, b, a) values.
	/// \param r    The intensity value of the red channel. This value must be between 0 (no color) and 255 (full color).
	/// \param g    The intensity value of the green channel. This value must be between 0 (no color) and 255 (full color).
	/// \param b    The intensity value of the blue channel. This value must be between 0 (no color) and 255 (full color).
	/// \param a    The intensity value of the alpha channel. This value must be between 0 (transparent) and 255 (opaque). 
	VisualColor(KyUInt8 r, KyUInt8 g, KyUInt8 b, KyUInt8 a = 255 ) { Set(r, g, b, a); }

	/// Creates a new VisualColor instance copying (r, g, b, a) values from the specified VisualColor. 
	VisualColor(const VisualColor& color) { Set(color.m_r, color.m_g, color.m_b, color.m_a); }

	/// Creates a new VisualColor instance copying (r, g, b) values from the specified VisualColor
	/// and setting the alpha value accordingly to the provided a ratio.
	VisualColor(const VisualColor& color, KyFloat32 a) { Set(color.m_r, color.m_g, color.m_b, (KyUInt8)(a * 255.0f)); }

	/// Creates a new VisualColor instance copying (r, g, b) values from the specified VisualColor
	/// and setting the alpha value accordingly to the provided a value.
	VisualColor(const VisualColor& color, KyUInt8 a) { Set(color.m_r, color.m_g, color.m_b, a); }


	/// Returns true if the RGBA values of the specified VisualColor are the same as for this object. 
	bool operator==(const VisualColor& other) const
	{
		return m_r == other.m_r && m_g == other.m_g && m_b == other.m_b && m_a == other.m_a;
	}

	/// Returns true if at least one of the RGBA values of the specified VisualColor is different than for this object. 
	bool operator!=(const VisualColor& other) const
	{
		return !(operator==(other));
	}

	/// Sets the color value of this object.
	/// \param r    The intensity value of the red channel. This value must be between 0 (no color) and 255 (full color).
	/// \param g    The intensity value of the green channel. This value must be between 0 (no color) and 255 (full color).
	/// \param b    The intensity value of the blue channel. This value must be between 0 (no color) and 255 (full color).
	/// \param a    The intensity value of the alpha channel. This value must be between 0 (transparent) and 255 (opaque). 
	void Set(KyUInt8 r, KyUInt8 g, KyUInt8 b, KyUInt8 a = 255)
	{
		m_r = r;
		m_g = g;
		m_b = b;
		m_a = a;
	}

	/// Sets the color value of this object.
	/// \param r    The intensity value of the red channel. This value must be between 0.f (no color) and 1.f (full color).
	/// \param g    The intensity value of the green channel. This value must be between 0.f (no color) and 1.f (full color).
	/// \param b    The intensity value of the blue channel. This value must be between 0.f (no color) and 1.f (full color).
	/// \param a    The intensity value of the alpha channel. This value must be between 0.f (transparent) and 1.f (opaque). 
	void SetFloat(KyFloat32 r, KyFloat32 g, KyFloat32 b, KyFloat32 a = 1.0f)
	{
		m_r = (KyUInt8)(r * 255.0f);
		m_g = (KyUInt8)(g * 255.0f);
		m_b = (KyUInt8)(b * 255.0f);
		m_a = (KyUInt8)(a * 255.0f);
	}

public:
	KyUInt8 m_r; ///< The intensity value of the red channel. This value must be between 0 (no color) and 255 (full color). 
	KyUInt8 m_g; ///< The intensity value of the green channel. This value must be between 0 (no color) and 255 (full color). 
	KyUInt8 m_b; ///< The intensity value of the blue channel. This value must be between 0 (no color) and 255 (full color). 
	KyUInt8 m_a; ///< The intensity value of the alpha channel. This value must be between 0 (transparent) and 255 (opaque). 

public:
	static const VisualColor AliceBlue;         ///< Represents the color with RGBA values (240, 248, 255, 255). \colorbox{AliceBlue}  
	static const VisualColor AntiqueWhite;      ///< Represents the color with RGBA values (250, 235, 215, 255). \colorbox{AntiqueWhite}  
	static const VisualColor Aqua;              ///< Represents the color with RGBA values (  0, 255, 255, 255). \colorbox{Aqua}  
	static const VisualColor Aquamarine;        ///< Represents the color with RGBA values (127, 255, 212, 255). \colorbox{Aquamarine}  
	static const VisualColor Azure;             ///< Represents the color with RGBA values (240, 255, 255, 255). \colorbox{Azure}  
	static const VisualColor Beige;             ///< Represents the color with RGBA values (245, 245, 220, 255). \colorbox{Beige}  
	static const VisualColor Bisque;            ///< Represents the color with RGBA values (255, 228, 196, 255). \colorbox{Bisque}  
	static const VisualColor Black;             ///< Represents the color with RGBA values (  0,   0,   0, 255). \colorbox{Black}  
	static const VisualColor BlanchedAlmond;    ///< Represents the color with RGBA values (255, 235, 205, 255). \colorbox{BlanchedAlmond}  
	static const VisualColor Blue;              ///< Represents the color with RGBA values (  0,   0, 255, 255). \colorbox{Blue}  
	static const VisualColor BlueViolet;        ///< Represents the color with RGBA values (138,  43, 226, 255). \colorbox{BlueViolet}  
	static const VisualColor Brown;             ///< Represents the color with RGBA values (165,  42,  42, 255). \colorbox{Brown}  
	static const VisualColor BurlyWood;         ///< Represents the color with RGBA values (222, 184, 135, 255). \colorbox{BurlyWood}  
	static const VisualColor CadetBlue;         ///< Represents the color with RGBA values ( 95, 158, 160, 255). \colorbox{CadetBlue}  
	static const VisualColor Chartreuse;        ///< Represents the color with RGBA values (127, 255,   0, 255). \colorbox{Chartreuse}  
	static const VisualColor Chocolate;         ///< Represents the color with RGBA values (210, 105,  30, 255). \colorbox{Chocolate}  
	static const VisualColor Coral;             ///< Represents the color with RGBA values (255, 127,  80, 255). \colorbox{Coral}  
	static const VisualColor CornflowerBlue;    ///< Represents the color with RGBA values (100, 149, 237, 255). \colorbox{CornflowerBlue}  
	static const VisualColor Cornsilk;          ///< Represents the color with RGBA values (255, 248, 220, 255). \colorbox{Cornsilk}  
	static const VisualColor Crimson;           ///< Represents the color with RGBA values (220,  20,  60, 255). \colorbox{Crimson}  
	static const VisualColor Cyan;              ///< Represents the color with RGBA values (  0, 255, 255, 255). \colorbox{Cyan}  
	static const VisualColor DarkBlue;          ///< Represents the color with RGBA values (  0,   0, 139, 255). \colorbox{DarkBlue}  
	static const VisualColor DarkCyan;          ///< Represents the color with RGBA values (  0, 139, 139, 255). \colorbox{DarkCyan}  
	static const VisualColor DarkGoldenrod;     ///< Represents the color with RGBA values (184, 134,  11, 255). \colorbox{DarkGoldenrod}  
	static const VisualColor DarkGray;          ///< Represents the color with RGBA values (169, 169, 169, 255). \colorbox{DarkGray}  
	static const VisualColor DarkGreen;         ///< Represents the color with RGBA values (  0, 100,   0, 255). \colorbox{DarkGreen}  
	static const VisualColor DarkKhaki;         ///< Represents the color with RGBA values (189, 183, 107, 255). \colorbox{DarkKhaki}  
	static const VisualColor DarkMagenta;       ///< Represents the color with RGBA values (139,   0, 139, 255). \colorbox{DarkMagenta}  
	static const VisualColor DarkOliveGreen;    ///< Represents the color with RGBA values ( 85, 107,  47, 255). \colorbox{DarkOliveGreen}  
	static const VisualColor DarkOrange;        ///< Represents the color with RGBA values (255, 140,   0, 255). \colorbox{DarkOrange}  
	static const VisualColor DarkOrchid;        ///< Represents the color with RGBA values (153,  50, 204, 255). \colorbox{DarkOrchid}  
	static const VisualColor DarkRed;           ///< Represents the color with RGBA values (139,   0,   0, 255). \colorbox{DarkRed}  
	static const VisualColor DarkSalmon;        ///< Represents the color with RGBA values (233, 150, 122, 255). \colorbox{DarkSalmon}  
	static const VisualColor DarkSeaGreen;      ///< Represents the color with RGBA values (143, 188, 139, 255). \colorbox{DarkSeaGreen}  
	static const VisualColor DarkSlateBlue;     ///< Represents the color with RGBA values ( 72,  61, 139, 255). \colorbox{DarkSlateBlue}  
	static const VisualColor DarkSlateGray;     ///< Represents the color with RGBA values ( 47,  79,  79, 255). \colorbox{DarkSlateGray}  
	static const VisualColor DarkTurquoise;     ///< Represents the color with RGBA values (  0, 206, 209, 255). \colorbox{DarkTurquoise}  
	static const VisualColor DarkViolet;        ///< Represents the color with RGBA values (148,   0, 211, 255). \colorbox{DarkViolet}  
	static const VisualColor DeepPink;          ///< Represents the color with RGBA values (255,  20, 147, 255). \colorbox{DeepPink}  
	static const VisualColor DeepSkyBlue;       ///< Represents the color with RGBA values (  0, 191, 255, 255). \colorbox{DeepSkyBlue}  
	static const VisualColor DimGray;           ///< Represents the color with RGBA values (105, 105, 105, 255). \colorbox{DimGray}  
	static const VisualColor DodgerBlue;        ///< Represents the color with RGBA values ( 30, 144, 255, 255). \colorbox{DodgerBlue}  
	static const VisualColor Firebrick;         ///< Represents the color with RGBA values (178,  34,  34, 255). \colorbox{Firebrick}  
	static const VisualColor FloralWhite;       ///< Represents the color with RGBA values (255, 250, 240, 255). \colorbox{FloralWhite}  
	static const VisualColor ForestGreen;       ///< Represents the color with RGBA values ( 34, 139,  34, 255). \colorbox{ForestGreen}  
	static const VisualColor Fuchsia;           ///< Represents the color with RGBA values (255,   0, 255, 255). \colorbox{Fuchsia}  
	static const VisualColor Gainsboro;         ///< Represents the color with RGBA values (220, 220, 220, 255). \colorbox{Gainsboro}  
	static const VisualColor GhostWhite;        ///< Represents the color with RGBA values (248, 248, 255, 255). \colorbox{GhostWhite}  
	static const VisualColor Gold;              ///< Represents the color with RGBA values (255, 215,   0, 255). \colorbox{Gold}  
	static const VisualColor Goldenrod;         ///< Represents the color with RGBA values (218, 165,  32, 255). \colorbox{Goldenrod}  
	static const VisualColor Gray;              ///< Represents the color with RGBA values (128, 128, 128, 255). \colorbox{Gray}  
	static const VisualColor Green;             ///< Represents the color with RGBA values (  0, 128,   0, 255). \colorbox{Green}  
	static const VisualColor GreenYellow;       ///< Represents the color with RGBA values (173, 255,  47, 255). \colorbox{GreenYellow}  
	static const VisualColor Honeydew;          ///< Represents the color with RGBA values (240, 255, 240, 255). \colorbox{Honeydew}  
	static const VisualColor HotPink;           ///< Represents the color with RGBA values (255, 105, 180, 255). \colorbox{HotPink}  
	static const VisualColor IndianRed;         ///< Represents the color with RGBA values (205,  92,  92, 255). \colorbox{IndianRed}  
	static const VisualColor Indigo;            ///< Represents the color with RGBA values ( 75,   0, 130, 255). \colorbox{Indigo}  
	static const VisualColor Ivory;             ///< Represents the color with RGBA values (255, 255, 240, 255). \colorbox{Ivory}  
	static const VisualColor Khaki;             ///< Represents the color with RGBA values (240, 230, 140, 255). \colorbox{Khaki}  
	static const VisualColor Lavender;          ///< Represents the color with RGBA values (230, 230, 250, 255). \colorbox{Lavender}  
	static const VisualColor LavenderBlush;     ///< Represents the color with RGBA values (255, 240, 245, 255). \colorbox{LavenderBlush}  
	static const VisualColor LawnGreen;         ///< Represents the color with RGBA values (124, 252,   0, 255). \colorbox{LawnGreen}  
	static const VisualColor LemonChiffon;      ///< Represents the color with RGBA values (255, 250, 205, 255). \colorbox{LemonChiffon}  
	static const VisualColor LightBlue;         ///< Represents the color with RGBA values (173, 216, 230, 255). \colorbox{LightBlue}  
	static const VisualColor LightCoral;        ///< Represents the color with RGBA values (240, 128, 128, 255). \colorbox{LightCoral}  
	static const VisualColor LightCyan;         ///< Represents the color with RGBA values (224, 255, 255, 255). \colorbox{LightCyan}  
	static const VisualColor LightGoldenrodYellow; ///< Represents the color with RGBA values (250, 250, 210, 255). \colorbox{LightGoldenrodYellow}  
	static const VisualColor LightGray;         ///< Represents the color with RGBA values (211, 211, 211, 255). \colorbox{LightGray}  
	static const VisualColor LightGreen;        ///< Represents the color with RGBA values (144, 238, 144, 255). \colorbox{LightGreen}  
	static const VisualColor LightPink;         ///< Represents the color with RGBA values (255, 182, 193, 255). \colorbox{LightPink}  
	static const VisualColor LightSalmon;       ///< Represents the color with RGBA values (255, 160, 122, 255). \colorbox{LightSalmon}  
	static const VisualColor LightSeaGreen;     ///< Represents the color with RGBA values ( 32, 178, 170, 255). \colorbox{LightSeaGreen}  
	static const VisualColor LightSkyBlue;      ///< Represents the color with RGBA values (135, 206, 250, 255). \colorbox{LightSkyBlue}  
	static const VisualColor LightSlateGray;    ///< Represents the color with RGBA values (119, 136, 153, 255). \colorbox{LightSlateGray}  
	static const VisualColor LightSteelBlue;    ///< Represents the color with RGBA values (176, 196, 222, 255). \colorbox{LightSteelBlue}  
	static const VisualColor LightYellow;       ///< Represents the color with RGBA values (255, 255, 224, 255). \colorbox{LightYellow}  
	static const VisualColor Lime;              ///< Represents the color with RGBA values (  0, 255,   0, 255). \colorbox{Lime}  
	static const VisualColor LimeGreen;         ///< Represents the color with RGBA values ( 50, 205,  50, 255). \colorbox{LimeGreen}  
	static const VisualColor Linen;             ///< Represents the color with RGBA values (250, 240, 230, 255). \colorbox{Linen}  
	static const VisualColor Magenta;           ///< Represents the color with RGBA values (255,   0, 255, 255). \colorbox{Magenta}  
	static const VisualColor Maroon;            ///< Represents the color with RGBA values (128,   0,   0, 255). \colorbox{Maroon}  
	static const VisualColor MediumAquamarine;  ///< Represents the color with RGBA values (102, 205, 170, 255). \colorbox{MediumAquamarine}  
	static const VisualColor MediumBlue;        ///< Represents the color with RGBA values (  0,   0, 205, 255). \colorbox{MediumBlue}  
	static const VisualColor MediumOrchid;      ///< Represents the color with RGBA values (186,  85, 211, 255). \colorbox{MediumOrchid}  
	static const VisualColor MediumPurple;      ///< Represents the color with RGBA values (147, 112, 219, 255). \colorbox{MediumPurple}  
	static const VisualColor MediumSeaGreen;    ///< Represents the color with RGBA values ( 60, 179, 113, 255). \colorbox{MediumSeaGreen}  
	static const VisualColor MediumSlateBlue;   ///< Represents the color with RGBA values (123, 104, 238, 255). \colorbox{MediumSlateBlue}  
	static const VisualColor MediumSpringGreen; ///< Represents the color with RGBA values (  0, 250, 154, 255). \colorbox{MediumSpringGreen}  
	static const VisualColor MediumTurquoise;   ///< Represents the color with RGBA values ( 72, 209, 204, 255). \colorbox{MediumTurquoise}  
	static const VisualColor MediumVioletRed;   ///< Represents the color with RGBA values (199,  21, 133, 255). \colorbox{MediumVioletRed}  
	static const VisualColor MidnightBlue;      ///< Represents the color with RGBA values ( 25,  25, 112, 255). \colorbox{MidnightBlue}  
	static const VisualColor MintCream;         ///< Represents the color with RGBA values (245, 255, 250, 255). \colorbox{MintCream}  
	static const VisualColor MistyRose;         ///< Represents the color with RGBA values (255, 228, 225, 255). \colorbox{MistyRose}  
	static const VisualColor Moccasin;          ///< Represents the color with RGBA values (255, 228, 181, 255). \colorbox{Moccasin}  
	static const VisualColor NavajoWhite;       ///< Represents the color with RGBA values (255, 222, 173, 255). \colorbox{NavajoWhite}  
	static const VisualColor Navy;              ///< Represents the color with RGBA values (  0,   0, 128, 255). \colorbox{Navy}  
	static const VisualColor OldLace;           ///< Represents the color with RGBA values (253, 245, 230, 255). \colorbox{OldLace}  
	static const VisualColor Olive;             ///< Represents the color with RGBA values (128, 128,   0, 255). \colorbox{Olive}  
	static const VisualColor OliveDrab;         ///< Represents the color with RGBA values (107, 142,  35, 255). \colorbox{OliveDrab}  
	static const VisualColor Orange;            ///< Represents the color with RGBA values (255, 165,   0, 255). \colorbox{Orange}  
	static const VisualColor OrangeRed;         ///< Represents the color with RGBA values (255,  69,   0, 255). \colorbox{OrangeRed}  
	static const VisualColor Orchid;            ///< Represents the color with RGBA values (218, 112, 214, 255). \colorbox{Orchid}  
	static const VisualColor PaleGoldenrod;     ///< Represents the color with RGBA values (238, 232, 170, 255). \colorbox{PaleGoldenrod}  
	static const VisualColor PaleGreen;         ///< Represents the color with RGBA values (152, 251, 152, 255). \colorbox{PaleGreen}  
	static const VisualColor PaleTurquoise;     ///< Represents the color with RGBA values (175, 238, 238, 255). \colorbox{PaleTurquoise}  
	static const VisualColor PaleVioletRed;     ///< Represents the color with RGBA values (219, 112, 147, 255). \colorbox{PaleVioletRed}  
	static const VisualColor PapayaWhip;        ///< Represents the color with RGBA values (255, 239, 213, 255). \colorbox{PapayaWhip}  
	static const VisualColor PeachPuff;         ///< Represents the color with RGBA values (255, 218, 185, 255). \colorbox{PeachPuff}  
	static const VisualColor Peru;              ///< Represents the color with RGBA values (205, 133,  63, 255). \colorbox{Peru}  
	static const VisualColor Pink;              ///< Represents the color with RGBA values (255, 192, 203, 255). \colorbox{Pink}  
	static const VisualColor Plum;              ///< Represents the color with RGBA values (221, 160, 221, 255). \colorbox{Plum}  
	static const VisualColor PowderBlue;        ///< Represents the color with RGBA values (176, 224, 230, 255). \colorbox{PowderBlue}  
	static const VisualColor Purple;            ///< Represents the color with RGBA values (128,   0, 128, 255). \colorbox{Purple}  
	static const VisualColor Red;               ///< Represents the color with RGBA values (255,   0,   0, 255). \colorbox{Red}  
	static const VisualColor RosyBrown;         ///< Represents the color with RGBA values (188, 143, 143, 255). \colorbox{RosyBrown}  
	static const VisualColor RoyalBlue;         ///< Represents the color with RGBA values ( 65, 105, 225, 255). \colorbox{RoyalBlue}  
	static const VisualColor SaddleBrown;       ///< Represents the color with RGBA values (139,  69,  19, 255). \colorbox{SaddleBrown}  
	static const VisualColor Salmon;            ///< Represents the color with RGBA values (250, 128, 114, 255). \colorbox{Salmon}  
	static const VisualColor SandyBrown;        ///< Represents the color with RGBA values (244, 164,  96, 255). \colorbox{SandyBrown}  
	static const VisualColor SeaGreen;          ///< Represents the color with RGBA values ( 46, 139,  87, 255). \colorbox{SeaGreen}  
	static const VisualColor SeaShell;          ///< Represents the color with RGBA values (255, 245, 238, 255). \colorbox{SeaShell}  
	static const VisualColor Sienna;            ///< Represents the color with RGBA values (160,  82,  45, 255). \colorbox{Sienna}  
	static const VisualColor Silver;            ///< Represents the color with RGBA values (192, 192, 192, 255). \colorbox{Silver}  
	static const VisualColor SkyBlue;           ///< Represents the color with RGBA values (135, 206, 235, 255). \colorbox{SkyBlue}  
	static const VisualColor SlateBlue;         ///< Represents the color with RGBA values (106,  90, 205, 255). \colorbox{SlateBlue}  
	static const VisualColor SlateGray;         ///< Represents the color with RGBA values (112, 128, 144, 255). \colorbox{SlateGray}  
	static const VisualColor Snow;              ///< Represents the color with RGBA values (255, 250, 250, 255). \colorbox{Snow}  
	static const VisualColor SpringGreen;       ///< Represents the color with RGBA values (  0, 255, 127, 255). \colorbox{SpringGreen}  
	static const VisualColor SteelBlue;         ///< Represents the color with RGBA values ( 70, 130, 180, 255). \colorbox{SteelBlue}  
	static const VisualColor Tan;               ///< Represents the color with RGBA values (210, 180, 140, 255). \colorbox{Tan}  
	static const VisualColor Teal;              ///< Represents the color with RGBA values (  0, 128, 128, 255). \colorbox{Teal}  
	static const VisualColor Thistle;           ///< Represents the color with RGBA values (216, 191, 216, 255). \colorbox{Thistle}  
	static const VisualColor Tomato;            ///< Represents the color with RGBA values (255,  99,  71, 255). \colorbox{Tomato}  
	static const VisualColor TransparentBlack;  ///< Represents the color with RGBA values (  0,   0,   0,   0).  
	static const VisualColor TransparentWhite;  ///< Represents the color with RGBA values (255, 255, 255,   0).  
	static const VisualColor Turquoise;         ///< Represents the color with RGBA values ( 64, 224, 208, 255). \colorbox{Turquoise}  
	static const VisualColor Violet;            ///< Represents the color with RGBA values (238, 130, 238, 255). \colorbox{Violet}  
	static const VisualColor Wheat;             ///< Represents the color with RGBA values (245, 222, 179, 255). \colorbox{Wheat}  
	static const VisualColor White;             ///< Represents the color with RGBA values (255, 255, 255, 255). \colorbox{White}  
	static const VisualColor WhiteSmoke;        ///< Represents the color with RGBA values (245, 245, 245, 255). \colorbox{WhiteSmoke}  
	static const VisualColor Yellow;            ///< Represents the color with RGBA values (255, 255,   0, 255). \colorbox{Yellow}  
	static const VisualColor YellowGreen;       ///< Represents the color with RGBA values (154, 205,  50, 255). \colorbox{YellowGreen}  

public:
	/// Retrieves the color with the specified index within a limited internal palette of 31 distinct colors. 
	static const VisualColor& GetDistinctColor(KyUInt32 index);

	/// Retrieves the color with the specified index within a limited internal palette of 30 distinct colors, without VisualColor::Red. 
	static const VisualColor& GetDistinctColorWithoutRed(const KyUInt32 index);

	/// Retrieves the color with the specified index within a limited internal palette of 31 distinct colors.
	/// The palette can be limited further by thenbColorsInPalette parameter. 
	static const VisualColor& GetDistinctColor(KyUInt32 index, KyUInt32 nbColorsInPalette);

	/// Retrieves the color with the specified index from the specified palette.
	/// \param index				The index of the color to retrieve.
	/// \param customPalette		An array of colors from which the specified index will be retrieved. 
	/// \param nbColorsInPalette	The number of elements in thecustomPalette array. 
	static const VisualColor& GetDistinctColor(const KyUInt32 index, const VisualColor* customPalette, KyUInt32 nbColorsInPalette);
};

inline void SwapEndianness(Endianness::Target /*e*/, VisualColor& /*self*/)
{
	// nothing to swap
}

}

#endif //Navigation_VisualColor_H
