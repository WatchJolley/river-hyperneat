#include "River/RIVER_shunting.h"
#include "NEAT_Globals.h"
#include "River/RIVER_defines.h"
#include <numeric>

#include <math.h>

using namespace std;

shunting::shunting() {
  for (int x = 0; x < RIVER::worldsize; x++) for (int y = 0; y < RIVER::worldsize; y++) {
    iota[x][y] = 0;
    activity[x][y] = 0;
  }
}

void shunting::activate()
{
  int glc = RIVER::worldsize;
  for (int x=0;x<glc;x++) for (int y=0;y<glc;y++) activity[x][y] = 0;

  for (int cycle = 0; cycle < 50; cycle++) {
    float aNEW[glc][glc];
    for (int x=0;x<glc;x++) for (int y=0;y<glc;y++) {
      aNEW[x][y] = iota[x][y] + 0.25 * (m(x-1,y)+m(x,y-1)+m(x+1,y)+m(x,y+1));
      if (iota[x][y] == -1 ) aNEW[x][y] = -1;
      if (aNEW[x][y] > 15) aNEW[x][y]= 15.0;
    }
    for (int x=0;x<glc;x++) for (int y=0;y<glc;y++) activity[x][y] = aNEW[x][y];
  }
}

void shunting::updateActivity(int x, int y, double value) {activity[x][y] = value; }

void shunting::updateIota(int x, int y, double value) {iota[x][y] = value; }

double shunting::getIota(int x, int y) {return iota[x][y];}

void shunting::move(int &x, int &y) {
  double startA = activity[x][y];
  double bestA = startA;
  int newX = x;
  int newY = y;

  for (int x2 = f(x, -1); x2 <= f(x, 1); x2++) for (int y2 = f(y, -1); y2 <= f(y, 1); y2++) {
    if ( !((x2 == x) && (y2 == y)) ) {
      if (activity[x2][y2] > bestA) {
        bestA = activity[x2][y2];
        newX = x2;
        newY = y2;
      }
    }
  }

  x = newX;
  y = newY;

  // if no iota value is larger then roam
  if (startA == bestA) roam(x, y);
}

void shunting::roam(int &x, int &y) {
  int newX, newY;

  do {
    newX = rndMove(x);
  } while ( !(inRange(newX)) );

  do {
    newY = rndMove(y);
  } while ( !(inRange(newY)) );

  x = newX;
  y = newY;
}

int shunting::rndMove(int location)
{
  int randVal = NEAT::Globals::getSingleton()->getRandom().getRandomWithinRange(-1, 1);
  location += randVal;
  return location;
}

void shunting::print() {
  print(-1,-1);
}

void shunting::print(int Cx, int Cy) {
  for (int y = 0; y < RIVER::worldsize; y++) {
    for (int x = 0; x < RIVER::worldsize; x++) {
      if ((Cx == x) && (Cy == y)) {
        cout << "A";
      } else {
        cout << activity[x][y];
      }
      if (x != RIVER::worldsize-1) cout << ",";
    }
    cout << " " << endl;
  }
  cout << " " << endl;
}