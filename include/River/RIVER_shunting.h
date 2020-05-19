#ifndef RIVER_SHUNTING_H_INCLUDED
#define RIVER_SHUNTING_H_INCLUDED

#include "NEAT_Random.h"
#include "River/RIVER_defines.h"

class shunting {
private:
  double activity[RIVER::worldsize][RIVER::worldsize];
  double iota[RIVER::worldsize][RIVER::worldsize];

  // if lower than zero; return zero
  double  p(double f) { return f (f > 0) ? f : 0; }

  // if x,y in range: return activty; else zero
  double m(int x, int y) {
    if ( ( inRange(x) ) && ( inRange(y) ) ) {
        return p(activity[x][y]);
      } else {
        return 0;
      }
  }

  // validates new proposed position in RC World
  int f(int value, int delta)
  {
    int move = value + delta;
    int result = ( inRange(move) ) ? move : value;
    return result;
  }

  // validates a single coordinate is within the RC World size
  bool inRange(int location)
  {
    if ( (0 <= location) && (location < RIVER::worldsize ) ) {
       return true;
    }
    return false;
  }

  void roam(int &x, int &y); // move in the RC world within the moore neighbourhood of x,y positions
  int rndMove(int location); // randomly roam the RC world

protected:
public:
  shunting(); 
  void activate(); // activate the shunting equation
  void updateActivity(int x, int y, double value); // change activity value with parameter
  void updateIota(int x, int y, double value); // change iota value with parameter
  double getIota(int x, int y); // return the iota value
  void move(int &x, int &y); // use gradient ascent to traverse enviroment
  void print(); // print activity landscape
  void print(int Cx, int Cy); // print activity landscape with animat position
};

#endif
