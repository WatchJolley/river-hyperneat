#ifndef  random_h
#define  random_h
#include <cstdlib>
#include <fstream>

unsigned randomise(unsigned s=0); // seed random number generator with: s if >0, random_device or (failing that) system clock

int32_t random(int32_t n); // return a random integer between 0 and n-1

double  randomdouble();         // return a random double from [0,1)
double  randomdouble(double d); // return a random double from [0,d)
double  randomGaussianDouble(double mean=0, double stddev=1.0); // return a random double from N(mean,stddev)

float   randomfloat();         // return a random float from [0,1)
float   randomfloat(float f);  // return a random float from [0,f)
float   randomGaussianFloat(float mean=0.0f, float stddev=1.0f); // return a random float from N(mean,stddev)

#endif //random_h
