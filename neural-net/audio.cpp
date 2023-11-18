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
WAV Audio::Load(std::string Path = C_EMPTY_STRING) 
{

    if (Path.empty() && path.empty())
    {
        throw std::invalid_argument("Invalid file path!");
    }
    else if (path.empty() == false)
    {
        Path = path;
    }

    // Check if the file is opened successfully
    std::ifstream ifs(Path, std::ios::binary | std::ios::ate);
    if (!ifs.is_open())
    {
        throw std::runtime_error("Failed to open file: " + Path);
    }

    std::ifstream::pos_type pos = ifs.tellg();
    std::vector<char> result(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(&result[0], pos);

    std::vector<uint8_t> file_vec(result.begin(), result.end());
    uint8_t* conv_file_vec = file_vec.data();

    audio_wav = GetHeaderFromBytes(Path, conv_file_vec);

    // Data should be converted to mono.
    StereoToMono();

    // Now, the obtained data will be converted to float. 
    // This is done to enable the use of various audio analysis functions.
    file = ByteToFloat(audio_wav.Data, audio_wav.size);

    GetAsFrames();

    return audio_wav;
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
        size = audio_wav.size;
    }
    if (bytes == nullptr)
    {
        return nullptr;
    }

    boost::float32_t* f = new boost::float32_t[size];

    for (int i = 0; i < size; i++)
    {
        f[i] = static_cast<boost::float32_t>(bytes[i]);
    }

    return f;
}

/*
* @brief Takes a byte array and extracts the .wav header from it. It is assumed that the data given is a wav.
* @param bytes The bytes to be used
* @return A data structure representing the layout of a .wav file, including the header.
*/
WAV Audio::GetHeaderFromBytes(std::string path, uint8_t * bytes)
{
    WAV wav;
    memcpy(&wav.header, bytes, sizeof(WAV_Header));
    wav.Data = bytes + sizeof(WAV_Header);
    wav.size = GetDataSize(path);
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

    while (hi <= audio_wav.size)
    {
        boost::float32_t *temp = new boost::float32_t[int_sample_rate];

        for (int i = 0; i < int_sample_rate; i++)
        {
            temp[i] = audio_wav.Data[lo + i];
        }

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
        uint8_t* monoData = new uint8_t[audio_wav.size / 2];

        for (int i = 0; i < audio_wav.size; i += 2)
        {
            uint32_t v = static_cast<uint32_t>(audio_wav.Data[i]) + audio_wav.Data[i + 1];

            monoData[i / 2] = static_cast<uint8_t>(v / 2);
        }

        audio_wav.Data = monoData;
        audio_wav.header.NumChannels[0] = 1;
        audio_wav.size = audio_wav.size / 2;
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

int Bulk_Audio::LoadFiles(std::string path, bool recursive)
{
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(path))
    {
        if (dir_entry.is_regular_file())
        {
            try
            {
                Audio::Load(dir_entry.path().string());
                std::cout << "\033[1;32m[INFOS] " << dir_entry.path().string() << std::endl;
            }
            catch(const std::exception& e)
            {
                std::cerr << "\033[1;31m[ERROR] " << dir_entry.path().string() << " could not be loaded!\n";
            }
        }
    }
    return 0;
}

int Bulk_Audio::LoadFiles(std::vector<std::string> path, bool recursive)
{
    for (std::string p : path)
    {
        Audio::Load(p);
    }
    return 0;
}