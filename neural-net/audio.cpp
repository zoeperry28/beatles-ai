#include "audio.hpp"
#include <math.h>
#include <string>
#include <fftw3.h>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <vector>

int Audio::Load(std::string Path) 
{
    std::vector<char>result;

    if (std::experimental::filesystem::path(Path).extension() == ".wav")
    { 
        std::ifstream ifs(Path, std::ios::binary|std::ios::ate);
        std::ifstream::pos_type pos = ifs.tellg();

        std::vector<char> result(pos);

        ifs.seekg(0, std::ios::beg);
        ifs.read(&result[0], pos);

        for (int item = 0 ; item < (int) result.size() ; item++)
        {
            std::cout << result[item];
        }
    }
    else
    {
        throw std::invalid_argument("As this model expects data to be generated from the externally included script, the program will only accept .wav files.");
    }
    return 1;
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

