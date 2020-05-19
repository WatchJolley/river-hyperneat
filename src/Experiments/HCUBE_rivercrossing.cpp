#include "HCUBE_Defines.h"
#include "River/RIVER_defines.h"

#include "Experiments/HCUBE_rivercrossing.h"
#include "River/RIVER_animat.h"
#include "River/RIVER_dsworld.h"
#include "River/RIVER_shunting.h"

/* CPPN Settings */
#define DEBUG_USE_ABSOLUTE (1)
#define DEBUG_USE_DELTAS (1)

// fitness mod: 10 Evolutionary runs of the RC Task for a single reward
#define FITNESS_MOD (1)

namespace HCUBE {
using namespace NEAT;

RivercrossingExperiment::RivercrossingExperiment(string _experimentName, int _threadID) : Experiment(_experimentName, _threadID), cakeRandomSeed(1000) {

  #ifndef HCUBE_NOGUI
  subInit = false;
  animatInit = false;
  seedInit = false;
  world = 0;
  #endif

  // clear old layerinfo if present
  layerInfo.layerNames.clear();
  layerInfo.layerSizes.clear();
  layerInfo.layerValidSizes.clear();
  layerInfo.layerAdjacencyList.clear();
  layerInfo.layerIsInput.clear();
  layerInfo.layerLocations.clear();

  int w = RIVER::worldsize;

  layerInfo.layerNames.push_back("Input");
  layerInfo.layerSizes.push_back(Vector2<int>(w,w));
  layerInfo.layerValidSizes.push_back(JGTL::Vector2<int>(w,w));
  layerInfo.layerIsInput.push_back(true);
  layerInfo.layerLocations.push_back(Vector3<float>(0, 0, 0));

  layerInfo.layerNames.push_back("Output");
  layerInfo.layerSizes.push_back(Vector2<int>(w,w));
  layerInfo.layerValidSizes.push_back(JGTL::Vector2<int>(w,w));
  layerInfo.layerIsInput.push_back(false);
  layerInfo.layerLocations.push_back(Vector3<float>(0, 4, 0));

  // add adjacency information for the creation of links
  layerInfo.layerAdjacencyList.push_back(std::pair<string, string>("Input", "Output"));

  // normalises all incoming link weights
  layerInfo.normalize = true;

  // this should only be true for legacy experiments
  layerInfo.useOldOutputNames = false;

  substrate.setLayerInfo(layerInfo);
}

GeneticPopulation * RivercrossingExperiment::createInitialPopulation(int populationSize) {
  GeneticPopulation *population = new GeneticPopulation();
  vector<GeneticNodeGene> genes;

  genes.push_back(GeneticNodeGene("Bias","NetworkSensor",0,false));
  genes.push_back(GeneticNodeGene("X1","NetworkSensor",0,false));
  genes.push_back(GeneticNodeGene("Y1","NetworkSensor",0,false));
#if DEBUG_USE_ABSOLUTE
  genes.push_back(GeneticNodeGene("X2","NetworkSensor",0,false));
  genes.push_back(GeneticNodeGene("Y2","NetworkSensor",0,false));
#endif
#if DEBUG_USE_DELTAS
  genes.push_back(GeneticNodeGene("DeltaX","NetworkSensor",0,false));
  genes.push_back(GeneticNodeGene("DeltaY","NetworkSensor",0,false));
#endif

  genes.push_back(GeneticNodeGene("Output_Input_Output", "NetworkOutputNode", 1, false, ACTIVATION_FUNCTION_SIGMOID));

  for (int a = 0; a < populationSize; a++) {
    shared_ptr<GeneticIndividual> individual( new GeneticIndividual(genes, true, 1.0));
    for (int b = 0; b < 0; b++) { individual->testMutate(); }
    population->addIndividual(individual);
  }

  cout << "Finished creating population\n";
  return population;
}

void RivercrossingExperiment::processGroup(shared_ptr<NEAT::GeneticGeneration> generation) {

   // for the CPPN to populate the substrate
  shared_ptr<NEAT::GeneticIndividual> individual = group.front();

  unsigned int seed = NEAT::Globals::getSingleton()->getRandom().getRandomWithinRange(0, 763479917);
  individual->setGUISeed(seed);
  Globals::getSingleton()->seedRandom(seed);

  // you get 0.1 points just for starting, wahooo!
  individual->setFitness(0.1);
  substrate.populateSubstrate(individual);

  animat *a = new animat(substrate);

    double overallScore = 0.0;
#if FITNESS_MOD
    for (int numOfRuns = 0; numOfRuns < 10; numOfRuns++) {
#endif
      double score = 0.0;
      for (int w = 0; w < RIVER::numworlds; w++) {
        if (score == w) {
          a->init(new dsWorld(w));
          score += a->run();
        }
      }
      overallScore += score;
#if FITNESS_MOD
    }
#endif

#if FITNESS_MOD
    overallScore = overallScore/10;
#endif

  individual->reward(overallScore);
  delete a;
}

// Robustness Test: 10'000 RC World runs
void RivercrossingExperiment::processIndividualPostHoc(
  shared_ptr<NEAT::GeneticIndividual> individual) {
  cout << "INDIVIDUAL FITNESS BEFORE: " << individual->getFitness() << endl;
  double average = 0.0;
  int runs = 10000;

  vector<double> scores;

  substrate.populateSubstrate(individual);

  Globals::getSingleton()->seedRandom(264758);
  unsigned int seeds[runs];

  // generate seeds for each evoultionary run in the robustness test
  for (int i = 0; i < runs; i++)
  {
    unsigned int seed = NEAT::Globals::getSingleton()->getRandom().getRandomWithinRange(0, 763479917);
    seeds[i] = seed;
  }

  // loop for each evoultionary run of the River Crossing Task at river size 3
  for (int i = 0; i < runs; i++)
  {
    int score = 0;
    Globals::getSingleton()->seedRandom(seeds[i]);
    animat *a = new animat(substrate);

    a->init(new dsWorld(3));
    score += a->run();

    average = average + score;
    scores.push_back(score);

    delete a;
  }

  // stats calculation
  double median;
  double sum = 0.0;
  double standardDeviation = 0.0;
  double mean = double(average/runs);

  size_t size = scores.size();
  sort(scores.begin(), scores.end());
  int centerPoint = (size / 2);

  if (size  % 2 == 0){ 
    median = (scores[centerPoint - 1] + scores[centerPoint] / 2;
  } else {
    median = scores[centerPoint];
  }

  for(int i = 0; i < scores.size(); i++) standardDeviation += pow( scores.at(i) - mean, 2);

  cout << "Mean: " << mean << endl;
  cout << "Medium: " << median << endl;
  cout << "std: " << sqrt(standardDeviation / scores.size()) << endl;
  clearGroup();
  addIndividualToGroup(individual);

  std::ostringstream ss;
  ss << mean << ",";

  individual->setUserData(ss.str());
}

CheckersNEATDatatype RivercrossingExperiment::processEvaluation(wxDC *drawContext) {
  return 5.0;
}

#ifndef HCUBE_NOGUI
    bool RivercrossingExperiment::handleMousePress(wxMouseEvent& event,wxSize &bitmapSize)
    {
      int cell = GUIanimat->returnWorldCell(GUIanimat->location_x,GUIanimat->location_y);
        if (cell == RESOURCE) {
          world++;
          animatInit = false;
          return true;
        }

        if (cell == TRAP || cell == WATER || GUIanimat->age > 100) return false;

     GUIanimat->move();
     return true;
    }

    void RivercrossingExperiment::createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual)
    {

       double average = 0.0;
       int runs = 10000;

       vector<double> scores;

      if (subInit == false) {
          substrate.populateSubstrate(individual);

          Globals::getSingleton()->seedRandom(264758);
          unsigned int seeds[runs];

          for (int i = 0; i < runs; i++)
          {
            unsigned int seed = NEAT::Globals::getSingleton()->getRandom().getRandomWithinRange(0, 763479917);
            seeds[i] = seed;
          }


          for (int i = 0; i < runs; i++)
          {
              int score = 0;
              Globals::getSingleton()->seedRandom(seeds[i]);
              animat *a = new animat(substrate);

              a->init(new dsWorld(3));
              score += a->run();

              average = average + score;
              scores.push_back(score);

              delete a;
          }

      }

      double median;
      double sum = 0.0;
      double standardDeviation = 0.0;
      double mean = double(average/runs);

      size_t size = scores.size();
      int centerPoint = (size / 2);
      sort(scores.begin(), scores.end());

      if (size  % 2 == 0)
      { median = (scores[centerPoint - 1] + scores[centerPoint]) / 2;
      } else {
        median = scores[centerPoint];
      }


      for(int i = 0; i < scores.size(); i++) standardDeviation += pow( scores.at(i) - mean, 2);

      cout << "Mean: " << mean << endl;
      cout << "Medium: " << median << endl;
      cout << "std: " << sqrt(standardDeviation / scores.size()) << endl;

      int SizeX = drawContext.GetSize().x;
      int SizeY = drawContext.GetSize().y;

      int startPos = ((SizeX-SizeY) / 2);
      int boxSize = (drawContext.GetSize().y / RIVER::worldsize);
      int objectSize = (boxSize/2) - 5;

      // initialise animat if not already
      if (seedInit == false) {
        cout << individual->getGUISeed() << endl;
        Globals::getSingleton()->seedRandom(individual->getGUISeed());
        //-----------------------------------------------
        // uncomment to control seed
        // Globals::getSingleton()->seedRandom(2);
        //-----------------------------------------------
        seedInit = true;
      }

      // initialise animat if not already
      if (subInit == false) {
          substrate.populateSubstrate(individual);
          GUIanimat = new animat(substrate);
          subInit = true;
       }

      // initialise animat for world if not already
      if (animatInit == false)
        {
          GUIanimat->init(new dsWorld(world));
          GUIanimat->updateDecisionNet();
          GUIanimat->activateShunting();

          cout << "Setting up Animat" << endl;
          animatInit = true;
        }

        drawContext.Clear();

        // draw borders
        drawContext.SetPen(wxPen(*wxBLACK,2,wxSOLID));
        for(int w = 0; w < RIVER::worldsize+1; w++) {
          drawContext.DrawLine(
              int(startPos + (boxSize * w)),
              int(0),
              int(startPos + (boxSize * w)),
              int(SizeY)
          );
          drawContext.DrawLine(
                int(startPos),
                int(boxSize * w),
                int(SizeX- startPos),
                int(boxSize * w)
          );
        }

        // draw world
        for(int x = 0; x < RIVER::worldsize; x++) {
         for(int y = 0; y < RIVER::worldsize; y++) {
             int cell = GUIanimat->returnWorldCell(x,y);
             if (cell > 0)
               {
                 switch(cell){
                   case 1: // resource
                     drawContext.SetBrush(wxBrush(wxColour(135, 91, 255)));
                     break;
                   case 2: // stone
                     drawContext.SetBrush(wxBrush(wxColour(206, 206, 206)));
                     break;
                   case 3: // water
                     drawContext.SetBrush(wxBrush(wxColour(79, 175, 255)));
                     break;
                   case 4: // trap
                     drawContext.SetBrush(wxBrush(wxColour(198, 35, 35)));
                     break;
                   }

                 drawContext.DrawCircle(
                     int(startPos+x*boxSize) + (boxSize/2) ,
                     int(y*boxSize) + (boxSize/2),
                     objectSize
                 );
               }
           }
        }

        // draw animat
        drawContext.SetBrush(wxBrush(wxColour(255, 255, 81)));
        drawContext.DrawCircle(
            int(startPos+GUIanimat->location_x*boxSize) + (boxSize/2) ,
            int(GUIanimat->location_y*boxSize) + (boxSize/2),
            objectSize
        );
    }
#endif

Experiment *RivercrossingExperiment::clone() {
  RivercrossingExperiment *experiment = new RivercrossingExperiment(*this);
  return experiment;
}

void RivercrossingExperiment::resetGenerationData(shared_ptr<NEAT::GeneticGeneration> generation) {}

void RivercrossingExperiment::addGenerationData( shared_ptr<NEAT::GeneticGeneration> generation, shared_ptr<NEAT::GeneticIndividual> individual) {}
}