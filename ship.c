#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ship.h"


/* =========================================================
   RANDOM NUMBER HELPER
   ========================================================= */

static double randomBetween(double min, double max)
{
    return min + ((double)rand() / RAND_MAX) * (max - min);
}


/* =========================================================
   BATTLESHIP TYPE VALIDATION
   ========================================================= */

static int validBattleshipType(char type)
{
    type = (char)toupper((unsigned char)type);

    return type == 'U' ||
           type == 'M' ||
           type == 'R' ||
           type == 'S';
}


/* =========================================================
   ESCORT SHIP TYPE PROPERTIES
   ========================================================= */

static void setEscortType(EscortShip *escort, int randomType)
{
    switch (randomType)
    {
        case 0:
            strcpy(escort->type, "EA");
            escort->impactPower = 0.08;
            escort->angleRange = 20.0;
            break;

        case 1:
            strcpy(escort->type, "EB");
            escort->impactPower = 0.06;
            escort->angleRange = 30.0;
            break;

        case 2:
            strcpy(escort->type, "EC");
            escort->impactPower = 0.07;
            escort->angleRange = 25.0;
            break;

        case 3:
            strcpy(escort->type, "ED");
            escort->impactPower = 0.05;
            escort->angleRange = 50.0;
            break;

        default:
            strcpy(escort->type, "EE");
            escort->impactPower = 0.04;
            escort->angleRange = 70.0;
            break;
    }
}


/* =========================================================
   ESCORT SHIP GENERATION
   ========================================================= */

static void generateEscortShips(Battlefield *battlefield)
{
    int i;

    for (i = 0; i < battlefield->escortCount; i++)
    {
        EscortShip *escort = &battlefield->escorts[i];

        /* Unique ID */
        escort->id = i + 1;


        /* Random position inside the battlefield */
        escort->x = randomBetween(
            0.0,
            battlefield->canvasSize
        );

        escort->y = randomBetween(
            0.0,
            battlefield->canvasSize
        );


        /* Random Escort ship type */
        setEscortType(
            escort,
            rand() % 5
        );


        /* Random valid minimum angle */
        escort->minAngle = randomBetween(
            0.0,
            90.0 - escort->angleRange
        );

        escort->maxAngle =
            escort->minAngle +
            escort->angleRange;


        /*
           EA maximum velocity =
           1.2 * Battleship maximum velocity
        */

        if (strcmp(escort->type, "EA") == 0)
        {
            escort->maxVelocity =
                1.2 *
                battlefield->battleship.maxVelocity;
        }
        else
        {
            /*
               Other Escort maximum velocities
               are generated below the
               Battleship maximum velocity.
            */

            escort->maxVelocity =
                randomBetween(
                    0.55 *
                    battlefield->battleship.maxVelocity,

                    0.95 *
                    battlefield->battleship.maxVelocity
                );
        }


        /* Random minimum velocity */
        escort->minVelocity =
            randomBetween(
                0.20 *
                escort->maxVelocity,

                0.60 *
                escort->maxVelocity
            );


        /* =================================================
           BASIC STATUS
           ================================================= */

        escort->alive = 1;


        /* =================================================
           PART 1-C INITIAL VALUES
           ================================================= */

        escort->hasFired = 0;


        /* =================================================
           PART 2-B / PART 2-C INITIAL VALUES
           ================================================= */

        /*
           Escort starts with 100% health.
        */

        escort->health = 1.0;


        /*
           Fire interval values will be configured
           when Part 2-B is implemented.
        */

        escort->fireInterval = 0.0;
        escort->nextFireTime = 0.0;


        /*
           At the beginning, current impact power
           is equal to the Escort's normal
           impact power.
        */

        escort->currentImpactPower =
            escort->impactPower;


        /*
           Gamma value will be configured
           in Part 2-C.
        */

        escort->gamma = 0.0;


        /*
           No shots have been fired yet.
        */

        escort->firingCount = 0;
    }
}


/* =========================================================
   BATTLEFIELD SETUP
   ========================================================= */

