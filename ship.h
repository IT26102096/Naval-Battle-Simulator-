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

    /* Part 2-A */
    double fireInterval;
    double nextFireTime;

    /* Part 2-C */
    double initialImpactPower;
    double currentImpactPower;
    double gamma;

    int firingCount;

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

    /* Base impact power */
    double impactPower;

    /* Part 1-C */
    int hasFired;

    /* Part 2-B */
    double fireInterval;
    double nextFireTime;

    /* Part 2-C */
    double health;
    double currentImpactPower;
    double gamma;

    int firingCount;

    int alive;

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