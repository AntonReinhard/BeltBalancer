#The file to be compiled
OBJS = Belt_Balancer_Creator.cpp

#CC Compiler
CC = g++

#INCLUDE_PATHS
INCLUDE_PATHS = -IC:\mingw_dev_lib\include\SDL2

#LIBRARY_PATHS
LIBRARY_PATHS = -LC:\mingw_dev_lib\lib

#COMPILER_FLAGS
# -w suppressing warnings
# -Wl,-subsystem,windows to get rid of command windows
# -std=c++0x to compile using C++11
COMPILER_FLAGS = -w -Wl,-subsystem,windows -std=c++0x

#LINKER_FLAGS linked libraries
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -static-libgcc -static-libstdc++

#OBJ_NAME output name
OBJ_NAME = Belt_Balancer_Creator

#Compile command
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)