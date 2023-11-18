#include <cstdint>
#include <iostream>
#include "neural_net.hpp"
#include "audio.hpp"

#include <boost/cstdfloat.hpp>

int main(int argc, char * argv[]) 
{
    Audio AudioReader;
    AudioReader.Load("C:\\projects\\beatles-ai\\Data\\john\\1_imagine-vocal.wav");
    std::cout << "hello" << std::endl;
    return 0;
}

