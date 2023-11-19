//#include <cstdint>
#include <iostream>
#include "neural_net.hpp"
#include "audio.hpp"
#include <boost/cstdfloat.hpp>
int main(int argc, char * argv[]) 
{	
	// DATA GATHERING
	if (argc == 2 && argv[1] == "-d" || argv[1] == "-D")
	{
		
	}
	// TRAINING MODE
	if (argc == 2 && argv[1] == "-t" || argv[1] == "-T")
	{

	}
	// EXECUTION MODE
	else if (argc == 2 && argv[1] == "-e" || argv[1] == "-E")
	{
		Audio AudioReader("C:\\projects\\beatles-ai\\Data\\john\\1_imagine-vocal.wav");
		Prime_Data P;
		Neural_Net NN;
		NN_Audio_Parameters * param = P.PrepareAudioData(&AudioReader);
		NN.Feed_Forward(param);
		std::cout << "DONE\n";
		return 0;
	}
	// NOT ENOUGH ARGUEMNTS
	else
	{
		std::cerr << "Expected Usage: ./main.exe [-d/-D|-t/-T|-e/-E]\n";
	}
}

