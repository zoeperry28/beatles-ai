#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <string.h>
#include <iostream>
 
#define C_REF_PITCH_NOTE_NUM (69)
#define C_UNIQUE_PITCHES (12)
#define C_MIDI_INDEX_ADJUST (21)

const std::string Notes [C_UNIQUE_PITCHES] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};

class Audio
{
    private:
        int CountZeroCrossings(float * signal, int signal_size);
    public:
        Audio()
        {
            
        }
        int Load(std::string path);
        float CalculatePitch(float * signal, int signal_size, int sample_rate);
        int GetMidiNote(float pitch, float reference_pitch);
        std::string GetActualNote(float pitch, float reference_pitch);
};


#endif //  __AUDIO_H__