#ifndef SHIP_H
#define SHIP_H

#define MAX_ESCORTS 100

typedef struct
{
    char type;

    double x;
    double y;

    double maxVelocity;

    int alive;

} Battleship;


typedef struct
{
    int id;
    char type[3];

    double x;
    double y;

    double minVelocity;
    double maxVelocity;

    double minAngle;
    double maxAngle;
    double angleRange;

    double impactPower;

    int alive;

} EscortShip;


typedef struct
{
    double canvasSize;

    int escortCount;

    Battleship battleship;

    EscortShip escorts[MAX_ESCORTS];

} Battlefield;


void setupBattlefield(Battlefield *battlefield);

void printBattlefield(const Battlefield *battlefield);

#endif