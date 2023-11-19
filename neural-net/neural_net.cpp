#include "neural_net.hpp"
#include "audio.hpp"

void Prime_Data::PrepareAudioData(Audio * audio)
{
    // This gives us an array of arrays which represent the floating point data from
    // the audio file
    boost::float32_t ** frames = audio->ToFrames(audio->file, audio->audio_wav.size);

    
}

int Neural_Net::Feed_Forward()
{
    return -1;
}

int Neural_Net::Fetch_Result()
{
    return -1;
}

