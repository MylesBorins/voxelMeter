//-------------------------------------------------------------------------
// name: VoxeLib.cpp
// desc: VoxeLib class implementation
//
// author: Myles Borins (mborins@ccrma.stanford.edu)
//   date: fall 2013
//
//-------------------------------------------------------------------------
#include "VoxeLib.h"
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include "chuck_fft.h"

using namespace std;

Vector3d::Vector3d( float x, float y, float z)
{
    m_x = x;
    m_y = y;
    m_z = z;
};

Block::Block(  )
{
    // ima use an object for the vector
    this->m_position = new Vector3d(0, 0, 0);
    // but an array for the color... kinda silly
    this->m_color = new float[3];
};

Block::~Block( )
{
    delete this->m_position;
    delete m_color;
};

void Block::init() {
    this->setColor(1, 1, 1);
    this->setAlpha(1);
    this->m_wireframe = false;
    this->m_active = false;
};

void Block::setActive ( bool active )
{
    this->m_active = active;
};

void Block::setWireframe ( bool wireframe )
{
    this->m_wireframe = wireframe;
};

void Block::setColor( float r, float g, float b )
{
    this->m_color[0] = r;
    this->m_color[1] = g;
    this->m_color[2] = b;
}

void Block::setAlpha( float a )
{
    this->m_alpha = a;
}

void Block::setPosition( float x, float y, float z )
{
    this->m_position->m_x = x;
    this->m_position->m_y = y;
    this->m_position->m_z = z;
}

void Block::drawMe( )
{
    if(this->m_active)
    {
        glPushMatrix();
        {
            glTranslatef( m_position->m_x, m_position->m_y , m_position->m_z );
            
            if (this->m_wireframe)
            {
                glColor4f(1.0, 1.0, 1.0, 0.5);
   
                glutWireCube (0.25);
            }
            glColor4f (m_color[0], m_color[1], m_color[2], m_alpha);

            glutSolidCube (0.24);
        }
        glPopMatrix();
    }
};

Column::Column( )
{
    m_position = new Vector3d(0,0,0);
};

Column::~Column( )
{
    delete m_position;
    for(int i = 0; i < m_length; i++)
    {
        delete m_blocks[i];
    }
};

void Column::setPosition( float x, float y, float z)
{
    this->m_position->m_x = x;
    this->m_position->m_y = y;
    this->m_position->m_z = z;
};

void Column::updatePosition( )
{
    for (int i = 0; i < this->m_length; i++)
    {
        m_blocks[i]->setPosition(m_position->m_x, m_position->m_y + 0.5 * i, m_position->m_z);
    }
};

void Column::setColors( float colors[][3] )
{
    for(int i = 0; i < this->m_length; i++)
    {
        m_blocks[i]->setColor(colors[i][0], colors[i][1], colors[i][2]);
    }
};

void Column::setAlphas( float alphas[] )
{
    for(int i = 0; i < this->m_length; i++)
    {
        m_blocks[i]->setAlpha(alphas[i]);
    }
};

void Column::setWireframe ( bool wireframe )
{
    for (int i = 0; i < this->m_length; i++)
    {
        m_blocks[i]->setWireframe(wireframe);
    }
};

void Column::init( int length, float x, float y, float z )
{
  this->m_length = length;
  this->setPosition(x, y, z);
  m_blocks = new Block*[length];
  for(int i = 0; i < length; i++)
  {
      m_blocks[i] = new Block();
  }
  this->updatePosition();
};

void Column::drawMe( )
{
    for(int i = 0; i < this->m_length; i++)
    {
        m_blocks[i]->drawMe();
    }
};

Grid::Grid()
{
    this->m_position = new Vector3d(0,0,0);
};

Grid::~Grid()
{
    delete this->m_position;
    this->m_blocks.erase(this->m_blocks.begin(), this->m_blocks.end());
};

void Grid::init( int width, int height, Vector3d position )
{
    this->m_width = width;
    this->m_height = height;
    this->setPosition(position);
    
    this->m_blocks.resize(width);
    for (vector<vector <Block *> >::iterator it = this->m_blocks.begin(); it != this->m_blocks.end(); ++it)
    {
        for ( int i = 0; i < this->m_height; i++)
        {
            it->push_back(new Block);
            it->at(i)->init();
        }
    }
    this->updatePosition();
};

void Grid::resizeGrid( int width, int height )
{
    this->m_blocks.erase(this->m_blocks.begin(), this->m_blocks.end());
    this->init(width, height, *this->m_position);
};

