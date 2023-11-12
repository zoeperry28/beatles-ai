#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <string.h>
#include <iostream>
#include <filesystem>
 
#define C_REF_PITCH_NOTE_NUM (69)
#define C_UNIQUE_PITCHES (12)
#define C_MIDI_INDEX_ADJUST (21)
#define C_WAV_HEADER_SIZE (44)
#define C_MB_TO_BYTES (1e+6)

const std::string Notes [C_UNIQUE_PITCHES] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};

typedef struct RIFF_Chunk
{
    uint8_t ChunkID[4];
    uint8_t ChunkSize[4];
    uint8_t Format[4];
} RIFF_Chunk;

typedef struct FMT_Chunk
{
    uint8_t Subchunk1ID[4];
    uint8_t Subchunk1Size[4];
    uint8_t AudioFormat[2];
    uint8_t NumChannels[2];
    uint8_t SampleRate[4];
    uint8_t ByteRate[4];
    uint8_t BlockAlign[2];
    uint8_t BitsPerSample[2];
    uint8_t EMPTY[2];
} FMT_Chunk;

typedef struct Data_Chunk
{
    uint8_t Subchunk2ID[4];
    uint8_t Subchunk2Size[4];
} Data_Chunk;

typedef union WAV_Header 
{
    RIFF_Chunk RIFF;
    FMT_Chunk FMT;
    Data_Chunk DAT;
} WAV_Header;

typedef struct WAV
{
    WAV_Header header;
    uint8_t* Data;
} WAV;

class Audio
{
    private:
        uint64_t data_size; 
        WAV audio_wav;

        int CountZeroCrossings(float * signal, int signal_size);
        WAV GetHeaderFromBytes(uint8_t * bytes);
        int GetDataSize(std::string Path, uint8_t * bytes);
        uint8_t * GetData(std::string Path, uint8_t * data);
    public:
        Audio()
        {
            
        }
        int Load(std::string Path);
        float CalculatePitch(float * signal, int signal_size, int sample_rate);
        int GetMidiNote(float pitch, float reference_pitch);
        std::string GetActualNote(float pitch, float reference_pitch);
};


#endif //  __AUDIO_H__
