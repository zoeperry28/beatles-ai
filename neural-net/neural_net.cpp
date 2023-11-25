#include "neural_net.hpp"
#include "audio.hpp"

NN_Audio_Parameters * Prime_Data::PrepareAudioData(WAV * wavs, int file_count)
{
    NN_Audio_Parameters * A_Audio_Parameters;
    for (int i = 0 ; i < file_count; i++)
    {
        //This gives us an array of arrays which represent the floating point data from
        //the audio file
        boost::float32_t * Current = wavs[i].frames[0];

        while(Current != nullptr)
        {
            NN_Audio_Parameters Audio_Parameters = {
                Current, 
                AS->CountZeroCrossings(Current, wavs[i].size),
                AS->CalculatePitch(Current, wavs[i].size)
            };
            A_Audio_Parameters = &Audio_Parameters;
            A_Audio_Parameters++;
            Current++;
        }
    }
    return A_Audio_Parameters;
}

int Neural_Net::Feed_Forward(NN_Audio_Parameters * Prepared_Data)
{
    
    return -1;
}

int Neural_Net::Fetch_Result()
{
    return -1;
}

