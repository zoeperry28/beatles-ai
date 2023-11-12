CC = g++
CFLAGS = -std=c++11 -Wall
INCLUDE_PATH = -IC:\vcpkg\installed\x64-mingw-dynamic\include
LIBRARY_PATH = -LC:\vcpkg\installed\x64-mingw-dynamic\lib
LIBRARIES = -lfftw3

all: your_executable

your_executable: main.cpp audio.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDE_PATH) $(LIBRARY_PATH) $(LIBRARIES)

clean:
	del your_executable.exe
