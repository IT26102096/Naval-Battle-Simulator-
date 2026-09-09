#ifndef BATTLE_H
#define BATTLE_H

#include "ship.h"

typedef struct
{
    int possible;
    double velocity;
    double angle;
    double time;

} ShotSolution;


double calculateDistance(
    double x1,
    double y1,
    double x2,
    double y2
);

int findShotSolution(
    double distance,
    double minVelocity,
    double maxVelocity,
    double minAngle,
    double maxAngle,
    ShotSolution *solution
);

void runPart1A(Battlefield *battlefield);
void runPart1B(const Battlefield *initialBattlefield);
void runPart1C(const Battlefield *initialBattlefield);
void runPart2A(const Battlefield *initialBattlefield);
void runPart2B(const Battlefield *initialBattlefield);
void runPart2C(
    const Battlefield *initialBattlefield
);

#endif