// private header for the jkl library
#ifndef JKLLIB_LOCAL_H
#define JKLLIB_LOCAL_H

#define MAX_LINE_LENGTH 4096

enum e_jkl_section
{
	SECTION_JK,
	SECTION_COPYRIGHT,
	SECTION_HEADER,
	SECTION_SOUNDS,
	SECTION_MATERIALS,
	SECTION_GEORESOURCE,
	SECTION_SECTORS,
	SECTION_AICLASS,
	SECTION_MODELS,
	SECTION_SPRITES,
	SECTION_KEYFRAMES,
	SECTION_ANIMCLASS,
	SECTION_SOUNDCLASS,
	SECTION_COGSCRIPTS,
	SECTION_COGS,
	SECTION_TEMPLATES,
	SECTION_THINGS,
	SECTION_ARCHLIGHTING,
	SECTION_LIGHTS,
	SECTION_MAX,
	SECTION_INVALID = 0xffffffff
};

struct jkl_section_name
{
	e_jkl_section	_section;
	char*			_sectionTitle;
};

typedef void (*jkl_section_line_parser)( const char* line, jkl_data* data, void* parser_data );

struct jkl_georesource_parser_data
{
	enum e_subsection
	{
		GR_COLORMAPS,
		GR_VERTICES,
		GR_TEXTURE_VERTICES,
		GR_ADJOINS,
		GR_SURFACES,
		GR_SURFACE_NORMALS,
		GR_MAX,
		GR_INVALID = 0xffffffff
	};

	e_subsection	_mode;
	int				_numReadSurfaces;

	jkl_georesource_parser_data()
		: _mode( GR_INVALID ),
		_numReadSurfaces( 0 )
	{
	}
};

struct jkl_sector_parser_data
{
	int				_numReadSectors;

	jkl_sector_parser_data()
		: _numReadSectors( 0 )
	{
	}
};

#endif // JKLLIB_LOCAL_H
