#include "helper.hpp"
#include <iostream>
#include "neural_net.hpp"
#include "audio.hpp"
#include <boost/cstdfloat.hpp>
#include <vector>

int main(int argc, char * argv[]) 
{	
	// DATA GATHERING
	if (argc > 2 && argv[1] == "-d" || argv[1] == "-D")
	{
		/*
		 * 1. check if the file given by the user is a folder or a file. 
		 *    - If it is a file only, reccomend that a larger data set is used
		 *    - If the user gives multiple folders as arguemnts, these should all be 
		 * 		checked AND put into seperate files, with their corresponding folder
		 *		as the name of the file, with a timestamp.
		 */

		std::vector<std::string> paths;  
		if (argc >= 3)
		{
			for (int i = 2; i < argc; i++)
			{
				std::string temp = std::string(argv[i]);
				if (IsFolder(temp) == false)
				{
					std::cout << "[WARN ] You have only given files, not directories.\nFor better results, we would reccomend a larger dataset.\n";
				}
				if (IsWavFile(temp) == true)
				{
					paths.push_back(temp);
				}
			}
		}
		else
		{
			std::cerr << "Expected Usage: ./main.exe [-d/-D|-t/-T|-e/-E] -F <file 1>...<file n> \n";
		}

		/*
		 * 2. Go through all of the files recursively. Perform the defined data checks 
		 *	  on each of the files. 
		 */
		Bulk_Audio * AudioReader;

		WAV * wavs = (WAV *)malloc(paths.size()); 
		for (int i = 0 ; i < paths.size(); i++)
		{
			wavs[i] = *AudioReader->LoadFiles(paths[i], true);
			// perform the data checks - the wav data will be stored along side the actual data that has beeen produced. 
		}

		/*
		 * 3. Once all of the files have been checked, take the values and put them
		 *    into a CSV file.
		 */

		/* 4. If specified by the user, the data can be saved with a specific name */
	}
	// TRAINING MODE
	else if (argc > 2 && argv[1] == "-t" || argv[1] == "-T")
	{
		// 1. The user gives the model the .csv data from the data gathering stage. 
		//	  - The format should be checked. 
		// 2. The data is parsed and is put into a struct
		// 3. The scruct can then be passed into the model, with each of it's parameters
		//    going to the appropriate node
		// 4. Information about the run, such as weights and biases, should be stored
		//    in a seperate file, where it can be used for: 
		//    1. Analysis
		//	  2. Execution later on
	}
	// EXECUTION MODE
	else if (argc > 2 && argv[1] == "-e" || argv[1] == "-E")
	{
		// 1. First, the software should check if a xxxxxxx.dat exists anywhere on the path
		//    - If the data does not exist, the execution mode cannot proceed, prompt the 
		//      user to switch modes. 
		// 2. If the .dat exists, the user can proceed. Check the file that has been passed
		//    and see if it is a suitable candidate for the model
		//    -> Suitable candidate is defined as
		// 		 - PCM-16 Mono .wav file 
		// 3. Parse the file for the appropriate data points, then from that, put it through
		//    the model
		// 4. A log of the execution should also be stored off, detailing parameters used.
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
		// Expecting arguments from the user to denote the mode being used. 
		std::cerr << "Expected Usage: ./main.exe [-d/-D|-t/-T|-e/-E] -F <file 1>...<file n> \n";
	}
}

