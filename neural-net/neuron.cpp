#include "neuron.hpp"
#include <utility>
#include <vector>
#include "helper.hpp"

void Input::PrepareInputs(NN_Audio_Parameters NNAP)
{
	HandleZeroCrossings(&NNAP.ZeroCrossingCount, 1);
	HandlePitch(&NNAP.Pitch, 1);
	HandleMagnitude(&NNAP.Magnitude, 1);
	HandlePhase(&NNAP.Phase, 1);
}

void Input::HandleZeroCrossings(int * value, int size)
{
	float ov = 0;
	for (int i = 0; i < size; i++)
	{
		ov += Activation::Relu(value[i]);
	}
}

void Input::HandlePitch(float * value, int size)
{
	float ov = 0;
	for (int i = 0; i < size; i++)
	{
		ov += Activation::Signoid(value[i]);
	}
}


void Input::HandleMagnitude(float * value, int size)
{

}


void Input::HandlePhase(float * value, int size)
{

}
