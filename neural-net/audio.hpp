#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <string.h>
#include <iostream>
#include <filesystem>
#include <boost/dynamic_bitset.hpp>
#include <boost/cstdfloat.hpp>
 
#define C_REF_PITCH_NOTE_NUM (69)
#define C_UNIQUE_PITCHES (12)
#define C_MIDI_INDEX_ADJUST (21)
#define C_WAV_HEADER_SIZE (78)
#define C_MB_TO_BYTES (1e+6)

// WAV Header Sizes
#define C_CHUNKID_SIZE (4U)
#define C_CHUNKSIZE_SIZE (4U)
#define C_FORMAT_SIZE (4U)
#define C_SUBCHUNK1ID_SIZE (4U)
#define C_SUBCHUNK1SIZE_SIZE (4U)
#define C_AUDIOFORMAT_SIZE (2U)
#define C_NUMCHANNELS_SIZE (2U)
#define C_SAMPLERATE_SIZE (4U)
#define C_BYTERATE_SIZE (4U)
#define C_BLOCKALIGN_SIZE (2U)
#define C_BITSPERSAMPLE_SIZE (2U)
#define C_EMPTY_SIZE (34U)
#define C_SUBCHUNK2ID_SIZE (4U)
#define C_SUBCHUNK2SIZE_SIZE (4U)

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

typedef struct WAV_Header {
    // RIFF Chunk
    char ChunkID      [4];
    char ChunkSize    [4];
    char Format       [4];
    // FMT Chunk
    char Subchunk1ID  [4];
    char Subchunk1Size[4];
    char AudioFormat  [2];
    char NumChannels  [2];
    char SampleRate   [4];
    char ByteRate     [4];
    char BlockAlign   [2];
    char BitsPerSample[2];
    char EMPTY        [34];
    // DATA Chunk
    char Subchunk2ID  [4];
    char Subchunk2Size[4];
} WAV_Header;

typedef struct WAV
{
    WAV_Header header;
    uint8_t* Data;
    int size;
} WAV;

class Audio
{
    private:
        WAV audio_wav;
        std::string path = "";

        int CountZeroCrossings(float * signal, int signal_size);
        WAV GetHeaderFromBytes(std::string path, uint8_t * bytes);
        int GetDataSize(std::string Path);
        uint8_t * GetData(std::string Path, uint8_t * data);
        std::vector<boost::float32_t *> GetAsFrames();
        std::vector<Frame> ExtractFrameData();
        int GetAsInt(char *c, int sz);

    public:

        boost::float32_t * file; 
        WAV Load(std::string Path);

        Audio()
        {
            
        }
        Audio(std::string Path)
        {
            path = Path;
            Load(path);
        }
        boost::float32_t * ByteToFloat(uint8_t * bytes, int size);
        float CalculatePitch(float * signal, int signal_size, int sample_rate);
        int GetMidiNote(float pitch, float reference_pitch);
        std::string GetActualNote(float pitch, float reference_pitch);
        int NumOfChannels();
        bool StereoToMono();

        int ToFrames();


    friend class Bulk_Audio;
    
};

class Bulk_Audio : Audio
{
    public: 
        Bulk_Audio()
        {
            
        }
        Bulk_Audio(std::string Path)
        {
            LoadFiles(Path);
        }
        
        int LoadFiles(std::string path, bool recursive = false);
        int LoadFiles(std::vector<std::string> path, bool recursive = false);
};

#endif //  __AUDIO_H__
