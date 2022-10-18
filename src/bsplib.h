//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//

#pragma once

#include <cassert>

// This is an extremely abridged version of bsplib from the Source 2013 SDK
// it's abridged because I didn't want to require the entire SDK source code to be installed

#define	HEADER_LUMPS		64

// little-endian "VBSP"
#define IDBSPHEADER	(('P'<<24)+('S'<<16)+('B'<<8)+'V')	

// Common limits
// leaffaces, leafbrushes, planes, and verts are still bounded by
// 16 bit short limits
#define	MAX_MAP_MODELS					1024
#define	MAX_MAP_BRUSHES					8192
#define	MAX_MAP_ENTITIES				8192
#define	MAX_MAP_TEXINFO					12288
#define MAX_MAP_TEXDATA					2048
#define MAX_MAP_DISPINFO				2048
#define MAX_MAP_DISP_VERTS				( MAX_MAP_DISPINFO * ((1<<MAX_MAP_DISP_POWER)+1) * ((1<<MAX_MAP_DISP_POWER)+1) )
#define MAX_MAP_DISP_TRIS				( (1 << MAX_MAP_DISP_POWER) * (1 << MAX_MAP_DISP_POWER) * 2 )
#define MAX_DISPVERTS					NUM_DISP_POWER_VERTS( MAX_MAP_DISP_POWER )
#define MAX_DISPTRIS					NUM_DISP_POWER_TRIS( MAX_MAP_DISP_POWER )
#define	MAX_MAP_AREAS					256
#define MAX_MAP_AREA_BYTES				(MAX_MAP_AREAS/8)
#define	MAX_MAP_AREAPORTALS				1024
// Planes come in pairs, thus an even number.
#define	MAX_MAP_PLANES					65536
#define	MAX_MAP_NODES					65536
#define	MAX_MAP_BRUSHSIDES				65536
#define	MAX_MAP_LEAFS					65536
#define	MAX_MAP_VERTS					65536
#define MAX_MAP_VERTNORMALS				256000
#define MAX_MAP_VERTNORMALINDICES		256000
#define	MAX_MAP_FACES					65536
#define	MAX_MAP_LEAFFACES				65536
#define	MAX_MAP_LEAFBRUSHES 			65536
#define	MAX_MAP_PORTALS					65536
#define MAX_MAP_CLUSTERS				65536
#define MAX_MAP_LEAFWATERDATA			32768
#define MAX_MAP_PORTALVERTS				128000
#define	MAX_MAP_EDGES					256000
#define	MAX_MAP_SURFEDGES				512000
#define	MAX_MAP_LIGHTING				0x1000000
#define	MAX_MAP_VISIBILITY				0x1000000			// increased BSPVERSION 7
#define	MAX_MAP_TEXTURES				1024
#define MAX_MAP_WORLDLIGHTS				8192
#define MAX_MAP_CUBEMAPSAMPLES			1024
#define MAX_MAP_OVERLAYS				512 
#define MAX_MAP_WATEROVERLAYS			16384
#define MAX_MAP_TEXDATA_STRING_DATA		256000
#define MAX_MAP_TEXDATA_STRING_TABLE	65536
// this is stuff for trilist/tristrips, etc.
#define MAX_MAP_PRIMITIVES				32768
#define MAX_MAP_PRIMVERTS				65536
#define MAX_MAP_PRIMINDICES				65536
#define	MAXLIGHTMAPS	4

// List of lumps taken from Valve's code.

