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

    LoadingBar L(("Processing: " + wav.filename), wav.windows.size());
    std::vector<std::vector<FourierOut>> final(wav.windows.size());

    for (int i = 0; i < wav.windows.size(); i++) 
    {   
        FFT((std::vector<std::vector<float>> &)wav.windows[i]);

        std::vector<FourierOut> a;
        for (std::size_t j = 0; j < wav.windows[i].size(); ++j)
        {
            FourierOut Out = 
            {
                wav.windows[i][j],
                std::abs(wav.windows[i][j]),
                std::arg(wav.windows[i][j])
            };
            a.push_back(Out);
        }
        final.push_back(a);
        L.LogProgress1();
    }
    wav.fourier = final;
}

void AudioSuite::FFT(std::vector<std::vector<float>>& data)
{
    const std::size_t N = data.size();

    // Base case: if the size of the data is 1 or 0, no processing is needed
    if (N <= 1)
    {
        return;
    }

    // Divide the data into even and odd parts
    std::vector<std::vector<boost::float32_t>> even(N / 2);
    std::vector<std::vector<boost::float32_t>> odd(N / 2);

    for (std::size_t i = 0; i < N / 2; ++i)
    {
        even[i] = data[2 * i];
        odd[i] = data[2 * i + 1];
    }

    // Recursive FFT on even and odd parts
    FFT(even);
    FFT(odd);

    // Combine the results of FFT on even and odd parts
    for (std::size_t i = 0; i < N / 2; ++i)
    {
        // Twiddle factor to perform complex multiplication
        boost::float32_t angle = -2.0f * std::numbers::pi * i / N;
        std::complex<boost::float32_t> twiddle(std::cos(angle), std::sin(angle));

        // Multiply odd part by twiddle factor
        for (std::size_t j = 0; j < data[i].size(); ++j)
        {
            std::complex<boost::float32_t> temp = twiddle * odd[i][j];

            // Combine results
            data[i][j] = even[i][j] + temp.real();  // Use real() to get the real part
            data[i + N / 2][j] = even[i][j] - temp.real();
        }
    }
}


boost::float32_t AudioSuite::MelScale(boost::float32_t f)
{
    return 1125 * std::log(1 + f / 700);
}

boost::float32_t AudioSuite::MelToFreq(boost::float32_t mel)
{
    return 700 * (std::exp(mel / 1125) - 1);
}

std::vector<boost::float32_t> AudioSuite::MelToFreq(const std::vector<float>& mel)
{
    std::vector<boost::float32_t> to_return(mel.size());
    std::transform(mel.begin(), mel.end(), to_return.begin(), [](float mel_value) {
        return 700 * (std::exp(mel_value / 1125) - 1);
        });
    return to_return;
}

std::vector<boost::float32_t> AudioSuite::PointFloor(int FFTSize, std::vector<boost::float32_t> freq_points, int SampleRate)
{
    std::vector<boost::float32_t> to_return(freq_points.size());
    for (int i = 0; i < freq_points.size(); i++)
    {
        to_return[i] = std::floor((FFTSize + 1) * freq_points[i] / SampleRate);
    }
    return to_return;
}


// Function to calculate points for the Mel Filter Bank
void MFB_CalcForPoints(std::vector<float>& all_points, const std::vector<float>& bin_points, int m, bool useBinMinusOne)
{
    for (int i = 0; i < all_points.size(); i++)
    {
        int binIndex = useBinMinusOne ? m - 1 : m;
        if (binIndex >= 0 && binIndex < bin_points.size() && m + 1 < bin_points.size()) {
            all_points[i] = (all_points[i] - bin_points[binIndex]) / (bin_points[m + 1] - bin_points[m]);
        }
    }
}

std::vector<std::vector<boost::float32_t>> AudioSuite::MelFilterBank(int numFilters, int fftSize, int sampleRate) {
    const int lowFreq = 0;
    const int highFreq = sampleRate / 2;

    const boost::float32_t melLow = MelScale(lowFreq);
    const boost::float32_t melHigh = MelScale(highFreq);

    const std::vector<float> melPoints  = LinSpace(melLow, melHigh, numFilters + 2);
    const std::vector<float> freqPoints = MelToFreq(melPoints);
    const std::vector<float> binPoints  = PointFloor(fftSize, freqPoints, sampleRate);

    std::vector<std::vector<float>> filters(numFilters-2);

    for (int m = 1; m < numFilters-1; m++) {
        std::vector<float> filterM = Zeros(numFilters);

        AddAtIndex(filterM, Arange(binPoints[m - 1], (binPoints[m] + 1), 1), m - 1, m);
        MFB_CalcForPoints(filterM, binPoints, m, true);
        
        AddAtIndex(filterM, Arange(binPoints[m - 1], binPoints[m] + 1, 1), m, m + 1);
        MFB_CalcForPoints(filterM, binPoints, m, false);

        filters.push_back(filterM);
    }

    return filters;
}

