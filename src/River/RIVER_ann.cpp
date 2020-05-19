#include "River/RIVER_ann.h"
#include "River/RIVER_neuron.h"
#include <cassert>
#include <vector>

#include <algorithm>
#include <fstream>
#include <iostream>

using namespace std;

void ann::create(vector<unsigned> &topology, vector<float> &genotype) {
  unsigned numLayers = topology.size();

  for (unsigned layerNum = 0; layerNum < numLayers; layerNum++) {
    m_layers.push_back(Layer());

    // creates a number of connections a neuron needs, an output layer
    // needing none
    unsigned numOutputs =
        (layerNum == (topology.size() - 1)) ? 0 : topology[layerNum + 1];

    for (unsigned neuronNum = 0; neuronNum < topology[layerNum]; ++neuronNum) {
      Neuron newNeuron =
          Neuron(numOutputs, neuronNum, genotype);  // creates neuron

      m_layers.back().push_back(newNeuron);          // add neuron to layer
      c_genotype.push_back(newNeuron.getWeights());  // add weights to genotype
    }
  }
}

void ann::create(vector<weights> genotype, vector<unsigned> &topology) {
  unsigned numLayers = topology.size();
  int weightCount = 0;

  for (unsigned layerNum = 0; layerNum < numLayers; layerNum++) {
    m_layers.push_back(Layer());

    // creates the number of connections a neuron needs, an output layer needing
    // none
//    unsigned numOutputs =
//        (layerNum == (numLayers - 1)) ? 0 : topology[layerNum + 1];

    for (unsigned neuronNum = 0; neuronNum < topology[layerNum]; ++neuronNum) {
      Neuron newNeuron =
          Neuron(genotype[weightCount], neuronNum);  // creates neuron

      m_layers.back().push_back(newNeuron);          // add neuron to layer
      c_genotype.push_back(newNeuron.getWeights());  // add weights to genotype
      weightCount++;
    }
  }
}

void ann::create(vector<weights> c_parent1, vector<weights> c_parent2,
                 vector<unsigned> &topology) {
  int point = crossoverPoint(c_parent1.size());
  int weightCount = 0;
  unsigned numLayers = topology.size();

  for (unsigned layerNum = 0; layerNum < numLayers; layerNum++) {
    m_layers.push_back(Layer());

    for (unsigned neuronNum = 0; neuronNum < topology[layerNum]; ++neuronNum) {
      if (weightCount < point) {
        Neuron newNeuron = Neuron(c_parent1[weightCount], neuronNum);
        m_layers.back().push_back(newNeuron);
        c_genotype.push_back(newNeuron.getWeights());
      }

      if (weightCount == point) {
        Neuron newNeuron =
            Neuron(c_parent1[weightCount], c_parent2[weightCount], neuronNum);
        m_layers.back().push_back(newNeuron);
        c_genotype.push_back(newNeuron.getWeights());
      }

      if (weightCount > point) {
        Neuron newNeuron = Neuron(c_parent2[weightCount], neuronNum);
        m_layers.back().push_back(newNeuron);
        c_genotype.push_back(newNeuron.getWeights());
      }
      weightCount++;
    }
  }
}

vector<weights> ann::getGenotype() { return c_genotype; }

void ann::printGenotype() {
  int gen = 1;
  for (int i = 0; i < int(c_genotype.size()); i++) {
    for (int n = 0; n < int(c_genotype[i].size()); n++) {
      cout << gen;
      cout << " : ";
      cout << c_genotype[i][n];
      cout << " " << endl;
      gen++;
    }
  }
  cout << " " << endl;
}

void ann::writeGenotype(bool newFile) {
  ofstream myfile;
  if (newFile == true) {
    myfile.open("genotype.csv", ofstream::out);
  } else {
    myfile.open("genotype.csv", ofstream::out | ofstream::app);
  }

  for (int i = 0; i < int(c_genotype.size()); i++) {
    for (int n = 0; n < int(c_genotype[i].size()); n++) {
      myfile << c_genotype[i][n];
      myfile << ",";
    }
  }
  myfile.close();
}

