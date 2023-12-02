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

    std::vector<float> n;
    // Assuming result->header.Data and result->fl_data have the same size
    result->fl_data.resize(result->header.Data.size());

    for (int i = 0; i < result->header.Data.size(); i++)
    {
        result->fl_data[i] = static_cast<float>(result->header.Data[i]);
    }

    Windowing(*result);

    FourierTransform(*result);

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
    // Define the size of the input sequence
    int N = wav.size;

    // Create input and output arrays
    float* out = static_cast<float*>(malloc(N * sizeof(float)));
    if (!out) {
        std::cerr << "Memory allocation failed for out array." << std::endl;
        return;
    }

    float* in = static_cast<float*>(malloc(N * sizeof(float)));
    if (!in) {
        std::cerr << "Memory allocation failed for in array." << std::endl;
        free(out);  // Free previously allocated memory
        return;
    }

    std::copy(wav.windows.begin(), wav.windows.end(), in);

    // Create a FFTW plan
    fftw_complex* fftw_in = reinterpret_cast<fftw_complex*>(in);
    fftw_complex* fftw_out = reinterpret_cast<fftw_complex*>(out);

    fftw_plan plan = fftw_plan_dft_1d(N, fftw_in, fftw_out, FFTW_FORWARD, FFTW_ESTIMATE);

    if (!plan) {
        std::cerr << "FFTW plan creation failed." << std::endl;
        free(in);   // Free allocated memory
        free(out);
        return;
    }

    // Execute the plan to compute the Fourier transform
    fftw_execute(plan);

    // Clean up
    fftw_destroy_plan(plan);
    free(in);
    free(out);
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

void AudioSuite::Windowing(WAV& wav)
{
    std::cout << "Data size: " << wav.size << std::endl;

    wav.windows.clear();

    std::vector<float> all(wav.size);

    for (int n = 0; n < wav.size; ++n)
    {
        all[n] = 0.5f * (1 - std::cos((2 * std::numbers::pi * wav.fl_data[n]) / C_HANN_WINDOW_SIZE_L));
    }

    wav.windows = all;
}