#include <stdio.h>

#include "strategy.h"
#include "battle.h"


/* =========================================================
   COMPARE TWO TARGETS
   ========================================================= */

/*
   Returns 1 when target A should have
   higher priority than target B.
*/

static int higherPriority(
    const TargetPriority *a,
    const TargetPriority *b
)
{
    /*
       Priority 1:
       Escort ship that can currently hit B.
    */

    if (a->isThreat != b->isThreat)
    {
        return a->isThreat > b->isThreat;
    }


    /*
       Priority 2:
       Higher impact power.
    */

    if (a->impactPower != b->impactPower)
    {
        return a->impactPower > b->impactPower;
    }


    /*
       Priority 3:
       Nearest target.
    */

    if (a->distance != b->distance)
    {
        return a->distance < b->distance;
    }


    /*
       If everything above is equal,
       original Escort array order is kept.
    */

    return 0;
}


/* =========================================================
   CREATE ATTACK ORDER
   ========================================================= */

int createAttackOrder(
    const Battlefield *battlefield,
    int attackOrder[]
)
{
    TargetPriority targets[MAX_ESCORTS];

    int targetCount = 0;

    int i;
    int j;


    /*
       Build information about every living
       Escort ship.
    */

    for (i = 0; i < battlefield->escortCount; i++)
    {
        const EscortShip *escort =
            &battlefield->escorts[i];

        ShotSolution escortShot;

        double distance;

        int canThreatenBattleship;


        if (!escort->alive)
        {
            continue;
        }


        distance =
            calculateDistance(
                escort->x,
                escort->y,
                battlefield->battleship.x,
                battlefield->battleship.y
            );


        canThreatenBattleship =
            findShotSolution(
                distance,
                escort->minVelocity,
                escort->maxVelocity,
                escort->minAngle,
                escort->maxAngle,
                &escortShot
            );


        targets[targetCount].escortIndex =
            i;

        targets[targetCount].isThreat =
            canThreatenBattleship;

        targets[targetCount].impactPower =
            escort->impactPower;

        targets[targetCount].distance =
            distance;


        targetCount++;
    }


    /*
       Simple bubble sort.

       This is intentionally kept simple
       for a first-year C project.
    */

    for (i = 0; i < targetCount - 1; i++)
    {
        for (j = 0; j < targetCount - i - 1; j++)
        {
            if (
                higherPriority(
                    &targets[j + 1],
                    &targets[j]
                )
            )
            {
                TargetPriority temp =
                    targets[j];

                targets[j] =
                    targets[j + 1];

                targets[j + 1] =
                    temp;
            }
        }
    }


    /*
       Copy sorted Escort array indexes
       into attackOrder.
    */

    for (i = 0; i < targetCount; i++)
    {
        attackOrder[i] =
            targets[i].escortIndex;
    }


    return targetCount;
}