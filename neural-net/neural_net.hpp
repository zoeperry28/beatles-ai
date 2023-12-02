#include <boost/dynamic_bitset.hpp>
#include <boost/cstdfloat.hpp>
#include "audio.hpp"
#include <tuple>
#include <vector>

#ifndef _NEURAL_NET_HPP_
#define _NEURAL_NET_HPP_

#define C_INPUTSIZE_VALUE (0U)
#define C_HIDDENLAYERSIZE_VALUE (0U)
#define C_OUTPUTSIZE_VALUE (0U)
#define C_NUMBER_OF_CHECKS (2)

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

typedef enum Neural_Net_Mode
{
    E_NO_MODE = 0,
    E_DATA_GATHERING = 1, 
    E_TRAINING = 2, 
    E_EXECUTION = 3
} Neural_Net_Mode;

const NN_LayerSizes LayerSizes = 
{
    C_INPUTSIZE_VALUE,
    C_HIDDENLAYERSIZE_VALUE,
    C_OUTPUTSIZE_VALUE,
};

class Prime_Data
{
    AudioSuite * AS;   
    public:
        Prime_Data()
        {

        }
        Prime_Data(AudioSuite& audio)
        { 
            AS = &audio;
        }
        NN_Audio_Parameters * PrepareAudioData(std::vector<WAV *>& wav);
    
    friend class Audio;
    friend class Neural_Net_Modes;
};

class Neural_Net
{
    private:
        NEURAL_NET_Params params;
    public:

        std::tuple<std::string, int, boost::float32_t> CSV_Line;

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

class Neural_Net_Modes : AudioSuite
{
    private: 
        static std::vector<WAV*> Get_Wavs(std::vector<std::string>& files);
    public:
        static Neural_Net_Mode ParseArgs(const std::string& MODE);
        static bool Data_Gathering(std::vector<std::string>& files);
        static bool Training(std::vector<std::string>& files);
        static bool Execution(std::vector<std::string>& files);
};

#endif // NEURAL_NET_HPP
