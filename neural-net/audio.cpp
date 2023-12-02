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
#include <numbers> // std::numbers

#include <boost/dynamic_bitset.hpp>
#include <boost/cstdfloat.hpp>
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
    std::ifstream input(Path, std::ios::binary);

    std::vector<char> bytes(
        (std::istreambuf_iterator<char>(input)),
        (std::istreambuf_iterator<char>()));

    input.close();

    WAV* result = new WAV;

    if (bytes.size() < sizeof(WAV))
    {
        return nullptr;
    }
    WAV* wav = reinterpret_cast<WAV*>(bytes.data());
    result = std::move(wav);
    result->size = bytes.size() - C_WAV_File_SIZE;

    // Calculate the size of the data
    std::size_t dataSize = bytes.size() - C_WAV_File_SIZE;

    // Copy the data portion into the data vector
    result->header.Data.resize(dataSize);

    std::memcpy(result->header.Data.data(), bytes.data() + C_WAV_File_SIZE, dataSize);

    StereoToMono(*result);

    std::vector<boost::float32_t> n;
    for (int i = 0; i < result->header.Data.size(); i++)
    {
        n.push_back((boost::float32_t)result->header.Data[i]);
    }

    result->fl_data = n;
    GetFrames(*result);

    return result;
}

/*********************************************************************************************************************/

bool AudioSuite::StereoToMono(WAV& wav)
{
    bool res = true;
    bool is_good = true;
    if (&wav == nullptr)
    {
        is_good = false;
    }

    if (is_good == true)
    {
        std::vector<char> monoData;

        for (int i = 0; i < wav.header.Data.size(); i+=2)
        {
            monoData.push_back((wav.header.Data[i] + wav.header.Data[i+1]) / 2);
        }

        wav.header.Data = monoData;
        wav.header.NumChannels = 1;
        wav.size = wav.size / 2;
    }
    else
    {
        res = false;
    }

    return res;
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

void AudioSuite::FourierTransform(WAV& wav)
{
    std::vector<HannWindow> HW = Windowing(wav);
    for (const auto& window : HW)
    {
        std::vector<boost::float32_t> to_check = window.Points;
        for (int point = 0; point < to_check.size(); point++)
        {

        }
    }
}

/////////////////////////

std::vector<std::vector<boost::float32_t>> AudioSuite::GetFrames(WAV &wav)
{
    std::vector<std::vector<boost::float32_t>> to_return;
    std::vector<boost::float32_t> current; 

    int frame_size = wav.header.SampleRate - 1;

    for (int count = 0; count < wav.fl_data.size(); count++)
    {
        if (count % frame_size == 0)
        {
            to_return.push_back(current);
            current = {};
        }
        current.push_back(wav.fl_data[count]);
    }
    return to_return;
}

std::vector<HannWindow> AudioSuite::Windowing(WAV& wav)
{
    std::vector<HannWindow> vWN;
    HannWindow wv;
    for (int i = 0; i < wav.size; i++)
    {
        for (int j = i; j < i+C_HANN_WINDOW_SIZE_L; j++)
        {
            wv.Points.push_back((std::pow(std::sin(std::numbers::pi * wav.header.Data[j] / C_HANN_WINDOW_SIZE_L), 2)));  
        }
        wv.start_and_end = { i, i+C_HANN_WINDOW_SIZE_L};
        vWN.push_back(wv);
        wv = {};
    }
    return vWN;
}