std::vector<std::vector<boost::float32_t>> AudioSuite::LogCompress(std::vector<std::vector<boost::float32_t>>& f)
{
    std::vector<std::vector<boost::float32_t>> G;
    for (std::vector<boost::float32_t>& row : f) 
    {
        std::vector<boost::float32_t> h(row.size());
        for (boost::float32_t& value : row) 
        {
            value = std::log(value);
            h.push_back(value);
        }
        G.push_back(h);
    }
    return G;
}

void AudioSuite::PreEmphasis(std::vector<boost::float32_t> & data)
{
    for (int i = 1; i < data.size(); i++)
    {
        data[i] = data[i] - (C_PREMPHASIS_COEFF * data[i - 1]);
    }
}

boost::float32_t AudioSuite::DiscreteCosTransform_Norm(int m, int M)
{
    boost::float32_t to_return = 0;
    if (m == 0)
    {
        to_return = std::sqrt(2 / M);
    }
    else if (m > 0)
    {
        to_return = std::sqrt(2 / M)* std::cos((std::numbers::pi * m) / M);
    }
    return to_return;
}

std::vector<std::vector<float>> AudioSuite::DiscreteCosTransform(std::vector<std::vector<boost::float32_t>>& C, int M)
{
    std::vector<std::vector<float>> X(M);
    for (int m = 0; m < M; m++)
    {
        std::vector<float> Y(C[m].size());
        for (int x = 0; x < C[m].size(); x++)
        {
            Y[x] = DiscreteCosTransform_Norm(m, M)* (M - 1)* C[m][x] * std::cos((std::numbers::pi * ((2 * m) + 1) * m) / 2 * M);
        }
        X[m] = Y;
    }
    return X;
} 

std::vector<std::vector<boost::float32_t>> AudioSuite::TriangularFilter()
{
    std::vector<std::vector<boost::float32_t>> to_return;

    return to_return;
}

std::vector<std::vector<boost::float32_t>> AudioSuite::TriangularFilterBank(int numFilters, int fftSize, int sampleRate)
{
    std::vector<std::vector<boost::float32_t>> to_return;

    return to_return;
}

std::vector<boost::float32_t> AudioSuite::ApplyTriangularFilterBank(const std::vector<boost::float32_t>& spectrum, const std::vector<std::vector<boost::float32_t>>& filterBank)
{
    std::vector<boost::float32_t> to_return;
    return to_return;
}

std::vector<std::vector<float>> AudioSuite::MFCC(WAV& wav)
{
    // Preprocess the audio data
    PreEmphasis(wav.fl_data);
    GetFrames(wav);

    // Apply windowing to each frame
    Windowing(wav);

    // Apply FFT to windowed frames
    FFT(wav.windows);

    // Mel filter bank calculation
    std::vector<std::vector<boost::float32_t>> melFilterBank = MelFilterBank(20, 10, wav.header.SampleRate);


    // Create triangular filter bank
    std::vector<std::vector<boost::float32_t>> triangularFilterBank = TriangularFilterBank(20, 10, wav.header.SampleRate);

    // Apply triangular filter bank to the magnitude spectrum
    std::vector<std::vector<boost::float32_t>> filterBankOutput;
    for (const auto& spectrum : wav.windows)
    {
        filterBankOutput.push_back(ApplyTriangularFilterBank(spectrum, triangularFilterBank));
    }

    // Log-compress the filter bank energies
    std::vector<std::vector<boost::float32_t>> logCompressed = LogCompress(melFilterBank);

    // Discrete cosine transform
    std::vector<std::vector<float>> mfccResult = DiscreteCosTransform(logCompressed, melFilterBank.size());

    // The result 'mfccResult' contains the MFCC coefficients for each frame
    // Depending on your application, you may want to use or store these coefficients
    return mfccResult;
}

