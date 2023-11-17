#include "audio.hpp"
#include <math.h>
#include <string>
#include <fftw3.h>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include <boost/cstdfloat.hpp>
#include <boost/filesystem.hpp>

// Info on .wav file structure is taken from http://soundfile.sapp.org/doc/WaveFormat/

int Audio::GetDataSize(std::string Path, uint8_t * bytes)
{
    return std::filesystem::file_size(std::filesystem::path(Path)) - C_WAV_HEADER_SIZE;
}

WAV Audio::GetHeaderFromBytes(uint8_t * bytes)
{
    WAV wav;
    memcpy(&wav.header, bytes, sizeof(WAV_Header));
    wav.Data = bytes + sizeof(WAV_Header);
    return wav;
}

bool Audio::StereoToMono()
{
    bool res = true;
    uint8_t * m;
    if (NumOfChannels() == 2)
    {
        for (uint32_t i = 0 ; i < data_size; i+=2)
        {
            uint32_t v = audio_wav.Data[i] + audio_wav.Data[i+1];
            *m = (uint8_t) v / 2;
            m++;
        }
    }
    else
    {
        res = false;
    }
    audio_wav.Data = m;
    return res;
}

int Audio::NumOfChannels()
{
    return (int)audio_wav.header.NumChannels[0];
}

boost::float32_t * Audio::ByteToFloat()
{
    boost::float32_t * f; 
    for (uint64_t i = 0 ; i < data_size; i++)
    {
        *f = (boost::float32_t) audio_wav.Data[i];
        f++;
    }
    return f;
}

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
        data_size = GetDataSize(Path, conv_file_vec);
        status = 1;
    }
    else if (IsValidFile == false)
    {
        throw std::invalid_argument("Invalid file!");
    }
    else
    {
        throw std::invalid_argument("As this model expects data to be generated from the externally included script, the program will only accept .wav files. Sorry!");
    }
    return status;
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
    return (Notes[note_number % C_UNIQUE_PITCHES] + std::to_string(note_number / C_UNIQUE_PITCHES + 1));
} 

int Bulk_Audio::LoadFiles(std::string path, bool recursive = false)
{
    return 0;
}

int Bulk_Audio::LoadFiles(std::vector<std::string> path, bool recursive = false)
{
    return 0;
}