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
    std::string path; 
    WAV_Header header;
    uint8_t* Data;
    boost::float32_t * fl_data;
    int size;
    boost::float32_t ** frames;
} WAV;

class Audio
{
    private:
        boost::float32_t * float_data;
        int GetDataSize(std::string Path);
        uint8_t * GetData(std::string Path, uint8_t * data);
        std::vector<boost::float32_t *> GetAsFrames();
        std::vector<Frame> ExtractFrameData();
        int GetAsInt(char *c, int sz);
        boost::float32_t * ByteToFloat(uint8_t * bytes, int size);
        boost::float32_t ** ToFrames(boost::float32_t * bytes, int sz);

    public:
        WAV audio_wav;
        boost::float32_t DataAsFloat();
        WAV GetHeaderFromBytes(std::string path, uint8_t * bytes);

        Audio(std::string Path)
        {
        }
        Audio(std::vector<std::string> Path)
        {
        }
        Audio()
        {
            
        }
        ~Audio()
        {
        }

    friend class AudioSuite;
};

/*
 * @brief This class defines a bunch of useful functions which can be performed on audio data. 
 */ 
class AudioSuite
{
    public:
        Audio * audio;
        std::vector<Audio> files; 

        WAV * Load(std::string Path, bool recursive = true);
        WAV * Load(std::vector<std::string> path, bool recursive = true);
        int CountZeroCrossings(boost::float32_t * signal, int signal_size);
        float CalculatePitch(float * signal, int signal_size, int sample_rate=-1);
        int GetMidiNote(float pitch, float reference_pitch);
        std::string GetActualNote(float pitch, float reference_pitch);
        int NumOfChannels(WAV * wav = nullptr);
        bool StereoToMono(WAV * wav = nullptr);
    
    friend class audio;
};

#endif //  __AUDIO_H__
