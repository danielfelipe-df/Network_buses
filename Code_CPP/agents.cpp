/*
 * agents.cpp
 *
 *  Created on: 27 Feb 2020
 *      Author: daniel
 */

#include "agents.h"

agents::agents(double p)
{
  if(p < 0.3){
    susceptible2 = true;
    susceptible1 = false;
  }
  else{
    susceptible2 = false;
    susceptible1 = true;
  }
  
  infected = false;	
  exposed = false;
  
  time_station = 0;
  time_bus = 0;
  
  in_bus = false;
  in_station = true;
}



