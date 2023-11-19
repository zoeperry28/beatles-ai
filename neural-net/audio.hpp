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
    char ChunkID      [C_CHUNKID_SIZE];
    char ChunkSize    [C_CHUNKSIZE_SIZE];
    char Format       [C_FORMAT_SIZE];
    // FMT Chunk
    char Subchunk1ID  [C_SUBCHUNK1ID_SIZE];
    char Subchunk1Size[C_SUBCHUNK1SIZE_SIZE];
    char AudioFormat  [C_AUDIOFORMAT_SIZE];
    char NumChannels  [C_NUMCHANNELS_SIZE];
    char SampleRate   [C_SAMPLERATE_SIZE];
    char ByteRate     [C_BYTERATE_SIZE];
    char BlockAlign   [C_BLOCKALIGN_SIZE];
    char BitsPerSample[C_BITSPERSAMPLE_SIZE];
    char EMPTY        [C_EMPTY_SIZE];
    // DATA Chunk
    char Subchunk2ID  [C_SUBCHUNK2ID_SIZE];
    char Subchunk2Size[C_SUBCHUNK2SIZE_SIZE];
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
        std::string path = "";

        WAV GetHeaderFromBytes(std::string path, uint8_t * bytes);
        int GetDataSize(std::string Path);
        uint8_t * GetData(std::string Path, uint8_t * data);
        std::vector<boost::float32_t *> GetAsFrames();
        std::vector<Frame> ExtractFrameData();
        int GetAsInt(char *c, int sz);

    public:

        WAV audio_wav;
        boost::float32_t * file; 
        WAV Load(std::string Path);

        Audio(){}
        Audio(std::string Path)
        {
            path = Path;
            Load(path);
        }
        ~Audio()
        {
            file = nullptr;
        }
        
        int CountZeroCrossings(boost::float32_t * signal, int signal_size);
        boost::float32_t * ByteToFloat(uint8_t * bytes, int size);
        float CalculatePitch(float * signal, int signal_size, int sample_rate=-1);
        int GetMidiNote(float pitch, float reference_pitch);
        std::string GetActualNote(float pitch, float reference_pitch);
        int NumOfChannels(WAV * wav = nullptr);
        boost::float32_t ** ToFrames(boost::float32_t * bytes, int sz);
        /*
         * Functions which modify the audio in some way 
         */
        bool StereoToMono(WAV * wav = nullptr);
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
        
        WAV * LoadFiles(std::string path, bool recursive = true);
        WAV * LoadFiles(std::vector<std::string> path, bool recursive = true);
};

#endif //  __AUDIO_H__