void ann::feedForward(const vector<float> &inputVals) {
  int inputLayerSize = inputVals.size();
  assert(inputLayerSize == int(m_layers[0].size()) && "A is not equal to B");

  // inserts an output value for each node in the input layer
  for (int i = 0; i < int(inputLayerSize); ++i) {
    m_layers[0][i].setOutputVal(inputVals[i]);
  }

  // goes through hidden and output layer feeding inputs forward
  for (unsigned layerNum = 1; layerNum < m_layers.size(); layerNum++) {
    Layer &prevLayer = m_layers[layerNum - 1];
    for (unsigned n = 0; n < m_layers[layerNum].size(); ++n) {
      m_layers[layerNum][n].feedForwardHyperbolic(prevLayer);
    }
  }
}

void ann::pnFeedForward(const vector<float> &inputVals) {
  int inputLayerSize = inputVals.size();
  assert(inputLayerSize == int(m_layers[0].size()) && "A is not equal to B");

  // inserts an output value for each node in the input layer
  for (int i = 0; i < int(inputLayerSize); ++i) {
    m_layers[0][i].setOutputVal(inputVals[i]);
  }

  // goes through hidden and output layer feeding inputs forward
  for (unsigned layerNum = 1; layerNum < m_layers.size(); layerNum++) {
    Layer &prevLayer = m_layers[layerNum - 1];
    for (unsigned n = 0; n < m_layers[layerNum].size(); ++n) {
      if (layerNum >= m_layers.size() - 2) {
        if (layerNum == m_layers.size() - 2) {
          m_layers[layerNum][n].feedForwardSigmoid(prevLayer);
        } else if (layerNum == m_layers.size() - 1) {
          m_layers[layerNum][n].feedForward(prevLayer);
        }
      } else {
        m_layers[layerNum][n].feedForwardHyperbolic(prevLayer);
      }
    }
  }
}

void ann::feedForwardSigmoid(const vector<float> &inputVals) {
  int inputLayerSize = inputVals.size();
  assert(inputLayerSize ==int( m_layers[0].size() ) );

  // inserts an output value for each node in the input layer
  for (int i = 0; i < int(inputLayerSize); ++i)
    m_layers[0][i].setOutputVal(inputVals[i]);

  // goes through hidden and output layer feeding inputs forward
  for (unsigned layerNum = 1; layerNum < m_layers.size(); layerNum++) {
    Layer &prevLayer = m_layers[layerNum - 1];
    for (unsigned n = 0; n < m_layers[layerNum].size(); ++n)
      m_layers[layerNum][n].feedForwardSigmoid(prevLayer);
  }
}

void ann::backProp(const vector<float> &targetVals) {
  Layer &outputLayer = m_layers.back();
  m_error = 0.0;

  for (unsigned n = 0; n < outputLayer.size(); ++n) {
    float delta = targetVals[n] - outputLayer[n].getOutputVal();
    m_error = +delta * delta;
  }

  m_error /= outputLayer.size() - 1;
  // m_error - sqrt(m_error);

  for (unsigned n = 0; n < outputLayer.size(); ++n) {
    outputLayer[n].activationFunctionDerivative(targetVals[n]);
  }

  for (unsigned layerNum = m_layers.size() - 2; layerNum > 0; --layerNum) {
    Layer &hiddenLayer = m_layers[layerNum];
    Layer &nextLayer = m_layers[layerNum + 1];

    for (int n = 0; n < int( hiddenLayer.size() ); ++n) {
      hiddenLayer[n].calcHiddenGradients(nextLayer);
    }
  }

  for (unsigned layerNum = m_layers.size() - 1; layerNum > 0; --layerNum) {
    Layer &layer = m_layers[layerNum];
    Layer &prevLayer = m_layers[layerNum - 1];

    for (int n = 0; n < int( layer.size() ); ++n) {
      layer[n].updateInputWeights(prevLayer);
    }
  }
}

void ann::getResults(vector<float> &resultVals) {
  resultVals.clear();

  for (int n = 0; n < int( m_layers.back().size() ); ++n) {
    float result = m_layers.back()[n].getOutputVal();
    resultVals.push_back(result);
  }
}

void ann::dnGetResults(vector<float> &resultVals) {
  resultVals.clear();

  for (int n = 0; n < int( m_layers.back().size() ); ++n) {
    float result = m_layers.back()[n].getOutputVal();
    if (result > 0.3f) {
      result = 1;
    } else if (result < -0.3f) {
      result = -1;
    } else {
      result = 0;
    }
    resultVals.push_back(result);
  }
}
