#define _USE_MATH_DEFINES
#include <cmath>
#include <cassert>
#include <malloc.h>
#include "glut.h"
#include "jkllib.h"

static jkl_library* _jkl_lib = 0;
static jkl_data* _jkl = 0;

static float _jkl_display_scale = 10.0f;

static float _viewerPos[3] = { 4, 4, 2 };
static float _viewerYaw = 0.75f * M_PI;

static float _light0Diffuse[4] = { 0.5f, 0.75f, 1.0f, 1.0f };
static float _light0Position[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
static float _light1Diffuse[4] = { 1.0f, 0.75f, 0.5f, 1.0f };
static float _light1Position[4] = {-1.0f,-1.0f, 1.0f, 0.0f };

static bool _showWireframe = false;

static void app_exit( )
{
	if( _jkl )
		jkl_close( _jkl_lib, _jkl );

	jkl_quit( _jkl_lib );
}

static void app_init( const char* fname )
{
	_jkl_lib = jkl_init( malloc, free );
	atexit( app_exit );
	_jkl = jkl_open( _jkl_lib, fname );
	assert( _jkl );
}

static void idle( )
{
	glutPostRedisplay( );
}

static void key( unsigned char key, int x, int y )
{
	switch( key )
	{
	case 27:
		exit(0);
		break;
	case 'w':
		_showWireframe = !_showWireframe;
		break;
	default:
		break;
	};
}

static void special( int key, int x, int y )
{
	float lookAtVec[3] = { cosf( _viewerYaw ), -sinf( _viewerYaw ), 0.0f };

	switch( key )
	{
	case GLUT_KEY_UP:
		_viewerPos[0] += lookAtVec[0];
		_viewerPos[1] += lookAtVec[1];
		_viewerPos[2] += lookAtVec[2];
		break;
	case GLUT_KEY_DOWN:
		_viewerPos[0] -= lookAtVec[0];
		_viewerPos[1] -= lookAtVec[1];
		_viewerPos[2] -= lookAtVec[2];
		break;
	case GLUT_KEY_RIGHT:
		_viewerYaw += M_PI / 16.f;
		break;
	case GLUT_KEY_LEFT:
		_viewerYaw -= M_PI / 16.f;
		break;
	case GLUT_KEY_PAGE_UP:
		_viewerPos[2] += 1.0f;
		break;
	case GLUT_KEY_PAGE_DOWN:
		_viewerPos[2] -= 1.0f;
		break;
	default:
		break;
	}
}

static void display_axes( )
{
	glDisable( GL_LIGHTING );

	glBegin( GL_LINES );
		glColor3f( 1, 0, 0 );
		glVertex3f( 0, 0, 0 );
		glVertex3f( 1, 0, 0 );
		glColor3f( 0, 1, 0 );
		glVertex3f( 0, 0, 0 );
		glVertex3f( 0, 1, 0 );
		glColor3f( 0, 0, 1 );
		glVertex3f( 0, 0, 0 );
		glVertex3f( 0, 0, 1 );
	glEnd( );

	glEnable( GL_LIGHTING );
}

static void display( )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	float lookAtVec[3] = { cosf( _viewerYaw ), -sinf( _viewerYaw ), 0.0f };
	float lookAtPt[3] = { 
		_viewerPos[0] + lookAtVec[0], 
		_viewerPos[1] + lookAtVec[1],
		_viewerPos[2] + lookAtVec[2] };

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt(
		_viewerPos[0], _viewerPos[1], _viewerPos[2],
		lookAtPt[0], lookAtPt[1], lookAtPt[2],
		0, 0, 1
		);

	glLightfv( GL_LIGHT0, GL_POSITION, _light0Position );
	glLightfv( GL_LIGHT1, GL_POSITION, _light1Position );

	display_axes();

	for( int isec = 0; isec < _jkl->_numSectors; isec++ )
	{
		jkl_sector& sec = _jkl->_sectors[isec];
		for( int isurf = 0; isurf < sec._numSurfaces; isurf++ )
		{
			jkl_surface& sur = _jkl->_surfaces[sec._firstSurface + isurf];

			if( -1 != sur._material && sur._surfflags & 0x4 )
			{
				glEnable( GL_POLYGON_OFFSET_FILL );
				glPolygonOffset( 1.0f, 1.0f );
				glBegin( GL_TRIANGLE_FAN );
				glNormal3fv( sur._nrm );
				for( int ivert = 0; ivert < sur._numVerts; ivert++ )
				{
					jkl_vertex& vtx = _jkl->_verts[_jkl->_surfaceVerts[sur._firstVert + ivert]._vert];
					glVertex3f( vtx._xyz[0] * _jkl_display_scale, vtx._xyz[1] * _jkl_display_scale, vtx._xyz[2] * _jkl_display_scale );
				}
				glEnd();
				glDisable( GL_POLYGON_OFFSET_FILL );

				if( _showWireframe )
				{
					glDisable( GL_LIGHTING );
					glColor3f( 1, 1, 1 );
					glBegin( GL_LINE_LOOP );
					glNormal3fv( sur._nrm );
					for( int ivert = 0; ivert < sur._numVerts; ivert++ )
					{
						jkl_vertex& vtx = _jkl->_verts[_jkl->_surfaceVerts[sur._firstVert + ivert]._vert];
						glVertex3f( vtx._xyz[0] * _jkl_display_scale, vtx._xyz[1] * _jkl_display_scale, vtx._xyz[2] * _jkl_display_scale );
					}
					glEnd();
					glEnable( GL_LIGHTING );
				}
			}
		}
	}

	glutSwapBuffers( );
}

int main( int argc, char* argv[] )
{
	if( argc != 2 )
		return -1;

	app_init( argv[1] );

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize( 512, 512 );
	glutCreateWindow( "jkl viewer" );
	glutIdleFunc( idle );
	glutKeyboardFunc( key );
	glutSpecialFunc( special );
	glutDisplayFunc( display );

	glClearColor( 0.6f, 0.6f, 1.0f, 1.0f );
	glClearDepth( 1.0f );

	glLightfv( GL_LIGHT0, GL_DIFFUSE, _light0Diffuse );
	glLightfv( GL_LIGHT0, GL_POSITION, _light0Position );
	glLightfv( GL_LIGHT1, GL_DIFFUSE, _light1Diffuse );
	glLightfv( GL_LIGHT1, GL_POSITION, _light1Position );
	glEnable( GL_LIGHT0 );
	glEnable( GL_LIGHT1 );

	glEnable( GL_LIGHTING );

	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glDepthMask( GL_TRUE );

	glEnable( GL_CULL_FACE );

	glShadeModel( GL_SMOOTH );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 75.0f, 1.0f, 0.1f, 1000.0f );

	glutMainLoop( );

	return 0;
}
