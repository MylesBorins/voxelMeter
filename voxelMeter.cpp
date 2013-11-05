//-----------------------------------------------------------------------------
// name: vMeter.cpp
// desc: draw a fancy voxel
//
// author: Myles Borins
//   date: fall 2013
//
//-----------------------------------------------------------------------------
#include "RtAudio.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
using namespace std;

#ifdef __MACOSX_CORE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include "VoxeLib.h"
#include "chuck_fft.h"
#include "Thread.h"


//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
void initGfx();
void setupMatrix();
void idleFunc();
void displayFunc();
void reshapeFunc( GLsizei width, GLsizei height );
void keyboardFunc( unsigned char, int, int );
void mouseFunc( int button, int state, int x, int y );
void specialFunc( int, int, int );

// our datetype
#define SAMPLE MY_FLOAT
// corresponding format for RtAudio
#define MY_FORMAT RTAUDIO_FLOAT32
// sample rate
#define MY_SRATE 44100
// number of channels
#define MY_CHANNELS 1
// for convenience
#define MY_PIE 3.14159265358979
// for fft
#define SND_BUFFER_SIZE 1024
#define SND_FFT_SIZE ( SND_BUFFER_SIZE * 2 )

#define GRIDWIDTH 32
#define GRIDHEIGHT 32
#define GRIDDEPTH 32

// width and height
GLsizei g_width = 1024;
GLsizei g_height = 720;
GLsizei g_last_width = g_width;
GLsizei g_last_height = g_height;

vector<VuGrid *> vuGrids;
// VuGrid * vuGrid = new VuGrid;

bool isFull = FALSE;

// global audio buffer
SAMPLE g_fft_buffer[SND_FFT_SIZE];
SAMPLE g_buffer[SND_BUFFER_SIZE]; // latest mono buffer (possibly preview)
long g_bufferSize = SND_BUFFER_SIZE;
GLint g_fft_size = SND_FFT_SIZE;

float g_prevBuffers[GRIDDEPTH][SND_BUFFER_SIZE];

// Global Variables
double g_gridWidth = GRIDWIDTH; 
double g_gridHeight = GRIDHEIGHT;

int g_wireframe = false;

Mutex g_mutex;

bool g_ready = FALSE;

Vector3d g_bottomLeft(0,0,0);

//-----------------------------------------------------------------------------
// name: callme()
// desc: audio callback
//-----------------------------------------------------------------------------
int callme( void * outputBuffer, void * inputBuffer, unsigned int numFrames,
            double streamTime, RtAudioStreamStatus status, void * data )
{
    // lock
    g_mutex.lock();
    // cast!
    SAMPLE * input = (SAMPLE *)inputBuffer;
    SAMPLE * output = (SAMPLE *)outputBuffer;
    
    // copy
    memcpy( g_buffer, input, numFrames * sizeof(SAMPLE) );
    memset( output, 0, numFrames * sizeof(SAMPLE) );

    for( int i; i < numFrames; i ++ )
    {
        output[i] = 0;
    }
    
    // set flag
    g_ready = TRUE;
    // unlock
    g_mutex.unlock();
    
    
    return 0;
}

