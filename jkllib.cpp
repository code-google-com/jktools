#include "jkllib.h"
#include "jkllib_local.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <ctype.h>

jkl_library* jkl_init( jkl_malloc a, jkl_free f )
{
	jkl_library* lib = (jkl_library*)a( sizeof( jkl_library ) );
	lib->_malloc = a;
	lib->_free = f;
	return lib;
}

void jkl_quit( jkl_library* lib )
{
	lib->_free( lib );
}

#ifndef HAVE_STRNCASECMP
static int strncasecmp(const char *s1, const char *s2, size_t n)
{
	if (n == 0)
		return 0;

	while ((n-- != 0)
			&& (tolower(*(unsigned char *) s1) ==
			tolower(*(unsigned char *) s2))) {
		if (n == 0 || *s1 == '\0' || *s2 == '\0')
			return 0;
		s1++;
		s2++;
	}
	return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
}
#endif // HAVE_STRNCASECMP

jkl_section_name jkl_sections[] =
{
	{ SECTION_JK, "JK" },
	{ SECTION_COPYRIGHT, "COPYRIGHT" },
	{ SECTION_HEADER, "HEADER" },
	{ SECTION_SOUNDS, "SOUNDS" },
	{ SECTION_MATERIALS, "MATERIALS" },
	{ SECTION_GEORESOURCE, "GEORESOURCE" },
	{ SECTION_SECTORS, "SECTORS" },
	{ SECTION_AICLASS, "AICLASS" },
	{ SECTION_MODELS, "MODELS" },
	{ SECTION_SPRITES, "SPRITES" },
	{ SECTION_KEYFRAMES, "KEYFRAMES" },
	{ SECTION_ANIMCLASS, "ANIMCLASS" },
	{ SECTION_SOUNDCLASS, "SOUNDCLASS" },
	{ SECTION_COGSCRIPTS, "COGSCRIPTS" },
	{ SECTION_COGS, "COGS" },
	{ SECTION_TEMPLATES, "TEMPLATES" },
	{ SECTION_THINGS, "THINGS" },
	{ SECTION_ARCHLIGHTING, "ARCHLIGHTING" },
	{ SECTION_LIGHTS, "LIGHTS" },
};

static e_jkl_section get_section( const char* line )
{
	e_jkl_section sect = SECTION_INVALID;

	char sectionTitle[64] = {0};
	sscanf( line, "SECTION: %s\n", &sectionTitle );
	if( 0 == sectionTitle[0] )
		sscanf( line, "Section: %s\n", &sectionTitle );

	assert( strlen( sectionTitle ) );
	for( int i = 0; i < SECTION_MAX; i++ )
	{
		const char* st = jkl_sections[i]._sectionTitle;
		if( 0 == strncasecmp( sectionTitle, st, strlen( st ) ) )
		{
			fprintf( stderr, "found section %s\n", st );
			sect = jkl_sections[i]._section;
			break;
		}
	}

	return sect;
}

static bool is_comment( const char* line )
{
	if( '#' == line[0] )
		return true;
	return false;
}

const char* skip_words( const char* line, int num_words )
{
	int skipped = 0;
	while( *line && skipped < num_words )
	{
		while( !isspace( *line++ ) )
			;
		while( isspace( *line ) )
			line++;
		skipped++;
	}
	return line;
}

static void read_section( FILE* jkl_file, jkl_data* jkl, jkl_section_line_parser parser, void* parser_data )
{
	char buffer[MAX_LINE_LENGTH];

	while( !feof( jkl_file ) )
	{
		fpos_t pos;
		fgetpos( jkl_file, &pos );
		const char* line = fgets( buffer, MAX_LINE_LENGTH, jkl_file );

		if( !line )
			return;	

		if( is_comment( line ) )
			continue;

		if( 1 == strlen( line ) )
			continue;

		if( 0 == strncasecmp( line, "SECTION:", strlen("SECTION:") ) )
		{
			fsetpos( jkl_file, &pos );
			return;
		}

		parser( line, jkl, parser_data );
	}
}

