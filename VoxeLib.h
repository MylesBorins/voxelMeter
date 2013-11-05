//-------------------------------------------------------------------------
// name: VoxeLib.h
// desc: some classes to play with VoxeLibs
//
// author: Myles Borins (mborins@ccrma.stanford.edu)
//   date: fall 2013
//
//   Inspired by https://sites.google.com/site/letsmakeaVoxeLibengine/
//-------------------------------------------------------------------------
#ifndef __VoxeLib_H__
#define __VoxeLib_H__

#ifdef __MACOSX_CORE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif
#include <vector>

class Vector3d
{
public:
    // constructor
    Vector3d( float x, float y, float z );
    // desctructor
    ~Vector3d( ){};
public:
    float m_x, m_y, m_z;
};

class Block
{
public:
    // contructor
    Block( );
    // destructor
    ~Block( );
  
public:
    void init( );
    void setColor( float r, float g, float b );
    void setAlpha( float a);
    void setPosition( float x, float y, float z );
    void setActive( bool active );
    void setWireframe( bool wireframe );
    void drawMe( );
    bool m_active;
protected:
    Vector3d * m_position;
    float * m_color;
    float m_alpha;
    
    bool m_wireframe;
};

class Column
{
public:
    // contructor
    Column( );
    ~Column( );
  
public:
    void init( int length, float x, float y, float z );
    void setAlphas( float alphas[]);
    void setColors( float colors[][3]);
    void setPosition( float x, float y, float z);
    void setWireframe( bool wireframe );
    void drawMe( );
protected:
    void updatePosition( );
    Block ** m_blocks;
    float ** m_colors;
    float * m_alphas;
    int m_length;
    Vector3d * m_position;
};

class Grid
{
public:
    Grid();
    ~Grid( );

public:
    void init( int width, int height, Vector3d position);
    void setBlockColor( int x, int y, float colors[3] );
    void setBlockAlpha( int x, int y, float alpha );
    void setColumnColor( int column, float colors[][3] );
    void setColumnAlpha( int column, std::vector<float> alpha );
    void setRowColor( int row, float colors[][3] );
    void setRowAlpha( int row, std::vector<float> alpha );
    void setPosition( Vector3d position );
    void setWireframe( bool wireframe );
    void resizeGrid( int width, int height );
    void drawMe( );
protected:
    void updatePosition( );
    std::vector< std::vector<Block *> > m_blocks;
    int m_width;
    int m_height;
    Vector3d * m_position;
};

class VuGrid
{
public:
    VuGrid( );
    ~VuGrid( );
public:
    void init( int width, int height, Vector3d position, int bufferSize, float colorOffset );
    void setPosition( Vector3d position );
    void setWireframe( bool wireframe );
    void updateBuffer ( float * newBuffer );
    void resizeGrid( int width, int height );
    float * getPrevBuffer( );
    void drawMe ( );
protected:
    Grid * m_grid;
    void setColors ( );
    void setAlphas ( );
    void setColumnAlpha ( float fftVal, float ceiling, int bin, float decay );
    void datFFT( );
    float * m_buffer;
    float * m_prevBuffer;
    std::vector< std::vector<float> > m_alphas;
    std::vector< std::vector<float> > m_prevAlphas;
    // float ** m_alphas;
    // float ** m_prevAlphas;
    int m_width;
    int m_height;
    int m_bufferSize;
    float * m_window;
    Vector3d * m_position;
    bool isWireframe;
    float m_colorOffset;
};

#endif