enum
{
	LUMP_ENTITIES = 0,	// *
	LUMP_PLANES = 1,	// *
	LUMP_TEXDATA = 2,	// *
	LUMP_VERTEXES = 3,	// *
	LUMP_VISIBILITY = 4,	// *
	LUMP_NODES = 5,	// *
	LUMP_TEXINFO = 6,	// *
	LUMP_FACES = 7,	// *
	LUMP_LIGHTING = 8,	// *
	LUMP_OCCLUSION = 9,
	LUMP_LEAFS = 10,	// *
	LUMP_FACEIDS = 11,
	LUMP_EDGES = 12,	// *
	LUMP_SURFEDGES = 13,	// *
	LUMP_MODELS = 14,	// *
	LUMP_WORLDLIGHTS = 15,	// 
	LUMP_LEAFFACES = 16,	// *
	LUMP_LEAFBRUSHES = 17,	// *
	LUMP_BRUSHES = 18,	// *
	LUMP_BRUSHSIDES = 19,	// *
	LUMP_AREAS = 20,	// *
	LUMP_AREAPORTALS = 21,	// *
	LUMP_UNUSED0 = 22,
	LUMP_UNUSED1 = 23,
	LUMP_UNUSED2 = 24,
	LUMP_UNUSED3 = 25,
	LUMP_DISPINFO = 26,
	LUMP_ORIGINALFACES = 27,
	LUMP_PHYSDISP = 28,
	LUMP_PHYSCOLLIDE = 29,
	LUMP_VERTNORMALS = 30,
	LUMP_VERTNORMALINDICES = 31,
	LUMP_DISP_LIGHTMAP_ALPHAS = 32,
	LUMP_DISP_VERTS = 33,		// CDispVerts
	LUMP_DISP_LIGHTMAP_SAMPLE_POSITIONS = 34,	// For each displacement
												//     For each lightmap sample
												//         byte for index
												//         if 255, then index = next byte + 255
												//         3 bytes for barycentric coordinates
	// The game lump is a method of adding game-specific lumps
	// FIXME: Eventually, all lumps could use the game lump system
	LUMP_GAME_LUMP = 35,
	LUMP_LEAFWATERDATA = 36,
	LUMP_PRIMITIVES = 37,
	LUMP_PRIMVERTS = 38,
	LUMP_PRIMINDICES = 39,
	// A pak file can be embedded in a .bsp now, and the file system will search the pak
	//  file first for any referenced names, before deferring to the game directory 
	//  file system/pak files and finally the base directory file system/pak files.
	LUMP_PAKFILE = 40,
	LUMP_CLIPPORTALVERTS = 41,
	// A map can have a number of cubemap entities in it which cause cubemap renders
	// to be taken after running vrad.
	LUMP_CUBEMAPS = 42,
	LUMP_TEXDATA_STRING_DATA = 43,
	LUMP_TEXDATA_STRING_TABLE = 44,
	LUMP_OVERLAYS = 45,
	LUMP_LEAFMINDISTTOWATER = 46,
	LUMP_FACE_MACRO_TEXTURE_INFO = 47,
	LUMP_DISP_TRIS = 48,
	LUMP_PHYSCOLLIDESURFACE = 49,	// deprecated.  We no longer use win32-specific havok compression on terrain
	LUMP_WATEROVERLAYS = 50,
	LUMP_LEAF_AMBIENT_INDEX_HDR = 51,	// index of LUMP_LEAF_AMBIENT_LIGHTING_HDR
	LUMP_LEAF_AMBIENT_INDEX = 52,	// index of LUMP_LEAF_AMBIENT_LIGHTING

	// optional lumps for HDR
	LUMP_LIGHTING_HDR = 53,
	LUMP_WORLDLIGHTS_HDR = 54,
	LUMP_LEAF_AMBIENT_LIGHTING_HDR = 55,	// NOTE: this data overrides part of the data stored in LUMP_LEAFS.
	LUMP_LEAF_AMBIENT_LIGHTING = 56,	// NOTE: this data overrides part of the data stored in LUMP_LEAFS.

	LUMP_XZIPPAKFILE = 57,   // deprecated. xbox 1: xzip version of pak file
	LUMP_FACES_HDR = 58,	// HDR maps may have different face data.
	LUMP_MAP_FLAGS = 59,   // extended level-wide flags. not present in all levels
	LUMP_OVERLAY_FADES = 60,	// Fade distances for overlays
};


// Lumps that have versions are listed here
enum
{
	LUMP_LIGHTING_VERSION = 1,
	LUMP_FACES_VERSION = 1,
	LUMP_OCCLUSION_VERSION = 2,
	LUMP_LEAFS_VERSION = 1,
	LUMP_LEAF_AMBIENT_LIGHTING_VERSION = 1,
};

// Structs taken directly from Valve's code.
// Only edits made were:
// remove references to DECLARE_BYTWSWAP_DATADESC() which is a macro Valve has to swap endianness
// replace vectors with 3 floats (Valve's vectors are NOT the same as C++ Standard vectors)
// replace "byte" with unsigned char (it is simply used for clarity in Valve's code)
// Constructors and operator overloads added to dvertex_t

struct ColorRGBExp32
{
	unsigned char r, g, b;
	signed char exponent;
};

struct lump_t
{
	int		fileofs, filelen;
	int		version;		// default to zero
	// this field was char fourCC[4] previously, but was unused, favoring the LUMP IDs above instead. It has been
	// repurposed for compression.  0 implies the lump is not compressed.
	int		uncompressedSize; // default to zero
};

struct dheader_t
{
	int			ident;
	int			version;
	lump_t		lumps[ HEADER_LUMPS ];
	int			mapRevision;				// the map's revision (iteration, version) number (added BSPVERSION 6)
};

// planes (x&~1) and (x&~1)+1 are always opposites
struct dplane_t
{
	// (fiend) Valve used a data type they call a vector here for the normal, i don't have that so i'm just manually doing it because fuck you
	float normalX;
	float normalY;
	float normalZ;
	float	dist;
	int		type;		// PLANE_X - PLANE_ANYZ ?remove? trivial to regenerate
};

struct dface_t
{
	unsigned short	planenum;
	unsigned char		side;	// faces opposite to the node's plane direction
	unsigned char		onNode; // 1 of on node, 0 if in leaf

