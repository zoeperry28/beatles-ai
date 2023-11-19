#include <cstdint>
#include <iostream>
#include "neural_net.hpp"
#include "audio.hpp"

#include <boost/cstdfloat.hpp>

int main(int argc, char * argv[]) 
{
    Audio AudioReader("C:\\projects\\beatles-ai\\Data\\john\\1_imagine-vocal.wav");
    Prime_Data P;
    P.PrepareAudioData(&AudioReader);
    std::cout << "DONE\n";
    return 0;
}