//-----------------------------------------------------------------------------
// name: main()
// desc: entry point
//-----------------------------------------------------------------------------
int main( int argc, char ** argv )
{
    for(int i = 0; i < GRIDDEPTH; i++)
    {
        vuGrids.push_back(new VuGrid);
        g_bottomLeft.m_z += i * -0.075;
        vuGrids[i]->init(g_gridWidth, g_gridHeight, g_bottomLeft, SND_BUFFER_SIZE, i * 0.05);
    }

    // instantiate RtAudio object
    RtAudio audio;
    // variables
    unsigned int bufferBytes = 0;
    // frame size
    unsigned int bufferFrames = SND_BUFFER_SIZE;
    
    
    // check for audio devices
    if( audio.getDeviceCount() < 1 )
    {
        // nopes
        cout << "no audio devices found!" << endl;
        exit( 1 );
    }
    // initialize GLUT
    glutInit( &argc, argv );
    // init gfx
    initGfx();

    // let RtAudio print messages to stderr.
    audio.showWarnings( true );

    // set input and output parameters
    RtAudio::StreamParameters iParams, oParams;
    iParams.deviceId = audio.getDefaultInputDevice();
    iParams.nChannels = MY_CHANNELS;
    iParams.firstChannel = 0;
    oParams.deviceId = audio.getDefaultOutputDevice();
    oParams.nChannels = MY_CHANNELS;
    oParams.firstChannel = 0;

    // create stream options
    RtAudio::StreamOptions options;

    // go for it
    try {
        // open a stream
        audio.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames, &callme, (void *)&bufferBytes, &options );
    }
    catch( RtError& e )
    {
        // error!
        cout << e.getMessage() << endl;
        exit( 1 );
    }

    // compute
    bufferBytes = bufferFrames * MY_CHANNELS * sizeof(SAMPLE);
    // allocate global buffer
    // g_bufferSize = bufferFrames;
    // g_buffer = new SAMPLE[g_bufferSize];
    memset( g_buffer, 0, sizeof(SAMPLE)*g_bufferSize );
    // clean dose buffers

    for (int i = 0; i < GRIDDEPTH; i++)
    {
        memset( g_prevBuffers[i], 0, sizeof(SAMPLE)*g_bufferSize);
    }

    // go for it
    try {
        // start stream
        audio.startStream();

        // let GLUT handle the current thread from here
        glutMainLoop();
        
        // stop the stream.
        audio.stopStream();
    }
    catch( RtError& e )
    {
        // print error message
        cout << e.getMessage() << endl;
    }
    
    if( audio.isStreamOpen() )
        audio.closeStream();
    
    // delete vuGrid;
    vuGrids.erase(vuGrids.begin(), vuGrids.end());
    return 0;
}




//-----------------------------------------------------------------------------
// Name: reshapeFunc( )
// Desc: called when window size changes
//-----------------------------------------------------------------------------
void initGfx()
{
    // double buffer, use rgb color, enable depth buffer
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    // initialize the window size
    glutInitWindowSize( g_width, g_height );
    // set the window postion
    glutInitWindowPosition( 100, 100 );
    // create the window
    glutCreateWindow( "vMeter2" );
    
    // set the idle function - called when idle
    glutIdleFunc( idleFunc );
    // set the display function - called when redrawing
    glutDisplayFunc( displayFunc );
    // set the reshape function - called when client area changes
    glutReshapeFunc( reshapeFunc );
    // set the keyboard function - called on keyboard events
    glutKeyboardFunc( keyboardFunc );
    // set the mouse function - called on mouse stuff
    glutMouseFunc( mouseFunc );
    // set the specialkey function
    glutSpecialFunc( specialFunc );
    
    // set clear color
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    // set shade mode
    glShadeModel (GL_FLAT);
    // enable color material
    glEnable( GL_COLOR_MATERIAL );
    // enable depth test
    glEnable( GL_DEPTH_TEST );
    // enable alpha
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    // print opengl version
    // cout << glGetString( GL_VERSION ) << endl;
}

