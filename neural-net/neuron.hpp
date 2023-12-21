#ifndef __NEURON_H__
#define __NEURON_H__

#include "neural_net.hpp"
#include <boost/math/constants/constants.hpp>

namespace Activation {

    template <typename T>
    T LeakyRelu(T z, float NegSlope=0.01)
    {
        return std::max(static_cast<T>(0), z) + NegSlope * std::min(static_cast<T>(0), z);
    }
    template <typename T>
    T Relu(T z) {
        return std::max(static_cast<T>(0), z);
    }

    template <typename T>
    T Linear(T z)
    {
        return z;
    }

    template <typename T>
    T Signoid(T z)
    {
        return 1 / (1 + (std::exp(-z)));
    }

    template <typename T>
    T Tanh(T z)
    {
        return ((std::exp(z) - std::exp(-z)) /
                (std::exp(z) + std::exp(-z)));
    }
} 

class Neuron
{
private:
    int input = 0;
    int weight = 0;
    int bias = 0;
    int threshold = 0;
public:
    Neuron()
    {

    }
    ~Neuron()
    {

    }
};

class Input : Neuron
{
public:
    Input()
    {

    }
    ~Input()
    {

    }
    
    static void PrepareInputs(NN_Audio_Parameters NNAP);

private:
    static void HandleZeroCrossings(int* value, int size);
    static void HandlePitch(float* value, int size);
    static void HandleMagnitude(float* value, int size);
    static void HandlePhase(float* value, int size);
};

class Hidden : Neuron
{
public:
    Hidden()
    {

    }
    ~Hidden()
    {

    }

private:

};

class Output : Neuron
{
public:
    Output()
    {

    }
    ~Output()
    {

    }

private:

};




#endif
