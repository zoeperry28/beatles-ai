rm CMakeCache.txt cmake_install.cmake
rm -rf CMakeFiles Debug simple_example.dir x64 x32
rm *.vcxproj *.vcxproj.filters *.sln

cmake -G "MinGW Makefiles" neural-net
cmake --build .

pause