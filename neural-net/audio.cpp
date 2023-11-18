#include "audio.hpp"
#include <math.h>
#include <string>
#include <fftw3.h>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <vector>
#include <charconv>
#include <sstream>
#include <iostream>
#include <boost/dynamic_bitset.hpp>
#include <boost/cstdfloat.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <typeinfo>

// Info on .wav file structure is taken from http://soundfile.sapp.org/doc/WaveFormat/

/*
*  @brief  This function takes the path of an audio file and loads it. Once the file is loaded, the data section of the 
*          file is then converted to float. 
*  @param  Path the path of the audio file to be loaded.
*  @return whether the file was read or not. 
*/
int Audio::Load(std::string Path = C_EMPTY_STRING) 
{
    bool IsValidFile = true;
    int status = 0;
    std::vector<char>result;

    if (Path == C_EMPTY_STRING && path == "")
    {
        IsValidFile = false;
    }

    if (std::filesystem::path(Path).extension() == ".wav" && IsValidFile != false)
    {  
        std::ifstream ifs(Path, std::ios::binary|std::ios::ate);
        std::ifstream::pos_type pos = ifs.tellg();

        std::vector<char> result(pos);

        ifs.seekg(0, std::ios::beg);
        ifs.read((char * ) &result[0], pos);

        std::vector<uint8_t> file_vec(result.begin(), result.end());
        uint8_t* conv_file_vec = file_vec.data();

        audio_wav = GetHeaderFromBytes(conv_file_vec);
        data_size = GetDataSize(Path);
        
        // Data should be converted to mono.
        StereoToMono();
        
        // Now, the obtained data will be converted to float. This is done to enable the use of various audio analysis functions
        file = ByteToFloat(audio_wav.Data, data_size);

        GetAsFrames();

        status = 1;
    }
    else if (IsValidFile == false)
    {
        throw std::invalid_argument("Invalid file!");
    }
    else
    {
        throw std::invalid_argument(
            "As this model expects data to be generated from the externally included script, the program will only accept .wav files. Sorry!");
    }
    return status;
}

/*
* @brief Takes a byte array and converts it to boost::float.
* @param bytes The bytes to be converted to boost::float
* @param size The size of the audio data
* @return A pointer to the boost::float pointer.
*/
boost::float32_t* Audio::ByteToFloat(uint8_t* bytes, int size)
{
    if (size == -1)
    {
        size = data_size;
    }
    if (bytes == nullptr)
    {
        // Handle nullptr case appropriately
        return nullptr;
    }

    // Allocate memory for the float array
    boost::float32_t* f = new boost::float32_t[size];

    for (int i = 0; i < size; i++)
    {
        // Convert each byte to boost::float32_t and store in the array
        f[i] = static_cast<boost::float32_t>(bytes[i]);
    }

    return f;
}

/*
* @brief Takes a byte array and extracts the .wav header from it. It is assumed that the data given is a wav.
* @param bytes The bytes to be used
* @return A data structure representing the layout of a .wav file, including the header.
*/
WAV Audio::GetHeaderFromBytes(uint8_t * bytes)
{
    WAV wav;
    memcpy(&wav.header, bytes, sizeof(WAV_Header));
    wav.Data = bytes + sizeof(WAV_Header);
    return wav;
}

/*
* @brief Gets the size of the audio data from the file path.
* @param Path The location of the audio file.
* @return The data size minus the size of the header. 
*/
int Audio::GetDataSize(std::string Path)
{
    return std::filesystem::file_size(std::filesystem::path(Path)) - C_WAV_HEADER_SIZE;
}

/*
* @brief Takes the audio data contained within the class and converts it to frames. 
* @return The data as float, divided up into frames. 
*/
std::vector<boost::float32_t *> Audio::GetAsFrames()
{
    int int_sample_rate = GetAsInt(audio_wav.header.SampleRate, 4);
    std::vector<boost::float32_t *> to_return;

    int lo = 0;
    int hi = int_sample_rate;

    while (hi <= data_size)
    {
        // Create a new frame buffer
        boost::float32_t *temp = new boost::float32_t[int_sample_rate];

        // Copy data to the frame buffer
        for (int i = 0; i < int_sample_rate; i++)
        {
            temp[i] = audio_wav.Data[lo + i];
        }

        // Update pointers and push back the frame
        lo = hi;
        hi += int_sample_rate;
        to_return.push_back(temp);
    }

    return to_return;
}

/*********************************************************************************************************************/

int Audio::GetAsInt(char *c, int sz)
{
    char buf[5];
    std::string ne = "";
    for (int i = 0 ; i < sz; i ++)
    {
        sprintf(buf, "%x", c[i]);
        std::string v = std::string(buf);

        if (v.size() > 2)
        {
            v = v.substr(v.size()-2, v.size());
        }
        else if (v.size() == 1 && v == "0")
        {
            v = "00";
        }
        ne = ne + v;
    }
    return std::stoul(ne, nullptr, 16);
}

bool Audio::StereoToMono()
{
    bool res = true;

    if (NumOfChannels() == 2)
    {
        uint8_t* monoData = new uint8_t[data_size / 2];

        for (int i = 0; i < data_size; i += 2)
        {
            uint32_t v = static_cast<uint32_t>(audio_wav.Data[i]) + audio_wav.Data[i + 1];

            monoData[i / 2] = static_cast<uint8_t>(v / 2);
        }

        audio_wav.Data = monoData;
        audio_wav.header.NumChannels[0] = 1;
        data_size = data_size / 2;
    }
    else
    {
        res = false;
    }

    return res;
}


int Audio::NumOfChannels()
{
    return (int)audio_wav.header.NumChannels[0];
}

int Audio::CountZeroCrossings(float * signal, int signal_size)
{
    int crossings = 0;

    for (int i = 1; i < signal_size; ++i) 
    {
        if ((signal[i] >= 0 && signal[i - 1] < 0) || 
            (signal[i] < 0 && signal[i - 1] >= 0)) 
        {
            crossings++;
        }
    }
    return crossings;
}

float Audio::CalculatePitch(float * signal, int signal_size, int sample_rate)
{
    float to_return = 0.0;
    int zero_crossings = CountZeroCrossings(signal, signal_size);

    if (zero_crossings == 0) 
    {
        to_return = 0.0f;
    }
    to_return = sample_rate / (2.0f * zero_crossings);
    return to_return;
}

int Audio::GetMidiNote(float pitch, float reference_pitch)
{
    return (C_REF_PITCH_NOTE_NUM + C_UNIQUE_PITCHES) * log2(pitch/reference_pitch);
} 

std::string Audio::GetActualNote(float pitch, float reference_pitch)
{
    int note_number = GetMidiNote(pitch, reference_pitch);
    note_number -= C_MIDI_INDEX_ADJUST; // see the explanation below.
    return (Notes[note_number  % C_UNIQUE_PITCHES] + std::to_string(note_number / C_UNIQUE_PITCHES + 1));
} 

int Bulk_Audio::LoadFiles(std::string path, bool recursive = false)
{
    
    return 0;
}

int Bulk_Audio::LoadFiles(std::vector<std::string> path, bool recursive = false)
{
    return 0;
}