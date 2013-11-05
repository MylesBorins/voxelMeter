==============================================================================

                                Myles Borins HW3
                                     OMG!!!!
                                     vMeter
                                     
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

then run the binary "./vMeter"

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

I spent a bunch of time researching voxels as a primary means of rendering.
I created a box class using the GLUT cube as a primary resource.  This class
wraps the object in its own frame and can have position / color / alpha updated
via a funtion.  It also has a single function that can be called to render.

This class was used to build a meta class column which will render a column of blocks
which can have position, color, alpha updating on a per block basic via an array of values.

This current iteration then uses the column class to render a grid of voxels.  A color
is initialized for all cubes at init.  Alpha is updated on a per frame basis based on an fft.

==============================================================================
                    
                                        Next?

==============================================================================

First I would like to abstract grid and use that instead of column.  
I would like to make this into a cube that presents data similar to a waterfall.
I would also like make it explode on large transients.