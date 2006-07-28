#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <direct.h>
#include "gob.h"

static void make_dirs( const char* filepath )
{
	char subpath[GOB_PATH_LENGTH];
	const char* current = filepath;
	while( *current != 0 )
	{
		while( *current && *current != '\\' )
			current++;

		if( *current == '\\' )
		{
			strncpy( subpath, filepath, current - filepath );
			subpath[current - filepath] = 0;
			mkdir( subpath );
			current++;
		}
	}
}

int main( int argc, char* argv[] )
{
	if( argc != 2 )
	{
		fprintf( stderr, "usage: %s <file>\n", argv[0] );
		return -1;
	}

	FILE* f = fopen( argv[1], "rb" );
	if( f )
	{
		gob_header hdr;
		fread( &hdr, sizeof( gob_header ), 1, f );

		for( int i = 0; i < hdr._numItems; i++ )
		{
			gob_item itm;
			fread( &itm, sizeof( gob_item ), 1, f );
			make_dirs( itm._name );
			fprintf( stderr, "%s\n", itm._name );

			FILE* of = fopen( itm._name, "wb" );
			assert( of );
			{
				long pos = ftell( f );
				fseek( f, itm._offset, SEEK_SET );
				unsigned char* chunk = (unsigned char*)malloc( itm._length );
				fread( chunk, 1, itm._length, f );
				fwrite( chunk, 1, itm._length, of );
				free( chunk );
				fseek( f, pos, SEEK_SET );
			}
			fclose( of );
		}

		fclose( f );
	}
	return 0;
}