void setupBattlefield(Battlefield *battlefield)
{
    char type;
    unsigned int seed;


    printf(
        "\n--- Battlefield Setup ---\n"
    );


    /* =====================================================
       CANVAS SIZE
       ===================================================== */

    do
    {
        printf(
            "Enter square canvas size D: "
        );

        scanf(
            "%lf",
            &battlefield->canvasSize
        );

    }
    while (
        battlefield->canvasSize <= 0.0
    );


    /* =====================================================
       NUMBER OF ESCORT SHIPS
       ===================================================== */

    do
    {
        printf(
            "Enter number of escort ships (1-%d): ",
            MAX_ESCORTS
        );

        scanf(
            "%d",
            &battlefield->escortCount
        );

    }
    while (
        battlefield->escortCount < 1 ||
        battlefield->escortCount > MAX_ESCORTS
    );


    /* =====================================================
       BATTLESHIP TYPE
       ===================================================== */

    do
    {
        printf(
            "Battleship type [U/M/R/S]: "
        );

        scanf(
            " %c",
            &type
        );

        type =
            (char)toupper(
                (unsigned char)type
            );

    }
    while (
        !validBattleshipType(type)
    );


    battlefield->battleship.type =
        type;


    /* =====================================================
       BATTLESHIP X POSITION
       ===================================================== */

    do
    {
        printf(
            "Battleship X position (0 to %.2f): ",
            battlefield->canvasSize
        );

        scanf(
            "%lf",
            &battlefield->battleship.x
        );

    }
    while (
        battlefield->battleship.x < 0.0 ||
        battlefield->battleship.x >
        battlefield->canvasSize
    );


    /* =====================================================
       BATTLESHIP Y POSITION
       ===================================================== */

    do
    {
        printf(
            "Battleship Y position (0 to %.2f): ",
            battlefield->canvasSize
        );

        scanf(
            "%lf",
            &battlefield->battleship.y
        );

    }
    while (
        battlefield->battleship.y < 0.0 ||
        battlefield->battleship.y >
        battlefield->canvasSize
    );


    /* =====================================================
       BATTLESHIP MAXIMUM SHELL VELOCITY
       ===================================================== */

    do
    {
        printf(
            "Battleship maximum shell velocity: "
        );

        scanf(
            "%lf",
            &battlefield->battleship.maxVelocity
        );

    }
    while (
        battlefield->battleship.maxVelocity <= 0.0
    );


    /* =====================================================
       RANDOM SEED
       ===================================================== */

    printf(
        "Enter random seed value: "
    );

    scanf(
        "%u",
        &seed
    );


    srand(seed);


    /* =====================================================
       BATTLESHIP BASIC INITIAL STATUS
       ===================================================== */

    battlefield->battleship.alive = 1;


    /* =====================================================
       PART 1-C INITIAL VALUES
       ===================================================== */

    /*
       1.0 represents 100% health.
    */

    battlefield->battleship.health = 1.0;


    /*
       Battleship initially has no damage.
    */

    battlefield->battleship.cumulativeImpact = 0.0;


    /* =====================================================
       PART 2-A INITIAL VALUES
       ===================================================== */

    /*
       Fire interval will be configured when
       Part 2-A is implemented.
    */

    battlefield->battleship.fireInterval = 0.0;

    battlefield->battleship.nextFireTime = 0.0;


    /* =====================================================
       PART 2-C INITIAL VALUES
       ===================================================== */

    /*
       Battleship starts with impact power 1.0.
    */

    battlefield->battleship.initialImpactPower = 1.0;

    battlefield->battleship.currentImpactPower = 1.0;


    /*
       Gamma will be configured later.
    */

    battlefield->battleship.gamma = 0.0;


    /*
       No Battleship shots fired yet.
    */

    battlefield->battleship.firingCount = 0;


    /* =====================================================
       GENERATE ESCORT SHIPS
       ===================================================== */

    generateEscortShips(
        battlefield
    );


    printf(
        "\nBattlefield setup completed.\n"
    );
}


/* =========================================================
   DISPLAY BATTLEFIELD
   ========================================================= */

void printBattlefield(
    const Battlefield *battlefield
)
{
    int i;


    printf(
        "\n========== BATTLEFIELD ==========\n"
    );


    printf(
        "Canvas: (0,0) to (%.2f, %.2f)\n",
        battlefield->canvasSize,
        battlefield->canvasSize
    );


    /* =====================================================
       BATTLESHIP DETAILS
       ===================================================== */

    printf(
        "\nBattleship\n"
    );


    printf(
        "Type: %c\n",
        battlefield->battleship.type
    );


    printf(
        "Position: (%.2f, %.2f)\n",
        battlefield->battleship.x,
        battlefield->battleship.y
    );


    printf(
        "Maximum shell velocity: %.2f\n",
        battlefield->battleship.maxVelocity
    );


    printf(
        "Status: %s\n",
        battlefield->battleship.alive
            ? "ALIVE"
            : "DESTROYED"
    );


    printf(
        "Health: %.2f%%\n",
        battlefield->battleship.health * 100.0
    );


    printf(
        "Cumulative Impact: %.2f\n",
        battlefield->battleship.cumulativeImpact
    );


    /*
       Part 2 values.
       These may still be zero before
       Part 2 setup is performed.
    */

    printf(
        "Fire Interval: %.2f seconds\n",
        battlefield->battleship.fireInterval
    );


    printf(
        "Current Impact Power: %.4f\n",
        battlefield->battleship.currentImpactPower
    );


    printf(
        "Gamma: %.4f\n",
        battlefield->battleship.gamma
    );


    printf(
        "Firing Count: %d\n",
        battlefield->battleship.firingCount
    );


    /* =====================================================
       ESCORT SHIP DETAILS
       ===================================================== */

    printf(
        "\nEscort Ships\n"
    );


    for (
        i = 0;
        i < battlefield->escortCount;
        i++
    )
    {
        const EscortShip *escort =
            &battlefield->escorts[i];


        printf(
            "\nE%d [%s]\n",
            escort->id,
            escort->type
        );


        printf(
            "Position: (%.2f, %.2f)\n",
            escort->x,
            escort->y
        );


        printf(
            "Velocity: %.2f - %.2f\n",
            escort->minVelocity,
            escort->maxVelocity
        );


        printf(
            "Angle: %.2f - %.2f degrees\n",
            escort->minAngle,
            escort->maxAngle
        );


        printf(
            "Base Impact Power: %.2f\n",
            escort->impactPower
        );


        printf(
            "Status: %s\n",
            escort->alive
                ? "ALIVE"
                : "DESTROYED"
        );


        printf(
            "Has Fired: %s\n",
            escort->hasFired
                ? "YES"
                : "NO"
        );


        /*
           Part 2 values
        */

        printf(
            "Health: %.2f%%\n",
            escort->health * 100.0
        );


        printf(
            "Fire Interval: %.2f seconds\n",
            escort->fireInterval
        );


        printf(
            "Current Impact Power: %.4f\n",
            escort->currentImpactPower
        );


        printf(
            "Gamma: %.4f\n",
            escort->gamma
        );


        printf(
            "Firing Count: %d\n",
            escort->firingCount
        );
    }
}