static void header_line_parser( const char* line, jkl_data* data, void* parser_data )
{
	if( 0 == strncmp( line, "Version", strlen("Version") ) )
	{
		sscanf( line, "Version %d", &data->_version );
	}
}

static void georesource_line_parser( const char* line, jkl_data* data, void* parser_data )
{
	jkl_georesource_parser_data* gr_data = (jkl_georesource_parser_data*)parser_data;

	// find a section if we don't have one
	if( !isdigit(line[0]) )
	{
		if( 0 == strncmp( line, "World Colormaps", strlen( "World Colormaps" ) ) )
		{
			gr_data->_mode = jkl_georesource_parser_data::GR_COLORMAPS;
			//sscanf( line, "World Colormaps %d\n", &data->_numColormaps );
		}
		else if( 0 == strncmp( line, "World vertices", strlen( "World vertices" ) ) )
		{
			gr_data->_mode = jkl_georesource_parser_data::GR_VERTICES;
			sscanf( line, "World vertices %d\n", &data->_numVerts );
		}
		else if( 0 == strncmp( line, "World texture vertices", strlen( "World texture vertices" ) ) )
		{
			gr_data->_mode = jkl_georesource_parser_data::GR_TEXTURE_VERTICES;
			sscanf( line, "World texture vertices %d\n", &data->_numTextureVerts );
		}
		else if( 0 == strncmp( line, "World adjoins", strlen( "World adjoins" ) ) )
		{
			gr_data->_mode = jkl_georesource_parser_data::GR_ADJOINS;
			//sscanf( line, "World adjoins %d\n", &data->_numAdjoins );
		}
		else if( 0 == strncmp( line, "World surfaces", strlen( "World surfaces" ) ) )
		{
			gr_data->_mode = jkl_georesource_parser_data::GR_SURFACES;
			sscanf( line, "World surfaces %d\n", &data->_numSurfaces );
		}
		else
		{
			assert( false );
		}
		return;
	}

	assert( gr_data->_mode != jkl_georesource_parser_data::GR_INVALID );

	switch( gr_data->_mode )
	{
	case jkl_georesource_parser_data::GR_COLORMAPS:
		break;
	case jkl_georesource_parser_data::GR_VERTICES:
		{
			struct _gr_vertex
			{
				int			_index;
				float		_xyz[3];
			};
			_gr_vertex gv;
			sscanf( line, "%d: %f %f %f\n", 
				&gv._index, &gv._xyz[0], &gv._xyz[1],  &gv._xyz[2] );
			assert( gv._index >= 0 && gv._index < data->_numVerts );
			data->_verts[ gv._index ]._xyz[0] = gv._xyz[0];
			data->_verts[ gv._index ]._xyz[1] = gv._xyz[1];
			data->_verts[ gv._index ]._xyz[2] = gv._xyz[2];
		}
		break;
	case jkl_georesource_parser_data::GR_TEXTURE_VERTICES:
		{
			struct _gr_texture_vertex
			{
				int			_index;
				float		_uv[2];
			};
			_gr_texture_vertex gtv;
			sscanf( line, "%d: %f %f\n", 
				&gtv._index, &gtv._uv[0], &gtv._uv[1] );
			assert( gtv._index >= 0 && gtv._index < data->_numTextureVerts );
			data->_textureVerts[ gtv._index ]._uv[0] = gtv._uv[0];
			data->_textureVerts[ gtv._index ]._uv[1] = gtv._uv[1];
		}
		break;
	case jkl_georesource_parser_data::GR_ADJOINS:
		break;
	case jkl_georesource_parser_data::GR_SURFACES:
		{
			struct _gr_surface_static
			{
				int			_index;
				int			_material;
				int			_surfflags;
				int			_faceflags;
				int			_geo;
				int			_light;
				int			_tex;
				int			_adjoin;
				float		_extralight;
				int			_numVerts;
			};
			_gr_surface_static gss;
			sscanf( line, "%d: %d 0x%x 0x%x %d %d %d %d %f %d\n", 
				&gss._index, &gss._material, &gss._surfflags, &gss._faceflags,
				&gss._geo, &gss._light, &gss._tex, &gss._adjoin, 
				&gss._extralight, &gss._numVerts
				);

			data->_surfaces[ gss._index ]._material = gss._material;
			data->_surfaces[ gss._index ]._surfflags = gss._surfflags;
			data->_surfaces[ gss._index ]._faceflags = gss._faceflags;
			data->_surfaces[ gss._index ]._geo = gss._geo;
			data->_surfaces[ gss._index ]._light = gss._light;
			data->_surfaces[ gss._index ]._tex = gss._tex;
			data->_surfaces[ gss._index ]._adjoin = gss._adjoin;
			data->_surfaces[ gss._index ]._extralight = gss._extralight;
			data->_surfaces[ gss._index ]._numVerts = gss._numVerts;
			data->_surfaces[ gss._index ]._firstVert = data->_numSurfaceVerts;

			const char* vertex_data = skip_words( line, 10 );
			for( int v = 0; v < gss._numVerts; v++ )
			{
				jkl_surface_vertex& jsv = data->_surfaceVerts[data->_numSurfaceVerts + v];
				sscanf( vertex_data, "%d,%d", &jsv._vert, &jsv._texVert );
				vertex_data = skip_words( vertex_data, 1 );
			}

			data->_numSurfaceVerts += gss._numVerts;

			// surface normals aren't explicitly delimited,
			// they just come after the surfaces (1-1 mapping)
			gr_data->_numReadSurfaces++;
			if( gr_data->_numReadSurfaces == data->_numSurfaces )
				gr_data->_mode = jkl_georesource_parser_data::GR_SURFACE_NORMALS;
		}
		break;
	case jkl_georesource_parser_data::GR_SURFACE_NORMALS:
		{
			struct _gr_normal
			{
				int			_index;
				float		_nrm[3];
			};
			_gr_normal gn;
			sscanf( line, "%d: %f %f %f\n", 
				&gn._index, &gn._nrm[0], &gn._nrm[1],  &gn._nrm[2] );
			assert( gn._index >= 0 && gn._index < data->_numSurfaces );
			data->_surfaces[ gn._index ]._nrm[0] = gn._nrm[0];
			data->_surfaces[ gn._index ]._nrm[1] = gn._nrm[1];
			data->_surfaces[ gn._index ]._nrm[2] = gn._nrm[2];
		}
		break;
	default:
		assert( false );
		break;
	}
}

