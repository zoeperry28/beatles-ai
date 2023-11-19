#include "neural_net.hpp"
#include "audio.hpp"

NN_Audio_Parameters * Prime_Data::PrepareAudioData(Audio * audio)
{
    // This gives us an array of arrays which represent the floating point data from
    // the audio file
    boost::float32_t ** frames = audio->ToFrames(audio->file, audio->audio_wav.size);
    boost::float32_t * Current = *frames;

    NN_Audio_Parameters * Prepared_Data; 
    while(Current != nullptr)
    {
        NN_Audio_Parameters Audio_Parameters = {
            Current, 
            audio->CountZeroCrossings(Current, audio->audio_wav.size),
            audio->CalculatePitch(Current, audio->audio_wav.size)
        };
        Prepared_Data = &Audio_Parameters;
        Prepared_Data++;
        Current++;
    }

    return Prepared_Data;
}

int Neural_Net::Feed_Forward(NN_Audio_Parameters * Prepared_Data)
{
    
    return -1;
}

int Neural_Net::Fetch_Result()
{
    return -1;
}

