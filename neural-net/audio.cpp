#include "audio.hpp"
#include <math.h>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <vector>
#include <charconv>
#include <sstream>
#include <iostream>
#include <iostream>
#include <typeinfo>

#include <boost/dynamic_bitset.hpp>
#include <boost/cstdfloat.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <fftw3.h>

// Info on .wav file structure is taken from http://soundfile.sapp.org/doc/WaveFormat/

/*
*  @brief  This function takes the path of an audio file and loads it. Once the file is loaded, the data section of the 
*          file is then converted to float. 
*  @param  Path the path of the audio file to be loaded.
*  @return whether the file was read or not. 
*/
WAV * AudioSuite::Load(std::string Path = "", bool recursive) 
{
    if (Path.empty())
    {
        throw std::invalid_argument("Invalid file path!");
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

    WAV a = audio->GetHeaderFromBytes(Path, conv_file_vec);

    // Data should be converted to mono.
    StereoToMono();

    // Now, the obtained data will be converted to float. 
    // This is done to enable the use of various audio analysis functions.
    a.fl_data = audio->ByteToFloat(a.Data, a.size);

    a.frames = audio->ToFrames(a.fl_data, a.size);

    return &a;
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
 * @brief This function takes an array of bytes from the .wav file and from it creates a set of frames, based on the sample rate
 * @param bytes the bytes from the file. If this is not given, the bytes in the class will be used 
 * @param sz the size of the data
 * @return an array of pointers, pointing to each frame in the audio. 
 */
boost::float32_t ** Audio::ToFrames(boost::float32_t * bytes, int sz)
{ 
    int FrameRate = GetAsInt(audio_wav.header.SampleRate, C_SAMPLERATE_SIZE);

    int FrameCount = sz / FrameRate;
    boost::float32_t ** to_return = (boost::float32_t **)malloc(FrameCount * sizeof(boost::float32_t *)+1);
    int ind = 0;
    for (int i = 0 ; i < FrameCount; i++)
    {
        boost::float32_t * temp = (boost::float32_t *)malloc(FrameRate * sizeof(boost::float32_t *));
        
        for (int j = 0 ; j < FrameRate; j++)
        {
            temp[j] = bytes[ind];
            ind = ind + 1;
        }
        to_return[i] = temp;
    }
    to_return[FrameCount] = nullptr;
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

bool AudioSuite::StereoToMono(WAV * wav)
{
    bool res = true;
    bool is_good = true;
    if (wav == nullptr)
    {
        is_good = false;
    }

    if (NumOfChannels() == 2 && is_good == true)
    {
        uint8_t* monoData = new uint8_t[wav->size / 2];

        for (int i = 0; i < wav->size; i += 2)
        {
            uint32_t v = static_cast<uint32_t>(wav->Data[i]) + wav->Data[i + 1];

            monoData[i / 2] = static_cast<uint8_t>(v / 2);
        }

        wav->Data = monoData;
        wav->header.NumChannels[0] = 1;
        wav->size = wav->size / 2;
    }
    else
    {
        res = false;
    }

    return res;
}

int AudioSuite::NumOfChannels(WAV * wav)
{
    bool is_good = true;
    if (wav == nullptr)
    {
        is_good = false;
    }

    return (int)wav->header.NumChannels[0];
}

int AudioSuite::CountZeroCrossings(boost::float32_t * signal, int signal_size)
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

float AudioSuite::CalculatePitch(float * signal, int signal_size, int sample_rate)
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

int AudioSuite::GetMidiNote(float pitch, float reference_pitch)
{
    return (C_REF_PITCH_NOTE_NUM + C_UNIQUE_PITCHES) * log2(pitch/reference_pitch);
} 

std::string AudioSuite::GetActualNote(float pitch, float reference_pitch)
{
    int note_number = GetMidiNote(pitch, reference_pitch);
    note_number -= C_MIDI_INDEX_ADJUST; // see the explanation below.
    return (Notes[note_number  % C_UNIQUE_PITCHES] + std::to_string(note_number / C_UNIQUE_PITCHES + 1));
} 

void AudioSuite::Spectrogram(WAV * wav)
{

}