static void sector_line_parser( const char* line, jkl_data* data, void* parser_data )
{
	jkl_sector_parser_data* sc_data = (jkl_sector_parser_data*)parser_data;
	jkl_sector& sec = data->_sectors[ sc_data->_numReadSectors ];

	if( 0 == data->_numSectors )
	{
		// read the sector chunk data
		sscanf( line, "World sectors %d\n", &data->_numSectors );
	}
	else if( !isdigit( line[0] ) )
	{
		// read this sector
		if( 0 == strncmp( line, "FLAGS", strlen( "FLAGS" ) ) )
		{
			sscanf( line, "FLAGS 0x%x\n", &sec._flags );
		}
		else if( 0 == strncmp( line, "AMBIENT LIGHT", strlen( "AMBIENT LIGHT" ) ) )
		{
			sscanf( line, "AMBIENT LIGHT %f\n", &sec._ambientLight );
		}
		else if( 0 == strncmp( line, "EXTRA LIGHT", strlen( "EXTRA LIGHT" ) ) )
		{
			sscanf( line, "EXTRA LIGHT %f\n", &sec._extraLight );
		}
		else if( 0 == strncmp( line, "COLORMAP", strlen( "COLORMAP" ) ) )
		{
			sscanf( line, "COLORMAP %d\n", &sec._colorMap );
		}
		else if( 0 == strncmp( line, "TINT", strlen( "TINT" ) ) )
		{
			sscanf( line, "TINT %f %f %f\n", &sec._tint[0], &sec._tint[1], &sec._tint[2] );
		}
		else if( 0 == strncmp( line, "BOUNDBOX", strlen( "BOUNDBOX" ) ) )
		{
			sscanf( line, "BOUNDBOX %f %f %f %f %f %f\n", 
				&sec._boundBox._min[0], &sec._boundBox._min[1], &sec._boundBox._min[2],
				&sec._boundBox._max[0], &sec._boundBox._max[1], &sec._boundBox._max[2] );
		}
		else if( 0 == strncmp( line, "COLLIDEBOX", strlen( "COLLIDEBOX" ) ) )
		{
			sscanf( line, "COLLIDEBOX %f %f %f %f %f %f\n", 
				&sec._collideBox._min[0], &sec._collideBox._min[1], &sec._collideBox._min[2],
				&sec._collideBox._max[0], &sec._collideBox._max[1], &sec._collideBox._max[2] );
		}
		else if( 0 == strncmp( line, "CENTER", strlen( "CENTER" ) ) )
		{
			sscanf( line, "CENTER %f %f %f\n", &sec._center[0], &sec._center[1], &sec._center[2] );
		}
		else if( 0 == strncmp( line, "RADIUS", strlen( "RADIUS" ) ) )
		{
			sscanf( line, "RADIUS %f\n", &sec._radius );
		}
		else if( 0 == strncmp( line, "VERTICES", strlen( "VERTICES" ) ) )
		{
			sscanf( line, "VERTICES %d\n", &sec._numVerts );
			sec._firstVert = data->_numSectorVerts;
		}
		else if( 0 == strncmp( line, "SURFACES", strlen( "SURFACES" ) ) )
		{
			sscanf( line, "SURFACES %d %d\n", &sec._firstSurface, &sec._numSurfaces );
			sc_data->_numReadSectors++;
		}
	}
	else
	{
		// we must be reading vertices at this point
		assert( sec._numVerts > 0 );
		struct _sc_vertex
		{
			int			_index;
			int			_vert;
		};
		_sc_vertex sv;
		sscanf( line, "%d: %d\n", &sv._index, &sv._vert );
		assert( sv._index >= 0 && sv._index < data->_numVerts );
		data->_sectorVerts[ data->_numSectorVerts ] = sv._vert;
		data->_numSectorVerts++;
	}
}

