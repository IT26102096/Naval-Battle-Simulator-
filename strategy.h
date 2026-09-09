#ifndef STRATEGY_H
#define STRATEGY_H

#include "ship.h"


typedef struct
{
    int escortIndex;

    int isThreat;

    double impactPower;
    double distance;

} TargetPriority;


/*
   Creates the Battleship attack order.

   Returns the number of valid living targets.
*/
int createAttackOrder(
    const Battlefield *battlefield,
    int attackOrder[]
);


#endif