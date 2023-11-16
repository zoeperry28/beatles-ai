#ifndef __NEURON_H__
#define __NEURON_H__

class Neuron
{
private:
    int input = 0;
    int weight = 0;
    int bias = 0;
    int threshold;
public:
    Neuron(/* args */);
    ~Neuron();
    int Activation();
};

Neuron::Neuron(/* args */)
{
}

Neuron::~Neuron()
{
}

#endif