#ifndef RIVER_DEFINES_H_INCLUDED
#define RIVER_DEFINES_H_INCLUDED

// int representations of objects in the RC World
enum CellContent { 
  EMPTYCELL = 0,
  RESOURCE 	= 1,
  STONE 	= 2,
  WATER 	= 3,
  TRAP 		= 4,
  AGENT 	= 10
};

namespace RIVER
{
const int worldsize 	  = 20;
const int numworlds 	  = 4;
const int numObjClasses = 4;
}

#endif // HCUBE_DEFINES_H_INCLUDED
