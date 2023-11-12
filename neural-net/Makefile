CC = g++
CFLAGS = -std=c++17 -Wall
INCLUDE_PATH = -IC:/msys64/mingw64/include
LIBRARY_PATH = -LC:/msys64/mingw64/lib
LIBRARIES = -lfftw3 -lstdc++fs

all: your_executable

ffnn: main.cpp audio.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDE_PATH) $(LIBRARY_PATH) $(LIBRARIES)

clean:
	rm -f ffnn
