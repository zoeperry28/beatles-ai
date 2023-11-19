#include <boost/dynamic_bitset.hpp>
#include <boost/cstdfloat.hpp>

#ifndef _NEURAL_NET_HPP_
#define _NEURAL_NET_HPP_

#include "audio.hpp"

#define C_INPUTSIZE_VALUE (0U)
#define C_HIDDENLAYERSIZE_VALUE (0U)
#define C_OUTPUTSIZE_VALUE (0U)

typedef struct NN_Input 
{
    int value1;
    int valuen;
} NN_Input;

typedef struct NN_LayerSizes
{
    int inputsize;
    int hiddenlayersize;
    int outputsize;
} NN_LayerSizes;

typedef struct NEURAL_NET_Params
{
    NN_Input Input;
    NN_LayerSizes Sizes;
} NEURAL_NET_Params;

typedef struct NN_Audio_Parameters
{
    boost::float32_t * frames; 
    int ZeroCrossingCount; 
    boost::float32_t Pitch;

} NN_Audio_Parameters;


const NN_LayerSizes LayerSizes = 
{
    C_INPUTSIZE_VALUE,
    C_HIDDENLAYERSIZE_VALUE,
    C_OUTPUTSIZE_VALUE,
};

class Prime_Data
{
    public:
        Prime_Data()
        {

        }
        NN_Audio_Parameters * PrepareAudioData(Bulk_Audio * audio); 
        NN_Audio_Parameters * PrepareAudioData(Audio * audio);
};

class Neural_Net
{
    private:
        NEURAL_NET_Params params;
    public:
        Neural_Net()
        {

        }
        Neural_Net(NEURAL_NET_Params _params)
        {
            params = _params;
        }
        int Feed_Forward(NN_Audio_Parameters * Prepared_Data);
        int Fetch_Result();
};

#endif // NEURAL_NET_HPP
