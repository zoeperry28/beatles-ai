#include "neural_net.hpp"
#include "audio.hpp"
#include "helper.hpp"
#include <filesystem>

//NN_Audio_Parameters* Prime_Data::PrepareAudioData(std::vector<WAV*>& wav, int file_count)
//{
//    NN_Audio_Parameters* A_Audio_Parameters = nullptr;
//    for (int i = 0; i < file_count; i++)
//    {
//        //This gives us an array of arrays which represent the floating point data from
//        //the audio file
//        boost::float32_t* Current = wav[i]->frames[0];
//
//        while (Current != nullptr)
//        {
//            NN_Audio_Parameters Audio_Parameters = {
//                Current,
//                AS->CountZeroCrossings(Current, wav[i]->size),
//                AS->CalculatePitch(Current, wav[i]->size)
//            };
//            A_Audio_Parameters = &Audio_Parameters;
//            A_Audio_Parameters++;
//            Current++;
//        }
//    }
//    return A_Audio_Parameters;
//
//}

void Prime_Data::Write_Data(std::string filename, NN_Audio_Parameters ** AP, int NoOfEntries)
{
    std::string to_return = "";
    for (int heading = 0; heading < HEADERS.size(); heading++)
    {
        if (heading != HEADERS.size() - 1)
        {
            to_return = to_return + HEADERS[heading] + ",";
        }
        else
        {
            to_return = to_return + HEADERS[heading] + "\n";
        }
    }

    for (int entry = 0; entry < NoOfEntries; entry++)
    {
        to_return = to_return + std::to_string(AP[entry]->ZeroCrossingCount) + "," +
                                std::to_string(AP[entry]->Pitch) + "," +
                                std::to_string(AP[entry]->Magnitude) + "," +
                                std::to_string(AP[entry]->Phase) + "\n";
    }



    //Now, the string is ready!
    std::cout << to_return;
}

NN_Audio_Parameters** Prime_Data::PrepareAudioData(std::vector<WAV>& wav, int NoOfFiles)
{
    NN_Audio_Parameters ** A_Audio_Parameters;
    A_Audio_Parameters = (NN_Audio_Parameters**)malloc(NoOfFiles);
    for (int i = 0; i < wav.size(); i++)
    {
        AS->FourierTransform(wav[i]);

        NN_Audio_Parameters AP = {
            .wav               = wav[i],
            .ZeroCrossingCount = AS->CountZeroCrossings(wav[i]),
            .Pitch             = AS->CalculatePitch(wav[i]),
            .Magnitude         = StdDev(AS->FFT_GetMagnitude(wav[i])),
            .Phase             = StdDev(AS->FFT_GetPhase(wav[i])),
        };
        A_Audio_Parameters[i] = &AP;
    }
    return A_Audio_Parameters;
}

int Neural_Net::Feed_Forward(NN_Audio_Parameters * Prepared_Data)
{
    
    return -1;
}

int Neural_Net::Fetch_Result()
{
    return -1;
}

Neural_Net_Mode Neural_Net_Modes::ParseArgs(const std::string& MODE)
{
    Neural_Net_Mode to_return = E_NO_MODE;
    if (MODE == "-d")
    {
        to_return = E_DATA_GATHERING;
    }
    else if (MODE == "-t")
    {
        to_return = E_TRAINING;
    }
    else if (MODE == "-e")
    {
        to_return = E_EXECUTION;
    }
    else
    {
        std::cerr << "Expected Usage: ./main.exe [-d/-D|-t/-T|-e/-E] -F <file 1>...<file n> \n";
    }
    return to_return;
}
std::vector<WAV> Neural_Net_Modes::Get_Wavs(std::vector<std::string>& files)
{
    std::vector<WAV> wavs;  // Initialize an empty vector

    AudioSuite AS;
    Prime_Data PD;

    for (const auto& folder : files)
    {
        for (const auto& entry : std::filesystem::directory_iterator(folder))
        {
            std::cout << entry << std::endl;
            std::string to_add = entry.path().string();
            if (to_add != "" && IsWavFile(to_add))
            {
                WAV w = AS.Load(to_add, true);
                wavs.push_back(w);
            }
        }
    }

    return wavs;
}

// 1. check if the file given by the user is a folder or a file.
//    - If it is a file only, reccomend that a larger data set is used
//    - If the user gives multiple folders as arguemnts, these should all be
// 		checked AND put into seperate files, with their corresponding folder
// 		as the name of the file, with a timestamp.
// 2. Go through all of the files recursively. Perform the defined data checks
//   on each of the files.
// 3. Once all of the files have been checked, take the values and put them
//    into a CSV file.
// 4. If specified by the user, the data can be saved with a specific name
bool Neural_Net_Modes::Data_Gathering(std::vector<std::string>& files)
{
    AudioSuite AS;
    Prime_Data PD;
    //std::vector<std::vector<HannWindow>> HannWindows;

    std::vector<WAV>wavs = Get_Wavs(files);
    
    NN_Audio_Parameters** Data = PD.PrepareAudioData(wavs, wavs.size());

    PD.Write_Data("test", Data, files.size());

    return false;
}

// 1. The user gives the model the .csv data from the data gathering stage. 
//	  - The format should be checked. 
// 2. The data is parsed and is put into a struct
// 3. The scruct can then be passed into the model, with each of it's parameters
//    going to the appropriate node
// 4. Information about the run, such as weights and biases, should be stored
//    in a seperate file, where it can be used for: 
//    1. Analysis
//	  2. Execution later on
bool Neural_Net_Modes::Training(std::vector<std::string>& files)
{

    return false;
}

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
bool Neural_Net_Modes::Execution(std::vector<std::string>& files)
{
    //Audio AudioReader("C:\\projects\\beatles-ai\\Data\\john\\1_imagine-vocal.wav");
    Neural_Net NN;
    return false;
}
