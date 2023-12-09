#include "helper.hpp"
#include <iostream>
#include "neural_net.hpp"
#include "audio.hpp"
#include <boost/cstdfloat.hpp>
#include <vector>
#include <iostream>
#include <cctype>
#include<windows.h> 

#include <boost/algorithm/string/case_conv.hpp>

#include "helper.hpp"
int main(int argc, char* argv[])
{
	AudioSuite AudioReader;
	Prime_Data PD(AudioReader);
	std::vector<std::string> file_names; 
	std::string MODE = "";
	std::string FILE = "";
	
	if (argc > 3)
	{
		MODE = std::string(argv[1]);
		std::transform(MODE.begin(), MODE.end(), MODE.begin(), ::tolower);
		FILE = std::string(argv[2]);
		std::transform(FILE.begin(), FILE.end(), FILE.begin(), ::tolower);
	
		for (int i = 3; i < argc; i++)
		{
			file_names.push_back(argv[i]);
		}
	}
	else
	{
		std::cerr << "Insufficient Arguments!\n";
	}
	
	switch (Neural_Net_Modes::ParseArgs(MODE))
	{
		case E_DATA_GATHERING:
			Neural_Net_Modes::Data_Gathering(file_names);
			break;
		case E_TRAINING:
			Neural_Net_Modes::Training(file_names);
			break;
		case E_EXECUTION:
			Neural_Net_Modes::Execution(file_names);
			break;
		default:
			std::cerr << "Expected Usage: ./main.exe [-d/-D|-t/-T|-e/-E] -F <file 1>...<file n> \n";
			break;
	}
}

