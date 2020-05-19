#ifndef ANIMAT_H
#define ANIMAT_H

#include "NEAT_LayeredSubstrate.h"
#include "River/RIVER_dsworld.h"
#include "River/RIVER_shunting.h"
#include "River/RIVER_ann.h"

#define DN_INPUTLAYERSIZE    6
#define DN_HIDDENLAYERSIZE   4
#define DN_OUTPUTLAYERSIZE   5

class animat {
private:
  // pre-evolved weights for decision network to achieve greatest possible
  // reactive and deliberative behaviour in the RC Task 
  ann decsionNetwork; 
  float DNpre[44] = {
      1.0,       1.0,      1.0,       1.0,      1.0,       -0.379906, 0.941343,
      -0.140263, 1,        0.553955,  -1.0,     -0.628359, -0.30434,  0.809606,
      0.673832,  0.553807, -0.720775, 1.0,      -0.856804, 0.699742,  -0.899549,
      -0.703512, -1.0,     0.833468,  0.850612, 0.572756,  0.748668,  -0.340014,
      -1.0,      1.0,      0.141466,  0.300839, -0.982453, -0.137943, 1.0,
      0.130108,  0.193418, -1.0,      -1.0,     -1.0,      0.819229,  -0.809369,
      0.124843,  -0.813307
    };

  void action(bool pickUpAction); // logic for a change in the RC World

  NEAT::LayeredSubstrate<float> substrate; // HyperNEAT's substrate; which acts as the
                                           // artificial potential field (shunting model)

public:
  animat(NEAT::LayeredSubstrate<float> sub);
  ~animat();

  
  dsWorld world;
  int age;
  int fitness;
  bool carrying;
  int location_x;
  int location_y;
  
  shunting shuntingModel; //used for debuging (legacy shunting model)
  std::vector<float> dn_outputs;
  std::vector<float> resultVals;

  void init(dsWorld *onWorld); // inits location (x,y), age, carrying flag and RC World
  int move(); // logic for animat each time step
  int run(); // starts an evoultionary run with a stopping condition 
  int returnWorldCell(int x, int y); // return RC World cell at x,y

  int  activateShunting();  // activates the shunting model (HyperNEAT substrate) with
                            // RC world after being processed by the decision network
  bool updateDecisionNet(); // activates the decision network with animats inputs
                            // returns whether outputs have changed
};

#endif // ANIMAT_H
