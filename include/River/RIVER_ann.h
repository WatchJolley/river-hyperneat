#ifndef ANN_H
#define ANN_H

#include "River/RIVER_defines.h"

#include "River/RIVER_neuron.h"
#include <iostream>
#include <vector>

typedef std::vector<Neuron> Layer;
typedef std::vector<float> weights;

class ann {
  private:
    int    num_layers;
    int    num_input;
    int    num_hidden;
    int    num_output;
    bool   bool_bais;
    float  biasValue;

    std::vector<Layer> m_layers;
    std::vector<weights> c_genotype;
    float m_error;
    int crossoverPoint(int c_length) { return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/c_length)) ; }
  public:
    void create(std::vector<unsigned> &topology, std::vector<float> &genotype);
    void create(std::vector<unsigned> &topology, float bais, std::vector<float> &genotype);
    void create(std::vector<weights> genotype, std::vector<unsigned> &topology);
    void create(std::vector<weights> genotype, std::vector<unsigned> &topology, float bias);
    void create(std::vector<weights> c_parent1, std::vector<weights> c_parent2, std::vector<unsigned> &topology);
    std::vector<weights> getGenotype();
    void printGenotype();
    void writeGenotype(bool newFile);
    void feedForward(const std::vector<float> &inputVals);
    void pnFeedForward(const std::vector<float> &inputVals);
    void feedForwardSigmoid(const std::vector<float> &inputVals);
    void backProp(const std::vector<float> &targetVals);
    void getResults(std::vector<float> &inputVals);
    void dnGetResults(std::vector<float> &inputVals);
    void getResultsClean(std::vector<float> &inputVals);
};

#endif // ANN_H
