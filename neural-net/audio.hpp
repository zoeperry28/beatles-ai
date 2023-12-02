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
#define C_HANN_WINDOW_SIZE_L (10)
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
    WAV_File header;

    std::string path; 
    std::vector<float> fl_data;
    std::vector<float> windows; 

    int size;

} WAV;

/*
 * @brief This class defines a bunch of useful functions which can be performed on audio data. 
 */ 
class AudioSuite
{
    public:
        WAV * Load(std::string Path, bool recursive = true);
        int CountZeroCrossings(boost::float32_t * signal, int signal_size);
        float CalculatePitch(float * signal, int signal_size, int sample_rate=-1);
        int GetMidiNote(float pitch, float reference_pitch);
        std::string GetActualNote(float pitch, float reference_pitch);
        bool StereoToMono(WAV& wav );

        void Windowing(WAV& wav);
        void FourierTransform(WAV& wav);
        void Spectrogram(WAV * wav);
    private:
        std::vector<std::vector<boost::float32_t>> GetFrames(WAV& wav);

    friend class Audio;
};

#endif //  __AUDIO_H__
