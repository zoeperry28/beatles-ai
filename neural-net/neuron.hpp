#ifndef __NEURON_H__
#define __NEURON_H__

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

typedef enum Neuron_Type
{
    E_INPUT = 0,
    E_HIDDEN = 1, 
    E_OUTPUT = 2,
}Neuron_Type;

class Neuron
{
private:
    int input = 0;
    int weight = 0;
    int bias = 0;
    int threshold;
public:
    Neuron(Neuron_Type);
    ~Neuron();

};

Neuron::Neuron(Neuron_Type Neuron)
{
    switch (Neuron)
    {
    case E_INPUT:
        break;
    case E_HIDDEN:
        break;
    case E_OUTPUT:
        break;
    default:
        break;
    }
}

Neuron::~Neuron()
{
}

#endif