	int			firstedge;		// we must support > 64k edges
	short		numedges;
	short		texinfo;
	// This is a union under the assumption that a fog volume boundary (ie. water surface)
	// isn't a displacement map.
	// FIXME: These should be made a union with a flags or type field for which one it is
	// if we can add more to this.
//	union
//	{
	short       dispinfo;
	// This is only for surfaces that are the boundaries of fog volumes
	// (ie. water surfaces)
	// All of the rest of the surfaces can look at their leaf to find out
	// what fog volume they are in.
	short		surfaceFogVolumeID;
	//	};

		// lighting info
	unsigned char		styles[ MAXLIGHTMAPS ];
	int			lightofs;		// start of [numstyles*surfsize] samples
	float       area;

	// TODO: make these unsigned chars?
	int			m_LightmapTextureMinsInLuxels[ 2 ];
	int			m_LightmapTextureSizeInLuxels[ 2 ];

	int origFace;				// reference the original face this face was derived from

public:

	unsigned short GetNumPrims() const;
	void SetNumPrims( unsigned short nPrims );
	bool AreDynamicShadowsEnabled();
	void SetDynamicShadowsEnabled( bool bEnabled );

	// non-polygon primitives (strips and lists)
private:
	unsigned short m_NumPrims;	// Top bit, if set, disables shadows on this surface (this is why there are accessors).

public:
	unsigned short	firstPrimID;

	unsigned int	smoothingGroups;
};

inline unsigned short dface_t::GetNumPrims() const
{
	return m_NumPrims & 0x7FFF;
}

inline void dface_t::SetNumPrims( unsigned short nPrims )
{
	// (fiend) "Assert" changed to "assert" because one is valve defined and one is c++ standard
	assert( ( nPrims & 0x8000 ) == 0 );
	m_NumPrims &= ~0x7FFF;
	m_NumPrims |= ( nPrims & 0x7FFF );
}

inline bool dface_t::AreDynamicShadowsEnabled()
{
	return ( m_NumPrims & 0x8000 ) == 0;
}

inline void dface_t::SetDynamicShadowsEnabled( bool bEnabled )
{
	if (bEnabled)
		m_NumPrims &= ~0x8000;
	else
		m_NumPrims |= 0x8000;
}

typedef struct texinfo_s
{
	float		textureVecsTexelsPerWorldUnits[ 2 ][ 4 ];			// [s/t][xyz offset]
	float		lightmapVecsLuxelsPerWorldUnits[ 2 ][ 4 ];			// [s/t][xyz offset] - length is in units of texels/area
	int			flags;				// miptex flags + overrides
	int			texdata;			// Pointer to texture name, size, etc.
} texinfo_t;

#define TEXTURE_NAME_LENGTH	 128			// changed from 64 BSPVERSION 8

struct dtexdata_t
{
	// (fiend) once again splitting a vector into 3 floats because fuck you
	float		reflectivity[ 3 ];
	int			nameStringTableID;				// index into g_StringTable for the texture name
	int			width, height;					// source image
	int			view_width, view_height;		//
};

struct dedge_t
{
	unsigned short	v[ 2 ];	// vertex indices
};

struct dvertex_t
{
	// more vector to 3 floats. fuck you
	float	vertX;
	float	vertY;
	float	vertZ;
	
	// constructors
	dvertex_t()
	{
		vertX = 0;
		vertY = 0;
		vertZ = 0;
	}
	dvertex_t(dvertex_t &vert)
	{
		vertX = vert.vertX;
		vertY = vert.vertY;
		vertZ = vert.vertZ;
	}
	dvertex_t(float x, float y, float z)
	{
		vertX = x;
		vertY = y;
		vertZ = z;
	}
	
	// operators
	struct dvertex_t& operator+=(dvertex_t rhs)
	{
		vertX += rhs.vertX;
		vertY += rhs.vertY;
		vertZ += rhs.vertZ;
		return *this;
	}
	struct dvertex_t& operator-=(dvertex_t rhs)
	{
		vertX -= rhs.vertX;
		vertY -= rhs.vertY;
		vertZ -= rhs.vertZ;
		return *this;
	}
	struct dvertex_t& operator*=(float rhs)
	{
		vertX *= rhs;
		vertY *= rhs;
		vertZ *= rhs;
		return *this;
	}
	struct dvertex_t& operator/=(float rhs)
	{
		vertX /= rhs;
		vertY /= rhs;
		vertZ /= rhs;
		return *this;
	}

	// vertex operators
	dvertex_t& operator+(dvertex_t rhs)
	{
		rhs.vertX += vertX;
		rhs.vertY += vertY;
		rhs.vertZ += vertZ;
		return rhs;
	}
	dvertex_t& operator-(dvertex_t rhs)
	{
		rhs.vertX -= vertX;
		rhs.vertY -= vertY;
		rhs.vertZ -= vertZ;
		return rhs;
	}
	dvertex_t& operator*(float rhs)
	{
		dvertex_t tempVert(*this);
		vertX *= rhs;
		vertY *= rhs;
		vertZ *= rhs;
		return *this;
	}
	dvertex_t& operator/(float rhs)
	{
		dvertex_t tempVert(*this);
		vertX /= rhs;
		vertY /= rhs;
		vertZ /= rhs;
		return *this;
	}
};


