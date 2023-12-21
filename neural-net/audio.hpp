#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <string.h>
#include <iostream>
#include <filesystem>
#include <boost/dynamic_bitset.hpp>
#include <boost/cstdfloat.hpp>


#define C_BITFIELD_MUL (8)
#define C_NO_OF_EMPTY_FIELDS (5)
#define C_REF_PITCH_NOTE_NUM (69)
#define C_UNIQUE_PITCHES (12)
#define C_MIDI_INDEX_ADJUST (21)
#define C_MB_TO_BYTES (1e+6)
#define C_HANN_WINDOW_SIZE_L (512)
// WAV Header Sizes
#define C_CHUNKID_SIZE       (4U)
#define C_CHUNKSIZE_SIZE     (4U)
#define C_FORMAT_SIZE        (4U)
#define C_SUBCHUNK1ID_SIZE   (4U)
#define C_SUBCHUNK1SIZE_SIZE (4U)
#define C_AUDIOFORMAT_SIZE   (2U)
#define C_NUMCHANNELS_SIZE   (2U)
#define C_SAMPLERATE_SIZE    (4U)
#define C_BYTERATE_SIZE      (4U)
#define C_BLOCKALIGN_SIZE    (2U)
#define C_BITSPERSAMPLE_SIZE (2U)
#define C_EMPTY_SIZE         (34U)
#define C_SUBCHUNK2ID_SIZE   (4U)
#define C_SUBCHUNK2SIZE_SIZE (4U)

const float C_PREMPHASIS_COEFF = 0.97;

const int C_WAV_File_SIZE =
    C_CHUNKID_SIZE +
    C_CHUNKSIZE_SIZE +
    C_FORMAT_SIZE +
    C_SUBCHUNK1ID_SIZE +
    C_SUBCHUNK1SIZE_SIZE +
    C_AUDIOFORMAT_SIZE +
    C_NUMCHANNELS_SIZE +
    C_SAMPLERATE_SIZE +
    C_BYTERATE_SIZE +
    C_BLOCKALIGN_SIZE +
    C_BITSPERSAMPLE_SIZE +
    C_EMPTY_SIZE +
    C_SUBCHUNK2ID_SIZE +
    C_SUBCHUNK2SIZE_SIZE
;
const std::string C_EMPTY_STRING = "NONE";

// Info on .wav file structure is taken from http://soundfile.sapp.org/doc/WaveFormat/

const std::string Notes [C_UNIQUE_PITCHES] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};

typedef struct Frame
{
    boost::float32_t * Frame;
    std::string AverageMidiNote;
    float AveragePitch; 
    int ZeroCrossings; 

} Frame;

typedef struct FourierOut
{
    std::complex<float> freq;
    int mag;
    int phase;

}FourierOut;

