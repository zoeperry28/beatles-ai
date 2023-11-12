#include <iostream>
#include "neural_net.hpp"
#include "audio.hpp"

int main() 
{
    Audio AudioReader;
    AudioReader.Load("D:\\zoepe\\projects\\beatles-ai\\Data\\john\\1_imagine-vocal.wav");

    return 0;
}
