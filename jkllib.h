// public header for the jkl library
#ifndef JKLLIB_H
#define JKLLIB_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef void* (*jkl_malloc)( size_t bytes );
typedef void (*jkl_free)( void* ptr );

struct jkl_library
{
	jkl_malloc	_malloc;
	jkl_free	_free;
};

jkl_library* jkl_init( jkl_malloc a, jkl_free f );
void jkl_quit( jkl_library* lib );

#define MAX_VERTS		262144
#define MAX_SURFACES	65536
#define MAX_SECTORS		4096

struct jkl_vertex
{
	float		_xyz[3];
};

struct jkl_texture_vertex
{
	float		_uv[2];
};

struct jkl_adjoin
{
	int			_flags;
	int			_mirror;
	float		_dist;
};

struct jkl_surface
{
	int			_material;
	int			_surfflags;
	int			_faceflags;
	int			_geo;
	int			_light;
	int			_tex;
	int			_adjoin;
	float		_extralight;

	int			_firstVert; // index into _surfaceVerts
	int			_numVerts;

	float		_nrm[3];
};

struct jkl_surface_vertex
{
	int			_vert;
	int			_texVert;
};

struct jkl_box
{
	float		_min[3];
	float		_max[3];
};

struct jkl_sector
{
	int			_flags;
	float		_ambientLight;
	float		_extraLight;
	int			_colorMap;
	float		_tint[3];
	jkl_box		_boundBox;
	jkl_box		_collideBox;
	float		_center[3];
	float		_radius;

	int			_firstVert; // index into _sectorVerts
	int			_numVerts;

	int			_firstSurface;
	int			_numSurfaces;
};

struct jkl_data
{
	int					_version;

	int					_numVerts;
	jkl_vertex			_verts[MAX_VERTS];

	int					_numTextureVerts;
	jkl_texture_vertex	_textureVerts[MAX_VERTS];

	int					_numSurfaces;
	jkl_surface			_surfaces[MAX_SURFACES];

	int					_numSurfaceVerts;
	jkl_surface_vertex	_surfaceVerts[MAX_VERTS];

	int					_numSectors;
	jkl_sector			_sectors[MAX_SECTORS];

	int					_numSectorVerts;
	int					_sectorVerts[MAX_VERTS];
};

jkl_data* jkl_open( jkl_library* lib, const char* fname );
void jkl_close( jkl_library* lib, jkl_data* d );

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // JKLLIB_H
