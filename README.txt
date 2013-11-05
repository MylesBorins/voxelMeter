==============================================================================

                                  voxelMeter
                                  
                       thealphanerd        fall 2013
                                      
==============================================================================

                            _..--+~/@-@--.
                       _-=~      (  .    )
                    _-~     _.--=.\ \''''  - About tree fiddy
                  _~      _-       \ \_\
                 =      _=          '--'
                '      =                             .
               :      :                              '=_. ___
               |      ;                                  '~--.~.
               ;      ;                                       } |
               =       \             __..-...__           ___/__/__
               :        =_     _.-~~          ~~--.__
            __  \         ~-+-~                   ___~=_______
                 ~@#~~ == ...______ __ ___ _--~~--_

==============================================================================

                              How to make it work

==============================================================================

start by running the command "make"

----------

then run the binary "./voxelMeter"

----------

FUN!

==============================================================================

                                    controls

==============================================================================

                            w   ~ toggle wireframe
                            f   ~ toggle fullscreen
                            ▶  ~ add column
                            ◀  ~ remove column
                            ▲  ~ add row
                            ▼  ~ remove row


==============================================================================

                                    How it works

==============================================================================

inspired by https://sites.google.com/site/letsmakeavoxelengine/home/

The mic input is put through an fft which is equally distributed into
an array the same width as the cube.  This array is used to calculate
a column of alphas, each representing a specific frequency bin.

Each layer of the cube has a simple feedback and attenuation that is used
to give the decay rate of the alpha.

A VuGrid class is used to handle all fft calculations and subsequent
determining of alphas.

Glut cube are stored in 2D vectors to renders a 32 x 32 x 32 chunk
I was able to make a simple optimization by only drawing cubes
that are above a certain alpha threshold.

This is fairly inefficient and does not use Display Lists or Vertex Buffers
which I think would be necessary to use many more voxels. 

==============================================================================
