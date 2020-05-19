#ifndef NEURON_H
#define NEURON_H

#include <iostream>
#include <vector>
#include <cstdlib>

#include "River/RIVER_random.h"
using namespace std;


struct Connection {
    float weight;
    float deltaWeight;
};


struct Neuron;
typedef vector<Neuron> Layer;

struct Neuron {
  private:
    static float eta;
    static float alpha;
    static float randomWeight(void) 	{
        return 	rand()  / 	(float)((RAND_MAX/2)) - 1.0 ;
    }
    static float mutation(void) 		{
        return 	0.4f 	* 	randomGaussianFloat();
    }
    static float mutationRate(void) 	{
        return 	rand() / 	(float)((RAND_MAX/1)) ;
    }
    int    crossoverPoint(int c_length);
    float m_outputVal;
    vector<Connection> m_outputWeights;
    unsigned  m_myIndex;
    float    m_gradient;
    float    sumDOW(const Layer &nextLayer);
  protected:
  public:
    Neuron(unsigned numOutputs, unsigned myIndex, std::vector<float> &genotype);
    Neuron(vector<float> outputs, unsigned myIndex);
    Neuron(vector<float> p1_outputs, vector<float> p2_outputs, unsigned myIndex);
    Neuron(vector<float> outputs);
    static float   activationFunction(float x);
    static float   sigmoidFunction(float x);
    static float   activationFunctionDerivative(float x);
    void     setOutputVal (float val)   {
        m_outputVal = val;
    }
    float   getOutputVal()              {
        return m_outputVal;
    }
    void     feedForwardHyperbolic(Layer &prevLayer);
    void     feedForwardSigmoid(Layer &prevLayer);
    void     feedForward(Layer &prevLayer);
    float   calcOutputGradients (float targetVal);
    float   calcHiddenGradients (const Layer &nextLayer);
    void     updateInputWeights (Layer &prevLayer);
    vector<float> getWeights();
};
#endif // NEURON_H
