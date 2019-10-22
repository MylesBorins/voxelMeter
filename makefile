
CXX=g++
INCLUDES=

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
FLAGS=-D__UNIX_JACK__ -c -g
LIBS=-lasound -lpthread -ljack -lstdc++ -lm \
	-lGL -lGLU -lglut
endif
ifeq ($(UNAME), Darwin)
FLAGS=-D__MACOSX_CORE__ -c -g -Wno-deprecated-declarations
LIBS=-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
	-framework IOKit -framework Carbon  -framework OpenGL \
	-framework GLUT -framework Foundation \
	-framework AppKit -lstdc++ -lm
endif

OBJS=  RtAudio.o chuck_fft.o Thread.o Stk.o VoxeLib.o voxelMeter.o

voxelMeter: $(OBJS)
	$(CXX) -o voxelMeter $(OBJS) $(LIBS)
	
voxelMeter.o: voxelMeter.cpp RtAudio.h
	$(CXX) $(FLAGS) voxelMeter.cpp
	
VoxeLib.o: VoxeLib.h VoxeLib.cpp
	$(CXX) $(FLAGS) VoxeLib.cpp

chuck_fft.o: chuck_fft.h chuck_fft.c
	$(CXX) $(FLAGS) chuck_fft.c
	
Thread.o: Thread.h Thread.cpp
	$(CXX) $(FLAGS) Thread.cpp
	
Stk.o: Stk.h Stk.cpp
	$(CXX) $(FLAGS) Stk.cpp
	
RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
	$(CXX) $(FLAGS) RtAudio.cpp

clean:
	rm -f *~ *# *.o voxelMeter