//-----------------------------------------------------------------------------
// Name: reshapeFunc( )
// Desc: called when window size changes
//-----------------------------------------------------------------------------
void reshapeFunc( GLsizei w, GLsizei h )
{
    // save the new window size
    g_width = w; g_height = h;
    // map the view port to the client area
    glViewport( 0, 0, w, h );
    // set the matrix mode to project
    glMatrixMode( GL_PROJECTION );
    // load the identity matrix
    glLoadIdentity( );
    // create the viewing frustum
    glFrustum (-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
    // set the matrix mode to modelview
    glMatrixMode( GL_MODELVIEW );
    // load the identity matrix
    glLoadIdentity( );
    // position the view point
    gluLookAt( 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f );
}

//-----------------------------------------------------------------------------
// Name: keyboardFunc( )
// Desc: key event
//-----------------------------------------------------------------------------
void keyboardFunc( unsigned char key, int x, int y )
{
    switch( key )
    {
        case 'f':
        case 'F':
            if(!isFull)
            {
                glutFullScreen();
                isFull = TRUE;
            }
            else {
                glutReshapeWindow(g_last_width, g_last_height);
                isFull = FALSE;
            }
            break;
        case 'w':
        case 'W':
            g_wireframe = !g_wireframe;
            for(int i = 0; i < GRIDDEPTH; i++)
            {
                vuGrids[i]->setWireframe(g_wireframe);
            }
            break;
        case 'Q':
        case 'q':
            exit(1);
            break;
    }
    
    glutPostRedisplay( );
}




//-----------------------------------------------------------------------------
// Name: mouseFunc( )
// Desc: handles mouse stuff
//-----------------------------------------------------------------------------
void mouseFunc( int button, int state, int x, int y )
{
    if( button == GLUT_LEFT_BUTTON )
    {
        // when left mouse button is down
        if( state == GLUT_DOWN )
        {
        }
        else
        {
        }
    }
    else if ( button == GLUT_RIGHT_BUTTON )
    {
        // when right mouse button down
        if( state == GLUT_DOWN )
        {
        }
        else
        {
        }
    }
    else
    {
    }
    
    glutPostRedisplay( );
}

//-----------------------------------------------------------------------------
// Name: mouseFunc( )
// Desc: handles mouse stuff
//-----------------------------------------------------------------------------
void specialFunc( int key, int x, int y ) {

    if (key == GLUT_KEY_RIGHT)
    {
        if(g_gridWidth < 128)
        {
            g_gridWidth++;
            for(int i = 0; i < GRIDDEPTH; i++)
            {
                vuGrids[i]->resizeGrid(g_gridWidth, g_gridHeight);
            }
        }
    }
 

    else if (key == GLUT_KEY_LEFT)
    {
        if (g_gridWidth > 1)
        {
            g_gridWidth--;
            for(int i = 0; i < GRIDDEPTH; i++)
            {
                vuGrids[i]->resizeGrid(g_gridWidth, g_gridHeight);
            }
        }
    }
     
    else if (key == GLUT_KEY_UP)
    {
        if(g_gridHeight < 128)
        {
            g_gridHeight++;
            for(int i = 0; i < GRIDDEPTH; i++)
            {
                vuGrids[i]->resizeGrid(g_gridWidth, g_gridHeight);
            }
        }
    }
     
    else if (key == GLUT_KEY_DOWN)
    {
        if (g_gridHeight > 1)
        {
            g_gridHeight--;
            for(int i = 0; i < GRIDDEPTH; i++)
            {
                vuGrids[i]->resizeGrid(g_gridWidth, g_gridHeight);
            }
        }
    }
 
    //  Request display update
    glutPostRedisplay();
 
}


//-----------------------------------------------------------------------------
// Name: idleFunc( )
// Desc: callback from GLUT
//-----------------------------------------------------------------------------
void idleFunc( )
{
    // render the scene
    glutPostRedisplay( );
}

//-----------------------------------------------------------------------------
// Name: displayFunc( )
// Desc: callback function invoked to draw the client area
//-----------------------------------------------------------------------------
void displayFunc( )
{
    // local variables
    SAMPLE * buffer = g_fft_buffer;
    
    // clear the color and depth buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // Reset transformations
    glLoadIdentity();
    gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    
    // save current matrix state
    glPushMatrix( );
        // wait for data
        while( !g_ready ) usleep( 1000 );

        // lock
        g_mutex.lock();
        
        // copy currently playing audio into buffer
        memcpy( buffer, g_buffer, g_bufferSize * sizeof(SAMPLE) );
        
        // some flag (hand off to audio cb thread)
        g_ready = FALSE;

        // unlock
        g_mutex.unlock();
        
        vuGrids[0]->updateBuffer(buffer);
        for(int i = 1; i < GRIDDEPTH; i++){
            vuGrids[i]->updateBuffer(g_prevBuffers[i]);
        }
        
        glTranslatef(-1.6,-1.5,0);
        for(int i = GRIDDEPTH - 1; i >= 0; i--)
        {
            vuGrids[i]->drawMe();
        }
    
    // restore matrix state
    glPopMatrix( );
    
    glFlush ();
    
    // swap the double buffer
    glutSwapBuffers( );
    
    for ( int i = GRIDDEPTH - 1; i >= 1; i--)
    {
        memcpy( g_prevBuffers[i], g_prevBuffers[i - 1], g_bufferSize * sizeof(SAMPLE) );
    }
    memcpy( g_prevBuffers[0], buffer, g_bufferSize * sizeof(SAMPLE) );
}
