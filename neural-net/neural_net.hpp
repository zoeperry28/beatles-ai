#ifndef _NEURAL_NET_HPP_
#define _NEURAL_NET_HPP_

#include <boost/dynamic_bitset.hpp>
#include <boost/cstdfloat.hpp>

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

class Prime_Data
{
    
};

class Neural_Net
{
    private:
        NEURAL_NET_Params params;
    public:
        Neural_Net(NEURAL_NET_Params _params)
        {
            params = _params;
        }
        int Feed_Forward();
        int Fetch_Result();
};

#endif // NEURAL_NET_HPP
