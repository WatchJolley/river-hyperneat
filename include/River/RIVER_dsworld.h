#ifndef DSWORLD_H
#define DSWORLD_H
#include "River/RIVER_defines.h"

class dsWorld {
private:
  void buildWorld(int riverWidth);

public:
  // creates 2-d array of grass,water, traps, stones and a resource
  // objects are intergers based on 'CellContent' (see RIVER_defines.h)
  // size of array is based on 'worldsize (see RIVER_defines.h)
  dsWorld();
  dsWorld(int riverWidth);
  dsWorld(int riverWidth, int seed); // the seed is made for repeatable 
                                     // RC worlds

  int cell[RIVER::worldsize][RIVER::worldsize];
  int getAnimatCell(int x, int y); //returns the object the animat occupies 

  void print(); // print RC World w
  void printAnimat(int location_x, int location_y); // print RC World with animat location

};

#endif // DSWORLD_H
