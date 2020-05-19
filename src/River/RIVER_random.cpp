#include "HCUBE_Defines.h"
#include <River/RIVER_random.h>
#include <fstream>

unsigned randomise(unsigned s) {
  NEAT::Globals::getSingleton()->seedRandom(s);
  return s;
}

int32_t random(int32_t n) { return NEAT::Globals::getSingleton()->getRandom().getRandomInt(n); } // 0..(n-1)

double randomdouble() { return NEAT::Globals::getSingleton()->getRandom().getRandomDouble(); }
float  randomfloat()  { return float( NEAT::Globals::getSingleton()->getRandom().getRandomDouble() );  }

double randomdouble(double d) { return NEAT::Globals::getSingleton()->getRandom().getRandomDouble() * d; }
float  randomfloat(float f)   { return float( NEAT::Globals::getSingleton()->getRandom().getRandomDouble() * f );}

double randomGaussianDouble (double mean, double stddev) {return exp(-pow(randomdouble(), 2.0)); }
float  randomGaussianFloat  (float mean, float stddev) {return expf(-powf(randomdouble(), 2.0)); }
