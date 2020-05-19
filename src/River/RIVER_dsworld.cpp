#include "River/RIVER_dsworld.h"

#include "NEAT_Globals.h"
#include "River/RIVER_defines.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

dsWorld::dsWorld() { buildWorld(0); }

dsWorld::dsWorld(int riverWidth) { buildWorld(riverWidth); }

dsWorld::dsWorld(int riverWidth, int worldseed) {
  int seed = worldseed + (riverWidth * worldseed);
  srand (seed);
  srand(rand());
  buildWorld(riverWidth);
}

int dsWorld::getAnimatCell(int x, int y) {
  int returnCell = cell[x][y];
  return returnCell;
}

void dsWorld::buildWorld(int riverWidth) {
  int c;

  // init cells
  for (int x = 0; x < RIVER::worldsize; x++) {
    for (int y = 0; y < RIVER::worldsize; y++) {
      cell[x][y] = EMPTYCELL;
    }
  }

  // create river (water)
  int riverPosition = 0;
  int riverValidation = (int(RIVER::worldsize / 2) + 2);

  if (riverValidation <= RIVER::worldsize) {
    if (riverValidation == RIVER::worldsize) {
      riverPosition = RIVER::worldsize - 3;
    } else {
      riverPosition = ((RIVER::worldsize / 2) + (((RIVER::worldsize / 2) - 2) / 2));
    }
  }

  for (int x = riverPosition; x < riverPosition + (riverWidth); x++) {
    for (int y = 0; y < RIVER::worldsize; y++) {
      cell[x][y] = WATER;
    }
  }

  // place traps
  float trapProb = int((RIVER::worldsize * RIVER::worldsize) * 0.0175);
  int numTraps = (trapProb < 1) ? 1 : int(trapProb);

  if ( (RIVER::worldsize == 4) && (riverWidth == 3) ) { } else {
    for (int n = 0; n < numTraps; n++) {
      int x;
      int y;
      do {
          x = NEAT::Globals::getSingleton()->getRandom().getRandomWithinRange(
              0, RIVER::worldsize - 1);
          y = NEAT::Globals::getSingleton()->getRandom().getRandomWithinRange(
              0, RIVER::worldsize - 1);

        c = cell[x][y];
      } while (c & RESOURCE);

      cell[x][y] = TRAP;
    }
  }

  // place stone
  for (int n = 0; n < int((RIVER::worldsize * RIVER::worldsize) * 0.0425) + riverWidth; n++) {
    int x;
    int y;
    do {
        x = NEAT::Globals::getSingleton()->getRandom().getRandomWithinRange(
            0, RIVER::worldsize - 1);
        y = NEAT::Globals::getSingleton()->getRandom().getRandomWithinRange(
            0, RIVER::worldsize - 1);
      c = cell[x][y];
    } while (x > riverPosition || c & WATER || c & TRAP);
    cell[x][y] = STONE;
  }

  // place resource
  int resourcePos = ( (RIVER::worldsize - 1) - (riverPosition + (riverWidth)) )/2;
  cell[(RIVER::worldsize-1) - resourcePos][int(RIVER::worldsize / 2) - 1] = RESOURCE;
}

void dsWorld::print() {
  printAnimat(-1,-1);
}

// print RC World with animat location
void dsWorld::printAnimat(int location_x, int location_y) {
  for (int y = 0; y < RIVER::worldsize; y++) {
    for (int x = 0; x < RIVER::worldsize; x++) {
      if (x == location_x && (y == location_y)) {
          cout << "A";
        } else {
          cout << cell[x][y];
        }
      cout << ",";
    }
    cout << " " << endl;
  }
  cout << " " << endl;
}