typedef struct WAV_File {
    uint32_t ChunkID        :C_BITFIELD_MUL * C_CHUNKID_SIZE;
    uint32_t ChunkSize      :C_BITFIELD_MUL * C_CHUNKSIZE_SIZE;
    uint32_t Format         :C_BITFIELD_MUL * C_FORMAT_SIZE;
    uint32_t Subchunk1ID    :C_BITFIELD_MUL * C_SUBCHUNK1ID_SIZE;
    uint32_t Subchunk1Size  :C_BITFIELD_MUL * C_SUBCHUNK1SIZE_SIZE;
    uint32_t AudioFormat    :C_BITFIELD_MUL * C_AUDIOFORMAT_SIZE;
    uint32_t NumChannels    :C_BITFIELD_MUL * C_NUMCHANNELS_SIZE;
    uint32_t SampleRate     :C_BITFIELD_MUL * C_SAMPLERATE_SIZE;
    uint32_t ByteRate       :C_BITFIELD_MUL * C_BYTERATE_SIZE;
    uint32_t BlockAlign     :C_BITFIELD_MUL * C_BLOCKALIGN_SIZE;
    uint32_t BitsPerSample  :C_BITFIELD_MUL * C_BITSPERSAMPLE_SIZE;
    uint64_t EMPTY0         :(C_BITFIELD_MUL * C_EMPTY_SIZE) / C_NO_OF_EMPTY_FIELDS;
    uint64_t EMPTY1         :(C_BITFIELD_MUL * C_EMPTY_SIZE) / C_NO_OF_EMPTY_FIELDS;
    uint64_t EMPTY2         :(C_BITFIELD_MUL * C_EMPTY_SIZE) / C_NO_OF_EMPTY_FIELDS;
    uint64_t EMPTY3         :(C_BITFIELD_MUL * C_EMPTY_SIZE) / C_NO_OF_EMPTY_FIELDS;
    uint64_t EMPTY4         :(C_BITFIELD_MUL * C_EMPTY_SIZE) / C_NO_OF_EMPTY_FIELDS;
    uint32_t Subchunk2ID    :C_BITFIELD_MUL * C_SUBCHUNK2ID_SIZE;
    uint32_t Subchunk2Size  :C_BITFIELD_MUL * C_SUBCHUNK2SIZE_SIZE;
    std::vector<char> Data;

} WAV_File;



typedef struct WAV
{
    std::string filename;
    WAV_File header;

    std::string path; 
    std::vector<float> fl_data;
    std::vector<std::vector<boost::float32_t>> frames;
    std::vector<std::vector<boost::float32_t>> windows;
    std::vector<std::vector<FourierOut>> fourier;

    int size;

} WAV;

/*
 * @brief This class defines a bunch of useful functions which can be performed on audio data. 
 */ 
class AudioSuite
{
    public:
        WAV Load(std::string Path, bool recursive = true);
        int CountZeroCrossings(WAV &wav);
        float CalculatePitch(WAV &wav);
        int GetMidiNote(float pitch, float reference_pitch);
        std::string GetActualNote(float pitch, float reference_pitch);
        bool StereoToMono(WAV& wav );

        void FourierTransform(WAV& wav);
        void Spectrogram(WAV * wav);        
        std::vector<boost::float32_t> FFT_GetMagnitude(WAV& wav);
        std::vector<boost::float32_t> FFT_GetPhase(WAV& wav);
        void MFCC(WAV& wav);
private:
        void Windowing(WAV& wav);
        void GetFrames(WAV& wav);
        void FFT(std::vector<std::complex<boost::float32_t>>& data);
        std::vector<std::complex<boost::float32_t>> FastFourierTransform(std::vector<boost::float32_t>& to_check);


        void FFT(std::vector<std::vector<boost::float32_t>>& data);
        void PreEmphasis(std::vector<boost::float32_t>& data);
        std::vector<std::vector<boost::float32_t>> MelFilterBank(int FilterNo, int FFTSize, int SampleRate);

        boost::float32_t MelScale(boost::float32_t f);
        boost::float32_t MelToFreq(boost::float32_t mel);
        std::vector<boost::float32_t> MelToFreq(const std::vector<float>& mel);
        std::vector<boost::float32_t> PointFloor(int FFTSize, std::vector<boost::float32_t> freq_points, int SampleRate);
        std::vector<std::vector<boost::float32_t>> LogCompress(std::vector<std::vector<boost::float32_t>>& f);

        std::vector<std::vector<float>> DiscreteCosTransform(std::vector<std::vector<boost::float32_t>>& C, int M);
        boost::float32_t DiscreteCosTransform_Norm(int m, int M);

        std::vector<std::vector<boost::float32_t>> AudioSuite::TriangularFilterBank(int numFilters, int fftSize, int sampleRate);
        std::vector<boost::float32_t> AudioSuite::ApplyTriangularFilterBank(const std::vector<boost::float32_t>& spectrum, const std::vector<std::vector<boost::float32_t>>& filterBank);
};

#endif //  __AUDIO_H__
