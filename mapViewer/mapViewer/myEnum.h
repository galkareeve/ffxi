#pragma once

enum GL_DRAWTYPE { E_TRIANGLE_LIST, E_TRIANGLE_STRIP, E_LINE, E_LINE_LOOP, E_POLYGON };

//! An enumeration for all types of built-in scene node animators
enum ESCENE_NODE_ANIMATOR_TYPE
{
	//! Fly circle scene node animator
	ESNAT_FLY_CIRCLE = 0,

	//! Fly straight scene node animator
	ESNAT_FLY_STRAIGHT,

	//! Follow spline scene node animator
	ESNAT_FOLLOW_SPLINE,

	//! Rotation scene node animator
	ESNAT_ROTATION,

	//! Texture scene node animator
	ESNAT_TEXTURE,

	//! Deletion scene node animator
	ESNAT_DELETION,

	//! Collision respose scene node animator
	ESNAT_COLLISION_RESPONSE,

	//! FPS camera animator
	ESNAT_CAMERA_FPS,

	//! Maya camera animator
	ESNAT_CAMERA_MAYA,

	//! Amount of built-in scene node animators
	ESNAT_COUNT,

	//! Unknown scene node animator
	ESNAT_UNKNOWN,

	//! This enum is never used, it only forces the compiler to compile this enumeration to 32 bit.
	ESNAT_FORCE_32_BIT = 0x7fffffff
};

enum OVERLAP { OUTSIDE, INTERSECT, INSIDE };
//enum E_POS {OUTSIDE, PARTIAL, INSIDE, LEFT_OUTSIDE, RIGHT_OUTSIDE, TOP_OUTSIDE, BOTTOM_OUTSIDE, NEAR_OUTSIDE, FAR_OUTSIDE};


typedef struct
{
	float x1, x2;
	float y1, y2;
	float z1, z2;
} BoundingBox;