static void skip_line_parser( const char* line, jkl_data* data, void* parser_data )
{
}

static void read_data( FILE* jkl_file, jkl_data* jkl )
{
	char buffer[MAX_LINE_LENGTH];

	while( !feof( jkl_file ) )
	{
		const char* line = fgets( buffer, MAX_LINE_LENGTH, jkl_file );
		if( !line )
			break;

		if( is_comment( line ) )
			continue;

		if( 1 == strlen( line ) )
			continue;

		assert( 0 == strncasecmp( line, "SECTION:", strlen("SECTION:") ) );
		e_jkl_section sect = get_section( line );
		assert( SECTION_INVALID != sect );
		
		switch( sect )
		{
		case SECTION_HEADER:
			{
				read_section( jkl_file, jkl, header_line_parser, NULL );
				assert( -1 != jkl->_version );
				break;
			}
		case SECTION_GEORESOURCE:
			{
				jkl_georesource_parser_data data;
				read_section( jkl_file, jkl, georesource_line_parser, &data );
				break;
			}
		case SECTION_SECTORS:
			{
				jkl_sector_parser_data data;
				read_section( jkl_file, jkl, sector_line_parser, &data );
				break;
			}
		default:
			read_section( jkl_file, jkl, skip_line_parser, NULL );
			break;
		};
	}
}

jkl_data* jkl_open( jkl_library* lib, const char* fname )
{
	jkl_data* jkl = 0;
	FILE* jkl_file = fopen( fname, "rt" );
	if( jkl_file )
	{
		jkl = (jkl_data*)lib->_malloc( sizeof(jkl_data) );
		memset( jkl, 0, sizeof(jkl_data) );
		jkl->_version = -1;
		read_data( jkl_file, jkl );
	}
	return jkl;
}

void jkl_close( jkl_library* lib, jkl_data* jkl )
{
	lib->_free( jkl );
}
