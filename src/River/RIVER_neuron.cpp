#include "River/RIVER_neuron.h"
#include <math.h>
#include <cmath>
#include <vector>

using namespace std;

float Neuron::eta = 0.1;
float Neuron::alpha = 0.5;

// create neuron with a number of inputs for connections
Neuron::Neuron(unsigned numOutputs, unsigned myIndex, vector<float> &genotype) {
  for (unsigned c = 0; c < numOutputs; ++c) {
    float randomfloat = randomGaussianFloat();
    m_outputWeights.push_back(Connection());
    m_outputWeights.back().weight = randomfloat;
    genotype.push_back(randomfloat);
  }

  m_myIndex = myIndex;
}

// load neuron with output variables
Neuron::Neuron(vector<float> outputs, unsigned myIndex) {
  // cout << outputs.size() << endl;
  for (unsigned c = 0; c < outputs.size(); ++c) {
    m_outputWeights.push_back(Connection());
    m_outputWeights.back().weight = outputs[c];
  }

  m_myIndex = myIndex;
}

// crossover neuron with two-parent variables
Neuron::Neuron(vector<float> p1_outputs, vector<float> p2_outputs,
               unsigned myIndex) {
  int point = crossoverPoint(p1_outputs.size());
  for (unsigned c = 0; c < p1_outputs.size(); ++c) {
    m_outputWeights.push_back(Connection());
    float output;

    if (c <= point) output = p1_outputs[c];
    if (c > point) output = p2_outputs[c];

    float mutaionChance = mutationRate();

    if (0.95 < mutaionChance) {
      output += mutation();
      if (output > 1) {
        output = 1;
      } else if (output < -1) {
        output = -1;
      }
    }
    m_outputWeights.back().weight = output;
  }

  m_myIndex = myIndex;
}

// return random crossoverpoint
int Neuron::crossoverPoint(int c_length) {
  return c_length == 0 ? 0 : (rand() / (float)(RAND_MAX / c_length));
}

// gets and returns neuron weights in a float vector
vector<float> Neuron::getWeights() {
  vector<float> outputWeights;
  for (int i = 0; i < m_outputWeights.size(); i++) {
    outputWeights.push_back(m_outputWeights[i].weight);
  }

  return outputWeights;
}


void Neuron::feedForwardHyperbolic(Layer &prevLayer) {
  float sum = 0.0;
  for (unsigned n = 0; n < prevLayer.size(); ++n)
    sum += prevLayer[n].getOutputVal() *
           prevLayer[n].m_outputWeights[m_myIndex].weight;

  m_outputVal = activationFunction(sum);
}

void Neuron::feedForwardSigmoid(Layer &prevLayer) {
  float sum = 0.0;
  for (unsigned n = 0; n < prevLayer.size(); ++n)
    sum += prevLayer[n].getOutputVal() *
           prevLayer[n].m_outputWeights[m_myIndex].weight;

  m_outputVal = activationFunction(sum);
}

void Neuron::feedForward(Layer &prevLayer) {
  float sum = 0.0;
  for (unsigned n = 0; n < prevLayer.size(); ++n)
    sum += prevLayer[n].getOutputVal() *
           prevLayer[n].m_outputWeights[m_myIndex].weight;

  m_outputVal = sum;
}

// returns hyperbolic tangent function
float Neuron::activationFunction(float x) {
  float retunrVal = tanh(x);
  return retunrVal;
};

// returns hyperbolic tangent function
float Neuron::sigmoidFunction(float x) { return x / (1 + abs(x)); };

// returns activation functions derivative
float Neuron::activationFunctionDerivative(float x) { return 1.0 - x * x; }

float Neuron::calcOutputGradients(float targetVal) {
  float delta = targetVal - m_outputVal;
  m_gradient = delta * activationFunctionDerivative(m_outputVal);
  return m_gradient;
}

float Neuron::sumDOW(const Layer &nextLayer) {
  float sum = 0.0;
  for (unsigned n = 0; n < nextLayer.size(); ++n)
    sum = +m_outputWeights[n].weight * nextLayer[n].m_gradient;

  return sum;
}

float Neuron::calcHiddenGradients(const Layer &nextLayer) {
  float dow = sumDOW(nextLayer);
  m_gradient = dow * activationFunctionDerivative(m_outputVal);
  return dow;
}

void Neuron::updateInputWeights(Layer &prevLayer) {
  for (unsigned n = 0; n < prevLayer.size(); ++n) {
    Neuron &neuron = prevLayer[n];
    float oldDeltaWeight = neuron.m_outputWeights[m_myIndex].deltaWeight;

    float newDeltaWeight =
        eta * neuron.getOutputVal() * m_gradient + alpha * oldDeltaWeight;

    neuron.m_outputWeights[m_myIndex].deltaWeight = newDeltaWeight;
    neuron.m_outputWeights[m_myIndex].weight += newDeltaWeight;
  }
}
