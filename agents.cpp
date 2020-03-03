/*
 * agents.cpp
 *
 *  Created on: 27 Feb 2020
 *      Author: daniel
 */

#include "agents.h"

agents::agents()
{
	infected = false;	
	susceptible = true;
	exposed = false;
	
	time_station = 0;
	time_bus = 0;
	
	in_bus = false;
	in_station = true;
}



