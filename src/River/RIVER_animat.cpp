#include "River/RIVER_animat.h"
#include "River/RIVER_defines.h"
#include <math.h>

animat::~animat() { }

animat::animat(NEAT::LayeredSubstrate<float> sub) {
  for (int outputVal = 0; outputVal < 5; outputVal++) dn_outputs.push_back(0);
  for (int outputVal = 0; outputVal < 5; outputVal++) resultVals.push_back(0);

  substrate = sub;

  // build pre-evolved decision network
  vector<weights> genotype;
  int count = 0;

  // input to hidden #1 - weights
  for (int i = 0; i < DN_INPUTLAYERSIZE; i++) {
    vector<float> w;
    for (int i = 0; i < DN_HIDDENLAYERSIZE; i++) {
      w.push_back(DNpre[count]);
      count++;
    }
    genotype.push_back(w);
  }

  // hidden #1 to output - weights
  for (int i = 0; i < DN_HIDDENLAYERSIZE; i++) {
    vector<float> w;
    for (int i = 0; i < DN_OUTPUTLAYERSIZE; i++) {
      w.push_back(DNpre[count]);
      count++;
    }
    genotype.push_back(w);
  }

  // output - weights
  for (int i = 0; i < DN_OUTPUTLAYERSIZE; i++) {
    vector<float> w;
    genotype.push_back(w);
  }

  vector<unsigned> topology;
  topology.push_back(DN_INPUTLAYERSIZE);
  topology.push_back(DN_HIDDENLAYERSIZE);
  topology.push_back(DN_OUTPUTLAYERSIZE);

  decsionNetwork.create(genotype, topology);

}

void animat::init(dsWorld *onWorld) {
  location_x = 0;
  location_y = NEAT::Globals::getSingleton()->getRandom().getRandomInt(RIVER::worldsize - 1);

  age = 0;
  carrying = false;

  // tranfer world to animat class
  for (int x = 0; x < RIVER::worldsize; x++) for (int y = 0; y < RIVER::worldsize; y++) 
    world.cell[x][y] = onWorld->cell[x][y];

  delete onWorld;
}

bool animat::updateDecisionNet() {
  bool outputsChanged = false;

  vector<float> oldOutputs;
  vector<float> inputVals;
  vector<float> resultVals;

  for (int outputVal = 0; outputVal < DN_OUTPUTLAYERSIZE; outputVal++) oldOutputs.push_back(dn_outputs.at(outputVal));

  int c = world.cell[location_x][location_y];

  // binary inputs for the object the animat is located on
  // and whether the animat is holding an object (carrying)
  inputVals.push_back((c == EMPTYCELL) ? 1 : 0);
  inputVals.push_back((c == RESOURCE) ? 1 : 0);
  inputVals.push_back((c == STONE) ? 1 : 0);
  inputVals.push_back((c == WATER) ? 1 : 0);
  inputVals.push_back((c == TRAP) ? 1 : 0);
  inputVals.push_back(carrying ? 1 : 0);

  // activate ann
  decsionNetwork.feedForward(inputVals);
  decsionNetwork.dnGetResults(resultVals);

  for (int o = 0; o < DN_OUTPUTLAYERSIZE; o++)
    dn_outputs.at(o) = resultVals.at(o);

  // compare old results to new results
  for (int o = 0; o < DN_OUTPUTLAYERSIZE; o++)
    if (dn_outputs.at(o) != oldOutputs.at(o)) outputsChanged = true;

  return outputsChanged;

}

int animat::move() {
  int pickUp = 0;
  shuntingModel.move(location_x, location_y); // legacy code for activity landscape comparisons
  bool needToUpdateShuntingANN = updateDecisionNet(); // inspects whether the shunting model requires updating
                                                      // if decision networks outputs are the same, they do not
  if (needToUpdateShuntingANN) pickUp = activateShunting();

  // pick up stone
  if ((!carrying) && (dn_outputs.at(0) >= 0.1f) && (world.cell[location_x][location_y] == STONE)) action(true);

  // put down stone
  if ((carrying) && (dn_outputs.at(0) <= -0.1f) && (world.cell[location_x][location_y] == WATER)) action(false);

  age++;
  return world.cell[location_x][location_y];
}

void animat::action(bool pickUpAction)
{
  carrying = pickUpAction;
  world.cell[location_x][location_y] = EMPTYCELL;
  updateDecisionNet();
  activateShunting();
}

int animat::run() {
  updateDecisionNet();
  activateShunting();

  while (true) {
    int c = 0;
    c = move();

    //stopping conditions (success or failure)
    if (c == RESOURCE) return 1;
    if (c == TRAP || c == WATER || age >= 100) return 0;
  }
}


int animat::activateShunting()
{
  substrate.getNetwork()->reinitialize();
  substrate.getNetwork()->dummyActivation();

  for (int x = 0; x < RIVER::worldsize; x++) {
    for (int y = 0; y < RIVER::worldsize; y++) {
      int iota = (world.getAnimatCell(x, y));
      // formats inputs based on the decision networks outputs
      if (iota > 0) {
        iota = dn_outputs.at(output);
        iota = (iota == 1.0) ? iota : 0.0;
      }
      shuntingModel.updateActivity(x, y, iota); // used to compare landscapes
      substrate.setValue(NEAT::Node(x, y, 0), iota);

      }
    }

  substrate.getNetwork()->update();

    for (int x = 0; x < RIVER::worldsize; x++) {
      for (int y = 0; y < RIVER::worldsize; y++) {
        double activity = (substrate.getValue(NEAT::Node(x, y, 1)));
        // incorporate the iota back into the output
        int iota = (world.getAnimatCell(x, y));
        if (iota > 0) iota = dn_outputs.at(iota);
        activity += iota;
        shuntingModel.updateActivity(x, y, output);
        }
      }
  return 0;
}

int animat::returnWorldCell(int x, int y) {return world.cell[x][y]; }