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
#include <cmath>
#include <boost/dynamic_bitset.hpp>
#include <boost/cstdfloat.hpp>
#include <boost/lexical_cast.hpp>
#include <fftw3.h>
#include <iterator>
#include <vector>
#include <iterator>
#include <cmath>
#include <complex>
#include <vector>
#include <boost/cstdint.hpp>
#include <vector>
#include <cmath>
#include <complex>
#include <iomanip>
#include <iostream>
#include <numbers>
#include "helper.hpp"

// Info on .wav file structure is taken from http://soundfile.sapp.org/doc/WaveFormat/

/*
*  @brief  This function takes the path of an audio file and loads it. Once the file is loaded, the data section of the 
*          file is then converted to float. 
*  @param  Path the path of the audio file to be loaded.
*  @return whether the file was read or not. 
*/
WAV AudioSuite::Load(std::string Path, bool recursive)
{
    std::ifstream input(Path, std::ios::binary);

    std::vector<char> bytes(
        (std::istreambuf_iterator<char>(input)),
        (std::istreambuf_iterator<char>()));

    input.close();

    WAV result; // Use a local variable instead of dynamic allocation
    result.filename = Path;

    if (bytes.size() < sizeof(WAV_File))
    {
        return result; // Return the local variable by value
    }

    WAV_File* wavFile = reinterpret_cast<WAV_File*>(bytes.data());
    result.header = *wavFile;
    result.size = bytes.size() - C_WAV_File_SIZE;

    // Calculate the size of the data
    std::size_t dataSize = bytes.size() - C_WAV_File_SIZE;

    // Copy the data portion into the data vector
    result.header.Data.resize(dataSize);
    std::memcpy(result.header.Data.data(), bytes.data() + C_WAV_File_SIZE, dataSize);

    StereoToMono(result);

    // Assuming result.header.Data and result.fl_data have the same size
    result.fl_data.resize(result.header.Data.size());

    for (int i = 0; i < result.header.Data.size(); i++)
    {
        result.fl_data[i] = static_cast<float>(result.header.Data[i]);
    }

    GetFrames(result);

    Windowing(result);

    return result;
}

/*********************************************************************************************************************/

std::vector<boost::float32_t> AudioSuite::FFT_GetMagnitude(WAV& wav)
{
    std::vector<boost::float32_t> to_return;
    to_return.reserve(wav.fourier.size() * wav.fourier[0].size()); // Reserve space for better performance

    for (const auto& row : wav.fourier)
    {
        for (const auto& entry : row)
        {
            to_return.push_back(entry.mag);
        }
    }

    return to_return;
}

std::vector<boost::float32_t> AudioSuite::FFT_GetPhase(WAV& wav)
{
    std::vector<boost::float32_t> to_return;
    to_return.reserve(wav.fourier.size() * wav.fourier[0].size()); // Reserve space for better performance

    for (const auto& row : wav.fourier)
    {
        for (const auto& entry : row)
        {
            to_return.push_back(entry.phase);
        }
    }

    return to_return;
}

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

int AudioSuite::CountZeroCrossings(WAV& wav)
{
    int crossings = 0;

    for (int i = 1; i < wav.size; ++i)
    {
        if ((wav.fl_data[i] >= 0 && wav.fl_data[i - 1] < 0) ||
            (wav.fl_data[i] < 0 && wav.fl_data[i - 1] >= 0))
        {
            crossings++;
        }
    }
    return crossings;
}

float AudioSuite::CalculatePitch(WAV &wav)
{
    float to_return = 0.0;
    int zero_crossings = CountZeroCrossings(wav);

    if (zero_crossings == 0) 
    {
        to_return = 0.0f;
    }
    to_return = wav.header.SampleRate / (2.0f * zero_crossings);
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

void AudioSuite::GetFrames(WAV& wav) {
    std::vector<std::vector<boost::float32_t> > to_return;

    for (int i = 0; i < wav.fl_data.size(); i += C_HANN_WINDOW_SIZE_L) {

        std::vector<boost::float32_t> all;

        int sz = C_HANN_WINDOW_SIZE_L;
        if (i != 0)
        {
            sz = C_HANN_WINDOW_SIZE_L + 2;
        }

        for (int j = i; j < i + sz; j++)
        {
            all.push_back(wav.fl_data[i]);
        }

        to_return.push_back(all);
    }
    wav.frames = to_return;
}

void AudioSuite::Windowing(WAV& wav)
{
    std::vector<std::vector<boost::float32_t>> final(wav.frames.size());

    for (int i = 0; i < wav.frames.size(); i++)
    {
        std::vector<boost::float32_t> all(wav.frames[i].size());
        for (int j = 0; j < wav.frames[i].size(); j++)
        {
            all[j] = 0.5f * (1 - std::cos((2 * std::numbers::pi * wav.frames[i][j]) / wav.frames[i].size()));
        }
        final[i] = all;
    }
    wav.windows = final;
    wav.frames.clear();
}

void AudioSuite::FourierTransform(WAV& wav, LoadingBar * LB) {

    bool Loading = false;
    if (LB != nullptr)
    {
        Loading = true;
    }

    LoadingBar L("Fourier Transform", wav.windows.size());
    std::vector<std::vector<FourierOut>> final;

    for (int i = 0; i < wav.windows.size(); i++) 
    {
        std::vector<std::complex<boost::float32_t>> result(wav.windows[i].begin(), wav.windows[i].end());
        FFT(result);

        std::vector<FourierOut> a;
        for (std::size_t j = 0; j < result.size(); ++j) 
        {
            FourierOut Out = 
            {
                result[j],
                std::abs(result[j]),
                std::arg(result[j])
            };
            a.push_back(Out);
        }
        final.push_back(a);
        L.LogProgress1();
    }
    wav.fourier = final;
}

void AudioSuite::FFT(std::vector<std::complex<boost::float32_t>>& data) 
{
    const std::size_t N = data.size();

    if (N <= 1) 
    {
        return;
    }

    std::vector<std::complex<boost::float32_t>> even(N / 2);
    std::vector<std::complex<boost::float32_t>> odd(N / 2);

    for (std::size_t i = 0; i < N / 2; ++i) 
    {
        even[i] = data[2 * i];
        odd[i] = data[2 * i + 1];
    }

    FFT(even);
    FFT(odd);

    for (std::size_t i = 0; i < N / 2; ++i)
    {
        std::complex<boost::float32_t> t(std::cos(-2.0f * std::numbers::pi * i / N),
            std::sin(-2.0f * std::numbers::pi * i / N));
        std::complex<boost::float32_t> temp = t * odd[i];
        data[i] = even[i] + temp;
        data[i + N / 2] = even[i] - temp;
    }
}