void Grid::setPosition( Vector3d position )
{
    this->m_position->m_x = position.m_x;
    this->m_position->m_y = position.m_y;
    this->m_position->m_z = position.m_z;
};

void Grid::updatePosition( )
{
    
    for (int i = 0; i < this->m_width; i++)
    {
        for(int j = 0; j < this->m_height; j++)
        {
            this->m_blocks[i][j]->setPosition(this->m_position->m_x + 0.25 * i - 1.625, this->m_position->m_y + 0.25 * j - 1.75, this->m_position->m_z);
        }
    }
};

void Grid::setBlockColor( int x, int y, float colors[3] )
{
    this->m_blocks.at(x).at(y)->setColor(colors[0], colors[1], colors[2]);
};

void Grid::setBlockAlpha( int x, int y, float alpha )
{
    this->m_blocks.at(x).at(y)->setAlpha( alpha );
    if(alpha <= 0.0375)
    {
        this->m_blocks.at(x).at(y)->m_active = false;
    }
    else {
        this->m_blocks.at(x).at(y)->m_active = true;
    }
    

};

void Grid::setColumnColor( int column, float colors[][3] )
{
    for( int i = 0; i < this->m_height; i++)
    {
        this->m_blocks.at(column)[i]->setColor(colors[i][0], colors[i][1], colors[i][2]);
    }
};

void Grid::setColumnAlpha( int column, vector<float> alphas )
{
    for( int i = 0; i < this->m_height; i++)
    {
        this->setBlockAlpha(column, i, alphas[i]);
    }
};

void Grid::setRowColor( int row, float colors[][3] )
{
    for( int i = 0; i < this->m_width; i++)
    {
        this->m_blocks[i].at(row)->setColor(colors[i][0], colors[i][1], colors[i][2]);
    }
};

void Grid::setRowAlpha( int row, vector<float> alphas )
{
    for( int i = 0; i < this->m_width; i++)
    {
        this->m_blocks[i].at(row)->setAlpha( alphas[i] );
    }
};

void Grid::setWireframe( bool wireframe )
{
    for (int i = 0; i < this->m_width; i++)
    {
        for(int j = 0; j < this->m_height; j++)
        {
            this->m_blocks[i][j]->setWireframe(wireframe);
        }
    }
};
    

void Grid::drawMe( )
{
    for (vector<vector <Block *> >::iterator it = this->m_blocks.begin(); it != this->m_blocks.end(); ++it)
    {
        for (vector<Block *>::iterator jit = it->begin(); jit != it->end(); ++jit)
        {
            Block* block = (*jit);
            block->drawMe();
        }
    }
};

VuGrid::VuGrid( )
{
    this->m_grid = new Grid();
    this->m_position = new Vector3d(0,0,0);
    this->isWireframe = false;
};

VuGrid::~VuGrid( )
{
    delete this->m_grid;
    delete this->m_buffer;
    delete this->m_prevBuffer;
    delete this->m_window;
    this->m_alphas.erase(this->m_alphas.begin(), this->m_alphas.end());
    this->m_prevAlphas.erase(this->m_prevAlphas.begin(), this->m_prevAlphas.end());
};

void VuGrid::init( int width, int height, Vector3d position, int bufferSize, float colorOffset )
{
    // Setup the grid
    this->m_width = width;
    this->m_height = height;
    
    this->m_position->m_x = position.m_x;
    this->m_position->m_y = position.m_y;
    this->m_position->m_z = position.m_z;
    this->m_grid->init(width, height, position);
    
    this->m_colorOffset = colorOffset;
    
    // Setup the buffer
    this->m_bufferSize = bufferSize;
    this->m_buffer = new float[this->m_bufferSize];
    this->m_prevBuffer = new float[this->m_bufferSize];
    
    // Zero that buffer
    memset( this->m_buffer, 0, bufferSize * sizeof(float) );
    memset( this->m_prevBuffer, 0, bufferSize * sizeof(float) );
    
    // Setup alphas
    this->m_alphas.resize(width);
    this->m_prevAlphas.resize(width);
    for ( int i = 0; i < width; i++)
    {
        this->m_alphas[i].resize(height);
        this->m_prevAlphas[i].resize(height);
    }
    
    // Setup window for fft
    this->m_window = new float[bufferSize];
    // make the transform window
    hanning( this->m_window, this->m_bufferSize );
    
    // Set colors
    this->setColors( );
    // Set Alphas
    this->setAlphas( );
    
    // Turn on the wireframe to start
    this->m_grid->setWireframe(this->isWireframe);
};

