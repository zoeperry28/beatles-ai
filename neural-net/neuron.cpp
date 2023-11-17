#include "neuron.hpp"

int Neuron::Activation(int value)
{
    int to_return = 0;
    if (value < threshold)
    {
        to_return = 0;
    }
    else if (value >= threshold)
    {
        to_return = 1;
    }
    return to_return;
}