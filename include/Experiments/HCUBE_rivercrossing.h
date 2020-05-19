#ifndef HCUBE_RivercrossingExperimenteriment_H_INCLUDED
#define HCUBE_RivercrossingExperimenteriment_H_INCLUDED

#include "Experiments/HCUBE_CheckersCommon.h"
#include "Experiments/HCUBE_Experiment.h"

#include "River/RIVER_animat.h"

namespace HCUBE {
class RivercrossingExperiment : public Experiment {
private:
#ifndef HCUBE_NOGUI
  animat *GUIanimat;
  bool animatInit;
  bool subInit;
  bool seedInit;
  int world;
#endif
protected:
  NEAT::LayeredSubstrate<CheckersNEATDatatype> substrate;
  int currentSubstrateIndex;
  const static int handCodedAISubstrateIndex = 1;
  shared_ptr<const NEAT::GeneticIndividual> substrateIndividuals;
  int handCodedType;

  int numHandCodedStreams;
  int numHandCodedEvaluations;
  int numHyperNEATStreams;
  int numHyperNEATEvaluations;

  int cakeRandomSeed;
  SEARCHINFO searchInfo;

  int currentRound;

public:
  RivercrossingExperiment(string _experimentName, int _threadID);
  virtual NEAT::GeneticPopulation *createInitialPopulation(int populationSize);
  CheckersNEATDatatype processEvaluation(wxDC *drawContext);
  virtual void processGroup(shared_ptr<NEAT::GeneticGeneration> generation);
  virtual void
  processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual);
  virtual Experiment *clone();

#ifndef HCUBE_NOGUI
  virtual bool handleMousePress(wxMouseEvent& event,wxSize &bitmapSize);
  virtual void createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual);
#endif

  virtual void
  resetGenerationData(shared_ptr<NEAT::GeneticGeneration> generation);
  virtual void
  addGenerationData(shared_ptr<NEAT::GeneticGeneration> generation,
                    shared_ptr<NEAT::GeneticIndividual> individual);
};
}

#endif // HCUBE_RivercrossingExperimenteriment_H_INCLUDED