void VuGrid::setColors( )
{
    float colors[m_height][3];
    
    for (int i = 0; i < m_height; i++)
    {
        colors[i][0] = 0 + (1 / (float)m_height) * i;
        colors[i][1] = 1 - (1 / (float)m_height) * i;
        colors[i][2] = 0 + this->m_colorOffset;
    }
    for (int i = 0; i < m_width; i++)
    {
        m_grid->setColumnColor(i, colors);
    }
};

void VuGrid::setColumnAlpha ( float fftVal, float ceiling, int bin, float decay )
{   
    float segment = ceiling / this->m_height;
    
    for(int i = 0; i < this->m_height; i++)
    {
        fftVal -= segment;
        if( fftVal >= segment)
        {
            this->m_alphas[bin][i] = 1;
        }
        else if (fftVal >= 0)
        {
            this->m_alphas[bin][i] = fftVal / segment;
        }
        else
        {
            this->m_alphas[bin][i] = 0;
        }
        this->m_alphas[bin][i] = (this->m_alphas[bin][i] + this->m_prevAlphas[bin][i]);
        this->m_prevAlphas[bin][i] = this->m_alphas[bin][i] * decay;
    }
}

void VuGrid::setAlphas( )
{
    apply_window( (float*)this->m_buffer, this->m_window, this->m_bufferSize );
    
    // take forward FFT; result in buffer as FFT_SIZE/2 complex values
    rfft( (float *)this->m_buffer, m_bufferSize, true );
    
    // cast to complex
    complex * cbuf = (complex *)this->m_buffer;
    
    for( int i = 0; i < this->m_bufferSize; i++)
    {
        this->m_buffer[i] = ::pow( 25 * cmp_abs( cbuf[i] ), .5 );
    }
        
    float blocks[this->m_width];
    
    memset( blocks, 0, this->m_width * sizeof(float) );
    
    // TODO linear spacing -> log spacing
    
    int blockSize = this->m_bufferSize / this->m_width;
    
    for( int i = 0; i < this->m_width; i++)
    {
        for( int j = 0; j < blockSize; j++)
        {
            blocks[i] += this->m_buffer[j + i * blockSize];
        }
        blocks[i] /= blockSize;
    }
    
    for (int i = 0; i < this->m_width; i++)
    {
        this->setColumnAlpha(blocks[i], 0.15, i, .5);
        this->m_grid->setColumnAlpha(i, this->m_alphas[i]);
    }
};

void VuGrid::setPosition( Vector3d position )
{
    this->m_grid->setPosition(position);
};

void VuGrid::setWireframe( bool wireframe )
{
    this->isWireframe = wireframe;
    this->m_grid->setWireframe(wireframe);
};

void VuGrid::updateBuffer ( float * newBuffer )
{
    for ( int i = 0; i < this->m_bufferSize; i++)
    {
        // this->m_prevBuffer[i] = this->m_buffer[i];
        this->m_buffer[i] = newBuffer[i];
    }
    // memcpy( this->m_prevBuffer, this->m_buffer, this->m_bufferSize * sizeof(float) );
    // memcpy( this->m_buffer, newBuffer, this->m_bufferSize * sizeof(float) );
    this->setAlphas( );
};

void VuGrid::resizeGrid( int width, int height)
{
    this->m_width = width;
    this->m_height = height;
    this->m_grid->resizeGrid(width, height);
    // reset colors
    this->setColors();
    // reset wireframes
    this->setWireframe(this->isWireframe);
    // reset alphas
    this->m_alphas.resize(width);
    this->m_prevAlphas.resize(width);
    for ( int i = 0; i < width; i++)
    {
        this->m_alphas[i].resize(height);
        this->m_prevAlphas[i].resize(height);
    }
};

float * VuGrid::getPrevBuffer ()
{
    return this->m_prevBuffer;
};


void VuGrid::drawMe ( )
{
    glPushMatrix();
    {
        // glTranslatef( this->m_position->m_x, this->m_position->m_y, this->m_position->m_z );
        // glScalef(8 / (float)this->m_width, 8 / (float)this->m_height, 1);
        // glRotatef(90, -0.2, -0.2, 0);
        this->m_grid->drawMe();
    }
    glPopMatrix();
};