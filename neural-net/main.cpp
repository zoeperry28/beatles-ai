#include <cstdint>
#include <iostream>
#include "neural_net.hpp"
#include "audio.hpp"

#include <boost/cstdfloat.hpp>

int main(int argc, char * argv[]) 
{
    Bulk_Audio AudioReader("C:\\projects\\beatles-ai\\Data\\john");
    std::cout << "hello" << std::endl;
    return 0;
}

