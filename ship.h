#ifndef SHIP_H
#define SHIP_H

#define MAX_ESCORTS 100


/* =========================
   Battleship Structure
   ========================= */

typedef struct
{
    char type;

    double x;
    double y;

    double maxVelocity;

    /* Part 1-C */
    double health;
    double cumulativeImpact;

    int alive;

} Battleship;


/* =========================
   Escort Ship Structure
   ========================= */

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

    /* Part 1-C
       Each escort ship can fire only once */
    int hasFired;

} EscortShip;


/* =========================
   Battlefield Structure
   ========================= */

typedef struct
{
    double canvasSize;

    int escortCount;

    Battleship battleship;

    EscortShip escorts[MAX_ESCORTS];

} Battlefield;


/* =========================
   Function Declarations
   ========================= */

void setupBattlefield(Battlefield *battlefield);

void printBattlefield(const Battlefield *battlefield);


#endif