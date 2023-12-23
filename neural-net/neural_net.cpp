#include "neural_net.hpp"
#include "audio.hpp"
#include "helper.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "neuron.hpp"
#include <algorithm>
#include <cctype>
#include <boost/algorithm/string.hpp>
#include <string>

std::vector<NN_Audio_Parameters> Prime_Data::Read_Data(std::string filename)
{
    std::vector<NN_Audio_Parameters> NNAP;
    std::string line;
    std::ifstream input(filename);
    int inc = 0;
    for (std::string line; getline(input, line);)
    {
        if (inc == 0)
        {
            inc++;
            continue;
        }
        else
        {
            std::vector<std::string> found = SplitByDelimiter(line, ",");
            WAV wav;
            wav.filename = found[0];
            NN_Audio_Parameters cur = {
               .Label = found[1],
               .wav = wav,
               .ZeroCrossingCount = std::stoi(found[2]),
               .Pitch = std::stof(found[3]),
               .Magnitude = std::stof(found[4]),
               .Phase = std::stof(found[5]),
            };
            NNAP.push_back(cur);
            
        }
        inc++;
    }
    return NNAP;
}

void Prime_Data::Write_Data(std::string filename, std::vector<NN_Audio_Parameters>& AP, int NoOfEntries)
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

    for (int entry = 0; entry < AP.size(); entry++)
    {
        to_return = to_return + AP[entry].wav.filename + "," +
                                AP[entry].Label + "," +
                                std::to_string(AP[entry].ZeroCrossingCount) + "," +
                                std::to_string(AP[entry].Pitch) + "," +
                                std::to_string(AP[entry].Magnitude) + "," +
                                std::to_string(AP[entry].Phase) + "\n";
    }

    std::ofstream myfile;
    myfile.open(filename);
    myfile << to_return;
    myfile.close();
}

std::string Prime_Data::Get_Data_Label(std::string filename)
{
    int n = filename.rfind("/")+1;

    std::string e(&filename[n], &filename[filename.size()]);

    std::string f(&e[0], &e[
        std::min(e.find("\\"), e.find("/"))
    ]);

    std::string newstr = boost::to_upper_copy<std::string>(f);
    return newstr;
}

std::vector<NN_Audio_Parameters> Prime_Data::PrepareAudioData(std::vector<WAV> wav, int NoOfFiles)
{
    std::vector<NN_Audio_Parameters> A_Audio_Parameters(NoOfFiles);
    for (int i = 0; i < wav.size(); i++)
    {

        AS->MFCC(wav[i]);
        AS->FourierTransform(wav[i]);

        NN_Audio_Parameters AP;
        AP.Label             = Get_Data_Label(wav[i].filename);
        AP.wav = wav[i],
            AP.ZeroCrossingCount = AS->CountZeroCrossings(wav[i]),
            AP.Pitch = AS->CalculatePitch(wav[i]),
            AP.Magnitude = StdDev(AS->FFT_GetMagnitude(wav[i])),
            AP.Phase = StdDev(AS->FFT_GetPhase(wav[i])),


        A_Audio_Parameters[i] = AP;
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
    std::vector<WAV> wavs;

    AudioSuite AS;

    for (const auto& folder : files)
    {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(folder))
        {
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

    std::vector<WAV>wavs = Get_Wavs(files);
    
    std::vector<NN_Audio_Parameters> Data = PD.PrepareAudioData(wavs, wavs.size());

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
    Neural_Net NN;
    Prime_Data PD;

    std::vector<std::vector<NN_Audio_Parameters>> AP;
    for (int i = 0; i < files.size(); i++)
    {
        if (IsCSVFile(files[i]))
        {
            AP.push_back(PD.Read_Data(files[i]));
        }
    }
    // flatten the data
    std::vector<NN_Audio_Parameters> final_data; 
    if (AP.size() > 1)
    {
        for (int i = 0; i < AP.size(); i++)
        {
            for (int j = 0; j < AP[i].size(); j++)
            {
                final_data.push_back(AP[i][j]);
            }
        }
    }
    else
    {
        final_data = AP[0];
    }
    LoadingBar LB("Input Prepearation", final_data.size());
    for (int i = 0; i < final_data.size(); i++)
    {
        Input::PrepareInputs(final_data[i]);
        LB.LogProgress1();
    }
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
    Prime_Data PD;

    std::vector<WAV>wavs = Get_Wavs(files);

    std::vector<NN_Audio_Parameters> Data = PD.PrepareAudioData(wavs, wavs.size());

    for (int i = 0; i < Data.size(); i++)
    {
        Input::PrepareInputs(Data[i]);
    }

    return false;
}
